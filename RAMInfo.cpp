#include "RAMInfo.h"
#include "strconvert.h"

std::wstring RAMInfo::GetRamInfoText()
{
	 MEMORYSTATUSEX statex;
	 statex.dwLength = sizeof(MEMORYSTATUSEX);
	 GlobalMemoryStatusEx(&statex);
	 DWORD total_memory = (statex.ullTotalPhys / 1024 / 1024 / 1024);
	 std::string s = std::string("Total System Memory: ") + std::to_string(total_memory) + 
		 std::string(" GB");
	 std::wstring wstr = s2ws(s);
	 return wstr;
}

 double RAMInfo::GetRamUsage()
{
	MEMORYSTATUSEX memInfo;
	memInfo.dwLength = sizeof(MEMORYSTATUSEX);
	GlobalMemoryStatusEx(&memInfo);
	return (double(1.0) - double(memInfo.ullAvailPhys) / double(memInfo.ullTotalPhys)) * 100;
}
