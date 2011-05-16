#include <GsTLAppli/grid/grid_model/sgrid_cursor.h>
#include <GsTLAppli/math/gstlpoint.h>

#include <iostream.h>


int main(int argc, char* argv[]) {
  SGrid_cursor gc(5,5,1, 1);

  cout << endl << "level 2" << endl;
  gc.set_multigrid_level( 2 );
  cout << "number of nodes in multigrid: " << gc.max_index() << endl;

  for( GsTLInt i = 0; i < 25 ; ++i ) {
    if( gc.check_node_id(i) ) {
      gc.go_to(i);
      GsTLPoint loc;
      gc.current_coord( loc[0], loc[1], loc[2] );
      
      cout << "node-id= " << i << "   loc: " << loc << endl;
    }
  }


  cout << endl << "iterating on multigrid using local index " << endl;
  for( int i=0 ; i<gc.max_index() ; i++)
    cout << "node_id for local id " << i << " is " << gc.node_id( i ) << endl;

  cout << endl << "level 3" << endl;
  gc.set_multigrid_level( 3 );
  cout << "number of nodes in multigrid: " << gc.max_index() << endl;

  for( GsTLInt i = 0; i < 25 ; ++i ) {
    if( gc.check_node_id(i) ) {
      gc.go_to(i);
      GsTLPoint loc;
      gc.current_coord( loc[0], loc[1], loc[2] );
      
      cout << "node-id= " << i << "   loc: " << loc << endl;
    }
  }

  cout << endl << "back to level 2" << endl;
  gc.set_multigrid_level( 2 );
  cout << "number of nodes in multigrid: " << gc.max_index() << endl;
  GsTLPoint loc2(2,2,0);
  cout << "node id of " << loc2 << " : " << gc.node_id(loc2[0], loc2[1], loc2[2]) << endl;
  
  cout << endl << "back to level 3" << endl;
  gc.set_multigrid_level( 3 );
  cout << "number of nodes in multigrid: " << gc.max_index() << endl;
  GsTLPoint loc3(1,1,0);
  cout << "node id of " << loc3 << " : " << gc.node_id(loc3[0], loc3[1], loc3[2]) << endl;
  
  return 0;
}
