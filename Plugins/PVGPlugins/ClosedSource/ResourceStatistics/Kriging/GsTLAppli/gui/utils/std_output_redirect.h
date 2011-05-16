#ifndef __GSTLAPPLI_MAIN_STD_OUTPUT_REDIRECT_H__
#define __GSTLAPPLI_MAIN_STD_OUTPUT_REDIRECT_H__

#include <iostream>

class QTextEdit;


class GsTLbuf: public std::streambuf {
 private:
  QTextEdit* text_edit_;
  void put_buffer(void);
  void put_char(int);

 protected:
  int	overflow(int);
  int	sync();

 public:
  GsTLbuf( QTextEdit* text_edit, int bsize );
  ~GsTLbuf();
};

#endif
