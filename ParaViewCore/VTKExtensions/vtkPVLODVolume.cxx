/*=========================================================================

  Program:   ParaView
  Module:    vtkPVLODVolume.cxx

  Copyright (c) Kitware, Inc.
  All rights reserved.
  See Copyright.txt or http://www.paraview.org/HTML/Copyright.html for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "vtkPVLODVolume.h"

#include "vtkAbstractVolumeMapper.h"
#include "vtkLODProp3D.h"
#include "vtkMapper.h"
#include "vtkProperty.h"
#include "vtkVolumeProperty.h"
#include "vtkColorTransferFunction.h"
#include "vtkMath.h"
#include "vtkMatrix4x4.h"
#include "vtkObjectFactory.h"
#include "vtkProperty.h"
#include "vtkRenderWindow.h"
#include "vtkRenderer.h"
#include "vtkTexture.h"
#include "vtkTimerLog.h"
#include "vtkTransform.h"

#include <math.h>

//-----------------------------------------------------------------------------
vtkStandardNewMacro(vtkPVLODVolume);

//----------------------------------------------------------------------------
vtkPVLODVolume::vtkPVLODVolume()
{
  this->LODProp = vtkLODProp3D::New();
  this->LODProp->AutomaticLODSelectionOff();
  this->LODProp->AutomaticPickLODSelectionOff();

  this->HighLODId = -1;
  this->LowLODId = -1;

  this->MapperBounds[0] = this->MapperBounds[1] = this->MapperBounds[2] = 0;
  this->MapperBounds[3] = this->MapperBounds[4] = this->MapperBounds[5] = 0;

  this->EnableLOD = 0;
}

//----------------------------------------------------------------------------
vtkPVLODVolume::~vtkPVLODVolume()
{
  this->LODProp->Delete();
}

//----------------------------------------------------------------------------
int vtkPVLODVolume::SelectLOD()
{
  if (this->LowLODId < 0)
    {
    return this->HighLODId;
    }
  if (this->HighLODId < 0)
    {
    return this->LowLODId;
    }

  if (this->EnableLOD)
    {
    return this->LowLODId;
    }

  return this->HighLODId;
}



//-----------------------------------------------------------------------------
int vtkPVLODVolume::RenderOpaqueGeometry(vtkViewport *vp)
{
  int retval = this->LODProp->RenderOpaqueGeometry(vp);

  this->EstimatedRenderTime = this->LODProp->GetEstimatedRenderTime();

  return retval;
}

//-----------------------------------------------------------------------------
int vtkPVLODVolume::RenderTranslucentPolygonalGeometry(vtkViewport *vp)
{
  int retval = this->LODProp->RenderTranslucentPolygonalGeometry(vp);

  this->EstimatedRenderTime = this->LODProp->GetEstimatedRenderTime();

  return retval;
}


//-----------------------------------------------------------------------------
int vtkPVLODVolume::RenderVolumetricGeometry(vtkViewport *vp)
{
  int retval = this->LODProp->RenderVolumetricGeometry(vp);

  this->EstimatedRenderTime = this->LODProp->GetEstimatedRenderTime();

  return retval;
}

//-----------------------------------------------------------------------------
void vtkPVLODVolume::ReleaseGraphicsResources(vtkWindow *renWin)
{
  this->Superclass::ReleaseGraphicsResources(renWin);
  
  // broadcast the message down to the individual LOD mappers
  this->LODProp->ReleaseGraphicsResources(renWin);
}


//-----------------------------------------------------------------------------
// Get the bounds for this Actor as (Xmin,Xmax,Ymin,Ymax,Zmin,Zmax).
double *vtkPVLODVolume::GetBounds()
{
  int i,n;
  double *bounds, bbox[24], *fptr;

  int lod = this->SelectLOD();
  if (lod < 0)
    {
    return this->Bounds;
    }

  vtkAbstractMapper3D *mapper = this->LODProp->GetLODMapper(lod);

  vtkDebugMacro( << "Getting Bounds" );

  // get the bounds of the Mapper if we have one
  if (!mapper)
    {
    return this->Bounds;
    }

  bounds = mapper->GetBounds();
  // Check for the special case when the mapper's bounds are unknown
  if (!bounds)
    {
    return bounds;
    }

  // Check for the special case when the actor is empty.
  if (bounds[0] > bounds[1])
    { 
    memcpy( this->MapperBounds, bounds, 6*sizeof(double) );
    vtkMath::UninitializeBounds(this->Bounds);
    this->BoundsMTime.Modified();
    return this->Bounds;
    }

  // Check if we have cached values for these bounds - we cache the
  // values returned by this->Mapper->GetBounds() and we store the time
  // of caching. If the values returned this time are different, or
  // the modified time of this class is newer than the cached time,
  // then we need to rebuild.
  if ( ( memcmp( this->MapperBounds, bounds, 6*sizeof(double) ) != 0 ) ||
       ( this->GetMTime() > this->BoundsMTime ) )
    {
    vtkDebugMacro( << "Recomputing bounds..." );

    memcpy( this->MapperBounds, bounds, 6*sizeof(double) );

    // fill out vertices of a bounding box
    bbox[ 0] = bounds[1]; bbox[ 1] = bounds[3]; bbox[ 2] = bounds[5];
    bbox[ 3] = bounds[1]; bbox[ 4] = bounds[2]; bbox[ 5] = bounds[5];
    bbox[ 6] = bounds[0]; bbox[ 7] = bounds[2]; bbox[ 8] = bounds[5];
    bbox[ 9] = bounds[0]; bbox[10] = bounds[3]; bbox[11] = bounds[5];
    bbox[12] = bounds[1]; bbox[13] = bounds[3]; bbox[14] = bounds[4];
    bbox[15] = bounds[1]; bbox[16] = bounds[2]; bbox[17] = bounds[4];
    bbox[18] = bounds[0]; bbox[19] = bounds[2]; bbox[20] = bounds[4];
    bbox[21] = bounds[0]; bbox[22] = bounds[3]; bbox[23] = bounds[4];
  
    // save the old transform
    this->Transform->Push(); 
    this->Transform->SetMatrix(this->GetMatrix());

    // and transform into actors coordinates
    fptr = bbox;
    for (n = 0; n < 8; n++) 
      {
      this->Transform->TransformPoint(fptr,fptr);
      fptr += 3;
      }
  
    this->Transform->Pop();  
  
    // now calc the new bounds
    this->Bounds[0] = this->Bounds[2] = this->Bounds[4] = VTK_DOUBLE_MAX;
    this->Bounds[1] = this->Bounds[3] = this->Bounds[5] = -VTK_DOUBLE_MAX;
    for (i = 0; i < 8; i++)
      {
      for (n = 0; n < 3; n++)
        {
        if (bbox[i*3+n] < this->Bounds[n*2]) 
          {
          this->Bounds[n*2] = bbox[i*3+n];
          }
        if (bbox[i*3+n] > this->Bounds[n*2+1]) 
          {
          this->Bounds[n*2+1] = bbox[i*3+n];
          }
        }
      }
    this->BoundsMTime.Modified();
    }

  return this->Bounds;
}


//-----------------------------------------------------------------------------
int vtkPVLODVolume::HasTranslucentPolygonalGeometry()
{
  if (this->SelectLOD() == this->LowLODId)
    {
    return 1;
    }
  return 0;
}

//-----------------------------------------------------------------------------
void vtkPVLODVolume::ShallowCopy(vtkProp *prop)
{
  vtkPVLODVolume *a = vtkPVLODVolume::SafeDownCast(prop);
  if ( a != NULL )
    {
    this->LODProp->ShallowCopy(a->LODProp);
    }

  // Now do superclass
  this->vtkVolume::ShallowCopy(prop);
}


//-----------------------------------------------------------------------------
void vtkPVLODVolume::SetProperty(vtkVolumeProperty *property)
{
  this->Superclass::SetProperty(property);

  this->UpdateLODProperty();
}


//-----------------------------------------------------------------------------
void vtkPVLODVolume::SetMapper(vtkAbstractVolumeMapper *mapper)
{
  if (this->HighLODId >= 0)
    {
    if (mapper == this->LODProp->GetLODMapper(this->HighLODId)) return;
    this->LODProp->RemoveLOD(this->HighLODId);
    this->HighLODId = -1;
    }

  if (mapper)
    {
    this->HighLODId = this->LODProp->AddLOD(mapper, this->GetProperty(), 1.0);
    this->UpdateLODProperty();
    }
}

void vtkPVLODVolume::SetLODMapper(vtkAbstractVolumeMapper *mapper)
{
  if (this->LowLODId >= 0)
    {
    if (mapper == this->LODProp->GetLODMapper(this->LowLODId)) return;
    this->LODProp->RemoveLOD(this->LowLODId);
    this->LowLODId = -1;
    }

  if (mapper)
    {
    this->LowLODId = this->LODProp->AddLOD(mapper, this->GetProperty(), 0.0);
    this->UpdateLODProperty();
    }
}

void vtkPVLODVolume::SetLODMapper(vtkMapper *mapper)
{
  if (this->LowLODId >= 0)
    {
    if (mapper == this->LODProp->GetLODMapper(this->LowLODId)) return;
    this->LODProp->RemoveLOD(this->LowLODId);
    this->LowLODId = -1;
    }

  if (mapper)
    {
    vtkProperty *property = vtkProperty::New();
    property->SetOpacity(0.5);
    this->LowLODId = this->LODProp->AddLOD(mapper, property, 0.0);
    property->Delete();
    this->UpdateLODProperty();
    }
}


//-----------------------------------------------------------------------------
void vtkPVLODVolume::SetAllocatedRenderTime(double t, vtkViewport *v)
{
  this->Superclass::SetAllocatedRenderTime(t, v);

  // This is a good time to update the LODProp.
  this->LODProp->SetUserMatrix(this->GetMatrix());

  int lod = this->SelectLOD();
  if (lod < 0)
    {
    vtkErrorMacro(<< "You must give me a mapper!");
    }
  this->LODProp->SetSelectedLODID(lod);
  this->LODProp->SetSelectedPickLODID(lod);

  this->LODProp->SetAllocatedRenderTime(t, v);
}

//-----------------------------------------------------------------------------
void vtkPVLODVolume::UpdateLODProperty()
{
  if (!this->Property)
    {
    vtkErrorMacro("No property is defined cannot update.");
    return;
    }

  if (this->HighLODId >= 0)
    {
    this->LODProp->SetLODProperty(this->HighLODId, this->Property);
    }

  if (this->LowLODId >= 0)
    {
    vtkMapper *mapper;
    this->LODProp->GetLODMapper(this->LowLODId, &mapper);
    if (mapper)
      {
      // Why change the LODMapper's LUT? 
      // It has already been intialized correctly.
      // This is a surface mapper.  Map the colors of the transfer function
      // to the surface.
      // mapper->SetLookupTable(this->Property->GetRGBTransferFunction());
      }
    else
      {
      // This is a volume mapper.  Just share the property.
      this->LODProp->SetLODProperty(this->LowLODId, this->Property);
      }
    }
}


//----------------------------------------------------------------------------
void vtkPVLODVolume::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);

  os << indent << "EnableLOD: " << this->EnableLOD << endl;
  os << indent << "LODProp: " << endl;
  this->LODProp->PrintSelf(os, indent.GetNextIndent());
}
