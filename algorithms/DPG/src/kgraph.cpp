//static char const *kgraph_version = KGRAPH_VERSION "-" KGRAPH_BUILD_NUMBER "," KGRAPH_BUILD_ID;
static char const *kgraph_version = "deiversified proximity graph";

#include <omp.h>
#include <unordered_set>
#include <mutex>
#include <iostream>
#include <fstream>
#include <random>
#include <algorithm>
#include <queue>
#include <boost/timer/timer.hpp>
#define timer timer_for_boost_progress_t
#include <boost/progress.hpp>
#undef timer
#include <boost/dynamic_bitset.hpp>
#include <boost/accumulators/accumulators.hpp>
#include <boost/accumulators/statistics/stats.hpp>
#include <boost/accumulators/statistics/mean.hpp>
#include <boost/accumulators/statistics/moment.hpp>
#include "boost/smart_ptr/detail/spinlock.hpp"
#include "kgraph.h"
#include "kgraph-data.h"

namespace kgraph {

    using namespace std;
    using namespace boost;
    using namespace boost::accumulators;

    unsigned verbosity = default_verbosity;

    typedef boost::detail::spinlock Lock;
    typedef std::lock_guard<Lock> LockGuard;

    // generate size distinct random numbers < N
    template <typename RNG>
    static void GenRandom (RNG &rng, unsigned *addr, unsigned size, unsigned N) {
        for (unsigned i = 0; i < size; ++i) {
            addr[i] = rng() % (N - size);
        }
        sort(addr, addr + size);
        for (unsigned i = 1; i < size; ++i) {
            if (addr[i] <= addr[i-1]) {
                addr[i] = addr[i-1] + 1;
            }
        }
        unsigned off = rng() % N;
        for (unsigned i = 0; i < size; ++i) {
            addr[i] = (addr[i] + off) % N;
        }
    }

    struct Neighbor {
        unsigned id;
        float dist;
        bool flag;  // whether this entry is a newly found one      
        Neighbor () {}
        Neighbor (unsigned i, float d, bool f = true): id(i), dist(d), flag(f) {
        }
    };

    static inline bool operator < (Neighbor const &n1, Neighbor const &n2) {
      if (n1.dist == n2.dist)
	return n1.id < n2.id;
      else 
        return n1.dist < n2.dist;
    }

    typedef vector<Neighbor> Neighbors;

    // both pool and knn should be sorted in ascending order
    static float EvaluateRecall (Neighbors const &pool, Neighbors const &knn) {
        if (knn.empty()) return 1.0;
        unsigned found = 0;
        unsigned n_p = 0;
        unsigned n_k = 0;
        for (;;) {
            if (n_p >= pool.size()) break;
            if (n_k >= knn.size()) break;
            if (knn[n_k].dist < pool[n_p].dist) {
                ++n_k;
            }
            else if (knn[n_k].dist == pool[n_p].dist) {
                ++found;
                ++n_k;
                ++n_p;
            }
            else {
                cerr << "Distance is unstable." << endl;
                cerr << "Exact";
                for (auto const &p: knn) {
                    cerr << ' ' << p.id << ':' << p.dist;
                }
                cerr << endl;
                cerr << "Approx";
                for (auto const &p: pool) {
                    cerr << ' ' << p.id << ':' << p.dist;
                }
                cerr << endl;
                throw runtime_error("distance is unstable");
            }
        }
        return float(found) / knn.size(); //recall
    }

    static float EvaluateAccuracy (Neighbors const &pool, Neighbors const &knn) {
        unsigned m = std::min(pool.size(), knn.size());
        float sum = 0;
        unsigned cnt = 0;
        for (unsigned i = 0; i < m; ++i) {
            if (knn[i].dist >  0) {
                sum += abs(pool[i].dist - knn[i].dist) / knn[i].dist;
                ++cnt;
            }
        }
        return cnt > 0 ? sum / cnt: 0;
    }

    static float EvaluateOneRecall (Neighbors const &pool, Neighbors const &knn) {
        if (pool[0].dist == knn[0].dist) return 1.0;
        return 0;
    }
  
    static float EvaluateDelta (Neighbors const &pool, unsigned K) {
        unsigned c = 0;
        unsigned N = K;
        if (pool.size() < N) N = pool.size();
        for (unsigned i = 0; i < N; ++i) {
            if (pool[i].flag) ++c;
        }
        return float(c) / K;  //the fraction of true in pool's flags
    }

    struct Control {
        unsigned id;
        Neighbors neighbors;
    };

    // try insert nn into the list
    // the array addr must contain at least K+1 entries:
    //      addr[0..K-1] is a sorted list
    //      addr[K] is as output parameter
    // * if nn is already in addr[0..K-1], return K+1
    // * Otherwise, do the equivalent of the following
    //      put nn into addr[K]
    //      make addr[0..K] sorted
    //      return the offset of nn's index in addr (could be K)
    //
    // Special case:  K == 0
    //      addr[0] <- nn
    //      return 0
    static inline unsigned UpdateKnnList (Neighbor *addr, unsigned K, Neighbor nn) {
        // find the location to insert
        unsigned j;
        unsigned i = K;
        while (i > 0) {
            j = i - 1;
            if (addr[j].dist <= nn.dist) break;
            i = j;
        }
        // check for equal ID
        unsigned l = i;
        while (l > 0) {
            j = l - 1;
            if (addr[j].dist < nn.dist) break;
            if (addr[j].id == nn.id) return K + 1;
            l = j;
        }
        // i <= K-1
        j = K;
        while (j > i) {
            addr[j] = addr[j-1];
            --j;
        }
        addr[i] = nn;
        return i;
    }

    void LinearSearch (IndexOracle const &oracle, unsigned i, unsigned K, vector<Neighbor> *pnns) {
        vector<Neighbor> nns(K+1);
        unsigned N = oracle.size();
        Neighbor nn;
        nn.id = 0;
        nn.flag = true; // we don't really use this
        unsigned k = 0;
        while (nn.id < N) {
            if (nn.id != i) {
                nn.dist = oracle(i, nn.id);
                UpdateKnnList(&nns[0], k, nn);
                if (k < K) ++k;
            }
            ++nn.id;
        }
        nns.resize(K);
        pnns->swap(nns);
    }

    unsigned SearchOracle::search (unsigned K, float epsilon, unsigned *ids, float *dists) const {
        vector<Neighbor> nns(K+1);
        //cout<<"oracle searching"<<endl;
        unsigned N = size();
        unsigned L = 0;
        for (unsigned k = 0; k < N; ++k) {
            float k_dist = operator () (k);
            if (k_dist > epsilon) continue;
            UpdateKnnList(&nns[0], L, Neighbor(k, k_dist));
            if (L < K) ++L;
        }
        if (ids) {
            for (unsigned k = 0; k < L; ++k) {
                ids[k] = nns[k].id;
            }
        }
        if (dists) {
            for (unsigned k = 0; k < L; ++k) {
                dists[k] = nns[k].dist;
            }
        }
        return L;
    }

    void GenerateControl (IndexOracle const &oracle, unsigned C, unsigned K, vector<Control> *pcontrols) {
        vector<Control> controls(C);
        {
            vector<unsigned> index(oracle.size());
            int i = 0;
            for (unsigned &v: index) {
                v = i++;
            }
            random_shuffle(index.begin(), index.end());
#ifdef PARALLEL            
#pragma omp parallel for
#endif             
            for (unsigned i = 0; i < C; ++i) {
                controls[i].id = index[i];
                LinearSearch(oracle, index[i], K, &controls[i].neighbors);
            }
        }
        pcontrols->swap(controls);
    }

    static char const *KGRAPH_MAGIC = "KNNGRAPH";
    static unsigned constexpr KGRAPH_MAGIC_SIZE = 8;
    static uint32_t constexpr VERSION_MAJOR = 2;
    static uint32_t constexpr VERSION_MINOR = 0;

    class KGraphImpl: public KGraph {
    protected:
        vector<unsigned> M;
        vector<vector<Neighbor>> graph;
    public:
        virtual ~KGraphImpl () {
        }
        virtual void load (char const *path) {
            BOOST_VERIFY(sizeof(unsigned) == sizeof(uint32_t));
            ifstream is(path, ios::binary);
            char magic[KGRAPH_MAGIC_SIZE];
            uint32_t major;
            uint32_t minor;
            uint32_t N;
            is.read(magic, sizeof(magic));
            is.read(reinterpret_cast<char *>(&major), sizeof(major));
            is.read(reinterpret_cast<char *>(&minor), sizeof(minor));
            if (major != VERSION_MAJOR) throw runtime_error("data version not supported.");
            is.read(reinterpret_cast<char *>(&N), sizeof(N));
            if (!is) runtime_error("error reading index file.");
            for (unsigned i = 0; i < KGRAPH_MAGIC_SIZE; ++i) {
                if (KGRAPH_MAGIC[i] != magic[i]) runtime_error("index corrupted.");
            }
            graph.resize(N);
            M.resize(N);
            for (unsigned i = 0; i < graph.size(); ++i) {
                auto &knn = graph[i];
                unsigned K;
                is.read(reinterpret_cast<char *>(&M[i]), sizeof(M[i]));
                is.read(reinterpret_cast<char *>(&K), sizeof(K));
                if (!is) runtime_error("error reading index file.");
                knn.resize(K);
                //modified by Yifang
                for (unsigned j = 0; j < K; ++j){
                  is.read(reinterpret_cast<char *>(&knn[j].id), sizeof(knn[j].id));
                  knn[j].flag = true;
                  knn[j].dist = 0.0;
                }
                //is.read(reinterpret_cast<char *>(&knn[0]), K * sizeof(knn[0]));
            }
        }

      virtual void merge(char const * graph_path, char const * id_path){
	ifstream is(graph_path, ios::binary);
	char magic[KGRAPH_MAGIC_SIZE];
	uint32_t major;
	uint32_t minor;
	uint32_t N;
	is.read(magic, sizeof(magic));
	is.read(reinterpret_cast<char *>(&major), sizeof(major));
	is.read(reinterpret_cast<char *>(&minor), sizeof(minor));
	if (major != VERSION_MAJOR) throw runtime_error("data version not supported.");
	is.read(reinterpret_cast<char *>(&N), sizeof(N));
	if (!is) runtime_error("error reading index file.");
	for (unsigned i = 0; i < KGRAPH_MAGIC_SIZE; ++i) {
	  if (KGRAPH_MAGIC[i] != magic[i]) runtime_error("index corrupted.");
	}
	vector<vector<Neighbor> > small_graph;
	vector<unsigned> small_M;
	small_graph.resize(N);
	small_M.resize(N);
	for (unsigned i = 0; i < small_graph.size(); ++i) {
	  auto &knn = small_graph[i];
	  unsigned K;
	  is.read(reinterpret_cast<char *>(&small_M[i]), sizeof(small_M[i]));
	  is.read(reinterpret_cast<char *>(&K), sizeof(K));
	  if (!is) runtime_error("error reading index file.");
	  knn.resize(K);
	  is.read(reinterpret_cast<char *>(&knn[0]), K * sizeof(knn[0]));
	}
	//loaded the small graph
	FILE *fp = fopen(id_path, "rb");
	unsigned * id_arr = new unsigned[N];
	fread(id_arr, sizeof(unsigned), N, fp);
	fclose(fp);

	for (unsigned i = 0; i < N; ++i){
	  unsigned real_id = id_arr[i];
	  auto &small_knn = small_graph[i];
	  auto &knn = graph[real_id];
	  if (M[real_id] != knn.size()){
	    knn.resize(M[real_id]);
	  }
	  for (unsigned k = 0; k < small_M[i]; ++k){
	    knn.push_back(Neighbor(id_arr[small_knn[k].id], small_knn[k].dist, false));
	  }
	  sort(knn.begin(), knn.end());
	  for (unsigned k = 1; k < knn.size(); ++k){
	    if (knn[k].id == knn[k-1].id){
	      knn.erase(knn.begin() + k);
	      k--;
	    }
	  }
	  M[real_id] = knn.size();
	}	
      }

