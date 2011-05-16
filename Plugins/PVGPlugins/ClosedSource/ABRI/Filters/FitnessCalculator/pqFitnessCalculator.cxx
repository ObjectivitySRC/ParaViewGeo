/*=========================================================================
  Program:   Visualization Toolkit
  Module:    $ pqFitnessCalculator.h $
  Authors:   Nehme Bilal & Arolde VIDJINNAGNI 
  MIRARCO, Laurentian University
  Date:      
  Version:   0.1
=========================================================================*/


#include "pqFitnessCalculator.h"
#include <QLineEdit>
#include <QComboBox>
#include <QWidget>
#include <QFileDialog>
#include <QToolButton>
#include <QPushButton>
#include <QDir>
#include <QStringList>
#include <QtDebug>
#include <QDockWidget>
#include <QDialogButtonBox>
#include <QListWidget>
#include <QListWidgetItem>
#include <QFile>
#include <QTextStream>
#include <QTextEdit>
#include <QCheckBox>
#include <QDoubleSpinBox>
#include "vtkSMIntVectorProperty.h"
#include "vtkSMStringVectorProperty.h"
#include "vtkSMProxy.h"
#include "pqProxy.h"
#include "pqPipelineSource.h"
#include "pqOutputPort.h"
#include "vtkPVDataInformation.h"
#include "vtkPVArrayInformation.h"
#include "vtkPVDataSetAttributesInformation.h"
#include "vtkEventQtSlotConnect.h"
#include <QTreeWidget>

#include "pqPipelineFilter.h"

#include <QDesktopWidget>
#include <QApplication>
#include <QtDebug>
#include <QSpinBox>
#include <QSlider>

#include "vtkSMProperty.h"
#include "vtkSMStringVectorProperty.h"
#include "vtkSMIntVectorProperty.h"
#include "vtkSMProxy.h"
#include "vtkSMSourceProxy.h"
//--------------------------------------------------------------------------------
pqFitnessCalculator::pqFitnessCalculator(pqProxy* pxy, QWidget* p) :
  pqLoadedFormObjectPanel(":/ABRIFilters/FitnessCalculator/pqFitnessCalculator.ui", pxy, p)
  {
	this->selectedFunctions = 0;
	this->pythonPath = this->findChild<QLineEdit*>("pythonPath");
	QToolButton *browse = this->findChild<QToolButton*>("browsePythonPath");
	QObject::connect(browse, SIGNAL(released()), this, SLOT(onBrowse()));

	QPushButton *editScriptButton = this->findChild<QPushButton*>("editScriptButton");
	QObject::connect(editScriptButton, SIGNAL(released()), this, SLOT(onEditScript()));

	this->pythonEditorDock = this->findChild<QDockWidget*>("dockWidget");
	this->pythonEditorDock->hide();

	QPushButton *saveAs = this->findChild<QPushButton*>("saveAs");
	QObject::connect(saveAs, SIGNAL(released()), this, SLOT(onSaveAs()));

	QPushButton *save = this->findChild<QPushButton*>("save");
	QObject::connect(save, SIGNAL(released()), this, SLOT(onSave()));

	QPushButton *discard = this->findChild<QPushButton*>("discard");
	QObject::connect(discard, SIGNAL(released()), pythonEditorDock, SLOT(close()));

	this->filesList = this->findChild<QListWidget*>("filesList");

	this->pythonEditor = this->findChild<QTextEdit*>("textEdit");
	highlighter = new Highlighter(pythonEditor->document());
	this->pythonEditor->setTabStopWidth(8);

	this->previewTextEdit = this->findChild<QTextEdit*>("previewTextEdit");
	highlighter2 = new Highlighter(previewTextEdit->document());

	this->functionsTree = this->findChild<QTreeWidget*>("functionsTree");	
	QObject::connect(functionsTree, SIGNAL(itemChanged(QTreeWidgetItem*, int)), this, SLOT(functionsListClicked(QTreeWidgetItem*, int)));
	stv = vtkSMStringVectorProperty::SafeDownCast(this->proxy()->GetProperty("functions"));

	this->RegularBlock= this->findChild<QCheckBox*>("RegularBlock");

	
	QObject::connect(this->filesList, SIGNAL(itemSelectionChanged()), this, SLOT(onPreview()));

	vtkSMStringVectorProperty *helperFile = vtkSMStringVectorProperty::SafeDownCast(
		this->proxy()->GetProperty("helperFile"));
	if(strlen(helperFile->GetElement(0)))
	{
		QString helperFileName(helperFile->GetElement(0));
		this->updateFilesList();
		for(int i=0; i < this->filesList->count(); i++)
		{
			if(helperFileName == this->filesList->item(i)->text())
			{
				this->filesList->setCurrentRow(i);
			}
		}
		//this->referenceProxy()->setModifiedState(pqProxy::UNMODIFIED);
	}

	QObject::connect(this->filesList, SIGNAL(itemSelectionChanged()), this, SLOT(markServerModified()));
	QObject::connect(this->pythonPath, SIGNAL(editingFinished ()), this, SLOT(updateFilesList()));


	this->VTKConnect = vtkSmartPointer<vtkEventQtSlotConnect>::New();
	this->VTKConnect->Connect(helperFile, vtkCommand::ModifiedEvent, 
	this, SLOT(restoreGUI()));

	/*
	QStringList inputArrays;
	pqPipelineSource* source = qobject_cast<pqPipelineFilter*>(this->referenceProxy())->getInput(0);

	pqOutputPort* port = source->getOutputPort(0);
	if(!port)
	{
		qDebug() << "port is null";
	}
	else
	{
		vtkPVDataSetAttributesInformation *pointArrayInfo = port->getDataInformation(0)->GetPointDataInformation();
		if(pointArrayInfo)
		{
			for(int i=0; i < pointArrayInfo->GetNumberOfArrays(); i++)
			{
				//if(pointArrayInfo->IsArrayAnAttribute(i) == vtkDataSetAttributes::SCALARS)
				//{
				inputArrays.append(pointArrayInfo->GetArrayInformation(i)->GetName());
				//}
			}
		}
	}

*/	

  this->linkServerManagerProperties();	
	}


