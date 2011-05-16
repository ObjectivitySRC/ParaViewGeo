#ifndef _pqDrillHoleReader_h
#define _pqDrillHoleReader_h

#include "pqNamedObjectPanel.h"
#include "vtkSmartPointer.h"


class vtkEventQtSlotConnect;
class vtkSMDoubleVectorProperty;

class pqGocadReader : public pqNamedObjectPanel{
  Q_OBJECT
public:
  //constructor
  pqGocadReader(pqProxy* proxy, QWidget* p = NULL);
  
	//destructor
  ~pqGocadReader();
	
  virtual void accept();
  virtual void reset();

public slots:
	void UpdateRGB();
  
protected:
	vtkSMDoubleVectorProperty *RGB;

	vtkSmartPointer<vtkEventQtSlotConnect> QVTKConnect;

  /// populate widgets with properties from the server manager
  virtual void linkServerManagerProperties();

};

#endif

