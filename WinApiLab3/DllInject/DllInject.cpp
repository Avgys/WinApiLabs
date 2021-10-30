#include <iostream>
#include <Windows.h>
#include <tlhelp32.h>
#include <thread>
#include <chrono>
using namespace std::chrono_literals;

bool InjectDll(int pId, LPCSTR szDllPath);
HANDLE GetProcessByName(PCWSTR name);

int main()
{
	
	if (!InjectDll(GetProcessId(GetProcessByName(L"Project1.exe")), "C:\\Users\\ilyuh\\Desktop\\WinApi\\WinApiLabs\\WinApiLab3\\dll\\x64\\Debug\\injectdll.dll")) {
		return GetLastError();
	}
	return 0;
}

bool InjectDll(int pId, LPCSTR szDllPath) {
	HANDLE hProcess = NULL;
	LPVOID pRemoteBuf = NULL;
	int dwBufSize = (strlen(szDllPath) + 1) * sizeof(TCHAR);
	LPTHREAD_START_ROUTINE pThreadProc = NULL;
	HMODULE hMod = NULL;

	if (!(hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pId)))
	{
		std::cout << (L"InjectDll() : OpenProcess(%d) failed!!! [%d]\n",
			pId, GetLastError());
		return false;
	}

	pRemoteBuf = VirtualAllocEx(hProcess, NULL, dwBufSize, MEM_COMMIT, PAGE_READWRITE);
	
	if (pRemoteBuf == NULL)
	{
		std::cout << (L"InjectDll() : VirtualAllocEx() failed!!! [%d]\n",
			GetLastError());
		return false;
	}

	if (!WriteProcessMemory(hProcess, pRemoteBuf, (LPVOID)szDllPath, dwBufSize, NULL)) 
	{
		std::cout << (L"InjectDll() : WriteProcessMemory() failed!!! [%d]\n",
			GetLastError());
		return false;
	}

	hMod = GetModuleHandle(L"kernel32.dll"); 

	if (hMod == NULL)
	{
		std::cout << (L"InjectDll() : GetModuleHandle(\"kernel32.dll\") failed!!! [%d]\n",
			GetLastError());
		return false;
	}

	pThreadProc = (LPTHREAD_START_ROUTINE)GetProcAddress(hMod, "LoadLibraryA"); 

	if (pThreadProc == NULL)
	{
		std::cout << (L"InjectDll() : GetProcAddress(\"LoadLibraryW\") failed!!! [%d]\n");	

		return false;
	}

	if (!CreateRemoteThread(hProcess, NULL, 0, pThreadProc, pRemoteBuf, 0, NULL)) 
	{
		std::cout << (L"InjectDll() : MyCreateRemoteThread() failed!!!\n", GetLastError());

		return false;
	}

	std::this_thread::sleep_for(1s);
	if (pRemoteBuf)
		VirtualFreeEx(hProcess, pRemoteBuf, 0, MEM_RELEASE);
	if (hProcess)
		CloseHandle(hProcess);

	return true;
}

HANDLE GetProcessByName(PCWSTR name)
{
	DWORD pid = 0;

	// Create toolhelp snapshot.
	HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	PROCESSENTRY32 process;
	ZeroMemory(&process, sizeof(process));
	process.dwSize = sizeof(process);

	// Walkthrough all processes.
	if (Process32First(snapshot, &process))
	{
		do
		{
			if (std::wstring(process.szExeFile) == std::wstring(name))
			{
				pid = process.th32ProcessID;
				break;
			}
		} while (Process32Next(snapshot, &process));
	}

	CloseHandle(snapshot);

	if (pid != 0)
	{
		return OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
	}

	return NULL;
}
