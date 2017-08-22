// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件

#pragma once

#ifndef STRICT
#define STRICT
#endif

#include "targetver.h"

#define _ATL_APARTMENT_THREADED
#define _ATL_NO_AUTOMATIC_NAMESPACE

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS	// 某些 CString 构造函数将是显式的

#define _SECURE_ATL 1

//#define GDIPVER 0x0110

#include "resource.h"
#include <atlbase.h>
#include <atlcom.h>
#include <atlctl.h>
#include <atltypes.h>
#include <atlstr.h>

// WTL
//#include <atlmisc.h>
#include <atlapp.h>
#include <atlgdi.h>

using namespace ATL;

#pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")

//#define _DEBUG_MEMORY

#if defined(_DEBUG) //&& defined(_DEBUG_MEMORY)
	#define _CRTDBG_MAP_ALLOC
	#include <stdlib.h>
	#include <crtdbg.h>
	#define NEW new( _CLIENT_BLOCK, __FILE__, __LINE__)
#else
	#define NEW new
#endif