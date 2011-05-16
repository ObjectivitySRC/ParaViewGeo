#include "pqMSCdhGenerator.h"

#include "pqFileDialog.h"

#include <QToolButton>
#include <QLineEdit>

//----------------------------------------------------------------------------
pqMSCdhGenerator::pqMSCdhGenerator(pqProxy* pxy, QWidget* p) :
  pqLoadedFormObjectPanel(":/CustomWidget/pqMSCdhGenerator.ui", pxy, p)
{
	QToolButton* browseCollarFile = this->findChild<QToolButton*>(
		"tb_browseCollarFile");
	QToolButton* browseOutputFile = this->findChild<QToolButton*>(
		"tb_browseOutputFile");

	QObject::connect(browseCollarFile, SIGNAL(clicked()), 
		this, SLOT(onBrowseCollarFile()));
	QObject::connect(browseOutputFile, SIGNAL(clicked()), 
		this, SLOT(onBrowseOutputFile()));

	this->linkServerManagerProperties();
}


//----------------------------------------------------------------------------
pqMSCdhGenerator::~pqMSCdhGenerator()
{
}



//----------------------------------------------------------------------------
void pqMSCdhGenerator::onBrowseCollarFile()
{
  QString filters;
  filters += "Collar Points File (*.csv)";
  //filters += ";;All files (*)";

  QString path;

	pqFileDialog *fileDialog = new pqFileDialog(NULL,
      this, tr("Open Collar Points File:"), QString(), filters);
  //fileDialog->setAttribute(Qt::WA_DeleteOnClose);
  fileDialog->setObjectName("CollarPointsFileDialog");
  fileDialog->setFileMode(pqFileDialog::ExistingFile);

  if (fileDialog->exec() == QDialog::Accepted)
	{
    path = fileDialog->getSelectedFiles()[0];
	}

	if(path == QString())
		return;

	this->findChild<QLineEdit*>("CollarFile")->setText(path);
}


//----------------------------------------------------------------------------
void pqMSCdhGenerator::onBrowseOutputFile()
{
  QString filters;
  filters += "MSC File (*.pmsc)";
  //filters += ";;All files (*)";

  QString path;

	pqFileDialog *fileDialog = new pqFileDialog(NULL,
      this, tr("Save MSC File to:"), QString(), filters);
  //fileDialog->setAttribute(Qt::WA_DeleteOnClose);
  fileDialog->setObjectName("MSCFileDialog");
  fileDialog->setFileMode(pqFileDialog::AnyFile);

  if (fileDialog->exec() == QDialog::Accepted)
	{
    path = fileDialog->getSelectedFiles()[0];
	}

	if(path == QString())
		return;

	this->findChild<QLineEdit*>("OutputFile")->setText(path);
}

//----------------------------------------------------------------------------
void pqMSCdhGenerator::accept()
{
	pqLoadedFormObjectPanel::accept();
}

//----------------------------------------------------------------------------
void pqMSCdhGenerator::reset()
{
  // reset widgets controlled by the parent class
	pqLoadedFormObjectPanel::reset();
}

//----------------------------------------------------------------------------
void pqMSCdhGenerator::linkServerManagerProperties()
{
  // parent class hooks up some of our widgets in the ui
	pqLoadedFormObjectPanel::linkServerManagerProperties();

}