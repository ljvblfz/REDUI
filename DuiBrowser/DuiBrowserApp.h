// DuiBrowserApp.h : CDuiBrowserApp 的声明

#pragma once
#include "resource.h"       // 主符号

#include "DuiBrowserAuto_i.h"
#include "MainDlg.h"
#include <atlstr.h>
#include "../common/dispatchimpl.h"

extern CMainDlg* gpDlg;

#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Windows CE 平台(如不提供完全 DCOM 支持的 Windows Mobile 平台)上无法正确支持单线程 COM 对象。定义 _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA 可强制 ATL 支持创建单线程 COM 对象实现并允许使用其单线程 COM 对象实现。rgs 文件中的线程模型已被设置为“Free”，原因是该模型是非 DCOM Windows CE 平台支持的唯一线程模型。"
#endif



// CDuiBrowserApp

class ATL_NO_VTABLE CDuiBrowserApp :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CDuiBrowserApp, &CLSID_DuiBrowser>,
	public IDispatchImpl<IDuiBrowserApp, &IID_IDuiBrowserApp, &LIBID_DuiBrowserAutoLib, /*wMajor =*/ 0xffff, /*wMinor =*/ 0xffff>
{
public:
	static ULONG m_ulInstances;
	static CPeer m_peer;
	static IUnknown* m_peerUnk; // 纯粹为了编译不出错

	CDuiBrowserApp()
	{
	}

//DECLARE_REGISTRY_RESOURCEID(IDR_DUIBROWSERAPP)
DECLARE_NO_REGISTRY()

DECLARE_NOT_AGGREGATABLE(CDuiBrowserApp)

BEGIN_COM_MAP(CDuiBrowserApp)
	COM_INTERFACE_ENTRY(IDuiBrowserApp)
	COM_INTERFACE_ENTRY(IDispatch)
	//COM_INTERFACE_ENTRY_AGGREGATE_BLIND(m_peerUnk) // 这句不可使用，否则外部 HTA 远程创建本组件时会导致本进程崩溃，跟踪发现类厂请求创建 IMarshal 失败。
END_COM_MAP()

//DECLARE_CLASSFACTORY_SINGLETON(CDuiBrowserApp)

	DECLARE_PROTECT_FINAL_CONSTRUCT()

	typedef IDispatchImpl<IDuiBrowserApp, &IID_IDuiBrowserApp, &LIBID_DuiBrowserAutoLib, /*wMajor =*/ 0xffff, /*wMinor =*/ 0xffff> baseDispatch;

	HRESULT FinalConstruct()
	{
		m_ulInstances++;
		UpdateInfo();
		return S_OK;
	}

	void FinalRelease()
	{
		m_ulInstances--;
		UpdateInfo();
	}

	static void UpdateInfo();

	// IDispatch overide
public:
	STDMETHOD(GetIDsOfNames)(REFIID riid, LPOLESTR* rgszNames, UINT cNames, LCID lcid, DISPID* rgdispid);
	STDMETHOD(Invoke)(DISPID dispidMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS* pdispparams, VARIANT* pvarResult, EXCEPINFO* pexcepinfo, UINT* puArgErr);

public:

	STDMETHOD(load)(BSTR xml);
	STDMETHOD(loadUrl)(BSTR url);
	STDMETHOD(activate)(VARIANT* vUrl);
	//STDMETHOD(get_version)(BSTR* pVal);
	STDMETHOD(get_visible)(VARIANT_BOOL* pVal);
	//STDMETHOD(axo)(BSTR progid, IDispatch** obj);
	//STDMETHOD(httpLoad)(BSTR url, VARIANT success, VARIANT* ret);
	//STDMETHOD(fileVersion)(BSTR path, BSTR* pVal);
};

OBJECT_ENTRY_AUTO(__uuidof(DuiBrowser), CDuiBrowserApp)
