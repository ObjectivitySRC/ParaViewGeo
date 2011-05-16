#include "vtkEPSReader.h"

#include "vtkDataArray.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkObjectFactory.h"
#include "vtkOnePieceExtentTranslator.h"
#include "vtkStreamingDemandDrivenPipeline.h"
#include "vtkPythonProgrammableFilter.h"
#include "vtkDoubleArray.h"
#include "vtkStringArray.h"

vtkCxxRevisionMacro(vtkEPSReader, "$Revision: 3.18 $");
vtkStandardNewMacro(vtkEPSReader);

// Constructor
vtkEPSReader::vtkEPSReader()
  {
  this->SetExecuteMethod(vtkEPSReader::ExecuteScript, this);
  this->OutputDataSetType = VTK_POLY_DATA;
  
  this->FileName = NULL;
  this->Date = NULL;  

  //work arounds for the problem of the python code can't return info
  this->PyDate = vtkStringArray::New();
  this->PyDate->SetNumberOfValues(2);
  
  this->PyPointFile = vtkStringArray::New();
  this->PyPointFile->SetNumberOfValues(1);
   
  this->PyOffset = vtkDoubleArray::New();
  this->PyOffset->SetNumberOfValues(1);
  
  
  //we should be able to do this a better way once we can figure out how vtkPythonPro...Filter actually does its python binding

  vtkStdString requestData;
  requestData = "import csv\n\
from datetime import datetime\n\
import time\n\
DATE_FORMAT=\"%Y-%m-%dT%H:%M:%S\"\n\
OFFSET = float(_Offset.GetValue(0))\n\
DATE = _Date.GetValue(0)\n\
NDate = _Date.GetValue(1)\n\
if (NDate):\n\
  DATE = NDate\n\
POINTFILENAME = _PointFile.GetValue(0)\n\
def ReadPointFile(pointFile):\n\
  points = vtk.vtkPoints()\n\
  try:\n\
    pointCSV = csv.reader(pointFile)\n\
    headerLine = pointCSV.next()\n\
    header = list()\n\
    for key in headerLine:\n\
      header.append(key.lower().strip())\n\
    parseFile = csv.DictReader(pointFile,header)\n\
    points.InsertNextPoint(0,0,0)\n\
    for line in parseFile:\n\
      try:\n\
        x = float(line['xp'])\n\
        y = float(line['yp'])\n\
        z = float(line['zp'])\n\
      except:\n\
        pass\n\
      else:\n\
        points.InsertNextPoint(x,y,z)\n\
  except:\n\
    print 'Failed to parse point file'\n\
    return None\n\
  else:\n\
    return points\n\
def ReadTriangleFile(triangleFile):\n\
  cells = vtk.vtkCellArray()\n\
  properties = dict()\n\
  try:\n\
    triangleFile.next()\n\
    triangleFile.next()\n\
    triangleCSV = csv.reader(triangleFile)\n\
    headerLine = triangleCSV.next()\n\
    header = list()\n\
    for key in headerLine:\n\
      header.append(key.lower().strip())\n\
    parseFile = csv.DictReader(triangleFile,header)\n\
    TRI = 3 #number of points in a triangle\n\
    triKeys = set(['pid1', 'pid2', 'pid3'])   #keys used in the csv to be point id's\n\
    startKey = \"start\"\n\
    endKey = \"duration\"\n\
    for line in parseFile:\n\
      cells.InsertNextCell(TRI)\n\
      for key in line.keys():\n\
        if (key in triKeys):\n\
          p = int ( line[ key ] )\n\
          cells.InsertCellPoint(p)\n\
        elif startKey == key:\n\
          addTimeValues(properties,startKey,endKey,line[startKey],line[endKey])\n\
        elif not (endKey == key): #we need to ignore the end key, as it is handled in startkey\n\
          addProperty(properties,key, line[key])\n\
  except ValueError:\n\
    print 'failed to load triangle File'\n\
    return None,None\n\
  else:\n\
    return cells,properties\n\
def addTimeValues(properties,startKey,endKey,startValue,endValue):\n\
  addProperty(properties,startKey,startValue)\n\
  duration = float(endValue)/OFFSET\n\
  addProperty(properties,endKey, duration)\n\
  absStart = \"JulianTime Start\"\n\
  absEnd = \"JulianTime End\"\n\
  startOffset = float(startValue) * OFFSET\n\
  startDate = datetime.strptime(DATE,DATE_FORMAT)\n\
  seconds = startDate.second + startOffset\n\
  julianStart = findJulianTime(startDate.year,startDate.month, startDate.day, startDate.hour, startDate.minute, seconds)\n\
  seconds += float(endValue)\n\
  julianEnd = findJulianTime(startDate.year,startDate.month, startDate.day, startDate.hour, startDate.minute, seconds)\n\
  addProperty(properties, absStart, julianStart)\n\
  addProperty(properties, absEnd, julianEnd)\n\
def findJulianTime(year,month,day,hour,min,sec):\n\
  UT=hour+min/60+sec/3600\n\
  total_seconds=hour*3600+min*60+sec\n\
  fracday=total_seconds/86400\n\
  if (100*year+month-190002.5)>0:\n\
      sig=1\n\
  else:\n\
      sig=-1\n\
  JD = 367*year - int(7*(year+int((month+9)/12))/4) + int(275*month/9) + day + 1721013.5 + UT/24 - 0.5*sig +0.5\n\
  return JD\n\
def addProperty(properties,key,value):\n\
  if not (key in properties):\n\
    try:\n\
      prop = float(value)\n\
    except:\n\
      prop = value\n\
      properties[key]= vtk.vtkStringArray()\n\
    else:\n\
      properties[key] = vtk.vtkDoubleArray()\n\
    properties[key].SetName(key)\n\
    properties[key].InsertNextValue(prop)\n\
  else:\n\
    try:\n\
      prop = float(value)\n\
    except:\n\
      prop = value\n\
    properties[key].InsertNextValue(prop)\n\
output = self.GetPolyDataOutput()\n\
try:\n\
  triangleFile = file( FILENAME )\n\
  pointFile = file( POINTFILENAME )\n\
except:\n\
  print \"failed to load data, since one of the paths was incorrect\"\n\
else:\n\
  cells,properties = ReadTriangleFile(triangleFile)\n\
  points = ReadPointFile(pointFile)\n\
  if (cells and points):\n\
    if (properties):\n\
      for prop in properties.values():\n\
        output.GetCellData().AddArray(prop)\n\
    output.SetPoints(points)\n\
    output.SetPolys(cells)\n\
  triangleFile.close()\n\
  pointFile.close()\n\
";
  
  this->SetScript( requestData.c_str() );  
      
  vtkStdString requestInfo;
  requestInfo = "import time\n\
import os\n\
def findStartDate(triangleFilePath):\n\
  START_DATE=\"STARTDATE\"\n\
  FILE_FORMAT=\"%Y-%m-%d\"\n\
  OUTPUT_FORMAT=\"%Y-%m-%dT%H:%M:%S\"\n\
  DEFAULT_START_TIME=\"2008-01-01T00:00:01\"\n\
  try:\n\
    triangleFile = file(triangleFilePath)\n\
    startDateRaw = triangleFile.next()\n\
    startDateRaw = startDateRaw.upper()\n\
    if ( startDateRaw.find(START_DATE) >= 0 ):\n\
      raw = startDateRaw.split(START_DATE)[1].strip()\n\
      raw = raw[1:] #skip the seperator\n\
      tempDate = time.strptime(raw,FILE_FORMAT)\n\
    else:\n\
      tempDate = time.strptime(DEFAULT_START_TIME,OUTPUT_FORMAT)\n\
  except ValueError:\n\
    date = DEFAULT_START_TIME\n\
  else:\n\
    date =  time.strftime(OUTPUT_FORMAT,tempDate)\n\
  _Date.SetValue(0,date)\n\
def findOffsetNumber(triangleFilePath):\n\
  DEFAULT_OFFSET=604800.0\n\
  OFFSET = \"OFFSET\"\n\
  offset = DEFAULT_OFFSET\n\
  try:\n\
    triangleFile = file(triangleFilePath)\n\
    triangleFile.next()\n\
    offsetLine = triangleFile.next() #skip down to the offset line\n\
    offsetLine = offsetLine.upper()\n\
    if(offsetLine.find(OFFSET) >= 0):\n\
      raw = offsetLine.split(OFFSET)[1].strip()\n\
      offset = float(raw[1:]) #convert the number to a float, and strip the seperator\n\
  except:\n\
    pass #use default\n\
  _Offset.SetValue(0,offset)\n\
def findPointFile(triangleFilePath):\n\
  goodExts = [\".csv\",\".sept\",\".setr\"]\n\
  (filepath,filename) = os.path.split(triangleFilePath)\n\
  (name, ext) = os.path.splitext(filename)\n\
  allFiles = os.listdir(filepath)\n\
  TR=False\n\
  TR_STRING = \"tr\"\n\
  PT_STRINGS = [\"pt\", \"pts\"]\n\
  TRSIZE=2\n\
  if (name[-TRSIZE:].lower() == TR_STRING):\n\
    TR=True\n\
  PointFiles = list()\n\
  name = name.lower() #we need to make this lower to make sure all the compares will work correctly\n\
  for f in allFiles:\n\
    (tmpName, tmpExt) = os.path.splitext(f)\n\
    tmpName = tmpName.lower()\n\
    if (tmpExt.lower() in goodExts):\n\
      if TR:\n\
        for value in PT_STRINGS:\n\
          size = len(value)\n\
          if tmpName[-size:] == value and tmpName[:-size] == name[:-TRSIZE]:\n\
            PointFiles.insert(0,f)\n\
      elif not (tmpExt == ext) and tmpName == name:\n\
        PointFiles.append(f)\n\
      else:\n\
        for value in PT_STRINGS:\n\
          if (tmpName[-len(value):] == value):\n\
            PointFiles.append(f)\n\
  if (len(PointFiles) > 0):\n\
    _PointFile.SetValue(0, os.path.join(filepath,PointFiles[0]) )\n\
  else:\n\
    _PointFile.SetValue(0,\"\")\n\
tf = FILENAME\n\
findStartDate(tf)\n\
findOffsetNumber(tf)\n\
findPointFile(tf)\n\
";  
  this->SetInformationScript( requestInfo.c_str() );
  }

