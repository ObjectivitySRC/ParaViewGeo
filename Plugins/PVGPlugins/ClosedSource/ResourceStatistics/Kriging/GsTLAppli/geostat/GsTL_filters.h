#ifndef __GSTLAPPLI_GEOSTAT_PLUGIN_GSTLFILTERS_H__
#define __GSTLAPPLI_GEOSTAT_PLUGIN_GSTLFILTERS_H__

#include <vector>
#include <cmath>


template<typename Window_neighborhood>
class GsTL_filter {
public:
  typedef std::vector<float> Weigths;
  GsTL_filter(){}
  virtual ~GsTL_filter(){}

  virtual float operator()(Window_neighborhood& neigh, int filter_id) = 0;

  virtual std::string name(int filter_number = 0 ) = 0;

  virtual int number_filters(void ) = 0;


protected:
  bool skip_uninformed_;
  int nfilter_;
};


template<typename Window_neighborhood>
class Moving_average : public GsTL_filter<Window_neighborhood> {
public:
  Moving_average(){nfilter_=1;}
  virtual ~Moving_average(){}

  virtual std::string name(int filter_number=1 ) {return "Moving_average";}
  virtual int number_filters(void ) {return 1;}

  virtual float operator()(Window_neighborhood& neigh, int filter_id=0) {
    float x = 0.;
    int n=0;
    Window_neighborhood::iterator it = neigh.begin();
    for(; it != neigh.end(); ++it ) {
      if( !it->is_informed() ) continue;
      x += it->property_value();
      n++;
    }
    if(n >0 ) return x/n;
    else return -9966699; //indicates not informed
  }

protected :

};

template<typename Window_neighborhood>
class Moving_variance : public GsTL_filter<Window_neighborhood> {
public:
  Moving_variance(){nfilter_=1;}
  ~Moving_variance(){}

  virtual std::string name(int filter_number ) {return "Moving_variance";}
  virtual int number_filters(void ) {return 1;}

  virtual float operator()(Window_neighborhood& neigh, int filter_id=0) {
    float x = 0.;
    float x2 = 0.;
    int n=0;
    Window_neighborhood::iterator it = neigh.begin();
    for(; it != neigh.end(); ++it ) {
      if( !it->is_informed() ) continue;
      float val = it->property_value();
      x += val;
      x2 += val*val;
      n++;
    }
    if(n >2 ) return (x2 - x*x)/n;
    else return -9966699; //indicates not informed
  }
protected :

};



template<
  typename Window_neighborhood,
  typename Functor
>
class Functional_filter : public GsTL_filter<Window_neighborhood> {
public:
  Functional_filter():is_raster_(false),skip_uninformed_(true){nfilter_=1;}
  ~Functional_filter(){}

  Functional_filter(Functor func, bool skip_uninf=true, bool normalize = true ):skip_uninformed_(skip_uninf),
    is_raster_(false),normalize_(normalize),func_(func){}

  virtual std::string name(int filter_number ) {return func_.name();}
  virtual int number_filters(void ) {return 1;}


  template< 
    typename Grid_geometry,
    typename Iter_window
  >
  void pre_compute_weights(Grid_geometry geom,
      Iter_window begin,Iter_window end ) 
  {
    //is_raster_ = true;
    for(; begin != end; ++begin ) weights_.push_back( func_(*begin) );

  }

    virtual float operator()(Window_neighborhood& neigh, int filter_id=0) {
      if( neigh.is_empty() ) return -9966699;
      float score=0.;
      float sum_weight=0.;
      std::vector<float>::iterator it_w = weights_.begin();

      Window_neighborhood::iterator it = neigh.begin();
      for(; it != neigh.end(); ++it, ++it_w ) {
        if( !it->is_informed() ) {
          if(!skip_uninformed_) return -9966699;
          continue;
        }
        if(is_raster_) {
          score += *it_w*it->property_value();
          sum_weight += *it_w;
        }
        else {
          float weight  = func_( it->location(), (neigh.center()).location() );
          sum_weight += weight;
          score += weight*it->property_value();
        }
      }
      if(normalize_) return score/sum_weight;
      else return score;
      //else return -996699; //indicates not informed
    }

protected :
  bool is_raster_;
  bool skip_uninformed_;
  bool normalize_;
  std::vector<float> weights_;
  Functor func_;
};


