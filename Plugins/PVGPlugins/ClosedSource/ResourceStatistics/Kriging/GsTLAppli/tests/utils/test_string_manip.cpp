#include <iostream>

#include <GsTLAppli/utils/string_manipulation.h>

static std::ostream& operator<<(std::ostream& os, String_Op::string_pair& name) {
  os << "'" << name.first << "'    '" << name.second << "'" ;
  return os;
}

int main() {
  typedef std::vector<std::string> str_vector;

  std::string str("param1/param2/param3//param4");
  std::vector<std::string> vec( String_Op::decompose_string( str, "/" ) );

  cout << "'" ;
  for( str_vector::iterator it= vec.begin(); it!= vec.end(); ++it )
    cout << *it << "'   '" ;

  cout << endl;

  std::cout << endl << endl 
	    << "---------------------------------------------" << endl
	    << "   testing convertions " << endl << endl;
  std::string number("00.3211233");
  double num = String_Op::to_number<double>( number );
  cout << "string was: " << number << "   converted to: " << num << endl;

  double num2 = 1.23456780;
  std::string number2= String_Op::to_string( num2 );
  cout << "number was: " << num2 << "   converted to: " << number2 << endl;


 std::cout << endl << endl 
	    << "---------------------------------------------" << endl
	    << "   testing search " << endl << endl;
 std::string string1( "Simple Kriging (SK)" );
 std::string substring1( "sImPlE" );
 std::cout << "\"" << string1 << "\" contains \"" 
	   << substring1 << "\" ? " 
	   << String_Op::contains( string1, "sImPlE", false ) << endl;
}
