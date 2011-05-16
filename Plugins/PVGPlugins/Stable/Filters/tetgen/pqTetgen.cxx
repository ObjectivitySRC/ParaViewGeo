//===================================================================================//
//				Mirarco Mining Innovation
//
// Filter:   Volumique Tetrahedralisation
// Class:    pqTetgen
// Author:   Nehmeh Bilal
// Director: Marie-Grabrielle Valet 
// Date:     July 2008
// contact: nehmebilal@gmail.com 
//====================================================================================//

#include "pqTetgen.h"
#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QComboBox>
#include <QWidget>
#include <QDoubleSpinBox>

	
pqTetgen::pqTetgen(pqProxy* pxy, QWidget* p) :
  pqLoadedFormObjectPanel(":/StableFilters/tetgen/pqTetgen.ui", pxy, p)
  {
	  this->RadiusEdgeRatio = this->findChild<QDoubleSpinBox*>("RadiusEdgeRatio");
	  this->MaxTetrahedronVolume = this->findChild<QDoubleSpinBox*>("MaxTetrahedronVolume");
	  this->tetgenCommand = this->findChild<QComboBox*>("tetgenCommand");
		this->MaxTetrahedronVolumeInBox = this->findChild<QDoubleSpinBox*>("MaxTetrahedronVolumeInBox");
		this->MaxTetrahedronVolumeInBox_2 = this->findChild<QDoubleSpinBox*>("MaxTetrahedronVolumeInBox2");
		this->WithRegions = this->findChild<QCheckBox*>("WithRegions");
		this->WithPredefinedRegions = this->findChild<QCheckBox*>("WithPredefinedRegions");
		this->WithBoundingBox = this->findChild<QCheckBox*>("WithBoundingBox");

		this->WithBBX1 = this->findChild<QCheckBox*>("WithBBX1");
		this->deltaX = this->findChild<QDoubleSpinBox*>("deltaX");
		this->deltaY = this->findChild<QDoubleSpinBox*>("deltaY");
		this->deltaZ = this->findChild<QDoubleSpinBox*>("deltaZ");

		this->WithBBX2 = this->findChild<QCheckBox*>("WithBBX2");
		this->deltaX_2 = this->findChild<QDoubleSpinBox*>("deltaX2");
		this->deltaY_2 = this->findChild<QDoubleSpinBox*>("deltaY2");
		this->deltaZ_2 = this->findChild<QDoubleSpinBox*>("deltaZ2");

		this->wbbx1 = this->findChild<QDockWidget*>("wbbx1");
		this->wbbx2 = this->findChild<QDockWidget*>("wbbx2");

		this->pbbx1 = this->findChild<QPushButton*>("pbbx1");
		this->pbbx2 = this->findChild<QPushButton*>("pbbx2");

		this->wbbx1->close();
		this->wbbx1->setShown(false);
		this->wbbx2->close();
		this->wbbx2->setShown(false);

		this->pClean = this->findChild<QPushButton*>("pClean");
		this->wClean = this->findChild<QDockWidget*>("wClean");
		this->wClean->close();

		this->ToleranceIsAbsolute = this->findChild<QCheckBox*>("ToleranceIsAbsolute");
		this->AbsoluteTolerance = this->findChild<QLineEdit*>("AbsoluteTolerance");
		this->Tolerance = this->findChild<QLineEdit*>("Tolerance");
		
		this->RegionArray = this->findChild<QComboBox*>("RegionArray");

		this->numberOfTetrahedron = this->findChild<QLineEdit*>("numberOfTetrahedronInfo");
		this->numberOfTetrahedron->setVisible(false);

		this->numberOfTetrahedronLabel = this->findChild<QLabel*>("numberOfTetrahedronLabel");

		QObject::connect(this->numberOfTetrahedron, SIGNAL(textChanged (const QString&)),
			this, SLOT(onNumberOfTetrahedronChanged(const QString&)),Qt::QueuedConnection);


	  QObject::connect(this->tetgenCommand, SIGNAL(currentIndexChanged(int)),
                   this, SLOT(updateMenu(void)));

		
/*
	  QObject::connect(this->MaxTetrahedronVolume, SIGNAL(valueChanged(double)),
                   this, SLOT(updateDoubleSpineBoxStep(void)));

	  QObject::connect(this->RadiusEdgeRatio, SIGNAL(valueChanged(double)),
                   this, SLOT(updateDoubleSpineBoxStep2(void)));
*/

		QObject::connect(this->WithRegions, SIGNAL(stateChanged(int)),this, SLOT(updateMenu_WithRegions(void)));
		QObject::connect(this->WithPredefinedRegions, SIGNAL(stateChanged(int)),this, SLOT(updateMenu_WithPredefinedRegions(void)));
		QObject::connect(this->WithBoundingBox, SIGNAL(stateChanged(int)),this, SLOT(updateMenu_WithBoundingBox(void)));
		QObject::connect(this->pbbx1, SIGNAL(clicked()),this, SLOT(showbbx1(void)));
		QObject::connect(this->pbbx2, SIGNAL(clicked()),this, SLOT(showbbx2(void)));

		QObject::connect(this->WithBBX1, SIGNAL(stateChanged(int)),this, SLOT(updateWbbx1(void)));
		QObject::connect(this->WithBBX2, SIGNAL(stateChanged(int)),this, SLOT(updateWbbx2(void)));

		QObject::connect(this->pClean, SIGNAL(clicked()),this, SLOT(showClean(void)));
		QObject::connect(this->ToleranceIsAbsolute, SIGNAL(stateChanged(int)),this, SLOT(updateToleranceIsAbsolute(void)));
		
		this->RadiusEdgeRatio->setDisabled(true);
		this->MaxTetrahedronVolume->setDisabled(true);
		this->WithRegions->setDisabled(true);
		this->WithPredefinedRegions->setDisabled(true);
		this->WithBoundingBox->setDisabled(true);
		this->MaxTetrahedronVolumeInBox->setDisabled(true);
		this->RegionArray->setDisabled(true);
		this->pbbx1->setDisabled(true);
		this->pbbx2->setDisabled(true);

		
		this->deltaX->setDisabled(true);
		this->deltaY->setDisabled(true);
		this->deltaZ->setDisabled(true);

		this->deltaX_2->setDisabled(true);
		this->deltaY_2->setDisabled(true);
		this->deltaZ_2->setDisabled(true);

		this->AbsoluteTolerance->setDisabled(true);
		
		
	  this->linkServerManagerProperties();	
}

