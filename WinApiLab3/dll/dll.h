#pragma once

#ifdef DLL_EXPORT
#define DLL_API __declspec(dllexport)
#else
#define DLL_API __declspec(dllimport)
#endif

extern "C" { DLL_API void ProccessMain(char* strToFind, char* strToReplace); }
//
//extern "C" { __declspec(dllexport) void Osisp3TestFunc(); }
//
//#include <Windows.h>
//
//BOOL WINAPI DllMain(
//    HINSTANCE hinstDLL,  // handle to DLL module
//    DWORD fdwReason,     // reason for calling function
//    LPVOID lpReserved)  // reserved
//{
//    // Perform actions based on the reason for calling.
//    switch (fdwReason)
//    {
//    case DLL_PROCESS_ATTACH:
//        // Initialize once for each new process.
//        // Return FALSE to fail DLL load.
//
//        MessageBoxA(NULL, "YOU CRACKED, NOOOOOOOOOBBB", "", MB_OK);
//        break;
//
//    case DLL_THREAD_ATTACH:
//        // Do thread-specific initialization.
//        break;
//
//    case DLL_THREAD_DETACH:
//        // Do thread-specific cleanup.
//        break;
//
//    case DLL_PROCESS_DETACH:
//        // Perform any necessary cleanup.
//        break;
//    }
//    return TRUE;  // Successful DLL_PROCESS_ATTACH.
//}