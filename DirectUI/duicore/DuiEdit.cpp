#include "StdAfx.h"
#include "DuiEdit.h"
#include "DuiStyleParser.h"
#include "../../common/theme.h"
#include <tom.h>

#pragma comment(lib, "imm32.lib")
#pragma comment(lib, "riched20.lib")

CDuiEditable::CDuiEditable( ) // : m_pOwner(owner), /*m_dwProps(0),*/ m_bTimer(FALSE), m_bActive(FALSE)
{
	m_dwProps = m_dwStyles = -1;
}

CDuiEditable::~CDuiEditable()
{

}

void CDuiEditable::UpdateCharFormat()
{
	LOGFONTW lf;
	if (!m_pLayoutMgr->BuildControlFont(m_pOwner, &lf))
	{
		HFONT hfont = (HFONT)::GetStockObject(DEFAULT_GUI_FONT);
		::GetObjectW(hfont, sizeof(LOGFONTW), &lf);
	}
	m_fontHeight = lf.lfHeight;

	memset(&m_cf, 0, sizeof(CHARFORMAT2W));
	m_cf.cbSize = sizeof(CHARFORMAT2W);
	HDC hdc = ::GetDC(NULL/*m_hWnd*/);
	LONG yPixPerInch = GetDeviceCaps(hdc, LOGPIXELSY);
	m_cf.yHeight = lf.lfHeight * 1440 / yPixPerInch;
	::ReleaseDC(NULL/*m_hWnd*/, hdc);

	m_cf.yOffset = 0;
	//m_cf.crTextColor = RGB(255,0,0); // dwEffects包含了CFE_AUTOBACKCOLOR和CFE_AUTOCOLOR，表示颜色从TxGetSysColor中获取，参数分别是COLOR_WINDOW（背景）和COLOR_WINDOWTEXT（字体）

	m_cf.dwEffects = CFM_EFFECTS | CFE_AUTOBACKCOLOR | CFE_AUTOCOLOR;
	m_cf.dwEffects &= ~(CFE_PROTECTED | CFE_LINK);

	if(lf.lfWeight < FW_BOLD)
		m_cf.dwEffects &= ~CFE_BOLD;
	if(!lf.lfItalic)
		m_cf.dwEffects &= ~CFE_ITALIC;
	if(!lf.lfUnderline)
		m_cf.dwEffects &= ~CFE_UNDERLINE;
	if(!lf.lfStrikeOut)
		m_cf.dwEffects &= ~CFE_STRIKEOUT;

	m_cf.dwMask = CFM_ALL2/* | CFM_BACKCOLOR*/;
	//m_cf.dwMask = CFM_BOLD
	//			|CFM_CHARSET
	//			|CFM_COLOR
	//			|CFM_FACE
	//			|CFM_ITALIC
	//			|CFM_OFFSET
	//			|CFM_PROTECTED
	//			|CFM_SIZE
	//			|CFM_STRIKEOUT
	//			|CFM_UNDERLINE
	//			;
	m_cf.bCharSet = lf.lfCharSet;
	m_cf.bPitchAndFamily = lf.lfPitchAndFamily;
	lstrcpyW(m_cf.szFaceName, lf.lfFaceName);

	if (m_ts)
	{
		HRESULT hr = m_ts->OnTxPropertyBitsChange(TXTBIT_CHARFORMATCHANGE, TXTBIT_CHARFORMATCHANGE);

	//	//CComQIPtr<ITextDocument> td(m_ts);
	//	//if (td.p)
	//	//{
	//	//	CComPtr<ITextRange> range;
	//	//	hr = td.p->Range(0,0, &range);
	//	//	if (SUCCEEDED(hr))
	//	//	{
	//	//		hr = range->Move/*End*/(tomStory, 1, NULL);
	//	//		CComPtr<ITextFont> font;
	//	//		hr = range->GetFont(&font);
	//	//		COLORREF clr = 0;
	//	//		hr = font->GetForeColor((long*)&clr);
	//	//		hr = font->SetForeColor((long)m_cf.crTextColor);
	//	//		hr = font->GetForeColor((long*)&clr);
	//	//		hr = range->SetFont(font);
	//	//	}
	//	//}
	}
}

