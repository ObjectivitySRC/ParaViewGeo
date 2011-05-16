#include <GsTLAppli/utils/gstl_data_array.h>

#include <vector>
#include <algorithm>
#include <stdlib.h>

int main() {
    int n = 10;

  
    GsTLDataArray<double> ab(n, typeid(float));

    srand48( 211111 );
    for (int i = 0; i < n; ++i) {
      float val = drand48()*10;
      ab.set_value( val, i); 
    }

    int j=0;
    for ( GsTLDataArray<double>::const_iterator it=ab.begin(); 
	  it != ab.end(); ++it, ++j ) {
      //std::cout << ab.get_value(j) << std::endl; 
      cout <<  *it << endl ; 
    }
    
    GsTLDataArray<double>::const_iterator iter = ab.begin();
    cout << endl << "begin+5= " << *( iter + 5 ) << endl ;
    iter+=5;
    cout << "*iter= " << *iter << endl << endl;
    
    cout << "min= " << *min_element( ab.begin(), ab.end() ) << endl;
    cout << "max= " << *max_element( ab.begin(), ab.end() ) << endl << endl;

    
    std::vector<float> vec(n);
    std::copy( ab.begin(), ab.end(), vec.begin() );
    std::sort( vec.begin(), vec.end() );
    std::copy( vec.begin(), vec.end(), ostream_iterator<float>( cout, "\n" ) );

    
    /*
    for (int i = 0; i < n; ++i) {
        float val = i;
        vec[i] = val;
    }
    */

    return 0;
}
