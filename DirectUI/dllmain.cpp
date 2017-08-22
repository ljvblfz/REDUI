// dllmain.cpp : DllMain 的实现。

#include "stdafx.h"
#include "resource.h"
#include "DirectUI_i.h"
#include "dllmain.h"
#include "dlldatax.h"
#include "OleDuiWindow.h"
#include "duicore/dxt.h"
#include "duicore/mttimer.h"
//#include "duicore/db/db.h"

#pragma warning(push)
//#pragma warning(disable:4217)

#include <uxtheme.h>
#pragma comment(lib, "uxtheme.lib")

CDirectUIModule _AtlModule;

// DLL 入口点
extern "C" BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
	if (dwReason == DLL_PROCESS_ATTACH)
	{
		DWORD dwFlags = (STAP_ALLOW_NONCLIENT |
		STAP_ALLOW_CONTROLS | STAP_ALLOW_WEBCONTENT);
		SetThemeAppProperties(dwFlags);

		COleDuiWindow::RegisterWindowClass();
		//Database* db = Database::Open(); // 确保内存数据库初始化
	}
	else if (dwReason == DLL_PROCESS_DETACH)
	{
		//d3d::OnThreadFinish();
		COleDuiWindow::UnregisterWindowClass();
		OnEndOfThread();
		DXT::clear();
#ifdef _DEBUG
		_CrtSetDbgFlag ( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
#endif // _DEBUG
	}
	else if (dwReason == DLL_THREAD_DETACH)
	{
#ifndef NO3D
		d3d::OnThreadFinish();
#endif // NO3D
		OnEndOfThread();
	}

#ifdef _MERGE_PROXYSTUB
	if (!PrxDllMain(hInstance, dwReason, lpReserved))
		return FALSE;
#endif
	hInstance;
	return _AtlModule.DllMain(dwReason, lpReserved); 
}

#pragma warning(pop)
