#ifndef __DUIEDITHELPER_H__
#define __DUIEDITHELPER_H__

#pragma once

#include <Richedit.h>
#include <TextServ.h>
#include <RichOle.h>
#include <atlalloc.h>

#define __AutomationMethod(type, func) type func

#define __AutomationGet_Begin(prop) HRESULT Get##prop(VARIANT* pRet) { CComVariant vProp; {
#define __AutomationGet_End() } vProp.Detach(pRet); return S_OK; }
#define __AutomationGet(prop, func, type) HRESULT Get##prop(VARIANT* pRet) { CComVariant v=(type)func(); v.Detach(pRet); return S_OK; }

#define __AutomationPut_Begin(prop) HRESULT Put##prop(VARIANT* pVar) { CComVariant vProp=*pVar; {
#define __AutomationPut_End() } return S_OK; }
#define __AutomationPut(prop, func, type) HRESULT Put##prop(VARIANT* pVar) { func((type)pVar->lVal); return S_OK; }

EXTERN_C __declspec(selectany) const IID IID_ITextServices = { // 8d33f740-cf58-11ce-a89d-00aa006cadc5
	0x8d33f740,
	0xcf58,
	0x11ce,
	{0xa8, 0x9d, 0x00, 0xaa, 0x00, 0x6c, 0xad, 0xc5}
};

EXTERN_C __declspec(selectany) const IID IID_ITextHost = { /* c5bdd8d0-d26e-11ce-a89e-00aa006cadc5 */
	0xc5bdd8d0,
	0xd26e,
	0x11ce,
	{0xa8, 0x9e, 0x00, 0xaa, 0x00, 0x6c, 0xad, 0xc5}
};

#pragma warning(push)
#pragma warning(disable:4800)


//// Ole Point
//class COlePoint : public POINT
//{
//public:
//	COlePoint() {x=y=0;}
//
//	Begin_Auto_Disp_Map(COlePoint)
//		Disp_Property_Base(1, x, POINT, long)
//		Disp_Property_Base(2, y, POINT, long)
//	End_Disp_Map()
//};
//
//// Ole Rect
//class COleRect : public RECT
//{
//public:
//	COleRect() { ::SetRectEmpty(this); }
//
//	Begin_Auto_Disp_Map(COleRect)
//		Disp_Property_Base(1, left, RECT, long)
//		Disp_Property_Base(2, right, RECT, long)
//		Disp_Property_Base(3, top, RECT, long)
//		Disp_Property_Base(4, bottom, RECT, long)
//	End_Disp_Map()
//};
//
//// Ole CharFormat
//class COleCharFormat : public CHARFORMAT2W
//{
//public:
//	COleCharFormat() { memset((CHARFORMAT2W*)this, 0, sizeof(CHARFORMAT2W)); cbSize = sizeof(CHARFORMAT2W); }
//
//	Begin_Auto_Disp_Map(COleCharFormat)
//		// CHARFORMATW
//		Disp_Property_Base_Ex(1, mask,			CHARFORMATW, dwMask,			DWORD)
//		Disp_Property_Base_Ex(2, effects,		CHARFORMATW, dwEffects,			DWORD)
//		Disp_Property_Base_Ex(3, height,		CHARFORMATW, yHeight,			LONG)
//		Disp_Property_Base_Ex(4, offset,		CHARFORMATW, yOffset,			LONG)
//		Disp_Property_Base_Ex(5, textColor,		CHARFORMATW, crTextColor,		COLORREF)
//		Disp_Property_Base_Ex(6, charset,		CHARFORMATW, bCharSet,			BYTE)
//		Disp_Property_Base_Ex(7, pitchAndFamily,CHARFORMATW, bPitchAndFamily,	BYTE)
//		//Disp_Property_Base_Ex(8, faceName, CHARFORMATW, szFaceName, LPCOLESTR)
//		Disp_Property(8, faceName)
//
//		// CHARFORMAT2W
//		Disp_Property_Base_Ex(21, weight,		CHARFORMAT2W, wWeight,			WORD	)	// Font weight (LOGFONT value)
//		Disp_Property_Base_Ex(22, spacing,		CHARFORMAT2W, sSpacing,			SHORT	)	// Amount to space between letters
//		Disp_Property_Base_Ex(23, backColor,	CHARFORMAT2W, crBackColor,		COLORREF)	// Background color
//		Disp_Property_Base_Ex(24, lcid,			CHARFORMAT2W, lcid,				LCID	)	// Locale ID
//		Disp_Property_Base_Ex(25, style,		CHARFORMAT2W, sStyle,			SHORT	)	// Style handle
//		Disp_Property_Base_Ex(26, kerning,		CHARFORMAT2W, wKerning,			WORD	)	// Twip size above which to kern char pair
//		Disp_Property_Base_Ex(27, underline,	CHARFORMAT2W, bUnderlineType,	BYTE	)	// Underline type
//		Disp_Property_Base_Ex(28, animation,	CHARFORMAT2W, bAnimation	,	BYTE	)	// Animated text like marching ants
//		Disp_Property_Base_Ex(29, revAuthor,	CHARFORMAT2W, bRevAuthor	,	BYTE	)	// Revision author index
//	End_Disp_Map()
//
//	HRESULT GetfaceName(VARIANT* pRet)
//	{
//		CComVariant v = szFaceName;
//		v.Detach(pRet);
//		return S_OK;
//	}
//
//	HRESULT PutfaceName(VARIANT* pVal)
//	{
//		CComVariant v = *pVal;
//		if (SUCCEEDED(v.ChangeType(VT_BSTR, pVal)) && lstrcpynW(szFaceName, v.bstrVal, LF_FACESIZE))
//			return S_OK;
//		return E_FAIL;
//	}
//};
//
//// Ole ParaFormat
//class COleParaFormat : public PARAFORMAT2
//{
//public:
//	COleParaFormat() { memset((PARAFORMAT2*)this, 0, sizeof(PARAFORMAT2)); cbSize = sizeof(PARAFORMAT2); }
//
//	Begin_Auto_Disp_Map(COleParaFormat)
//		// PARAFORMAT
//		Disp_Property_Base_Ex(1, mask,			PARAFORMAT, dwMask,			DWORD	)
//		Disp_Property_Base_Ex(2, numbering,		PARAFORMAT, wNumbering,		WORD	)
//		Disp_Property_Base_Ex(3, effects,		PARAFORMAT, wEffects,		WORD	)
//		Disp_Property_Base_Ex(4, startIndent,	PARAFORMAT, dxStartIndent,	LONG	)
//		Disp_Property_Base_Ex(5, rightIndent,	PARAFORMAT, dxRightIndent,	LONG	)
//		Disp_Property_Base_Ex(6, offset,		PARAFORMAT, dxOffset,		LONG	)
//		Disp_Property_Base_Ex(7, alignment,		PARAFORMAT, wAlignment,		WORD	)
//		Disp_Property_Base_Ex(8, tabCount,		PARAFORMAT, cTabCount,		SHORT	)
//		//Disp_Property_Base_Ex(9, tabs,			PARAFORMAT, rgxTabs,		LONG	)
//		Disp_Method_With_Option(9, tabs, LONG, 2, Params(LONG), ParamsOpt1(LONG), ParamsOptDefValue(-1))
//
//		// PARAFORMAT2
//		Disp_Property_Base_Ex(21, spaceBefore,		PARAFORMAT2, dySpaceBefore,			LONG	)	// Vertical spacing before para
//		Disp_Property_Base_Ex(22, spaceAfter,		PARAFORMAT2, dySpaceAfter,			LONG	)	// Vertical spacing after para
//		Disp_Property_Base_Ex(23, lineSpacing,		PARAFORMAT2, dyLineSpacing,			LONG	)	// Line spacing depending on Rule
//		Disp_Property_Base_Ex(24, style,			PARAFORMAT2, sStyle,				SHORT	)	// Style handle
//		Disp_Property_Base_Ex(25, lineSpacingRule,	PARAFORMAT2, bLineSpacingRule,		BYTE	)	// Rule for line spacing (see tom.doc)
//		Disp_Property_Base_Ex(26, outlineLevel,		PARAFORMAT2, bOutlineLevel,			BYTE	)	// Outline level
//		Disp_Property_Base_Ex(27, shadingWeight,	PARAFORMAT2, wShadingWeight,		WORD	)	// Shading in hundredths of a per cent
//		Disp_Property_Base_Ex(28, shadingStyle,		PARAFORMAT2, wShadingStyle,			WORD	)	// Nibble 0: style, 1: cfpat, 2: cbpat
//		Disp_Property_Base_Ex(29, numberingStart,	PARAFORMAT2, wNumberingStart,		WORD	)	// Starting value for numbering
//		Disp_Property_Base_Ex(30, numberingStyle,	PARAFORMAT2, wNumberingStyle,		WORD	)	// Alignment, roman/arabic, (), ), ., etc.
//		Disp_Property_Base_Ex(31, numberingTab,		PARAFORMAT2, wNumberingTab,			WORD	)	// Space bet FirstIndent & 1st-line text
//		Disp_Property_Base_Ex(32, borderSpace,		PARAFORMAT2, wBorderSpace,			WORD	)	// Border-text spaces (nbl/bdr in pts)
//		Disp_Property_Base_Ex(33, borderWidth,		PARAFORMAT2, wBorderWidth,			WORD	)	// Pen widths (nbl/bdr in half pts)
//		Disp_Property_Base_Ex(34, borders,			PARAFORMAT2, wBorders,				WORD	)	// Border styles (nibble/border)
//	End_Disp_Map()
//
//	LONG tabs(LONG lIndex, LONG newTab=-1)
//	{
//		if (lIndex<0 || lIndex>=MAX_TAB_STOPS) return -1;
//
//		// newTab==-1 means that get tab
//		if (newTab==-1)
//			return rgxTabs[lIndex];
//		// newTab >= 0 means that set tab
//		else
//			return rgxTabs[lIndex]=newTab;
//	}
//};


