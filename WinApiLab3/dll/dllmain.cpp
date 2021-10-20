// dllmain.cpp : Определяет точку входа для приложения DLL.
#include "pch.h"
#include <sysinfoapi.h>
#include <vector>
#include <string.h>
#include <iostream>

#define DLL_EXPORT

#include "dll.h"

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:        
    case DLL_THREAD_ATTACH:
		MessageBox(NULL, L"You cracked", NULL, MB_OK);
    case DLL_THREAD_DETACH:
		MessageBox(NULL, L"By", NULL, MB_OK);
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

//DLL_API void ProccessMain(char* strToFind, char* strToReplace);

DLL_API void ProccessMain(char* strToFind, char* strToReplace) {
	char str_test[12] = "To Find";
	char str_rep[12] = "To Replace";
	strToFind = str_test;
	strToReplace = str_rep;
	SYSTEM_INFO sysInfo;
	GetSystemInfo(&sysInfo);
	//sysInfo.
	void* min = sysInfo.lpMinimumApplicationAddress;
	void* max = sysInfo.lpMaximumApplicationAddress;
	void* regionAddr = min;
	HANDLE currProc = GetCurrentProcess();
	MEMORY_BASIC_INFORMATION infoBuffer;
	while ((regionAddr < max) && VirtualQuery(regionAddr, &infoBuffer, sizeof(infoBuffer))) {

		std::vector<char> tempBuffer;
		if (infoBuffer.State == MEM_COMMIT && (infoBuffer.Protect == PAGE_READWRITE || infoBuffer.Protect == PAGE_EXECUTE_READWRITE)) {

			SIZE_T bytesRead;

			tempBuffer.resize(infoBuffer.RegionSize);
			ReadProcessMemory(currProc, regionAddr, &tempBuffer[0], infoBuffer.RegionSize, &bytesRead);
			tempBuffer.resize(bytesRead);
			int currAddr = (int)regionAddr;
			for (auto j = 0; j < tempBuffer.size(); j++) {
				if (tempBuffer[j] == strToFind[0]) {
					int i = 0;
					while (tempBuffer[j + i] == strToFind[i] && (i < strlen(strToFind))) {
						i++;
					}
					if (i == strlen(strToFind)) {
						if (WriteProcessMemory(currProc, (LPVOID)((unsigned int)regionAddr + j), strToReplace, strlen(strToReplace) + 1, NULL)) {
							//char Buffer[100];
							std::cout << "Replaced" << std::endl;	
							ReadProcessMemory(currProc, (LPVOID)((unsigned int)regionAddr + j), &tempBuffer[0], infoBuffer.RegionSize, &bytesRead);
						}
					}
				}
			}
		}

		regionAddr = (void*)(infoBuffer.RegionSize + (SIZE_T)regionAddr);
	}
}

