/**********************************************************************
** Author: Nicolas Remy
** Copyright (C) 2002-2004 The Board of Trustees of the Leland Stanford Junior
**   University
** All rights reserved.
**
** This file is part of the "utils" module of the Geostatistical Earth
** Modeling Software (GEMS)
**
** This file may be distributed and/or modified under the terms of the 
** license defined by the Stanford Center for Reservoir Forecasting and 
** appearing in the file LICENSE.XFREE included in the packaging of this file.
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
** See http://www.gnu.org/copyleft/gpl.html for GPL licensing information.
**
** Contact the Stanford Center for Reservoir Forecasting, Stanford University
** if any conditions of this licensing are not clear to you.
**
**********************************************************************/

#include <GsTLAppli/utils/gstl_messages_private.h>
#include <GsTLAppli/utils/string_manipulation.h> 

__BEGIN_DECLS
void __appli_warning(std::string file, int line, std::string str) {
    std::cerr << file << ":" << line << "  " 
	      << str << std::endl;
}
void __appli_message(std::string file, int line, std::string str) {
  std::cout << file << ":" << line << "  "
	    << str << std::endl;
}

/*
void __qt_appli_warning(std::string file, int line, std::string str) {
  QString file_name(file.c_str() );
  QString line_str;
  line_str.setNum(line);
  
  QMessageBox::warning(0, file_name+":"+line_str, QString(str.c_str() ), 
      QMessageBox::Ok , QMessageBox::NoButton, QMessageBox::NoButton);
}
void __qt_appli_message(std::string file, int line, std::string str) {
  QString file_name(file.c_str() );
  QString line_str;
  line_str.setNum(line);
  QMessageBox::information(0, file_name+":"+line_str, QString(str.c_str() ), 
      QMessageBox::Ok , QMessageBox::NoButton, QMessageBox::NoButton);
}
*/

__END_DECLS



//============================================

bool Scribe::subscribe( Channel& channel ) {
  return channel.attach(this);
}

bool Scribe::unsubscribe( Channel& channel ) {
  return channel.detach(this);
}



//============================================
Channel::Channel() 
: use_buffer_( true ) {
  buffer_ = new std::ostringstream;
}

Channel::~Channel() {
  // delete buffer_;
}

bool Channel::attach( Scribe* scribe ) {
  scribes_.push_back( scribe );
  return true;
}

bool Channel::detach( Scribe* scribe ) {
  unsigned int size = scribes_.size();
  scribes_.remove( SmartPtr<Scribe>( scribe ) );
  return scribes_.size() == size-1;
}

void Channel::send() { 
  std::string message = buffer_->str();
  if( message.empty() ) return;

  for( Channel::list_iterator it = scribes_.begin(); 
       it != scribes_.end(); ++it) {
    (*it)->write( message, this );
  }

  clear();
}

//============================================

Channel& operator << ( Channel& ch, const std::string& str ) {
  *(ch.buffer_) << str;
  if( !ch.use_buffer() )
    ch.send();
  return ch;
}

Channel& operator << ( Channel& ch, const char* str ) {
  *ch.buffer_ << str;
  if( !ch.use_buffer() )
    ch.send();
  return ch;
}

Channel& operator << ( Channel& ch, int n ) {
  *ch.buffer_ << n;
  if( !ch.use_buffer() )
    ch.send();
  return ch;
}

Channel& operator << ( Channel& ch, float n ) {
  *ch.buffer_ << n;
  if( !ch.use_buffer() )
    ch.send();
  return ch;
}

Channel& operator << ( Channel& ch, double n ){
  *ch.buffer_ << n;
  if( !ch.use_buffer() )
    ch.send();
  return ch;
}

Channel& operator << ( Channel& ch, long int n ){
  *ch.buffer_ << n;
  if( !ch.use_buffer() )
    ch.send();
  return ch;
}

Channel& operator << ( Channel& ch, bool n ){
  *ch.buffer_ << n;
  if( !ch.use_buffer() )
    ch.send();
  return ch;
}

Channel& operator << ( Channel& ch, Channel& (*f)(Channel&) ) {
  return f( ch );
}



//------------------------------------
// Channel singletons

Out_channel* GsTLAppli_output::instance_ = 0;

Out_channel* GsTLAppli_output::instance() {
  if(instance_ == 0) {
    instance_ = new Out_channel;
  }
  return instance_;
}


Error_channel* GsTLAppli_error::instance_ = 0;

Error_channel* GsTLAppli_error::instance() {
  if(instance_ == 0) {
    instance_ = new Error_channel;
  }
  return instance_;
}


Log_channel* GsTLAppli_logging::instance_ = 0;

Log_channel* GsTLAppli_logging::instance() {
  if(instance_ == 0) {
    instance_ = new Log_channel;
  }
  return instance_;
}


#if defined(__GNUC__) || (defined(_MSC_VER) && _MSC_VER >= 1310)   // 1310 == VC++ 7.1
/* With g++ > 2.96: */
extern "C" UTILS_DECL Out_channel& GsTLcout = *GsTLAppli_output::instance();
extern "C" UTILS_DECL Error_channel& GsTLcerr = *GsTLAppli_error::instance();
extern "C" UTILS_DECL Log_channel& GsTLlog = *GsTLAppli_logging::instance();

#endif

//------------------------------------
// Channel manipulators
namespace gstlIO {

Channel& end( Channel& ch ) {
  ch.send();
  return ch;
}

Channel& wait_end( Channel& ch ) {
  ch.use_buffer( true );
  return ch;
}

Channel& no_wait_end( Channel& ch ) {
  ch.use_buffer( false) ;
  return ch;
}
} // end namespace IO
