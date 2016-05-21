// Copyright 2012 Yandex Artem Babenko
#include <iostream>

#include <boost/program_options.hpp>
#include <sys/time.h>
#include <mkl.h>

#include "searcher.h"
#include "indexer.h"

using namespace boost::program_options;
using std::endl;
using std::ofstream;

/**
 * Number of threads for indexing
 */
Dimensions SPACE_DIMENSION;
/**
 * File with vocabularies for multiindex structure
 */
string coarse_vocabs_file;
/**
 * File with vocabularies for reranking
 */
string fine_vocabs_file;
/**
 * Reranking approach, should be USE_RESIDUALS or USE_INIT_POINTS
 */
RerankMode mode;
/**
 * Common prefix of all multiindex files
 */
string index_files_prefix;
/**
 * File with queries (.bvec or .fvec)
 */
string queries_file;
/**
 * Type, should be BVEC or FVEC
 */
PointType query_point_type;
/**
 * File with groundtruth (.ivec)
 */
string groundtruth_file;
/**
 * Number of queries to search
 */
int queries_count;
/**
 * Should we rerank?
 */
bool do_rerank;
/**
 * Number of neighbours to look over
 */
int neighbours_count;
/**
 * File to write report in
 */
string report_file;
/**
 * Number of nearest centroids for each group of dimensions to handle
 */
int subspaces_centroids_count;

string data_file;
int data_count;

int use_originaldata;

int k;

int SetOptions(int argc, char** argv) {
  options_description description("Options");
  description.add_options()
    ("index_files_prefix,i", value<string>())
    ("queries_file,q", value<string>())
    ("queries_count,n", value<int>())
    ("neighbours_count,k", value<int>())
    ("groundtruth_file,g", value<string>())
    ("data_file,a", value<string>())
	("data_count,m", value<int>())
    ("coarse_vocabs_file,c", value<string>())
    ("fine_vocabs_file,f", value<string>())
    ("query_point_type,t", value<string>())
    ("use_residuals,r", value<int>())
    ("do_rerank,l", value<int>()) 
    ("report_file,o", value<string>())
    ("space_dim,D", value<int>())
	("dim,d", value<int>())
	("use_originaldata,u", value<int>())
    ("subspaces_centroids_count,s", value<int>())
    ("k_neighbors,K", value<int>());

  variables_map name_to_value;
  try {
    store(command_line_parser(argc, argv).options(description).run(), name_to_value);
  } catch (const invalid_command_line_syntax &inv_syntax) {
    switch (inv_syntax.kind()) {
      case invalid_syntax::missing_parameter :
        cout << "Missing argument for option '" << inv_syntax.tokens() << "'.\n";
        break;
      default:
        cout << "Syntax error, kind " << int(inv_syntax.kind()) << "\n";
        break;
       };
    return 1;
  } catch (const unknown_option &unkn_opt) {
    cout << "Unknown option '" << unkn_opt.get_option_name() << "'\n";
    return 1;
  }
  if (name_to_value.count("help")) {
    cout << description << "\n";
    return 1;
  }

  coarse_vocabs_file =         name_to_value["coarse_vocabs_file"].as<string>();
  fine_vocabs_file =           name_to_value["fine_vocabs_file"].as<string>();
  SPACE_DIMENSION =            name_to_value["dim"].as<int>();
  index_files_prefix =         name_to_value["index_files_prefix"].as<string>();
  queries_file =               name_to_value["queries_file"].as<string>();
  report_file =                name_to_value["report_file"].as<string>();
  groundtruth_file =           name_to_value["groundtruth_file"].as<string>();
  queries_count =              name_to_value["queries_count"].as<int>();
  neighbours_count =           name_to_value["neighbours_count"].as<int>();
  subspaces_centroids_count =  name_to_value["subspaces_centroids_count"].as<int>();
  data_file=                   name_to_value["data_file"].as<string>();
  data_count =                 name_to_value["data_count"].as<int>();
  use_originaldata =           name_to_value["use_originaldata"].as<int>();
  k =                          name_to_value["k_neighbors"].as<int>();

  do_rerank =                  (name_to_value["do_rerank"].as<int>() == 0) ? true : false;
  //cout<< "rerank:"<<do_rerank<<endl;
  mode =                       (name_to_value["use_residuals"].as<int>() == 0) ? USE_RESIDUALS : USE_INIT_POINTS;

  if (name_to_value["query_point_type"].as<string>() == "FVEC") {
    query_point_type = FVEC;
  } else if(name_to_value["query_point_type"].as<string>() == "BVEC") {
    query_point_type = BVEC;
  }
  return 0;
}

float diff_timeval(timeval t1, timeval t2) {
  return (float) (t1.tv_sec - t2.tv_sec) + (t1.tv_usec - t2.tv_usec) * 1e-6;
}

template<class TSearcher>
void TestSearcher(TSearcher& searcher,
                  const Points& queries,
                  const vector<vector<PointId> >& groundtruth,const Points& dataset) {
    searcher.Init(index_files_prefix, coarse_vocabs_file,
                fine_vocabs_file, mode,
                subspaces_centroids_count,
                do_rerank,use_originaldata); //,use_originaldata

	std::ofstream out(report_file.c_str(),ios::app);

    vector< vector< DistanceToPoint> > result;
    result.resize(queries_count);
    timeval start;
	timeval end;
    float time=0;

  	gettimeofday(&start, NULL);
    for(int i = 0; i < queries_count; ++i) //queries_count
    {
    	searcher.GetNearestNeighbours(queries[i],neighbours_count, &result[i],dataset);
    }
  	gettimeofday(&end, NULL);
  	time += diff_timeval(end, start);
    float recall = 0.0;
	for(int i = 0; i < queries_count; ++i) //queries_count
    {
      recall += GetRecall(k,groundtruth[i],result[i]);
	}

	recall = recall/queries_count;
	time= time/queries_count ;
	
    out.setf(ios::fixed);  
    out << recall<<" "<< time<<" #N_"<<neighbours_count<<" "<<endl;  
}

int main(int argc, char** argv) {
  SetOptions(argc, argv);
  cout << "Options are set ...\n";

  Points queries;
  if(query_point_type == BVEC) {
    ReadPoints<unsigned char, Coord>(queries_file, &queries, queries_count);
  } else if (query_point_type == FVEC) {
    ReadPoints<float, Coord>(queries_file, &queries, queries_count);
  }
  //dataset
  Points dataset;
  ReadPoints<float, Coord>(data_file, &dataset, data_count);
  cout << "Queries are read ...\n";
  vector<vector<PointId> > groundtruth;
  ReadPoints<int, PointId>(groundtruth_file, &groundtruth, queries_count);
  MKL_Set_Num_Threads(1);

  cout << "Groundtruth is read ...\n";
  vector<Centroids> fine_vocabs;
  ReadFineVocabs<float>(fine_vocabs_file, &fine_vocabs);
  if(fine_vocabs.size() == 8) {
    MultiSearcher<RerankADC8, PointId> searcher;
    TestSearcher<MultiSearcher<RerankADC8, PointId> > (searcher, queries, groundtruth,dataset);
  } else if(fine_vocabs.size() == 16) {
    MultiSearcher<RerankADC16, PointId> searcher;
    TestSearcher<MultiSearcher<RerankADC16, PointId> > (searcher, queries, groundtruth,dataset);
  }

  return 0;
}
