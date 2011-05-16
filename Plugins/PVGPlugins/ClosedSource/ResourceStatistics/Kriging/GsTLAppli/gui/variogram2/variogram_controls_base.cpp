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

#include "variogram_controls_base.h"

#include <qvariant.h>
#include <qgroupbox.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qpushbutton.h>
#include <qspinbox.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>

/* 
 *  Constructs a Variogram_controls_base which is a child of 'parent', with the 
 *  name 'name' and widget flags set to 'f'.
 */
Variogram_controls_base::Variogram_controls_base( QWidget* parent,  const char* name, WFlags fl )
    : QWidget( parent, name, fl )
{
    if ( !name )
	setName( "Variogram_controls_base" );
    resize( 248, 182 ); 
    setCaption( trUtf8( "Variogram Controls" ) );
    Variogram_controls_baseLayout = new QHBoxLayout( this, 1, 6, "Variogram_controls_baseLayout"); 

    Layout18 = new QVBoxLayout( 0, 0, 6, "Layout18"); 

    Layout2 = new QHBoxLayout( 0, 0, 6, "Layout2"); 

    TextLabel1 = new QLabel( this, "TextLabel1" );
    TextLabel1->setText( trUtf8( "Nugget Effect" ) );
    Layout2->addWidget( TextLabel1 );

    nugget_effect_edit_ = new QLineEdit( this, "nugget_effect_edit_" );
    nugget_effect_edit_->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, nugget_effect_edit_->sizePolicy().hasHeightForWidth() ) );
    nugget_effect_edit_->setMaximumSize( QSize( 60, 32767 ) );
    Layout2->addWidget( nugget_effect_edit_ );
    QSpacerItem* spacer = new QSpacerItem( 20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    Layout2->addItem( spacer );
    Layout18->addLayout( Layout2 );
    QSpacerItem* spacer_2 = new QSpacerItem( 20, 16, QSizePolicy::Minimum, QSizePolicy::Fixed );
    Layout18->addItem( spacer_2 );

    structures_main_box_ = new QGroupBox( this, "structures_main_box_" );
    structures_main_box_->setFrameShape( QGroupBox::NoFrame );
    structures_main_box_->setFrameShadow( QGroupBox::Plain );
    structures_main_box_->setTitle( trUtf8( "" ) );
    structures_main_box_->setColumnLayout(0, Qt::Vertical );
    structures_main_box_->layout()->setSpacing( 6 );
    structures_main_box_->layout()->setMargin( 2 );
    structures_main_box_Layout = new QHBoxLayout( structures_main_box_->layout() );
    structures_main_box_Layout->setAlignment( Qt::AlignTop );

    Layout1 = new QHBoxLayout( 0, 0, 6, "Layout1"); 
    QSpacerItem* spacer_3 = new QSpacerItem( 20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    Layout1->addItem( spacer_3 );

    TextLabel2 = new QLabel( structures_main_box_, "TextLabel2" );
    TextLabel2->setText( trUtf8( "No of Structures" ) );
    Layout1->addWidget( TextLabel2 );

    structures_count_ = new QSpinBox( structures_main_box_, "structures_count_" );
    structures_count_->setMaxValue( 10 );
    structures_count_->setValue( 1 );
    Layout1->addWidget( structures_count_ );
    QSpacerItem* spacer_4 = new QSpacerItem( 20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    Layout1->addItem( spacer_4 );
    structures_main_box_Layout->addLayout( Layout1 );
    Layout18->addWidget( structures_main_box_ );
    QSpacerItem* spacer_5 = new QSpacerItem( 20, 20, QSizePolicy::Minimum, QSizePolicy::Expanding );
    Layout18->addItem( spacer_5 );
    Variogram_controls_baseLayout->addLayout( Layout18 );
}

/*  
 *  Destroys the object and frees any allocated resources
 */
Variogram_controls_base::~Variogram_controls_base()
{
    // no need to delete child widgets, Qt does it all for us
}

