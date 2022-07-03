#ifndef _ONEFACTOROLS_H_
#define _ONEFACTOROLS_H_

#include "time.h"
#include "stdafx.h"
#include "Array.h"
#include <string>
#include <fstream>
#include <iostream>	// required	for	the	input and output streams
using namespace std;

inline double operator *(Array<double,1>& arr1, Array<double,1>& arr2);

class onefactorOLS{
private:
	double intercept;
	double coef;
	Array<double,1>* yValues;
	Array<double,1>* xValues;
	double expectation(Array<double,1>& arr);

public:
	onefactorOLS(Array<double,1>* yVals_, Array<double,1>* xVals_){
		yValues = yVals_;
		xValues = xVals_;

		Array<double,1>& yArr = (*yValues);
		Array<double,1>& xArr = (*xValues);

		double varX, covXY, EX, EY;
		EX = expectation(xArr);
		//cout << EX << endl;
		EY = expectation(yArr);
		//cout << EY << endl;
		varX = (xArr * xArr)/xArr.size() - EX * EX;
		covXY = (yArr * xArr)/xArr.size() - EX * EY;

		coef = covXY/varX;
		intercept = EY - coef * EX;
	}
	double getIntercept(){return intercept;};
	double getCoefficient(){return coef;};
};

double onefactorOLS::expectation(Array<double,1>& arr){
	double result=0;
	for(int i=0; i<arr.dim1(); i++){
		result += arr[i];
	}
	return result/arr.dim1();
}

inline double operator *(Array<double,1>& arr1, Array<double,1>& arr2){
	double result=0;
	if(arr1.dim1() != arr2.dim1()){
		return result;
	}else{
		for(int i=0; i<arr1.dim1(); i++){
			result += arr1[i] * arr2[i];
		}
		return result;
	}
}

#endif