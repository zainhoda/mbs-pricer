#ifndef _TERMSTRUCTURE_H_
#define _TERMSTRUCTURE_H_

#include "Array.h"
#include <fstream>
#include <vector>
#include <iostream>	// required	for	the	input and output streams
using namespace std;

using namespace std;

class TermStructure
{
	protected:
		Array<double,1>* swapRatePtr;
		Array<double,1>* readFile(string inFile,double offset=0);
		Array<double,1>* readAndExtrapolate(string inFile);

	public:
		TermStructure(string rateFile, bool complete, double offset=0) {
			if(complete){
				//cout << offset << endl;
				swapRatePtr = readFile(rateFile,offset);
			}else{
				swapRatePtr = readAndExtrapolate(rateFile);
			}
		}

		virtual Array<double,2>* getNYearSwapRate(int n) = 0;
		virtual Array<double,2>* getShortrate() = 0;
		//virtual Array<double,1>* getSwapRatePtr(){return swapRatePtr;}
};

#endif