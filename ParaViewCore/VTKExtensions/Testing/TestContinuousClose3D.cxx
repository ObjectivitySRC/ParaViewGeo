/*=========================================================================

  Program:   ParaView
  Module:    TestContinuousClose3D.cxx

  Copyright (c) Kitware, Inc.
  All rights reserved.
  See Copyright.txt or http://www.paraview.org/HTML/Copyright.html for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "vtkImageData.h"
#include "vtkImageReader.h"
#include "vtkImageContinuousDilate3D.h"
#include "vtkImageContinuousErode3D.h"
#include "vtkImageMedian3D.h"
#include "vtkImageGradient.h"
#include "vtkImageViewer.h"
#include "vtkImageGradientMagnitude.h"
#include "vtkTestUtilities.h"
#include "vtkTesting.h"

int main(int argc, char* argv[])
{
  char* fname = 
    vtkTestUtilities::ExpandDataFileName(argc, argv, "Data/headsq/quarter");

  // Image pipeline
  vtkImageReader *reader = vtkImageReader::New();
  reader->SetDataByteOrderToLittleEndian();
  reader->SetDataExtent (0, 63, 0, 63, 1, 93);
  reader->SetFilePrefix ( fname );
  reader->SetDataMask (0x7fff);

  delete [] fname;

  vtkImageContinuousDilate3D *dilate = vtkImageContinuousDilate3D::New();
  dilate->SetInput(reader->GetOutput());
  dilate->SetKernelSize( 11, 11, 1);

  vtkImageContinuousErode3D *erode = vtkImageContinuousErode3D::New();
  erode->SetInput (dilate->GetOutput());
  erode->SetKernelSize(11,11,1);
  
  vtkImageMedian3D *median = vtkImageMedian3D::New();
  median->SetInput( erode->GetOutput() );
  
  vtkImageGradient *gradient = vtkImageGradient::New();
  gradient->SetInput( median->GetOutput() );
  gradient->SetDimensionality (3);
  gradient->Update(); //discard gradient

  vtkImageGradientMagnitude *magnitude = vtkImageGradientMagnitude::New();
  magnitude->SetInput( erode->GetOutput() );
  magnitude->SetDimensionality (3);

  vtkImageViewer *viewer = vtkImageViewer::New();
  viewer->SetInput ( magnitude->GetOutput() );
  viewer->SetColorWindow(2000);
  viewer->SetColorLevel(1000);
  viewer->Render();

  reader->Delete();
  dilate->Delete();
  erode->Delete();
  median->Delete();
  gradient->Delete();
  magnitude->Delete();
  viewer->Delete();

  return 0;
}
