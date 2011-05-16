import math
import shared

class Helper(object):

  #####################################################################################
  def __init__(self, input, regularB, kdtree, result, xmin, xmax, ymin, ymax, zmin, zmax, dx, dy, dz, pointTable, d1, d2, d3, geoTechArray, AccessPos , densityArray):
    self.input = input    
    self.pointTable = pointTable    # internal use
    self.densityArray = densityArray
    self.shared = shared.Shared(input, regularB, kdtree, result, xmin, xmax, ymin, ymax, zmin, zmax, dx, dy, dz, pointTable, d1, d2, d3, geoTechArray, AccessPos , densityArray)


    

  #####################################################################################
  # This function will compute the fitness of a block with a given id.
  #####################################################################################
  def computeFitness(self, iD):
    
    coord = self.input.GetPoint(iD)  # coord = [x, y, z] of the block with the given id
                                     # coord[0] = x ; coord[1] = y; coord[2] = z    
    
    BV1 = ((self.shared.getPropertyValue("Ni-Minus-All-Falc",iD)*99.0) + (self.shared.getPropertyValue("Cu-Minus-All-Falc",iD)*66.0) + (self.shared.getPropertyValue("Co-Minus All-Falc",iD)*2.2)) *self.shared.getPropertyValue(self.densityArray,iD)

    neighbors = self.shared.getPointNeighborsWithinRadius(coord[0], coord[1], coord[2], 8)
    average = 0.0
    for point in neighbors:
      average += ((self.shared.getPropertyValue("Ni-Minus-All-Falc",point[0])*99.0) + (self.shared.getPropertyValue("Cu-Minus-All-Falc",point[0])*66.0) + (self.shared.getPropertyValue("Co-Minus All-Falc",point[0])*2.2)) *self.shared.getPropertyValue(self.densityArray,point[0])

    numberOfNeighbors = len(neighbors)
    if(numberOfNeighbors > 0):  
      average = average/len(neighbors)
      
    BV2 = BV1 + 0.8*(average)  

    return BV2
    
      
    





    
