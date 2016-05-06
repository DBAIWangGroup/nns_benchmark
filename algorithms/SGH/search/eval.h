/*
 * eval.h
 *
 *  Created on: 14/10/2015
 *      Author: liwen
 */

#ifndef EVAL_H_
#define EVAL_H_

#include <vector>
#include <cmath>
#include <cstring>
#include <malloc.h>
#include <getopt.h>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <set>

using namespace std;
using std::string;
using std::vector;
using std::cout;
using std::ifstream;
using std::ios;
using std::endl;

float compute_recall(vector<vector<int> >* gnds, vector<vector<int> >* results)
{
	size_t nq = results->size();
	size_t nn = results->at(0).size();
	int avg = 0;
	for (size_t i=0; i<nq; ++i) {
		for (size_t j=0; j<nn; ++j) {
			for (size_t k=0;k<nn;++k) {
				if (results->at(i)[j] == gnds->at(i)[k]) {
					avg ++;
				}
			}
		}
	}
	return float(avg)/(nn*nq);
}

float compute_mean_reciprocal_rank(vector<vector<int> >* gnds, vector<vector<int> >* results)
{
	size_t nq = results->size();
	size_t nn = results->at(0).size();
	float avg = 0.0;
	for (size_t i=0; i<nq; ++i) 
	{
		float rate=0.0;
		for(size_t k=0;k<nn;k++)
		{
			if(gnds->at(i)[0]==results->at(i)[k])
			{
				rate +=1.0/(k+1);
				break;
			}
		}
		avg+=rate;
	}
	return avg/nq;
}

float compute_number_closer(vector<vector<int> >* gnds, vector<vector<int> >* results)
{
	size_t nq = results->size();
	size_t nn = results->at(0).size();
	float avg = 0.0;
	for (size_t i=0; i<nq; ++i)
	{
		float rate=0.0;
		for(size_t j=0;j<nn;j++)
		{
			for(size_t k=0;k<nn;k++)
			{
				if(gnds->at(i)[j]==results->at(i)[k])
				{
					rate +=(float)(j+1)/(k+1);
					break;
				}
			}
		}
		avg+=rate/nn;
	}
	return avg/nq;
}

float compute_relative_distance_error(vector<vector<float> >* points, vector<vector<float> >* query, vector<vector<int> >* gnds, vector<vector<int> >* results)
{
	size_t nq = results->size();
	size_t nn = results->at(0).size();
	float count = 0.0;
	for (size_t i=0; i<nq; ++i) 
	{
		float sum=0.0;

		for (size_t j=0;j<nn;++j)
		{
			float min_distance= get_distance(points->at(gnds->at(i)[j]),query->at(i));
			float test_distance = get_distance(points->at(results->at(i)[j]),query->at(i));
			float d= (test_distance - min_distance )/min_distance;
			if(d >4)
				sum +=4;
			else
				sum +=d;
		}
		count += sum /nn;
	}
	return count/ nq;
}

float compute_mean_average_precision (vector<vector<int> >* gnds, vector<vector<int> >* results)
{
	size_t nq = results->size();
	size_t nn = results->at(0).size();
	float sum = 0;
	for (size_t i = 0; i < nq; ++i) 
	{
		float rate = 0.0;
		int found = 0;

		std::set<int> gnd_row;
  		for(int j = 0; j <nn; ++j) {
      		gnd_row.insert(gnds->at(i)[j]);
  		}

		for(int j=0;j <nn; j++)
		{
      		if(gnd_row.find(results->at(i)[j]) != gnd_row.end()) 
			{
				found ++;
				rate += 1.0 * found/(j+1);
      		}
    	}
		sum += rate/nn;
	}
	return sum / nq;
}

float compute_discounted_culmulative_gain (vector<vector<int> >* gnds, vector<vector<int> >* results) 
{
	size_t nq = results->size();
	size_t nn = results->at(0).size();
	float sum = 0;
	for (size_t i = 0; i < nq; ++i) 
	{
		float rate = 0.0;

		std::set<int> gnd_row;
  		for(size_t j = 0; j <nn; ++j) {
      		gnd_row.insert(gnds->at(i)[j]);
  		}

		for(size_t j=0;j <nn; j++)
		{
      		if(gnd_row.find(results->at(i)[j]) != gnd_row.end()) 
			{
				rate += 1.0 /log2(j+2);
      		}
    	}
		sum += rate;
	}
	return sum / nq;
}

