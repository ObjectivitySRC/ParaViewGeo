import csv
from datetime import datetime
import time

DATE_FORMAT="%Y-%m-%dT%H:%M:%S"
OFFSET = float(_Offset.GetValue(0))
DATE = _Date.GetValue(0)
NDate = _Date.GetValue(1)
if (NDate):
  DATE = NDate
POINTFILENAME = _PointFile.GetValue(0)

def ReadPointFile(pointFile):
  #we have finnished reading the triangle file, now onto the point file
  points = vtk.vtkPoints()
  try:
    pointCSV = csv.reader(pointFile)
    #need a parsed header line
    headerLine = pointCSV.next()
    
    #add the lowercase/trimmed lines to the header list
    header = list()
    for key in headerLine:
      header.append(key.lower().strip())
    
    #create a dictonary of each line in the file
    parseFile = csv.DictReader(pointFile,header)
    #have to add a point at zero since the eps files start at point index 1
    points.InsertNextPoint(0,0,0)
    for line in parseFile:
      try:
        x = float(line['xp'])
        y = float(line['yp'])
        z = float(line['zp'])          
      except:
        #ignore bad lines
        pass          
      else:          
        points.InsertNextPoint(x,y,z)
  except:
    print 'Failed to parse point file'
    return None
  else:
    return points
def ReadTriangleFile(triangleFile):
  #attempt to read the file
  cells = vtk.vtkCellArray()
  properties = dict()
  try:
    triangleFile.next()
    triangleFile.next()    
    triangleCSV = csv.reader(triangleFile)
    #need a parsed header line
    headerLine = triangleCSV.next()
    
    #add the lowercase/trimmed lines to the header list
    header = list()
    for key in headerLine:
      header.append(key.lower().strip())
    
    #create a dictonary of each line in the file
    parseFile = csv.DictReader(triangleFile,header)

    TRI = 3 #number of points in a triangle    
    triKeys = set(['pid1', 'pid2', 'pid3'])   #keys used in the csv to be point id's
    
    #keys for time component of the file
    startKey = "start"
    endKey = "duration"
    
    for line in parseFile:
      #add the cell for that line, plus the properties      
      cells.InsertNextCell(TRI)
      for key in line.keys():
        if (key in triKeys):
          p = int ( line[ key ] )    
          cells.InsertCellPoint(p)
        elif startKey == key:
          #add the start and end value properties
          addTimeValues(properties,startKey,endKey,line[startKey],line[endKey])
        elif not (endKey == key): #we need to ignore the end key, as it is handled in startkey 
          addProperty(properties,key, line[key])
  except ValueError:
    print 'failed to load triangle File'
    return None,None
  else:
    return cells,properties
  


def addTimeValues(properties,startKey,endKey,startValue,endValue):  
  addProperty(properties,startKey,startValue)
  #endValue is in seconds, divide by the offset to get the number of timeblocks
  duration = float(endValue)/OFFSET
  addProperty(properties,endKey, duration)
  
  #I now need to use the DATE and the start and end dates to add a property
  #that is in unix seconds that is equal to the start and end absolute times
  absStart = "JulianTime Start"
  absEnd = "JulianTime End"
  
  startOffset = float(startValue) * OFFSET
    
  #get the start date as a datetime object
  startDate = datetime.strptime(DATE,DATE_FORMAT)
  
  #need to make seconds equal to the absolute start time + the stopes start offset
  seconds = startDate.second + startOffset
  julianStart = findJulianTime(startDate.year,startDate.month, startDate.day, startDate.hour, startDate.minute, seconds)
  
  seconds += float(endValue)    
  julianEnd = findJulianTime(startDate.year,startDate.month, startDate.day, startDate.hour, startDate.minute, seconds)
  addProperty(properties, absStart, julianStart)
  addProperty(properties, absEnd, julianEnd)
  

def findJulianTime(year,month,day,hour,min,sec):  
  UT=hour+min/60+sec/3600
  total_seconds=hour*3600+min*60+sec
  fracday=total_seconds/86400
  
  # print dd,month,year, hour,min,sec, UT  
  if (100*year+month-190002.5)>0:
      sig=1
  else:
      sig=-1  
  JD = 367*year - int(7*(year+int((month+9)/12))/4) + int(275*month/9) + day + 1721013.5 + UT/24 - 0.5*sig +0.5  
  return JD

def addProperty(properties,key,value):
  if not (key in properties):
    #property does not exist, add it
    try:
      prop = float(value)
    except:
      #has to be a string
      prop = value
      properties[key]= vtk.vtkStringArray()
    else:
      #has to be a float
      properties[key] = vtk.vtkDoubleArray()
    
    #set the name
    properties[key].SetName(key)        
    properties[key].InsertNextValue(prop)
  else:
    try:
      prop = float(value)
    except:
      prop = value          
    properties[key].InsertNextValue(prop)        




#get output reference
output = self.GetPolyDataOutput()
#grab files

try:  
  triangleFile = file( FILENAME )
  pointFile = file( POINTFILENAME )
except:
  print "failed to load data, since one of the paths was incorrect"
else:
  #try to load the Triangle file
  cells,properties = ReadTriangleFile(triangleFile)  
  
  points = ReadPointFile(pointFile)  
  if (cells and points):
    #also read properties in
    if (properties):
      for prop in properties.values():
        output.GetCellData().AddArray(prop)        
    output.SetPoints(points)
    output.SetPolys(cells)
          
  triangleFile.close()
  pointFile.close()
 