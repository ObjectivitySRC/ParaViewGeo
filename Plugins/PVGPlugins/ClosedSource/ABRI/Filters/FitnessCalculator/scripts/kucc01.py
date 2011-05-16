import math
import shared

class Helper(object):

  #####################################################################################
   def __init__(self, input, regularB, kdtree, result, xmin, xmax, ymin, ymax, zmin, zmax, dx, dy, dz, pointTable, d1, d2, d3, geoTechArray, AccessPos , densityArray):
    self.input = input
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
    self.shared = shared.Shared(input, regularB, kdtree, result, xmin, xmax, ymin, ymax, zmin, zmax, dx, dy, dz, pointTable, d1, d2, d3, geoTechArray, AccessPos , densityArray)


  #####################################################################################
  # This function will compute the fitness of a block with a given id.
  #####################################################################################
  def computeFitness1(self, iD):
    
    coord = self.input.GetPoint(iD)  # coord = [x, y, z] of the block with the given id
                                     # coord[0] = x ; coord[1] = y; coord[2] = z    
    BV1 = ((self.shared.getPropertyValue("AU",iD)*980.0) + (self.shared.getPropertyValue("AG",iD)*13.30) + (self.shared.getPropertyValue("CU",iD)*2.2)) *self.shared.getPropertyValue(self.densityArray,iD)

    return BV1
		
  def computeFitness2(self, iD):
    
    coord = self.input.GetPoint(iD)  # coord = [x, y, z] of the block with the given id
                                     # coord[0] = x ; coord[1] = y; coord[2] = z    
    BV1 = ((self.shared.getPropertyValue("AU",iD)*200.0) + (self.shared.getPropertyValue("AG",iD)*13.30) + (self.shared.getPropertyValue("CU",iD)*2.2)) *self.shared.getPropertyValue(self.densityArray,iD)

    return BV1
		
  def computeFitness3(self, iD):
    
    coord = self.input.GetPoint(iD)  # coord = [x, y, z] of the block with the given id
                                     # coord[0] = x ; coord[1] = y; coord[2] = z
    BV1 = ((self.shared.getPropertyValue("AU",iD)*100.0) + (self.shared.getPropertyValue("AG",iD)*13.30) + (self.shared.getPropertyValue("CU",iD)*2.2)) *self.shared.getPropertyValue(self.densityArray,iD)
		

    return BV1
		
  def computeFitness4(self, iD):
    
    coord = self.input.GetPoint(iD)  # coord = [x, y, z] of the block with the given id
                                     # coord[0] = x ; coord[1] = y; coord[2] = z
    BV1 = ((self.shared.getPropertyValue("AU",iD)*100.0) + (self.shared.getPropertyValue("AG",iD)*13.30) + (self.shared.getPropertyValue("CU",iD)*2.2)) *self.shared.getPropertyValue(self.densityArray,iD)

    return BV1
    
      
    





    
