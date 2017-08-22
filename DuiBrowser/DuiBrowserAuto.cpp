// DuiBrowserAuto.cpp : WinMain 的实现


#include "stdafx.h"
#include "resource.h"
#include "DuiBrowserAuto_i.h"
#include "MainDlg.h"
#include "../common/macro.h"
#include "../common/mimefilter.h"
#include "DuiBrowserApp.h"

#pragma comment(lib, "version.lib")


//#define ReduiApiOnly

#ifdef ReduiApiOnly

#include <atlimage.h>
class __initGdiPlus
{
	CImage _img;
public:
	__initGdiPlus() { _img.Load((LPCTSTR)NULL); }
} ___initGdiPlus;

#include "../common/apiloader.h"

#define __foreach_reduiapi(v) \
	v(BOOL, IsDirectUIMessage, (HWND hwndDirectUI, LPMSG lpMsg), (hwndDirectUI,lpMsg)) \
	v(BOOL, IsDirectUIWindow, (HWND hwnd), (hwnd)) \
	v(HWND, CreateDirectUIWindowW, (HWND hwndParent, LPCWSTR lpszWindowName, const RECT rc, DWORD dwStyle, DWORD dwExStyle, HMENU hMenu, HINSTANCE hInstance), (hwndParent,lpszWindowName,rc,dwStyle,dwExStyle,hMenu,hInstance))\
	v(BOOL, BstrFromStream, (IStream* pStream, BSTR* pbstr), (pStream,pbstr)) \
	v(BOOL, StreamFromUrl, (LPCOLESTR lpszUrl, IStream** ppStream, HINSTANCE hMod), (lpszUrl,ppStream,hMod)) \
	v(BOOL, CalcScriptExpressionW, (HWND hwndDirectUI, LPCWSTR lpszExpression, LPVARIANT lpVarResult), (hwndDirectUI,lpszExpression,lpVarResult))

ApiLoader reduiapi(L"redui.dll");

