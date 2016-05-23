#include "random.h"
#include "char_bit_cnt.h"

class CenterChooser
{
	vector<vector<unsigned>* > dataset_;
public:

	void setDataset(vector<vector<unsigned> >& dataset)
    {
		size_t size_ = dataset.size();

		dataset_.resize(size_);
		for (size_t i=0;i<size_;i++)
			dataset_.at(i) = &dataset.at(i);

    }
///*
	int get_distance(int p1, int p2)
	{
		int dist=0;
		int cbits = dataset_.at(0)->size();
		for(int t=0; t<cbits; t++)
		{
			unsigned y = dataset_.at(p1)->at(t) ^ dataset_.at(p2)->at(t);
			dist += char_bit_cnt[y];
		}
		return dist;
	} //*/

	void chooseCenters(int k, int* indices, int indices_length, int* centers, int& centers_length)
	{
		UniqueRandom r(indices_length);

        int index;
        for (index=0; index<k; ++index) {
            bool duplicate = true;
            int rnd;
            while (duplicate) {
                duplicate = false;
                rnd = r.next();
                if (rnd<0) {
                    centers_length = index;
                    return;
                }

                centers[index] = indices[rnd];

                for (int j=0; j<index; ++j) {
                    int sq = get_distance(centers[index], centers[j]);//(dataset_.at(centers[index]), dataset_.at(centers[j]));
                    if (sq <1e-16 ) {//<1e-16
                        duplicate = true;
                    }
                }
            }
        }
        centers_length = index;
	}
};
