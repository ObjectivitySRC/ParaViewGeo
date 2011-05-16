#include <GsTLAppli/gui/oinv_description/oinv_slice.h>
#include <GsTLAppli/gui/oinv_description/oinv_cgrid.h>
#include <GsTLAppli/gui/oinv_description/gstl_SoNode.h>
#include <GsTLAppli/gui/utils/colormap.h>
#include <GsTLAppli/appli/manager_repository.h>
#include <GsTLAppli/gui/appli/display_pref_panel.h>

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
#include <Inventor/nodes/SoLightModel.h>
#include <Inventor/nodes/SoDrawStyle.h>

#include <qwidget.h>
#include <qcheckbox.h>

#include <fstream>


int main(int argc, char **argv) {
  // Initialize Inventor and Qt
  QWidget *myWindow = SoQt::init(argv[0]);
  GsTL_SoNode::initClass();
  if (myWindow == NULL) exit(1);
  
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

  Root::instance()->list_all( cout );

  int nx=5;
  int ny=3;
  int nz=6;

  Cartesian_grid grid;
  grid.set_dimensions( nx, ny, nz, 1, 1, 1 );
  grid.origin( GsTLPoint( -0.5, -0.5, -0.5 ) );
  GsTLGridProperty* prop = grid.add_property( "toto", typeid( float ) );
  for( int i=0; i < nx*ny*nz; i ++ ) 
    prop->set_value( i, i );

  Oinv_cgrid oinv_grid;
  oinv_grid.init( &grid );
  oinv_grid.set_property( "toto" );
  oinv_grid.oinv_node()->visible = true;
  
  cout << "Setting up oinv scene" << endl;
  
  SoSelection* root = new SoSelection;

  SoSeparator* line_sep = new SoSeparator;
  SoLightModel* light_model = new SoLightModel;
  light_model->model = SoLightModel::BASE_COLOR;
  line_sep->addChild( light_model );
  SoMaterial* material = new SoMaterial;
  material->diffuseColor.setValue( 1.0, 0.0, 0.0 );
  line_sep->addChild( material );
  SoDrawStyle* draw_style = new SoDrawStyle;
  draw_style->style = SoDrawStyle::LINES;
  draw_style->lineWidth = 2;
  line_sep->addChild( draw_style );
  oinv_grid.property_display_mode( Oinv::NOT_PAINTED );
  line_sep->addChild( oinv_grid.oinv_node() );

  root->addChild( line_sep );
  
  SoSeparator* filled_sep = new SoSeparator;
  SoDrawStyle* draw_style_filled = new SoDrawStyle;
  draw_style_filled->style = SoDrawStyle::FILLED;
  filled_sep->addChild( draw_style_filled );
  oinv_grid.property_display_mode( Oinv::PAINTED );
  filled_sep->addChild( oinv_grid.oinv_node() );
  
  root->addChild( filled_sep );



  SoQtGsTLViewer* myViewer = new SoQtGsTLViewer( (QWidget*) myWindow, "camera" );
  //SoQtExaminerViewer *myViewer = new SoQtExaminerViewer(myWindow);
  myViewer->setSceneGraph(root);
  myViewer->show();
  SoQt::show(myWindow);
  
  SoQt::mainLoop();
  
  
}
