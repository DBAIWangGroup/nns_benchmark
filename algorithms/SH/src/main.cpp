#include "statisticsmodule.h"
#include "SHindex.h"
#include "SHgeneral.h"
#include "SHselection.h"
#include "data.h"
#include <iostream>
#include <cstring>

#define no_argument 0
#define required_argument 1
#define optional_argument 2

SHGeneral shg;
SHSelection shs;
SHIndex shi;
StatisticsModule st;

float data[datasize][D];
IO io;
Knn knn;


//int main()
//{
    //knn.getBaseR();
    //R=120;
    /*
    io.diskread_float("ran/ran_base.dat", data[0], datasize*D);
    shi.query_load("ran/ran_query.dat");
    cout<<"data read from disk"<<endl;

    //shi.GetDataQueryDistance();

    //io.diskread_float("dq_distance.dat",a[0],2000*100);
    double start_=clock();
    shi.linear_search();
    double finish_=clock();
    cout<<"time"<<(finish_ - start_)*1.0/ CLOCKS_PER_SEC<<endl;
    io.diskwrite_int("ran/ran_groundtruth.dat",shi.queryresult[0],querysize*K);
    //shi.result_write("sift/SIFT1M result.dat");
    //st.stat_output("query.dat","groundtruth.dat","sift/SIFT1M result.dat","sift/SIFT1M statistics.txt",1);

    cout<<"program finished"<<endl;
    int forcin; cin>>forcin;*/
	//return 0;
//}


int main()
{
    
    char data_name[20]="audio";
    //char params[10];
    //sprintf(params,"%.1fc",c);
    char data_path[100];
    sprintf(data_path,"../../../data/%s_base.lshkit",data_name);
    char query_path[100];
    sprintf(query_path,"../../../data/%s_query.lshkit",data_name);

    
    char index_path[100];
    sprintf(index_path,"../indices/%s_%.1fc_index",data_name,c);

    //sprintf(index_path,"indices_randomquery/%s_%.1fc_index",data_name,c);
    char result_path[100];
    sprintf(result_path,"../result/%s_%d_%.1fc.txt",data_name,K,c);
    //sprintf(result_path,"/home/yzhang4/HD/home/liwen/Downloads/Dataset/%s/exp_fig/result/DSH/k=%d/%.1fc_randomquery.txt",file_path,K,c);

    char gnd_path[100];
    sprintf(gnd_path,"../../../data/%s_groundtruth.lshkit", data_name);
    //sprintf(gnd_path,"/home/yzhang4/HD/home/liwen/Downloads/Dataset/%s/gnd/%s_groundtruth_randomquery.dat",file_path,data_name);
    char decision_path[50]="../result/decision.dat";
    char query_result_path[50]="../result/result.dat";

    io.diskread_float(data_path, data[0], datasize*D);
    cerr <<"data read from disk ........"<<endl;
//*

    int start_=clock();
	shg.init();
	shs.radius_selection(decision_path);
	//index
    shi.index_construct(decision_path);
    int finish_=clock();
    float indextime=(finish_-start_)*1.0/CLOCKS_PER_SEC;

    //ofstream fout;
    //fout.open(result_path,ios::app);
    cout<<indextime<<" #index construction time "<<endl;

    shi.index_write(index_path);

    cerr << "start the query processing ..." << endl;


    shi.index_load(index_path);
    //fout.close();
    //int Ltemp[16] = {5,6,7,8,9,10,11,12,13,14,15,16,17,18,20,22,23,26,30,35,40,45,50};

    //search
    int MaxChecked[20]={50,100,300,500,700,1000,2000,3000,5000,7000,8000,10000,12000,15000,20000,30000,50000,60000,70000,100000};
    int i=L;
    //for(int i = 1; i <= 20; i++)
    {
    shi.query_load(query_path);
    cerr<<"load query .. "<<endl;
    for(int j=0;j<20;j++)
    {
        st.begin();
        {
            shi.query_execute(i,MaxChecked[j]);
        }
        cerr<<"query success";
        //shi.query_execute(i);
        st.finish();
        shi.result_write(query_result_path);

        //fout.open(result_path, ios::out | ios::app);
        //fout.close();

        st.stat_output(query_path,gnd_path,query_result_path,result_path,MaxChecked[j]);
      }
    }    

    cerr <<"program finished ....... "<<endl;
    //int forcin; cin>>forcin;
	return 0;
}


