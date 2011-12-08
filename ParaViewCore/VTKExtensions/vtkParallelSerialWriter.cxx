/*=========================================================================

  Program:   ParaView
  Module:    vtkParallelSerialWriter.cxx

  Copyright (c) Kitware, Inc.
  All rights reserved.
  See Copyright.txt or http://www.paraview.org/HTML/Copyright.html for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "vtkParallelSerialWriter.h"

#include "vtkClientServerInterpreter.h"
#include "vtkClientServerInterpreterInitializer.h"
#include "vtkClientServerStream.h"
#include "vtkCompositeDataIterator.h"
#include "vtkCompositeDataSet.h"
#include "vtkDataSet.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkMultiProcessController.h"
#include "vtkObjectFactory.h"
#include "vtkReductionFilter.h"
#include "vtkSmartPointer.h"
#include "vtkStreamingDemandDrivenPipeline.h"

#include <vtksys/ios/sstream>
#include <vtksys/SystemTools.hxx>

#include <vtkstd/string>

vtkStandardNewMacro(vtkParallelSerialWriter);
vtkCxxSetObjectMacro(vtkParallelSerialWriter, Writer, vtkAlgorithm);
vtkCxxSetObjectMacro(vtkParallelSerialWriter, PreGatherHelper, vtkAlgorithm);
vtkCxxSetObjectMacro(vtkParallelSerialWriter, PostGatherHelper, vtkAlgorithm);
//-----------------------------------------------------------------------------
vtkParallelSerialWriter::vtkParallelSerialWriter()
{
  this->SetNumberOfOutputPorts(0);

  this->Writer = 0;

  this->FileNameMethod = 0;
  this->FileName = 0;

  this->Piece = 0;
  this->NumberOfPieces = 1;
  this->GhostLevel = 0;

  this->PreGatherHelper = 0;
  this->PostGatherHelper = 0;

  this->WriteAllTimeSteps = 0;
  this->NumberOfTimeSteps = 0;
  this->CurrentTimeIndex = 0;

  this->Interpreter = 0;
  this->SetInterpreter(vtkClientServerInterpreterInitializer::GetInterpreter());
}

//-----------------------------------------------------------------------------
vtkParallelSerialWriter::~vtkParallelSerialWriter()
{
  this->SetWriter(0);
  this->SetFileNameMethod(0);
  this->SetFileName(0);
  this->SetPreGatherHelper(0);
  this->SetPostGatherHelper(0);
  this->SetInterpreter(0);
}

//----------------------------------------------------------------------------
int vtkParallelSerialWriter::Write()
{
  // Make sure we have input.
  if (this->GetNumberOfInputConnections(0) < 1)
    {
    vtkErrorMacro("No input provided!");
    return 0;
    }

  // always write even if the data hasn't changed
  this->Modified();

  this->Update();
  return 1;
}

//----------------------------------------------------------------------------
int vtkParallelSerialWriter::RequestInformation(
  vtkInformation* vtkNotUsed(request),
  vtkInformationVector** inputVector,
  vtkInformationVector* vtkNotUsed(outputVector))
{
  vtkInformation *inInfo = inputVector[0]->GetInformationObject(0);
  if ( inInfo->Has(vtkStreamingDemandDrivenPipeline::TIME_STEPS()) )
    {
    this->NumberOfTimeSteps = 
      inInfo->Length( vtkStreamingDemandDrivenPipeline::TIME_STEPS() );
    }
  else
    {
    this->NumberOfTimeSteps = 0;
    }
  return 1;
}

//----------------------------------------------------------------------------
int vtkParallelSerialWriter::RequestUpdateExtent(
  vtkInformation* vtkNotUsed(request),
  vtkInformationVector** inputVector,
  vtkInformationVector* vtkNotUsed(outputVector))
{
  vtkInformation* inInfo = inputVector[0]->GetInformationObject(0);

  inInfo->Set(
    vtkStreamingDemandDrivenPipeline::UPDATE_NUMBER_OF_PIECES(), 
    this->NumberOfPieces);
  inInfo->Set(
    vtkStreamingDemandDrivenPipeline::UPDATE_PIECE_NUMBER(), this->Piece);
  inInfo->Set(
    vtkStreamingDemandDrivenPipeline::UPDATE_NUMBER_OF_GHOST_LEVELS(), 
    this->GhostLevel);

  double *inTimes = inputVector[0]->GetInformationObject(0)->Get(
      vtkStreamingDemandDrivenPipeline::TIME_STEPS());
  if (inTimes && this->WriteAllTimeSteps)
    {
    double timeReq = inTimes[this->CurrentTimeIndex];
    inputVector[0]->GetInformationObject(0)->Set( 
        vtkStreamingDemandDrivenPipeline::UPDATE_TIME_STEPS(), 
        &timeReq, 1);
    }
  return 1;  
}

//----------------------------------------------------------------------------
int vtkParallelSerialWriter::RequestData(
  vtkInformation* request,
  vtkInformationVector** inputVector,
  vtkInformationVector* vtkNotUsed(outputVector))
{
  if (!this->Writer)
    {
    vtkErrorMacro("No internal writer specified. Cannot write.");
    return 0;
    }

  bool write_all = (this->WriteAllTimeSteps != 0 && this->NumberOfTimeSteps > 0);

  if (write_all)
    {
    if (this->CurrentTimeIndex == 0)
      {
      // Tell the pipeline to start looping.
      request->Set(vtkStreamingDemandDrivenPipeline::CONTINUE_EXECUTING(), 1);
      }
    }
  else
    {
    request->Remove(vtkStreamingDemandDrivenPipeline::CONTINUE_EXECUTING());
    this->CurrentTimeIndex = 0;
    }
  
  vtkInformation* inInfo = inputVector[0]->GetInformationObject(0);
  vtkDataObject* input = inInfo->Get(vtkDataObject::DATA_OBJECT());
  this->WriteATimestep(input);

  if (write_all)
    {
    this->CurrentTimeIndex++;
    if (this->CurrentTimeIndex >= this->NumberOfTimeSteps)
      {
      // Tell the pipeline to stop looping.
      request->Remove(vtkStreamingDemandDrivenPipeline::CONTINUE_EXECUTING());
      this->CurrentTimeIndex = 0;
      }
    }
  
  return 1;
}

//----------------------------------------------------------------------------
void vtkParallelSerialWriter::WriteATimestep(vtkDataObject* input)
{
  vtkCompositeDataSet* cds = vtkCompositeDataSet::SafeDownCast(input); 
  if (cds)
    {
    vtkSmartPointer<vtkCompositeDataIterator> iter;
    iter.TakeReference(cds->NewIterator());
    iter->SetSkipEmptyNodes(0);
    int idx;
    for(idx=0, iter->InitTraversal(); 
        !iter->IsDoneWithTraversal(); 
        iter->GoToNextItem(), idx++)
      {
      vtkDataObject* curObj = iter->GetCurrentDataObject();
      vtkstd::string path = 
        vtksys::SystemTools::GetFilenamePath(this->FileName);
      vtkstd::string fnamenoext =
        vtksys::SystemTools::GetFilenameWithoutLastExtension(this->FileName);
      vtkstd::string ext =
        vtksys::SystemTools::GetFilenameLastExtension(this->FileName);
      vtksys_ios::ostringstream fname;
      fname << path << "/" << fnamenoext << idx << ext;
      this->WriteAFile(fname.str().c_str(), curObj);
      }
    }
  else if (input)
    {
    vtkSmartPointer<vtkDataObject> inputCopy;
    inputCopy.TakeReference(input->NewInstance());
    inputCopy->ShallowCopy(input);
    this->WriteAFile(this->FileName, inputCopy);
    }
  
}

//----------------------------------------------------------------------------
void vtkParallelSerialWriter::WriteAFile(const char* filename, vtkDataObject* input)
{
  vtkMultiProcessController* controller =
    vtkMultiProcessController::GetGlobalController();
  
  vtkSmartPointer<vtkReductionFilter> md = vtkSmartPointer<vtkReductionFilter>::New();
  md->SetController(controller);
  md->SetPreGatherHelper(this->PreGatherHelper);
  md->SetPostGatherHelper(this->PostGatherHelper);
  if (input)
    {
    md->SetInputConnection(0, input->GetProducerPort());
    }
  md->UpdateInformation();
  vtkInformation* outInfo = md->GetExecutive()->GetOutputInformation(0);
  outInfo->Set(
    vtkStreamingDemandDrivenPipeline::UPDATE_PIECE_NUMBER(),
    this->Piece);
  outInfo->Set(
    vtkStreamingDemandDrivenPipeline::UPDATE_NUMBER_OF_PIECES(),
    this->NumberOfPieces);
  outInfo->Set(
    vtkStreamingDemandDrivenPipeline::UPDATE_NUMBER_OF_GHOST_LEVELS(),
    this->GhostLevel);
  md->Update();

  if (controller->GetLocalProcessId() == 0)
    {
    vtkDataObject* output = md->GetOutputDataObject(0);
    if (vtkDataSet::SafeDownCast(output) == 0 || 
      vtkDataSet::SafeDownCast(output)->GetNumberOfCells() != 0)
      {
      vtkSmartPointer<vtkDataObject> outputCopy;
      outputCopy.TakeReference(output->NewInstance());
      outputCopy->ShallowCopy(output);

      vtksys_ios::ostringstream fname;
      if (this->WriteAllTimeSteps)
        {
        vtkstd::string path = 
          vtksys::SystemTools::GetFilenamePath(filename);
        vtkstd::string fnamenoext =
          vtksys::SystemTools::GetFilenameWithoutLastExtension(filename);
        vtkstd::string ext =
          vtksys::SystemTools::GetFilenameLastExtension(filename);
        fname << path << "/" << fnamenoext << "." << this->CurrentTimeIndex << ext;
        }
      else
        {
        fname << filename;
        }
      this->Writer->SetInputConnection(outputCopy->GetProducerPort());
      this->SetWriterFileName(fname.str().c_str());
      this->WriteInternal();
      this->Writer->SetInputConnection(0);
      }
    }
}

//----------------------------------------------------------------------------
// Overload standard modified time function. If the internal reader is 
// modified, then this object is modified as well.
unsigned long vtkParallelSerialWriter::GetMTime()
{
  unsigned long mTime=this->vtkObject::GetMTime();
  unsigned long readerMTime;

  if ( this->Writer )
    {
    readerMTime = this->Writer->GetMTime();
    mTime = ( readerMTime > mTime ? readerMTime : mTime );
    }

  return mTime;
}

//-----------------------------------------------------------------------------
void vtkParallelSerialWriter::WriteInternal()
{
  if (this->Writer && this->FileNameMethod)
    {
    // Get the local process interpreter.
    vtkClientServerStream stream;
    stream << vtkClientServerStream::Invoke
           << this->Writer << "Write"
           << vtkClientServerStream::End;
    this->Interpreter->ProcessStream(stream);
    }
}

//-----------------------------------------------------------------------------
void vtkParallelSerialWriter::SetWriterFileName(const char* fname)
{
  if (this->Writer && this->FileName && this->FileNameMethod)
    {
    // Get the local process interpreter.
    vtkClientServerStream stream;
    stream << vtkClientServerStream::Invoke
           << this->Writer << this->FileNameMethod << fname
           << vtkClientServerStream::End;
    this->Interpreter->ProcessStream(stream);
    }
}

//-----------------------------------------------------------------------------
void vtkParallelSerialWriter::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}
