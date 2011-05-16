import math
import shared

class Helper(object):

  #####################################################################################
  def __init__(self, input, regularB, kdtree, result, xmin, xmax, ymin, ymax, zmin, zmax, dx, dy, dz, pointTable, d1, d2, d3, geoTechArray, AccessPos , densityArray):
    self.input = input
    self.zmax = zmax
    self.zmin = zmin
    
    self.pointTable = pointTable    # internal use

    self.geoTechArray = geoTechArray
    self.AccessPos = AccessPos
    self.densityArray = densityArray
    self.shared = shared.Shared(input, regularB, kdtree, result, xmin, xmax, ymin, ymax, zmin, zmax, dx, dy, dz, pointTable, d1, d2, d3, geoTechArray, AccessPos , densityArray)


  #####################################################################################
  # This function will compute the fitness of a block with a given id.
  #####################################################################################
  def computeFitness(self, iD):
    
    coord = self.input.GetPoint(iD)  # coord = [x, y, z] of the block with the given id
                                     # coord[0] = x ; coord[1] = y; coord[2] = z
    Ni = "Ni-Minus-All-Falc"
    Cu = "Cu-Minus-All-Falc"
    Co = "Co-Minus All-Falc"
    
    
    BV1 = ((self.shared.getPropertyValue(Ni,iD)*99.0) + (self.shared.getPropertyValue(Cu,iD)*66.0) + (self.shared.getPropertyValue(Co,iD)*2.2)) *self.shared.getPropertyValue(self.densityArray,iD)

    neighbors = self.shared.getPointNeighborsWithinRadius(coord[0], coord[1], coord[2], 8)
    average = 0.0
    for point in neighbors:
      average += ((self.shared.getPropertyValue(Ni,point[0])*99.0) + (self.shared.getPropertyValue(Cu,point[0])*66.0) + (self.shared.getPropertyValue(Co,point[0])*2.2)) *self.shared.getPropertyValue(self.densityArray,point[0])

    numberOfNeighbors = len(neighbors)
    if(numberOfNeighbors > 0):  
      average = average/len(neighbors)
      
    BV2 = BV1 + 0.8*(average)  

    neighbors = self.shared.getPointNeighborsWithinRadius(coord[0], coord[1], coord[2], 50)
    average = 0.0
    for point in neighbors:
      if(point[1] > 8):
        average += ((self.shared.getPropertyValue(Ni,point[0])*99.0) + (self.shared.getPropertyValue(Cu,point[0])*66.0) + (self.shared.getPropertyValue(Co,point[0])*2.2)) *self.shared.getPropertyValue(self.densityArray,point[0])

    numberOfNeighbors = len(neighbors)
    if(numberOfNeighbors > 0):
      average = average/len(neighbors)
    BV3 = BV2 + 0.2*(average)

    BV4 = BV3 * (  1 - ( (self.zmax - coord[2])/(self.zmax - self.zmin) )  )

    return BV4
    
      
    





    