void CDuiEditable::UpdateParaFormat()
{
	memset(&m_pf, 0, sizeof(PARAFORMAT2));
	m_pf.cbSize = sizeof(PARAFORMAT2);
	m_pf.dwMask = PFM_ALL2;
	m_pf.wAlignment = PFA_LEFT;
	m_pf.dyLineSpacing = 20 * m_fontHeight;
	m_pf.bLineSpacingRule = 4;
	m_pf.cTabCount = 1;
	m_pf.rgxTabs[0] = lDefaultTab;

	if (m_ts)
		m_ts->OnTxPropertyBitsChange(TXTBIT_PARAFORMATCHANGE, TXTBIT_PARAFORMATCHANGE);
}

void CDuiEditable::Init()
{
	if (m_pLayoutMgr==NULL)
	{
		m_pLayoutMgr = (m_pOwner)->m_pLayoutMgr;
		m_hWnd = m_pLayoutMgr->GetPaintWindow();
	}
}

BOOL CDuiEditable::ParseTextBitString(LPCOLESTR lpszData, DWORD& bit)
{
#define MAP_ITEM(x) {OLESTR(#x), lstrlenW(L#x), TXTBIT_##x},
	static const struct {LPCOLESTR name; int len_of_name; DWORD bit;} __mapitem_entry[] = {
		MAP_ITEM(ALLOWBEEP)			MAP_ITEM(AUTOWORDSEL)	MAP_ITEM(DISABLEDRAG)	MAP_ITEM(HIDESELECTION) 
		MAP_ITEM(MULTILINE) 		MAP_ITEM(READONLY) 		MAP_ITEM(RICHTEXT)		MAP_ITEM(SAVESELECTION) 
		MAP_ITEM(SHOWACCELERATOR) 	MAP_ITEM(USEPASSWORD) 	MAP_ITEM(WORDWRAP) 
	};
#undef MAP_ITEM

	int len = lstrlenW(lpszData);
	for (int i=0; i<sizeof(__mapitem_entry)/sizeof(__mapitem_entry[0]); i++)
	{
		if (__mapitem_entry[i].len_of_name==len && lstrcmpiW(lpszData,__mapitem_entry[i].name)==0)
			return bit|=__mapitem_entry[i].bit, TRUE;
	}

	return FALSE;
}

