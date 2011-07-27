#include "pqConvertPointsToBlocks.h"
#include <pqFileDialog.h>
#include <QtDebug>

pqConvertPointsToBlocks::pqConvertPointsToBlocks(pqProxy* pxy, QWidget* p) :
  pqLoadedFormObjectPanel(":/StableFilters/ConvertPointsToBlocks/pqConvertPointsToBlocks.ui", pxy, p)
  {
		this->SizeCX= this->findChild<QDoubleSpinBox*>("SizeCX");
		this->SizeCY= this->findChild<QDoubleSpinBox*>("SizeCY");
		this->SizeCZ= this->findChild<QDoubleSpinBox*>("SizeCZ");		
		this->XINC= this->findChild<QComboBox*>("XINC");
		this->YINC= this->findChild<QComboBox*>("YINC");
		this->ZINC= this->findChild<QComboBox*>("ZINC");
	this->xEntry = this->findChild<QComboBox*>("XEntry");
	this->yEntry = this->findChild<QComboBox*>("YEntry");
	this->zEntry = this->findChild<QComboBox*>("ZEntry");

	QObject::connect(this->xEntry, SIGNAL(currentIndexChanged(int)),this, SLOT(updateGui(void)));
	QObject::connect(this->yEntry, SIGNAL(currentIndexChanged(int)),this, SLOT(updateGui(void)));
	QObject::connect(this->zEntry, SIGNAL(currentIndexChanged(int)),this, SLOT(updateGui(void)));

	this->linkServerManagerProperties();

	this->updateGui();
	}

pqConvertPointsToBlocks::~pqConvertPointsToBlocks()
{
}

void pqConvertPointsToBlocks::accept()
{
	pqLoadedFormObjectPanel::accept();
}

void pqConvertPointsToBlocks::reset()
{
  // reset widgets controlled by the parent class
	pqLoadedFormObjectPanel::reset();
}

void pqConvertPointsToBlocks::linkServerManagerProperties()
{
  // parent class hooks up some of our widgets in the ui
	pqLoadedFormObjectPanel::linkServerManagerProperties();
}


void pqConvertPointsToBlocks::updateGui()
{	 	  
	if(this->xEntry->currentIndex() == 1)
	{
		this->XINC->setEnabled(true);
		this->SizeCX->setEnabled(false);
	}
	else
	{
		this->XINC->setEnabled(false);
		this->SizeCX->setEnabled(true);
	}

	if(this->yEntry->currentIndex() == 1)
	{
		this->YINC->setEnabled(true);
		this->SizeCY->setEnabled(false);
	}
	else
	{
		this->YINC->setEnabled(false);
		this->SizeCY->setEnabled(true);
}

	if(this->zEntry->currentIndex() == 1)
{
		this->ZINC->setEnabled(true);
		this->SizeCZ->setEnabled(false);
	}
	else
		{
		this->ZINC->setEnabled(false);
		this->SizeCZ->setEnabled(true);
		}
}
