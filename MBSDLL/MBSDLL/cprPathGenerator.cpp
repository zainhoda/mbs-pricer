
#include "Array.h"
#include "time.h"
#include "stdafx.h"
#include "cprPathGenerator.h"
#include <cmath>
#include <fstream>
#include <iostream>	// required	for	the	input and output streams
#include <assert.h>
using namespace std;


cprPathGenerator::cprPathGenerator(Array<double,2> *mtgeRatePath_, string seasonality_, int startMonth_, double currMtgeRate_, string type_)
{
	currMtgeRate = currMtgeRate_;
	mtgeRatePath = mtgeRatePath_;
	seasonalityFile = seasonality_;
	startMonth = startMonth_;
	type = type_;
	upramp = 0.2;
	uprampperiod = 30;
}

Array<double,2>* cprPathGenerator::getCPRpath()
{
	Array<double,2> &mtgeRateP = *mtgeRatePath;
	Array<double,2> *CPRpaths = new Array<double,2>(mtgeRateP.dim1(),mtgeRateP.dim2());
	Array<double,2> &CPRpathP = *CPRpaths;

	addTurnoverRate(CPRpathP);

	if(type=="A"){
		addRefinancingRate(CPRpathP, mtgeRateP);
	}else{
		CPRmethodB(CPRpathP,mtgeRateP);
	}

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
	//ofstream outfile("incentiveToCPR55mtge.csv");
	double incentive=0.0;
	double unif=0;
	double proportionInterest=0.0;
	double survivalRate=1.0;
	double refinCPR=0;
	double midMaxCPR=200.0;	//tops out at 60 + 6 = 66
	//double currMaxCPR=200.0;
	double maxCPR = 60.0;
	double aveBPdiff=50.0;
	double bpFactor = 10000.0;
	//if you increase scale factor you make extreme events more likely
	double scaleFactor = 120.0;	
	//maxCPR += (rand()/double(RAND_MAX) - 0.5) * 10;

	double g = currMtgeRate/12.0;
	double d = 1/(1+g);
	int numMths = CPRpathP.dim2();
	double SPcurr;
	SPcurr = g*(pow(d,numMths))/(1-pow(d,numMths))*d;	//*d because /d later
	double BM,Fn,Qn,SMM,SPtild,PP,Bnhalf;
	double Bnohalf = 1/(1-pow(d,numMths));

	for(int nsim=0; nsim<CPRpathP.dim1(); nsim++){
		if(nsim==1){
			nsim =1;
		}
		survivalRate = 1.0;
		Bnhalf = pow(d,numMths)/(1-pow(d,numMths));
		BM = 1.0;
		Fn = 1.0;
		Qn = Fn/BM;
		for(int month=1; month<CPRpathP.dim2(); month++){
			//from studies, people react to one month lagged incentive rate
			incentive = currMtgeRate - mtgeRateP[nsim][month-1];
			//if(month>119 )outfile << nsim << "," << incentive << ",";
			proportionInterest = NormCdf((incentive-aveBPdiff/bpFactor)*scaleFactor);

			refinCPR=0.0;
			
			/*if(proportionInterest > (1-survivalRate)){
				refinCPR = ((proportionInterest - (1 - survivalRate))/survivalRate)*maxCPR;
				//survivalRate = 1 - proportionInterest;
			}*/
			
			/*do{
				unif = rand()/double(RAND_MAX);
			}while(unif<0 || unif>1);
			currMaxCPR = midMaxCPR + (unif - 0.5)*50;*/
			refinCPR = proportionInterest*midMaxCPR*(0.3+0.7*BM);

			if(month<uprampperiod){
				//scale down for the first few months since people do not refinance early
				CPRpathP[nsim][month] += refinCPR * month/uprampperiod;	
			}else{
				CPRpathP[nsim][month] += refinCPR;
			}
			//if(month>119 )outfile << CPRpathP[nsim][month] << endl;
			//assert(CPRpathP[nsim][month] > 0);
			SMM = 1 - pow((1 - CPRpathP[nsim][month]/100),1.0/12);
			if(SMM!=SMM){
				SMM = CPRpathP[nsim][month]/100/12.0;
			}
			//assert(SMM == SMM);
			//assert(SMM > 0);
			
			Bnhalf = Bnhalf/d;
			Qn = (1 - SMM)*Qn;
			BM = (Bnohalf - Bnhalf)*Qn;
			
			//survivalRate *= (1-SMM);
		}
		//outfile <<endl;
	}
	//outfile.close();
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

void cprPathGenerator::CPRmethodB(Array<double,2> &CPRpathP, Array<double,2> &mtgeRateP)
{
	//ofstream outfile("cprb5.csv");
	//note this function assumes you passed in the 10 yr zcb rate path instead of the mtge rate path
	
	int CPRscalingFactor = 100;		//model gives CPR in decimals.  Want CPR in percent
	double upRampShrink = 6.0;		//addTurnover reaches a peak of 6, we want it to peak at 1 instead.
	double RI=0;
	double BM=1;
	double Qn;
	double Fn;
	double SMM;
	double SP;
	double PP;
	double g = currMtgeRate/12.0;
	double d = 1/(1+g);
	int numMths = CPRpathP.dim2();

	for(int nsim=0; nsim<CPRpathP.dim1(); nsim++){
		BM = 1.0;
		Fn = 1.0;
		Qn = Fn/BM;
		CPRpathP[nsim][0] = 0;	//at start, no CPR of course!
		for(int month=1; month<CPRpathP.dim2(); month++){
			RI = 0.28 + 0.14*atan(-8.571 + 430*(currMtgeRate - mtgeRateP[nsim][month-1]));
			//if(month>119)outfile<<currMtgeRate - mtgeRateP[nsim][month-1] << ",";
			CPRpathP[nsim][month] *= (RI*(0.3+0.7*BM)/upRampShrink)*CPRscalingFactor;
			//if(month>119)outfile<<CPRpathP[nsim][month] << endl;
			SMM = 1 - pow((1 - CPRpathP[nsim][month]/100),1.0/12);
			if(SMM!=SMM){
				SMM = CPRpathP[nsim][month]/100/12.0;
			}
			SP = g*(pow(d,numMths-month+1))/(1-pow(d,numMths));
			SP = Qn*SP;
			PP = (BM - SP)*SMM;
			BM = BM - SP - PP;
			Qn = (1 - SMM)*Qn;
			
			//outfile << BM << endl;
		}
	}
	//outfile.close();
}