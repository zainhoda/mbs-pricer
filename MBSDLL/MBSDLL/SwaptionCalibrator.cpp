#include "Array.h"
#include "time.h"
#include "stdafx.h"
#include "SwaptionCalibrator.h"
#include "SimulatedTermStructure.h"
#include "ZVTermStructure.h"
#include <cmath>
#include <string>
#include <fstream>
#include <iostream>	// required	for	the	input and output streams
#include <assert.h>
using namespace std;

void SigmaSearch(string rateFile, double swaptionSigma, int Tzero, int Tn, double dt, double freq, int nsim, int maxiter, double maxerr, double a, double &sigma, double &error)
{
	double givenPrice=0;
	double swapRate=0;
	Array<double,2>* fwdRates;
	ZVTermStructure *zv = new ZVTermStructure(rateFile,true);
	fwdRates = zv->getShortrate();

	int numStepsBwPymt = 1/dt*freq;
	//int numPymts = (Tn - Tzero)/numStepsBwPymt;
	/*double pTzeroTn = getBondPrice(fwdRates, Tzero, Tn, dt);
	double sumpTzeroTi=0;
	for(int i=Tzero+numStepsBwPymt; i<=Tn; i=i+numStepsBwPymt){
		sumpTzeroTi += getBondPrice(fwdRates, Tzero, i, dt);
	}
	swapRate = (1-pTzeroTn)/sumpTzeroTi/freq;*/
	double pZeroTn = getBondPrice(fwdRates, 0, Tn, dt);
	double sumpZeroTi=0;
	for(int i=Tzero+numStepsBwPymt; i<=Tn; i=i+numStepsBwPymt){
		sumpZeroTi += getBondPrice(fwdRates, 0, i, 1.0/12);
	}
	swapRate = (getBondPrice(fwdRates,0,Tzero,dt)-pZeroTn)/sumpZeroTi/freq;

	givenPrice = blackSwaptionPrice(swapRate,swapRate,fwdRates,swaptionSigma,Tzero,Tn,dt,freq);	//Swapstrike and swaprate equal since ATM swaption

	//iterate to find sigma
	int iterations = 0;
	double difference = 0;
	double percentageDifference=0;
	//Array<double,2>* SimsShortRates;
	SimulatedTermStructure* ts = new SimulatedTermStructure(rateFile,nsim,a,sigma,true,"none");
	do {
		ts->setSigma(sigma);
		ts->simulateTermStructure();
		//SimsShortRates = ts->getShortrate();
		difference = givenPrice - getSwaptionPrice(ts,swapRate,Tzero,Tn,dt,freq,nsim);
		percentageDifference = difference / givenPrice;

		sigma = sigma * (1 + percentageDifference);	//increasing sigma increases swaption price, so know direction

		iterations++;
	} while (iterations < maxiter && abs(difference) > maxerr);
	sigma = sigma / (1+percentageDifference);
	error = difference;
}

double getSwapRate(Array<double,2>* ShortRates, int Tzero, int Tn, double dt, double freq)
{
	int numStepsBwPymt = 1/dt*freq;
	//int numPymts = (Tn - Tzero)/numStepsBwPymt;
	double pTzeroTn = getBondPriceEst(ShortRates, Tzero, Tn, dt);
	double sumpTzeroTi=0;
	for(int i=Tzero+numStepsBwPymt; i<=Tn; i=i+numStepsBwPymt){
		sumpTzeroTi += getBondPriceEst(ShortRates, Tzero, i, dt);
	}
	return (1-pTzeroTn)/sumpTzeroTi/freq;
}

double getBondPriceEst(Array<double,2>* ShortRates, int Tzero, int Tn, double dt)
{
	//E(exp(integral rs ds))
	Array<double,1>* oneRun = new Array<double,1>((*ShortRates).dim2());
	double result=0;
	for(int sim =0; sim<(*ShortRates).dim1(); sim++){
		(*oneRun) = (*ShortRates)[sim];
		double sumrates=0;
		for(int i=Tzero; i<=Tn; i++){
			sumrates+=(*oneRun)[i];
		}
		result += exp(-sumrates*dt);
	}
	delete oneRun;
	return result/((*ShortRates).dim1());
}

double getBondPrice(Array<double,2>* fwdRates, int Tzero, int Tn, double dt)
{
	//exp(integral f(u,s) ds)
	double sumrates=0;
	for(int i=Tzero; i<=Tn; i++){
		sumrates+=(*fwdRates)[0][i];
	}
	return exp(-sumrates*dt);
}
/*
double getSwaptionPrice(Array<double,2>* SimsShortRates, double swapStrike, int Tzero, int Tn, double dt, double freq)
{
	Array<double,1>* ShortRates = new Array<double,1>((*SimsShortRates).dim2());
	//(*ShortRates) = (*SimsShortRates)[0];
	double SwapRate=0;
	double result=0;
	double discount=0;
	double sumPTzeroTi=0;
	int numStepsBwPymt = 1/dt*freq;
	
	for(int sim=0; sim<(*SimsShortRates).dim1(); sim++){
		(*ShortRates) = (*SimsShortRates)[sim];
		SwapRate = getSwapRate(SimsShortRates,Tzero,Tn,dt,freq);
		if(SwapRate > swapStrike){
			//discount = getBondPrice(ShortRates,0,Tzero,dt);
			sumPTzeroTi=0;
			for(int i=Tzero+numStepsBwPymt; i<=Tn; i=i+numStepsBwPymt){
				//sumPTzeroTi += getBondPrice(fwdRates, Tzero, i, dt);
				sumPTzeroTi += getBondPriceEst(SimsShortRates, Tzero, i, dt);
			} 
			result += (SwapRate - swapStrike) * freq * sumPTzeroTi;
		}
	}
	discount = getBondPriceEst(SimsShortRates,0,Tzero,dt);	//discount from start to To
	delete ShortRates;
	return discount*result/((*SimsShortRates).dim1());
}*/