        virtual void output_info(int const id){
	  if (id > graph.size()){
	    fprintf(stderr, "out_of_index_bound!\n");
	    return;
	  }
	  auto const &knn = graph[id];
	  knn.size();
	  
	  fprintf(stderr, "%d\n%d\n=================\n", M[id], knn.size());
	  for (int i = 0; i < knn.size(); ++i){
	    fprintf(stderr, "%d %f\n", knn[i].id, knn[i].dist);
	  }
	}


      struct hub_pair{
	unsigned id;
	int in_degree;
	hub_pair(unsigned x, int y){
	  id = x;
	  in_degree = y;
	}
	bool operator<(const hub_pair& rhs) const{
	  return in_degree > rhs.in_degree;
	}
      };
      
      struct rknn_pair{
	unsigned id;
	float dist;
	rknn_pair(unsigned x, float y){
	  id = x;
	  dist = y;
	}
      };

      virtual void connectivity(unsigned const source, unsigned const dest){
	uint32_t N = graph.size();
	int level[N];
	for (int i = 0; i < N; ++i){
	  level[i] = 0;
	}
	
	queue<unsigned> q;
	q.push(source);
	level[source] = 1;
	bool find = false;
	while (!q.empty() && !find){
	  unsigned cur_id = q.front();
	  auto const &knn = graph[cur_id];
	  q.pop();
	  for (int i = 0; i < M[cur_id]; ++i){
	    if (level[knn[i].id] != 0){
	      continue;
	    }
	    q.push(knn[i].id);
	    level[knn[i].id] = level[cur_id] + 1;
	    if (knn[i].id == dest){
	      find = true;
	      fprintf(stderr, "Found! Level = %d\n", level[knn[i].id]);
	    }
	  }
	}
	if (!find){
	  fprintf(stderr, "Not Found!\n");
	}
      }

      virtual void get_partitions(char const *output_path){
	vector<vector<int> > partitions;
	uint32_t N = graph.size();
	for (int i = 0; i < N; ++i){
	  if (i % 1000 == 0) {
            fprintf(stderr, "\r%d (%.3f\%)", i, 1.0 * i / N * 100);
          }
	  auto const &knn = graph[i];
	  int major_set_pos = -1;
	  for (int j = 0; j < partitions.size(); ++j){
	    if (find (partitions[j].begin(), partitions[j].end(), i) != partitions[j].end()){
	      major_set_pos = j;
	      break;
	    }
	  }
	  if (major_set_pos == -1){
	    vector<int> new_set;
	    new_set.push_back(i);
	    partitions.push_back(new_set);
	    major_set_pos = partitions.size() - 1;
	    //fprintf(stderr, "%d is in %d\n", i, major_set_pos);
	  }

	  //fprintf(stderr, "i is in %d\n", major_set_pos);
	  
	  unsigned maxM = M[i];
	  for (int j = 0; j < maxM; ++j){
	    bool done = false;
	    //test if knn[j] is in some partition
	    for (int k = 0; k < partitions.size(); ++k){
	      if (find (partitions[k].begin(), partitions[k].end(), knn[j].id) != partitions[k].end()){
		if (k == major_set_pos){
		  done = true;
		  break;
		} else {
		  //merge
		  //fprintf(stderr, "markmerge\n");
		  for ( auto it = partitions[k].begin(); it != partitions[k].end(); ++it ){
		    //fprintf(stderr, "markmerge %d %d\n", *it, partitions[major_set_pos].max_size());
		    partitions[major_set_pos].push_back(*it);
		    //fprintf(stderr, "markmergeee\n");
		  }
		  //fprintf(stderr, "markmerge done\n");
		  //partitions[major_set_pos].insert(partitions[k].begin(), partitions[k].end());
		  //vector<int> temp = partitions.end();
		  
		  //		  partitions[k].swap(partitions.back());
		  partitions[k].clear();
		  for ( auto it = partitions.back().begin(); it != partitions.back().end(); ++it ){
                    //fprintf(stderr, "markmerge %d %d\n", *it, partitions[major_set_pos].max_size());
                    partitions[k].push_back(*it);
                    //fprintf(stderr, "markmergeee\n");
                  }
		  partitions.back().clear();
		  //partitions.erase(partitions.begin() + k);
		  done = true;
		  //fprintf(stderr, "mark merge done\n");
		  break;
		}
	      }
	    }
	    if (!done){
	      //add
	      partitions[major_set_pos].push_back(knn[j].id);
	      continue;
	    }
	  }
	  //fprintf(stderr, "mark2\n");
	}
	fprintf(stderr, "%d\n", partitions.size());
	//	return partitions;
	int count = 0;
	for (int i = 0; i < partitions.size(); ++i){
	  if (partitions[i].size() != 0){
	    fprintf(stderr, "%d\n", partitions[i].size());
	    count ++;
	  }
	}
	fprintf(stderr, "%d\n", count);

      }

      virtual void output_M (char const* filename) {
        FILE *fp = fopen(filename, "wb");
        unsigned N = graph.size();
        fwrite(&N, sizeof(unsigned), 1, fp);
	for (int i = 0; i < N; ++i){
          unsigned len = M[i];
          fwrite(&len, sizeof(unsigned), 1, fp);
        }
	fclose(fp);
      }


      virtual void make_inverse_graph(){
	uint32_t N = graph.size();
	vector<vector<Neighbor> > inverse_graph;
	for (unsigned i = 0; i < N; ++i){
	  inverse_graph.push_back(vector<Neighbor>());
	}
	for (unsigned i = 0; i < N; ++i) {
          auto const &knn = graph[i];
	  for (unsigned j = 0; j < M[i]; j++)  {
	    inverse_graph[knn[j].id].push_back(Neighbor(i, knn[j].dist, false));
	  }
	}
	for (unsigned i = 0; i < N; ++i) {
	  M[i] = inverse_graph[i].size();
	}
	graph = inverse_graph;
      }
      
      

    virtual void test_connectivity(unsigned dest){ // for inverse kgraph
	uint32_t N = graph.size();
	bool * traversed = new bool[N];
	queue<unsigned> q;
	q.push(dest);
	memset(traversed, 0, sizeof(bool)*N);
	traversed[dest] = true;
	int count = 1;
	
	while(!q.empty()){
	  unsigned pivot = q.front();
	  q.pop();
	  auto const &knn = graph[pivot];
	  for (unsigned i = 0; i < knn.size(); ++i){
	    if (!traversed[knn[i].id]){
	      q.push(knn[i].id);
	      traversed[knn[i].id] = true;
	      count++;
	    }
	  }
	}

	fprintf(stderr, "%d points connected with pivot\n", count);
      }

      virtual void add_random_backward_edges(char const *data_path, char const *hubs_path, int const num_of_hubs, char const * start_points_path, unsigned num_of_query){
        uint32_t N = graph.size();
	//ofstream os(hubs_path, ios::binary);
        vector <hub_pair > hubs;
	vector<vector<rknn_pair> > rknn_graph;
        Matrix<float> data;
        data.load_lshkit(data_path);

	int count = 0;

        for (unsigned i = 0; i < N; ++i) {
	  hubs.push_back(hub_pair(i, 0));
          rknn_graph.push_back(vector<rknn_pair>());
	}

	for (unsigned i = 0; i < N; ++i) {
          auto const &knn = graph[i];
          uint32_t K = knn.size();
	  for (unsigned j=0; j<K; j++)  {
            hubs[knn[j].id].in_degree++;
            rknn_graph[knn[j].id].push_back(rknn_pair(i, knn[j].dist));
          }
        }

	//	for (unsigned i = 0; i < N; ++i) {
        //  os<<hubs[i].in_degree <<endl;
        //}

        //sort(hubs.begin(), hubs.end());
	
	for (unsigned i = 0; i < num_of_hubs; ++i){
	  unsigned p = rand() % N;
	  hub_pair temp = hubs[i];
	  hubs[i] = hubs[p];
	  hubs[p] = temp;
	}

	Matrix<unsigned> ids;
	ids.resize(num_of_query, num_of_hubs);
        Matrix<float> hub_data;
        hub_data.resize(num_of_hubs, data.dim());

	//unsigned * ids = new unsigned[num_of_hubs];
        for (unsigned i = 0; i < num_of_hubs; ++i){
          //fprintf(stderr, "%d: %d %d\n", i, hubs[i].id, hubs[i].in_degree);
	  // add backward edges
          vector<rknn_pair> rknn_list = rknn_graph[hubs[i].id];
	  count += rknn_list.size();
          for (unsigned j = 0; j < rknn_list.size(); ++j){
            graph[hubs[i].id].push_back(Neighbor(rknn_list[j].id, rknn_list[j].dist, true)); //rknn_list[j]);
          }

          for (unsigned j = 0; j < hub_data.dim(); ++j){
            hub_data[i][j] = data[hubs[i].id][j];
          }

          // write start point file
	  for (int j = 0; j < num_of_query; ++j)
            ids[j][i] = hubs[i].id;
          //fprintf(stderr, "%d: %d %d\n", i, ids[j][i], hubs[i].in_degree);
          sort(graph[hubs[i].id].begin(), graph[hubs[i].id].end());
          M[hubs[i].id] = rknn_list.size();
	}
        ids.save_lshkit(start_points_path);
	hub_data.save_lshkit(hubs_path);
        //      for (unsigned i = 0; i < num_of_query; ++i){
        //  os2.write((const char *)ids, sizeof(unsigned) * header[2]);
        //}
        //delete[] ids;
	fprintf(stderr, "inverse edges: %d\n", count);
      }

