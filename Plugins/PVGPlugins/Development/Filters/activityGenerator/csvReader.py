import csv
output = self.GetOutput()
#Reading the triangle file
triangleFile = file("C:\\ProjParaview\\trunk\\Mirarco\\activityGenerator\\Nehme\\FromSSA\\OZ310709\\phb8m1tr.csv")

cells = vtk.vtkCellArray() #dynamic array
points = vtk.vtkPoints()
group = vtk.vtkIntArray()
surface = vtk.vtkIntArray()
result = vtk.vtkIntArray()
colour = vtk.vtkIntArray()
ijk = vtk.vtkIntArray()
valcut = vtk.vtkIntArray()
stope = vtk.vtkIntArray()
value = vtk.vtkFloatArray()
revfac = vtk.vtkIntArray()
covfac = vtk.vtkIntArray()

group.SetName("group")
surface.SetName("surface")
result.SetName("result")
colour.SetName("colour")
ijk.SetName("ijk")
valcut.SetName("valcut")
stope.SetName("stope")
value.SetName("value")
revfac.SetName("revfac")
covfac.SetName("covfac")


for count,line in enumerate(triangleFile):
  try:
    if(count == 0):
        continue
    lineSplit = line.split(',')  
    PID1 = int(lineSplit[3])
    PID2 = int(lineSplit[4])
    PID3 = int(lineSplit[5])
    
    gr = int(lineSplit[0])
    surf = int(lineSplit[1])
    res = int(lineSplit[6])
    col = int(lineSplit[7])
    ij = int(lineSplit[8])
    valc = int(lineSplit[9])
    stp = int(lineSplit[10])
    val = float(lineSplit[11])
    rev = int(lineSplit[12])
    cov = int(lineSplit[13])
    
  except ValueError:
    print "Cannot display line number ",str(count)
  else:
    cells.InsertNextCell(3) # triangle has 3 points
    cells.InsertCellPoint(PID1-1)
    cells.InsertCellPoint(PID2-1)
    cells.InsertCellPoint(PID3-1)
    
    group.InsertValue(count-1,gr)
    surface.InsertValue(count-1,surf)
    result.InsertValue(count-1,res)
    colour.InsertValue(count-1,col)
    ijk.InsertValue(count-1,ij)
    valcut.InsertValue(count-1,valc)
    stope.InsertValue(count-1,stp)
    value.InsertValue(count-1,val)
    revfac.InsertValue(count-1,rev)
    covfac.InsertValue(count-1,cov)

    
# Reading the points file
pointFile = file("C:\\ProjParaview\\trunk\\Mirarco\\activityGenerator\\Nehme\\FromSSA\\OZ310709\\phb8m1pt.csv")

#points.InsertPoint(0,0,0,0)
for count,line in enumerate(pointFile):
  try:
    if(count == 0):
        continue      
    lineSplit = line.split(',')
    #ID = int(line['PID'])
    x = float(lineSplit[2])
    y = float(lineSplit[3])
    z = float(lineSplit[4])
  except ValueError:
    print "Cannot display line number ",str(count)
  else:
    points.InsertNextPoint(x,y,z)

output.SetPoints(points)
output.SetPolys(cells)
output.GetCellData().AddArray(group)
output.GetCellData().AddArray(surface)
output.GetCellData().AddArray(result)
output.GetCellData().AddArray(colour)
output.GetCellData().AddArray(ijk)
output.GetCellData().AddArray(valcut)
output.GetCellData().AddArray(stope)
output.GetCellData().AddArray(value)
output.GetCellData().AddArray(revfac)
output.GetCellData().AddArray(covfac)

triangleFile.close()
pointFile.close()
