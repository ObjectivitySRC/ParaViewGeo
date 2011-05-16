#ifndef __GSTLAPPLI_PLUGIN_CLONE_GROUPBOX_H__
#define __GSTLAPPLI_PLUGIN_CLONE_GROUPBOX_H__


#include <qgroupbox.h>

#include <vector>


class CloneGroupBox : public QGroupBox {

  Q_OBJECT

 public:
  CloneGroupBox( QWidget* parent = 0, const char* name = 0 );
  int count() const { return count_; }

 public slots:
  void set_count( int n );

 protected:
  int count_;
  std::vector< QWidget* > cloned_widgets_;
};


#endif
