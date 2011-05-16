#include <GsTLAppli/extra/qtplugins/clone_groupbox.h>
#include <GsTLAppli/utils/string_manipulation.h>
#include <GsTLAppli/utils/gstl_messages.h>

#include <qwidgetfactory.h>
#include <qobjectlist.h>

CloneGroupBox::CloneGroupBox( QWidget* parent, const char* name )
  : QGroupBox( 2, Qt::Vertical, parent, name ),
    count_( 1 ) {

}


void CloneGroupBox::set_count( int n ) {

  if( n == count_ ) return;

  cout << "hello" << endl;
  // only duplicate the first widget in the group box
  QObjectList* children = queryList( "QWidget", 0, TRUE, FALSE );
  QObject* obj = children->first() ;
  QWidget* current_widget = (QWidget*) obj;
  cout << "widget retrieved" << endl;
  if( !current_widget ) return;

  cout << "widget class: " << current_widget->className() << endl;

  if( n > count_ ) {
    setColumns( n );
    QWidgetFactory factory;
    int to_be_added = n - count_;
    for( int i = 0; i < to_be_added; i++ ) {
      //addSpace( 10 );
      count_++;
      QString id; 
      id.setNum( count_ );
      QString new_name( current_widget->name() );
      new_name += "_" + id ;
      cout << "new name: " << new_name << endl;
      QWidget* clone = factory.createWidget( current_widget->className(), 
					     this, new_name );
      
      if( !clone ) cout << "merde\n";
      clone->show();
      cloned_widgets_.push_back( clone );
    }
  }
  else {
    cout << "count_ =" << count_ << "   n=" << n 
	 << "   size=" << cloned_widgets_.size() << endl;

    for( int i=cloned_widgets_.size()-1; i>n-2; i-- ) {
      cout << "i=" << i << "   size=" <<  cloned_widgets_.size() << endl;
      cout << "deleting: " <<  cloned_widgets_[i]->name() << endl;
      delete cloned_widgets_[i];
    }
    count_ = n;
    cloned_widgets_.resize( n-1 );
    setColumns( count_ );
  }

  cout << "now count=" << count_ << endl;
}
  