//--------------------------------------------------------------------------------
pqFitnessCalculator::~pqFitnessCalculator()
{
	delete this->highlighter;
	delete this->highlighter2;
}



//--------------------------------------------------------------------------------
void pqFitnessCalculator::accept()
{
	if(this->selectedFunctions == 0)
	{
		qDebug()<<"please select at least one fitness function..."<<endl;
		return;
	}
	//this->referenceProxy()->setModifiedState(pqProxy::MODIFIED);
	QList<QListWidgetItem*> selected = this->filesList->selectedItems();
	if(selected.size() == 0)
	{
		qDebug()<<"please select the helper file"<<endl;
		return;
	}

	this->VTKConnect->Disconnect();
	if(this->selectedFunctions)
		stv->SetElements(this->selectedFunctions, this->elements);
	else
		stv->SetElements(1, this->elements);
	stv->SetImmediateUpdate(1);

	vtkSMStringVectorProperty *helperFileName = vtkSMStringVectorProperty::SafeDownCast(
			this->proxy()->GetProperty("helperFile"));
	helperFileName->SetElement(0, selected[0]->text().toAscii().data());
	helperFileName->SetImmediateUpdate(1);
	helperFileName->SetImmediateUpdate(0);

	vtkSMIntVectorProperty *modified = vtkSMIntVectorProperty::SafeDownCast(
		this->proxy()->GetProperty("modified"));
	modified->SetImmediateUpdate(1);
	modified->SetImmediateUpdate(0);

	this->VTKConnect->Connect(helperFileName, vtkCommand::ModifiedEvent, 
	this, SLOT(restoreGUI()));
	pqLoadedFormObjectPanel::accept();
}


//--------------------------------------------------------------------------------
void pqFitnessCalculator::reset()
{
  // reset widgets controlled by the parent class
	pqLoadedFormObjectPanel::reset();
}



//--------------------------------------------------------------------------------
void pqFitnessCalculator::linkServerManagerProperties()
{
  // parent class hooks up some of our widgets in the ui
	pqLoadedFormObjectPanel::linkServerManagerProperties();

}



//--------------------------------------------------------------------------------
void pqFitnessCalculator::restoreGUI()
{
	vtkSMStringVectorProperty *helperFile = vtkSMStringVectorProperty::SafeDownCast(
		this->proxy()->GetProperty("helperFile"));
	if(strlen(helperFile->GetElement(0)))
	{
		QString helperFileName(helperFile->GetElement(0));
		this->updateFilesList();
		for(int i=0; i < this->filesList->count(); i++)
		{
			if(helperFileName == this->filesList->item(i)->text())
			{
				this->filesList->setCurrentRow(i);
			}
		}
	}
	pqLoadedFormObjectPanel::accept();
}

