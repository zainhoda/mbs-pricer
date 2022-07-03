#ifndef _CFGENERATOR_H_
#define _CFGENERATOR_H_
#include "time.h"
#include "stdafx.h"
#include "Array.h"
#include <string>
#include <fstream>
#include <iostream>	// required	for	the	input and output streams
using namespace std;

class CFgenerator{
private:
	double currMtgeRate;	//mortgage paid by homeowner
	double MBScpn;			//rate received by MBS holder
	Array<double,2> *CPRpaths;
public:
	CFgenerator(Array<double,2> *CPRpaths_, double currMtgeRate_, double MBScpn_);
	Array<double,2> *getCashFlows();
};

#endif