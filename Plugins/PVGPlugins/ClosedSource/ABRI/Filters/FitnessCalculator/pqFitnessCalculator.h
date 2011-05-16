/*=========================================================================
  Program:   Visualization Toolkit
  Module:    $ pqFitnessCalculator.h $
  Authors:   Nehme Bilal & Arolde VIDJINNAGNI 
  MIRARCO, Laurentian University
  Date:      
  Version:   0.1
=========================================================================*/


#ifndef _pqFitnessCalculator_h
#define _pqFitnessCalculator_h

#include "pqLoadedFormObjectPanel.h"
#include <QSyntaxHighlighter>
#include <QtDebug>

#include "vtkSmartPointer.h"

class vtkEventQtSlotConnect;
class QLineEdit;
class QDockWidget;
class QListWidget;
class QTextEdit;
class QComboBox;
class QDoubleSpinBox;
class QCheckBox;
class QSpinBox;
class QSlider;
class QTreeWidget;
class vtkSMStringVectorProperty;
class vtkSMIntVectorProperty;
class QTreeWidgetItem;

class Highlighter : public QSyntaxHighlighter
{
   Q_OBJECT

public:
   Highlighter(QTextDocument *parent)
     : QSyntaxHighlighter(parent)
 {
     HighlightingRule rule;

     keywordFormat.setForeground(Qt::darkBlue);
     keywordFormat.setFontWeight(QFont::Bold);
     QStringList keywordPatterns;
     keywordPatterns << "\\bchar\\b" << "\\bclass\\b" << "\\bconst\\b"
                     << "\\bdouble\\b" << "\\benum\\b" << "\\bexplicit\\b"
                     << "\\bfriend\\b" << "\\binline\\b" << "\\bint\\b"
                     << "\\blong\\b" << "\\bnamespace\\b" << "\\boperator\\b"
                     << "\\bprivate\\b" << "\\bprotected\\b" << "\\bpublic\\b"
                     << "\\bshort\\b" << "\\bsignals\\b" << "\\bsigned\\b"
                     << "\\bslots\\b" << "\\bstatic\\b" << "\\bstruct\\b"
                     << "\\btemplate\\b" << "\\btypedef\\b" << "\\btypename\\b"
                     << "\\bunion\\b" << "\\bunsigned\\b" << "\\bvirtual\\b"
                     << "\\bvoid\\b" << "\\bvolatile\\b" << "\\bdef\\b"
										 << "\\bimport\\b" << "\\bfor\\b" << "\\bself\\b"
										 << "\\bin\\b" << "\\blen\\b" << "\\band\\b"
										 << "\\breturn\\b" << "\\bprint\\b" << "\\bdict\\b";
     foreach (const QString &pattern, keywordPatterns) {
         rule.pattern = QRegExp(pattern);
         rule.format = keywordFormat;
         highlightingRules.append(rule);
     }
     classFormat.setFontWeight(QFont::Bold);
     classFormat.setForeground(Qt::darkMagenta);
     rule.pattern = QRegExp("\\bQ[A-Za-z]+\\b");
     rule.format = classFormat;
     highlightingRules.append(rule);

     singleLineCommentFormat.setForeground(Qt::red);
		 rule.pattern = QRegExp("#[^\n]*");
     rule.format = singleLineCommentFormat;
     highlightingRules.append(rule);

     multiLineCommentFormat.setForeground(Qt::red);

     quotationFormat.setForeground(Qt::darkGreen);
     rule.pattern = QRegExp("\".*\"");
     rule.format = quotationFormat;
     highlightingRules.append(rule);

     //functionFormat.setFontItalic(true);
     functionFormat.setForeground(Qt::blue);
     rule.pattern = QRegExp("\\b[A-Za-z0-9_]+(?=\\()");
     rule.format = functionFormat;
     highlightingRules.append(rule);

     commentStartExpression = QRegExp("\"\"\"");
     commentEndExpression = QRegExp("\"\"\"");
     //commentStartExpression = QRegExp("\"\"\"");
     //commentEndExpression = QRegExp("\"\"\"");
 }


protected:
   void highlightBlock(const QString &text)
 {
     foreach (const HighlightingRule &rule, highlightingRules) {
         QRegExp expression(rule.pattern);
         int index = expression.indexIn(text);
         while (index >= 0) {
             int length = expression.matchedLength();
             setFormat(index, length, rule.format);
             index = expression.indexIn(text, index + length);
         }
     }
     setCurrentBlockState(0);

     int startIndex = 0;
     if (previousBlockState() != 1)
         startIndex = commentStartExpression.indexIn(text);

     while (startIndex >= 0) {
         int endIndex = commentEndExpression.indexIn(text, startIndex);
         int commentLength;
         if (endIndex == -1) {
             setCurrentBlockState(1);
             commentLength = text.length() - startIndex;
         } else {
             commentLength = endIndex - startIndex
                             + commentEndExpression.matchedLength();
         }
         setFormat(startIndex, commentLength, multiLineCommentFormat);
         startIndex = commentStartExpression.indexIn(text, startIndex + commentLength);
		 }
	 }

private:
   struct HighlightingRule
   {
       QRegExp pattern;
       QTextCharFormat format;
   };
   QVector<HighlightingRule> highlightingRules;

   QRegExp commentStartExpression;
   QRegExp commentEndExpression;

   QTextCharFormat keywordFormat;
   QTextCharFormat classFormat;
   QTextCharFormat singleLineCommentFormat;
   QTextCharFormat multiLineCommentFormat;
   QTextCharFormat quotationFormat;
   QTextCharFormat functionFormat;
};








class pqFitnessCalculator : public pqLoadedFormObjectPanel  {
  Q_OBJECT
public:
  /// constructor
  pqFitnessCalculator(pqProxy* proxy, QWidget* p = NULL);
  /// destructor
  ~pqFitnessCalculator();
  virtual void accept();
  virtual void reset();
	void writePythonEditorToFile(QString& path);
	
	
  //virtual void updateInformationAndDomains();

  protected slots:

		void onBrowse();
		void onEditScript();
		void saveModificationToScript();
		void onSave();
		void onSaveAs();
		void onPreview();
		void updateFilesList();
		void markServerModified();
		void restoreGUI();
		void listFunctions();
		void functionsListClicked(QTreeWidgetItem* item, int column);
		


protected:
  /// populate widgets with properties from the server manager
	virtual void linkServerManagerProperties();

	QLineEdit* pythonPath;
	QDockWidget* pythonEditorDock;
	QListWidget * filesList;
	QTextEdit *pythonEditor;
	QTextEdit *previewTextEdit;
	Highlighter *highlighter;
	Highlighter *highlighter2;
	QString currentPath;
	QTreeWidget *functionsTree;
	vtkSMStringVectorProperty* stv;
	const char** elements;
	int numberOfFunctions;
	int selectedFunctions;

	QCheckBox* RegularBlock;
	vtkSmartPointer<vtkEventQtSlotConnect> VTKConnect;
};

#endif

