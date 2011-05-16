#ifndef _pqDrillHolesPointsRefinement_h
#define _pqDrillHolesPointsRefinement_h

#include "pqLoadedFormObjectPanel.h"
#include "pqComponentsExport.h"
#include <QActionEvent>
#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QCheckBox>
#include <QSpinBox>
#include <QComboBox>
#include <QWidget>
#include <QDoubleSpinBox>


class pqDrillHolesPointsRefinement : public pqLoadedFormObjectPanel  {
  Q_OBJECT
public:
  /// constructor
  pqDrillHolesPointsRefinement(pqProxy* proxy, QWidget* p = NULL);
  /// destructor
  ~pqDrillHolesPointsRefinement();
  virtual void accept();
  virtual void reset();
  //virtual void updateInformationAndDomains();



  protected slots:
	virtual  void MakeBoundsMenu();
	  
protected:
  /// populate widgets with properties from the server manager
	virtual void linkServerManagerProperties();

	QCheckBox *MakeBounds;
	QSpinBox *OriginCx;
	QSpinBox *OriginCy;
	QSpinBox *OriginCz;
	QSpinBox *SizeCx;
	QSpinBox *SizeCy;
	QSpinBox *SizeCz;
	QSpinBox *Dimensions;
	QSpinBox *MaximumLevel;
	QSpinBox *MinimumLevel;
	QSpinBox *SpanThreshold;


};

#endif

