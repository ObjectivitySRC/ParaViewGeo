#include "pqMine24DtoMap3D.h"
#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QComboBox>
#include <QWidget>
#include <QDoubleSpinBox>
#include "vtkPoints.h"
//#include <QApplication>
//#include <QtGui>

	
pqMine24DtoMap3D::pqMine24DtoMap3D(pqProxy* pxy, QWidget* p) :
  pqLoadedFormObjectPanel(":/CustomWidget/pqMine24DtoMap3D.ui", pxy, p)
  {
		this->AngleTolerance = this->findChild<QDoubleSpinBox*>("AngleTolerance");
		this->WithActivity = this->findChild<QCheckBox*>("WithActivity");
		this->ActivityFile = this->findChild<QLineEdit*>("ActivityFile");
		this->ActivityStep = this->findChild<QDoubleSpinBox*>("ActivityStep");
		this->dateFormat = this->findChild<QComboBox*>("dateFormat");
		this->Browse = this->findChild<QPushButton*>("Browse");

		QObject::connect(this->WithActivity, SIGNAL(stateChanged(int)),
                   this, SLOT(withActivityMenu(void)));
		QObject::connect(this->Browse, SIGNAL(clicked()),
                   this, SLOT(BrowseDialog(void)));


		this->ActivityFile->setDisabled(true);
		this->ActivityStep->setDisabled(true);
		this->dateFormat->setDisabled(true);
		this->Browse->setDisabled(true);


	  this->linkServerManagerProperties();	
	}

pqMine24DtoMap3D::~pqMine24DtoMap3D()
{
}

void pqMine24DtoMap3D::accept()
{
	pqLoadedFormObjectPanel::accept();
}

void pqMine24DtoMap3D::reset()
{
  // reset widgets controlled by the parent class
	pqLoadedFormObjectPanel::reset();
}

void pqMine24DtoMap3D::linkServerManagerProperties()
{
  // parent class hooks up some of our widgets in the ui
	pqLoadedFormObjectPanel::linkServerManagerProperties();

}


void pqMine24DtoMap3D::withActivityMenu(void)
{
	if(this->WithActivity->isChecked())
	{
		this->ActivityFile->setDisabled(false);
		this->ActivityStep->setDisabled(false);
		this->dateFormat->setDisabled(false);
		this->Browse->setDisabled(false);
	}
	else
	{
		this->ActivityFile->setDisabled(true);
		this->ActivityStep->setDisabled(true);
		this->dateFormat->setDisabled(true);
		this->Browse->setDisabled(true);
	}

}

void pqMine24DtoMap3D::BrowseDialog(void)
{
  QString path;
  
  path = QFileDialog::getOpenFileName(this,"Choose a file to open",QString::null,QString::null);

	this->ActivityFile->setText( path );


}
