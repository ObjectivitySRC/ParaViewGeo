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

#ifndef __GSTLAPPLI_UTILS_GSTL_MESSAGES_PRIVATE_H__ 
#define __GSTLAPPLI_UTILS_GSTL_MESSAGES_PRIVATE_H__ 
 
 
#include <GsTLAppli/utils/common.h>
#include <GsTLAppli/utils/singleton_holder.h> 
#include <GsTLAppli/utils/named_interface.h> 

#include <GsTL/utils/smartptr.h> 
 
#include <iostream> 
#include <string> 
#include <list> 
#include <cassert> 
#include <sstream>
 
/* This file contains macros that output warnings and messages. 
 * 3 macros are defined: 
 *   - gstl_message 
 *   - gstl_warning 
 *   - gstl_assert 
 * The 2 first can be disabled by defining GSTLAPPLI_SILENT. 
 * The last one is disabled by defining GSTLAPPLI_NOCHECK 
 * Defining GSTLAPPLI_NDEBUG disables all 3 macros. 
 */ 
 
 
#undef __BEGIN_DECLS 
#undef __END_DECLS 
#ifdef __cplusplus 
# define __BEGIN_DECLS extern "C" { 
# define __END_DECLS } 
#else 
# define __BEGIN_DECLS /* empty */ 
# define __END_DECLS /* empty */ 
#endif 
 

# define GSTLAPPLI_VOID_CAST static_cast<void> 
 
/*============================== 
 * if GSTLAPPLI_NDEBUG is defined, disable gstl_assert  
 * and don't output messages or warnings 
 */ 
#ifdef GSTLAPPLI_NDEBUG 
#  define GSTLAPPLI_NOCHECK 
#  define GSTLAPPLI_SILENT 
#endif 
 
/*============================== 
 * if GSTLAPPLI_NOCHECK is defined, disable gstl_assert  
 * gstl_assert checks if the supplied expression is 0  
 */ 
 
#ifdef	GSTLAPPLI_NOCHECK 
# define appli_assert(expr)		(GSTLAPPLI_VOID_CAST (0)) 
 
#else /* Not GSTLAPPLI_NOCHECK  */ 
 
# define appli_assert(expr) \
  (assert(expr)) 
 
#endif  
 
 
   
/*============================== 
 * if GSTLAPPLI_SILENT is defined, do not output messages and 
 * warnings. 
 */ 
 
__BEGIN_DECLS 
UTILS_DECL void __appli_warning(std::string file, int line, std::string str); 
UTILS_DECL void __appli_message(std::string file, int line, std::string str); 
 
UTILS_DECL void __qt_appli_warning(std::string file, int line, std::string str); 
UTILS_DECL void __qt_appli_message(std::string file, int line, std::string str); 
 
__END_DECLS 


#ifdef GSTLAPPLI_SILENT 
#  define appli_warning(expr) 
#  define appli_message(expr) 
 
#else 
 
# define appli_warning(expr) \
{ \
  std::ostringstream ostr; \
  ostr << expr ;  \
  __appli_warning(__FILE__, __LINE__, ostr.str() ); \
}  
 
# define appli_message(expr) \
{ \
  std::ostringstream ostr; \
  ostr << expr ;  \
  __appli_message(__FILE__, __LINE__, ostr.str() ); \
}   
#endif   
   
  
 
 
//====================================================== 
//   Classes (not macros) to write messages to the 
//   user (the messages sent by the previous macros are 
//   more intended for the developer).  
 
class Channel; 

/** A Scribe is an object that can listen to the messages sent by a channel
* and "write" them. A Scribe can listen to several channels at the same time
* by subscribing to each of them. 
*/ 
//class UTILS_DECL Scribe : public SmartPtr_interface<Scribe> { 
class UTILS_DECL Scribe : public Named_interface { 
 public: 
  virtual ~Scribe() {} 
  /** Start listening to Channel \c channel.
  * @return false if the subscription failed
  */
  virtual bool subscribe( Channel& channel );
  
  /** Stop listening to Channel \c channel.
  * @return false if the un-subscription failed
  */
  virtual bool unsubscribe( Channel& channel ); 

  /** Process message \c str sent by one of the channels
  * the scribe is listening to.
  */
  virtual void write( const std::string& str, const Channel* sender ) = 0; 
 
}; 
 

/** A Channel is used to broadcast messages to anyone currently listening.
* Messages are sent using the traditional stream syntax:
*   channel << "this is " << 1 << " message" << IO::end
* A channel normally waits to receive the "end" instruction before broadcasting
* a message. In the previous example, "this is 1 message" is sent to all 
* subscriber. If the trailing "<< IO::end" had been omitted, no message would have
* been sent.
* This default behavior can be modified by sending the "no_wait_end" instruction
* to the channel:
*   channel << IO::no_wait_end << "this is " << 1 << " message"
* would immediatly send the message.
* To come back to the default behavior, use the "wait_end" manipulator.
*/ 
class UTILS_DECL Channel { 
public: 
  Channel();
  virtual ~Channel();

