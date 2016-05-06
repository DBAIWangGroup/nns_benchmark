/** @file */

// Copyright 2012 Yandex Artem Babenko
#pragma once


#include <bitset>
#include <fstream>
#include <ios>
#include <iostream>
#include <map>
#include <set>
#include <vector>

#include <boost/bind.hpp>
#include <boost/thread.hpp>

#include "mkl_cblas.h"

#include "multitable.hpp"

using std::bitset;
using std::cout;
using std::ifstream;
using std::ios;
using std::endl;
using std::multimap;
using std::pair;
using std::set;
using std::string;
using std::vector;

/**
 * \typedef
 *  Data type for coordinate (bool, char, int, float, etc.)
 */
typedef float Coord;
/**
 * \typedef
 *  Data type for distance in multidimensional space
 */
typedef float Distance;
/**
 * \typedef
 *  Dimensionality of space = number of point coordinates
 */
typedef int Dimensions;
/**
 * \typedef
 *  Data type for point identifier
 */
typedef int PointId;
/**
 * \typedef
 *  Data type for cluster identifier
 */
typedef int ClusterId;
/**
 * \typedef
 *  Just vector of coordinates
 */
typedef vector<Coord> Point;
/**
 * \typedef
 *  Class for a number of points
 */
typedef vector<Point> Points;
/**
 * \typedef
 *  Class for a number of point identifiers
 */
typedef vector<PointId> PointIds;
/**
 * \typedef
 *  Class for a set of points
 */
typedef set<PointId> SetPoints;
/**
 * \typedef
 *  Class for a number of cluster identifiers
 */
typedef vector<ClusterId> ClusterIds;
/**
 * \typedef
 *  Class for representation of point coarse quantization,
 *  ids of nearest centroids for each group of dimensions
 */
typedef vector<ClusterId> CoarseQuantization;
/**
 * \typedef
 *  Data type for fine cluster identifier
 */
typedef unsigned char FineClusterId;
/**
 * \typedef
 *  Class for representation of point fine quantization
 */
typedef vector<FineClusterId> FineQuantization;
/**
 * \typedef
 *  Class for clusters representation
 *  ClusterId -> (PointId, PointId, PointId, .... )
 */
typedef vector<SetPoints> ClustersToPoints;
/**
 * \typedef
 *  Class for belonging to clusters representation
 *  PointId -> ClusterId
 */
typedef std::vector<ClusterId> PointsToClusters; 
/**
 * \typedef
 *  Centroids of the clustered points set
 */
typedef std::vector<Point> Centroids;

/**
 * \enum This enumeration presents different types of input point
 *       coordinate can be float or uint8
 */
enum PointType {
  FVEC,
  BVEC
};

/**
 * \enum This enumeration presents different ways to get rerank info.
 * Algorithm can quantize residuals after coarse quantization or init points without
 * centroids subtraction
 */
enum RerankMode {
  USE_RESIDUALS, 
  USE_INIT_POINTS
};

/**
 * \struct MultiIndex incorporates all data structures we need to make search
 */
template<class Record>
struct MultiIndex {
  vector<Record> multiindex;
  Multitable<int> cell_edges;    ///< Table with index cell edges in array
};

/**
 * Function calculates squared euclidian distance between two points (points must have the same dimensionality)
 * @param x first point
 * @param y second point
 */
Distance Eucldistance(const Point& x, const Point& y);
/**
 * Function calculates squared euclidian distance point with small dimensionality and 
 * subpoint of point with bigger dimensionality.  
 * @param x first point
 * @param y second point
 * @param start first dimension of subpoint
 * @param finish dimension after the last dimension of subpoint
 */
Distance Eucldistance(const Point& x, const Point& y, Dimensions start, Dimensions finish);

/**
 * This simple function casts number of type T to the nearest number of type U 
 */
template<class T, class U>
inline U Round(T number) {
  return (U)(number);
}

/**
 * Function reads point written in .fvecs or .bvecs format.
 * Input points have coordinates of type T.
 * Result points have coordinates of type U
 * @param filename .fvecs or .bvecs file name
 * @param points_count how many points to read
 * @param points result list of read points
 */