template<
  typename Window_neighborhood,
  typename  Rasterized_weights
>
class Rasterized_filter : public GsTL_filter<Window_neighborhood> {

public :

  Rasterized_filter() {}
  ~Rasterized_filter() {}


  Rasterized_filter(Rasterized_weights filters)
    : skip_uninformed_(true),filters_(filters) {
      n_weights_ = std::distance(filters_.weights_begin(0),filters_.weights_end(0));

  }

  virtual std::string name(int filter_number ) { return filters_.names(filter_number); }
  virtual int number_filters(void ) {return filters_.number_filters();}

  virtual void operator()(Window_neighborhood& neigh,
      std::vector<float>& scores ) 
  {
    scores.clear();
    if(neigh.is_empty()) {
      scores.insert(scores.begin(),nfilter_,-9966699);
      return;
    }
    scores.insert(scores.begin(),nfilter_,0.);

    for(int i=0; i<nfilter_; i++ ) {
      std::vector<float>::iterator it_w = filters_.weights_begin(i);
      Window_neighborhood::iterator it = neigh.begin();
      for(; it != neigh.end(); ++it, ++it_w ) {
        scores[i] += *it_w * it->property_value();
      }
    }
  }

  virtual float operator()(Window_neighborhood& neigh, int filter_id )
  {
    if(neigh.is_empty()) return -9966699;
    if( n_weights_ != neigh.size() ) return -9966699;

    std::vector<float>::iterator it_w = filters_.weights_begin(filter_id);
    Window_neighborhood::iterator it = neigh.begin();
    float scores = 0.;
    
    for(; it != neigh.end(); ++it, ++it_w ) {
      if( !it->is_informed() ) return -9966699;
      scores += *it_w * it->property_value();
    }
    return scores;

  }

protected : 

  bool skip_uninformed_;
  int n_weights_;
  Rasterized_weights filters_;

};



class Sobel_weights {
public :

  Sobel_weights()
  {
    double temp1[9] = {-1,-2,-1,0,0,0,1,2,1};
	  double temp2[9] = {-1,0,1,-2,0,2,-1,0,1};
    std::vector<float> v1(temp1, temp1+9 );
	  std::vector<float> v2(temp2, temp2+9 );
	  multi_weights_.push_back( v1 );
	  multi_weights_.push_back( v2 );
  }
  ~Sobel_weights(){}

  int number_filters(){ return 2; }

  std::string names(int filter_number){
    std::string name;
    (filter_number == 0 ) ? name = "Sobel_1" : name =  "Sobel_2";
    return name;
  }

  std::vector<float> get_weights(int i) {
    return multi_weights_[i];
  }

  std::vector<float>::iterator weights_begin(int filter_number=0) { 
    return multi_weights_[filter_number].begin(); 
  }

  std::vector<float>::iterator weights_end(int filter_number=0) { 
    return multi_weights_[filter_number].end(); 
  }

protected:
  std::vector< std::vector<float> > multi_weights_;

};


class Gaussian_kernel {
public:
  Gaussian_kernel( float sigma ):sigma_(sigma){}
  Gaussian_kernel():sigma_(1.){}
  ~Gaussian_kernel(){}

  std::string name() {return "Gaussian Kernel";}

  template<typename Vector>
  float operator()(Vector x) {
    return compute( std::sqrt(x[0]*x[0]+x[1]*x[1]+x[2]*x[2]) );
  }

  template<typename Vector>
  float operator()(Vector x1, Vector x2) {
    Vector x = x2-x1;
    return compute(std::sqrt(x[0]*x[0]+x[1]*x[1]+x[2]*x[2]));
  }

protected:
  float sigma_;
  float compute(float x) { return std::exp(-x*x/sigma_); }
};




#endif