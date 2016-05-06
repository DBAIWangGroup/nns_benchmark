/** @file */
// Copyright 2012 Yandex Artem Babenko
#pragma once

#include <vector>

using std::vector;

/**
 *  This class implements interface of multidimensional array with
 *  fast write/read operations. In fact data is stored in a long array.
 *  Global index of particular item in this array is calculated from item coordinates.
 */
template<class T>
struct Multitable {
 /**
  *  This constructor gets width of table for each dimension
  *  @param dimensions array of sizes of table along each dimension
  */
  Multitable(const vector<int>& dimensions = vector<int>());
 /**
  *  This function resize the table to new dimensions
  *  @param dimensions array of sizes of table along each dimension
  */
  void Resize(const vector<int>& dimensions, T value = T());
 /**
  *  This function sets value in one cell
  *  @param value value to set
  *  @param cell_indices coordinates of cell in the table
  */
  void SetValue(T value, const vector<int>& cell_indices);
 /**
  *  This function gets value of one cell
  *  @param cell_indices coordinates of cell in the table
  */
  T GetValue(const vector<int>& cell_indices);
 /**
  *  Actual data as one-dimensional array
  */
  vector<T> table;
 /**
  *  Dimensions of table
  */
  vector<int> dimensions;
 /**
  *  Function for Boost.Serialization
  */
  template<class Archive>
  void serialize(Archive& arc, unsigned int version) {
    arc & table;
    arc & dimensions;
  }
 /**
  *  Function converts cell coordinates to global index in a long array
  *  @param cell_indices coordinates of cell in the table
  */
  int GetCellGlobalIndex(const vector<int>& cell_indices) const;
};

template<class T>
int Multitable<T>::GetCellGlobalIndex(const vector<int>& indices) const {
  if(indices.empty()) {
    throw std::logic_error("Empty indices array!");
  }
  int global_index = 0;
  int subtable_capacity = table.size();
  for(int dimension_index = 0; dimension_index < dimensions.size(); ++dimension_index) {
    subtable_capacity = subtable_capacity / dimensions[dimension_index];
    global_index += subtable_capacity * indices[dimension_index];
  }
  return global_index;
}

template<class T>
void Multitable<T>::Resize(const vector<int>& new_dimensions, T value) {
  int table_size = 1;
  dimensions = new_dimensions;
  for(int dimension_index = 0; dimension_index < new_dimensions.size(); ++dimension_index) {
    table_size *= new_dimensions[dimension_index];
  }
  table.resize(table_size, value);
}

template<class T>
Multitable<T>::Multitable(const vector<int>& dimensions) {
  Resize(dimensions);
}

template<class T>
void Multitable<T>::SetValue(T value, const vector<int>& indices) {
  int global_index = GetCellGlobalIndex(indices);
  table.at(global_index) = value;
}

template<class T>
T Multitable<T>::GetValue(const vector<int>& indices) {
  int global_index = GetCellGlobalIndex(indices);
  return table.at(global_index);
}