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
	string type;
	//type "A" or "B"
	//if you use type B, make sure to pass in 10 yr rate path, not mtge rate path
	double currMtgeRate;
	int startMonth;
	Array<double,2> *mtgeRatePath;
	string seasonalityFile;
	double upramp;		//0.2 per month
	int uprampperiod;	//30 months

	double NormCdf(double d);
	void addTurnoverRate(Array<double,2> &CPRpathP);
	void addRefinancingRate(Array<double,2> &CPRpathP, Array<double,2> &mtgePathP);
	void CPRmethodB(Array<double,2> &CPRpathP, Array<double,2> &mtgeRateP);

public:
	cprPathGenerator(Array<double,2> *mtgeRatePath_, string seasonality_, int startMonth_, double currMtgeRate_, string type_);
	cprPathGenerator(){};
	Array<double,2>* getCPRpath();
};

#endif