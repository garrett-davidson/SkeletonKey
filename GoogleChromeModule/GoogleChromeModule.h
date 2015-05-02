// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the GOOGLECHROMEMODULE_EXPORTS
// symbol defined on the command line. This symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// GOOGLECHROMEMODULE_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.

#ifdef GOOGLECHROMEMODULE_EXPORTS
#define GOOGLECHROMEMODULE_API __declspec(dllexport)
#else
#define GOOGLECHROMEMODULE_API __declspec(dllimport)
#endif

#include <string>
#include "sqlite3.h"


enum LoginDataColumns : int
{
	ACTION_URL = 0,
	USERNAME_VALUE = 1,
	PASSWORD_VALUE = 2
};

// This class is exported from the GoogleChromeModule.dll
class GOOGLECHROMEMODULE_API CGoogleChromeModule {

public:
	CGoogleChromeModule(void);
	// TODO: add your methods here.
	std::wstring runModule(bool isAdmin, std::wstring userHome, std::wstring ouputPath);
};

//extern GOOGLECHROMEMODULE_API int nGoogleChromeModule;

//GOOGLECHROMEMODULE_API int fnGoogleChromeModule(void);
