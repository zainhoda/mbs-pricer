#ifndef _ZVTERMSTRUCTURE_H_
#define _ZVTERMSTRUCTURE_H_

#include "TermStructure.h"
#include <string>

class ZVTermStructure : public TermStructure
{
public:
	ZVTermStructure(string rateFile, bool complete=true) : TermStructure(rateFile,complete) {}
	virtual Array<double,2>* getNYearSwapRate(int n);
	virtual Array<double,2>* getShortrate();
};

#endif