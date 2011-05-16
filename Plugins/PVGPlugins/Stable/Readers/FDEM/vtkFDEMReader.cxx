/*=========================================================================

  Program:   ParaViewGeo
  Module:    $RCSfile: vtkEllipsoid.cxx,v $

  Copyright (c) Robert Maynard
  All rights reserved.  
     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

  Replicates the work done by the M1 FEMDEM visual program
=========================================================================*/
#include "vtkFDEMReader.h"

#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkObjectFactory.h"
#include "vtkMultiBlockDataSet.h"
#include "vtkPolyData.h"
#include "vtkPoints.h"
#include "vtkCellArray.h"
#include "vtkCellData.h"
#include "vtkFloatArray.h"
#include <vtksys/RegularExpression.hxx>
#include <sstream>

//special classes for the FDEM storage
#include "TriangleProperties.h"
#include "FDEMFill.h"
#include "FDEMPoint.h"
#include "FDEMCrackJoint.h"


vtkCxxRevisionMacro(vtkFDEMReader, "$Revision: 0.314 $");
vtkStandardNewMacro(vtkFDEMReader);

#define TRIANGLE 1
#define JOINT 3

// --------------------------------------    
// Constructor
vtkFDEMReader::vtkFDEMReader()
  {
  this->SetNumberOfInputPorts(0);
  this->SetNumberOfOutputPorts(1);  
  
  this->FileName = NULL;
  this->RawFile = NULL;   
  
  this->Fill = 0;  
  this->Point = 0;
  this->Crack = 0;
     
  this->YFileFound = false;
  
  //we make these 1 so that if the .y file is not found, we do not wreck the property values
  this->NormCoord=1;
  this->NormForce=1;
  this->NormStress=1;
  this->NormVelocity=1;
  
  this->CharacterTable = "0123456789-=qwertyuiop[]^asdfghjkl;zxcvbnm,./~!@#$%&*()_+QWERTYUIOP{}|ASDFGHJKL:ZXCVBNM<>?";    
  
  //make sure all the arrays have nothing in them
  for (int i=0; i < VTK_FDEM_BLOCK_SIZE; i++)
    {    
    this->RawBlock[i]=NULL;  
    this->Block[i]=NULL;
    }      
    
  //define the max array
  this->Max[0] = 1;
  for(int i=1; i<10; i++)
    { 
    this->Max[i] = Max[i-1] * VTK_FDEM_CODE_BASE;
    }
  
  //define the lookuptables
  long temp = 0;
  
  //populate with 0's
  for (long i=0; i < 1000; i++)
    {
    //drop in a empty entry
    this->Lookup[i] = 0;
    }
          
  //connect to the characterTable
  for(long i=0; i< VTK_FDEM_CODE_BASE; i++)
    { 
    temp =(long)this->CharacterTable[i];
    if(temp<0)
      {
      temp+=500;
      }    
    this->Lookup[temp] = i;
    }
  }      

// --------------------------------------
// Destructor
vtkFDEMReader::~vtkFDEMReader()
  {
  this->SetFileName(0);
  }

// --------------------------------------
void vtkFDEMReader::PrintSelf(ostream& os, vtkIndent indent)
  {
  this->Superclass::PrintSelf(os,indent);
  os << indent << "FileName: "
     << (this->FileName ? this->FileName : "(none)") << "\n";
  }

