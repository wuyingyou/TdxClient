#include "stdafx.h"
#include "TdxCalSets.h"
#include <windows.h>
#include <winhttp.h> 
#include <winsock2.h>// get mac
#include <iphlpapi.h>// get mac
#include <string> // get mac
#pragma comment(lib,"winhttp.lib")
#pragma comment(lib,"user32.lib")
#pragma comment(lib, "iphlpapi.lib")

#define WORKING_BUFFER_SIZE 15000
#define MAX_TRIES 3
#define MALLOC(x) HeapAlloc(GetProcessHeap(), 0, (x))
#define FREE(x) HeapFree(GetProcessHeap(), 0, (x))

void TestPlugin1(int DataLen, float* pfOUT, float* pfINa, float* pfINb, float* pfINc)
{
	DWORD dwSize = 0;
	DWORD dwDownloaded = 0;
	LPVOID lpOutBuffer = NULL;
	HINTERNET hSession = NULL,
		hConnect = NULL,
		hRequest = NULL;

	BOOL bResults = FALSE;

	hSession = WinHttpOpen(L"User Agent", WINHTTP_ACCESS_TYPE_DEFAULT_PROXY, WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0);

	if (hSession)
	{
		hConnect = WinHttpConnect(hSession, L"localhost", 55069, 0);
	}

	if (hConnect)
	{
		hRequest = WinHttpOpenRequest(hConnect, L"GET", L"api/values?userName=test&pwd=testpwd", NULL, WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES, 0);
	}

	if (hRequest)
	{
		bResults = WinHttpSendRequest(hRequest, WINHTTP_NO_ADDITIONAL_HEADERS, 0, WINHTTP_NO_REQUEST_DATA, 0, 0, 0);
	}

	if (bResults)
	{
		bResults = WinHttpReceiveResponse(hRequest, NULL);

	}


	if (bResults)
	{
		bResults = WinHttpQueryHeaders(hRequest, WINHTTP_QUERY_RAW_HEADERS_CRLF, WINHTTP_HEADER_NAME_BY_INDEX, NULL, &dwSize, WINHTTP_NO_HEADER_INDEX);

		if (GetLastError() == ERROR_INSUFFICIENT_BUFFER)
		{
			lpOutBuffer = new WCHAR[dwSize / sizeof(WCHAR)];
			bResults = WinHttpQueryHeaders(hRequest, WINHTTP_QUERY_RAW_HEADERS_CRLF, WINHTTP_HEADER_NAME_BY_INDEX, lpOutBuffer, &dwSize, WINHTTP_NO_HEADER_INDEX);
		}
	}

	//if (bResults)
	//{
	//	printf("Header contents: \n%S", lpOutBuffer);
	//}

	if (!WinHttpQueryDataAvailable(hRequest, &dwSize))
	{
		PopuldateOutput(DataLen, pfOUT, 0);
		return;
	}
	//printf("Error %u in WinHttpQueryDataAvailable.\n", GetLastError());

	LPSTR pszOutBuffer = new char[dwSize + 1];
	ZeroMemory(pszOutBuffer, dwSize + 1);
	if (!WinHttpReadData(hRequest, (LPVOID)pszOutBuffer, dwSize, &dwDownloaded))
	{
		PopuldateOutput(DataLen, pfOUT, 0);
		return;
	}
	//printf("Error %u in WinHttpReadData.\n", GetLastError());

	else
	{
		if (getAuth(pszOutBuffer))
		{
			PopuldateOutput(DataLen, pfOUT, 1);
		}
		else
		{
			PopuldateOutput(DataLen, pfOUT, 0);
		}
	}

	delete[] lpOutBuffer;
	delete[] pszOutBuffer;

	if (hRequest) WinHttpCloseHandle(hRequest);
	if (hConnect) WinHttpCloseHandle(hConnect);
	if (hSession) WinHttpCloseHandle(hSession);
}

PluginTCalcFuncInfo g_CalcFuncSets[] =
{
	{ 1, (pPluginFUNC)&TestPlugin1 },
	{ 0, NULL },
};

BOOL RegisterTdxFunc(PluginTCalcFuncInfo** pFun)
{
	if (*pFun == NULL)
	{
		(*pFun) = g_CalcFuncSets;
		return TRUE;
	}
	return FALSE;
}

void PopuldateOutput(int DataLen, float* pfOUT, int value)
{
	for (int i = 0; i < DataLen; i++)
	{
		pfOUT[i] = value;
	}
}

bool getAuth(LPSTR pszOutBuffer)
{
	return strcmp(pszOutBuffer, "true") == 0;
}

const char* getMac(){
	//https://msdn.microsoft.com/en-us/library/aa365915(VS.85).aspx
	DWORD dwRetVal = 0;
	unsigned int i = 0;
	ULONG flags = GAA_FLAG_INCLUDE_PREFIX;
	// default to unspecified address family (both)
	ULONG family = AF_INET; // AF_UNSPEC, AF_INET6, default we get 4
	PIP_ADAPTER_ADDRESSES pAddresses = NULL;
	ULONG outBufLen = 0;
	ULONG Iterations = 0;
	PIP_ADAPTER_ADDRESSES pCurrAddresses = NULL;
	std::string mac_addr;

	// Allocate a 15 KB buffer to start with.
	outBufLen = WORKING_BUFFER_SIZE;
	do {
		pAddresses = (IP_ADAPTER_ADDRESSES *)MALLOC(outBufLen);
		if (pAddresses == NULL) {
			//printf("Call to MALLOC failed with error."); 
		}
		dwRetVal = GetAdaptersAddresses(family, flags, NULL, pAddresses, &outBufLen);
		if (dwRetVal == ERROR_BUFFER_OVERFLOW) {
			FREE(pAddresses);
			pAddresses = NULL;
		}
		else {
			break;
		}
		Iterations++;
	} while ((dwRetVal == ERROR_BUFFER_OVERFLOW) && (Iterations < MAX_TRIES));

	if (dwRetVal == NO_ERROR) {
		pCurrAddresses = pAddresses;
		while (pCurrAddresses) {
			if (pCurrAddresses->PhysicalAddressLength != 0) {
				mac_addr = std::to_string((int)pCurrAddresses->PhysicalAddress[0]) +
					std::to_string((int)pCurrAddresses->PhysicalAddress[1]) +
					std::to_string((int)pCurrAddresses->PhysicalAddress[2]) +
					std::to_string((int)pCurrAddresses->PhysicalAddress[3]) +
					std::to_string((int)pCurrAddresses->PhysicalAddress[4]) +
					std::to_string((int)pCurrAddresses->PhysicalAddress[5]);
			}
			pCurrAddresses = pCurrAddresses->Next;
		}
	}
	else {
		//printf("Call to GetAdaptersAddresses failed with error: %d\n",dwRetVal);
	}

	if (pAddresses) {
		FREE(pAddresses);
	}
	return mac_addr.c_str();
}