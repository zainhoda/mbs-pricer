#include "stdafx.h"
#include "TermStructure.h"

Array<double,1>* TermStructure::readFile(string inFile) {
	ifstream inputFile(inFile.c_str());
	vector<double> rate;
	while (inputFile) {
		double temp;
		inputFile >> temp;
		rate.push_back(temp);
	}

	Array<double,1>* Ptr = new Array<double,1>(rate.size());

	for (int i = 0; i < rate.size(); i++) {
		(*Ptr)[i] = rate[i];
	}

	return Ptr;
}