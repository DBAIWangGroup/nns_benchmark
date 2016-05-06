// Copyright 2012 Yandex Artem Babenko
#include "perfomance_util.h"

extern string report_file;

PerfTester::PerfTester() {
  report_file_ = report_file;
  current_points_count = 0;
  handled_queries_count = 0;
  cells_traversed = 0;
  nearest_subcentroids_time = 0;
  cache_init_time = 0;
  merger_init_time = 0;
  full_traversal_time = 0;
  cell_coordinates_time = 0;
  cell_edges_time = 0;
  residual_time = 0;
  refining_time = 0;
  full_search_time = 0;

  for(int i = 0; i < 21; ++i) {
    list_length_thresholds_.push_back(std::pow(2.0, i));
  }
  current_threshold_index_ = 0;
  list_length_times_.resize(list_length_thresholds_.size(), 0.0);
}

void PerfTester::ResetQuerywiseStatistic() {
  current_threshold_index_ = 0;
  current_points_count = 0;
}

void PerfTester::NextNeighbour() {
  ++current_points_count;
  if(current_points_count >= list_length_thresholds_[current_threshold_index_]) {
    clock_t current_time = clock();
    list_length_times_[current_threshold_index_] += current_time - search_start;
    ++current_threshold_index_;
  }
}

void PerfTester::DoReport(std::ofstream& out) {
  out << "Queries count: "
      << handled_queries_count << endl;
  out << "Average cells count: "
      << (double)cells_traversed / handled_queries_count << endl;
  out << "Average nearest subcentroids getting time: "
      << (double)nearest_subcentroids_time / handled_queries_count << endl;
  out << "Average cache init time: "
      << (double)cache_init_time / handled_queries_count << endl;
  out << "Average merger init time: "
      << (double)merger_init_time / handled_queries_count << endl;
  out << "Average full traversal time: "
      << (double)full_traversal_time / handled_queries_count << endl;
  out << "Average cells coordinates getting time: "
      << (double)cell_coordinates_time / handled_queries_count << endl;
  out << "Average cell edges getting time: "
      << (double)cell_edges_time/ handled_queries_count << endl;
  out << "Average residual time: "
      << (double)residual_time / handled_queries_count << endl;
  out << "Average refining time: "
      <<(double)refining_time / handled_queries_count << endl;
  out << "Average full search time: "
      << (double)full_search_time / handled_queries_count << endl;
}

void PerfTester::DoReport() {
  std::ofstream out(report_file_.c_str(),ios::app);
  DoReport(out);
}

int GetRecallAt(const int length, const vector<PointId>& groundtruth,
                const vector<DistanceToPoint>& result) {
  if(groundtruth.empty()) {
    cout << "Groundtruth is empty!" << endl;
    return 0;
  }
  for(int index = 0; index < length && index < result.size(); ++index) {
    if(result[index].second == groundtruth[0]) {
      return 1;
    }
  }
  return 0;
}

double GetPresicionAt(const int length, const vector<PointId>& groundtruth,
                      const vector<DistanceToPoint>& result) {
 
  int found = 0;
  std::set<PointId> ground_points;
  for(int i = 0; i < groundtruth.size(); ++i) {
      ground_points.insert(groundtruth[i]);
  }
  for(int index = 0; index < length && index < result.size() ; ++index) {
    if(ground_points.find(result[index].second) != ground_points.end()) {
      found += 1;
    }
  }
  return (double)found/length; //
}

double GetRecall(int k, const vector<PointId>& groundtruth,
                 const vector<DistanceToPoint>& result) {
  if(groundtruth.empty()) {
    cout << "Groundtruth is empty!" << endl;
    return 0;
  }
  std::set<PointId> returned_points;
  for(int i = 0; i < k; ++i) {
      returned_points.insert(result[i].second);
  }
  double found = 0.0;
  for(int index = 0; index < k; ++index) {
      if(returned_points.find(groundtruth[index]) != returned_points.end()) {
          found += 1;
      }
  }
  return found/k;
}

float get_distance(Point data, Point query)
{
  float distance =0.0;
  for(int i=0;i< data.size();i++)
  {
	  float diff = data[i] - query[i];
      distance += diff * diff;
  }
  return distance;
}
float compute_relative_distance_error(int k, const Points& dataset, const Point& query, const vector<PointId>& groundtruth, const vector<DistanceToPoint>& result)
{
	float dist = 0.0;
    for (int i = 0; i <k; ++i) //dists.rows
	{
		float d_qr = get_distance(dataset[result[i].second],query);   //query.cols
		float d_qg = get_distance(dataset[groundtruth[i]],query);
        float d=(d_qr - d_qg)/d_qg;
		if(d<=4)
			dist += d;
		else
			dist += 4; 
    } 
	return dist/k;     
}

float compute_number_closer (int k, const vector<PointId>& groundtruth, const vector<DistanceToPoint>& result)
{        
	float rate = 0.0;
	for (int gs_n=0;gs_n < k ;gs_n++)
	{
		for(int re_n=0; re_n < k; re_n++)
		{
			if(groundtruth[gs_n]==result[re_n].second)
			{
				rate += (float)(gs_n+1)/(re_n+1);
				break;
			}
		}
    }
    return rate / k;
}

float compute_mean_reciprocal_rank (int k, const vector<PointId>& groundtruth, const vector<DistanceToPoint>& result)
{
	float sum = 0;
	float rate = 0.0;
	for(int re_n=0; re_n < k; re_n++)
	{
		if(groundtruth[0]==result[re_n].second)
		{
			rate += 1.0/(re_n+1);
			break;
		}
    }
    return rate;
}

float compute_mean_average_precision (int k, const vector<PointId>& groundtruth, const vector<DistanceToPoint>& result)
{
	float rate = 0.0;
	int found_last = 0;
	std::set<PointId> gnds;
	for(int i=0; i <k; i++)
	{  	
      	gnds.insert(groundtruth[i]);
		int count=0;
		for(int j=0;j<=i;j++)
      		if(gnds.find(result[j].second) != gnds.end()) 
			{
				count ++;
      		}
		rate += count*1.0 * (count-found_last)/(i+1);
		found_last=count;
    }
    return rate / k;
}

float compute_discounted_culmulative_gain(int k, const vector<PointId>& groundtruth, const vector<DistanceToPoint>& result)
{
	float rate = 0.0;

	std::set<PointId> gnds;
  	for(int i = 0; i <k; ++i) {
      	gnds.insert(groundtruth[i]);
  	}
	for(unsigned i=0; i <k; i++)
	{
      	if(gnds.find(result[i].second) != gnds.end()) 
		{
         	rate += 1.0/log2(i+2);
      	}
    }
    return rate;
}

