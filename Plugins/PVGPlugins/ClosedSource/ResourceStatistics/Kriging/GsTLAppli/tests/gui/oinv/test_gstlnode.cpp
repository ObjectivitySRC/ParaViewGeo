#include <stdlib.h>
#include <Inventor/Qt/SoQt.h>
#include <Inventor/Qt/SoQtRenderArea.h>
#include <Inventor/nodes/SoCone.h>
#include <Inventor/nodes/SoDirectionalLight.h>
#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/nodes/SoPerspectiveCamera.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoSwitch.h>

#include <GsTLAppli/gui/oinv_description/gstl_SoNode.h>


int
main(int , char **argv)
{
   // Initialize Inventor. This returns a main window to use.
   // If unsuccessful, exit.
   QWidget* myWindow = SoQt::init(argv[0]); // pass the app name
   if (myWindow == NULL) exit(1);

   // Make a scene containing a red cone
   SoSeparator *root = new SoSeparator;
   SoPerspectiveCamera *myCamera = new SoPerspectiveCamera;
   SoMaterial *myMaterial = new SoMaterial;
   root->ref();
   root->addChild(myCamera);
   root->addChild(new SoDirectionalLight);
   myMaterial->diffuseColor.setValue(1.0, 0.0, 0.0);   // Red
   root->addChild(myMaterial);
   
   
//   GsTL_SoNode::initClass();
   GsTL_SoNode* obj = new GsTL_SoNode;
   obj->addChild(new SoCone);
   root->addChild( obj );
   obj->visible = true;
   

   // Create a renderArea in which to see our scene graph.
   // The render area will appear within the main window.
   SoQtRenderArea *myRenderArea = new SoQtRenderArea(myWindow);

   // Make myCamera see everything.
   myCamera->viewAll(root, myRenderArea->getViewportRegion());

   // Put our scene in myRenderArea, change the title
   myRenderArea->setSceneGraph(root);
   myRenderArea->setTitle("Hello Cone");
   myRenderArea->show();

   SoQt::show(myWindow);  // Display main window
   SoQt::mainLoop();      // Main Inventor event loop
}