class CRichEditHelper
{
private:
	CTempBuffer<OLECHAR> _strBuf;

public:
	ITextServices* m_ts;
	HRESULT m_hrLast;

public:
	// Constructors
	CRichEditHelper() : m_ts(NULL), m_hrLast(S_FALSE)
	{ }

	virtual ~CRichEditHelper()
	{
		if (m_ts)
		{
			m_ts->Release();
			m_ts = NULL;
		}
	}

	HRESULT Create(ITextHost* pHost)
	{
		if (pHost == NULL) return E_INVALIDARG;

		CComPtr<IUnknown> spUnk;
		m_hrLast = ::CreateTextServices(NULL, pHost, &spUnk);
		if (SUCCEEDED(m_hrLast))
			m_hrLast = spUnk.p->QueryInterface(IID_ITextServices, (void**)&m_ts);
		return m_hrLast;
	}

	static LPCTSTR GetLibraryName()
	{
#if (_RICHEDIT_VER >= 0x0200)
		return _T("RICHED20.DLL");
#else
		return _T("RICHED32.DLL");
#endif
	}

	HRESULT GetLastHResult() const
	{
		return m_hrLast;
	}

	LRESULT TxSendMessage(UINT msg, WPARAM wparam=0, LPARAM lparam=0)
	{
		ATLASSERT(m_ts);
		LRESULT lRes = 0;
		m_hrLast = m_ts->TxSendMessage(msg, wparam, lparam, &lRes);
		return lRes;
	}

	int GetLineCount()
	{
		ATLASSERT(m_ts);
		return (int)TxSendMessage(EM_GETLINECOUNT, 0, 0L);
	}
	//__AutomationGet(lineCount, GetLineCount, LONG)

	BOOL GetModify()
	{
		ATLASSERT(m_ts);
		return (BOOL)TxSendMessage(EM_GETMODIFY, 0, 0L);
	}

	void SetModify(BOOL bModified = TRUE)
	{
		ATLASSERT(m_ts);
		TxSendMessage(EM_SETMODIFY, bModified, 0L);
	}
	//__AutomationGet(isDirty, GetModify, bool)
	//__AutomationPut(isDirty, SetModify, BOOL)

	void GetRect(LPRECT lpRect)
	{
		ATLASSERT(m_ts);
		TxSendMessage(EM_GETRECT, 0, (LPARAM)lpRect);
	}
	//__AutomationGet_Begin(rect) 
	//{
	//	vProp.vt = VT_DISPATCH;
	//	COleRect* prc = NULL;
	//	prc->CreateInstance(&V_DISPATCH(&vProp), &prc);
	//	if (prc)
	//		GetRect(prc);
	//} __AutomationGet_End()

	DWORD GetOptions()
	{
		ATLASSERT(m_ts);
		return (DWORD)TxSendMessage(EM_GETOPTIONS, 0, 0L);
	}
	//__AutomationGet(options, GetOptions, ULONG)

	DWORD SetOptions(WORD wOperation, DWORD dwOptions)
	{
		ATLASSERT(m_ts);
		return (DWORD)TxSendMessage(EM_SETOPTIONS, wOperation, dwOptions);
	}

	// NOTE: first word in lpszBuffer must contain the size of the buffer!
	int GetLine(int nIndex, LPTSTR lpszBuffer)
	{
		ATLASSERT(m_ts);
		return (int)TxSendMessage(EM_GETLINE, nIndex, (LPARAM)lpszBuffer);
	}

	int GetLine(int nIndex, LPTSTR lpszBuffer, int nMaxLength)
	{
		ATLASSERT(m_ts);
		*(LPWORD)lpszBuffer = (WORD)nMaxLength;
		return (int)TxSendMessage(EM_GETLINE, nIndex, (LPARAM)lpszBuffer);
	}
	//__AutomationMethod(LPCOLESTR, getLine)(long nIndex)
	//{
	//	int n = LineLength(nIndex);
	//	LPOLESTR p = _strBuf.Reallocate(n+1);
	//	n = GetLine(nIndex, p, n+1);
	//	p[n] = 0;
	//	return _strBuf;
	//}

	BOOL CanUndo()
	{
		ATLASSERT(m_ts);
		return (BOOL)TxSendMessage(EM_CANUNDO, 0, 0L);
	}
	//__AutomationGet(canUndo, CanUndo, bool)

