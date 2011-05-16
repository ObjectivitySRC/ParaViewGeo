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
		this->AngleStep = this->findChild<QDoubleSpinBox*>("AngleStep");

		//----------------------CleanPolyData------------------------------------
		this->pClean = this->findChild<QPushButton*>("pClean");
		this->wClean = this->findChild<QDockWidget*>("wClean");
		this->wClean->close();

		this->ToleranceIsAbsolute = this->findChild<QCheckBox*>("ToleranceIsAbsolute");
		this->AbsoluteTolerance = this->findChild<QLineEdit*>("AbsoluteTolerance");
		this->Tolerance = this->findChild<QLineEdit*>("Tolerance");

		QObject::connect(this->pClean, SIGNAL(clicked()),this, SLOT(showClean(void)));
		QObject::connect(this->ToleranceIsAbsolute, SIGNAL(stateChanged(int)),this, SLOT(updateToleranceIsAbsolute(void)));
		this->AbsoluteTolerance->setDisabled(true);
		//------------------------------------------------------------------------



		QObject::connect(this->WithActivity, SIGNAL(stateChanged(int)),
                   this, SLOT(withActivityMenu(void)));
		QObject::connect(this->Browse, SIGNAL(clicked()),
                   this, SLOT(BrowseDialog(void)));


		this->ActivityFile->setDisabled(true);
		this->ActivityStep->setDisabled(true);
		this->dateFormat->setDisabled(true);
		this->Browse->setDisabled(true);
		this->AngleStep->setDisabled(true);
		this->AngleTolerance->setDisabled(true);


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

	this->ActivityFile->setText(path);
}


void pqMine24DtoMap3D::showClean()
{
	this->wClean->setFloating(true);
	this->wClean->setShown(true);
}



void pqMine24DtoMap3D::updateToleranceIsAbsolute()
{
	if(this->ToleranceIsAbsolute->isChecked())
	{
		this->AbsoluteTolerance->setDisabled(false);
		this->Tolerance->setDisabled(true);
	}
	else
	{
		this->AbsoluteTolerance->setDisabled(true);
		this->Tolerance->setDisabled(false);
	}
}