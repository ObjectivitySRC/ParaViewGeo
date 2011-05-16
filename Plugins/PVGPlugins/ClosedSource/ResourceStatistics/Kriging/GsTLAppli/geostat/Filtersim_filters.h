#ifndef __FILTERSIM_FILTERS_WRAPPER_H__
#define __FILTERSIM_FILTERS_WRAPPER_H__

#include <vector>
#include <cmath>
#include <iostream>
#include <GsTLAppli/geostat/filtersim_std/filters.h>

/* -----------------------

This is a wraper around the Filtersim filters
-----------------------  */ 
class Filtersim_filters  {
public :

  Filtersim_filters( ){filters_ = NULL;}

  Filtersim_filters(const Filtersim_filters& rhs)
  {
    filter_size_[0] = rhs.filter_size_[0];
    filter_size_[1] = rhs.filter_size_[1];
    filter_size_[2] = rhs.filter_size_[2];
    if( dynamic_cast<Filters_default*>(rhs.filters_) )
      filters_ = new Filters_default(filter_size_[0], filter_size_[1], filter_size_[2], 9);
    else if(dynamic_cast<Filters_user_define*>(rhs.filters_) ) {
      filename_ = rhs.filename_;
      filters_ =  new Filters_user_define(filter_size_[0], filter_size_[1], filter_size_[2], filename_); 
    }
  }

 // Filtersim_filters( std::string filename ){}

  ~Filtersim_filters( )
  {
    if(filters_) delete filters_;
  }

  Filtersim_filters(int nxdt, int nydt, int nzdt ) {
    filter_size_[0] = nxdt;
    filter_size_[1] = nydt;
    filter_size_[2] = nzdt;
    filters_ = new Filters_default(nxdt, nydt, nzdt, 9); }

  Filtersim_filters(std::string filename ) {
    filename_ = filename;
    get_bounding_filter_geom( filename, filter_size_[0], filter_size_[1], filter_size_[2] );
    filters_ =  new Filters_user_define(filter_size_[0], filter_size_[1], filter_size_[2], filename); 
  }

  int number_filters(){ return filters_->get_total_filter_number(); }

  Grid_template* get_geometry() { return filters_->get_window_geometry(); }

  std::vector<float> get_weights(int i) { return filters_->get_weights(i); }

  std::vector<float>::iterator weights_begin(int filter_number=0) {
    return (filters_->get_weights(filter_number)).begin(); 
  }

  std::vector<float>::iterator weights_end(int filter_number=0) { 
   return (filters_->get_weights(filter_number)).end(); 
  }

  std::string names(int filter_number) {
    return filters_->get_filter_name(filter_number);
  }

protected:
  Filter* filters_;
  int filter_size_[3];
  std::string filename_;

  void get_bounding_filter_geom(string filename, int& nx, int& ny, int& nz ) {
  nx = ny = nz = 0;
  int dx, dy, dz;
  string line;
  ifstream infile( filename.c_str() );
  getline(infile,line);
  
  while( getline(infile,line) )
  {
      string cur_str;
      vector< string > str; 
      istringstream scan_stream( line );

      while( scan_stream >> cur_str ) 
          str.push_back( cur_str );

      if ( str.size() == 4 )  // contains offset and loading weight
      {
          // offset in X, Y, Z direction
          dx = std::abs( String_Op::to_number<int>(str[0]) );
          dy = std::abs( String_Op::to_number<int>(str[1]) );
          dz = std::abs( String_Op::to_number<int>(str[2]) );

          if( dx > nx ) nx = dx;
          if( dy > ny ) ny = dy;
          if( dz > nz ) nz = dz;
      }
  }
  infile.close();
}
 
};
/*
void Filtersim_filters::get_bounding_filter_geom(string filename, int& nx, int& ny, int& nz ) {
  nx = ny = nz = 0;
  int dx, dy, dz;
  string line;
  ifstream infile( filename.c_str() );
  getline(infile,line);
  
  while( getline(infile,line) )
  {
      string cur_str;
      vector< string > str; 
      istringstream scan_stream( line );

      while( scan_stream >> cur_str ) 
          str.push_back( cur_str );

      if ( str.size() == 4 )  // contains offset and loading weight
      {
          // offset in X, Y, Z direction
          dx = String_Op::to_number<int>(str[0]);
          dy = String_Op::to_number<int>(str[1]);
          dz = String_Op::to_number<int>(str[2]);

          if( std::abs(dx) > nx ) nx = dx;
          if( std::abs(dy) > ny ) ny = dy;
          if( std::abs(dz) > nz ) nz = dz;
      }
  }
  infile.close();
}
*/
#endif