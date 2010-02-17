//---------------------------------------------------
// Template class for direct-form II IIR filter 
//---------------------------------------------------

#ifndef KG_classIIR_Direct2

template <class T, int nOrd> class IIR_Direct2_Branch
{
private:
	T un[nOrd+1], ak[nOrd], bk[nOrd+1];
    T Az, Bz; // added 
    short scaling;
public:
    IIR_Direct2_Branch();
    inline T Execute(const T xin);
    inline void setCoefficient(const T am[], const  T bm[], const short _scaling = 0);
    inline const T getAz();
    inline const T getBz();
};


// Constructor
template <class T, int nOrd>
IIR_Direct2_Branch<T, nOrd>::IIR_Direct2_Branch():
Az(0), Bz(0), scaling(0)
{
    for (int m=0; m<=nOrd; m++) un[m] = 0.0;
}


template <class T, int nOrd>
inline const T IIR_Direct2_Branch<T, nOrd>::getAz(){return Az;}

template <class T, int nOrd>
inline const T IIR_Direct2_Branch<T, nOrd>::getBz(){return Bz;}

// set Coefficient
template <class T, int nOrd>
inline void  IIR_Direct2_Branch<T, nOrd>::setCoefficient(const T am[], const T bm[], const short _scaling)
{
    for (int m=0; m < nOrd; m++) ak[m] = am[m];
	for (int m=0; m <= nOrd; m++) bk[m] = bm[m];
	scaling = _scaling;
}

// Execution of direct-form II IIR filter
template <class T, int nOrd>
inline T IIR_Direct2_Branch<T, nOrd>::Execute(const T xin)
{
    T utmp = xin;
    for (int m=0; m<nOrd; m++) {
    	//utmp = utmp + ak[m]*un[m+1];
		utmp = utmp - ak[m]*un[m+1];
	}
	//if (scaling < -1 && scaling > 1) un[0] = (shortfract)(scaling * utmp);
	if (scaling < -1 || scaling > 1) {
	    //un[0] = (float)scaling * (float)utmp;
	    //un[0] = (shortfract)(scaling * (short)utmp);
	    //un[0] = (shortfract)scaling * utmp;
	    un[0] = scaling * (short)utmp;
	    //un[0] = utmp * scaling;
	    //un[0] = (shortfract)((scaling * (32768 * (short)utmp))/32768);
	}
	else {un[0] = utmp;}
	Az = un[0]; 
	Bz = un[nOrd];
    T yn = 0.0;
    for (int m=0; m<=nOrd; m++) {
    	yn = yn + bk[m]*un[m];
	}
	for (int m=nOrd; m>0; m--) un[m] = un[m-1];
	
	return yn;
}

#define KG_classIIR_Direct2
#endif

