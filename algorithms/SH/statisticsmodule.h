#ifndef STAT_H_INCLUDED
#define STAT_H_INCLUDED

class StatisticsModule;

#include "constants.h"
#include "data.h"
#include <string>
#include <fstream>

class StatisticsModule{
    public:
    void begin(){start_ = clock();};
    void finish(){finish_ = clock();};
    void stat_output(string query_file,string groundtruth_file,string result_file,string output_file,int);
    //void stat_output(string query_file,string groundtruth_file,string output_file,int);
    void gen_query_and_groundtruth(string query_file, string groundtruth_file);
	float compute_recall();
	float compute_mean_reciprocal_rank();
	float compute_number_closer();
	float compute_mean_average_precision();
	float compute_relative_distance_error();
	float compute_discounted_culmulative_gain();

    private:
    void sample_query();
    void batch_linear_scan();

    private:
    double start_,finish_;
    int result[querysize][K];
    int groundtruth[querysize][K];
    float query[querysize][D];

    public:
    double sumcheck;
};

#endif // STAT_H_INCLUDED
