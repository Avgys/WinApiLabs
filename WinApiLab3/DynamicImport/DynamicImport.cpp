#include <Windows.h>
#include <iostream>

typedef void (*procName)  (const char* , const char* );

int main()
{

    char str_test[12] = "To Find";
    char str_rep[12] = "To Replace";
    const WCHAR* lpFileName = L"C:\\Users\\ilyuh\\Desktop\\WinApi\\WinApiLabs\\WinApiLab3\\dll\\Debug\\dll.dll";
    HMODULE lib = LoadLibrary(lpFileName);
    if (lib == NULL) {
        const DWORD errorCode = GetLastError();
        return errorCode;
    }

    FARPROC proc = GetProcAddress(lib, "ProccessMain");    
    procName func = (procName)proc;
    func(str_test, str_rep);
    FreeLibrary(lib);
}