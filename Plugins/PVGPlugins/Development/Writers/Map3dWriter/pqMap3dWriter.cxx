#include "pqMap3dWriter.h"
#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QComboBox>
#include <QWidget>
#include <QDoubleSpinBox>
#include "vtkPoints.h"
//#include <QApplication>
//#include <QtGui>

	
pqMap3dWriter::pqMap3dWriter(pqProxy* pxy, QWidget* p) :
  pqLoadedFormObjectPanel(":/CustomWidget/pqMap3dWriter.ui", pxy, p)
  {

		this->MiningBlock = this->findChild<QComboBox*>("MiningBlk");
		this->MiningBlock->setDisabled(true);
	  this->linkServerManagerProperties();	
}

pqMap3dWriter::~pqMap3dWriter()
{
}

void pqMap3dWriter::accept()
{
	pqLoadedFormObjectPanel::accept();
}

void pqMap3dWriter::reset()
{
  // reset widgets controlled by the parent class
	pqLoadedFormObjectPanel::reset();
}

void pqMap3dWriter::linkServerManagerProperties()
{
  // parent class hooks up some of our widgets in the ui
	pqLoadedFormObjectPanel::linkServerManagerProperties();

}
