#pragma once
#include <windows.h>
#include <string>

class CpuInfo
{
public:
	CpuInfo();
	static std::string GetProcessorName();
	static double GetCpuUsage();
	static DWORD GetProcessorCount();
	static void InitializeSystemTimes();
	static std::wstring GetCpuInfoText();
private:
	static FILETIME currIdleTime_, lastIdleTime_;
	static FILETIME currKernelTime_, lastKernelTime_;
	static FILETIME currUserTime_, lastUserTime_;
	static ULONGLONG FileTimeToUlonglong(FILETIME* ft);
};
