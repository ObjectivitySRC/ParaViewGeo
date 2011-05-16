/****************************************************************************
** Form interface generated from reading ui file 'dynamic.ui'
**
** Created: Thu Sep 5 16:05:47 2002
**      by:  The User Interface Compiler (uic)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/
#ifndef FORM1_H
#define FORM1_H

#include <qvariant.h>
#include <qdialog.h>
class QVBoxLayout; 
class QHBoxLayout; 
class QGridLayout; 
class QButtonGroup;
class QComboBox;
class QFrame;
class QLabel;
class QLineEdit;
class QRadioButton;
class QSlider;

class Form1 : public QDialog
{ 
    Q_OBJECT

public:
    Form1( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
    ~Form1();

    QFrame* Frame6;
    QLabel* TextLabel1;
    QComboBox* GridSelector;
    QLabel* TextLabel1_2;
    QComboBox* PropertyName;
    QRadioButton* TransformData;
    QButtonGroup* ButtonGroup1;
    QRadioButton* FromData;
    QRadioButton* FromFile;
    QLineEdit* TransformFileName;
    QSlider* realization;


public slots:
    virtual void hideSlot();

};

#endif // FORM1_H
