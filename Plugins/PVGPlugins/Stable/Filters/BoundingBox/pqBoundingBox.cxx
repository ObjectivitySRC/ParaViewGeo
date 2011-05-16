#include "pqBoundingBox.h"
#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QComboBox>
#include <QWidget>
#include <QDoubleSpinBox>
#include "vtkPoints.h"
//#include <QApplication>
//#include <QtGui>

	
pqBoundingBox::pqBoundingBox(pqProxy* pxy, QWidget* p) :
  pqLoadedFormObjectPanel(":/StableFilters/BoundingBox/pqBoundingBox.ui", pxy, p)
  {
	  this->linkServerManagerProperties();	
}

pqBoundingBox::~pqBoundingBox()
{
}

void pqBoundingBox::accept()
{
	pqLoadedFormObjectPanel::accept();
}

void pqBoundingBox::reset()
{
  // reset widgets controlled by the parent class
	pqLoadedFormObjectPanel::reset();
}

void pqBoundingBox::linkServerManagerProperties()
{
  // parent class hooks up some of our widgets in the ui
	pqLoadedFormObjectPanel::linkServerManagerProperties();

}
