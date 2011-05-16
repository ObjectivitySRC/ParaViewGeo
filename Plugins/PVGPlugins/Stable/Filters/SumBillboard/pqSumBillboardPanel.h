#ifndef _pqSumBillboardPanel_h
#define _pqSumBillboardPanel_h

#include "pqLoadedFormObjectPanel.h"
#include "pqComponentsExport.h"
class pqDoubleRangeWidget;

class pqSumBillboardPanel : public pqLoadedFormObjectPanel {
  Q_OBJECT
public:
  /// constructor
  pqSumBillboardPanel(pqProxy* proxy, QWidget* p = NULL);
  /// destructor
  ~pqSumBillboardPanel();

  virtual void accept();
  virtual void reset();


protected:
  /// populate widgets with properties from the server manager
  virtual void linkServerManagerProperties();

};

#endif

