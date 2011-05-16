#ifndef _pqArgReader_h
#define _pqArgReader_h

#include "pqLoadedFormObjectPanel.h"
#include "pqComponentsExport.h"
#include <QActionEvent>
#include <QModelIndex>
#include <QStandardItem>
#include <QString>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QHeaderView>
#include <QFlags>
#include "vtkSmartPointer.h"

class vtkEventQtSlotConnect;
class vtkSMStringVectorProperty;

class pqArgReader : public pqLoadedFormObjectPanel {
  Q_OBJECT
public:
  /// constructor
  pqArgReader(pqProxy* proxy, QWidget* p = NULL);
  /// destructor
  ~pqArgReader();

  virtual void accept();
  virtual void reset();

protected slots:
		void onDMTableSelectionChanged(QTableWidgetItem * item);
		void updateTable();
		void onPUNOTableSelectionChanged(QTableWidgetItem * item);
		void onDeclineTableSelectionChanged(QTableWidgetItem * item);
	    
protected:
  /// populate widgets with properties from the server manager
  virtual void linkServerManagerProperties();

	//DM variables
	QTableWidget* DMTable;
	vtkSMStringVectorProperty *PArraySelection;
	const char** Arrays;
	int DMNumberOfArrays;

	//PUNO variables
	QTableWidget* PUNOTable;
	int PUNONumberOfArrays;

	//PUNO variables
	QTableWidget* DeclineTable;
	int DeclineNumberOfArrays;


	int nArrays;

	vtkSmartPointer<vtkEventQtSlotConnect> QVTKConnect;


};

#endif

