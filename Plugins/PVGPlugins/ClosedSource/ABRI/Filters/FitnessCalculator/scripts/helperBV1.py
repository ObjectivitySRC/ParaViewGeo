import math
import shared

class Helper(object):

  #####################################################################################
  def __init__(self, input, xmin, xmax, ymin, ymax, zmin, zmax, dx, dy, dz, pointTable, d1, d2, d3, geoTechArray, AccessPos , densityArray ):
    self.pointTable = pointTable    # internal use
    self.densityArray = densityArray
    self.shared = shared.Shared(input, xmin, xmax, ymin, ymax, zmin, zmax, dx, dy, dz, pointTable, d1, d2, d3, geoTechArray, AccessPos , densityArray)

 
  #####################################################################################
  # This function will compute the fitness of a block with a given id.
  #####################################################################################
  def computeFitness(self, iD):
    BV1 = ((self.shared.getPropertyValue("CU",iD)*66.0) + (self.shared.getPropertyValue("AU",iD)*2.2)) *self.shared.getPropertyValue(self.densityArray,iD)

    return BV1
    
      
    





    
