#ifndef _pqLineNetworkReader_h
#define _pqLineNetworkReader_h

#include "pqLoadedFormObjectPanel.h"
#include "pqComponentsExport.h"
#include <QSet>
#include "vtkSmartPointer.h"

class vtkEventQtSlotConnect;
class vtkSMStringVectorProperty;
class QTreeWidget;
class QComboBox;
class QTreeWidgetItem;
class string;



class pqLineNetworkReader : public pqLoadedFormObjectPanel  {
  Q_OBJECT
public:
  /// constructor
  pqLineNetworkReader(pqProxy* proxy, QWidget* p = NULL);
  /// destructor
  ~pqLineNetworkReader();

  virtual void accept();
  virtual void reset();

	void readPropertiesFromFile();
	bool isNumeric(std::string value);
	void restoreGuiState();
	void setComboBoxIndex(QComboBox* comboBox, QString &text);

protected slots:
	void onTreeSelectionChanged(QTreeWidgetItem * item, int column);
	void updateCoordComboBoxes(int index);
	void onPropertiesModified();


protected:

	QTreeWidget *tree;
	QComboBox *ArrowProp;
	QSet<QString> items;

	QComboBox *x1;
	QComboBox *y1;
	QComboBox *z1;
	QComboBox *x2;
	QComboBox *y2;
	QComboBox *z2;

	vtkSMStringVectorProperty* stv;
	vtkSmartPointer<vtkEventQtSlotConnect> VTKConnect;

	const char** elements;
	int numberOfArrays;

  /// populate widgets with properties from the server manager
  virtual void linkServerManagerProperties();



};

#endif

