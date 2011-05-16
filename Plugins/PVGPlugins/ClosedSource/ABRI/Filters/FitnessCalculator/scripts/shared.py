import math
import imp

class Shared(object):

  #####################################################################################
  def __init__(self, input, regularB, kdtree, result, xmin, xmax, ymin, ymax, zmin, zmax, dx, dy, dz, pointTable, d1, d2, d3, geoTechArray, AccessPos , densityArray ):
    self.input = input
    self.regularB = regularB
    self.kdtree = kdtree
    self.result = result
    self.xmin = xmin
    self.xmax = xmax
    self.ymin = ymin
    self.ymax = ymax
    self.zmin = zmin
    self.zmax = zmax
    self.dx = dx      # block delta x
    self.dy = dy      # block delta y
    self.dz = dz      # block delta z
    
    self.pointTable = pointTable    # internal use
    
    self.d1 = d1    # number of blocks in x direction
    self.d2 = d2    # number of blocks in y direction
    self.d3 = d3    # number of blocks in z direction
    
    self.pattern = {}       # internal use
    self.distances = []     # internal use

    self.geoTechArray = geoTechArray
    self.AccessPos = AccessPos
    self.densityArray = densityArray

    



  #####################################################################################
  """
  Given the indices (i,j,k) of 2 points,
  this method compute the distance between them
  """
  def distanceBetween2Points(self, i1, j1, k1, i2, j2, k2):
    x1 = i1*self.dx
    y1 = j1*self.dy
    z1 = k1*self.dz
    
    x2 = i2*self.dx
    y2 = j2*self.dy
    z2 = k2*self.dz

    return math.sqrt( pow((x2-x1),2) + pow((y2-y1),2) + pow((z2-z1),2) )




  #####################################################################################
  #
  # Given (x, y, z) of a point and the radius of the search area,
  # this method will find the neighbors of this point in the search
  # area and return them in a list. The list will contain the point id
  # and the distance for each neighbor. this list will look like this:
  # [ (id1, d1) , (id2, d2) , ... , (idn, dn) ] where:
  # idi = id of neighbor i
  # di = distance between the block and the neighbor i
  #####################################################################################
  def getPointNeighborsWithinRadius(self, x, y , z , radius):
    neighbors = []
    if(self.regularB == 1):
      if(not(radius in self.pattern)):
        self.findPattern(radius)
      i = int((x - self.xmin)/self.dx)
      j = int((y - self.ymin)/self.dy)
      k = int((z - self.zmin)/self.dz)
                          
      patternLength = len(self.pattern[radius])
      for count in range(0,patternLength):
        pointI = i+self.pattern[radius][count][0]
        pointJ = j+self.pattern[radius][count][1]
        pointK = k+self.pattern[radius][count][2]
                                  
        if((pointI >= 0) and (pointI < self.d1) and (pointJ >= 0) and (pointJ < self.d2) and (pointK >= 0) and (pointK < self.d3)):
           pointID = self.pointTable[pointI][pointJ][pointK]
           if not(pointID == -1):
             neighbors.append((pointID,self.distances[count]))
    else:
      currentPoint = [x, y, z]
      self.kdtree.FindPointsWithinRadius(radius, currentPoint, self.result)
      n = self.result.GetNumberOfIds()
      for id in range(0, n):
        PointId =  self.result.GetId(id)
        coord = self.input.GetPoint(PointId)
	if( (coord[0] != x) and (coord[1] != y) and (coord[2] != z)):
          distance = self.distanceBetween2Points(x, y, z, coord[0], coord[1], coord[2])			
	  neighbors.append((PointId,distance))
	self.result.DeleteId(PointId)
    
    return neighbors
	
  #####################################################################################
  #
  # Given a radius, this method will find the pattern of the neighborhood
  # in the search area
  #####################################################################################
  def findPattern(self,radius):
    if(radius in self.pattern):
      return
    self.pattern[radius] = []
    length = self.xmax - self.xmin
    width = self.ymax - self.ymin
    height = self.zmax - self.zmin
    centerI = (int)(length/(2*self.dx))
    centerJ = (int)(width/(2*self.dy))
    centerK = (int)(height/(2*self.dz))
	
    di = (int)(radius/self.dx)
    dj = (int)(radius/self.dy)
    dk = (int)(radius/self.dz)

	
    iCounterMin = centerI - di
    jCounterMin = centerJ - dj
    kCounterMin = centerK - dk
    if(iCounterMin < 0):
      iCounterMin = 0
    if(jCounterMin < 0):
      jCounterMin = 0
    if(kCounterMin < 0):
      kCounterMin = 0

    iCounterMax = centerI + di + 1
    if(iCounterMax > self.d1):
      iCounterMax = self.d1
    jCounterMax = centerJ + dj + 1
    if(jCounterMax > self.d2):
      jCounterMax = self.d2
    kCounterMax = centerK + dk + 1
    if(kCounterMax > self.d3):
      kCounterMax = self.d3
	  
    for count1 in range(iCounterMin, iCounterMax):
      for count2 in range(jCounterMin, jCounterMax):
        for count3 in range(kCounterMin, kCounterMax):
          iD = self.pointTable[count1][count2][count3]
          distance = self.distanceBetween2Points(centerI, centerJ, centerK, count1, count2, count3)
          if(distance <= radius):
            self.pattern[radius].append((count1 - centerI,count2 - centerJ,count3 - centerK))
            self.distances.append(distance)
    self.pattern[radius].remove((0,0,0))
    self.distances.remove(0)

  #######################################################################################
  # This function will get a point property from the input Block Model
  # To use this property and get the value of a block with a given id, use:
  # property.GetValue(id)
  #######################################################################################
  def getPropertyValue(self, name, iD):
    nameParts = name.split();
    formattedName = ""
    for part in nameParts:
      formattedName += part+ " "
    formattedName = formattedName[:-1]
    pointData = self.input.GetPointData()
    for i in range(0, pointData.GetNumberOfArrays()):
      propNameParts = pointData.GetArrayName(i).split()
      propName = ""
      for part in propNameParts:
        propName += part + " "
      propName = propName[:-1]
      if(formattedName == propName):
         return pointData.GetArray(i).GetValue(iD)
    
    





    
