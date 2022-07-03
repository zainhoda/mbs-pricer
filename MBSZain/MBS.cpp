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

MBS::MBS(int startMonth_, double currMtgeRate_, double MBScpn_, double alpha_, double b1_, double b2_, double a_, double sig_, string rateFile_, string derivFile_, string seasonalFile_)
{
	startMonth = startMonth_;
	currMtgeRate = currMtgeRate_;
	MBScpn = MBScpn_;
	alpha = alpha_;
	b1 = b1_;
	b2 = b2_;
	a = a_;
	sigma = sig_;
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
				sumrates += SR[sim][i]/100 * dt;
			}
			discofact = exp(-sumrates-lambda*month*dt);
			resultpersim += discofact * CF[sim][month];
		}
		result += resultpersim;
	}

	return result/nsim;
}

double MBS::getPrice(double lambda, int nsim)
{
	//assumes shortrates in decimals
	SimulatedTermStructure* ts = new SimulatedTermStructure(rateFile, nsim, a, sigma, derivativeFile);
	ts->getNYearSwapRate(10);

	MortgageRateGenerator* mg = new MortgageRateGenerator(ts->getNYearSwapRate(10), NULL, alpha, b1, b2);
	cprPathGenerator* cpr = new cprPathGenerator(mg->getMortgageRatePath(), seasonalFile, startMonth, currMtgeRate);
	CFgenerator* cf = new CFgenerator( cpr->getCPRpath(), currMtgeRate, MBScpn);
	
	Array<double,2>* ShortRates = ts->getShortrate();
	Array<double,2>* CashFlows = cf->getCashFlows();

	return getPrice(lambda, ShortRates, CashFlows, nsim);
}

double MBS::ZVspread(double price, double guess)
{
	//assumes shortrates in decimals
	ZVTermStructure* ts = new ZVTermStructure(rateFile);
	ts->getNYearSwapRate(10);

	MortgageRateGenerator* mg = new MortgageRateGenerator(ts->getNYearSwapRate(10), NULL, alpha, b1, b2);
	cprPathGenerator* cpr = new cprPathGenerator(mg->getMortgageRatePath(), seasonalFile, startMonth, currMtgeRate);
	CFgenerator* cf = new CFgenerator( cpr->getCPRpath(), currMtgeRate, MBScpn);
	
	Array<double,2>* ShortRates = ts->getShortrate();
	Array<double,2>* CashFlows = cf->getCashFlows();

	return findLambda(price, guess, ShortRates, CashFlows, 1);
}

double MBS::OAspread(double price, double guess, int nsim)
{
	//assumes shortrates in decimals
	SimulatedTermStructure* ts = new SimulatedTermStructure(rateFile, nsim, a, sigma, derivativeFile);
	ts->getNYearSwapRate(10);

	MortgageRateGenerator* mg = new MortgageRateGenerator(ts->getNYearSwapRate(10), NULL, alpha, b1, b2);
	cprPathGenerator* cpr = new cprPathGenerator(mg->getMortgageRatePath(), seasonalFile, startMonth, currMtgeRate);
	CFgenerator* cf = new CFgenerator( cpr->getCPRpath(), currMtgeRate, MBScpn);
	
	Array<double,2>* ShortRates = ts->getShortrate();
	Array<double,2>* CashFlows = cf->getCashFlows();

	return findLambda(price, guess, ShortRates, CashFlows, nsim);
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
	cout << "Iterations = " << iterations << endl;
	cout << "Difference = " << difference << endl;

	return currentLambda;
}