#include "pqEqualAreaPropFilter.h"

pqEqualAreaPropFilter::pqEqualAreaPropFilter(pqProxy* pxy, QWidget* p) :
  pqLoadedFormObjectPanel(":/CustomWidget/pqEqualAreaPropFilter.ui", pxy, p)
{


  this->linkServerManagerProperties();
}

pqEqualAreaPropFilter::~pqEqualAreaPropFilter()
{
}

void pqEqualAreaPropFilter::accept()
{
  // accept widgets controlled by the parent class
  pqLoadedFormObjectPanel::accept();
}

void pqEqualAreaPropFilter::reset()
{
  // reset widgets controlled by the parent class
  pqLoadedFormObjectPanel::reset();
}

void pqEqualAreaPropFilter::linkServerManagerProperties()
{
  // parent class hooks up some of our widgets in the ui
  pqLoadedFormObjectPanel::linkServerManagerProperties();
}

