#ifndef _pqTimeThreshold_h
#define _pqTimeThreshold_h

#include "pqLoadedFormObjectPanel.h"
#include "pqComponentsExport.h"

class pqTimeThreshold : public pqLoadedFormObjectPanel {
  Q_OBJECT
public:
  /// constructor
  pqTimeThreshold(pqProxy* proxy, QWidget* p = NULL);
  /// destructor
  ~pqTimeThreshold();

  virtual void accept();
  virtual void reset();
    
protected:
  /// populate widgets with properties from the server manager
  virtual void linkServerManagerProperties();

};

#endif

