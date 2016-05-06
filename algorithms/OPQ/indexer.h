/** @file */

// Copyright 2012 Yandex Artem Babenko
#ifndef INDEXER_H_
#define INDEXER_H_

#include <ctime>
#include <map>

#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>

#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string.hpp>

#include <boost/lexical_cast.hpp>

#include <boost/serialization/serialization.hpp>
#include <boost/serialization/set.hpp>
#include <boost/serialization/vector.hpp>

#include "data_util.h"
#include "multitable.hpp"
#include <sstream>
#include <string>
#include <vector>

using std::ifstream;
using std::map;
using std::multimap;
using std::ofstream;
using std::string;
using std::pair;
using std::make_pair;

using boost::lexical_cast;
using boost::split;

extern int THREADS_COUNT;

extern Dimensions SPACE_DIMENSION;

extern enum PointType point_type;

IndexConfig gConfig;

/**
 * This is the main class for creating multiindex for a set of points
 * in a multidimensional space. Clusterization and vocabs learning happen
 * outside of this class, multiindexer receives prepared vocabs in input
 */
template<class Record>
class MultiIndexer { 
 public:
 /**
  * This is the simple MultiIndexer constructor
  * @param multiplicity how many parts input points will be divide on
  */
  MultiIndexer(const int multiplicity = 2);
 /**
  * This is the main function of MultiIndexer
  * @param points_filename file with points in .fvecs or .bvecs format
  * @param points_count how many points should we index
  * @param coarse_vocabs vocabularies for coarse quantization
  * @param fine_vocabs vocabularies for fine quantization for reranking
  * @param mode determines the way of rerank info calculating
  * @param build_coarse_quantization should we get coarse quantization or not
  * @param files_prefix all index filenames will have this prefix
  * @param coarse_quantization_filename file with coarse quantization (if exists)
  */
  void BuildMultiIndex(const string& points_filename,
                       const string& metainfo_filename,
                       const int points_count,
                       const vector<Centroids>& coarse_vocabs,
                       const vector<Centroids>& fine_vocabs,
                       const RerankMode& mode,
                       const bool build_coarse_quantization,
                       const string& files_prefix,
                       const string& coarse_quantization_filename = "");
 private:
	

  int IsExist(const vector<string>* inverts,const string hex);
  void report(const vector<vector<ClusterId> >* transposed_coarse_quantizations, int coarse_k,const vector<Centroids>& coarse_vocabs);
  


 /**
  * This function prepares for each point its coarse quantization
  * @param points_filename file with points in .fvecs or .bvecs format
  * @param points_count how many points should we handle
  * @param coarse_vocabs vocabularies for coarse quantization
  */
  void PrepareCoarseQuantization(const string& points_filename,
                                 const int points_count,
                                 const vector<Centroids>& coarse_vocabs);
 /**
  * This function prepares for each point in subset its coarse quantization
  * @param points_filename file with points in .fvecs or .bvecs format
  * @param start_pid identifier of the first point in subset
  * @param subset_size points count in subset
  * @param coarse_vocabs vocabularies for coarse quantization
  * @param transposed_coarse_quantizations result
  */
  void GetCoarseQuantizationsForSubset(const string& points_filename,
                                       const int start_pid,
                                       const int subset_size,
                                       const vector<Centroids>& coarse_vocabs,
                                       vector<vector<ClusterId> >*
                                       transposed_coarse_quantizations);
 /**
  * This function serializes prepared coarse quantizations to file
  * @param transposed_coarse_quantizations quantizations to serialize.
  * They are transposed because of effective memory usage
  * @param filename file we should serialize to
  */
  void SerializeCoarseQuantizations(const vector<vector<ClusterId> >&
                                    transposed_coarse_quantizations,
                                    const string& filename);
 /**
  * This function saves index to files.
  * All filenames start form the common files prefix
  */
  void SerializeMultiIndexFiles();
 /**
  * This function converts counts of points in cells to cell edges
  */
  void ConvertPointsInCellsCountToCellEdges();

