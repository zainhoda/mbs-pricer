#ifndef _MORTGAGERATEGENERATOR_H_
#define _MORTGAGERATEGENERATOR_H_

#include "Array.h"

class MortgageRateGenerator
{
protected:
	Array<double,2>* swapRates1Ptr;
	Array<double,2>* swapRates2Ptr;
	double alpha;
	double beta1;
	double beta2;

public:
	MortgageRateGenerator(Array<double,2>* swapRates1Ptr, Array<double,2>* swapRates2Ptr = NULL, double alpha = 0, double beta1 = 0, double beta2 = 0) {
		this->swapRates1Ptr = swapRates1Ptr;
		this->swapRates2Ptr = swapRates2Ptr;
		this->alpha = alpha;
		this->beta1 = beta1;
		this->beta2 = beta2;
	}

	Array<double, 2>* getMortgageRatePath();
};

#endif