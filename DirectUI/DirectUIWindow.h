#ifndef __DIRECTUIWINDOW_H__
#define __DIRECTUIWINDOW_H__

#define WM_LOADXML	(WM_USER + 0x1f00)
#define WM_LOADURL	(WM_USER + 0x1f01)

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#ifndef DUIAPI
#define DUIAPI WINAPI
#endif

#ifndef HDE
	typedef HANDLE HDE;
#endif

// Flags used for controlling the paint and style state
#define DUISTATE_FOCUSED      0x00000001
#define DUISTATE_SELECTED     0x00000002
#define DUISTATE_DISABLED     0x00000004
#define DUISTATE_HOT          0x00000008
#define DUISTATE_HOVER		  DUISTATE_HOT
#define DUISTATE_PUSHED       0x00000010
#define DUISTATE_CHECKED      0x00000020
#define DUISTATE_READONLY     0x00000040
#define DUISTATE_CAPTURED     0x00000080
#define DUISTATE_EXPANDED	  0x00000100
#define DUISTATE_HIDDEN		  0x00000200
#define DUISTATE_MASK		  0x0000FFFF

// for WM_NOTIFY message
// Send to parent of directui window or directui window
// WPARAM - HWND of directui window, sames to DUINMHDR::hdr.hwndFrom
// LPARAM - LPDUINMHDR (cast from LPNMHDR)
/*
 *	example:
	if (pMsg->message == WM_NOTIFY && (HWND)wParam==hwndMyDui)
	{
		LPDUINMHDR nmh = (LPDUINMHDR)lParam;
		HDE hde = (HDE)nmh->hdr.idFrom;
		OutputDebugStringW(::DuiGetAttributeString(hde, L"tagName");
		if (nmh->hdr.code == (UINT)nmc_click)
		{
			// do something
		}
	}
 */
enum DUINM_CODE
{
	nmc__unknown = 0,
	nmc__first = 0x2010,
	nmc_mousemove = nmc__first,
	nmc_mouseleave, nmc_mouseout = nmc_mouseleave,
	nmc_mouseenter, nmc_mouseover = nmc_mouseenter,
	nmc_mousehover,
	nmc_keydown,
	nmc_keyup,
	nmc_char,
	nmc_syskey,
	nmc_killfocus, nmc_blur = nmc_killfocus,
	nmc_setfocus, nmc_focus = nmc_setfocus,
	nmc_buttondown,
	nmc_buttonup,
	nmc_click,
	nmc_dblclick,
	nmc_contextmenu,
	nmc_scrollwheel,
	nmc_size,
	nmc_timer,
	nmc_load,
	nmc_unload,
	nmc_propertychange,
	nmc__last
};

typedef struct tagDUINMHDR
{
	NMHDR hdr; // hwndFrom - DirectUI Window; idFrom - HDE; code - DUINM_CODE or eventName(LPCOLESTR)

	DWORD dwTimestamp;
	POINT ptMouse;
	OLECHAR chKey;
	WORD wKeyState;
	LPCWSTR pPropertyName;
	BOOL bHandled;		// Please set to TRUE if you don't want this message to be processed
	BOOL bCancelBubble;
	WPARAM wParam;
	LPARAM lParam;
	DWORD reserved;
} DUINMHDR, *LPDUINMHDR;

/*
 *	Global(hwndDirectUI is NULL) or local options
 */
//BOOL WINAPI DirectUISetOptions(HWND hwndDirectUI, LPCOLESTR lpszOptions);

/*
 *	API for DirectUI Window
 */
HWND DUIAPI CreateDirectUIWindowA(HWND hwndParent, LPCSTR lpszWindowName, const RECT rc, DWORD dwStyle, DWORD dwExStyle, HMENU hMenu, HINSTANCE hInstance);
HWND DUIAPI CreateDirectUIWindowW(HWND hwndParent, LPCWSTR lpszWindowName, const RECT rc, DWORD dwStyle, DWORD dwExStyle, HMENU hMenu, HINSTANCE hInstance);

