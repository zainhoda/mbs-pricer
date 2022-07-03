#include "stdafx.h"
#include "SimulatedTermStructure.h"
#include <math.h>
#include "rangen.h"

void SimulatedTermStructure::imputeDerivative()
{
	swapRateDerivativePtr = new Array<double,1>( (*swapRatePtr).dim1() - 1);
	Array<double,1>& swapRate = *swapRatePtr;
	Array<double,1>& swapRateDerivative = *swapRateDerivativePtr;

	swapRateDerivative[0] = 0;
	for (int i = 0; i < swapRate.dim1()-2; i++) {
		double slopeLag0 = (swapRate[i+1] - swapRate[i]) / dt;
		double slopeLag1 = (swapRate[i+2] - swapRate[i+1]) / dt;

		swapRateDerivative[i+1] = (slopeLag0 + slopeLag1) / 2.0;
	}
}

void SimulatedTermStructure::simulateTermStructure()
{
	Array<double,1>& swapRate = *swapRatePtr;
	Array<double,1>& swapRateDerivative = *swapRateDerivativePtr;
	termStructurePtr = new Array<double,2>(nsim, swapRate.dim1());
	Array<double,2>& termStructure = *termStructurePtr;

	for (int sim = 0; sim < termStructure.dim1(); sim++) {
		termStructure[sim][0] = swapRate[0];
		for (int step = 1; step < termStructure.dim2(); step++) {
			double rbar = swapRate[step] + (1/a) * swapRateDerivative[step-1] + (sigma * sigma * (1 - exp(-2 * a * step * dt))) / (2 * a * a);

			termStructure[sim][step] = termStructure[sim][step-1] + a * (rbar - termStructure[sim][step-1]) * dt + sigma * sqrt(dt) * normsim();
		}
	}
}

Array<double,2>* SimulatedTermStructure::getNYearSwapRate(int n)
{
	Array<double,2>& termStructure = *termStructurePtr;

	Array<double,2>* newTermStructurePtr = new Array<double,2>(termStructure.dim1(), termStructure.dim2() - (n * 12) );
	Array<double,2>& newTermStructure = *newTermStructurePtr;

	

	for (int sim = 0; sim < newTermStructure.dim1(); sim++) {
		for (int step = 0; step < newTermStructure.dim2(); step++) {
			double sum = 0;

			for (int forward = step; forward < (step+n*12); forward++) {
				sum += termStructure[sim][forward];
			}
			newTermStructure[sim][step] = sum / (n * 12);
		}
	}

	return newTermStructurePtr;
}

Array<double,2>* SimulatedTermStructure::getShortrate()
{
	Array<double,2>& termStructure = *termStructurePtr;

	Array<double,2>* newTermStructurePtr = new Array<double,2>(termStructure.dim1(), termStructure.dim2() - (10 * 12) );
	Array<double,2>& newTermStructure = *newTermStructurePtr;

	for (int sim = 0; sim < newTermStructure.dim1(); sim++) {
		for (int step = 0; step < newTermStructure.dim2(); step++) {
			newTermStructure[sim][step] = termStructure[sim][step];
		}
	}

	return newTermStructurePtr;
}