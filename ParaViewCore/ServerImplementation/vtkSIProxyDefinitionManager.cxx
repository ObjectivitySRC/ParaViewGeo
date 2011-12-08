/*=========================================================================

  Program:   ParaView
  Module:    vtkSIProxyDefinitionManager.cxx

  Copyright (c) Kitware, Inc.
  All rights reserved.
  See Copyright.txt or http://www.paraview.org/HTML/Copyright.html for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "vtkSIProxyDefinitionManager.h"

#include "vtkCollection.h"
#include "vtkCollectionIterator.h"
#include "vtkCommand.h"
#include "vtkInstantiator.h"
#include "vtkNew.h"
#include "vtkObjectFactory.h"
#include "vtkPVConfig.h"
#include "vtkPVPlugin.h"
#include "vtkPVPluginTracker.h"
#include "vtkPVProxyDefinitionIterator.h"
#include "vtkPVServerManagerPluginInterface.h"
#include "vtkPVSession.h"
#include "vtkPVXMLElement.h"
#include "vtkPVXMLParser.h"
#include "vtkReservedRemoteObjectIds.h"
#include "vtkSmartPointer.h"
#include "vtkSMMessage.h"
#include "vtkStdString.h"
#include "vtkStringList.h"
#include "vtkTimerLog.h"

#include <vtksys/DateStamp.h> // For date stamp
#include <vtksys/ios/sstream>
#include <vtkstd/map>
#include <vtksys/RegularExpression.hxx>
#include <vtkstd/set>
#include <vtkstd/string>
#include <vtkstd/vector>

#include <assert.h>

// this file must be included after vtkPVConfig etc. are included.
#include "vtkSMGeneratedModules.h"

//****************************************************************************/
//                    Internal Classes and typedefs
//****************************************************************************/
typedef vtkSmartPointer<vtkPVXMLElement>        XMLElement;
typedef vtkstd::map<vtkStdString, XMLElement>   StrToXmlMap;
typedef vtkstd::map<vtkStdString, StrToXmlMap>  StrToStrToXmlMap;

class vtkSIProxyDefinitionManager::vtkInternals
{
public:
  // Keep track of ServerManager definition
  StrToStrToXmlMap CoreDefinitions;
  // Keep track of custom definition
  StrToStrToXmlMap CustomsDefinitions;
  //-------------------------------------------------------------------------
  void Clear()
    {
    this->CoreDefinitions.clear();
    this->CustomsDefinitions.clear();
    }
  //-------------------------------------------------------------------------
  bool HasCoreDefinition( const char* groupName, const char* proxyName)
  {
    return this->GetProxyElement( this->CoreDefinitions,
                                  groupName, proxyName) != NULL;
  }
  //-------------------------------------------------------------------------
  bool HasCustomDefinition( const char* groupName, const char* proxyName)
  {
    return this->GetProxyElement( this->CustomsDefinitions,
                                  groupName, proxyName ) != NULL;
  }
  //-------------------------------------------------------------------------
  unsigned int GetNumberOfProxy(const char* groupName)
  {
    unsigned int nbProxy = 0;
    if(groupName)
    {
      nbProxy += static_cast<unsigned int>(
        this->CoreDefinitions[groupName].size());
      nbProxy += static_cast<unsigned int>(
        this->CustomsDefinitions[groupName].size());
    }
    return nbProxy;
  }
  //-------------------------------------------------------------------------
  vtkPVXMLElement* GetProxyElement( const char* groupName,
                                    const char* proxyName )
  {
    vtkPVXMLElement* elementToReturn = NULL;

    // Search in ServerManager definitions
    elementToReturn = this->GetProxyElement( this->CoreDefinitions,
                                             groupName, proxyName );

    // If not found yet, search in customs ones...
    if(elementToReturn == NULL)
      {
      elementToReturn = this->GetProxyElement( this->CustomsDefinitions,
                                         groupName, proxyName );
      }

    return elementToReturn;
  }
  //-------------------------------------------------------------------------
  vtkPVXMLElement* GetProxyElement( const StrToStrToXmlMap& map,
                                    const char* firstStr,
                                    const char* secondStr)
  {
    vtkPVXMLElement* elementToReturn = NULL;

    // Test if parameters are valid
    if (firstStr && secondStr)
      {
      // Find the value based on both keys
      StrToStrToXmlMap::const_iterator it = map.find(firstStr);
      if (it != map.end())
        {
        // We found a match for the first key
        StrToXmlMap::const_iterator it2 = it->second.find(secondStr);
        if (it2 != it->second.end())
          {
          // We found a match for the second key
          elementToReturn = it2->second.GetPointer();
          }
        }
      }

    // The result might be NULL if the value was not found
    return elementToReturn;
  }