BOOL DUIAPI CalcScriptExpressionA(HWND hwndDirectUI, LPCSTR lpszExpression, LPVARIANT lpVarResult);
BOOL DUIAPI CalcScriptExpressionW(HWND hwndDirectUI, LPCWSTR lpszExpression, LPVARIANT lpVarResult);

#ifdef UNICODE
#define CreateDirectUIWindow CreateDirectUIWindowW
#define CalcScriptExpression CalcScriptExpressionW
#else // !UNICODE
#define CreateDirectUIWindow CreateDirectUIWindowA
#define CalcScriptExpression CalcScriptExpressionA
#endif // UNICODE

BOOL DUIAPI IsDirectUIMessage(HWND hwndDirectUI, LPMSG lpMsg);
BOOL DUIAPI IsDirectUIWindow(HWND hwnd);
void DUIAPI SetDirectUIWindowExternal(HWND hwndDirectUI, IDispatch* pDisp);
HDE DUIAPI GetRootElement(HWND hwndDirectUI);
HDE DUIAPI GetElementById(HWND hwndDirectUI, LPCWSTR lpszId);
BOOL DUIAPI ParseStyle(HWND hwnd, LPCWSTR lpszTargetName, DWORD dwTargetState, LPCWSTR lpszStyle);

/*
 *	API Helper
 */
BOOL DUIAPI StreamFromUrl(LPCOLESTR lpszUrl, IStream** ppStream, HINSTANCE hMod);
BOOL DUIAPI BstrFromStream(IStream* pStream, BSTR* pbstr);

/*
 *	API for DirectUI Element
 */
BOOL DUIAPI		DuiIsElement		(HDE hde);
HWND DUIAPI		DuiGetContainerHwnd	(HDE hde);
HDE DUIAPI		DuiGetParent		(HDE hde);
HDE DUIAPI		DuiGetTopParent		(HDE hde);
LPCWSTR DUIAPI	DuiGetID			(HDE hde);
LPCWSTR DUIAPI	DuiGetText			(HDE hde);
void DUIAPI		DuiSetText			(HDE hde, LPCWSTR lpszText);
LPCWSTR DUIAPI	DuiGetToolTip		(HDE hde);
void DUIAPI		DuiSetTooTip		(HDE hde, LPCWSTR lpszToolTip);
WCHAR DUIAPI	DuiGetShortcut		(HDE hde);
void DUIAPI		DuiSetShortcut		(HDE hde, WCHAR ch);
DWORD DUIAPI	DuiGetState			(HDE hde);
void DUIAPI		DuiModifyState		(HDE hde, DWORD dwAdded, DWORD dwRemoved);
BOOL DUIAPI		DuiHasState			(HDE hde, DWORD dwState);
BOOL DUIAPI		DuiIsVisible		(HDE hde);
void DUIAPI		DuiSetVisible		(HDE hde, BOOL bVisible);
BOOL DUIAPI		DuiIsEnabled		(HDE hde);
void DUIAPI		DuiSetEnabled		(HDE hde, BOOL bEnabled);
BOOL DUIAPI		DuiIsFocused		(HDE hde);
void DUIAPI		DuiSetFocus			(HDE hde);
void DUIAPI		DuiInvalidate		(HDE hde);
RECT DUIAPI		DuiGetRect			(HDE hde);
RECT DUIAPI		DuiGetClientRect	(HDE hde);
int DUIAPI		DuiGetChildCount	(HDE hde);
HDE DUIAPI		DuiGetChild			(HDE hde, int index);
BOOL DUIAPI		DuiRemoveChild		(HDE hde, HDE hdeChild);
void DUIAPI		DuiRemoveAllChildren(HDE hde);
BOOL DUIAPI		DuiSetInnerXML		(HDE hde, LPCWSTR lpszXml);
BOOL DUIAPI		DuiSetOuterXML		(HDE hde, LPCWSTR lpszXml);
HRGN DUIAPI		DuiGetRegion		(HDE hde);
BOOL DUIAPI		DuiGetAttribute		(HDE hde, LPCWSTR lpszAttr, VARIANT* pvResult);
LPCWSTR DUIAPI	DuiGetAttributeString(HDE hde, LPCWSTR lpszAttr);
void DUIAPI		DuiSetAttribute		(HDE hde, LPCWSTR lpszAttr, VARIANT* value);
void DUIAPI		DuiSetAttributeString(HDE hde, LPCWSTR lpszAttr, LPCWSTR lpszValue);
void DUIAPI		DuiRemoveAttribute	(HDE hde, LPCWSTR lpszAttr);

