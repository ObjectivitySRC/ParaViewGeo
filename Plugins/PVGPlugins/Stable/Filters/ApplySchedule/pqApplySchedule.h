
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


#ifndef _pqApplySchedule_h
#define _pqApplySchedule_h

#include "pqLoadedFormObjectPanel.h"
#include "pqComponentsExport.h"
#include <QActionEvent>
#include <QLineEdit>
#include <QWidget>
#include <QPushButton>
#include <QFileDialog>
#include <QFlags>
#include <QFile>
#include <QTextStream>
#include <QString>

#include <QSet>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QHeaderView>
#include <QComboBox>

#include "vtkSmartPointer.h"

class vtkEventQtSlotConnect;
class vtkSMStringVectorProperty;

class pqApplySchedule : public pqLoadedFormObjectPanel  {
  Q_OBJECT
public:
  /// constructor
  pqApplySchedule(pqProxy* proxy, QWidget* p = NULL);
  /// destructor
  ~pqApplySchedule();
  virtual void accept();
  virtual void reset();
  //virtual void updateInformationAndDomains();
	
	void restoreRowState(QString &value);

	protected slots:
		void onBrowse();
		void onTableSelectionChanged(QTableWidgetItem * item);
		void onFileNameChanged();
		void clearTimeColumn(int id);

		void onStateReloaded();
		void updateTable();
		

protected:
  /// populate widgets with properties from the server manager
	virtual void linkServerManagerProperties();

	QPushButton *BrowseButton;
	QLineEdit *fileName;
	//QLineEdit *ArraySelection;
	QTableWidget* table;
	QSet<QString> items;
	bool loading;

	vtkSMStringVectorProperty *ArraySelection;
	const char** arrays;
	int numberOfArrays;

	int durationRow;
	int startRow;

	QComboBox *ScheduleSegID;
	QComboBox *DurationPropertyName;

	vtkSmartPointer<vtkEventQtSlotConnect> QVTKConnect;
	vtkSMStringVectorProperty *serverSideSelection;


};

#endif

