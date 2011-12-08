from paraview.simple import *
import SMPythonTesting

import sys
import os
import os.path

SMPythonTesting.ProcessCommandLineArguments()

xmf_file = os.path.join(SMPythonTesting.DataDir, "Data/poisson_3dall_2.xmf")
OpenDataFile(xmf_file)
MergeBlocks()
Show()
view = Render()

view.CameraViewUp = [0.40869219753099151, 0.77141145522009946, -0.48774486612623352]
view.CameraPosition = [-1.1388262528701965, 2.72641269058122, 2.6480590031133158]
view.CameraClippingRange = [1.7569458900016599, 5.7047917549472977]
view.CameraFocalPoint = [0.49999999999999989, 0.49999999999999978, 0.49999999999999989]
view.CameraParallelScale = 0.90892941461412635
view.CenterOfRotation = [0.5, 0.5, 0.5]

Render()

if not SMPythonTesting.DoRegressionTesting(view.SMProxy):
    raise SMPythonTesting.TestError, 'Test failed.'
