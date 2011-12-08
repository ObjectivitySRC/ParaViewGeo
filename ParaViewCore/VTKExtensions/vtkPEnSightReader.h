/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkPEnSightReader.h

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notice for more information.

  =========================================================================*/
// .NAME vtkPEnSightReader
// .SECTION Description
// Superclass for EnSight file parallel readers
// .SECTION Thanks
// <verbatim>
//
// This file has been developed as part of the CARRIOCAS (Distributed
// computation over ultra high optical internet network ) project (
// http://www.carriocas.org/index.php?lng=ang ) of the SYSTEM@TIC French ICT
// Cluster (http://www.systematic-paris-region.org/en/index.html) under the
// supervision of CEA (http://www.cea.fr) and EDF (http://www.edf.fr) by
// Oxalya (http://www.oxalya.com)
//
//  Copyright (c) CEA
//
// </verbatim>


#ifndef __vtkPEnSightReader_h
#define __vtkPEnSightReader_h

#include "vtkPGenericEnSightReader.h"

#include "vtkIdTypeArray.h" // For ivars
#include <vtkstd/map> // For ivars
#include <vtkstd/string> // For ivars
#include <vtkstd/algorithm> // For ivars
#include <vtkstd/string> // For ivars
#include <vtkstd/vector> // For ivars
#include <vtkstd/map> // For ivars

class vtkDataSet;
class vtkIdList;
class vtkMultiBlockDataSet;
class vtkInformation;
class vtkInformationVector;
class vtkUnsignedCharArray;
class vtkUnstructuredGrid;
class vtkFloatArray;
class vtkPEnSightReaderCellIdsType;

#define NEXTMODULO3( x ) (x==0) ? 1 : ((x==1) ? 2 : 0 )

class VTK_EXPORT vtkPEnSightReader : public vtkPGenericEnSightReader
{
 public:
  vtkTypeMacro(vtkPEnSightReader, vtkPGenericEnSightReader);
  void PrintSelf(ostream& os, vtkIndent indent);

  //BTX

  //----------------------------------------------------------------------------
  // PointIds and CellIds must be stored in a different way:
  // vtkstd::vector in non distributed mode
  // vtkstd::map in distributed mode
  // note: Ensight Ids are INTEGERS, not longs
  class vtkPEnSightReaderCellIds
  {

  public:
    typedef vtkstd::map< int , int > IntIntMap;
    typedef vtkstd::vector< int > IntVector;

    vtkPEnSightReaderCellIds()
      {
      this->mode = NON_SPARSE_MODE;
      this->cellMap = NULL;
      this->cellVector = NULL;
      }

    vtkPEnSightReaderCellIds(EnsightReaderCellIdMode amode)
      {
      this->mode = amode;
      if( this->mode == SPARSE_MODE )
        {
        this->cellMap = new IntIntMap;
        this->cellNumberOfIds = 0;
        this->cellVector = NULL;
        }
      else if( this->mode == IMPLICIT_STRUCTURED_MODE )
        {
        this->ImplicitDimensions = new int[3];
        this->ImplicitSplitDimension = -1;
        this->ImplicitSplitDimensionBeginIndex = -1;
        this->ImplicitSplitDimensionEndIndex = -1;
        }
      else
        {
        this->cellMap = NULL;
        this->cellVector = new IntVector;
        this->cellNumberOfIds = -1;
        this->cellLocalNumberOfIds = -1;
        }
      }

    ~vtkPEnSightReaderCellIds()
      {
      if( this->mode == SPARSE_MODE )
        delete this->cellMap;
      else if( this->mode == IMPLICIT_STRUCTURED_MODE )
        delete this->ImplicitDimensions;
      else
        delete this->cellVector;
      }

