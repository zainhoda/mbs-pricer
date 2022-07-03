#include "Array.h"
#include "time.h"
#include "stdafx.h"
#include "CFgenerator.h"
#include <cmath>
#include <fstream>
#include <iostream>	// required	for	the	input and output streams
using namespace std;

CFgenerator::CFgenerator(Array<double,2> *CPRpaths_, double currMtgeRate_, double MBScpn_)
{
	CPRpaths = CPRpaths_;
	currMtgeRate = currMtgeRate_;
	MBScpn = MBScpn_;
}

Array<double,2> *CFgenerator::getCashFlows()
{
	//ofstream outfile("spppi20CPR.csv");
	Array<double,2> &CPRpathP = *CPRpaths;
	Array<double,2> *CFpaths = new Array<double,2>(CPRpathP.dim1(),CPRpathP.dim2());
	Array<double,2> &CFpathP = *CFpaths;

	int numSims = CFpathP.dim1();
	int numMths = CFpathP.dim2();
	double g = currMtgeRate/12.0;
	double c = MBScpn/12.0;
	double d = 1/(1+g);
	double SMM=0.0;
	double SP=0.0;
	double PP=0.0;
	double In =0.0;
	double Fn = 1.0;
	double Bn = 1.0;
	double Bnalt = 1.0;
	double Qn = Fn/Bn;

	for(int nsim=0; nsim<numSims; nsim++){
		SMM=0.0;
		SP=0.0;
		PP=0.0;
		In =0.0;
		Fn = 1.0;
		Bn = 1.0;
		Qn = Fn/Bn;
		Bnalt = 1.0;
		for(int month=1; month<numMths; month++){
			SMM = 1 - pow((1 - CPRpathP[nsim][month]/100),1.0/12);
			if(SMM!=SMM){
				SMM = CPRpathP[nsim][month]/100/12.0;
			}

			SP = g*(pow(d,numMths-month+1))/(1-pow(d,numMths));
			SP = Qn*SP;

			//outfile << "SP," << SP*100 << ",";

			In = c * (1.0 - pow(d,numMths-month+1))/(1.0 -pow(d,numMths));
			In = Qn*In;

			PP = (Bn - SP)*SMM;

			//outfile << PP*100 << ",";

			//outfile << In*100 << ",";

			CFpathP[nsim][month] = SP + PP + In;

			//outfile << "CF," << CFpathP[nsim][month] << endl;

			Bn = Bn - SP - PP;
			//outfile << nsim << "," << Bn <<endl;
			//check
			//Bnalt = Qn*(1-pow(d,numMths-month))/(1-pow(d,numMths));

			Qn = (1 - SMM)*Qn;

		}
	}

	//outfile.close();
	delete CPRpaths;
	return CFpaths;
}