      virtual void add_backward_edges(){
	uint32_t N = graph.size();
	//ofstream os(hubs_path, ios::binary);
	//vector <hub_pair > hubs;
	vector<vector<rknn_pair> > rknn_graph;
        rknn_graph.resize(N);
        
	int count = 0;
        
	
	for (unsigned i = 0; i < N; ++i) {
	  auto const &knn = graph[i];
	  uint32_t K = M[i]; //knn.size();
	  for (unsigned j=0; j<K; j++)  {
	    rknn_graph[knn[j].id].push_back(rknn_pair(i, knn[j].dist));
	  }
	}

	for (unsigned i = 0; i < N; ++i){
	  vector<rknn_pair> rknn_list = rknn_graph[i];
	  count += rknn_list.size();
	  
          for (unsigned j = 0; j < rknn_list.size(); ++j){
	    graph[i].push_back(Neighbor(rknn_list[j].id, rknn_list[j].dist, true)); //rknn_list[j]);
	    //sum += exp(-1 * sqrt(rknn_list[j].dist) * beta); // a function with dist
	  }

	  sort(graph[i].begin(), graph[i].end());
	  for (unsigned j = 1; j < graph[i].size(); ++j){
	    if (graph[i][j].id == graph[i][j - 1].id){
	      graph[i].erase(graph[i].begin() + j);
	      j--;
	    }
	  }
	  
          M[i] = graph[i].size();
	}
	fprintf(stderr, "inverse edges: %d\n", count);
      }


 virtual void compute_diversity(IndexOracle const &oracle, int d, float * norm){ // diversity
	if (oracle.size() != graph.size()){
          throw runtime_error("Build kgraph before adding long edges");
        }

        uint32_t N = oracle.size();
	float * avg_arr = new float[N];
	uint32_t true_N = N;
#pragma omp parallel for
        for (unsigned k = 0; k < N; k++){
          int len = M[k];
	  int true_len = len * (len - 1) / 2;
	  float sum_angle = 0.0;
          for (int i = 0; i < len; ++i){ // the first max_edge_num elements are the remaining ones
	    if (graph[k][i].dist < 0.00001){
	      true_len -= len - i - 1;
	      continue;
	    }
            for (int cand_pos = i + 1; cand_pos < len; cand_pos++){
	      float angle = oracle.angle(k, graph[k][cand_pos].id, graph[k][i].id);
	      if (!(angle > 0.00001)){
		true_len --;
		continue;
	      }
              sum_angle += angle; //oracle.angle(k, graph[k][cand_pos].id, graph[k][i].id);
            }
          }
	  if (true_len <= 0){
	    avg_arr[k] = 0.0;
	    true_N--;
	  } else {
	    avg_arr[k] = sum_angle / true_len;
	  }
	  //	  if (!(avg_arr[k] > 0.00001))
	  //  fprintf(stderr, "%d %f %f\n", k, sum_angle, avg_arr[k]);

        }

	double avg_angle = 0.0;
	double normed_angle = 0.0;
	for (unsigned k = 0; k < N; ++k){
	  avg_angle = avg_angle + avg_arr[k];
	  normed_angle += avg_arr[k] / norm[M[k] - 1];
	}

	//avg_angle /= N;
	//normed_angle /= N;

	cout << "Average pairwise angle: " << avg_angle / true_N << endl;
	cout << "Normed pairwise angle: " << normed_angle / N << endl;

	delete[] avg_arr;
      }


virtual void diversify_by_cut(IndexOracle const &oracle, const int edge_num){

    if (oracle.size() != graph.size()){
      throw runtime_error("diversify_by_cut : inconsistent." );
    }
    
    uint32_t N = oracle.size();

    int cnt = 0;
    int nnt = 0;

    unsigned step = N / 100;
    cerr << endl << "Progress : ";

    // #pragma omp parallel for
    for (unsigned k = 0; k < N; k++){

    if ( k % step == 0 ) cerr <<"*";


      // float *map = new float[N];
      // memset(map, 0, sizeof(float)*N);

      int len = graph[k].size();
      while(graph[k][len-1].id == 0){
        len--;
      }
      if (len > 2 * edge_num){
        len = 2 * edge_num;
      } 
      
      // materialize the ditance here 
      int* hit = new int[len];

      for ( int i=0; i< len; i++){
        graph[k][i].dist = oracle(k, graph[k][i].id );
        hit[i] = 0;
      }      

      for ( int i=0; i < len-1; i++ ){
         // for ( int j=0; j < len; j++){
         for ( int j=i+1; j < len; j++){
            
            if ( i==j ) continue;

            Neighbor n_b = graph[k][i]; 
            Neighbor n_k = graph[k][j];

            float dist = oracle(n_b.id, n_k.id);
            // cout << dist << " : (" << n_b.id << " , "  << n_k.id << endl;  
            if ( dist < n_k.dist ){
                hit[j]++;
            }
        }
      }


      // sort by the hits and find the cuts 
      int* b_hit = new int[len];
      for ( int i=0; i< len; i++ )
        b_hit[i] = hit[i];

      // memcpy( b_hit, hit, sizeof(int)*len);

      sort(b_hit, b_hit + len );
      float cut = b_hit[edge_num];

      // update the neighbors by #hits 
      Neighbor tmp[len];
      for ( int i=0; i < len; i++)
        tmp[i] = graph[k][i];


      cnt = 0;
      for ( int i=0; i < len; i++){        
        if ( hit[i] <= cut )
            graph[k][cnt++] = tmp[i];
      }

      graph[k].resize(edge_num); // reset the size of NN list 

      delete []hit;
      delete []b_hit;

      /*      
      if ( k % 100 == 0 ){
        for ( int i=0; i< len; i++){
            if ( hit[i]> cut )
                cout << hit[i] << "* ";
            else
                cout << hit[i] << " ";
        }
        cout << endl;
      }
      */      

      /*
      for (int i = 1; i < edge_num; ++i){ // the first max_edge_num elements are the remaining ones
        float max_sum_angle = -1.0;
        int max_pos = -1;
        for (int cand_pos = i; cand_pos < len; cand_pos++){
          map[graph[k][cand_pos].id] += oracle.angle(k, graph[k][cand_pos].id, graph[k][i - 1].id);
          float sum_angle = map[graph[k][cand_pos].id];
          if (sum_angle > max_sum_angle){
        max_sum_angle = sum_angle;
        max_pos = cand_pos;
          }
        }
        Neighbor temp = graph[k][i];
        graph[k][i] = graph[k][max_pos];
        graph[k][max_pos] = temp;
      }
      */


      // graph[k].resize(edge_num); // reset the size of NN list 

      // sort(graph[k].begin(), graph[k].end());
      
      M[k] = edge_num;
      
      // delete[] map;
    }

    cerr << endl;

    } // end of diversify_by_cut
      

    virtual  float statistics(IndexOracle const &oracle, const int edge_num){ // diversity
    if (oracle.size() != graph.size()){
      throw runtime_error("in consistent ~!");
    }

    uint32_t N = oracle.size();
    float avg_co (0.0);

    unsigned step = N / 100;
    cerr << "Progress : ";

    for (unsigned k = 0; k < N; k++){

       if ( k % step == 0) cerr <<"*"; 

      int len = graph[k].size();
      while(graph[k][len-1].id == 0){
        len--;
      }
      if (len >  edge_num){
        len = edge_num;
      }


      // check each pair of neighbors   
      long hit = 0; 
      for ( int i=0; i < len; i++ )        
        for ( int j=i+1; j < len; j++ ){

            // check if the edge < id[i] and id[j] exisits in the K-NN-graph; now we assume 
            bool exist = false;
            // check i's NBs 
            unsigned id_1 = graph[k][i].id;
            unsigned id_2 = graph[k][j].id;
            for ( int s=0; s < len; s++)
                if (graph[id_1][s].id == id_2) {
                    exist = true; break;
                }

            if ( !exist ) continue; 

            for ( int s=0; s < len; s++)
                if (graph[id_2][s].id == id_1) {
                    hit++; break;
                }
        }   

            // also check j's NBs  
        float ratio = 2.0*hit/(len*(len-1)); 
        avg_co += ratio;

    } // end of K 

    cerr << endl;

    avg_co = avg_co / N;

    return avg_co;
        
    } // end of function statistics   
               

    virtual void remove_near_edges(IndexOracle const &oracle, const int edge_num){ // diversity
	if (oracle.size() != graph.size()){
	  throw runtime_error("Build kgraph before adding long edges");
	}
	
	uint32_t N = oracle.size();
    unsigned step = N / 100;
    cerr << "Progress : ";

    //#pragma omp parallel for
	for (unsigned k = 0; k < N; k++){

        if ( k % step == 0 ) cerr <<"*";

	  //if (k % 1000 == 0) {
	  //  fprintf(stderr, "\r%d (%.3f\%)", k, 1.0 * k / N * 100);
	  //}
	  //auto const &knn = graph[k];
          //edge_num[k] /= 2;
	  float * map = new float[N];
	  memset(map, 0, sizeof(float)*N);
	  int len = graph[k].size();
	  while(graph[k][len-1].id == 0){
	    len--;
	  }
	  if (len > 2 * edge_num){
	    len = 2 * edge_num;
	  } 
	  for (int i = 1; i < edge_num; ++i){ // the first max_edge_num elements are the remaining ones
	    float max_sum_angle = -1.0;
	    int max_pos = -1;
	    for (int cand_pos = i; cand_pos < len; cand_pos++){


          // this is the version before sharing computation cost   
          float sum_angle = 0.0;
          for (int ppp = 1; ppp < i; ++ppp){
            sum_angle += oracle.angle(k, graph[k][cand_pos].id, graph[k][ppp].id);
          }

          // map[graph[k][cand_pos].id] += oracle.angle(k, graph[k][cand_pos].id, graph[k][i - 1].id);
          // float sum_angle = map[graph[k][cand_pos].id];



	      if (sum_angle > max_sum_angle){
		max_sum_angle = sum_angle;
		max_pos = cand_pos;
	      }
	    }
	    Neighbor temp = graph[k][i];
	    graph[k][i] = graph[k][max_pos];
	    graph[k][max_pos] = temp;
	  }
	  graph[k].resize(edge_num); // reset the size of NN list 

	  sort(graph[k].begin(), graph[k].end());
	  M[k] = edge_num;
	  delete[] map;
	   }

        cerr << endl;

      }
     



 
      virtual void remove_near_edges_2(IndexOracle const &oracle, vector<unsigned> const &edge_num, unordered_set<unsigned> const &skip_set){ // diversification, maxmin
        if (oracle.size() != graph.size()){
          throw runtime_error("Build kgraph before adding long edges");
        }

        uint32_t N = oracle.size();

// #pragma omp parallel for
        for (unsigned k = 0; k < N; k++){
	  if (skip_set.find(k) != skip_set.end()){
            continue;
          }
          //if (k % 1000 == 0) {
          //  fprintf(stderr, "\r%d (%.3f\%)", k, 1.0 * k / N * 100);
          //}
          //auto const &knn = graph[k];
          float * map = new float[N];
          memset(map, 0, sizeof(float)*N);
          int len = graph[k].size();
          while(graph[k][len-1].dist == 0.0){
            len--;
          }
          for (int i = 1; i < edge_num[k]; ++i){ // the first max_edge_num elements are the remaining ones
            float min_angle = 10000000000.0;
            int min_pos = -1;
            for (int cand_pos = i; cand_pos < len; cand_pos++){
	      float angle = oracle.angle(k, graph[k][cand_pos].id, graph[k][i - 1].id);
	      if (map[graph[k][cand_pos].id] == 0 || map[graph[k][cand_pos].id] < angle){
		map[graph[k][cand_pos].id] = angle;
	      }
	      //map[graph[k][cand_pos].id] += oracle.angle(k, graph[k][cand_pos].id, graph[k][i - 1].id);
              //float sum_angle = oracle.angle(k, graph[k][cand_pos].id, graph[k][i - 1].id);
              if (min_angle > map[graph[k][cand_pos].id]){
                min_angle = map[graph[k][cand_pos].id];
                min_pos = cand_pos;
              }
            }
            Neighbor temp = graph[k][i];
            graph[k][i] = graph[k][min_pos];
            graph[k][min_pos] = temp;
          }
          graph[k].resize(edge_num[k]);
          sort(graph[k].begin(), graph[k].end());
          M[k] = edge_num[k];
          delete[] map;
        }
      }

