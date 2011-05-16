#include <GsTLAppli/gui/oinv_description/oinv_pointset.h>
#include <GsTLAppli/gui/oinv_description/gstl_SoNode.h>
#include <GsTLAppli/gui/utils/colormap.h>
#include <GsTLAppli/appli/manager_repository.h>
#include <GsTLAppli/gui/appli/display_pref_panel.h>
#include <GsTLAppli/grid/grid_model/point_set.h>
#include <GsTLAppli/filters/gslib/gslib_filter.h>
#include <GsTLAppli/filters/library_filters_init.h>
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
#include <Inventor/nodes/SoPointSet.h>
#include <Inventor/nodes/SoDrawStyle.h>

#include <qwidget.h>
#include <qcheckbox.h>

#include <fstream>
#include <stdlib.h>



int main(int argc, char **argv) {
  // Initialize Inventor and Qt
  QWidget *myWindow = SoQt::init(argv[0]);
  GsTL_SoNode::initClass();
  if (myWindow == NULL) exit(1);
  
  library_filters_init::init_lib();
  library_grid_init::init_lib();

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

  Gslib_infilter* filter = new Gslib_infilter;

  cout << "filter initialized" << endl;

  ifstream infile( "cluster.dat" );
  if( !infile ) {
    cerr << "Can't open cluster.dat" << endl;
    return 1;
  }
  cout << "reading file" << endl;

  SmartPtr<Geostat_grid> pset_sp = filter->read( infile );
  Geostat_grid* pset = pset_sp.raw_ptr();
  Root::instance()->list_all( cerr );

  cout << "number of references " << pset->references() << endl;
  
  
  Oinv_pointset oinv_pset;
  oinv_pset.init( pset );
  oinv_pset.set_property( "Primary" );
  oinv_pset.oinv_node()->visible = true;

  cout << "Setting up oinv scene" << endl;
  
  SoSelection* root = new SoSelection;

  /*
  //================================
  
  Point_set* grid_ = dynamic_cast<Point_set*>( pset );
  SoCoordinate3* coords_node = new SoCoordinate3;
  const std::vector<Point_set::location_type>& locations =
    grid_->point_locations();
  
  float (*coords)[3] = new float[ locations.size() ][3];
  for( int i=0; i< int( locations.size() ); i++ ) {
    coords[i][0] = locations[i].x();
    coords[i][1] = locations[i].y();
    coords[i][2] = locations[i].z();
  }
  coords_node->point.setValues(0, locations.size(), coords); 
  root->addChild( coords_node );
  delete [] coords;

  float (*colors)[3] = new float[ locations.size() ][3];
  for( int i=0; i< int( locations.size() ); i++ ) {
    colors[i][0] = drand48();
    colors[i][1] = drand48();
    colors[i][2] = drand48();
  }
  
  SoMaterialBinding* bind = new SoMaterialBinding;
  bind->value = SoMaterialBinding::PER_PART;

  SoMaterial* material = new SoMaterial;
  material->diffuseColor.setValues( 0, locations.size(), colors );
  delete [] colors;

  root->addChild( bind );
  root->addChild( material );

  SoDrawStyle* draw_style_node = new SoDrawStyle;
  draw_style_node->pointSize = 4;
  root->addChild( draw_style_node );
  root->addChild( new SoPointSet );

  root->addChild( new SoPointSet );
  //==========================
  */
  
  root->addChild( oinv_pset.oinv_node() );

  Display_pref_panel* controls =
    new Display_pref_panel( &oinv_pset, 0, 0 );
  
  SoQtGsTLViewer* myViewer = new SoQtGsTLViewer( (QWidget*) myWindow, "camera" );
  myViewer->setSceneGraph(root);
  myViewer->show();
  controls->show();
  SoQt::show(myWindow);
  
  SoQt::mainLoop();
  
  
}
