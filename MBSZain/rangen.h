#include "stdafx.h"
#include <math.h>	// required	to use the exponential function
using namespace std;

//remember to put srand(time(null)) into the first line of main cpp file!

Array<double> vectUnifSim(int num);
double normsim();
Array<double,2> twoDnormsim(int firstDim, int secondDim, double var);
double gammasim(double a, double b);
double nchisim(double d, double alpha);
double nchisimExt(double d, double alpha);
int poissonsim(double lambda);
double exponentialsim(double lambda);

