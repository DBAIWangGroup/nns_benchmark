#include <algorithm>
#include <cstring>
#include <iostream>
#include <limits>
#include <set>
#include <vector>


struct DistanceIndex
{
    DistanceIndex(int dist, int index) :
        dist_(dist), index_(index)
    {
    }
    bool operator<(const DistanceIndex& dist_index) const
    {
        return (dist_ < dist_index.dist_) || ((dist_ == dist_index.dist_) && index_ < dist_index.index_);
    }
    int dist_;
    int index_;
};


class ResultSet
{
public:
	//typedef DistanceIndex<int> DistIndex;

	ResultSet(size_t capacity_) :
        capacity_(capacity_)
    {
		// reserving capacity to prevent memory re-allocations
		dist_index_.reserve(capacity_);
    	clear();
    }

    ~ResultSet()
    {
    }

    /**
     * Clears the result set
     */
    void clear()
    {
        dist_index_.clear();
        worst_dist_ = std::numeric_limits<int>::max();
        is_full_ = false;
    }

    /**
     *
     * @return Number of elements in the result set
     */
    size_t size() const
    {
        return dist_index_.size();
    }

    /**
     * Radius search result set always reports full
     * @return
     */
    bool full()
    {
        return is_full_;
    }

    /**
     * Add another point to result set
     * @param dist distance to point
     * @param index index of point
     * Pre-conditions: capacity_>0
     */
    void addPoint(int dist, int index)
    {
    	if (dist>=worst_dist_) return;

    	if (dist_index_.size()==capacity_) {
    		// if result set if filled to capacity, remove farthest element
    		std::pop_heap(dist_index_.begin(), dist_index_.end());
        	dist_index_.pop_back();
    	}

    	// add new element
    	dist_index_.push_back(DistanceIndex(dist,index));
    	if (is_full_) { // when is_full_==true, we have a heap
    		std::push_heap(dist_index_.begin(), dist_index_.end());
    	}

    	if (dist_index_.size()==capacity_) {
    		if (!is_full_) {
    			std::make_heap(dist_index_.begin(), dist_index_.end());
            	is_full_ = true;
    		}
    		// we replaced the farthest element, update worst distance
        	worst_dist_ = dist_index_[0].dist_;
        }
    }

    /**
     * Copy indices and distances to output buffers
     * @param indices
     * @param dists
     * @param num_elements Number of elements to copy
     * @param sorted Indicates if results should be sorted
     */
    void copy(vector<int>* indices, vector<int>* dists, size_t num_elements)
    {
    		// std::sort_heap(dist_index_.begin(), dist_index_.end());
    		// sort seems faster here, even though dist_index_ is a heap
    	//std::sort(dist_index_.begin(), dist_index_.end());

    	size_t n = std::min(dist_index_.size(), num_elements);
    	for (size_t i=0; i<n; ++i) {
    		indices->at(i) = dist_index_[i].index_;
    		dists->at(i) = dist_index_[i].dist_;
    	}
    }

    int worstDist() const
    {
    	return worst_dist_;
    }

private:
    size_t capacity_;
    int worst_dist_;
    std::vector<DistanceIndex> dist_index_;
    bool is_full_;
};
