#include "DiskInfo.h"
#include <comutil.h>
#include <iostream>


HRESULT DiskInfo::hres = NULL;
IWbemServices* DiskInfo::pSvc = NULL;
IWbemLocator* DiskInfo::pLoc = NULL;
IEnumWbemClassObject* DiskInfo::pEnumerator = NULL;

int DiskInfo::GetDiskUsage()
{
	InitCimCom();
	IWbemClassObject* pclsObj = NULL;
	ULONG uReturn = 0;

	HRESULT hr = pEnumerator->Next(WBEM_INFINITE, 1,
	                               &pclsObj, &uReturn);

	VARIANT vtProp;

	// Get the value of the Name property
	hr = pclsObj->Get(L"PercentDiskTime", 0, &vtProp, 0, 0);
	int i = std::stoi(vtProp.bstrVal);
	VariantClear(&vtProp);

	pclsObj->Release();


	pSvc->Release();
	pLoc->Release();
	pEnumerator->Release();
	CoUninitialize();

	return i;
}

void DiskInfo::InitCimCom()
{
	hres = CoInitializeEx(0, COINIT_MULTITHREADED);
	if (FAILED(hres))
	{
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
		CoUninitialize(); // Program has failed.
	}

	hres = CoCreateInstance(
		CLSID_WbemLocator,
		0,
		CLSCTX_INPROC_SERVER,
		IID_IWbemLocator, (LPVOID*)&pLoc);

	if (FAILED(hres))
	{
		CoUninitialize();
		// Program has failed.
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
		// Program has failed.
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
		bstr_t("SELECT * FROM Win32_PerfFormattedData_PerfDisk_PhysicalDisk"),
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


std::wstring DiskInfo::GetDiskDriveCaption()
{
	InitStorageCom();
	IWbemClassObject* pclsObj = NULL;
	ULONG uReturn = 0;

	HRESULT hr = pEnumerator->Next(WBEM_INFINITE, 1,
	                                           &pclsObj, &uReturn);

	VARIANT vtProp;

	// Get the value of the Name property
	hr = pclsObj->Get(L"FriendlyName", 0, &vtProp, 0, 0);
	std::wstring diskCaption = vtProp.bstrVal;
	hr = pclsObj->Get(L"Size", 0, &vtProp, 0, 0);
	std::wstring ws(vtProp.bstrVal, SysStringLen(vtProp.bstrVal));
	ws.resize(ws.size() - 9);
	diskCaption += L"	Capacity: " + ws + L" GB";
	VariantClear(&vtProp);

	pclsObj->Release();

	return diskCaption;
}

float DiskInfo::GetDiskSpeed(LPCWSTR wszName)
{
	InitCimCom();
	IWbemClassObject* pclsObj = NULL;
	ULONG uReturn = 0;

	HRESULT hr = pEnumerator->Next(WBEM_INFINITE, 1,
	                                           &pclsObj, &uReturn);

	VARIANT vtProp;

	// Get the value of the Name property
	hr = pclsObj->Get(wszName, 0, &vtProp, 0, 0);
	int i = std::stoi(vtProp.bstrVal);
	float speed = i / 1024 / 5;
	VariantClear(&vtProp);

	pclsObj->Release();

	pSvc->Release();
	pLoc->Release();
	pEnumerator->Release();
	CoUninitialize();

	return speed;
}

void DiskInfo::InitStorageCom()
{
	hres = CoInitializeEx(0, COINIT_MULTITHREADED);
	if (FAILED(hres))
	{
	}

	hres = CoInitializeSecurity(
		NULL,
		-1, // COM authentication
		NULL, // Authentication services
		NULL, // Reserved
		RPC_C_AUTHN_LEVEL_DEFAULT, // Default authentication 
		RPC_C_IMP_LEVEL_DELEGATE, // Default Impersonation  
		NULL, // Authentication info
		EOAC_NONE, // Additional capabilities 
		NULL // Reserved
	);


	if (FAILED(hres))
	{
		CoUninitialize(); // Program has failed.
	}

	hres = CoCreateInstance(
		CLSID_WbemLocator,
		0,
		CLSCTX_INPROC_SERVER,
		IID_IWbemLocator, (LPVOID*)& pLoc);

	if (FAILED(hres))
	{
		CoUninitialize();
		// Program has failed.
	}

	hres = pLoc->ConnectServer(
		_bstr_t(L"Root\\Microsoft\\Windows\\Storage"), // Object path of WMI namespace
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
		// Program has failed.
	}

	hres = CoSetProxyBlanket(
		pSvc, // Indicates the proxy to set
		RPC_C_AUTHN_WINNT, // RPC_C_AUTHN_xxx
		RPC_C_AUTHZ_NONE, // RPC_C_AUTHZ_xxx
		NULL, // Server principal name 
		RPC_C_AUTHN_LEVEL_CALL, // RPC_C_AUTHN_LEVEL_xxx 
		RPC_C_IMP_LEVEL_DELEGATE, // RPC_C_IMP_LEVEL_xxx
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
		bstr_t("SELECT * FROM MSFT_Disk"),
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
