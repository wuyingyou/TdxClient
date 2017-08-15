#include "stdafx.h"
//#include "stdio.h"
#include <windows.h>
#include <iphlpapi.h> // get MAC
#include "Utility.h"
#pragma comment(lib, "iphlpapi.lib") // get MAC

char* getMAC(){
	PIP_ADAPTER_INFO AdapterInfo;
	IP_ADAPTER_INFO _IP_ADAPTER_INFO;
	DWORD dwBufLen = sizeof(AdapterInfo);
	char mac_addr[17];// malloc(17);
	AdapterInfo = &_IP_ADAPTER_INFO;
	if (AdapterInfo == NULL) {
		//printf("Error allocating memory needed to call GetAdaptersinfo\n");
	}

	// Make an initial call to GetAdaptersInfo to get the necessary size into the dwBufLen     variable
	if (GetAdaptersInfo(AdapterInfo, &dwBufLen) == ERROR_BUFFER_OVERFLOW) {

		//AdapterInfo = (IP_ADAPTER_INFO *)malloc(dwBufLen);
		//if (AdapterInfo == NULL) {
			//printf("Error allocating memory needed to call GetAdaptersinfo\n");
		//}
	}

	if (GetAdaptersInfo(AdapterInfo, &dwBufLen) == NO_ERROR) {
		PIP_ADAPTER_INFO pAdapterInfo = AdapterInfo;// Contains pointer to current adapter info
		do {
			//sprintf(mac_addr, "%02X:%02X:%02X:%02X:%02X:%02X",
			//	pAdapterInfo->Address[0], pAdapterInfo->Address[1],
			//	pAdapterInfo->Address[2], pAdapterInfo->Address[3],
			//	pAdapterInfo->Address[4], pAdapterInfo->Address[5]);
			//printf("Address: %s, mac: %s\n", pAdapterInfo->IpAddressList.IpAddress.String, mac_addr);
			return mac_addr;

			//printf("\n");
			pAdapterInfo = pAdapterInfo->Next;
		} while (pAdapterInfo);
	}
	//free(AdapterInfo);
}