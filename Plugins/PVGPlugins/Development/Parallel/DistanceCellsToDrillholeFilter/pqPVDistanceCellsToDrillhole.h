#ifndef _pqPVDistanceCellsToDrillhole_h
#define _pqPVDistanceCellsToDrillhole_h

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

class pqPVDistanceCellsToDrillhole : public pqLoadedFormObjectPanel  {
  Q_OBJECT
public:
  /// constructor
  pqPVDistanceCellsToDrillhole(pqProxy* proxy, QWidget* p = NULL);
  /// destructor
  ~pqPVDistanceCellsToDrillhole();
  virtual void accept();
  virtual void reset();
 
	protected slots:
		void onBrowse();
		
protected:
  /// populate widgets with properties from the server manager
	virtual void linkServerManagerProperties();

	QPushButton *BrowseButton;
	QLineEdit *FileName;
    QSpinBox *Nbclosest;
	QDoubleSpinBox *MinDistance;
	QComboBox *ComputeDistanceTo;
	

};

#endif

