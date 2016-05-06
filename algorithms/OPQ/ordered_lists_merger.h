/** @file */
// Copyright 2012 Yandex Artem Babenko
#pragma once

#include "data_util.h"
#include "multitable.hpp"

/**
 * \typedef
 *  Typedef for indices of merged list element
 */
typedef vector<int> MergedItemIndices;

/**
 * This class merges any number of ordered lists and yields
 * elements of merged list in Order-increasing order.
 * Initial lists are ordered by Order.
 * This class is used in multilist algorithm to get coordinates of cells in multiindex.
 * Class does not return the whole result list, it just yields by one item.
 * Class also assumes that input lists are the same length
 */
template<class OrderType, class MetaInfo>
class OrderedListsMerger {
 public:
 /**
  *  This constructor inits merger for input lists
  */
  OrderedListsMerger();
 /**
  * This function sets lists to merge
  * @param lists input lists to merge (must be ordered)
  */
  void setLists(const vector<vector<pair<OrderType, MetaInfo> > >& lists);
 /**
  * This function yields indices if next item of merged list.
  * It returns "false" if all items have been already yielded and "true" otherwise
  * @param merged_item_indices result indices
  */
  inline bool GetNextMergedItemIndices(MergedItemIndices* merged_item_indices);
 /**
  * Pointer to input lists
  */
  const vector<vector<pair<OrderType, MetaInfo> > >* lists_ptr;
 /**
  * Getter
  */
  Multitable<char>& GetYieldedItems() {
    return yielded_items_indices_;
  }
 private:
 /**
  *  This function pushes new item into priority queue
  * @param merged_item_indices indices of item to add
  */
  void InsertMergedItemIndicesInHeap(const MergedItemIndices& merged_item_indices);
 /**
  * This function tries to update priority queue after yielding
  * @param merged_item_indices new indices we should try to push in priority queue
  */
  void UpdatePrioirityQueue(MergedItemIndices& merged_item_indices);
 /**
  *  Proirity queue for multilist algorithm 
  */
  multimap<OrderType, MergedItemIndices> heap_;
 /**
  *  Table with "1"-value for yielded items and "0"-value  otherwise
  */
  Multitable<char> yielded_items_indices_;
};

////////////////////      IMPLEMENTATION         //////////////////////////////////////////

template<class OrderType, class MetaInfo>
OrderedListsMerger<OrderType, MetaInfo>::OrderedListsMerger() {
}

template<class OrderType, class MetaInfo>
void OrderedListsMerger<OrderType, MetaInfo>::InsertMergedItemIndicesInHeap(const MergedItemIndices& merged_item_indices) {
  OrderType sum = 0;
  for(int list_index = 0; list_index < lists_ptr->size(); ++list_index) {
    sum += lists_ptr->at(list_index)[merged_item_indices[list_index]].first;
  }
  heap_.insert(std::make_pair(sum, merged_item_indices));
}

template<class OrderType, class MetaInfo>
void OrderedListsMerger<OrderType, MetaInfo>::setLists(const vector<vector<pair<OrderType, MetaInfo> > >& lists) {
  lists_ptr = &lists;
  heap_.clear();
  MergedItemIndices first_item_indices(lists.size());
  for(int list_index = 0; list_index < lists.size(); ++list_index) {
    first_item_indices[list_index] = 0;
  }
  memset(&(yielded_items_indices_.table[0]), 0, yielded_items_indices_.table.size());
  InsertMergedItemIndicesInHeap(first_item_indices);
}

template<class OrderType, class MetaInfo>
void OrderedListsMerger<OrderType, MetaInfo>::UpdatePrioirityQueue(MergedItemIndices& merged_item_indices) {
  for(int list_index = 0; list_index < lists_ptr->size(); ++list_index) {
    if(merged_item_indices[list_index] >= lists_ptr->at(list_index).size()) {
      return;
    }
    int current_index = merged_item_indices[list_index];
    merged_item_indices[list_index] -= 1;
    if(current_index > 0 && !yielded_items_indices_.GetValue(merged_item_indices)) {
      merged_item_indices[list_index] += 1;
      return;
    } else {
      merged_item_indices[list_index] += 1;
    }
  }
  InsertMergedItemIndicesInHeap(merged_item_indices);
}

template<class OrderType, class MetaInfo>
inline bool OrderedListsMerger<OrderType, MetaInfo>::GetNextMergedItemIndices(MergedItemIndices* next_merged_item_indices) {
  if(heap_.empty()) {
    return false;
  }
  *next_merged_item_indices = heap_.begin()->second;
  yielded_items_indices_.SetValue(1, *next_merged_item_indices);
  for(int list_index = 0; list_index < lists_ptr->size(); ++list_index) {
    next_merged_item_indices->at(list_index) += 1;
    UpdatePrioirityQueue(*next_merged_item_indices);
    next_merged_item_indices->at(list_index) -= 1;
  }
  heap_.erase(heap_.begin());
  return true;
}

template class OrderedListsMerger<Distance, PointId>;
template class OrderedListsMerger<Distance, pair<ClusterId, ClusterId> >;