pqTetgen::~pqTetgen()
{
}

void pqTetgen::accept()
{
  // accept widgets controlled by the parent class
	pqLoadedFormObjectPanel::accept();
}

void pqTetgen::reset()
{
  // reset widgets controlled by the parent class
	pqLoadedFormObjectPanel::reset();
}

void pqTetgen::linkServerManagerProperties()
{
  // parent class hooks up some of our widgets in the ui
	pqLoadedFormObjectPanel::linkServerManagerProperties();
}

void pqTetgen::updateMenu()
{
	switch(this->tetgenCommand->currentIndex())
	{
	case 1:
	case 2:
		this->WithRegions->setDisabled(false);
		this->WithPredefinedRegions->setDisabled(false);
		this->WithBoundingBox->setDisabled(false);
		this->RadiusEdgeRatio->setDisabled(true);
		this->MaxTetrahedronVolume->setDisabled(true);
		this->MaxTetrahedronVolumeInBox->setDisabled(true);
		this->MaxTetrahedronVolumeInBox_2->setDisabled(true);

		if(!(this->WithBoundingBox->isChecked()))
		{
			this->pbbx1->setDisabled(true);
			this->pbbx2->setDisabled(true);
		}
		break;

	case 3:
		this->RadiusEdgeRatio->setDisabled(false);
		this->RadiusEdgeRatio->setFocus();
		this->RadiusEdgeRatio->selectAll();
		this->MaxTetrahedronVolume->setDisabled(true);
		this->WithRegions->setDisabled(false);
		this->WithPredefinedRegions->setDisabled(false);
		this->WithBoundingBox->setDisabled(false);
		this->MaxTetrahedronVolumeInBox->setDisabled(true);
		this->MaxTetrahedronVolumeInBox_2->setDisabled(true);

		if(!(this->WithBoundingBox->isChecked()))
		{
			this->pbbx1->setDisabled(true);
			this->pbbx2->setDisabled(true);
		}

		break;

	case 4:
		this->MaxTetrahedronVolume->setDisabled(false);
		this->MaxTetrahedronVolume->setFocus();
		this->MaxTetrahedronVolume->selectAll();
		this->RadiusEdgeRatio->setDisabled(true);
		this->WithRegions->setDisabled(false);
		this->WithPredefinedRegions->setDisabled(false);
		this->WithBoundingBox->setDisabled(false);

		if(!(this->WithBoundingBox->isChecked()))
		{
			this->pbbx1->setDisabled(true);
			this->pbbx2->setDisabled(true);
			this->MaxTetrahedronVolumeInBox->setDisabled(true);
			this->MaxTetrahedronVolumeInBox_2->setDisabled(true);
		}
		if((this->WithBoundingBox->isChecked()) && (this->WithPredefinedRegions->isChecked()))
		{
			if(this->WithBBX1->isChecked())
				this->MaxTetrahedronVolumeInBox->setDisabled(false);
			if(this->WithBBX2->isChecked())
				this->MaxTetrahedronVolumeInBox_2->setDisabled(false);
		}
		break;

	case 5:
		this->MaxTetrahedronVolume->setDisabled(false);
		this->RadiusEdgeRatio->setDisabled(false);
		this->RadiusEdgeRatio->setFocus();
		this->RadiusEdgeRatio->selectAll();
		this->WithRegions->setDisabled(false);
		this->WithPredefinedRegions->setDisabled(false);
		this->WithBoundingBox->setDisabled(false);

		if(!(this->WithBoundingBox->isChecked()))
		{
			this->pbbx1->setDisabled(true);
			this->pbbx2->setDisabled(true);
			this->MaxTetrahedronVolumeInBox->setDisabled(true);
			this->MaxTetrahedronVolumeInBox_2->setDisabled(true);
		}
		if((this->WithBoundingBox->isChecked()) && (this->WithPredefinedRegions->isChecked()))
		{
			if(this->WithBBX1->isChecked())
				this->MaxTetrahedronVolumeInBox->setDisabled(false);
			if(this->WithBBX2->isChecked())
				this->MaxTetrahedronVolumeInBox_2->setDisabled(false);
		}
		break;

	default:
		this->RadiusEdgeRatio->setDisabled(true);
		this->MaxTetrahedronVolume->setDisabled(true);
		this->WithRegions->setDisabled(true);
		this->WithPredefinedRegions->setDisabled(true);
		this->WithBoundingBox->setDisabled(true);
		this->pbbx1->setDisabled(true);
		this->pbbx2->setDisabled(true);
		this->MaxTetrahedronVolumeInBox->setDisabled(true);
		this->MaxTetrahedronVolumeInBox_2->setDisabled(true);

		this->WithRegions->setChecked(false);
		this->WithPredefinedRegions->setChecked(false);
		this->WithBoundingBox->setChecked(false);
		
	}



}

