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

/*=========================================================================
MIRARCO MINING INNOVATION
Author: Nehme Bilal (nehmebilal@gmail.com)
===========================================================================*/


#ifndef _pqSOTResult_h
#define _pqSOTResult_h

#include "pqLoadedFormObjectPanel.h"
#include "pqComponentsExport.h"
#include "pqSMProxy.h"
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QHeaderView>

class QComboBox;
class QGroupBox;
class QCheckBox;
class QStackedWidget;
class pqImplicitPlaneWidget;
class pqBoxWidget;
class QSpinBox;
class QLineEdit;
class QPushButton;
class vtkSMDoubleVectorProperty;

class vtkEventQtSlotConnect;
class vtkSMStringVectorProperty;

class pqSOTResult : public pqLoadedFormObjectPanel {
  Q_OBJECT
  Q_PROPERTY(pqSMProxy cutFunction READ cutFunction WRITE setCutFunction)

  typedef pqLoadedFormObjectPanel Superclass;
public:
  /// constructor
  pqSOTResult(pqProxy* proxy, QWidget* p = NULL);
  /// destructor
  ~pqSOTResult();

  virtual void accept();
  virtual void reset();
  virtual void select();
  virtual void deselect();

	void restoreGuiState();
	void setComboBoxIndex(QComboBox* comboBox, QString &text);

  /// returns the currently selected cut function.
  pqSMProxy cutFunction() const;

public slots:
  /// set the current cut function.
  void setCutFunction(pqSMProxy);
	void onResetBounds();
	void onDataTableSelectionChanged(QTableWidgetItem * item);
	void updateDataTable();


signals:
  void cutFunctionChanged();

protected slots:
  void toogleParallelProjection(int state);
	void onApplyPressed(const QString& text);
  void setup3DWidgetsIfNeeded();
  void onCutFunctionChanged();
	void onOriginChanged(const QString& text);
	void onTransXChanged(const QString& text);
	void onTransYChanged(const QString& text);
	void onTransZChanged(const QString& text);
	void onScaleXChanged(const QString& text);
	void onScaleYChanged(const QString& text);
	void onScaleZChanged(const QString& text);
	void onSliceTypeChanged(const QString& text);
    
protected:
  /// populate widgets with properties from the server manager
  virtual void linkServerManagerProperties();

	QComboBox *radiusVariable;
	QComboBox *distanceVariable;
	QComboBox *angleVariable;
	QComboBox *thVariable;
  QGroupBox *sliceTypeGroup;
  QStackedWidget *sliceFrame;
  pqBoxWidget* boxWidget;
 
	double *bounds;

	QSpinBox *referenceCirclesSize;
	QSpinBox *distanceBetween; 

	QLineEdit *originX;
	QLineEdit *originY;
	QLineEdit *originZ;

	QLineEdit *normalX;
	QLineEdit *normalY;
	QLineEdit *normalZ;	

	QPushButton *useZNormal;

	QLineEdit* clipWidgetDistance;
	QLineEdit* planeDistanceVariable;

	vtkSMDoubleVectorProperty* distMin;
 	vtkSMDoubleVectorProperty* distMax;

	// Grab data from box widget
	QLineEdit* boxTransX;
	QLineEdit* boxTransY;
	QLineEdit* boxTransZ;
	QLineEdit* boxScaleX;
	QLineEdit* boxScaleY;
	QLineEdit* boxScaleZ;

	// Used to send box data to servers
	QLineEdit* bTransXVariable;
	QLineEdit* bTransYVariable;
	QLineEdit* bTransZVariable;
	QLineEdit* bScaleXVariable;
	QLineEdit* bScaleYVariable;
	QLineEdit* bScaleZVariable;

	QCheckBox* showWidgetCB;

	const char** Arrays;
	vtkSMStringVectorProperty *PArraySelection;
	QTableWidget* DataTable;
	int NbArrays;

	vtkSMStringVectorProperty *headers;
	vtkSMStringVectorProperty *stringHeaders;

	vtkSmartPointer<vtkEventQtSlotConnect> QVTKConnect;


};

#endif