//--------------------------------------------------------------------------------
void pqFitnessCalculator::onBrowse()
{
	currentPath = QFileDialog::getExistingDirectory(this,"Select Python Directory",currentPath);
	if(currentPath.isEmpty())
		return;

	this->pythonPath->setText(currentPath);
	this->updateFilesList();
}


//--------------------------------------------------------------------------------
void pqFitnessCalculator::updateFilesList()
{
	QDir pythonDir(this->pythonPath->text());
	QStringList filters;
	filters << "*.py";
	QStringList files = pythonDir.entryList(filters, QDir::Files);

	this->filesList->disconnect(this->filesList, SIGNAL(itemSelectionChanged()), this, SLOT(onPreview()));
	this->filesList->clear();
	bool requestDataExists = false;
	foreach(QString file, files)
	{
		if(file != "RequestData.py")
			this->filesList->addItem(file);	
		else
			requestDataExists = true;
	}

	QObject::connect(this->filesList, SIGNAL(itemSelectionChanged()), this, SLOT(onPreview()));
	QObject::connect(this->filesList, SIGNAL(itemSelectionChanged()), this, SLOT(listFunctions()));
	//QObject::connect(this->filesList, SIGNAL(itemSelectionChanged()), this, SLOT(markServerModified()));

	if(requestDataExists)
	{
		vtkSMStringVectorProperty *requestDataPath = vtkSMStringVectorProperty::SafeDownCast(
			this->proxy()->GetProperty("requestData"));
		requestDataPath->SetElement(0, "RequestData.py");
	}
	else
		qDebug()<<"Folder does not contain RequestData.py. Please choose another folder";

}


//--------------------------------------------------------------------------------
void pqFitnessCalculator::onEditScript()
{

	QList<QListWidgetItem*> selected = this->filesList->selectedItems();
	if(selected.size() == 0)
	{
		return;
	}

	QString path = this->pythonPath->text() + 
		"/" + selected[0]->text();

	QFile file(path);

	if(!file.open(QIODevice::ReadOnly))
	{
		qDebug() << "Failed to read the file";
		return;
	}

	QTextStream t( &file );
	QString code = t.readAll();
	this->pythonEditor->setText(code);

  QDesktopWidget desktop;
  QRect desktopRect = desktop.availableGeometry( desktop.primaryScreen() );

	int w = desktopRect.width()/2;
	int h = desktopRect.height()/2; 
	double x = desktopRect.x() + (w/2);
	double y = desktopRect.y() + (h/2);

	this->pythonEditorDock->setGeometry(x, y, w, h);

	this->pythonEditorDock->show();

	file.close();
}



//--------------------------------------------------------------------------------
void pqFitnessCalculator::onPreview()
{
	QString path = this->pythonPath->text() + 
		"/" + this->filesList->selectedItems()[0]->text();

	QFile file(path);

	if(!file.open(QIODevice::ReadOnly))
	{
		qDebug() << "Failed to read the file";
		return;
	}

	QTextStream t( &file );
	QString code = t.readLine() + "\n";
	code += t.readLine() + "\n";
	code += t.readLine() + "\n";
	code += t.readLine() + "\n";
	code += t.readLine() + "\n";
	code += t.readLine() + "\n";
	code += t.readLine() + "\n";
	code += t.readLine() ;

	this->previewTextEdit->setText(code);


	file.close();
}


//--------------------------------------------------------------------------------
void pqFitnessCalculator::saveModificationToScript()
{}


//--------------------------------------------------------------------------------
void pqFitnessCalculator::onSave()
{
	QString path = this->pythonPath->text() + 
		"/" + this->filesList->selectedItems()[0]->text();
	this->writePythonEditorToFile(path);
	this->pythonEditorDock->close();

	this->markServerModified();
}


//--------------------------------------------------------------------------------
void pqFitnessCalculator::onSaveAs()
{
	currentPath = QFileDialog::getSaveFileName(this->pythonEditorDock, "Save Script As", currentPath, QString("(*.py)"));
	if(currentPath != QString())
	{
		this->writePythonEditorToFile(currentPath);
	}
	this->updateFilesList();
}



