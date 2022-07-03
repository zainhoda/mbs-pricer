#include "Array.h"
#include "time.h"
#include "stdafx.h"
#include "MBS.h"
#include <cmath>
#include <fstream>
#include <iostream>	// required	for	the	input and output streams
#include "SimulatedTermStructure.h"
#include "MortgageRateGenerator.h"
#include "cprPathGenerator.h"
#include "CFGenerator.h"
#include "ZVTermStructure.h"

using namespace std;

MBS::MBS(int startMonth_, double currMtgeRate_, double MBScpn_, double alpha_, double b1_, double b2_, double a_, double sig_, bool complete_, string rateFile_, string derivFile_, string seasonalFile_)
{
	startMonth = startMonth_;
	currMtgeRate = currMtgeRate_;
	MBScpn = MBScpn_;
	alpha = alpha_;
	b1 = b1_;
	b2 = b2_;
	a = a_;
	sigma = sig_;
	complete = complete_;	//whether the rate file has all 360 rates or just 7
	rateFile = rateFile_;
	derivativeFile = derivFile_;
	seasonalFile = seasonalFile_;
}

double MBS::getPrice(double lambda, Array<double,2>* ShortRates, Array<double,2>* CashFlows, int nsim)
{
	Array<double,2> &SR = *ShortRates;
	Array<double,2> &CF = *CashFlows;

	double sumrates=0;
	double discofact=0;
	double dt = 1.0/12;
	double resultpersim=0;
	double result=0;

	for(int sim=0; sim<CF.dim1(); sim++){
		resultpersim = 0;
		for(int month=1; month<CF.dim2(); month++){
			sumrates=0;
			for(int i=0; i<=month; i++){
				sumrates += SR[sim][i] * dt;
			}
			discofact = exp(-sumrates-lambda*month*dt);
			resultpersim += discofact * CF[sim][month];
		}
		result += resultpersim;
	}

	return result/nsim;
}

double MBS::getPrice(double lambda, int nsim, string CPRmodel)
{
	lambda = lambda / 10000;
	//assumes shortrates in decimals
	SimulatedTermStructure* ts = new SimulatedTermStructure(rateFile, nsim, a, sigma, complete, derivativeFile);
	cprPathGenerator* cpr;

	if(CPRmodel=="A"){
		MortgageRateGenerator* mg = new MortgageRateGenerator(ts->getNYearSwapRate(10), NULL, alpha, b1, b2);
		cpr = new cprPathGenerator(mg->getMortgageRatePath(), seasonalFile, startMonth, currMtgeRate, CPRmodel);
	}else{
		//WARNING
		//eventually, we'll have to get 10 yr zcb rate instead of 10 yr swap rate
		cpr = new cprPathGenerator(ts->getNYearSwapRate(10), seasonalFile, startMonth, currMtgeRate, CPRmodel);
	}

	CFgenerator* cf = new CFgenerator( cpr->getCPRpath(), currMtgeRate, MBScpn);
	
	Array<double,2>* ShortRates = ts->getShortrate();
	Array<double,2>* CashFlows = cf->getCashFlows();

	return getPrice(lambda, ShortRates, CashFlows, nsim);
}

double MBS::ZVspread(double price, double guess, string CPRmodel)
{
	guess = guess / 10000;
	//assumes shortrates in decimals
	ZVTermStructure* ts = new ZVTermStructure(rateFile,complete);
	cprPathGenerator* cpr;

	if(CPRmodel=="A"){
		MortgageRateGenerator* mg = new MortgageRateGenerator(ts->getNYearSwapRate(10), NULL, alpha, b1, b2);
		cpr = new cprPathGenerator(mg->getMortgageRatePath(), seasonalFile, startMonth, currMtgeRate, CPRmodel);
	}else{
		//WARNING
		//eventually, we'll have to get 10 yr zcb rate instead of 10 yr swap rate
		cpr = new cprPathGenerator(ts->getNYearSwapRate(10), seasonalFile, startMonth, currMtgeRate, CPRmodel);
	}

	CFgenerator* cf = new CFgenerator( cpr->getCPRpath(), currMtgeRate, MBScpn);
	
	Array<double,2>* ShortRates = ts->getShortrate();
	Array<double,2>* CashFlows = cf->getCashFlows();

	return findLambda(price, guess, ShortRates, CashFlows, 1) * 10000;
}

