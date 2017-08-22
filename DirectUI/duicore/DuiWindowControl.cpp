#include "stdafx.h"
#include "DuiWindowControl.h"

//////////////////////////////////////////////////////////////////////////
// parse helpers

BOOL CDuiWindowControl::ParseGeneralWindowStyleString(LPCOLESTR lpszData, DWORD& style)
{
#define MAP_ITEM(x) {OLESTR(#x), lstrlenW(L#x), WS_##x},
	static const struct {LPCOLESTR name; int len_of_name; DWORD style;} __mapitem_entry[] = {
		MAP_ITEM(OVERLAPPED)	MAP_ITEM(POPUP)			/*MAP_ITEM(CHILD)*/			MAP_ITEM(MINIMIZE) 
		/*MAP_ITEM(VISIBLE) 		MAP_ITEM(DISABLED)*/ 		MAP_ITEM(CLIPSIBLINGS)	MAP_ITEM(CLIPCHILDREN) 
		MAP_ITEM(MAXIMIZE) 		MAP_ITEM(CAPTION) 		/*MAP_ITEM(BORDER)*/ 		MAP_ITEM(DLGFRAME) 
		MAP_ITEM(VSCROLL) 		MAP_ITEM(HSCROLL) 		MAP_ITEM(SYSMENU) 		MAP_ITEM(THICKFRAME) 
		MAP_ITEM(GROUP) 		/*MAP_ITEM(TABSTOP)*/ 		MAP_ITEM(MINIMIZEBOX) 	MAP_ITEM(MAXIMIZEBOX) 
		MAP_ITEM(TILED) 		MAP_ITEM(ICONIC) 		MAP_ITEM(SIZEBOX) 		MAP_ITEM(TILEDWINDOW) 
		MAP_ITEM(OVERLAPPEDWINDOW) MAP_ITEM(POPUPWINDOW) MAP_ITEM(CHILDWINDOW) 
	};
#undef MAP_ITEM

	int len = lstrlenW(lpszData);
	for (int i=0; i<sizeof(__mapitem_entry)/sizeof(__mapitem_entry[0]); i++)
	{
		if (__mapitem_entry[i].len_of_name==len && lstrcmpiW(lpszData,__mapitem_entry[i].name)==0)
			return style|=__mapitem_entry[i].style, TRUE;
	}

	return FALSE;
}

BOOL CDuiWindowControl::ParseExtendedWindowStyleString(LPCOLESTR lpszData, DWORD& style)
{
#define MAP_ITEM(x) {OLESTR(#x), lstrlenW(L#x), WS_EX_##x},
	static const struct {LPCOLESTR name; int len_of_name; DWORD style;} __mapitem_entry[] = {
		MAP_ITEM(DLGMODALFRAME)		MAP_ITEM(NOPARENTNOTIFY)	MAP_ITEM(TOPMOST)		MAP_ITEM(ACCEPTFILES) 
		MAP_ITEM(TRANSPARENT) 		MAP_ITEM(MDICHILD)			MAP_ITEM(TOOLWINDOW)	MAP_ITEM(WINDOWEDGE) 
		MAP_ITEM(CLIENTEDGE) 		MAP_ITEM(CONTEXTHELP) 		MAP_ITEM(RIGHT) 		MAP_ITEM(LEFT) 
		MAP_ITEM(RTLREADING) 		MAP_ITEM(LTRREADING) 		MAP_ITEM(LEFTSCROLLBAR) MAP_ITEM(RIGHTSCROLLBAR) 
		MAP_ITEM(CONTROLPARENT) 	MAP_ITEM(STATICEDGE) 		MAP_ITEM(APPWINDOW) 	MAP_ITEM(OVERLAPPEDWINDOW) 
		MAP_ITEM(PALETTEWINDOW) 	MAP_ITEM(LAYERED)			MAP_ITEM(NOINHERITLAYOUT)	MAP_ITEM(LAYOUTRTL) 
		/*MAP_ITEM(COMPOSITED)*/ 		MAP_ITEM(NOACTIVATE) 
	};
#undef MAP_ITEM

	int len = lstrlenW(lpszData);
	for (int i=0; i<sizeof(__mapitem_entry)/sizeof(__mapitem_entry[0]); i++)
	{
		if (__mapitem_entry[i].len_of_name==len && lstrcmpiW(lpszData,__mapitem_entry[i].name)==0)
			return style|=__mapitem_entry[i].style, TRUE;
	}

	return FALSE;
}

