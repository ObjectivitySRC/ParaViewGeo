import math
import imp
import time
 
startTime = time.time()
minimumAu = 0


#outputFitnessArrayName = "Fitness"
dx = BlockSizeX
dy = BlockSizeY
dz = BlockSizeZ
regularB = regularBlock
#print "density: ",densityInput
#return
functionsList = []
i = 1
var = ""
while(1):
  try:
    var = locals()['function'+str(i)]
  except KeyError:
    break
  else:
    functionsList.append(var)
    i = i+1

#print "number: ",len(functionsList)
if(len(functionsList) == 0):
  return


input = self.GetPolyDataInput()
output = self.GetPolyDataOutput()
numberOfPoints = input.GetNumberOfCells()
numberOfCells = input.GetNumberOfPoints()

kdtree = vtk.vtkKdTree()
kdtree.Initialize()
kdtree.BuildLocatorFromPoints(input)

result = vtk.vtkIdList()

fitnessArrays = []
for i in range(0,len(functionsList)):
  fitnessArrays.append(vtk.vtkDoubleArray())
  fitnessArrays[i].SetName("Fitness"+str(i+1))

newPoints = vtk.vtkPoints()
newCells = vtk.vtkCellArray()

bounds = input.GetBounds()
xmin = bounds[0]
xmax = bounds[1]
ymin = bounds[2]
ymax = bounds[3]
zmin = bounds[4]
zmax = bounds[5]

pointTable = []

d1 = int((xmax - xmin)/dx) + 1
d2 = int((ymax - ymin)/dy) + 1
d3 = int((zmax - zmin)/dz) + 1

importer = imp.load_source('helper',helperFile)

geoTechArray = input.GetPointData().GetArray(geotechField)
helper = importer.Helper(input, regularB, kdtree, result, xmin, xmax, ymin, ymax, zmin, zmax, dx, dy, dz, pointTable, d1, d2, d3, geoTechArray, AccessPos, densityInput)

initialValue = -1
for i in range(0,d1):
  liste1 = []
  for j in range(0,d2):
    liste2 = []
    for k in range(0,d3):
      liste2.append(-1)
    liste1.append(liste2)
  helper.pointTable.append(liste1)
for count in range(0,numberOfPoints):
  coord = input.GetPoint(count)
  i = int((coord[0] - xmin)/dx)
  j = int((coord[1] - ymin)/dy)
  k = int((coord[2] - zmin)/dz)
  helper.pointTable[i][j][k] = count

	
#newPoints = vtk.vtkPoints()
newVerts = vtk.vtkCellArray()

data = input.GetPointData()

			
for count in range(0, numberOfPoints):
  newVerts.InsertNextCell(1)
  newVerts.InsertCellPoint(count)
  for i in range(0,len(functionsList)):
    function = getattr(helper,functionsList[i])
    #print "count: ",count
    fitness = function(count)
    fitnessArrays[i].InsertNextValue(fitness)

output.SetPoints(input.GetPoints())
output.SetVerts(newVerts)
for i in range(0, len(functionsList)):
  output.GetPointData().AddArray(fitnessArrays[i])
 
return
