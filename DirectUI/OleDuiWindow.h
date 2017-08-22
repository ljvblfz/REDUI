// OleDuiWindow.h : COleDuiWindow 的声明

#pragma once
#include "resource.h"       // 主符号
#include <DispEx.h>

#include "DirectUI_i.h"
#include "DuiControl.h"
#include "duicore/script/pulsedef.h"
#include "duicore/DuiBase.h"
#include "../common/dispatchimpl.h"


#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Windows CE 平台(如不提供完全 DCOM 支持的 Windows Mobile 平台)上无法正确支持单线程 COM 对象。定义 _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA 可强制 ATL 支持创建单线程 COM 对象实现并允许使用其单线程 COM 对象实现。rgs 文件中的线程模型已被设置为“Free”，原因是该模型是非 DCOM Windows CE 平台支持的唯一线程模型。"
#endif

#define uses_dispatchex

#ifdef uses_dispatchex
#define IDispatchImpl	IDispatchExImpl
//#define _IDispatchExImpl	IDispatchExImpl
//#else
//#define _IDispatchExImpl	IDispatchImpl
#endif

class COleDuiWindow;

//////////////////////////////////////////////////////////////////////////
// CWinExt 处理窗口状态
class CWinExt
{
public:
	CWindow _win;
	COleDuiWindow* _olewin;

	CSimpleArray<CComPtr<IDispatch> > m_onReadys;
	CSimpleArray<CComPtr<IDispatch> > m_onEnds;

	CWinExt(COleDuiWindow* olewin) : _olewin(olewin){}

	BOOL __DispidOfName(LPCOLESTR szName, DISPID* pDispid);
	HRESULT __Invoke(DISPID dispid, WORD wFlags, DISPPARAMS* pdispparams, VARIANT *pVarResult);

	void MoveTo(long l, long t, long r, long b) { if (_win.IsWindow()) _win.MoveWindow(l,t,r-l,b-t,TRUE); }
	void Center() { if (_win.IsWindow()) _win.CenterWindow(); }

	bool $(IDispatch* callback); // 模仿jQuery，用以注册一个回调函数，当BODY解析完成时调用
	bool $$(IDispatch* callback); // 注册一个回调函数，当布局即将卸载时调用

	Begin_Disp_Map(CWinExt)
		Disp_Method(1, $, bool, 1, IDispatch*)
		Disp_Method(2, $$, bool, 1, IDispatch*)
		Disp_Method(10, MoveTo, void, 4, long, long, long, long)
		Disp_Method(11, Center, void, 0)
	End_Disp_Map()
};

// COleDuiWindow

