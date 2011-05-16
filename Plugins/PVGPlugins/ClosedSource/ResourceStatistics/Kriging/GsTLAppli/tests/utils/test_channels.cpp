#include <GsTLAppli/utils/gstl_messages.h>

#include <qstring.h>

#include <iostream>


class Test_scribe1 : public Scribe {
public:
  virtual void write( const std::string& str );
};

void Test_scribe1::write( const std::string& str ) {
  std::cout << str ;
}


class Test_scribe2 : public Scribe {
public:
  virtual void write( const std::string& str ) {
    cerr << "(" << str << ")" ;
  }
};



Channel& operator << ( Channel& ch, const QString& str ) {
  return ( ch << str.latin1() );
}


int main() {
  Test_scribe1 scribe1;
  Test_scribe2 scribe2;
  scribe1.subscribe( GsTLcout );
  scribe2.subscribe( GsTLcout );
  
  QString str( "blabala" );
  GsTLcout << "43" << "\n" << str << "\n";

  cerr << endl << "unsubscribing" << endl << endl;
  scribe2.unsubscribe( GsTLcout );
  GsTLcout << "43\n" << str << "\n";

  scribe1.subscribe( GsTLlog );
  GsTLlog << "gstl log\n";
}