double MBS::OAspread(double price, double guess, int nsim, string CPRmodel)
{
	guess = guess / 10000;
	//assumes shortrates in decimals
	SimulatedTermStructure* ts = new SimulatedTermStructure(rateFile, nsim, a, sigma, complete,derivativeFile);
	cprPathGenerator* cpr;

	if(CPRmodel=="A"){
		MortgageRateGenerator* mg = new MortgageRateGenerator(ts->getNYearSwapRate(10), NULL, alpha, b1, b2);
		cpr = new cprPathGenerator(mg->getMortgageRatePath(), seasonalFile, startMonth, currMtgeRate, CPRmodel);
	}else{
		//WARNING
		//eventually, we'll have to get 10 yr zcb rate instead of 10 yr swap rate
		cpr = new cprPathGenerator(ts->getNYearSwapRate(10), seasonalFile, startMonth, currMtgeRate, CPRmodel);
	}

	CFgenerator* cf = new CFgenerator( cpr->getCPRpath(), currMtgeRate, MBScpn);
	
	Array<double,2>* ShortRates = ts->getShortrate();
	Array<double,2>* CashFlows = cf->getCashFlows();

	return findLambda(price, guess, ShortRates, CashFlows, nsim) * 10000;
}

double MBS::findLambda(double price, double guess, Array<double,2>* ShortRates, Array<double,2>* CashFlows, int nsim)
{
	double currentLambda = guess;
	int iterations = 0;
	double difference;

	do {
		difference = getPrice(currentLambda, ShortRates, CashFlows, nsim) - price;
		double percentageDifference = difference / price;

		currentLambda = currentLambda * (1 + percentageDifference);

		iterations++;
	} while (iterations < 10 && abs(difference) > 0.01);
	//cout << "Iterations = " << iterations << endl;
	//cout << "Difference = " << difference << endl;

	return currentLambda;
}

double MBS::OAduration(int nsim, string CPRmodel, bool both, double OAS, double price)
{
	int numIters=3;
	int mid=1;
	double offset=0.005;
	
	Array<double,1> prices(numIters);
	prices[mid] = price;

	if(price>0){
		OAS = OAspread(price,OAS,nsim,CPRmodel);
	}else if(!both){
		prices[mid] = getPrice(OAS,nsim,CPRmodel);
	}
	
	for(int i=0; i<numIters; i++){
		if(i!=mid){
			
			SimulatedTermStructure* ts = new SimulatedTermStructure(rateFile, nsim, a, sigma, complete,derivativeFile,-offset+i*offset);
			cprPathGenerator* cpr;

			if(CPRmodel=="A"){
				MortgageRateGenerator* mg = new MortgageRateGenerator(ts->getNYearSwapRate(10), NULL, alpha, b1, b2);
				cpr = new cprPathGenerator(mg->getMortgageRatePath(), seasonalFile, startMonth, currMtgeRate, CPRmodel);
				delete mg;
			}else{
				//WARNING
				//eventually, we'll have to get 10 yr zcb rate instead of 10 yr swap rate
				cpr = new cprPathGenerator(ts->getNYearSwapRate(10), seasonalFile, startMonth, currMtgeRate, CPRmodel);
			}

			CFgenerator* cf = new CFgenerator( cpr->getCPRpath(), currMtgeRate, MBScpn);
	
			Array<double,2>* ShortRates = ts->getShortrate();
			Array<double,2>* CashFlows = cf->getCashFlows();

			prices[i] = getPrice(OAS/10000,ShortRates,CashFlows,nsim);
			delete CashFlows;
			delete ShortRates;
			delete ts;
			delete cpr;
		}
	}
	//cout << prices[0] << endl;
	//cout << prices[1] << endl;
	//cout << prices[2] << endl;
	return (prices[0] - prices[2])/prices[mid]*100;
}

