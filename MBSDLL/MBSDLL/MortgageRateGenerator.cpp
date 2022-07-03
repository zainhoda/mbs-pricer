#include "stdafx.h"
#include "MortgageRateGenerator.h"
#include <iostream>	// required	for	the	input and output streams
using namespace std;

Array<double, 2>* MortgageRateGenerator::getMortgageRatePath()
{
	Array<double,2>* mortgageRatePtr;

	Array<double,2>& swapRates1 = *swapRates1Ptr;

	if (swapRates2Ptr == NULL) {
		mortgageRatePtr = new Array<double,2>(swapRates1.dim1(), swapRates1.dim2());

		Array<double,2>& mortgageRate = *mortgageRatePtr;

		for (int sim = 0; sim < mortgageRate.dim1(); sim++) {
			for (int step = 0; step < mortgageRate.dim2(); step++) {
				mortgageRate[sim][step] = alpha + beta1 * swapRates1[sim][step];
			}
		}
	} else {
		Array<double,2>& swapRates2 = *swapRates2Ptr;
		int dim1, dim2;
		if (swapRates1.dim1() > swapRates2.dim1()) {
			dim1 = swapRates2.dim1();
		} else {
			dim1 = swapRates1.dim1();
		}
		if (swapRates1.dim2() > swapRates2.dim2()) {
			dim2 = swapRates2.dim2();
		} else {
			dim2 = swapRates1.dim2();
		}

		mortgageRatePtr = new Array<double,2>(dim1, dim2);

		Array<double,2>& mortgageRate = *mortgageRatePtr;

		for (int sim = 0; sim < mortgageRate.dim1(); sim++) {
			for (int step = 0; step < mortgageRate.dim2(); step++) {
				mortgageRate[sim][step] = alpha + beta1 * swapRates1[sim][step] + beta2 * swapRates2[sim][step];
			}
		}
	}

	return mortgageRatePtr;
}