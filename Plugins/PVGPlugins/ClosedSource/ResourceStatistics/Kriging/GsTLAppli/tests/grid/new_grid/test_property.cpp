#include <GsTLAppli/grid/grid_model/grid_property.h>
#include <GsTLAppli/utils/clock.h>

#include <fstream>
#include <iterator>
#include <iostream>
#include <algorithm>
#include <functional>
#include <vector>

void print( const GsTLGridProperty& prop ) {
    for( int i=0; i< prop.size() ; i++ ) {
    if( prop.is_informed( i ) )
      cout << prop.get_value( i ) << endl;
    else
      cout << "-99" << endl;
      }
}


void exec( GsTLGridProperty& prop, int size ) {
  for( int i = 0; i < size ; i++ ) {
    prop.set_value( prop.get_value(i) * 2.0, i );
  }
}



int main() {
  const int size = 10;
  GsTLGridProperty prop( size, "toto" );

  for( int i = 0; i < size ; i++ ) {
    prop.set_value( float(i), i );
  }

  print(prop);
  
  cout << endl
       << "------------------------------" << endl
       << "  swaping to disk" << endl;
  { 
    USClock clock;
    prop.swap_to_disk();
  }
  {
    cout << endl << "  modifying property" << endl;
    USClock clock;
    exec( prop, prop.size() );
    print(prop);
  }

  
  cout << endl
       << "------------------------------" << endl
       << "  swaping back to memory" << endl;
  {
    USClock clock;
    prop.swap_to_memory(); 
  }
  {
    cout << endl << "  modifying property" << endl;
    USClock clock;
    exec( prop, prop.size() );
    print(prop);
  }


  cout << endl
       << "------------------------------" << endl
       << "  swaping again to disk" << endl;
  {
    USClock clock;
    prop.swap_to_disk();
  }
  {
    USClock clock;
    cout << endl << "  modifying property" << endl;
    exec( prop, prop.size() );
    print( prop );
  }

  ofstream out( "property_values.txt" );
  for( int i=0; i< prop.size() ; i++ ) {
    if( prop.is_informed( i ) )
      out << prop.get_value( i ) << "\n";
    else
      out << "-99\n";
  }


  cout << endl << endl
       << "----------------------------------------" << endl
       << "   testing GsTLGridProperty::iterator" << endl;
  typedef GsTLGridProperty::iterator iterator;
  
  std::copy( prop.begin(), prop.end(),
	     std::ostream_iterator<float>( cout, "\n" ) );
  
  std::transform( prop.begin(), prop.end(),
		  prop.begin(), bind2nd(multiplies<float>(), 2.0 ) );

  cout << endl;
  std::copy( prop.begin(), prop.end(),
	     std::ostream_iterator<float>( cout, "\n" ) );

  cout << endl << "un-informing id=2, id = 4 and 6" << endl << endl; 
  prop.set_not_informed( 2 );
  prop.set_not_informed( 4 );
  prop.set_not_informed( 6 );
  std::transform( prop.begin(), prop.end(),
		  prop.begin(), bind2nd(multiplies<float>(), 0.5 ) );

  cout << endl;
  std::copy( prop.begin(), prop.end(),
	     std::ostream_iterator<float>( cout, "\n" ) );

  std::vector< float > distr;
  std::copy( prop.begin(), prop.end(),
	     std::back_inserter( distr ) );


}