  static void ExtractMetaInformation(vtkPVXMLElement* proxy,
                                     vtkstd::map<vtkstd::string, vtkSmartPointer<vtkPVXMLElement> > &subProxyMap,
                                     vtkstd::map<vtkstd::string, vtkSmartPointer<vtkPVXMLElement> > &propertyMap)
    {
    vtkstd::set<vtkstd::string> propertyTypeName;
    propertyTypeName.insert("DoubleVectorProperty");
    propertyTypeName.insert("IntVectorProperty");
    propertyTypeName.insert("ProxyProperty");
    propertyTypeName.insert("Property");

    unsigned int numChildren = proxy->GetNumberOfNestedElements();
    unsigned int cc;
    for (cc=0; cc < numChildren; cc++)
      {
      vtkPVXMLElement* child = proxy->GetNestedElement(cc);
      if (child && child->GetName())
        {
        if(strcmp(child->GetName(), "SubProxy") == 0)
          { // SubProxy
          if(child->GetAttribute("name"))
            {
            subProxyMap[child->GetAttribute("name")] = child;
            }
          vtkPVXMLElement* exposedPropElement =
              child->FindNestedElementByName("ExposedProperties");

          // exposedPropElement can be NULL if only Shared property are used...
          if(exposedPropElement)
            {
            unsigned int ccSub = 0;
            unsigned int numChildrenSub = exposedPropElement->GetNumberOfNestedElements();
            for(ccSub = 0; ccSub < numChildrenSub; ccSub++)
              {
              vtkPVXMLElement* subProxyChild = exposedPropElement->GetNestedElement(ccSub);
              if (subProxyChild && subProxyChild->GetName()
                && propertyTypeName.find(subProxyChild->GetName()) != propertyTypeName.end())
                  { // Property
                const char* propName = subProxyChild->GetAttribute("exposed_name");
                propName = propName ? propName : subProxyChild->GetAttribute("name");
                propertyMap[propName] = subProxyChild;
                } // Property end ---------------------
              }
            }
          } // SubProxy end ------------------
        else if(propertyTypeName.find(child->GetName()) != propertyTypeName.end())
          { // Property
          const char* propName = child->GetAttribute("exposed_name");
          propName = propName ? propName : child->GetAttribute("name");
          propertyMap[propName] = child;
          } // Property end ---------------------
        }
      }
    }

};
//****************************************************************************/
class vtkInternalDefinitionIterator : public vtkPVProxyDefinitionIterator
{
public:

  static vtkInternalDefinitionIterator* New();
  vtkTypeMacro(vtkInternalDefinitionIterator, vtkPVProxyDefinitionIterator);

  //-------------------------------------------------------------------------
  void GoToFirstItem()
  {
    this->Reset();
  }
  //-------------------------------------------------------------------------
  void GoToNextItem()
  {
    if(!this->IsDoneWithCoreTraversal())
      {
      this->NextCoreDefinition();
      }
    else if(!this->IsDoneWithCustomTraversal())
      {
      this->NextCustomDefinition();
      }
    else
      {
      while( this->IsDoneWithCoreTraversal() &&
             this->IsDoneWithCustomTraversal() &&
             !this->IsDoneWithGroupTraversal() )
        {
        this->NextGroup();
        }
      }
  }
  //-------------------------------------------------------------------------
  bool IsDoneWithTraversal()
  {
    if(!this->Initialized)
    {
      this->GoToFirstItem();
    }
    if( this->IsDoneWithCoreTraversal() && this->IsDoneWithCustomTraversal())
      {
      if(this->IsDoneWithGroupTraversal())
        {
        return true;
        }
      else
        {
        this->NextGroup();
        return this->IsDoneWithTraversal();
        }
      }
    return false;
  }
  //-------------------------------------------------------------------------
  virtual const char* GetGroupName()
  {
    return this->CurrentGroupName.c_str();
  }
  //-------------------------------------------------------------------------
  virtual const char* GetProxyName()
  {
    if(this->IsCustom())
      {
      return this->CustomProxyIterator->first.c_str();
      }
    else
      {
      return this->CoreProxyIterator->first.c_str();
      }
  }
  //-------------------------------------------------------------------------
  virtual bool IsCustom()
  {
    return this->IsDoneWithCoreTraversal();
  }
  //-------------------------------------------------------------------------
  virtual vtkPVXMLElement* GetProxyDefinition()
  {
    if(this->IsCustom())
      {
      return this->CustomProxyIterator->second.GetPointer();
      }
    else
      {
      return this->CoreProxyIterator->second.GetPointer();
      }
  }
  //-------------------------------------------------------------------------
  virtual vtkPVXMLElement* GetProxyHints()
  {
    vtkPVXMLElement* definition = this->GetProxyDefinition();
    if(definition)
      {
      return definition->FindNestedElementByName("Hints");
      }
    return NULL;
  }
  //-------------------------------------------------------------------------
  void AddTraversalGroupName(const char* groupName)
  {
    this->GroupNames.insert(vtkStdString(groupName));
  }
  //-------------------------------------------------------------------------
  void RegisterCoreDefinitionMap(StrToStrToXmlMap* map)
  {
    this->CoreDefinitionMap = map;
    this->InvalidCoreIterator = true;
  }
  //-------------------------------------------------------------------------
  void RegisterCustomDefinitionMap(StrToStrToXmlMap* map)
  {
    this->CustomDefinitionMap = map;
    this->InvalidCustomIterator = true;
  }

 //-------------------------------------------------------------------------
  void GoToNextGroup()
  {
    this->NextGroup();
  }

protected:
  vtkInternalDefinitionIterator()
  {
    this->Initialized = false;
    this->CoreDefinitionMap = NULL;
    this->CustomDefinitionMap = 0;
    this->InvalidCoreIterator = true;
    this->InvalidCustomIterator = true;
  }
  ~vtkInternalDefinitionIterator(){}

