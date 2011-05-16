/****************************************************************************
** Form implementation generated from reading ui file 'test.ui'
**
** Created: Wed Jan 29 22:12:37 2003
**      by:  The User Interface Compiler (uic)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/
#include "test.h"

#include <qvariant.h>
#include </home/nremy/SCRF/GsTLAppli/src/extra/qtplugins/clone_groupbox.h>
#include <qcombobox.h>
#include <qpushbutton.h>
#include <qspinbox.h>
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
    resize( 256, 521 ); 
    setCaption( trUtf8( "Form1" ) );
    Form1Layout = new QVBoxLayout( this, 11, 6, "Form1Layout"); 

    Layout1 = new QHBoxLayout( 0, 0, 6, "Layout1"); 

    SpinBox1 = new QSpinBox( this, "SpinBox1" );
    SpinBox1->setMinValue( 1 );
    Layout1->addWidget( SpinBox1 );
    QSpacerItem* spacer = new QSpacerItem( 61, 0, QSizePolicy::Expanding, QSizePolicy::Minimum );
    Layout1->addItem( spacer );
    Form1Layout->addLayout( Layout1 );

    CloneGroupBox1 = new CloneGroupBox( this, "CloneGroupBox1" );
    CloneGroupBox1->setFrameShape( CloneGroupBox::Box );
    CloneGroupBox1->setFrameShadow( CloneGroupBox::Sunken );
    CloneGroupBox1->setTitle( trUtf8( "" ) );
    CloneGroupBox1Layout = new QVBoxLayout( CloneGroupBox1, 11, 6, "CloneGroupBox1Layout"); 

    ComboBox1 = new QComboBox( FALSE, CloneGroupBox1, "ComboBox1" );
    CloneGroupBox1Layout->addWidget( ComboBox1 );
    Form1Layout->addWidget( CloneGroupBox1 );

    // signals and slots connections
    connect( SpinBox1, SIGNAL( valueChanged(int) ), CloneGroupBox1, SLOT( set_count(int) ) );
}

/*  
 *  Destroys the object and frees any allocated resources
 */
Form1::~Form1()
{
    // no need to delete child widgets, Qt does it all for us
}

