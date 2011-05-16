/*=========================================================================

   Program:   Visualization Toolkit
  Module:    $ vtkMapAttributesFromGrid.h $
  Author:    Robert Maynard
  MIRARCO, Laurentian University
  Date:      March  18 2008
  Version:   0.4




=========================================================================*/


#ifndef __vtkMapAttributesFromGrid_h
#define __vtkMapAttributesFromGrid_h

#include "vtkPolyDataAlgorithm.h"

#define VTK_ATTRIBUTE_MODE_DEFAULT         0
#define VTK_ATTRIBUTE_MODE_USE_POINT_DATA  1
#define VTK_ATTRIBUTE_MODE_USE_CELL_DATA   2

// order / values are important because of the SetClampMacro
#define VTK_COMPONENT_MODE_USE_SELECTED    0
#define VTK_COMPONENT_MODE_USE_ALL         1
#define VTK_COMPONENT_MODE_USE_ANY         2

class VTK_EXPORT vtkMapAttributesFromGrid : public vtkPolyDataAlgorithm
{
public:
  static vtkMapAttributesFromGrid *New();
  vtkTypeRevisionMacro(vtkMapAttributesFromGrid,vtkPolyDataAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // Control how the filter works with scalar point data and cell attribute
  // data.  By default (AttributeModeToDefault), the filter will use point
  // data, and if no point data is available, then cell data is
  // used. Alternatively you can explicitly set the filter to use point data
  // (AttributeModeToUsePointData) or cell data (AttributeModeToUseCellData).
  vtkSetMacro(AttributeMode,int);
  vtkGetMacro(AttributeMode,int);
  void SetAttributeModeToDefault() 
    {this->SetAttributeMode(VTK_ATTRIBUTE_MODE_DEFAULT);};
  void SetAttributeModeToUsePointData() 
    {this->SetAttributeMode(VTK_ATTRIBUTE_MODE_USE_POINT_DATA);};
  void SetAttributeModeToUseCellData() 
    {this->SetAttributeMode(VTK_ATTRIBUTE_MODE_USE_CELL_DATA);};
  const char *GetAttributeModeAsString();

  // Description:
  // Control how the decision of in / out is made with multi-component data.
  // The choices are to use the selected component (specified in the
  // SelectedComponent ivar), or to look at all components. When looking at
  // all components, the evaluation can pass if all the components satisfy
  // the rule (UseAll) or if any satisfy is (UseAny). The default value is
  // UseSelected.
  vtkSetClampMacro(ComponentMode,int,
                   VTK_COMPONENT_MODE_USE_SELECTED,
                   VTK_COMPONENT_MODE_USE_ANY);
  vtkGetMacro(ComponentMode,int);
  void SetComponentModeToUseSelected() 
    {this->SetComponentMode(VTK_COMPONENT_MODE_USE_SELECTED);};
  void SetComponentModeToUseAll() 
    {this->SetComponentMode(VTK_COMPONENT_MODE_USE_ALL);};
  void SetComponentModeToUseAny() 
    {this->SetComponentMode(VTK_COMPONENT_MODE_USE_ANY);};
  const char *GetComponentModeAsString();
  
  // Description:
  // When the component mode is UseSelected, this ivar indicated the selected
  // component. The default value is 0.
  vtkSetClampMacro(SelectedComponent,int,0,VTK_INT_MAX);
  vtkGetMacro(SelectedComponent,int);
  
  // Description:
  // Get a pointer to the source object.
  vtkDataObject *GetSource();

  // Description:
  // Specify the point locations
  void SetSourceConnection(vtkAlgorithmOutput* algOutput);
  
protected:
  int    AttributeMode;
  int    ComponentMode;
  int    SelectedComponent;
  
  vtkMapAttributesFromGrid();  
  ~vtkMapAttributesFromGrid(); 
  
  virtual int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *);
  
  virtual int FillInputPortInformation(int port, vtkInformation* info);
  virtual int FillOutputPortInformation(int port, vtkInformation* info);

private: 
  vtkMapAttributesFromGrid(const vtkMapAttributesFromGrid&);  // Not implemented.
  void operator=(const vtkMapAttributesFromGrid&);  // Not implemented.
};


#endif
