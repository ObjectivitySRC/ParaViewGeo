#include <GsTLAppli/gui/oinv_description/oinv_slice.h>
#include <GsTLAppli/gui/oinv_description/oinv_cgrid.h>
#include <GsTLAppli/gui/oinv_description/gstl_SoNode.h>
#include <GsTLAppli/gui/utils/colormap.h>
#include <GsTLAppli/appli/manager_repository.h>
#include <GsTLAppli/gui/appli/display_pref_panel.h>
#include <GsTLAppli/grid/library_grid_init.h>

#include <GsTLAppli/gui/appli/SoQtGsTLViewer.h>

#include <Inventor/Qt/SoQt.h>
#include <Inventor/Qt/viewers/SoQtExaminerViewer.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/nodes/SoCube.h>
#include <Inventor/nodes/SoMaterialBinding.h>
#include <Inventor/nodes/SoCoordinate3.h>
#include <Inventor/nodes/SoTransform.h>
#include <Inventor/nodes/SoQuadMesh.h>
#include <Inventor/nodes/SoSelection.h>

#include <qwidget.h>
#include <qcheckbox.h>

#include <fstream>


void print( const GsTLGridProperty* prop ) {
    for( int i=0; i< prop->size() ; i++ ) {
    if( prop->is_informed( i ) )
      cout << prop->get_value( i ) << endl;
    else
      cout << "-99" << endl;
      }
}





int main(int argc, char **argv) {
  // Initialize Inventor and Qt
  QWidget *myWindow = SoQt::init(argv[0]);
  GsTL_SoNode::initClass();
  if (myWindow == NULL) exit(1);
  
  // Create a Manager for colormaps
  SmartPtr<Named_interface> ni_cmaps = 
    Root::instance()->new_interface("directory://colormaps",
				    colormap_manager );
  Manager* dir = dynamic_cast<Manager*>( ni_cmaps.raw_ptr() );
    
  if( !dir ) {
    GsTLlog << "could not create directory " 
	      << colormap_manager << "\n";
    return 1;
  }
  dir->factory( "colormap", Color_scale::create_new_interface );
  Root::instance()->new_interface( "colormap://rainbow.map", 
				   colormap_manager + "/rainbow" ); 
  Root::instance()->new_interface( "colormap://cyan_red.map", 
				   colormap_manager + "/cyan_red" ); 


// Create a Manager grid-objects
  library_grid_init::init_lib();
  Root::instance()->list_all( cout );



  //--------------------

  int nx=5;
  int ny=3;
  int nz=6;

  Cartesian_grid grid;
  grid.set_dimensions( nx, ny, nz, 1, 1, 1 );
  grid.origin( GsTLPoint( -0.5, -0.5, -0.5 ) );
  GsTLGridProperty* prop = grid.add_property( "toto" );
  for( int i=0; i < nx*ny*nz; i ++ ) 
    prop->set_value( i, i );

  cout << "property values:" << endl;
  print( prop );

  cout << "swaping to disk... New property values:" << endl;
  prop->swap_to_disk();
  print( prop );

  cout << "swaping to memory... New property values:" << endl;
  prop->swap_to_memory();
  for( int i =0; i< prop->size(); i++ )
    cout << prop->get_value( i ) << endl;
  cout << endl;

  /*
  Oinv_cgrid oinv_grid;
  oinv_grid.init( &grid );
  oinv_grid.set_property( "toto" );
  oinv_grid.oinv_node()->visible = true;
  cout << "Setting up oinv scene" << endl;
  
  SoSelection* root = new SoSelection;
  root->addChild( oinv_grid.oinv_node() );

  Display_pref_panel* controls =
    new Display_pref_panel( &oinv_grid, 0, 0 );

  SoQtGsTLViewer* myViewer = new SoQtGsTLViewer( (QWidget*) myWindow, "camera" );
  //SoQtExaminerViewer *myViewer = new SoQtExaminerViewer(myWindow);
  myViewer->setSceneGraph(root);
  myViewer->show();
  controls->show();
  SoQt::show(myWindow);
  
  SoQt::mainLoop();
  
  */
}
