/*=========================================================================

  Program:   Visualization Toolkit
  Module:    $RCSfile: vtkSQDistributedStreamTracer.h,v $

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// .NAME vtkSQDistributedStreamTracer - Distributed streamline generator
// .SECTION Description
// This filter integrates streamlines on a distributed dataset. It is
// essentially a serial algorithm: only one process is active at one
// time and it is not more efficient than a single process integration.
// It is useful when the data is too large to be on one process and
// has to be kept distributed.
// .SECTION See Also
// vtkStreamTracer vtkPStreamTracer

#ifndef __vtkSQDistributedStreamTracer_h
#define __vtkSQDistributedStreamTracer_h

#include "vtkSQPStreamTracer.h"

class VTK_EXPORT vtkSQDistributedStreamTracer : public vtkSQPStreamTracer
{
public:
  vtkTypeRevisionMacro(vtkSQDistributedStreamTracer,vtkSQPStreamTracer);
  void PrintSelf(ostream& os, vtkIndent indent);

  static vtkSQDistributedStreamTracer *New();

protected:
  //BTX
  enum {
    TASK_SEEK=0,
    TASK_INTEGRATE=1,
    TASK_CANCEL=2 };
  //ETX

  vtkSQDistributedStreamTracer();
  ~vtkSQDistributedStreamTracer();

  void ForwardTask(double seed[3],
                   int direction,
                   int taskType,
                   int originatingProcId,
                   int originatingStreamId,
                   int currentLine,
                   double* firstNormal,
                   double propagation,
                   vtkIdType numSteps);
  int ProcessTask(double seed[3],
                  int direction,
                  int taskType,
                  int originatingProcId,
                  int originatingStreamId,
                  int currentLine,
                  double* firstNormal,
                  double propagation,
                  vtkIdType numSteps);
  int ProcessNextLine(int currentLine);
  int ReceiveAndProcessTask();

  virtual void ParallelIntegrate();

private:
  vtkSQDistributedStreamTracer(const vtkSQDistributedStreamTracer&);  // Not implemented.
  void operator=(const vtkSQDistributedStreamTracer&);  // Not implemented.
};


#endif


