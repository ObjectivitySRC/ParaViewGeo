/*=========================================================================

  Program:   ParaView
  Module:    vtkSMCompoundProxyDefinitionLoader.h

  Copyright (c) Kitware, Inc.
  All rights reserved.
  See Copyright.txt or http://www.paraview.org/HTML/Copyright.html for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// .NAME vtkSMCompoundProxyDefinitionLoader - Creates a compound proxy from an
// XML definition.
// .SECTION Description
// vtkSMCompoundProxyDefinitionLoader can load a compound proxy definition
// from a given vtkPVXMLElement. This element can be populated by a 
// vtkPVXMLElement or obtained from the proxy manager.
// .SECTION See Also
// vtkPVXMLElement vtkPVXMLParser vtkSMProxyManager

#ifndef __vtkSMCompoundProxyDefinitionLoader_h
#define __vtkSMCompoundProxyDefinitionLoader_h

#include "vtkSMDeserializer.h"

class vtkPVXMLElement;

class VTK_EXPORT vtkSMCompoundProxyDefinitionLoader : public vtkSMDeserializer
{
public:
  static vtkSMCompoundProxyDefinitionLoader* New();
  vtkTypeMacro(vtkSMCompoundProxyDefinitionLoader, vtkSMDeserializer);
  void PrintSelf(ostream& os, vtkIndent indent);

  virtual void SetRootElement(vtkPVXMLElement*);

protected:
  vtkSMCompoundProxyDefinitionLoader();
  ~vtkSMCompoundProxyDefinitionLoader();

  // Description:
  // Locate the XML for the proxy with the given id.
  virtual vtkPVXMLElement* LocateProxyElement(int id);

  vtkPVXMLElement* RootElement;
private:
  vtkSMCompoundProxyDefinitionLoader(const vtkSMCompoundProxyDefinitionLoader&); // Not implemented
  void operator=(const vtkSMCompoundProxyDefinitionLoader&); // Not implemented
};

#endif