// --------------------------------------
int vtkFDEMReader::RequestInformation(vtkInformation *vtkNotUsed(request), vtkInformationVector **vtkNotUsed(inputVector), vtkInformationVector *outputVector)
{
  //we need to open up the related .y file and scan it for a couple important properties
  if(!this->FileName)
    {
    vtkErrorMacro("A FileName must be specified.");
		return 1;
    }
  if(strlen(this->FileName)==0)
    {
    vtkErrorMacro("A NULL FileName.");
		return 1;
    }
  
  //we only want to call this code for the first file in the animation
  if ( ! this->YFileFound )
    {        
    vtkStdString fName = this->FileName;

    /*this is dangerous, since we are presuming the program that creates the files will
    /always use the naming convention of:
      name0.ym
      name1.ym
      name2.ym
      ....
      name1000.ym
      name.y    
    */    
    vtksys::RegularExpression regExp = "[0-9]+.[a-zA-Z]+$";
    int found = regExp.find(fName);
        
    if (found)
      {
      vtkStdString yFileName = fName.substr(0, regExp.start() );  
    
      yFileName+=".y"; //add the extension back on
      
      ifstream  yFile;
      yFile.open(yFileName, ifstream::in);
      
      /*go through the file looking for certain lines
      /the lines we want look something like this:
       /YD/YDC/NAME VALUE
       
      so we split it on /YD/YDC/ and the space    
      */      
      vtkStdString line; //raw line
      vtkStdString flag = "/YD/YDC/"; //the start of lines we want
      
      vtkStdString command;
      vtkStdString value; //the value of the command    
      
      int found;
      int doneReading=0;    
      while( yFile.good() && doneReading < 4 ) //5 since we are looking for 4 properties
        {
        this->YFileFound = true;
        getline(yFile,line);
              
        found = line.compare(0,flag.size(),flag);
        //0 means the compare is good!
        if (found == 0)      
          {
          vtkstd::stringstream stream;
          //split the line        
          int pos = line.find(' ');
          command = line.substr(0,pos);
          value = line.substr(pos);
          
          //now we need to strip the value off, and figure out what class variable we are looking at        
          if(command == "/YD/YDC/DCSIZC")
            {
            stream << value;
            stream >> this->NormCoord;
            doneReading++;
            }
          else if(command == "/YD/YDC/DCSIZF")
            {
            stream << value;
            stream >> this->NormForce;
            doneReading++;
            }
          else if(command == "/YD/YDC/DCSIZV")
            {
            stream << value;
            stream >> this->NormVelocity;
            doneReading++;
            }
          else if(command == "/YD/YDC/DCSIZS")
            {
            stream << value;
            stream >> this->NormStress;
            doneReading++;
            }                  
          }
        }        
      }
    }        
  return 1;
}
// --------------------------------------
int vtkFDEMReader::RequestData(vtkInformation *vtkNotUsed(request), vtkInformationVector **vtkNotUsed(inputVector), vtkInformationVector *outputVector)

