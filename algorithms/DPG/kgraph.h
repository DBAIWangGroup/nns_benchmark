// Copyright (C) 2013-2015 Wei Dong <wdong@wdong.org>. All Rights Reserved.
//
// \mainpage KGraph: A Library for Efficient K-NN Search
// \author Wei Dong \f$ wdong@wdong.org \f$
// \author 2013-2015
//

#ifndef WDONG_KGRAPH
#define WDONG_KGRAPH

#include <iostream>
#include <unordered_set>
#include <vector>
#include <unordered_map>
//#include "kgraph-data.h"

using namespace std;

namespace kgraph {
    static unsigned const default_iterations =  30;
    static unsigned const default_L = 100;
    static unsigned const default_K = 100;
    static unsigned const default_P = 100;
    static unsigned const default_M = 0;
    static unsigned const default_T = 1;
    static unsigned const default_S = 10;
    static unsigned const default_Maxchecks = 10;
    static unsigned const default_seedIndex = 1;
    static unsigned const default_R = 100;
    static unsigned const default_controls = 100;
    static unsigned const default_seed = 1998;
    static float const default_delta = 0.002;
    static float const default_recall = 0.99;
    static float const default_epsilon = 1e30;
    static unsigned const default_verbosity = 1;
    enum {
        PRUNE_LEVEL_1 = 1,
        PRUNE_LEVEL_2 = 2
    };
    static unsigned const default_prune = 0;

    /// Verbosity control
    /** Set verbosity = 0 to disable information output to stderr.
     */
    extern unsigned verbosity;

    /// Index oracle
    /** The index oracle is the user-supplied plugin that computes
     * the distance between two arbitrary objects in the dataset.
     * It is used for offline k-NN graph construction.
     */
    class IndexOracle {
    public:
        /// Returns the size of the dataset.
        virtual unsigned size () const = 0;
        /// Computes similarity
        /**
         * 0 <= i, j < size() are the index of two objects in the dataset.
         * This method return the distance between objects i and j.
         */
        virtual float operator () (unsigned i, unsigned j) const = 0;
	virtual float angle (unsigned orign, unsigned pivot, unsigned x) const = 0;
    };

    /// Search oracle
    /** The search oracle is the user-supplied plugin that computes
     * the distance between the query and a arbitrary object in the dataset.
     * It is used for online k-NN search.
     */
    class SearchOracle {
    public:
        /// Returns the size of the dataset.
        virtual unsigned size () const = 0;
        /// Computes similarity
        /**
         * 0 <= i < size() are the index of an objects in the dataset.
         * This method return the distance between the query and object i.
         */
        virtual float operator () (unsigned i) const = 0;
        /// Search with brutal force.
        /**
         * Search results are guaranteed to be ranked in ascending order of distance.
         *
         * @param K Return at most K nearest neighbors.
         * @param epsilon Only returns nearest neighbors within distance epsilon.
         * @param ids Pointer to the memory where neighbor IDs are returned.
         * @param dists Pointer to the memory where distance values are returned, can be nullptr.
         */
        unsigned search (unsigned K, float epsilon, unsigned *ids, float *dists = nullptr) const;
    };

    /// The KGraph index.
    /** This is an abstract base class.  Use KGraph::create to create an instance.
     */
    class KGraph {
    public:
        /// Indexing parameters.
        struct IndexParams {
            unsigned iterations; 
            unsigned L;
            unsigned K;
            unsigned S;
            unsigned R;
            unsigned controls;
            unsigned seed;
            float delta;
            float recall;
            unsigned prune;

            /// Construct with default values.
            IndexParams (): iterations(default_iterations), L(default_L), K(default_K), S(default_S), R(default_R), controls(default_controls), seed(default_seed), delta(default_delta), recall(default_recall), prune(default_prune) {
            }
        };

        /// Search parameters.
        struct SearchParams {
            unsigned K;
            unsigned M;
            unsigned P;
            unsigned T;
            unsigned Maxchecks;
            float epsilon;
            unsigned seed;
            unsigned init;
      

