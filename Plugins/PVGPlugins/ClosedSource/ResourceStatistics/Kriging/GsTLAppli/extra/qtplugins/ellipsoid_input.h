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

#ifndef __GSTLAPPLI_QTPLUGINS_EllipsoidInput_H__
#define __GSTLAPPLI_QTPLUGINS_EllipsoidInput_H__


#include <GsTLAppli/extra/qtplugins/common.h>

#include <qgroupbox.h>
#include <qstring.h>

class GsTLTable;
class QResizeEvent;


class QTPLUGINS_DECL EllipsoidInput : public QWidget {

  Q_OBJECT

public:
  EllipsoidInput( QWidget *parent = 0, const char *name = 0);
  virtual ~EllipsoidInput();

  void set_max_width( int );

  void set_ranges( double r1, double r2, double r3 );
  void set_max_range( double a );
  void set_med_range( double a );  
  void set_min_range( double a );

  void set_angles( double r1, double r2, double r3 );
  void set_azimuth( double a );
  void set_dip( double a );  
  void set_rake( double a );

  void ranges( double& r1, double& r2, double& r3 );
  double max_range() const;
  double med_range() const;
  double min_range() const;

  void angles( double& a1, double& a2, double& a3 );
  double azimuth() const;
  double dip() const;
  double rake() const;

protected:
  virtual void resizeEvent ( QResizeEvent * );

private:
  GsTLTable* ranges_table_;
  GsTLTable* angles_table_;
};


#endif


