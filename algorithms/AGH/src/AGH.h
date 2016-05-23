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

#ifndef AGH_H_
#define AGH_H_

class AGH
{
	public:
	int nbits;
	int m;
	int s;
	float sigma;
	vector<vector<float> > anchor;
	vector<vector<float> > W;
	vector<vector<float> > Thres;
    
	void ReadAGHParams(string filename, int m_,int dim, int isTwo)
	{
		m = m_;
		FILE* fp=fopen(filename.c_str(),"r");
		//read nbits
		fscanf(fp,"%d",&nbits);

		// read delta
		fscanf(fp,"%f",&sigma);
		
		//read anchor
		for (int i=0;i<m;i++)
		{
			vector<float> tmp;
			tmp.resize(dim);
			for(int d = 0; d < dim; ++d) 
			{
		   		fscanf(fp,"%f",&tmp[d]);
			}
			anchor.push_back(tmp);
		}

		//read W
		if(isTwo==1)
		{
			for (int i=0; i<m; i++)
			{
				vector<float> tmp;
				tmp.resize(nbits);
				for(int d = 0; d < nbits; ++d)
				{
					fscanf(fp,"%f",&tmp[d]);
				}
				W.push_back(tmp);
			}
		}
		else
		{
			for (int i=0; i<m; i++)
			{
				vector<float> tmp;
				tmp.resize(nbits/2);
				for(int d = 0; d < nbits/2; ++d)
				{
					fscanf(fp,"%f",&tmp[d]);
				}
				W.push_back(tmp);
			}
		}
		
		//read Thres
		if(isTwo==2)
		{
			for (int i=0; i<2; i++)
			{
				vector<float> tmp;
				tmp.resize(nbits/2);
				for(int d = 0; d < nbits/2; ++d)
				{
					fscanf(fp,"%f",&tmp[d]);
				}
				Thres.push_back(tmp);
			}
		}
	}
	
	void compactBits(vector<vector<float> > U,vector<vector<unsigned> >* B)
	{
		//U n*nbits
		int n = U.size();
		int nbits = U.at(0).size();
		for(int i=0;i<n;i++)
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
	
	struct L_Neighbor{
		int pos;
		float val;
		L_Neighbor()
		{
			pos=0;
			val=-1;
		}
	};

	void UpdatePriorityList(L_Neighbor *BList, L_Neighbor tmp) {
	// find the location to insert
	int j;
    int i = s;
    while (i > 0) {
        j = i - 1;
        if (BList[j].val <= tmp.val && BList[j].val!=-1) break;
        i = j;
    }
	if(i < s)
	{
    	j = s-1;
    	while (j > i) {
        	BList[j] = BList[j-1];
        	--j;
    	}
    	BList[i] = tmp;
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

	void compressAGH_OneLayer(vector<vector<float> >* X, vector<vector<unsigned> >* B,int s_)
	{
		s = s_;
		int nq = X->size();
		int dim = X->at(0).size();
		
		vector<vector<L_Neighbor> > Y;
		
		vector<vector<float> > Z;
		for(int i=0; i<nq; i++)
		{
			vector<float> Z_(m,0);
			Z.push_back(Z_);

			vector<L_Neighbor> LNN(s);
			for(int j=0; j<m; j++)
			{
				float dist = get_distance(X->at(i),anchor[j]);
				L_Neighbor L_;
				L_.pos=j;
				L_.val=dist;
				UpdatePriorityList(&LNN[0],L_);
			}
			float sum = 0.0;
			for(int j=0;j<s;j++)
			{
				L_Neighbor L_= LNN[j];
				float val=exp((-1)* L_.val/pow(sigma,2));
				L_.val=val;
				LNN[j]=L_;
				sum += val;
			}
			sum = 1.0/sum;
			for(int j=0; j<s; j++)
			{
				L_Neighbor L_=LNN[j];
				float val= L_.val*sum;
				L_.val=val;
				LNN[j] = L_;
			}

			Y.push_back(LNN);		
		}
		for(int i=0; i<nq; i++)
		{
			for(int j=0; j<s; j++)
			{
				int tep = Y[i][j].pos*nq+i;
				Z[tep%nq][ceil(tep/nq)] = Y[i][j].val;
			}
		}
		vector<vector<float> > U;
		for(int i=0; i<nq; i++)
		{
			vector<float> U_;
			for(int j=0; j<nbits; j++)
			{
				float tmp=0.0;
				for(int t=0; t<m; t++)
				{
					tmp += Z[i][t]*W[t][j];
				}
				U_.push_back(tmp);
			}
			U.push_back(U_);
		}
		compactBits(U,B);
	}

	void compressAGH_TwoLayer(vector<vector<float> >* X, vector<vector<unsigned> >* B,int s_)
	{
		s = s_;
		int nq = X->size();
		
		vector<vector<L_Neighbor> > T;
		
		vector<vector<float> > Z;
		for(int i=0; i<nq; i++)
		{
			vector<float> Z_(m,0);
			Z.push_back(Z_);

			vector<L_Neighbor> LNN(s);
			for(int j=0; j<m; j++)
			{
				float dist = get_distance(X->at(i),anchor[j]);
				L_Neighbor L_;
				L_.pos=j;
				L_.val=dist;
				UpdatePriorityList(&LNN[0],L_);
			}
			float sum = 0.0;
			for(int j=0;j<s;j++)
			{
				L_Neighbor L_= LNN[j];
				float val=exp((-1)* L_.val/pow(sigma,2));
				L_.val=val;
				LNN[j]=L_;
				sum += val;
			}
			sum = 1.0/sum;
			for(int j=0; j<s; j++)
			{
				L_Neighbor L_=LNN[j];
				float val= L_.val*sum;
				L_.val=val;
				LNN[j] = L_;
			}

			T.push_back(LNN);		
		}
		for(int i=0; i<nq; i++)
		{
			for(int j=0; j<s; j++)
			{
				int tep = T[i][j].pos*nq+i;
				Z[tep%nq][ceil(tep/nq)] = T[i][j].val;
			}
		}
		vector<vector<float> > Y;
		for(int i=0; i<nq; i++)//nq
		{
			vector<float> U_;
			U_.resize(nbits);
			for(int j=0; j<nbits/2; j++)
			{
				float tmp=0.0;
				for(int t=0; t<m; t++)
				{
					tmp += Z[i][t]*W[t][j];
				}
				U_[j]=tmp;
				if(tmp > 0)
				{
					U_[nbits/2+j]=tmp-Thres[0][j];
				}
				else
				{
					U_[nbits/2+j]=-tmp+Thres[1][j];
				};
			}
			Y.push_back(U_);
		}
		compactBits(Y,B);
	}
};

#endif 
