#include "pqEPSReader.h"

// Qt includes

pqEPSReader::pqEPSReader(pqProxy* pxy, QWidget* p) :
  pqLoadedFormObjectPanel(":/StableReaders/EPSReader/pqEPSReader.ui", pxy, p)
{     
  this->linkServerManagerProperties();
}

pqEPSReader::~pqEPSReader()
{
}

void pqEPSReader::accept()
{
  // accept widgets controlled by the parent class
  pqLoadedFormObjectPanel::accept();
}

void pqEPSReader::reset()
{
  // reset widgets controlled by the parent class
  pqLoadedFormObjectPanel::reset();
}

void pqEPSReader::linkServerManagerProperties()
{
  // parent class hooks up some of our widgets in the ui
  pqLoadedFormObjectPanel::linkServerManagerProperties();
}


