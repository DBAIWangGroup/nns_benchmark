/* 
    Copyright (C) 2010,2011 Wei Dong <wdong.pku@gmail.com>. All Rights Reserved.

    DISTRIBUTION OF THIS PROGRAM IN EITHER BINARY OR SOURCE CODE FORM MUST BE
    PERMITTED BY THE AUTHOR.
*/
#ifndef KGRAPH_VALUE_TYPE
#define KGRAPH_VALUE_TYPE float
#endif


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


typedef KGRAPH_VALUE_TYPE value_type;

int main_index(int argc, char *argv[]) {
    string data_path;
    string output_path;
    KGraph::IndexParams params;
    unsigned D;
    unsigned skip;
    unsigned gap;
    unsigned synthetic;
    float noise;

    bool lshkit = true;

    po::options_description desc_visible("General options");
    desc_visible.add_options()
    ("help,h", "produce help message.")
    ("version,v", "print version information.")
    ("data", po::value(&data_path), "input path")
    ("output", po::value(&output_path), "output path")
    (",K", po::value(&params.K)->default_value(default_K), "number of nearest neighbor")
    ("controls,C", po::value(&params.controls)->default_value(default_controls), "number of control pounsigneds")
    ;

    po::options_description desc_hidden("Expert options");
    desc_hidden.add_options()
    ("iterations,I", po::value(&params.iterations)->default_value(default_iterations), "")
    (",S", po::value(&params.S)->default_value(default_S), "")
    (",R", po::value(&params.R)->default_value(default_R), "")
    (",L", po::value(&params.L)->default_value(default_L), "")
    ("delta", po::value(&params.delta)->default_value(default_delta), "")
    ("recall", po::value(&params.recall)->default_value(default_recall), "")
    ("prune", po::value(&params.prune)->default_value(default_prune), "")
    ("noise", po::value(&noise)->default_value(0), "noise")
    ("seed", po::value(&params.seed)->default_value(default_seed), "")
    ("dim,D", po::value(&D), "dimension, see format")
    ("skip", po::value(&skip)->default_value(0), "see format")
    ("gap", po::value(&gap)->default_value(0), "see format")
    ("raw", "read raw binary file, need to specify D.")
    ("synthetic", po::value(&synthetic)->default_value(0), "generate synthetic data, for performance evaluation only, specify number of points")
    ;

    po::options_description desc("Allowed options");
    desc.add(desc_visible).add(desc_hidden);

    po::positional_options_description p;
    p.add("data", 1);
    p.add("output", 1);

    po::variables_map vm;
    po::store(po::command_line_parser(argc, argv).options(desc).positional(p).run(), vm);
    po::notify(vm);

    if (vm.count("raw") == 1) {
        lshkit = false;
    }

    if (vm.count("version")) {
        cout << "KGraph version " << KGraph::version() << endl;
        return 0;
    }

    if (vm.count("help")
            || (synthetic && (vm.count("dim") == 0 || vm.count("data")))
            || (!synthetic && (vm.count("data") == 0 || (vm.count("dim") == 0 && !lshkit)))) {
        cout << "Usage: index [OTHER OPTIONS]... INPUT [OUTPUT]" << endl;
        cout << desc_visible << endl;
        cout << desc_hidden << endl;
        return 0;
    }

    if (params.S == 0) {
        params.S = params.K;
    }

    if (lshkit && (synthetic == 0)) {   // read dimension information from the data file
        static const unsigned LSHKIT_HEADER = 3;
        ifstream is(data_path.c_str(), ios::binary);
        unsigned header[LSHKIT_HEADER]; /* entry size, row, col */
        is.read((char *)header, sizeof header);
        BOOST_VERIFY(is);
        BOOST_VERIFY(header[0] == sizeof(value_type));
        is.close();
        D = header[2];
        skip = LSHKIT_HEADER * sizeof(unsigned);
        gap = 0;
    }

    Matrix<value_type> data;
    if (synthetic) {
        if (!std::is_floating_point<value_type>::value) {
            throw runtime_error("synthetic data not implemented for non-floating-point values.");
        }
        data.resize(synthetic, D);
        cerr << "Generating synthetic data..." << endl;
        default_random_engine rng(params.seed);
        uniform_real_distribution<double> distribution(-1.0, 1.0);
        data.zero(); // important to do that
        for (unsigned i = 0; i < synthetic; ++i) {
            value_type *row = data[i];
            for (unsigned j = 0; j < D; ++j) {
                row[j] = distribution(rng);
            }
        }
    }
    else {
        data.load(data_path, D, skip, gap);
    }
    if (noise != 0) {
        if (!std::is_floating_point<value_type>::value) {
            throw runtime_error("noise injection not implemented for non-floating-point value.");
        }
        tr1::ranlux64_base_01 rng;
        double sum = 0, sum2 = 0;
        for (unsigned i = 0; i < data.size(); ++i) {
            for (unsigned j = 0; j < data.dim(); ++j) {
                value_type v = data[i][j];
                sum += v;
                sum2 += v * v;
            }
        }
        double total = double(data.size()) * data.dim();
        double avg2 = sum2 / total, avg = sum / total;
        double dev = sqrt(avg2 - avg * avg);
        cerr << "Adding Gaussian noise w/ " << noise << "x sigma(" << dev << ")..." << endl;
        boost::normal_distribution<double> gaussian(0, noise * dev);
        for (unsigned i = 0; i < data.size(); ++i) {
            for (unsigned j = 0; j < data.dim(); ++j) {
                data[i][j] += gaussian(rng);
            }
        }
    }

    MatrixOracle<value_type, metric::l2sqr> oracle(data);
    KGraph::IndexInfo info;
    KGraph *kgraph = KGraph::create(); //(oracle, params, &info);
    {
        auto_cpu_timer timer;
        kgraph->build(oracle, params, output_path.c_str(), &info);
        cerr << info.stop_condition << endl;
    }

    if (output_path.size()) {
      //kgraph->save(output_path.c_str());
    }


    /*
    // add the diversification part here 
    cerr << "Start the diversification process..." << endl << endl;
    kgraph->remove_near_edges(oracle, params.L/2); // here not knn's k, L is the length of NN list ( note that the true NN list length might be smaller than L) 


    // reverse the edges here 
    cerr << "Add reverse edges ..." << endl << endl;
    kgraph->add_backward_edges();

    if (output_path.size()) {
      kgraph->save(output_path.c_str());
      }*/
    
    delete kgraph;

    return 0;
}