template<class T, class U>
void ReadPoints(const string& filename,
                vector<vector<U> >* points,
                int count) {
  ifstream input;
	input.open(filename.c_str(), ios::binary);
  if(!input.good()) {
    throw std::logic_error("Invalid filename");
  }
  points->resize(count);
  int dimension;
  for(PointId pid = 0; pid < count; ++pid) {
    input.read((char*)&dimension, sizeof(dimension));
    if(dimension <= 0) {
      throw std::logic_error("Bad file content: non-positive dimension");
    }
    points->at(pid).resize(dimension);
    for(Dimensions d = 0; d < dimension; ++d) {
      T buffer;
      input.read((char*)&(buffer), sizeof(T));
      points->at(pid)[d] = Round<T, U>(buffer);
    }
  }
}

/**
 * Function reads one vector of coordinates of type T.
 * Function assumes that the first int32-number in input stream is
 * vector dimensionality. Result vector will have coordinates of type U.
 * @param input input stream
 * @param v result vector
 */
template<class T, class U>
void ReadVector(ifstream& input, vector<U>* v) {
  if(!input.good()) {
    throw std::logic_error("Bad input stream");
  }
  int dimension;
  input.read((char*)&dimension, sizeof(dimension));
  if(dimension <= 0) {
    throw std::logic_error("Bad file content: non-positive dimension");
  }
  v->resize(dimension);
  for(Dimensions d = 0; d < dimension; ++d) {
    T buffer;
    input.read((char*)&buffer, sizeof(buffer));
    v->at(d) = Round<T, U>(buffer);
  }    
}

/**
 * Function reads vocabulary of centroids produced by matlab script.
 * @param input input stream
 * @param dimension one centroid dimensionality
 * @param vocabulary_size centroids count
 * @param Centroids* result centroids
 */
template<class T>
void ReadVocabulary(ifstream& input,
                    Dimensions dimension,
                    int vocabulary_size,
                    Centroids* centroids) {
  
  if(!input.good()) {
    throw std::logic_error("Bad input stream");
  }
  centroids->resize(vocabulary_size);
  for(ClusterId centroid_index = 0; centroid_index < centroids->size(); ++centroid_index) {
    centroids->at(centroid_index).resize(dimension);
    for(Dimensions dimension_index = 0; dimension_index < dimension; ++dimension_index) {
      T buffer;
      input.read((char*)&buffer, sizeof(buffer));
      centroids->at(centroid_index)[dimension_index] = Round<T, Coord>(buffer);
    }
  }
}

/**
 * Function reads vocabularies of centroids produced by matlab script.
 * Function assumes that the first int32 in input is dimensionality of centroids and
 * the second is the number of centroids in each vocabulary
 * @param input input stream
 * @param dimension one centroid dimensionality
 * @param vocabulary_size centroids count
 * @param Centroids* result centroids
 */
template<class T>
void ReadVocabularies(const string& filename,
                      Dimensions space_dimension,
                      vector<Centroids>* centroids) {
  ifstream vocabulary;
  vocabulary.open(filename.c_str(), ios::binary);
	
  if(!vocabulary.good()) {
    throw std::logic_error("Bad vocabulary file");
  }
  int dimension;//dimension in each subspace
  vocabulary.read((char*)&dimension, sizeof(dimension));
  if(dimension <= 0) {
    throw std::logic_error("Bad file content: non-positive dimension");
  }
  int vocabs_count = space_dimension / dimension;
  if(space_dimension < dimension) {
    throw std::logic_error("Space dimension is less than vocabulary dimension");
  }
  centroids->resize(vocabs_count);
  int vocabulary_size;
  vocabulary.read((char*)&vocabulary_size, sizeof(vocabulary_size));
  for(int vocab_item = 0; vocab_item < vocabs_count; ++vocab_item) {
    ReadVocabulary<T>(vocabulary, dimension, vocabulary_size, &(centroids->at(vocab_item)));
  }
}

/**
 * This function reads fine vocabs of centroids
 * @param fine_vocabs_filename file with vocabularies
 * @param fine_vocabs fine centroids lists
 */
template<class T>
void ReadFineVocabs(const string& fine_vocabs_filename, vector<Centroids>* fine_vocabs) {
  ifstream fine_vocabs_stream;
  fine_vocabs_stream.open(fine_vocabs_filename.c_str(), ios::binary);
  if(!fine_vocabs_stream.good()) {
    throw std::logic_error("Bad fine vocabulary file");
  }
  int vocabs_count, centroids_count, vocabs_dim;
  fine_vocabs_stream.read((char*)&vocabs_count, sizeof(vocabs_count));
  if(vocabs_count < 1) {
    throw std::logic_error("Bad fine vocabulary file content: number of vocabularies < 1");
  }
  fine_vocabs_stream.read((char*)&centroids_count, sizeof(centroids_count));
  if(centroids_count < 1) {
    throw std::logic_error("Bad fine vocabulary file content: vocabulary capacity < 1");
  }
  fine_vocabs_stream.read((char*)&vocabs_dim, sizeof(vocabs_dim));
  if(vocabs_dim < 1) {
    throw std::logic_error("Bad fine vocabulary file content: vocabulary dimension < 1");
  }
  fine_vocabs->resize(vocabs_count);
  for(int voc_index = 0; voc_index < vocabs_count; ++voc_index) {
    ReadVocabulary<T>(fine_vocabs_stream, vocabs_dim, centroids_count, &(fine_vocabs->at(voc_index)));
  }
}

