#ifndef _pqPointSetReader_h
#define _pqPointSetReader_h

#include "pqLoadedFormObjectPanel.h"
#include "pqComponentsExport.h"
#include <QSet>
#include <QLineEdit>
#include <QTableWidget>
#include <QPushButton>
#include <vtkSMStringVectorProperty.h>
#include <vtkSmartPointer.h>

class QTreeWidget;
class QComboBox;
class QTreeWidgetItem;
class string;
class vtkEventQtSlotConnect;

class pqPointSetReader : public pqLoadedFormObjectPanel  {
  Q_OBJECT
public:
  /// constructor
  pqPointSetReader(pqProxy* proxy, QWidget* p = NULL);
  /// destructor
  ~pqPointSetReader();

  virtual void accept();
  virtual void reset();

	void readPropertiesFromFile();
	void readPreviewData();
	bool isNumeric(std::string value);
	void restoreGuiState();
	void setComboBoxIndex(QComboBox* comboBox, QString &text);

protected slots:
	void onTreeSelectionChanged(QTreeWidgetItem * item, int column);
	void updateCoordComboBoxes(int index);
	void onSepartorChanged( const QString & text);
	void togglePreviewTable();
	void updatePreviewTable();
	void onPropertiesModified();

protected:

	QTreeWidget *tree;
	QSet<QString> items;
	QLineEdit *separator;
	QLineEdit *noDataValue;
	QTableWidget *previewTable;
	QPushButton *previewButton;

	QComboBox *x1;
	QComboBox *y1;
	QComboBox *z1;
	bool showPreview;
	int previewRows;
	int previewCols;

	vtkSmartPointer<vtkEventQtSlotConnect> VTKConnect;

	vtkSMStringVectorProperty* stv;
	const char** elements;
	int numberOfArrays;
  /// populate widgets with properties from the server manager
  virtual void linkServerManagerProperties();



};

#endif

