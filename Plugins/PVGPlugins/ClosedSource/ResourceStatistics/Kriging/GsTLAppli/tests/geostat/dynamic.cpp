/****************************************************************************
** Form implementation generated from reading ui file 'dynamic.ui'
**
** Created: Thu Sep 5 16:05:51 2002
**      by:  The User Interface Compiler (uic)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/
#include "dynamic.h"

#include <qvariant.h>
#include <qbuttongroup.h>
#include <qcombobox.h>
#include <qframe.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qradiobutton.h>
#include <qslider.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>

/* 
 *  Constructs a Form1 which is a child of 'parent', with the 
 *  name 'name' and widget flags set to 'f'.
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
Form1::Form1( QWidget* parent,  const char* name, bool modal, WFlags fl )
    : QDialog( parent, name, modal, fl )
{
    if ( !name )
	setName( "Form1" );
    resize( 276, 459 ); 
    setCaption( trUtf8( "Form1" ) );

    Frame6 = new QFrame( this, "Frame6" );
    Frame6->setGeometry( QRect( 30, 20, 211, 391 ) ); 
    Frame6->setFrameShape( QFrame::StyledPanel );
    Frame6->setFrameShadow( QFrame::Raised );

    TextLabel1 = new QLabel( Frame6, "TextLabel1" );
    TextLabel1->setGeometry( QRect( 11, 1, 178, 48 ) ); 
    TextLabel1->setFrameShape( QLabel::MShape );
    TextLabel1->setFrameShadow( QLabel::MShadow );
    TextLabel1->setText( trUtf8( "Grid" ) );

    GridSelector = new QComboBox( FALSE, Frame6, "GridSelector" );
    GridSelector->setGeometry( QRect( 11, 55, 178, 27 ) ); 
    GridSelector->setEditable( TRUE );
    GridSelector->setAutoCompletion( TRUE );
    QToolTip::add( GridSelector, trUtf8( "Select grid" ) );

    TextLabel1_2 = new QLabel( Frame6, "TextLabel1_2" );
    TextLabel1_2->setGeometry( QRect( 11, 88, 178, 48 ) ); 
    TextLabel1_2->setText( trUtf8( "Property" ) );

    PropertyName = new QComboBox( FALSE, Frame6, "PropertyName" );
    PropertyName->setGeometry( QRect( 11, 142, 178, 27 ) ); 
    PropertyName->setEditable( TRUE );

    TransformData = new QRadioButton( Frame6, "TransformData" );
    TransformData->setGeometry( QRect( 11, 175, 178, 25 ) ); 
    TransformData->setText( trUtf8( "Transform data" ) );

    ButtonGroup1 = new QButtonGroup( Frame6, "ButtonGroup1" );
    ButtonGroup1->setEnabled( FALSE );
    ButtonGroup1->setGeometry( QRect( 11, 206, 178, 131 ) ); 
    ButtonGroup1->setFrameShape( QButtonGroup::WinPanel );
    ButtonGroup1->setFrameShadow( QButtonGroup::Sunken );
    ButtonGroup1->setTitle( trUtf8( "ButtonGroup1" ) );

    FromData = new QRadioButton( ButtonGroup1, "FromData" );
    FromData->setGeometry( QRect( 10, 20, 120, 31 ) ); 
    FromData->setText( trUtf8( "Infer from data" ) );
    FromData->setChecked( TRUE );

    FromFile = new QRadioButton( ButtonGroup1, "FromFile" );
    FromFile->setGeometry( QRect( 10, 50, 140, 31 ) ); 
    FromFile->setText( trUtf8( "Use ref distribution" ) );

    TransformFileName = new QLineEdit( ButtonGroup1, "TransformFileName" );
    TransformFileName->setEnabled( FALSE );
    TransformFileName->setGeometry( QRect( 10, 80, 141, 31 ) ); 

    realization = new QSlider( Frame6, "realization" );
    realization->setGeometry( QRect( 10, 350, 151, 24 ) ); 
    realization->setOrientation( QSlider::Horizontal );

    // signals and slots connections
    connect( TransformData, SIGNAL( toggled(bool) ), ButtonGroup1, SLOT( setEnabled(bool) ) );
    connect( FromData, SIGNAL( toggled(bool) ), TransformFileName, SLOT( setDisabled(bool) ) );
    connect( FromFile, SIGNAL( toggled(bool) ), TransformFileName, SLOT( setEnabled(bool) ) );
}

/*  
 *  Destroys the object and frees any allocated resources
 */
Form1::~Form1()
{
    // no need to delete child widgets, Qt does it all for us
}

void Form1::hideSlot()
{
    qWarning( "Form1::hideSlot(): Not implemented yet!" );
}

