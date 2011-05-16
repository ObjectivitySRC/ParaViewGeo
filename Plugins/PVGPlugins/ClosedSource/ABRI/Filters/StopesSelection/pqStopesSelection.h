#ifndef _pqStopesSelection_h
#define _pqStopesSelection_h

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
#include <QLineEdit>
#include <vtkSMIntVectorProperty.h>
#include <QRadioButton>
#include <QLabel>
#include <QLineEdit>
#include <QToolButton>
#include <QGroupBox>
#include <QComboBox>
#include <QCheckBox>
#include <QDoubleSpinBox>
#include <QSignalMapper>
#include <QSlider>
#include <QMenu>

class pqStopesSelection : public pqLoadedFormObjectPanel  {
  Q_OBJECT
public:
  /// constructor
  pqStopesSelection(pqProxy* proxy, QWidget* p = NULL);
  /// destructor
  ~pqStopesSelection();
  virtual void accept();
  virtual void reset();

	protected slots:
		void onBrowse();		
		void SaveAsSlot();
		void SaveSlot();
		void onRadioButtonClicked();
  	/// slot called when any calculator button is pushed
    void buttonPressed(const QString& t);
		void updateVariables();
		void variableChosen(QAction* a);
		void disableResults(bool);
		void XINC_Click(int index);		


protected:
  /// populate widgets with properties from the server manager
	virtual void linkServerManagerProperties();
	void XML_browseButtonClicked(const char* name);
	void writeFile(const char* name);
	QLineEdit* fileNameLine;
	QString currentFile;
	QRadioButton *fileRadioButton;
	QRadioButton *manualButton;
	QLabel *fileLabel;
	QToolButton *fileButton;
	QGroupBox *manualGroup;
	QComboBox* densityInput;
	QDoubleSpinBox*	stopeWidth;
	QDoubleSpinBox* stopeDepth;
	QToolButton*	SaveAs;
	QToolButton*	Save;

	/********************calculator parameters***************************/
	QPushButton*  Vectors;
	QPushButton*  Scalars;
	QLineEdit*    Function;
	QToolButton*  xy;
	QToolButton*  v1v2;
	QToolButton*  Clear;
	QLineEdit*    FitnessArrayName;
	QLineEdit*    ReplacementValue;
	QCheckBox*    ReplaceInvalidResult;

	QMenu ScalarsMenu;
  QMenu VectorsMenu;

	QComboBox *XINC;	
	QComboBox *YINC;	
	QComboBox *ZINC;	

	char* path;
};

#endif

