import math

class Helper(object):

  #####################################################################################
  def __init__(self, input, xmin, xmax, ymin, ymax, zmin, zmax, dx, dy, dz, pointTable, d1, d2, d3, auArray ):
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
    
    self.auArray = auArray

  #####################################################################################
  # This function will compute the fitness of a block with a given id.
  #####################################################################################
  def computeFitness(self, iD):
    coord = self.input.GetPoint(iD)  # coord = [x, y, z] of the block with the given id
                                     # coord[0] = x ; coord[1] = y; coord[2] = z
    radius = 10
    neighbors = self.getPointNeighborsWithinRadius(coord[0], coord[1], coord[2], radius)
    fitness = (self.auArray.GetValue(iD)) * ( 1 + (  ( (coord[2] - self.zmin)/self.dz )/self.d3  ) )

    # neighbors = [ (id1, d1) , (id2, d2) , ... , (idn, dn) ] where:
    # idi = id of neighbor i
    # di = distance between the block and the neighbor i
    # in the following for loop, each iteration, point will be equal to [idi, di]
    for point in neighbors:
      fitness += self.auArray.GetValue(point[0]) * radius / point[1] 

    return fitness
      
    





    
