
#include "StdAfx.h"
#include <WindowsX.h>
#include <MMSystem.h>
#include "DuiBase.h"
#include "DuiManager.h"

#ifdef _DEBUG
#include <shlwapi.h>
#pragma comment(lib, "shlwapi.lib")
#endif

#pragma comment(lib, "winmm.lib")

#pragma warning(push)
//#pragma warning(disable:4049)
//#pragma warning(disable:4217)


/////////////////////////////////////////////////////////////////////////////////////
//
//

//void  __Trace(LPCTSTR pstrFormat, ...)
//{
//#ifdef _DEBUG
//	TCHAR szBuffer[300] = { 0 };
//	va_list args;
//	va_start(args, pstrFormat);
//	::wvnsprintf(szBuffer, 300 - 2, pstrFormat, args);
//	lstrcat(szBuffer, _T("\n"));
//	va_end(args);
//	::OutputDebugString(szBuffer);
//#endif
//}
//
//LPCTSTR __TraceMsg(UINT uMsg)
//{
//#define MSGDEF(x) if(uMsg==x) return _T(#x)
//	MSGDEF(WM_SETCURSOR);
//	MSGDEF(WM_NCHITTEST);
//	MSGDEF(WM_NCPAINT);
//	MSGDEF(WM_PAINT);
//	MSGDEF(WM_ERASEBKGND);
//	MSGDEF(WM_NCMOUSEMOVE);  
//	MSGDEF(WM_MOUSEMOVE);
//	MSGDEF(WM_MOUSELEAVE);
//	MSGDEF(WM_MOUSEHOVER);   
//	MSGDEF(WM_NOTIFY);
//	MSGDEF(WM_COMMAND);
//	MSGDEF(WM_MEASUREITEM);
//	MSGDEF(WM_DRAWITEM);   
//	MSGDEF(WM_LBUTTONDOWN);
//	MSGDEF(WM_LBUTTONUP);
//	MSGDEF(WM_LBUTTONDBLCLK);
//	MSGDEF(WM_RBUTTONDOWN);
//	MSGDEF(WM_RBUTTONUP);
//	MSGDEF(WM_RBUTTONDBLCLK);
//	MSGDEF(WM_SETFOCUS);
//	MSGDEF(WM_KILLFOCUS);  
//	MSGDEF(WM_MOVE);
//	MSGDEF(WM_SIZE);
//	MSGDEF(WM_SIZING);
//	MSGDEF(WM_MOVING);
//	MSGDEF(WM_GETMINMAXINFO);
//	MSGDEF(WM_CAPTURECHANGED);
//	MSGDEF(WM_WINDOWPOSCHANGED);
//	MSGDEF(WM_WINDOWPOSCHANGING);   
//	MSGDEF(WM_NCCALCSIZE);
//	MSGDEF(WM_NCCREATE);
//	MSGDEF(WM_NCDESTROY);
//	MSGDEF(WM_TIMER);
//	MSGDEF(WM_KEYDOWN);
//	MSGDEF(WM_KEYUP);
//	MSGDEF(WM_CHAR);
//	MSGDEF(WM_SYSKEYDOWN);
//	MSGDEF(WM_SYSKEYUP);
//	MSGDEF(WM_SYSCOMMAND);
//	MSGDEF(WM_SYSCHAR);
//	MSGDEF(WM_VSCROLL);
//	MSGDEF(WM_HSCROLL);
//	MSGDEF(WM_CHAR);
//	MSGDEF(WM_SHOWWINDOW);
//	MSGDEF(WM_PARENTNOTIFY);
//	MSGDEF(WM_CREATE);
//	MSGDEF(WM_NCACTIVATE);
//	MSGDEF(WM_ACTIVATE);
//	MSGDEF(WM_ACTIVATEAPP);   
//	MSGDEF(WM_CLOSE);
//	MSGDEF(WM_DESTROY);
//	MSGDEF(WM_GETICON);   
//	MSGDEF(WM_GETTEXT);
//	MSGDEF(WM_GETTEXTLENGTH);   
//	static TCHAR szMsg[10];
//	::wsprintf(szMsg, _T("0x%04X"), uMsg);
//	return szMsg;
//}



/////////////////////////////////////////////////////////////////////////////////////
//
//

CDuiWindowBase::CDuiWindowBase() : m_hWnd(NULL), m_OldWndProc(::DefWindowProc), m_bSubclassed(FALSE), m_bAutoDestroy(FALSE)
	, m_hdc32(NULL), m_hdib(NULL), m_hbmpOld(NULL), m_pBits(NULL), m_bAlpha(255), m_bDisableNotify(FALSE)
{
}

HWND CDuiWindowBase::GetHWND() const 
{ 
	return m_hWnd; 
}

UINT CDuiWindowBase::GetClassStyle() const
{
	return 0;
}

LPCTSTR CDuiWindowBase::GetSuperClassName() const
{
	return NULL;
}

LPCTSTR CDuiWindowBase::GetWindowClassName() const
{
	return NULL;
}

CDuiWindowBase::operator HWND() const
{
	return m_hWnd;
}

HWND CDuiWindowBase::Create(HWND hwndParent, LPCTSTR pstrName, DWORD dwStyle, DWORD dwExStyle, const RECT rc, HMENU hMenu, HINSTANCE hInstance)
{
	return Create(hwndParent, pstrName, dwStyle, dwExStyle, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top, hMenu);
}

