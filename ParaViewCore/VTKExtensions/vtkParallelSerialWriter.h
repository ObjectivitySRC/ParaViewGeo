/*=========================================================================

  Program:   ParaView
  Module:    vtkParallelSerialWriter.h

  Copyright (c) Kitware, Inc.
  All rights reserved.
  See Copyright.txt or http://www.paraview.org/HTML/Copyright.html for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// .NAME vtkParallelSerialWriter - parallel meta-writer for serial formats
// .SECTION Description:
// vtkParallelSerialWriter is a meta-writer that enables serial writers
// to work in parallel. It gathers data to the 1st node and invokes the
// internal writer. The reduction is controlled defined by the PreGatherHelper
// and PostGatherHelper.
// This also makes it possible to write time-series for temporal datasets using
// simple non-time-aware writers.

#ifndef __vtkParallelSerialWriter_h
#define __vtkParallelSerialWriter_h

#include "vtkDataObjectAlgorithm.h"

class vtkClientServerInterpreter;

class VTK_EXPORT vtkParallelSerialWriter : public vtkDataObjectAlgorithm
{
public:
  static vtkParallelSerialWriter* New();
  vtkTypeMacro(vtkParallelSerialWriter, vtkDataObjectAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // Set/get the internal writer.
  void SetWriter(vtkAlgorithm*);
  vtkGetObjectMacro(Writer, vtkAlgorithm);

  // Description:
  // Return the MTime also considering the internal writer.
  virtual unsigned long GetMTime();

  // Description:
  // Name of the method used to set the file name of the internal
  // writer. By default, this is SetFileName.
  vtkSetStringMacro(FileNameMethod);
  vtkGetStringMacro(FileNameMethod);

  // Description:
  // Get/Set the name of the output file.
  vtkSetStringMacro(FileName);
  vtkGetStringMacro(FileName);

  // Description:
  // Invoke the writer.  Returns 1 for success, 0 for failure.
  int Write();

  // Description:
  // Get/Set the piece number to write.  The same piece number is used
  // for all inputs.
  vtkGetMacro(Piece, int);
  vtkSetMacro(Piece, int);
  
  // Description:
  // Get/Set the number of pieces into which the inputs are split.
  vtkGetMacro(NumberOfPieces, int);
  vtkSetMacro(NumberOfPieces, int);
  
  // Description:
  // Get/Set the number of ghost levels to be written.
  vtkGetMacro(GhostLevel, int);
  vtkSetMacro(GhostLevel, int);

  // Description:
  // Get/Set the pre-reduction helper. Pre-Reduction helper is an algorithm 
  // that runs on each node's data before it is sent to the root.
  void SetPreGatherHelper(vtkAlgorithm*);
  vtkGetObjectMacro(PreGatherHelper, vtkAlgorithm);

  // Description:
  // Get/Set the reduction helper. Reduction helper is an algorithm with
  // multiple input connections, that produces a single output as
  // the reduced output. This is run on the root node to produce a result
  // from the gathered results of each node.
  void SetPostGatherHelper(vtkAlgorithm*);
  vtkGetObjectMacro(PostGatherHelper, vtkAlgorithm);

  // Description:
  // Must be set to true to write all timesteps, otherwise only the current
  // timestep will be written out. Off by default.
  vtkGetMacro(WriteAllTimeSteps, int);
  vtkSetMacro(WriteAllTimeSteps, int);
  vtkBooleanMacro(WriteAllTimeSteps, int);

//BTX
  // Description:
  // Get/Set the interpreter to use to call methods on the writer.
  void SetInterpreter(vtkClientServerInterpreter* interp)
    { this->Interpreter = interp; }

protected:
  vtkParallelSerialWriter();
  ~vtkParallelSerialWriter();

  int RequestInformation(vtkInformation* request,
                         vtkInformationVector** inputVector,
                         vtkInformationVector* outputVector);
  int RequestUpdateExtent(vtkInformation* request,
                          vtkInformationVector** inputVector,
                          vtkInformationVector* outputVector);
  int RequestData(vtkInformation* request,
                  vtkInformationVector** inputVector,
                  vtkInformationVector* outputVector);

private:
  vtkParallelSerialWriter(const vtkParallelSerialWriter&); // Not implemented.
  void operator=(const vtkParallelSerialWriter&); // Not implemented.
  
  void WriteATimestep(vtkDataObject* input);
  void WriteAFile(const char* fname, vtkDataObject* input);

  void SetWriterFileName(const char* fname);
  void WriteInternal();

  vtkAlgorithm* PreGatherHelper;
  vtkAlgorithm* PostGatherHelper;

  vtkAlgorithm* Writer;
  char* FileNameMethod;
  int Piece;
  int NumberOfPieces;
  int GhostLevel;

  int WriteAllTimeSteps;
  int NumberOfTimeSteps;
  int CurrentTimeIndex;

  // The name of the output file.
  char* FileName;

  vtkClientServerInterpreter* Interpreter;
//ETX
};

#endif