    void SetMode(EnsightReaderCellIdMode amode)
    {
      this->mode = amode;
      if( this->mode == SPARSE_MODE )
        {
        this->cellMap = new IntIntMap;
        this->cellNumberOfIds = 0;
        this->cellVector = NULL;
        }
      else if( this->mode == IMPLICIT_STRUCTURED_MODE )
        {
        this->ImplicitDimensions = new int[3];
        this->ImplicitSplitDimension = -1;
        this->ImplicitSplitDimensionBeginIndex = -1;
        this->ImplicitSplitDimensionEndIndex = -1;
        }
      else
        {
        this->cellMap = NULL;
        this->cellVector = new IntVector;
        this->cellNumberOfIds = -1;
        this->cellLocalNumberOfIds = -1;
        }
    }

    void SetImplicitDimensions(int dim1, int dim2, int dim3)
    {
      this->ImplicitDimensions[0] = dim1;
      this->ImplicitDimensions[1] = dim2;
      this->ImplicitDimensions[2] = dim3;
    }

    void SetImplicitSplitDimension(int dim)
    {
      this->ImplicitSplitDimension = dim;
    }

    void SetImplicitSplitDimensionBeginIndex(int begin)
    {
      this->ImplicitSplitDimensionBeginIndex = begin;
    }

    void SetImplicitSplitDimensionEndIndex(int end)
    {
      this->ImplicitSplitDimensionEndIndex = end;
    }

    // return -1 if not found
    int GetId(int id)
    {
      switch ( this->mode )
        {
        case SINGLE_PROCESS_MODE:
          {
          // Single Process compatibility
          return id;
          break;
          }
        case IMPLICIT_STRUCTURED_MODE:
          {
          if( this->ImplicitSplitDimension == -1 )
            return -1; // not initialized

          // Compute the global i j k index
          // id = i + j * dim[0] + k * dim[1] * dim[0]
          int index[3];
          index[2] = id / (this->ImplicitDimensions[0] * this->ImplicitDimensions[1]); // k
          index[1] = (id - (index[2] * this->ImplicitDimensions[0] * this->ImplicitDimensions[1])) / this->ImplicitDimensions[0]; // j
          index[0] = id - index[1] * this->ImplicitDimensions[0] - index[2] * this->ImplicitDimensions[1] * this->ImplicitDimensions[0]; // i
          if( (index[this->ImplicitSplitDimension] < this->ImplicitSplitDimensionBeginIndex) || (index[this->ImplicitSplitDimension] >= this->ImplicitSplitDimensionEndIndex) )
            {
            // not for me
            return -1;
            }
          else
            {
            // Compute the local id
            int localIndex[3];
            int localDim[3];
            int dim = this->ImplicitSplitDimension;
            localIndex[dim] = index[dim] - this->ImplicitSplitDimensionBeginIndex;
            localDim[dim] = this->ImplicitSplitDimensionEndIndex - this->ImplicitSplitDimensionBeginIndex;
            dim = NEXTMODULO3(dim);
            localIndex[ dim ] = index[ dim ];
            localDim[dim] = this->ImplicitDimensions[dim];
            dim = NEXTMODULO3(dim);
            localDim[dim] = this->ImplicitDimensions[dim];
            localIndex[ dim ] = index[ dim ];
            return localIndex[0] + localDim[0] * localIndex[1] + localDim[0] *  localDim[1] * localIndex[2];
            }
          }
        case SPARSE_MODE:
          {
          vtkstd::map<int,int>::iterator it = this->cellMap->find(id);
          if( it == this->cellMap->end() )
            return -1;
          else
            return (*this->cellMap)[id];
          break;
          }
        default:
          {
          if( this->cellVector->size() < (unsigned int)(id + 1) )
            return -1;
          else
            return (*this->cellVector)[id];
          break;
          }
        }
      return -1;
    }