	BOOL CanPaste(UINT nFormat = 0)
	{
		ATLASSERT(m_ts);
		return (BOOL)TxSendMessage(EM_CANPASTE, nFormat, 0L);
	}
	//__AutomationGet(canPaste, CanPaste, bool)

	void GetSel(LONG& nStartChar, LONG& nEndChar)
	{
		ATLASSERT(m_ts);
		CHARRANGE cr = { 0, 0 };
		TxSendMessage(EM_EXGETSEL, 0, (LPARAM)&cr);
		nStartChar = cr.cpMin;
		nEndChar = cr.cpMax;
	}

	void GetSel(CHARRANGE &cr)
	{
		ATLASSERT(m_ts);
		TxSendMessage(EM_EXGETSEL, 0, (LPARAM)&cr);
	}
	//__AutomationGet_Begin(selMin) { CHARRANGE cr={0}; GetSel(cr); vProp=cr.cpMin; } __AutomationGet_End();
	//__AutomationGet_Begin(selMax) { CHARRANGE cr={0}; GetSel(cr); vProp=cr.cpMax; } __AutomationGet_End();

	int SetSel(LONG nStartChar, LONG nEndChar)
	{
		ATLASSERT(m_ts);
		CHARRANGE cr = { nStartChar, nEndChar };
		return (int)TxSendMessage(EM_EXSETSEL, 0, (LPARAM)&cr);
	}

	int SetSel(CHARRANGE &cr)
	{
		ATLASSERT(m_ts);
		return (int)TxSendMessage(EM_EXSETSEL, 0, (LPARAM)&cr);
	}
	//__AutomationMethod(void, select)(LONG nStart, LONG nEnd) { SetSel(nStart, nEnd); }

	int SetSelAll()
	{
		return SetSel(0, -1);
	}
	//__AutomationMethod(void, selectAll)() { SetSelAll(); }

	int SetSelNone()
	{
		return SetSel(-1, 0);
	}
	//__AutomationMethod(void, unselect)() { SetSelNone(); }

	DWORD GetDefaultCharFormat(CHARFORMAT& cf)
	{
		ATLASSERT(m_ts);
		cf.cbSize = sizeof(CHARFORMAT);
		return (DWORD)TxSendMessage(EM_GETCHARFORMAT, 0, (LPARAM)&cf);
	}
	//__AutomationGet_Begin(defaultCharFormat)
	//{
	//	vProp.vt = VT_DISPATCH;
	//	COleCharFormat* cf = NULL;
	//	cf->CreateInstance(&V_DISPATCH(&vProp), &cf);
	//	if (cf)
	//		GetDefaultCharFormat(*cf);
	//} __AutomationGet_End()

	DWORD GetSelectionCharFormat(CHARFORMAT& cf)
	{
		ATLASSERT(m_ts);
		cf.cbSize = sizeof(CHARFORMAT);
		return (DWORD)TxSendMessage(EM_GETCHARFORMAT, 1, (LPARAM)&cf);
	}
	//__AutomationGet_Begin(selectionCharFormat)
	//{
	//	vProp.vt = VT_DISPATCH;
	//	COleCharFormat* cf = NULL;
	//	cf->CreateInstance(&V_DISPATCH(&vProp), &cf);
	//	if (cf)
	//		GetSelectionCharFormat(*cf);
	//} __AutomationGet_End()

	DWORD GetEventMask()
	{
		ATLASSERT(m_ts);
		return (DWORD)TxSendMessage(EM_GETEVENTMASK, 0, 0L);
	}

	LONG GetLimitText()
	{
		ATLASSERT(m_ts);
		return (LONG)TxSendMessage(EM_GETLIMITTEXT, 0, 0L);
	}
	//__AutomationGet(limit, GetLimitText, long)

	DWORD GetParaFormat(PARAFORMAT& pf)
	{
		ATLASSERT(m_ts);
		pf.cbSize = sizeof(PARAFORMAT);
		return (DWORD)TxSendMessage(EM_GETPARAFORMAT, 0, (LPARAM)&pf);
	}
	//__AutomationGet_Begin(paraFormat)
	//{
	//	vProp.vt = VT_DISPATCH;
	//	COleParaFormat* pf = NULL;
	//	pf->CreateInstance(&V_DISPATCH(&vProp), &pf);
	//	if (pf)
	//		GetParaFormat(*pf);
	//} __AutomationGet_End()

#if (_RICHEDIT_VER >= 0x0200)
	LONG GetSelText(LPTSTR lpstrBuff)
	{
		ATLASSERT(m_ts);
		return (LONG)TxSendMessage(EM_GETSELTEXT, 0, (LPARAM)lpstrBuff);
	}
#else // !(_RICHEDIT_VER >= 0x0200)
	// RichEdit 1.0 EM_GETSELTEXT is ANSI only
	LONG GetSelText(LPSTR lpstrBuff)
	{
		ATLASSERT(m_ts);
		return (LONG)TxSendMessage(EM_GETSELTEXT, 0, (LPARAM)lpstrBuff);
	}
#endif // !(_RICHEDIT_VER >= 0x0200)

#ifndef _ATL_NO_COM
	BOOL GetSelTextBSTR(BSTR& bstrText)
	{
		USES_CONVERSION;
		ATLASSERT(m_ts);
		ATLASSERT(bstrText == NULL);

		CHARRANGE cr = { 0, 0 };
		TxSendMessage(EM_EXGETSEL, 0, (LPARAM)&cr);

#if (_RICHEDIT_VER >= 0x0200)
		CTempBuffer<TCHAR, _WTL_STACK_ALLOC_THRESHOLD> buff;
		LPTSTR lpstrText = buff.Allocate(cr.cpMax - cr.cpMin + 1);
		if(lpstrText == NULL)
			return FALSE;
		if(TxSendMessage(EM_GETSELTEXT, 0, (LPARAM)lpstrText) == 0)
			return FALSE;

		bstrText = ::SysAllocString(T2W(lpstrText));
#else // !(_RICHEDIT_VER >= 0x0200)
		CTempBuffer<char, _WTL_STACK_ALLOC_THRESHOLD> buff;
		LPSTR lpstrText = buff.Allocate(cr.cpMax - cr.cpMin + 1);
		if(lpstrText == NULL)
			return FALSE;
		if(TxSendMessage(EM_GETSELTEXT, 0, (LPARAM)lpstrText) == 0)
			return FALSE;

		bstrText = ::SysAllocString(A2W(lpstrText));
#endif // !(_RICHEDIT_VER >= 0x0200)

		return (bstrText != NULL) ? TRUE : FALSE;
	}
	//__AutomationGet_Begin(selection) { BSTR bstr=NULL; GetSelTextBSTR(bstr); vProp=bstr; ::SysFreeString(bstr); } __AutomationGet_End();
#endif // !_ATL_NO_COM

#if defined(_WTL_USE_CSTRING) || defined(__ATLSTR_H__)
	LONG GetSelText(_CSTRING_NS::CString& strText)
	{
		ATLASSERT(m_ts);

		CHARRANGE cr = { 0, 0 };
		TxSendMessage(EM_EXGETSEL, 0, (LPARAM)&cr);

#if (_RICHEDIT_VER >= 0x0200)
		LONG lLen = 0;
		LPTSTR lpstrText = strText.GetBufferSetLength(cr.cpMax - cr.cpMin);
		if(lpstrText != NULL)
		{
			lLen = (LONG)TxSendMessage(EM_GETSELTEXT, 0, (LPARAM)lpstrText);
			strText.ReleaseBuffer();
		}
#else // !(_RICHEDIT_VER >= 0x0200)
		CTempBuffer<char, _WTL_STACK_ALLOC_THRESHOLD> buff;
		LPSTR lpstrText = buff.Allocate(cr.cpMax - cr.cpMin + 1);
		if(lpstrText == NULL)
			return 0;
		LONG lLen = (LONG)TxSendMessage(EM_GETSELTEXT, 0, (LPARAM)lpstrText);
		if(lLen == 0)
			return 0;

		USES_CONVERSION;
		strText = A2T(lpstrText);
#endif // !(_RICHEDIT_VER >= 0x0200)

		return lLen;
	}
#endif // defined(_WTL_USE_CSTRING) || defined(__ATLSTR_H__)

