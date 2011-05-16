#include "pqSumBillboardPanel.h"


pqSumBillboardPanel::pqSumBillboardPanel(pqProxy* pxy, QWidget* p) :
  pqLoadedFormObjectPanel(":/StableFilters/SumBillboard/SumBillboard.ui", pxy, p)
{  
  

  this->linkServerManagerProperties();
}

pqSumBillboardPanel::~pqSumBillboardPanel()
{
}

void pqSumBillboardPanel::accept()
{
  // accept widgets controlled by the parent class
  pqLoadedFormObjectPanel::accept();
}

void pqSumBillboardPanel::reset()
{
  // reset widgets controlled by the parent class
  pqLoadedFormObjectPanel::reset();
}

void pqSumBillboardPanel::linkServerManagerProperties()
{
  // parent class hooks up some of our widgets in the ui
  pqLoadedFormObjectPanel::linkServerManagerProperties();
}

