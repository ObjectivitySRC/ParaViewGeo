#ifndef _pqPDataMineWireFrameReader_h
#define _pqPDataMineWireFrameReader_h

#include "pqLoadedFormObjectPanel.h"
#include "pqComponentsExport.h"

#include <QModelIndex>
#include <QStandardItem>
#include "vtkSmartPointer.h"

class QLineEdit;
class QStandardItemModel;
class QTableView;
class vtkEventQtSlotConnect;

class InternalCheckableItem : public QObject, public QStandardItem{
	Q_OBJECT

public:
	InternalCheckableItem();  
  virtual ~InternalCheckableItem();
	virtual void setData(const QVariant &value, int role = Qt::UserRole + 1);


public slots:
	void unChecked();		
signals:
	void checked();
	void modified();

};

class pqPDataMineWireFrameReader : public pqLoadedFormObjectPanel {
  Q_OBJECT
public:
  /// constructor
  pqPDataMineWireFrameReader(pqProxy* proxy, QWidget* p = NULL);
  /// destructor
  ~pqPDataMineWireFrameReader();

  virtual void accept();
  virtual void reset();
    
public slots:
	void pointFileLoad();
	void topoFileLoad();
	void updateProperties();	

protected:
  /// populate widgets with properties from the server manager
  virtual void linkServerManagerProperties();

	bool FileLoad(QString &name, QString &filterText, QLineEdit *lineEdit);
  
	QLineEdit *PointFileName;
	QLineEdit *TopoFileName;
	QLineEdit *FileName;
	
	QString CurrentDirectory;

	QStandardItemModel *Model;
	QTableView *View;
	
	vtkSmartPointer<vtkEventQtSlotConnect> QVTKConnect;
};

#endif

