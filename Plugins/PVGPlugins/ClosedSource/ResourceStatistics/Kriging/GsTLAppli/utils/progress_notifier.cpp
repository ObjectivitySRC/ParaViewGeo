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

#include <GsTLAppli/utils/progress_notifier.h>

#include <algorithm>


Progress_notifier::Progress_notifier( int total_steps,
                                      const std::string& title
                                     ) {
  private_channel_ = new Channel;
  private_channel_->attach( this );
  
  // When subscribing to the private channel, the number of references
  // to "this" is increased by 1, since the channel keeps a smart pointer
  // to the subscriber. But since the channel is destroyed only if "this"
  // is destroyed, we need to artificially decrease the number of references
  // by 1 (we ignore the reference increase due to the subscription)
  this->unref_no_delete();

  total_steps_ = total_steps;
  frequency_ = 1;
}
                                      
Progress_notifier::~Progress_notifier() {
  delete private_channel_;
}
  
Channel& Progress_notifier::message() {
  return *private_channel_;
}

int Progress_notifier::total_steps() const {
  return total_steps_;
}

void Progress_notifier::total_steps( int count ) {
  total_steps_ = count;
}

int Progress_notifier::frequency() const {
  return frequency_;
}

void Progress_notifier::frequency( int f ) {
  frequency_ = std::max( f, 1 );
}
  


//============================================

Named_interface* Text_progress_notifier::create_new_interface( std::string& ) {
  return new Text_progress_notifier( 0, "" );
}

Text_progress_notifier::Text_progress_notifier( int total_steps, 
                                                const std::string& title )
  : Progress_notifier( total_steps, title ) {
  notify_count_ = 0;
  steps_done_ = 0;
}


bool Text_progress_notifier::notify(){

  // do not report to the user if we haven't completed frequency_ steps yet
  if( ++notify_count_ % frequency_ != 0 ) return true;

  if( progress_bar_.empty() ) {
    int steps = std::min( 20, int(total_steps_/frequency_) )+1;
    progress_bar_.resize( steps, ' ' );
    progress_bar_[0] = '|';
  }

  // report progress done
  steps_done_++;
  int percent = int( float(steps_done_*frequency_)/float(total_steps_)*100 );

  if( steps_done_ < progress_bar_.size() )
    progress_bar_[steps_done_] = '#';
  
  std::cout << "\r" << progress_bar_ << "|    " << percent << "%" << std::flush;

  /*
  for( int i=0; i < steps_done_; i++ )
    std::cout << "#" ;
  for( int j=0; j < (total_steps_/frequency_) - steps_done_; j++ )
    std::cout << " ";

  std::cout << "|   " << percent << "%" ;
*/
  if( percent >= 100 ) 
    std::cout << std::endl;

  return true;
}

void Text_progress_notifier::write( const std::string& str, const Channel* ) {
  std::cout << str ;
}
    

//============================================

Named_interface* Void_notifier::create_new_interface( std::string& ) {
  return new Void_notifier();
}
 

Void_notifier::Void_notifier() 
: Progress_notifier( 0, "" ) {
}


