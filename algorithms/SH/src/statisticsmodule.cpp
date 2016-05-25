#include "statisticsmodule.h"
#include <iostream>
#include <cmath>
#include <algorithm>
#include <time.h>
#include <set>



void StatisticsModule::stat_output(string query_file,string groundtruth_file,string result_file,string output_file,int Lused)
{
	ofstream fout;
    fout.open(output_file.c_str(), ios::out | ios::app);
    if(!fout)
	{
	    cout << "Cannot open file!" << endl;;
		exit(1);
	}
    io.diskread_float(query_file.c_str(), query[0], querysize*D);
    io.diskread_int(groundtruth_file.c_str(), groundtruth[0], querysize*K);
    io.diskread_int(result_file.c_str(), result[0], querysize*K);
    //cout<<groundtruth[0][1]<<endl;
    //cout<<"load performance";
	float recall = compute_recall();
	//float mrr = compute_mean_reciprocal_rank();
	//float nc = compute_number_closer();
	//float map = compute_mean_average_precision();
	float rde = compute_relative_distance_error();
	//float dcg = compute_discounted_culmulative_gain();

    fout.setf(ios::fixed);//" "<<mrr<<" "<<nc <<" "<<map<<" "<<dcg<<
    fout<<recall << " "<<rde <<" "<<(double)(finish_ - start_)/CLOCKS_PER_SEC/querysize<<" #N_" <<Lused<<" "<<endl;
	fout.close();
}


float StatisticsModule::compute_recall()
{
	int sum=0;
	for(int i=0;i<querysize;i++)
	{
		int re_n=0;
		int gs_n=0;
		while(re_n<K && gs_n<K)
		{
			if(result[i][re_n] == groundtruth[i][gs_n])
			{
				sum++;
				re_n++;
				gs_n++;
			}
			else
			{
				gs_n++;
			}
		}
	}
	return sum*1.0/querysize/K;
}

float StatisticsModule::compute_mean_reciprocal_rank()
{
	float avg = 0.0;
	for (int i=0;i<querysize;i++)
	{
		float rate=0.0;
		for(int j=0;j<K;j++)
		{
			if(groundtruth[i][0] == result[i][j])
			{
				rate +=1.0/(j+1);
				break;
			}
		}
		avg+=rate;
	}
	return avg/querysize;
}

float StatisticsModule::compute_number_closer()
{
	float avg = 0.0;
	for (int i=0;i<querysize;i++)
	{
		float rate=0.0;
		for(int j=0;j<K;j++)
		{
			for(int t=0;t<K;t++)
			{
				if(groundtruth[i][j] == result[i][t])
				{
					rate +=(float)(j+1)/(t+1);
					break;
				}
			}
		}
		avg+=rate/K;
	}
	return avg/querysize;
}

float StatisticsModule::compute_relative_distance_error()
{
	float count = 0.0;
	for (int i=0; i<querysize; i++)//querysize
	{
		float sum=0.0;
		for (int j=0;j<K;j++)//K
		{
			float min_distance= MyVector::distancel2sq(D,data[groundtruth[i][j]],query[i],100);
			//cout<<min_distance<<endl;
			if(min_distance==0)
				continue;
			float test_distance = MyVector::distancel2sq(D,data[result[i][j]],query[i],100);

            if(test_distance<min_distance)
                cout<<i<<" "<<j<<" "<<groundtruth[i][j]<<" "<<min_distance<<" "<<test_distance<<endl;
               // cout<<i<<" "<<j<<endl;
			float d= (test_distance - min_distance )/min_distance;
			if(d >4)
				sum +=4;
			else
				sum +=d;
		}
		count += sum /K;
	}
	return count/ querysize;
}

float StatisticsModule::compute_mean_average_precision ()
{
	float sum = 0;
	for (int i = 0; i < querysize; ++i)
	{
		float rate = 0.0;
		int found = 0;
		std::set<int> gnd_row;
  		for(int j = 0; j <K; ++j) {
      		gnd_row.insert(groundtruth[i][j]);
  		}
		for(int j=0;j <K; j++)
		{
      		if(gnd_row.find(result[i][j]) != gnd_row.end())
			{
				found++;
				rate += 1.0 * found/(j+1);
      		}
    	}
		sum += rate/K;
	}
	return sum / querysize;
}