  //-------------------------------------------------------------------------
  void Reset()
  {
    this->Initialized = true;
    this->InvalidCoreIterator = true;
    this->InvalidCustomIterator = true;

    if(this->GroupNames.size() == 0)
      {
      // Look for all name available
      if(this->CoreDefinitionMap)
        {
        StrToStrToXmlMap::iterator it = this->CoreDefinitionMap->begin();
        while(it != this->CoreDefinitionMap->end())
          {
          this->AddTraversalGroupName(it->first.c_str());
          it++;
          }
        }
      if(this->CustomDefinitionMap)
        {
        StrToStrToXmlMap::iterator it = this->CustomDefinitionMap->begin();
        while(it != this->CustomDefinitionMap->end())
          {
          this->AddTraversalGroupName(it->first.c_str());
          it++;
          }
        }

      if(this->GroupNames.size() == 0)
        {
        // TODO vtkErrorMacro("No definition available for that iterator.");
        return;
        }
      this->Initialized = false;
      this->Reset();
      }
    else
      {
      this->GroupNameIterator = this->GroupNames.begin();
      }
  }
  //-------------------------------------------------------------------------
  bool IsDoneWithGroupTraversal()
  {
    return this->GroupNames.size() == 0
        || this->GroupNameIterator == this->GroupNames.end();
  }
  //-------------------------------------------------------------------------
  bool IsDoneWithCoreTraversal()
  {
    if(this->CoreDefinitionMap && !this->InvalidCoreIterator)
      {
      return this->CoreProxyIterator == this->CoreProxyIteratorEnd;
      }
    return true;
  }
  //-------------------------------------------------------------------------
  bool IsDoneWithCustomTraversal()
  {
    if(this->CustomDefinitionMap && !this->InvalidCustomIterator)
      {
      return this->CustomProxyIterator == this->CustomProxyIteratorEnd;
      }
    return true;
  }
  //-------------------------------------------------------------------------
  void NextGroup()
  {
    this->CurrentGroupName = *GroupNameIterator;
    this->GroupNameIterator++;
    if(this->CoreDefinitionMap)
      {
      this->CoreProxyIterator    =
          (*this->CoreDefinitionMap)[this->CurrentGroupName].begin();
      this->CoreProxyIteratorEnd =
          (*this->CoreDefinitionMap)[this->CurrentGroupName].end();
      this->InvalidCoreIterator = false;
      }
    if(this->CustomDefinitionMap)
      {
      this->CustomProxyIterator    =
          (*this->CustomDefinitionMap)[this->CurrentGroupName].begin();
      this->CustomProxyIteratorEnd =
          (*this->CustomDefinitionMap)[this->CurrentGroupName].end();
      this->InvalidCustomIterator = false;
      }
  }
  //-------------------------------------------------------------------------
  void NextCoreDefinition()
  {
    this->CoreProxyIterator++;
  }
  //-------------------------------------------------------------------------
  void NextCustomDefinition()
  {
    this->CustomProxyIterator++;
  }

private:
  bool Initialized;
  vtkStdString CurrentGroupName;
  StrToXmlMap::iterator CoreProxyIterator;
  StrToXmlMap::iterator CoreProxyIteratorEnd;
  StrToXmlMap::iterator CustomProxyIterator;
  StrToXmlMap::iterator CustomProxyIteratorEnd;
  StrToStrToXmlMap* CoreDefinitionMap;
  StrToStrToXmlMap* CustomDefinitionMap;
  vtkstd::set<vtkStdString> GroupNames;
  vtkstd::set<vtkStdString>::iterator GroupNameIterator;
  bool InvalidCoreIterator;
  bool InvalidCustomIterator;
};

//****************************************************************************/
vtkStandardNewMacro(vtkSIProxyDefinitionManager)
vtkStandardNewMacro(vtkInternalDefinitionIterator)
//---------------------------------------------------------------------------
vtkSIProxyDefinitionManager::vtkSIProxyDefinitionManager()
{
  this->Internals = new vtkInternals;
  this->InternalsFlatten = new vtkInternals;

  // Load the generated modules
# include "vtkParaViewIncludeModulesToSMApplication.h"

  // Now register with the plugin tracker, so that when new plugins are loaded,
  // we parse the XML if provided and automatically add it to the proxy
  // definitions.
  vtkPVPluginTracker* tracker = vtkPVPluginTracker::GetInstance();
  tracker->AddObserver( vtkCommand::RegisterEvent, this,
                        &vtkSIProxyDefinitionManager::OnPluginLoaded);
  // process any already loaded plugins.
  for (unsigned int cc=0; cc < tracker->GetNumberOfPlugins(); cc++)
    {
    this->HandlePlugin(tracker->GetPlugin(cc));
    }
}

//---------------------------------------------------------------------------
vtkSIProxyDefinitionManager::~vtkSIProxyDefinitionManager()
{
  delete this->Internals;
  delete this->InternalsFlatten;
}

//----------------------------------------------------------------------------
void vtkSIProxyDefinitionManager::InvokeCustomDefitionsUpdated()
{
  this->InvokeEvent(
    vtkSIProxyDefinitionManager::CompoundProxyDefinitionsUpdated);
  this->InvokeEvent(
    vtkSIProxyDefinitionManager::ProxyDefinitionsUpdated);
}

//----------------------------------------------------------------------------
void vtkSIProxyDefinitionManager::AddElement(const char* groupName,
                                             const char* proxyName,
                                             vtkPVXMLElement* element)
{
  bool updated = false;
  if (element->GetName() && strcmp(element->GetName(), "Extension") == 0)
    {
    // This is an extension for an existing definition.
    vtkPVXMLElement* coreElem = this->Internals->GetProxyElement(
        this->Internals->CoreDefinitions, groupName, proxyName);
    if(coreElem)
      {
      // We found it, so we can extend it
      for (unsigned int cc=0; cc < element->GetNumberOfNestedElements(); cc++)
        {
        coreElem->AddNestedElement(element->GetNestedElement(cc));
        }
      updated = true;
      }
    else
      {
      vtkWarningMacro("Extension for (" << groupName << ", " << proxyName
                      << ") ignored since could not find core definition.");
      }
    }
  else
    {
    // Just referenced it
    this->Internals->CoreDefinitions[groupName][proxyName] = element;
    updated = true;
    }

  if (updated)
    {
    // Let the world know that a core-definition was registered i.e. added or
    // modified.
    RegisteredDefinitionInformation info(groupName, proxyName, false);
    this->InvokeEvent(vtkCommand::RegisterEvent, &info);
    }
}

