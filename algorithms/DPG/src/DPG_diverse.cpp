#include <cctype>
#include <type_traits>
#include <iostream>
#include <boost/timer/timer.hpp>
#include <boost/program_options.hpp>
#include <sys/time.h>
#include <cctype>
#include <random>
#include <iomanip>
#include <type_traits>
#include <boost/timer/timer.hpp>
#include <boost/tr1/random.hpp>
#include <boost/format.hpp>
#include <boost/program_options.hpp>

#include "kgraph.h"
#include "kgraph-data.h"

using namespace boost::timer;
using namespace std;
using namespace boost;
using namespace kgraph;
namespace po = boost::program_options; 

#ifndef KGRAPH_VALUE_TYPE
#define KGRAPH_VALUE_TYPE float
#endif


typedef KGRAPH_VALUE_TYPE value_type;

int main(int argc, char *argv[]){
    vector<string> params;
    string input_data_path;
    string index_NNList_path;
    string index_DPG_path; 
    unsigned L; 

    po::options_description desc_visible("General options");
    desc_visible.add_options()
    ("help,h", "produce help message.")
    ("version,v", "print version number.")
    ("data", po::value(&input_data_path), "input data path")
    ("index", po::value(&index_NNList_path), "NNList index path")
    ("output", po::value(&index_DPG_path), "DPG index path")
    (",L", po::value(&L)->default_value(default_K), "")
    ; 

    po::options_description desc("Allowed options");
    desc.add(desc_visible);

    po::positional_options_description p;
    p.add("data", 1);
    p.add("index", 1);    
    p.add("output", 1);

    po::variables_map vm; 
    po::store(po::command_line_parser(argc, argv).options(desc).positional(p).run(), vm);
    po::notify(vm); 

    if (vm.count("help") || vm.count("data") == 0 || vm.count("index") == 0 || vm.count("output") == 0) {
        cout << "DPG_diverse <data> <K-NN graph index> <diversified index> " << endl;
        cout << desc_visible << endl;
        return 0;
    }

    cerr << L << " : L value" << endl;

    Matrix<value_type> data;
    data.load_lshkit(input_data_path.c_str());
    MatrixOracle<value_type, metric::l2sqr> oracle(data);

    KGraph *kgraph = KGraph::create();
    if (index_NNList_path.size() >0 )
    {
        kgraph->load(index_NNList_path.c_str());    
    }

    boost::timer::auto_cpu_timer timer;

    // start the diversification ?? ( seperately ) 
    // add the diversification part here 
    // cerr << "Start the diversification process... (angular dissimilarity )" << endl << endl;
    // kgraph->remove_near_edges(oracle, L); // here not knn's k, L is the length of NN list ( note that the true NN list length might be smaller than L) 

    cerr << "Start the diversification process... ( counting cuts )" << endl << endl;
    kgraph->diversify_by_cut(oracle, L); // here not knn's k, L is the length of NN list ( note that the true NN list length might be smaller than L) 


    // reverse the edges here 
    cerr << "Add reverse edges ..." << endl << endl;
    kgraph->add_backward_edges();

    float time = timer.elapsed().wall / 1e9;

    if ( index_DPG_path.size() > 0 )
    kgraph->save( index_DPG_path.c_str());

    cout << time << " # diversification time" << endl;

    delete kgraph;

}
