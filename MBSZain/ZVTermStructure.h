#ifndef _ZVTERMSTRUCTURE_H_
#define _ZVTERMSTRUCTURE_H_

#include "TermStructure.h"
#include <string>

class ZVTermStructure : public TermStructure
{
public:
	ZVTermStructure(string rateFile) : TermStructure(rateFile) {}
	virtual Array<double,2>* getNYearSwapRate(int n);
	virtual Array<double,2>* getShortrate();
};

#endif