#define do_api(ret, name, params_decl, params_var) \
	typedef ret (WINAPI * pfn##name) params_decl; \
	pfn##name fn##name = (pfn##name)reduiapi(#name); \
	extern "C" ret WINAPI name params_decl \
	{ \
		ATLASSERT(fn##name); \
		return fn##name params_var; \
	} \

__foreach_reduiapi(do_api)
#undef do_api

#else // ReduiApiOnly

#pragma comment(lib, "redui.lib")

#endif // ReduiApiOnly

inline BOOL GetFixedFileVersion(LPCWSTR lpFile, VS_FIXEDFILEINFO* lpffvi)
{
	if (lpffvi==NULL)
		return FALSE;

	DWORD dwHandle;
	DWORD dwSize = GetFileVersionInfoSizeW(lpFile, &dwHandle);
	if (dwSize == 0) return FALSE;

	BYTE* pData = new BYTE[dwSize+1];
	GetFileVersionInfoW(lpFile, dwHandle, dwSize, (LPVOID)pData);

	LPVOID pBuffer=NULL;
	UINT uLen=0;
	if (VerQueryValueW((LPCVOID)pData,_T("\\"),&pBuffer,&uLen) && pBuffer && uLen!=0)
	{
		*lpffvi = *(VS_FIXEDFILEINFO*)pBuffer;
		delete[] pData;
		return TRUE;
	}

	delete[] pData;
	return FALSE;
}

inline BOOL GetFixedFileVersion(HINSTANCE hInst, VS_FIXEDFILEINFO* lpffvi)
{
	OLECHAR file[MAX_PATH] = L"";
	::GetModuleFileNameW(hInst, file, MAX_PATH);
	return GetFixedFileVersion(file, lpffvi);
}

CMessageLoop g_msgLoop;
CMainDlg* gpDlg = NULL;
CScript gScript;

class CScriptHost
{
public:
	Begin_Disp_Map(CScriptHost)
		Disp_PropertyPut(1, log)
		Disp_PropertyGet(2, isDebug, bool)
		Disp_PropertyGet(3, cacheDir)
		Disp_Method(4, fileVersion, CComBSTR, 1, BSTR)
	End_Disp_Map()

	bool isDebug;

	CScriptHost() : isDebug(false) {}

	HRESULT Setlog(VARIANT* val)
	{
		CComVariant v;
		if (SUCCEEDED(v.ChangeType(VT_BSTR, val)))
		{
			OutputDebugStringW(L"\n[SCRIPT] ");
			OutputDebugStringW(v.bstrVal);
			OutputDebugStringW(L"\n");
		}
		return S_OK;
	}

	HRESULT GetcacheDir(VARIANT* val)
	{
		static OLECHAR _path[_MAX_PATH] = L"";
		if (_path[0] == 0)
		{
			::GetModuleFileNameW(NULL, _path, _MAX_PATH);
			LPOLESTR p = _path + lstrlenW(_path);
			while (*p != L'\\' && p>=_path) p--;
			if (p>=_path) *p = 0;
			lstrcatW(_path, L"\\Cache");
		}
		CComVariant v = _path;
		return v.Detach(val);
	}

	CComBSTR fileVersion(BSTR path)
	{
		HINSTANCE hInst = NULL;
		if (path && *path!=0) hInst = GetModuleHandleW(path);

		VS_FIXEDFILEINFO ffvi;
		GetFixedFileVersion(hInst, &ffvi);
		CStringW str;
		str.Format(L"%d.%d.%d",
			HIWORD(ffvi.dwFileVersionMS),
			LOWORD(ffvi.dwFileVersionMS),
			HIWORD(ffvi.dwFileVersionLS)//,
			/*LOWORD(ffvi.dwFileVersionLS)*/);
		return CComBSTR(str);
	}
};
CScriptHost gScriptHost;

CComBSTR GetFileVersion(BSTR path)
{
	return gScriptHost.fileVersion(path);
}

class CDuiBrowserAutoModule : public CAtlExeModuleT< CDuiBrowserAutoModule >
{
public :
	DECLARE_LIBID(LIBID_DuiBrowserAutoLib)
	DECLARE_REGISTRY_APPID_RESOURCEID(IDR_DUIBROWSERAUTO, "{032862CD-4A19-49F6-974D-865685670D70}")

	CComBSTR m_url;
	CMimeFilterClassFactory m_filterCF;

	bool ParseCommandLine(LPCTSTR lpCmdLine, HRESULT* pnRetCode) throw()
	{
		*pnRetCode = S_OK;

		// 处理URL参数
		int nArgs;
		LPWSTR *szArglist = CommandLineToArgvW(GetCommandLineW(), &nArgs);
		ATLASSERT(szArglist);
		for (int i=1; i<nArgs; i++)
		{
			if (szArglist[i][0]!=L'-' && szArglist[i][0]!=L'/') // 后面跟着url
			{
				//ATLASSERT(FALSE);
				if (szArglist[i][0]==L'\"' || szArglist[i][0]==L'\'') // 有引号
				{
					LPWSTR p = szArglist[i] + 1;
					while ((*p==L' ' || *p==L'\t') && *p!=0) p++;
					if (*p==0) continue; // 参数有误，忽略
					p[::lstrlenW(p)-1] = L'\0';
					m_url = p;
				}
				else // 没有引号
				{
					LPWSTR p = szArglist[i];
					while ((*p==L' ' || *p==L'\t') && *p!=0) p++;
					if (*p==0) continue; // 参数有误，忽略
					m_url = p;
				}
			}
		}
		LocalFree(szArglist);

		// 处理普通开关
		TCHAR szTokens[] = _T("-/");

		//LPCTSTR lpszToken = FindOneOf(lpCmdLine, szTokens);
		for (LPCTSTR lpszToken=FindOneOf(lpCmdLine, szTokens); lpszToken != NULL; lpszToken=FindOneOf(lpszToken, szTokens))
		{
			if ((WordCmpI(lpszToken, _T("RegServer")) == 0))
			{
				//*pnRetCode = RegisterAppId();
				//if (SUCCEEDED(*pnRetCode))
				//	*pnRetCode = RegisterServer(TRUE);
				AtlMessageBox(NULL, _T("Registering is NOT supported."), _T("Command Line"));
				//AtlMessageBox(NULL, _T("软件不支持注册，请修改执行参数重新运行。"), _T("命令行参数"));
				return false;
			}

			else if ((WordCmpI(lpszToken, _T("UnregServer")) == 0))
			{
				UnregisterServer(TRUE);
				UnregisterAppId();
				return false;
			}

			else if ((WordCmpI(lpszToken, _T("DebugScript")) == 0))
			{
				gScriptHost.isDebug = true;
			}
		}

		CComPtr<IClassFactory> spCF;
		if (SUCCEEDED(CoGetClassObject(CLSID_DuiBrowser, CLSCTX_LOCAL_SERVER, NULL, IID_IClassFactory,(LPVOID*)&spCF)))
		{
			CComDispatchDriver spApp;
			spCF->CreateInstance(NULL, IID_IDispatch, (LPVOID*)&spApp);
			if (spApp.p)
			{
				CComVariant v;
				if (m_url.Length() > 0)
					v = m_url;
				spApp.Invoke1(L"activate", &v);
			}
			//CComPtr<IDuiBrowserApp> spApp;
			//spCF->CreateInstance(NULL, IID_IDuiBrowserApp, (LPVOID*)&spApp);
			//if (spApp.p)
			//{
			//	spApp->activate();
			//}
			return false;
		}

		m_bDelayShutdown = false;
		return true;
	}

	void RunMessageLoop() throw()
	{
		//HRESULT hr = RegisterAppId();
		//if (SUCCEEDED(hr))
		//	hr = RegisterServer(TRUE);
		m_filterCF.RegisterFilter();

		CMainDlg dlgMain;
		if (m_url.Length() > 0)
		{
			dlgMain.LoadUrl(m_url);
		}

		if(dlgMain.Create(NULL) == NULL)
		{
			ATLTRACE(_T("Main dialog creation failed!\n"));
			return;
		}

		dlgMain.ShowWindow(SW_SHOW);

		HRESULT hr = S_OK;
		CComDispatchDriver spScriptDisp;
		if (gScript.CreateJScript())
		{
			hr = gScript.AddNamedUnknown(L"host", gScriptHost.GetDispatch());
			hr = gScript.Execute(L""); // 确保开始执行

			{
				// 为什么从资源中读取代码后执行，启动在线资源时就发生 RPC 崩溃？
				// 直接将代码字符串执行就不会发生问题，何解？
				CComBSTR autolib;
				LoadResourceFromModule(GetModuleHandleW(NULL), L"automation.lib", autolib);
				//GetResourceText(L"raptor:automation.lib", autolib);
				//CComPtr<IStream> spStm;
				//if (SUCCEEDED(CDownload::LoadUrlToStream(L"automation.lib", &spStm.p, NULL)))
				//	CDownload::StreamToBSTR(spStm.p, autolib);

				//WCHAR autolib[10240] = L"";
				//GetResourceText(L"raptor:automation.lib", autolib, 10240);
				hr = gScript.Execute(autolib);
			}

			// 把脚本引擎全局对象附加到组件对象中
			hr = gScript.GetScriptDispatch(NULL, &spScriptDisp);
			if (spScriptDisp.p)
			{
				CDuiBrowserApp::m_peer.AddObject(spScriptDisp.p);
				//CDuiBrowserApp::m_peer.AddObject(gScriptHost.GetDispatch());
			}
		}

		g_msgLoop.Run();

		if (gScript.IsValid())
		{
			if (spScriptDisp.p)
			{
				//CDuiBrowserApp::m_peer.RemoveObject(gScriptHost.GetDispatch());
				CDuiBrowserApp::m_peer.RemoveObject(spScriptDisp.p);
				spScriptDisp.Release();
			}
			gScript.Close();
		}

		//UnregisterServer(TRUE);
		//UnregisterAppId();
		m_filterCF.UnregisterFilter();
	}
};

CDuiBrowserAutoModule _AtlModule;



//
extern "C" int WINAPI _tWinMain(HINSTANCE /*hInstance*/, HINSTANCE /*hPrevInstance*/, 
                                LPTSTR /*lpCmdLine*/, int nShowCmd)
{
    return _AtlModule.WinMain(nShowCmd);
}

