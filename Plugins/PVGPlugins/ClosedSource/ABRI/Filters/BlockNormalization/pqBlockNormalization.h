#ifndef _pqBlockNormalization_h
#define _pqBlockNormalization_h

#include "pqLoadedFormObjectPanel.h"
#include "pqComponentsExport.h"
#include <QActionEvent>
#include <QLineEdit>
#include <QWidget>
#include <QPushButton>
#include <QFileDialog>
#include <QFlags>
#include <QFile>
#include <QTextStream>
#include <QString>

#include <QSet>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QHeaderView>
#include <QComboBox>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QRadioButton>
#include <QGroupBox>
#include <vtkSMIntVectorProperty.h>
#include <QCheckBox>

class pqBlockNormalization : public pqLoadedFormObjectPanel  {
  Q_OBJECT
public:
  /// constructor
  pqBlockNormalization(pqProxy* proxy, QWidget* p = NULL);
  /// destructor
  ~pqBlockNormalization();
  virtual void accept();
  virtual void reset();

protected slots:
  void XINC_Click(int index);		

protected:
  /// populate widgets with properties from the server manager
	virtual void linkServerManagerProperties();

	QDoubleSpinBox *SizeCX;
	QDoubleSpinBox *SizeCY;
	QDoubleSpinBox *SizeCZ;

	QComboBox *XINC;	
	QComboBox *YINC;	
	QComboBox *ZINC;	

};

#endif