class ATL_NO_VTABLE COleDuiWindow :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<COleDuiWindow, &CLSID_DuiWindow>,
	public CDuiNotifyListener,
	public CDuiObjectCreator,
	public INamedItemHost,
	public IDispatchImpl<IDuiWindow, &IID_IDuiWindow, &LIBID_DirectUILib, /*wMajor =*/ 0xffff, /*wMinor =*/ 0xffff>
{
public:
	HWND m_hWnd;
	CDuiLayoutManager* m_pLayoutMgr;
	CComPtr<IDispatch> m_pExternal;
	//CSimpleArray<CStdString> m_namedItems;
	CPeer m_peer;
	CWinExt m_winExt;
	IUnknown* m_peerUnk; // 纯粹为了编译不出错

	CSimpleMap<long, CComVariant> m_timers;
	CSimpleArray<long> m_timerOnce;

public:
	COleDuiWindow() : m_hWnd(NULL), m_pLayoutMgr(NULL), m_peer(NULL), m_peerUnk(&m_peer), m_winExt(this)
	{
	}

DECLARE_REGISTRY_RESOURCEID(IDR_OLEDUIWINDOW)


BEGIN_COM_MAP(COleDuiWindow)
	COM_INTERFACE_ENTRY(IDuiWindow)
	COM_INTERFACE_ENTRY2(IDispatch, IDispatchEx)
	COM_INTERFACE_ENTRY(IDispatchEx)
	COM_INTERFACE_ENTRY_AGGREGATE_BLIND(m_peerUnk)
END_COM_MAP()


	typedef IDispatchImpl<IDuiWindow, &IID_IDuiWindow, &LIBID_DirectUILib, 0xffff, 0xffff> baseDispatch;

	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct()
	{
		return S_OK;
	}

	void FinalRelease()
	{
		ATLASSERT(m_hWnd==NULL);

		if (m_pLayoutMgr)
		{
			delete m_pLayoutMgr;
			m_pLayoutMgr = NULL;
		}
		// 脚本引擎也必须放在销毁窗口时清理，不能放在这里清理，否则组件可能永远也无法释放
	}

	static BOOL RegisterWindowClass();
	static BOOL UnregisterWindowClass();
	static LRESULT CALLBACK __DuiWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	// 工具方法
	static COleDuiWindow* FromHandle(HWND hwnd);
	DuiNode* GetControlById(LPCOLESTR szId);
	DuiNode* GetRoot() { return m_pLayoutMgr ? m_pLayoutMgr->m_pRoot : NULL; }
	static void CALLBACK TimerProc(LPVOID pData, DWORD dwId);
	virtual BOOL GetNamedItem(LPCOLESTR name, VARIANT* pRet); // 如果有命名项，返回TRUE，否则返回FALSE

	// 从XML内容中加载布局
	BOOL LoadXML(LPCOLESTR lpszXml, HINSTANCE hMod=NULL);

	// 从URL指示的XML文件中加载布局
	BOOL LoadFromUrl(LPCOLESTR lpszUrl, HINSTANCE hMod=NULL);


	// 属性成员
protected:

public:
	void OnFinalMessage(HWND hWnd);

	// CDuiNotifyListener
public:
	virtual void OnNotify(DuiNotify& msg);

	// CDuiObjectCreator
	virtual IDispatch* GetWindowObject();
	virtual HRESULT CreateControlObject(DuiNode* pCtrl, IDispatch** ppDisp);
	virtual void OnLayoutBegin(); // 开始加载一个新布局
	virtual void OnLayoutReady(); // 布局加载完毕
	virtual void OnLayoutEnd();   // 布局即将卸载
	virtual void OnLayoutError(); // 布局解析失败

	// message dispatch
public:
	//BOOL PreTranslateMessage(LPMSG lpMsg);

	// IDispatch
public:
	//STDMETHOD(GetIDsOfNames)(REFIID riid, LPOLESTR* rgszNames, UINT cNames, LCID lcid, DISPID* rgdispid);
	//STDMETHOD(Invoke)(DISPID dispidMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS* pdispparams, VARIANT* pvarResult, EXCEPINFO* pexcepinfo, UINT* puArgErr);
	STDMETHOD(GetDispID)(BSTR bstrName, DWORD grfdex, DISPID *pid);
	STDMETHOD(InvokeEx)(DISPID id, LCID lcid, WORD wFlags, DISPPARAMS *pdp, VARIANT *pvarRes, EXCEPINFO *pei, IServiceProvider *pspCaller);

public:
	STDMETHOD(get_window)(IDispatch** pVal);
	STDMETHOD(get_event)(IDispatch** pVal);
	STDMETHOD(get_external)(IDispatch** pVal);
	STDMETHOD(get_focus)(IDispatch** pVal);
	STDMETHOD(get_body)(IDispatch** pVal);
	STDMETHOD(alert)(BSTR Content, BSTR Caption);
	STDMETHOD(close)(void);
	STDMETHOD(createElement)(BSTR eTag, IDispatch** newElem);
	STDMETHOD(setInterval)(VARIANT callback, LONG msec, LONG* timerId);
	STDMETHOD(setTimeout)(VARIANT callback, LONG msec, LONG* timerId);
	STDMETHOD(clearInterval)(LONG timerId);
	STDMETHOD(clearTimeout)(LONG timerId);
	STDMETHOD(createEvent)(BSTR eventName, IDispatch** pVal);
	STDMETHOD(fireEvent)(IDuiControl* ctrl, IDuiEvent* eventObj);
	};

OBJECT_ENTRY_AUTO(__uuidof(DuiWindow), COleDuiWindow)

#undef uses_dispatchex
