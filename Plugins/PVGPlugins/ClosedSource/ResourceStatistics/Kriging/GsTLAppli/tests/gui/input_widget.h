#ifndef __GSTLAPPLI_INPUT_WIDGET_H__
#define __GSTLAPPLI_INPUT_WIDGET_H__

#include <qscrollview.h>
#include <qpushbutton.h>
#include <qstring.h>
#include <qlayout.h>

#include <iostream>

class Input_widget : public QWidget {

  Q_OBJECT

  public:
    Input_widget( const QString& uifile, QWidget *parent=0, const char *name=0 );
    ~Input_widget();
  
  public slots:
    void output_content();
    
  private:
    
    QWidget* dynamic_widget_;
    QScrollView* scrollview_;
    QPushButton* run_;
};

#endif
