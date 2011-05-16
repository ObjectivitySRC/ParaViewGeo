
#include "vtkGocadVoxetWriter.h"
#include "vtkGocadWriter.h"

#include "vtkGocadBase.h"
#include "vtkDataSet.h"
#include "vtkGocadCompositeWriter.h"
#include "vtkStreamingDemandDrivenPipeline.h"
#include "vtkCompositeDataPipeline.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkObjectFactory.h"
#include "vtkSmartPointer.h"
#include "vtkCompositeDataSet.h"
#include "vtkCompositeDataIterator.h"
#include "vtkStdString.h"
#include <vtksys/ios/sstream>
#include <vtkstd/string>
#include <vtkstd/vector>


vtkCxxRevisionMacro(vtkGocadCompositeWriter, "$Revision: 1.2 $");
vtkStandardNewMacro(vtkGocadCompositeWriter);


class vtkGocadCompositeWriterInternals
{
public:
  vtkstd::vector< vtkSmartPointer<vtkGocadBase> > Writers;
  vtkstd::string FilePath;
  vtkstd::string FilePrefix;
  vtkstd::vector<int> DataTypes;
};

vtkGocadCompositeWriter::vtkGocadCompositeWriter()
{
	
  this->Internal = new vtkGocadCompositeWriterInternals;
}

vtkGocadCompositeWriter::~vtkGocadCompositeWriter()
{ 
  delete this->Internal;
}


//----------------------------------------------------------------------------
int vtkGocadCompositeWriter::RequestData(vtkInformation*,
                                              vtkInformationVector** inputVector,
                                              vtkInformationVector*)
{
  vtkInformation* inInfo = inputVector[0]->GetInformationObject(0);

  vtkCompositeDataSet *compositeData = vtkCompositeDataSet::SafeDownCast(
    inInfo->Get(vtkDataObject::DATA_OBJECT()));
  
  if (!compositeData) 
    {
    vtkErrorMacro("No hierarchical input has been provided. Cannot write"); 
    return 0;
    }
    
  // Make sure we have a file to write.
  if(!this->FileName)
    {
    vtkErrorMacro("Writer called with no FileName set.");        
    return 0;
    }
    
  // Create writers for each input.
  this->CreateWriters(compositeData);
    
  //recurse throught the file    
  int writerIdx=0;    
  
  //clear the file, so that everything can use append mode
  ostream *fp = this->OpenFile( this->FileName );    
  this->CloseFile( fp );
  
  //only append from now on!
  this->Append = true;
  
  
  //write everything in the block
  int ret = this->WriteComposite(compositeData, writerIdx);
    
  
  return ret;
}

//----------------------------------------------------------------------------
void vtkGocadCompositeWriter::CreateWriters(vtkCompositeDataSet* hdInput)
{
  this->Internal->Writers.clear();
  this->FillDataTypes(hdInput);

  vtkSmartPointer<vtkCompositeDataIterator> iter;
  iter.TakeReference(hdInput->NewIterator());
  iter->VisitOnlyLeavesOn();
  iter->TraverseSubTreeOn();

  size_t numDatasets = this->Internal->DataTypes.size();
  this->Internal->Writers.resize(numDatasets);

  int i = 0;
  for (iter->InitTraversal(); !iter->IsDoneWithTraversal(); iter->GoToNextItem())
    {
    vtkDataSet* ds = vtkDataSet::SafeDownCast(
      iter->GetCurrentDataObject());

    // Create a writer based on the type of this input.
    switch (this->Internal->DataTypes[i])
      {
    case VTK_POLY_DATA:  
		case VTK_UNSTRUCTURED_GRID:
    case VTK_STRUCTURED_GRID:
    case VTK_RECTILINEAR_GRID:
      if(!this->Internal->Writers[i].GetPointer() ||
        (strcmp(this->Internal->Writers[i]->GetClassName(),
                "vtkGocadWriter") != 0))
        {
        vtkGocadWriter* w = vtkGocadWriter::New();
        this->Internal->Writers[i] = w;               
        w->Delete();
        }
      vtkGocadBase::SafeDownCast(this->Internal->Writers[i].GetPointer())
        ->SetInput(ds);
      break;
    case VTK_STRUCTURED_POINTS:
    case VTK_IMAGE_DATA:
    case VTK_UNIFORM_GRID:
      if(!this->Internal->Writers[i].GetPointer() ||
        (strcmp(this->Internal->Writers[i]->GetClassName(),
                "vtkGocadVoxetWriter") != 0))
        {        
        vtkGocadVoxetWriter* w = vtkGocadVoxetWriter::New();
        this->Internal->Writers[i] = w;
        w->Delete();
        }
      vtkGocadBase::SafeDownCast(this->Internal->Writers[i].GetPointer())
        ->SetInput(ds);
      break;    
    default:
      this->Internal->Writers[i] = 0;    
      }
    if (vtkGocadBase* w = this->Internal->Writers[i].GetPointer())
      {
      w->SetAppend(true);     
      w->SetX_Axis( this->X_Axis );
      w->SetY_Axis( this->Y_Axis );
      w->SetZ_Axis( this->Z_Axis );       
      }
    i++;
    }
}

//----------------------------------------------------------------------------
void vtkGocadCompositeWriter::FillDataTypes(vtkCompositeDataSet* hdInput)
{
  vtkSmartPointer<vtkCompositeDataIterator> iter;
  iter.TakeReference(hdInput->NewIterator());
  iter->VisitOnlyLeavesOn();
  iter->TraverseSubTreeOn();

  this->Internal->DataTypes.clear();
  for (iter->InitTraversal(); !iter->IsDoneWithTraversal(); iter->GoToNextItem())
    {
    vtkDataSet* ds = vtkDataSet::SafeDownCast(
      iter->GetCurrentDataObject());
    if (ds)
      {
      this->Internal->DataTypes.push_back(ds->GetDataObjectType());
      }
    else
      {
      this->Internal->DataTypes.push_back(-1);
      }
    }
}

