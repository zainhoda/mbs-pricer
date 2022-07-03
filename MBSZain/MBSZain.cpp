// MBSZain.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "TermStructure.h"
#include "SimulatedTermStructure.h"
#include "ZVTermStructure.h"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "MBS.h"

using namespace std;

int main()
{
	srand(time(NULL));
	char wait;
	//SimulatedTermStructure* s = new SimulatedTermStructure();
	
	//cout << s->getTest();
	//SimulatedTermStructure* zv = new SimulatedTermStructure("rateFile.txt", 1, 1, 1, "none");
	/*
	Array<double,2>& sr = *(zv->getShortrate());

	
	for (int i = 0; i < sr.dim2(); i++) {
		cout << sr[0][i] << endl;
	}*/

	MBS* mbs = new MBS(1, 0.05, 0.05, 1.94, 0.86, 0, 1, 1, "rateFile.txt", "none", "testseasonality.txt");
	double price = mbs->getPrice(0.01, 100);
	cout << "Price = " << price << endl;
	/*
	cout << mbs->getPrice(0.015, 100) <<endl;
	cout << mbs->getPrice(0.02, 100) <<endl;
	cout << mbs->getPrice(0.025, 100) <<endl;
	cout << mbs->getPrice(0.03, 100) <<endl;
	cout << mbs->getPrice(0.035, 100) <<endl;
	*/
	cout << "ZV Spread = " << mbs->ZVspread(price, 0.02) << endl;
	cout << "OAS Spread = " << mbs->OAspread(price, 0.02, 100) << endl;
	cin >> wait;
	
	return 0;
}