double blackImpliedVol(double swapRate, double swapStrike, Array<double,2>* fwdCurve, double price, int Tzero, int Tn, double dt, double freq)
{
	double sigma = 0.1;	//initial guess
	double difference=0;
	double percentageDifference = 0;
	do {
		sigma = sigma * (1 + percentageDifference);	//increasing sigma increases swaption price, so know direction
		//SimsShortRates = ts->getShortrate();
		difference = price - blackSwaptionPrice(swapRate,swapStrike,fwdCurve,sigma,Tzero,Tn,dt,freq);
		percentageDifference = difference / price;
		if(percentageDifference<-0.99)percentageDifference=-0.9;
	} while (abs(difference) > 0.0005);
	if(sigma<0)return 0.01;
	return sigma;
}

double blackSwaptionPrice(double swapRate, double swapStrike, Array<double,2> *fwdCurve, double sigma, int Tzero, int Tn, double dt, double freq)
{
	int numStepsBwPymt = 1/dt*freq;
	double sumpTzeroTi=0;
	for(int i=Tzero+numStepsBwPymt; i<=Tn; i=i+numStepsBwPymt){
		sumpTzeroTi += getBondPrice(fwdCurve, Tzero, i, dt);
	}
	double d1, d2;
	d1 = (log(swapRate/swapStrike) + sigma*sigma*Tzero*dt)/sigma/sqrt(Tzero*dt);
	d2 = (log(swapRate/swapStrike) - sigma*sigma*Tzero*dt)/sigma/sqrt(Tzero*dt);
	return ((swapRate*NormCdf(d1) - swapStrike*NormCdf(d2))*sumpTzeroTi*freq);
}

double NormCdf(double d)
{
	// approximation of the cumulative distribution function
	// of the normal ditribution with mean 0 and variance 1
	if (d < 0.0) return 1.0 - NormCdf(-d);
	else
	{
		double y = 1.0 / (1.0 + 0.33267 * d);
		return 1.0 - 0.39894228 * exp(-0.5 * d * d) * y * (0.4361836 + y * (-0.1201676 + 0.937298 * y));
	}
}


double getSwaptionPrice(SimulatedTermStructure* ts, double swapStrike,int Tzero, int Tn, double dt, double freq, int nsim)
{
	double result=0;
	Array<double,2>* SimsShortRates;
	Array<double,1>* fwdRates;
	//ZVTermStructure *zv = new ZVTermStructure(rateFile,true);
	//fwdRates = zv->getShortrate();
	//SimulatedTermStructure* ts = new SimulatedTermStructure(rateFile,nsim,a,sigma,true,"none");
	for(int i=0; i<nsim; i++){
		if(i!=0)ts->simulateTermStructure();
		SimsShortRates = ts->getShortrate();
		result += oneRunSwaption(SimsShortRates,swapStrike,Tzero,Tn,dt,freq);
	}
	return result/nsim;
}

double oneRunSwaption(Array<double,2>* SimsShortRates, double swapStrike, int Tzero, int Tn, double dt, double freq)
{
	Array<double,2>* ShortRates = new Array<double,2>(1,(*SimsShortRates).dim2());
	(*ShortRates)[0] = (*SimsShortRates)[0];
	double SwapRate=0;
	double result=0;
	double discount=0;
	double sumpTzeroTi=0;
	int numStepsBwPymt = 1/dt*freq;
	
	//for(int sim=0; sim<(*SimsShortRates).dim1(); sim++){
		//(*ShortRates) = (*SimsShortRates)[sim];
	SwapRate = getSwapRate(SimsShortRates,Tzero,Tn,dt,freq);
	if(SwapRate > swapStrike){
		//discount = getBondPrice(ShortRates,0,Tzero,dt);
		discount = getBondPriceEst(SimsShortRates,0,Tzero,dt);
		sumpTzeroTi=0;
		for(int i=Tzero+numStepsBwPymt; i<=Tn; i=i+numStepsBwPymt){
			sumpTzeroTi += getBondPriceEst(SimsShortRates, Tzero, i, dt);
		} 
		result = discount * (SwapRate - swapStrike) * freq * sumpTzeroTi;
	}
	//}
	
	delete ShortRates;
	return result; ///((*SimsShortRates).dim1());
}