//----------------------------------------------------------------------------
int vtkGocadCompositeWriter::WriteNonCompositeData(vtkDataObject* dObj, vtkStdString name, int &writerIdx)
{
  // Write a leaf dataset.
  int myWriterIndex = writerIdx;
  writerIdx++;

  // Locate the actual data writer for this dataset.
  vtkGocadBase* writer = this->GetWriter(myWriterIndex);
  if (!writer)
    {
    return 1;
    }
    	
  vtkDataSet* curDS = vtkDataSet::SafeDownCast(dObj);
  if (!curDS)
    {
    if (dObj)
      {
      vtkWarningMacro("This writer cannot handle sub-datasets of type: "
        << dObj->GetClassName()
        << " Dataset will be skipped.");
      }
    return 1;
    }

  // Set the file name.  
  writer->SetFileName(this->FileName);
  
  //exception is that we had no meta data so ignore the null name
  if ( name )
		{		   
		writer->SetHeaderName( name.c_str() );  
		}
  writer->Write();  
  
  return 1;
}

//----------------------------------------------------------------------------
int vtkGocadCompositeWriter::WriteComposite(vtkCompositeDataSet* compositeData, int &writerIdx)
{
  if (!compositeData->IsA("vtkMultiBlockDataSet"))
    {
    vtkErrorMacro("vtkMultiBlockDataSet can only contain other "
      "vtkMultiBlockDataSet dataset nodes");
    return 0;
    }
    
  // Write each input.
  vtkSmartPointer<vtkCompositeDataIterator> iter;
  iter.TakeReference(compositeData->NewIterator());
  iter->VisitOnlyLeavesOff();
  iter->TraverseSubTreeOff();

  for (iter->InitTraversal(); !iter->IsDoneWithTraversal(); iter->GoToNextItem() )
    {
    vtkDataObject* curDO = iter->GetCurrentDataObject();
    if (curDO->IsA("vtkCompositeDataSet"))
      {
      // Write a composite dataset.
      vtkCompositeDataSet* curCD = vtkCompositeDataSet::SafeDownCast(curDO);
      
      //check to see if we can grab a group name
      //first we check if somebody has set the meta data information, if not we use a hard coded name
      vtkStdString name;                  
      if ( iter->HasCurrentMetaData() )      
				{
				vtkInformation *info = iter->GetCurrentMetaData();
				name = info->Get( vtkCompositeDataSet::NAME() );         
				}
			else
				{
				//since the sub block does not have a name, we use the general block name
				name="Block";
				}
				
			//write the header for the gocad group file      
			this->WriteGroupHeader( name , writerIdx+1 );      
			int ret = this->WriteComposite(curCD, writerIdx);
			//write the end section
			this->WriteGroupEnd();    
			if (!ret)
				{
				return 0;
				}        		
			}
    else
      {     
      //need to check and see if the iterator has any name meta data
      //so that we can pass this to the writer to use instead of the filename
      vtkStdString name;
      if ( iter->HasCurrentMetaData() )      
				{				
				vtkInformation *info = iter->GetCurrentMetaData();
				name = info->Get( vtkCompositeDataSet::NAME() );         				
				}
			 			
			this->WriteNonCompositeData( curDO, name, writerIdx);
      }
    }    
  
  return 1;
}

//----------------------------------------------------------------------------
vtkGocadBase* vtkGocadCompositeWriter::GetWriter(int index)
{
  int size = static_cast<int>(this->Internal->Writers.size());
  if(index >= 0 && index < size)
    {
    return this->Internal->Writers[index].GetPointer();
    }
  return 0;
}


//----------------------------------------------------------------------------
void vtkGocadCompositeWriter::WriteGroupHeader( int num)
{
  vtkStdString fName = this->FindFileName();
  this->WriteGroupHeader( fName, num );
}
//----------------------------------------------------------------------------
void vtkGocadCompositeWriter::WriteGroupHeader( vtkStdString name, int num )
{
  ostream *fp = this->OpenFile( this->FileName ); 	
	
	vtkstd::stringstream number;
	number << num;
	*fp << "GOCAD HeterogeneousGroup "  << number.str() << endl;
	*fp << "HEADER {" << endl;
	*fp << "name:" << name << endl;
	*fp << "}" << endl;
	*fp << "GOCAD_ORIGINAL_COORDINATE_SYSTEM" << endl;
	*fp << "NAME Default" << endl;
	*fp << "AXIS_NAME \"X\" \"Y\" \"Z\"" << endl;
	//Print the axis units to the file (meters or feet)
	*fp << this->GetUnitString() << endl;
	*fp << "ZPOSITIVE Elevation" << endl;
	*fp << "END_ORIGINAL_COORDINATE_SYSTEM" << endl;
	*fp << "BEGIN_MEMBERS" << endl;
	
  this->CloseFile( fp );
}

//----------------------------------------------------------------------------
void vtkGocadCompositeWriter::WriteGroupEnd()
{  
  ostream *fp = this->OpenFile( this->FileName );
 
  *fp << "END_MEMBERS" << endl;
  *fp << "END" << endl;
  
  this->CloseFile( fp );
  
}

//----------------------------------------------------------------------------
int vtkGocadCompositeWriter::FillInputPortInformation(
  int vtkNotUsed(port), vtkInformation* info)
{
  info->Set(vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkCompositeDataSet");
  return 1;
}