/**
 * This function returns subpoints limited by start_dim and final_dim
 * for every point in points
 * @param points all points
 * @param start_dim first dimension of subpoint
 * @param final_dim dimension after the last dimension of subpoint
 * @param subpoints result subpoints
 */
void GetSubpoints(const Points& points,
                  const Dimensions start_dim,
                  const Dimensions final_dim,
                  Points* subpoints);

/**
 * This function returns identifier of clusters which centroid is the nearest to 
 * subpoint limited by start_dim and final_dim
 * @param point full point
 * @param Centroids all centroids (function finds the nearest one)
 * @param start_dim first dimension of subpoint
 * @param final_dim dimension after the last dimension of subpoint
 */
ClusterId GetNearestClusterId(const Point& point, const Centroids& centroids,
                              const Dimensions start_dim, const Dimensions final_dim);

/**
 * This function calculates quantization residual. 
 * @param point initial point
 * @param coarse_quantizations point coarse quantization
 * @param centroids lists of centroids
 * @param residual result residual
 */
void GetResidual(const Point& point, const CoarseQuantization& coarse_quantizations,
                 const vector<Centroids>& centroids, Point* residual);
/**
 * This function calculates quantization residual. 
 * @param point initial point
 * @param coarse_quantizations point coarse quantization
 * @param centroids lists of centroids
 * @param residual pointer to start of residual
 */
void GetResidual(const Point& point, const CoarseQuantization& coarse_quantizations,
                 const vector<Centroids>& centroids, Coord* residual);

/**
 * This function finds nearest cluster identifiers for points from start_pid to final_pid.
 * We need this function for multi-threading
 * @param points all points
 * @param centroids centroids of clusters
 * @param start_pid first point function finds nearest cluster
 * @param final_pid point after the last point function finds nearest cluster
 */
void GetNearestClusterIdsForPointSubset(const Points& points, const Centroids& centroids,
                                        const PointId start_pid, const PointId final_pid,
                                        vector<ClusterId>* nearest);

/**
 * This function finds cluster identifiers nearest to subpoints for a number of points.
 * Subpoints are limited by start_dim and finish_dim
 * @param points all points
 * @param centroids centroids of clusters
 * @param start_dim first dimesion of subpoint
 * @param final_dim dimesion after the last dimension of subpoint
 * @param threads_count number of threads
 * @param nearest result
 */
void GetNearestClusterIdsForSubpoints(const Points& points, const Centroids& centroids,
                                      const Dimensions start_dim, const Dimensions final_dim,
                                      int threads_count, vector<ClusterId>* nearest);

/**
 * This function calculates points coarse product quantizations
 * @param points all points
 * @param centroids centroids of clusters
 * @param threads_count number of threads
 * @param coarse_quantizations result quantizations
 */
void GetPointsCoarseQuaintizations(const Points& points, const vector<Centroids>& centroids,
                                   const int threads_count,
                                   vector<CoarseQuantization>* coarse_quantizations);


/**
 * \struct All indexation parameters
 */
struct IndexConfig {
  RerankMode rerank_mode;
  vector<Centroids> fine_vocabs;
};

/**
 * \struct Type of record in multiindex, contains
 * id of point and 8 bytes for ADC reranking
 */
struct RerankADC8 {
  PointId pid;
  FineClusterId quantizations[8];
  template<class Archive>
  void serialize(Archive& arc, unsigned int version) {
    arc & pid;
    arc & quantizations;
  }
};

/**
 * \struct Type of record in multiindex, contains
 * id of point and 16 bytes for ADC reranking
 */
struct RerankADC16 {
  PointId pid;
  FineClusterId quantizations[16];
  template<class Archive>
  void serialize(Archive& arc, unsigned int version) {
    arc & pid;
    arc & quantizations;
  }
};



















