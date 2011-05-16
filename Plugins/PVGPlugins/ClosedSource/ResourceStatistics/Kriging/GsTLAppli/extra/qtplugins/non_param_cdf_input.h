#ifndef NONPARAMCDFINPUT_H
#define NONPARAMCDFINPUT_H

#include <GsTLAppli/extra/qtplugins/common.h>

#include <qwidget.h>

#include <vector>

class QComboBox;
class PropertySelector;
class FileChooser;
class QLabel;
class QLineEdit;
class QRadioButton;


class QTPLUGINS_DECL TailCdfInput : public QWidget {

  Q_OBJECT

 public:
  TailCdfInput( QWidget* parent = 0, const char* name = 0 );
  virtual ~TailCdfInput() {}

  QString getFunctionType() const;
  float getOmega() const;
  float getExtremeValue() const;
  void setOmega( QString minStr );
  void setExtremeValue( QString omegaStr );
  void setFunctionType(QString functionType);

 public slots:
  void setExtremeBoxEnabbled(int index);
  void setOmegaBoxEnabbled(int index);

protected :
	QComboBox* function_type;
  QLabel* min_max_label_;
  QLineEdit* extremeValue;
  QLabel* omega_label;
  QLineEdit* omega;

  std::vector<QString> function_type_index;
};


class QTPLUGINS_DECL LowerTailCdfInput : public TailCdfInput {

  Q_OBJECT

 public:
  LowerTailCdfInput( QWidget* parent = 0, const char* name = 0 );
  virtual ~LowerTailCdfInput() {}
};


class QTPLUGINS_DECL UpperTailCdfInput : public TailCdfInput {

  Q_OBJECT

 public:
  UpperTailCdfInput( QWidget* parent = 0, const char* name = 0 );
  virtual ~UpperTailCdfInput() {}
};



//======================================

class QTPLUGINS_DECL NonParamCdfInput : public QWidget {

  Q_OBJECT

 public:
  NonParamCdfInput( QWidget* parent = 0, const char* name = 0 );
  virtual ~NonParamCdfInput() {}

  QString getRefGridName() const;
  QString getRefPropName() const;
  QString getRefFileName() const;
  bool isRefOnFile();
  bool isRefOnGrid();
  bool isTieBreaking();
  void setRefGridName(QString gridName);
  void setRefPropName(QString propName);
  void setRefFileName(QString fileName);
  void setRefOnFile(bool on);
  void setRefOnGrid(bool on);
  void setTieBreaking(bool on );
	LowerTailCdfInput* LTI;
	UpperTailCdfInput* UTI;
public slots:
  void setReferenceGrid( const QString& grid_name);
  void setReferenceProperty(const QString& prop_name);

protected :
	PropertySelector* refPropSelector;
	FileChooser*  refFileChooser;
  QRadioButton* from_file; 
  QRadioButton* from_grid;
  QRadioButton* break_ties;

};


#endif

