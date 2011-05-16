#ifndef _pqGeoReferencedImageReader_h
#define _pqGeoReferencedImageReader_h

#include "pqLoadedFormObjectPanel.h"
#include "pqComponentsExport.h"
#include "pqSMProxy.h"
class QComboBox;
class QGroupBox;
class QStackedWidget;
class pqImplicitPlaneWidget;
class pqBoxWidget;

class pqGeoReferencedImageReader:public pqLoadedFormObjectPanel  {
  Q_OBJECT

  typedef pqLoadedFormObjectPanel Superclass;
public:
  /// constructor
  pqGeoReferencedImageReader(pqProxy* proxy, QWidget* p = NULL);
  /// destructor
  ~pqGeoReferencedImageReader();

  virtual void accept();
  virtual void reset();
    
protected slots:
	virtual bool loadTexture();
protected:
  /// populate widgets with properties from the server manager
  virtual void linkServerManagerProperties();

	QString filename;
};

#endif

