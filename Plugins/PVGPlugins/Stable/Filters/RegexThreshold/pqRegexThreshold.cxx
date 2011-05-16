#include "pqRegexThreshold.h"
#include "pqLoadedFormObjectPanel.h"

// Qt includes
#include <QLineEdit>
#include <QComboBox>

pqRegexThreshold::pqRegexThreshold(pqProxy* pxy, QWidget* p) :
  pqLoadedFormObjectPanel(":/StableFilters/RegexThreshold/pqRegexThreshold.ui", pxy, p)
{
	this->regLine = this->findChild<QLineEdit*>("Regex");

	this->scalarBox = this->findChild<QComboBox*>("SelectInputScalars");

	this->scalarCheck = this->findChild<QCheckBox*>("AllScalars");

	this->linkServerManagerProperties();
}

pqRegexThreshold::~pqRegexThreshold()
{
}

void pqRegexThreshold::accept()
{
  // accept widgets controlled by the parent class
  pqLoadedFormObjectPanel::accept();
}

void pqRegexThreshold::reset()
{
  // reset widgets controlled by the parent class
  pqLoadedFormObjectPanel::reset();
}

void pqRegexThreshold::linkServerManagerProperties()
{
  // parent class hooks up some of our widgets in the ui
  pqLoadedFormObjectPanel::linkServerManagerProperties();
}


