// dllmain.cpp : Определяет точку входа для приложения DLL.
#include "pch.h"
#include <sysinfoapi.h>
#include <vector>
#include <string>
#include <iostream>

#define DLL_EXPORT

#include "dll.h"

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
	/*const char* strToFind = "stroka";
	char str_test[] = "stroka";
	char str_rep[] = "abcdef";*/
    switch (ul_reason_for_call)
    {
		case DLL_PROCESS_ATTACH: {
		/*	MessageBox(NULL, L"You cracked", NULL, MB_OK);
			ProccessMain(str_test, str_rep);*/
			break;
		}
		case DLL_THREAD_ATTACH: {
				break;
			}
		case DLL_THREAD_DETACH: {
			break; 
		}
		case DLL_PROCESS_DETACH: {
			//MessageBox(NULL, L"Bye", NULL, MB_OK);
			break;
		}
    }
    return TRUE;
}

DLL_API void ProccessMain(const char* strToFind, char* strToReplace) 
{	
	SYSTEM_INFO sysInfo;
	GetSystemInfo(&sysInfo);
	void* min = sysInfo.lpMinimumApplicationAddress;
	void* max = sysInfo.lpMaximumApplicationAddress;
	void* regionAddr = min;

	//MessageBox(NULL, L"Work", NULL, MB_OK);
	HANDLE currProc = GetCurrentProcess();
	MEMORY_BASIC_INFORMATION infoBuffer;
	while ((regionAddr < max) && VirtualQuery(regionAddr, &infoBuffer, sizeof(infoBuffer))) {
		if (infoBuffer.State == MEM_COMMIT && (infoBuffer.Protect == PAGE_READWRITE || infoBuffer.Protect == PAGE_EXECUTE_READWRITE)) {			
			char* ptr = (char*)regionAddr;
			for (auto j = 0; j < infoBuffer.RegionSize; j++) {
				if (ptr[j] == strToFind[0]) {
					if (strcmp(ptr + j, strToFind) == 0) {
						if (WriteProcessMemory(currProc, (LPVOID)((std::uint64_t)ptr + j), strToReplace, strlen(strToReplace) + 1, NULL)) {
							std::cout << "Replaced:" << (std::uint64_t)ptr + j << std::endl;
						}
					}
				}
			}
		}

		regionAddr = (void*)(infoBuffer.RegionSize + (SIZE_T)regionAddr);
	}
}