#ifdef __cplusplus
}

//////////////////////////////////////////////////////////////////////////
// C++ wrapped class

#ifdef ATLASSERT
#define _assert ATLASSERT
#elif defined(ASSERT)
#define _assert ASSERT
#else
#define _assert __noop
#endif

// forword declaration
class CDuiWindow;
class CDuiElement;

//////////////////////////////////////////////////////////////////////////
// CDuiWindow
class CDummyWindow
{
public:
	HWND m_hWnd;
	operator HWND() { return m_hWnd; }
};

class CDuiWindow : 
#ifdef __ATLWIN_H__
	public ATL::CWindow
#elif defined(__AFXWIN_H__)
	public CWnd
#else
	public CDummyWindow
#endif // __ATLWIN_H__
{
public:
	CDuiWindow(HWND hwnd = NULL) throw() { m_hWnd = hwnd; }

	CDuiWindow& operator=(HWND hwnd) throw()
	{
		m_hWnd = hwnd;
		return *this;
	}

	static LPCTSTR GetWndClassName() throw()
	{
		return _T("DirectUIHWND");
	}

	BOOL LoadFromXml(LPCTSTR lpszXml)
	{
		_assert(::IsDirectUIWindow(m_hWnd));
		return (BOOL)::SendMessage(m_hWnd, WM_LOADXML, (WPARAM)(LPCOLESTR)CT2COLE(lpszXml), 0);
	}

	BOOL LoadFromUrl(LPCTSTR lpszUrl, HINSTANCE hMod=NULL)
	{
		_assert(::IsDirectUIWindow(m_hWnd));
		return (BOOL)::SendMessage(m_hWnd, WM_LOADURL, (WPARAM)(LPCOLESTR)CT2COLE(lpszUrl), (LPARAM)hMod);
	}

#ifdef __ATLWIN_H__
	HWND Create(HWND hWndParent, _U_RECT rect = NULL, LPCTSTR szWindowName = NULL,
		DWORD dwStyle = 0, DWORD dwExStyle = 0,
		_U_MENUorID MenuOrID = 0U, LPVOID lpCreateParam = NULL) throw()
	{
		ATLASSUME(m_hWnd == NULL);
		if (rect.m_lpRect == NULL)
			rect.m_lpRect = &rcDefault;
		m_hWnd = ::CreateDirectUIWindow(hWndParent, szWindowName, *rect.m_lpRect, dwStyle, dwExStyle, MenuOrID.m_hMenu, _AtlBaseModule.GetModuleInstance());
		return m_hWnd;
	}
#endif // __ATLWIN_H__

#ifdef __AFXWIN_H__
	//DECLARE_DYNCREATE(CDuiWindow)
#ifdef _AFXDLL
	CRuntimeClass* PASCAL _GetBaseClass() { return RUNTIME_CLASS(CWnd); }
	CRuntimeClass* PASCAL GetThisClass() { return (CRuntimeClass*)GetRuntimeClass(); }
#endif // _AFXDLL
	virtual CRuntimeClass* GetRuntimeClass() const
	{
		static const CRuntimeClass _runtimeClass = {"CDuiWindow", sizeof(CDuiWindow), 0xFFFF, CDuiWindow::CreateObject, 
#ifdef _AFXDLL
			&CDuiWindow::_GetBaseClass,
#else
			RUNTIME_CLASS(CWnd),
#endif // _AFXDLL
			NULL, NULL};
		return &_runtimeClass;
	}
	static CObject* PASCAL CreateObject() { return NEW CDuiWindow; }

	BOOL Create(CWnd* pParentWnd, const RECT& rect, LPCTSTR lpszWindowName = NULL, DWORD dwStyle = 0, DWORD dwExStyle = 0,
		HMENU nIDorHMenu = NULL, LPVOID lpParam = NULL)
	{
		//return Attach(::CreateDirectUIWindow(pParentWnd?pParentWnd->GetSafeHwnd():NULL, lpszWindowName, rect, dwStyle, dwExStyle, nIDorHMenu, AfxGetInstanceHandle()));
		return CWnd::CreateEx(dwExStyle, GetWndClassName(), lpszWindowName, dwStyle, rect, pParentWnd, (UINT)(UINT_PTR)nIDorHMenu, lpParam);
	}

	virtual BOOL PreTranslateMessage(MSG* pMsg)
	{
		return IsDirectUIMessage(pMsg) || CWnd::PreTranslateMessage(pMsg);
	}
#endif // __AFXWIN_H__

	BOOL IsDirectUIMessage(LPMSG lpMsg)
	{
		_assert(::IsDirectUIWindow(m_hWnd));
		return ::IsDirectUIMessage(m_hWnd, lpMsg);
	}

	CDuiElement GetRootElement();
	CDuiElement GetElementById(LPCTSTR lpszId);

	BOOL ParseStyle(LPCWSTR lpszTargetName, DWORD dwTargetState, LPCWSTR lpszStyle)
	{
		_assert(::IsDirectUIWindow(m_hWnd));
		return ::ParseStyle(m_hWnd, lpszTargetName, dwTargetState, lpszStyle);
	}

	void SetExternal(IDispatch* pDisp)
	{
		_assert(::IsDirectUIWindow(m_hWnd));
		::SetDirectUIWindowExternal(m_hWnd, pDisp);
	}

	BOOL CalcExpression(LPCTSTR lpszExpression, LPVARIANT lpVarResult=NULL)
	{
		_assert(::IsDirectUIWindow(m_hWnd));
		return ::CalcScriptExpression(m_hWnd, lpszExpression, lpVarResult);
	}

	// 以下封装方法是对上述成员函数的再封装，GET类型的函数不包含在内，用于级联调用，忽略所有原始返回值，且首字母改成小写，要注意区分
	CDuiWindow& loadFromXml(LPCTSTR lpszXml) { LoadFromXml(lpszXml); return *this; }
	CDuiWindow& loadFromUrl(LPCTSTR lpszUrl, HINSTANCE hMod=NULL) { LoadFromUrl(lpszUrl, hMod); return *this; }
#ifdef __ATLWIN_H__
	CDuiWindow& create(HWND hWndParent, _U_RECT rect = NULL, LPCTSTR szWindowName = NULL,
		DWORD dwStyle = 0, DWORD dwExStyle = 0,
		_U_MENUorID MenuOrID = 0U, LPVOID lpCreateParam = NULL)
	{ Create(hWndParent, rect, szWindowName, dwStyle, dwExStyle, MenuOrID, lpCreateParam); return *this; }
#endif // __ATLWIN_H__

#ifdef __AFXWIN_H__
	CDuiWindow& create(CWnd* pParentWnd, const RECT& rect, LPCTSTR lpszWindowName = NULL, DWORD dwStyle = 0, DWORD dwExStyle = 0,
		HMENU nIDorHMenu = NULL, LPVOID lpParam = NULL)
	{ Create(pParentWnd, rect, lpszWindowName, dwStyle, dwExStyle, nIDorHMenu, lpParam); return *this; }
#endif // __AFXWIN_H__

	CDuiWindow& external(IDispatch* pDisp) { SetExternal(pDisp); return *this; }
	CDuiWindow& calcExpression(LPCTSTR lpszExpression, LPVARIANT lpVarResult=NULL) { CalcExpression(lpszExpression, lpVarResult); return *this; }
};

