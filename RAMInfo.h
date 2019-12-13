#pragma once
#include <windows.h>
#include <string>

class RAMInfo
{
public:
	static std::wstring GetRamInfoText();
	static double GetRamUsage();
};

