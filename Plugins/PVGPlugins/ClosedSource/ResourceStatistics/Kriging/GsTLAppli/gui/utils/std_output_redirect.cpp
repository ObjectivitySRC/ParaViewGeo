#include <GsTLAppli/gui/utils/std_output_redirect.h>

#include <qtextedit.h>


GsTLbuf::GsTLbuf( QTextEdit* text_edit, int bsize)
  : std::streambuf(), text_edit_( text_edit ) {
  if (bsize) {
    char *ptr = new char[bsize];
    setp(ptr, ptr + bsize);
  }
  else
    setp(0, 0);
  
  setg(0, 0, 0);
}


GsTLbuf::~GsTLbuf() {
  sync();
  delete[] pbase();
}



int GsTLbuf::overflow(int c) {
  put_buffer();
  
  if (c != EOF)
    if (pbase() == epptr())
      put_char(c);
    else
      sputc(c);
        
  return 0;
}



int GsTLbuf::sync() {
  put_buffer();
  return 0;
}

void GsTLbuf::put_char(int chr) {
  char c = chr;
  QChar qc( c );
  std::cerr << "put_char(" << c <<")" << std::endl;
  text_edit_->append( QString( qc ) );
}


void GsTLbuf::put_buffer() {
  if( pbase() != pptr() ) {
    int len = (pptr() - pbase());
    char* buffer = new char[len + 1];
    
    strncpy(buffer, pbase(), len);
    buffer[len] = 0;
    std::cerr << "put_buffer: " << buffer << std::endl;
    text_edit_->append( buffer );
        
    setp(pbase(), epptr());
    delete [] buffer;
    
  }
}