void pqTetgen::updateDoubleSpineBoxStep()
{
	if(this->MaxTetrahedronVolume->value() < 6.0)
	{
		this->MaxTetrahedronVolume ->setSingleStep(1.0);
		if(this->MaxTetrahedronVolume->value() < 2.0)
		{
			this->MaxTetrahedronVolume ->setSingleStep(0.1);
			if(this->MaxTetrahedronVolume->value() < 0.2)
			{
				this->MaxTetrahedronVolume ->setSingleStep(0.01);
			}
		}

	}	
	else
		this->MaxTetrahedronVolume -> setSingleStep(5);
}


void pqTetgen::updateDoubleSpineBoxStep2()
{
	if(this->RadiusEdgeRatio->value() < 1.1)
	{
		this->RadiusEdgeRatio->setSingleStep(0.01);
	}
	else
		this->RadiusEdgeRatio->setSingleStep(0.1);

}

void pqTetgen::updateMenu_WithBoundingBox()
{
	if((this->WithBoundingBox->isChecked()))
	{
		this->pbbx1->setDisabled(false);
		this->pbbx2->setDisabled(false);

		if(this->WithPredefinedRegions->isChecked() && (this->tetgenCommand->currentIndex() > 3))
		{
			if(this->WithBBX1->isChecked())
				this->MaxTetrahedronVolumeInBox->setDisabled(false);
			if(this->WithBBX2->isChecked())
				this->MaxTetrahedronVolumeInBox_2->setDisabled(false);
		}

		else
		{
			this->MaxTetrahedronVolumeInBox->setDisabled(true);
			this->MaxTetrahedronVolumeInBox_2->setDisabled(true);
		}
	}
	else
	{
		this->pbbx1->setDisabled(true);
		this->pbbx2->setDisabled(true);
		this->WithBBX1->setChecked(false);
		this->WithBBX2->setChecked(false);
	}	
}

