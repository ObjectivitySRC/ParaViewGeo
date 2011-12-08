/*=========================================================================

  Program:   ParaView
  Module:    vtkRedistributePolyData.h

  Copyright (c) Kitware, Inc.
  All rights reserved.
  See Copyright.txt or http://www.paraview.org/HTML/Copyright.html for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
/*----------------------------------------------------------------------------
 Copyright (c) Los Alamos National Laboratory
 See Copyright.txt or http://www.paraview.org/HTML/Copyright.html for details.
----------------------------------------------------------------------------*/

// .NAME vtkRedistributePolyData - redistribute poly cells from other processes
//                        (special version to color according to processor)

#ifndef __vtkRedistributePolyData_h
#define __vtkRedistributePolyData_h

#include "vtkPolyDataToPolyDataFilter.h"

//*******************************************************************
class vtkDataArray;
class vtkDataSetAttributes;
class vtkMultiProcessController;
class vtkSocketController;

class VTK_EXPORT vtkRedistributePolyData : public vtkPolyDataToPolyDataFilter 
{
public:
  vtkTypeMacro(vtkRedistributePolyData, vtkPolyDataToPolyDataFilter);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  static vtkRedistributePolyData *New();

  // Description:
  // The filter needs a controller to determine which process it is in.
  virtual void SetController(vtkMultiProcessController*);
  vtkGetObjectMacro(Controller, vtkMultiProcessController); 

  vtkGetMacro(ColorProc,int);
  vtkSetMacro(ColorProc,int);
  void SetColorProc() { this->ColorProc = 1; };

  // Description:
  // These are here for ParaView compatibility. Not used.
  virtual void SetSocketController(vtkSocketController*) {};
  virtual void SetPassThrough(int) {};
  virtual int  GetPassThrough() { return 0; };
  vtkBooleanMacro(PassThrough, int);

protected:
  vtkRedistributePolyData();
  ~vtkRedistributePolyData();

//BTX
  enum {
    CELL_ID_TAG        = 10,
    POINT_COORDS_TAG   = 20,
    NUM_POINTS_TAG     = 30,
    NUM_CELLS_TAG      = 40,
    BOUNDS_TAG         = 60,
    CNT_SEND_TAG       = 80,
    CNT_REC_TAG        = 90,
    SEND_PROC_TAG      = 100,
    SEND_NUM_TAG       = 110,
    REC_PROC_TAG       = 120,
    REC_NUM_TAG        = 130,
    NUM_CURR_CELLS_TAG = 140,

    CELL_CNT_TAG       = 150,
    CELL_TAG           = 160,
    POINTS_SIZE_TAG    = 170,
    POINTS_TAG         = 180
  };

  class VTK_EXPORT vtkCommSched
  {
  public:
    vtkCommSched();
    ~vtkCommSched();
   
    int SendCount;
    int ReceiveCount;
    int* SendTo;
    int* ReceiveFrom;
    vtkIdType* NumberOfCells;
    vtkIdType** SendNumber;
    vtkIdType** ReceiveNumber;
   
    vtkIdType*** SendCellList;
    vtkIdType** KeepCellList;
   
  private:
    vtkCommSched(const vtkCommSched&); // Not implemented
    void operator=(const vtkCommSched&); // Not implemented
  };

//ETX

  virtual void MakeSchedule (vtkCommSched*);
  void OrderSchedule (vtkCommSched*);

  void SendCellSizes (vtkIdType*, vtkIdType*, vtkPolyData*, int, 
                      vtkIdType&, vtkIdType*, vtkIdType**); 
  void CopyCells (vtkIdType*,vtkPolyData*, vtkPolyData*, vtkIdType**); 
  void SendCells (vtkIdType*, vtkIdType*, vtkPolyData*, vtkPolyData*, 
                  int, vtkIdType&, vtkIdType*, vtkIdType**); 
  void ReceiveCells (vtkIdType*, vtkIdType*, vtkPolyData*, int, 
                     vtkIdType*, vtkIdType*, vtkIdType, vtkIdType);

  void FindMemReq (vtkIdType*, vtkPolyData*, vtkIdType&, vtkIdType*);

  void AllocateCellDataArrays (vtkDataSetAttributes*, vtkIdType**, 
                               int, vtkIdType*);
  void AllocatePointDataArrays (vtkDataSetAttributes*, vtkIdType*, 
                                int, vtkIdType);
  void AllocateArrays (vtkDataArray*, vtkIdType);

  void CopyDataArrays(vtkDataSetAttributes*, vtkDataSetAttributes*,
                      vtkIdType, vtkIdType*, int);

  void CopyCellBlockDataArrays(vtkDataSetAttributes* , 
                               vtkDataSetAttributes* ,
                               vtkIdType , vtkIdType, 
                               vtkIdType, vtkIdType, int);

  void CopyArrays (vtkDataArray*, vtkDataArray*, vtkIdType, vtkIdType*, 
                   int); 

  void CopyBlockArrays (vtkDataArray*, vtkDataArray*, vtkIdType, 
                        vtkIdType, vtkIdType, vtkIdType, int); 

  void SendDataArrays (vtkDataSetAttributes*, vtkDataSetAttributes*,
                       vtkIdType, int, vtkIdType*, int); 

  void SendCellBlockDataArrays (vtkDataSetAttributes*, 
                                vtkDataSetAttributes*,
                                vtkIdType, int, vtkIdType, int); 

  void SendArrays (vtkDataArray*, vtkIdType, int, 
                   vtkIdType*, int); 

  void SendBlockArrays (vtkDataArray*, vtkIdType, int, 
                        vtkIdType, int); 

  void ReceiveDataArrays (vtkDataSetAttributes*, vtkIdType, int, 
                          vtkIdType*, int); 

  void ReceiveArrays (vtkDataArray*, vtkIdType, int, 
                      vtkIdType*, int); 

  void Execute();

  // Do this as a proprocessing step.
  void CompleteInputArrays(vtkPolyData* input);
  void ReceiveInputArrays(vtkDataSetAttributes* attr, int recFrom);
  void SendInputArrays(vtkDataSetAttributes* attr, int sendTo);
  int DoubleCheckArrays(vtkPolyData* input);

  vtkMultiProcessController *Controller;

  int ColorProc; // Set to 1 to color data according to processor

private:
  vtkRedistributePolyData(const vtkRedistributePolyData&); // Not implemented
  void operator=(const vtkRedistributePolyData&); // Not implemented
  
};

//****************************************************************

#endif


