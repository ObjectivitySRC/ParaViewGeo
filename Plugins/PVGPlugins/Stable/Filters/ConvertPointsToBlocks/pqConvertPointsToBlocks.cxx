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
		this->UseSizes = this->findChild<QCheckBox*>("UseSizes");

	  this->SizeCX->setDisabled(true);
	  this->SizeCY->setDisabled(true);
	  this->SizeCZ->setDisabled(true);

		//search the index of the array in the XINC combox and use that to set the others
		if(this->XINC->count()>1)
		{
			this->YINC->setCurrentIndex(this->XINC->currentIndex()+1);
			this->ZINC->setCurrentIndex(this->XINC->currentIndex()+2);
		}
		QObject::connect(this->XINC, SIGNAL(currentIndexChanged(int)), this,
                 SLOT(XINC_Click(int)));

		QObject::connect(this->UseSizes, SIGNAL(stateChanged(int)),this, SLOT(UseSizesMenu(void)));
		this->linkServerManagerProperties();
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


void pqConvertPointsToBlocks::UseSizesMenu()
{	 	  
	if(this->UseSizes->isChecked())
	{
		this->XINC->setEnabled(true);
		this->YINC->setEnabled(true);
		this->ZINC->setEnabled(true);

	  this->SizeCX->setDisabled(true);
	  this->SizeCY->setDisabled(true);
	  this->SizeCZ->setDisabled(true);

	}
	else
	{
	  this->XINC->setDisabled(true);
	  this->YINC->setDisabled(true);
	  this->ZINC->setDisabled(true);

		this->SizeCX->setEnabled(true);
		this->SizeCY->setEnabled(true);
		this->SizeCZ->setEnabled(true);
	}
}

void pqConvertPointsToBlocks::XINC_Click(int index)
{
	if(this->XINC->count()>1)
		{
			this->YINC->setCurrentIndex(index+1);
			this->ZINC->setCurrentIndex(index+2);
		}
}
