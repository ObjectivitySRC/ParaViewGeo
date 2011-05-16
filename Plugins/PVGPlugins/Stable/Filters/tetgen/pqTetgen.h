/*
   ParaView is a free software; you can redistribute it and/or modify it
   under the terms of the ParaView license version 1.2. 

   See License_v1.2.txt for the full ParaView license.
   A copy of this license can be obtained by contacting
   Kitware Inc.
   28 Corporate Drive
   Clifton Park, NY 12065
   USA

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHORS OR
CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


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

#ifndef _pqTetgen_h
#define _pqTetgen_h

#include "pqLoadedFormObjectPanel.h"
#include "pqComponentsExport.h"
#include <QActionEvent>
#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QComboBox>
#include <QWidget>
#include <QDoubleSpinBox>
#include <QCheckBox>
#include <QDockWidget>
#include <QPushButton>



class pqTetgen : public pqLoadedFormObjectPanel  {
  Q_OBJECT
public:
  /// constructor
  pqTetgen(pqProxy* proxy, QWidget* p = NULL);
  /// destructor
  ~pqTetgen();
  virtual void accept();
  virtual void reset();

  protected slots:
	  void updateMenu();
		void updateDoubleSpineBoxStep();
		void updateDoubleSpineBoxStep2();
		void updateMenu_WithRegions();
		void updateMenu_WithPredefinedRegions();
		void updateMenu_WithBoundingBox();
		void showbbx1();
		void showbbx2();
		void updateWbbx1();
		void updateWbbx2();

		void showClean();
		void updateToleranceIsAbsolute();

		void onNumberOfTetrahedronChanged(const QString& n);


protected:
  /// populate widgets with properties from the server manager
	virtual void linkServerManagerProperties();
	QDoubleSpinBox *RadiusEdgeRatio;
	QDoubleSpinBox *MaxTetrahedronVolume;
	QDoubleSpinBox *MaxTetrahedronVolumeInBox;
	QDoubleSpinBox *MaxTetrahedronVolumeInBox_2;

	QCheckBox *WithBoundingBox;
	QDoubleSpinBox *deltaX;
	QDoubleSpinBox *deltaY;
	QDoubleSpinBox *deltaZ;

	QCheckBox *WithBBX1;
	QCheckBox *WithBBX2;

	QDoubleSpinBox *deltaX_2;
	QDoubleSpinBox *deltaY_2;
	QDoubleSpinBox *deltaZ_2;
	QCheckBox *WithRegions;
	QCheckBox *WithPredefinedRegions;

	QComboBox *tetgenCommand;
	QComboBox *RegionArray;
	QDockWidget *wbbx1;
	QDockWidget *wbbx2;
	QPushButton *pbbx1;
	QPushButton *pbbx2;

	QPushButton *pClean;
	QDockWidget *wClean;
	QCheckBox *ToleranceIsAbsolute;
	QLineEdit *AbsoluteTolerance;
	QLineEdit *Tolerance;

	QLabel *numberOfTetrahedronLabel;
	QLineEdit *numberOfTetrahedron;

};

#endif

