#ifndef _pqDrillHoleReader_h
#define _pqDrillHoleReader_h

#include "pqLoadedFormObjectPanel.h"
#include "pqComponentsExport.h"
#include "pqPropertyLinks.h"
#include "pqSMSignalAdaptors.h"
#include "vtkSmartPointer.h"
#include <QStandardItem>
#include <QGroupBox>
#include <QCheckBox>

class QListWidget;
class QLineEdit;
class vtkEventQtSlotConnect;
class vtkSMStringVectorProperty;

class pqDrillHoleReader : public pqLoadedFormObjectPanel  {
  Q_OBJECT
public:
  /// constructor
  pqDrillHoleReader(pqProxy* proxy, QWidget* p = NULL);
  /// destructor
  ~pqDrillHoleReader();
	
  virtual void accept();
  virtual void reset();
  
public slots:
	void updateFilter( const QString& pattern);	
  void updateAccept( );
	void UpdateGUI();
	void EnableCellProp();
protected:

	QListWidget *view;		
	QLineEdit *filter;
	QCheckBox *CreatePointProp;
	QCheckBox *KeepCellProp;

	vtkSMStringVectorProperty *drillHoles;
	vtkSmartPointer<vtkEventQtSlotConnect> QVTKConnect;

  /// populate widgets with properties from the server manager
  virtual void linkServerManagerProperties();

};

#endif

