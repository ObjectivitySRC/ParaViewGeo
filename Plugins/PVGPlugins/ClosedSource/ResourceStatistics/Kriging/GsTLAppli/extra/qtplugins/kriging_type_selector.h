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

#ifndef __GSTLAPPLI_QTPLUGINS_KRIGING_TYPE_SELECTOR_H__
#define __GSTLAPPLI_QTPLUGINS_KRIGING_TYPE_SELECTOR_H__

#include <GsTLAppli/extra/qtplugins/common.h>
#include <GsTLAppli/extra/qtplugins/kriging_type_selector_base.h>

#include <qstring.h>

#include <vector>
#include <utility>




class QTPLUGINS_DECL TrendComponents : public QWidget {

  Q_OBJECT

public:
  TrendComponents( QWidget* parent = 0, const char* name = 0 );
  virtual ~TrendComponents() {}

  void setTrendComponents( const std::vector<bool>& components );
  std::vector<bool> trendComponents() const;

protected:
  QCheckBox* x_;
  QCheckBox* y_;
  QCheckBox* z_;
  QCheckBox* xz_;
  QCheckBox* xy_;
  QCheckBox* yz_;
  QCheckBox* z2_;
  QCheckBox* y2_;
  QCheckBox* x2_;
};





class QTPLUGINS_DECL KrigingTypeSelector : public Kriging_type_selector_base {

  Q_OBJECT

 public:
  KrigingTypeSelector( QWidget* parent = 0, const char* name = 0 );
  virtual ~KrigingTypeSelector() {}
//  virtual QSize sizeHint() const { return QSize( 245, 80 ); }

  void setKrigingType( const QString& type );
  QString krigingType() const;

  void setSkMean( float m );
  float skMean() const;

  void setTrendComponents( const std::vector<bool>& components );
  std::vector<bool> trendComponents() const;

  void setLocalMeanProperty( const std::pair<QString, QString>& prop );
  std::pair<QString, QString> LocalMeanProperty() const;


 public slots:
  void promptForParameters( int id );
 
 signals:
  void krigingTypeChanged( const QString& );
  void krigingParametersChanged();
};

#endif
