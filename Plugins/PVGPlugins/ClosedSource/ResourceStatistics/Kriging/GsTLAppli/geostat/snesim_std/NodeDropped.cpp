
#include "NodeDropped.h"

// member functions
NodeDropped::NodeDropped( vector<int>& grid_path, int nb_cut )
{
    path_size = grid_path.size();

    for(int i=0; i<path_size; i++)
        nodes.push_back( make_pair( grid_path[i], 0 ) );

    nb_cut_ = nb_cut;
    if ( nb_cut_<0 )      nb_cut_ =99999;

}


void NodeDropped::ResetPath( vector<int>& grid_path )
{
    path_size = grid_path.size();
    nodes.clear();

    for(int i=0; i<path_size; i++)
        nodes.push_back( make_pair( grid_path[i], 0 ) );
}

void NodeDropped::operator()(int loc, int nb_dropped) 
{
    nodes[loc].second = nb_dropped;
}


void NodeDropped::GetRevisitNodes( vector<int>& grid_path_new)
{
    grid_path_new.clear();

    if ( nb_cut_ < 99999 )
    {
        for (int i=0; i<path_size; i++)
        {
            if ( nodes[i].second > nb_cut_ ) 
                grid_path_new.push_back( nodes[i].first );
            else if ( nodes[i].second < 0 ) 
                grid_path_new.push_back( nodes[i].first );
        }
    }
}



void NodeDropped::setDNProp( RGrid* grid, GsTLGridProperty* prop )
{
    SGrid_cursor  cursor = SGrid_cursor( *(grid->cursor()) );
    int node_id;

    for (int i=0; i<path_size; i++)
    {
        node_id = cursor.node_id( nodes[i].first );
        prop->set_value( nodes[i].second, node_id );
    }
}