float StatisticsModule::compute_discounted_culmulative_gain ()
{
	float sum = 0;
	for (int i=0;i<querysize;i++)
	{
		float rate = 0.0;
		std::set<int> gnd_row;
  		for(size_t j = 0; j <K; ++j) {
      		gnd_row.insert(groundtruth[i][j]);
  		}

		for(size_t j=0;j <K; j++)
		{
      		if(gnd_row.find(result[i][j]) != gnd_row.end())
			{
				rate += 1.0 /log2(j+2);
      		}
    	}
		sum += rate;
	}
	return sum / querysize;
}

void StatisticsModule::gen_query_and_groundtruth(string query_file, string groundtruth_file)
{
    sample_query();
    cout<<"query sampled"<<endl;
    io.diskwrite_float(query_file.c_str(), query[0], querysize*D);
    cout<<"query write to disk"<<endl;
    batch_linear_scan();
    cout<<"linear scan finished"<<endl;
    io.diskwrite_int(groundtruth_file.c_str(), groundtruth[0], querysize*K);
    cout<<"groundtruth write to disk"<<endl;
}

//sample querysize query points from data and store in query
void StatisticsModule::sample_query()
{
    for(int i = 0; i < querysize; i++)
    {
        int temp = MyRandom::int_random(datasize);
        for(int j = 0; j < D; j++) query[i][j] = data[temp][j];
    }
    return;
}

//do batch check for all points
void StatisticsModule::batch_linear_scan()
{
    for(int i = 0; i < querysize; i++)
    {
        knn.linear_scan(data, query[i]);
        for(int j = 0; j < K; j++) groundtruth[i][j] = knn.knnlist[j];
        cout<<i<<"   "<<knn.sqrtbound<<endl;
    }
    return;
}/*
void StatisticsModule::stat_output(string query_file,string groundtruth_file,string result_file,string output_file,int Lused)
{
	ofstream fout;
    fout.open(output_file.c_str(), ios::out | ios::app);
    if(!fout)
	{
	    cout << "Cannot open file!" << endl;;
		exit(1);
	}
    io.diskread_float(query_file.c_str(), query[0], querysize*D);
    io.diskread_int(groundtruth_file.c_str(), groundtruth[0], querysize*K);
    io.diskread_int(result_file.c_str(), result[0], querysize*K);

    float sumerrorrate = 0;
    float aveerrorate;
    int sumrecall = 0;
    float testdist[K],gtdist[K];
    for(int i = 0; i < querysize; i++)
    {
		 for(int j = 0; j < K; j++)
		 {
			 testdist[j] = MyVector::distancel2sq(D,data[result[i][j]],query[i],100);
			 //testdist[j] = sqrt(testdist[j]);
			 gtdist[j] = MyVector::distancel2sq(D,data[groundtruth[i][j]],query[i],100);
			 //gtdist[j] = sqrt(gtdist[j]);
		 }
		 sort(testdist,testdist+K);
		 sort(gtdist,gtdist+K);
		 for(int j = 0; j < K; j++)
		 {
		     if(testdist[j] <= gtdist[K-1] + 0.0001)sumrecall++;
			 if(testdist[j] <= gtdist[j] + 0.0001) continue;
			 double temp = (testdist[j] - gtdist[j])/ gtdist[j];
			 if(temp > 4) temp = 4;
			 if(temp < -0.01) cout<<"error perform better than optimal!"<<endl;
			 sumerrorrate += temp;
		 }
	}
	float recall = (float)sumrecall/ (float)(querysize*K);
    aveerrorate = sumerrorrate / (querysize*K);
    //fout<<"recall is "<<recall<<endl;
    //cout<<"recall is "<<recall<<endl;
    //fout<<"error rate is "<<aveerrorate<<endl;
    //cout<<"error rate is "<<aveerrorate<<endl;
    float tempfl = (float)sumcheck;
    tempfl = tempfl*50/((float)(querysize)*datasize);
    //fout<<"check rate is (%) "<<tempfl<<endl;
    //cout<<"check rate is (%) "<<tempfl<<endl;
    fout<<recall << " "<<(double)(finish_ - start_)/CLOCKS_PER_SEC/querysize<<" #N_" <<Lused<<" "<<endl;

    /*fout<<Lused<<" "<<"   "<<(double)(finish_ - start_)/CLOCKS_PER_SEC<<" ";
    fout<<recall<<" "<<aveerrorate<<" "<<sumcheck*1.0/querysize<<endl;
    cout<<Lused<<" "<<(finish_ - start_)*1.0/ CLOCKS_PER_SEC<<" ";
    cout<<recall<<" "<<aveerrorate<<" "<<sumcheck*1.0/querysize<<endl;
	fout.close();
}*/