//---------------------------------------------------------------------------
vtkPVXMLElement* vtkSIProxyDefinitionManager::GetProxyDefinition(
                 const char* groupName, const char* proxyName,
                 const bool throwError)
{
  vtkPVXMLElement* element = this->Internals->GetProxyElement( groupName,
                                                               proxyName );
  if (!throwError || element)
    {
    return element;
    }
  else
    {
    vtkErrorMacro( << "No proxy that matches: group=" << groupName
                   << " and proxy=" << proxyName << " were found.");
    }
  return 0;
}

//---------------------------------------------------------------------------
void vtkSIProxyDefinitionManager::ClearCustomProxyDefinitions()
{
  this->Internals->CustomsDefinitions.clear();
  this->InvokeCustomDefitionsUpdated();
}

//---------------------------------------------------------------------------
void vtkSIProxyDefinitionManager::RemoveCustomProxyDefinition(
    const char* groupName, const char* proxyName)
{
  if (this->Internals->HasCustomDefinition(groupName, proxyName))
    {
    this->Internals->CustomsDefinitions[groupName].erase(proxyName);

    // Let the world know that definitions may have changed.
    RegisteredDefinitionInformation info(groupName, proxyName, true);
    this->InvokeEvent(vtkCommand::UnRegisterEvent, &info);
    this->InvokeCustomDefitionsUpdated();
    }
}

//---------------------------------------------------------------------------
void vtkSIProxyDefinitionManager::AttachShowInMenuHintsToProxy(vtkPVXMLElement* proxy)
{
  if(!proxy)
    {
    return;
    }

  vtkPVXMLElement* hints = proxy->FindNestedElementByName("Hints");
  if(hints == NULL)
    {
    vtkNew<vtkPVXMLElement> madehints;
    madehints->SetName("Hints");
    vtkNew<vtkPVXMLElement> showInMenu;
    showInMenu->SetName("ShowInMenu");
    madehints->AddNestedElement(showInMenu.GetPointer());
    proxy->AddNestedElement(madehints.GetPointer());
    }
  else if(hints->FindNestedElementByName("ShowInMenu") == NULL)
    {
    vtkNew<vtkPVXMLElement> showInMenu;
    showInMenu->SetName("ShowInMenu");
    hints->AddNestedElement(showInMenu.GetPointer());
    }
}

//---------------------------------------------------------------------------
void vtkSIProxyDefinitionManager::
    AttachShowInMenuHintsToProxyFromProxyGroups(vtkPVXMLElement* root)
{
  if(!root)
    {
    return;
    }

  if(!strcmp(root->GetName(), "ProxyGroup"))
    {
    if( !strcmp(root->GetAttribute("name"), "sources") ||
        !strcmp(root->GetAttribute("name"), "filters"))
      {
      int size = root->GetNumberOfNestedElements();
      for(int cc=0; cc < size; ++cc)
        {
        this->AttachShowInMenuHintsToProxy(root->GetNestedElement(cc));
        }
      }
    }
  else
    {
    vtkNew<vtkCollection> collection;
    root->FindNestedElementByName("ProxyGroup", collection.GetPointer());
    int size = collection->GetNumberOfItems();
    for(int cc=0; cc < size; ++cc)
      {
      vtkPVXMLElement* group =
          vtkPVXMLElement::SafeDownCast(collection->GetItemAsObject(cc));
      this->AttachShowInMenuHintsToProxyFromProxyGroups(group);
      }
    }
}

//---------------------------------------------------------------------------
void vtkSIProxyDefinitionManager::AddCustomProxyDefinition(
    const char* groupName, const char* proxyName, const char* xmlcontents)
{
  vtkNew<vtkPVXMLParser> parser;
  if (parser->Parse(xmlcontents))
    {
    this->AddCustomProxyDefinition(groupName, proxyName,
      parser->GetRootElement());
    }
  else
    {
    vtkErrorMacro("Failed to parse xml.");
    }
}

//---------------------------------------------------------------------------
void vtkSIProxyDefinitionManager::AddCustomProxyDefinition(
    const char* groupName, const char* proxyName, vtkPVXMLElement* top)
{
  if (this->AddCustomProxyDefinitionInternal(groupName, proxyName, top))
    {
    this->InvokeCustomDefitionsUpdated();
    }
}

//---------------------------------------------------------------------------
bool vtkSIProxyDefinitionManager::AddCustomProxyDefinitionInternal(
    const char* groupName, const char* proxyName, vtkPVXMLElement* top)
{
  if (!top)
    {
    return false;
    }

  // Attach automatic hints so it will show up in the menu
  if(!strcmp(groupName, "sources") || !strcmp(groupName, "filters"))
    {
    this->AttachShowInMenuHintsToProxy(top);
    }

  vtkPVXMLElement* currentCustomElement =
      this->Internals->GetProxyElement( this->Internals->CustomsDefinitions,
                                        groupName, proxyName);

  // There's a possibility that the custom proxy definition is the
  // state has already been loaded (or another proxy definition with
  // the same name exists). If that existing definition matches what
  // the state is requesting, we don't need to raise any errors,
  // simply skip it.
  if(currentCustomElement && !currentCustomElement->Equals(top))
    {
    // A definition already exist with not the same content
    vtkErrorMacro("Proxy definition has already been registered with name \""
                  << proxyName << "\" under group \"" << groupName <<"\".");
    return false;
    }
  else
    {
    this->Internals->CustomsDefinitions[groupName][proxyName] = top;

    // Let the world know that definitions may have changed.
    RegisteredDefinitionInformation info(groupName, proxyName, true);
    this->InvokeEvent(vtkCommand::RegisterEvent, &info);
    return true;
    }
}

