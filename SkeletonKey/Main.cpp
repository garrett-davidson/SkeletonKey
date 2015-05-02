#include "SKModule.h"
#include <vector>
#include <Windows.h>
#include <iostream>

#include <strsafe.h>

using namespace std;
typedef wstring(__stdcall *f_runModule)(bool, wstring, wstring);

void ErrorExit(LPTSTR lpszFunction)
{
	// Retrieve the system error message for the last-error code

	LPVOID lpMsgBuf;
	LPVOID lpDisplayBuf;
	DWORD dw = GetLastError();

	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM |
		FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		dw,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf,
		0, NULL);

	// Display the error message and exit the process

	lpDisplayBuf = (LPVOID)LocalAlloc(LMEM_ZEROINIT,
		(lstrlen((LPCTSTR)lpMsgBuf) + lstrlen((LPCTSTR)lpszFunction) + 40) * sizeof(TCHAR));
	StringCchPrintf((LPTSTR)lpDisplayBuf,
		LocalSize(lpDisplayBuf) / sizeof(TCHAR),
		TEXT("%s failed with error %d: %s"),
		lpszFunction, dw, lpMsgBuf);
	MessageBox(NULL, (LPCTSTR)lpDisplayBuf, TEXT("Error"), MB_OK);

	LocalFree(lpMsgBuf);
	LocalFree(lpDisplayBuf);
	ExitProcess(dw);
}

wstring currentDirectory()
{
	wchar_t buffer[MAX_PATH];
	GetModuleFileName(NULL, buffer, MAX_PATH);
	wstring ws(buffer);
	int end = ws.find_last_of(L"\\");

	return ws.substr(0, end);
}

wstring userDirectory()
{
	wchar_t *buffer;
	size_t size = 0;
	_wdupenv_s(&buffer, &size, L"USERPROFILE");
	return buffer;
}

void writeLog(char *fileName, wstring output)
{
 	FILE *log;
	if (fopen_s(&log, fileName, "w") == 0)
	{
		fwprintf(log, output.c_str());

		fflush(log);
		fclose(log);
	}
}

wstring runModule(HINSTANCE module, bool isAdmin, wstring userHome, wstring outputDirectory)
{
	f_runModule runModule = (f_runModule)GetProcAddress(module, "runModule");
	wstring returnString = runModule(isAdmin, userHome, outputDirectory);

	return returnString;
}

vector<HINSTANCE> getModules()
{
	const wstring searchString = L"./modules/*.dll";
	vector<HINSTANCE> modules;

	WIN32_FIND_DATA fileData;
	HANDLE moduleHandle = FindFirstFile(searchString.c_str(), &fileData);
	if (moduleHandle != INVALID_HANDLE_VALUE)
	{
		do 
		{
			wstring s = L"./modules/";
			s.append(fileData.cFileName);
			modules.push_back(LoadLibrary(s.c_str()));
		} while (FindNextFile(moduleHandle, &fileData));
		FindClose(moduleHandle);
	}

	return modules;
}

int main()
{
	wstring cd = currentDirectory();
	wstring userHome = userDirectory().append(L"\\");
	wstring outputDirectory = cd.append(L"\\SkeletonKey\\");
	_wmkdir(outputDirectory.c_str());

	wstring *output = new wstring;

	for each (HINSTANCE module in getModules())
	{
		wstring b = runModule(module, false, userHome, cd);
		output->append(b);
	}

	writeLog("log.txt", *output);
	
	return EXIT_SUCCESS;
}