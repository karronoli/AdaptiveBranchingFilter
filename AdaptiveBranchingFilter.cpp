#include <stdio.h>
#include "myparam.cpp"
#include "classIIR_Direct2_Branch.cpp"
// É≈^2(k) = É¿x^2(k) + (1 - É¿)É≈^2(k-1)
template <class T> class NormalizePowerFilter
{
private:
    T eta; // É≈
	const T beta; // É¿
public:
	NormalizePowerFilter():eta(0), beta(0.1){}
	//inline T set(const T xyin)
	inline void set(const T xin)
	{
	  //eta = beta*xin*xin+eta*(1.0r-beta); 
	  eta = beta*xin*xin+eta-eta*beta; 
	  //if (-0.001r < eta && eta < 0.001r) {
	  if (-0.001r < eta && 0.001r > eta) {
        //if(eta > 0.0r) {
        if(0.0r < eta) {
	      eta = 0.001r;
        } else {
          eta = -0.001r;
        }
	  }
      //return eta;
	}
	inline const T get(){return eta;}
	

};

// Z
template <class T> class Z
{
private:
    T _Z; // É≈
public:
	Z():_Z(0){}
	inline void set(const T xin){_Z = xin;}
	inline const T get(){return _Z;}
};

/*
lattice test 
tmp1 = alpha*(x - z.get());
y = tmp1 + z.get()
z.set(x + tmp1)
*/

template <class T> class AdaptiveBranchingFilter
{
private:
	T apf_paramA[1], apf_paramB[2]; // All Pass Filter params
	T psi2a_paramA[1], psi2a_paramB[2]; // Éµ2a params (3.3 lower right)
	T variable_stepA[1], variable_stepB[2]; // Éµ2a params (3.3 lower right)
    
	IIR_Direct2_Branch<T, 1>* apf; // 1st order All Pass Filter
	IIR_Direct2_Branch<T, 1>* psi2a; // 1st order Éµ2a
	IIR_Direct2_Branch<T, 1>* variable_step; // 1st order Éµ2a
    //FILE *fp;
	NormalizePowerFilter<T>* npf;
	Z<T>* z;
	T Alpha; // Éø
	T Beta;
    T Myu; // É 
	T Yl; // Low Pass Filter Result
	T Yh; // High Pass Filter Result
	

public:
    AdaptiveBranchingFilter();
	~AdaptiveBranchingFilter();
	inline const T getHigh();
	inline const T getLow();
	inline const T getAlpha();
	inline void updateMyu(const T xin);
	inline const T getMyu();
	inline void Adapt();
	inline void Execute(const T xin);
};


// Constructor
template <class T>
AdaptiveBranchingFilter<T>::AdaptiveBranchingFilter()
:
//Alpha(0.99999), 
//Alpha(-0.99999r), 
Alpha(-0.9999r), 
//Alpha(-0.5),
//Alpha(-0.88),
//Alpha(0.85)
//Alpha(0.1),
//Myu(8E-5)
Beta(-0.9999r),
//Myu(0.00008r)
Myu(0.01r)
{
  //fp = fopen("/tmp/b.txt", "rw");
  apf = new IIR_Direct2_Branch<T, 1>;
  psi2a = new IIR_Direct2_Branch<T, 1>;
  npf = new NormalizePowerFilter<T>;
  variable_step = new IIR_Direct2_Branch<T, 1>;
  z = new Z<T>;
  this->Adapt();
}

template <class T>
inline void AdaptiveBranchingFilter<T>::Adapt()
{
	// (H(z) = (Éø + z^-1) / (1 + Éø*z^-1)
	
    apf_paramA[0] = Alpha;
	apf_paramB[0] = Alpha;
	//apf_paramB[1] = 1.0r;
	apf_paramB[1] = 0.9999r;
	
	// (H(z) = (-1 + z^-1) / (1 + Éø*z^-1)
	psi2a_paramA[0] = Alpha; 
	psi2a_paramB[0] = -0.9999r;
	psi2a_paramB[1] = 0.9999r;

	
	variable_stepA[0] = Beta;
	variable_stepB[0] = Beta;
	variable_stepB[1] = 0.9999r;
	
	apf->setCoefficient(apf_paramA, apf_paramB);
	psi2a->setCoefficient(psi2a_paramA, psi2a_paramB);
	variable_step->setCoefficient(variable_stepA, variable_stepB);
}