 /**
  * This function fills multiindex data structures.
  * @param points_filename file with points in .fvecs or .bvecs format
  * @param points_count how many points should we index
  * @param coarse_vocabs vocabularies for coarse quantization
  * @param fine_vocabs vocabularies for fine quantization for reranking
  * @param mode determines the way of rerank info calculating
  */
  void FillMultiIndex(const string& points_filename,
                      const int points_count,
                      const vector<Centroids>& coarse_vocabs,
                      const vector<Centroids>& fine_vocabs,
                      const RerankMode& mode);
 /**
  * This function fills multiindex data structures.
  * @param points_filename file with points in .fvecs or .bvecs format
  * @param start_pid identifier of the first point in subset
  * @param subset_size points count in subset
  * @param coarse_vocabs vocabularies for coarse quantization
  * @param fine_vocabs vocabularies for fine quantization for reranking
  * @param mode determines the way of rerank info calculating
  * @param points_written_in_index auxillary structure for correct index filling
  */
  void FillMultiIndexForSubset(const string& points_filename,
                               const PointId start_pid,
                               const int points_count,
                               const vector<Centroids>& coarse_vocabs,
                               const vector<Centroids>& fine_vocabs,
                               const RerankMode& mode,
                               Multitable<int>* points_written_in_index);

 /**
  * This function reads point coarse quantization from file
  * @param pid identifier of target point
  * @param filename file with coarse quantizations
  * @param coarse_quantization result
  */
  void GetPointCoarseQuantization(const PointId pid,
                                  const string& filename,
                                  vector<ClusterId>* coarse_quantization);
 /**
  * This function calculates rerank info for point
  * @param point target point
  * @param pid identifier of target point
  * @param fine_vocabs vocabularies for rerank info calculation
  */
  void FillPointRerankInfo(const Point& point,
                           const PointId pid,
                           const vector<Centroids>& fine_vocabs);
 /**
  * This function restores counts of points from coarse quantizations
  * @param points_filename file with points in .fvecs or .bvecs format
  * @param points_count how many points should we index
  * @param coarse_vocabs vocabularies for coarse quantization
  * We need them to init counts table correctly
  */
  void RestorePointsInCellsCountFromCourseQuantization(const string& points_filename,
                                                       const int points_count,
                                                       const vector<Centroids>& coarse_vocabs);
 /**
  * This simple function returns size of one coordinate of input point
  */
  int GetInputCoordSizeof();
 /**
  * This simple function reads one point from input stream
  * @param input input stream
  * @param point result point
  */
  void ReadPoint(ifstream& input, Point* point);
 /**
  * Initialize all structures for BLAS operations
  * @param coarse_vocabs coarse vocabularies
  */
  void InitBlasStructures(const vector<Centroids>& coarse_vocabs);	
 /**
  *  All index filenames will start from this prefix
  */
  string files_prefix_;
 /**
  *  Filename of file with coarse quantizations
  */
  string coarse_quantization_filename_;
 /**
  *  Multiplicity (how many parts point space is divided on)
  */
  int multiplicity_;
 /**
  *  Table with number of points in each cell
  */
  Multitable<int> point_in_cells_count_;
 /**
  *  Multiindex
  */
  MultiIndex<Record> multiindex_;
 /**
  *  Mutex for critical section in filling index stage
  */
  boost::mutex cell_counts_mutex_;
 /**
  * Struct for BLAS
  */
  vector<float*> coarse_vocabs_matrices_;
 /**
  * Struct for BLAS
  */
  vector<vector<float> > coarse_centroids_norms_;

};

template<class Record>
inline void GetRecord(const Point& point, const PointId pid,
                      const vector<ClusterId> coarse_quantization,
                      const vector<Centroids>& coarse_vocabs,
                      Record* result) {
}

template<class Record>
void InitParameters(const vector<Centroids>& fine_vocabs,
                    const RerankMode& mode,
                    const string& metainfo_filename) {
  gConfig.fine_vocabs = fine_vocabs;
  gConfig.rerank_mode = mode;
}


//////////////////// IMPLEMENTATION //////////////////////
template<class Record>
MultiIndexer<Record>::MultiIndexer(const int multiplicity) {
  if(multiplicity < 0) {
    throw std::logic_error("Multiplicity < 0");
  }
  multiplicity_ = multiplicity;
}