//---------------------------------------------------------------------------
void vtkSIProxyDefinitionManager::LoadCustomProxyDefinitions(vtkPVXMLElement* root)
{
  if (!root)
    {
    return;
    }

  bool updated = true;
  vtksys::RegularExpression proxyDefRe(".*Proxy$");
  unsigned int numElems = root->GetNumberOfNestedElements();
  for (unsigned int i=0; i<numElems; i++)
    {
    vtkPVXMLElement* currentElement = root->GetNestedElement(i);
    if ( currentElement->GetName() &&
         strcmp(currentElement->GetName(), "CustomProxyDefinition") == 0 )
      {
      vtkstd::string group = currentElement->GetAttributeOrEmpty("group");
      vtkstd::string name = currentElement->GetAttributeOrEmpty("name");
      if (!name.empty() && !group.empty())
        {
        if (currentElement->GetNumberOfNestedElements() == 1)
          {
          vtkPVXMLElement* defElement = currentElement->GetNestedElement(0);
          const char* tagName = defElement->GetName();
          if (tagName && proxyDefRe.find(tagName))
            {
            // Register custom proxy definitions for all elements ending with
            // "Proxy".
            updated = this->AddCustomProxyDefinitionInternal(group.c_str(),
              name.c_str(), defElement) || updated;
            }
          }
        }
      else
        {
        vtkErrorMacro("Missing name or group");
        }
      }
    }
  if (updated)
    {
    this->InvokeCustomDefitionsUpdated();
    }
}

//---------------------------------------------------------------------------
void vtkSIProxyDefinitionManager::LoadCustomProxyDefinitionsFromString(
  const char* xmlContent)
{
  vtkNew<vtkPVXMLParser> parser;
  if (parser->Parse(xmlContent))
    {
    this->LoadCustomProxyDefinitions(parser->GetRootElement());
    }
}

//---------------------------------------------------------------------------
void vtkSIProxyDefinitionManager::SaveCustomProxyDefinitions(
  vtkPVXMLElement* root)
{
  assert(root != NULL);

  vtkPVProxyDefinitionIterator* iter =
      this->NewIterator(vtkSIProxyDefinitionManager::CUSTOM_DEFINITIONS);
  while(!iter->IsDoneWithTraversal())
    {
    vtkPVXMLElement* elem = iter->GetProxyDefinition();
    if (elem)
      {
      vtkNew<vtkPVXMLElement> defElement;
      defElement->SetName("CustomProxyDefinition");
      defElement->AddAttribute("name", iter->GetProxyName());
      defElement->AddAttribute("group", iter->GetGroupName());
      defElement->AddNestedElement(elem, 0);
      root->AddNestedElement(defElement.GetPointer());
      }
    iter->GoToNextItem();
    }
  iter->Delete();
}

//---------------------------------------------------------------------------
bool vtkSIProxyDefinitionManager::LoadConfigurationXMLFromString(const char* xmlContent)
{
  return this->LoadConfigurationXMLFromString(xmlContent, false);
}
//---------------------------------------------------------------------------
bool vtkSIProxyDefinitionManager::LoadConfigurationXMLFromString( const char* xmlContent,
                                                                  bool attachHints)
{
  vtkNew<vtkPVXMLParser> parser;
  return (parser->Parse(xmlContent) != 0) &&
      this->LoadConfigurationXML( parser->GetRootElement(), attachHints );
}

//---------------------------------------------------------------------------
bool vtkSIProxyDefinitionManager::LoadConfigurationXML(vtkPVXMLElement* root)
{
  return this->LoadConfigurationXML(root, false);
}

//---------------------------------------------------------------------------
bool vtkSIProxyDefinitionManager::LoadConfigurationXML(vtkPVXMLElement* root, bool attachHints)
{
  if (!root)
    {
    vtkErrorMacro("Must parse a configuration before storing it.");
    return false;
    }

  // Attach ShowInMenu hints
  if(attachHints)
    {
    this->AttachShowInMenuHintsToProxyFromProxyGroups(root);
    }

  // Loop over the top-level elements.
  for (unsigned int i=0; i < root->GetNumberOfNestedElements(); ++i)
    {
    vtkPVXMLElement* group = root->GetNestedElement(i);
    vtkstd::string groupName = group->GetAttributeOrEmpty("name");
    vtkstd::string proxyName;

    // Loop over the top-level elements.
    for(unsigned int cc=0; cc < group->GetNumberOfNestedElements(); ++cc)
      {
      vtkPVXMLElement* proxy = group->GetNestedElement(cc);
      proxyName = proxy->GetAttributeOrEmpty("name");
      if (!proxyName.empty())
        {
        this->AddElement(groupName.c_str(), proxyName.c_str(), proxy);
        }
      }
    }
  this->InvokeEvent(vtkSIProxyDefinitionManager::ProxyDefinitionsUpdated);
  return true;
}

