#ifndef _pqSOTResult_h
#define _pqSOTResult_h

#include "pqLoadedFormObjectPanel.h"
#include "pqComponentsExport.h"
#include "pqSMProxy.h"

class QComboBox;
class QGroupBox;
class QCheckBox;
class QStackedWidget;
class pqImplicitPlaneWidget;
class pqBoxWidget;
class QSpinBox;
class QLineEdit;
class QPushButton;
class vtkSMDoubleVectorProperty;

class pqSOTResult : public pqLoadedFormObjectPanel {
  Q_OBJECT
  Q_PROPERTY(pqSMProxy cutFunction READ cutFunction WRITE setCutFunction)

  typedef pqLoadedFormObjectPanel Superclass;
public:
  /// constructor
  pqSOTResult(pqProxy* proxy, QWidget* p = NULL);
  /// destructor
  ~pqSOTResult();

  virtual void accept();
  virtual void reset();
  virtual void select();
  virtual void deselect();

	void restoreGuiState();
	void setComboBoxIndex(QComboBox* comboBox, QString &text);

  /// returns the currently selected cut function.
  pqSMProxy cutFunction() const;

public slots:
  /// set the current cut function.
  void setCutFunction(pqSMProxy);
	void onResetBounds();

signals:
  void cutFunctionChanged();

protected slots:
  void toogleParallelProjection(int state);
	void onApplyPressed(const QString& text);
  void setup3DWidgetsIfNeeded();
  void onCutFunctionChanged();
	void onOriginChanged(const QString& text);
	void onTransXChanged(const QString& text);
	void onTransYChanged(const QString& text);
	void onTransZChanged(const QString& text);
	void onScaleXChanged(const QString& text);
	void onScaleYChanged(const QString& text);
	void onScaleZChanged(const QString& text);
	void onSliceTypeChanged(const QString& text);
    
protected:
  /// populate widgets with properties from the server manager
  virtual void linkServerManagerProperties();

	QComboBox *radiusVariable;
	QComboBox *distanceVariable;
	QComboBox *angleVariable;
	QComboBox *thVariable;
  QGroupBox *sliceTypeGroup;
  QComboBox *sliceType;
  QStackedWidget *sliceFrame;
  pqBoxWidget* boxWidget;
  pqImplicitPlaneWidget* planeWidget;

	double *bounds;

	QSpinBox *referenceCirclesSize;
	QSpinBox *distanceBetween; 

	QLineEdit *originX;
	QLineEdit *originY;
	QLineEdit *originZ;

	QLineEdit *normalX;
	QLineEdit *normalY;
	QLineEdit *normalZ;	

	QPushButton *useZNormal;

	QLineEdit* clipWidgetDistance;
	QLineEdit* planeDistanceVariable;

	vtkSMDoubleVectorProperty* distMin;
 	vtkSMDoubleVectorProperty* distMax;

	// Grab data from box widget
	QLineEdit* boxTransX;
	QLineEdit* boxTransY;
	QLineEdit* boxTransZ;
	QLineEdit* boxScaleX;
	QLineEdit* boxScaleY;
	QLineEdit* boxScaleZ;

	// Used to send box data to servers
	QLineEdit* bTransXVariable;
	QLineEdit* bTransYVariable;
	QLineEdit* bTransZVariable;
	QLineEdit* bScaleXVariable;
	QLineEdit* bScaleYVariable;
	QLineEdit* bScaleZVariable;

	QLineEdit* clipType;

	QCheckBox* showWidgetCB;

};

#endif

