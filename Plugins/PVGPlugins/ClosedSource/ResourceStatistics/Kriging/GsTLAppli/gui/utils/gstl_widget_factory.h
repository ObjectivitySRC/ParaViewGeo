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

#ifndef __GSTLAPPLI_GSTL_WIDGET_FACTORY__ 
#define __GSTLAPPLI_GSTL_WIDGET_FACTORY__ 
 
 
#include <GsTLAppli/gui/common.h>
#include <qwidgetfactory.h> 
 
class GsTL_project; 
 
 
class GUI_DECL GsTL_widget_factory : public QWidgetFactory { 
 public: 
  /** Create a widget from description provided by dev. 
   * Connections are set up automatically, as indicated by pre-defined keywords 
   * found in the description provided by dev. 
   * 
   * Example: 
   *  - a widget whose name starts with ADV_ will be considered an advanced 
   *    parameters and will be shown only if the user has selected the 
   *    advanced mode. 
   */ 
  static QWidget* create( GsTL_project* project,  
			  QIODevice* dev,  
			  QWidget* parent = 0 ,  
			  const char* name = 0 ); 
}; 
 
 
#endif 
