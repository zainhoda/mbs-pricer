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

	//filenames
	string rateFile;
	string derivativeFile;
	string seasonalFile;
	double getPrice(double lambda, Array<double,2>* ShortRates, Array<double,2>* CashFlows, int nsim);
	double findLambda(double price, double guess, Array<double,2>* ShortRates, Array<double,2>* CashFlows, int nsim);

public:
	MBS(int startMonth_, double currMtgeRate_, double MBScpn_, double alpha_, double b1_, double b2_, double a_, double sig_, string rateFile_, string derivFile_, string seasonalFile_);
	double getPrice(double lambda, int nsim);
	double ZVspread(double price, double guess);
	double OAspread(double price, double guess, int nsim);
};
#endif