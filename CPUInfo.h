#pragma once
#include <intrin.h>
#include <memory.h>
#include <iostream>
#include <windows.h>

class CpuInfo
{
public:
	CpuInfo();
	static void GetProcessorName();
	static double GetCpuUsage();

	static void InitializeSystemTimes();
	static DWORD GetProcessorCount();
private:
	static FILETIME currIdleTime_, lastIdleTime_;
	static FILETIME currKernelTime_, lastKernelTime_;
	static FILETIME currUserTime_, lastUserTime_;

	static ULONGLONG FileTimeToUlonglong(FILETIME* ft);
};