      virtual void add_long_edges(IndexOracle const &oracle, int const rank_top_k, int const change_num){
	  if (oracle.size() != graph.size()){
	    throw runtime_error("Build kgraph before adding long edges");
	  }

	  //fprintf(stderr, "mark\n");

	  uint32_t N = oracle.size();
	  vector<unsigned> rank_id_vec; // index: rank;  value: id
	  vector<unsigned> id_rank_vec; // index: id;  value: rank
	  for (unsigned i = 0; i < N; ++i){
	    rank_id_vec.push_back(i);
	  }
	  id_rank_vec.resize(N);

	  //fprintf(stderr, "mark\n");

	  for (unsigned i = 0; i < N; ++i){ 
	    //fprintf(stderr, "=======length: %d=========\n", graph[i].size());
	    if (i % change_num == 0){
	      // re-shuffle
	      random_shuffle(rank_id_vec.begin(), rank_id_vec.end());
	      for (unsigned j = 0; j < N; ++j){
		id_rank_vec[rank_id_vec[j]] = j;
	      }
	    }
	    unsigned start = id_rank_vec[i] - rank_top_k / 2;
	    if (start < 0) {
	      start = 0;
	    }
	    unsigned end = id_rank_vec[i] + rank_top_k / 2;
	    if (end >= N) {
	      end = N - 1;
	    }
	    
	    //fprintf(stderr, "start = %d, end = %d\n", start, end);

	    for (unsigned j = start; j <= end; ++j){
	      //test if rank_id[j] should be added
	      float dist = oracle(rank_id_vec[j], i);
	      bool is_dominated = false;
	      for (int k = 0; k < graph[i].size(); ++k){
		// if there exists a point in graph[i] that is closer to i in both distance and rank, 
		// then the test point will be not added into the graph
		if (abs(j - id_rank_vec[i]) > abs(id_rank_vec[i] - id_rank_vec[graph[i][k].id]) && dist > graph[i][k].dist){
		  is_dominated = true;
		  break;
		}
	      }
	      if (!is_dominated){
		M[i] += 1;
		graph[i].push_back(Neighbor(rank_id_vec[j], dist, true));
	      }
	    }

	    //fprintf(stderr, "length: %d\n", graph[i].size());

	    sort(graph[i].begin(), graph[i].end());
	  } // for
	  //fprintf(stderr, "final mark\n");
	}

      virtual void saver (char const *path) const {
	uint32_t N = graph.size();
	//fprintf(stderr,"%d\n", N);
	ofstream os(path, ios::binary);
	os.write(KGRAPH_MAGIC, KGRAPH_MAGIC_SIZE);
	os.write(reinterpret_cast<char const *>(&VERSION_MAJOR), sizeof(VERSION_MAJOR));
	os.write(reinterpret_cast<char const *>(&VERSION_MINOR), sizeof(VERSION_MINOR));
	os.write(reinterpret_cast<char const *>(&N), sizeof(N));
	for (unsigned i = 0; i < graph.size(); ++i) {
	  auto const &knn = graph[i];
	  uint32_t K = knn.size();
	  //cout<<K<<endl;
	  //fprintf(stderr,"%d\n", K);
	  os.write(reinterpret_cast<char const *>(&K), sizeof(K));
	  os.write(reinterpret_cast<char const *>(&K), sizeof(K));
	  os.write(reinterpret_cast<char const *>(&knn[0]), K * sizeof(knn[0]));
	}
      }

        virtual void save (char const *path) const {
            uint32_t N = graph.size();
            ofstream os(path, ios::binary);
            os.write(KGRAPH_MAGIC, KGRAPH_MAGIC_SIZE);
            os.write(reinterpret_cast<char const *>(&VERSION_MAJOR), sizeof(VERSION_MAJOR));
            os.write(reinterpret_cast<char const *>(&VERSION_MINOR), sizeof(VERSION_MINOR));
            os.write(reinterpret_cast<char const *>(&N), sizeof(N));
            for (unsigned i = 0; i < graph.size(); ++i) {
                auto const &knn = graph[i];
                uint32_t K = knn.size();
				
                os.write(reinterpret_cast<char const *>(&K), sizeof(K)); // modified by Yifang
                os.write(reinterpret_cast<char const *>(&K), sizeof(K));
                for (unsigned j = 0; j < K; ++j){
                  os.write(reinterpret_cast<char const *>(&knn[j].id), sizeof(knn[j].id));
                }
                //os.write(reinterpret_cast<char const *>(&knn[0]), K * sizeof(knn[0]));
            }
        }
		
        virtual void get_hubs (char const *hubs_path,char const *knn_path) const {
            uint32_t N = graph.size();
            ofstream os(hubs_path, ios::binary);
            ofstream knn_out(knn_path, ios::binary);
            int hubs[N];
	    memset(hubs,0,sizeof(int)*N);
            for (unsigned i = 0; i < N; ++i) {
                auto const &knn = graph[i];
                uint32_t K = knn.size();
                for (unsigned j=0;j<K;j++)  {
		  hubs[knn[j].id]++;
		  knn_out<<knn[j].id<<" ";
		}
		knn_out<<"\n";
		
            }
	    for (unsigned i = 0; i < N; ++i) {
	      os<<hubs[i]<<endl;
            }
        }

      virtual void build (IndexOracle const &oracle, IndexParams const &param,  char const *path, IndexInfo *info);
      //virtual void build (IndexOracle const &oracle, char const *path, IndexParams const &param);

        /*
        virtual void prune (unsigned K) {
            for (auto &v: graph) {
                if (v.size() > K) {
                    v.resize(K);
                }
            }
            return;
            vector<vector<unsigned>> pruned(graph.size());
            vector<set<unsigned>> reachable(graph.size());
            vector<bool> added(graph.size());
            for (unsigned k = 0; k < K; ++k) {
#pragma omp parallel for
                for (unsigned n = 0; n < graph.size(); ++n) {
                    vector<unsigned> const &from = graph[n];
                    if (from.size() <= k) continue;
                    unsigned e = from[k];
                    if (reachable[n].count(e)) {
                        added[n] = false;
                    }
                    else {
                        pruned[n].push_back(e);
                        added[n] = true;
                    }
                }
                // expand reachable
#pragma omp parallel for
                for (unsigned n = 0; n < graph.size(); ++n) {
                    vector<unsigned> const &to = pruned[n];
                    set<unsigned> &nn = reachable[n];
                    if (added[n]) {
                        for (unsigned v: pruned[to.back()]) {
                            nn.insert(v);
                        }
                    }
                    for (unsigned v: to) {
                        if (added[v]) {
                            nn.insert(pruned[v].back());
                        }
                    }
                }
            }
            graph.swap(pruned);
        }
        */

      virtual void kgraph2dot (char * const &filename){
	std::ofstream fs;
	fs.open(filename, std::ofstream::out);

	//	fs << "strict digraph g {" << endl;

	for (unsigned i = 0; i < graph.size(); ++i){
	  for (unsigned j = 0; j < M[i]; ++j){
	    fs << i << " => " << graph[i][j].id << endl; //"[ label=\"" << graph[i][j].dist << "\" ];" << endl;;
	  }
	}

	//fs << "}" << endl;
      }

