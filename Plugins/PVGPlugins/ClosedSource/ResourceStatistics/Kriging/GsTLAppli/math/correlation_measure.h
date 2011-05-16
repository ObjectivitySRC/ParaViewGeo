/**********************************************************************
** Author: Nicolas Remy
** Copyright (C) 2002-2004 The Board of Trustees of the Leland Stanford Junior
**   University
** All rights reserved.
**
** This file is part of the "math" module of the Geostatistical Earth
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

#ifndef __GSTLAPPLI_MATH_CORRELATION_MEASURE_H__
#define __GSTLAPPLI_MATH_CORRELATION_MEASURE_H__

#include <GsTLAppli/math/common.h>

#include <utility>
#include <vector>
#include <map>
#include <string>



/** This is the base class for all types of correlation measures, eg variogram,
 * covariance, etc. The correlation_measure can handle auto-correlations or
 * cross-correlations (ie the head and tail variables can be different).
 * For each locations pairs u, u+h, the knowledge of Z1(u), Z1(u+h), Z2(u) and
 * Z2(u+h) is required (in general).
*/
class MATH_DECL Correlation_measure {
public:
  typedef std::pair<double, double> ValPair;
  static double NaN;

public:
  Correlation_measure();
  virtual ~Correlation_measure() {}

  virtual Correlation_measure* clone() const = 0;
  virtual void set_parameters( const std::vector<double>& params ) {}

  /** head_prop is the pair (Z1(u), Z1(u+h)) and tail prop is the pair
  * (Z2(u), Z2(u+h)) where Z1 is the head variable and Z2 the tail variable
  */
  virtual void add_pair( const ValPair& head_prop, const ValPair& tail_prop );

  /** Computes the correlation between a set of pairs (Z1(u), Z1(u+h)) and 
  * (Z2(u), Z2(u+h)) for different u. 
  */ 
  virtual double correlation( const std::vector<ValPair>& head_prop_values,
                              const std::vector<ValPair>& tail_prop_values );

  /** Call this function after adding all the pairs with add_pairs, to get
  * the correlation between all the added pairs. 
  */
  virtual double correlation() ;

  /** Returns how pairs have been added so far
  */
  int pair_count() const { return pair_count_; }

protected:
  virtual double compute_single( const ValPair& head_prop, 
                                 const ValPair& tail_prop ) = 0;

protected:
  double accumulated_value_;
  int pair_count_;

};


typedef Correlation_measure* (*CorrelationCallBackType)();

class MATH_DECL Correlation_measure_factory {

public:
  static Correlation_measure* instantiate( const std::string& method_name );
  static bool add_method( const std::string& name, 
                          const CorrelationCallBackType& callback );
  static std::vector<std::string> available_methods();

private:
  static void initialize();

  typedef std::map<std::string, CorrelationCallBackType> CallBackMap;
  static CallBackMap* methods_map_;
  static std::vector<std::string>* available_methods_;
};



class MATH_DECL Variogram_measure : public Correlation_measure {
public:
  static Correlation_measure* new_instance() { return new Variogram_measure; }

public:
  Variogram_measure() : Correlation_measure() {}

  virtual Correlation_measure* clone() const;

protected:
  virtual double compute_single( const ValPair& head_prop, 
                                 const ValPair& tail_prop );
};



class MATH_DECL Covariance_measure : public Correlation_measure {
public:
  static Correlation_measure* new_instance() { return new Covariance_measure; }

public:
  Covariance_measure() : Correlation_measure(), means_(0,0) {}
  virtual Correlation_measure* clone() const;

  virtual void add_pair( const ValPair& head_prop, 
                         const ValPair& tail_prop );
  
  virtual double correlation( const std::vector<ValPair>& head_prop_values,
                              const std::vector<ValPair>& tail_prop_values );
  virtual double correlation() const;

protected:
  virtual double compute_single( const ValPair& head_prop, 
                                 const ValPair& tail_prop );

protected:
  std::pair<double,double> means_;
};




class MATH_DECL Correlogram_measure : public Correlation_measure {
public:
  static Correlation_measure* new_instance() { return new Correlogram_measure; }

public:
  Correlogram_measure() : Correlation_measure() {}
  virtual Correlation_measure* clone() const;

  virtual void add_pair( const ValPair& head_prop, 
                         const ValPair& tail_prop );
  virtual double correlation();

protected:
  virtual double compute_single( const ValPair& head_prop, 
                                 const ValPair& tail_prop );

protected:
  std::pair<double,double> means_;
  std::pair<double,double> vars_;
};


class MATH_DECL Indicator_variogram_measure : public Variogram_measure {
public:
  static Correlation_measure* new_instance() { 
    return new Indicator_variogram_measure; 
  }

public:
  Indicator_variogram_measure() : Variogram_measure() {}
  virtual Correlation_measure* clone() const;

  virtual void set_parameters( const std::vector<double>& params );
  virtual void add_pair( const ValPair& head_prop, 
                         const ValPair& tail_prop );

protected:
  ValPair indicator( const ValPair& v, double thresh );

protected:
  double head_threshold_;
  double tail_threshold_;
};



#endif
