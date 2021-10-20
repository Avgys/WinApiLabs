
#include <iostream>
#include <Windows.h>


bool InjectDll(int pId, LPCSTR szDllPath);

int main()
{
	if (!InjectDll(15400, "C:\\Users\\ilyuh\\Desktop\\WinApi\\WinApiLabs\\WinApiLab3\\dll\\Debug\\dll.dll")) {
		return GetLastError();
	}
	return 0;
	//LoadLibrary
		/*Proccess
		VirtualAllocEx
		WriteProcessMemory
	CreateRemoteThread();*/
}

bool InjectDll(int pId, LPCSTR szDllPath) {
	HANDLE hProcess = NULL;
	LPVOID pRemoteBuf = NULL;
	int dwBufSize = (strlen(szDllPath) + 1) * sizeof(TCHAR); // Размер открытой памяти
	LPTHREAD_START_ROUTINE pThreadProc = NULL;
	HMODULE hMod = NULL;

	if (!(hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pId))) // Открываем целевой процесс и получаем дескриптор
	{
		std::cout << (L"InjectDll() : OpenProcess(%d) failed!!! [%d]\n",
			pId, GetLastError());
		return false;
	}

	pRemoteBuf = VirtualAllocEx(hProcess, NULL, dwBufSize, MEM_COMMIT, PAGE_READWRITE); // Открываем часть памяти в пространстве целевого процесса
	
	if (pRemoteBuf == NULL)
	{
		std::cout << (L"InjectDll() : VirtualAllocEx() failed!!! [%d]\n",
			GetLastError());
		return false;
	}

	if (!WriteProcessMemory(hProcess, pRemoteBuf, (LPVOID)szDllPath, dwBufSize, NULL)) // Копируем путь к dll в открытую память
	{
		std::cout << (L"InjectDll() : WriteProcessMemory() failed!!! [%d]\n",
			GetLastError());
		return false;
	}

	hMod = GetModuleHandle(L"kernel32.dll"); // Получаем дескриптор модуля этого процесса kernel32.dll

	if (hMod == NULL)
	{
		std::cout << (L"InjectDll() : GetModuleHandle(\"kernel32.dll\") failed!!! [%d]\n",
			GetLastError());
		return false;
	}

	pThreadProc = (LPTHREAD_START_ROUTINE)GetProcAddress(hMod, "LoadLibraryA"); // Получить начальный адрес функции LoadLibraryW

	if (pThreadProc == NULL)
	{
		std::cout << (L"InjectDll() : GetProcAddress(\"LoadLibraryW\") failed!!! [%d]\n");	

		return false;
	}

	if (!CreateRemoteThread(hProcess, NULL, 0, pThreadProc, pRemoteBuf, 0, NULL)) // Выполнение удаленного потока
	{
		std::cout << (L"InjectDll() : MyCreateRemoteThread() failed!!!\n", GetLastError());

		return false;
	}

	if (pRemoteBuf)
		VirtualFreeEx(hProcess, pRemoteBuf, 0, MEM_RELEASE);
	if (hProcess)
		CloseHandle(hProcess);

	return true;
}
