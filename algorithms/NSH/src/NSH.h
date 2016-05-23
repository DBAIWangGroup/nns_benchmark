/*
 * SH.h
 *
 *  Created on: 13/10/2015
 *      Author: liwen
 */
#include <vector>
#include <cmath>
#include <cstring>
#include <malloc.h>
#include <getopt.h>
#include <cstdlib>
#include <iostream>
#include <fstream>

using namespace std;
using std::string;
using std::vector;
using std::cout;
using std::ifstream;
using std::ios;
using std::endl;

#ifndef NSH_H_
#define NSH_H_

class NSH
{
	public:
	int nbits;
	int m;
	float eps;
	vector<vector<float> > pivots;
	vector<vector<float> > W;

	void ReadNSHParams(string filename, int dim)
	{
		FILE* fp=fopen(filename.c_str(),"r");
		//read nbits
		fscanf(fp,"%d",&nbits);
		fscanf(fp,"%f",&eps);

		//cout<<nbits<<" "<<eps<<endl;
		m=4*nbits;
		//read pivots
		for (int i=0;i<m;i++)
		{
			vector<float> tmp;
			tmp.resize(dim);
			for(int d = 0; d < dim; ++d) 
			{
		   		fscanf(fp,"%f",&tmp[d]);
			}
			fscanf(fp,"\n");
			pivots.push_back(tmp);
		}
		//read W
		for (int i=0; i<m+1; i++)
		{
			vector<float> tmp;
			tmp.resize(nbits);
			for(int d = 0; d < nbits; ++d)
			{
				fscanf(fp,"%f",&tmp[d]);
			}
			fscanf(fp,"\n");
			W.push_back(tmp);
		}
	}

	void compressNSH(vector<vector<float> >* X, vector<vector<float> >* U)
	{
	}
    
	
	/*
	void compactBits(vector<vector<float> > U,vector<vector<unsigned> >* B)
	{
		//U nbits*n
		int nbits = U.size();
		int n = U.at(0).size();
		for(int i=0;i<n;i++)//n
		{
			vector<unsigned> B_;
			unsigned tmp=0;
			for(int j=0;j<=nbits;j++)
			{
				if(j==nbits)
				{
					B_.push_back(tmp);
					break;
				}
				if(j%8==0 && j!=0)
				{
					B_.push_back(tmp);
					tmp=0;
				}
				if(U.at(j)[i]>0)
				{
					unsigned pos=1<<(j%8);
					tmp |= pos;
				}
			}
			B->push_back(B_);
		}
	}*/

	void compactBits(vector<vector<float> > U,vector<vector<unsigned> >* B)
	{
			//U n*nbits
		int n = U.size();
		int nbits = U.at(0).size();
		for(int i=0;i<n;i++)//n
		{
			vector<unsigned> B_;
			unsigned tmp=0;
			for(int j=0;j<=nbits;j++)
			{
				if(j==nbits)
				{
					B_.push_back(tmp);
					break;
				}
				if(j%8==0 && j!=0)
				{
					B_.push_back(tmp);
					tmp=0;
				}
				if(U.at(i)[j]>0)
				{
					unsigned pos=1<<(j%8);
					tmp |= pos;
				}
			}
			B->push_back(B_);
		}
	}
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

	void compressNSH(vector<vector<float> >* X, vector<vector<unsigned> >* B)
	{
		//X nq*dim
		int n = X->size();
		int dim = X->at(0).size();

		vector<vector<float> > QK;
		QK.resize(n);

		for(int i=0; i<n; i++)
		{
			QK.at(i).resize(m+1);
			for(int j=0;j< m; j++)
			{
				float dist = get_distance(X->at(i),pivots.at(j));
				//cout<<"dist:"<<i<<" "<<j<<":"<<dist<<endl;
				QK[i][j] = exp((-1)*dist/pow(eps,2));
			}
			QK[i][m]=1;
		}
		vector<vector<float> > HH;
		HH.resize(n);
		for(int i=0; i<n; i++)
		{
			HH.at(i).resize(nbits);
			for(int j = 0 ; j < nbits; j++)
			{
				float tmp = 0.0;
				for(int t=0;t<m+1;t++)
				{
					tmp += QK[i][t] * W[t][j];
				}
				HH[i][j] = tmp;
			}
		}
		compactBits(HH,B);
	}

	void compressNSH(vector<vector<float> >* X, vector<vector<int> >* U)
	{
	}
};



#endif
