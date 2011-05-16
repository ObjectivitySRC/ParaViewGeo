/****************************************************************************
** Form interface generated from reading ui file 'test.ui'
**
** Created: Wed Jan 29 22:12:26 2003
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
class CloneGroupBox;
class QComboBox;
class QSpinBox;

class Form1 : public QDialog
{ 
    Q_OBJECT

public:
    Form1( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
    ~Form1();

    QSpinBox* SpinBox1;
    CloneGroupBox* CloneGroupBox1;
    QComboBox* ComboBox1;


protected:
    QVBoxLayout* Form1Layout;
    QHBoxLayout* Layout1;
    QVBoxLayout* CloneGroupBox1Layout;
};

#endif // FORM1_H