HWND CDuiWindowBase::Create(HWND hwndParent, LPCTSTR pstrName, DWORD dwStyle, DWORD dwExStyle, int x, int y, int cx, int cy, HMENU hMenu, HINSTANCE hInstance)
{
	ASSERT(m_hWnd==NULL);
	//// 首先直接创建，然后再尝试注册窗口类
	////HWND hWnd = ::CreateWindowEx(dwExStyle, GetWindowClassName(), pstrName, dwStyle, x, y, cx, cy, hwndParent, hMenu, hInstance, this);
	////if (hWnd) return Subclass(hWnd);

	//if( GetSuperClassName() != NULL && !RegisterSuperclass(GetSuperClassName(), GetWindowClassName()) ) return NULL;
	//if( GetSuperClassName() == NULL && !RegisterWindowClass(GetClassStyle(), GetWindowClassName()) ) return NULL;
	//m_hWnd = ::CreateWindowEx(dwExStyle, GetWindowClassName(), pstrName, dwStyle, x, y, cx, cy, hwndParent, hMenu, hInstance, this);
	HWND hWnd = ::CreateWindowEx(dwExStyle, GetSuperClassName(), pstrName, dwStyle, x, y, cx, cy, hwndParent, hMenu, hInstance, this);
	//ASSERT(m_hWnd!=NULL);
	if (hWnd)
	{
		Subclass(hWnd);
		::SendMessage(m_hWnd, WM_SETFONT, (WPARAM)(HFONT)::GetStockObject(DEFAULT_GUI_FONT), 0);
	}
	return m_hWnd;
}

CDuiWindowBase* CDuiWindowBase::FromHWND(HWND hwnd, BOOL bAutoCreate/*=TRUE*/)
{
	if (!::IsWindow(hwnd)) return NULL;

	CDuiWindowBase* pThis = reinterpret_cast<CDuiWindowBase*>(::GetProp(hwnd, _T("WndX")));
	if (pThis == NULL && bAutoCreate)
	{
		pThis = NEW CDuiWindowBase;
		pThis->Subclass(hwnd);
		pThis->m_bAutoDestroy = TRUE;
	}
	return pThis;
}

HWND CDuiWindowBase::Subclass(HWND hWnd)
{
	ASSERT(::IsWindow(hWnd));
	ASSERT(m_hWnd==NULL);

	WNDCLASSEX wc = { 0 };
	wc.cbSize = sizeof(WNDCLASSEX);
	LPCOLESTR classname = GetSuperClassName();
	if( !::GetClassInfoEx(NULL, classname, &wc) ) {
		if( !::GetClassInfoEx(_AtlBaseModule.GetModuleInstance(), classname, &wc) ) {
			ASSERT(!"Unable to locate window class");
			return NULL;
		}
	}

	m_OldWndProc = SubclassWindow(hWnd, __WndProc);
	if( m_OldWndProc == NULL ) return NULL;
	m_bSubclassed = TRUE;
	m_hWnd = hWnd;
	::SetProp(m_hWnd, _T("WndX"), (HANDLE) this);
	::SetClassLongW(m_hWnd, GCL_STYLE, (LONG)(::GetClassLongW(m_hWnd, GCL_STYLE) | CS_PARENTDC));
	return m_hWnd;
}

void CDuiWindowBase::Unsubclass()
{
	ASSERT(::IsWindow(m_hWnd));
	if( !::IsWindow(m_hWnd) ) return;
	if( !m_bSubclassed ) return;
	SubclassWindow(m_hWnd, m_OldWndProc);
	m_OldWndProc = ::DefWindowProc;
	m_bSubclassed = FALSE;
	::SetProp(m_hWnd, _T("WndX"), (HANDLE) NULL);
}

void CDuiWindowBase::ShowWindow(BOOL bShow /*= TRUE*/, BOOL bTakeFocus /*= FALSE*/)
{
	ASSERT(::IsWindow(m_hWnd));
	if( !::IsWindow(m_hWnd) ) return;
	::ShowWindow(m_hWnd, bShow ? (bTakeFocus ? SW_SHOWNORMAL : SW_SHOWNOACTIVATE) : SW_HIDE);
}

BOOL CDuiWindowBase::ShowModal()
{
	ASSERT(::IsWindow(m_hWnd));
	HWND hWndParent = GetWindowOwner(m_hWnd);
	::ShowWindow(m_hWnd, SW_SHOWNORMAL);
	::EnableWindow(hWndParent, FALSE);
	MSG msg = { 0 };
	while( ::IsWindow(m_hWnd) && ::GetMessage(&msg, NULL, 0, 0) ) {
		if( msg.message == WM_CLOSE ) {
			::EnableWindow(hWndParent, TRUE);
			::SetFocus(hWndParent);
		}
		if( !CDuiLayoutManager::PreTranslateMessage(&msg) ) {
			::TranslateMessage(&msg);
			::DispatchMessage(&msg);
		}
		if( msg.message == WM_QUIT ) break;
	}
	::EnableWindow(hWndParent, TRUE);
	::SetFocus(hWndParent);
	if( msg.message == WM_QUIT ) ::PostQuitMessage(msg.wParam);
	return TRUE;
}

void CDuiWindowBase::Close()
{
	if( !::IsWindow(m_hWnd) ) return;
	PostMessage(WM_CLOSE);
}

