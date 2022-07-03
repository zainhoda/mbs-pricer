#ifndef _TERMSTRUCTURE_H_
#define _TERMSTRUCTURE_H_

#include "Array.h"
#include <fstream>
#include <vector>

using namespace std;

class TermStructure
{
	protected:
		Array<double,1>* swapRatePtr;
		Array<double,1>* readFile(string inFile);

	public:
		TermStructure(string rateFile) {
			swapRatePtr = readFile(rateFile);
		}

		virtual Array<double,2>* getNYearSwapRate(int n) = 0;
		virtual Array<double,2>* getShortrate() = 0;
};

#endif