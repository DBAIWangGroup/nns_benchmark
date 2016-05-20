#ifndef __DISTDATA_H
#define	__DISTDATA_H

#ifndef DISTDATA_EUCLIDEAN_
#define DISTDATA_EUCLIDEAN_
#endif

#ifndef DISTDATA_INVALID_
#define DISTDATA_INVALID_ (-1.0F)
#endif

class DistData {

public:
	float* value;
    int length;
public:
    DistData ();
	DistData (float* val, int len);
	DistData (double* val, int len);
	DistData (DistData* vec);
    float distanceTo (DistData* vec);
	int getCoordValues (float* buffer, int capacity);
    ~DistData() ;
	int getLength();
	float* getValue();
};

#endif	/* __DISTDATA_H */

