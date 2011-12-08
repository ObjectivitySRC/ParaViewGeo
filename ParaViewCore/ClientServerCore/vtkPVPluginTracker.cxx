/*=========================================================================

  Program:   ParaView
  Module:    vtkPVPluginTracker.cxx

  Copyright (c) Kitware, Inc.
  All rights reserved.
  See Copyright.txt or http://www.paraview.org/HTML/Copyright.html for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "vtkPVPluginTracker.h"

#include "vtkClientServerInterpreterInitializer.h"
#include "vtkCommand.h"
#include "vtkObjectFactory.h"
#include "vtkProcessModule.h"
#include "vtkPVConfig.h"
#include "vtkPVOptions.h"
#include "vtkPVPlugin.h"
#include "vtkPVPluginLoader.h"
#include "vtkPVPythonModule.h"
#include "vtkPVPythonPluginInterface.h"
#include "vtkPVServerManagerPluginInterface.h"
#include "vtkPVXMLElement.h"
#include "vtkPVXMLParser.h"

#include <assert.h>
#include <vtkstd/string>
#include <vtkstd/vector>
#include <vtksys/ios/sstream>
#include <vtksys/SystemTools.hxx>
#include <vtksys/String.hxx>


#if defined(WIN32) && !defined(__CYGWIN__)
/* String comparison routine. */
# define VTKSTRNCASECMP _strnicmp
#else
# include "strings.h"
# define VTKSTRNCASECMP strncasecmp
#endif


#define vtkPVPluginTrackerDebugMacro(x)\
{ if (debug_plugin) {\
  vtksys_ios::ostringstream vtkerror;\
  vtkerror << x << endl;\
  vtkOutputWindowDisplayText(vtkerror.str().c_str());} }

namespace
{
  class vtkItem
    {
  public:
    vtkstd::string FileName;
    vtkstd::string PluginName;
    vtkPVPlugin* Plugin;
    bool AutoLoad;
    vtkItem()
      {
      this->Plugin = NULL;
      this->AutoLoad = false;
      }
    };

  vtkstd::string vtkLocatePlugin(const char* plugin, bool add_extensions)
    {
    // Make sure we can get the options before going further
    if(vtkProcessModule::GetProcessModule() == NULL)
      {
      return vtkstd::string();
      }

    bool debug_plugin = vtksys::SystemTools::GetEnv("PV_PLUGIN_DEBUG") != NULL;
    vtkPVOptions* options = vtkProcessModule::GetProcessModule()->GetOptions();
    vtkstd::string app_dir = options->GetApplicationPath();
    app_dir = vtksys::SystemTools::GetProgramPath(app_dir.c_str());

    vtkstd::vector<vtkstd::string> paths_to_search;
    paths_to_search.push_back(app_dir);
    paths_to_search.push_back(app_dir + "/plugins/" + plugin);
#if defined(__APPLE__)
    paths_to_search.push_back(app_dir + "/../Plugins");
    paths_to_search.push_back(app_dir + "/../../..");
#endif
    // On windows configuration files are in the parent directory
    paths_to_search.push_back(app_dir + "/../");

    vtkstd::string name = plugin;
    vtkstd::string filename = name;
    if (add_extensions)
      {
#if defined(_WIN32) && !defined(__CYGWIN__)
      filename = name + ".dll";
#elif defined(__APPLE__)
      filename = "lib" + name + ".dylib";
#else
      filename = "lib" + name + ".so";
#endif
      }
    for (size_t cc=0; cc < paths_to_search.size(); cc++)
      {
      vtkstd::string path = paths_to_search[cc];
      if (vtksys::SystemTools::FileExists(
          (path + "/" + filename).c_str(), true))
        {
        return (path + "/" + filename);
        }
      vtkPVPluginTrackerDebugMacro(
        (path + "/" + filename).c_str() << "-- not found");
      }
    return vtkstd::string();
    }

