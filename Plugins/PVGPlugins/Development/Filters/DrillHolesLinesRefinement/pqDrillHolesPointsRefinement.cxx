/*=========================================================================

  Program:   Visualization Toolkit
  Module:    $ pqDrillHolesPointsRefinementTo.cxx $
  Author:    Arolde VIDJINNAGNI
  MIRARCO, Laurentian University
  Date:      
  Version:   0.1
  =========================================================================*/
#include "pqDrillHolesPointsRefinement.h"
#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QComboBox>
#include <QWidget>
#include <QDoubleSpinBox>
#include <QCheckBox>
#include <QSpinBox>
#include "vtkPoints.h"
//#include <QApplication>
//#include <QtGui>

	
pqDrillHolesPointsRefinement::pqDrillHolesPointsRefinement(pqProxy* pxy, QWidget* p) :
  pqLoadedFormObjectPanel(":/CustomWidget/pqDrillHolesPointsRefinement.ui", pxy, p)
  {
	  this->MakeBounds = this->findChild<QCheckBox*>("MakeBounds");
	  this->Dimensions = this->findChild<QSpinBox*>("Dimensions");
	  this->MaximumLevel = this->findChild<QSpinBox*>("MaximumLevel");
	  this->MinimumLevel = this->findChild<QSpinBox*>("MinimumLevel");	 
	  this->SpanThreshold = this->findChild<QSpinBox*>("SpanThreshold");
	  this->OriginCx = this->findChild<QSpinBox*>("OriginCx");
	  this->OriginCy = this->findChild<QSpinBox*>("OriginCy");
	  this->OriginCz = this->findChild<QSpinBox*>("OriginCz");
	  this->SizeCx = this->findChild<QSpinBox*>("SizeCx");
	  this->SizeCy = this->findChild<QSpinBox*>("SizeCy");
	  this->SizeCz = this->findChild<QSpinBox*>("SizeCz");
	  

	  this->OriginCx->setDisabled(true);//disable by default
	  this->OriginCy->setDisabled(true);
	  this->OriginCz->setDisabled(true);
	  this->SizeCx->setDisabled(true);
	  this->SizeCy->setDisabled(true);
	  this->SizeCz->setDisabled(true);
	
	  QObject::connect(this->MakeBounds, SIGNAL(stateChanged(int)),this, SLOT(MakeBoundsMenu(void)));

	  this->linkServerManagerProperties();	
}

pqDrillHolesPointsRefinement::~pqDrillHolesPointsRefinement()
{
}

void pqDrillHolesPointsRefinement::accept()
{
	pqLoadedFormObjectPanel::accept();
}

void pqDrillHolesPointsRefinement::reset()
{
  // reset widgets controlled by the parent class
	pqLoadedFormObjectPanel::reset();
}

void pqDrillHolesPointsRefinement::linkServerManagerProperties()
{
  // parent class hooks up some of our widgets in the ui
	pqLoadedFormObjectPanel::linkServerManagerProperties();

}

void pqDrillHolesPointsRefinement::MakeBoundsMenu()
{
	 	  
	if(this->MakeBounds->isChecked())
	{
	  this->OriginCx->setDisabled(true);//disable by default
	  this->OriginCy->setDisabled(true);
	  this->OriginCz->setDisabled(true);
	  this->SizeCx->setDisabled(true);
	  this->SizeCy->setDisabled(true);
	  this->SizeCz->setDisabled(true);
	}
	else
	{
	  this->OriginCx->setDisabled(false);//disable by default
	  this->OriginCy->setDisabled(false);
	  this->OriginCz->setDisabled(false);
	  this->SizeCx->setDisabled(false);
	  this->SizeCy->setDisabled(false);
	  this->SizeCz->setDisabled(false);
	}
}
