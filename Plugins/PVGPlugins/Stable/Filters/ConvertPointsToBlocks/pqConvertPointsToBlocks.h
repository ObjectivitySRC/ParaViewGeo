#ifndef _pqConvertPointsToBlocks_h
#define _pqConvertPointsToBlocks_h

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

class pqConvertPointsToBlocks : public pqLoadedFormObjectPanel  {
  Q_OBJECT
public:
  /// constructor
  pqConvertPointsToBlocks(pqProxy* proxy, QWidget* p = NULL);
  /// destructor
  ~pqConvertPointsToBlocks();
  virtual void accept();
  virtual void reset();

 protected slots:
  virtual  void updateGui();	

protected:
  /// populate widgets with properties from the server manager
	virtual void linkServerManagerProperties();

	QDoubleSpinBox *SizeCX;
	QDoubleSpinBox *SizeCY;
	QDoubleSpinBox *SizeCZ;

	QComboBox *XINC;	
	QComboBox *YINC;	
	QComboBox *ZINC;	

	QComboBox* xEntry;
	QComboBox* yEntry;
	QComboBox* zEntry;


};

#endif

