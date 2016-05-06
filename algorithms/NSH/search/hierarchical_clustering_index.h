#include <algorithm>
#include <string>
#include <cmath>
#include <vector>
#include <iostream>
#include "center_chooser.h"
//#include "char_bit_cnt.h"
#include "dynamic_bitset.h"
#include "heap.h"
#include "result_set.h"

using namespace std;
using std::string;
using std::vector;
using std::cout;
using std::ifstream;
using std::ios;
using std::endl;

//typedef unsigned ElementType;
//typedef int DistanceType;

class HierarchicalClusteringIndex
{
	private:

	int branching_;
    int trees_;
    int leaf_max_size_;

	size_t size_;
 	size_t veclen_;
	size_t size_at_build_;
	size_t last_id_;

	vector<size_t> ids_;
	vector<vector<unsigned>* > points_;
	CenterChooser Centers_;
	
	//DynamicBitset removed_points_;

	void setDataset(vector<vector<unsigned> >& dataset)
    {
    	size_ = dataset.size();

		points_.resize(size_);
		for (size_t i=0;i<size_;i++)
		{
			points_.at(i) = &dataset.at(i);
		}
		//cout<<"points00: "<<points_.at(0)->at(0)<<endl;
    }
	public:

	void set_params(vector<vector<unsigned> >& inputData, int b=32,int t=4,int leaf=100)
    {
        branching_ = b;
        trees_ = t;
        leaf_max_size_ = leaf;

		setDataset(inputData);

        Centers_.setDataset(inputData);
		DynamicBitset checked(size_);
    }


    /**
     * Builds the index
     */
    void buildIndex()
    {
        if (branching_<2) {
            cerr<<"Branching factor must be at least 2"<<endl;
			return;
        }
        tree_roots_.resize(trees_);
        vector<int> indices(size_);
        for (int i=0; i<trees_; ++i) {
            for (size_t j=0; j<size_; ++j) {
                indices[j] = j;
            }
            tree_roots_[i] = new Node();
            //cout<< points_.at(0)->at(0)<<" "<<points_.at(1)->at(0)<<" "<< get_distance(0,1)<<endl;
            computeClustering(tree_roots_[i], &indices[0], size_);
        }
    }

	struct PointInfo
    {
    	
    	size_t index;   /** Point index */
    	
    	vector<unsigned>* point;  /** Point data */
    };



	struct Node
    {
        
    	vector<unsigned>* pivot;  /** The cluster center  */

    	size_t pivot_index;
        
        vector<Node*> childs;        /**   Child nodes (only for non-terminal nodes)  */
        
        vector<PointInfo> points;    /**    Node points (only for terminal nodes)   */

        ~Node()
        {
        	for(size_t i=0; i<childs.size(); i++){
        		childs[i]->~Node();
        	}
        };
    };

	typedef Node* NodePtr;
	struct BranchStruct
	{
    	Node* node;           /* Tree node at which search resumes */
    	int mindist;     /* Minimum distance to query for all nodes below. */

    	BranchStruct() {}
    	BranchStruct(Node* aNode, int dist) : node(aNode), mindist(dist)
    	{//cout<<"branch node:"<<aNode->pivot_index <<" "<<node->pivot_index<<endl;
    	}

    	bool operator<(const BranchStruct& rhs) const
    	{
        	return mindist<rhs.mindist;
    	}
	};
///*
	int get_distance(int p1, int p2)
	{
		int dist=0;
		int cbits = points_.at(0)->size();
		for(int t=0; t<cbits; t++)
		{
			unsigned y = points_.at(p1)->at(t) ^ points_.at(p2)->at(t);
			dist += char_bit_cnt[y];
		}
		return dist;
	}

	
	int get_distance(const vector<unsigned>* p1, const vector<unsigned>* p2)
	{
		int dist=0;
		int cbits = p1->size();
		for(int t=0; t<cbits; t++)
		{
			unsigned y = p1->at(t) ^ p2->at(t);
			dist += char_bit_cnt[y];
		}
		return dist;
	}
//*/

	void computeLabels(int* indices, int indices_length,  int* centers, int centers_length, int* labels)
    {
        for (int i=0; i<indices_length; ++i) {
            int dist = get_distance(indices[i],centers[0]);
            labels[i] = 0;
            for (int j=1; j<centers_length; ++j) {
                int new_dist = get_distance(indices[i],centers[j]); //(points_.at(indices[i]),points_.at(centers[j]));
                if (dist>new_dist) {
                    labels[i] = j;
                    dist = new_dist;
                }
            }
        }
    }

