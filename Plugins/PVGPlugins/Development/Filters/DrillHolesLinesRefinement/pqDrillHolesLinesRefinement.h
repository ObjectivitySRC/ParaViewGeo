#ifndef _pqDrillHolesLinesRefinement_h
#define _pqDrillHolesLinesRefinement_h

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


class pqDrillHolesLinesRefinement : public pqLoadedFormObjectPanel  {
  Q_OBJECT
public:
  /// constructor
  pqDrillHolesLinesRefinement(pqProxy* proxy, QWidget* p = NULL);
  /// destructor
  ~pqDrillHolesLinesRefinement();
  virtual void accept();
  virtual void reset();
  //virtual void updateInformationAndDomains();



  protected slots:
	  void MakeBoundsMenu();
	  void TypeOfRefinementMenu();

protected:
  /// populate widgets with properties from the server manager
	virtual void linkServerManagerProperties();

	QComboBox *TypeOfRefinement;
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
	QCheckBox *Capping;
	QSpinBox  *NumberOfSides;
	QSpinBox  *Radius;


};

#endif

