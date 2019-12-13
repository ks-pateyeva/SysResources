#pragma once
#include <Wbemidl.h>
#include <string>

class DiskInfo
{
public:
	static std::wstring GetDiskDriveCaption();
	int static GetDiskUsage();
	float static GetDiskSpeed(LPCWSTR wszName);
private:
	void static InitStorageCom();
	void static InitCimCom();
	static HRESULT hres;
	static IWbemServices* pSvc;
	static IWbemLocator* pLoc;
	static IEnumWbemClassObject* pEnumerator;
};