//////////////////////////////////////////////////////////////////////////
// CDuiElement
class CDuiElement
{
public:
	HDE m_hde;

	CDuiElement(HDE hde = NULL) throw() : m_hde(hde) {}

	CDuiElement& operator=(HDE hde) throw()
	{
		m_hde = hde;
		return *this;
	}

	operator HDE() const throw() { return m_hde; }

	BOOL IsElement()
	{
		return ::DuiIsElement(m_hde);
	}

	CDuiWindow GetContainerWindow()
	{
		_assert(::DuiIsElement(m_hde));
		return CDuiWindow(::DuiGetContainerHwnd(m_hde));
	}

	CDuiElement GetParent()
	{
		_assert(::DuiIsElement(m_hde));
		return CDuiElement(::DuiGetParent(m_hde));
	}

	CDuiElement GetTopParent()
	{
		_assert(::DuiIsElement(m_hde));
		return CDuiElement(::DuiGetTopParent(m_hde));
	}

	LPCWSTR GetID()
	{
		_assert(::DuiIsElement(m_hde));
		return ::DuiGetID(m_hde);
	}

	LPCWSTR GetText()
	{
		_assert(::DuiIsElement(m_hde));
		return ::DuiGetText(m_hde);
	}

	void SetText(LPCWSTR lpszText)
	{
		_assert(::DuiIsElement(m_hde));
		::DuiSetText(m_hde, lpszText);
	}

