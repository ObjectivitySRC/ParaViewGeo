#include <GsTL/geometry/covariance.h>
#include <GsTLAppli/math/gstlpoint.h>

int main() {
  Covariance<GsTLPoint> covar;
  typedef std::vector<std::string> Vector;
  std::vector<std::string> types = covar.available_types();
  for( int i=0; i<types.size() ; i++ )
    cout << types[i] << "  " ;
  cout << endl;
}
