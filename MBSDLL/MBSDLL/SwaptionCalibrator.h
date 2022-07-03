#ifndef _SWAPTIONCALIBRATOR_H_
#define _SWAPTIONCALIBRATOR_H_

#include "Array.h"
#include "SimulatedTermStructure.h"
#include "time.h"
#include "stdafx.h"
#include <cmath>
#include <fstream>
#include <iostream>	// required	for	the	input and output streams
#include <assert.h>
using namespace std;

void SigmaSearch(string rateFile, double swaptionSigma, int Tzero, int Tn, double dt, double freq, int nsim, int maxiter, double maxerr, double a, double &sigma, double &error);
double getSwapRate(Array<double,2>* ShortRates, int Tzero, int Tn, double dt, double freq);
double getBondPriceEst(Array<double,2>* ShortRates, int Tzero, int Tn, double dt);
//double getSwaptionPrice(Array<double,2>* SimsShortRates, double swapStrike, int Tzero, int Tn, double dt, double freq);
double getBondPrice(Array<double,2>* ShortRates, int Tzero, int Tn, double dt);
double blackImpliedVol(double swapRate, double swapStrike, Array<double,2>* fwdCurve, double price, int Tzero, int Tn, double dt, double freq);
double blackSwaptionPrice(double swapRate, double swapStrike, Array<double,2> *fwdCurve, double sigma, int Tzero, int Tn, double dt, double freq);
double NormCdf(double d);
double getSwaptionPrice(SimulatedTermStructure* ts, double swapStrike,int Tzero, int Tn, double dt, double freq, int nsim);
double oneRunSwaption(Array<double,2>* SimsShortRates, double swapStrike, int Tzero, int Tn, double dt, double freq);

#endif