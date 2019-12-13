#pragma once
#include <Wbemidl.h>
#include <string>

class WiFiInfo
{
public:
	static void InitCom();
	static std::wstring GetNetworkAdapterCaption();
	static float GetSpeed(LPCWSTR wszName);
private:
	static HRESULT hres;
	static  IWbemServices* pSvc;
	static  IWbemLocator* pLoc;
	static  IEnumWbemClassObject* pEnumerator;
};