  vtkstd::string vtkGetPluginNameFromFileName(const vtkstd::string& filename)
    {
    vtkstd::string defaultname =
      vtksys::SystemTools::GetFilenameWithoutExtension(filename);
    if (defaultname.size() > 3 &&
      VTKSTRNCASECMP(defaultname.c_str(), "lib", 3) == 0)
      {
      defaultname.erase(0, 3);
      }
    return defaultname;
    }
}

class vtkPVPluginTracker::vtkPluginsList :
  public vtkstd::vector<vtkItem>
{
public:
  iterator LocateUsingPluginName(const char* pluginname)
    {
    for (iterator iter = this->begin(); iter != this->end(); ++iter)
      {
      if (iter->PluginName == pluginname)
        {
        return iter;
        }
      }
    return this->end();
    }

  iterator LocateUsingFileName(const char* filename)
    {
    for (iterator iter = this->begin(); iter != this->end(); ++iter)
      {
      if (iter->FileName == filename)
        {
        return iter;
        }
      }
    return this->end();
    }
};

vtkStandardNewMacro(vtkPVPluginTracker);
//----------------------------------------------------------------------------
vtkPVPluginTracker::vtkPVPluginTracker()
{
  this->PluginsList = new vtkPluginsList();
}

//----------------------------------------------------------------------------
vtkPVPluginTracker::~vtkPVPluginTracker()
{
  delete this->PluginsList;
  this->PluginsList = NULL;
}

//----------------------------------------------------------------------------
vtkPVPluginTracker* vtkPVPluginTracker::GetInstance()
{
  static vtkSmartPointer<vtkPVPluginTracker> Instance;
  if (Instance.GetPointer() == NULL)
    {
    vtkPVPluginTracker* mgr = vtkPVPluginTracker::New();
    Instance = mgr;
    mgr->FastDelete();

    bool debug_plugin = vtksys::SystemTools::GetEnv("PV_PLUGIN_DEBUG") != NULL;
    vtkPVPluginTrackerDebugMacro("Locate and load distributed plugin list.");

    // Locate ".plugins" file and process it.
    // This will setup the distributed-list of plugins. Also it will load any
    // auto-load plugins.
    vtkstd::string _plugins = vtkLocatePlugin(".plugins", false);
    if (!_plugins.empty())
      {
      mgr->LoadPluginConfigurationXML(_plugins.c_str());
      }
    else
      {
      vtkPVPluginTrackerDebugMacro(
        "Could not find .plugins file for distributed plugins");
      }

    // Now load any plugins located in the PV_PLUGIN_PATH environment variable.
    // These are always loaded (not merely located).
    vtkPVPluginLoader* loader = vtkPVPluginLoader::New();
    loader->LoadPluginsFromPluginSearchPath();
    loader->Delete();
    }

  return Instance;
}

//----------------------------------------------------------------------------
void vtkPVPluginTracker::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}

//----------------------------------------------------------------------------
void vtkPVPluginTracker::LoadPluginConfigurationXML(const char* filename)
{
  bool debug_plugin = vtksys::SystemTools::GetEnv("PV_PLUGIN_DEBUG") != NULL;
  vtkPVPluginTrackerDebugMacro("Loading plugin configuration xml: " << filename);
  if (!vtksys::SystemTools::FileExists(filename, true))
    {
    vtkPVPluginTrackerDebugMacro("Failed to located configuration xml. "
      "Could not populate the list of plugins distributed with application.");
    return;
    }

  vtkSmartPointer<vtkPVXMLParser> parser = vtkSmartPointer<vtkPVXMLParser>::New();
  parser->SetFileName(filename);
  parser->SuppressErrorMessagesOn();
  if (!parser->Parse())
    {
    vtkPVPluginTrackerDebugMacro("Configuration file not a valid xml.");
    return;
    }

  this->LoadPluginConfigurationXML(parser->GetRootElement());
}

