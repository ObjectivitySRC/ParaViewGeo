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

#ifndef __TEST_GSTLAPPLI_OINV_GSTLNODE_H__ 
#define __TEST_GSTLAPPLI_OINV_GSTLNODE_H__ 


#ifdef WIN32
#include <SoWinLeaveScope.h>
#endif

#include <GsTLAppli/gui/common.h>

#include <Inventor/nodes/SoSwitch.h> 
#include <Inventor/fields/SoSFBool.h> 
#include <Inventor/fields/SoSFInt32.h> 
 
class SoAction; 
class SoGLRenderAction; 
class SoGetMatrixAction;


class GUI_DECL GsTL_SoNode : public SoSwitch { 
 
  SO_NODE_HEADER(GsTL_SoNode); 
   
 public: 
 
  // Fields 
  SoSFBool visible;   
 
  // Initializes the class 
  static void initClass(); 
 
  // default constructor 
  GsTL_SoNode(); 
  virtual ~GsTL_SoNode() {} 
 
 protected: 
  virtual void getBoundingBox(SoGetBoundingBoxAction *action); 
  virtual void GLRender(SoGLRenderAction *action); 
  virtual void handleEvent(SoHandleEventAction *action); 
 virtual void  getMatrix(SoGetMatrixAction *action);
  virtual void pick(SoPickAction *action); 
  virtual void doAction( SoAction* action ); 
}; 


#ifdef WIN32
#include <SoWinEnterScope.h>
#endif

 
#endif 