	LPCWSTR GetToolTip()
	{
		_assert(::DuiIsElement(m_hde));
		return ::DuiGetToolTip(m_hde);
	}

	void SetToolTip(LPCWSTR lpszToolTip)
	{
		_assert(::DuiIsElement(m_hde));
		::DuiSetTooTip(m_hde, lpszToolTip);
	}

	WCHAR GetShortcut()
	{
		_assert(::DuiIsElement(m_hde));
		return ::DuiGetShortcut(m_hde);
	}

	void SetShortcut(WCHAR ch)
	{
		_assert(::DuiIsElement(m_hde));
		::DuiSetShortcut(m_hde, ch);
	}

	DWORD GetState()
	{
		_assert(::DuiIsElement(m_hde));
		return ::DuiGetState(m_hde);
	}

	void ModifyState(DWORD dwAdded=0, DWORD dwRemoved=0)
	{
		_assert(::DuiIsElement(m_hde));
		::DuiModifyState(m_hde, dwAdded, dwRemoved);
	}

	BOOL HasState(DWORD dwState)
	{
		_assert(::DuiIsElement(m_hde));
		return ::DuiHasState(m_hde, dwState);
	}

	BOOL IsVisible()
	{
		_assert(::DuiIsElement(m_hde));
		return ::DuiIsVisible(m_hde);
	}

	void SetVisible(BOOL bVisible)
	{
		_assert(::DuiIsElement(m_hde));
		::DuiSetVisible(m_hde, bVisible);
	}

	BOOL IsEnabled()
	{
		_assert(::DuiIsElement(m_hde));
		return ::DuiIsEnabled(m_hde);
	}

	void SetEnabled(BOOL bEnabled)
	{
		_assert(::DuiIsElement(m_hde));
		::DuiSetEnabled(m_hde, bEnabled);
	}

	BOOL IsFocused()
	{
		_assert(::DuiIsElement(m_hde));
		return ::DuiIsFocused(m_hde);
	}

	void SetFocus()
	{
		_assert(::DuiIsElement(m_hde));
		::DuiSetFocus(m_hde);
	}

	void Invalidate()
	{
		_assert(::DuiIsElement(m_hde));
		::DuiInvalidate(m_hde);
	}

	RECT GetRect()
	{
		_assert(::DuiIsElement(m_hde));
		return ::DuiGetRect(m_hde);
	}

	RECT GetClientRect()
	{
		_assert(::DuiIsElement(m_hde));
		return ::DuiGetClientRect(m_hde);
	}

	int GetChildCount()
	{
		_assert(::DuiIsElement(m_hde));
		return ::DuiGetChildCount(m_hde);
	}

	CDuiElement GetChild(int index)
	{
		_assert(::DuiIsElement(m_hde));
		return CDuiElement(::DuiGetChild(m_hde, index));
	}

	BOOL RemoveChild(HDE hdeChild)
	{
		_assert(::DuiIsElement(m_hde));
		return ::DuiRemoveChild(m_hde, hdeChild);
	}

	void RemoveAllChildren()
	{
		_assert(::DuiIsElement(m_hde));
		::DuiRemoveAllChildren(m_hde);
	}

