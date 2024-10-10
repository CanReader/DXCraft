#ifndef _DEBUGGER_H_
#define _DEBUGGER_H_

#include <chrono>
#include <ctime>
#include <iostream>
#include "Log.h"

#ifdef BUILD_DLL
 #define DLL_API __declspec(dllexport)
#else
 #define DLL_API __declspec(dllimport)
#endif // DEBUG


#define END(hr) if(FAILED(hr)) return false;

#define SAFE_RELEASE(x) if(x) { x->Release(); x = nullptr;}

#define SAFE_DELETE(x) if(x) { delete x; x = nullptr; }

#define SAFE_DELETE_ARRAY(x) if(x) { delete[] x; x = nullptr; }

#define InfoBox(text) MessageBox(NULL,text,"INFO",MB_OK);

static std::string GetTime()
{
	std::time_t t = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());

	tm m;
	localtime_s(&m,&t);

	char buffer[11];
	strftime(buffer, 11,"%T",&m);

	return buffer;
}

#endif // !1



