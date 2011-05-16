/**********************************************************************
** Author: Nicolas Remy
** Copyright (C) 2002-2004 The Board of Trustees of the Leland Stanford Junior
**   University
** All rights reserved.
**
** This file is part of the "extra" module of the Geostatistical Earth
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

#ifndef __GSTLAPPLI_EXTRA_GSTL_WIDGETS_PLUGIN_H__
#define __GSTLAPPLI_EXTRA_GSTL_WIDGETS_PLUGIN_H__

#include <GsTLAppli/extra/qtplugins/common.h>

#include <qwidgetplugin.h>


class QTPLUGINS_DECL GsTL_Widgets_plugin : public QWidgetPlugin {
 public:
  GsTL_Widgets_plugin();

  QStringList keys() const;
  QWidget* create( const QString& classname,
		   QWidget* parent = 0, const char* name = 0 );
  QString group( const QString& ) const;
  QIconSet iconSet( const QString& ) const;
  QString includeFile( const QString& ) const;
  QString toolTip( const QString& ) const;
  QString WhatsThis( const QString& ) const;
  bool isContainer( const QString& ) const;
};

#endif