    void SetId(int id, int value)
    {
      switch ( this->mode )
        {
        case SINGLE_PROCESS_MODE:
        case IMPLICIT_STRUCTURED_MODE:
          {
          // Compatibility Only
          // do noting
          break;
          }
        case SPARSE_MODE:
          {
          vtkstd::map<int,int>::iterator it = this->cellMap->find(id);
          if( it == this->cellMap->end() )
            this->cellNumberOfIds++;

          (*this->cellMap)[id] = value;
          break;
          }
        default:
          {
          if( this->cellVector->size() < (unsigned int)(id + 1) )
            {
            int k;
            int currentSize = static_cast<int>(this->cellVector->size());
            this->cellVector->resize(id + 1);
            for(k = currentSize ; k < id ; k++)
              {
              (*this->cellVector)[k] = -1;
              }
            (*this->cellVector)[id] = value;
            }
          else
            {
            (*this->cellVector)[id] = value;
            }
          break;
          }
        }
    }

    // In distributed mode, if id == -1, do not insert it in map
    int InsertNextId(int id)
    {
      switch ( this->mode )
        {
        case SINGLE_PROCESS_MODE:
        case IMPLICIT_STRUCTURED_MODE:
          {
          // Single Process compatibility
          // do noting
          break;
          }
        case SPARSE_MODE:
          {
          if( id != -1 )
            {
            (*this->cellMap)[this->cellNumberOfIds] = id;
            }
          // increment fake number of ids
          this->cellNumberOfIds++;
          return this->cellNumberOfIds - 1;
          break;
          }
        default:
          {
          this->cellVector->push_back(id);
          return static_cast<int>(this->cellVector->size() - 1);
          break;
          }
        }
      return static_cast<int>(this->cellVector->size() - 1);
    }

    int GetNumberOfIds()
    {
      switch ( this->mode )
        {
        case SINGLE_PROCESS_MODE:
          {
          // Single Process compatibility
          return this->cellNumberOfIds;
          break;
          }
        case IMPLICIT_STRUCTURED_MODE:
          {
          return this->cellNumberOfIds;
          }
        case SPARSE_MODE:
          {
          return this->cellNumberOfIds;
          break;
          }
        default:
          {
          // Point Ids are directly injected in the vector,
          // contrary to cell Ids which are "stacked" with
          // InsertNextId. So the real total number of Ids
          // for Points cannot be the size of the vector.
          // So we must inject it manually
          if( this->cellNumberOfIds >= 0 )
            return this->cellNumberOfIds;
          else
            return static_cast<int>(this->cellVector->size());
          break;
          }
        }
      return 0;
    }

    // Just inject the real total number of Ids
    void SetNumberOfIds(int n)
    {
      if( this->mode ==  SPARSE_MODE )
        {
        // do nothing
        }
      else
        {
        // Non sparse Or Single Process
        this->cellNumberOfIds = n;
        }
    }

    void SetLocalNumberOfIds(int n)
    {
      if( this->mode ==  SPARSE_MODE )
        {
        // do nothing
        }
      else
        {
        // Non sparse Or Single Process
        // Used for Structured compatibility
        this->cellLocalNumberOfIds = n;
        }
    }

    void Reset()
    {
      if( this->mode ==  SPARSE_MODE )
        {
        this->cellMap->clear();
        this->cellNumberOfIds = 0;
        }
      else
        {
        if( this->mode == NON_SPARSE_MODE )
          this->cellVector->clear();
        if( this->cellNumberOfIds >= 0 )
          this->cellNumberOfIds = -1;
        if( this->cellLocalNumberOfIds >= 0 )
          this->cellLocalNumberOfIds = -1;
        }
    }

    int GetLocalNumberOfIds()
    {
      switch ( this->mode )
        {
        case SINGLE_PROCESS_MODE:
          {
          // Single Process compatibility
          return this->cellNumberOfIds;
          break;
          }
        case IMPLICIT_STRUCTURED_MODE:
          {
          return this->cellLocalNumberOfIds;
          }
        case SPARSE_MODE:
          {
          return static_cast<int>(this->cellMap->size());
          break;
          }
        default:
          {
          if( this->cellLocalNumberOfIds >= 0 )
            {
            return this->cellLocalNumberOfIds;
            }
          else
            {
            //return this->cellVector->size();
            int result = 0;
            for(unsigned int i = 0 ; i < this->cellVector->size() ; i++)
              {
              if( (*this->cellVector)[i] != -1 )
                result++;
              }
            return result;
            }
          break;
          }
        }
      return 0;
    }

