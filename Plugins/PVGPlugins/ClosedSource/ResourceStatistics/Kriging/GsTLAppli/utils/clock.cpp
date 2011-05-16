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

#include <GsTLAppli/utils/clock.h>



USClock::USClock() {
#ifdef WIN32
#else
    clock_t init_user = times(&start_) ;
    while((start_user_ = times(&start_)) == init_user) ;
#endif
}

USClock::~USClock() {
    print_elapsed_time();
}

double USClock::elapsed_user_time() const {
#ifdef WIN32
    return 0 ;
#else
    clock_t end_user ;
    tms end ;
    end_user = times(&end) ;
    return double(end_user - start_user_) / 100.0 ;
#endif
}
 
void USClock::print_elapsed_time() {
#ifdef WIN32
    std::cerr << "Sorry, USClock not implemented on Windows" << std::endl ;
#else
    clock_t end_user ;
    tms end ;
    end_user = times(&end) ;
    
    std::cerr << "---- Times (seconds) ----" << std::endl ;
    std::cerr << "  Real time: " 
         << double(end_user - start_user_) / 100.0 << std::endl ;

    std::cerr << "  User time: " 
         << double(end.tms_utime - start_.tms_utime) / 100.0 << std::endl ;

    std::cerr << "  Syst time: " 
         << double(end.tms_stime - start_.tms_stime) / 100.0 << std::endl ;    
#endif    
}

void USClock::fprint_elapsed_time( std::ostream& os) {
    #ifdef WIN32
    std::cerr << "Sorry, USClock not implemented on Windows" << std::endl ;
#else
    clock_t end_user ;
    tms end ;
    end_user = times(&end) ;
    
    os << "---- Times (seconds) ----" << std::endl ;
    os << "  Real time: " 
         << double(end_user - start_user_) / 100.0 << std::endl ;

    os << "  User time: " 
         << double(end.tms_utime - start_.tms_utime) / 100.0 << std::endl ;

    os << "  Syst time: " 
         << double(end.tms_stime - start_.tms_stime) / 100.0 << std::endl ;    
#endif  
}



//========================================
#include <qdatetime.h>
Qt_clock::Qt_clock() {
  timer_ = new QTime;
}

Qt_clock::~Qt_clock() {
  delete timer_;
}

void Qt_clock::start() {
  timer_->start();
}

void Qt_clock::restart() {
  timer_->restart();
}

int Qt_clock::elapsed() {
  int ms = timer_->elapsed();
  return ms;
} 
