#include "pqBlockNormalization.h"
#include <pqFileDialog.h>
#include <QtDebug>

pqBlockNormalization::pqBlockNormalization(pqProxy* pxy, QWidget* p) :
  pqLoadedFormObjectPanel(":/ABRIFilters/BlockNormalization/pqBlockNormalization.ui", pxy, p)
  {
		this->SizeCX= this->findChild<QDoubleSpinBox*>("NormalSizeCX");
		this->SizeCY= this->findChild<QDoubleSpinBox*>("NormalSizeCY");
		this->SizeCZ= this->findChild<QDoubleSpinBox*>("NormalSizeCZ");		
		this->XINC= this->findChild<QComboBox*>("XINC");
		this->YINC= this->findChild<QComboBox*>("YINC");
		this->ZINC= this->findChild<QComboBox*>("ZINC");
	
		//search the index of the array in the XINC combox and use that to set the others
		if(this->XINC->count()>1)
		{
			this->YINC->setCurrentIndex(this->XINC->currentIndex()+1);
			this->ZINC->setCurrentIndex(this->XINC->currentIndex()+2);
		}

		QObject::connect(this->XINC, SIGNAL(currentIndexChanged(int)), this,
                 SLOT(XINC_Click(int)));

		this->linkServerManagerProperties();
	}

pqBlockNormalization::~pqBlockNormalization()
{
}

void pqBlockNormalization::accept()
{
	pqLoadedFormObjectPanel::accept();
}

void pqBlockNormalization::reset()
{
  // reset widgets controlled by the parent class
	pqLoadedFormObjectPanel::reset();
}

void pqBlockNormalization::linkServerManagerProperties()
{
  // parent class hooks up some of our widgets in the ui
	pqLoadedFormObjectPanel::linkServerManagerProperties();
}


void pqBlockNormalization::XINC_Click(int index)
{
	if(this->XINC->count()>1)
		{
			this->YINC->setCurrentIndex(index+1);
			this->ZINC->setCurrentIndex(index+2);
		}
}