void pqTetgen::updateMenu_WithRegions()
{
	if(this->WithRegions->isChecked())
		this->WithPredefinedRegions->setChecked(false);
}

void pqTetgen::updateMenu_WithPredefinedRegions()
{
	if(this->WithPredefinedRegions->isChecked())
	{
		this->RegionArray->setDisabled(false);
		this->WithRegions->setChecked(false);
		if(this->WithBoundingBox->isChecked()&& (this->tetgenCommand->currentIndex() > 3))
		{
			if(this->WithBBX1->isChecked())
				this->MaxTetrahedronVolumeInBox->setDisabled(false);
			if(this->WithBBX2->isChecked())
				this->MaxTetrahedronVolumeInBox_2->setDisabled(false);
		}
	}
	else
	{
		this->RegionArray->setDisabled(true);
		if(this->WithBoundingBox->isChecked())
		{
			this->MaxTetrahedronVolumeInBox->setDisabled(true);
			this->MaxTetrahedronVolumeInBox_2->setDisabled(true);
		}
	}
}

void pqTetgen::showbbx1()
{
	this->wbbx1->setFloating(true);
	//this->wbbx1->showMaximized();
	this->wbbx1->setShown(true);
	this->wbbx1->setWindowTitle("Bounding Box 1");
}


void pqTetgen::showbbx2()
{
	this->wbbx2->setFloating(true);
	this->wbbx2->setShown(true);
	this->wbbx2->setWindowTitle("Bounding Box 2");
}



void pqTetgen::updateWbbx1()
{
	if(this->WithBBX1->isChecked())
	{
		this->deltaX->setDisabled(false);
		this->deltaY->setDisabled(false);
		this->deltaZ->setDisabled(false);

		if(this->WithPredefinedRegions->isChecked() && (this->tetgenCommand->currentIndex() > 3))
		{
			this->MaxTetrahedronVolumeInBox->setDisabled(false);
		}
	}

	else
	{
		this->deltaX->setDisabled(true);
		this->deltaY->setDisabled(true);
		this->deltaZ->setDisabled(true);

		this->MaxTetrahedronVolumeInBox->setDisabled(true);

	}
}


void pqTetgen::updateWbbx2()
{
	if(this->WithBBX2->isChecked())
	{
		this->deltaX_2->setDisabled(false);
		this->deltaY_2->setDisabled(false);
		this->deltaZ_2->setDisabled(false);

		if(this->WithPredefinedRegions->isChecked() && (this->tetgenCommand->currentIndex() > 3))
		{
			this->MaxTetrahedronVolumeInBox_2->setDisabled(false);
		}
	}

	else
	{
		this->deltaX_2->setDisabled(true);
		this->deltaY_2->setDisabled(true);
		this->deltaZ_2->setDisabled(true);

		this->MaxTetrahedronVolumeInBox_2->setDisabled(true);

	}
}




void pqTetgen::showClean()
{
	this->wClean->setFloating(true);
	this->wClean->setShown(true);
	this->wClean->setWindowTitle("Clean Filter");
}



void pqTetgen::updateToleranceIsAbsolute()
{
	if(this->ToleranceIsAbsolute->isChecked())
	{
		this->AbsoluteTolerance->setDisabled(false);
		this->Tolerance->setDisabled(true);
	}
	else
	{
		this->AbsoluteTolerance->setDisabled(true);
		this->Tolerance->setDisabled(false);
	}
}



void pqTetgen::onNumberOfTetrahedronChanged(const QString& n)
{
	this->numberOfTetrahedronLabel->setText(n);
}