      virtual unsigned advanced_search (SearchOracle const &oracle, SearchParams const &params, unsigned *ids, float *dists, SearchInfo *pinfo, string const &info_path) const {// Matrix<float> const &d_init,
	/*fprintf(stderr, "all_dist=(%f", oracle(0));
	    for (int i = 1; i < oracle.size(); ++i){
	        fprintf(stderr, ", %f", oracle(i));
		}*/
	//fprintf(stderr, "access_dist=(");
	//fprintf(stdout, "minimum_dist=(");

	bool print_flag = true;
	std::ofstream path_fs, mind_fs, curd_fs, nh_fs;
	string file_name;
	if (info_path.empty()) {
	  print_flag = false;
	}
	
	if (print_flag){
	  file_name.assign(info_path);
	  file_name += "_path.txt";
	  path_fs.open(file_name.c_str(), std::ofstream::out | std::ofstream::app);
	  file_name.assign(info_path);
          file_name += "_mindist.txt";
	  mind_fs.open(file_name.c_str(), std::ofstream::out | std::ofstream::app);
	  file_name.assign(info_path);
          file_name += "_curdist.txt";
	  curd_fs.open(file_name.c_str(), std::ofstream::out | std::ofstream::app);
	  file_name.assign(info_path);
          file_name += "_nh.txt";
          nh_fs.open(file_name.c_str(), std::ofstream::out | std::ofstream::app);
	}
	
	if (graph.size() > oracle.size()) {
	  throw runtime_error("dataset larger than index");
	}
	if (params.P >= graph.size()) {
	  if (pinfo) {
	    pinfo->updates = 0;
	    pinfo->cost = 1.0;
	  }
	  return oracle.search(params.K, params.epsilon, ids, dists);
	}
	vector<Neighbor> knn(params.K + params.P +1);
	vector<Neighbor> init_knn(params.init);
	vector<Neighbor> results;
	boost::dynamic_bitset<> flags(graph.size(), false);

	if (params.init && params.T > 1) {
	  throw runtime_error("when init > 0, T must be 1.");
	}

	unsigned seed = params.seed;
	unsigned updates = 0;
	if (seed == 0) seed = time(NULL);
	mt19937 rng(seed);
	unsigned n_comps = 0;
	for (unsigned trial = 0; trial < params.T; ++trial) {
	  unsigned L = params.init;
	  //init>=K
	  if (L == 0) {   // generate random starting points
	    vector<unsigned> random(params.P);
	    GenRandom(rng, &random[0], random.size(), graph.size());
	    for (unsigned s: random) {
	      if (!flags[s]) {
		knn[L++].id = s;
		//flags[s] = true;
	      }
	    }
	    for (unsigned k = 0; k < L; ++k) {
	      flags[knn[k].id] = true;
	      knn[k].flag = true;
	      knn[k].dist = oracle(knn[k].id);
	            
	    }
	    sort(knn.begin(), knn.begin() + L);
	  }
	  else {          // user-provided starting points.
	    BOOST_VERIFY(ids); //ids
	    //std::cout<<"set seed"<<endl;
	    //BOOST_VERIFY(L < params.K);
	    for (unsigned l = 0; l < L; ++l) { // L -> knn.size()
	      init_knn[l].id = ids[l];  //ids
	      init_knn[l].flag = true;
              init_knn[l].dist = dists[l];
	      //fprintf(stderr, "%d: %d\n", l, knn[l].id);
	    }
	    //std::cout<<knn.size()<<endl;
	    //cout<<knn[0].id<<endl;
                
	           
	    //sort(init_knn.begin(), init_knn.begin() + L);
	        
	    /*//delete
	          vector<Neighbor> init_fknn(graph.size());
		      for (unsigned k = 0; k < graph.size(); ++k){
		            init_fknn[k].id = k;
			          init_fknn[k].flag = true;
				        init_fknn[k].dist = oracle(k);
					    }
					        sort(init_fknn.begin(), init_fknn.end());
						    //end delete
						    */
	    L = params.K;
	    for (int ii = 0; ii < L; ++ii){
	      //flags[init_fknn[graph.size() - 1 - ii].id] = true;
	      //knn[L - 1 - ii] = init_fknn[graph.size() - 1 - ii];
	      //recover
	      flags[init_knn[ii].id] = true;
	      knn[ii] = init_knn[ii];
	      //end recover
	      //fprintf(stderr, "%d %f\n", knn[ii].id, knn[ii].dist);
	    }
	  }

	  if (print_flag){
	    for (unsigned k = 0; k < L; ++k){
	      path_fs << "-3 " << knn[k].id << endl;
	      mind_fs << knn[0].dist << endl;
	      curd_fs << knn[k].dist << endl;
	          
	      nh_fs << M[knn[k].id] << " ";
	      unordered_set<int> set;
	      set.insert(knn[k].id);
	      auto const &neighbors = graph[knn[k].id];
	      for (unsigned l = 0; l < M[knn[k].id]; ++l){
		set.insert(neighbors[l].id);
		auto const &new_neighbors = graph[neighbors[l].id];
		for (unsigned ll = 0; ll < M[neighbors[l].id]; ++ll){
		  set.insert(new_neighbors[ll].id);
		}
	      }
	      nh_fs << set.size() << endl;
	    }
	  }
	  
	  unsigned k = 0;
	  while (k < L) {
	    unsigned nk = L;
	    if (knn[k].flag) {
	      knn[k].flag = false;
	      unsigned cur = knn[k].id;
	      //BOOST_VERIFY(cur < graph.size());
	      unsigned maxM = M[cur];
	      if (params.M > maxM) maxM = params.M;
	      auto const &neighbors = graph[cur];
	      if (maxM > neighbors.size()) {
		maxM = neighbors.size();
	      }
	      for (unsigned m = 0; m < maxM; ++m) {
		unsigned id = neighbors[m].id;
		//BOOST_VERIFY(id < graph.size());
		if (flags[id]) continue;
		flags[id] = true;
		++n_comps;
		//if(n_comps >= params.Maxchecks)
		//  {
		//  break;
		//  }
		float dist = oracle(id);
		    
		/*if(n_comps == 1){
		        fprintf(stderr, "%f", dist);
			    } else {
			          fprintf(stderr, ", %f", dist);
				  }*/
		Neighbor nn(id, dist);
		unsigned r = UpdateKnnList(&knn[0], L, nn);
		if (print_flag){
		  path_fs << cur << " " << id << endl;
		  mind_fs << knn[0].dist << endl;
		  curd_fs << dist << endl;

		  nh_fs << M[id] << " ";
		  unordered_set<int> set;
		  set.insert(id);
		  auto const &nbors = graph[id];
		  for (unsigned l_1 = 0; l_1 < M[id]; ++l_1){
		    set.insert(nbors[l_1].id);
		    auto const &new_neighbors = graph[nbors[l_1].id];
		    for (unsigned ll = 0; ll < M[nbors[l_1].id]; ++ll){
		      set.insert(new_neighbors[ll].id);
		    }
		  }
		  nh_fs << set.size() << endl;
		}
		/*if(n_comps == 1){
                              fprintf(stdout, "%f", knn[0].dist);
                            } else {
                              fprintf(stdout, ", %f", knn[0].dist);
			      }*/
		BOOST_VERIFY(r <= L);
		//if (r > L) continue;
		if (L + 1 < knn.size()) ++L;
		if (r < nk) {
		  nk = r;
		}
	      }
	      //fprintf(stderr, "}\n");
	    }
	    if (nk <= k) {
	      k = nk;
	    }
	    else {
	      ++k;
	    }
	    //if(n_comps >= params.Maxchecks)
	    //{
	    //   break;
	    //}
	  }
	  if (L > params.K) L = params.K;
	  if (results.empty()) {
	    results.reserve(params.K + 1);
	    results.resize(L + 1);
	    copy(knn.begin(), knn.begin() + L, results.begin());
	  }
	  else {
	    // update results
	    for (unsigned l = 0; l < L; ++l) {
	      unsigned r = UpdateKnnList(&results[0], results.size() - 1, knn[l]);
	      if (r < results.size() /* inserted */ && results.size() < (params.K + 1)) {
		results.resize(results.size() + 1);
	      }
	    }
	  }
	}
	// check epsilon
	{
	  for (unsigned l = 0; l < results.size(); ++l) {
	    if (results[l].dist > params.epsilon) {
	      results.resize(l);
	      break;
	    }
	  }
	}
	unsigned L = results.size() - 1;
	BOOST_VERIFY(L <= params.K);
	// check epsilon
	if (ids) {
	  for (unsigned k = 0; k < L; ++k) {
	    ids[k] = results[k].id;
	  }
	}
	if (dists) {
	  for (unsigned k = 0; k < L; ++k) {
	    dists[k] = results[k].dist;
	  }
	}
	if (pinfo) {
	  pinfo->updates = updates;
	  pinfo->cost = float(n_comps) / graph.size();
	  pinfo->checks=n_comps;
	}
	if (print_flag){
	  path_fs << "-1" << endl;
	  for (unsigned k = 0; k < results.size() - 1; ++k){
	    path_fs << results[k].id << endl; //fprintf(stderr, "%d [color=lightblue, style=filled, label=\"top-%d\"]\n", results[k].id, k+1);
	  }
	  path_fs << "-2" << endl;
	  mind_fs << "-1" << endl;
	  curd_fs << "-1" << endl;
	  nh_fs << "-1" << endl;
	}
	//fprintf(stderr, ")\n");
	//fprintf(stdout, ")\n");
	//fprintf(stderr, ")\nplt.hist(kg_access_dist, bins=100, normed=False, alpha=0.75, label='accessed')\nplt.hist(all_dist, bins=100, normed=False, alpha=0.5, label='all')\nplt.title(\"Distribution of accessed points by KGraph\")\nplt.xlabel(\"distance\")\nplt.ylabel(\"Frequency\")\nplt.legend(loc='upper right')\nshow()\n");
	return L;
      }

      virtual void find_lm (SearchOracle const &oracle) const {
	unsigned K = 10;
	unsigned N = oracle.size();
	unsigned * ordered_nn_id = new unsigned[N];
	unsigned * rank = new unsigned[N];
	float * ordered_nn_dist = new float[N];
	bool * is_lm = new bool[N];
	unordered_set<unsigned> top_set;
	int * top_cover = new int[K];
	bool * traverse = new bool[N];
	int counter = 0;


	queue<unsigned> new_queue;
	queue<unsigned> cur_queue;
	
	oracle.search(N, default_epsilon, ordered_nn_id, ordered_nn_dist);


	for (unsigned i = 0; i < N; ++i){
	  rank[ordered_nn_id[i]] = i;
	  is_lm[i] = true;
	  traverse[i] = false;
	}
	for (unsigned i = 0; i < K; ++i){
	  top_set.insert(i);
	}
	
	for (unsigned i = 0; i < N; ++i){
	  auto const &neighbors = graph[i];
	  for (unsigned l = 0; l < M[i]; ++l){
	    if (rank[neighbors[l].id] < rank[i]){
	      is_lm[i] = false;
	      break;
	    }
	  }
	  if (is_lm[i]){ // is LM, output something
	    counter ++;
	    fprintf(stderr, "rank:\t%d\n", rank[i]);
	    fprintf(stderr, "dist:\t%f\n", ordered_nn_dist[rank[i]]);
	    cur_queue.push(i);
	    traverse[i] = true;
	    if (top_set.find(rank[i]) != top_set.end()){
	      top_cover[rank[i]] = 0;
	      top_set.erase(rank[i]);
	    }
	  }
	}

	//	fprintf(stderr, "N=%d\n", N);
	//find coverage of top-K
	unsigned level = 0;
	while(true){
	  level++;
	  //fprintf(stderr, "mark2.1\n");
	  while(!cur_queue.empty()){
	    unsigned id = cur_queue.front();
	    cur_queue.pop();
	    auto const &neighbors = graph[id];
	    //fprintf(stderr, "mark2.5 %d\n", M[id]);
	    for (unsigned l = 0; l < M[id]; ++l){
	      //fprintf(stderr, "%d\n", neighbors[l].id);
	      //fprintf(stderr, "%d\n", traverse[neighbors[l].id]);
	      if (!traverse[neighbors[l].id]){
		new_queue.push(neighbors[l].id);
		traverse[neighbors[l].id] = true;
		//fprintf(stderr, "-> %d\n", rank[neighbors[l].id]);
		if (rank[neighbors[l].id] < K && top_set.find(rank[neighbors[l].id]) != top_set.end()){
		  top_cover[rank[neighbors[l].id]] = level;
		  top_set.erase(rank[neighbors[l].id]);
		  if (top_set.empty()){
		    goto jump_out;
		  }
		}
	      }
	    } // end for loop
	  }
	  cur_queue.swap(new_queue);
	}


      jump_out:
	fprintf(stderr, "mark3\n");
	fprintf(stderr, "# of LMins:\t%d\n", counter);
	for (unsigned i = 0; i < K; ++i){
	  fprintf(stderr, "top-%d is %d hops away from LMins\n", i + 1, top_cover[i]);
	}
	
	delete[] ordered_nn_id;
	delete[] rank;
	delete[] ordered_nn_dist;
	delete[] is_lm;
	delete[] top_cover;
      }
      