BOOL CDuiEditable::ParseEditStyleString(LPCOLESTR lpszData, DWORD& style)
{
#define MAP_ITEM(x) {OLESTR(#x), lstrlenW(L#x), SES_##x},
	static const struct {LPCOLESTR name; int len_of_name; DWORD style;} __mapitem_entry[] = {
		MAP_ITEM(EMULATESYSEDIT)			MAP_ITEM(BEEPONMAXTEXT)	MAP_ITEM(EXTENDBACKCOLOR)	/*MAP_ITEM(NOXLTSYMBOLRANGE)*/ 
		MAP_ITEM(USEAIMM) 		MAP_ITEM(UPPERCASE) 		MAP_ITEM(LOWERCASE)		MAP_ITEM(NOINPUTSEQUENCECHK) 
		MAP_ITEM(XLTCRCRLFTOCR) 	MAP_ITEM(NOIME) 	MAP_ITEM(BIDI)		MAP_ITEM(SCROLLONKILLFOCUS)
		MAP_ITEM(DRAFTMODE)		MAP_ITEM(USECTF)		MAP_ITEM(HIDEGRIDLINES)		MAP_ITEM(USEATFONT)
		MAP_ITEM(CTFALLOWEMBED)		MAP_ITEM(CTFALLOWSMARTTAG)	MAP_ITEM(CTFALLOWPROOFING)
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

DWORD CDuiEditable::ParseTextBit(BOOL bUpdate/*=FALSE*/)
{
	if (m_dwProps == -1)
	{
		m_dwProps = 0;
		CStrArray strs;
		if (!SplitStringToArray((m_pOwner)->GetAttributeString(L"edit-style"), strs)) return 0;

		for (int i=0; i<strs.GetSize(); i++)
			ParseTextBitString(strs[i], m_dwProps);
	}

	if (m_ts && bUpdate)
	{
		m_ts->OnTxPropertyBitsChange(-1, m_dwProps);
	}
	return m_dwProps;
}

DWORD CDuiEditable::ParseEditStyle()
{
	if (m_dwStyles == -1)
	{
		m_dwStyles = 0;
		CStrArray strs;
		if (!SplitStringToArray((m_pOwner)->GetAttributeString(L"edit-style"), strs)) return 0;

		for (int i=0; i<strs.GetSize(); i++)
			ParseEditStyleString(strs[i], m_dwStyles);
	}

	if (m_ts)
	{
		SetEditStyle(m_dwStyles);
	}
	return m_dwStyles;
}


HDC CDuiEditable::TxGetDC()
{
	//if (rt(m_pOwner)->m_pSurface)
	//	return rt(m_pOwner)->m_pSurface->GetDC();
	return ::GetDC(NULL/*m_hWnd*/);
}

INT CDuiEditable::TxReleaseDC( HDC hdc )
{
	//if (rt(m_pOwner)->m_pSurface)
	//	return rt(m_pOwner)->m_pSurface->ReleaseDC(hdc), 1;
	return ::ReleaseDC(NULL/*m_hWnd*/, hdc);
}

BOOL CDuiEditable::TxShowScrollBar( INT fnBar, BOOL fShow )
{
	if (fnBar==SB_BOTH || fnBar==SB_VERT) m_pOwner->GetScrollbar()->ShowScrollbar(fShow);
	if (fnBar==SB_BOTH || fnBar==SB_HORZ) m_pOwner->GetScrollbar(FALSE)->ShowScrollbar(fShow);
	return TRUE;
}

BOOL CDuiEditable::TxEnableScrollBar( INT fuSBFlags, INT fuArrowflags )
{
	if (fuSBFlags==SB_BOTH || fuSBFlags==SB_VERT) m_pOwner->GetScrollbar()->EnableScrollbar(fuArrowflags!=ESB_DISABLE_BOTH);
	if (fuSBFlags==SB_BOTH || fuSBFlags==SB_HORZ) m_pOwner->GetScrollbar(FALSE)->EnableScrollbar(fuArrowflags!=ESB_DISABLE_BOTH);
	return TRUE;
}

BOOL CDuiEditable::TxSetScrollRange( INT fnBar, LONG nMinPos, INT nMaxPos, BOOL fRedraw )
{
	m_pOwner->GetScrollbar(fnBar!=SB_HORZ)->SetScrollRange(nMaxPos, fRedraw);
	return TRUE;
}

BOOL CDuiEditable::TxSetScrollPos( INT fnBar, INT nPos, BOOL fRedraw )
{
	// 必须阻止重入，因为滚动条位置变化可能来源于EDIT内部，也可能来源于用户操作滚动条
	// 如果不保护，会跟CDuiRichEdit::OnScroll形成无限嵌套
	if (!m_bScrollPending)
	{
		m_bScrollPending = TRUE;
		m_pOwner->GetScrollbar(fnBar!=SB_HORZ)->SetScrollPos(nPos, fRedraw);
		m_bScrollPending = FALSE;
	}
	return TRUE;
}

void CDuiEditable::TxInvalidateRect( LPCRECT prc, BOOL fMode )
{
	(m_pOwner)->Invalidate();
	//::InvalidateRect(m_hWnd, prc, fMode);
}

void CDuiEditable::TxViewChange( BOOL fUpdate )
{
	(m_pOwner)->Invalidate();
	// ::UpdateWindow();
}

BOOL CDuiEditable::TxCreateCaret( HBITMAP hbmp, INT xWidth, INT yHeight )
{
	BOOL b = ::CreateCaret(m_hWnd, hbmp, xWidth, yHeight);
#ifndef NO3D
	if (m_pLayoutMgr->Get3DDevice())
		b = m_pLayoutMgr->Get3DDevice()->CreateCaret(hbmp, xWidth, yHeight, (m_pOwner)->m_pSurface);
#endif
	return b;
}

BOOL CDuiEditable::TxShowCaret( BOOL fShow )
{
	m_bCaretShow = fShow;
#ifndef NO3D
	if (m_pLayoutMgr->Get3DDevice())
		return m_pLayoutMgr->Get3DDevice()->ShowCaret(fShow);
#endif
	BOOL b;
	if(fShow)
		b = ::ShowCaret(m_hWnd);
	else
		b = ::HideCaret(m_hWnd);

	//rt(m_pOwner)->Invalidate();
	return b;
}

BOOL CDuiEditable::TxSetCaretPos( INT x, INT y )
{
	BOOL b = ::SetCaretPos(x, y);
#ifndef NO3D
	if (m_pLayoutMgr->Get3DDevice())
		b = m_pLayoutMgr->Get3DDevice()->SetCaretPos(x,y);
	else
#endif
		(m_pOwner)->Invalidate();
	return b;
}

BOOL CDuiEditable::TxSetTimer( UINT idTimer, UINT uTimeout )
{
	m_bTimer = TRUE;
	return ::SetTimer(m_hWnd, idTimer, uTimeout, NULL);
}

void CDuiEditable::TxKillTimer( UINT idTimer )
{
	::KillTimer(m_hWnd, idTimer);
	m_bTimer = FALSE;
}

void CDuiEditable::TxScrollWindowEx( INT dx, INT dy, LPCRECT lprcScroll, LPCRECT lprcClip, HRGN hrgnUpdate, LPRECT lprcUpdate, UINT fuScroll )
{
	//::ScrollWindowEx(m_hWnd, dx, dy, lprcScroll, lprcClip, hrgnUpdate, lprcUpdate, fuScroll);
}

void CDuiEditable::TxSetCapture( BOOL fCapture )
{
	m_pLayoutMgr->SetCapture(fCapture ? m_pOwner : NULL);
}

void CDuiEditable::TxSetFocus()
{
	m_pLayoutMgr->SetFocus(m_pOwner);
}

void CDuiEditable::TxSetCursor( HCURSOR hcur, BOOL fText )
{
	::SetCursor(hcur);
}

BOOL CDuiEditable::TxScreenToClient( LPPOINT lppt )
{
	return ::ScreenToClient(m_hWnd, lppt);	
}

BOOL CDuiEditable::TxClientToScreen( LPPOINT lppt )
{
	return ::ClientToScreen(m_hWnd, lppt);
}

HRESULT CDuiEditable::TxActivate( LONG * plOldState )
{
	*plOldState = (LONG)m_bActive;
	m_bActive = TRUE;
	return /**plOldState=0,*/ S_OK;
}

HRESULT CDuiEditable::TxDeactivate( LONG lNewState )
{
	m_bActive = FALSE;
	return S_OK;
}

HRESULT CDuiEditable::TxGetClientRect( LPRECT prc )
{
	if (!m_bActive) return E_FAIL;
	return *prc = m_pOwner->GetClientRect(), S_OK;
}

HRESULT CDuiEditable::TxGetViewInset( LPRECT prc )
{
	return memset(prc, 0, sizeof(RECT)), S_OK;
}

HRESULT CDuiEditable::TxGetCharFormat( const CHARFORMATW **ppCF )
{
	return *ppCF=&m_cf, S_OK;
}

HRESULT CDuiEditable::TxGetParaFormat( const PARAFORMAT **ppPF )
{
	return *ppPF=&m_pf, S_OK;
}

COLORREF CDuiEditable::TxGetSysColor( int nIndex )
{
	if (nIndex == COLOR_WINDOW)
	{
		DuiStyleVariant* sv = m_pOwner->GetStyleAttribute(SAT_BACKGROUND);
		if (sv->svt == SVT_COLOR)
			return sv->colorVal;
	}
	else if (nIndex == COLOR_WINDOWTEXT)
	{
		DuiStyleVariant* sv = m_pOwner->GetStyleAttribute(SAT_COLOR);
		if (sv->svt == SVT_COLOR)
		{
			return FromStyleVariant(sv, COLORREF);
		}
	}

	return ::GetSysColor(nIndex);
}

HRESULT CDuiEditable::TxGetBackStyle( TXTBACKSTYLE *pstyle )
{
	//*pstyle = TXTBACK_OPAQUE;
	DuiStyleVariant* sv = m_pOwner->GetStyleAttribute(SAT_BACKGROUND);
	*pstyle = /*(sv->svt == SVT_COLOR) ? TXTBACK_OPAQUE :*/ TXTBACK_TRANSPARENT;
	if (sv->svt == SVT_COLOR)
	{
		COLORREF clr = FromStyleVariant(sv, COLORREF);
		BYTE a = (clr>>24);
		if (a==0)
		{
			a = 255;
			CComVariant v = m_pOwner->GetAttribute(L"background-alpha");
			if (v.vt!=VT_EMPTY && SUCCEEDED(v.ChangeType(VT_UI1)))
				a = (BYTE)V_UI1(&v);
		}
		if (a==255)
			*pstyle = TXTBACK_OPAQUE;
	}
	return S_OK;
}

HRESULT CDuiEditable::TxGetMaxLength( DWORD *plength )
{
	*plength = (DWORD)m_pOwner->_attributes.GetAttributeLong(L"max-length", INFINITE);
	return S_OK;
}

HRESULT CDuiEditable::TxGetScrollBars( DWORD *pdwScrollBar )
{
	*pdwScrollBar = (WS_VSCROLL | WS_HSCROLL | ES_AUTOVSCROLL | ES_AUTOHSCROLL /*| ES_DISABLENOSCROLL*/);
	return S_OK;
}

HRESULT CDuiEditable::TxGetPasswordChar( TCHAR *pch )
{
	LPCOLESTR p = (m_pOwner)->GetAttributeString(L"password-char");
	if (p) *pch = *p;
	return *pch == 0 ? S_FALSE : S_OK;
}

HRESULT CDuiEditable::TxGetAcceleratorPos( LONG *pcp )
{
	return *pcp=-1, S_OK;
}

HRESULT CDuiEditable::TxGetExtent( LPSIZEL lpExtent )
{
	//return E_NOTIMPL;
	CRect rc = (m_pOwner)->GetPos();
	SIZEL pxSize = { rc.Width(), rc.Height() };
	AtlPixelToHiMetric(&pxSize, lpExtent);
	return S_OK;
}

HRESULT CDuiEditable::OnTxCharFormatChange( const CHARFORMATW * pcf )
{
	//memcpy_s(&m_cf, sizeof(CHARFORMAT2W), pcf, sizeof(CHARFORMAT2W));
	return S_OK;
}

HRESULT CDuiEditable::OnTxParaFormatChange( const PARAFORMAT * ppf )
{
	//memcpy_s(&m_pf, sizeof(PARAFORMAT), ppf, sizeof(PARAFORMAT));
	return S_OK;
}

HRESULT CDuiEditable::TxGetPropertyBits( DWORD dwMask, DWORD *pdwBits )
{
	DWORD props = ParseTextBit();
	*pdwBits = props & dwMask;
	return S_OK;
}

HRESULT CDuiEditable::TxNotify( DWORD iNotify, void *pv )
{
#define __foreach_edit_notify(V) \
	V(DROPFILES) \
	V(LINK) \
	V(OLEOPFAILED) \
	V(PROTECTED) \
	/*V(REQUESTRESIZE)*/ \
	V(SAVECLIPBOARD) \
	V(SELCHANGE) \
	V(STOPNOUNDO) \
	/*V(UPDATE)*/ \
	V(CHANGE) \
	V(ERRSPACE) \
	V(HSCROLL) \
	/*V(KILLFOCUS)*/ \
	V(MAXTEXT) \
	/*V(SETFOCUS)*/ \
	V(VSCROLL)

	DuiEvent evt = {NULL};
	evt.lParam = (LPARAM)pv;
	evt.cancelBubble = TRUE;
	HRESULT hr = S_FALSE;

	switch (iNotify)
	{
#define __on_notify(x) case EN_##x: evt.Name=L"on" L#x; /*OutputDebugStringW(L"on" L#x L"\n");*/ m_pLayoutMgr->FireEvent(m_pOwner, evt); if (evt.returnValue) hr=S_OK; break;
		__foreach_edit_notify(__on_notify)
#undef __on_notify
	}

	//switch (iNotify)
	//{
	//case EN_UPDATE:
	//case EN_CHANGE:
	//case EN_SELCHANGE:
	//case EN_HSCROLL:
	//case EN_VSCROLL:
	//	rt(m_pOwner)->Invalidate();
	//}
// 	if (iNotify==EN_REQUESTRESIZE)
// 	{
// 		REQRESIZE* resize = (REQRESIZE*)pv;
// 		ATLASSERT(resize);
// 		m_szCached.cx = resize->rc.right-resize->rc.left;
// 		m_szCached.cy = resize->rc.bottom-resize->rc.top;
// 		CDuiControlExtension::SetScrollRange(m_pOwner, m_szCached.cx, m_szCached.cy);
// 		//ShowScrollBar(SB_HORZ, m_pOwner->GetScrollbar(FALSE)->IsValid());
// 		//ShowScrollBar(SB_VERT, m_pOwner->GetScrollbar(TRUE)->IsValid());
// 		rt(m_pOwner)->Invalidate();
// 	}
	//HWND hwnd = This->hWnd;
	//HWND parent = GetParent(hwnd);
	//UINT id = GetWindowLongW(hwnd, GWLP_ID);
	///* Note: EN_MSGFILTER is documented as not being sent to TxNotify */

	//switch (iNotify)
	//{
	//case EN_DROPFILES:
	//case EN_LINK:
	//case EN_OLEOPFAILED:
	//case EN_PROTECTED:
	//case EN_REQUESTRESIZE:
	//case EN_SAVECLIPBOARD:
	//case EN_SELCHANGE:
	//case EN_STOPNOUNDO:
	//	{
	//		/* FIXME: Verify this assumption that pv starts with NMHDR. */
	//		NMHDR *info = pv;
	//		if (!info)
	//			return E_FAIL;

	//		info->hwndFrom = hwnd;
	//		info->idFrom = id;
	//		info->code = iNotify;
	//		SendMessageW(parent, WM_NOTIFY, id, (LPARAM)info);
	//		break;
	//	}

	//case EN_UPDATE:
	//	/* Only sent when the window is visible. */
	//	if (!IsWindowVisible(This->hWnd))
	//		break;
	//	/* Fall through */
	//case EN_CHANGE:
	//case EN_ERRSPACE:
	//case EN_HSCROLL:
	//case EN_KILLFOCUS:
	//case EN_MAXTEXT:
	//case EN_SETFOCUS:
	//case EN_VSCROLL:
	//	SendMessageW(parent, WM_COMMAND, MAKEWPARAM(id, iNotify), (LPARAM)hwnd);
	//	break;

	//default:
	//	return E_FAIL;
	//}

	return hr;
}

HIMC CDuiEditable::TxImmGetContext()
{
	return ::ImmGetContext(m_hWnd);
}

void CDuiEditable::TxImmReleaseContext( HIMC himc )
{
	::ImmReleaseContext(m_hWnd, himc);
}

HRESULT CDuiEditable::TxGetSelectionBarWidth( LONG *lSelBarWidth )
{
	return *lSelBarWidth=212, S_OK; /* in HIMETRIC */ // ??????????
}

void CDuiEditable::AutoActivate()
{
	if (!m_bActive)
	{
		HRESULT hr = m_ts->OnTxInPlaceActivate(NULL);
		m_bActive = TRUE;

		// set IME
		DWORD dw = TxSendMessage(EM_GETLANGOPTIONS);
		dw |= IMF_AUTOKEYBOARD | IMF_DUALFONT;
		dw &= ~IMF_AUTOFONT;
		TxSendMessage(EM_SETLANGOPTIONS, 0, dw);

		dw = /*GetEventMask();*//*ENM_CHANGE*/ 0x0FFF007F;
		//dw &= ~(ENM_SCROLLEVENTS | ENM_SCROLL);
		dw &= ~(ENM_REQUESTRESIZE); // 如果设置了 ENM_REQUESTRESIZE，将收不到 TxShowScrollbar 调用
		SetEventMask(dw);

		//TxSendMessage(EM_SETCHARFORMAT, SCF_ALL, (LPARAM)&m_cf);
		//TxSendMessage(EM_SETPARAFORMAT, 0, (LPARAM)&m_pf);

		ParseTextBit(TRUE);
		ParseEditStyle();

		// 设置文本 TxSetText
		LPCOLESTR text = m_pOwner->GetText();
		if (text)
			SetTextEx(text);
		//hr = m_ts->TxSetText(text); // 避免使用这个用法

		//if (m_ts)
		//{
		//	hr = m_ts->OnTxPropertyBitsChange(TXTBIT_CHARFORMATCHANGE, TXTBIT_CHARFORMATCHANGE);
		//	hr = m_ts->OnTxPropertyBitsChange(TXTBIT_PARAFORMATCHANGE, TXTBIT_PARAFORMATCHANGE);
		//}
	}
}

//EXTERN_C const IID IID_ITextServices = { // 8d33f740-cf58-11ce-a89d-00aa006cadc5
//	0x8d33f740,
//	0xcf58,
//	0x11ce,
//	{0xa8, 0x9d, 0x00, 0xaa, 0x00, 0x6c, 0xad, 0xc5}
//};
//
//EXTERN_C const IID IID_ITextHost = { /* c5bdd8d0-d26e-11ce-a89e-00aa006cadc5 */
//	0xc5bdd8d0,
//	0xd26e,
//	0x11ce,
//	{0xa8, 0x9e, 0x00, 0xaa, 0x00, 0x6c, 0xad, 0xc5}
//};

HRESULT CDuiEditable::CreateEdit()
{
	// 两个调用顺序不要反，因为ParaFormat依赖CharFormat中的字体高度信息
	UpdateCharFormat();
	UpdateParaFormat();

	HRESULT hr = Create(this);

	if (SUCCEEDED(hr))
	{
		//hr = m_ts->OnTxInPlaceActivate(NULL);
		//m_bActive = TRUE;

		//// set IME
		//DWORD dw = TxSendMessage(EM_GETLANGOPTIONS);
		//dw |= IMF_AUTOKEYBOARD | IMF_DUALFONT;
		//dw &= ~IMF_AUTOFONT;
		//TxSendMessage(EM_SETLANGOPTIONS, 0, dw);

		//dw = /*GetEventMask();*//*ENM_CHANGE*/ 0x0FFF007F;
		////dw &= ~(ENM_SCROLLEVENTS);
		////SetEventMask(dw);

		//ParseTextBit(TRUE);
		//ParseEditStyle();

		//// 设置文本 TxSetText
		//LPCOLESTR text = r->GetText();
		//if (text)
		//	SetTextEx(text);
		//	//hr = m_ts->TxSetText(text); // 避免使用这个用法

		CComQIPtr<IPeerHolder2> peer(m_pOwner->GetObject(TRUE));
		if (peer.p)
		{
			peer->AttachObject2(GetDispatch(), __ClassName());

			// TOM 扩展对象必须最后附加，因为针对 ITextDocument 调用GetIDsOfNames时，任何不合法的名称都返回S_OK，而且DISPID为0，导致之后附加的扩展对象完全失效
			CComDispatchDriver tom(m_ts);
			if (tom.p)
				peer->AttachObject(tom.p);
		}

		AutoActivate();

	}

	return hr;
}

void CDuiEditable::ReleaseEdit()
{
	CComQIPtr<IPeerHolder> peer(m_pOwner->GetObject(TRUE));
	if (peer.p)
	{
		CComDispatchDriver tom(m_ts);
		if (tom.p)
			peer->DetachObject(tom.p);

		peer->DetachObject(GetDispatch());
	}

	if (m_ts == NULL) return;

	if (m_bActive)
	{
		m_ts->OnTxInPlaceDeactivate();
		m_bActive = FALSE;
	}
	m_ts->Release();
	m_ts = NULL;
}

BOOL CDuiEditable::OnMessage( UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& lResult )
{
	if (m_ts==NULL)
		return FALSE;

	// 如果需要则就地激活
	if (!m_bActive)
	{
		return FALSE;
		//m_ts->OnTxInPlaceActivate(NULL);
		//m_bActive = TRUE;
	}

	HRESULT hr = m_ts->TxSendMessage(uMsg, wParam, lParam, &lResult);
	return (hr==S_OK /*|| hr==S_MSG_KEYIGNORED*/);
}

//////////////////////////////////////////////////////////////////////////
//

CDuiRichEdit::CDuiRichEdit()
{

}

CDuiRichEdit::~CDuiRichEdit()
{

}

void CDuiRichEdit::OnConnect()
{
	Init();
}

void CDuiRichEdit::SetAttribute( HDE hde, LPCWSTR szName, VARIANT* value )
{
	__super::SetAttribute(hde, szName, value);
	if (lstrcmpiW(szName, L"edit-style") == 0)
	{
		m_dwProps = m_dwStyles = -1;
		ParseTextBit(TRUE);
		ParseEditStyle();
	}
}

BOOL CDuiRichEdit::IsMyEvent( HANDLE, DuiEvent* pEvent )
{
	DuiEvent& event = *pEvent;
	DUI_EVENTTYPE e = DuiNode::NameToEventType(event.Name);
	LRESULT lRes = 0;

	HRESULT hr = S_OK;
	if (e == (DUIET_load))
	{
		hr = CreateEdit();
		if (SUCCEEDED(hr))
		{
			//// now attach richedit object to control peer.
			//CComQIPtr<IPeerHolder> peer(r->GetObject(TRUE));
			//if (peer.p)
			//{
			//	peer->AttachObject(GetDispatch());
			//}
		}
	}
	else if (e == (DUIET_unload))
	{
		ReleaseEdit();
	}
	else
	{
		//if (!m_bActive)
		//{
		//	RECT rc = CDuiControlExtension::GetClientRect(m_pOwner);
		//	hr = m_ts->OnTxInPlaceActivate(NULL/*&rc*/);
		//	m_bActive = TRUE;
		//}

		if (e == (DUIET_setfocus) && m_ts)
		{
			m_bFocus = TRUE;
			hr = m_ts->OnTxUIActivate();
			hr = m_ts->TxSendMessage(WM_SETFOCUS, NULL, 0, &lRes);
		}
		else if (e == (DUIET_killfocus) && m_ts)
		{
			m_bFocus = FALSE;
			hr = m_ts->OnTxUIDeactivate();
			hr = m_ts->TxSendMessage(WM_KILLFOCUS, NULL, 0, &lRes);
		}
		else if (e == (DUIET_size) && m_ts)
		{
			//AutoActivate();
			hr = m_ts->OnTxPropertyBitsChange(TXTBIT_EXTENTCHANGE|TXTBIT_CLIENTRECTCHANGE, TXTBIT_EXTENTCHANGE|TXTBIT_CLIENTRECTCHANGE);
		}
	}
	return FALSE;
}

void CDuiRichEdit::OnScroll( HANDLE, HDE pScroll, BOOL bVert/*=TRUE*/ )
{
	if (m_ts && !m_bScrollPending)
	{
		// 如果是某个父级控件的滚动消息，则EDIT的位置会发生变化，应该发生通知
		// 
		if (pScroll != m_pOwner->Handle())
			m_ts->OnTxPropertyBitsChange(TXTBIT_CLIENTRECTCHANGE, TXTBIT_CLIENTRECTCHANGE);

		//m_bScrollPending = TRUE;
		POINT pt = {0};
		GetScrollPos(&pt);
		if (bVert) pt.y = m_pOwner->GetScrollbar(bVert)->GetScrollPos();
		else pt.x = m_pOwner->GetScrollbar(bVert)->GetScrollPos();
		SetScrollPos(&pt);
		//m_ts->OnTxPropertyBitsChange(TXTBIT_SCROLLBARCHANGE, TXTBIT_SCROLLBARCHANGE);
		//m_bScrollPending = FALSE;
	}
}

//void CDuiRichEdit::OnGetContentSize(SIZE& sz)
//{
//	sz = m_szCached;
//}

void CDuiRichEdit::OnPaintContent( HDE hde, HDC hdc, RECT rcPaint, BOOL bDraw, InDrawData* pidd )
{
	//if (bDraw && !m_bActive && m_ts)
	//{
	//	m_ts->OnTxInPlaceActivate(NULL);
	//	m_bActive = TRUE;
	//}

	InDrawData& idd = *pidd;

	if (bDraw && m_ts)
	{
		RECT rc = m_pOwner->GetClientRect();
		RECT rcClip = m_pOwner->GetClipRect();
		if (!::IsRectEmpty(&rcClip))
		{
			CRenderContentOrg org(hdc, NULL, m_pOwner);
			//POINT pt1;
			//::SetWindowOrgEx(hdc, 0, 0, &pt1);
			CRenderClip clip(hdc, &rcClip, m_pOwner->GetClipRgnChildren());
			HRESULT hr = m_ts->TxDraw(DVASPECT_CONTENT, 0, NULL, NULL, hdc, NULL, (RECTL*) &rc, NULL/*(RECTL*) &rcClip*/, NULL, NULL, 0, m_bActive?TXTVIEW_ACTIVE:TXTVIEW_INACTIVE); 
			//::SetWindowOrgEx(hdc, pt1.x, pt1.y, NULL);
		}

		m_pOwner->UpdateDrawData(idd);
	}
// 	else if (!bDraw && m_ts && m_bActive)
// 	{
// 		idd.cyTotalHeight += m_szCached.cy;
// 		idd.pt.x = idd.rc.left + idd.lLineIndent;
// 		idd.pt.y += m_szCached.cy;
// 		rt(this)->UpdateDrawData(idd);
// 	}
	else
		__super::OnPaintContent(hde, hdc, rcPaint, bDraw, pidd);
}