	BOOL SetInnerXML(LPCWSTR xml)
	{
		_assert(::DuiIsElement(m_hde));
		return ::DuiSetInnerXML(m_hde, xml);
	}

	BOOL SetOuterXML(LPCWSTR xml)
	{
		_assert(::DuiIsElement(m_hde));
		return ::DuiSetOuterXML(m_hde, xml);
	}

	// only for inline element
	HRGN GetRegion()
	{
		_assert(::DuiIsElement(m_hde));
		return ::DuiGetRegion(m_hde);
	}

	BOOL GetAttribute(LPCWSTR lpszAttr, VARIANT* pvResult = NULL)
	{
		_assert(::DuiIsElement(m_hde));
		return ::DuiGetAttribute(m_hde, lpszAttr, pvResult);
	}

	LPCWSTR GetAttributeString(LPCWSTR lpszAttr)
	{
		_assert(::DuiIsElement(m_hde));
		return ::DuiGetAttributeString(m_hde, lpszAttr);
	}

	void SetAttribute(LPCWSTR lpszAttr, VARIANT* value)
	{
		_assert(::DuiIsElement(m_hde));
		::DuiSetAttribute(m_hde, lpszAttr, value);
	}

	void SetAttributeString(LPCWSTR lpszAttr, LPCWSTR lpszValue)
	{
		_assert(::DuiIsElement(m_hde));
		::DuiSetAttributeString(m_hde, lpszAttr, lpszValue);
	}

	void RemoveAttribute(LPCWSTR lpszAttr)
	{
		_assert(::DuiIsElement(m_hde));
		::DuiRemoveAttribute(m_hde, lpszAttr);
	}

	// 以下封装方法是对上述成员函数的再封装，GET类型的函数不包含在内，用于级联调用，忽略所有原始返回值，且首字母改成小写，要注意区分
	/* example:
		CDuiElement(hde).attribute(L"display", L"block").text(L"hello world").enabled(TRUE).focus().GetAttributeString(L"tagName");
	*/
	CDuiElement& text(LPCWSTR lpszText) { SetText(lpszText); return *this; }
	CDuiElement& tooltip(LPCWSTR lpszToolTip) { SetToolTip(lpszToolTip); return *this; }
	CDuiElement& shortcut(WCHAR ch) { SetShortcut(ch); return *this; }
	CDuiElement& modifyState(DWORD dwAdded=0, DWORD dwRemoved=0) { ModifyState(dwAdded, dwRemoved); return *this; }
	CDuiElement& visible(BOOL bVisible) { SetVisible(bVisible); return *this; }
	CDuiElement& enabled(BOOL bEnabled) { SetEnabled(bEnabled); return *this; }
	CDuiElement& focus() { SetFocus(); return *this; }
	CDuiElement& invalidate() { Invalidate(); return *this; }
	CDuiElement& removeChild(HDE hdeChild) { RemoveChild(hdeChild); return *this; }
	CDuiElement& removeAllChildren() { RemoveAllChildren(); return *this; }
	CDuiElement& innerXML(LPCWSTR xml) { SetInnerXML(xml); return *this; }
	CDuiElement& outerXML(LPCWSTR xml) { SetOuterXML(xml); return *this; }
	CDuiElement& attribute(LPCWSTR lpszAttr, VARIANT* value) { SetAttribute(lpszAttr, value); return *this; }
	CDuiElement& attribute(LPCWSTR lpszAttr, LPCWSTR lpszValue) { SetAttributeString(lpszAttr, lpszValue); return *this; }
	CDuiElement& removeAttribute(LPCWSTR lpszAttr) { RemoveAttribute(lpszAttr); return *this; }
};

inline CDuiElement CDuiWindow::GetRootElement()
{
	_assert(::IsDirectUIWindow(m_hWnd));
	return CDuiElement(::GetRootElement(m_hWnd));
}

inline CDuiElement CDuiWindow::GetElementById(LPCTSTR lpszId)
{
	_assert(::IsDirectUIWindow(m_hWnd));
	return CDuiElement(::GetElementById(m_hWnd, CT2CW(lpszId)));
}


#endif // __cplusplus

#endif // __DIRECTUIWINDOW_H__