      virtual unsigned search (SearchOracle const &oracle, SearchParams const &params, unsigned *ids, float *dists, SearchInfo *pinfo, string const &info_path) const {// Matrix<float> const &d_init,
	  /*fprintf(stderr, "all_dist=(%f", oracle(0));
	  for (int i = 1; i < oracle.size(); ++i){
	    fprintf(stderr, ", %f", oracle(i));
	    }*/
	  //fprintf(stderr, "access_dist=(");
	  //fprintf(stdout, "minimum_dist=(");

	bool print_flag = true;
	std::ofstream path_fs, mind_fs, curd_fs, nh_fs;
	string file_name;
	if (info_path.empty()) {
	  print_flag = false;
	}
	
	if (print_flag){
	  file_name.assign(info_path);
	  file_name += "_path.txt";
	  path_fs.open(file_name.c_str(), std::ofstream::out | std::ofstream::app);
	  file_name.assign(info_path);
          file_name += "_mindist.txt";
	  mind_fs.open(file_name.c_str(), std::ofstream::out | std::ofstream::app);
	  file_name.assign(info_path);
          file_name += "_curdist.txt";
	  curd_fs.open(file_name.c_str(), std::ofstream::out | std::ofstream::app);
	  file_name.assign(info_path);
          file_name += "_nh.txt";
          nh_fs.open(file_name.c_str(), std::ofstream::out | std::ofstream::app);
	}
	
	if (graph.size() > oracle.size()) {
	  throw runtime_error("dataset larger than index");
	}
	if (params.P >= graph.size()) {
	  if (pinfo) {
	    pinfo->updates = 0;
	    pinfo->cost = 1.0;
	  }
	  return oracle.search(params.K, params.epsilon, ids, dists);
	}
            vector<Neighbor> knn(params.K + params.P +1);
	    vector<Neighbor> init_knn(params.init);
            vector<Neighbor> results;
            boost::dynamic_bitset<> flags(graph.size(), false);

            if (params.init && params.T > 1) {
                throw runtime_error("when init > 0, T must be 1.");
            }

            unsigned seed = params.seed;
            unsigned updates = 0;
            if (seed == 0) seed = time(NULL);
            mt19937 rng(seed);
            unsigned n_comps = 0;
            for (unsigned trial = 0; trial < params.T; ++trial) {
                unsigned L = params.init;
                //init>=K
                if (L == 0) {   // generate random starting points
                    vector<unsigned> random(params.P);
                    GenRandom(rng, &random[0], random.size(), graph.size());
                    for (unsigned s: random) {
                        if (!flags[s]) {
                            knn[L++].id = s;
                            //flags[s] = true;
                        }
                    }
		    for (unsigned k = 0; k < L; ++k) {
		      flags[knn[k].id] = true;
		      knn[k].flag = true;
		      knn[k].dist = oracle(knn[k].id);
		      
		    }
		    sort(knn.begin(), knn.begin() + L);
                }
                else {          // user-provided starting points.
                    BOOST_VERIFY(ids); //ids
                    //std::cout<<"set seed"<<endl;
                    //BOOST_VERIFY(L < params.K);
                    for (unsigned l = 0; l < L; ++l) { // L -> knn.size()
                        init_knn[l].id = ids[l];  //ids
			//fprintf(stderr, "%d: %d\n", l, knn[l].id);
                    }
                    //std::cout<<knn.size()<<endl;
                    //cout<<knn[0].id<<endl;
                
	       
		    for (unsigned k = 0; k < L; ++k) {
		      //flags[knn[k].id] = true;
		      init_knn[k].flag = true;
		      init_knn[k].dist = oracle(init_knn[k].id);
		    }
		    sort(init_knn.begin(), init_knn.begin() + L);
		    
		    /*//delete
		    vector<Neighbor> init_fknn(graph.size());
		    for (unsigned k = 0; k < graph.size(); ++k){
		      init_fknn[k].id = k;
		      init_fknn[k].flag = true;
		      init_fknn[k].dist = oracle(k);
		    }
		    sort(init_fknn.begin(), init_fknn.end());
		    //end delete
		    */
		    L = params.K;
		    for (int ii = 0; ii < L; ++ii){
		      //flags[init_fknn[graph.size() - 1 - ii].id] = true;
		      //knn[L - 1 - ii] = init_fknn[graph.size() - 1 - ii];
		      //recover
		      flags[init_knn[ii].id] = true;
		      knn[ii] = init_knn[ii];
		      //end recover
		      //fprintf(stderr, "%d %f\n", knn[ii].id, knn[ii].dist);
		    }
		}

		if (print_flag){
		  for (unsigned k = 0; k < L; ++k){
		    path_fs << "-3 " << knn[k].id << endl;
		    mind_fs << knn[0].dist << endl;
		    curd_fs << knn[k].dist << endl;
		    
		    nh_fs << M[knn[k].id] << " ";
		    unordered_set<int> set;
		    set.insert(knn[k].id);
		    auto const &neighbors = graph[knn[k].id];
		    for (unsigned l = 0; l < M[knn[k].id]; ++l){
		      set.insert(neighbors[l].id);
		      auto const &new_neighbors = graph[neighbors[l].id];
		      for (unsigned ll = 0; ll < M[neighbors[l].id]; ++ll){
			set.insert(new_neighbors[ll].id);
		      }
		    }
		    nh_fs << set.size() << endl;
		  }
		}
		
		unsigned k = 0;
 
                while (k < L) {
                    unsigned nk = L;
                    if (knn[k].flag) {
                        knn[k].flag = false;
                        unsigned cur = knn[k].id;
			//BOOST_VERIFY(cur < graph.size());
                        unsigned maxM = M[cur];
                        if (params.M > maxM) maxM = params.M;
                        auto const &neighbors = graph[cur];
                        if (maxM > neighbors.size()) {
                            maxM = neighbors.size();
                        }
                        for (unsigned m = 0; m < maxM; ++m) {
                            unsigned id = neighbors[m].id;
                            //BOOST_VERIFY(id < graph.size());
                            if (flags[id]) continue;
                            flags[id] = true;
                            ++n_comps;
                            //if(n_comps >= params.Maxchecks)
			    //  {
                            //  break;
			    //  }
                            float dist = oracle(id);
			    
			    /*if(n_comps == 1){
			      fprintf(stderr, "%f", dist);
			    } else {
			      fprintf(stderr, ", %f", dist);
			      }*/
			    Neighbor nn(id, dist);
			    unsigned r = UpdateKnnList(&knn[0], L, nn);
			    if (print_flag){
                              path_fs << cur << " " << id << endl;
                              mind_fs << knn[0].dist << endl;
			      curd_fs << dist << endl;

			      nh_fs << M[id] << " ";
			      unordered_set<int> set;
			      set.insert(id);
			      auto const &nbors = graph[id];
			      for (unsigned l_1 = 0; l_1 < M[id]; ++l_1){
				set.insert(nbors[l_1].id);
				auto const &new_neighbors = graph[nbors[l_1].id];
				for (unsigned ll = 0; ll < M[nbors[l_1].id]; ++ll){
				  set.insert(new_neighbors[ll].id);
				}
			      }
			      nh_fs << set.size() << endl;
                            }
			    /*if(n_comps == 1){
                              fprintf(stdout, "%f", knn[0].dist);
                            } else {
                              fprintf(stdout, ", %f", knn[0].dist);
			      }*/
                            BOOST_VERIFY(r <= L);
                            //if (r > L) continue;
                            if (L + 1 < knn.size()) ++L;
                            if (r < nk) {
                                nk = r;
                            }
                        }
			//fprintf(stderr, "}\n");
                    }
                    if (nk <= k) {
                        k = nk;
                    }
                    else {
                        ++k;
                    }
                    //if(n_comps >= params.Maxchecks)
		    //			{
                    //   break;
                    //}
                }
                if (L > params.K) L = params.K;
                if (results.empty()) {
                    results.reserve(params.K + 1);
                    results.resize(L + 1);
                    copy(knn.begin(), knn.begin() + L, results.begin());
                }
                else {
                    // update results
                    for (unsigned l = 0; l < L; ++l) {
                        unsigned r = UpdateKnnList(&results[0], results.size() - 1, knn[l]);
                        if (r < results.size() /* inserted */ && results.size() < (params.K + 1)) {
                            results.resize(results.size() + 1);
                        }
                    }
                }
            }
	    //	    results.pop_back();
            // check epsilon
            {
                for (unsigned l = 0; l < results.size(); ++l) {
                    if (results[l].dist > params.epsilon) {
                        results.resize(l);
                        break;
                    }
                }
            }
            unsigned L = results.size() - 1;
            BOOST_VERIFY(L <= params.K);
            // check epsilon
            if (ids) {
                for (unsigned k = 0; k < L; ++k) {
                    ids[k] = results[k].id;
                }
            }
            if (dists) {
                for (unsigned k = 0; k < L; ++k) {
                    dists[k] = results[k].dist;
                }
            }
            if (pinfo) {
                pinfo->updates = updates;
                pinfo->cost = float(n_comps) / graph.size();
                pinfo->checks=n_comps;
            }
	    if (print_flag){
	      path_fs << "-1" << endl;
	      for (unsigned k = 0; k < results.size() - 1; ++k){
		path_fs << results[k].id << endl; //fprintf(stderr, "%d [color=lightblue, style=filled, label=\"top-%d\"]\n", results[k].id, k+1);
	      }
	      path_fs << "-2" << endl;
	      mind_fs << "-1" << endl;
	      curd_fs << "-1" << endl;
	      nh_fs << "-1" << endl;
	    }
	    //fprintf(stderr, ")\n");
	    //fprintf(stdout, ")\n");
	    //fprintf(stderr, ")\nplt.hist(kg_access_dist, bins=100, normed=False, alpha=0.75, label='accessed')\nplt.hist(all_dist, bins=100, normed=False, alpha=0.5, label='all')\nplt.title(\"Distribution of accessed points by KGraph\")\nplt.xlabel(\"distance\")\nplt.ylabel(\"Frequency\")\nplt.legend(loc='upper right')\nshow()\n");
            return L;
        }


