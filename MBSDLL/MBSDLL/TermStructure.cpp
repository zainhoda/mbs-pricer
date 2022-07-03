#include "stdafx.h"
#include "TermStructure.h"
#include <iostream>	// required	for	the	input and output streams
using namespace std;

Array<double,1>* TermStructure::readFile(string inFile,double offset) {
	ifstream inputFile(inFile.c_str());
	vector<double> rate;
	while (inputFile) {
		double temp;
		inputFile >> temp;
		//cout << temp <<"," <<temp + offset << endl;
		rate.push_back(temp+offset);
	}

	Array<double,1>* Ptr = new Array<double,1>(rate.size());

	for (int i = 0; i < rate.size(); i++) {
		(*Ptr)[i] = rate[i];
	}

	return Ptr;
}

Array<double,1>* TermStructure::readAndExtrapolate(string inFile){
	ifstream inputFile(inFile.c_str());
	vector<double> rate;
	vector<double> maturity;
	bool readRate = true;
	int freq = 12;	// for 12 months
	int lastEntry = 0;
	int currEntry = 0;	
	double temp=0;
	//tracks the current maturity in the maturity vector that is larger than but closest to
	//the maturity in the Ptr array.

	//assumes input file is space delimited, each line containing rate, then maturity
	//if more rates than maturities, ignores last rate
	while (inputFile) {
		inputFile >> temp;
		if(readRate){
			rate.push_back(temp);
		}else{
			maturity.push_back(temp);
		}
		readRate = !readRate;
	}

	lastEntry = rate.size()>maturity.size()? maturity.size():rate.size();

	//assumes last rate in input is last rate in output

	Array<double,1>* Ptr = new Array<double,1>(maturity[lastEntry]*freq+1);	//plus one since want 361 slots

	for (int month = 0; month < (*Ptr).dim1(); month++) {
		if(month==maturity[currEntry]*freq){
			(*Ptr)[month] = rate[currEntry++];	
			//add 1 to currEntry so that next time, we're checking the next entry
		}else{
			//either equal or less than, since once we reach equal we increase currEntry
			if(currEntry==0){
				(*Ptr)[month] = rate[currEntry];	//any month less than first maturity is just rate of first maturity
			}else{
				//linear interpolation
				temp = ((month - maturity[currEntry-1]*freq)*rate[currEntry] + (maturity[currEntry]*freq - month)*rate[currEntry-1])/freq/(maturity[currEntry] - maturity[currEntry-1]);
				(*Ptr)[month] = temp;
			}
		}
	}

	return Ptr;
}