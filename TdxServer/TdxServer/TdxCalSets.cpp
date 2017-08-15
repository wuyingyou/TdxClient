#include "stdafx.h"
#include "TdxCalSets.h"
#include <windows.h>
#include <winhttp.h> 
#pragma comment(lib,"winhttp.lib")
#pragma comment(lib,"user32.lib")

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