//----------------------------------------------------------------------------
void vtkPVPluginTracker::LoadPluginConfigurationXMLFromString(const char* xmlcontents)
{
  bool debug_plugin = vtksys::SystemTools::GetEnv("PV_PLUGIN_DEBUG") != NULL;
  vtkSmartPointer<vtkPVXMLParser> parser = vtkSmartPointer<vtkPVXMLParser>::New();
  parser->SuppressErrorMessagesOn();
  if (!parser->Parse(xmlcontents))
    {
    vtkPVPluginTrackerDebugMacro("Configuration file not a valid xml.");
    return;
    }

  this->LoadPluginConfigurationXML(parser->GetRootElement());
}

//----------------------------------------------------------------------------
void vtkPVPluginTracker::LoadPluginConfigurationXML(vtkPVXMLElement* root)
{
  if (root == NULL)
    {
    return;
    }

  bool debug_plugin = vtksys::SystemTools::GetEnv("PV_PLUGIN_DEBUG") != NULL;
  if (strcmp(root->GetName(), "Plugins") != 0)
    {
    vtkPVPluginTrackerDebugMacro("Root element in the xml must be <Plugins/>. "
      "Got " << root->GetName());
    return;
    }

  for (unsigned int cc=0; cc < root->GetNumberOfNestedElements(); cc++)
    {
    vtkPVXMLElement* child = root->GetNestedElement(cc);
    if (child && child->GetName() && strcmp(child->GetName(), "Plugin") == 0)
      {
      vtkstd::string name = child->GetAttributeOrEmpty("name");
      int auto_load;
      if (name.empty() || !child->GetScalarAttribute("auto_load", &auto_load))
        {
        vtkPVPluginTrackerDebugMacro(
          "Missing required attribute name or auto_load. Skipping element.");
        continue;
        }
      vtkPVPluginTrackerDebugMacro("Trying to locate plugin with name: "
        << name.c_str());
      vtkstd::string plugin_filename;
      if (child->GetAttribute("filename") &&
        vtksys::SystemTools::FileExists(child->GetAttribute("filename"), true))
        {
        plugin_filename = child->GetAttribute("filename");
        }
      else
        {
        plugin_filename = vtkLocatePlugin(name.c_str(), true);
        }
      if (plugin_filename.empty())
        {
        int required = 0;
        child->GetScalarAttribute("required", &required);
        if (required)
          {
          vtkErrorMacro(
            "Failed to locate required plugin: " << name.c_str() << "\n"
            "Application may not work exactly as expected.");
          }
        vtkPVPluginTrackerDebugMacro("Failed to locate file plugin: "
          << name.c_str());
        continue;
        }
      vtkPVPluginTrackerDebugMacro("--- Found " << plugin_filename);
      unsigned int index = this->RegisterAvailablePlugin(plugin_filename.c_str());
      if (auto_load && !this->GetPluginLoaded(index))
        {
        // load the plugin.
        vtkPVPluginLoader* loader = vtkPVPluginLoader::New();
        loader->LoadPlugin(plugin_filename.c_str());
        loader->Delete();
        }
      (*this->PluginsList)[index].AutoLoad = (auto_load != 0);
      }
    }
}

//----------------------------------------------------------------------------
unsigned int vtkPVPluginTracker::GetNumberOfPlugins()
{
  return static_cast<unsigned int>(this->PluginsList->size());
}

//----------------------------------------------------------------------------
unsigned int vtkPVPluginTracker::RegisterAvailablePlugin(const char* filename)
{
  vtkstd::string defaultname = vtkGetPluginNameFromFileName(filename);
  vtkPluginsList::iterator iter =
    this->PluginsList->LocateUsingFileName(filename);
  if (iter == this->PluginsList->end())
    {
    iter = this->PluginsList->LocateUsingPluginName(defaultname.c_str());
    }
  if (iter == this->PluginsList->end())
    {
    vtkItem item;
    item.FileName = filename;
    item.PluginName = defaultname;
    this->PluginsList->push_back(item);
    return static_cast<unsigned int>(this->PluginsList->size()-1);
    }
  else
    {
    iter->FileName = filename;
    return static_cast<unsigned int>(iter - this->PluginsList->begin());
    }
}