	WORD GetSelectionType()
	{
		ATLASSERT(m_ts);
		return (WORD)TxSendMessage(EM_SELECTIONTYPE, 0, 0L);
	}
	//__AutomationGet(selectionType, GetSelectionType, WORD)

	COLORREF SetBackgroundColor(COLORREF cr)
	{
		ATLASSERT(m_ts);
		return (COLORREF)TxSendMessage(EM_SETBKGNDCOLOR, 0, cr);
	}

	COLORREF SetBackgroundColor()   // sets to system background
	{
		ATLASSERT(m_ts);
		return (COLORREF)TxSendMessage(EM_SETBKGNDCOLOR, 1, 0);
	}
	//__AutomationPut(backgroundColor, SetBackgroundColor, COLORREF)

	BOOL SetCharFormat(CHARFORMAT& cf, WORD wFlags)
	{
		ATLASSERT(m_ts);
		cf.cbSize = sizeof(CHARFORMAT);
		return (BOOL)TxSendMessage(EM_SETCHARFORMAT, (WPARAM)wFlags, (LPARAM)&cf);
	}
	//__AutomationMethod(bool, setCharFormat)(IDispatch* dispcf, WORD flags=SCF_SELECTION)
	//{
	//	if (dispcf==NULL) return false;

	//	CComQIPtr<IDispHost> host(dispcf);
	//	if (host.p == NULL) return false;
	//	COleCharFormat* pcf = (COleCharFormat*)host->GetOwner();
	//	if (pcf==NULL) return false;
	//	return !!SetCharFormat(*pcf, flags);
	//}

	BOOL SetDefaultCharFormat(CHARFORMAT& cf)
	{
		ATLASSERT(m_ts);
		cf.cbSize = sizeof(CHARFORMAT);
		return (BOOL)TxSendMessage(EM_SETCHARFORMAT, 0, (LPARAM)&cf);
	}
	//__AutomationPut_Begin(defaultCharFormat)
	//{
	//	if (vProp.vt!=VT_DISPATCH || vProp.pdispVal==NULL) return E_INVALIDARG;

	//	CComQIPtr<IDispHost> host(vProp.pdispVal);
	//	if (host.p == NULL) return E_FAIL;
	//	COleCharFormat* pcf = (COleCharFormat*)host->GetOwner();
	//	if (pcf==NULL) return E_FAIL;
	//	if (!SetDefaultCharFormat(*pcf)) return E_FAIL;
	//}__AutomationPut_End()

	BOOL SetSelectionCharFormat(CHARFORMAT& cf)
	{
		ATLASSERT(m_ts);
		cf.cbSize = sizeof(CHARFORMAT);
		return (BOOL)TxSendMessage(EM_SETCHARFORMAT, SCF_SELECTION, (LPARAM)&cf);
	}
	//__AutomationPut_Begin(selectionCharFormat)
	//{
	//	if (vProp.vt!=VT_DISPATCH || vProp.pdispVal==NULL) return E_INVALIDARG;

	//	CComQIPtr<IDispHost> host(vProp.pdispVal);
	//	if (host.p == NULL) return E_FAIL;
	//	COleCharFormat* pcf = (COleCharFormat*)host->GetOwner();
	//	if (pcf==NULL) return E_FAIL;
	//	if (!SetSelectionCharFormat(*pcf)) return E_FAIL;
	//}__AutomationPut_End()

	BOOL SetWordCharFormat(CHARFORMAT& cf)
	{
		ATLASSERT(m_ts);
		cf.cbSize = sizeof(CHARFORMAT);
		return (BOOL)TxSendMessage(EM_SETCHARFORMAT, SCF_SELECTION | SCF_WORD, (LPARAM)&cf);
	}
	//__AutomationPut_Begin(wordCharFormat)
	//{
	//	if (vProp.vt!=VT_DISPATCH || vProp.pdispVal==NULL) return E_INVALIDARG;

	//	CComQIPtr<IDispHost> host(vProp.pdispVal);
	//	if (host.p == NULL) return E_FAIL;
	//	COleCharFormat* pcf = (COleCharFormat*)host->GetOwner();
	//	if (pcf==NULL) return E_FAIL;
	//	if (!SetWordCharFormat(*pcf)) return E_FAIL;
	//}__AutomationPut_End()

	DWORD SetEventMask(DWORD dwEventMask)
	{
		ATLASSERT(m_ts);
		return (DWORD)TxSendMessage(EM_SETEVENTMASK, 0, dwEventMask);
	}

	BOOL SetParaFormat(PARAFORMAT& pf)
	{
		ATLASSERT(m_ts);
		pf.cbSize = sizeof(PARAFORMAT);
		return (BOOL)TxSendMessage(EM_SETPARAFORMAT, 0, (LPARAM)&pf);
	}
	//__AutomationPut_Begin(paraFormat)
	//{
	//	if (vProp.vt!=VT_DISPATCH || vProp.pdispVal==NULL) return E_INVALIDARG;

	//	CComQIPtr<IDispHost> host(vProp.pdispVal);
	//	if (host.p == NULL) return E_FAIL;
	//	COleParaFormat* ppf = (COleParaFormat*)host->GetOwner();
	//	if (ppf==NULL) return E_FAIL;
	//	if (!SetParaFormat(*ppf)) return E_FAIL;
	//}__AutomationPut_End()

	BOOL SetTargetDevice(HDC hDC, int cxLineWidth)
	{
		ATLASSERT(m_ts);
		return (BOOL)TxSendMessage(EM_SETTARGETDEVICE, (WPARAM)hDC, cxLineWidth);
	}

	int GetTextLength()
	{
		ATLASSERT(m_ts);
		return (int)TxSendMessage(WM_GETTEXTLENGTH, 0, 0L);
	}
	//__AutomationGet(length, GetTextLength, long)