	void computeClustering(Node* node, int* indices, int indices_length)
    {
		//cout<<"begin:"<<indices_length<<endl;
        if (indices_length < leaf_max_size_) { // leaf node
            node->points.resize(indices_length);
            for (int i=0;i<indices_length;++i) {
            	node->points[i].index = indices[i];
            	node->points[i].point = points_.at(indices[i]);
            }
            node->childs.clear();
			//cout<<"indices:"<<indices_length<<"  "<< node->points.size()<<endl;
            return;
        }

        std::vector<int> centers(branching_);
        std::vector<int> labels(indices_length);

        int centers_length;
		Centers_.chooseCenters(branching_, indices, indices_length, &centers[0], centers_length);
		//cout<<"choose centers_length :  "<<centers_length<<endl;

        if (centers_length<branching_) {
            node->points.resize(indices_length);
            for (int i=0;i<indices_length;++i) {
            	node->points[i].index = indices[i];
            	node->points[i].point = points_.at(indices[i]);
            }
            node->childs.clear();
			//cout<<" centers: "<<centers_length<<"  "<< node->points.size()<<endl;
            return;
        }

        //  assign points to clusters
        computeLabels(indices, indices_length, &centers[0], centers_length, &labels[0]);

        node->childs.resize(branching_);
        int start = 0;
        int end = start;
        for (int i=0; i<branching_; ++i) {
            for (int j=0; j<indices_length; ++j) {
                if (labels[j]==i) {
                    std::swap(indices[j],indices[end]);
                    std::swap(labels[j],labels[end]);
                    end++;
                }
            }

            node->childs[i] = new Node();
            node->childs[i]->pivot_index = centers[i];
            node->childs[i]->pivot = points_.at(centers[i]);
            node->childs[i]->points.clear();
            computeClustering(node->childs[i],indices+start, end-start);
            start=end;
        }
    }

	typedef BranchStruct BranchSt;

	void findNN(NodePtr node, ResultSet& result, const vector<unsigned>* vec, int& checks,const int maxChecks,
	                Heap<BranchSt>* heap,  DynamicBitset& checked)
	{
	    if (node->childs.empty())
	    {
	        if (checks>=maxChecks) {
	              if (result.full()) return;
	        }

	        for (size_t i=0; i<node->points.size(); ++i)
	        {
	            PointInfo& pointInfo = node->points[i];
	            if (checked.test(pointInfo.index)) continue;
	            int dist = get_distance(pointInfo.point, vec);
	            result.addPoint(dist, pointInfo.index);
	                checked.set(pointInfo.index);
	                ++checks;
	            }
	        }
	        else {
	            int* domain_distances = new int[branching_];
	            int best_index = 0;
	            domain_distances[best_index] = get_distance(vec, node->childs[best_index]->pivot);
	            for (int i=1; i<branching_; ++i) {
	                domain_distances[i] = get_distance(vec, node->childs[i]->pivot);
	                if (domain_distances[i]<domain_distances[best_index]) {
	                    best_index = i;
	                }
	            }
	            for (int i=0; i<branching_; ++i) {
	                if (i!=best_index) {
	                    heap->insert(BranchSt(node->childs[i],domain_distances[i]));
	                }
	            }
	            delete[] domain_distances;
	            findNN(node->childs[best_index],result,vec, checks, maxChecks, heap, checked);
	        }
	    }



	void findNeighborsWithRemoved(ResultSet& result, const vector<unsigned>* vec, const int maxChecks)
    {
        //int maxChecks = searchParams.checks;

        // Priority queue storing intermediate branches in the best-bin-first search
        Heap<BranchSt>* heap = new Heap<BranchSt>(size_);

        DynamicBitset checked(size_);
        int checks = 0;
        for (int i=0; i<trees_; ++i) {
            findNN(tree_roots_[i], result, vec, checks, maxChecks, heap, checked);
        }

        BranchSt branch;
        while (heap->popMin(branch) && (checks<maxChecks || !result.full())) {
            NodePtr node = branch.node;
            findNN(node, result, vec, checks, maxChecks, heap, checked);
        }

        delete heap;
    }




	int knnSearch(const vector<vector<unsigned> >& queries, vector< vector<int> >& indices,	vector< vector<int> >& dists,size_t knn,const int maxChecks)
    {
        if (indices.size() < queries.size() ) indices.resize(queries.size());
		if (dists.size() < queries.size() )   dists.resize(queries.size());

		int count = 0;

		for (int i = 0; i < (int)queries.size(); i++)
		{
				//resultSet.clear();
				ResultSet resultSet(knn);
				findNeighborsWithRemoved(resultSet, &queries.at(i), maxChecks);
				size_t n = std::min(resultSet.size(), knn);
				indices[i].resize(n);
				dists[i].resize(n);
				if (n>0)
				{
					resultSet.copy(&indices.at(i), &dists.at(i), n);
				}
				count += n;
		}
		return count;
    }

	vector<Node*> tree_roots_;
};

