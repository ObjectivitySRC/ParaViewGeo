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

#ifndef VARIOGRAM_CONTROLS_BASE_H
#define VARIOGRAM_CONTROLS_BASE_H

#include <qvariant.h>
#include <qwidget.h>
class QVBoxLayout; 
class QHBoxLayout; 
class QGridLayout; 
class QGroupBox;
class QLabel;
class QLineEdit;
class QSpinBox;

class Variogram_controls_base : public QWidget
{ 
    Q_OBJECT

public:
    Variogram_controls_base( QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
    ~Variogram_controls_base();

    QLabel* TextLabel1;
    QLineEdit* nugget_effect_edit_;
    QGroupBox* structures_main_box_;
    QLabel* TextLabel2;
    QSpinBox* structures_count_;


protected:
    QHBoxLayout* Variogram_controls_baseLayout;
    QVBoxLayout* Layout18;
    QHBoxLayout* Layout2;
    QHBoxLayout* structures_main_box_Layout;
    QHBoxLayout* Layout1;
};

#endif // VARIOGRAM_CONTROLS_BASE_H
