#pragma once

#define UTF8_TEXT(str) ((const char*)u8##str)

#if defined(_WIN32) || defined(WIN32) || defined(__CYGWIN__) || defined(__MINGW32__) || defined(__BORLANDC__)
	#define OS_WINDOWS
#elif __linux__
	#define OS_LINUX
#endif

// Not Crossplatform for now :(
// Because of weird fstream
#ifdef OS_WINDOWS
	#include <Windows.h>
#endif