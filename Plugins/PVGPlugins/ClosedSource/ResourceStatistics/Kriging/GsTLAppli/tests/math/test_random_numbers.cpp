#include <GsTLAppli/math/random_numbers.h>

#include <iostream>

int main() {
  Global_random_number_generator::instance()->seed( 2111 );
  Random_number_generator rand_gen;
  for( int i=0; i<5 ; i++ )
    cout << rand_gen() << endl;
  
  Random_number_generator rand_gen2;
  for( int i=0; i<5 ; i++ )
    cout << rand_gen2() << endl;
  
  cout << endl;
  Global_random_number_generator::instance()->seed( 2111 );
  for( int i=0; i<10 ; i++ )
    cout << rand_gen() << endl;
  

  cout << endl << endl 
       << "testing STL_generator" << endl;
  Global_random_number_generator::instance()->seed( 2111 );
  STL_generator stl_gen;
  for( int i=0; i<5 ; i++ )
    cout << stl_gen(120) << endl;
  
  STL_generator stl_gen2;
  for( int i=0; i<5 ; i++ )
    cout << stl_gen2(120) << endl;
  
  cout << endl;
  Global_random_number_generator::instance()->seed( 2111 );
  for( int i=0; i<10 ; i++ )
    cout << stl_gen(120) << endl;
  
}
