#ifndef _CPRPATHGENERATOR_H_
#define _CPRPATHGENERATOR_H_
#include "time.h"
#include "stdafx.h"
#include "Array.h"
#include <string>
#include <fstream>
#include <iostream>	// required	for	the	input and output streams
using namespace std;

class cprPathGenerator{
private:
	double currMtgeRate;
	int startMonth;
	Array<double,2> *mtgeRatePath;
	string seasonalityFile;
	double upramp;
	int uprampperiod;

	double NormCdf(double d);
	void addTurnoverRate(Array<double,2> &CPRpathP);
	void addRefinancingRate(Array<double,2> &CPRpathP, Array<double,2> &mtgePathP);

public:
	cprPathGenerator(Array<double,2> *mtgeRatePath_, string seasonality_, int startMonth_, double currMtgeRate_);
	Array<double,2>* getCPRpath();
};

#endif