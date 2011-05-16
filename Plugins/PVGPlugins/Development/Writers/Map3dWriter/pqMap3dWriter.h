#ifndef _pqMap3dWriter_h
#define _pqMap3dWriter_h

#include "pqLoadedFormObjectPanel.h"
#include "pqComponentsExport.h"
#include <QActionEvent>
#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QComboBox>
#include <QWidget>
#include <QDoubleSpinBox>


class pqMap3dWriter : public pqLoadedFormObjectPanel  {
  Q_OBJECT
public:
  /// constructor
  pqMap3dWriter(pqProxy* proxy, QWidget* p = NULL);
  /// destructor
  ~pqMap3dWriter();
  virtual void accept();
  virtual void reset();
  //virtual void updateInformationAndDomains();


protected:
  /// populate widgets with properties from the server manager
	virtual void linkServerManagerProperties();

	QComboBox *MiningBlock;


};

#endif