{
  // Make sure we have a file to read.
  if(!this->FileName)
    {
    vtkErrorMacro("A FileName must be specified.");
		return 1;
    }
  if(strlen(this->FileName)==0)
    {
    vtkErrorMacro("A NULL FileName.");
		return 1;
    }
  
  //make instances of the classes that are needed
  FDEMFill *fills = NULL;
  FDEMPoint *points = NULL;
  FDEMCrackJoint *cracks = NULL;
  FDEMCrackJoint *joints = NULL;
  
  //only construct classes that are actually used
  if ( this->GetFill() )
    fills = new FDEMFill("Fills", this->NormCoord, this->NormForce, this->NormVelocity, this->NormStress);
  if ( this->GetPoint() )
    points = new FDEMPoint("Points", this->NormCoord, this->NormForce, this->NormVelocity, this->NormStress);
  if ( this->GetCrack() )
    cracks = new FDEMCrackJoint("Cracks", 0, this->NormCoord);
  if ( this->GetJoint() )
    joints = new FDEMCrackJoint("Joints", 1, this->NormCoord);
    
   
  //open the file as a raw binary file  
  this->RawFile = fopen(this->FileName,"rb");
  
  //make sure the rawFile is properly opened
  if ( this->RawFile == NULL )
    {
    //this is a workaround to the problem that file animations do not save properly
    //in the recent file list in paraview
    return 1;
    }
  
  /*the next step is to actually step through the file, 
  //one block at a time
  skip the first block, since it holds junk*/
  this->ReadBlock();
  
  //the block type determines if it we need to draw a joint or triangle
  long blockType = 0;
  while ( this->ReadBlock() )
    {
    blockType = this->ConvertBlock();    
    //determine if each block is of the type triangle, or joint
    if ( blockType == TRIANGLE )
      {
      //we can have fills and points for each block, so we have to
      //have two if statements to detect which ones we want
      if ( this->GetFill() )
        {
        fills->Add( this->Block );
        }
      if ( this->GetPoint() )
        {
        points->Add( this->Block );
        }            
      }
    else if ( blockType == JOINT )      
      {
      //likewise we can have cracks and joints so we want to be able to send the 
      //block to both of those objects
      if ( this->GetCrack() )
        {
        cracks->Add( this->Block );
        }
      if ( this->GetJoint() )
        {
        joints->Add( this->Block );
        }
      }
    }
  
  
  //we have finished reading the block information 
  //and now want to create the multiblock output
  vtkInformation *outInfo = outputVector->GetInformationObject(0);
  vtkMultiBlockDataSet *output = vtkMultiBlockDataSet::SafeDownCast( outInfo->Get(vtkDataObject::DATA_OBJECT()));
  
  
  //we need to keep track of how many blocks we have created      
  int blockCounter = 0;
  
  //add each block
  //while the user may select a form of representation that does not mean
  //the dataset will actually contain the information, so we need to poll the data before
  //we add it as a block
  if ( this->GetFill() )
    {    
    output->SetBlock(blockCounter,fills->GetOutput());
    //we want to name each block, with the name from the class
    output->GetMetaData(blockCounter)->Set(vtkCompositeDataSet::NAME(), fills->GetName());    
    blockCounter++;
    }
  if ( this->GetPoint() )
    {
    output->SetBlock(blockCounter,points->GetOutput());
    output->GetMetaData(blockCounter)->Set(vtkCompositeDataSet::NAME(), points->GetName());    
    blockCounter++;
    }
  if ( this->GetCrack() )
    {
    output->SetBlock(blockCounter,cracks->GetOutput());
    output->GetMetaData(blockCounter)->Set(vtkCompositeDataSet::NAME(), cracks->GetName());    
    blockCounter++;       
    }  
  if ( this->GetJoint() )
    {
    output->SetBlock(blockCounter,joints->GetOutput());
    output->GetMetaData(blockCounter)->Set(vtkCompositeDataSet::NAME(), joints->GetName());    
    blockCounter++;        
    }
  if (output->GetNumberOfPoints() == 0 )
    {
    vtkErrorMacro("No points in all blocks.");
    } 
  //cleanup
  delete fills;
  delete points;
  delete cracks;
  delete joints;
  
  //need to close the file
  fclose(this->RawFile);
     
  return 1;

  }
// --------------------------------------  
bool vtkFDEMReader::ReadBlock()
  {
  if ( feof( this->RawFile ) )
    {
    //we have hit the end of the file
    return false;
    }
  else
    {
    //read the next block, and store it in the class variable RawBlock
    fscanf(this->RawFile,"%s",this->RawBlock);
    return true;
    }
  }
  
// --------------------------------------  
long vtkFDEMReader::ConvertBlock()
  {
  //this code is the code from M1 functions convertChartoInt, and convertIntToDouble
  //combined into one method
  long tempBlock[100]; 
  long digit = this->FindValue(0);  
  long size = this->FindValue(1);
  
  tempBlock[0] = digit;
  tempBlock[1] = size; 
  for(int i=2; i < size; i++)
    { 
    tempBlock[i]=0;
    for(long j = (digit-1); j>=0; j-- )
      { 
      long value = this->FindValue( (i-2)* digit + 2 + j );      
      tempBlock[i] += value * this->Max[j];
      }
    }      
    
  //update digit and size, from the long conversion
  digit = tempBlock[0];
  size = tempBlock[1]; 
  double dmax =(double)(this->Max[digit]-1);
  double dval;  
  for(long i=0; i<size; i++)
    { 
    dval = (double)tempBlock[i];
    this->Block[i]= 2.0 * dval / dmax - 1.0;
    }         
  
  //we need to return the type of block as an long, or it will not turn out correctly
  //this way we can figure out if we have triangles or joints
  return tempBlock[2];
  }

// -------------------------------------- 
long vtkFDEMReader::FindValue( long position )
  {
  long temp = (long)this->RawBlock[position];
  if( temp < 0)
    {
    temp+=500;
    }   
  return this->Lookup[temp];  
  } 


