#ifndef __DUIEDIT_H__
#define __DUIEDIT_H__

#pragma once

#include "DuiManager.h"
#include "../../common/dispatchimpl.h"
#include "DuiEditHelper.h"

class CDuiRichEdit;

struct DuiEditableData
{
	CDuiLayoutManager* m_pLayoutMgr;
	HWND m_hWnd;

	CHARFORMAT2W m_cf;					// Default character format
	PARAFORMAT2 m_pf;					// Default paragraph format
	LONG m_fontHeight;					// default font height
	DWORD m_dwProps;					// text properties
	DWORD m_dwStyles;					// text style
	DWORD m_dwPropsChanged;				// runtime text properties state

	// cache
	DWORD m_bTimer:1;
	DWORD m_bActive:1;
	DWORD m_bFocus:1;
	DWORD m_bCaretShow:1;
	DWORD m_bScrollPending:1;

	DuiEditableData() { memset(this, 0, sizeof(DuiEditableData)); }
};

class CDuiEditable : protected DuiEditableData,
					public ITextHost,
					public CRichEditHelper,
					public ControlExtension
{
	DECLARE_BASE_CLASS_CAST(CDuiEditable, ControlExtension)

protected:
	//ITextServices* m_ts;

	//SIZE m_szCached;

	BOOL ParseTextBitString(LPCOLESTR lpszData, DWORD& bit);
	BOOL ParseEditStyleString(LPCOLESTR lpszData, DWORD& style);
	void AutoActivate();

public:
	CDuiEditable();
	~CDuiEditable();

	void Init();
	void UpdateCharFormat();
	void UpdateParaFormat();

	DWORD ParseTextBit(BOOL bUpdate=FALSE);
	DWORD ParseEditStyle();

	HRESULT CreateEdit();
	void ReleaseEdit();

	BOOL OnMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& lResult);

	// IUnknown
public:
	STDMETHOD_(ULONG, AddRef)() { return 2; }
	STDMETHOD_(ULONG, Release)() { return 1; }
	STDMETHOD(QueryInterface)(REFIID iid, LPVOID* ppvObj)
	{
		if (!ppvObj) return E_POINTER;
		*ppvObj = NULL;

		if (IsEqualIID(iid, __uuidof(IUnknown)) ||
			IsEqualIID(iid, IID_ITextHost) )
			*ppvObj = (ITextHost*)this;

		return *ppvObj ? S_OK : E_NOINTERFACE;
	}

	// ITextHost members