      virtual unsigned search_greedy (SearchOracle const &oracle, SearchParams const &params, unsigned *ids, float *dists, SearchInfo *pinfo, string const &info_path) const {// Matrix<float> const &d_init,
      /*fprintf(stderr, "all_dist=(%f", oracle(0));
      for (int i = 1; i < oracle.size(); ++i){
        fprintf(stderr, ", %f", oracle(i));
        }*/
      //fprintf(stderr, "access_dist=(");
      //fprintf(stdout, "minimum_dist=(");

    bool print_flag = true;
    std::ofstream path_fs, mind_fs, curd_fs;
    string file_name;
    if (info_path.empty()) {
      print_flag = false;
    }
    
    if (print_flag){
      file_name.assign(info_path);
      file_name += "_path.txt";
      path_fs.open(file_name.c_str(), std::ofstream::out | std::ofstream::app);
      file_name.assign(info_path);
          file_name += "_mindist.txt";
      mind_fs.open(file_name.c_str(), std::ofstream::out | std::ofstream::app);
      file_name.assign(info_path);
          file_name += "_curdist.txt";
      curd_fs.open(file_name.c_str(), std::ofstream::out | std::ofstream::app);
    }
    
    if (graph.size() > oracle.size()) {
      throw runtime_error("dataset larger than index");
    }
    if (params.P >= graph.size()) {
      if (pinfo) {
        pinfo->updates = 0;
        pinfo->cost = 1.0;
      }
      return oracle.search(params.K, params.epsilon, ids, dists);
    }
            vector<Neighbor> knn(params.K + params.P +1);
        vector<Neighbor> init_knn(params.init);
            vector<Neighbor> results;
            boost::dynamic_bitset<> flags(graph.size(), false);

            if (params.init && params.T > 1) {
                throw runtime_error("when init > 0, T must be 1.");
            }

            unsigned seed = params.seed;
            unsigned updates = 0;
            if (seed == 0) seed = time(NULL);
            mt19937 rng(seed);
            unsigned n_comps = 0;
            for (unsigned trial = 0; trial < params.T; ++trial) {
                unsigned L = params.init;
                //init>=K
                if (L == 0) {   // generate random starting points
                    vector<unsigned> random(params.P);
                    GenRandom(rng, &random[0], random.size(), graph.size());
                    for (unsigned s: random) {
                        if (!flags[s]) {
                            knn[L++].id = s;
                            //flags[s] = true;
                        }
                    }
            for (unsigned k = 0; k < L; ++k) {
              flags[knn[k].id] = true;
              knn[k].flag = true;
              knn[k].dist = oracle(knn[k].id);
            }
            sort(knn.begin(), knn.begin() + L);
                }
                else {          // user-provided starting points.
                    BOOST_VERIFY(ids); //ids
                    //std::cout<<"set seed"<<endl;
                    //BOOST_VERIFY(L < params.K);
                    for (unsigned l = 0; l < L; ++l) { // L -> knn.size()
                        init_knn[l].id = ids[l];  //ids
            //fprintf(stderr, "%d: %d\n", l, knn[l].id);
                    }
                    //std::cout<<knn.size()<<endl;
                    //cout<<knn[0].id<<endl;
                
           
            for (unsigned k = 0; k < L; ++k) {
              //flags[knn[k].id] = true;
              init_knn[k].flag = true;
              init_knn[k].dist = oracle(init_knn[k].id);
            }
            sort(init_knn.begin(), init_knn.begin() + L);
            
            /*//delete
            vector<Neighbor> init_fknn(graph.size());
            for (unsigned k = 0; k < graph.size(); ++k){
              init_fknn[k].id = k;
              init_fknn[k].flag = true;
              init_fknn[k].dist = oracle(k);
            }
            sort(init_fknn.begin(), init_fknn.end());
            //end delete
            */
            L = params.K;
            for (int ii = 0; ii < L; ++ii){
              //flags[init_fknn[graph.size() - 1 - ii].id] = true;
              //knn[L - 1 - ii] = init_fknn[graph.size() - 1 - ii];
              //recover
              flags[init_knn[ii].id] = true;
              knn[ii] = init_knn[ii];
              //end recover
              //fprintf(stderr, "%d %f\n", knn[ii].id, knn[ii].dist);
            }
        }

        if (print_flag){
          mind_fs << knn[0].dist << endl;
          curd_fs << knn[0].dist << endl;
        }
        
        unsigned k = 0;
                while (k < L) {
                    unsigned nk = L;
                    if (knn[k].flag) {
                        knn[k].flag = false;
                        unsigned cur = knn[k].id;
            //BOOST_VERIFY(cur < graph.size());
                        unsigned maxM = M[cur];
			float thres = knn[k].dist;
                        if (params.M > maxM) maxM = params.M;
                        auto const &neighbors = graph[cur];
                        if (maxM > neighbors.size()) {
                            maxM = neighbors.size();
                        }
                        for (unsigned m = 0; m < maxM; ++m) {
                            unsigned id = neighbors[m].id;
                            //BOOST_VERIFY(id < graph.size());
                            if (flags[id]) continue;
                            flags[id] = true;
                            ++n_comps;
			    //                            if(n_comps >= params.Maxchecks)
			    //  {
			    //break;
			    //  }
                            float dist = oracle(id);
			    if (dist > thres) {
			      continue;
			    }
                /*if(n_comps == 1){
                  fprintf(stderr, "%f", dist);
                } else {
                  fprintf(stderr, ", %f", dist);
                  }*/
			    Neighbor nn(id, dist);
			    unsigned r = UpdateKnnList(&knn[0], L, nn);
			    if (print_flag){
                              path_fs << cur << " " << id << endl;
                              mind_fs << knn[0].dist << endl;
			      curd_fs << dist << endl;
                            }
			    /*if(n_comps == 1){
                              fprintf(stdout, "%f", knn[0].dist);
			      } else {
                              fprintf(stdout, ", %f", knn[0].dist);
			      }*/
                            BOOST_VERIFY(r <= L);
                            //if (r > L) continue;
                            if (L + 1 < knn.size()) ++L;
                            if (r < nk) {
			      nk = r;
                            }
                        }
			//fprintf(stderr, "}\n");
                    }
                    if (nk <= k) {
                        k = nk;
                    }
                    else {
                        ++k;
                    }
		    //                    if(n_comps >= params.Maxchecks)
                    //{
                    //   break;
		    //    }
                }
                if (L > params.K) L = params.K;
                if (results.empty()) {
                    results.reserve(params.K + 1);
                    results.resize(L + 1);
                    copy(knn.begin(), knn.begin() + L, results.begin());
                }
                else {
                    // update results
                    for (unsigned l = 0; l < L; ++l) {
                        unsigned r = UpdateKnnList(&results[0], results.size() - 1, knn[l]);
                        if (r < results.size() /* inserted */ && results.size() < (params.K + 1)) {
                            results.resize(results.size() + 1);
                        }
                    }
                }
            }
            // check epsilon
            {
                for (unsigned l = 0; l < results.size(); ++l) {
                    if (results[l].dist > params.epsilon) {
                        results.resize(l);
                        break;
                    }
                }
            }
            unsigned L = results.size() - 1;
            BOOST_VERIFY(L <= params.K);
            // check epsilon
            if (ids) {
                for (unsigned k = 0; k < L; ++k) {
                    ids[k] = results[k].id;
                }
            }
            if (dists) {
                for (unsigned k = 0; k < L; ++k) {
                    dists[k] = results[k].dist;
                }
            }
            if (pinfo) {
                pinfo->updates = updates;
                pinfo->cost = float(n_comps) / graph.size();
                pinfo->checks=n_comps;
            }
        if (print_flag){
          path_fs << "-1" << endl;
          for (unsigned k = 0; k < results.size() - 1; ++k){
        path_fs << results[k].id << endl; //fprintf(stderr, "%d [color=lightblue, style=filled, label=\"top-%d\"]\n", results[k].id, k+1);
          }
          path_fs << "-2" << endl;
          mind_fs << "-1" << endl;
          curd_fs << "-1" << endl;
        }
        //fprintf(stderr, ")\n");
        //fprintf(stdout, ")\n");
        //fprintf(stderr, ")\nplt.hist(kg_access_dist, bins=100, normed=False, alpha=0.75, label='accessed')\nplt.hist(all_dist, bins=100, normed=False, alpha=0.5, label='all')\nplt.title(\"Distribution of accessed points by KGraph\")\nplt.xlabel(\"distance\")\nplt.ylabel(\"Frequency\")\nplt.legend(loc='upper right')\nshow()\n");
            return L;
        }

        virtual void get_nn (unsigned id, unsigned *nns, float *dist, unsigned *pM, unsigned *pL) const {
            BOOST_VERIFY(id < graph.size());
            auto const &v = graph[id];
            *pM = M[id];
            *pL = v.size();
            if (nns) {
                for (unsigned i = 0; i < v.size(); ++i) {
                    nns[i] = v[i].id;
                }
            }
            if (dist) {
                for (unsigned i = 0; i < v.size(); ++i) {
                    dist[i] = v[i].dist;
                }
            }
        }

        void prune1 () {
            for (unsigned i = 0; i < graph.size(); ++i) {
                if (graph[i].size() > M[i]) {
                    graph[i].resize(M[i]);
                }
            }
        }

        void prune2 () {
#if 0
            vector<vector<unsigned>> new_graph(graph.size());
            vector<unsigned> new_M(graph.size());
            vector<vector<unsigned>> reverse(graph.size());
            vector<unordered_set<unsigned>> todo(graph.size());
            unsigned L = 0;
            {
                cerr << "Level 2 Prune, stage 1/2 ..." << endl;
                progress_display progress(graph.size(), cerr);
                for (unsigned i = 0; i < graph.size(); ++i) {
                    if (graph[i].size() > L) L = graph[i].size();
                    todo[i] = unordered_set<unsigned>(graph[i].begin(), graph[i].end());
                    ++progress;
                }
            }
            {
                cerr << "Level 2 Prune, stage 2/2 ..." << endl;
                progress_display progress(L, cerr);
                for (unsigned l = 0; l < L; ++l) {
                    for (unsigned i = 0; i < graph.size(); ++i) {
                        if (todo[i].empty()) continue;
                        BOOST_VERIFY(l < graph[i].size());
                        unsigned T = graph[i][l];
                        if (todo[i].erase(T)) { // still there, need to be added
                            new_graph[i].push_back(T);
                            reverse[T].push_back(i);
                            // mark newly reachable nodes
                            for (auto n2: new_graph[T]) {
                                todo[i].erase(n2);
                            }
                            for (auto r: reverse[i]) {
                                todo[r].erase(T);
                            }
                        }
                        if (l + 1 == M[i]) {
                            new_M[i] = new_graph[i].size();
                        }
                    }
                    ++progress;
                }
            }
            graph.swap(new_graph);
            M.swap(new_M);
#endif
        }

        virtual void prune (IndexOracle const &oracle, unsigned level) {
            if (level & PRUNE_LEVEL_1) {
                prune1();
            }
            if (level & PRUNE_LEVEL_2) {
                prune2();
            }
        }
    };

    class KGraphConstructor: public KGraphImpl {
        // The neighborhood structure maintains a pool of near neighbors of an object.
        // The neighbors are stored in the pool.  "n" (<="params.L") is the number of valid entries
        // in the pool, with the beginning "k" (<="n") entries sorted.
        struct Nhood { // neighborhood
            Lock lock;
            float radius;   // distance of interesting range
            float radiusM;
            Neighbors pool;
            unsigned L;     // # valid items in the pool,  L + 1 <= pool.size()
            unsigned M;     // we only join items in pool[0..M)
            bool found;     // helped found new NN in this round
            vector<unsigned> nn_old;
            vector<unsigned> nn_new;
            vector<unsigned> rnn_old;
            vector<unsigned> rnn_new;

