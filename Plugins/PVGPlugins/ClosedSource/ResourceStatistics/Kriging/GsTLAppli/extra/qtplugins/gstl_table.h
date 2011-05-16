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

#ifndef __GSTLAPPLI_QTPLUGINS_GSTL_TABLE_H__
#define __GSTLAPPLI_QTPLUGINS_GSTL_TABLE_H__


#include <GsTLAppli/extra/qtplugins/common.h>

#include <qtable.h>
#include <qstring.h>

class QEvent;
class QHeader;

class QTPLUGINS_DECL GsTLTable : public QTable {

  Q_OBJECT
  Q_PROPERTY( int default_column_size READ defaultColsize WRITE setDefaultColsize DESIGNABLE true STORED true )
  Q_PROPERTY( QString row_headers READ rowHeaders WRITE setRowHeaders DESIGNABLE true STORED true )
  Q_PROPERTY( QString col_headers READ colHeaders WRITE setColHeaders DESIGNABLE true STORED true )

public:
  GsTLTable(QWidget *parent = 0, const char *name = 0);

  virtual ~GsTLTable();

  bool eventFilter(QObject *o, QEvent *e);

  int defaultColsize() const { return default_colsize_; }
  void setDefaultColsize( int s ); 
 
  QString rowHeaders() const;
  QString colHeaders() const;
  void setRowHeaders( const QString& headers );
  void setColHeaders( const QString& headers );

protected:
  void copy();
  void paste();
  void delete_cells();
  QString headerLabels( QHeader* header ) const;
  void setHeaderLabels( QHeader* header, const QStringList& label_str );

private:
//  QString clipboard;
  int default_colsize_;

  QStringList row_labels_;
  QStringList col_labels_;
};


#endif