	BOOL SetReadOnly(BOOL bReadOnly = TRUE)
	{
		ATLASSERT(m_ts);
		return (BOOL)TxSendMessage(EM_SETREADONLY, bReadOnly, 0L);
	}
	//__AutomationPut(readonly, SetReadOnly, BOOL)

	int GetFirstVisibleLine()
	{
		ATLASSERT(m_ts);
		return (int)TxSendMessage(EM_GETFIRSTVISIBLELINE, 0, 0L);
	}
	//__AutomationGet(firstVisibleLine, GetFirstVisibleLine, long)

	EDITWORDBREAKPROCEX GetWordBreakProcEx()
	{
		ATLASSERT(m_ts);
		return (EDITWORDBREAKPROCEX)TxSendMessage(EM_GETWORDBREAKPROCEX, 0, 0L);
	}

	EDITWORDBREAKPROCEX SetWordBreakProcEx(EDITWORDBREAKPROCEX pfnEditWordBreakProcEx)
	{
		ATLASSERT(m_ts);
		return (EDITWORDBREAKPROCEX)TxSendMessage(EM_SETWORDBREAKPROCEX, 0, (LPARAM)pfnEditWordBreakProcEx);
	}

	int GetTextRange(TEXTRANGE* pTextRange)
	{
		ATLASSERT(m_ts);
		return (int)TxSendMessage(EM_GETTEXTRANGE, 0, (LPARAM)pTextRange);
	}

#if (_RICHEDIT_VER >= 0x0200)
	int GetTextRange(LONG nStartChar, LONG nEndChar, LPTSTR lpstrText)
	{
		ATLASSERT(m_ts);
		TEXTRANGE tr = { 0 };
		tr.chrg.cpMin = nStartChar;
		tr.chrg.cpMax = nEndChar;
		tr.lpstrText = lpstrText;
		return (int)TxSendMessage(EM_GETTEXTRANGE, 0, (LPARAM)&tr);
	}
	//__AutomationMethod(LPCOLESTR, textRange)(LONG nStart, LONG nEnd)
	//{
	//	LPOLESTR p = _strBuf.Reallocate(nEnd-nStart+1);
	//	p[GetTextRange(nStart, nEnd, p)] = 0;
	//	return _strBuf;
	//}
#else // !(_RICHEDIT_VER >= 0x0200)

	int GetTextRange(LONG nStartChar, LONG nEndChar, LPSTR lpstrText)
	{
		ATLASSERT(m_ts);
		TEXTRANGE tr = { 0 };
		tr.chrg.cpMin = nStartChar;
		tr.chrg.cpMax = nEndChar;
		tr.lpstrText = lpstrText;
		return (int)TxSendMessage(EM_GETTEXTRANGE, 0, (LPARAM)&tr);
	}
#endif // !(_RICHEDIT_VER >= 0x0200)

#if (_RICHEDIT_VER >= 0x0200)
	DWORD GetDefaultCharFormat(CHARFORMAT2& cf)
	{
		ATLASSERT(m_ts);
		cf.cbSize = sizeof(CHARFORMAT2);
		return (DWORD)TxSendMessage(EM_GETCHARFORMAT, 0, (LPARAM)&cf);
	}

	BOOL SetCharFormat(CHARFORMAT2& cf, WORD wFlags)
	{
		ATLASSERT(m_ts);
		cf.cbSize = sizeof(CHARFORMAT2);
		return (BOOL)TxSendMessage(EM_SETCHARFORMAT, (WPARAM)wFlags, (LPARAM)&cf);
	}

	BOOL SetDefaultCharFormat(CHARFORMAT2& cf)
	{
		ATLASSERT(m_ts);
		cf.cbSize = sizeof(CHARFORMAT2);
		return (BOOL)TxSendMessage(EM_SETCHARFORMAT, 0, (LPARAM)&cf);
	}

	DWORD GetSelectionCharFormat(CHARFORMAT2& cf)
	{
		ATLASSERT(m_ts);
		cf.cbSize = sizeof(CHARFORMAT2);
		return (DWORD)TxSendMessage(EM_GETCHARFORMAT, 1, (LPARAM)&cf);
	}

	BOOL SetSelectionCharFormat(CHARFORMAT2& cf)
	{
		ATLASSERT(m_ts);
		cf.cbSize = sizeof(CHARFORMAT2);
		return (BOOL)TxSendMessage(EM_SETCHARFORMAT, SCF_SELECTION, (LPARAM)&cf);
	}

	BOOL SetWordCharFormat(CHARFORMAT2& cf)
	{
		ATLASSERT(m_ts);
		cf.cbSize = sizeof(CHARFORMAT2);
		return (BOOL)TxSendMessage(EM_SETCHARFORMAT, SCF_SELECTION | SCF_WORD, (LPARAM)&cf);
	}

	DWORD GetParaFormat(PARAFORMAT2& pf)
	{
		ATLASSERT(m_ts);
		pf.cbSize = sizeof(PARAFORMAT2);
		return (DWORD)TxSendMessage(EM_GETPARAFORMAT, 0, (LPARAM)&pf);
	}

	BOOL SetParaFormat(PARAFORMAT2& pf)
	{
		ATLASSERT(m_ts);
		pf.cbSize = sizeof(PARAFORMAT2);
		return (BOOL)TxSendMessage(EM_SETPARAFORMAT, 0, (LPARAM)&pf);
	}

	TEXTMODE GetTextMode()
	{
		ATLASSERT(m_ts);
		return (TEXTMODE)TxSendMessage(EM_GETTEXTMODE, 0, 0L);
	}

	BOOL SetTextMode(TEXTMODE enumTextMode)
	{
		ATLASSERT(m_ts);
		return !(BOOL)TxSendMessage(EM_SETTEXTMODE, enumTextMode, 0L);
	}

	UNDONAMEID GetUndoName()
	{
		ATLASSERT(m_ts);
		return (UNDONAMEID)TxSendMessage(EM_GETUNDONAME, 0, 0L);
	}
	//__AutomationGet(undoName, GetUndoName, long)

	UNDONAMEID GetRedoName()
	{
		ATLASSERT(m_ts);
		return (UNDONAMEID)TxSendMessage(EM_GETREDONAME, 0, 0L);
	}
	//__AutomationGet(redoName, GetRedoName, long)

	BOOL CanRedo()
	{
		ATLASSERT(m_ts);
		return (BOOL)TxSendMessage(EM_CANREDO, 0, 0L);
	}
	//__AutomationGet(canRedo, CanRedo, bool)

	BOOL GetAutoURLDetect()
	{
		ATLASSERT(m_ts);
		return (BOOL)TxSendMessage(EM_GETAUTOURLDETECT, 0, 0L);
	}

	BOOL SetAutoURLDetect(BOOL bAutoDetect = TRUE)
	{
		ATLASSERT(m_ts);
		return !(BOOL)TxSendMessage(EM_AUTOURLDETECT, bAutoDetect, 0L);
	}
	//__AutomationGet(autoUrl, GetAutoURLDetect, bool)
	//__AutomationPut(autoUrl, SetAutoURLDetect, BOOL)

	// this method is deprecated, please use SetAutoURLDetect
	BOOL EnableAutoURLDetect(BOOL bEnable = TRUE) { return SetAutoURLDetect(bEnable); }