            // only non-readonly method which is supposed to be called in parallel
            unsigned parallel_try_insert (unsigned id, float dist) {
                if (dist > radius) return pool.size();
                LockGuard guard(lock);
                unsigned l = UpdateKnnList(&pool[0], L, Neighbor(id, dist, true));
                if (l <= L) { // inserted
                    if (L + 1 < pool.size()) { // if l == L + 1, there's a duplicate
                        ++L;
                    }
                    else {
                        radius = pool[L-1].dist;
                    }
                }
                return l;
            }

            // join should not be conflict with insert
            template <typename C>
            void join (C callback) const {
                for (unsigned const i: nn_new) {
                    for (unsigned const j: nn_new) {
                        if (i < j) {
                            callback(i, j);
                        }
                    }
                    for (unsigned j: nn_old) {
                        callback(i, j);
                    }
                }
            }
        };

        IndexOracle const &oracle;
        IndexParams params;
        IndexInfo *pinfo;
        vector<Nhood> nhoods;
        size_t n_comps;

        void init () {
            unsigned N = oracle.size();
            unsigned seed = params.seed;
            mt19937 rng(seed);
            for (auto &nhood: nhoods) {
                nhood.nn_new.resize(params.S * 2);
                nhood.pool.resize(params.L+1);
                nhood.radius = numeric_limits<float>::max();
            }
            #pragma omp parallel
            {
              #ifdef _OPENMP
              mt19937 rng(seed ^ omp_get_thread_num());
              #else
                mt19937 rng(seed);
              #endif
                vector<unsigned> random(params.S + 1);
               #pragma omp for
                for (unsigned n = 0; n < N; ++n) {
                    auto &nhood = nhoods[n];
                    Neighbors &pool = nhood.pool;
                    GenRandom(rng, &nhood.nn_new[0], nhood.nn_new.size(), N); //nn_new 2*S random sampling 
                    GenRandom(rng, &random[0], random.size(), N);  //random s+1
                    nhood.L = params.S;
                    nhood.M = params.S;
					//cout<<nhood.L<<endl;
                    unsigned i = 0;
                    for (unsigned l = 0; l < nhood.L; ++l) {
                        if (random[i] == n) ++i;
                        auto &nn = nhood.pool[l];
                        nn.id = random[i++];
                        nn.dist = oracle(nn.id, n);
                        nn.flag = true;
                    }
                    sort(pool.begin(), pool.begin() + nhood.L);
                }
            }
        }
        void join () {
            size_t cc = 0;
            #pragma omp parallel for default(shared) schedule(dynamic, 100) reduction(+:cc)
            for (unsigned n = 0; n < oracle.size(); ++n) {
                size_t uu = 0;
                nhoods[n].found = false;
                nhoods[n].join([&](unsigned i, unsigned j) {
                        float dist = oracle(i, j);
                        ++cc;
                        unsigned r;
                        r = nhoods[i].parallel_try_insert(j, dist);
                        if (r < params.K) ++uu;
                        nhoods[j].parallel_try_insert(i, dist);
                        if (r < params.K) ++uu;
                });
                nhoods[n].found = uu > 0;
            }
            n_comps += cc;
        }
        void update () {
            unsigned N = oracle.size();
            for (auto &nhood: nhoods) {
                nhood.nn_new.clear();
                nhood.nn_old.clear();
                nhood.rnn_new.clear();
                nhood.rnn_old.clear();
                nhood.radius = nhood.pool.back().dist;
            }
            //!!! compute radius2
            #pragma omp parallel for
            for (unsigned n = 0; n < N; ++n) {
                auto &nhood = nhoods[n];
                if (nhood.found) {
                    unsigned maxl = std::min(nhood.M + params.S, nhood.L);
                    unsigned c = 0;
                    unsigned l = 0;
                    while ((l < maxl) && (c < params.S)) {
                        if (nhood.pool[l].flag) ++c;
                        ++l;
                    }
                    nhood.M = l;
                }
                BOOST_VERIFY(nhood.M > 0);
                nhood.radiusM = nhood.pool[nhood.M-1].dist;
            }
            #pragma omp parallel for
            for (unsigned n = 0; n < N; ++n) {
                auto &nhood = nhoods[n];
                auto &nn_new = nhood.nn_new;
                auto &nn_old = nhood.nn_old;
                for (unsigned l = 0; l < nhood.M; ++l) {
					
                    auto &nn = nhood.pool[l];
                    auto &nhood_o = nhoods[nn.id];  // nhood on the other side of the edge
                    if (nn.flag) {
                        nn_new.push_back(nn.id);
                        if (nn.dist > nhood_o.radiusM) {
                            LockGuard guard(nhood_o.lock);
                            nhood_o.rnn_new.push_back(n);
                        }
                        nn.flag = false;
                    }
                    else {
                        nn_old.push_back(nn.id);
                        if (nn.dist > nhood_o.radiusM) {
                            LockGuard guard(nhood_o.lock);
                            nhood_o.rnn_old.push_back(n);
                        }
                    }
                }
            }
            for (unsigned i = 0; i < N; ++i) {
                auto &nn_new = nhoods[i].nn_new;
                auto &nn_old = nhoods[i].nn_old;
                auto &rnn_new = nhoods[i].rnn_new;
                auto &rnn_old = nhoods[i].rnn_old;
                if (params.R && (rnn_new.size() > params.R)) {
                    random_shuffle(rnn_new.begin(), rnn_new.end());
                    rnn_new.resize(params.R);
                }
                nn_new.insert(nn_new.end(), rnn_new.begin(), rnn_new.end());
                if (params.R && (rnn_old.size() > params.R)) {
                    random_shuffle(rnn_old.begin(), rnn_old.end());
                    rnn_old.resize(params.R);
                }
                nn_old.insert(nn_old.end(), rnn_old.begin(), rnn_old.end());
            }
        }

public:
      KGraphConstructor (IndexOracle const &o, IndexParams const &p, IndexInfo *r, char const *path)
            : oracle(o), params(p), pinfo(r), nhoods(o.size()), n_comps(0)
        {
            boost::timer::cpu_timer timer;
            //params.check();
            unsigned N = oracle.size(); // size of dataset
            if (N <= params.K) throw runtime_error("K larger than dataset size");

            vector<Control> controls;  //struct control {unsigned id;Neighbors neighbors;}
            if (verbosity > 0) cerr << "Generating control..." << endl;
            GenerateControl(oracle, params.controls, params.K, &controls); //controls data points are sampled, and then calculate the true knn 
            if (verbosity > 0) cerr << "Initializing..." << endl;
            // initialize nhoods
            init();

            // iterate until converge
            float total = N * float(N - 1) / 2;
            IndexInfo info;
            info.stop_condition = IndexInfo::ITERATION;
            info.recall = 0;
            info.accuracy = numeric_limits<float>::max();
            info.cost = 0;
            info.iterations = 0;
            info.delta = 1.0;

            for (unsigned it = 0; (params.iterations <= 0) || (it < params.iterations); ++it) {
                ++info.iterations;
                join();
                {
                    info.cost = n_comps / total;
                    accumulator_set<float, stats<tag::mean>> one_exact;
                    accumulator_set<float, stats<tag::mean>> one_approx;
                    accumulator_set<float, stats<tag::mean>> one_recall;
                    accumulator_set<float, stats<tag::mean>> recall;
                    accumulator_set<float, stats<tag::mean>> accuracy;
                    accumulator_set<float, stats<tag::mean>> M;
                    accumulator_set<float, stats<tag::mean>> delta;
                    for (auto const &nhood: nhoods) {
                        M(nhood.M);
                        delta(EvaluateDelta(nhood.pool, params.K));
                    }
                    for (auto const &c: controls) {
                        one_approx(nhoods[c.id].pool[0].dist);
                        one_exact(c.neighbors[0].dist);
                        one_recall(EvaluateOneRecall(nhoods[c.id].pool, c.neighbors));
                        recall(EvaluateRecall(nhoods[c.id].pool, c.neighbors));
                        accuracy(EvaluateAccuracy(nhoods[c.id].pool, c.neighbors));
                    }
                    info.delta = mean(delta);
                    info.recall = mean(recall);
                    info.accuracy = mean(accuracy);
                    info.M = mean(M);
                    auto times = timer.elapsed();
                    if (verbosity > 0) {

                        cerr << "iteration: " << info.iterations
                             << " recall: " << info.recall
                             << " accuracy: " << info.accuracy
                             << " cost: " << info.cost
                             << " M: " << info.M
                             << " delta: " << info.delta
                             << " time: " << times.wall / 1e9
                             << " one-recall: " << mean(one_recall)
                             << " one-ratio: " << mean(one_approx) / mean(one_exact)
                             << endl;
                    }
                }
                if (info.delta <= params.delta) {
                    info.stop_condition = IndexInfo::DELTA;
                    break;
                }
                if (info.recall >= params.recall) {
                    info.stop_condition = IndexInfo::RECALL;
                    break;
                }
                update();
            }

            ofstream os(path, ios::binary);
            os.write(KGRAPH_MAGIC, KGRAPH_MAGIC_SIZE);
            os.write(reinterpret_cast<char const *>(&VERSION_MAJOR), sizeof(VERSION_MAJOR));
            os.write(reinterpret_cast<char const *>(&VERSION_MINOR), sizeof(VERSION_MINOR));
            os.write(reinterpret_cast<char const *>(&N), sizeof(N));
            
            
            for (unsigned n = 0; n < N; ++n) {
              auto const &pool = nhoods[n].pool;
              unsigned K = params.L;
              os.write(reinterpret_cast<char const *>(&K), sizeof(K));
              os.write(reinterpret_cast<char const *>(&K), sizeof(K));
              for (unsigned j = 0; j < K; ++j){
                os.write(reinterpret_cast<char const *>(&pool[j].id), sizeof(pool[j].id));
              }
            }
            /* // deleted by yifang
            M.resize(N);
            graph.resize(N);
            if (params.prune > 2) throw runtime_error("prune level not supported.");
            for (unsigned n = 0; n < N; ++n) {
                auto &knn = graph[n];
                M[n] = nhoods[n].M;
                auto const &pool = nhoods[n].pool;

                unsigned K = params.L;
                knn.resize(K);
                for (unsigned k = 0; k < K; ++k) {
                    knn[k].id = pool[k].id;
                    knn[k].dist = pool[k].dist;
                }
                nhoods[n].pool.clear();
                }
            if (params.prune) {
                prune(o, params.prune);
            }
            if (pinfo) {
                *pinfo = info;
                }*/
        }

    };

void KGraphImpl::build (IndexOracle const &oracle, IndexParams const &param, char const *path, IndexInfo *info) {
  KGraphConstructor con(oracle, param, info, path);
  /* //deleted by yifang
  M.swap(con.M);
  graph.swap(con.graph);
  */
}

    KGraph *KGraph::create () {
        return new KGraphImpl;
    }

    char const* KGraph::version () {
        return kgraph_version;
    }
}

