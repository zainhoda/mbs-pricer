// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the MBSDLL_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// MBSDLL_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef MBSDLL_EXPORTS
#define MBSDLL_API __declspec(dllexport)
#else
#define MBSDLL_API __declspec(dllimport)
#endif



extern "C" MBSDLL_API int _stdcall initializeMBS(int startMonth, double currMtgeRate, double MBScpn, double alpha, double beta, double a, double sigma, char* rateFilePtr, char* seasonalFilePtr);
extern "C" MBSDLL_API double _stdcall mcPrice(double oas, int nsim, char* model);
extern "C" MBSDLL_API double _stdcall ZVspread(double price, double guess, char* model);
extern "C" MBSDLL_API double _stdcall OAspread(double price, double guess, int nsim, char* model);
extern "C" MBSDLL_API double _stdcall OAduration(int nsim, char* model, double OAS, double price);
extern "C" MBSDLL_API double _stdcall OAconvexity(int nsim, char* model, double OAS, double price);