            /// Construct with default values.
            SearchParams (): K(default_K), M(default_M), P(default_P), T(default_T), epsilon(default_epsilon), seed(1998), init(0) {
            }
        };

        /// Information and statistics of the indexing algorithm.
        struct IndexInfo {
            enum StopCondition {
                ITERATION = 0,
                DELTA,
                RECALL
            } stop_condition;
            unsigned iterations;
            float cost;
            float recall;
            float accuracy;
            float delta;
            float M;
        };

        /// Information and statistics of the search algorithm.
        struct SearchInfo {
            float cost;
            unsigned updates;
            int checks;
        };

        virtual ~KGraph () {
        }
        /// Load index from file.
        /**
         * @param path Path to the index file.
         */
        virtual void load (char const *path) = 0;
        /// Save index to file.
        /**sa
         * @param path Path to the index file.
         */
        virtual void save (char const *path) const = 0; // save to file
	    virtual void saver (char const *path) const = 0;
		virtual void get_hubs (char const *hubs_path,char const *knn_path) const = 0; // get_hubs
		virtual void add_long_edges(IndexOracle const &oracle, int const rank_top_k, int const change_num) = 0; //add long edges
		virtual void output_M (char const * filename) = 0;
		virtual void output_info(int const id) = 0; // output info for id-th point
		virtual void merge(char const * graph_path, char const * id_path) = 0;
		virtual void test_connectivity(unsigned dest) = 0;
		virtual void remove_near_edges(IndexOracle const &oracle, const int edge_num) = 0;
        virtual float statistics(IndexOracle const &oracle, const int edge_num) = 0;
        virtual void diversify_by_cut(IndexOracle const &oracle, const int edge_num) = 0;
		virtual void remove_near_edges_2(IndexOracle const &oracle, vector<unsigned> const &edge_num, unordered_set<unsigned> const &skip_set) = 0;
		virtual void add_random_backward_edges(char const *data_path, char const *hubs_path, int const num_of_hubs, char const * start_points_path, unsigned num_of_query) = 0;
		virtual void add_backward_edges() = 0;
		virtual void get_partitions(char const *output_path) = 0;
		virtual void compute_diversity(IndexOracle const &oracle, int d, float * norm) = 0;
		virtual void connectivity(unsigned const source, unsigned const dest) = 0;
		virtual void make_inverse_graph() = 0;
		virtual void find_lm (SearchOracle const &oracle) const = 0;
		virtual void kgraph2dot (char * const &filename) = 0;
        /// Build the index
                virtual void build (IndexOracle const &oracle, IndexParams const &params, char const *path, IndexInfo *info = 0) = 0;
        /// Prune the index
        /**
         * Pruning makes the index smaller to save memory, and makes online search on the pruned index faster.
         * (The cost parameters of online search must be enlarged so accuracy is not reduced.)
         *
         * Currently only two pruning levels are supported:
         * - PRUNE_LEVEL_1 = 1: Only reduces index size, fast.
         * - PRUNE_LEVEL_2 = 2: For improve online search speed, slow.
         *
         * No pruning is done if level = 0.
         */
        virtual void prune (IndexOracle const &oracle, unsigned level) = 0;
        /// Online k-NN search.
        /**
         * Search results are guaranteed to be ranked in ascending order of distance.
         *
         * @param ids Pointer to the memory where neighbor IDs are stored, must have space to save params.K ids.
         */
        unsigned search (SearchOracle const &oracle, SearchParams const &params, unsigned *ids, SearchInfo *info = 0, string const &info_path = "") const {
            //std::cout <<"h search \n";
	  return search(oracle, params, ids, nullptr, info, info_path);// d_init, Matrix<float> const &d_init,
        }

