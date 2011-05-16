import csv
output = self.GetOutput()
#Reading the triangle file
File = file("E:\mirarco\ActivityGenerator\inputData\ActGenImports\dataFromMarcus2008Sept19\PHB8MSP.csv")

cells = vtk.vtkCellArray() #dynamic array
points = vtk.vtkPoints()


for count,line in enumerate(File):
  try:
    if(count == 0):
        continue      
    lineSplit = line.split(',')
    #ID = int(line['PID'])
    x = float(lineSplit[1])
    y = float(lineSplit[2])
    z = float(lineSplit[3])
  except ValueError:
    print "Cannot display line number ",str(count)
  else:
    points.InsertNextPoint(x,y,z)
    cells.InsertNextCell(1)
    cells.InsertCellPoint(count-1)

output.SetPoints(points)
output.SetVerts(cells)
File.close()