    vtkIdTypeArray* GenerateGlobalIdsArray(const char* name)
    {
      // Generate a sorted Array For Global Ids
      // Your local Ids must be consistent !
      if( this->mode == IMPLICIT_STRUCTURED_MODE)
        {
        vtkIdTypeArray* array = vtkIdTypeArray::New();
        array->SetNumberOfComponents(1);
        array->SetName(name);
        int localDim[3];

        int dim = this->ImplicitSplitDimension;
        localDim[dim] = this->ImplicitSplitDimensionEndIndex - this->ImplicitSplitDimensionBeginIndex;
        dim = NEXTMODULO3(dim);
        localDim[dim] = this->ImplicitDimensions[dim];
        dim = NEXTMODULO3(dim);
        localDim[dim] = this->ImplicitDimensions[dim];
        array->SetNumberOfTuples( localDim[0] * localDim[1] * localDim[2] );

        int index = 0;
        for(int k = 0; k < this->ImplicitDimensions[2] ; k++)
          {
          for(int j = 0; j < this->ImplicitDimensions[1] ; j++)
            {
            for(int i = 0; i < this->ImplicitDimensions[0] ; i++)
              {
              int n = ( this->ImplicitSplitDimension == 0 ) ? i : (( this->ImplicitSplitDimension == 1 ) ? j : k);
              if( (n >= this->ImplicitSplitDimensionBeginIndex) && (n < this->ImplicitSplitDimensionEndIndex) )
                {
                vtkIdType nn = n;
                array->SetTupleValue(index,&nn);
                index++;
                }
              }
            }
          }
        return array;
        }
      else
        {
        int i;
        vtkIdTypeArray* array = vtkIdTypeArray::New();
        array->SetNumberOfComponents(1);
        array->SetName(name);
        array->SetNumberOfTuples(this->GetLocalNumberOfIds());
        int min = 1000000000;
        int max = -1;
        for(i = 0; i < this->GetNumberOfIds() ; i++)
          {
          int id = this->GetId(i);
          if(id != -1)
            {
            vtkIdType ii = i;
            if( ii < min )
              min = ii;
            if( ii > max )
              max = ii;
            array->SetTupleValue(id, &ii);
            }
          }
        return array;
        }
    }

  protected:
    IntIntMap* cellMap;
    int cellNumberOfIds;
    int cellLocalNumberOfIds;
    IntVector* cellVector;
    // Implicit Structured Real (global) dimensions
    int *ImplicitDimensions;
    // Implicit Structured local dimensions
    int *ImplicitLocalDimensions;
    // Implicit Structured Split Dimension
    int ImplicitSplitDimension;
    // Implicit Structured Split Dimension Begin Index. Inclusive
    int ImplicitSplitDimensionBeginIndex;
    // Implicit StructuredSplit Dimension End Index. Exclusive
    int ImplicitSplitDimensionEndIndex;

    EnsightReaderCellIdMode mode;
  };


  enum ElementTypesList
  {
    POINT     = 0,
    BAR2      = 1,
    BAR3      = 2,
    NSIDED    = 3,
    TRIA3     = 4,
    TRIA6     = 5,
    QUAD4     = 6,
    QUAD8     = 7,
    NFACED    = 8,
    TETRA4    = 9,
    TETRA10   = 10,
    PYRAMID5  = 11,
    PYRAMID13 = 12,
    HEXA8     = 13,
    HEXA20    = 14,
    PENTA6    = 15,
    PENTA15   = 16,
    NUMBER_OF_ELEMENT_TYPES  = 17
  };

