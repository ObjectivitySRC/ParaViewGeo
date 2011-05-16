#include <GsTLAppli/gui/oinv_description/oinv_slice.h>
#include <GsTLAppli/gui/oinv_description/oinv_cgrid.h>
#include <GsTLAppli/gui/utils/colormap.h>

#include <Inventor/Qt/SoQt.h>
#include <Inventor/Qt/viewers/SoQtExaminerViewer.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/nodes/SoCube.h>
#include <Inventor/nodes/SoMaterialBinding.h>
#include <Inventor/nodes/SoCoordinate3.h>
#include <Inventor/nodes/SoTransform.h>
#include <Inventor/nodes/SoQuadMesh.h>

#include <fstream>


static float coords[25][3] = 
  {
{0, 0, 0},
{0, 0, 0.1},
{0, 0, 0.2},
{0, 0, 0.3},
{0, 1, 0},
{0, 1, 1},
{0, 1, 2},
{0, 1, 3},
{0, 2, 0},
{0, 2, 1},
{0, 2, 2},
{0, 2, 3},
{0, 3, 0},
{0, 3, 1},
{0, 3, 2},
{0, 3, 3},
{0, 4, 0},
{0, 4, 1},
{0, 4, 2},
{0, 4, 3},
{0, 5, 0},
{0, 5, 1},
{0, 5, 2},
{0, 5, 3}
  };

int main(int argc, char **argv) {
  // Initialize Inventor and Qt
  QWidget *myWindow = SoQt::init(argv[0]);
  if (myWindow == NULL) exit(1);
  
  int nx=5;
  int ny=3;
  int nz=4;

  std::vector<RGB_color> block;
  Colormap cmap( "cmap.txt" );
  
  /*
  ifstream infile( "train.dat" );
  appli_assert( infile );

  cout << "Setting color block" << endl;

  cmap.set_bounds( 0, 1 );

  for( int i=0; i < nx*ny*nz; i++ ) {
    float value;
    infile >> value;
  
    block.push_back( cmap.color( value ) );
  }
  */
  cmap.set_bounds( 0, nx*ny*nz );
  for( int i=0; i < nx*ny*nz; i++ ) {
    
    block.push_back( cmap.color(i) );
  }
  
  cout << "Setting up oinv scene" << endl;
  SoSeparator* root = new SoSeparator;
  
  Oinv_slice_cgrid slice(nx,ny,nz,
			 block,
			 Oinv::Z_AXIS, 0,
			 1, 1, 1, 0.5, 0.5, 0.5);
 
  root->addChild( slice.oinv_node() );
  

  slice.property_display_mode( Oinv::PAINTED );
  
  /*
  Full_volume vol( nx,ny,nz,
		   block,
		   1, 1, 1,
		   0.5, 0.5, 0.5); 
  vol.property_display_mode( Oinv::PAINTED );
  root->addChild( vol.oinv_node() );
  */



  SoQtExaminerViewer *myViewer = new SoQtExaminerViewer(myWindow);
  myViewer->setSceneGraph(root);
  myViewer->show();
  SoQt::show(myWindow);
  SoQt::mainLoop();
  
  
}
