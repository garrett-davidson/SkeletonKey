#include "SKModule.h"

using namespace std;

namespace SkeletonKey
{
	void logLine(wstring line)
	{
		//output += line + L"\n";
	}

	wstring run(bool isAdmin, wstring userHome, wstring outputPath)
	{
		logLine(L"Running new module");
		return L"New module";
	}

}