  enum VariableTypesList
  {
    SCALAR_PER_NODE            = 0,
    VECTOR_PER_NODE            = 1,
    TENSOR_SYMM_PER_NODE       = 2,
    SCALAR_PER_ELEMENT         = 3,
    VECTOR_PER_ELEMENT         = 4,
    TENSOR_SYMM_PER_ELEMENT    = 5,
    SCALAR_PER_MEASURED_NODE   = 6,
    VECTOR_PER_MEASURED_NODE   = 7,
    COMPLEX_SCALAR_PER_NODE    = 8,
    COMPLEX_VECTOR_PER_NODE    = 9,
    COMPLEX_SCALAR_PER_ELEMENT = 10,
    COMPLEX_VECTOR_PER_ELEMENT = 11
  };

  enum SectionTypeList
  {
    COORDINATES = 0,
    BLOCK       = 1,
    ELEMENT     = 2
  };
  //ETX

  // Description:
  // Get the Measured file name. Made public to allow access from
  // apps requiring detailed info about the Data contents
  vtkGetStringMacro(MeasuredFileName);

  // Description:
  // Get the Match file name. Made public to allow access from
  // apps requiring detailed info about the Data contents
  vtkGetStringMacro(MatchFileName);

  // Description:
  // The MeasuredGeometryFile should list particle coordinates
  // from 0->N-1.
  // If a file is loaded where point Ids are listed from 1-N
  // the Id to points reference will be wrong and the data
  // will be generated incorrectly.
  // Setting ParticleCoordinatesByIndex to true will force
  // all Id's to increment from 0->N-1 (relative to their order
  // in the file) and regardless of the actual Id of of the point.
  // Warning, if the Points are listed in non sequential order
  // then setting this flag will reorder them.
  vtkSetMacro(ParticleCoordinatesByIndex, int);
  vtkGetMacro(ParticleCoordinatesByIndex, int);
  vtkBooleanMacro(ParticleCoordinatesByIndex, int);

 protected:
  vtkPEnSightReader();
  ~vtkPEnSightReader();

  virtual int RequestInformation(vtkInformation*,
                                 vtkInformationVector**,
                                 vtkInformationVector*);
  virtual int RequestData(vtkInformation*,
                          vtkInformationVector**,
                          vtkInformationVector*);

  /*int RequestUpdateExtent(
    vtkInformation *vtkNotUsed(request),
    vtkInformationVector **inputVector,
    vtkInformationVector *outputVector);
  */

  // Description:
  // Set the Measured file name.
  vtkSetStringMacro(MeasuredFileName);

  // Description:
  // Set the Match file name.
  vtkSetStringMacro(MatchFileName);

  // Description:
  // Read the case file.  If an error occurred, 0 is returned; otherwise 1.
  int ReadCaseFile();
  int ReadCaseFileGeometry(char* line);
  int ReadCaseFileVariable(char* line);
  int ReadCaseFileTime(char* line);
  int ReadCaseFileFile(char* line);

  // set in UpdateInformation to value returned from ReadCaseFile
  int CaseFileRead;

  // Description:
  // Read the geometry file.  If an error occurred, 0 is returned; otherwise 1.
  virtual int ReadGeometryFile(const char* fileName, int timeStep,
                               vtkMultiBlockDataSet *output) = 0;

  // Description:
  // Read the measured geometry file.  If an error occurred, 0 is returned;
  // otherwise 1.
  virtual int ReadMeasuredGeometryFile(const char* fileName, int timeStep,
                                       vtkMultiBlockDataSet *output) = 0;

  // Description:
  // Read the variable files. If an error occurred, 0 is returned; otherwise 1.
  int ReadVariableFiles(vtkMultiBlockDataSet *output);

  // Description:
  // Read scalars per node for this dataset.  If an error occurred, 0 is
  // returned; otherwise 1.
  virtual int ReadScalarsPerNode(const char* fileName, const char* description,
                                 int timeStep, vtkMultiBlockDataSet *output,
                                 int measured = 0, int numberOfComponents = 1,
                                 int component = 0) = 0;

