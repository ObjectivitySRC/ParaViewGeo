#include "pqTimeThreshold.h"

pqTimeThreshold::pqTimeThreshold(pqProxy* pxy, QWidget* p) :
  pqLoadedFormObjectPanel(":/StableFilters/TimeThreshold/pqTimeThreshold.ui", pxy, p)
{      

                   
  this->linkServerManagerProperties();
}

pqTimeThreshold::~pqTimeThreshold()
{
}

void pqTimeThreshold::accept()
{
  // accept widgets controlled by the parent class
  pqLoadedFormObjectPanel::accept();
}

void pqTimeThreshold::reset()
{
  // reset widgets controlled by the parent class
  pqLoadedFormObjectPanel::reset();
}

void pqTimeThreshold::linkServerManagerProperties()
{
  // parent class hooks up some of our widgets in the ui
  pqLoadedFormObjectPanel::linkServerManagerProperties();
}