//--------------------------------------------------------------------------------
void pqFitnessCalculator::writePythonEditorToFile(QString& path)
{
	QFile file(path);

	if(!file.open(QFile::WriteOnly | QFile::Truncate))
	{
		qDebug() << "Failed to read the file";
		return;
	}
	QTextStream out(&file);
	out << this->pythonEditor->toPlainText();
	file.close();
}



//--------------------------------------------------------------------------------
void pqFitnessCalculator::markServerModified()
{
	vtkSMIntVectorProperty *modified = vtkSMIntVectorProperty::SafeDownCast(
		this->proxy()->GetProperty("modified"));
	//modified->SetImmediateUpdate(1);
	modified->SetElement(0, -1*modified->GetElement(0));
	this->referenceProxy()->setModifiedState(pqProxy::MODIFIED);
}

void pqFitnessCalculator::listFunctions()
{
	this->numberOfFunctions = 0;
	this->functionsTree->clear();
	QString functionName;
	QList<QListWidgetItem*> selected = this->filesList->selectedItems();
	QFlags<Qt::ItemFlag> flg=Qt::ItemIsUserCheckable |
													Qt::ItemIsSelectable | Qt::ItemIsEnabled;
	if(selected.size() == 0)
	{
		return;
	}
		QString path = this->pythonPath->text() + "/" + selected[0]->text();

	QFile file(path);

	if(!file.open(QIODevice::ReadOnly))
	{
		qDebug() << "Failed to read the file";
		return;
	}

	QTextStream t( &file );
	QString scriptLine("");
	bool test = false;
	while(true)
	{
		scriptLine = t.readLine();
		if(t.atEnd())
		{
			break;
		}
		if(scriptLine.contains("def ") &&!scriptLine.contains("__") && scriptLine.contains("computeFitness"))
		{
			int start = scriptLine.indexOf("def")+3;
			while(scriptLine.at(start).isSpace() && start<scriptLine.length())
				start++;
			int end = scriptLine.indexOf("(")-1;
			while(!scriptLine.at(end).isLetterOrNumber() && end>=0)
				end--;
			if(start <= end)
			{
				functionName = "";
				for(int i=start ; i<= end ; i++)
					functionName.push_back(scriptLine.at(i));
			}
			QTreeWidgetItem *item = new QTreeWidgetItem(QStringList(functionName));
			this->functionsTree->addTopLevelItem(item);
			item->setFlags(flg);
			item->setCheckState(0, Qt::Unchecked);
			this->numberOfFunctions++;
		}

	}
	//this->elements = new const char*[this->numberOfFunctions];
	//char* element;
	//for(int i=0 ; i<this->numberOfFunctions ; i++)
	//{
	//	QString tempFunction = this->functionsTree->topLevelItem(i)->text(0);
	//	element = new char[tempFunction.length()+1];
	//	int j;
	//	for(j=0 ; j<tempFunction.length() ; j++)
	//	{
	//		element[j] = tempFunction[j].toAscii();
	//	}
	//	element[j] = '\0';
	//	elements[i] = element;
	//}
	this->elements = new const char*[1];
	this->elements[0] = "-";
	//this->numberOfFunctions = 0;
	this->selectedFunctions = 0;
	file.close();
}

void pqFitnessCalculator::functionsListClicked(QTreeWidgetItem* item, int column)
{
	column = this->functionsTree->indexOfTopLevelItem(item);
	if(item->checkState(0) == Qt::Checked)
	{
		this->selectedFunctions++;
	}
	else
	{
		this->selectedFunctions--;
	}
	if(this->selectedFunctions > 0)
	{
		this->elements = new const char*[this->selectedFunctions];
		char* element;
		int index = 0;		
		for(int i=0 ; i<this->numberOfFunctions ; i++)
		{
			QTreeWidgetItem* currentItem = this->functionsTree->topLevelItem(i);
			if(currentItem->checkState(0) == Qt::Checked)
			{
				QString tempFunction = this->functionsTree->topLevelItem(i)->text(0);
				element = new char[tempFunction.length()+1];
				int j;
				for(j=0 ; j<tempFunction.length() ; j++)
				{
					element[j] = tempFunction[j].toAscii();
				}
				element[j] = '\0';
				elements[index] = element;
				index++;
			}
		}
	}
	else if(this->selectedFunctions == 0)
	{
		this->elements = new const char*[1];
		this->elements[0] = "-";
	}
	this->referenceProxy()->setModifiedState(pqProxy::MODIFIED);
}