  // Description:
  // Read vectors per node for this dataset.  If an error occurred, 0 is
  // returned; otherwise 1.
  virtual int ReadVectorsPerNode(const char* fileName, const char* description,
                                 int timeStep, vtkMultiBlockDataSet *output,
                                 int measured = 0) = 0;

  // Description:
  // Read tensors per node for this dataset.  If an error occurred, 0 is
  // returned; otherwise 1.
  virtual int ReadTensorsPerNode(const char* fileName, const char* description,
                                 int timeStep, vtkMultiBlockDataSet *output) = 0;

  // Description:
  // Read scalars per element for this dataset.  If an error occurred, 0 is
  // returned; otherwise 1.
  virtual int ReadScalarsPerElement(const char* fileName, const char* description,
                                    int timeStep, vtkMultiBlockDataSet *output,
                                    int numberOfComponents = 1,
                                    int component = 0) = 0;

  // Description:
  // Read vectors per element for this dataset.  If an error occurred, 0 is
  // returned; otherwise 1.
  virtual int ReadVectorsPerElement(const char* fileName, const char* description,
                                    int timeStep, vtkMultiBlockDataSet *output) = 0;

  // Description:
  // Read tensors per element for this dataset.  If an error occurred, 0 is
  // returned; otherwise 1.
  virtual int ReadTensorsPerElement(const char* fileName, const char* description,
                                    int timeStep, vtkMultiBlockDataSet *output) = 0;

  // Description:
  // Read an unstructured part (partId) from the geometry file and create a
  // vtkUnstructuredGrid output.  Return 0 if EOF reached.
  virtual int CreateUnstructuredGridOutput(int partId,
                                           char line[80],
                                           const char* name,
                                           vtkMultiBlockDataSet *output) = 0;

  // Description:
  // Read a structured part from the geometry file and create a
  // vtkStructuredGridOutput.  Return 0 if EOF reached.
  virtual int CreateStructuredGridOutput(int partId,
                                         char line[80],
                                         const char* name,
                                         vtkMultiBlockDataSet *output) = 0;

  // Description:
  // Add another file name to the list for a particular variable type.
  void AddVariableFileName(const char* fileName1, const char* fileName2 = NULL);

  // Description:
  // Add another description to the list for a particular variable type.
  void AddVariableDescription(const char* description);

  // Description:
  // Record the variable type for the variable line just read.
  void AddVariableType();

  // Description:
  // Determine the element type from a line read a file.  Return -1 for
  // invalid element type.
  int GetElementType(const char* line);

  // Description:
  // Determine the section type from a line read a file.  Return -1 for
  // invalid section type.
  int GetSectionType(const char *line);

  // Description:
  // Replace the *'s in the filename with the given filename number.
  void ReplaceWildcards(char* filename, int num);

  // Description:
  // Remove leading blank spaces from a string.
  void RemoveLeadingBlanks(char *line);

  // Description:
  // Get the list for the given output index and cell type.
  vtkPEnSightReaderCellIds* GetCellIds(int index, int cellType);

  // Description:
  // Distributed Read Only.
  // Get the vtkIdList for the given GLOBAL output index and cell type.
  vtkIdType GetTotalNumberOfCellIds(int index);
  vtkIdType GetLocalTotalNumberOfCellIds(int index);

  // Description:
  // Distributed Read Only.
  // Get the list for the given points index.
  vtkPEnSightReaderCellIds* GetPointIds(int index);

  // Description:
  // Convenience method use to convert the readers from VTK 5 multiblock API
  // to the current composite data infrastructure.
  void AddToBlock(vtkMultiBlockDataSet* output,
                  unsigned int blockNo,
                  vtkDataSet* dataset);

  // Description:
  // Convenience method use to convert the readers from VTK 5 multiblock API
  // to the current composite data infrastructure.
  vtkDataSet* GetDataSetFromBlock(vtkMultiBlockDataSet* output,
                                  unsigned int blockNo);

