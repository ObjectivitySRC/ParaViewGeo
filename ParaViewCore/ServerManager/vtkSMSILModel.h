/*=========================================================================

  Program:   ParaView
  Module:    vtkSMSILModel.h

  Copyright (c) Kitware, Inc.
  All rights reserved.
  See Copyright.txt or http://www.paraview.org/HTML/Copyright.html for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// .NAME vtkSMSILModel - is a helper for to work with SILs.
//
// .SECTION Description
// vtkSMSILModel makes it easier to make checks/unchecks for the SIL while
// respecting the links/dependencies defined by the SIL.
//
// There are two ways of initializing the model:
// \li One way is to initialize it with a SIL (using Initialize(vtkGraph*).
// Then the model can be used as a simple API to check/uncheck elements.
// \li Second way is to initialize with a proxy and property (using
// Initialize(vtkSMProxy, vtkSMProperty*). In that case, the SIL is obtained
// from  the property's vtkSMSILDomain. Also as the user changes the check
// states, the property is updated/pushed.
//
// .SECTION Events
// \li vtkCommand::UpdateDataEvent -- fired when the check state of any element
// changes. calldata = vertexid for the element whose check state changed.

#ifndef __vtkSMSILModel_h 
#define __vtkSMSILModel_h

#include "vtkSMObject.h"
#include <vtkstd/set> // required for vtkset

class vtkGraph;
class vtkSMStringVectorProperty;
class vtkSMProxy;

class VTK_EXPORT vtkSMSILModel : public vtkSMObject
{
public:
  static vtkSMSILModel* New();
  vtkTypeMacro(vtkSMSILModel, vtkSMObject);
  void PrintSelf(ostream& os, vtkIndent indent);

  //BTX
  enum CheckState
    {
    UNCHECKED = 0,
    PARTIAL = 1,
    CHECKED = 2
    };
  //ETX

  // Description:
  // Initialize the model using a SIL.
  // There are two ways of initializing the model:
  // \li One way is to initialize it with a SIL (using Initialize(vtkGraph*).
  // Then the model can be used as a simple API to check/uncheck elements.
  // \li Second way is to initialize with a proxy and property (using
  // Initialize(vtkSMProxy, vtkSMProperty*). In that case, the SIL is obtained
  // from  the property's vtkSMSILDomain. Also as the user changes the check
  // states, the property is updated/pushed.
  void Initialize(vtkGraph* sil);
  vtkGetObjectMacro(SIL, vtkGraph);

  // Description:
  // Initialize the model using a proxy and its property.
  // If a property is set, then the model keeps the property updated when the
  // check states are changed or when the property changes, the model's internal
  // check states are updated. If the property has a SILDomain, then the model
  // attaches itself to the domain so that whenever the domains is updated (i.e.
  // a new SIL is obtained from the server) the model updates the sil as well.
  // 
  // There are two ways of initializing the model:
  // \li One way is to initialize it with a SIL (using Initialize(vtkGraph*).
  // Then the model can be used as a simple API to check/uncheck elements.
  // \li Second way is to initialize with a proxy and property (using
  // Initialize(vtkSMProxy, vtkSMProperty*). In that case, the SIL is obtained
  // from  the property's vtkSMSILDomain. Also as the user changes the check
  // states, the property is updated/pushed.
  void Initialize(vtkSMProxy*, vtkSMStringVectorProperty*);
  vtkGetObjectMacro(Proxy, vtkSMProxy);
  vtkGetObjectMacro(Property, vtkSMStringVectorProperty);

  // Description:
  // Returns the number of children for the given vertex.
  // A node is a child node if it has no out-going edges or all out-going edges
  // have "CrossEdges" set to 1. If vertex id is invalid, returns -1.
  int GetNumberOfChildren(vtkIdType vertexid);

  // Description:
  // Returns the vertex id for the n-th child where n=child_index. Returns 0 if
  // request is invalid.
  vtkIdType GetChildVertex(vtkIdType parentid, int child_index);

  // Description: 
  // Returns the parent vertex i.e. the vertex at the end of an in-edge which is
  // not a cross-edge. It's an error to call this method for the root vertex id
  // i.e. 0.
  vtkIdType GetParentVertex(vtkIdType parent);

  // Description:
  // Get the name for the vertex.
  const char* GetName(vtkIdType vertex);

  // Description:
  // Get the check state for a vertex.
  int GetCheckStatus(vtkIdType vertex);

  // Description:
  // Set the check state for a vertex.
  // Returns true if the status was changed, false if unaffected.
  bool SetCheckState(vtkIdType vertex, int status);
  bool SetCheckState(const char* name, int status)
    {
    vtkIdType vertex = this->FindVertex(name);
    if (vertex != -1)
      {
      return this->SetCheckState(vertex, status);
      }
    vtkErrorMacro("Failed to locate " << name);
    return false;
    }

  // Description:
  // Convenience methods to check/uncheck all items.
  void CheckAll();
  void UncheckAll();

  // Description:
  // Updates the property using the check states maintained by the model.
  void UpdatePropertyValue(vtkSMStringVectorProperty*);

  // Description:
  // Updates the check states maintained internally by the model using the
  // status from the property. 
  void UpdateStateFromProperty(vtkSMStringVectorProperty*);

  // Description:
  // Locate a vertex with the given name. Returns -1 if the vertex is not found.
  vtkIdType FindVertex(const char* name);

//BTX 
  void GetLeaves(vtkstd::set<vtkIdType>& leaves,
    vtkIdType root, bool traverse_cross_edges);
protected:
  vtkSMSILModel();
  virtual ~vtkSMSILModel();

  void UpdateProperty();
  void OnPropertyModified();
  void OnDomainModified();

  /// Called to check/uncheck an item.
  void Check(vtkIdType vertexid, bool checked, vtkIdType inedgeid = -1);

  /// Determine vertexid's check state using its immediate children.
  /// If the check-state for the vertex has changed, then it propagates the call
  /// to the parent node.
  void UpdateCheck(vtkIdType vertexid);

  bool BlockUpdate;

  void SetSIL(vtkGraph*);

  vtkSMProxy* Proxy;
  vtkSMStringVectorProperty* Property;
  vtkGraph* SIL;
  vtkCommand* PropertyObserver;
  vtkCommand* DomainObserver;
private:
  vtkSMSILModel(const vtkSMSILModel&); // Not implemented.
  void operator=(const vtkSMSILModel&); // Not implemented.

  class vtkInternals;
  vtkInternals* Internals;
//ETX
};

#endif