template<class Record>
int MultiIndexer<Record>::GetInputCoordSizeof() {
  if(point_type == FVEC) {
    return (int)sizeof(float);
  } else if(point_type == BVEC) {
    return (int)sizeof(unsigned char);
  }
}

template<class Record>
void MultiIndexer<Record>::ReadPoint(ifstream& input, Point* point) {
  if(!input.good()) {
    throw std::logic_error("Bad input stream");
  }
  if(point_type == FVEC) {
    ReadVector<float, Coord>(input, point);
  } else if(point_type == BVEC) {
    ReadVector<unsigned char, Coord>(input, point);
  }    
}

template<class Record>
void MultiIndexer<Record>::SerializeCoarseQuantizations(const vector<vector<ClusterId> >&
		                                                          transposed_coarse_quantizations,
                                                        const string& filename) {
  ofstream quantizations_stream;
  quantizations_stream.open(filename.c_str(), ios::binary);
  if(!quantizations_stream.good()) {
    throw std::logic_error("Bad input stream");
  }
  cout << "Writing coarse quantizations started" << endl;
  for(PointId pid = 0; pid < transposed_coarse_quantizations[0].size(); ++pid) {
    for(int subspace_index = 0; subspace_index < multiplicity_; ++subspace_index) {
      ClusterId quantization = transposed_coarse_quantizations[subspace_index][pid];
      quantizations_stream.write((char*)&quantization, sizeof(quantization));
    }
  }
  quantizations_stream.close();
  cout << "Writing coarse quantizations started" << endl;
}

template<class Record>
void MultiIndexer<Record>::SerializeMultiIndexFiles() {
  cout << "Start multiindex serializing....\n";
  ofstream cell_edges(string(files_prefix_ + "_cell_edges.bin").c_str(), ios::binary);
  boost::archive::binary_oarchive arc_cell_edges(cell_edges);
  arc_cell_edges << multiindex_.cell_edges;
  ofstream multi_array(string(files_prefix_ + "_multi_array.bin").c_str(), ios::binary);
  boost::archive::binary_oarchive arc_multi_array(multi_array);
  arc_multi_array << multiindex_.multiindex;
  cout << "Finish multiindex serializing....\n";
}

template<class Record>
int MultiIndexer<Record>::IsExist(const vector<string>* inverts, const string hex)
{
	for(int i=0;i<inverts->size();i++)
	{
		if(inverts->at(i).compare(hex)==0)
			return i;
	}
	return -1;
}
/*
template<class Record>
void MultiIndexer<Record>::report(const vector<vector<ClusterId> >* transposed_coarse_quantizations, int coarse_k)
{
	int data_size = transposed_coarse_quantizations->at(0).size();
	cout<<data_size<<" ";
	int m=2;
    vector<std::string> bit_set;
	vector<int> counts(data_size,0);
	for(int i=0; i<data_size ;i++)
	{
		std::stringstream ss;
		for(int j=0;j<m;j++)
			ss<<transposed_coarse_quantizations->at(j)[i]<<";";
		std::string hex=ss.str();
		int index = IsExist(&bit_set,hex);
		if(index==-1)
		{
			bit_set.push_back(hex);
			counts[bit_set.size()-1]++;
		}
		else
		{
			counts[index]++;
		}
	}
	if(bit_set.size()==data_size)
	{
		cout<<"Failed!!!"<<endl;
		return;
	}
	cout<<bit_set.size()<<" "<<bit_set.size()*1.0/coarse_k/coarse_k<<endl;
	sort(counts.begin(),counts.begin()+bit_set.size());
	int min = counts[0];
	int max = counts[bit_set.size()-1];
	float avg = data_size * 1.0 / bit_set.size();
	float var = 0;
	for(int i=bit_set.size()-1; i>=0;i--)
	{
		float tmp = counts[i]-avg ; 
		var += tmp * tmp ;
	}
	var /= bit_set.size();
	cout<<"\n"<<min<<" "<<max<<" "<<avg<<" "<<var<<"\n"<<endl;
	for(int i=bit_set.size()-1; i>=0;i--)
	{
		cout<<counts[i]<<" ";
	}

}*/

