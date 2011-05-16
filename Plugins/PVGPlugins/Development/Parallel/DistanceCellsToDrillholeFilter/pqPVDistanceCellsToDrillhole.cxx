#include "pqPVDistanceCellsToDrillhole.h"
#include <pqFileDialog.h>
#include <QtDebug>
#include "vtkSMProperty.h"
#include "vtkSMStringVectorProperty.h"
#include "vtkSMProxy.h"
#include "vtkSMSourceProxy.h"


pqPVDistanceCellsToDrillhole::pqPVDistanceCellsToDrillhole(pqProxy* pxy, QWidget* p) :
  pqLoadedFormObjectPanel(":/CustomWidget/pqPVDistanceCellsToDrillhole.ui", pxy, p)
  {
		this->BrowseButton = this->findChild<QPushButton*>("BrowseButton");
		this->FileName = this->findChild<QLineEdit*>("FileName");
		//this->FileName->setWriteOnly(true);
		this->Nbclosest=   this->findChild<QSpinBox*>("Nbclosest");
	    this->MinDistance= this->findChild<QDoubleSpinBox*>("MinDistance");
	    this->ComputeDistanceTo=  this->findChild<QComboBox*>("ComputeDistanceTo");;

		QObject::connect(this->BrowseButton, SIGNAL(clicked()), this, SLOT(onBrowse()));		
		
		this->linkServerManagerProperties();	
	}

pqPVDistanceCellsToDrillhole::~pqPVDistanceCellsToDrillhole()
{
}

void pqPVDistanceCellsToDrillhole::accept()
{
	pqLoadedFormObjectPanel::accept();
}

void pqPVDistanceCellsToDrillhole::reset()
{
  // reset widgets controlled by the parent class
	pqLoadedFormObjectPanel::reset();
}

void pqPVDistanceCellsToDrillhole::linkServerManagerProperties()
{
  // parent class hooks up some of our widgets in the ui
	pqLoadedFormObjectPanel::linkServerManagerProperties();
}


void pqPVDistanceCellsToDrillhole::onBrowse()
{
  QString filters;
  filters =  "Fichier (*.txt)";
  QString path;

  pqFileDialog *fileDialog = new pqFileDialog(NULL,
      this, tr("Save Distance File:"), QString(), filters);

 // path= fileDialog->getSaveFileName(this, "Save A File", QString(), filters);

  fileDialog->setObjectName("FileSaveDistance");
  fileDialog->setFileMode(pqFileDialog::AnyFile);

  if (fileDialog->exec() == QDialog::Accepted)
	{
    path = fileDialog->getSelectedFiles()[0];
	}

	if(path == QString())
		return;

	this->FileName->setText(path);
}