// 第二个参数用于指定具体控件的风格或扩展风格，例如编辑框可能提供自己的解析函数
DWORD CDuiWindowControl::TryLoadWindowStyleFromString(LPCOLESTR lpszValue)
{
	if (lpszValue==NULL || *lpszValue==0) return 0;

	DWORD style = 0;
	CStrArray strs;
	if (!SplitStringToArray(lpszValue, strs)) return 0;

	for (int i=0; i<strs.GetSize(); i++)
		if (!ParseWindowStyle(strs[i], style)) // call virtual
			ParseGeneralWindowStyleString(strs[i], style);
	return style;
}

DWORD CDuiWindowControl::TryLoadExtendedWindowStyleFromString(LPCOLESTR lpszValue)
{
	if (lpszValue==NULL || *lpszValue==0) return 0;

	DWORD style = 0;
	CStrArray strs;
	if (!SplitStringToArray(lpszValue, strs)) return 0;

	for (int i=0; i<strs.GetSize(); i++)
		if (!ParseWindowExtendedStyle(strs[i], style)) // call virtual
			ParseExtendedWindowStyleString(strs[i], style);
	return style;
}

DWORD CDuiWindowControl::ParseStyleFromVariant(CComVariant v)
{
	if (v.vt == VT_EMPTY) return 0;
	if (SUCCEEDED(v.ChangeType(VT_UI4))) return V_UI4(&v);
	if (FAILED(v.ChangeType(VT_BSTR))) return 0;
	return TryLoadWindowStyleFromString(v.bstrVal);
}

DWORD CDuiWindowControl::ParseExtendedStyleFromVariant(CComVariant v)
{
	if (v.vt == VT_EMPTY) return 0;
	if (SUCCEEDED(v.ChangeType(VT_UI4))) return V_UI4(&v);
	if (FAILED(v.ChangeType(VT_BSTR))) return 0;
	return TryLoadExtendedWindowStyleFromString(v.bstrVal);
}

//////////////////////////////////////////////////////////////////////////
//

CDuiManagedWnd::CDuiManagedWnd() : m_pOwner(NULL)
{

}

CDuiManagedWnd::~CDuiManagedWnd()
{
	if (m_pOwner) m_pOwner->m_pWin=NULL;
}

void CDuiManagedWnd::Init( CDuiWindowControl* pOwner )
{
	if (pOwner==NULL) return;
	m_pOwner = pOwner;
	DuiNode* r = m_pOwner->m_pOwner;
	Create(r->m_pLayoutMgr->GetPaintWindow(),
		r->GetText(),
		m_pOwner->m_dwStyle,
		m_pOwner->m_dwExStyle,
		//((DWORD)r->_attributes.GetAttributeLong(L"window-style", WS_CHILD|WS_VISIBLE)) | WS_CHILD,
		//(DWORD)r->_attributes.GetAttributeLong(L"extend-window-style"), 
		CRect(0,0,0,0));
}

LPCTSTR CDuiManagedWnd::GetWindowClassName() const
{
	return L"DUI_CONTROL_HWND";
}

LPCTSTR CDuiManagedWnd::GetSuperClassName() const
{
	if (m_pOwner && m_pOwner->m_pOwner)
	{
		return m_pOwner->m_pOwner->_attributes.GetAttributeString(L"class");
	}
	return NULL;
}