	UINT SetUndoLimit(UINT uUndoLimit)
	{
		ATLASSERT(m_ts);
		return (UINT)TxSendMessage(EM_SETUNDOLIMIT, uUndoLimit, 0L);
	}
	//__AutomationPut(undoLimit, SetUndoLimit, UINT)

	void SetPalette(HPALETTE hPalette)
	{
		ATLASSERT(m_ts);
		TxSendMessage(EM_SETPALETTE, (WPARAM)hPalette, 0L);
	}

	int GetTextEx(GETTEXTEX* pGetTextEx, LPTSTR lpstrText)
	{
		ATLASSERT(m_ts);
		return (int)TxSendMessage(EM_GETTEXTEX, (WPARAM)pGetTextEx, (LPARAM)lpstrText);
	}

	int GetTextEx(LPTSTR lpstrText, int nTextLen, DWORD dwFlags = GT_DEFAULT, UINT uCodePage = 1200/*CP_ACP-ANSI; 1200-UNICODE*/, LPCSTR lpDefaultChar = NULL, LPBOOL lpUsedDefChar = NULL)
	{
		ATLASSERT(m_ts);
		GETTEXTEX gte = { 0 };
		gte.cb = nTextLen * sizeof(TCHAR);
		gte.codepage = uCodePage;
		gte.flags = dwFlags;
		gte.lpDefaultChar = lpDefaultChar;
		gte.lpUsedDefChar = lpUsedDefChar;
		return (int)TxSendMessage(EM_GETTEXTEX, (WPARAM)&gte, (LPARAM)lpstrText);
	}
	__AutomationGet_Begin(text)
	{
		int len = GetTextLengthEx();
		LPOLESTR p = _strBuf.Reallocate(len+1);
		GetTextEx(p, len+1);
		p[len] = 0;
		vProp = _strBuf;
	}__AutomationGet_End()

	int GetTextLengthEx(GETTEXTLENGTHEX* pGetTextLengthEx)
	{
		ATLASSERT(m_ts);
		return (int)TxSendMessage(EM_GETTEXTLENGTHEX, (WPARAM)pGetTextLengthEx, 0L);
	}

	int GetTextLengthEx(DWORD dwFlags = GTL_DEFAULT, UINT uCodePage = 1200)
	{
		ATLASSERT(m_ts);
		GETTEXTLENGTHEX gtle = { 0 };
		gtle.codepage = uCodePage;
		gtle.flags = dwFlags;
		return (int)TxSendMessage(EM_GETTEXTLENGTHEX, (WPARAM)&gtle, 0L);
	}
#endif // (_RICHEDIT_VER >= 0x0200)

#if (_RICHEDIT_VER >= 0x0300)
	int SetTextEx(SETTEXTEX* pSetTextEx, LPCTSTR lpstrText)
	{
		ATLASSERT(m_ts);
		return (int)TxSendMessage(EM_SETTEXTEX, (WPARAM)pSetTextEx, (LPARAM)lpstrText);
	}

	int SetTextEx(LPCTSTR lpstrText, DWORD dwFlags = ST_DEFAULT, UINT uCodePage = 1200)
	{
		ATLASSERT(m_ts);
		SETTEXTEX ste = { 0 };
		ste.flags = dwFlags;
		ste.codepage = uCodePage;
		return (int)TxSendMessage(EM_SETTEXTEX, (WPARAM)&ste, (LPARAM)lpstrText);
	}
	__AutomationPut_Begin(text)
	{
		if (vProp.vt==VT_EMPTY || FAILED(vProp.ChangeType(VT_BSTR))) return E_INVALIDARG;
		SetTextEx(vProp.bstrVal);
	}
	__AutomationPut_End()

	int GetEditStyle()
	{
		ATLASSERT(m_ts);
		return (int)TxSendMessage(EM_GETEDITSTYLE, 0, 0L);
	}

	int SetEditStyle(int nStyle, int nMask = -1)
	{
		ATLASSERT(m_ts);
		if(nMask == -1)
			nMask = nStyle;   // set everything specified
		return (int)TxSendMessage(EM_SETEDITSTYLE, nStyle, nMask);
	}

	BOOL SetFontSize(int nFontSizeDelta)
	{
		ATLASSERT(m_ts);
		ATLASSERT(nFontSizeDelta >= -1637 && nFontSizeDelta <= 1638);
		return (BOOL)TxSendMessage(EM_SETFONTSIZE, nFontSizeDelta, 0L);
	}

	void GetScrollPos(LPPOINT lpPoint)
	{
		ATLASSERT(m_ts);
		ATLASSERT(lpPoint != NULL);
		TxSendMessage(EM_GETSCROLLPOS, 0, (LPARAM)lpPoint);
	}
	//__AutomationGet_Begin(scrollPos)
	//{
	//	vProp.vt = VT_DISPATCH;
	//	COlePoint* pp = NULL;
	//	pp->CreateInstance(&V_DISPATCH(&vProp), &pp);
	//	if (pp)
	//		GetScrollPos(pp);
	//} __AutomationGet_End()

	void SetScrollPos(LPPOINT lpPoint)
	{
		ATLASSERT(m_ts);
		ATLASSERT(lpPoint != NULL);
		TxSendMessage(EM_SETSCROLLPOS, 0, (LPARAM)lpPoint);
	}
	//__AutomationPut_Begin(scrollPos)
	//{
	//	if (vProp.vt!=VT_DISPATCH || vProp.pdispVal==NULL) return E_INVALIDARG;

	//	CComQIPtr<IDispHost> host(vProp.pdispVal);
	//	if (host.p == NULL) return E_FAIL;
	//	COlePoint* pp = (COlePoint*)host->GetOwner();
	//	if (pp==NULL) return E_FAIL;
	//	SetScrollPos(pp);
	//}__AutomationPut_End()

	BOOL GetZoom(int& nNum, int& nDen)
	{
		ATLASSERT(m_ts);
		return (BOOL)TxSendMessage(EM_GETZOOM, (WPARAM)&nNum, (LPARAM)&nDen);
	}

	BOOL SetZoom(int nNum, int nDen)
	{
		ATLASSERT(m_ts);
		ATLASSERT(nNum >= 0 && nNum <= 64);
		ATLASSERT(nDen >= 0 && nDen <= 64);
		return (BOOL)TxSendMessage(EM_SETZOOM, nNum, nDen);
	}

	BOOL SetZoomOff()
	{
		ATLASSERT(m_ts);
		return (BOOL)TxSendMessage(EM_SETZOOM, 0, 0L);
	}
#endif // (_RICHEDIT_VER >= 0x0300)

	// Operations
	void LimitText(LONG nChars = 0)
	{
		ATLASSERT(m_ts);
		TxSendMessage(EM_EXLIMITTEXT, 0, nChars);
	}
	//__AutomationPut(limitText, LimitText, LONG)

	int LineFromChar(LONG nIndex)
	{
		ATLASSERT(m_ts);
		return (int)TxSendMessage(EM_EXLINEFROMCHAR, 0, nIndex);
	}
	//__AutomationMethod(long, lineFromChar)(LONG nIndex) { return LineFromChar(nIndex); }

