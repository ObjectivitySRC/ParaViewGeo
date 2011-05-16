
#include <QLabel>
#include <QLineEdit>
#include <QLayout>
#include <QListWidget>


#include "pqPlotPropertiesOverTimePanel.h"


pqPlotPropertiesOverTimePanel::pqPlotPropertiesOverTimePanel(pqProxy* pxy, QWidget* p) :
  pqLoadedFormObjectPanel(":/StableFilters/PlotPropertiesOverTime/pqPlotPropertiesOverTimePanel.ui", pxy, p)
{		
	//link the rest of the objects in the xml
	this->linkServerManagerProperties();
}

pqPlotPropertiesOverTimePanel::~pqPlotPropertiesOverTimePanel()
{
}

void pqPlotPropertiesOverTimePanel::accept()
{  
	// accept widgets controlled by the parent class	
	pqLoadedFormObjectPanel::accept();
}

void pqPlotPropertiesOverTimePanel::reset()
{
  // reset widgets controlled by the parent class
  pqLoadedFormObjectPanel::reset();
}

void pqPlotPropertiesOverTimePanel::linkServerManagerProperties()
{
  // parent class hooks up some of our widgets in the ui
  pqLoadedFormObjectPanel::linkServerManagerProperties();
}

