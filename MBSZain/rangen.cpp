#include "stdafx.h"
#include "Array.h"
#include "rangen.h"
#include <stdio.h>
#include <fstream>
#include <iostream>	// required	for	the	input and output streams
#include <math.h>	// required	to use the exponential function
using namespace std;

# define pi 3.14159265358979323846

Array<double> vectUnifSim(int num){
	Array<double> result(num);
	for(int i=0; i<num; i++){
		result[i] = rand()/double(RAND_MAX);
	}
	return result;
}

double normsim(){
	double x1,x2,w,result;
	do {
		x1 = 2.0 * rand()/double(RAND_MAX) - 1.0;
		x2 = 2.0 * rand()/double(RAND_MAX) - 1.0;
		w = x1 * x1 + x2 * x2;
	} while ( w >= 1.0 );

	w = sqrt( (-2.0 * log( w ) ) / w );
	result = x1 * w;
	return result;
}

Array<double,2> twoDnormsim(int firstDim, int secondDim, double sig){
	Array<double,2> result(firstDim,secondDim);
	double x1,x2,w,norma,normb;
	int numNorms;
	
	numNorms = firstDim*secondDim;
	if(numNorms%2!=0){
		result[firstDim-1][secondDim-1] = normsim()*sig;
		numNorms--;
	}
	for(int i=0; i<numNorms-1;i++){
		do {
			x1 = 2.0 * rand()/double(RAND_MAX) - 1.0;
			x2 = 2.0 * rand()/double(RAND_MAX) - 1.0;
			w = x1 * x1 + x2 * x2;
		} while ( w >= 1.0 );

		w = sqrt( (-2.0 * log( w ) ) / w );
		norma = x1 * w;
		normb = x2 * w;
		result[i/secondDim][i%secondDim]=norma*sig;
		i++;
		result[i/secondDim][i%secondDim]=normb*sig;
	}
	return result;
}

double gammasim(double a, double b){
	double beta, y, z, U;
	bool valid=false;
	if(a<=1){
		beta = (a+exp(1.0))/(exp(1.0));
		while(!valid){
			y=beta * rand()/double(RAND_MAX);
			U = rand()/double(RAND_MAX);
			if(y<=1){
				z = pow(y,1/a);
				if(U<exp(-z))valid=true;
			}else{
				z = -log((beta-y)/a);
				if(U<=pow(z,a-1))valid=true;
			}
		}
		return b*z;
	}else{
		double alpha, d, m, v;
		alpha = a-1;
		beta = (a-(1/6/a))/alpha;
		m=2/alpha;
		d=m+2;
		while(!valid){
			U=rand()/double(RAND_MAX);
			v=beta*rand()/double(RAND_MAX)/U;
			if(m*U-d+v+(1/v)<=0){
				valid=true;
			}else{
				if(m*log(U)-log(v)+v-1<=0)valid=true;
			}
		}
		return alpha*b*v;
	}
}

double nchisim(double d, double alpha){
	int v;
	v=poissonsim(alpha/2);
	return gammasim((d+double(2*v))/2,2);
}

int poissonsim(double lambda){
	double Tcurrent = 0;
	double Tnew;
	int counter = -1;
	while(Tcurrent<1){
		Tnew = exponentialsim(lambda);
		Tcurrent += Tnew;
		counter++;
	}
	return counter;
}

double exponentialsim(double lambda){
	double unif,x;
	x = -log(1-rand()/double(RAND_MAX))/lambda;
	while(x!=x){
		unif = rand()/double(RAND_MAX);
		x = -log(1-unif)/lambda;
	}
	return x;
}