template<class Record>
void MultiIndexer<Record>::report(const vector<vector<ClusterId> >* transposed_coarse_quantizations, int coarse_k,const vector<Centroids>& coarse_vocabs)
{
	int data_size = transposed_coarse_quantizations->at(0).size();
	int m=2;
    vector<std::string> bit_set;
	vector<pair<int,int> > center_ids;
	vector<int> counts(data_size,0);
	vector<vector<int> > ds;
	for(int i=0; i<data_size ;i++)
	{
		std::stringstream ss;
		for(int j=0;j<m;j++)
			ss<<transposed_coarse_quantizations->at(j)[i]<<";";
		std::string hex=ss.str();
		int index = IsExist(&bit_set,hex);
		if(index==-1)
		{
			bit_set.push_back(hex);
			counts[bit_set.size()-1]++;
			vector<int> dd;
			dd.push_back(i);
			ds.push_back(dd);
			center_ids.push_back(make_pair(transposed_coarse_quantizations->at(0)[i],transposed_coarse_quantizations->at(1)[i]));
		}
		else
		{
			counts[index]++;
			vector<int> dd=ds[index];
			dd.push_back(i);
			ds[index]=dd;
		}
	}

	FILE * fp =fopen("index.opq","wb");
	int cluster_size = bit_set.size();
	fwrite(&cluster_size,sizeof(int),1,fp);
	cout<<cluster_size<<endl;
	for(int i=0; i<bit_set.size();i++)
	{
		float * center = new float[SPACE_DIMENSION];
		int lindex = center_ids[i].first;
		int rindex = center_ids[i].second;
		for (int j=0;j<SPACE_DIMENSION/2;j++)
			center[j] = coarse_vocabs.at(0)[lindex][j];
		for (int j=0;j<SPACE_DIMENSION/2;j++)
			center[j+SPACE_DIMENSION/2] = coarse_vocabs.at(1)[rindex][j];
		fwrite(center,sizeof(float),SPACE_DIMENSION,fp);

		int size = counts[i];
		fwrite(&size,sizeof(int),1,fp);
		int * ids = new int[size];
		for(int j=0;j<size;j++)
			ids[j]=ds[i][j];
		fwrite(ids,sizeof(int),size,fp);
	}
	fclose(fp);
}
template<class Record>
void MultiIndexer<Record>::GetCoarseQuantizationsForSubset(const string& points_filename,
                                                           const int start_pid,
                                                           const int subset_size,
                                                           const vector<Centroids>& coarse_vocabs,
                                                           vector<vector<ClusterId> >*
                                                                  transposed_coarse_quantizations) {


  ifstream point_stream;
  point_stream.open(points_filename.c_str(), ios::binary);
  if(!point_stream.good()) {
    throw std::logic_error("Bad input points stream");
  }
  // we assume points are stored in .fvecs or .bvecs format
  point_stream.seekg(start_pid * (GetInputCoordSizeof() * SPACE_DIMENSION + sizeof(Dimensions)), ios::beg);
  vector<ClusterId> coarse_quantization(multiplicity_);
  //cout<<"subset_size:" << subset_size << endl;
  for(int point_number = 0; point_number < subset_size; ++point_number) {
    /*if(point_number % 10000 == 0) {
      cout << "Getting coarse quantization, point # " << start_pid + point_number << endl;
    }*/
    //cout<<"point number:"<<point_number<<endl;
    Point current_point;
    ReadPoint(point_stream, &current_point);
    int subpoints_dimension = SPACE_DIMENSION / multiplicity_;
    for(int coarse_index = 0; coarse_index < multiplicity_; ++coarse_index) {
      Dimensions start_dim = coarse_index * subpoints_dimension;
      Dimensions final_dim = start_dim + subpoints_dimension;
      ClusterId nearest = GetNearestClusterId(current_point, coarse_vocabs.at(coarse_index),
                                              start_dim, final_dim);
      //cout<<"coarse_index:"<<coarse_index<<" nearest:"<<nearest<<endl;
      transposed_coarse_quantizations->at(coarse_index)[start_pid + point_number] = nearest;
      coarse_quantization[coarse_index] = nearest;
      cblas_saxpy(subpoints_dimension, -1, &(coarse_vocabs.at(coarse_index)[nearest][0]), 1, &(current_point[start_dim]), 1);
    }
    
    int global_index = point_in_cells_count_.GetCellGlobalIndex(coarse_quantization);
    cell_counts_mutex_.lock();
    ++(point_in_cells_count_.table[global_index]);
    cell_counts_mutex_.unlock();
  }
	
  //cout<<"transposed:"<<transposed_coarse_quantizations->size()<<" "<<transposed_coarse_quantizations->at(0).size()<<endl;

  //cout<<coarse_vocabs.size()<<" "<<coarse_vocabs[0].size()<<endl;
  //report(transposed_coarse_quantizations, coarse_vocabs[0].size(),coarse_vocabs);
}



