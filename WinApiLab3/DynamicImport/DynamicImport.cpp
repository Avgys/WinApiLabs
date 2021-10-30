#include <Windows.h>
#include <iostream>

typedef void (*procName)  (const char* , const char* );

int main()
{
    const char* strToFind= "To Find";
    char str_test[20] = "To Find";
    std::cout << str_test << " " << (SIZE_T)&str_test << std::endl;

    char str_rep[20] = "To Replace";
    const WCHAR* lpFileName = L"C:\\Users\\ilyuh\\Desktop\\WinApi\\WinApiLabs\\WinApiLab3\\dll\\x64\\Debug\\dll.dll";
    HMODULE lib = LoadLibrary(lpFileName);

    if (lib == NULL) {
        const DWORD errorCode = GetLastError();
        return errorCode;
    }

    FARPROC proc = GetProcAddress(lib, "ProccessMain");    
    procName func = (procName)proc;
    func(strToFind, str_rep);
    FreeLibrary(lib);

    std::cout << str_test << std::endl;
    return 0;
}