//---------------------------------------------------------------------------
void vtkSIProxyDefinitionManager::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}
//---------------------------------------------------------------------------
// vtkSIProxyDefinitionManager::ALL_DEFINITIONS    = 0
// vtkSIProxyDefinitionManager::CORE_DEFINITIONS   = 1
// vtkSIProxyDefinitionManager::CUSTOM_DEFINITIONS = 2
vtkPVProxyDefinitionIterator*
    vtkSIProxyDefinitionManager::NewSingleGroupIterator(char const* groupName,
                                                        int scope)
{
  vtkPVProxyDefinitionIterator* iterator = this->NewIterator(scope);
  iterator->AddTraversalGroupName(groupName);
  return iterator;
}
//---------------------------------------------------------------------------
// vtkSIProxyDefinitionManager::ALL_DEFINITIONS    = 0
// vtkSIProxyDefinitionManager::CORE_DEFINITIONS   = 1
// vtkSIProxyDefinitionManager::CUSTOM_DEFINITIONS = 2
vtkPVProxyDefinitionIterator* vtkSIProxyDefinitionManager::NewIterator(int scope)
{
  vtkInternalDefinitionIterator* iterator = vtkInternalDefinitionIterator::New();
  switch(scope)
    {
    case vtkSIProxyDefinitionManager::CORE_DEFINITIONS: // Core only
      iterator->RegisterCoreDefinitionMap( & this->Internals->CoreDefinitions);
      break;
    case vtkSIProxyDefinitionManager::CUSTOM_DEFINITIONS: // Custom only
      iterator->RegisterCustomDefinitionMap( & this->Internals->CustomsDefinitions);
      break;
    default: // Both
      iterator->RegisterCoreDefinitionMap( & this->Internals->CoreDefinitions);
      iterator->RegisterCustomDefinitionMap( & this->Internals->CustomsDefinitions);
      break;
    }
  return iterator;
}
//---------------------------------------------------------------------------
void vtkSIProxyDefinitionManager::InvalidateCollapsedDefinition()
{
  this->InternalsFlatten->CoreDefinitions.clear();
}
//---------------------------------------------------------------------------
vtkPVXMLElement* vtkSIProxyDefinitionManager::ExtractSubProxy(
    vtkPVXMLElement* proxyDefinition, const char* subProxyName)
{
  if(!subProxyName)
    {
    return proxyDefinition;
    }

  vtksys::RegularExpression proxyDefRe(".*Proxy$");

  // Extract just the sub-proxy in-line definition
  for(unsigned int cc=0;cc<proxyDefinition->GetNumberOfNestedElements();cc++)
    {
    if(!strcmp(proxyDefinition->GetNestedElement(cc)->GetName(), "SubProxy"))
      {
      unsigned int nbChildren =
          proxyDefinition->GetNestedElement(cc)->GetNumberOfNestedElements();
      for(unsigned int childIdx = 0; childIdx < nbChildren; childIdx++)
        {
        vtkPVXMLElement* subProxyDef =
            proxyDefinition->GetNestedElement(cc)->GetNestedElement(childIdx);
        // Look for element name that are ending with "Proxy"
        const char* tagname = subProxyDef->GetName();
        if ( tagname && proxyDefRe.find(tagname) &&
             !strcmp(subProxyDef->GetAttribute("name"), subProxyName) )
          {
          return subProxyDef;
          }
        }
      }
    }

  return NULL;
}

