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
#include <sys/time.h>
#include <boost/timer/timer.hpp>
#include <boost/program_options.hpp>
#include <kgraph.h>
#include <kgraph-data.h>
#include <stdio.h>

using namespace std;
using namespace boost;
using namespace kgraph;
namespace po = boost::program_options;

typedef KGRAPH_VALUE_TYPE value_type;

float diff_timeval(timeval t1, timeval t2) {
  return (float) (t1.tv_sec - t2.tv_sec) + (t1.tv_usec - t2.tv_usec) * 1e-6;
}

int main (int argc, char *argv[]) {
    vector<string> params;
    string input_path;
    string index_path;
    string query_path;
    string output_path;
    string init_path;
    string eval_path;
    string result_path;
	string result_seq_path;
    unsigned Index_K, K, M, P, T ,Maxchecks,seedIndex,L; //add Maxchecks

    po::options_description desc_visible("General options");
    desc_visible.add_options()
    ("help,h", "produce help message.")
    ("version,v", "print version number.")
    ("data", po::value(&input_path), "input path")
    ("index", po::value(&index_path), "index path")
    ("query", po::value(&query_path), "query path")
    ("output", po::value(&output_path), "output path")
    ("result", po::value(&result_path), "result path")
	("result_seq", po::value(&result_seq_path), "result sequence path")
    ("init", po::value(&init_path), "init path")
    ("eval", po::value(&eval_path), "eval path")
    ("Index_K", po::value(&Index_K)->default_value(default_K), "")
	(",L", po::value(&L)->default_value(default_L), "")
    (",K", po::value(&K)->default_value(default_K), "")
    (",M", po::value(&M)->default_value(default_M), "")
    (",P", po::value(&P)->default_value(default_P), "")
    (",T", po::value(&T)->default_value(default_T), "")
    (",C",po::value(&Maxchecks)->default_value(default_Maxchecks), "")
    (",S",po::value(&seedIndex)->default_value(default_seedIndex), "")
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

    Matrix<value_type> data;
    Matrix<value_type> query;
    Matrix<value_type> d_init;
    Matrix<unsigned> result; //(query.size(), U);
    unsigned init = 0;

    data.load_lshkit(input_path);
    query.load_lshkit(query_path);
    ofstream out;
    out.open(result_path,ios::app);
    if (init_path.size()) {
        d_init.load_lshkit(init_path);
        BOOST_VERIFY(d_init.size() == query.size());
        init = d_init.dim();
        BOOST_VERIFY(init >= K);
    }
    MatrixOracle<value_type, metric::l2sqr> oracle(data);
    {
    float recall = 0;
	float rde=0;
	float map = 0;
	//float precision =0;
	//float accuracy=0;
    //float minDist=0;
    //float maxDist=0;
    //float avgDist=0;
    float cost = 0;
    float time = 0;
    timeval start;
    timeval end;
    float checks=0;
    if (vm.count("linear")) {
        boost::timer::cpu_timer timer;
        result.resize(query.size(), K);
//#pragma omp parallel for
        for (unsigned i = 0; i < query.size(); ++i) {
            oracle.query(query[i]).search(K, default_epsilon, result[i]);
        }
        cost = 1.0;
        auto times = timer.elapsed();
        out << times.wall/1e9 <<" #linear_time "<<endl;
    }
    else {
        result.resize(query.size(), K);
        KGraph::SearchParams params;
        params.K = K;
        params.M = M;
        params.P = P;
        params.T = T;
		params.Maxchecks=Maxchecks;
        params.init = init;
        KGraph *kgraph = KGraph::create();
        kgraph->load(index_path.c_str());

        gettimeofday(&start, NULL);
        //cout << "Searching..." << endl;
//#pragma omp parallel for reduction(+:cost)        
        for (unsigned i = 0; i < query.size() ; ++i) {
            KGraph::SearchInfo info;
            kgraph->search(oracle.query(query[i]), params, d_init[i], result[i], &info);
            cost += info.cost;
            checks+=info.checks;
        }
        cost /= query.size();
        gettimeofday(&end, NULL);
        time += diff_timeval(end, start);
        delete kgraph;
		/*
		ofstream se_out;
    	se_out.open(result_seq_path,ios::app);
		for(unsigned i=0;i<result.size();i++)
		{
			unsigned const *re_row = result[i];
			for(unsigned j=0;j<result.dim();j++)
				se_out<<re_row[j]<<" ";
			se_out<<"\n";
		}
		*/
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
		
//#pragma omp parallel for
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
        //recall = AverageRecall(gs_dist, result_dist, K);
		recall = AverageRecall(gs, result, K);
		//nc = compute_number_closer(gs_dist, result_dist, K);
		//mrr = compute_mean_reciprocal_rank(gs_dist, result_dist, K);
		AverageAccuracy(gs_dist, result_dist, &rde);
		map = compute_mean_average_precision(gs, result, K);
		//dcg = compute_discounted_culmulative_gain(gs_dist, result_dist, K);

        //AverageMinDist(gs_dist, result_dist, &minDist,&avgDist,&maxDist);
		//AveragePrecision(gs_dist, result_dist, &precision);
		//AverageAccuracy(gs_dist, result_dist, &accuracy); 
    }
	out.setf(ios::fixed); 
	out << recall << " " << rde <<" "<< time/query.size() << " #N_"<< int(checks/query.size()) << " #P_"<<P<<" "<<map<<" "<< endl; 

    }
    return 0;
}