double MBS::OAconvexity(int nsim, string CPRmodel, bool both, double OAS, double price)
{
	int numIters=5;
	int mid=2;
	double offset=0.005;
	double durationUp, durationDown;
	
	Array<double,1> prices(numIters);
	prices[mid] = price;
	if(price>0){
		OAS = OAspread(price,OAS,nsim,CPRmodel);
	}else if(!both){
		price = getPrice(OAS,nsim,CPRmodel);
	}
	for(int i=0; i<numIters; i++){
		if(i!=mid){
			SimulatedTermStructure* ts = new SimulatedTermStructure(rateFile, nsim, a, sigma, complete,derivativeFile,-2*offset+i*offset);
			cprPathGenerator* cpr;

			if(CPRmodel=="A"){
				MortgageRateGenerator* mg = new MortgageRateGenerator(ts->getNYearSwapRate(10), NULL, alpha, b1, b2);
				cpr = new cprPathGenerator(mg->getMortgageRatePath(), seasonalFile, startMonth, currMtgeRate, CPRmodel);
				delete mg;
			}else{
				//WARNING
				//eventually, we'll have to get 10 yr zcb rate instead of 10 yr swap rate
				cpr = new cprPathGenerator(ts->getNYearSwapRate(10), seasonalFile, startMonth, currMtgeRate, CPRmodel);
			}

			CFgenerator* cf = new CFgenerator( cpr->getCPRpath(), currMtgeRate, MBScpn);
	
			Array<double,2>* ShortRates = ts->getShortrate();
			Array<double,2>* CashFlows = cf->getCashFlows();

			prices[i] = getPrice(OAS/10000,ShortRates,CashFlows,nsim);
			delete CashFlows;
			delete ShortRates;
			delete ts;
			delete cpr;
		}
	}
	durationDown = (prices[0] - prices[2])/prices[1]*100;
	durationUp = (prices[2] - prices[4])/prices[3]*100;
	//cout << durationUp << endl;
	//cout << durationDown <<endl;
	return (durationDown - durationUp);
}

/*
//for testing purposes
Array<double,1>* MBS::ConvexityTest(Array<double,1>* MtgeRates, Array<double,1>* CpnRates, double lambda, int nsim, string CPRmodel)
{
	//assumes shortrates in decimals
	Array<double,1>* result = new Array<double,1>((*MtgeRates).dim1());

	SimulatedTermStructure* ts = new SimulatedTermStructure(rateFile, nsim, a, sigma, complete, derivativeFile);
	ts->getNYearSwapRate(10);

	MortgageRateGenerator* mg = new MortgageRateGenerator(ts->getNYearSwapRate(10), NULL, alpha, b1, b2);
	Array<double,2>* MtgePath = mg->getMortgageRatePath();
	
	for(int i=(*MtgeRates).dim1()-1; i>=0; i--){
		cprPathGenerator* cpr;
		if(CPRmodel=="A"){
			cpr = new cprPathGenerator(MtgePath, seasonalFile, startMonth, (*MtgeRates)[i], CPRmodel);
		}else{
			cpr = new cprPathGenerator(ts->getNYearSwapRate(10), seasonalFile, startMonth, (*MtgeRates)[i], CPRmodel);
		}
		CFgenerator* cf = new CFgenerator( cpr->getCPRpath(), (*MtgeRates)[i], (*CpnRates)[i]);
	
		Array<double,2>* ShortRates = ts->getShortrate();
		Array<double,2>* CashFlows = cf->getCashFlows();

		(*result)[i] = getPrice(lambda, ShortRates, CashFlows, nsim);
		delete cpr;
		delete cf;
	}
	delete ts;
	delete mg;
	return result;
}
*/