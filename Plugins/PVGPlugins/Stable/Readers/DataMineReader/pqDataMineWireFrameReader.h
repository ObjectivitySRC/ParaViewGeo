#ifndef _pqDataMineWireFrameReader_h
#define _pqDataMineWireFrameReader_h

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

class pqDataMineWireFrameReader : public pqLoadedFormObjectPanel {
  Q_OBJECT
public:
  /// constructor
  pqDataMineWireFrameReader(pqProxy* proxy, QWidget* p = NULL);
  /// destructor
  ~pqDataMineWireFrameReader();

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

