#ifndef HAMMINGDIST_H_
#define HAMMINGDIST_H_

#include <vector>
#include <cmath>
#include <cstring>
#include <malloc.h>
#include <getopt.h>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include "hierarchical_clustering_index.h"

using namespace std;
using std::string;
using std::vector;
using std::cout;
using std::ifstream;
using std::ios;
using std::endl;

struct B_Neighbor{
	int id;
	int dist;
	B_Neighbor()
	{
		id=0;
		dist=-1;
	}
	int get_id(){return id;}
};

struct E_Neighbor{
	int id;
	float dist;
	E_Neighbor()
	{
		id=0;
		dist=-1;
	}
};

float get_distance(vector<float> point, vector<float> query)
{
	int dim = query.size();
	float dist=0.0;
	for(int i=0; i<dim; i++)
	{
		dist += pow(point[i]-query[i],2);
	}
	return dist;
}

void hammingDist(vector<vector<unsigned> >* B2,vector<vector<unsigned> >* B1, vector<vector<int> >* Dist)
{
	int n = B1->size();
	int nq = B2->size();
	int cbits = B2->at(0).size();
	for(int i=0; i<nq; i++)
	{
		vector<int> d_;
		for(int j=0; j<n; j++)
		{
			int tmp = 0;
			for(int t=0; t<cbits; t++)
			{
				unsigned y = B1->at(j)[t] ^ B2->at(i)[t];
				tmp += char_bit_cnt[y];
			}
			d_.push_back(tmp);
		}
		Dist->push_back(d_);
	}
}

void UpdatePriorityList_Binary (B_Neighbor *BList, B_Neighbor tmp, int len) {
	// find the location to insert
	int j;
    int i = len;
    while (i > 0) {
        j = i - 1;
        if (BList[j].dist <= tmp.dist && BList[j].dist!=-1) break;
        i = j;
    }
	if(i < len)
	{
    	j = len-1;
    	while (j > i) {
        	BList[j] = BList[j-1];
        	--j;
    	}
    	BList[i] = tmp;
	}
}

void hammingDist(vector<vector<unsigned> >* B2,vector<vector<unsigned> >* B1, vector<vector<int> >* Dist,int nn)
{
	int len = nn;
	int n = B1->size();
	int nq = B2->size();
	int cbits = B2->at(0).size();

	for(int i=0; i<nq; i++)//nq
	{
		vector<B_Neighbor> BNN(len);
		for(int j=0; j<n; j++)
		{
			int tmp = 0;
			for(int t=0; t<cbits; t++)
			{
				unsigned y = B1->at(j)[t] ^ B2->at(i)[t];
				tmp += char_bit_cnt[y];
			}

			B_Neighbor b_;
			b_.id=j;
			b_.dist=tmp;

			UpdatePriorityList_Binary ( &BNN[0], b_, len);
		}
		vector<int> d_;
		for(int j=0;j<len;j++)
		{
			d_.push_back(BNN.at(j).id);
		}
		Dist->push_back(d_);
	}
}

void hammingDist_ByRange(vector<vector<unsigned> >* B2,vector<vector<unsigned> >* B1, vector<vector<int> >* IDs,int radis)
{
	int n = B1->size();
	int nq = B2->size();
	int cbits = B2->at(0).size();

	for(int i=0; i<nq; i++)//nq
	{
		vector<int> BNN;
		for(int j=0; j<n; j++)
		{
			int tmp = 0;
			for(int t=0; t<cbits; t++)
			{
				unsigned y = B1->at(j)[t] ^ B2->at(i)[t];
				tmp += char_bit_cnt[y];
			}
			if(tmp<=radis)
				BNN.push_back(j);
		}
		IDs->push_back(BNN);
	}
}

void UpdatePriorityList_Euclidean (E_Neighbor *BList, E_Neighbor tmp, int len) {
	// find the location to insert
	int j;
    int i = len;
    while (i > 0) {
        j = i - 1;
        if (BList[j].dist <= tmp.dist && BList[j].dist!=-1) break;
        i = j;
    }
	if(i < len)
	{
    	j = len-1;
    	while (j > i) {
        	BList[j] = BList[j-1];
        	--j;
    	}
    	BList[i] = tmp;
	}
}

vector<vector<int> > Search(vector<vector<float> >* points, vector<vector<float> >* query, vector<vector<int> >* IDs, int K)
{
	int nq=query->size();
	vector<vector<int> > results;
	for(int i=0; i<nq; i++)
	{
		vector<int> result;
		vector<E_Neighbor> ENN(K);
		int len = IDs->at(i).size();
		vector<int> ids_ = IDs->at(i);
		for(int j=0; j<len; j++)
		{
			float dist = get_distance(query->at(i),points->at(ids_.at(j)));
			E_Neighbor e_;
			e_.id=ids_.at(j);
			e_.dist=dist;
			UpdatePriorityList_Euclidean (&ENN[0], e_, K);
		}
		for(int j=0;j<K;j++)
		{
			result.push_back(ENN[j].id);
		}
		results.push_back(result);
	}
	return results;
}


/*
vector<vector<E_Neighbor> > Search(vector<vector<float> >* points, vector<vector<float> >* query, vector<vector<int> > IDs, int K)
{
	int nq=query->size();
	vector<vector<E_Neighbor> > results;
	for(int i=0; i<nq; i++)
	{
		vector<E_Neighbor> ENN(K);
		int len = IDs.at(i).size();
		vector<int> ids_ = IDs.at(i);
		for(int j=0; j<len; j++)
		{
			float dist = get_distance(query->at(i),points->at(ids_.at(j)));
			E_Neighbor e_;
			e_.id=ids_.at(j);
			e_.dist=dist;
			UpdatePriorityList_Euclidean (&ENN[0], e_, K);
		}
		//for(int j=0;j<K;j++)
		//{
			//cout<<ENN[j].id<<" "<<ENN[j].dist<<endl;
		//}
		results.push_back(ENN);
	}
	return results;
}
*/

#endif /* HAMMINGDIST_H_ */