void CDuiWindowBase::CenterWindow()
{
	ASSERT(::IsWindow(m_hWnd));
	ASSERT((GetWindowStyle(m_hWnd)&WS_CHILD)==0);
	RECT rcDlg = { 0 };
	::GetWindowRect(m_hWnd, &rcDlg);
	RECT rcArea = { 0 };
	RECT rcCenter = { 0 };
	HWND hWndParent = ::GetParent(m_hWnd);
	HWND hWndCenter = ::GetWindowOwner(m_hWnd);
	::SystemParametersInfo(SPI_GETWORKAREA, NULL, &rcArea, NULL);
	if( hWndCenter == NULL ) rcCenter = rcArea; else ::GetWindowRect(hWndCenter, &rcCenter);

	int DlgWidth = rcDlg.right - rcDlg.left;
	int DlgHeight = rcDlg.bottom - rcDlg.top;

	// Find dialog's upper left based on rcCenter
	int xLeft = (rcCenter.left + rcCenter.right) / 2 - DlgWidth / 2;
	int yTop = (rcCenter.top + rcCenter.bottom) / 2 - DlgHeight / 2;

	// The dialog is outside the screen, move it inside
	if( xLeft < rcArea.left ) xLeft = rcArea.left;
	else if( xLeft + DlgWidth > rcArea.right ) xLeft = rcArea.right - DlgWidth;
	if( yTop < rcArea.top ) yTop = rcArea.top;
	else if( yTop + DlgHeight > rcArea.bottom ) yTop = rcArea.bottom - DlgHeight;
	::SetWindowPos(m_hWnd, NULL, xLeft, yTop, -1, -1, SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
}

BOOL CDuiWindowBase::RegisterWindowClass(UINT style, LPCTSTR szClassName)
{
	WNDCLASS wc = { 0 };
	wc.style = style | CS_PARENTDC;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hIcon = NULL;
	wc.lpfnWndProc = CDuiWindowBase::__WndProc;
	wc.hInstance = _AtlBaseModule.GetModuleInstance();
	wc.hCursor = ::LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = NULL;
	wc.lpszMenuName  = NULL;
	wc.lpszClassName = szClassName;
	ATOM ret = ::RegisterClass(&wc);
	ASSERT(ret!=NULL || ::GetLastError()==ERROR_CLASS_ALREADY_EXISTS);
	return ret != NULL || ::GetLastError() == ERROR_CLASS_ALREADY_EXISTS;
}

BOOL CDuiWindowBase::RegisterSuperclass(LPCTSTR szSuperClassName, LPCTSTR szWindowClassName)
{
	// Get the class information from an existing
	// window so we can subclass it later on...
	WNDCLASSEX wc = { 0 };
	wc.cbSize = sizeof(WNDCLASSEX);
	if( !::GetClassInfoEx(NULL, szSuperClassName, &wc) ) {
		if( !::GetClassInfoEx(_AtlBaseModule.GetModuleInstance(), szSuperClassName, &wc) ) {
			ASSERT(!"Unable to locate window class");
			return NULL;
		}
	}
	m_OldWndProc = wc.lpfnWndProc;
	wc.lpfnWndProc = CDuiWindowBase::__ControlProc;
	wc.hInstance = _AtlBaseModule.GetModuleInstance();
	wc.lpszClassName = szWindowClassName;
	ATOM ret = ::RegisterClassEx(&wc);
	ASSERT(ret!=NULL || ::GetLastError()==ERROR_CLASS_ALREADY_EXISTS);
	return ret != NULL || ::GetLastError() == ERROR_CLASS_ALREADY_EXISTS;
}

LRESULT CALLBACK CDuiWindowBase::__WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	CDuiWindowBase* pThis = NULL;
	if( uMsg == WM_NCCREATE )
	{
		LPCREATESTRUCT lpcs = reinterpret_cast<LPCREATESTRUCT>(lParam);
		pThis = static_cast<CDuiWindowBase*>(lpcs->lpCreateParams);
		pThis->m_hWnd = hWnd;
		::SetProp(hWnd, _T("WndX"), (HANDLE) pThis);
		//::SetWindowLongW(hWnd, GWL_EXSTYLE, (DWORD)::GetWindowLongW(hWnd,GWL_EXSTYLE) & ~WS_EX_COMPOSITED);
		//pThis->HookToDC();
		//::SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LPARAM>(pThis));
	} 
	else
	{
		pThis = reinterpret_cast<CDuiWindowBase*>(::GetProp(hWnd, _T("WndX")));
		//pThis = reinterpret_cast<CDuiWindowBase*>(::GetWindowLongPtr(hWnd, GWLP_USERDATA));
		if( uMsg == WM_NCDESTROY && pThis != NULL )
		{
			LRESULT lRes = ::CallWindowProc(pThis->m_OldWndProc, hWnd, uMsg, wParam, lParam);
			//::SetWindowLongPtr(pThis->m_hWnd, GWLP_USERDATA, 0L);
			if( pThis->m_bSubclassed ) pThis->Unsubclass();
			::RemoveProp(hWnd, _T("WndX"));
			::RemoveProp(hWnd, AtomString(3333));
			pThis->ReleaseDib32();
			pThis->m_hWnd = NULL;
			pThis->OnFinalMessage(hWnd);
			//if (!::IsBadReadPtr(pThis,sizeof(CDuiWindowBase)) && pThis->m_bAutoDestroy) delete pThis;
			return lRes;
		}
	}
	if( pThis != NULL )
		return pThis->HandleMessage(uMsg, wParam, lParam);
	else
		return ::DefWindowProc(hWnd, uMsg, wParam, lParam);
}

LRESULT CALLBACK CDuiWindowBase::__ControlProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	CDuiWindowBase* pThis = NULL;
	if( uMsg == WM_NCCREATE )
	{
		LPCREATESTRUCT lpcs = reinterpret_cast<LPCREATESTRUCT>(lParam);
		pThis = static_cast<CDuiWindowBase*>(lpcs->lpCreateParams);
		::SetProp(hWnd, _T("WndX"), (HANDLE) pThis);
		pThis->m_hWnd = hWnd;
		//pThis->HookToDC();
	} 
	else
	{
		pThis = reinterpret_cast<CDuiWindowBase*>(::GetProp(hWnd, _T("WndX")));
		if( uMsg == WM_NCDESTROY && pThis != NULL )
		{
			LRESULT lRes = ::CallWindowProc(pThis->m_OldWndProc, hWnd, uMsg, wParam, lParam);
			if( pThis->m_bSubclassed ) pThis->Unsubclass();
			::RemoveProp(hWnd, _T("WndX"));
			::RemoveProp(hWnd, AtomString(3333));
			pThis->ReleaseDib32();
			pThis->m_hWnd = NULL;
			pThis->OnFinalMessage(hWnd);
			//if (!::IsBadReadPtr(pThis,sizeof(CDuiWindowBase)) && pThis->m_bAutoDestroy) delete pThis;
			return lRes;
		}
	}
	if( pThis != NULL )
		return pThis->HandleMessage(uMsg, wParam, lParam);
	else
		return ::DefWindowProc(hWnd, uMsg, wParam, lParam);
}

