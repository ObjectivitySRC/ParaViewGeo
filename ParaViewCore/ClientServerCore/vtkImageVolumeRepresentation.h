/*=========================================================================

  Program:   ParaView
  Module:    vtkImageVolumeRepresentation.h

  Copyright (c) Kitware, Inc.
  All rights reserved.
  See Copyright.txt or http://www.paraview.org/HTML/Copyright.html for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// .NAME vtkImageVolumeRepresentation - representation for showing image
// datasets as a volume.
// .SECTION Description
// vtkImageVolumeRepresentation is a representation for volume rendering
// vtkImageData. Unlike other data-representations used by ParaView, this
// representation does not support delivery to client (or render server) nodes.
// In those configurations, it merely delivers a outline for the image to the
// client and render-server and those nodes simply render the outline.

#ifndef __vtkImageVolumeRepresentation_h
#define __vtkImageVolumeRepresentation_h

#include "vtkPVDataRepresentation.h"

class vtkColorTransferFunction;
class vtkFixedPointVolumeRayCastMapper;
class vtkImageData;
class vtkOutlineSource;
class vtkPiecewiseFunction;
class vtkPolyDataMapper;
class vtkPVCacheKeeper;
class vtkPVLODVolume;
class vtkPVUpdateSuppressor;
class vtkUnstructuredDataDeliveryFilter;
class vtkVolumeMapper;
class vtkVolumeProperty;

class VTK_EXPORT vtkImageVolumeRepresentation : public vtkPVDataRepresentation
{
public:
  static vtkImageVolumeRepresentation* New();
  vtkTypeMacro(vtkImageVolumeRepresentation, vtkPVDataRepresentation);
  void PrintSelf(ostream& os, vtkIndent indent);

  // This is same a vtkDataObject::FieldAssociation types so you can use those
  // as well.
  enum AttributeTypes
    {
    POINT_DATA=0,
    CELL_DATA=1
    };

  // Description:
  // Methods to control scalar coloring. ColorAttributeType defines the
  // attribute type.
  vtkSetMacro(ColorAttributeType, int);
  vtkGetMacro(ColorAttributeType, int);

  // Description:
  // Pick the array to color with.
  vtkSetStringMacro(ColorArrayName);
  vtkGetStringMacro(ColorArrayName);

  // Description:
  // Register a volume mapper with the representation.
  void AddVolumeMapper(const char* name, vtkVolumeMapper*);

  // Description:
  // Set the active volume mapper to use.
  virtual void SetActiveVolumeMapper(const char*);
  vtkVolumeMapper* GetActiveVolumeMapper();

  // Description:
  // vtkAlgorithm::ProcessRequest() equivalent for rendering passes. This is
  // typically called by the vtkView to request meta-data from the
  // representations or ask them to perform certain tasks e.g.
  // PrepareForRendering.
  virtual int ProcessViewRequest(vtkInformationRequestKey* request_type,
    vtkInformation* inInfo, vtkInformation* outInfo);

  // Description:
  // This needs to be called on all instances of vtkGeometryRepresentation when
  // the input is modified. This is essential since the geometry filter does not
  // have any real-input on the client side which messes with the Update
  // requests.
  virtual void MarkModified();

  // Description:
  // Get/Set the visibility for this representation. When the visibility of
  // representation of false, all view passes are ignored.
  virtual void SetVisibility(bool val);

  //***************************************************************************
  // Forwarded to Actor.
  void SetOrientation(double, double, double);
  void SetOrigin(double, double, double);
  void SetPickable(int val);
  void SetPosition(double, double, double);
  void SetScale(double, double, double);

  //***************************************************************************
  // Forwarded to vtkVolumeProperty.
  void SetInterpolationType(int val);
  void SetColor(vtkColorTransferFunction* lut);
  void SetScalarOpacity(vtkPiecewiseFunction* pwf);
  void SetScalarOpacityUnitDistance(double val);
  void SetAmbient(double);
  void SetDiffuse(double);
  void SetSpecular(double);
  void SetSpecularPower(double);
  void SetShade(bool);

//BTX
protected:
  vtkImageVolumeRepresentation();
  ~vtkImageVolumeRepresentation();

  // Description:
  // Fill input port information.
  virtual int FillInputPortInformation(int port, vtkInformation* info);

  // Description:
  virtual int RequestData(vtkInformation*,
    vtkInformationVector**, vtkInformationVector*);

  // Description:
  // Produce meta-data about this representation that the view may find useful.
  bool GenerateMetaData(vtkInformation*, vtkInformation*);

  // Description:
  // Adds the representation to the view.  This is called from
  // vtkView::AddRepresentation().  Subclasses should override this method.
  // Returns true if the addition succeeds.
  virtual bool AddToView(vtkView* view);

  // Description:
  // Removes the representation to the view.  This is called from
  // vtkView::RemoveRepresentation().  Subclasses should override this method.
  // Returns true if the removal succeeds.
  virtual bool RemoveFromView(vtkView* view);

  // Description:
  // Overridden to check with the vtkPVCacheKeeper to see if the key is cached.
  virtual bool IsCached(double cache_key);

  // Description:
  // Passes on parameters to the active volume mapper
  virtual void UpdateMapperParameters();

  vtkImageData* Cache;
  vtkPVCacheKeeper* CacheKeeper;
  vtkFixedPointVolumeRayCastMapper* DefaultMapper;
  vtkVolumeProperty* Property;
  vtkPVLODVolume* Actor;

  vtkOutlineSource* OutlineSource;
  vtkUnstructuredDataDeliveryFilter* OutlineDeliveryFilter;
  vtkPVUpdateSuppressor* OutlineUpdateSuppressor;
  vtkPolyDataMapper* OutlineMapper;;

  int ColorAttributeType;
  char* ColorArrayName;

private:
  vtkImageVolumeRepresentation(const vtkImageVolumeRepresentation&); // Not implemented
  void operator=(const vtkImageVolumeRepresentation&); // Not implemented

  class vtkInternals;
  vtkInternals* Internals;
//ETX
};

#endif