// --------------------------------------
// Destructor
vtkEPSReader::~vtkEPSReader()
  {
  this->SetFileName(0);
  this->SetDate(0);
  
  this->PyOffset->Delete();
  this->PyDate->Delete();
  this->PyPointFile->Delete();
  }

//----------------------------------------------------------------------------
int vtkEPSReader::RequestInformation(
  vtkInformation*, 
  vtkInformationVector**, 
  vtkInformationVector* outputVector)
{
 
  int result = 1;         
  if (this->Date)
    {
    result = vtkPythonProgrammableFilter::RequestInformation(NULL,NULL,outputVector);
  	this->PyDate->SetValue(1, this->GetDate() );
  	}
    
  return result;
}

//----------------------------------------------------------------------------
vtkStdString  vtkEPSReader::CreateReference(const char* funcname)
{

  //this needs to be cleaned up
  vtkStdString runscript = "FILENAME = '";
  runscript += this->GetFileName();
  runscript +="'\n";
      
  //set the memory references for the offset, Date, and PointFileName
  char addrofOffset[1024];
  sprintf(addrofOffset, "%p", this->PyOffset);
  char *memref = addrofOffset;
  if ((addrofOffset[0] == '0') && 
      ((addrofOffset[1] == 'x') || addrofOffset[1] == 'X'))
    {
    memref += 2; //skip over "0x"
    }
  runscript += "_Offset = vtk.vtkDoubleArray('";
  runscript += memref;
  runscript += "')\n";
  
  char addrofDate[1024];
  sprintf(addrofDate, "%p", this->PyDate);
  memref = addrofDate;
  if ((addrofDate[0] == '0') && 
       ((addrofDate[1] == 'x') || addrofDate[1] == 'X'))
     {
     memref += 2; //skip over "0x"
     }
  runscript += "_Date= vtk.vtkStringArray('";
  runscript += memref;
  runscript += "')\n";
   
  char addrofFile[1024];
  sprintf(addrofFile, "%p", this->PyPointFile);
  memref = addrofFile;
  if ((addrofFile[0] == '0') && 
      ((addrofFile[1] == 'x') || addrofFile[1] == 'X'))
    {
    memref += 2; //skip over "0x"
    }
  runscript += "_PointFile = vtk.vtkStringArray('";
  runscript += memref;
  runscript += "')\n";
  
  // Set self to point to this
  char addrofthis[1024];
  sprintf(addrofthis, "%p", this);    
  char *aplus = addrofthis; 
  if ((addrofthis[0] == '0') && 
      ((addrofthis[1] == 'x') || addrofthis[1] == 'X'))
    {
    aplus += 2; //skip over "0x"
    }
  runscript += funcname;
  runscript += "(vtk.vtkProgrammableFilter('";
  runscript += aplus;
  runscript += "'))\n";  

  return runscript; 
}

// --------------------------------------
void vtkEPSReader::PrintSelf(ostream& os, vtkIndent indent)
  {
  this->Superclass::PrintSelf(os,indent);
  os << indent << "FileName: "
     << (this->FileName ? this->FileName : "(none)") << "\n";
  
  os <<indent << "Date:"
      <<(this->GetDate() ? this->GetDate() : "(none)") << "\n";
  }