LRESULT CDuiWindowBase::SendMessage(UINT uMsg, WPARAM wParam /*= 0*/, LPARAM lParam /*= 0*/)
{
	ASSERT(::IsWindow(m_hWnd));
	return ::SendMessage(m_hWnd, uMsg, wParam, lParam);
} 

LRESULT CDuiWindowBase::PostMessage(UINT uMsg, WPARAM wParam /*= 0*/, LPARAM lParam /*= 0*/)
{
	ASSERT(::IsWindow(m_hWnd));
	return ::PostMessage(m_hWnd, uMsg, wParam, lParam);
}

void CDuiWindowBase::ResizeClient(int cx /*= -1*/, int cy /*= -1*/)
{
	ASSERT(::IsWindow(m_hWnd));
	RECT rc = { 0 };;
	if( !::GetClientRect(m_hWnd, &rc) ) return;
	if( cx != -1 ) rc.right = cx;
	if( cy != -1 ) rc.bottom = cy;
	if( !::AdjustWindowRectEx(&rc, GetWindowStyle(m_hWnd), (!(GetWindowStyle(m_hWnd) & WS_CHILD) && (::GetMenu(m_hWnd) != NULL)), GetWindowExStyle(m_hWnd)) ) return;
	UINT uFlags = SWP_NOZORDER | SWP_NOMOVE;
	::SetWindowPos(m_hWnd, NULL, 0, 0, rc.right - rc.left, rc.bottom - rc.top, uFlags);
}

LRESULT CDuiWindowBase::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	//if (uMsg==WM_PAINT && ::GetParent(m_hWnd)!=NULL && m_hdc32 && wParam==0) // 必须判断 wParam==0，这是普通的绘制消息，当UpdateDib也会产生绘制消息，但是wParam!=0
	//{
	//	/*if (m_hdc32)*/ UpdateDib();
	//	PAINTSTRUCT ps;
	//	::BeginPaint(m_hWnd, &ps);
	//	::EndPaint(m_hWnd, &ps);
	//	if (!m_bDisableNotify) ::SendMessage(::GetParent(m_hWnd), WM_CHILDPAINTED, (WPARAM)m_hWnd, (LPARAM)this);
	//	return 0;
	//	//wParam = (WPARAM)m_hdc32;
	//}
	//else
	if (uMsg == WM_SIZE) UpdateSize();
#if 1
	else if ((uMsg==WM_PRINTCLIENT || uMsg==WM_PAINT) && m_hdc32)
	{
		WCHAR name[256] = L"";
		::GetClassNameW(m_hWnd, name, 256);
		//WNDPROC fn = g_wcs.GetWndProc(name);
		//if (fn) return fn(m_hWnd, WM_PAINT, wParam, lParam);
		//WNDCLASSEXW wc = {sizeof(WNDCLASSEXW)};
		//BOOL b = ::GetClassInfoExW(NULL, name, &wc);
		WNDPROC wp1 = (WNDPROC)GetWindowLongPtrW(m_hWnd, GWLP_WNDPROC);
		WNDPROC wp2 = (WNDPROC)GetClassLongPtrW(m_hWnd, GCLP_WNDPROC);
		//return ::CallWindowProc(m_OldWndProc, m_hWnd, WM_PAINT, (WPARAM)0, lParam);
		return ::CallWindowProc(m_OldWndProc, m_hWnd, uMsg, wParam, lParam);
		//return ::SendMessage(m_hWnd, WM_PAINT, wParam, lParam);
	}
#endif // _DEBUG
	//if (uMsg==WM_PAINT && m_hdc32 /*&& wParam==0*/)
	//{
	//	//wParam = (WPARAM)m_hdc32;
	//	return ::CallWindowProc(m_OldWndProc, m_hWnd, uMsg, wParam, lParam);
	//}

	LRESULT lres = ::CallWindowProc(m_OldWndProc, m_hWnd, uMsg, wParam, lParam);
	
	if (m_hdc32 &&
		((uMsg>=WM_KEYFIRST && uMsg<=WM_KEYLAST) ||
		(uMsg>=WM_MOUSEFIRST && uMsg<=WM_MOUSELAST)) /*&&
		!::GetUpdateRect(m_hWnd, NULL, FALSE)*/) // 如果GetUpdateRect返回值为TRUE，则会自动产生WM_PAINT消息，所以这里只关注FALSE的情况
	{
		UpdateDib();
		if (!m_bDisableNotify) ::SendMessage(::GetParent(m_hWnd), WM_CHILDPAINTED, (WPARAM)m_hWnd, (LPARAM)this);
	}
	else if (uMsg == WM_CHILDPAINTED)
	{
		if (m_hdc32) UpdateDib();
		if (!m_bDisableNotify) ::SendMessage(::GetParent(m_hWnd), WM_CHILDPAINTED, (WPARAM)m_hWnd, (LPARAM)this);
	}
	else if (/*m_hdc32 &&*/ uMsg==WM_SETFOCUS)
	{
		GUITHREADINFO gti = {sizeof(GUITHREADINFO)};
		GetGUIThreadInfo(GetCurrentThreadId(), &gti);
		if (gti.hwndCaret && (m_hWnd==gti.hwndCaret /*|| ::IsChild(m_hWnd,gti.hwndCaret)*/))
		{
			UINT blinkTime = GetCaretBlinkTime();
			if (blinkTime!=0 && blinkTime!=INFINITE)
			{
				::SetTimer(m_hWnd, 3456, blinkTime, NULL);
			}
		}
	}
	else if (/*m_hdc32 &&*/ uMsg==WM_KILLFOCUS)
	{
		::KillTimer(m_hWnd, 3456);
	}
	else if (/*m_hdc32 &&*/ uMsg==WM_TIMER && wParam==3456)
	{
		/*if (m_hdc32)*/ UpdateCaret(TRUE);
		if (!m_bDisableNotify) ::SendMessage(::GetParent(m_hWnd), WM_CHILDPAINTED, (WPARAM)m_hWnd, (LPARAM)this);
	}
	return lres;
}

void CDuiWindowBase::OnFinalMessage(HWND /*hWnd*/)
{
	if (m_bAutoDestroy) delete this;
}

