#ifndef __GSTLAPPLI_TEST_QCONTROLS_H__
#define __GSTLAPPLI_TEST_QCONTROLS_H__

#include <GsTLAppli/gui/oinv_description/oinv_strati_grid.h>

#include <Inventor/nodes/SoGroup.h>

#include <qwidget.h>
#include <qcheckbox.h>
#include <qcombobox.h>
#include <qpushbutton.h>
#include <qslider.h>
#include <qlcdnumber.h>

class Controls : public QWidget {
  Q_OBJECT

public:
  Controls( SoGroup* root, Oinv_description* oinv_grid );

public slots:
  void paint_property( bool on );
  void show_grid( bool on );
  void change_cmap( int map );
  void add_to_display();
  void new_Z_slice();
  void toggle_view( bool );
  void move_slice_to( int );
 
private:
  QCheckBox* paint_;
  QCheckBox* show_;
  QComboBox* cmaps_;
  QPushButton* add_button_;
  QPushButton* new_Z_slice_;
  QCheckBox* slice_view_;
  QSlider* slice_pos_;
  QLCDNumber* slice_pos_label_;

  bool added_;
  Oinv_strati_grid* oinv_grid_;
  SoGroup* root_;
};


#endif
