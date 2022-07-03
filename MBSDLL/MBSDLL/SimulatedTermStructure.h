#ifndef _SIMULATEDTERMSTRUCTURE_H_
#define _SIMULATEDTERMSTRUCTURE_H_

#include "TermStructure.h"
#include <fstream>
#include <string>

using namespace std;

class SimulatedTermStructure : public TermStructure
{
protected:
	double test; // delete later
	Array<double,1>* swapRateDerivativePtr; // first value is 0
	Array<double,2>* termStructurePtr;
	
	int nsim;
	double a;
	double sigma;
	double dt;
	void imputeDerivative();

public:
	SimulatedTermStructure(string rateFile, int nsim, double a, double sigma, bool complete = true, string derivativeFile = "none", double offset=0) : TermStructure(rateFile, complete, offset) {
		this->a = a;
		this->sigma = sigma;
		this->nsim = nsim;
		if (derivativeFile != "none") {
			swapRateDerivativePtr = readFile(derivativeFile);
		} else {
			imputeDerivative();
		}
		dt = 1.0/12.0;
		
		simulateTermStructure();
	}

	virtual Array<double,2>* getNYearSwapRate(int n);
	virtual Array<double,2>* getShortrate();
	void simulateTermStructure();
	void setSigma(double sigma_){sigma = sigma_;}

	double getTest() {return test;} // delete later
};

#endif