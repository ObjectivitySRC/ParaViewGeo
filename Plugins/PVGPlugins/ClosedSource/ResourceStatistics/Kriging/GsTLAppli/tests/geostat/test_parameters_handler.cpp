#include <GsTLAppli/geostat/parameters_handler_impl.h>

#include <sstream>

int main() {
  ostringstream xml;
  xml << "<parameters>" << endl 
      << "<variogram>" << endl
      << "  <structure type=\"exponential\" >" << endl
      << "    <angles alpha=\"1\" beta=\"2\" tetha=\"3\" />" << endl
      << "    <ranges x=\"4\" y=\"5\" z=\"6\" />" << endl
      << "  </structure>" << endl
      << "</variogram> " << endl
      << "</parameters> " << endl;

  Parameters_handler_xml handler( xml.str() );

  cout << xml.str() << endl << endl;

  std::string param;
  while( true ) {
    cin >> param;
    cout << handler.value( param ) << endl; 
  }
}
