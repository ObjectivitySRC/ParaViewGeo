#ifndef __GSTLAPPLI_GEOSTAT_NODES_DROPPED_H__
#define __GSTLAPPLI_GEOSTAT_NODES_DROPPED_H__

#include <GsTLAppli/geostat/common.h>
#include <GsTLAppli/grid/grid_model/geovalue.h>
#include <GsTLAppli/grid/grid_model/rgrid.h>
#include <GsTLAppli/grid/grid_model/gval_iterator.h>

#include <algorithm>
#include <vector>

using namespace std;

typedef pair<int, int> NodeType;

struct  node_drop_less_than 
{
	bool operator() ( NodeType p1,  NodeType p2) 
	{    
		return ( p1.second <= p2.second );
	}  
};

/*
 * class to recorded the number of nodes dropped during simulation
 * will return the nodes which are needed to be resimulated
 */
class GEOSTAT_DECL NodeDropped {
 public:
     NodeDropped( vector<int>& grid_path, int nb_cut=-1);

     //void NodeDropped( vector<int>& grid_path, int cutoff);

     void ResetPath( vector<int>& grid_path );
     void operator()(int loc, int nb_dropped);
     void GetRevisitNodes( vector<int>& grid_path_new);
     
     //void GetRevisitNodes( vector<int>& grid_path_new);

     void setDNProp( RGrid* grid, GsTLGridProperty* prop );

private:
    int nb_cut_;
    int path_size;
    vector< NodeType > nodes;
};


#endif