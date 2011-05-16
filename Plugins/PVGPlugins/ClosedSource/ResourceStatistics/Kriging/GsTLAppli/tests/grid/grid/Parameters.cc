#include <Parameters.h>


Parameters::Parameters(std::string parameter_file) {
    ifstream inFile( parameter_file.c_str() );
    if( !inFile ) {
        cerr << "Unable to open file " << parameter_file << endl;
        abort();
    }
    
    string comments;
    
    int nx, ny, nz;
    
    //=====================================
    // Simulation grid
    inFile >> nx >> ny >> nz;
    getline(inFile, comments,'\n');
    
    cout << nx << "  " << ny << "  " << nz << endl;
    
    simulation_grid = new GsTLRGrid("simul_grid");
    
    GsTLRGridGeometry* geom = new GsTLSimpleRGridGeometry();
    geom->set_size(0, nx);
    geom->set_size(1, ny);
    geom->set_size(2, nz);
    
    simulation_grid->set_geometry(geom);
    
    
    simulation_grid->add_property("facies", typeid(PropertyType));
    simulation_grid->select_property("facies");
    simulation_grid->set_level(1);
    
    
    
    //=====================================
    // Training image
    inFile >> nx >> ny >> nz;
    getline(inFile, comments,'\n');
    
    cout << nx << "  " << ny << "  " << nz << endl;
    
    training_image = new GsTLRGrid("training_image");
    
    GsTLRGridGeometry* geom2 = new GsTLSimpleRGridGeometry();
    geom2->set_size(0, nx);
    geom2->set_size(1, ny);
    geom2->set_size(2, nz);
    
    training_image->set_geometry(geom2);
    
    
    training_image->add_property("facies", typeid(PropertyType));
    
    training_image->select_property("facies");
    training_image->set_level(1);
    
    GsTLGridProperty* prop = training_image->find_property("facies");
    
    GsTLFloatDataArray* image_data = prop->data();
    
    string training_image_file;
    int column;
    inFile >> training_image_file;
    inFile >> column;
    getline(inFile, comments,'\n');
    
    cout << training_image_file << "  " << column << endl;
    
    ifstream inTrain(training_image_file.c_str());
    if( !inTrain ) {
        cerr << "Unable to open file " << training_image_file << endl;
        abort();
    }
    
    // Read gslib-formated training image file
    getline(inTrain, comments, '\n');   //skip header
    
    int nb_of_properties;
    inTrain >> nb_of_properties;       // nb of properties in file
    getline(inTrain, comments, '\n');
    
    if( column > nb_of_properties ) {
        cerr << "Column " << column <<" does not exist in file " << training_image_file << endl
        <<  "Only " << nb_of_properties << " are present in the file" << endl;
        abort();
    }
    
    // skip property names
    for( int j=0; j<nb_of_properties; j++ ) {
        getline(inTrain, comments, '\n');
    }
    
    // read training image values
    for( int i=0; i<training_image->size(); i++ ) {
        float val;
        for(int i1=0; i1<column; i1++) inTrain >> val;
        
        image_data->set_value(val, i);
        
        getline(inTrain, comments, '\n');
    }
    
    ofstream outTrain("train.dbg");
    outTrain << "training\n1\nfacies\n";
    
    {for( int i = 0; i <training_image->size(); i++ ) {
            outTrain << image_data->get_value(i) << endl;
        }}
    
    
    //=====================================
    // Initialize Hard data
    
    string harddata_file;
    inFile >> harddata_file;
    getline(inFile, comments, '\n');
    
    cout <<  harddata_file << endl;
    
    ifstream inHard(harddata_file.c_str());
    if( !inHard ) {
        cerr << "unable to open file " << harddata_file << endl;
        abort();
    }
    
    getline(inHard, comments, '\n');
    
    inHard >> nb_of_properties;
    getline(inHard, comments, '\n');
    
    for( int j1=0; j1<nb_of_properties; j1++ ) {
        getline(inHard, comments, '\n');
    }
    
    
    GsTLInt x y z;
    PropertyType val;
    
    while( inHard >> x ) {
        inHard >> y >> z >> val;
        GsTLGridGeovalue gval = simulation_grid->geovalue(x, y, z);
        gval.set_property_value(val);
        hard_data.push_back(gval);
    }
    
    cout  << " hard data read" << endl;
    for( std::vector<GeoValue>::iterator it=hard_data.begin(); it!=hard_data.end(); ++it ) {
        std::cout << it->location()[0] << " "
        << it->location()[1] << " "
        << it->location()[2] << " "
        << it->property_value() << endl;
        std::cout << endl;
    }
    
    
    
    //=====================================
    // Initialize Template
    
    int max_window_size;
    inFile >> max_window_size;
    getline(inFile, comments, '\n');
    
    std::string window_file;
    inFile >> window_file;
    getline(inFile, comments, '\n');
    
    cout << max_window_size << endl
    << window_file << endl;
    
    ifstream inWindow(window_file.c_str());
    if( !inWindow ) {
        cerr << "Unable to open file " << window_file << endl;
        abort();
    }
    
    for( int i2=0; i2<max_window_size; i2++ ) {
        GsTLInt  x,y,z;
        inWindow >> x >> y >> z;
        grid_template.append_node(x, y, z);
    }
    
    
    
    //===================================================
    // Misc parameters (number of multiple grids, number of facies)
    
    inFile >> nb_of_facies >> nb_of_multiple_grids;
    getline(inFile, comments, '\n');
    
    cout << nb_of_facies << "   " << nb_of_multiple_grids << endl;
    
    
    
    //==================================================
    // Marginal cdf
    
    vector<short int> zvalues;
    vector<double> pvalues;
    for( int i3=0; i3<nb_of_facies ; i3++ ) {
        zvalues.push_back(i3);
        
        double p;
        inFile >> p;
        cout << p << "  ";
        pvalues.push_back(p);
    }
    getline(inFile, comments, '\n');
    cout << endl;
    
    ccdf = new Cdf(zvalues.begin(), zvalues.end() );
    marginal_cdf = new Cdf(zvalues.begin(), zvalues.end(),
        pvalues.begin() );
    
    
    //=====================================================
    // Output stream
    
    string outfilename;
    inFile >> outfilename;
    getline(inFile, comments, '\n');
    
    cout << outfilename << endl;
    output.open(outfilename.c_str());
    
    
    //=====================================================
    // Number of realizations, seed, cdf reproduction parameter
    
    inFile >> number_of_realizations;
    getline(inFile, comments, '\n');
    cout << number_of_realizations << endl;
    
    inFile >> seed;
    getline(inFile, comments, '\n');
    cout << seed << endl;
    
    inFile >> constraint_to_target_cdf;
    getline(inFile, comments, '\n');
    cout << constraint_to_target_cdf << endl;
    
}





Parameters::~Parameters() {
    delete simulation_grid;
    delete training_image;
    
    delete ccdf;
    delete marginal_cdf;
}
