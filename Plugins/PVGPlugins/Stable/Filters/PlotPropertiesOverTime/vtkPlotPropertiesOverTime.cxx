
#include "vtkPlotPropertiesOverTime.h"

#include "vtkCellData.h"
#include "vtkCompositeDataIterator.h"
#include "vtkCompositeDataSet.h"
#include "vtkDoubleArray.h"
#include "vtkGraph.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkIntArray.h"
#include "vtkIOStream.h"
#include "vtkMath.h"
#include "vtkObjectFactory.h"
#include "vtkOnePieceExtentTranslator.h"
#include "vtkPointData.h"
#include "vtkRectilinearGrid.h"
#include "vtkSmartPointer.h"
#include "vtkStreamingDemandDrivenPipeline.h"

#include <vtkstd/vector>
#include <vtkstd/map>
#include <vtkstd/string>

#include "math.h"


vtkStandardNewMacro(vtkPlotPropertiesOverTime);
vtkCxxRevisionMacro(vtkPlotPropertiesOverTime, "$Revision: 1.20 $");
//-----------------------------------------------------------------------------
vtkPlotPropertiesOverTime::vtkPlotPropertiesOverTime()
{
	this->MSDate_Property = NULL;
	this->Y_Property = NULL;
	this->BinCount = 10;
}

//-----------------------------------------------------------------------------
vtkPlotPropertiesOverTime::~vtkPlotPropertiesOverTime()
{
}

//-----------------------------------------------------------------------------
void vtkPlotPropertiesOverTime::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}

//-----------------------------------------------------------------------------
int vtkPlotPropertiesOverTime::FillInputPortInformation (int port, 
                                                   vtkInformation *info)
{
  this->Superclass::FillInputPortInformation(port, info);
  
  info->Set(vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkDataObject");
  return 1;
}

//----------------------------------------------------------------------------
int vtkPlotPropertiesOverTime::RequestInformation(
  vtkInformation* vtkNotUsed(request),
  vtkInformationVector** vtkNotUsed(inputVector),
  vtkInformationVector* outputVector)
{
  // get the info objects
  vtkInformation* outInfo = outputVector->GetInformationObject(0);

  // Extents are {0, no. of bins, 0, 0, 0, 0};
  int extent[6] = {0,0,0,0,0,0};
  extent[1] = this->BinCount;
  outInfo->Set(vtkStreamingDemandDrivenPipeline::WHOLE_EXTENT(), extent, 6);

  // Setup ExtentTranslator so that all downstream piece requests are
  // converted to whole extent update requests, as need by the histogram filter.
  vtkStreamingDemandDrivenPipeline* sddp = 
    vtkStreamingDemandDrivenPipeline::SafeDownCast(this->GetExecutive());
  if (strcmp(
      sddp->GetExtentTranslator(outInfo)->GetClassName(), 
      "vtkOnePieceExtentTranslator") != 0)
    {
    vtkExtentTranslator* et = vtkOnePieceExtentTranslator::New();
    sddp->SetExtentTranslator(outInfo, et);
    et->Delete();
    }

	return 1;
}


//-----------------------------------------------------------------------------
int vtkPlotPropertiesOverTime::GetInputFieldAssociation()
{
	return 1;
}

//-----------------------------------------------------------------------------
vtkFieldData* vtkPlotPropertiesOverTime::GetInputFieldData(vtkDataObject* input)
{
  return 0;
}


//-----------------------------------------------------------------------------
int vtkPlotPropertiesOverTime::RequestData(vtkInformation* /*request*/, 
                                     vtkInformationVector** inputVector, 
                                     vtkInformationVector* outputVector)
{
	 vtkInformation *inInfo = inputVector[0]->GetInformationObject ( 0 );
	 vtkDataSet *input = vtkDataSet::SafeDownCast (
						  inInfo->Get ( vtkDataObject::DATA_OBJECT() ) );

  // Build an empty output grid in advance, so we can bail-out if we
  // encounter any problems
  vtkInformation* const output_info = outputVector->GetInformationObject(0);
  vtkRectilinearGrid* const output_data = vtkRectilinearGrid::SafeDownCast(
    output_info->Get(vtkDataObject::DATA_OBJECT()));
  output_data->Initialize();
  //output_data->SetDimensions(this->BinCount+1, 1, 1);

	// "bin_extents" will contain the boundaries between each histogram bin
  vtkDoubleArray* bin_extents = vtkDoubleArray::New();
	bin_extents->SetName("bin_extents");

  vtkDoubleArray* bin_values = vtkDoubleArray::New();
	bin_values->SetName("bin_values");

  vtkDoubleArray* zCoords = vtkDoubleArray::New();
	zCoords->InsertNextValue(0);


	typedef vtkstd::map < double, std::pair<double,double> > dpMap;
	typedef vtkstd::multimap <double,double > ddMultiMap;
	ddMultiMap xyMultiMap;
	vtkDataArray *xArray = input->GetCellData()->GetArray(this->MSDate_Property);
	vtkDataArray *yArray = input->GetCellData()->GetArray(this->Y_Property);

	if(!xArray)
	{
		xArray = input->GetPointData()->GetArray(this->MSDate_Property);
		yArray = input->GetPointData()->GetArray(this->Y_Property);
		if( !(xArray&&yArray) )
		{
			vtkErrorMacro("the input arrays must be cell properties or point properties and not one of each" );
			return 1;
		}
	}


	for(vtkIdType i=0; i<xArray->GetNumberOfTuples(); i++)
	{
		xyMultiMap.insert(vtkstd::pair<double,double>(xArray->GetComponent(i,0), yArray->GetComponent(i,0)));
	}

	double xMin = xyMultiMap.begin()->first;
	double xMax = xyMultiMap.rbegin()->first;

	unsigned long numberOfDays = this->Years*365 + this->Months*30 + this->Days;

	if(numberOfDays == 0)
	{
		vtkErrorMacro("you must choose a bin extend greater than 0 days");
		return 0;
	}
	this->BinCount = (xMax - xMin)/(numberOfDays);

	double step = (xMax - xMin)/this->BinCount;

	dpMap binMap;
	double currentBin = step;
	double current;
	int factor;

	for(int i=1; i<this->BinCount+1; i++)
	{
		binMap[(step*i) + xMin].first = 0;
		binMap[(step*i) + xMin].second = 1;
	}

	for(ddMultiMap::iterator iter = xyMultiMap.begin(); iter!= xyMultiMap.end(); iter++)
	{
		factor = ceil( (iter->first - xMin)/step );
		current = factor*step + xMin;
		if(current == 0)
		{
			current = step + xMin;
		}
		binMap[current].first += iter->second;
		binMap[current].second ++;

	}



	bin_extents->InsertNextValue(xMin);
	for(dpMap::iterator iter = binMap.begin(); iter!= binMap.end(); iter++)
	{
		bin_extents->InsertNextValue(iter->first);
		if(iter->second.second != 1)
		{
			bin_values->InsertNextValue( (iter->second.first)/(iter->second.second-1) );
		}
		else
		{
			bin_values->InsertNextValue( (iter->second.first) );
		}
	}
  

	output_data->SetDimensions(this->BinCount+1, 1, 1);

	output_data->SetXCoordinates(bin_extents);
	output_data->SetYCoordinates(bin_values);
	output_data->SetZCoordinates(zCoords);

	output_data->GetCellData()->AddArray(bin_values);
	output_data->GetPointData()->AddArray(bin_extents);

	bin_values->Delete();
	bin_extents->Delete();
	zCoords->Delete();

  return 1;
}