//----------------------------------------------------------------------------
void vtkPVPluginTracker::RegisterPlugin(vtkPVPlugin* plugin)
{
  assert(plugin != NULL);

  vtkPluginsList::iterator iter = this->PluginsList->LocateUsingPluginName(
    plugin->GetPluginName());
  // use filename for matching is present, that's a better test.
  if (plugin->GetFileName())
    {
    iter = this->PluginsList->LocateUsingFileName(plugin->GetFileName());
    }
  if (iter == this->PluginsList->end())
    {
    vtkItem item;
    item.FileName = plugin->GetFileName()? plugin->GetFileName() :
      "linked-in";
    item.PluginName = plugin->GetPluginName();
    item.Plugin = plugin;
    this->PluginsList->push_back(item);
    }
  else
    {
    iter->Plugin = plugin;
    if (plugin->GetFileName())
      {
      iter->FileName = plugin->GetFileName();
      }
    }

  // Do some basic processing of the plugin here itself.

  // If this plugin has functions for initializing the interpreter, we set them
  // up right now.
  vtkPVServerManagerPluginInterface* smplugin =
    dynamic_cast<vtkPVServerManagerPluginInterface*>(plugin);
  if (smplugin)
    {
    if (smplugin->GetInitializeInterpreterCallback())
      {
      // This also initializes any existing instances of
      // vtkClientServerInterpreter. Refer to
      // vtkClientServerInterpreterInitializer::RegisterCallback implementation
      // for details.
      vtkClientServerInterpreterInitializer::GetInitializer()->RegisterCallback(
        smplugin->GetInitializeInterpreterCallback());
      }
    }

  // If this plugin has Python modules, process those.
  vtkPVPythonPluginInterface* pythonplugin =
    dynamic_cast<vtkPVPythonPluginInterface*>(plugin);
  if (pythonplugin)
    {
    vtkstd::vector<vtkstd::string> modules, sources;
    vtkstd::vector<int> package_flags;
    pythonplugin->GetPythonSourceList(modules, sources, package_flags);
    assert(modules.size() == sources.size() &&
      sources.size() == package_flags.size());
    for (size_t cc=0; cc < modules.size(); cc++)
      {
      vtkPVPythonModule* module = vtkPVPythonModule::New();
      module->SetFullName(modules[cc].c_str());
      module->SetSource(sources[cc].c_str());
      module->SetIsPackage(package_flags[cc]);
      vtkPVPythonModule::RegisterModule(module);
      module->Delete();
      }
    }

  this->InvokeEvent(vtkCommand::RegisterEvent, plugin);
}

//----------------------------------------------------------------------------
vtkPVPlugin* vtkPVPluginTracker::GetPlugin(unsigned int index)
{
  if (index >= this->GetNumberOfPlugins())
    {
    vtkWarningMacro("Invalid index: " << index);
    return NULL;
    }
  return (*this->PluginsList)[index].Plugin;
}

//----------------------------------------------------------------------------
const char* vtkPVPluginTracker::GetPluginName(unsigned int index)
{
if (index >= this->GetNumberOfPlugins())
    {
    vtkWarningMacro("Invalid index: " << index);
    return NULL;
    }
  return (*this->PluginsList)[index].PluginName.c_str();
}

//----------------------------------------------------------------------------
const char* vtkPVPluginTracker::GetPluginFileName(unsigned int index)
{
if (index >= this->GetNumberOfPlugins())
    {
    vtkWarningMacro("Invalid index: " << index);
    return NULL;
    }
  return (*this->PluginsList)[index].FileName.c_str();
}

//----------------------------------------------------------------------------
bool vtkPVPluginTracker::GetPluginLoaded(unsigned int index)
{
if (index >= this->GetNumberOfPlugins())
    {
    vtkWarningMacro("Invalid index: " << index);
    return false;
    }
  return (*this->PluginsList)[index].Plugin != NULL;
}

//----------------------------------------------------------------------------
bool vtkPVPluginTracker::GetPluginAutoLoad(unsigned int index)
{
  if (index >= this->GetNumberOfPlugins())
    {
    vtkWarningMacro("Invalid index: " << index);
    return false;
    }
  return (*this->PluginsList)[index].AutoLoad;
}
