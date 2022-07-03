
#include "Array.h"
#include "time.h"
#include "stdafx.h"
#include "cprPathGenerator.h"
#include <cmath>
#include <fstream>
#include <iostream>	// required	for	the	input and output streams
using namespace std;

cprPathGenerator::cprPathGenerator(Array<double,2> *mtgeRatePath_, string seasonality_, int startMonth_, double currMtgeRate_)
{
	currMtgeRate = currMtgeRate_;
	mtgeRatePath = mtgeRatePath_;
	seasonalityFile = seasonality_;
	startMonth = startMonth_;
	upramp = 0.2;
	uprampperiod = 30;
}

Array<double,2>* cprPathGenerator::getCPRpath()
{
	Array<double,2> &mtgeRateP = *mtgeRatePath;
	Array<double,2> *CPRpaths = new Array<double,2>(mtgeRateP.dim1(),mtgeRateP.dim2());
	Array<double,2> &CPRpathP = *CPRpaths;

	addTurnoverRate(CPRpathP);
	addRefinancingRate(CPRpathP, mtgeRateP);

	delete mtgeRatePath;
	return CPRpaths;
}

void cprPathGenerator::addTurnoverRate(Array<double,2> &CPRpathP)
{
	//get monthly factor
	int numMonths = 12;
	Array<double,1> monthFactor(numMonths);
	ifstream infile(seasonalityFile.c_str());
	for(int month=1; month<=(numMonths-1); month++){
		infile >> monthFactor[month];
	}
	//december goes in first slot
	infile >> monthFactor[0];

	//add seasoning ramp, multiplied by monthly factor
	int month=0;

	for(month=0; month<=uprampperiod; month++){
		CPRpathP[0][month] = month*upramp*monthFactor[(month+startMonth)%numMonths];
	}

	for(month=uprampperiod+1; month<CPRpathP.dim2(); month++){
		CPRpathP[0][month] = uprampperiod*upramp*monthFactor[(month+startMonth)%numMonths];
	}

	for(int nsim=1; nsim<CPRpathP.dim1(); nsim++){
		CPRpathP[nsim] = CPRpathP[0];
	}
}

void cprPathGenerator::addRefinancingRate(Array<double,2> &CPRpathP, Array<double,2> &mtgeRateP)
{
	double incentive=0.0;
	double proportionInterest=0.0;
	double survivalRate=1.0;
	double refinCPR=0;
	double maxCPR=60.0;	//tops out at 60 + 6 = 66
	double aveBPdiff=50.0;
	int bpFactor = 100;
	//if you increase scale factor you make extreme events more likely
	double scaleFactor = 1.5;	
	//maxCPR += (rand()/double(RAND_MAX) - 0.5) * 10;

	for(int nsim=0; nsim<CPRpathP.dim1(); nsim++){
		survivalRate = 1.0;
		for(int month=1; month<CPRpathP.dim2(); month++){
			incentive = (currMtgeRate - mtgeRateP[nsim][month])*bpFactor;
			proportionInterest = NormCdf(incentive-aveBPdiff/double(bpFactor)*scaleFactor);

			refinCPR=0.0;
			if(proportionInterest > (1-survivalRate)){
				refinCPR = ((proportionInterest - (1 - survivalRate))/survivalRate)*maxCPR;
				survivalRate = 1 - proportionInterest;
			}

			if(month<uprampperiod){
				//scale down for the first few months since people do not refinance early
				CPRpathP[nsim][month] += refinCPR * month/uprampperiod;	
			}else{
				CPRpathP[nsim][month] += refinCPR;
			}
		}
	}
}

double cprPathGenerator::NormCdf(double d)
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