#include "stdafx.h"
#include "ZVTermStructure.h"

Array<double,2>* ZVTermStructure::getNYearSwapRate(int n)
{
	Array<double,1>& termStructure = *swapRatePtr;

	Array<double,2>* newTermStructurePtr = new Array<double,2>(1, termStructure.dim1() - (n * 12) );
	Array<double,2>& newTermStructure = *newTermStructurePtr;

	for (int step = 0; step < newTermStructure.dim2(); step++) {
		double sum = 0;

		for (int forward = step; forward < (step+n*12); forward++) {
			sum += termStructure[forward];
		}
		newTermStructure[0][step] = sum / (n * 12);
	}

	return newTermStructurePtr;
}

Array<double,2>* ZVTermStructure::getShortrate()
{
	Array<double,1>& termStructure = *swapRatePtr;

	Array<double,2>* newTermStructurePtr = new Array<double,2>(1, termStructure.dim1() - (10 * 12) );
	Array<double,2>& newTermStructure = *newTermStructurePtr;

	for (int step = 0; step < newTermStructure.dim2(); step++) {
		newTermStructure[0][step] = termStructure[step];
	}

	return newTermStructurePtr;
}