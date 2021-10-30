#pragma once

#ifdef DLL_EXPORT
#define DLL_API __declspec(dllexport)
#else
#define DLL_API __declspec(dllimport)
#endif

extern "C" { DLL_API void ProccessMain(const char* strToFind, char* strToReplace); }