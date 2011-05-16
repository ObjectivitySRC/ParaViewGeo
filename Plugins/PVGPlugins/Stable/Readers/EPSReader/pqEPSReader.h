#ifndef _pqEPSReaderPanel_h
#define _pqEPSReaderPanel_h

#include "pqLoadedFormObjectPanel.h"
#include "pqComponentsExport.h"

// MLIVINGSTONE
// LAME QT INCLUDE HACK/BUG
// If no QT objects are included, building will error with unresolved externals to QT
// NOTE: CANNOT USE class FORWARD DECLARATION WHEN BUILDING ALL UI AS SINGLE PLUGIN
#include <QLineEdit>
#include <QLabel>
#include <QTreeWidget>

class pqEPSReader : public pqLoadedFormObjectPanel {
  Q_OBJECT
public:
  /// constructor
  pqEPSReader(pqProxy* proxy, QWidget* p = NULL);
  /// destructor
  ~pqEPSReader();

  virtual void accept();
  virtual void reset();

protected:
  /// populate widgets with properties from the server manager
  virtual void linkServerManagerProperties();  
};

#endif