template<class Record>
void MultiIndexer<Record>::PrepareCoarseQuantization(const string& points_filename,
                                                     const int points_count,
                                                     const vector<Centroids>& coarse_vocabs) {
  // we use transposed quantizations for efficient memory usage
  vector<vector<ClusterId> > transposed_coarse_quantizations; 
  transposed_coarse_quantizations.resize(multiplicity_);
  vector<int> multiindex_table_dimensions;
  for(int i = 0; i < multiplicity_; ++i) {
    transposed_coarse_quantizations[i].resize(points_count);
    multiindex_table_dimensions.push_back(coarse_vocabs[i].size());
  }
  point_in_cells_count_.Resize(multiindex_table_dimensions);
  cout << "Memory for coarse quantizations allocated" << endl;
  boost::thread_group index_threads;
  int thread_points_count = points_count / THREADS_COUNT;
  for(int thread_id = 0; thread_id < THREADS_COUNT; ++thread_id) {
    PointId start_pid = thread_points_count * thread_id;
    index_threads.create_thread(boost::bind(&MultiIndexer::GetCoarseQuantizationsForSubset,
                                            this, points_filename, start_pid, thread_points_count,
                                            boost::cref(coarse_vocabs), &transposed_coarse_quantizations));
  }
  index_threads.join_all();
  if(coarse_quantization_filename_.empty()) {
    coarse_quantization_filename_ = files_prefix_ + "_coarse_quantizations.bin";
  }
  cout << "Coarse quantizations are calculated" << endl;
  SerializeCoarseQuantizations(transposed_coarse_quantizations, coarse_quantization_filename_);
  cout << "Coarse quantizations are serialized" << endl;
}

template<class Record>
void MultiIndexer<Record>::ConvertPointsInCellsCountToCellEdges() {
  cout << "Converting points in cells to cell edges...\n";
  multiindex_.cell_edges = point_in_cells_count_;
  multiindex_.cell_edges.table[0] = 0;
  for(int global_index = 1;
      global_index < point_in_cells_count_.table.size();
      ++global_index) {
    multiindex_.cell_edges.table[global_index] = multiindex_.cell_edges.table[global_index - 1] +
                                                 point_in_cells_count_.table[global_index - 1];
  }
  // we do not need this table more
  point_in_cells_count_.table.clear();
  cout << "Finish converting points in cells to cell edges...\n";
}

template<class Record>
void MultiIndexer<Record>::GetPointCoarseQuantization(const PointId pid,
                                                      const string& filename,
                                                      vector<ClusterId>* coarse_quantization) {
  ifstream coarse_quantization_stream;
  coarse_quantization_stream.open(filename.c_str(), ios::binary);
  if(!coarse_quantization_stream.good()) {
    throw std::logic_error("Bad input coarse quantizations stream");
  }
  coarse_quantization_stream.seekg((long long)pid * sizeof(ClusterId) * multiplicity_, ios::beg);
  for(int coarse_index = 0; coarse_index < multiplicity_; ++coarse_index) {
    coarse_quantization_stream.read((char*)&(coarse_quantization->at(coarse_index)),
                                    sizeof(coarse_quantization->at(coarse_index)));
  }
}