	unsigned search_greedy (SearchOracle const &oracle, SearchParams const &params, unsigned *ids, SearchInfo *info = 0, string const &info_path = "") const {
	  //std::cout <<"h search \n";
          return search_greedy(oracle, params, ids, nullptr, info, info_path);// d_init, Matrix<float> const &d_init,
        }
        /// Online k-NN search.
        /**
         * Search results are guaranteed to be ranked in ascending order of distance.
         *
         * @param ids Pointer to the memory where neighbor IDs are stored, must have space to save params.K values.
         * @param dists Pointer to the memory where distances are stored, must have space to save params.K values.
         */
	virtual unsigned advanced_search (SearchOracle const &oracle, SearchParams const &params, unsigned *ids, float *dists, SearchInfo *info, string const &info_path) const = 0; //,Matrix<float> const &d_init

        virtual unsigned search (SearchOracle const &oracle, SearchParams const &params, unsigned *ids, float *dists, SearchInfo *info, string const &info_path) const = 0; //,Matrix<float> const &d_init

	virtual unsigned search_greedy (SearchOracle const &oracle, SearchParams const &params, unsigned *ids, float *dists, SearchInfo *info, string const &info_path) const = 0; //,Matrix<float> const &d_init
        /// Constructor.
        static KGraph *create ();
        /// Returns version string.
        static char const* version ();

        /// Get offline computed k-NNs of a given object.
        /**
         * See the full version of get_nn.
         */
        virtual void get_nn (unsigned id, unsigned *nns, unsigned *M, unsigned *L) const {
            get_nn(id, nns, nullptr, M, L);
        }
        /// Get offline computed k-NNs of a given object.
        /**
         * The user must provide space to save IndexParams::L values.
         * The actually returned L could be smaller than IndexParams::L, and
         * M <= L is the number of neighbors KGraph thinks
         * could be most useful for online search, and is usually < L.
         * If the index has been pruned, the returned L could be smaller than
         * IndexParams::L used to construct the index.
         *
         * @params id Object ID whose neighbor information are returned.
         * @params nns Neighbor IDs, must have space to save IndexParams::L values. 
         * @params dists Distance values, must have space to save IndexParams::L values.
         * @params M Useful number of neighbors, output only.
         * @params L Actually returned number of neighbors, output only.
         */
        virtual void get_nn (unsigned id, unsigned *nns, float *dists, unsigned *M, unsigned *L) const = 0;
    };
}

#if __cplusplus > 199711L
#include <functional>
namespace kgraph {
    /// Oracle adapter for datasets stored in a vector-like container.
    /**
     * If the dataset is stored in a container of CONTAINER_TYPE that supports
     * - a size() method that returns the number of objects.
     * - a [] operator that returns the const reference to an object.
     * This class can be used to provide a wrapper to facilitate creating
     * the index and search oracles.
     *
     * The user must provide a callback function that takes in two
     * const references to objects and returns a distance value.
     */
    template <typename CONTAINER_TYPE, typename OBJECT_TYPE>
    class VectorOracle: public IndexOracle {
    public:
        typedef std::function<float(OBJECT_TYPE const &, OBJECT_TYPE const &)> METRIC_TYPE;
    private:
        CONTAINER_TYPE const &data;
        METRIC_TYPE dist;
    public:
        class VectorSearchOracle: public SearchOracle {
            CONTAINER_TYPE const &data;
            OBJECT_TYPE const query;
            METRIC_TYPE dist;
        public:
            VectorSearchOracle (CONTAINER_TYPE const &p, OBJECT_TYPE const &q, METRIC_TYPE m): data(p), query(q), dist(m) {
            }
            virtual unsigned size () const {
                return data.size();
            }
            virtual float operator () (unsigned i) const {
                return dist(data[i], query);
            }
        };
        /// Constructor.
        /**
         * @param d: the container that holds the dataset.
         * @param m: a callback function for distance computation.  m(d[i], d[j]) must be
         *  a valid expression to compute distance.
         */
        VectorOracle (CONTAINER_TYPE const &d, METRIC_TYPE m): data(d), dist(m) {
        }
        virtual unsigned size () const {
            return data.size();
        }
        virtual float operator () (unsigned i, unsigned j) const {
            return dist(data[i], data[j]);
        }
        /// Constructs a search oracle for query object q.
        VectorSearchOracle query (OBJECT_TYPE const &q) const {
            return VectorSearchOracle(data, q, dist);
        }
    };
}
#endif

#endif

