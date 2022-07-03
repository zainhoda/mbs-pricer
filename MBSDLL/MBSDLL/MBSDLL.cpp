// MBSDLL.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "MBSDLL.h"
#include <string>
#include "MBS.h"
#include <fstream>

using namespace std;

#ifdef _MANAGED
#pragma managed(push, off)
#endif

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
    return TRUE;
}

#ifdef _MANAGED
#pragma managed(pop)
#endif

// When ready to integrate, uncomment this and comment the rest

MBS* mbs;

extern "C" MBSDLL_API int _stdcall initializeMBS(int startMonth, double currMtgeRate, double MBScpn, double alpha, double beta, double a, double sigma, char* rateFilePtr, char* seasonalFilePtr)
{
	static bool initialized = false;
	if (initialized) {
		delete mbs;
	}
	string rateFile(rateFilePtr);
	string seasonalFile(seasonalFilePtr);

	ifstream file1(rateFilePtr);
	ifstream file2(seasonalFilePtr);
	if (!file1) {
		return 1;
	} else if (!file2) {
		return 2;
	}
	file1.close();
	file2.close();

	mbs = new MBS(startMonth, currMtgeRate, MBScpn, alpha, beta, 0, a, sigma, true, rateFile, "none", seasonalFile);
	initialized = true;
	return 0;
}

extern "C" MBSDLL_API double _stdcall mcPrice(double oas, int nsim, char* model)
{
	string temp(model);
	return mbs->getPrice(oas, nsim, temp);
}

extern "C" MBSDLL_API double _stdcall ZVspread(double price, double guess, char* model)
{
	string temp(model);
	return mbs->ZVspread(price, guess, temp);
}

extern "C" MBSDLL_API double _stdcall OAspread(double price, double guess, int nsim, char* model)
{
	string temp(model);
	return mbs->OAspread(price, guess, nsim, temp);
}

extern "C" MBSDLL_API double _stdcall OAduration(int nsim, char* model, double OAS, double price)
{
	string temp(model);
	return mbs->OAduration(nsim, temp, true, OAS, price);
}

extern "C" MBSDLL_API double _stdcall OAconvexity(int nsim, char* model, double OAS, double price)
{
	string temp(model);
	return mbs->OAconvexity(nsim, temp, true, OAS, price);
}

/*
extern "C" MBSDLL_API int _stdcall initializeMBS(int startMonth, double currMtgeRate, double MBScpn, double a, double sigma, char* rateFilePtr, char* seasonalFilePtr)
{
	string rateFile(rateFilePtr);
	if (rateFile == "rateFile.txt") {
		return 47;
	}
	return 12;
}

extern "C" MBSDLL_API double _stdcall mcPrice(double oas, int nsim)
{
	double blah = oas;
	for (int i = 0; i < nsim; i++) {
		blah = blah + 0.01;
	}
	return oas;
}

extern "C" MBSDLL_API double _stdcall ZVspread(double price, double guess)
{
	return 5;
}

extern "C" MBSDLL_API double _stdcall OAspread(double price, double guess, int nsim)
{
	return 6;
}
*/