public:
	//@cmember Get the DC for the host
	virtual HDC 		TxGetDC();
	//@cmember Release the DC gotten from the host
	virtual INT			TxReleaseDC(HDC hdc);
	//@cmember Show the scroll bar
	virtual BOOL 		TxShowScrollBar(INT fnBar, BOOL fShow);
	//@cmember Enable the scroll bar
	virtual BOOL 		TxEnableScrollBar (INT fuSBFlags, INT fuArrowflags);
	//@cmember Set the scroll range
	virtual BOOL 		TxSetScrollRange(INT fnBar, LONG nMinPos, INT nMaxPos, BOOL fRedraw);
	//@cmember Set the scroll position
	virtual BOOL 		TxSetScrollPos (INT fnBar, INT nPos, BOOL fRedraw);
	//@cmember InvalidateRect
	virtual void		TxInvalidateRect(LPCRECT prc, BOOL fMode);
	//@cmember Send a WM_PAINT to the window
	virtual void 		TxViewChange(BOOL fUpdate);
	//@cmember Create the caret
	virtual BOOL		TxCreateCaret(HBITMAP hbmp, INT xWidth, INT yHeight);
	//@cmember Show the caret
	virtual BOOL		TxShowCaret(BOOL fShow);
	//@cmember Set the caret position
	virtual BOOL		TxSetCaretPos(INT x, INT y);
	//@cmember Create a timer with the specified timeout
	virtual BOOL 		TxSetTimer(UINT idTimer, UINT uTimeout);
	//@cmember Destroy a timer
	virtual void 		TxKillTimer(UINT idTimer);
	//@cmember Scroll the content of the specified window's client area
	virtual void		TxScrollWindowEx (INT dx, INT dy, LPCRECT lprcScroll, LPCRECT lprcClip, HRGN hrgnUpdate, LPRECT lprcUpdate, UINT fuScroll);
	//@cmember Get mouse capture
	virtual void		TxSetCapture(BOOL fCapture);
	//@cmember Set the focus to the text window
	virtual void		TxSetFocus();
	//@cmember Establish a new cursor shape
	virtual void 		TxSetCursor(HCURSOR hcur, BOOL fText);
	//@cmember Converts screen coordinates of a specified point to the client coordinates
	virtual BOOL 		TxScreenToClient (LPPOINT lppt);
	//@cmember Converts the client coordinates of a specified point to screen coordinates
	virtual BOOL		TxClientToScreen (LPPOINT lppt);
	//@cmember Request host to activate text services
	virtual HRESULT		TxActivate( LONG * plOldState );
	//@cmember Request host to deactivate text services
	virtual HRESULT		TxDeactivate( LONG lNewState );
	//@cmember Retrieves the coordinates of a window's client area
	virtual HRESULT		TxGetClientRect(LPRECT prc);
	//@cmember Get the view rectangle relative to the inset
	virtual HRESULT		TxGetViewInset(LPRECT prc);
	//@cmember Get the default character format for the text
	virtual HRESULT 	TxGetCharFormat(const CHARFORMATW **ppCF );
	//@cmember Get the default paragraph format for the text
	virtual HRESULT		TxGetParaFormat(const PARAFORMAT **ppPF);
	//@cmember Get the background color for the window
	virtual COLORREF	TxGetSysColor(int nIndex);
	//@cmember Get the background (either opaque or transparent)
	virtual HRESULT		TxGetBackStyle(TXTBACKSTYLE *pstyle);
	//@cmember Get the maximum length for the text
	virtual HRESULT		TxGetMaxLength(DWORD *plength);
	//@cmember Get the bits representing requested scroll bars for the window
	virtual HRESULT		TxGetScrollBars(DWORD *pdwScrollBar);
	//@cmember Get the character to display for password input
	virtual HRESULT		TxGetPasswordChar(TCHAR *pch);
	//@cmember Get the accelerator character
	virtual HRESULT		TxGetAcceleratorPos(LONG *pcp);
	//@cmember Get the native size
	virtual HRESULT		TxGetExtent(LPSIZEL lpExtent);
	//@cmember Notify host that default character format has changed
	virtual HRESULT 	OnTxCharFormatChange (const CHARFORMATW * pcf);
	//@cmember Notify host that default paragraph format has changed
	virtual HRESULT		OnTxParaFormatChange (const PARAFORMAT * ppf);
	//@cmember Bulk access to bit properties
	virtual HRESULT		TxGetPropertyBits(DWORD dwMask, DWORD *pdwBits);
	//@cmember Notify host of events
	virtual HRESULT		TxNotify(DWORD iNotify, void *pv);
	// Far East Methods for getting the Input Context
	virtual HIMC		TxImmGetContext();
	virtual void		TxImmReleaseContext( HIMC himc );
	//@cmember Returns HIMETRIC size of the control bar.
	virtual HRESULT		TxGetSelectionBarWidth (LONG *lSelBarWidth);
};

//////////////////////////////////////////////////////////////////////////

class CDuiRichEdit : public CDuiEditable
					//, public ISinkOnGetControlFlags
					, public ISinkIsMyEvent
					, public ISinkOnScroll
{
	DECLARE_CONTROL_EXTENSION(CDuiRichEdit)
	DECLARE_CLASS_CAST(CDuiRichEdit, CDuiEditable, /*ISinkOnGetControlFlags,*/ ISinkIsMyEvent, ISinkOnScroll)
	DECLARE_BASE_CLASS_STYLE("display:inlineblock; overflow:auto; cursor:text; border:solid 1 gray; padding:3 1;")
	virtual UINT OnGetControlFlags(HANDLE)
	{
		return DUIFLAG_NOFOCUSFRAME | DUIFLAG_NOCHILDREN | DUIFLAG_TABSTOP | ((m_dwProps&TXTBIT_MULTILINE) ? DUIFLAG_WANTRETURN : 0);
	}

public:
	CDuiRichEdit();
	~CDuiRichEdit();

	virtual BOOL IsMyEvent(HANDLE, DuiEvent* pEvent);
	virtual void OnScroll(HANDLE, HDE pScroll, BOOL bVert=TRUE);

	//virtual void OnGetContentSize(SIZE& sz);

	void OnConnect();
	virtual void OnPaintContent(HDE hde, HDC hdc, RECT rcPaint, BOOL bDraw, InDrawData* pidd);

	void SetAttribute(HDE hde, LPCWSTR szName, VARIANT* value);

protected:
	Begin_Disp_Map(CDuiRichEdit, CRichEditHelper)
		//Disp_PropertyGet(2, object)
	End_Disp_Map()
};

THIS_IS_CONTROL_EXTENSION3(CDuiRichEdit, Edit, TextBox, Text)

#endif // __DUIEDIT_H__