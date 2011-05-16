#ifndef _pqDrillHolesAdvancedRefinement_h
#define _pqDrillHolesAdvancedRefinement_h

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


class pqDrillHolesAdvancedRefinement : public pqLoadedFormObjectPanel  {
  Q_OBJECT
public:
  /// constructor
  pqDrillHolesAdvancedRefinement(pqProxy* proxy, QWidget* p = NULL);
  /// destructor
  ~pqDrillHolesAdvancedRefinement();
  virtual void accept();
  virtual void reset();
  //virtual void updateInformationAndDomains();



  protected slots:
	 virtual  void MakeBoundsMenu();
	 virtual  void TypeOfRefinementMenu();

protected:
  /// populate widgets with properties from the server manager
	virtual void linkServerManagerProperties();

	QComboBox *TypeOfInterpolation;
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
	QSpinBox  *Distance;


};

#endif

