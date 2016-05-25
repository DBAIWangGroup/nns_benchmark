#include "SHselection.h"
#include <iostream>
#include<string.h>
using namespace std;

void SHSelection::radius_selection(string decision_file)
{
	memset(decisionsignal, 0, sizeof(decisionsignal));
	//while(1)
	{
		//if(radius_test(0)==0)
			//shg.R[0]=shg.R[0]*2;
		//else
            {
                //for(int i = 1; i < Alter; i++)shg.R[i] = c * shg.R[i-1];
                //break;
            }

	}
	int sample_index[100];
	shg.familysample(sample_index,datasize, 100);
	float sum=0.0;
	for (int i=0;i<100;i++)
	{
		for (int j=0;j<1;j++)
		{
		    float a=MyVector::dotproduct(D,data[sample_index[i]],shg.familyvector[j]);
            sum += abs(a);
		}

	}
	shg.R[0] = sum/100/3; ///100
	cout<<shg.R[0]<<endl;
	for(int i = 1; i < Alter; i++)shg.R[i] = c * shg.R[i-1];
	for(int i = 0; i < Alter - 1; i++)radius_test(i);
	for(int i = 0; i < datasize; i++)
	{
	    if(decisionsignal[i] == 0)
        {
            decisionsignal[i] = 1;
            shg.decision[i] = Alter - 1;
        }
	}
	cout<<"finished"<<endl;
	shg.decisionavailable = true;
	io.diskwrite_int(decision_file, shg.decision, datasize);
}

int SHSelection::radius_test(int Rrank)
{
     cout<<"SHSelection::radius_test "<<Rrank<<endl;
     memset(hashkeylength, 0, sizeof(hashkeylength));
     for(int k = 0; k < datasize; k++)
     {
            if(k%100000 == 0) cout<<"current hashing data "<<k<<endl;
            shg.tableindex(shg.dataproduct[k],Rrank,shg.datahashresult[k]);
            for(int i = 0; i < L; i++)hashkeylength[i][shg.datahashresult[k][i]%bucketnum]++;
     }
     int sum = 0;
     for(int i = 0; i < datasize; i++)
     {
         int sumcount = 0;
         for(int j = 0; j < L; j++)
         {
             if(hashkeylength[j][shg.datahashresult[i][j]%bucketnum] >= thresholdpoint)sumcount++;
         }
         //cout<<"sumcount: "<<sumcount<<endl;
         //cout<<"thresholdtable: "<<thresholdtable<<endl;
         if(sumcount >= thresholdtable)
         {
             //cout<<"entered"<<endl;
             if(decisionsignal[i] == 0)
             {
                 //cout<<"entered insert"<<endl;
                 sum++;
                 decisionsignal[i] = 1;
                 shg.decision[i] = Rrank;
             }
         }
     }
     cout<<sum<<" points qualified for Round"<<Rrank<<endl;
	return sum;
	  if(sum==0)
		cout<<Rrank<<endl;
     //int forc;cin>>forc;
}