  // Description:
  // Set the name of a block.
  void SetBlockName(vtkMultiBlockDataSet* output, unsigned int blockNo,
                    const char* name);

  // Description:
  // Merge InsertNextCell & GetId->InsertNextId
  // Take Distributed Read into account.
  void InsertNextCellAndId(vtkUnstructuredGrid*, int vtkCellType, vtkIdType numPoints, vtkIdType *points , int partId, int ensightCellType, vtkIdType globalId, vtkIdType numElements);
  void InsertVariableComponent(vtkFloatArray* array, int i, int component, float* content, int partId, int ensightCellType, int insertionType);

  // Description:
  // 1. Find future split dimension for distribution (biggest)
  // 2. Compute New dimensions
  // 3. Update PointIds and CellIds for compatibility with variables injection
  // 4. Generate Ghost Cells/Points arrays in output if ghostLevel > 0
  void PrepareStructuredDimensionsForDistribution(int partId, int *oldDimensions, int *newDimensions, int *splitDimension, int *splitDimensionBeginIndex,
                                                  int ghostLevel, vtkUnsignedCharArray *pointGhostArray, vtkUnsignedCharArray *cellGhostArray);

  char* MeasuredFileName;
  char* MatchFileName; // may not actually be necessary to read this file

  // pointer to lists of list (cell ids per element type per part)
  vtkPEnSightReaderCellIdsType* CellIds;

  // pointer to lists of list (point ids per element type per part)
  vtkPEnSightReaderCellIdsType* PointIds;

  // part ids of unstructured outputs
  vtkIdList* UnstructuredPartIds;
  // part ids of structured outputs
  vtkIdList* StructuredPartIds;

  bool CoordinatesAtEnd;
  bool InjectGlobalElementIds;
  bool InjectGlobalNodeIds;
  int LastPointId;

  int VariableMode;

  // pointers to lists of filenames
  char** VariableFileNames; // non-complex
  char** ComplexVariableFileNames;

  // array of time sets
  vtkIdList *VariableTimeSetIds;
  vtkIdList *ComplexVariableTimeSetIds;

  // array of file sets
  vtkIdList *VariableFileSetIds;
  vtkIdList *ComplexVariableFileSetIds;

  // collection of filename numbers per time set
  vtkIdListCollection *TimeSetFileNameNumbers;
  vtkIdList *TimeSetsWithFilenameNumbers;

  // collection of filename numbers per file set
  vtkIdListCollection *FileSetFileNameNumbers;
  vtkIdList *FileSetsWithFilenameNumbers;

  // collection of number of steps per file per file set
  vtkIdListCollection *FileSetNumberOfSteps;

  // ids of the time and file sets
  vtkIdList *TimeSetIds;
  vtkIdList *FileSets;

  int GeometryTimeSet;
  int GeometryFileSet;
  int MeasuredTimeSet;
  int MeasuredFileSet;

  float GeometryTimeValue;
  float MeasuredTimeValue;

  int UseTimeSets;
  vtkSetMacro(UseTimeSets, int);
  vtkGetMacro(UseTimeSets, int);
  vtkBooleanMacro(UseTimeSets, int);

  int UseFileSets;
  vtkSetMacro(UseFileSets, int);
  vtkGetMacro(UseFileSets, int);
  vtkBooleanMacro(UseFileSets, int);

  int NumberOfGeometryParts;

  // global list of points for measured geometry
  int NumberOfMeasuredPoints;

  int NumberOfNewOutputs;
  int InitialRead;

  int CheckOutputConsistency();

  int ParticleCoordinatesByIndex;

  double ActualTimeValue;

  int GhostLevels;

//BTX
  vtkstd::map<vtkstd::string, vtkstd::map<int, long> > FileOffsets;
//ETX

 private:
  vtkPEnSightReader(const vtkPEnSightReader&);  // Not implemented.
  void operator=(const vtkPEnSightReader&);  // Not implemented.
};

#endif
