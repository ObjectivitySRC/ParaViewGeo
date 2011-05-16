import time
import os
#need to parse the FileName to gather the proper Point Name, and also
#figure out the default start time

def findStartDate(triangleFilePath):
  START_DATE="STARTDATE"
  FILE_FORMAT="%Y-%m-%d"
  OUTPUT_FORMAT="%Y-%m-%dT%H:%M:%S"  
  
  DEFAULT_START_TIME="2008-01-01T00:00:01"
  #determine what the startdate of the file is
  try:
    triangleFile = file(triangleFilePath)
    #make sure the file exists, and has a line
    startDateRaw = triangleFile.next()
    startDateRaw = startDateRaw.upper()    
    if ( startDateRaw.find(START_DATE) >= 0 ):
      #need to parse the line with string to date
      raw = startDateRaw.split(START_DATE)[1].strip()
      raw = raw[1:] #skip the seperator
      tempDate = time.strptime(raw,FILE_FORMAT)
      #proper format is "2008-01-01T00:00:00"
    else:
      #the first line does not contain a startdate, use the default
      tempDate = time.strptime(DEFAULT_START_TIME,OUTPUT_FORMAT)
  except ValueError:
    #set it to a default date
    date = DEFAULT_START_TIME
  else:
    date =  time.strftime(OUTPUT_FORMAT,tempDate)
  _Date.SetValue(0,date)
  #now hack it by adding it as field data to the output object

    
  

def findOffsetNumber(triangleFilePath):
  DEFAULT_OFFSET=604800.0
  OFFSET = "OFFSET"
  offset = DEFAULT_OFFSET
  #find the offset value, convert to double and set that for the class
  try:
    triangleFile = file(triangleFilePath)
    triangleFile.next()
    offsetLine = triangleFile.next() #skip down to the offset line
    offsetLine = offsetLine.upper()
    if(offsetLine.find(OFFSET) >= 0):
      raw = offsetLine.split(OFFSET)[1].strip()
      offset = float(raw[1:]) #convert the number to a float, and strip the seperator
  except:
    pass #use default
    #now hack it by adding it as field data to the output object
  _Offset.SetValue(0,offset)
    

def findPointFile(triangleFilePath):
  goodExts = [".csv",".sept",".setr"]
  #need to figure out what the point file name is
  
  (filepath,filename) = os.path.split(triangleFilePath)
  (name, ext) = os.path.splitext(filename)
  allFiles = os.listdir(filepath)
  
  #we need to check if the file name has a tr at the end
  #this is important for file type checking
  TR=False
  TR_STRING = "tr"
  PT_STRINGS = ["pt", "pts"]
  TRSIZE=2
  if (name[-TRSIZE:].lower() == TR_STRING):
    TR=True
  
  PointFiles = list()
  name = name.lower() #we need to make this lower to make sure all the compares will work correctly
  for f in allFiles:
    (tmpName, tmpExt) = os.path.splitext(f)
    tmpName = tmpName.lower()
    
    #this is the case of same ext, but the pt/tr on the name is the identifier
    if (tmpExt.lower() in goodExts):
      if TR:      
        #work around for pt and pts in the name
        for value in PT_STRINGS:
          size = len(value)        
          if tmpName[-size:] == value and tmpName[:-size] == name[:-TRSIZE]:
            PointFiles.insert(0,f)
      elif not (tmpExt == ext) and tmpName == name:
        #the names are the same, but not the extensions, so It is our next best guess            
        PointFiles.append(f)
      else:
        for value in PT_STRINGS:
          if (tmpName[-len(value):] == value):
            PointFiles.append(f)
        
  if (len(PointFiles) > 0):
    _PointFile.SetValue(0, os.path.join(filepath,PointFiles[0]) )
  else:
    _PointFile.SetValue(0,"")

tf = FILENAME
findStartDate(tf)
findOffsetNumber(tf)
findPointFile(tf)
