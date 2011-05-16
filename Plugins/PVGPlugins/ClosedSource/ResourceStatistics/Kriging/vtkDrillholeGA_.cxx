#include "vtkDrillholeGA.h"


#define LIB_STATIC
#include <vtk_gstl.h>
//#include "GsTLAppli/geostat/kriging.h"

#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkObjectFactory.h"
#include "vtkPolyDataAlgorithm.h"
#include "vtkPolyData.h"
#include "vtkMath.h"




vtkCxxRevisionMacro(vtkDrillholeGA, "$Revision: 1.0 $");
vtkStandardNewMacro(vtkDrillholeGA);

vtkDrillholeGA::vtkDrillholeGA()
  {
  this->SetNumberOfInputPorts(1);
  this->SetNumberOfOutputPorts(1);
  }

vtkDrillholeGA::~vtkDrillholeGA()
  {
  }

int vtkDrillholeGA::RequestData(vtkInformation *request, vtkInformationVector **inputVector, vtkInformationVector *outputVector)
  {
  //Kriging *k = new Kriging();

  /*char *argv[2];
  argv[0] = "arg1";
  argv[1] = "arg2";
  this->FakeMain(2, argv);*/
  return 1;
  }

int vtkDrillholeGA::FakeMain(int argc, char **argv)
  {
  //blah
  
  //Lib_initializer::minimal_init();
  //Lib_initializer::load_geostat_algos();
  //Lib_initializer::load_filters_plugins();


  return 1;
  }


void vtkDrillholeGA::PrintSelf(ostream& os, vtkIndent indent)
  {
  this->Superclass::PrintSelf(os, indent);
  }