	POINT PosFromChar(LONG nChar)
	{
		ATLASSERT(m_ts);
		POINT point = { 0, 0 };
		TxSendMessage(EM_POSFROMCHAR, (WPARAM)&point, nChar);
		return point;
	}
	//__AutomationMethod(IDispatch*, posFromChar)(LONG nChar)
	//{
	//	COlePoint* pp = NULL;
	//	IDispatch* disp = NULL;
	//	pp->CreateInstance(&disp, &pp, TRUE);
	//	if (pp)
	//	{
	//		POINT pt = PosFromChar(nChar);
	//		pp->x = pt.x;
	//		pp->y = pt.y;
	//	}
	//	return disp;
	//}

	int CharFromPos(POINT pt)
	{
		ATLASSERT(m_ts);
		POINTL ptl = { pt.x, pt.y };
		return (int)TxSendMessage(EM_CHARFROMPOS, 0, (LPARAM)&ptl);
	}
	//__AutomationMethod(long, charFromPos)(long x, long y)
	//{
	//	POINT pt = {x,y};
	//	return (long)CharFromPos(pt);
	//}

	void EmptyUndoBuffer()
	{
		ATLASSERT(m_ts);
		TxSendMessage(EM_EMPTYUNDOBUFFER, 0, 0L);
	}
	//__AutomationMethod(void, emptyUndoBuffer)(){ EmptyUndoBuffer(); }

	int LineIndex(int nLine = -1)
	{
		ATLASSERT(m_ts);
		return (int)TxSendMessage(EM_LINEINDEX, nLine, 0L);
	}
	//__AutomationMethod(long, lineIndex)(long lLine=-1) { return (long)LineIndex(lLine); }

	int LineLength(int nLine = -1)
	{
		ATLASSERT(m_ts);
		return (int)TxSendMessage(EM_LINELENGTH, nLine, 0L);
	}
	//__AutomationMethod(long, lineLength)(long lLine=-1) { return (long)LineLength(lLine); }

	BOOL LineScroll(int nLines, int nChars = 0)
	{
		ATLASSERT(m_ts);
		return (BOOL)TxSendMessage(EM_LINESCROLL, nChars, nLines);
	}
	//__AutomationMethod(bool, lineScroll)(long lLines, long lChars=0) { return !!LineScroll(lLines, lChars); }

	void ReplaceSel(LPCTSTR lpszNewText, BOOL bCanUndo = FALSE)
	{
		ATLASSERT(m_ts);
		TxSendMessage(EM_REPLACESEL, (WPARAM) bCanUndo, (LPARAM)lpszNewText);
	}
	//__AutomationMethod(void, replaceSel)(BSTR newText, bool canUndo=false) { ReplaceSel(newText, !!canUndo); }

	void SetRect(LPCRECT lpRect)
	{
		ATLASSERT(m_ts);
		TxSendMessage(EM_SETRECT, 0, (LPARAM)lpRect);
	}
	//__AutomationMethod(void, setRect)(long left, long top, long right, long bottom) { RECT rc={left,top,right,bottom}; SetRect(&rc); }

	BOOL DisplayBand(LPRECT pDisplayRect)
	{
		ATLASSERT(m_ts);
		return (BOOL)TxSendMessage(EM_DISPLAYBAND, 0, (LPARAM)pDisplayRect);
	}

	LONG FindText(DWORD dwFlags, FINDTEXT& ft)
	{
		ATLASSERT(m_ts);
#if (_RICHEDIT_VER >= 0x0200) && defined(_UNICODE)
		return (LONG)TxSendMessage(EM_FINDTEXTW, dwFlags, (LPARAM)&ft);
#else
		return (LONG)TxSendMessage(EM_FINDTEXT, dwFlags, (LPARAM)&ft);
#endif
	}

	LONG FindText(DWORD dwFlags, FINDTEXTEX& ft)
	{
		ATLASSERT(m_ts);
#if (_RICHEDIT_VER >= 0x0200) && defined(_UNICODE)
		return (LONG)TxSendMessage(EM_FINDTEXTEXW, dwFlags, (LPARAM)&ft);
#else
		return (LONG)TxSendMessage(EM_FINDTEXTEX, dwFlags, (LPARAM)&ft);
#endif
	}

	LONG FormatRange(FORMATRANGE& fr, BOOL bDisplay = TRUE)
	{
		ATLASSERT(m_ts);
		return (LONG)TxSendMessage(EM_FORMATRANGE, bDisplay, (LPARAM)&fr);
	}

	LONG FormatRange(FORMATRANGE* pFormatRange, BOOL bDisplay = TRUE)
	{
		ATLASSERT(m_ts);
		return (LONG)TxSendMessage(EM_FORMATRANGE, bDisplay, (LPARAM)pFormatRange);
	}

	void HideSelection(BOOL bHide = TRUE, BOOL bChangeStyle = FALSE)
	{
		ATLASSERT(m_ts);
		TxSendMessage(EM_HIDESELECTION, bHide, bChangeStyle);
	}

	void PasteSpecial(UINT uClipFormat, DWORD dwAspect = 0, HMETAFILE hMF = 0)
	{
		ATLASSERT(m_ts);
		REPASTESPECIAL reps = { dwAspect, (DWORD_PTR)hMF };
		TxSendMessage(EM_PASTESPECIAL, uClipFormat, (LPARAM)&reps);
	}

	void RequestResize()
	{
		ATLASSERT(m_ts);
		TxSendMessage(EM_REQUESTRESIZE, 0, 0L);
	}

	LONG StreamIn(UINT uFormat, EDITSTREAM& es)
	{
		ATLASSERT(m_ts);
		return (LONG)TxSendMessage(EM_STREAMIN, uFormat, (LPARAM)&es);
	}

	LONG StreamOut(UINT uFormat, EDITSTREAM& es)
	{
		ATLASSERT(m_ts);
		return (LONG)TxSendMessage(EM_STREAMOUT, uFormat, (LPARAM)&es);
	}

	DWORD FindWordBreak(int nCode, LONG nStartChar)
	{
		ATLASSERT(m_ts);
		return (DWORD)TxSendMessage(EM_FINDWORDBREAK, nCode, nStartChar);
	}

	// Additional operations
	void ScrollCaret()
	{
		ATLASSERT(m_ts);
		TxSendMessage(EM_SCROLLCARET, 0, 0L);
	}
	//__AutomationMethod(void, scrollCaret)() { ScrollCaret(); }

	int InsertText(long nInsertAfterChar, LPCTSTR lpstrText, BOOL bCanUndo = FALSE)
	{
		int nRet = SetSel(nInsertAfterChar, nInsertAfterChar);
		ReplaceSel(lpstrText, bCanUndo);
		return nRet;
	}
	//__AutomationMethod(long, insertText)(long nAfterChar, BSTR text, bool bCanUndo=false) { return InsertText(nAfterChar,text,!!bCanUndo); }

	int AppendText(LPCTSTR lpstrText, BOOL bCanUndo = FALSE)
	{
		return InsertText(GetTextLength(), lpstrText, bCanUndo);
	}
	//__AutomationMethod(long, appendText)(BSTR text, bool bCanUndo=false)
	//{
	//	return (long)AppendText(text, !!bCanUndo);
	//}

