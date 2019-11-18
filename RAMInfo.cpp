#include "RAMInfo.h"

 double RAMInfo::GetRamUsage()
{
	MEMORYSTATUSEX memInfo;
	memInfo.dwLength = sizeof(MEMORYSTATUSEX);
	GlobalMemoryStatusEx(&memInfo);
	return (double(1.0) - double(memInfo.ullAvailPhys) / double(memInfo.ullTotalPhys)) * 100;
}