void CDuiWindowBase::ReleaseDib32()
{
	if (m_hdc32)
	{
		if (m_hdib)
		{
			::SelectObject(m_hdc32, m_hbmpOld);
			::DeleteObject(m_hdib);
			m_hdib = NULL;
			m_pBits = NULL;
			m_hbmpOld = NULL;
		}
		::DeleteDC(m_hdc32);
		//::ReleaseDC(m_hWnd, m_hdc32);
		m_hdc32 = NULL;
	}
}

void CDuiWindowBase::UpdateSize()
{
	return;
	if (m_hdc32)
	{
		if (m_hdib)
		{
			::SelectObject(m_hdc32, m_hbmpOld);
			::DeleteObject(m_hdib);
			m_hdib = NULL;
			m_pBits = NULL;
			m_hbmpOld = NULL;
		}

		CRect rcWin;
		::GetWindowRect(m_hWnd, &rcWin);

		BITMAPINFO bmi = { 0 };
		bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		bmi.bmiHeader.biWidth = rcWin.Width();
		bmi.bmiHeader.biHeight = -rcWin.Height();
		bmi.bmiHeader.biPlanes = 1;
		bmi.bmiHeader.biBitCount = 32;
		bmi.bmiHeader.biCompression = BI_RGB;
		bmi.bmiHeader.biSizeImage = rcWin.Width() * rcWin.Height() * sizeof(DWORD);
		m_hdib = ::CreateDIBSection(CWindowDC(m_hWnd).m_hDC, &bmi, DIB_RGB_COLORS, (LPVOID*) &m_pBits, NULL, 0);
		if (m_hdib)
		{
			m_hbmpOld = (HBITMAP) ::SelectObject(m_hdc32, m_hdib);
		}

		UpdateDib();
	}
	::SendMessage(::GetParent(m_hWnd), WM_CHILDSIZED, (WPARAM)m_hWnd, (LPARAM)0);
}

void CDuiWindowBase::UpdateDib()
{
	return;
	if (m_hdc32 && m_pBits && ::IsWindow(m_hWnd) && ::IsWindowVisible(m_hWnd))
	{
		CRect rcWin;
		::GetWindowRect(m_hWnd, &rcWin);

		m_bDisableNotify = TRUE;
		::SendMessage(m_hWnd, WM_PRINT, (WPARAM)m_hdc32, (LPARAM)(PRF_CHECKVISIBLE|PRF_NONCLIENT|PRF_CLIENT|PRF_ERASEBKGND|PRF_CHILDREN|PRF_OWNED));
		//HWND hwndP = ::GetParent(m_hWnd);
		//::ScreenToClient(hwndP, (LPPOINT)&rcWin);
		//::ScreenToClient(hwndP, ((LPPOINT)&rcWin)+1);
		//::BitBlt(m_hdc32, 0, 0, rcWin.Width(), rcWin.Height(), CClientDC(hwndP).m_hDC, rcWin.left, rcWin.top, SRCCOPY);
		m_bDisableNotify = FALSE;
		UpdateCaret(FALSE);
		//UpdateAlpha();
	}
}

void CDuiWindowBase::UpdateCaret(BOOL bForce/*=FALSE*/)
{
	return;
	if (m_hdc32 && ::IsWindow(m_hWnd) && ::IsWindowVisible(m_hWnd))
	{
		GUITHREADINFO gti = {sizeof(GUITHREADINFO)};
		GetGUIThreadInfo(GetCurrentThreadId(), &gti);
		if (gti.hwndCaret && (m_hWnd==gti.hwndCaret || ::IsChild(m_hWnd,gti.hwndCaret)) && !::IsRectEmpty(&gti.rcCaret) &&
			(bForce || (gti.flags&GUI_CARETBLINKING)))
		{
			CRect rc = gti.rcCaret;
			::ClientToScreen(gti.hwndCaret, (LPPOINT)&rc);
			::ClientToScreen(gti.hwndCaret, ((LPPOINT)&rc)+1);
			::ScreenToClient(m_hWnd, (LPPOINT)&rc);
			::ScreenToClient(m_hWnd, ((LPPOINT)&rc)+1);
			CPoint pt;
			pt = CLayerHelper::GetClientOffset(m_hWnd);
			rc.OffsetRect(pt);

			CRect rcWin;
			::GetWindowRect(m_hWnd, &rcWin);
			for (int y=max(rc.top,0); y<min(rc.bottom,rcWin.Height()); y++)
			{
				LPDWORD pBits = m_pBits + y * rcWin.Width();
				for (int x=max(rc.left,0); x<min(rc.right,rcWin.Width()); x++)
				{
					pBits[x] = (0xFF000000 | (0x00FFFFFF-(pBits[x]&0x00FFFFFF)));
				}
			}
		}
	}
	else if (m_hdc32==NULL)
	{
		//::SendMessage(::GetParent(m_hWnd), WM_CHILDCARET, 0, 0);
		CDuiWindowBase* pwin = __GetOwnerParent(m_hWnd);
		if (pwin) pwin->UpdateCaret(bForce);
	}
}

void CDuiWindowBase::UpdateAlpha()
{
	return;
	if (m_hdc32 && m_pBits && ::IsWindow(m_hWnd) && ::IsWindowVisible(m_hWnd))
	{
		CRect rcWin;
		::GetWindowRect(m_hWnd, &rcWin);

		LPDWORD pBits = m_pBits;
		for( int y = 0; y < rcWin.Height(); y++ )
		{
			for( int x = 0; x < rcWin.Width(); x++ )
			{
				*pBits++ |= (m_bAlpha<<24);
			}
		}
	}
}

CDuiWindowBase* CDuiWindowBase::__GetOwnerParent(HWND hWnd)
{
	CDuiWindowBase* pwin = FromHWND(hWnd, FALSE);// reinterpret_cast<CDuiWindowBase*>(::GetProp(hWnd, _T("WndX")));
	if (pwin && pwin->m_hdc32) return pwin;

	HWND hwndParent = ::GetParent(hWnd);
	pwin = FromHWND(hwndParent, FALSE);
	while (pwin && pwin->m_hdc32==NULL)
	{
		hwndParent = ::GetParent(hwndParent);
		pwin = FromHWND(hwndParent, FALSE);
	}
	return pwin;
}

