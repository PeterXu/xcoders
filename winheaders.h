#pragma once


#ifdef WIN32

#ifndef WINVER
#define WINVER 0x0600
#endif

#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0600
#endif

#ifndef _WIN32_WINDOWS
#define _WIN32_WINDOWS 0x0410
#endif

#ifndef _WIN32_IE           
#define _WIN32_IE 0x0700   
#endif

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#ifdef WIN32_NULL
#ifdef DLL_FILE
#   define FF_EXPORT _declspec(dllexport)
#else
#   define FF_EXPORT _declspec(dllimport)
#endif
#else
#   define FF_EXPORT
#endif

#endif //WIN32