LRESULT CDuiManagedWnd::HandleMessage( UINT uMsg, WPARAM wParam, LPARAM lParam )
{
//#ifndef NO3D
//	if (uMsg==WM_PAINT && m_pOwner->m_pOwner->m_pSurface && m_pOwner->m_pOwner->m_pSurface->GetDevice()->IsLayeredWindow())
//	{
//		HDC hdc = m_pOwner->m_pOwner->m_pSurface->GetDC();
//		LRESULT lRes = ::CallWindowProcW(m_OldWndProc, m_hWnd, uMsg, wParam, lParam);
//		//lRes = ::CallWindowProcW(m_OldWndProc, m_hWnd, WM_PRINTCLIENT, (WPARAM)hdc, (LPARAM)(PRF_CHECKVISIBLE|PRF_NONCLIENT|PRF_CLIENT|PRF_ERASEBKGND|PRF_CHILDREN|PRF_OWNED));
//		lRes = ::CallWindowProcW(m_OldWndProc, m_hWnd, uMsg, (WPARAM)hdc, lParam);
//		//::SendMessage(m_pWin->m_hWnd, WM_PAINT, (WPARAM)hdc, 0);
//		//::SendMessage(m_pWin->m_hWnd, WM_PRINT, (WPARAM)hdc, (LPARAM)(PRF_CHECKVISIBLE|PRF_NONCLIENT|PRF_CLIENT|PRF_ERASEBKGND|PRF_CHILDREN|PRF_OWNED));
//		//::SendMessage(m_hWnd, WM_PRINTCLIENT, (WPARAM)hdc, (LPARAM)(/*PRF_CHECKVISIBLE|*/PRF_NONCLIENT|PRF_CLIENT|PRF_ERASEBKGND|PRF_CHILDREN|PRF_OWNED));
//
//		m_pOwner->m_pOwner->m_pSurface->ReleaseDC(hdc);
//		m_pOwner->m_pOwner->m_pSurface->Invalidate(TRUE);
//		return lRes;
//	}
//#endif
//#ifndef NO3D
//	if (m_pOwner->m_pOwner)
//	{
//		GdiLayer* surf = m_pOwner->m_pOwner->m_pSurface;
//		if (surf && surf->GetDevice()->IsLayeredWindow())
//		{
//			LRESULT lRes = ::CallWindowProc(m_OldWndProc, hWnd, uMsg, wParam, lParam);
//			//if (uMsg == WM_PAINT)
//			//	surf->Invalidate();
//			//else
//				if (uMsg == WM_SIZE)
//			{
//				CRect rc;
//				::GetWindowRect(m_hWnd, &rc);
//				POINT pt = rc.TopLeft();
//				::ScreenToClient(surf->GetDevice()->Handle(), &pt);
//				rc.MoveToXY(pt);
//				surf->Move(&rc);
//			}
//		}
//	}
//#endif
	return CDuiWindowBase::HandleMessage(uMsg, wParam, lParam);
}
//////////////////////////////////////////////////////////////////////////
//
CDuiWindowControl::CDuiWindowControl() : m_pWin(NULL), m_dwStyle(WS_CHILD), m_dwExStyle(0)
{

}

CDuiWindowControl::~CDuiWindowControl()
{

}

BOOL CDuiWindowControl::IsMyEvent( HANDLE, DuiEvent* pEvent )
{
	DuiEvent& event = *pEvent;
	DUI_EVENTTYPE e = DuiNode::NameToEventType(event.Name);
	//CDuiControlExtension::IsMyEvent(event);
	if (e == (DUIET_load))
	{
		UpdateState();
		if (m_pWin == NULL)
			m_pWin = OnCreateWindow();
		UpdatePos();
	}
	else if (e == (DUIET_unload))
	{
		if (m_pWin) m_pWin->Close();
	}
	else if (e == (DUIET_size))
	{
		UpdatePos();
	}
	return FALSE;
}

CDuiManagedWnd* CDuiWindowControl::OnCreateWindow()
{
	CDuiManagedWnd* pWnd = NEW CDuiManagedWnd;
	if (pWnd)
	{
		pWnd->Init(this);
		if (pWnd->GetHWND() == NULL)
		{
			pWnd = (delete pWnd, NULL);
		}
	}
	return pWnd;
}