float compute_recall(vector<vector<int> >* gnds, vector<vector<E_Neighbor> >* results, float* recall)
{
	size_t nq = results->size();
	size_t nn = results->at(0).size();
	cout<<"nq:"<<nq<<" "<<nn<<endl;
	int avg = 0;
	for (size_t i=0; i<nq; ++i) {
		for (size_t j=0; j<nn; ++j) {
			for (size_t k=0;k<nn;++k) {
				if (results->at(i)[j].id == gnds->at(i)[k]) {
					avg ++;
				}
			}
		}
	}
	return float(avg)/(nn*nq);
}

float compute_mean_reciprocal_rank(vector<vector<int> >* gnds, vector<vector<E_Neighbor> >* results)
{
	size_t nq = results->size();
	size_t nn = results->at(0).size();
	float avg = 0.0;
	for (size_t i=0; i<nq; ++i) 
	{
		float rate=0.0;
		for(size_t k=0;k<nn;k++)
		{
			if(gnds->at(i)[0]==results->at(i)[k].id)
			{
				rate +=1.0/(k+1);
				break;
			}
		}
		avg+=rate;
	}
	return avg/nq;
}

float compute_number_closer(vector<vector<int> >* gnds, vector<vector<E_Neighbor> >* results)
{
	size_t nq = results->size();
	size_t nn = results->at(0).size();
	float avg = 0.0;
	for (size_t i=0; i<nq; ++i)
	{
		float rate=0.0;
		for(size_t j=0;j<nn;j++)
		{
			for(size_t k=0;k<nn;k++)
			{
				if(gnds->at(i)[j]==results->at(i)[k].id)
				{
					rate +=(float)(j+1)/(k+1);
					break;
				}
			}
		}
		avg+=rate/nn;
	}
	return avg/nq;
}

float compute_relative_distance_error(vector<vector<float> >* points, vector<vector<float> >* query, vector<vector<int> >* gnds, vector<vector<E_Neighbor> >* results)
{
	size_t nq = results->size();
	size_t nn = results->at(0).size();
	float count = 0.0;
	for (size_t i=0; i<nq; ++i) 
	{
		float sum=0.0;

		for (size_t j=0;j<nn;++j)
		{
			float min_distance= get_distance(points->at(gnds->at(i)[j]),query->at(i));
			float test_distance=results->at(i)[j].dist;
			float d= (test_distance - min_distance )/min_distance;
			if(d >4)
				sum +=4;
			else
				sum +=d;
		}
		count += sum /nn;
	}
	return count/ nq;
}


float compute_mean_average_precision (vector<vector<int> >* gnds, vector<vector<E_Neighbor> >* results)
{
	size_t nq = results->size();
	size_t nn = results->at(0).size();
	float sum = 0;
	for (size_t i = 0; i < nq; ++i) 
	{
		float rate = 0.0;
		int found = 0;

		std::set<int> gnd_row;
  		for(int j = 0; j <nn; ++j) {
      		gnd_row.insert(gnds->at(i)[j]);
  		}

		for(int j=0;j <nn; j++)
		{
      		if(gnd_row.find(results->at(i)[j].id) != gnd_row.end()) 
			{
				found ++;
				rate += 1.0 * found/(j+1);
      		}
    	}
		sum += rate/nn;
	}
	return sum / nq;
}

float compute_discounted_culmulative_gain (vector<vector<int> >* gnds, vector<vector<E_Neighbor> >* results) 
{
	size_t nq = results->size();
	size_t nn = results->at(0).size();
	float sum = 0;
	for (size_t i = 0; i < nq; ++i) 
	{
		float rate = 0.0;

		std::set<int> gnd_row;
  		for(size_t j = 0; j <nn; ++j) {
      		gnd_row.insert(gnds->at(i)[j]);
  		}

		for(size_t j=0;j <nn; j++)
		{
      		if(gnd_row.find(results->at(i)[j].id) != gnd_row.end()) 
			{
				rate += 1.0 /log2(j+2);
      		}
    	}
		sum += rate;
	}
	return sum / nq;
}

#endif /* EVAL_H_ */