void CDuiWindowBase::DrawToDC32(HDC hdc, RECT rc, BOOL bUpdateAlpha/*=TRUE*/)
{
	return;
	if (::IsWindow(m_hWnd) && ::IsWindowVisible(m_hWnd))
	{
		CRect rcWin;
		::GetWindowRect(m_hWnd, &rcWin);

		if (m_hdc32 == NULL)
		{
			m_hdc32 = ::CreateCompatibleDC(CWindowDC(m_hWnd).m_hDC);
			//m_hdc32 = ::GetDC(m_hWnd);

			UpdateSize();
		}
		if (bUpdateAlpha)
			UpdateAlpha();

		::BitBlt(hdc, rc.left, rc.top, rcWin.Width(), rcWin.Height(), m_hdc32, 0, 0, SRCCOPY);
	}
}

void CDuiWindowBase::HookToDC()
{
	return;
	if (::IsWindow(m_hWnd) /*&& ::IsWindowVisible(m_hWnd)*/ && m_hdc32==NULL)
	{
		m_hdc32 = ::CreateCompatibleDC(NULL);
		::SetPropW(m_hWnd, AtomString(3333), m_hdc32);
		UpdateSize();
	}
}


/////////////////////////////////////////////////////////////////////////////////////
//
//

CStdValArray::CStdValArray(int iElementSize, int iPreallocSize /*= 0*/) : 
	m_pVoid(NULL), 
	m_nCount(0), 
	m_iElementSize(iElementSize), 
	m_nAllocated(iPreallocSize)
{
	ASSERT(iElementSize>0);
	ASSERT(iPreallocSize>=0);
	if( iPreallocSize > 0 ) m_pVoid = static_cast<LPBYTE>(malloc(iPreallocSize * m_iElementSize));
}

CStdValArray::~CStdValArray()
{
	if( m_pVoid != NULL ) free(m_pVoid);
}

BOOL CStdValArray::IsEmpty() const
{
	return m_nCount == 0;
}

BOOL CStdValArray::Insert(int iIndex, LPVOID pData)
{
	if (++m_nCount >= m_nAllocated)
	{
		m_nAllocated *= 2;
		if( m_nAllocated == 0 ) m_nAllocated = 11;
		m_pVoid = static_cast<LPBYTE>(realloc(m_pVoid, m_nAllocated * m_iElementSize));
		if( m_pVoid == NULL ) return FALSE;
	}
	if (iIndex < 0) return FALSE;
	if (iIndex >= m_nCount) iIndex = m_nCount - 1;
	for (int i=m_nCount-1; i>iIndex; i--) ::CopyMemory(m_pVoid+(i*m_iElementSize), m_pVoid+((i-1)*m_iElementSize), m_iElementSize);
	::CopyMemory(m_pVoid+(iIndex*m_iElementSize), pData, m_iElementSize);
	return TRUE;
}

BOOL CStdValArray::Add(LPCVOID pData)
{
	return Insert(m_nCount, (LPVOID)pData);
	//if( ++m_nCount >= m_nAllocated)
	//{
	//	m_nAllocated *= 2;
	//	if( m_nAllocated == 0 ) m_nAllocated = 11;
	//	m_pVoid = static_cast<LPBYTE>(realloc(m_pVoid, m_nAllocated * m_iElementSize));
	//	if( m_pVoid == NULL ) return FALSE;
	//}
	//::CopyMemory(m_pVoid + ((m_nCount - 1) * m_iElementSize), pData, m_iElementSize);
	//return TRUE;
}

BOOL CStdValArray::Remove(LPVOID pData)
{
	for (int i=0; i<m_nCount; i++)
		if (::memcmp(m_pVoid+(i*m_iElementSize), pData, m_iElementSize) == 0)
			return RemoveAt(i);
	return FALSE;
}

BOOL CStdValArray::RemoveAt(int iIndex)
{
	if( iIndex < 0 || iIndex >= m_nCount ) return FALSE;
	if( iIndex < --m_nCount ) ::CopyMemory(m_pVoid + (iIndex * m_iElementSize), m_pVoid + ((iIndex + 1) * m_iElementSize), (m_nCount - iIndex) * m_iElementSize);
	return TRUE;
}

void CStdValArray::RemoveAll()
{
	m_nCount = 0;
	m_nAllocated = 0;
	if (m_pVoid) free(m_pVoid);
	m_pVoid = NULL;
}

int CStdValArray::GetSize() const
{
	return m_nCount;
}

LPVOID CStdValArray::GetData()
{
	return static_cast<LPVOID>(m_pVoid);
}

LPVOID CStdValArray::GetAt(int iIndex) const
{
	if( iIndex < 0 || iIndex >= m_nCount ) return NULL;
	return m_pVoid + (iIndex * m_iElementSize);
}

LPVOID CStdValArray::operator[] (int iIndex) const
{
	ASSERT(iIndex>=0 && iIndex<m_nCount);
	return m_pVoid + (iIndex * m_iElementSize);
}

//////////////////////////////////////////////////////////////////////////
//
CStdPtrArray::CStdPtrArray( int iPreallocSize/*=0*/ ) :
	m_ppVoid(NULL), 
	m_nCount(0), 
	m_nAllocated(iPreallocSize)
{
	ASSERT(iPreallocSize>=0);
	if( iPreallocSize > 0 ) m_ppVoid = static_cast<LPVOID*>(malloc(iPreallocSize * sizeof(LPVOID)));
}

CStdPtrArray::~CStdPtrArray()
{
	if (m_ppVoid) free(m_ppVoid);
	m_ppVoid = NULL;
}

BOOL CStdPtrArray::IsEmpty() const
{
	return m_nCount == 0;
}