//---------------------------------------------------------------------------
vtkPVXMLElement* vtkSIProxyDefinitionManager::GetCollapsedProxyDefinition(
    const char* group, const char* name, const char* subProxyName, bool throwError)
{
  // Look in the cache
  vtkPVXMLElement* flattenDefinition =
      this->InternalsFlatten->GetProxyElement(group,name);
  if (flattenDefinition)
    {
    // Found it, so return it...
    return this->ExtractSubProxy(flattenDefinition, subProxyName);
    }

  // Not found in the cache, look if the definition exists
  vtkPVXMLElement* originalDefinition =
      this->GetProxyDefinition(group, name, throwError);

  // Look for parent hierarchy
  if(originalDefinition)
    {
    vtkPVXMLElement* realDefinition = originalDefinition;
    vtkstd::string base_group =
        originalDefinition->GetAttributeOrEmpty("base_proxygroup");
    vtkstd::string base_name =
        originalDefinition->GetAttributeOrEmpty("base_proxyname");

    if( !base_group.empty() && !base_name.empty())
      {
      vtkstd::vector<vtkPVXMLElement*> classHierarchy;
      while(originalDefinition)
        {
        classHierarchy.push_back(originalDefinition);
        if(!base_group.empty() && !base_name.empty())
          {
          originalDefinition = this->GetProxyDefinition( base_group.c_str(),
                                                         base_name.c_str(),
                                                         throwError);
          base_group =
              originalDefinition->GetAttributeOrEmpty("base_proxygroup");
          base_name  =
              originalDefinition->GetAttributeOrEmpty("base_proxyname");
          }
        else
          {
          originalDefinition = 0;
          }
        }

      // Build the flattened version of it
      vtkNew<vtkPVXMLElement> newElement;
      while(classHierarchy.size() > 0)
        {
        vtkPVXMLElement* currentElement = classHierarchy.back();
        classHierarchy.pop_back();
        this->MergeProxyDefinition(currentElement, newElement.GetPointer());
        }
      realDefinition->CopyAttributesTo(newElement.GetPointer());

      // Remove parent declaration
      newElement->RemoveAttribute("base_proxygroup");
      newElement->RemoveAttribute("base_proxyname");

      // Register it in the cache
      this->InternalsFlatten->CoreDefinitions[group][name] = newElement.GetPointer();

      return this->ExtractSubProxy(newElement.GetPointer(), subProxyName);
      }
    }

  // Could be either the original definition or a NULL pointer if not found
  return this->ExtractSubProxy(originalDefinition, subProxyName);
}
//---------------------------------------------------------------------------
void vtkSIProxyDefinitionManager::MergeProxyDefinition(vtkPVXMLElement* element,
                                                       vtkPVXMLElement* elementToFill)
{
  // Meta-data of elementToFill
  vtkstd::map<vtkstd::string, vtkSmartPointer<vtkPVXMLElement> > subProxyToFill;
  vtkstd::map<vtkstd::string, vtkSmartPointer<vtkPVXMLElement> > propertiesToFill;
  vtkInternals::ExtractMetaInformation( elementToFill,
                                        subProxyToFill,
                                        propertiesToFill);

  // Meta-data of element that should be merged into the other
  vtkstd::map<vtkstd::string, vtkSmartPointer<vtkPVXMLElement> > subProxySrc;
  vtkstd::map<vtkstd::string, vtkSmartPointer<vtkPVXMLElement> > propertiesSrc;
  vtkInternals::ExtractMetaInformation( element,
                                        subProxySrc,
                                        propertiesSrc);

  // Look for conflicting sub-proxy name and remove their definition if override
  vtkstd::map<vtkstd::string, vtkSmartPointer<vtkPVXMLElement> >::iterator mapIter;
  mapIter = subProxyToFill.begin();
  while( mapIter != subProxyToFill.end())
    {
    vtkstd::string name = mapIter->first;
    if( subProxySrc.find(name) != subProxySrc.end() )
      {
      if (!subProxySrc[name]->GetAttribute("override"))
        {
        vtkWarningMacro("#####################################" << endl
                        << "Find conflict between 2 SubProxy name. ("
                        << name.c_str() << ")" << endl
                        << "#####################################" << endl);
        return;
        }
      else
        { // Remove the given subProxy of the Element to Fill
        vtkPVXMLElement *subProxyDefToRemove = subProxyToFill[name].GetPointer();
        subProxyDefToRemove->GetParent()->RemoveNestedElement(subProxyDefToRemove);
        }
      }
    // Move to next
    mapIter++;
    }

  // Look for conflicting property name and remove their definition if override
  mapIter = propertiesToFill.begin();
  while( mapIter != propertiesToFill.end())
    {
    vtkstd::string name = mapIter->first;
    if( propertiesSrc.find(name) != propertiesSrc.end())
      {
      if (!propertiesSrc[name]->GetAttribute("override") &&
          !propertiesSrc[name]->GetParent()->GetParent()->FindNestedElementByName("Proxy")->GetAttribute("override"))
        {
        vtkWarningMacro(<< "Find conflict between 2 property name. ("
                        << name.c_str() << ")");
        return;
        }
      else
        { // Remove the given property of the Element to Fill
        vtkPVXMLElement *subPropDefToRemove = propertiesToFill[name].GetPointer();
        subPropDefToRemove->GetParent()->RemoveNestedElement(subPropDefToRemove);
        }
      }
    // Move to next
    mapIter++;
    }

  // By default alway overide the documentation
  if( element->FindNestedElementByName("Documentation") &&
      elementToFill->FindNestedElementByName("Documentation"))
    {
    elementToFill->RemoveNestedElement(
        elementToFill->FindNestedElementByName("Documentation"));
    }

  // Fill the output with all the input elements
  unsigned int numChildren = element->GetNumberOfNestedElements();
  unsigned int cc;
  for (cc=0; cc < numChildren; cc++)
    {
    vtkPVXMLElement* child = element->GetNestedElement(cc);
    vtkNew<vtkPVXMLElement> newElement;
    child->CopyTo(newElement.GetPointer());
    elementToFill->AddNestedElement(newElement.GetPointer());
    }
}

//---------------------------------------------------------------------------
void vtkSIProxyDefinitionManager::Pull(vtkSMMessage* msg)
{
  // Setup required message header
  msg->Clear();
  msg->set_global_id(vtkSIProxyDefinitionManager::GetReservedGlobalID());
  msg->set_location(vtkPVSession::DATA_SERVER);

  // This is made in a naive way, but we are sure that at each request
  // we have the correct and latest definition available.
  // This is not the most efficient way to do it. But optimistation should come
  // after. And for now, it is the less intrusive way to deal with server
  // XML definition centralisation state.
  ProxyDefinitionState_ProxyXMLDefinition *xmlDef;
  vtkPVProxyDefinitionIterator* iter;

  // Core Definition
  iter = this->NewIterator(vtkSIProxyDefinitionManager::CORE_DEFINITIONS);
  iter->GoToFirstItem();
  while( !iter->IsDoneWithTraversal() )
    {
    vtkstd::ostringstream xmlContent;
    iter->GetProxyDefinition()->PrintXML(xmlContent, vtkIndent());

    xmlDef = msg->AddExtension(ProxyDefinitionState::xml_definition_proxy);
    xmlDef->set_group(iter->GetGroupName());
    xmlDef->set_name(iter->GetProxyName());
    xmlDef->set_xml(xmlContent.str());

    iter->GoToNextItem();
    }
  iter->Delete();

  // Custome Definition
  iter = this->NewIterator(vtkSIProxyDefinitionManager::CUSTOM_DEFINITIONS);
  iter->GoToFirstItem();
  while( !iter->IsDoneWithTraversal() )
    {
    vtkstd::ostringstream xmlContent;
    iter->GetProxyDefinition()->PrintXML(xmlContent, vtkIndent());

    xmlDef = msg->AddExtension(ProxyDefinitionState::xml_custom_definition_proxy);
    xmlDef->set_group(iter->GetGroupName());
    xmlDef->set_name(iter->GetProxyName());
    xmlDef->set_xml(xmlContent.str());

    iter->GoToNextItem();
    }
  iter->Delete();
}

