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

#ifndef SGH_H_
#define SGH_H_

class SGH
{
	public:
	int nbits;
	float delta;
	int m;
	vector<vector<float> > center;
	vector<float> bias;
	vector<vector<float> > Wx;
    
	void ReadSGHParams(string filename, int m_, int dim)
	{
		m = m_;
		FILE* fp=fopen(filename.c_str(),"r");
		//read nbits
		fscanf(fp,"%d",&nbits);

		// read delta
		fscanf(fp,"%f",&delta);
		//read center
		for (int i=0;i<m;i++)
		{
			vector<float> tmp;
			tmp.resize(dim);
			for(int d = 0; d < dim; ++d) 
			{
		   		fscanf(fp,"%f",&tmp[d]);
			}
			center.push_back(tmp);
		}
	
		//read bias
		bias.resize(m);
		for (int i=0; i<m; i++)
		{
			fscanf(fp,"%f",&bias[i]);
		}
		
		//read Wx
		for (int i=0; i<m; i++)
		{
			vector<float> tmp;
			tmp.resize(nbits);
			for(int d = 0; d < nbits; ++d)
			{
				fscanf(fp,"%f",&tmp[d]);
			}
			Wx.push_back(tmp);
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

	void compressSGH(vector<vector<float> >* X, vector<vector<unsigned> >* B)
	{
		int nq = X->size();
		int dim = X->at(0).size();

		vector<vector<float> > KX;
		KX.resize(nq);
		vector<vector<float> > Y;
		for(int i=0; i<nq; i++)
		{
			KX.at(i).resize(m);
			for(int j=0; j<m; j++)
			{
				float tmp = 0.0;
				for (int t=0;t<dim;t++)
					tmp += pow(X->at(i)[t] - center[j][t],2);
				KX[i][j] = exp((-1)*tmp/(2*delta))-bias[j];
			}

			vector<float> Y_;
			for(int t=0; t<nbits; t++)
			{
				float tmp = 0.0;
				for(int j=0; j<m; j++)
				{
					tmp+= KX[i][j]*Wx[j][t];
				}
				Y_.push_back(tmp);
			}
			Y.push_back(Y_);
		}
		compactBits(Y,B);
	}
};



#endif /* SGH_H_ */
