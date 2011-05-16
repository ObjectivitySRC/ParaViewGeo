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

#ifndef __GSTLAPPLI_GUI_APPLI_SOQTGSTLVIEWER_H__ 
#define __GSTLAPPLI_GUI_APPLI_SOQTGSTLVIEWER_H__ 
 
#include <GsTLAppli/gui/common.h>
#include <GsTLAppli/math/gstlvector.h> 

#include <Inventor/Qt/viewers/SoQtExaminerViewer.h> 
#include <Inventor/nodes/SoTransform.h>


class SoEvent;

/** GsTLAppli's own Open Inventor viewer.   
 */ 
 
class GUI_DECL SoQtGsTLViewer : public SoQtExaminerViewer { 
 public: 
  SoQtGsTLViewer( QWidget* parent = 0, const char* name =0,  
		  SbBool embed = true ) ; 
 
  virtual ~SoQtGsTLViewer() {};
  
  void rotate_camera( GsTLVector<float> dir, float ang ) const;

  //TL modified
  void setColorbarStatus(SbBool s) { _colorbarvisible = s; }
  void setColorbarTransform(SoTransform * x) {
	  _colorbarx = x;
	  _oldtrans = x->translation;
	  _oldscale = x->scaleFactor;
	  _xscale = (_oldscale.getValue())[0];
  	  _yscale = (_oldscale.getValue())[1];
	  _xmove = (_oldtrans.getValue())[0];
	  _ymove = (_oldtrans.getValue())[1];
  }


 protected:
  SbBool processSoEvent( const SoEvent *const ev );

  //TL modified
  SoSFVec3f _oldtrans,_oldscale;
  float _xmove, _ymove, _xscale, _yscale;
  SbBool _colorbarvisible;
  SoTransform * _colorbarx;
}; 
 
 
#endif 
