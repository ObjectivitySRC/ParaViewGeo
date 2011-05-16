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

#ifndef FILECHOOSER_H
#define FILECHOOSER_H

#include <GsTLAppli/extra/qtplugins/common.h>

#include <qwidget.h>

class QLineEdit;
class QPushButton;

class QTPLUGINS_DECL FileChooser : public QWidget
{
    Q_OBJECT

    Q_ENUMS( Mode )
    Q_PROPERTY( Mode mode READ mode WRITE setMode )
    Q_PROPERTY( QString fileName READ fileName WRITE setFileName )

public:
    FileChooser( QWidget *parent = 0, const char *name = 0);

    enum Mode { Any, File, Directory };

    QString fileName() const;
    Mode mode() const;
    virtual QSize sizeHint() const { return QSize( 190, 30 ); }

public slots:
    void setFileName( const QString &fn );
    void setMode( Mode m );

signals:
    void fileNameChanged( const QString & );

private slots:
    void chooseFile();

private:
    QLineEdit *lineEdit;
    QPushButton *button;
    Mode md;

};

#endif
