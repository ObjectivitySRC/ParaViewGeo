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

/* 
 *  GXML/Graphite: Geometry and Graphics Programming Library + Utilities 
 *  Copyright (C) 2000 Bruno Levy 
 * 
 *  This program is free software; you can redistribute it and/or modify 
 *  it under the terms of the GNU General Public License as published by 
 *  the Free Software Foundation; either version 2 of the License, or 
 *  (at your option) any later version. 
 * 
 *  This program is distributed in the hope that it will be useful, 
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of 
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the 
 *  GNU General Public License for more details. 
 * 
 *  You should have received a copy of the GNU General Public License 
 *  along with this program; if not, write to the Free Software 
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA. 
 * 
 *  If you modify this software, you should include a notice giving the 
 *  name of the person performing the modification, the date of modification, 
 *  and the reason for such modification. 
 * 
 *  Contact: Bruno Levy 
 * 
 *     levy@loria.fr 
 * 
 *     ISA Project 
 *     LORIA, INRIA Lorraine,  
 *     Campus Scientifique, BP 239 
 *     54506 VANDOEUVRE LES NANCY CEDEX  
 *     FRANCE 
 * 
 *  Note that the GNU General Public License does not permit incorporating 
 *  the Software into proprietary programs.  
 */ 
  
  
#ifndef __CLOCK_H__  
#define __CLOCK_H__  
  
 
#include <GsTLAppli/utils/common.h>
#include <iostream> 
#include <cmath> 
#include <cstdlib> 
 
 
#ifdef WIN32 
#else 
#include <sys/types.h> 
#include <sys/times.h>  
#endif 
 
//______________________________________________________________________ 
 
/** 
  * A utility class to precisely compute the time taken 
  * by an algorithm. USClock prints to cerr the elapsed time 
  * between the line where it is declared and the end 
  * of the scope where it is declared. The times computed by 
  * USClock are expressed as system ticks, which is a system 
  * dependant unit. USClock prints three different times: 
  * 
  * real time: the really elapsed time (depends on the load of the 
  *   machine, i.e. on the others programs that are executed at the 
  *   same time). 
  * 
  * system time: the time spent in system calls. 
  * 
  * user time: the time really spent in the process. 
  * 
  * USClock is unimplemented on Windows, and will only issue 
  *   a message indicating that it is not implemented. 
  * 
  * example: 
  * <pre> 
  *   { 
  *      USClock clock ; 
  *        do_something() ; 
  *        clock.print_elapsed_time() ; 
  *   }  
  * </pre> 
  * 
  */ 
 
class UTILS_DECL USClock { 
public : 
    USClock() ; 
    ~USClock() ; 
 
    /** 
     * prints real, user and system times since the 
     * construction of this GxmlSystemStopWatch (in seconds). 
     */ 
    void print_elapsed_time() ; 
    void fprint_elapsed_time( std::ostream& os) ; 
    /** 
     * returns user elapsed time since the construction 
     * of this GxmlSystemStopWatch (in seconds). 
     */ 
    double elapsed_user_time() const ; 
 
private: 
#ifdef WIN32 
#else 
    tms start_ ; 
    clock_t start_user_ ; 
#endif     
} ; 



//=========================

class QTime;

class UTILS_DECL Qt_clock {
public:
  Qt_clock() ;
  ~Qt_clock() ;

  void start();
  void restart();
  int elapsed();

private:
  QTime* timer_;
};
 
 
#endif 