//---------------------------------------------------------------------------
void vtkSIProxyDefinitionManager::Push(vtkSMMessage* msg)
{
  vtkTimerLog::MarkStartEvent("vtkSIProxyDefinitionManager Load Definitions");
  // Init and local vars
  this->Internals->Clear();
  this->InternalsFlatten->Clear();
  vtkNew<vtkPVXMLParser> parser;

  // Fill the definition with the content of the state
  int size = msg->ExtensionSize(ProxyDefinitionState::xml_definition_proxy);
  const ProxyDefinitionState_ProxyXMLDefinition *xmlDef;
  for(int i=0; i < size; i++)
    {
    xmlDef = &msg->GetExtension(ProxyDefinitionState::xml_definition_proxy, i);
    parser->Parse(xmlDef->xml().c_str());
    this->AddElement( xmlDef->group().c_str(), xmlDef->name().c_str(),
                      parser->GetRootElement());
    }

  // Manage custom ones
  int custom_size = msg->ExtensionSize(ProxyDefinitionState::xml_custom_definition_proxy);
  for(int i=0; i < custom_size; i++)
    {
    xmlDef = &msg->GetExtension(ProxyDefinitionState::xml_custom_definition_proxy, i);
    parser->Parse(xmlDef->xml().c_str());
    this->AddCustomProxyDefinitionInternal(
      xmlDef->group().c_str(), xmlDef->name().c_str(), parser->GetRootElement());
    }

  if (custom_size > 0)
    {
    this->InvokeEvent(vtkSIProxyDefinitionManager::CompoundProxyDefinitionsUpdated);
    }

  this->InvokeEvent(vtkSIProxyDefinitionManager::ProxyDefinitionsUpdated);
  vtkTimerLog::MarkEndEvent("vtkSIProxyDefinitionManager Load Definitions");
}

//---------------------------------------------------------------------------
void vtkSIProxyDefinitionManager::OnPluginLoaded(
  vtkObject*, unsigned long, void* calldata)
{
  this->HandlePlugin(reinterpret_cast<vtkPVPlugin*>(calldata));
}

//---------------------------------------------------------------------------
void vtkSIProxyDefinitionManager::HandlePlugin(vtkPVPlugin* plugin)
{
  vtkPVServerManagerPluginInterface* smplugin =
    dynamic_cast<vtkPVServerManagerPluginInterface*>(plugin);
  if (smplugin)
    {
    vtkstd::vector<vtkstd::string> xmls;
    smplugin->GetXMLs(xmls);
    for (size_t cc=0; cc < xmls.size(); cc++)
      {
      this->LoadConfigurationXMLFromString(xmls[cc].c_str(), true);
      }

    // Make sure we invalidate any cached flatten version of our proxy definition
    this->InternalsFlatten->Clear();
    }
}
//---------------------------------------------------------------------------
bool vtkSIProxyDefinitionManager::HasDefinition( const char* groupName,
                                                 const char* proxyName)
{
  return this->Internals->HasCustomDefinition(groupName, proxyName) ||
      this->Internals->HasCoreDefinition(groupName, proxyName);
}

//---------------------------------------------------------------------------
// For now we dynamically convert InformationHelper
// into the correct si_class and attribute sets.
// THIS CODE MUST BE REMOVED once InformationHelper have been removed from
// legacy XML
void vtkSIProxyDefinitionManager::PatchXMLProperty(vtkPVXMLElement* propElement)
{
  vtkPVXMLElement* informationHelper = NULL;

  // Search InformationHelper XML element
  for(unsigned int i=0; i < propElement->GetNumberOfNestedElements(); ++i)
    {
    vtkPVXMLElement* currentElement = propElement->GetNestedElement(i);
    if ( vtkstd::string(currentElement->GetName()).find("Helper") !=
         vtkstd::string::npos)
      {
      informationHelper = currentElement;
      break;
      }
    }

  // Process InformationHelper
  if(informationHelper)
    {
    if(  !strcmp(informationHelper->GetName(),"StringArrayHelper")
      || !strcmp(informationHelper->GetName(),"DoubleArrayInformationHelper")
      || !strcmp(informationHelper->GetName(),"IntArrayInformationHelper")
      || !strcmp(informationHelper->GetName(),"IdTypeArrayInformationHelper"))
      {
      propElement->SetAttribute("si_class", "vtkSIDataArrayProperty");
      }
    else if (!strcmp(informationHelper->GetName(),"TimeStepsInformationHelper"))
      {
      propElement->SetAttribute("si_class", "vtkSITimeStepsProperty");
      }
    else if (!strcmp(informationHelper->GetName(),"TimeRangeInformationHelper"))
      {
      propElement->SetAttribute("si_class", "vtkSITimeRangeProperty");
      }
    else if (!strcmp(informationHelper->GetName(),"SILInformationHelper"))
      {
      propElement->SetAttribute("si_class", "vtkSISILProperty");
      propElement->SetAttribute("subtree", informationHelper->GetAttribute("subtree"));
      }
    else if (!strcmp(informationHelper->GetName(),"ArraySelectionInformationHelper"))
      {
      propElement->SetAttribute("si_class", "vtkSIArraySelectionProperty");
      propElement->SetAttribute("command", informationHelper->GetAttribute("attribute_name"));
      propElement->SetAttribute("number_of_elements_per_command", "2");
      }
    else if(!strcmp(informationHelper->GetName(),"SimpleDoubleInformationHelper")
      || !strcmp(informationHelper->GetName(),"SimpleIntInformationHelper")
      || !strcmp(informationHelper->GetName(),"SimpleStringInformationHelper")
      || !strcmp(informationHelper->GetName(),"SimpleIdTypeInformationHelper"))
      {
      // Nothing to do, just remove them
      }
    else
      {
      cerr << "No SIProperty for the following information helper: "
           << informationHelper->GetName() << endl;
      }

    // Remove InformationHelper from XML
    propElement->RemoveNestedElement(informationHelper);
    }
}
//----------------------------------------------------------------------------
vtkTypeUInt32 vtkSIProxyDefinitionManager::GetReservedGlobalID()
{
  return vtkReservedRemoteObjectIds::RESERVED_PROXY_DEFINITION_MANAGER_ID;
}