int main_statistic(int argc, char *argv[]){
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
    (",L", po::value(&L)->default_value(default_K), "")
    ; 

    po::options_description desc("Allowed options");
    desc.add(desc_visible);

    po::positional_options_description p;
    p.add("data", 1);
    p.add("index", 1);    


    po::variables_map vm; 
    po::store(po::command_line_parser(argc, argv).options(desc).positional(p).run(), vm);
    po::notify(vm); 

    if (vm.count("help") || vm.count("data") == 0 || vm.count("index") == 0 ) {
        cout << "graph_statistic <data> <NN List index> -L NN_list size" << endl;
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

    // compute the co-efficient
    float co = kgraph->statistics( oracle , L );

    cout << co << " # avg co-efficient " << endl;

}

int main_diversify(int argc, char *argv[]){
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
        cout << "graph_diversify <data> <NN List index> <diversified index> " << endl;
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


int main_search(int argc, char *argv[]) {
    vector<string> params;
    string input_path;
    string index_path;
    string query_path;
    string output_path;
    string init_path;
    string eval_path;
    unsigned K, M, P, T;

    po::options_description desc_visible("General options");
    desc_visible.add_options()
    ("help,h", "produce help message.")
    ("version,v", "print version number.")
    ("data", po::value(&input_path), "input path")
    ("index", po::value(&index_path), "index path")
    ("query", po::value(&query_path), "query path")
    ("output", po::value(&output_path), "output path")
    ("init", po::value(&init_path), "init path")
    ("eval", po::value(&eval_path), "eval path")
    (",K", po::value(&K)->default_value(default_K), "")
    (",M", po::value(&M)->default_value(default_M), "")
    (",P", po::value(&P)->default_value(default_P), "")
    (",T", po::value(&T)->default_value(default_T), "")
    ("linear", "")
    ;

    po::options_description desc("Allowed options");
    desc.add(desc_visible);

    po::positional_options_description p;
    p.add("data", 1);
    p.add("index", 1);
    p.add("query", 1);
    p.add("output", 1);

    po::variables_map vm; 
    po::store(po::command_line_parser(argc, argv).options(desc).positional(p).run(), vm);
    po::notify(vm); 

    if (vm.count("version")) {
        cout << "KGraph version " << KGraph::version() << endl;
        return 0;
    }

    if (vm.count("help") || vm.count("data") == 0 || vm.count("index") == 0 || vm.count("query") == 0) {
        cout << "search <data> <index> <query> [output]" << endl;
        cout << desc_visible << endl;
        return 0;
    }

    if (P < K) {
        P = K;
    }

    Matrix<value_type> data;
    Matrix<value_type> query;
    Matrix<unsigned> result; //(query.size(), U);
    unsigned init = 0;

    data.load_lshkit(input_path);
    query.load_lshkit(query_path);
    if (init_path.size()) {
        result.load_lshkit(init_path);
        BOOST_VERIFY(result.size() == query.size());
        init = result.dim();
        BOOST_VERIFY(init >= K);
    }
    MatrixOracle<value_type, metric::l2sqr> oracle(data);
    float recall = 0;
    float cost = 0;
    float time = 0;
    if (vm.count("linear")) {
        boost::timer::auto_cpu_timer timer;
        result.resize(query.size(), K);
// #pragma omp parallel for
        for (unsigned i = 0; i < query.size(); ++i) {
            oracle.query(query[i]).search(K, default_epsilon, result[i]);
        }
        cost = 1.0;
        time = timer.elapsed().wall / 1e9;
    }
    else {
        result.resize(query.size(), K);
        KGraph::SearchParams params;
        params.K = K;
        params.M = M;
        params.P = P;
        params.T = T;
        params.init = init;
        KGraph *kgraph = KGraph::create();
        kgraph->load(index_path.c_str());

        boost::timer::auto_cpu_timer timer;
        cerr << "Searching..." << endl;

// #pragma omp parallel for reduction(+:cost)
        for (unsigned i = 0; i < query.size(); ++i) {
            KGraph::SearchInfo info;
            kgraph->search(oracle.query(query[i]), params, result[i], &info);
            cost += info.cost;
        }
        cost /= query.size();
        time = timer.elapsed().wall / 1e9;
        //cerr << "Cost: " << cost << endl;
        delete kgraph;
    }
    if (output_path.size()) {
        result.save_lshkit(output_path);
    }
    if (eval_path.size()) {
        Matrix<unsigned> gs;
        gs.load_lshkit(eval_path);
        BOOST_VERIFY(gs.dim() >= K);
        BOOST_VERIFY(gs.size() >= query.size());
        kgraph::Matrix<float> gs_dist(query.size(), K);
        kgraph::Matrix<float> result_dist(query.size(), K);
// #pragma omp parallel for
        for (unsigned i = 0; i < query.size(); ++i) {
            auto Q = oracle.query(query[i]);
            float *gs_dist_row = gs_dist[i];
            float *result_dist_row = result_dist[i];
            unsigned const *gs_row = gs[i];
            unsigned const *result_row = result[i];
            for (unsigned k = 0; k < K; ++k) {
                gs_dist_row[k] = Q(gs_row[k]);
                result_dist_row[k] = Q(result_row[k]);
            }
            sort(gs_dist_row, gs_dist_row + K);
            sort(result_dist_row, result_dist_row + K);
        }
        recall = AverageRecall(gs_dist, result_dist, K);
    }
    cout << "Time: " << time << " Recall: " << recall << " Cost: " << cost << endl;

    return 0;
}

int main(int argc, char *argv[])
{
    cerr<< "INDEX CONSTRUCTION..." << endl;
    return main_index(argc, argv);
    
    // cerr<< "Diversification ......" << endl << endl;      
    //  return main_diversify( argc, argv);

    // cerr<< "Report statistics ......" << endl << endl;      
    // return main_statistic( argc, argv);

    // default search 
    // cerr<< "SEARCH ... " << endl;
	// return main_search(argc, argv); 
}