template<class Record>
void MultiIndexer<Record>::FillMultiIndexForSubset(const string& points_filename,
                                                   const PointId start_pid,
                                                   const int points_count,
                                                   const vector<Centroids>& coarse_vocabs,
                                                   const vector<Centroids>& fine_vocabs,
                                                   const RerankMode& mode,
                                                   Multitable<int>* points_written_in_index) {
  ifstream point_stream;
  point_stream.open(points_filename.c_str(), ios::binary);
  if(!point_stream.good()) {
    throw std::logic_error("Bad input points stream");
  }
  point_stream.seekg((long long)start_pid * (GetInputCoordSizeof() * SPACE_DIMENSION + sizeof(Dimensions)), ios::beg);
  for(int point_number = 0; point_number < points_count; ++point_number) {
    if(point_number % 10000 == 0) {
      cout << "Filling multiindex, point # " << start_pid + point_number << endl;
    }
  Point current_point;
  ReadPoint(point_stream, &current_point);
  vector<ClusterId> coarse_quantization(multiplicity_);
  GetPointCoarseQuantization(start_pid + point_number,
                             coarse_quantization_filename_,
                             &coarse_quantization);
  int current_written_count = points_written_in_index->GetValue(coarse_quantization);
  int pid_multiindex = multiindex_.cell_edges.GetValue(coarse_quantization) + current_written_count;
  GetRecord<Record>(current_point, start_pid + point_number,
                    coarse_quantization, coarse_vocabs, &(multiindex_.multiindex[pid_multiindex]));
  cell_counts_mutex_.lock();
  points_written_in_index->SetValue(current_written_count + 1, coarse_quantization);
  cell_counts_mutex_.unlock();
  }
}

template<class Record>
void MultiIndexer<Record>::FillMultiIndex(const string& points_filename,
                                          const int points_count,
                                          const vector<Centroids>& coarse_vocabs,
                                          const vector<Centroids>& fine_vocabs,
                                          const RerankMode& mode) {
  ConvertPointsInCellsCountToCellEdges();
  multiindex_.multiindex.resize(points_count);
  cout << "Indexing started..." << endl;

  Multitable<int> points_written_in_index(multiindex_.cell_edges.dimensions);
  int thread_points_count = points_count / THREADS_COUNT;
  boost::thread_group threads;
  for(int thread_id = 0; thread_id < THREADS_COUNT; ++thread_id) {
    PointId start_pid = thread_points_count * thread_id;
    threads.create_thread(boost::bind(&MultiIndexer::FillMultiIndexForSubset, this, points_filename, start_pid,
                                      thread_points_count, boost::cref(coarse_vocabs),
                                      boost::cref(fine_vocabs), mode, &points_written_in_index));
  }
  threads.join_all();
  cout << "Indexing finished..." << endl;
}

template<class Record>
void MultiIndexer<Record>::RestorePointsInCellsCountFromCourseQuantization(const string& points_filename,
                                                                           const int points_count,
                                                                           const vector<Centroids>& coarse_vocabs) {
  vector<int> dimensions;
  for(int i = 0; i < multiplicity_; ++i) {
    dimensions.push_back(coarse_vocabs[i].size());
  }
  point_in_cells_count_.Resize(dimensions);
  ifstream coarse_quantization_stream;
  coarse_quantization_stream.open(coarse_quantization_filename_.c_str(), ios::binary);
  if(!coarse_quantization_stream.good()) {
    throw std::logic_error("Bad input coarse quantizations stream");
  }
  CoarseQuantization quantization(multiplicity_);
  for(PointId pid = 0; pid < points_count; ++pid) {
    if(pid % 100000 == 0) {
      cout << pid << endl;
    }
    for(int subspace_index = 0; subspace_index < multiplicity_; ++subspace_index) {
      coarse_quantization_stream.read((char*)&(quantization[subspace_index]), 
                                      sizeof(ClusterId));
    }
    int cell_global_index = point_in_cells_count_.GetCellGlobalIndex(quantization);
    point_in_cells_count_.table[cell_global_index] += 1;
  }
}

template<class Record>
void MultiIndexer<Record>::BuildMultiIndex(const string& points_filename,
                                           const string& metainfo_filename,
                                           const int points_count,
                                           const vector<Centroids>& coarse_vocabs,
                                           const vector<Centroids>& fine_vocabs,
                                           const RerankMode& mode,
                                           const bool build_coarse_quantization,
                                           const string& files_prefix,
                                           const string& coarse_quantization_filename) {
  InitParameters<Record>(fine_vocabs, mode, metainfo_filename);
  InitBlasStructures(coarse_vocabs);
  files_prefix_ = files_prefix;
  coarse_quantization_filename_ = coarse_quantization_filename;
  if(build_coarse_quantization) {
    PrepareCoarseQuantization(points_filename, points_count, coarse_vocabs);
  } else {
  RestorePointsInCellsCountFromCourseQuantization(points_filename,
                                                  points_count,
                                                  coarse_vocabs);
  }
  FillMultiIndex(points_filename, points_count, coarse_vocabs, fine_vocabs, mode);
  cout << "Multiindex created" << endl;
  SerializeMultiIndexFiles();
  cout << "Multiindex serialized" << endl;
}

