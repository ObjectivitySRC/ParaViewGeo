#include <GsTLAppli/gui/utils/colormap.h>
#include <GsTLAppli/appli/manager_repository.h>
#include <GsTLAppli/gui/appli/SoQtGsTLViewer.h>
#include <GsTLAppli/gui/oinv_description/colormap_node.h>

#include <Inventor/Qt/SoQt.h>
#include <Inventor/Qt/viewers/SoQtExaminerViewer.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/nodes/SoLightModel.h>
#include <Inventor/nodes/SoDrawStyle.h>
#include <Inventor/nodes/SoCube.h>
#include <Inventor/nodes/SoMaterialBinding.h>
#include <Inventor/nodes/SoCoordinate3.h>
#include <Inventor/nodes/SoTransform.h>
#include <Inventor/nodes/SoQuadMesh.h>
#include <Inventor/nodes/SoPerspectiveCamera.h>
#include <Inventor/nodes/SoOrthographicCamera.h>
#include <Inventor/actions/SoGetBoundingBoxAction.h>
#include <Inventor/actions/SoWriteAction.h>

#include <fstream>


int main(int argc, char **argv) {
  // Initialize Inventor and Qt
  QWidget *myWindow = SoQt::init(argv[0]);
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

  SmartPtr<Named_interface> ni = 
    Root::instance()->interface( colormap_manager + "/rainbow" );
  Color_scale* c_scale = dynamic_cast<Color_scale*>( ni.raw_ptr() );





  Colormap cmap( c_scale );
  cmap.set_bounds( 0, 1 );

  Oinv_colormap_node cmap_node( cmap );
  SoSeparator* root = new SoSeparator;

  
  SoSeparator* scene_root = new SoSeparator;
  root->addChild( scene_root );
  scene_root->addChild( new SoPerspectiveCamera );
  //  SoTransform* cube_transf = new SoTransform;
  //  cube_transf->scaleFactor.setValue( 1002, 788, 999 );
  //  scene_root->addChild( cube_transf );
  //  scene_root->addChild( new SoCube );
  

  SoQtGsTLViewer* myViewer = new SoQtGsTLViewer( (QWidget*) myWindow, "camera" );
  myViewer->setSceneGraph(root);


  SoSeparator* cmap_root = new SoSeparator;
  root->addChild( cmap_root );

  SoOrthographicCamera * pcam = new SoOrthographicCamera;
  pcam->position = SbVec3f(0, 0, 5);
  pcam->nearDistance = 0.1;
  pcam->farDistance = 1000;
  pcam->viewportMapping = SoCamera::LEAVE_ALONE;
  cmap_root->addChild( pcam );
  
  SoGetBoundingBoxAction bbox_action( myViewer->getViewportRegion() );
  bbox_action.apply( cmap_node.oinv_node() );
  SbBox3f bbox = bbox_action.getBoundingBox();
  float dx, dy, dz;
  bbox.getSize( dx, dy, dz );
  cout << "Bounding box dimension: " << dx << " " << dy << " " << dz << endl;


  SoTransform* transf = new SoTransform;
  transf->translation.setValue( 0.7, 0 ,0 );
  transf->scaleFactor.setValue( 0.3/dx, 1, 1 );
  cmap_root->addChild( transf );
  

  cmap_root->addChild( cmap_node.oinv_node() );
  
  
  scene_root->addChild( new SoCube );

  myViewer->show();
  SoQt::show(myWindow);
  
  SoQt::mainLoop();
  

  // Write scene to file
  FILE* file = fopen("scene.iv", "w");
  SoWriteAction writer;

  writer.getOutput()->setFilePointer(file);
  writer.apply(root);
}
