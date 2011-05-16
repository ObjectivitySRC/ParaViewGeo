#ifndef _pqEqualAreaPropFilter_h
#define _pqEqualAreaPropFilter_h

#include "pqLoadedFormObjectPanel.h"
#include "pqComponentsExport.h"

class pqEqualAreaPropFilter : public pqLoadedFormObjectPanel {
  Q_OBJECT
public:
  /// constructor
  pqEqualAreaPropFilter(pqProxy* proxy, QWidget* p = NULL);
  /// destructor
  ~pqEqualAreaPropFilter();

  virtual void accept();
  virtual void reset();

protected:
  /// populate widgets with properties from the server manager
  virtual void linkServerManagerProperties();

};

#endif