BOOL CStdPtrArray::Insert( int iIndex, LPVOID pData )
{
	if (++m_nCount >= m_nAllocated)
	{
		m_nAllocated *= 2;
		if( m_nAllocated == 0 ) m_nAllocated = 11;
		m_ppVoid = static_cast<LPVOID*>(realloc(m_ppVoid, m_nAllocated * sizeof(LPVOID)));
		if( m_ppVoid == NULL ) return FALSE;
	}
	//if (iIndex < 0) return FALSE;
	if (iIndex<0 || iIndex >= m_nCount) iIndex = m_nCount - 1;
	for (int i=m_nCount-1; i>iIndex; i--) m_ppVoid[i]=m_ppVoid[i-1];
	m_ppVoid[iIndex] = pData;
	return TRUE;
}

BOOL CStdPtrArray::Add( LPCVOID pData )
{
	return Insert(m_nCount, (LPVOID)pData);
}

BOOL CStdPtrArray::Remove( LPVOID pData )
{
	if (m_ppVoid==NULL) return FALSE;
	for (int i=0; i<m_nCount; i++)
		if (m_ppVoid[i] == pData)
			return RemoveAt(i);
	return FALSE;
}

int CStdPtrArray::Find( const LPVOID& pData )
{
	if (m_ppVoid==NULL) return -1;
	for (int i=0; i<m_nCount; i++)
		if (m_ppVoid[i] == pData)
			return i;
	return -1;
}

LPVOID CStdPtrArray::GetAt( int iIndex ) const
{
	if(m_ppVoid==NULL || iIndex < 0 || iIndex >= m_nCount ) return NULL;
	return m_ppVoid[iIndex];
}

LPVOID CStdPtrArray::operator[]( int iIndex ) const
{
	return GetAt(iIndex);
}

BOOL CStdPtrArray::RemoveAt( int iIndex )
{
	if( iIndex < 0 || iIndex >= m_nCount ) return FALSE;
	if( iIndex < --m_nCount ) ::CopyMemory(m_ppVoid + iIndex, m_ppVoid + iIndex + 1, (m_nCount - iIndex) * sizeof(LPVOID));
	return TRUE;
}

void CStdPtrArray::RemoveAll()
{
	m_nCount = 0;
	m_nAllocated = 0;
	if (m_ppVoid) free(m_ppVoid);
	m_ppVoid = NULL;
}

int CStdPtrArray::GetSize() const
{
	return m_nCount;
}

LPVOID* CStdPtrArray::GetData()
{
	return m_ppVoid;
}

BOOL CStdPtrArray::Swap( LPVOID d1, LPVOID d2 )
{
	int idx1 = Find(d1), idx2 = Find(d2);
	if (idx1<0 || idx2<0) return FALSE;
	m_ppVoid[idx1] = d2;
	m_ppVoid[idx2] = d1;
	return TRUE;
}

BOOL CStdPtrArray::MoveTo( LPVOID pData, int iTo )
{
	if (iTo<0 || iTo>=m_nCount) return FALSE;
	if (Remove(pData))
		return Insert(iTo, pData);
	return FALSE;
}

/////////////////////////////////////////////////////////////////////////////////////
//
//

POINT CLayerHelper::GetClientOffset( HWND hwnd )
{
	CRect rc(0,0,0,0);
	AdjustWindowRectEx(&rc, (DWORD)GetWindowLong(hwnd, GWL_STYLE), ::GetMenu(hwnd)!=NULL, (DWORD)GetWindowLong(hwnd, GWL_EXSTYLE));
	return CPoint(-rc.left, -rc.top);
}

POINT CLayerHelper::OffsetPointInc( HWND hwnd, LPPOINT ppt )
{
	POINT pt = GetClientOffset(hwnd);
	ppt->x += pt.x;
	ppt->y += pt.y;
	return *ppt;
}

POINT CLayerHelper::OffsetPointDec( HWND hwnd, LPPOINT ppt )
{
	POINT pt = GetClientOffset(hwnd);
	ppt->x -= pt.x;
	ppt->y -= pt.y;
	return *ppt;
}

LineLog::~LineLog()
{
	if (_data)
	{
		CStringW& str = *(CStringW*)_data;
		OutputDebugStringW(str);
		OutputDebugStringW(L"\n");
		delete (CStringW*)_data;
	}
}

LineLog::LineLog( LPCSTR szFormat, ... )
{
	_data = NEW CStringW;
	if (_data==NULL) return;
	CStringW& str = *(CStringW*)_data;

	if (szFormat==NULL) return;
	char buf[1024];
	va_list ptr;
	va_start(ptr, szFormat);
	//ATL::CTrace::s_trace.TraceV(NULL, -1, atlTraceGeneral, 0, szFormat, ptr);
	_vsnprintf_s(buf, 1024, 1023, szFormat, ptr);
	va_end(ptr);

	str = buf;
	//OutputDebugStringA(buf);
}

LineLog::LineLog( LPCWSTR szFormat, ... )
{
	_data = NEW CStringW;
	if (_data==NULL) return;
	CStringW& str = *(CStringW*)_data;

	if (szFormat==NULL) return;
	WCHAR buf[1024];
	va_list ptr;
	va_start(ptr, szFormat);
	//ATL::CTrace::s_trace.TraceV(NULL, -1, atlTraceGeneral, 0, szFormat, ptr);
	_vsnwprintf_s(buf, 1024, 1023, szFormat, ptr);
	va_end(ptr);

	str = buf;
	//OutputDebugStringW(buf);
}

void LineLog::outputV( LPCSTR szFormat, va_list vars )
{
	if (_data==NULL) return;
	CStringW& str = *(CStringW*)_data;

	char buf[1024];
	_vsnprintf_s(buf, 1024, 1023, szFormat, vars);

	str += buf;
	//OutputDebugStringA(buf);
}

void LineLog::outputV( LPCWSTR szFormat, va_list vars )
{
	if (_data==NULL) return;
	CStringW& str = *(CStringW*)_data;

	WCHAR buf[1024];
	_vsnwprintf_s(buf, 1024, 1023, szFormat, vars);

	str += buf;
	//OutputDebugStringW(buf);
}

