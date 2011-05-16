#ifndef _pqRegexThresholdPanel_h
#define _pqRegexThresholdPanel_h

#include "pqLoadedFormObjectPanel.h"
#include "pqComponentsExport.h"

#include <QLineEdit>
#include <QComboBox>
#include <QCheckBox>

class pqRegexThreshold : public pqLoadedFormObjectPanel {
  Q_OBJECT
public:
  /// constructor
  pqRegexThreshold(pqProxy* proxy, QWidget* p = NULL);
  /// destructor
  ~pqRegexThreshold();

  virtual void accept();
  virtual void reset();

protected:
  /// populate widgets with properties from the server manager
  virtual void linkServerManagerProperties();

  QLineEdit *regLine;
  QComboBox *scalarBox;
  QCheckBox *scalarCheck;
};

#endif

