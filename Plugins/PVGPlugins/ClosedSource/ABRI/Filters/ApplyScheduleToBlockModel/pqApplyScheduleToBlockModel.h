#ifndef _pqApplyScheduleToBlockModel_h
#define _pqApplyScheduleToBlockModel_h

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

class pqApplyScheduleToBlockModel : public pqLoadedFormObjectPanel  {
  Q_OBJECT
public:
  /// constructor
  pqApplyScheduleToBlockModel(pqProxy* proxy, QWidget* p = NULL);
  /// destructor
  ~pqApplyScheduleToBlockModel();
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

	int finishRow;
	int startRow;

	QComboBox *ScheduleSegID;
	QComboBox *finishPropertyName;

	vtkSmartPointer<vtkEventQtSlotConnect> QVTKConnect;
	vtkSMStringVectorProperty *serverSideSelection;


};

#endif