void LineLog::operator ()(LPCSTR szFormat, ...)
{
	if (szFormat==NULL) return;
	va_list ptr;
	va_start(ptr, szFormat);
	outputV(szFormat, ptr);
	va_end(ptr);
}

void LineLog::operator ()(LPCWSTR szFormat, ...)
{
	if (szFormat==NULL) return;
	va_list ptr;
	va_start(ptr, szFormat);
	outputV(szFormat, ptr);
	va_end(ptr);
}

//////////////////////////////////////////////////////////////////////////

TimeCounter::TimeCounter( LPCWSTR szPrompt/*=NULL*/ ) : _ll(szPrompt), _dwStart(::timeGetTime())
{

}

TimeCounter::~TimeCounter()
{
	_ll(L" [%d ms]", ::timeGetTime()-_dwStart);
}

void TimeCounter::operator()( LPCWSTR szFormat, ... )
{
	DWORD dwOld = _dwStart;
	_ll(L" [%d ms]", (_dwStart=::timeGetTime())-dwOld);

	if (szFormat==NULL) return;
	va_list ptr;
	va_start(ptr, szFormat);
	_ll.outputV(szFormat, ptr);
	va_end(ptr);
}

//////////////////////////////////////////////////////////////////////////
CAutomationCollection::~CAutomationCollection()
{
	RemoveAll();
}

BOOL CAutomationCollection::Add( LPCOLESTR name, IDispatch* disp )
{
	AutomationCollectionItem* p = NEW AutomationCollectionItem;
	if (p==NULL) return FALSE;
	p->name = name;
	p->disp = disp;
	BOOL b = m_coll.Add(p);
	if (!b) delete p;
	return b;
}

HRESULT CAutomationCollection::Getlength( VARIANT* pVar )
{
	V_VT(pVar) = VT_I4;
	V_I4(pVar) = m_coll.GetSize();
	return S_OK;
}

HRESULT CAutomationCollection::GetItem( VARIANT vIndex, VARIANT* pVar )
{
	if (vIndex.vt==VT_EMPTY || vIndex.vt==VT_ERROR)
	{
		vIndex.vt = VT_I4;
		vIndex.lVal = 0;
	}

	CComVariant v;
	if (SUCCEEDED(v.ChangeType(VT_I4, &vIndex)) && v.lVal>=0 && v.lVal<m_coll.GetSize())
	{
		V_VT(pVar) = VT_DISPATCH;
		V_DISPATCH(pVar) = static_cast<AutomationCollectionItem*>(m_coll[vIndex.lVal])->disp.p;
		V_DISPATCH(pVar)->AddRef();
		return S_OK;
	}
	if (vIndex.vt == VT_BSTR)
	{
		for (int i=0,num=m_coll.GetSize(); i<num; i++)
		{
			AutomationCollectionItem* p = static_cast<AutomationCollectionItem*>(m_coll[i]);
			if (p && p->name.m_str && lstrcmpiW(p->name.m_str, vIndex.bstrVal)==0)
			{
				V_VT(pVar) = VT_DISPATCH;
				V_DISPATCH(pVar) = p->disp.p;
				V_DISPATCH(pVar)->AddRef();
				return S_OK;
			}
		}
	}

	return DISP_E_BADINDEX;
}

void CAutomationCollection::RemoveAll()
{
	for (int i=0, num=m_coll.GetSize(); i<num; i++) delete static_cast<AutomationCollectionItem*>(m_coll[i]);
	m_coll.RemoveAll();
}

int CDuiConnector::Find( LPVOID pClient, LPVOID pServer/*=NULL*/, DWORD_PTR protocol/*=0*/ )
{
	for (int i=0, num=records.GetSize(); i<num; i++)
	{
		if (records[i].client==pClient && records[i].server==pServer && records[i].protocol==protocol)
			return i;
	}
	return -1;
}

void CDuiConnector::Connect( LPVOID pClient, LPVOID pServer/*=NULL*/, DWORD_PTR protocol/*=0*/, CDuiConnectorCallback* cb/*=NULL*/ )
{
	if (pClient==NULL && pServer==NULL) return;

	CSLocker locker(cs);
	int idx = Find(pClient, pServer, protocol);
	if (idx < 0)
	{
		_ConnectorItem ci = {pClient, pServer, protocol, cb};
		records.Add(ci);
	}
	else
		records[idx].cb = cb;
}

void CDuiConnector::Disconnect( LPVOID pClient, LPVOID pServer/*=NULL*/, DWORD_PTR protocol/*=0*/ )
{
	if (pClient==NULL && pServer==NULL) return;

	CSLocker locker(cs);
	for (int i=records.GetSize()-1; i>=0; i--)
	{
		if ((!pClient || pClient==records[i].client) &&
			(!pServer || pServer==records[i].server) &&
			(protocol==0 || protocol==records[i].protocol))
			records.RemoveAt(i);
	}
}

void CDuiConnector::Fire( LPVOID pClient, LPVOID pServer/*=NULL*/, DWORD_PTR protocol/*=0*/, CDuiConnectorCallback* cb/*=NULL*/ )
{
	if (pClient==NULL && pServer==NULL) return;

	_ConnectorItem items[1024] = {0};
	int idx = 0;

	{
		CSLocker locker(cs);
		for (int i=0, num=records.GetSize(); i<num; i++)
		{
			if ((!pClient || pClient==records[i].client) &&
				(!pServer || pServer==records[i].server) &&
				(protocol==0 || (protocol&records[i].protocol)==protocol))
				items[idx++] = records[i];
		}
	}

	for (int i=0; i<idx; i++)
	{
		CDuiConnectorCallback* cb2 = cb;
		if (cb2==NULL) cb2 = items[i].cb;
		if (cb2 && cb2->OnHit(items[i].client, items[i].server, items[i].protocol))
			return;
	}
}

CDuiConnector gConnector;

#pragma warning(pop)