template<class Record>
void MultiIndexer<Record>::InitBlasStructures(const vector<Centroids>& coarse_vocabs) {
  coarse_vocabs_matrices_.resize(coarse_vocabs.size());
  coarse_centroids_norms_.resize(coarse_vocabs.size(), vector<float>(coarse_vocabs[0].size()));
  for(int coarse_id = 0; coarse_id < coarse_vocabs_matrices_.size(); ++coarse_id) {
    coarse_vocabs_matrices_[coarse_id] = new float[coarse_vocabs[0].size() * coarse_vocabs[0][0].size()];
    for(int i = 0; i < coarse_vocabs[0].size(); ++i) {
      Coord norm = 0;
      for(int j = 0; j < coarse_vocabs[0][0].size(); ++j) {
        coarse_vocabs_matrices_[coarse_id][coarse_vocabs[0][0].size() * i + j] = coarse_vocabs[coarse_id][i][j];
        norm += coarse_vocabs[coarse_id][i][j] * coarse_vocabs[coarse_id][i][j];
      }
      coarse_centroids_norms_[coarse_id][i] = norm;
    }
  }
}

template<>
inline void GetRecord<PointId> (const Point& point, const PointId pid,
                                const vector<ClusterId> coarse_quantization,
                                const vector<Centroids>& coarse_vocabs,
                                PointId* result) {
  *result = pid;
}

inline void FillAdcInfo(const Point& point, const PointId pid,
                        const vector<Centroids>& fine_vocabs,
                        char* result) {
  int subvectors_count = fine_vocabs.size();
  int subvector_dim = point.size() / subvectors_count;
  for(int subvector_index = 0; subvector_index < subvectors_count; ++subvector_index) {
    Dimensions start_dim = subvector_index * subvector_dim;
    Dimensions final_dim = start_dim + subvector_dim;
    *((FineClusterId*)result) = (FineClusterId)GetNearestClusterId(point, fine_vocabs[subvector_index],
			                                                       start_dim, final_dim);
    result += sizeof(FineClusterId);
  }
}

template<>
inline void GetRecord<RerankADC8> (const Point& point, const PointId pid,
                                   const vector<ClusterId> coarse_quantization,
                                   const vector<Centroids>& coarse_vocabs,
                                   RerankADC8* result) {
  result->pid = pid;
  char* rerank_info_ptr = (char*)result + sizeof(pid);
  if(gConfig.rerank_mode == USE_RESIDUALS) {
    Point residual;
    GetResidual(point, coarse_quantization, coarse_vocabs, &residual);
    FillAdcInfo(residual, pid, gConfig.fine_vocabs, rerank_info_ptr);
  } else if (gConfig.rerank_mode == USE_INIT_POINTS) {
    FillAdcInfo(point, pid, gConfig.fine_vocabs, rerank_info_ptr);
  }
}

template<>
inline void GetRecord<RerankADC16> (const Point& point, const PointId pid,
                                    const vector<ClusterId> coarse_quantization,
                                    const vector<Centroids>& coarse_vocabs,
                                    RerankADC16* result) {
  result->pid = pid;
  char* rerank_info_ptr = (char*)result + sizeof(pid);
  if(gConfig.rerank_mode == USE_RESIDUALS) {
    Point residual;
    GetResidual(point, coarse_quantization, coarse_vocabs, &residual);
    FillAdcInfo(residual, pid, gConfig.fine_vocabs, rerank_info_ptr);
  } else if (gConfig.rerank_mode == USE_INIT_POINTS) {
    FillAdcInfo(point, pid, gConfig.fine_vocabs, rerank_info_ptr);
  }
}

#endif