template <class T>
inline void AdaptiveBranchingFilter<T>::Execute(const T xin)
{
    const T _apf = apf->Execute(xin);
    Yl = 0.5r * (xin + _apf); // LPF
    
	
	//Yh = 0.5r * (xin - _apf); // HPF
	Yh = 0.5r * xin - 0.5r * _apf; // HPF

	//É’(psi) = É’1 * É’2
	const T az = apf->getAz();
	const T bz = apf->getBz();
	const T _psi2 = psi2a->Execute(az+bz) * xin;

	//const T psi = (Yh * Yh - Yl * Yl) * _psi2 / 0.5r; // CPU efficient 4% down
	//const T psi = (this->Yh * this->Yh - this->Yl * this->Yl) * _psi2 * 2; // NG
	const T tmp = (Yh * Yh - Yl * Yl) * _psi2;
	const T psi = tmp + tmp;
    
	npf->set(xin);
	const T _npf = npf->get();
    
    //Alpha = Alpha - Myu * psi;
	Alpha = Alpha - Myu * psi / (_npf * _npf); // (=_rcpsp(npf.get(xin) = 1 / É≈) 
	
	updateMyu(xin);
	if (-0.9999r > Alpha) { Alpha = -0.9999r;}
	else if (0.9999r < Alpha) {Alpha = 0.9999r;}
	
	param::apf = _apf;
	param::Alpha = Alpha;
	param::Myu = Myu;
	param::Psi = psi;
	param::In = xin;
	param::outLow = Yl;
	param::outHigh = Yh;
	
    ++param::Count;
    /*
    if (param::Count == 10000000) {
      printf("Count:%d\n", param::Count);
    }
    */
}


template <class T>
inline const T AdaptiveBranchingFilter<T>::getHigh(){return Yh;}

template <class T>
inline const T AdaptiveBranchingFilter<T>::getLow(){return Yl;}

template <class T>
inline const T AdaptiveBranchingFilter<T>::getAlpha(){return Alpha;}

template <class T>
inline const T AdaptiveBranchingFilter<T>::getMyu(){return Myu;}

template <class T>
inline void AdaptiveBranchingFilter<T>::updateMyu(const T xin){
  const T tmp1 = xin + z->get();
  const T beta0 = 0.1r;
  //const T beta0 = 0.9999r;
  const T tmp2 = beta0*tmp1;
  const T tmp3 = variable_step->Execute(tmp2);
  z->set(tmp3 - tmp1);
  T alpha_hpf = tmp2 - tmp3;
  if(alpha_hpf < 0.0r) alpha_hpf = -alpha_hpf;
  //const T h0 = 0.00002r;
  //const T h0 = 0.0001r;
  const T h0 = 0.001r;
  const T h1 = 0.00008r;
  //const T old_myu = Myu;
  /*
  FILE *fp;
  fp = fopen("/tmp/b.txt", "rw");
  fwrite(&Myu, sizeof(Myu), 1, fp);
  fwrite("\n", sizeof("\n"), 1, fp);
  fclose(fp);
  */
  Myu = h0 * alpha_hpf + h1;
  
  
  //T tmp = old_myu-Myu;
  //static bool flag = false;
  //if (tmp < 0.0r) tmp = - tmp;
  /*
  //if (!flag && tmp < 0.0000000001r) {
  if (!flag && tmp == 0.0r) {
    printf("Count:%d\n", param::Count);
    flag = true;
  }
  */
  
	        
}

// Destoructor
template <class T>
AdaptiveBranchingFilter<T>::~AdaptiveBranchingFilter()
{
  delete apf;
  delete psi2a;
  delete npf;
  delete variable_step;
  delete z;
  //fclose(fp);
//  delete[] apf_paramA;

}


