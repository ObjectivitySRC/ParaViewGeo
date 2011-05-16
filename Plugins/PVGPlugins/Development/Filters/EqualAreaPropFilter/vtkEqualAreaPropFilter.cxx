#include "vtkEqualAreaPropFilter.h"

#include "vtkPointData.h"
#include "vtkSmartPointer.h"
#include "vtkStreamingDemandDrivenPipeline.h"
#include "vtkGenericCell.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkObjectFactory.h"
#include "vtkCellData.h"
#include "vtkDataArray.h"
#include "vtkPointSet.h"
#include "vtkFloatArray.h"
#include "vtkMath.h"

#include <math.h>

vtkCxxRevisionMacro(vtkEqualAreaPropFilter, "$Revision: 1.6 $");
vtkStandardNewMacro(vtkEqualAreaPropFilter);

//----------------------------------------------------------------------------
vtkEqualAreaPropFilter::vtkEqualAreaPropFilter()
{
  const int DEFAULT = 20;
  this->SetNumberOfInputPorts ( 1 );
  // Sensible initial values
  this->Bins = DEFAULT;
}

//----------------------------------------------------------------------------
int vtkEqualAreaPropFilter::FillInputPortInformation ( int vtkNotUsed(port), vtkInformation* info )
  {
  info->Set ( vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkPointSet" );
  return 1;
  }

//----------------------------------------------------------------------------
int vtkEqualAreaPropFilter::FillOutputPortInformation(int vtkNotUsed(port), vtkInformation* info)
{
  info->Set(vtkDataObject::DATA_TYPE_NAME(), "vtkPointSet");
  return 1;
}


//----------------------------------------------------------------------------
int vtkEqualAreaPropFilter::RequestInformation(vtkInformation *vtkNotUsed(request),vtkInformationVector **inputVector,vtkInformationVector *outputVector)
{
  vtkInformation *outInfo = outputVector->GetInformationObject(0);
  return 1;
}



//----------------------------------------------------------------------------
int vtkEqualAreaPropFilter::RequestData(vtkInformation *vtkNotUsed(request),vtkInformationVector **inputVector,vtkInformationVector *outputVector)
{
  // get the info object
  vtkInformation *outInfo = outputVector->GetInformationObject(0);
  //get the output
  vtkPointSet *output = vtkPointSet::SafeDownCast(outInfo->Get(vtkDataObject::DATA_OBJECT()));
	// get input data
  vtkInformation *inInfo = inputVector[0]->GetInformationObject(0);
  vtkPointSet *input = vtkPointSet::SafeDownCast (inInfo->Get ( vtkDataObject::DATA_OBJECT() ) );
  vtkDataArray *inScalar = this->GetInputArrayToProcess(0,inputVector);
  if (!inScalar)
    {
    vtkDebugMacro(<<"No scalar data to process");
    return 1;
    }

	output->ShallowCopy(input);

  vtkIdType cellId;
  vtkIdList *cellPts;  
  vtkCell *cell;  
  int i, ptId, numPts, numTuples, numCellPts;
	double DataRange[2];
	inScalar->GetRange(DataRange);
  
  numPts = input->GetNumberOfPoints();
	numTuples = inScalar->GetNumberOfTuples();
  
  // are we using pointScalars?
  bool usePointScalars = (numTuples == numPts);
/***  
	FILE* dbfp = fopen("C:\\temp\\EAPdb.txt","w");
	fprintf(dbfp,"Attribute=%s  Range=%.2f to %.2f  Number of Pts=%d  Number of Tuples=%d\n",
		inScalar->GetName(), DataRange[0], DataRange[1], numPts, numTuples);
	fclose(dbfp);
***/
	float* sortTbl = new float[numTuples];
	int dataid, outid, oid;
	float fdata;
	bool bInsert;

	for (dataid=0; dataid<numTuples; dataid++)
		{
		fdata = inScalar->GetComponent(dataid, 0);
		bInsert = false;
		for (outid=0; outid<dataid; outid++)
			{
			if (fdata < sortTbl[outid])
				{
				bInsert = true;
				for (oid=dataid; oid>outid+1; oid--)
					{
					sortTbl[oid] = sortTbl[oid-1];
					}
				sortTbl[outid] = fdata;
				break;
				}
			}
		if (!bInsert)
			sortTbl[dataid] = fdata;
	}
	float* fBrkTbl = new float[Bins];
	float tblInterval = (float)(((float)numTuples)/((float)Bins));

	for (int bi=1; bi<Bins; bi++)
		{
		fBrkTbl[bi-1] = sortTbl[(int)(0.5+bi*tblInterval)];
		}
	fBrkTbl[Bins-1] = DataRange[1]+1.0;

	delete sortTbl;

  // Allocate space for the EqualAreaProperty bins scalar data.
  vtkSmartPointer<vtkFloatArray> binProperty =
    vtkSmartPointer<vtkFloatArray>::New();
  binProperty->SetNumberOfTuples(numTuples);
  char EAPropName[10];
  sprintf(EAPropName,"EAP%d",this->Bins);
  binProperty->SetName(EAPropName);
	int btid;
	float fValue, fLRng, fURng, fMidRng;

	// Check that the scalars of each cell satisfy the threshold criterion
  for (cellId=0; cellId < input->GetNumberOfCells(); cellId++)
    {
    cell = input->GetCell(cellId);
    cellPts = cell->GetPointIds();
    numCellPts = cell->GetNumberOfPoints();
    
    if ( usePointScalars )
      {      
      for ( i=0; i < numCellPts; i++)
        {
        ptId = cellPts->GetId(i);                   
        fdata = inScalar->GetComponent(ptId, 0);
				for (btid=0; btid<Bins; btid++)
					{
					if (fdata < fBrkTbl[btid])
						{
						if (btid==0) fLRng = (float)DataRange[0];
						else fLRng = fBrkTbl[btid-1];
						fURng = fBrkTbl[btid];
						fMidRng = (fLRng+fURng)/2;
						fValue = ((float)btid)+(fdata-fMidRng)/(fURng-fLRng);
						binProperty->SetValue(ptId,fValue);
						break;
						}
					}
        }              
      }
    else //use cell scalars
      {                   
      fdata = inScalar->GetComponent(cellId, 0);       
			for (btid=0; btid<Bins; btid++)
				{
				if (fdata < fBrkTbl[btid])
					{
					if (btid==0) fLRng = (float)DataRange[0];
					else fLRng = fBrkTbl[btid-1];
					fURng = fBrkTbl[btid];
					fMidRng = (fLRng+fURng)/2;
					fValue = ((float)btid)+(fdata-fMidRng)/(fURng-fLRng);
					binProperty->SetValue(btid,fValue);
					break;
					}
				}
      }
    }
  if ( usePointScalars )
	  output->GetPointData()->AddArray ( binProperty );
	else
	  output->GetCellData()->AddArray ( binProperty );

	delete fBrkTbl;
  return 1;
}

//----------------------------------------------------------------------------
void vtkEqualAreaPropFilter::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
   os << indent << "Bins: " << this->Bins << endl;
}
