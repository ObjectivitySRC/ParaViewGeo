/**********************************************************************
** Author: Nicolas Remy
** Copyright (C) 2002-2004 The Board of Trustees of the Leland Stanford Junior
**   University
** All rights reserved.
**
** This file is part of the "gui" module of the Geostatistical Earth
** Modeling Software (GEMS)
**
** This file may be distributed and/or modified under the terms of the 
** license defined by the Stanford Center for Reservoir Forecasting and 
** appearing in the file LICENSE.XFREE included in the packaging of this file.
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
** See http://www.gnu.org/copyleft/gpl.html for GPL licensing information.
**
** Contact the Stanford Center for Reservoir Forecasting, Stanford University
** if any conditions of this licensing are not clear to you.
**
**********************************************************************/


#ifdef WIN32
#include <SoWinEnterScope.h>
#endif


#include "gstl_SoNode.h"

#include <Inventor/actions/SoGLRenderAction.h> 
#include <Inventor/misc/SoChildList.h>
#include <Inventor/actions/SoGetBoundingBoxAction.h>
#include <Inventor/actions/SoHandleEventAction.h>
#include <Inventor/actions/SoPickAction.h>
#include <Inventor/actions/SoGetMatrixAction.h>

SO_NODE_SOURCE(GsTL_SoNode);


void GsTL_SoNode::initClass() {
	SO_NODE_INIT_CLASS(GsTL_SoNode, SoSwitch, "Switch");

}

GsTL_SoNode::GsTL_SoNode() 
: SoSwitch( 2 ) {
	SO_NODE_CONSTRUCTOR(GsTL_SoNode);
	SO_NODE_ADD_FIELD( visible, (false) );
}



void GsTL_SoNode::getBoundingBox(SoGetBoundingBoxAction *action) {
  GsTL_SoNode::doAction( action );
}


void GsTL_SoNode::GLRender( SoGLRenderAction * action ) {
  GsTL_SoNode::doAction( action );
}

void GsTL_SoNode::handleEvent(SoHandleEventAction *action){
  GsTL_SoNode::doAction( action );
}

void  GsTL_SoNode::pick(SoPickAction *action){
  GsTL_SoNode::doAction( action );
}


void GsTL_SoNode::doAction( SoAction* action) {
  if( visible.getValue() == true ) {
    children->traverse(action,0);
  }
}


// This implements the traversal for the SoGetMatrixAction,
// which is handled a little differently: it does not traverse
// below the root node or tail of the path it is applied to.
// Therefore, we need to compute the matrix only if this group
// is in the middle of the current path chain or is off the path
// chain (since the only way this could be true is if the group
// is under a group that affects the chain).

void
GsTL_SoNode::getMatrix(SoGetMatrixAction *action)
{
   int         numIndices;
   const int   *indices;

   // Use SoAction::getPathCode() to determine where this group
   // is in relation to the path being applied to (if any).
   switch (action->getPathCode(numIndices, indices)) {

    case SoAction::NO_PATH:
    case SoAction::BELOW_PATH:
     // If there's no path, or we're off the end, do nothing.
     break;

    case SoAction::OFF_PATH:
    case SoAction::IN_PATH:
     // If we are in the path chain or we affect nodes in the
     // path chain, traverse the children.
     GsTL_SoNode::doAction(action);
     break;
   }
}