  /** Register a new subscriber
  */
  virtual bool attach( Scribe* scribe );
  /** Remove subscriber \c scribe
  */
  virtual bool detach( Scribe* scribe ); 
  /** send what is in the buffer
  */
  virtual void send();
  /** clear the content of the bufer
  */
  virtual void clear() { buffer_->str( "" ); }
  virtual void use_buffer( bool on ) { use_buffer_ = on; }
  virtual bool use_buffer() const { return use_buffer_; }

  /** Return the current number of subscribers
  */
  int subscribers() const { return scribes_.size(); }

 protected: 
  typedef std::list< SmartPtr<Scribe> >::iterator list_iterator; 
  std::list< SmartPtr<Scribe> > scribes_ ;
  
  bool use_buffer_;
  std::ostringstream* buffer_; 
 
  friend UTILS_DECL Channel& operator << ( Channel& ch, const std::string& str ); 
  friend UTILS_DECL Channel& operator << ( Channel& ch, const char* str ); 
  friend UTILS_DECL Channel& operator << ( Channel& ch, int n ); 
  friend UTILS_DECL Channel& operator << ( Channel& ch, float n ); 
  friend UTILS_DECL Channel& operator << ( Channel& ch, double n ); 
  friend UTILS_DECL Channel& operator << ( Channel& ch, long int n ); 
  friend UTILS_DECL Channel& operator << ( Channel& ch, bool n ); 
  friend UTILS_DECL Channel& operator << ( Channel& ch, Channel& (*f)(Channel&) ); 
   
}; 
 
UTILS_DECL Channel& operator << ( Channel& ch, const std::string& str ); 
UTILS_DECL Channel& operator << ( Channel& ch, const char* str ); 
UTILS_DECL Channel& operator << ( Channel& ch, int n ); 
UTILS_DECL Channel& operator << ( Channel& ch, float n ); 
UTILS_DECL Channel& operator << ( Channel& ch, double n ); 
UTILS_DECL Channel& operator << ( Channel& ch, long int n ); 
UTILS_DECL Channel& operator << ( Channel& ch, bool n ); 
UTILS_DECL Channel& operator << ( Channel& ch, Channel& (*f)(Channel&) ); 
 
 
class UTILS_DECL Out_channel : public Channel { 
 public: 
  Out_channel() {} 
  virtual ~Out_channel() {} 
}; 
  
class UTILS_DECL Error_channel : public Channel { 
public: 
  Error_channel() {} 
  virtual ~Error_channel() {} 
}; 
 
class UTILS_DECL Log_channel : public Channel { 
public: 
  Log_channel() {} 
  virtual ~Log_channel() {} 
}; 
 


/* We now define a singleton for each channel. Using template class 
* Singleton_holder and defining a typedef (as in the following 3 lines) compiles 
* correctly but there seems to be some problem when using DLLs (no problem if 
* statically linked): the channels appear
* to lose their subscribers when entering a DLL (on Windows. It works fine 
* on Linux). If the singleton class is explicitly written, the channels behave 
* correctly.
* Nicolas - april 19 2003. 
* 
*/
//UTILS_DECL typedef Singleton_holder<Out_channel> GsTLAppli_output; 
//UTILS_DECL typedef Singleton_holder<Error_channel> GsTLAppli_error; 
//UTILS_DECL typedef Singleton_holder<Log_channel> GsTLAppli_logging; 

/** GsTLAppli_output is a globally accessible channel to send standard messages.
* It is the equivalent of std::cout.
*/
class UTILS_DECL GsTLAppli_output { 
public:
  static Out_channel* instance(); 
 
private: 
  static Out_channel* instance_; 
 
  GsTLAppli_output(); 
  GsTLAppli_output(const GsTLAppli_output&); 
  GsTLAppli_output& operator=(const GsTLAppli_output&); 
};

/** GsTLAppli_output is a globally accessible channel to send error messages.
* It is the equivalent of std::cerr.
*/
class UTILS_DECL GsTLAppli_error { 
public:
  static Error_channel* instance(); 
 
private: 
  static Error_channel* instance_; 
 
  GsTLAppli_error(); 
  GsTLAppli_error(const GsTLAppli_error&); 
  GsTLAppli_error& operator=(const GsTLAppli_error&); 
};

/** GsTLAppli_output is a globally accessible channel to send messages 
* that should be kept into a log. 
*/
class UTILS_DECL GsTLAppli_logging { 
public:
  static Log_channel* instance(); 
 
private: 
  static Log_channel* instance_; 
 
  GsTLAppli_logging(); 
  GsTLAppli_logging(const GsTLAppli_logging&); 
  GsTLAppli_logging& operator=(const GsTLAppli_logging&); 
}; 
 

namespace gstlIO {

/** Channel Manipulator function. Signals that the message is finished
* and should be broadcasted.
*/ 
UTILS_DECL Channel& end( Channel& ch );

/** Channel Manipulator function. Tells the channel to wait for the
* the "end" instruction before sending a message 
*/ 
UTILS_DECL Channel& wait_end( Channel& ch );

/** Channel Manipulator function. Tells the channel not to wait for the 
* "end" instruction before sending the message: 
*    channel << no_wait_end << "a message in " << 3 << " parts"
* would send the 3 messages:
*   - "a message in "
*   - "3" 
*   - " parts"
*/ 
UTILS_DECL Channel& no_wait_end( Channel& ch );

}  // end namespace IO

#endif 
