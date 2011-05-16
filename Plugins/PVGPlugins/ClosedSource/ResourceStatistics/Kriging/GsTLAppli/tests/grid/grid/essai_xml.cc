#include <qdom.h>
#include <qfile.h>
#include <iostream>


int main() {

  QDomDocument doc("document");
  QFile f( "document.xml");
  if ( !f.open( IO_ReadOnly ) )
    return 0;
  if ( !doc.setContent(&f)) {
    f.close();
    return 0;
  }
  f.close();

  QDomElement docElem = doc.documentElement();

  QDomNode n = docElem.firstChild();
  while( !n.isNull() ) {
    QDomElement e = n.toElement();
    if( !e.isNull() ) {
      cout << e.tagName() << endl;
    }
    n = n.nextSibling();
  }	  

  return 0;
}
