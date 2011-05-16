#ifndef _pqMine24DtoMap3D_h
#define _pqMine24DtoMap3D_h

#include "pqLoadedFormObjectPanel.h"
#include "pqComponentsExport.h"
#include <QActionEvent>
#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QComboBox>
#include <QWidget>
#include <QDoubleSpinBox>
#include <QCheckBox>
#include <QPushButton>
#include <QFileDialog>


class pqMine24DtoMap3D : public pqLoadedFormObjectPanel  {
  Q_OBJECT
public:
  /// constructor
  pqMine24DtoMap3D(pqProxy* proxy, QWidget* p = NULL);
  /// destructor
  ~pqMine24DtoMap3D();
  virtual void accept();
  virtual void reset();
  //virtual void updateInformationAndDomains();

  protected slots:
	  void withActivityMenu();
		void BrowseDialog();

protected:
  /// populate widgets with properties from the server manager
	virtual void linkServerManagerProperties();

	QDoubleSpinBox *AngleTolerance;
	QLineEdit *ActivityFile;
	QDoubleSpinBox *ActivityStep;
	QCheckBox *WithActivity;
	QComboBox *dateFormat;
	QPushButton *Browse;


};

#endif

