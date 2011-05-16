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

#ifndef __GSTLAPPLI_UTILS_PROGRESS_NOTIFIER_H__
#define __GSTLAPPLI_UTILS_PROGRESS_NOTIFIER_H__

#include <GsTLAppli/utils/gstl_messages_private.h>

#include <string>

/** A progress notifier is used when a long task has to be completed.
* It keeps the user informed about the progress made toward the completion
* of the task, e.g. by displaying a progress bar, or by detailing which
* sub-task is currently processed.
* The progress notifier must know in how many steps the main task is divided.
* Then if frequency is 1, each call to notify() will signal the completion of
* a step and the notifier will inform the user of the progress. 
* A given task can take a lot of iterations before being completed, and we might
* not want to notify the user each time an iteration is done. By setting 
* frequency to somthing greater than 1, the notifier will wait "frequency" 
* times (ie "frequency" calls to notify) before reporting some progress
* to the user. 
* Example:
* Our task takes 1000 iterations to complete. At each iteration, notify() is
* called. We want the progress notifier to report progress every 100 iterations.
* Then total_steps is 10 and frequency is 100.
* After each 100 iterations, the notifier will tell the user that another 10% 
* of the task has been completed.
*/
class UTILS_DECL Progress_notifier : public Scribe {
public:
  Progress_notifier( int total_steps, const std::string& title ="" );
  virtual ~Progress_notifier();
  
  /** Tell the notifier to display a message. See class Channel for more
  * details on how to write a message to a channel. In particular, it is
  * important not to forget that, by default, a channel waits for the 
  * gstl_io::end instruction before sending a message. 
  * ex: \code
  * notifier->message() << "this is the " << 1 << "st message"
  *                         << " of the day" << gstl_io::end;
  * \endcode
  */
  virtual Channel& message();

  /** This function tells the notifier (not the user! - see function frequency() )
  * that a new step toward the completion of the task has been completed.
  * @return false if the task was interrupted by the user. A notifier is not
  * required to provide a way to abort the task. In that case, notify() will
  * always return true.
  */
  virtual bool notify() = 0;

  virtual void title( const std::string& str ) {}
  
  /** Tells in how many steps the main task has been divided. If the requested
  * frequency is 1, then 
  */
  virtual int total_steps() const;
  virtual void total_steps( int count );
  virtual int frequency() const;

  /** Set the frequency at which the progress notifier should notify 
  * the user of progress. Specifically, \c f is the number of calls to
  * notify() that the notifier should ignore before repoting any progress
  * to the user.
  */
  virtual void frequency( int f );
  
protected:
  Channel* private_channel_;
  int total_steps_;
  int frequency_;
  std::string title_;
};


//============================================
/** This is a text-based progress notifier.
*/
class UTILS_DECL Text_progress_notifier : public Progress_notifier {
public:
  static Named_interface* create_new_interface( std::string& );

  Text_progress_notifier( int total_steps, 
                          const std::string& title = "" );
  virtual ~Text_progress_notifier() {}
  virtual bool notify();
  virtual void write( const std::string& str, const Channel* );
                          
protected:
  int notify_count_;
  int steps_done_;
  std::string progress_bar_;
};



//============================================
/** Void_notifier is a notifier that doesn't do anything.
*/
class UTILS_DECL Void_notifier : public Progress_notifier {
public:
  static Named_interface* create_new_interface( std::string& );

  Void_notifier();
  virtual ~Void_notifier() {}
  virtual bool notify() { return true; }
  virtual void write( const std::string& str, const Channel* ) {}
};




#endif
