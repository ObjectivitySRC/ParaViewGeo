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

#include "kriging_type_selector_base.h"

#include "selectors.h"

#include <qvariant.h>
#include <qcheckbox.h>
#include <qcombobox.h>
#include <qgroupbox.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qpushbutton.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>

/* 
 *  Constructs a Kriging_type_selector_base which is a child of 'parent', with the 
 *  name 'name' and widget flags set to 'f'.
 */
Kriging_type_selector_base::Kriging_type_selector_base( QWidget* parent,  const char* name, WFlags fl )
    : QWidget( parent, name, fl )
{
    if ( !name )
	setName( "Kriging_type_selector_base" );
    resize( 209, 421 ); 
    setCaption( trUtf8( "Kriging Type Selection" ) );
    Kriging_type_selector_baseLayout = new QVBoxLayout( this, 11, 6, "Kriging_type_selector_baseLayout"); 

    kriging_type_ = new QComboBox( FALSE, this, "kriging_type_" );
    kriging_type_->insertItem( trUtf8( "Simple Kriging (SK)" ) );
    kriging_type_->insertItem( trUtf8( "Ordinary Kriging (OK)" ) );
    kriging_type_->insertItem( trUtf8( "Kriging with Trend (KT)" ) );
    kriging_type_->insertItem( trUtf8( "SK with Locally Varying Mean (LVM)" ) );
    Kriging_type_selector_baseLayout->addWidget( kriging_type_ );

    sk_mean_box_ = new QGroupBox( this, "sk_mean_box_" );
    sk_mean_box_->setFrameShape( QGroupBox::NoFrame );
    sk_mean_box_->setTitle( trUtf8( "" ) );
    sk_mean_box_->setColumnLayout(0, Qt::Vertical );
    sk_mean_box_->layout()->setSpacing( 6 );
    sk_mean_box_->layout()->setMargin( 11 );
    sk_mean_box_Layout = new QHBoxLayout( sk_mean_box_->layout() );
    sk_mean_box_Layout->setAlignment( Qt::AlignTop );

    TextLabel2_2 = new QLabel( sk_mean_box_, "TextLabel2_2" );
    TextLabel2_2->setText( trUtf8( "Mean" ) );
    sk_mean_box_Layout->addWidget( TextLabel2_2 );

    sk_mean_edit_ = new QLineEdit( sk_mean_box_, "sk_mean_edit_" );
    sk_mean_box_Layout->addWidget( sk_mean_edit_ );
    Kriging_type_selector_baseLayout->addWidget( sk_mean_box_ );

    trend_components_box_ = new QGroupBox( this, "trend_components_box_" );
    trend_components_box_->setTitle( trUtf8( "Trend Components" ) );
    trend_components_box_->setColumnLayout(0, Qt::Vertical );
    trend_components_box_->layout()->setSpacing( 6 );
    trend_components_box_->layout()->setMargin( 11 );
    trend_components_box_Layout = new QGridLayout( trend_components_box_->layout() );
    trend_components_box_Layout->setAlignment( Qt::AlignTop );

    y_ = new QCheckBox( trend_components_box_, "y_" );
    y_->setText( trUtf8( "Y" ) );

    trend_components_box_Layout->addWidget( y_, 0, 1 );

    z_ = new QCheckBox( trend_components_box_, "z_" );
    z_->setText( trUtf8( "Z" ) );

    trend_components_box_Layout->addWidget( z_, 0, 2 );

    xz_ = new QCheckBox( trend_components_box_, "xz_" );
    xz_->setText( trUtf8( "X.Z" ) );

    trend_components_box_Layout->addWidget( xz_, 2, 1 );

    xy_ = new QCheckBox( trend_components_box_, "xy_" );
    xy_->setText( trUtf8( "X.Y" ) );

    trend_components_box_Layout->addWidget( xy_, 2, 0 );

    z2_ = new QCheckBox( trend_components_box_, "z2_" );
    z2_->setText( trUtf8( "Z^2" ) );

    trend_components_box_Layout->addWidget( z2_, 1, 2 );

    yz_ = new QCheckBox( trend_components_box_, "yz_" );
    yz_->setText( trUtf8( "Y.Z" ) );

    trend_components_box_Layout->addWidget( yz_, 2, 2 );

    y2_ = new QCheckBox( trend_components_box_, "y2_" );
    y2_->setText( trUtf8( "Y^2" ) );

    trend_components_box_Layout->addWidget( y2_, 1, 1 );

    x2_ = new QCheckBox( trend_components_box_, "x2_" );
    x2_->setText( trUtf8( "X^2" ) );

    trend_components_box_Layout->addWidget( x2_, 1, 0 );

    x_ = new QCheckBox( trend_components_box_, "x_" );
    x_->setText( trUtf8( "X" ) );

    trend_components_box_Layout->addWidget( x_, 0, 0 );
    Kriging_type_selector_baseLayout->addWidget( trend_components_box_ );

    local_mean_box_ = new QGroupBox( this, "local_mean_box_" );
    local_mean_box_->setFrameShape( QGroupBox::NoFrame );
    local_mean_box_->setTitle( trUtf8( "" ) );
    local_mean_box_->setColumnLayout(0, Qt::Vertical );
    local_mean_box_->layout()->setSpacing( 6 );
    local_mean_box_->layout()->setMargin( 11 );
    local_mean_box_Layout = new QVBoxLayout( local_mean_box_->layout() );
    local_mean_box_Layout->setAlignment( Qt::AlignTop );

    TextLabel4 = new QLabel( local_mean_box_, "TextLabel4" );
    TextLabel4->setText( trUtf8( "Property with local mean:" ) );
    local_mean_box_Layout->addWidget( TextLabel4 );

    Layout25 = new QHBoxLayout( 0, 0, 6, "Layout25"); 
    QSpacerItem* spacer = new QSpacerItem( 20, 20, QSizePolicy::Minimum, QSizePolicy::Minimum );
    Layout25->addItem( spacer );

    local_mean_property_ = new PropertySelector( local_mean_box_, "local_mean_property_" );
    local_mean_property_->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)5, 1, 0, local_mean_property_->sizePolicy().hasHeightForWidth() ) );
    Layout25->addWidget( local_mean_property_ );
    QSpacerItem* spacer_2 = new QSpacerItem( 20, 20, QSizePolicy::Minimum, QSizePolicy::Minimum );
    Layout25->addItem( spacer_2 );
    local_mean_box_Layout->addLayout( Layout25 );
    Kriging_type_selector_baseLayout->addWidget( local_mean_box_ );
    QSpacerItem* spacer_3 = new QSpacerItem( 20, 20, QSizePolicy::Minimum, QSizePolicy::Expanding );
    Kriging_type_selector_baseLayout->addItem( spacer_3 );
}

/*  
 *  Destroys the object and frees any allocated resources
 */
Kriging_type_selector_base::~Kriging_type_selector_base()
{
    // no need to delete child widgets, Qt does it all for us
}

