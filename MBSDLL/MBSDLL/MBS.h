#ifndef _MBS_H_
#define _MBS_H_
#include "time.h"
#include "stdafx.h"
#include "Array.h"
#include <string>
#include <fstream>
#include <iostream>	// required	for	the	input and output streams
using namespace std;

class MBS{
private:
	int startMonth;

	//in decimal terms e.g. 0.08
	double currMtgeRate;
	double MBScpn;

	//regression parameters for regression mortgage rate against swap rate
	double alpha;
	double b1;
	double b2;

	//Vasicek paramters
	double a;
	double sigma;

	//files
	bool complete;	//whether the rate file is complete (i.e. 360 entries) or just 7 entries
	string rateFile;
	string derivativeFile;
	string seasonalFile;
	double getPrice(double lambda, Array<double,2>* ShortRates, Array<double,2>* CashFlows, int nsim);
	double findLambda(double price, double guess, Array<double,2>* ShortRates, Array<double,2>* CashFlows, int nsim);

public:
	MBS(int startMonth_, double currMtgeRate_, double MBScpn_, double alpha_, double b1_, double b2_, double a_, double sig_, bool complete_, string rateFile_, string derivFile_, string seasonalFile_);
	//CPR model is "A" or "B"
	double getPrice(double lambda, int nsim, string CPRmodel);
	double ZVspread(double price, double guess, string CPRmodel);
	double OAspread(double price, double guess, int nsim, string CPRmodel);
	//enter OAS if you know it, or a guess if you don't
	double OAduration(int nsim, string CPRmodel, bool both, double OAS, double price=-1);
	double OAconvexity(int nsim, string CPRmodel, bool both, double OAS, double price=-1);

	//Array<double,1>* ConvexityTest(Array<double,1>* MtgeRates, Array<double,1>* CpnRates, double lambda, int nsim, string CPRmodel);
};
#endif