	// Clipboard operations
	BOOL Undo()
	{
		ATLASSERT(m_ts);
		return (BOOL)TxSendMessage(EM_UNDO, 0, 0L);
	}
	//__AutomationMethod(bool, undo)() { return TRUE==Undo(); }

	void Clear()
	{
		ATLASSERT(m_ts);
		TxSendMessage(WM_CLEAR, 0, 0L);
	}
	//__AutomationMethod(void, clear)() { Clear(); }

	void Copy()
	{
		ATLASSERT(m_ts);
		TxSendMessage(WM_COPY, 0, 0L);
	}
	//__AutomationMethod(void, copy)() { Copy(); }

	void Cut()
	{
		ATLASSERT(m_ts);
		TxSendMessage(WM_CUT, 0, 0L);
	}
	//__AutomationMethod(void, cut)() { Cut(); }

	void Paste()
	{
		ATLASSERT(m_ts);
		TxSendMessage(WM_PASTE, 0, 0L);
	}
	//__AutomationMethod(void, paste)() { Paste(); }

	// OLE support
	IRichEditOle* GetOleInterface()
	{
		ATLASSERT(m_ts);
		IRichEditOle *pRichEditOle = NULL;
		TxSendMessage(EM_GETOLEINTERFACE, 0, (LPARAM)&pRichEditOle);
		return pRichEditOle;
	}

	BOOL SetOleCallback(IRichEditOleCallback* pCallback)
	{
		ATLASSERT(m_ts);
		return (BOOL)TxSendMessage(EM_SETOLECALLBACK, 0, (LPARAM)pCallback);
	}

#if (_RICHEDIT_VER >= 0x0200)
	BOOL Redo()
	{
		ATLASSERT(m_ts);
		return (BOOL)TxSendMessage(EM_REDO, 0, 0L);
	}
	//__AutomationMethod(bool, redo)() { return TRUE==Redo(); }

	void StopGroupTyping()
	{
		ATLASSERT(m_ts);
		TxSendMessage(EM_STOPGROUPTYPING, 0, 0L);
	}

	void ShowScrollBar(int nBarType, BOOL bVisible = TRUE)
	{
		ATLASSERT(m_ts);
		TxSendMessage(EM_SHOWSCROLLBAR, nBarType, bVisible);
	}
#endif // (_RICHEDIT_VER >= 0x0200)

#if (_RICHEDIT_VER >= 0x0300)
	BOOL SetTabStops(int nTabStops, LPINT rgTabStops)
	{
		ATLASSERT(m_ts);
		return (BOOL)TxSendMessage(EM_SETTABSTOPS, nTabStops, (LPARAM)rgTabStops);
	}

	BOOL SetTabStops()
	{
		ATLASSERT(m_ts);
		return (BOOL)TxSendMessage(EM_SETTABSTOPS, 0, 0L);
	}

	BOOL SetTabStops(const int& cxEachStop)    // takes an 'int'
	{
		ATLASSERT(m_ts);
		return (BOOL)TxSendMessage(EM_SETTABSTOPS, 1, (LPARAM)(LPINT)&cxEachStop);
	}
	//__AutomationPut_Begin(tabstops)
	//{
	//	if (vProp.vt==VT_EMPTY || FAILED(vProp.ChangeType(VT_I4)))
	//		return E_INVALIDARG;
	//	int cx = vProp.lVal;
	//	SetTabStops(cx);
	//} __AutomationPut_End()
#endif // (_RICHEDIT_VER >= 0x0300)

	//__AutomationMethod(bool, test)(BSTR input, bool opt1=false, long opt2=34)
	//{
	//	LineLog(L">>>[TEST]<<< input:%s, opt1:%d, opt2:%d", input, opt1, opt2);
	//	return true;
	//}

		
	Begin_Disp_Map(CRichEditHelper)
		//Disp_PropertyGet(1000,lineCount)
		//Disp_Property(1010, isDirty)
		//Disp_PropertyGet(1020, options)
		//Disp_PropertyGet(1030, canUndo)
		//Disp_PropertyGet(1040, canPaste)
		//Disp_PropertyGet(1050, selMin)
		//Disp_PropertyGet(1060, selMax)
		//Disp_PropertyGet(1070, selection)
		//Disp_PropertyGet(1080, length)
		//Disp_PropertyPut(1090, readonly)
		//Disp_PropertyGet(1100, firstVisibleLine)
		//Disp_PropertyGet(1110, undoName)
		//Disp_PropertyGet(1120, redoName)
		//Disp_PropertyGet(1130, canRedo)
		//Disp_Property(1140, autoUrl)
		//Disp_PropertyPut(1150, undoLimit)
		Disp_Property(1160, text)
		//Disp_PropertyGet(1170, rect)
		//Disp_Property(1180, defaultCharFormat)
		//Disp_Property(1190, selectionCharFormat)
		//Disp_PropertyPut(1200, wordCharFormat)
		//Disp_Property(1210, paraFormat)
		//Disp_PropertyGet(1220, selectionType)
		//Disp_PropertyPut(1230, backgroundColor)
		//Disp_Property(1240, scrollPos)
		//Disp_PropertyPut(1250, limitText)

		//Disp_PropertyPut(1500,tabstops)

		//Disp_Method(4000, getLine, LPCOLESTR, 1, long)
		//Disp_Method(4010, select, void, 2, long, long)
		//Disp_Method(4020, selectAll, void, 0)
		//Disp_Method(4030, unselect, void, 0)
		//Disp_Method(4040, textRange, LPCOLESTR, 2, long, long)

		//Disp_Method(5440,redo,bool,0)
		//Disp_Method_With_Option(5450, appendText, long, 2, Params(BSTR), ParamsOpt1(bool), ParamsOptDefValue(false))
		//Disp_Method_With_Option(5460, setCharFormat, bool, 2, Params(IDispatch*), ParamsOpt1(WORD), ParamsOptDefValue(SCF_SELECTION))
		//Disp_Method(5470, lineFromChar, long, 1, long)
		//Disp_Method(5480, posFromChar, IDispatch*, 1, long)
		//Disp_Method(5490, charFromPos, long, 2, long, long)
		//Disp_Method(5500, emptyUndoBuffer, void, 0)
		//Disp_Method_With_Option(5510, lineIndex, long, 1, Params0(), ParamsOpt1(long), ParamsOptDefValue(-1))
		//Disp_Method_With_Option(5520, lineLength, long, 1, Params0(), ParamsOpt1(long), ParamsOptDefValue(-1))
		//Disp_Method_With_Option(5530, lineScroll, bool, 2, Params(long), ParamsOpt1(long), ParamsOptDefValue(0))
		//Disp_Method_With_Option(5540, replaceSel, void, 2, Params(BSTR), ParamsOpt1(bool), ParamsOptDefValue(false))
		//Disp_Method(5550, setRect, void, 4, long, long, long, long)
	End_Disp_Map()

};

#pragma warning(pop)

#endif //__DUIEDITHELPER_H__