void CDuiWindowControl::UpdatePos()
{
	CWindow win(m_pWin?m_pWin->GetHWND():NULL);
	if (win.IsWindow())
	{
		CRect rc2 = DuiNode::GetOffset(m_pOwner->GetOffsetParent()/*r->GetParent()*/, NULL, m_pOwner->GetPos(/*DUIRT_BORDER*/));
		////CRect rc2 = r->GetPos(DUIRT_BORDER);
		//if (IsLayered(r->m_pLayoutMgr->GetPaintWindow()))
		//{
		//	CLayerHelper::OffsetPointDec(r->m_pLayoutMgr->GetPaintWindow(), (LPPOINT)&rc2);
		//	CLayerHelper::OffsetPointDec(r->m_pLayoutMgr->GetPaintWindow(), ((LPPOINT)&rc2)+1);
		//}
		win.MoveWindow(&rc2);
	}
}

void CDuiWindowControl::UpdateState()
{
	m_dwStyle = ParseStyleFromVariant(m_pOwner->GetAttribute(L"window-style")) | WS_CHILD;
	m_dwExStyle = ParseStyleFromVariant(m_pOwner->GetAttribute(L"extend-window-style"));

	if (m_pOwner->IsVisible(TRUE)) m_dwStyle |= WS_VISIBLE;
	if (!m_pOwner->IsEnabled(TRUE)) m_dwStyle |= WS_DISABLED;
	if (m_pOwner->GetControlFlags() & DUIFLAG_TABSTOP) m_dwStyle |= WS_TABSTOP;

	CWindow win(m_pWin?m_pWin->GetHWND():NULL);
	if (win.IsWindow())
	{
		win.ModifyStyle(0xFFFFFFFF, m_dwStyle);
		win.ModifyStyleEx(0xFFFFFFFF, m_dwExStyle);
	}
}

void CDuiWindowControl::OnStateChanged(HANDLE, DWORD dwStateMask)
{
	UpdateState();
}

void CDuiWindowControl::OnPaintContent( HDE hde, HDC hdc, RECT rcPaint, BOOL bDraw, InDrawData* pidd )
{
	return;
	//if (!bDraw || m_pWin==NULL) __super::OnPaintContent(hDC, rcPaint, bDraw, idd);

#ifndef NO3D
	if (bDraw && m_pOwner->m_pSurface && m_pOwner->m_pSurface->GetDevice()->IsLayeredWindow())
	{
		::SetWindowOrgEx(hdc, 0, 0, NULL);
		//::SendMessage(m_pWin->m_hWnd, WM_PAINT, (WPARAM)hdc, 0);
		//::SendMessage(m_pWin->m_hWnd, WM_PRINT, (WPARAM)hdc, (LPARAM)(PRF_CHECKVISIBLE|PRF_NONCLIENT|PRF_CLIENT|PRF_ERASEBKGND|PRF_CHILDREN|PRF_OWNED));
		//::SendMessage(m_pWin->m_hWnd, WM_PRINTCLIENT, (WPARAM)hdc, (LPARAM)(PRF_CHECKVISIBLE|PRF_NONCLIENT|PRF_CLIENT|PRF_ERASEBKGND|PRF_CHILDREN|PRF_OWNED));

		//CClientDC dc(m_pWin->m_hWnd);
		HDC dc = ::GetDC(m_pWin->m_hWnd);
		CRect rc;
		::GetClientRect(m_pWin->m_hWnd, &rc);
		::BitBlt(hdc, 0, 0, rc.Width(), rc.Height(), dc, 0, 0, SRCCOPY);
		::ReleaseDC(m_pWin->m_hWnd, dc);
	}
#endif
}

void CDuiWindowControl::OnAttributeChanged( HANDLE, LPCOLESTR szAttr )
{
	if (lstrcmpiW(szAttr, L"window-style") == 0 ||
		lstrcmpiW(szAttr, L"extend-window-style") == 0)
		UpdateState();
}

