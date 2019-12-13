#include "WiFiInfo.h"
#define _WIN32_DCOM
#include <iostream>
using namespace std;
#include <comdef.h>
#include <Wbemidl.h>

#pragma comment(lib, "wbemuuid.lib")

HRESULT WiFiInfo::hres = NULL;
IWbemServices* WiFiInfo::pSvc = NULL;
IWbemLocator* WiFiInfo::pLoc = NULL;
IEnumWbemClassObject* WiFiInfo::pEnumerator = NULL;

void WiFiInfo::InitCom()
{
	hres = CoInitializeEx(0, COINIT_MULTITHREADED);
	if (FAILED(hres))
	{
		// Program has failed.
	}

	hres = CoInitializeSecurity(
		NULL,
		-1, // COM authentication
		NULL, // Authentication services
		NULL, // Reserved
		RPC_C_AUTHN_LEVEL_DEFAULT, // Default authentication 
		RPC_C_IMP_LEVEL_IMPERSONATE, // Default Impersonation  
		NULL, // Authentication info
		EOAC_NONE, // Additional capabilities 
		NULL // Reserved
	);


	if (FAILED(hres))
	{
		CoUninitialize();
	}

	hres = CoCreateInstance(
		CLSID_WbemLocator,
		0,
		CLSCTX_INPROC_SERVER,
		IID_IWbemLocator, (LPVOID*)&pLoc);

	if (FAILED(hres))
	{
		CoUninitialize();
	}

	hres = pLoc->ConnectServer(
		_bstr_t(L"Root\\CIMV2"), // Object path of WMI namespace
		NULL, // User name. NULL = current user
		NULL, // User password. NULL = current
		0, // Locale. NULL indicates current
		NULL, // Security flags.
		0, // Authority (for example, Kerberos)
		0, // Context object 
		&pSvc // pointer to IWbemServices proxy
	);

	if (FAILED(hres))
	{
		pLoc->Release();
		CoUninitialize();
	}

	hres = CoSetProxyBlanket(
		pSvc, // Indicates the proxy to set
		RPC_C_AUTHN_WINNT, // RPC_C_AUTHN_xxx
		RPC_C_AUTHZ_NONE, // RPC_C_AUTHZ_xxx
		NULL, // Server principal name 
		RPC_C_AUTHN_LEVEL_CALL, // RPC_C_AUTHN_LEVEL_xxx 
		RPC_C_IMP_LEVEL_IMPERSONATE, // RPC_C_IMP_LEVEL_xxx
		NULL, // client identity
		EOAC_NONE // proxy capabilities 
	);

	if (FAILED(hres))
	{
		pSvc->Release();
		pLoc->Release();
		CoUninitialize();
	}

	hres = pSvc->ExecQuery(
		bstr_t("WQL"),
		bstr_t("SELECT * FROM Win32_PerfFormattedData_Tcpip_NetworkInterface"),
		WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY,
		NULL,
		&pEnumerator);

	if (FAILED(hres))
	{
		pSvc->Release();
		pLoc->Release();
		CoUninitialize();
	}
}

std::wstring WiFiInfo::GetNetworkAdapterCaption()
{
	InitCom();
	// Step 7: -------------------------------------------------
	// Get the data from the query in step 6 -------------------

	IWbemClassObject* pclsObj = NULL;
	ULONG uReturn = 0;

	HRESULT hr = pEnumerator->Next(WBEM_INFINITE, 1,
	                               &pclsObj, &uReturn);

	VARIANT vtProp;

	// Get the value of the Name property
	hr = pclsObj->Get(L"Name", 0, &vtProp, 0, 0);
	std::wstring wifiAdapter = vtProp.bstrVal;
	VariantClear(&vtProp);

	pclsObj->Release();
	// Cleanup
	// ========

	pSvc->Release();
	pLoc->Release();
	pEnumerator->Release();
	CoUninitialize();

	return wifiAdapter;
}

float WiFiInfo::GetSpeed(LPCWSTR wszName)
{
	InitCom();

	IWbemClassObject* pclsObj = NULL;
	ULONG uReturn = 0;
	HRESULT hr = pEnumerator->Next(WBEM_INFINITE, 1,
	                               &pclsObj, &uReturn);
	VARIANT vtProp;

	// Get the value of the Name property
	hr = pclsObj->Get(wszName, 0, &vtProp, 0, 0);
	int i = std::stoi(vtProp.bstrVal);
	float speed = i / 100;
	VariantClear(&vtProp);

	pclsObj->Release();

	pSvc->Release();
	pLoc->Release();
	pEnumerator->Release();
	CoUninitialize();

	return speed; // Program successfully completed.
}
