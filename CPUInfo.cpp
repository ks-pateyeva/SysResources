#include "CPUInfo.h"
#include <sysinfoapi.h>
#include <intrin.h>
#include <string>
#include "strconvert.h"

FILETIME CpuInfo::currIdleTime_ = { 0,0 };
FILETIME CpuInfo::lastIdleTime_ = { 0,0 };
FILETIME CpuInfo::currKernelTime_ = { 0,0 };
FILETIME CpuInfo::lastKernelTime_ = { 0,0 };
FILETIME CpuInfo::currUserTime_ = { 0,0 };
FILETIME CpuInfo::lastUserTime_ = { 0,0 };

CpuInfo::CpuInfo()
{
};


void CpuInfo::InitializeSystemTimes()
{
	GetSystemTimes(&lastIdleTime_, &lastKernelTime_, &lastUserTime_);
}

std::wstring CpuInfo::GetCpuInfoText()
{	
	std::string s(GetProcessorName());
	s += std::string("\nNumber of Cores: ") + std::to_string(GetProcessorCount());
	std::wstring wstr = s2ws(s);
	return wstr;
}

DWORD CpuInfo::GetProcessorCount()
{
	SYSTEM_INFO sysInfo;
	GetSystemInfo(&sysInfo);
	return sysInfo.dwNumberOfProcessors;
}

ULONGLONG CpuInfo::FileTimeToUlonglong(FILETIME* ft)
{
	ULARGE_INTEGER ui;
	ui.LowPart = ft->dwLowDateTime;
	ui.HighPart = ft->dwHighDateTime;
	return ui.QuadPart;
}


std::string CpuInfo::GetProcessorName()
{
	int cpuInfo[4] = {-1};
	char cpuBrandString[0x40];
	__cpuid(cpuInfo, 0x80000000);
	unsigned int nExIds = cpuInfo[0];

	memset(cpuBrandString, 0, sizeof(cpuBrandString));

	// Get the information associated with each extended ID.
	for (int i = 0x80000000; i <= nExIds; ++i)
	{
		__cpuid(cpuInfo, i);
		// Interpret CPU brand string.
		if (0x80000002 == i)
			memcpy(cpuBrandString, cpuInfo, sizeof(cpuInfo));
		else if (i == 0x80000003)
			memcpy(cpuBrandString + 16, cpuInfo, sizeof(cpuInfo));
		else if (i == 0x80000004)
			memcpy(cpuBrandString + 32, cpuInfo, sizeof(cpuInfo));
	}
	return std::string(cpuBrandString);
}

double CpuInfo::GetCpuUsage()
{
	GetSystemTimes(&currIdleTime_, &currKernelTime_, &currUserTime_);
	ULONGLONG userTime = FileTimeToUlonglong(&currUserTime_) - FileTimeToUlonglong(&lastUserTime_);
	ULONGLONG kernelTime = FileTimeToUlonglong(&currKernelTime_) - FileTimeToUlonglong(&lastKernelTime_);
	ULONGLONG idleTime = FileTimeToUlonglong(&currIdleTime_) - FileTimeToUlonglong(&lastIdleTime_);
	ULONGLONG sysTime = kernelTime + userTime;
	double cpuUsage = (sysTime - idleTime) * 100.0 / (sysTime);
	
	lastIdleTime_ = currIdleTime_;
	lastKernelTime_ = currKernelTime_;
	lastUserTime_ = currUserTime_;
	return cpuUsage;
}
