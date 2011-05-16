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

#include <GsTLAppli/gui/appli/SoQtGsTLViewer.h>
#include <Inventor/nodes/SoCamera.h> 
#include <Inventor/events/SoEvent.h>
#include <Inventor/events/SoKeyboardEvent.h>
#include <GsTLAppli/utils/gstl_messages.h>
#include <Inventor/events/SoMouseButtonEvent.h>
#include <Inventor/SbLinear.h>
#include <Inventor/events/SoLocation2Event.h>
#include <Inventor/fields/SoSFRotation.h>

#define MSTEP 0.02
#define SSTEP 0.02

SoQtGsTLViewer::SoQtGsTLViewer( QWidget* parent, const char* name, 
				SbBool embed ) 
  : SoQtExaminerViewer( parent, name, embed ),  
   _colorbarx(NULL)
{
  _colorbarvisible = false;
  setDecoration( false );
  setFeedbackVisibility( true );
}



void SoQtGsTLViewer::rotate_camera( GsTLVector<float> dir, float ang ) const {
  SoCamera* camera = getCamera();
  if( !camera ) return;
  
  SbVec3f about_vec( dir.x(), dir.y(), dir.z() );
  camera->orientation.setValue( SbRotation( about_vec, ang ) );

}



SbBool SoQtGsTLViewer::processSoEvent( const SoEvent *const event ) {
  const SoType type(event->getTypeId());
  const SoKeyboardEvent * keyevent = 0;
  SbVec3f t;

  //TL modified
  if (type.isDerivedFrom(SoKeyboardEvent::getClassTypeId())) {
    keyevent = (SoKeyboardEvent *) event;
	if ( keyevent->getState() == SoButtonEvent::DOWN &&
		keyevent->getKey()== SoKeyboardEvent::Q ) {
			return TRUE;
	}
	if (_colorbarvisible == true){
		if (keyevent->wasCtrlDown()) {
			SbVec3f vec;
			float angle;
			_colorbarx->rotation.getValue(vec,angle);
			if (angle == 0.0) {
				switch(keyevent->getKey()) {
					case SoKeyboardEvent::LEFT_ARROW: _xscale -= SSTEP; break;
					case SoKeyboardEvent::RIGHT_ARROW: _xscale += SSTEP; break;
					case SoKeyboardEvent::UP_ARROW: _yscale -= SSTEP; break;
					case SoKeyboardEvent::DOWN_ARROW: _yscale += SSTEP; break;
					case SoKeyboardEvent::F12: 
						_colorbarx->scaleFactor = _oldscale;
						t = _oldscale.getValue();
						_xscale = t[0];
						_yscale = t[1];
						return true;
					default: return true;
				}
			} else {
				switch(keyevent->getKey()) {
					case SoKeyboardEvent::LEFT_ARROW: _yscale -= SSTEP; break;
					case SoKeyboardEvent::RIGHT_ARROW: _yscale += SSTEP; break;
					case SoKeyboardEvent::UP_ARROW: _xscale -= SSTEP; break;
					case SoKeyboardEvent::DOWN_ARROW: _xscale += SSTEP; break;
					case SoKeyboardEvent::F12: 
						_colorbarx->scaleFactor = _oldscale;
						t = _oldscale.getValue();
						_xscale = t[0];
						_yscale = t[1];
						return true;
					default: return true;
				}
			}
			_colorbarx->scaleFactor.setValue(_xscale,_yscale,1.0);
			return true;
		}
		else if (keyevent->wasAltDown()) {
			switch(keyevent->getKey()) {
			case SoKeyboardEvent::LEFT_ARROW: _xmove -= MSTEP; break;
			case SoKeyboardEvent::RIGHT_ARROW: _xmove += MSTEP; break;
			case SoKeyboardEvent::UP_ARROW: _ymove += MSTEP; break;
			case SoKeyboardEvent::DOWN_ARROW: _ymove -= MSTEP; break;
			case SoKeyboardEvent::F12: 
				_colorbarx->translation = _oldtrans;
				t = _oldtrans.getValue();
				_xmove = t[0];
				_ymove = t[1];
				return true;
			default: return true;
			}
			_colorbarx->translation.setValue(_xmove,_ymove,0);
			return true;
		}
	}
  }
  return SoQtExaminerViewer::processSoEvent( event );
}