void CDuiWindowControl::On3DSurfaceCreated(HANDLE)
{
#ifndef NO3D
	m_pOwner->m_pSurface->BindWindow(m_pWin->GetHWND(), &m_pOwner->_attributes);
#endif
}

//////////////////////////////////////////////////////////////////////////
// edit control

// edit window
class CDuiEditWnd : public CDuiManagedWnd
{
public:
	BOOL m_bInUpdation;
	CDuiEditWnd() : m_bInUpdation(FALSE) {}

	static const UINT uMsgKillFocus = WM_APP + 333;

	Final_Message()

	virtual void Init(CDuiWindowControl* pOwner)
	{
		if (pOwner==NULL) return;
		m_pOwner = pOwner;
		DuiNode* r = m_pOwner->m_pOwner;
		Create(r->m_pLayoutMgr->GetPaintWindow(),
			r->GetText(),
			m_pOwner->m_dwStyle/* | WS_CHILD | WS_BORDER*/,
			m_pOwner->m_dwExStyle,
			CRect(0,0,0,0));
		if (m_hWnd)
		{
			CWindow edit(m_hWnd);
			//edit.SetFont((HFONT)::GetStockObject(DEFAULT_GUI_FONT), TRUE);
			UpdateData(FALSE);
			edit.SendMessage(EM_SETMODIFY, (WPARAM)FALSE);
			edit.SendMessage(EM_SETMARGINS, EC_LEFTMARGIN | EC_RIGHTMARGIN, MAKELPARAM(2, 2));
			edit.SendMessage(EM_SETSEL, 0, -1);
			//edit.EnableWindow(rtOwner->IsEnabled());
			////edit.ShowWindow(pOwner->IsVisible() ? SW_SHOW : SW_HIDE);
			//edit.ShowWindow((int)rtOwner->IsVisible());
			////edit.SendMessage(EM_SETREADONLY, pOwner->IsReadOnly()==TRUE);
			edit.SetFocus();
		}
	}

	// CDuiWindowBase virtual function
	//virtual LPCTSTR GetWindowClassName() const { return L"DUI_" WC_EDITW; }
	virtual LPCTSTR GetSuperClassName() const { return WC_EDITW; }
	//virtual UINT GetClassStyle() const;
	//virtual void OnFinalMessage(HWND hWnd);
	virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		LRESULT lRes = 0;
		BOOL bHandled = TRUE;
		if (uMsg == WM_KILLFOCUS ) lRes = OnKillFocus(uMsg, wParam, lParam, bHandled);
		else if (uMsg == uMsgKillFocus ) lRes = OnInnerKillFocus(uMsg, wParam, lParam, bHandled);
		else if (uMsg == OCM_COMMAND && HIWORD(wParam) == EN_CHANGE ) lRes = OnEditChanged(uMsg, wParam, lParam, bHandled);
		else bHandled = FALSE;
		if( !bHandled ) return CDuiManagedWnd::HandleMessage(uMsg, wParam, lParam);
		return lRes;
	}

	virtual void UpdateData(BOOL bSave=TRUE)
	{
		if (m_pOwner == NULL) return;
		if (m_bInUpdation) return;

		DuiNode* r = m_pOwner->m_pOwner;
		m_bInUpdation = TRUE;
		if (bSave)
		{
			int cchLen = ::GetWindowTextLengthW(m_hWnd) + 1;
			LPOLESTR pstr = NEW OLECHAR[cchLen];
			if( pstr == NULL ) return;
			::GetWindowTextW(m_hWnd, pstr, cchLen);
			r->SetText(pstr);
			delete [] pstr;
		}
		else // load
		{
			::SetWindowTextW(*this, r->GetText());
		}
		m_bInUpdation = FALSE;
	}

	LRESULT OnInnerKillFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		Close();
		UpdateData(TRUE);
		return 0;
	}

	LRESULT OnKillFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE; // 基类还要处理这个消息
		//LRESULT lRes = ::DefWindowProc(m_hWnd, uMsg, wParam, lParam);
		//OnInnerKillFocus(uMsg, wParam, lParam, bHandled);
		////PostMessage(WM_CLOSE);
		//Close();
		//UpdateData(TRUE);
		m_pOwner->m_pOwner->m_pLayoutMgr->SetFocus(NULL);
		return 0;
	}

	LRESULT OnEditChanged(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		UpdateData(TRUE);
		return 0;
	}
};

