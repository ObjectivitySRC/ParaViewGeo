/*=========================================================================

  Program:   ParaView
  Module:    vtkSMMessage.h

  Copyright (c) Kitware, Inc.
  All rights reserved.
  See Copyright.txt or http://www.paraview.org/HTML/Copyright.html for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// .NAME vtkSMMessage
// .SECTION Description
// Header class that setup every thing in order to use Protobuf messages in
// a transparent manner

#ifndef __vtkSMMessage_h
#define __vtkSMMessage_h

#include "vtkSMMessageMinimal.h"

#include <string>
#if __GNUC__
# pragma GCC diagnostic ignored "-Wsign-compare"
#endif
#include "vtkSystemIncludes.h"
#include "vtkPVMessage.pb.h"
#if __GNUC__
# pragma GCC diagnostic warning "-Wsign-compare"
#endif

#include "vtkClientServerStream.h"

inline vtkClientServerStream& operator << (vtkClientServerStream& stream,
  const paraview_protobuf::Variant& variant)
{
  switch (variant.type())
    {
  case paraview_protobuf::Variant::INT:
    for (int cc=0; cc < variant.integer_size(); cc++)
      {
      stream << variant.integer(cc);
      }
    break;

  case paraview_protobuf::Variant::FLOAT64:
    for (int cc=0; cc < variant.float64_size(); cc++)
      {
      stream << variant.float64(cc);
      }
    break;

  case paraview_protobuf::Variant::IDTYPE:
    for (int cc=0; cc < variant.idtype_size(); cc++)
      {
      stream << variant.idtype(cc);
      }
    break;

  case paraview_protobuf::Variant::STRING:
    for (int cc=0; cc < variant.txt_size(); cc++)
      {
      stream << variant.txt(cc).c_str();
      }
    break;

  default:
    break;
    }
  return stream;
}

using namespace paraview_protobuf;
#endif