//////////////////////////////////////////////////////////////////////////
//
CDuiManagedWnd* CDuiEdit::OnCreateWindow()
{
	CDuiEditWnd* pWnd = NEW CDuiEditWnd;
	if (pWnd)
	{
		pWnd->Init(this);
		if (pWnd->GetHWND() == NULL) return (delete pWnd, NULL);
	}
	return pWnd;
}

BOOL CDuiEdit::IsMyEvent( HANDLE h, DuiEvent* pEvent )
{
	DuiEvent& event = *pEvent;
	DUI_EVENTTYPE e = DuiNode::NameToEventType(event.Name);
	if (e == (DUIET_load))
		return m_pOwner->OnEvent(event), TRUE; // 越过 CDuiWindowControl 的默认处理（创建窗口）
	else if (e == (DUIET_setfocus))
	{
		//LineLog dbg(L"Edit setfocus: oldWin:0x%08x", m_pWin);
		CDuiWindowControl::IsMyEvent(h,pEvent);
		UpdateState();
		if (m_pWin == NULL)
			m_pWin = OnCreateWindow();
		UpdatePos();
		//Invalidate();
		//dbg(L" newWin:0x%08x", m_pWin);
		return TRUE;
	}
	else if (e == (DUIET_killfocus))
	{
		if (m_pWin)
		{
			//LineLog dbg(L"Killfocus");
			::SendMessage(*m_pWin, CDuiEditWnd::uMsgKillFocus, 0, 0);
		}
	}
	return CDuiWindowControl::IsMyEvent(h, pEvent);
}

void CDuiEdit::OnAttributeChanged(HANDLE h, LPCOLESTR szAttr)
{
	/////__super::OnAttributeChanged(h, szAttr);
	if (lstrcmpiW(szAttr, L"Text")==0 && m_pWin && m_pWin->GetHWND())
		m_pWin->UpdateData(FALSE);
	//else if (lstrcmpiW(szAttr, L"window-style") == 0)
	//	m_dwStyle |= WS_BORDER;
}

BOOL CDuiEdit::ParseWindowStyle(LPCOLESTR lpszData, DWORD& style)
{
#define MAP_ITEM(x) {OLESTR(#x), lstrlenW(L#x), ES_##x},
	static const struct {LPCOLESTR name; int len_of_name; DWORD style;} __mapitem_entry[] = {
		MAP_ITEM(LEFT)			MAP_ITEM(CENTER) 		MAP_ITEM(RIGHT) 		MAP_ITEM(MULTILINE) 
		MAP_ITEM(UPPERCASE) 	MAP_ITEM(LOWERCASE) 	MAP_ITEM(PASSWORD)		MAP_ITEM(AUTOVSCROLL) 
		MAP_ITEM(AUTOHSCROLL) 	MAP_ITEM(NOHIDESEL) 	MAP_ITEM(OEMCONVERT) 	MAP_ITEM(READONLY) 
		MAP_ITEM(WANTRETURN) 	MAP_ITEM(NUMBER) 
	};
#undef MAP_ITEM

	int len = lstrlenW(lpszData);
	for (int i=0; i<sizeof(__mapitem_entry)/sizeof(__mapitem_entry[0]); i++)
	{
		if (__mapitem_entry[i].len_of_name==len && lstrcmpiW(lpszData,__mapitem_entry[i].name)==0)
			return style|=__mapitem_entry[i].style, TRUE;
	}

	return FALSE;
}