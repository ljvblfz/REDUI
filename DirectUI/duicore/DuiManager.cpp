
#include "StdAfx.h"
#include <CommCtrl.h>
#include "DuiManager.h"
//#include "../duilic/DuiLic.h"
#include "../../common/dispatchimpl.h"
#include "DuiPluginsManager.h"
#include "DuiAlpha.h"
#include "DuiHelper.h"
#include "../../common/theme.h"
#include "DuiActiveX.h"
#include "DuiEdit.h"

#define __Async_Download__
#include "../../common/mimefilter.h"

#include <zmouse.h>


#pragma comment(lib, "comctl32.lib")



//////////////////////////////////////////////////////////////////////////
//
#define DELEGATE_TO_OWNER(proc, para) if (__owner_ctrl) return __owner_ctrl->m_pLayoutMgr->proc##para;

//////////////////////////////////////////////////////////////////////////
//
DuiStyleVariant* WINAPI StyleVariantCreate()
{
	return NEW CDuiStyleVariant;
}

BOOL WINAPI StyleVariantInit(DuiStyleVariant* pVar)
{
	if (pVar==NULL) return FALSE;
	pVar->sat = SAT__UNKNOWN;
	pVar->svt = SVT_EMPTY;
	pVar->boolVal = FALSE;
	return TRUE;
}

BOOL WINAPI StyleVariantClear(DuiStyleVariant* pVar)
{
	if (pVar == NULL) return FALSE;
	switch (pVar->svt & SVT_MASK_ALL)
	{
	case SVT_BITMAP:	if (pVar->bitmapVal) DeleteObject(pVar->bitmapVal); break;
	case SVT_PEN:		if (pVar->penVal) delete pVar->penVal; break;
	case SVT_BRUSH:		if (pVar->brushVal) DeleteObject(pVar->brushVal); break;
	case SVT_ICON:		if (pVar->iconVal) DestroyIcon(pVar->iconVal); break;
	case SVT_CURSOR:	/*if (pVar->cursorVal) DestroyCursor(pVar->cursorVal);*/ break; // cursor 是共享的，无需释放
	case SVT_FONT:		if (pVar->fontVal) delete pVar->fontVal; break;
	case SVT_STRING:
	case SVT_EXPRESSION:if (pVar->strVal) delete[] pVar->strVal; break;
	case SVT_RECT:		if (pVar->rectVal) delete pVar->rectVal; break;
	case SVT_SIZE:		if (pVar->sizeVal) delete pVar->sizeVal; break;
	case SVT_POINT:		if (pVar->pointVal) delete pVar->pointVal; break;
	}

	pVar->svt = SVT_EMPTY;
	pVar->boolVal = FALSE;
	pVar->sat = SAT__UNKNOWN;
	return FALSE;
}

HPEN WINAPI StyleVariantGetPen(DuiStyleVariant* pVar, LPLOGPEN pRefPen)
{
	if (pVar==NULL) return NULL;
	return FromStyleVariant(pVar, HPEN, pRefPen);
}

void WINAPI StyleVariantSetPen(DuiStyleVariant* pVal, LPLOGPEN pRefPen, LONG mask)
{
	if (pVal==NULL) return;
	((CDuiStyleVariant*)pVal)->SetPen(pRefPen, mask);
}

HFONT WINAPI StyleVariantGetFont(DuiStyleVariant* pVar, LPLOGFONTW pRefFont)
{
	if (pVar==NULL) return NULL;
	return FromStyleVariant(pVar, HFONT, pRefFont);
}

void WINAPI StyleVariantSetFont(DuiStyleVariant* pVar, LPLOGFONTW pRefFont, LONG mask)
{
	if (pVar==NULL) return;
	((CDuiStyleVariant*)pVar)->SetFont(pRefFont, mask);
}

BOOL WINAPI StyleVariantGetLogBrush(DuiStyleVariant* pVar, LPLOGBRUSH pLogBrush)
{
	if (pVar==NULL) return FALSE;
	return ((CDuiStyleVariant*)pVar)->GetLogBrush(pLogBrush);
}

BOOL WINAPI StyleVariantGetBitmap(DuiStyleVariant* pVar, LPBITMAP pBitmap)
{
	if (pVar==NULL) return FALSE;
	return ((CDuiStyleVariant*)pVar)->GetBitmap(pBitmap);
}

BOOL WINAPI StyleVariantIsEmpty(DuiStyleVariant* pVar)
{
	if (pVar==NULL) return TRUE;
	return ((CDuiStyleVariant*)pVar)->IsEmpty();
}

LPVOID WINAPI StyleVariantGetExternal(DuiStyleVariant* pVar)
{
	if (pVar==NULL) return NULL;
	return ((CDuiStyleVariant*)pVar)->GetExternal();
}



/////////////////////////////////////////////////////////////////////////////////////
//
//

static UINT GetNameHash(LPCTSTR pstrName)
{
	UINT i = 0;
	SIZE_T len = _tcslen(pstrName);
	while( len-- > 0 ) i = (i << 5) + i + pstrName[len];
	return i;
}

static UINT MapKeyState()
{
	UINT uState = 0;
	if( ::GetKeyState(VK_CONTROL) < 0 ) uState |= MK_CONTROL;
	if( ::GetKeyState(VK_RBUTTON) < 0 ) uState |= MK_LBUTTON;
	if( ::GetKeyState(VK_LBUTTON) < 0 ) uState |= MK_RBUTTON;
	if( ::GetKeyState(VK_SHIFT) < 0 ) uState |= MK_SHIFT;
	if( ::GetKeyState(VK_MENU) < 0 ) uState |= MK_ALT;
	return uState;
}


/////////////////////////////////////////////////////////////////////////////////////
//

typedef struct tagFINDTABINFO
{
	DuiNode* pFocus;
	DuiNode* pLast;
	BOOL bForward;
	BOOL bNextIsIt;
} FINDTABINFO;

typedef struct tagFINDSHORTCUT
{
	TCHAR ch;
	BOOL bPickNext;
} FINDSHORTCUT;

typedef struct tagTIMERINFO
{
	DuiNode* pSender;
	UINT nLocalID;
	HWND hWnd;
	UINT uWinTimer;
} TIMERINFO;


/////////////////////////////////////////////////////////////////////////////////////
//

//CAnimationSpooler m_anim;


/////////////////////////////////////////////////////////////////////////////////////


CStdPtrArray CDuiLayoutManager::m_aPreMessages;
DuiSystemSettings CDuiLayoutManager::m_SystemConfig = {0};
DuiSystemMetrics CDuiLayoutManager::m_SystemMetrics = {0};

#define fire_unload(x) {DuiEvent event={event_type_to_name(DUIET_unload),DuiHandle<DuiNode>(x),::GetTickCount(),{0,0},0,0,NULL,0,0,TRUE}; (x)->m_pLayoutMgr->FireEvent(x, event);}
void fire_control_unload_events(DuiNode* pCtrl, BOOL bIncludeChildren=TRUE, BOOL bIncludeSelf=TRUE)
{
	if (pCtrl==NULL) return;
	if (bIncludeSelf)
		fire_unload(pCtrl);
	if (bIncludeChildren)
	{
		for (int i=0; i<pCtrl->GetChildCount(); i++)
			fire_control_unload_events(pCtrl->GetChildControl(i));
	}
}

AttrValue::~AttrValue() { if (props) delete props; props=NULL; }


/////////////////////////////////////////////////////////////////////////////////////
//
//

LPCOLESTR g_window_name = L"window";
UINT guMsgMouseWheel = ::RegisterWindowMessage(MSH_MOUSEWHEEL);

CDuiLayoutManager::CDuiLayoutManager() :
	__owner_ctrl(NULL),
	m_AxContainer(this),
	m_hWndPaint(NULL),
	__hwndTop(NULL),
	m_hbmpOffscreen(NULL),
#ifdef GDI_ONLY
	m_hDcPaint(NULL),
	m_hDcOffscreen(NULL),
#else
	m_pSurface(NULL),
	m_pSurfaceOffscreen(NULL),
	_primarySurface(NULL),
	_offscrnSurface(NULL),
	_gdiSurface(NULL),
	_offscrnGdiSurface(NULL),
#endif // GDI_ONLY

#ifndef NO3D
	m_pDevice(NULL),
	m_bD3DEnabled(FALSE),
#endif

	m_hwndTooltip(NULL),
	m_uTimerID(0x1000),
	m_pRoot(NULL),
	m_pFocus(NULL),
	m_pEventHover(NULL),
	m_pEventCaptured(NULL),
	m_pEventKey(NULL),
	m_fixedChildren(NULL),
	m_bFirstLayout(TRUE),
	m_bFocusNeeded(FALSE),
	m_bResizeNeeded(FALSE),
	m_bMouseTracking(FALSE),
	m_bOffscreenPaint(TRUE),
	m_bDestroying(FALSE),
	m_lPaintLocked(0),
	m_bInvalid(TRUE),
	m_bAlphaEnabled(FALSE),
	m_pBits(NULL),
	m_bDblClick(FALSE), m_pClick(NULL),

	m_Images(NULL),

	//m_hScript(NULL),
	m_pEvent(NULL),
	m_pObjectCreator(NULL)
{
	m_szMinWindow.cx = 140;
	m_szMinWindow.cy = 200;
	m_ptLastMousePos.x = m_ptLastMousePos.y = -1;

	m_hInstance = NULL;
	m_hLangInst = NULL;

	static BOOL bFirst = TRUE;
	if (bFirst)
	{
		bFirst = FALSE;
		// System Config
		m_SystemConfig.bShowKeyboardCues = FALSE; // 设置 TRUE 时还有BUG
		// System Metrics
		m_SystemMetrics.cxvscroll = (INT) ::GetSystemMetrics(SM_CXVSCROLL);
	}

	// 设置内容为纯文本的标签
	m_xml.AddConstTextTag(L"script");
	m_xml.AddConstTextTag(L"pre");
	m_xml.AddConstTextTag(L"style");
	m_xml.AddConstTextTag(L"stylesheet");
	m_xml.AddConstTextTag(L"css");
	m_xml.AddConstTextTag(L"template");
	m_xml.AddConstTextTag(L"effect"); // 设置 effect 标签的内容为纯文本

	// 设置无需关闭标签的标签
	m_xml.AddInlineTag(L"WhiteSpace"); m_xml.AddInlineTag(L"w");
	m_xml.AddInlineTag(L"LineBreak"); m_xml.AddInlineTag(L"br");
	m_xml.AddInlineTag(L"HorizontalLine"); m_xml.AddInlineTag(L"hr");
	m_xml.AddInlineTag(L"Img");

	AddMessageFilter(&m_AxContainer);
}


#define DeleteSurface(surface) if (surface) surface = (/*delete*/ surface->Delete(), NULL);

CDuiLayoutManager::~CDuiLayoutManager()
{
	m_bDestroying = TRUE;
	gConnector.Disconnect(NULL, this);
	//if (m_pRoot)
	//{
	//	m_pRoot->RemoveAllChildren();
	//	DuiEvent event={DUIET_unload,m_pRoot,::GetTickCount(),{0,0},0,0,0,0,TRUE};
	//	m_pRoot->OnEvent(event);
	//}
	if (m_pObjectCreator) m_pObjectCreator->OnLayoutEnd();
	fire_control_unload_events(m_pRoot);
	if (m_script.IsValid()) m_script.Close();
	// Delete the control-tree structures
	int i;
	for( i = 0; i < m_aDelayedCleanup.GetSize(); i++ ) static_cast<DuiNode*>(m_aDelayedCleanup[i])->DeleteThis();
	m_aDelayedCleanup.RemoveAll();
	if (m_pRoot) m_pRoot->DeleteThis();
	m_pRoot = NULL;

	// Release other collections
	for( i = 0; i < m_aTimers.GetSize(); i++ ) delete static_cast<TIMERINFO*>(m_aTimers[i]);
	m_aTimers.RemoveAll();
	// Reset other parts...
	if( m_hwndTooltip != NULL ) ::DestroyWindow(m_hwndTooltip);
#ifdef GDI_ONLY
	if( m_hDcOffscreen != NULL ) ::DeleteDC(m_hDcOffscreen);
	if( m_hDcPaint != NULL ) ::ReleaseDC(m_hWndPaint, m_hDcPaint);
#else
	DeleteSurface(_offscrnSurface);
	DeleteSurface(_primarySurface);
	DeleteSurface(_offscrnGdiSurface);
	DeleteSurface(_gdiSurface);
//#undef DeleteSurface
	m_pSurface = NULL;
	m_pSurfaceOffscreen = NULL;
#endif // GDI_ONLY
	if( m_hbmpOffscreen != NULL ) ::DeleteObject(m_hbmpOffscreen);

	RemoveMessageFilter(&m_AxContainer);
	m_aPreMessages.Remove((this));
	// Delete style
	for (i=0; i<m_aStyles.GetSize(); i++) delete static_cast<CDuiStyle*>(m_aStyles[i]);
	m_aStyles.RemoveAll();
	// Delete Resources
	//for (i=0; i<m_aResources.GetSize(); i++) delete static_cast<CDuiImageResource*>(m_aResources[i]);
	if (m_Images) m_Images = (delete m_Images, NULL);
}

void CDuiLayoutManager::Init(HWND hWnd, CDuiObjectCreator* pCreator/*=NULL*/, DuiNode* owner/*=NULL*/)
{
	ASSERT(::IsWindow(hWnd));
	__owner_ctrl = owner;
	// Remember the window context we came from
	m_hWndPaint = hWnd;
	__hwndTop = CWindow(hWnd).GetTopLevelWindow().m_hWnd;
#ifndef NO3D
	m_pDevice = (Device*)::GetPropW(m_hWndPaint, AtomString(atom_device));
#endif

#ifdef GDI_ONLY
	m_hDcPaint = ::GetDC(hWnd);
	::SelectObject(m_hDcPaint, (HFONT)::GetStockObject(DEFAULT_GUI_FONT));
#else
#ifndef NO3D
	//if (m_pDevice == NULL)
#endif
	{
		// Init Surface, first try ddsurface, then gdi
		_primarySurface = NEW CPrimarySurface(hWnd);
		if (_primarySurface)
		{
			if ((IDirectDrawSurface*)(*_primarySurface) == NULL)
			{
				DeleteSurface(_primarySurface);
			}
			else
			{
				m_pSurface = _primarySurface;
			}
		}
		if (_primarySurface == NULL)
		{
			_gdiSurface = NEW CGdiSurface(hWnd);
			ATLASSERT(_gdiSurface);
			m_pSurface = _gdiSurface;
		}
	}
#endif // GDI_ONLY

	// We'll want to filter messages globally too
	m_aPreMessages.Add(this);
	m_pObjectCreator = pCreator;
	if (__owner_ctrl==NULL) InitCommonStyles();

	if (pCreator)
	{
		IDispatch* disp = pCreator->GetWindowObject();
		if (disp)
		{
			if (!m_script.IsValid())
				m_script.CreateJScript(TRUE);
			m_script.SetHostHwnd(m_hWndPaint);
			m_script.AddNamedUnknown(g_window_name, disp);
			m_script.SetActiveNamedItem(g_window_name);
			m_script.Execute(L"", TRUE); // 确保脚本处于运行状态
#ifndef NO3D
			if (m_pDevice) m_pDevice->SetScriptObject(m_script);
#endif
		}
	}
}

BOOL CDuiLayoutManager::LoadFromXml(LPCOLESTR lpszXml)
{
	//test_get();
	//test_set();
	if (lpszXml==NULL || *lpszXml==0) return FALSE;
	if (m_pObjectCreator==NULL) return FALSE;

	m_strError.Empty();

	m_pObjectCreator->OnLayoutBegin();
	if (!m_xml.Load(lpszXml))
	{
		m_pObjectCreator->OnLayoutError();
		m_strError = L"XML Parse failed! \r\n\r\nMessage: ";
		m_strError.Append(m_xml.GetLastErrorMessage());
		m_strError.Append(L"\r\nLocation:\r\n\r\n");
		m_strError.Append(m_xml.GetLastErrorLocation());
		m_strError.Append(L"\r\n...");
		m_strError.Append(L"\r\n\r\nPlease visit www.huyuhan.net to find help.");
		m_strError.Append(L"\r\n请访问 www.huyuhan.net 网站以寻求帮助。");
		Invalidate(/*GetClientRect()*/);
		return ATLTRACE(L"XML Parse failed! Message:%s  Position=%s\n", m_xml.GetLastErrorMessage(), m_xml.GetLastErrorLocation()), FALSE;
	}

	CMarkupNode root = m_xml.GetRoot();
	if (!root.IsValid()) root = m_xml.GetRoot(FALSE); // 没有正常标签则取第一个标签（应该是文本块）
	if (!root.IsValid())
		return m_pObjectCreator->OnLayoutError(), FALSE;

	_ParseConfig(&root);
	_ParseResources(&root);

	CMarkupNode lyt;
	if (root.IsTextBlock())
	{
		lyt = root;
	}
	else
	{
		/*CMarkupNode*/ lyt = root.GetChild();
		while (lyt.IsValid() && lyt.GetName() && (lstrcmpiW(lyt.GetName(), L"Resources")==0 || lstrcmpiW(lyt.GetName(), L"Resource")==0 || lstrcmpiW(lyt.GetName(), L"Config")==0)) lyt = lyt.GetSibling();
		if (!lyt.IsValid())
		{
			// 没有正常标签则取文本块
			lyt = root.GetChild(FALSE);
			while (lyt.IsValid() && lyt.GetName() && (lstrcmpiW(lyt.GetName(), L"Resources")==0 || lstrcmpiW(lyt.GetName(), L"Resource")==0 || lstrcmpiW(lyt.GetName(), L"Config")==0)) lyt = lyt.GetSibling(FALSE);
		}
		if (!lyt.IsValid())
		{
			m_pObjectCreator->OnLayoutError();
			m_strError = L"NOTHING WILL BE RENDERED!";
			m_strError.Append(L"\r\n\r\nPlease visit www.huyuhan.net to find help.");
			m_strError.Append(L"\r\n请访问 www.huyuhan.net 网站以寻求帮助。");
			Invalidate(/*GetClientRect()*/);
			return FALSE;
		}
	}

	//if (psIsValidHandle(m_hScript)) psScriptExecute(m_hScript, L"", g_window_name, TRUE); // 确保脚本处于运行状态
	//if (m_script.IsValid()) m_script.Execute(L"", g_window_name, TRUE); // 确保脚本处于运行状态
	m_lPaintLocked++;
	DuiNode* pRoot = _AddChild(&lyt);
	ATLTRACE(L"Layout parse result: %s!\n", pRoot?L"succeed":L"failed");
	if (pRoot)
	{
		(pRoot)->SetReadyState(READYSTATE_INTERACTIVE); // 递归处理属性改变通知
		(pRoot)->SetReadyState(READYSTATE_COMPLETE); // 递归发送onload事件
	}
	m_lPaintLocked--;
	if (pRoot)
	{
		SetNextTabControl();
		UpdateLayout(TRUE);
		m_pObjectCreator->OnLayoutReady();
#ifndef NO3D
		if (Get3DDevice()) m_pDevice->Reset();
#endif
	}
	return pRoot!=NULL;
// 	if (pRoot==NULL)
// 		return FALSE;
// 	return AttachControl(pRoot);
}

void CDuiLayoutManager::_ParseConfig(CMarkupNode* pRoot)
{
	m_aAttrs.RemoveAll();
	for (int i=0; i<pRoot->GetAttributeCount(); i++)
	{
		m_aAttrs.SetAttribute(pRoot->GetAttributeName(i), pRoot->GetAttributeValue(i));
	}
	CMarkupNode cfg = pRoot->GetChild(L"Config");
	if (cfg.IsValid())
	{
		for (int i=0; i<cfg.GetAttributeCount(); i++)
		{
			m_aAttrs.SetAttribute(cfg.GetAttributeName(i), cfg.GetAttributeValue(i));
		}
	}

#ifdef _DEBUG
	if (GetAttributeBool(L"debugger", FALSE))
		DebugBreak();
#endif // _DEBUG

	// set window title
	LPCOLESTR szTitle = m_aAttrs.GetAttributeString(L"title");
	::SetWindowTextW(GetPaintWindow(), szTitle ? szTitle : L"");

	// set window position
	LPCOLESTR szPos = m_aAttrs.GetAttributeString(L"position");
	RECT rc;
	if (StyleParseRect(NULL,NULL,szPos,rc,TRUE))
		::MoveWindow(GetPaintWindow(), rc.left, rc.top, rc.right-rc.left, rc.bottom-rc.top, FALSE);

	// 处理baseurl
	LPOLESTR pUrl = (LPOLESTR)m_aAttrs.GetAttributeString(L"baseUrl");
	if (pUrl == NULL)
	{
		WCHAR path[MAX_PATH+16] = L"file:///";
		pUrl = path + lstrlenW(path);
		::GetModuleFileNameW(NULL, pUrl, MAX_PATH);
		while (*pUrl)
		{
			if (*pUrl==L'\\') *pUrl=L'/';
			pUrl++;
		}
		m_aAttrs.SetAttribute(L"baseUrl", path);
	}

#ifndef NO3D
	// process D3D flag
	m_bD3DEnabled = GetAttributeBool(L"d3d",
#ifdef _DEBUG
		TRUE
#else
		TRUE
#endif // _DEBUG
		);
#endif

	// 处理透明
	m_bAlphaEnabled = GetAttributeBool(L"transparent");
	if (m_bAlphaEnabled)
	{
		::SetWindowLongW(m_hWndPaint, GWL_STYLE, (::GetWindowLongW(m_hWndPaint, GWL_STYLE) & ~(WS_OVERLAPPEDWINDOW)));
		::SetWindowLongW(m_hWndPaint, GWL_EXSTYLE, (::GetWindowLongW(m_hWndPaint, GWL_EXSTYLE) | WS_EX_LAYERED) & ~(WS_EX_APPWINDOW | WS_EX_WINDOWEDGE));
	}
	else
	{
		::SetWindowLongW(m_hWndPaint, GWL_EXSTYLE, (::GetWindowLongW(m_hWndPaint, GWL_EXSTYLE) & ~WS_EX_LAYERED));
	}
#ifndef NO3D
	if (Get3DDevice()) m_pDevice->Reset();
#endif
}

BOOL CDuiLayoutManager::_IsImageResource(CMarkupNode& r)
{
	if (lstrcmpiW(r.GetName(), L"Image") == 0) // 位图资源
	{
		LPCOLESTR id = r.GetAttributeValue(L"id");
		LPCOLESTR url = r.GetAttributeValue(L"src");
		if (url && *url!=0)
		{
			CStdString tempid;
			if (id==NULL)
			{
				LPCOLESTR p = url + lstrlenW(url);
				while (p>=url && *p!=L'/' && *p!=L'\\') p--;
				CStdString str = p+1;
				int nStart = 0;
				tempid = str.Tokenize(L".\0", nStart);
				id = tempid;
			}
			CDuiImageResource* pImg = CreateImageResource(url, id);
			LPCOLESTR clip = r.GetAttributeValue(L"clip");
			RECT rcClip;
			if (pImg && clip && StyleParseRect(NULL, NULL, clip, rcClip))
				pImg->SetClipRect(rcClip);
		}
		return TRUE;
	}
	return FALSE;
}

BOOL CDuiLayoutManager::_IsStyleResource(CMarkupNode& r)
{
	if (lstrcmpiW(r.GetName(), L"Style") == 0) // 样式表
	{
		LPCOLESTR id = r.GetAttributeValue(L"id");
		LPCOLESTR tname = r.GetAttributeValue(L"TargetName");
		LPCOLESTR tid = r.GetAttributeValue(L"TargetId");
		DWORD state = TryLoadStateFromString(r.GetAttributeValue(L"TargetState"));
		LPCOLESTR refid = r.GetAttributeValue(L"Refer");
		CDuiStyle* pref = GetStyleById(refid);
		CDuiStyle* pStyle = CreateStyle(tname, tid, state, NULL, pref);
		if (pStyle)
		{
			if(id) pStyle->SetId(id);
			CMarkupNode cont_node = r.GetChild(FALSE);
			while (cont_node.IsValid() && !cont_node.IsTextBlock()) cont_node = cont_node.GetSibling(FALSE);
			if (cont_node.IsTextBlock())
			{
				LPCOLESTR cont = cont_node.GetValue();
				pStyle->ParseStyle(cont);
			}
		}
		return TRUE;
	}
	else if ((lstrcmpiW(r.GetName(), L"StyleSheet")==0 ||
			lstrcmpiW(r.GetName(), L"css")==0) &&
			m_script.IsValid()) // CSS 类型的样式表
	{
		CStringW css;

		// 首先读取外部资源
		LPCOLESTR url = r.GetAttributeValue(L"src");
		if (url)
		{
			CComPtr<IStream> spStream;
			CDownload::LoadUrlToStream(url, &spStream, GetResourceInstance());
			if (spStream)
			{
				CComBSTR bstr;
				CDownload::StreamToBSTR(spStream, bstr);

				css = bstr;
			}
		}
		// 读取内部内容
		CMarkupNode cont_node = r.GetChild(FALSE);
		if (cont_node.IsValid() && cont_node.IsTextBlock())
		{
			css += cont_node.GetValue();
		}

		// 开始解析 CSS 串
		/*
		 *	CSS 语法：
		 *		* { ... }
		 *		[targetName] [# targetId] [: targetState] [= id] [+ refid] { ... }
		 *		
		 *		行注释符和块注释符跟C++相同
		 */
		CComVariant vRet = m_script.CallFunction(L"parseCss", (LPCOLESTR)css);
		if (vRet.vt==VT_DISPATCH)
		{
			CComDispatchDriver dispRet = vRet.pdispVal;
			CComVariant vLen;
			HRESULT hr;
			hr = dispRet.GetPropertyByName(L"length", &vLen);
			if (SUCCEEDED(hr))
			{
				for (long i=0; i<vLen.lVal; i++)
				{
					CStringW strIndex;
					strIndex.Format(L"%d", i);
					CComVariant vItem;
					hr = dispRet.GetPropertyByName(strIndex, &vItem);
					if (SUCCEEDED(hr) && vItem.vt==VT_DISPATCH)
					{
						CComDispatchDriver dispItem = vItem.pdispVal;
						CComVariant vTargetName,vTargetId,vTargetState,vId,vRefid,vCont;
						LPCOLESTR tname=NULL, tid=NULL, id=NULL, refid=NULL, cont=NULL;
						DWORD state = 0;

						hr = dispItem.GetPropertyByName(L"cont", &vCont);
						if (SUCCEEDED(hr) && vCont.vt==VT_BSTR) cont = vCont.bstrVal;
						if (cont==NULL) continue;

						hr = dispItem.GetPropertyByName(L"targetName", &vTargetName);
						if (SUCCEEDED(hr) && vTargetName.vt==VT_BSTR && vTargetName.bstrVal[0]!=L'*') tname = vTargetName.bstrVal;
						hr = dispItem.GetPropertyByName(L"targetId", &vTargetId);
						if (SUCCEEDED(hr) && vTargetId.vt==VT_BSTR) tid = vTargetId.bstrVal;
						hr = dispItem.GetPropertyByName(L"targetState", &vTargetState);
						if (SUCCEEDED(hr) && vTargetState.vt==VT_BSTR) state = TryLoadStateFromString(vTargetState.bstrVal);
						hr = dispItem.GetPropertyByName(L"id", &vId);
						if (SUCCEEDED(hr) && vId.vt==VT_BSTR) id = vId.bstrVal;
						hr = dispItem.GetPropertyByName(L"refid", &vRefid);
						if (SUCCEEDED(hr) && vRefid.vt==VT_BSTR) refid = vRefid.bstrVal;

						CDuiStyle* pref = GetStyleById(refid);
						CDuiStyle* pStyle = CreateStyle(tname, tid, state, NULL, pref);
						if (pStyle)
						{
							if(id) pStyle->SetId(id);
							pStyle->ParseStyle(cont);
						}
					}
				}
			}
		}

		return TRUE;
	}
	return FALSE;
}

BOOL CDuiLayoutManager::_IsScriptResource(CMarkupNode& r)
{
	if (lstrcmpiW(r.GetName(), L"Script") == 0) // 脚本代码，最多只有src属性
	{
		if (!m_script.IsValid())
			return TRUE;

#ifndef NO3D
		d3d::SetActiveScriptWindow(m_hWndPaint);
#endif
		LPCOLESTR url = r.GetAttributeValue(L"src");
		if (url)
		{
			CComPtr<IStream> spStream;
			CDownload::LoadUrlToStream(url, &spStream, GetResourceInstance());
			if (spStream)
			{
				CComBSTR bstr;
				CDownload::StreamToBSTR(spStream, bstr);
				m_script.Execute(bstr, TRUE);
			}
		}

		CMarkupNode cont_node = r.GetChild(FALSE);
		while (cont_node.IsValid() && !cont_node.IsTextBlock()) cont_node = cont_node.GetSibling(FALSE);
		if (cont_node.IsTextBlock())
		{
			LPCOLESTR code = cont_node.GetValue();
			m_script.Execute(code, TRUE);
		}
#ifndef NO3D
		d3d::SetActiveScriptWindow(NULL);
#endif
		return TRUE;
	}
	return FALSE;
}

BOOL CDuiLayoutManager::_IsResource(CMarkupNode& r)
{
	if (_IsImageResource(r)) return TRUE;
	if (_IsStyleResource(r)) return TRUE;
	if (_IsScriptResource(r)) return TRUE;
#ifndef NO3D
	if (Get3DDevice()!=NULL && Get3DDevice()->IsChildNode(&r)) return TRUE;
#endif
	return FALSE;
}

void CDuiLayoutManager::_ParseResources(CMarkupNode* pRoot)
{
	CMarkupNode res = pRoot->GetChild(L"Resources");
	if (!res.IsValid()) res = pRoot->GetChild(L"Resource");
	if (!res.IsValid())
		return;

	// 首先加载 system.js 系统脚本
	if (/*!res.HasAttribute(L"nosystemscript") &&*/ m_script.IsValid())
	{
		CComPtr<IStream> spStream;
		CDownload::LoadUrlToStream(L"system.js", &spStream, _AtlBaseModule.GetResourceInstance());
		if (spStream)
		{
			CComBSTR bstr;
			CDownload::StreamToBSTR(spStream, bstr);
			m_script.Execute(bstr);
		}
	}

	// 首先读取外部资源
	LPCOLESTR url = res.GetAttributeValue(L"src");
	if (url)
	{
		CComPtr<IStream> spStream;
		CDownload::LoadUrlToStream(url, &spStream, GetResourceInstance());
		if (spStream)
		{
			CComBSTR bstr;
			CDownload::StreamToBSTR(spStream, bstr);

			CMarkup xml;
			// 设置内容为纯文本的标签
			xml.AddConstTextTag(L"script");
			xml.AddConstTextTag(L"pre");
			xml.AddConstTextTag(L"style");
			xml.AddConstTextTag(L"stylesheet");
			xml.AddConstTextTag(L"css");
			xml.AddConstTextTag(L"template");
			xml.AddConstTextTag(L"effect"); // 设置 effect 标签的内容为纯文本

			// 设置无需关闭标签的标签
			xml.AddInlineTag(L"WhiteSpace"); xml.AddInlineTag(L"w");
			xml.AddInlineTag(L"LineBreak"); xml.AddInlineTag(L"br");
			xml.AddInlineTag(L"HorizontalLine"); xml.AddInlineTag(L"hr");
			xml.AddInlineTag(L"Img");

			if (xml.Load(bstr))
			{
				for (CMarkupNode r=xml.GetRoot(); r.IsValid(); r=r.GetSibling())
				{
					_IsResource(r);
				}
			}
		}
	}

	for (CMarkupNode r=res.GetChild(); r.IsValid(); r=r.GetSibling())
	{
		_IsResource(r);
	}

	//LineLog(L"Begin Style Inherit Graph:");
	//for (int i=0; i<m_aStyles.GetSize(); i++)
	//{
	//	CDuiStyle* p = (CDuiStyle*)m_aStyles[i];
	//	LineLog dbg(L"[%s,%s,%d]", p->GetTargetName(), p->GetTargetId(), p->GetTargetState());
	//	while (p=(CDuiStyle*)p->GetParentStyle()) dbg(L" - [%s,%s,%d]", p->GetTargetName(), p->GetTargetId(), p->GetTargetState());
	//}
	//LineLog(L"End Style Inherit Graph");
}

DuiNode* CDuiLayoutManager::_AddChild(CMarkupNode* pNode, DuiNode* pCtrlParent/* =NULL */, BOOL bRemoveChildrenBefore/*=FALSE*/)
{
	if (pNode==NULL || !pNode->IsValid())
		return NULL;

	LPCOLESTR tagname = NULL, _tag = (pNode->IsTextBlock()?L"InlineText":pNode->GetName());
	tagname = _tag;
	PluginCallContext(TRUE).Output(&tagname).Call(OpCode_GetNormalName, _tag, NULL/*L"xml"*/);

	ATLTRACE(L"%s\n", tagname);
	// 如果需要先删除子控件
	if (bRemoveChildrenBefore)
	{
		if (pCtrlParent==NULL)
			pCtrlParent = m_pRoot;
		if (pCtrlParent)
		{
			//fire_control_unload_events(pCtrlParent, TRUE, FALSE);
			pCtrlParent->RemoveAllChildren();
		}
	}

	// 创建控件
	DuiNode* pRet = CreateControl(pCtrlParent, tagname);
	//if (pRet==NULL && !pNode->IsTextBlock()) // 这是一个自定义标签
	//{
	//	pRet = CreateControl(pCtrlParent, CLASS_NAME(CDuiControlExtension));
	//	if (pRet)
	//	{
	//		pRet->SetAttribute(L"tagname", CComVariant(tagname/*pNode->GetName()*/)); // 把自定义标签的名称存放到tagname属性中
	//	}
	//}
	if (pRet==NULL) return NULL;

	{
		DisableOp dop(pRet, DUIDISABLED_INVALIDATE | DUIDISABLED_UPDATELAYOUT | DUIDISABLED_UPDATELAYOUT_FROMCHILD);

		// 处理控件属性
		if (!pNode->IsTextBlock())
		{
			for (int i=0; i<pNode->GetAttributeCount(); i++)
			{
				LPCOLESTR name = pNode->GetAttributeName(i);
				LPCOLESTR val = pNode->GetAttributeValue(i);
				pRet->SetAttribute(name, CComVariant(val));
#ifndef NO3D
				// 如果提供了 Scene 属性，则不必使用 <Scene> 子标签，此时属性值代表设备中的命名场景资源
				if (Get3DDevice() && lstrcmpiW(name, L"Scene")==0)
				{
					pRet->m_pScene = Get3DDevice()->FindScene(val);
					if (pRet->m_pScene)
					{
						CComQIPtr<IPeerHolder> peer = pRet->GetObject(TRUE);
						if (peer.p)
						{
							peer->AttachObject(pRet->m_pScene->GetDispatch());
						}
					}
				}
#endif
			}
		}
		else
		{
			pRet->SetAttribute(L"text", CComVariant(pNode->GetValue()));
		}
	}

	pRet->__cache.masks = 0; // clear all caches.

	// 创建子控件
	for (CMarkupNode node=pNode->GetChild(FALSE); node.IsValid(); node=node.GetSibling(FALSE))
	{
#ifndef NO3D
		// 场景标签交给3D设备处理
		if (Get3DDevice() && lstrcmpiW(node.GetName(), L"Scene")==0)
		{
			// 有可能在属性中指定了场景资源，此时忽略场景标签
			if (pRet->m_pScene==NULL)
			{
				CComPtr<IDispatch> disp;
				Get3DDevice()->IsChildNode(&node, &disp);
				pRet->m_pScene = DISP_CAST(disp.p, Scene);
				if (pRet->m_pScene)
				{
					CComQIPtr<IPeerHolder> peer = pRet->GetObject(TRUE);
					if (peer.p)
					{
						peer->AttachObject(pRet->m_pScene->GetDispatch());
					}
				}
			}
			continue;
		}
#endif

		BOOL bDone = FALSE;
		InvokeAllSinksBool(pRet, OnAddChildNode, (HANDLE)&node);
		if (!bDone)
			_AddChild(&node, pRet);
	}
	pRet->SetReadyState(READYSTATE_LOADED);

	//r->m_readyState = TRUE;
	//r->SetReadyState(READYSTATE_COMPLETE);

	//// 现在激发load事件
	//DuiEvent event = { (DUI_EVENTTYPE)0 };
	//event.Name = event_type_to_name(DUIET_load);
	//event.pSender = pRet;
	//event.dwTimestamp = ::GetTickCount();
	//event.cancelBubble = TRUE;
	////pRet->OnEvent(event);
	//FireEvent(pRet, event);

	return pRet;
}

HINSTANCE CDuiLayoutManager::GetResourceInstance()
{
	DELEGATE_TO_OWNER(m_hInstance,;);
	return m_hInstance;
}

HINSTANCE CDuiLayoutManager::GetLanguageInstance()
{
	DELEGATE_TO_OWNER(m_hLangInst,;);
	return m_hLangInst;
}

void CDuiLayoutManager::SetResourceInstance(HINSTANCE hInst)
{
	m_hInstance = hInst;
	if( m_hLangInst == NULL ) m_hLangInst = hInst;
}

void CDuiLayoutManager::SetLanguageInstance(HINSTANCE hInst)
{
	m_hLangInst = hInst;
}

HWND CDuiLayoutManager::GetPaintWindow() const
{
	return m_hWndPaint;
}

#ifndef NO3D
Device* CDuiLayoutManager::Get3DDevice() const
{
	return m_bD3DEnabled ? m_pDevice : NULL;
}
#endif // NO3D

BOOL CDuiLayoutManager::__IsAlpha() const
{
	return m_bAlphaEnabled;
	//return m_pRoot && lstrcmpiW(m_pRoot->GetAttribute(L"wmode"), L"transparent")==0;
}

#ifdef GDI_ONLY
void CDuiLayoutManager::__PrepareDC()
{
	if (m_bOffscreenPaint && m_hDcOffscreen==NULL)
	{
		CRect rc, rcWin;
		::GetWindowRect(m_hWndPaint, &rcWin);
		::GetClientRect(m_hWndPaint, &rc);

		if (!__IsAlpha())
		{
			m_hDcOffscreen = ::CreateCompatibleDC(m_hDcPaint);
			ASSERT(m_hDcOffscreen);
			::SelectObject(m_hDcOffscreen, (HFONT)::GetStockObject(DEFAULT_GUI_FONT));
			m_hbmpOffscreen = ::CreateCompatibleBitmap(m_hDcPaint, rc.Width(), rc.Height()); 
			ASSERT(m_hbmpOffscreen);
			m_pBits = NULL;
			SelectObject(m_hDcOffscreen, m_hbmpOffscreen);
		}
		else
		{
			m_hDcOffscreen = ::CreateCompatibleDC(/*m_hDcPaint*/NULL);
			ASSERT(m_hDcOffscreen);
			::SelectObject(m_hDcOffscreen, (HFONT)::GetStockObject(DEFAULT_GUI_FONT));
			::SetBkMode(m_hDcOffscreen, TRANSPARENT);
			BITMAPINFOHEADER bih = {0};
			bih.biSize = sizeof(BITMAPINFOHEADER);
			bih.biBitCount = 32;
			bih.biCompression = BI_RGB;
			bih.biPlanes = 1;
			bih.biWidth = rcWin.Width();
			bih.biHeight = -rcWin.Height();
			bih.biSizeImage = rcWin.Width() * rcWin.Height() * sizeof(DWORD);
			//LPDWORD pBits = NULL;
			m_hbmpOffscreen = ::CreateDIBSection(NULL, (BITMAPINFO *)&bih, DIB_RGB_COLORS, (void **)&m_pBits, NULL, 0x0);
			SelectObject(m_hDcOffscreen, m_hbmpOffscreen);
			if (m_pBits)
			{
				LPDWORD pBits = m_pBits;
				for (int y=0; y<rcWin.Height(); y++)
					for (int x=0; x<rcWin.Width(); x++, pBits++)
						*pBits = 0xBF3F3F3F; //000000;
			}
		}
	}
}

HDC CDuiLayoutManager::GetPaintDC()
{
	__PrepareDC();
	return m_bOffscreenPaint ? m_hDcOffscreen : m_hDcPaint;
}

#else
void CDuiLayoutManager::__PrepareSurface()
{
	if (m_bOffscreenPaint && m_pSurfaceOffscreen==NULL)
	{
		if (m_pSurface == NULL)
		{
			m_pSurface = _primarySurface = NEW CPrimarySurface(m_hWndPaint); // 重新获取屏幕表面
			//m_pSurface = _gdiSurface = NEW CGdiSurface(m_hWndPaint);
			ATLASSERT(m_pSurface);
		}
		//if (__IsAlpha() && _primarySurface)
		//{
		//	DeleteSurface(_primarySurface);
		//	m_pSurface = _primarySurface = NEW CPrimarySurface(m_hWndPaint); // 重新获取屏幕表面
		//	//m_pSurface = _gdiSurface = NEW CGdiSurface(m_hWndPaint);
		//	ATLASSERT(m_pSurface);
		//	CSurfaceDC dc(m_pSurface);
		//	dc.SelectFont((HFONT)::GetStockObject(DEFAULT_GUI_FONT));
		//}

		if (_primarySurface) m_pSurfaceOffscreen = _offscrnSurface = (COffscreenSurface*)_primarySurface->Clone(); // NEW COffscreenSurface(*_primarySurface);
		else if (_gdiSurface) m_pSurfaceOffscreen = _offscrnGdiSurface = (COffscreenGdiSurface*)_gdiSurface->Clone(); // NEW COffscreenGdiSurface(*_gdiSurface);
		else ATLASSERT(FALSE);

		//CSurfaceDC dc(m_pSurfaceOffscreen);
		//dc.SelectFont((HFONT)::GetStockObject(DEFAULT_GUI_FONT));

		if (!__IsAlpha())
		{
			m_pBits = NULL;
		}
		else
		{
			// 首先填充背景颜色
			//m_pSurfaceOffscreen->FillRect(NULL, 0x3F3F3F, 0xBF);
			m_pSurfaceOffscreen->FillRect(NULL, 0x000000, 0xFF);
			//DDBLTFX bfx = {sizeof(DDBLTFX)};
			//bfx.dwFillColor = 0xBF3F3F3F;
			//((IDirectDrawSurface*)*m_pSurfaceOffscreen)->Blt(NULL, NULL, NULL, DDBLT_COLORFILL, &bfx);
			//CRect rc, rcWin;
			//::GetWindowRect(m_hWndPaint, &rcWin);
			//dc.SetBkMode(TRANSPARENT);
			//BITMAPINFOHEADER bih = {0};
			//bih.biSize = sizeof(BITMAPINFOHEADER);
			//bih.biBitCount = 32;
			//bih.biCompression = BI_RGB;
			//bih.biPlanes = 1;
			//bih.biWidth = rcWin.Width();
			//bih.biHeight = -rcWin.Height();
			//bih.biSizeImage = rcWin.Width() * rcWin.Height() * sizeof(DWORD);
			////LPDWORD pBits = NULL;
			//m_hbmpOffscreen = ::CreateDIBSection(NULL, (BITMAPINFO *)&bih, DIB_RGB_COLORS, (void **)&m_pBits, NULL, 0x0);
			//dc.SelectBitmap(m_hbmpOffscreen);
			//if (m_pBits)
			//{
			//	LPDWORD pBits = m_pBits;
			//	for (int y=0; y<rcWin.Height(); y++)
			//		for (int x=0; x<rcWin.Width(); x++, pBits++)
			//			*pBits = 0xBF3F3F3F; //000000;
			//}
		}
	}
}

CSurface* CDuiLayoutManager::GetSurface(BOOL bOffscreen/*=TRUE*/)
{
	__PrepareSurface();
	return (bOffscreen && m_bOffscreenPaint && m_pSurfaceOffscreen) ? m_pSurfaceOffscreen : m_pSurface;
}
#endif // GDI_ONLY

POINT CDuiLayoutManager::GetMousePos() const
{
	return m_ptLastMousePos;
}

RECT CDuiLayoutManager::GetClientRect() const
{
	CRect rc;
	::GetClientRect(m_hWndPaint, &rc);
	if (__IsAlpha()/* && !::IsZoomed(m_hWndPaint)*/) rc.OffsetRect(CLayerHelper::GetClientOffset(m_hWndPaint));
	return rc;
}

SIZE CDuiLayoutManager::GetClientSize() const
{
	CRect rc = GetClientRect();
	return rc.Size();
}

void CDuiLayoutManager::SetMinMaxInfo(int cx, int cy)
{
	ASSERT(cx>=0 && cy>=0);
	m_szMinWindow.cx = cx;
	m_szMinWindow.cy = cy;
}

void CDuiLayoutManager::__AdjustPoint(LPPOINT ppt)
{
	ATLASSERT(ppt);
	//if (__IsAlpha()/* && !::IsZoomed(m_hWndPaint)*/) CLayerHelper::OffsetPointInc(m_hWndPaint, ppt);
	//RECT rc={0};
	//rc = CDuiControlExtension::GetPosBy(m_pRoot, rc);
	//ppt->x -= rc.left;
	//ppt->y -= rc.top;
}

void CDuiLayoutManager::DrawChildrenHwnd(HDC hdc /*=NULL*/, HWND hwndChild /* = NULL */)
{
	if (__IsAlpha()
#ifndef NO3D
		&& Get3DDevice()==NULL
#endif
		)
	{
		CSurfaceDC* dc2 = NULL;
		if (hdc == NULL)
		{
			__PrepareSurface();
			dc2 = NEW CSurfaceDC(GetSurface(), TRUE/*, FALSE*/);
			hdc = dc2->m_hDC;
		}

		for (int i=0; i<m_aChildHwnds.GetSize(); i++)
		{
			HWND hwnd = (HWND)m_aChildHwnds[i];
			CDuiWindowBase* pWin = reinterpret_cast<CDuiWindowBase*>(::GetProp(hwnd, _T("WndX")));
			if (pWin && ::GetParent(hwnd)==m_hWndPaint && (hwndChild==NULL || hwndChild==hwnd))
			{
				CRect rc;
				::GetWindowRect(hwnd, &rc);
				::ScreenToClient(GetPaintWindow(), (LPPOINT)&rc);
				::ScreenToClient(GetPaintWindow(), ((LPPOINT)&rc) + 1);
				__AdjustPoint((LPPOINT)&rc);
				__AdjustPoint(((LPPOINT)&rc) + 1);

				pWin->DrawToDC32(hdc, rc);


				//CMemoryDC mdc(dc->m_hDC, rc);
				//::SendMessage(hwnd, WM_PRINT, (WPARAM)mdc.m_hDC, (LPARAM)(PRF_CHECKVISIBLE|PRF_NONCLIENT|PRF_CLIENT|PRF_ERASEBKGND|PRF_CHILDREN|PRF_OWNED));
				////CWindowDC ctrldc(hwnd);
				//HBITMAP hbmp = (HBITMAP)::GetCurrentObject(mdc.m_hDC, OBJ_BITMAP);
				//DWORD dwot = GetObjectType(hbmp);
				//BITMAP bitmap = {0};
				//int len1 = sizeof(BITMAP);
				//int len2 = sizeof(DIBSECTION);
				//int len = ::GetObjectW(hbmp, sizeof(BITMAP), &bitmap);
				//DWORD dwError;
				//if (len == 0) dwError = GetLastError();
				////dc->BitBlt(rc.left, rc.top, rc.Width(), rc.Height(), ctrldc.m_hDC, 0, 0, SRCCOPY);
				////CopyHDC32(dc->m_hDC, rc, ctrldc.m_hDC);
			}
		}

		if (dc2) delete dc2;
	}
}

void CDuiLayoutManager::__Draw()
{
	if (m_bDestroying || m_lPaintLocked>0 || ::IsIconic(m_hWndPaint)) return;

	// 首先执行延迟的更新任务
	gConnector.Fire(NULL, this, cp_delayed_update_layout);

	if (__IsAlphaOrDD()
		&& (m_bInvalid
		|| ::GetUpdateRect(m_hWndPaint, NULL, FALSE)
		))/* return;*/

	//
	// Render screen
	//
	//static int __i=0;
	//ATLTRACE(L"------Draw(%d)\n", __i++);
	//if( m_anim.IsAnimating() )
	//{
	//	// 3D animation in progress
	//	m_anim.Render();
	//	// Do a minimum paint loop
	//	// Keep the client area invalid so we generate lots of
	//	// WM_PAINT messages. Cross fingers that Windows doesn't
	//	// batch these somehow in the future.
	//	PAINTSTRUCT ps = { 0 };
	//	::BeginPaint(m_hWndPaint, &ps);
	//	::EndPaint(m_hWndPaint, &ps);
	//	//Invalidate(ps.rcPaint);
	//	::InvalidateRect(m_hWndPaint, NULL, FALSE);
	//}
	//else if( m_anim.IsJobScheduled() )
	//{
	//	// Animation system needs to be initialized
	//	m_anim.Init(m_hWndPaint);
	//	// A 3D animation was scheduled; allow the render engine to
	//	// capture the window content and repaint some other time
	//	if( !m_anim.PrepareAnimation(m_hWndPaint) ) m_anim.CancelJobs();
	//	::InvalidateRect(m_hWndPaint, NULL, TRUE);
	//}
	//else
//#ifndef GDI_ONLY
//	if (::GetUpdateRect(m_hWndPaint, NULL, FALSE))
//	//{
//	//	GetSurface()->BltToPrimary();
//	//}
//#endif // GDI_ONLY
	//else
	{
		if( m_bFirstLayout && m_pRoot )
		{
			m_bFirstLayout = FALSE;
			SendNotify(m_pRoot, _T("windowinit"));
		}

		if( m_bFocusNeeded ) SetNextTabControl();

#ifndef NO3D
		if (Get3DDevice())
		{
			m_pDevice->Render();
			::ValidateRect(m_hWndPaint, NULL);
			m_bInvalid = FALSE;
			return;
		}
#endif

		// Standard painting of control-tree - no 3D animation now.
		// Prepare offscreen bitmap?
#ifdef GDI_ONLY
		__PrepareDC();
		CDCHandle dc(GetPaintDC());
#else
		__PrepareSurface();
		CSurfaceDC dc(GetSurface(), TRUE/*, FALSE*/);
#endif // GDI_ONLY

		// Begin Windows paint
		CRect rc=GetClientRect();
		if (m_pRoot==NULL && m_strError.Length()>0)
		{
			BOOL bGdi32 = IsGdi32(this);
			if (bGdi32)
				FillRect32(dc, rc, RGB(255,255,255));
			else
			{
				COLORREF clrOld = ::SetBkColor(dc.m_hDC, RGB(255,255,255));
				if(clrOld != CLR_INVALID)
				{
					::ExtTextOut(dc.m_hDC, 0, 0, ETO_OPAQUE, &rc, NULL, 0, NULL);
					::SetBkColor(dc.m_hDC, clrOld);
				}
			}
			rc.DeflateRect(10,10);
			call_drawText(bGdi32, (dc, m_strError, m_strError.Length(), &rc, /*DT_CENTER|DT_VCENTER*/0, 0,0));
			::ValidateRect(m_hWndPaint, NULL);
			m_bInvalid = FALSE;
			return;
		}

		int iSaveDC = ::SaveDC(dc.m_hDC);
		InDrawData idd = {0};
		m_pRoot->InitDrawData(dc.m_hDC, idd);
		m_pRoot->DoPaint(dc.m_hDC, rc, TRUE, idd);

		for (DuiNode* r=m_fixedChildren; r; r=r->next)
		{
			if (!r->IsVisible()) continue;
			r->DoPaint(dc.m_hDC, rc, TRUE, idd);
		}

		::RestoreDC(dc.m_hDC, iSaveDC);
		if (__IsAlpha())
		{
			DrawChildrenHwnd(dc.m_hDC);
		}
		else
		{
#ifdef GDI_ONLY
			PAINTSTRUCT ps = { 0 };
			::BeginPaint(m_hWndPaint, &ps);
			if( m_bOffscreenPaint )
			{
				// We have an offscreen device to paint on for flickerfree display.
				HBITMAP hOldBitmap = (HBITMAP) dc.SelectBitmap(m_hbmpOffscreen);
				::BitBlt(ps.hdc, 
					ps.rcPaint.left, 
					ps.rcPaint.top, 
					ps.rcPaint.right - ps.rcPaint.left,
					ps.rcPaint.bottom - ps.rcPaint.top,
					m_hDcOffscreen,
					ps.rcPaint.left,
					ps.rcPaint.top,
					SRCCOPY);
				::SelectObject(m_hDcOffscreen, hOldBitmap);
			}
			else
			{
				// A standard paint job
				int iSaveDC = dc.SaveDC(/*ps.hdc*/);
				dc.SelectFont(/*ps.hdc, */(HFONT)::GetStockObject(DEFAULT_GUI_FONT));
				InDrawData idd = {0};
				m_pRoot->InitDrawData(dc.m_hDC, idd);
				m_pRoot->DoPaint(dc.m_hDC, ps.rcPaint, TRUE, idd);
				dc.RestoreDC(iSaveDC);
			}
			// All Done!
			::EndPaint(m_hWndPaint, &ps);
#else
			dc.Unlock();
			GetSurface()->BltToPrimary();
			dc.Lock();
			//::ValidateRect(m_hWndPaint, NULL);
#endif // GDI_ONLY
		}
	}
	::ValidateRect(m_hWndPaint, NULL);
	m_bInvalid = FALSE;
}

BOOL CDuiLayoutManager::PreMessageHandler( LPMSG pMsg )
{
	BOOL isKeyMsg = pMsg->message>=WM_KEYFIRST && pMsg->message<=WM_KEYLAST;
	if (isKeyMsg && m_pFocus)
	{
		DuiNode* pCtrl = m_pFocus ? m_pFocus : m_pRoot;
		CDuiAxSite* pAxCtrl = CLASS_CAST(pCtrl, CDuiAxSite);
		if (pAxCtrl)
		{
			CComPtr<IOleInPlaceActiveObject> spipa;
			pAxCtrl->QueryControl(&spipa);
			if (spipa.p && S_OK == spipa->TranslateAccelerator(pMsg))
				return TRUE;
		}
	}

	switch( pMsg->message )
	{
	case WM_KEYDOWN:
		{
			// Tabbing between controls
			if( pMsg->wParam == VK_TAB )
			{
				SetNextTabControl(::GetKeyState(VK_SHIFT) >= 0);
				m_SystemConfig.bShowKeyboardCues = TRUE;
				//::InvalidateRect(m_hWndPaint, NULL, FALSE);
				Invalidate(&m_pRoot->GetPos(DUIRT_TOTAL), TRUE);
				return TRUE;
			}
			// Handle default dialog controls OK and CANCEL.
			// If there are controls named "ok" or "cancel" they
			// will be activated on keypress.
			else if( pMsg->wParam == VK_RETURN )
			{
				HWND hwndFocus = ::GetFocus();
				//GUITHREADINFO gti = {sizeof(GUITHREADINFO)};
				//GetGUIThreadInfo(GetCurrentThreadId(), &gti);
				if (hwndFocus && (((DWORD)GetWindowLong(hwndFocus, GWL_STYLE))&ES_WANTRETURN))
				{
					return FALSE;
				}
				DuiNode* pDef = FindControl(_T("ok"));
				if (pDef==NULL || (pDef->GetControlFlags() & DUIFLAG_WANTRETURN))
					pDef = m_pFocus;
				if (pDef && (pDef->GetControlFlags() & DUIFLAG_WANTRETURN)==0)
				{
					DuiEvent event = { (DUI_EVENTTYPE)0 };
					event.Name = event_type_to_name(DUIET_click);
					event.dwTimestamp = ::GetTickCount();
					event.pSender = DuiHandle<DuiNode>(pDef);
					//pDef->OnEvent(event);
					FireEvent(pDef, event);
					return TRUE;
				}
			}
			else if( pMsg->wParam == VK_ESCAPE )
			{
				DuiNode* pControl = FindControl(_T("cancel"));
				if (pControl) return pControl->Activate(), TRUE;
			}
		}
		break;
	case WM_SYSCHAR:
		{
			// Handle ALT-shortcut key-combinations
			FINDSHORTCUT fs = { 0 };
			fs.ch = toupper(pMsg->wParam);
			DuiNode* pControl = m_pRoot->FindControl(__FindControlFromShortcut, &fs, DUIFIND_VISIBLE | DUIFIND_ENABLED | DUIFIND_ME_FIRST);
			if( pControl != NULL )
				return pControl->SetFocus(), pControl->Activate(), TRUE;
		}
		break;
	case WM_SYSKEYDOWN:
		{
			// Press ALT once and the shortcuts will be shown in view
			if( pMsg->wParam == VK_MENU && !m_SystemConfig.bShowKeyboardCues )
			{
				m_SystemConfig.bShowKeyboardCues = TRUE;
				//::InvalidateRect(m_hWndPaint, NULL, FALSE);
				Invalidate(&m_pRoot->GetPos(DUIRT_TOTAL), TRUE);
			}
			if( m_pFocus != NULL )
			{
				DuiEvent event = { NULL };
				event.Name = event_type_to_name(DUIET_syskey);
				event.chKey = pMsg->wParam;
				event.ptMouse = m_ptLastMousePos;
				event.wKeyState = MapKeyState();
				event.dwTimestamp = ::GetTickCount();
				//m_pFocus->OnEvent(event);
				FireEvent(m_pFocus, event);
			}
		}
		break;
	}
	return FALSE;
}

BOOL CDuiLayoutManager::MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& lRes)
{
	//if (::ImmIsUIMessageW(NULL, uMsg, wParam, lParam))
	//	return TRUE;
	//ATLTRACE(L"Message:0x%x\n", uMsg);
	if (m_pRoot == NULL && m_strError.Length()==0) return FALSE;
	// Not ready yet?
	if( m_hWndPaint == NULL ) return FALSE;
	// Cycle through listeners
	for( int i = 0; i < m_aMessageFilters.GetSize(); i++ ) 
	{
		BOOL bHandled = FALSE;
		LRESULT lResult = static_cast<CDuiMessageFilter*>(m_aMessageFilters[i])->MessageHandler(uMsg, wParam, lParam, bHandled);
		if( bHandled )
			return lRes = lResult, TRUE;
	}

	return DefMessageHandler(uMsg, wParam, lParam, lRes);
}

BOOL CDuiLayoutManager::DefMessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& lRes)
{
	//#define Dbg_Message

#ifdef Dbg_Message

	DWORD dwTick = GetTickCount();
	static ULONG __keyid = 0;
	ULONG kid = __keyid++;
	LineLog dbg(L"OnMessage[%d]:", kid);
#define dbg_tickcount() dbg(L" - cost[%d](%d)", kid, GetTickCount()-dwTick)
#define dbg_line() dbg(L" - line[%d](%d)", __LINE__, GetTickCount()-dwTick)
#define dbg_msg(msg) msg: dbg(L" - %s(%d)", L#msg, msg); //

#else
#define dbg	__noop
#define dbg_tickcount() 
#define dbg_line()
#define dbg_msg(msg) msg:
#endif // Dbg_Message

	// Custom handling of events
	switch( uMsg )
	{
	case WM_APP + 1:
		{
			// Delayed control-tree cleanup. See AttachControl() for details.
			for( int i = 0; i < m_aDelayedCleanup.GetSize(); i++ ) static_cast<DuiNode*>(m_aDelayedCleanup[i])->DeleteThis();
			m_aDelayedCleanup.RemoveAll();
		}
		break;
	case WM_CHILDPAINTED:
#ifndef GDI_ONLY
		if (__IsAlpha()
#ifndef NO3D
			&& Get3DDevice()==NULL
#endif
			)
		{
			HWND hwndCtrl = (HWND)wParam;
			if (!::IsWindow(hwndCtrl)) return TRUE;
			DrawChildrenHwnd(NULL, hwndCtrl);
			GetSurface()->BltToPrimary();
		}
#endif // GDI_ONLY
		lRes = 0;
		return TRUE;
		break;
#ifndef NO3D
	case WM_CHILDSIZED:
		if (Get3DDevice() && m_pDevice->IsLayeredWindow())
		{
			m_pDevice->ResizeChild((HWND)wParam);
		}
		lRes = 0;
		return TRUE;
		break;
#endif
	case WM_PARENTNOTIFY:
		lRes = 1;
		return FALSE;
		if (__IsAlpha() && (LOWORD(wParam)==WM_CREATE))
		{
			HWND hwndCtrl = (HWND)lParam;
			if (::IsChild(m_hWndPaint, hwndCtrl))
			//if (::GetParent(hwndCtrl)==m_hWndPaint /*&& ((DWORD)GetWindowLong(hwndCtrl, GWL_STYLE) & WS_CAPTION)==0*/)
			{
#ifndef NO3D
				if (Get3DDevice() && m_pDevice->IsLayeredWindow())
				{
					CDuiWindowBase* pWin = CDuiWindowBase::FromHWND(hwndCtrl);
					if (pWin) pWin->HookToDC();
					m_pDevice->AddChild(hwndCtrl);
				}
#endif
				////else
				//{
				//	m_aChildHwnds.Add((LPVOID)hwndCtrl);
				//	CDuiWindowBase::FromHWND(hwndCtrl);
				//}
				::SendMessage(hwndCtrl, WM_SETFONT, (WPARAM)(HFONT)::GetStockObject(DEFAULT_GUI_FONT), 0);
			}
		}
		else if (__IsAlpha() && (LOWORD(wParam)==WM_DESTROY))
		{
			HWND hwndCtrl = (HWND)lParam;
#ifndef NO3D
			if (Get3DDevice() && m_pDevice->IsLayeredWindow())
				m_pDevice->RemoveChild(hwndCtrl);
#endif
			//else
				//m_aChildHwnds.Remove((LPVOID)hwndCtrl);
		}
		return FALSE;
		break;
	case dbg_msg(WM_NCHITTEST)//:
		lRes = ::DefWindowProcW(m_hWndPaint, uMsg, wParam, lParam);
		if (lRes==HTCLIENT && IsLayered(m_hWndPaint))
		{
			POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
			::ScreenToClient(m_hWndPaint, &pt);
			__AdjustPoint(&pt);
			DuiNode* pCtrl = FindControl(pt);
			if (pCtrl)
			{
				LONG ht = pCtrl->NcHittest(pt);
				if (ht != HTNOWHERE)
				{
					while (ht==HTTRANSPARENT && pCtrl->GetOffsetParent()/*rt(pCtrl)->GetParent()*/)
					{
						pCtrl = pCtrl->GetOffsetParent(); // rt(pCtrl)->GetParent();
						ht = pCtrl->NcHittest(pt);
					}
					if (ht!=HTTRANSPARENT || pCtrl->GetOffsetParent()/*rt(pCtrl)->GetParent()*/==NULL)
						lRes = (LRESULT)ht;
				}
			}
		}
		return TRUE;
		break;
	case dbg_msg(WM_CLOSE)//:
		{
			// Make sure all matching "closing" events are sent
			DuiEvent event = { (DUI_EVENTTYPE)0 };
			event.ptMouse = m_ptLastMousePos;
			event.dwTimestamp = ::GetTickCount();
			if( m_pEventHover != NULL )
			{
				event.Name = event_type_to_name(DUIET_mouseleave);
				event.pSender = DuiHandle<DuiNode>(m_pEventHover);
				//m_pEventHover->OnEvent(event);
				FireEvent(m_pEventHover, event);
			}
			if( m_pEventCaptured != NULL )
			{
				event.Name = event_type_to_name(DUIET_buttonup);
				event.pSender = DuiHandle<DuiNode>(m_pEventCaptured);
				//m_pEventCaptured->OnEvent(event);
				FireEvent(m_pEventCaptured, event);
				event.Name = event_type_to_name(DUIET_click);
				event.pSender = DuiHandle<DuiNode>(m_pEventCaptured);
				//m_pEventCaptured->OnEvent(event);
				FireEvent(m_pEventCaptured, event);
			}
			SetFocus(NULL);
			// Hmmph, the usual Windows tricks to avoid
			// focus loss...
			HWND hwndParent = ::GetWindow(m_hWndPaint, GW_OWNER);
			if( hwndParent != NULL ) ::SetFocus(hwndParent);
		}
		break;
	case dbg_msg(WM_ERASEBKGND)//:
		{
			//if (m_pRoot == NULL) return lRes=0, FALSE;
			// We'll do the painting here...
			// 
			//if (m_pDevice)
			//{
			//	CDCHandle dc((HDC)wParam);
			//	RECT rc;
			//	::GetClientRect(m_hWndPaint, &rc);
			//	dc.FillSolidRect(&rc, d3d::KeyColor());
			//}
			lRes = 1;
		}
		dbg_tickcount();
		return TRUE;
	case dbg_msg(WM_PAINT)//:
		{
			//if (m_pRoot == NULL) return TRUE;
			// Should we paint?
			RECT rcPaint = { 0 };
			//if ( __IsAlphaOrDD() && !::GetUpdateRect(m_hWndPaint, &rcPaint, FALSE) ) return TRUE;
			//if (__IsAlphaOrDD() && !m_bInvalid) return TRUE;
			// Do we need to resize anything?
			// This is the time where we layout the controls on the form.
			// We delay this even from the WM_SIZE messages since resizing can be
			// a very expensize operation.
//			if( m_bResizeNeeded )
//			{
//				CRect rc, rcWin;
//				::GetWindowRect(m_hWndPaint, &rcWin);
//				RECT rcClient = GetClientRect(); //{ 0 };
//				//::GetClientRect(m_hWndPaint, &rcClient);
//				if( !::IsRectEmpty(&rcClient) )
//				{
//					RECT rcOld = m_pRoot->GetPos(DUIRT_TOTAL);
//					if (!::EqualRect(&rcClient, &rcOld))
//						m_pRoot->SetPos(/*__IsAlpha()?rcWin:*/rcClient);
//					//CDuiControlExtension::UpdateLayout(m_pRoot);
//					//HDC hDC = ::CreateCompatibleDC(m_hDcPaint);
//					//InDrawData idd;
//					//m_pRoot->InitDrawData(hDC, idd);
//					//m_pRoot->DoPaint(hDC, rcClient, FALSE, idd);
//					//::DeleteDC(hDC);
//					m_bResizeNeeded = FALSE;
//					// We'll want to notify the window when it is first initialized
//					// with the correct layout. The window form would take the time
//					// to submit swipes/animations.
//					//if( m_bFirstLayout )
//					//{
//					//	m_bFirstLayout = FALSE;
//					//	SendNotify(m_pRoot, _T("windowinit"));
//					//}
//				}
//#ifdef GDI_ONLY
//				// Reset offscreen device
//				if( m_hDcOffscreen != NULL ) ::DeleteDC(m_hDcOffscreen);
//				m_hDcOffscreen = NULL;
//#else
//				DeleteSurface(_offscrnSurface);
//				DeleteSurface(_offscrnGdiSurface);
//				m_pSurfaceOffscreen = NULL;
//#endif // GDI_ONLY
//				if( m_hbmpOffscreen != NULL ) ::DeleteObject(m_hbmpOffscreen);
//				m_hbmpOffscreen = NULL;
//			}
//			// Set focus to first control?
//			if( m_bFocusNeeded ) SetNextTabControl();
			//
			// Render screen
			//
			__Draw(); dbg_line();
		}
		// If any of the painting requested a resize again, we'll need
		// to invalidate the entire window once more.
		//if( m_bResizeNeeded ) ::InvalidateRect(m_hWndPaint, NULL, FALSE);
		dbg_tickcount();
		return TRUE;
	case dbg_msg(WM_PRINTCLIENT)//:
		{
			RECT rcClient = GetClientRect();
			//::GetClientRect(m_hWndPaint, &rcClient);
			HDC hDC = (HDC) wParam;
			int save = ::SaveDC(hDC);
			InDrawData idd;
			m_pRoot->InitDrawData(hDC, idd);
			m_pRoot->DoPaint(hDC, rcClient, TRUE, idd);
			// Check for traversing children. The crux is that WM_PRINT will assume
			// that the DC is positioned at frame coordinates and will paint the child
			// control at the wrong position. We'll simulate the entire thing instead.
			if( (lParam & PRF_CHILDREN) != 0 ) {
				HWND hWndChild = ::GetWindow(m_hWndPaint, GW_CHILD);
				while( hWndChild != NULL ) {
					RECT rcPos = { 0 };
					::GetWindowRect(hWndChild, &rcPos);
					::MapWindowPoints(HWND_DESKTOP, m_hWndPaint, reinterpret_cast<LPPOINT>(&rcPos), 2);
					::SetWindowOrgEx(hDC, -rcPos.left, -rcPos.top, NULL);
					// NOTE: We use WM_PRINT here rather than the expected WM_PRINTCLIENT
					//       since the latter will not print the nonclient correctly for
					//       EDIT controls.
					::SendMessage(hWndChild, WM_PRINT, wParam, lParam | PRF_NONCLIENT);
					hWndChild = ::GetWindow(hWndChild, GW_HWNDNEXT);
				}
			}
			::RestoreDC(hDC, save);
		}
		break;
	case dbg_msg(WM_GETMINMAXINFO)//:
		{
			LPMINMAXINFO lpMMI = (LPMINMAXINFO) lParam;
			lpMMI->ptMinTrackSize.x = m_szMinWindow.cx;
			lpMMI->ptMinTrackSize.y = m_szMinWindow.cy;
		}
		break;
	//case dbg_msg(WM_WINDOWPOSCHANGED)//:
		//DefWindowProc(m_hWndPaint, uMsg, wParam, lParam);
		//return TRUE;
		//break;
	case dbg_msg(WM_USER+888)//:
	case dbg_msg(WM_SIZE)//:
#if 0
	case dbg_msg(WM_EXITSIZEMOVE)//:
		if ((m_pDevice && uMsg==WM_EXITSIZEMOVE) ||
			(m_pDevice==NULL && uMsg==WM_SIZE))
#endif // _DEBUG
		{
			//lRes = ::DefWindowProc(m_hWndPaint, uMsg, wParam, lParam);
			if (::IsIconic(m_hWndPaint)) return TRUE;
#ifndef NO3D
			if (Get3DDevice()) m_pDevice->Reset();
#endif
			//RECT rcClient = GetClientRect(); //{ 0 };
			////::GetClientRect(m_hWndPaint, &rcClient);
			//if( !::IsRectEmpty(&rcClient) && m_pRoot )
			//	rt(m_pRoot)->SetPos(rcClient);
			//m_bResizeNeeded = TRUE;
			if (__IsAlphaOrDD())
			{
#ifdef GDI_ONLY
				if (m_hDcOffscreen) m_hDcOffscreen = (::DeleteDC(m_hDcOffscreen), NULL);
#else
				DeleteSurface(_offscrnSurface);
				DeleteSurface(_offscrnGdiSurface);
				m_pSurfaceOffscreen = NULL;
#endif // GDI_ONLY
				if (m_hbmpOffscreen) m_hbmpOffscreen = (::DeleteObject(m_hbmpOffscreen), NULL);
				UpdateLayout();
				//if (m_pDevice) m_pDevice->Reset();
				__Draw();
			}
		}
		dbg_tickcount();
		return FALSE;
		break;
	//case dbg_msg(WM_EXITSIZEMOVE)//:
	//	return FALSE;
	//	break;
	case dbg_msg(WM_TIMER)//:
		{
#ifndef NO3D
			if (Get3DDevice() && m_pDevice->IsBlindTimer((UINT)wParam)) break;
#endif

			if (wParam == -2010)
			{
				::KillTimer(m_hWndPaint, -2010);
				if (!m_bDblClick && m_pClick)
				{
					DuiEvent event = { NULL };
					event.Name = event_type_to_name(DUIET_click);
					event.wParam = wParam;
					event.lParam = lParam;
					event.ptMouse = m_ptLastMousePos;
					event.wKeyState = wParam;
					event.dwTimestamp = ::GetTickCount();
					//m_pEventCaptured->OnEvent(event);
					BOOL bRet = FireEvent(m_pClick, event);
				}
				m_pClick = NULL;
				m_bDblClick = FALSE;
				break;
			}

			for( int i = 0; i < m_aTimers.GetSize(); i++ ) {
				const TIMERINFO* pTimer = static_cast<TIMERINFO*>(m_aTimers[i]);
				if( pTimer->hWnd == m_hWndPaint && pTimer->uWinTimer == LOWORD(wParam) ) {
					DuiEvent event = { NULL };
					event.Name = event_type_to_name(DUIET_timer);
					event.wParam = pTimer->nLocalID;
					event.ptMouse = m_ptLastMousePos;
					event.dwTimestamp = ::GetTickCount();
					//pTimer->pSender->OnEvent(event);
					FireEvent(pTimer->pSender, event);
					break;
				}
			}
		}
		break;
	case dbg_msg(WM_ACTIVATE)//:
		if (wParam == WA_INACTIVE && GetAttributeBool(L"compact"))
			::SetProcessWorkingSetSize(GetCurrentProcess(), -1, -1);
		break;
	//case dbg_msg(WM_MOUSEHOVER)//:
	//	{
	//		m_bMouseTracking = FALSE;
	//		POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
	//		__AdjustPoint(&pt);
	//		CDuiControlExtensionBase* pHover = FindControl(pt);
	//		if( pHover == NULL || rt(pHover)->PtInScrollbar(pt) /*|| pHover==m_pEventHover*/ ) break;
	//		//// Generate mouse hover event
	//		//if( m_pEventHover != NULL ) {
	//		//	DuiEvent event = { (DUI_EVENTTYPE)0 };
	//		//	event.ptMouse = pt;
	//		//	event.Name = DUIET_mousehover;
	//		//	event.pSender = pHover;
	//		//	event.dwTimestamp = ::GetTickCount();
	//		//	//m_pEventHover->OnEvent(event);
	//		//	FireEvent(m_pEventHover, event);
	//		//}
	//		// Create tooltip information
	//		LPCOLESTR sToolTip = rt(pHover)->GetToolTip();
	//		if (sToolTip==NULL || *sToolTip==0) return TRUE;
	//		::ZeroMemory(&m_ToolTip, sizeof(TOOLINFO));
	//		m_ToolTip.cbSize = sizeof(TOOLINFO);
	//		m_ToolTip.uFlags = TTF_IDISHWND;
	//		m_ToolTip.hwnd = m_hWndPaint;
	//		m_ToolTip.uId = (UINT) m_hWndPaint;
	//		m_ToolTip.hinst = m_hInstance;
	//		m_ToolTip.lpszText = const_cast<LPWSTR>( sToolTip );
	//		m_ToolTip.rect = CDuiControlExtension::GetOffset(rt(pHover)->GetParent(), NULL, rt(pHover)->GetPos());
	//		if( m_hwndTooltip == NULL ) {
	//			m_hwndTooltip = ::CreateWindowEx(0, TOOLTIPS_CLASS, NULL, WS_POPUP | TTS_NOPREFIX | TTS_ALWAYSTIP, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, m_hWndPaint, NULL, m_hInstance, NULL);
	//			::SendMessage(m_hwndTooltip, TTM_ADDTOOL, 0, (LPARAM) &m_ToolTip);
	//		}
	//		::SendMessage(m_hwndTooltip, TTM_SETTOOLINFO, 0, (LPARAM) &m_ToolTip);
	//		::SendMessage(m_hwndTooltip, TTM_TRACKACTIVATE, TRUE, (LPARAM) &m_ToolTip);
	//	}
	//	if (__IsAlphaOrDD()) __Draw();
	//	dbg_tickcount();
	//	return TRUE;
	case dbg_msg(WM_MOUSELEAVE)//:
		{
			if( m_hwndTooltip != NULL ) ::SendMessage(m_hwndTooltip, TTM_TRACKACTIVATE, FALSE, (LPARAM) &m_ToolTip);
			//if( m_bMouseTracking ) ::SendMessage(m_hWndPaint, WM_MOUSEMOVE, 0, (LPARAM) -1);
			m_bMouseTracking = FALSE;
			if (m_pEventHover)
			{
				DuiEvent event = { event_type_to_name(DUIET_mouseleave) };
				event.dwTimestamp = ::GetTickCount();
				//m_pEventHover->OnEvent(event);
				FireEvent(m_pEventHover, event);
				m_pEventHover = NULL;
			}
		}
		break;
	case dbg_msg(WM_MOUSEMOVE)//:
		{
			//TimeCounter tc(L"=====MOUSE MOVE: ");
//#define timeofline
//#define timeofline(str) {tc(L"(%dlines - %s)", __LINE__, str);}
			// Start tracking this entire window again...
			if( !m_bMouseTracking ) {
				TRACKMOUSEEVENT tme = { 0 };
				tme.cbSize = sizeof(TRACKMOUSEEVENT);
				tme.dwFlags = /*TME_HOVER |*/ TME_LEAVE;
				tme.hwndTrack = m_hWndPaint;
				//tme.dwHoverTime = ((m_hwndTooltip == NULL) ? HOVER_DEFAULT : (DWORD) ::SendMessage(m_hwndTooltip, TTM_GETDELAYTIME, TTDT_INITIAL, 0L));
				_TrackMouseEvent(&tme); dbg_line();
				m_bMouseTracking = TRUE;
			}
			//if (lParam == -1) // 这是WM_MOUSELEAVE发来的
			//{
			//	if (m_pEventHover)
			//	{
			//		DuiEvent event = { DUIET_mouseleave };
			//		event.dwTimestamp = ::GetTickCount();
			//		m_pEventHover->OnEvent(event);
			//		m_pEventHover = NULL;
			//	}
			//	break;
			//}
			// Generate the appropriate mouse messages
			// 
			BOOL bActive = (::GetForegroundWindow()==__hwndTop);
			POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
			__AdjustPoint(&pt); dbg_line();
			m_ptLastMousePos = pt;
			DuiNode* pOldHover = m_pEventHover;
			DuiNode* pNewHover = FindControl(pt); dbg_line();
			//LineLog ll(L"OldHover:%s[0x%08x]; NewHover:%s[0x%08x]", pOldHover?pOldHover->__tag:L"(null)", pOldHover, pNewHover?pNewHover->__tag:L"(null)", pNewHover);
			if( pNewHover != NULL && (pNewHover)->m_pLayoutMgr != this ) break;
			DuiEvent event = { NULL };
			event.ptMouse = pt;
			event.dwTimestamp = ::GetTickCount();
			if( pNewHover != pOldHover && pOldHover != NULL ) {
				event.Name = event_type_to_name(DUIET_mouseleave);
				event.pSender = DuiHandle<DuiNode>(pNewHover);
				FireEvent(pOldHover, event);
				m_pEventHover = NULL; dbg_line();
				if ( bActive && m_hwndTooltip ) ::SendMessage(m_hwndTooltip, TTM_TRACKACTIVATE, FALSE, (LPARAM) &m_ToolTip); dbg_line();
			}
			if( pNewHover != pOldHover && pNewHover != NULL )
			{
				m_pEventHover = pNewHover; dbg_line();
				if (!(m_pEventHover)->PtInScrollbar(pt))
				{
					event.Name = event_type_to_name(DUIET_mouseenter);
					event.pSender = DuiHandle<DuiNode>(pOldHover);
					event.cancelBubble = FALSE;
					FireEvent(pNewHover, event);
					// 同时补发一次 hover 事件
					event.Name = event_type_to_name(DUIET_mousehover);
					event.pSender = DuiHandle<DuiNode>(pNewHover);
					event.cancelBubble = FALSE;
					FireEvent(pNewHover, event);
					// 测试 TOOLTIP
					// Create tooltip information
					// 
					if (bActive)
					{
						LPCOLESTR sToolTip = (pNewHover)->GetToolTip();
						if (sToolTip && *sToolTip!=0)
						{
							::ZeroMemory(&m_ToolTip, sizeof(TOOLINFO));
							m_ToolTip.cbSize = sizeof(TOOLINFO);
							m_ToolTip.uFlags = TTF_IDISHWND;
							m_ToolTip.hwnd = m_hWndPaint;
							m_ToolTip.uId = (UINT) m_hWndPaint;
							m_ToolTip.hinst = m_hInstance;
							m_ToolTip.lpszText = const_cast<LPWSTR>( sToolTip );
							m_ToolTip.rect = pNewHover->GetClientRect();// GetOffset(rt(pNewHover)->GetParent(), NULL, rt(pNewHover)->GetPos());
							if( m_hwndTooltip == NULL ) {
								m_hwndTooltip = ::CreateWindowEx(0, TOOLTIPS_CLASS, NULL, WS_POPUP | TTS_NOPREFIX | TTS_ALWAYSTIP, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, m_hWndPaint, NULL, m_hInstance, NULL);
								::SendMessage(m_hwndTooltip, TTM_ADDTOOL, 0, (LPARAM) &m_ToolTip);
								::SendMessage(m_hwndTooltip, TTM_SETMAXTIPWIDTH, 0, (LPARAM)9999);
							}
							::SendMessage(m_hwndTooltip, TTM_SETTOOLINFO, 0, (LPARAM) &m_ToolTip);
							::SendMessage(m_hwndTooltip, TTM_TRACKACTIVATE, TRUE, (LPARAM) &m_ToolTip);
						}
					}
				}
			}
			if( m_pEventCaptured != NULL ) {
				event.Name = event_type_to_name(DUIET_mousemove);
				event.pSender = NULL;
				event.cancelBubble = FALSE;
				return FireEvent(m_pEventCaptured, event);
			}
			else if( pNewHover != NULL ) {
				event.Name = event_type_to_name(DUIET_mousemove);
				event.pSender = NULL;
				event.cancelBubble = FALSE;
				return FireEvent(pNewHover, event); dbg_line();
			}
			//dbg(L" - end(%d)", GetTickCount()-dwTick);
		}
		break;
	case dbg_msg(WM_LBUTTONDOWN)//:
		{
			// We alway set focus back to our app (this helps
			// when Win32 child windows are placed on the dialog
			// and we need to remove them on focus change).
			::SetFocus(m_hWndPaint);
			POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
			__AdjustPoint(&pt);
			m_ptLastMousePos = pt;
			DuiNode* pControl = FindControl(pt);
			if( pControl == NULL ) break;
			if( pControl->m_pLayoutMgr != this ) break;
			SetCapture(pControl);
			DuiEvent event = { NULL };
			event.Name = event_type_to_name(DUIET_buttondown);
			event.wParam = wParam;
			event.lParam = lParam;
			event.ptMouse = pt;
			event.wKeyState = wParam;
			event.dwTimestamp = ::GetTickCount();
			//pControl->OnEvent(event);
			return FireEvent(pControl, event);
			// No need to burden user with 3D animations
			//m_anim.CancelJobs();
		}
		break;
	//case dbg_msg(WM_APP + 2)//: // click
	////case dbg_msg(WM_LBUTTONUP)//:
	//	if (!m_bDblClick)
	//	{
	//		DuiEvent event = { NULL };
	//		event.Name = event_type_to_name(DUIET_click);
	//		event.wParam = wParam;
	//		event.lParam = lParam;
	//		event.ptMouse = m_ptLastMousePos;
	//		event.wKeyState = wParam;
	//		event.dwTimestamp = ::GetTickCount();
	//		//m_pEventCaptured->OnEvent(event);
	//		BOOL bRet = FireEvent((CDuiControlExtensionBase*)wParam, event);
	//		return bRet;
	//	}
	//	m_bDblClick = FALSE;
	//	break;
	case dbg_msg(WM_LBUTTONUP)//:
		{
			POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
			__AdjustPoint(&pt);
			m_ptLastMousePos = pt;
			if( m_pEventCaptured == NULL ) break;
			BOOL bClick = ((m_pEventCaptured)->GetState() & DUISTATE_CAPTURED) && (m_pEventCaptured)->PtInControl(pt);
			DuiEvent event = { NULL };
			event.Name = event_type_to_name(DUIET_buttonup);
			event.wParam = wParam;
			event.lParam = lParam;
			event.ptMouse = pt;
			event.wKeyState = wParam;
			event.dwTimestamp = ::GetTickCount();
			//m_pEventCaptured->OnEvent(event);
			BOOL bRet = FireEvent(m_pEventCaptured, event);
			if (m_bDblClick==TRUE)
			{
				event.Name = event_type_to_name(DUIET_dblclick);
				event.dwTimestamp = ::GetTickCount();
				bRet = FireEvent(m_pEventCaptured, event);
				::KillTimer(m_hWndPaint, -2010);
				m_bDblClick = FALSE;
				m_pClick = NULL;
			}
			else if ((m_pEventCaptured)->m_dblclick) // 如果控件需要响应双击事件，则单击事件必须延迟执行
			{
				m_pClick = m_pEventCaptured;
				::SetTimer(m_hWndPaint, -2010, GetDoubleClickTime(), NULL);
				//::PostMessageW(m_hWndPaint, WM_APP+2, (WPARAM)m_pEventCaptured, 0);
			}
			else // 不需要双击，则直接响应单击事件
			{
				event.Name = event_type_to_name(DUIET_click);
				event.dwTimestamp = ::GetTickCount();
				bRet = FireEvent(m_pEventCaptured, event);
			}
			//m_bDblClick = FALSE;
			SetCapture(NULL);
			return bRet;
		}
		break;
	case dbg_msg(WM_LBUTTONDBLCLK)//:
		{
			POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
			__AdjustPoint(&pt);
			m_ptLastMousePos = pt;
			DuiNode* pControl = FindControl(pt);
			if( pControl == NULL ) break;
			if( (pControl)->m_pLayoutMgr != this ) break;
			m_bDblClick = TRUE;
			//DuiEvent event = { (DUI_EVENTTYPE)0 };
			//event.Name = event_type_to_name(DUIET_dblclick);
			//event.ptMouse = pt;
			//event.wKeyState = wParam;
			//event.dwTimestamp = ::GetTickCount();
			////pControl->OnEvent(event);
			//BOOL bRet = FireEvent(pControl, event);
			m_pEventCaptured = pControl;
			// We always capture the mouse
			::SetCapture(m_hWndPaint);
			//return bRet;
		}
		break;
	case dbg_msg(WM_CHAR)//:
		{
			if( m_pFocus == NULL ) break;
			DuiEvent event = { 0 };
			event.Name = event_type_to_name(DUIET_char);
			event.chKey = wParam;
			event.ptMouse = m_ptLastMousePos;
			event.wKeyState = MapKeyState();
			event.dwTimestamp = ::GetTickCount();
			//m_pFocus->OnEvent(event);
			FireEvent(m_pFocus, event);
		}
		break;
	case dbg_msg(WM_KEYDOWN)//:
		{
			if( m_pFocus == NULL ) break;
			DuiEvent event = { 0 };
			event.Name = event_type_to_name(DUIET_keydown);
			event.chKey = wParam;
			event.ptMouse = m_ptLastMousePos;
			event.wKeyState = MapKeyState();
			event.dwTimestamp = ::GetTickCount();
			//m_pFocus->OnEvent(event);
			FireEvent(m_pFocus, event);
			m_pEventKey = m_pFocus;
		}
		break;
	case dbg_msg(WM_KEYUP)//:
		{
			if( m_pEventKey == NULL ) break;
			DuiEvent event = { 0 };
			event.Name = event_type_to_name(DUIET_keyup);
			event.chKey = wParam;
			event.ptMouse = m_ptLastMousePos;
			event.wKeyState = MapKeyState();
			event.dwTimestamp = ::GetTickCount();
			//m_pEventKey->OnEvent(event);
			FireEvent(m_pEventKey, event);
			m_pEventKey = NULL;
		}
		break;
	case dbg_msg(WM_SETCURSOR)//:
		{
			POINT pt = { 0 };
			::GetCursorPos(&pt);
			::ScreenToClient(m_hWndPaint, &pt);
			__AdjustPoint(&pt);
			DuiNode* pControl = FindControl(pt);
			if( pControl == NULL ) break;
			//if( (CDuiControlExtension::GetControlFlags(pControl) & DUIFLAG_SETCURSOR) == 0 ) break;
			HCURSOR cs = FromStyleVariant((pControl)->GetStyleAttribute(SAT_CURSOR), HCURSOR);
			//if (cs == NULL) cs = LoadCursor(NULL, IDC_ARROW); //break;
			::SetCursor((cs && !(pControl)->PtInScrollbar(pt)) ? cs : LoadCursor(NULL, IDC_ARROW));
		}
		dbg_tickcount();
		return TRUE;
	case dbg_msg(WM_CTLCOLOREDIT)//:
		{
			::DefWindowProc(m_hWndPaint, uMsg, wParam, lParam);
			HDC hDC = (HDC) wParam;
			::SetTextColor(hDC, RGB(0,0,0));
			::SetBkColor(hDC, RGB(255,255,255));
			lRes = (LRESULT) RGB(255,255,255);
		}
		dbg_tickcount();
		return TRUE;
	case dbg_msg(WM_MEASUREITEM)//:
		{
			if( wParam == 0 ) break;
			HWND hWndChild = ::GetDlgItem(m_hWndPaint, ((LPMEASUREITEMSTRUCT) lParam)->CtlID);
			lRes = ::SendMessage(hWndChild, OCM__BASE + uMsg, wParam, lParam);
			dbg_tickcount();
			return TRUE;
		}
		break;
	case dbg_msg(WM_DRAWITEM)//:
		{
			if( wParam == 0 ) break;
			HWND hWndChild = ((LPDRAWITEMSTRUCT) lParam)->hwndItem;
			lRes = ::SendMessage(hWndChild, OCM__BASE + uMsg, wParam, lParam);
			dbg_tickcount();
			return TRUE;
		}
		break;
		//case WM_VSCROLL:
		// {
		//  if( lParam == NULL ) break;
		//  //CDuiControlExtensionBase* pCtrl = static_cast<CDuiControlExtensionBase*>(::GetPropW((HWND) lParam, L"DirectUI"));
		//  //if( pCtrl == NULL ) break;
		//  if( m_pEventCaptured == NULL ) break;
		//  DuiEvent event = { (DUI_EVENTTYPE)0 };
		//  event.Name = DUIET_vscroll;
		//  event.wParam = wParam;
		//  event.lParam = lParam;
		//  event.dwTimestamp = ::GetTickCount();
		//  m_pEventCaptured->OnEvent(event);
		// }
		// break;
	case dbg_msg(WM_NOTIFY)//:
		{
			if ((HWND)wParam == m_hWndPaint) return TRUE;
			LPNMHDR lpNMHDR = (LPNMHDR) lParam;
			if( lpNMHDR != NULL ) lRes = ::SendMessage(lpNMHDR->hwndFrom, OCM__BASE + uMsg, wParam, lParam);
			dbg_tickcount();
			return TRUE;
		}
		break;
	case dbg_msg(WM_COMMAND)//:
		{
			if( lParam == 0 ) break;
			HWND hWndChild = (HWND) lParam;
			lRes = ::SendMessage(hWndChild, OCM__BASE + uMsg, wParam, lParam);
			dbg_tickcount();
			return TRUE;
		}
		break;
	default:
		dbg(" - 0x%08x(%d)", uMsg, uMsg);
		// Handle WM_MOUSEWHEEL
		if( (uMsg == guMsgMouseWheel || uMsg == WM_MOUSEWHEEL/*0x020A*/) && m_pFocus != NULL )
		{
			//::GetCursorPos(&m_ptLastMousePos);
			//::ScreenToClient(m_hWndPaint, &m_ptLastMousePos);
			// 找光标下的有滚动条的控件，如果没有则找活动控件
			DuiNode* pDst = FindControl(m_ptLastMousePos);
			while (pDst && !pDst->GetScrollbar()->IsValid()) pDst = pDst->GetOffsetParent(); //rt(pDst)->GetParent();
			if (pDst == NULL) pDst = m_pFocus;

			int zDelta = (int) (short) HIWORD(wParam);
			DuiEvent event = { 0 };
			event.Name = event_type_to_name(DUIET_scrollwheel);
			event.wParam = MAKELPARAM(zDelta < 0 ? SB_LINEDOWN : SB_LINEUP, 0);
			event.lParam = lParam;
			event.dwTimestamp = ::GetTickCount();
			//pDst->OnEvent(event);
			FireEvent(pDst, event);
			// Simulate regular scrolling by sending scroll events
			event.Name = event_type_to_name(DUIET_vscroll);
			for( int i = 0; i < abs(zDelta); i += 40 ) FireEvent(pDst, event);
			//for( int i = 0; i < 3; i++ ) pDst->OnEvent(event); // 发送3次LINEDOWN或LINEUP
			// Let's make sure that the scroll item below the cursor is the same as before...
			POINT pt = m_ptLastMousePos;
			//if (__IsAlpha()) CLayerHelper::OffsetPointDec(m_hWndPaint, &pt);
			::SendMessage(m_hWndPaint, WM_MOUSEMOVE, 0, (LPARAM) MAKELPARAM(pt.x, pt.y));
		}
		break;
	}
	if (__IsAlphaOrDD()) __Draw();
	dbg_tickcount();
	return FALSE;
}

void CDuiLayoutManager::UpdateLayout(BOOL bForce/*=FALSE*/)
{
	if (m_bDestroying || m_lPaintLocked>0 || m_pRoot==NULL) return;

	m_bResizeNeeded = TRUE;
	if (__IsAlphaOrDD())
	{
		CRect rc = m_pRoot->GetPos(DUIRT_TOTAL);
		if (bForce || !rc.EqualRect(&GetClientRect()))
		{
			DisableOp dop(m_pRoot, DUIDISABLED_INVALIDATE);
			m_pRoot->SetPos(GetClientRect());
			//CDuiControlExtension::UpdateLayout(m_pRoot);
		}
		//else CDuiControlExtension::UpdateLayout(m_pRoot);
		//if( m_bResizeNeeded )
		//{
		//	CRect rcWin, rc = GetClientRect();
		//	::GetWindowRect(m_hWndPaint, &rcWin);
		//	if( !rc.IsRectEmpty())
		//	{
		//		m_pRoot->SetPos(rc);
		//		//HDC hDC = ::CreateCompatibleDC(m_hDcPaint);
		//		//InDrawData idd;
		//		//m_pRoot->InitDrawData(hDC, idd);
		//		//m_pRoot->DoPaint(hDC, rcClient, FALSE, idd);
		//		//::DeleteDC(hDC);
		//		m_bResizeNeeded = FALSE;
		//		// We'll want to notify the window when it is first initialized
		//		// with the correct layout. The window form would take the time
		//		// to submit swipes/animations.
		//		if( m_bFirstLayout )
		//		{
		//			m_bFirstLayout = FALSE;
		//			SendNotify(m_pRoot, _T("windowinit"));
		//		}
		//	}
		//	// Reset offscreen device
		//	//if( m_hDcOffscreen != NULL ) ::DeleteDC(m_hDcOffscreen);
		//	//if( m_hbmpOffscreen != NULL ) ::DeleteObject(m_hbmpOffscreen);
		//	//m_hDcOffscreen = NULL;
		//	//m_hbmpOffscreen = NULL;
		//}
		m_bInvalid = TRUE;
		::InvalidateRect(m_hWndPaint, NULL, FALSE);
		//__Draw();
	}
	//else
		//::InvalidateRect(m_hWndPaint, NULL, FALSE);
}

void CDuiLayoutManager::Invalidate(LPRECT lpRect, BOOL bForce/*=FALSE*/)
{
	if (__IsAlphaOrDD())
	{
		m_bInvalid = TRUE;
		::InvalidateRect(m_hWndPaint, lpRect, FALSE);
#ifdef GDI_ONLY
		__PrepareDC();
#else
#ifndef NO3D
		if (Get3DDevice()==NULL)
#endif
			__PrepareSurface();
#endif // GDI_ONLY
		//if (m_pBits)
		//{
		//	CRect rcWin;
		//	::GetWindowRect(m_hWndPaint, &rcWin);
		//	for (int y=rcItem.top; y<rcItem.bottom; y++)
		//		for (int x=rcItem.left; x<rcItem.right; x++)
		//		{
		//			LPDWORD pBits = m_pBits + y * rcWin.Width() + x;
		//			*pBits = 0xBF3F3F3F; //000000;
		//		}
		//}
		if (bForce) __Draw();
	}
	//else
		//::InvalidateRect(m_hWndPaint, &rcItem, FALSE);
}

BOOL CDuiLayoutManager::AttachControl(DuiNode* pControl)
{
	if (m_pRoot == pControl) return TRUE;
	ASSERT(::IsWindow(m_hWndPaint));
	// Reset any previous attachment
	SetFocus(NULL);
	m_pEventKey = NULL;
	m_pEventHover = NULL;
	m_pEventCaptured = NULL;
	// Remove the existing control-tree. We might have gotten inside this function as
	// a result of an event fired or similar, so we cannot just delete the objects and
	// pull the internal memory of the calling code. We'll delay the cleanup.
	if( m_pRoot != NULL ) {
		m_aDelayedCleanup.Add(m_pRoot);
		::PostMessage(m_hWndPaint, WM_APP + 1, 0, 0L);
	}
	// Set the dialog root element
	m_pRoot = pControl;
	//SetFocus(m_pRoot);
	m_pFocus = m_pRoot;
	// Go ahead...
	m_bResizeNeeded = TRUE;
	m_bFirstLayout = TRUE;
	m_bFocusNeeded = TRUE;
	// Initiate all control
	return InitControls(pControl);
}

BOOL CDuiLayoutManager::InitControls(DuiNode* pControl, DuiNode* pParent /*= NULL*/)
{
	//ASSERT(pControl);
	//if( pControl == NULL ) return FALSE;
	//rt(pControl)->SetManager(this, pParent);
	return TRUE;
}

void CDuiLayoutManager::ReapObjects(DuiNode* pControl)
{
	DELEGATE_TO_OWNER(ReapObjects, (pControl));

	if( pControl == m_pEventKey ) m_pEventKey = NULL;
	if( pControl == m_pEventHover ) m_pEventHover = NULL;
	if( pControl == m_pEventCaptured ) m_pEventCaptured = NULL;
	fire_control_unload_events(pControl, FALSE);
}

void CDuiLayoutManager::MessageLoop()
{
	MSG msg = { 0 };
	while( ::GetMessage(&msg, NULL, 0, 0) ) {
		if( !CDuiLayoutManager::PreTranslateMessage(&msg) ) {
			::TranslateMessage(&msg);
			::DispatchMessage(&msg);
		}
	}
}

BOOL CDuiLayoutManager::PreTranslateMessage(const LPMSG pMsg)
{
	// Pretranslate Message takes care of system-wide messages, such as
	// tabbing and shortcut key-combos. We'll look for all messages for
	// each window and any child control attached.
	if (!::IsWindow(pMsg->hwnd)) return FALSE;
	HWND hwndParent = ::GetParent(pMsg->hwnd);
	UINT uStyle = CWindow(pMsg->hwnd).GetStyle();
	LRESULT lRes = 0;
	// mousewheel hack
	if (pMsg->message == guMsgMouseWheel) pMsg->message = WM_MOUSEWHEEL;

	for( int i = 0; i < m_aPreMessages.GetSize(); i++ ) {
		CDuiLayoutManager* pT = static_cast<CDuiLayoutManager*>(m_aPreMessages[i]);
		if( pMsg->hwnd == pT->GetPaintWindow()
			|| (hwndParent == pT->GetPaintWindow() && ((uStyle & WS_CHILD) != 0)) )
		{
			if ( pT->PreMessageHandler(pMsg) ) return TRUE;
		}
	}
	return FALSE;
}

DuiNode* CDuiLayoutManager::GetFocus() const
{
	return m_pFocus;
}

void CDuiLayoutManager::SetFocus(DuiNode* pControl)
{
	// Paint manager window has focus?
	if( ::GetFocus() != m_hWndPaint ) ::SetFocus(m_hWndPaint);
	//if (pControl==NULL && m_pRoot && (m_pRoot->GetControlFlags()&DUIFLAG_TABSTOP))
	//	pControl = m_pRoot;
	// Already has focus?
	if( pControl == m_pFocus ) return;
	// Set focus to new control
	if( pControl != NULL 
		&& (pControl)->m_pLayoutMgr == this 
		&& (pControl)->IsVisible(TRUE) 
		&& (pControl)->IsEnabled(TRUE) ) 
	{
		// Remove focus from old control
		if( m_pFocus != NULL ) 
		{
#ifndef NO3D
			if (Get3DDevice())
				m_pDevice->DestroyCaret();
			else
#endif
				::DestroyCaret();

			DuiEvent event = { 0 };
			event.Name = event_type_to_name(DUIET_killfocus);
			event.pSender = DuiHandle<DuiNode>(pControl);
			event.dwTimestamp = ::GetTickCount();
			//m_pFocus->OnEvent(event);
			FireEvent(m_pFocus, event);
			SendNotify(m_pFocus, _T("killfocus"));
			m_pFocus = NULL;
		}

		m_pFocus = pControl;
		DuiEvent event = { 0 };
		event.Name = event_type_to_name(DUIET_setfocus);
		event.pSender = DuiHandle<DuiNode>(pControl);
		event.dwTimestamp = ::GetTickCount();
		//m_pFocus->OnEvent(event);
		FireEvent(m_pFocus, event);
		SendNotify(m_pFocus, _T("setfocus"));
	}
}

DuiNode* CDuiLayoutManager::GetCapture() const
{
	return m_pEventCaptured;
}

void CDuiLayoutManager::SetCapture(DuiNode* pCtrl)
{
	m_pEventCaptured = pCtrl;
	if (pCtrl)
	{
		::SetFocus(m_hWndPaint);
		pCtrl->SetFocus();
		// We always capture the mouse
		::SetCapture(m_hWndPaint);
	}
	else
		::ReleaseCapture();
}

BOOL CDuiLayoutManager::SetTimer(DuiNode* pControl, UINT nTimerID, UINT uElapse)
{
	ASSERT(pControl!=NULL);
	ASSERT(uElapse>0);
	m_uTimerID = (++m_uTimerID) % 0xFF;
	if( !::SetTimer(m_hWndPaint, m_uTimerID, uElapse, NULL) ) return FALSE;
	TIMERINFO* pTimer = NEW TIMERINFO;
	if( pTimer == NULL ) return FALSE;
	pTimer->hWnd = m_hWndPaint;
	pTimer->pSender = pControl;
	pTimer->nLocalID = nTimerID;
	pTimer->uWinTimer = m_uTimerID;
	return !!m_aTimers.Add(pTimer);
}

BOOL CDuiLayoutManager::KillTimer(DuiNode* pControl, UINT nTimerID)
{
	ASSERT(pControl!=NULL);
	for( int i = 0; i< m_aTimers.GetSize(); i++ ) {
		TIMERINFO* pTimer = static_cast<TIMERINFO*>(m_aTimers[i]);
		if( pTimer->pSender == pControl
			&& pTimer->hWnd == m_hWndPaint
			&& pTimer->nLocalID == nTimerID )
		{
			::KillTimer(pTimer->hWnd, pTimer->uWinTimer);
			delete pTimer;
			return !!m_aTimers.RemoveAt(i);
		}
	}
	return FALSE;
}

BOOL CDuiLayoutManager::SetNextTabControl(BOOL bForward)
{
	// If we're in the process of restructuring the layout we can delay the
	// focus calulation until the next repaint.
	//if( m_bResizeNeeded && bForward )
	//{
	//	m_bFocusNeeded = TRUE;
	//	//Invalidate(GetClientRect());
	//	::InvalidateRect(m_hWndPaint, NULL, FALSE);
	//	return TRUE;
	//}
	// Find next/previous tabbable control
	FINDTABINFO info1 = { 0 };
	info1.pFocus = m_pFocus;
	info1.bForward = bForward;
	DuiNode* pControl = m_pRoot->FindControl(__FindControlFromTab, &info1, DUIFIND_VISIBLE | DUIFIND_ENABLED | DUIFIND_ME_FIRST);
	if( pControl == NULL )
	{
		if( bForward )
		{
			// Wrap around
			FINDTABINFO info2 = { 0 };
			info2.pFocus = bForward ? NULL : info1.pLast;
			info2.bForward = bForward;
			pControl = m_pRoot->FindControl(__FindControlFromTab, &info2, DUIFIND_VISIBLE | DUIFIND_ENABLED | DUIFIND_ME_FIRST);
		}
		else
			pControl = info1.pLast;
	}
	if( pControl != NULL ) SetFocus(pControl);
	m_bFocusNeeded = FALSE;
	return TRUE;
}

DuiSystemSettings CDuiLayoutManager::GetSystemSettings()
{
	return m_SystemConfig;
}

void CDuiLayoutManager::SetSystemSettings(const DuiSystemSettings Config)
{
	m_SystemConfig = Config;
}

DuiSystemMetrics CDuiLayoutManager::GetSystemMetrics()
{
	return m_SystemMetrics;
}

BOOL CDuiLayoutManager::AddNotifier(CDuiNotifyListener* pNotifier)
{
	ASSERT(m_aNotifiers.Find(pNotifier)<0);
	return !!m_aNotifiers.Add(pNotifier);
}

BOOL CDuiLayoutManager::RemoveNotifier(CDuiNotifyListener* pNotifier)
{
	for( int i = 0; i < m_aNotifiers.GetSize(); i++ ) {
		if( static_cast<CDuiNotifyListener*>(m_aNotifiers[i]) == pNotifier ) {
			return !!m_aNotifiers.RemoveAt(i);
		}
	}
	return FALSE;
}

BOOL CDuiLayoutManager::AddMessageFilter(CDuiMessageFilter* pFilter)
{
	ASSERT(m_aMessageFilters.Find(pFilter)<0);
	return !!m_aMessageFilters.Add(pFilter);
}

BOOL CDuiLayoutManager::RemoveMessageFilter(CDuiMessageFilter* pFilter)
{
	for( int i = 0; i < m_aMessageFilters.GetSize(); i++ ) {
		if( static_cast<CDuiMessageFilter*>(m_aMessageFilters[i]) == pFilter ) {
			return !!m_aMessageFilters.RemoveAt(i);
		}
	}
	return FALSE;
}

void CDuiLayoutManager::SendNotify(DuiNode* pControl, LPCOLESTR pstrMessage, WPARAM wParam /*= 0*/, LPARAM lParam /*= 0*/)
{
	DuiNotify Msg;
	Msg.pSender = pControl;
	Msg.sType = pstrMessage;
	Msg.wParam = 0;
	Msg.lParam = 0;
	SendNotify(Msg);
}

#define __foreach_event_notify(V) \
	V(mousemove) \
	V(mouseleave)	/*V(mouseout)*/ \
	V(mouseenter)	/*V(mouseover)*/ \
	V(mousehover) \
	V(keydown) \
	V(keyup) \
	V(char) \
	V(syskey) \
	V(killfocus)	/*V(blur)*/ \
	V(setfocus)		/*V(focus)*/ \
	V(buttondown) \
	V(buttonup) \
	V(click) \
	V(dblclick) \
	V(contextmenu) \
	V(scrollwheel) \
	V(size) \
	V(timer) \
	V(load) \
	V(unload)

DUINM_CODE event_to_nmcode(LPCOLESTR eventName)
{
	if (HIWORD(eventName)) return (DUINM_CODE)(UINT_PTR)eventName;

#define __for_eventtype(name) case DUIET_##name: return nmc_##name;
	switch ((DUI_EVENTTYPE)LOWORD(eventName))
	{
		__foreach_event_notify(__for_eventtype)
	}
#undef __for_eventtype
	return nmc__unknown;
}

BOOL CDuiLayoutManager::FireEvent(DuiNode* pCtrl, DuiEvent& event)
{
	DELEGATE_TO_OWNER(FireEvent, (pCtrl, event));

	if (pCtrl==NULL && event.pSender==NULL) return TRUE;
	if (pCtrl == NULL)
		pCtrl = DuiHandle<DuiNode>(event.pSender);
	else if (event.pSender == NULL)
		event.pSender = DuiHandle<DuiNode>(pCtrl);

	DuiNode* r = (pCtrl);
	if (r->IsExtensionEvent(event) || event.returnValue) return TRUE;

	//ISinkIsMyEvent* pSink = CLASS_CAST(pCtrl, ISinkIsMyEvent);
	//if (!pSink || !pSink->IsMyEvent(pCtrl->Handle(), event))
	//	if (!event.returnValue)
			r->OnEvent(event);

	if (event.returnValue) return TRUE;
	// 阻止某些消息
	if ((!r->IsEnabled(TRUE) || !r->IsVisible(TRUE)) &&
		((event.Name>=event_type_to_name(DUIET_MOUSEBEGIN) && event.Name<=event_type_to_name(DUIET_MOUSEEND)) ||
		(event.Name>=event_type_to_name(DUIET_KEYBEGIN) && event.Name<=event_type_to_name(DUIET_KEYEND))))
		return TRUE;

	// 发送通知消息
	if (DuiHandle<DuiNode>(pCtrl)==event.pSender)
	{
		DUINM_CODE nmc = event_to_nmcode(event.Name);
		if (nmc != nmc__unknown)
		{
			DUINMHDR nmh = { {m_hWndPaint, (UINT_PTR)event.pSender, (UINT)nmc},
				event.dwTimestamp, event.ptMouse, event.chKey, event.wKeyState, event.pPropertyName, event.returnValue, event.cancelBubble, event.wParam, event.lParam, 0 };
			// 首先尝试发送给父窗口（同线程）---是否应改成同进程？
			HWND hwndParent = ::GetParent(m_hWndPaint);
			if (hwndParent && ::GetWindowThreadProcessId(hwndParent,NULL)==::GetWindowThreadProcessId(m_hWndPaint,NULL))
			{
				::SendMessage(hwndParent, WM_NOTIFY, (WPARAM)m_hWndPaint, (LPARAM)&nmh);
			}
			// 发送给DUI窗口本身
			if (!nmh.bHandled)
				::SendMessage(m_hWndPaint, WM_NOTIFY, (WPARAM)m_hWndPaint, (LPARAM)&nmh);

			if (nmh.bCancelBubble) event.cancelBubble = TRUE;
			if (nmh.bHandled)
				return event.returnValue=TRUE;
		}
	}

	// 现在处理外部事件调用
	//SetEvent(&event); // 让外部的事件回调能从根对象中获取事件对象
#ifndef NO3D
	d3d::SetActiveScriptWindow(m_hWndPaint);
#endif
	DuiEvent* pOldEvent = m_pEvent;
	m_pEvent = &event;
	if (r->m_fnEventCallback)
		r->m_fnEventCallback(DuiHandle<DuiNode>(pCtrl), event);
	//SetEvent(NULL);
	m_pEvent = pOldEvent;
#ifndef NO3D
	d3d::SetActiveScriptWindow(NULL);
#endif

	// 现在处理事件冒泡，注意非内置事件不能冒泡
	if( r->m_pParent && !event.cancelBubble && !event.returnValue && DuiNode::NameToEventType(event.Name)!=DUIET__INVALID )
		return FireEvent(r->m_pParent, event);
	return FALSE;
}

void CDuiLayoutManager::SendNotify(DuiNotify& Msg)
{
	// Pre-fill some standard members
	Msg.ptMouse = m_ptLastMousePos;
	Msg.dwTimestamp = ::GetTickCount();
	// Allow sender control to react
	//// 只有内置对象才有接收通知功能，因为CDuiControlExtension是内置控件的基类，外置控件是从 CDuiControlExtensionBase 继承的
	//ControlExtension* pSrc = CLASS_CAST(Msg.pSender, ControlExtension);
	//if (pSrc)
	////if (Msg.pSender->GetModuleHandle() == _AtlBaseModule.GetModuleInstance())
	//{
	//	//CDuiControlExtension* pSrc = (CDuiControlExtension*)Msg.pSender;
	//	pSrc->OnNotify(Msg);
	//}
	// Send to all listeners
	for( int i = 0; i < m_aNotifiers.GetSize(); i++ ) {
		static_cast<CDuiNotifyListener*>(m_aNotifiers[i])->OnNotify(Msg);
	}
}

DuiNode* CDuiLayoutManager::FindControl(LPCOLESTR pstrId)
{
	if (m_pRoot == NULL) return NULL;;
	if (pstrId == NULL) return NULL;
	return m_pRoot->FindControl(__FindControlFromId, (LPVOID)pstrId, DUIFIND_ALL);
}

DuiNode* CDuiLayoutManager::FindControl(POINT pt) const
{
	if (m_pRoot == NULL) return NULL;

	// 搜索正值的Z-ORDER
	DuiNode* pCtrl = COrderListFinder<false,true>::Find(m_fixedChildren, __FindControlFromPoint, &pt, DUIFIND_VISIBLE | DUIFIND_HITTEST);
	if (pCtrl) return pCtrl;

	pCtrl = m_pRoot->FindControl(__FindControlFromPoint, &pt, DUIFIND_VISIBLE | DUIFIND_HITTEST);
	if (pCtrl) return pCtrl;

	// 搜索负值的Z-ORDER
	return COrderListFinder<true,false>::Find(m_fixedChildren, __FindControlFromPoint, &pt, DUIFIND_VISIBLE | DUIFIND_HITTEST);
}

HDE CALLBACK CDuiLayoutManager::__FindControlFromCount(HDE /*pThis*/, LPVOID pData)
{
	int* pnCount = static_cast<int*>(pData);
	(*pnCount)++;
	return NULL;  // Count all controls
}

HDE CALLBACK CDuiLayoutManager::__FindControlFromTab(HDE pThis, LPVOID pData)
{
	FINDTABINFO* pInfo = static_cast<FINDTABINFO*>(pData);
	DuiNode* r = DuiHandle<DuiNode>(pThis);
	if( pInfo->pFocus == r ) {
		if( pInfo->bForward ) pInfo->bNextIsIt = TRUE;
		return pInfo->bForward ? NULL : pInfo->pLast;
	}
	if( (r->GetControlFlags() & DUIFLAG_TABSTOP) == 0 ) return NULL;
	pInfo->pLast = r;
	if( pInfo->bNextIsIt ) return pThis;
	if( pInfo->pFocus == NULL ) return pThis;
	return NULL;  // Examine all controls
}

HDE CALLBACK CDuiLayoutManager::__FindControlFromId(HDE pThis, LPVOID pData)
{
	CDuiLayoutManager* pManager = static_cast<CDuiLayoutManager*>(pData);
	LPCOLESTR pId = (LPCOLESTR)pData;
	// No name?
	DuiNode* n = DuiNode::FromHandle(pThis);
	LPCOLESTR szId = n->m_id; //GetID();
	if (szId && (pId==szId || lstrcmpiW(pId, szId)==0)) return pThis;
	return NULL;
}

HDE CALLBACK CDuiLayoutManager::__FindControlFromShortcut(HDE pThis, LPVOID pData)
{
	FINDSHORTCUT* pFS = static_cast<FINDSHORTCUT*>(pData);
	if( pFS->ch == toupper(DuiHandle<DuiNode>(pThis)->GetShortcut()) ) pFS->bPickNext = TRUE;
	//if( _tcsstr((pThis)->GetName(), _T("Label")) != NULL ) return NULL;   // Labels never get focus!
	return pFS->bPickNext ? pThis : NULL;
}

HDE CALLBACK CDuiLayoutManager::__FindControlFromPoint(HDE pThis, LPVOID pData)
{
	LPPOINT pPoint = static_cast<LPPOINT>(pData);
	//return pThis->PtInControl(*pPoint) ? pThis : NULL;
	return DuiHandle<DuiNode>(pThis)->PtInBackground(*pPoint) ? pThis : NULL; // 执行更精细的判断
}

CDuiStyle* CDuiLayoutManager::CreateStyle( LPCOLESTR lpszTargetName/*=NULL*/, LPCOLESTR lpszTargetId/*=NULL*/, DWORD dwTargetState/*=0*/, DuiNode* pOwnerCtrl/*=NULL*/, CDuiStyle* pRefStyle/*=NULL*/ )
{
	DELEGATE_TO_OWNER(CreateStyle, (lpszTargetName,lpszTargetId, dwTargetState, pOwnerCtrl, pRefStyle));

	// 不允许外部提供参考风格
	if (pRefStyle && !HasStyle(pRefStyle))
		pRefStyle = NULL;

	if (pOwnerCtrl == NULL)
	{
		// 寻找已存在的风格。如果已经存在，后两个参数无效
		CDuiStyle* pBase = GetStyle(lpszTargetName, lpszTargetId, dwTargetState);
		if (pBase)
			return pBase;
	}

	CDuiStyle* pStyle = NEW CDuiStyle(this, lpszTargetName, lpszTargetId, dwTargetState, pOwnerCtrl, pRefStyle);
	if (pStyle)
	{
		m_aStyles.Add(pStyle);
		UpdateStylesInherit();
	}
	return pStyle;
}

void CDuiLayoutManager::RemoveStyle(CDuiStyle* pStyle)
{
	DELEGATE_TO_OWNER(RemoveStyle, (pStyle));

	m_aStyles.Remove(pStyle);
	delete pStyle;
	UpdateStylesInherit();
}

BOOL CDuiLayoutManager::HasStyle( CDuiStyle* pStyle )
{
	DELEGATE_TO_OWNER(HasStyle, (pStyle));
	return m_aStyles.Find(pStyle) >= 0;
}

CDuiStyle* CDuiLayoutManager::GetStyle(LPCOLESTR lpszTargetName/* =NULL */, LPCOLESTR lpszTargetId/* =NULL */, DWORD dwTargetState/* =0 */, BOOL bAutoCreate/* =FALSE */,
										   DuiStyleAttributeType satFor/*=SAT__UNKNOWN*/, DuiNode* pOwner/*=NULL*/)
{
	DELEGATE_TO_OWNER(GetStyle, (lpszTargetName,lpszTargetId,dwTargetState,bAutoCreate,satFor,pOwner));
	// 首先寻找状态完全相同的
	for (int i=0; i<m_aStyles.GetSize(); i++)
	{
		CDuiStyle* pStyle = (CDuiStyle*)m_aStyles[i];
		if (pOwner && pStyle->GetOwnerControl() && pOwner!=pStyle->GetOwnerControl()) continue;
		LPCOLESTR tname = pStyle->GetTargetName();
		LPCOLESTR tid = pStyle->GetTargetId();
		DWORD tstate = pStyle->GetTargetState();
		if (//(lpszTargetName==NULL || pStyle->IsTargetNameSame(lpszTargetName, TRUE)) &&
			(lpszTargetName==tname || (lpszTargetName && tname && lstrcmpiW(lpszTargetName,tname)==0)) &&
			(lpszTargetId==tid || (lpszTargetId && tid && lstrcmpiW(lpszTargetId,tid)==0)) &&
			(dwTargetState==tstate) &&
			(satFor==SAT__UNKNOWN || pStyle->GetPrivateAttribute(satFor)))
			return pStyle;
	}

	// 再寻找局部状态相同的
	if (!bAutoCreate && dwTargetState!=0)
	for (int i=0; i<m_aStyles.GetSize(); i++)
	{
		CDuiStyle* pStyle = (CDuiStyle*)m_aStyles[i];
		if (pOwner && pStyle->GetOwnerControl() && pOwner!=pStyle->GetOwnerControl()) continue;
		LPCOLESTR tname = pStyle->GetTargetName();
		LPCOLESTR tid = pStyle->GetTargetId();
		DWORD tstate = pStyle->GetTargetState();
		if (//(lpszTargetName==NULL || pStyle->IsTargetNameSame(lpszTargetName)) &&
			(lpszTargetName==tname || (lpszTargetName && tname && lstrcmpiW(lpszTargetName,tname)==0)) &&
			(lpszTargetId==tid || (lpszTargetId && tid && lstrcmpiW(lpszTargetId,tid)==0)) &&
			((dwTargetState&tstate)==dwTargetState) &&
			(satFor==SAT__UNKNOWN || pStyle->GetPrivateAttribute(satFor)))
			return pStyle;
	}

	// not found
	if (bAutoCreate)
	{
		CDuiStyle* pStyle = NEW CDuiStyle(this, lpszTargetName, lpszTargetId, dwTargetState, NULL, NULL);
		if (pStyle)
		{
			// 这里不计算所有父子关系
			m_aStyles.Add(pStyle);
		}
		return pStyle;
	}

	return NULL;
}

CDuiStyle* CDuiLayoutManager::GetStyleById(LPCOLESTR lpszId)
{
	DELEGATE_TO_OWNER(GetStyleById, (lpszId));

	if (lpszId==NULL || *lpszId==0)
		return NULL;

	// 优先精确匹配
	for (int i=0; i<m_aStyles.GetSize(); i++)
	{
		CDuiStyle* pStyle = (CDuiStyle*)m_aStyles[i];
		LPCOLESTR id = pStyle->GetId();
		if (id && lstrcmpW(id, lpszId)==0)
			return pStyle;
	}

	// 在没有精确匹配的情况下，尝试大小写不敏感的匹配
	for (int i=0; i<m_aStyles.GetSize(); i++)
	{
		CDuiStyle* pStyle = (CDuiStyle*)m_aStyles[i];
		LPCOLESTR id = pStyle->GetId();
		if (id && lstrcmpiW(id, lpszId)==0)
			return pStyle;
	}
	return NULL;
}

CDuiStyle* CDuiLayoutManager::__FindStyle(CDuiStyleSelector* proc, DuiNode* pCtrl, DuiStyleAttributeType satFor/* =SAT__UNKNOWN */)
{
	DELEGATE_TO_OWNER(__FindStyle, (proc,pCtrl,satFor));

	ATLASSERT(proc && pCtrl);

	DuiNode* rtCtrl = (pCtrl);

	LPCOLESTR name = (pCtrl)->m_tagName;
	LPCOLESTR id = rtCtrl->GetID();
	DWORD state = rtCtrl->GetState();

	CDuiStyle* pOwnerStyle = rtCtrl->GetPrivateStyle();
	CDuiStyle* pStyle = NULL;
	DWORD dw = 0;
	if (state==0) dw |= DUISTYLEMATCH_STATE;
	if (id==NULL) dw |= DUISTYLEMATCH_ID;
	if (state)
	{
		for (int i=0; i<32; i++)
		{
			DWORD state2 = (state & (0x00000001 << i));
			if (state2)
			{
				if (id && (pStyle=GetStyle(name, id, state2, FALSE, satFor, pCtrl)) && proc->SelectorHandler(pStyle,satFor,DUISTYLEMATCH_ALL|dw)) return pStyle;
				if (id && (pStyle=GetStyle(NULL, id, state2, FALSE, satFor, pCtrl)) && proc->SelectorHandler(pStyle,satFor,DUISTYLEMATCH_ID|DUISTYLEMATCH_STATE|dw)) return pStyle;
				if ((pStyle=GetStyle(name, NULL, state2, FALSE, satFor, pCtrl)) && proc->SelectorHandler(pStyle,satFor,DUISTYLEMATCH_NAME|DUISTYLEMATCH_STATE|dw)) return pStyle;
				if ((pStyle=GetStyle(NULL, NULL, state2, FALSE, satFor, pCtrl)) && proc->SelectorHandler(pStyle,satFor,DUISTYLEMATCH_STATE|dw)) return pStyle;
			}
		}
		// 都没发现，尝试后续不关注state
		state = 0;
	}

	if (!state)
	{
		if (pOwnerStyle && pOwnerStyle->GetPrivateAttribute(satFor) && proc->SelectorHandler(pOwnerStyle, satFor, DUISTYLEMATCH_ALL)) return pOwnerStyle;
		if (id && (pStyle=GetStyle(name, id, 0, FALSE, satFor, pCtrl)) && proc->SelectorHandler(pStyle,satFor,DUISTYLEMATCH_NAME|DUISTYLEMATCH_ID|dw)) return pStyle;
		if (id &&(pStyle=GetStyle(NULL, id, 0, FALSE, satFor, pCtrl)) && proc->SelectorHandler(pStyle,satFor,DUISTYLEMATCH_ID|dw)) return pStyle;
		if ((pStyle=GetStyle(name, NULL, 0, FALSE, satFor, pCtrl)) && proc->SelectorHandler(pStyle,satFor,DUISTYLEMATCH_NAME|dw)) return pStyle;
		if ((pStyle=GetStyle(NULL, NULL, 0, FALSE, satFor, pCtrl)) && proc->SelectorHandler(pStyle,satFor,dw)) return pStyle;
	}

	//ATLASSERT(FALSE);
	return NULL; // 这句应该永远不会执行到
}

CDuiStyle* CDuiLayoutManager::FindSimilarStyle(DuiNode* pCtrl, DWORD& dwMatch, DuiStyleAttributeType satFor/*=SAT__UNKNOWN*/)
{
	DELEGATE_TO_OWNER(FindSimilarStyle, (pCtrl,dwMatch,satFor));

	ATLASSERT(pCtrl);
	// 	if (pCtrl==NULL) return NULL;

	class style_callback : public CDuiStyleSelector
	{
	public:
		CDuiStyle* m_pStyle;
		DWORD m_dwMatch;
		style_callback() : m_pStyle(NULL), m_dwMatch(0) {}
		BOOL SelectorHandler(CDuiStyle* pTestStyle, DuiStyleAttributeType satFor, DWORD dwMatchResult)
		{
			ATLASSERT(pTestStyle);
			m_pStyle = pTestStyle;
			m_dwMatch = dwMatchResult;
			return TRUE;
		}
	};

	style_callback cb;
	__FindStyle(&cb, pCtrl, satFor);
	dwMatch = cb.m_dwMatch;
	return cb.m_pStyle;
}

CDuiStyle* CDuiLayoutManager::ParseStyle(LPCOLESTR lpszTargetName, DWORD dwTargetState, LPCOLESTR lpszStyle)
{
	DELEGATE_TO_OWNER(ParseStyle, (lpszTargetName, dwTargetState, lpszStyle));

	if (lpszStyle==NULL)
		return NULL;

	CDuiStyle* pStyle = GetStyle(lpszTargetName, NULL, dwTargetState, TRUE);
	ATLASSERT(pStyle);
	if (pStyle)
	{
		pStyle->ParseStyle(lpszStyle);
	}
	return pStyle;
}

void CDuiLayoutManager::InitCommonStyles()
{
	DELEGATE_TO_OWNER(DoNothing,());

	// 首先创建一个根风格
	CDuiStyle* pRoot = GetStyle(NULL, NULL, 0, TRUE);
	//CDuiStyle* pRoot = (CDuiStyle*)ParseStyle(NULL, 0, L"color:#000000"); // 这里提供根风格定义
	ATLASSERT(pRoot);

	// 再为每个控件扩展类创建一个默认风格
	for (ControlExtensionEntry** ppEntry = &__pctrlextEntryFirst; ppEntry < &__pctrlextEntryLast; ppEntry++)
	{
		if (*ppEntry)
		{
			ControlExtensionEntry* pEntry = *ppEntry;
			if (pEntry->pfnRegisterClassStyle)
				pEntry->pfnRegisterClassStyle(GetPaintWindow(), pEntry->szName);
		}
	}

	UpdateStylesInherit();
}

void CDuiLayoutManager::UpdateStylesInherit()
{
	DELEGATE_TO_OWNER(DoNothing,());

	for (int i=0; i<m_aStyles.GetSize(); i++)
	{
		CDuiStyle* pSrc = (CDuiStyle*)m_aStyles[i];
		pSrc->SetParentStyle(NULL);
	}

	// 采用类似冒泡排序的方式扫描所有风格并计算父子关系
	for (int i=0; i<m_aStyles.GetSize(); i++)
	{
		CDuiStyle* pSrc = (CDuiStyle*)m_aStyles[i];
		for (int j=i+1; j<m_aStyles.GetSize(); j++)
		{
			CDuiStyle* pDst = (CDuiStyle*)m_aStyles[j];
			int kindDst = pSrc->GetKindOf(pDst);
			if (kindDst == CDuiStyle::kind_child)
			{
				CDuiStyle* pTmpParent = pDst->GetParentStyle();
				if (pTmpParent==NULL || pTmpParent->GetKindOf(pSrc)==CDuiStyle::kind_child)
					pDst->SetParentStyle(pSrc);
			}
			else if (kindDst == CDuiStyle::kind_parent)
			{
				// 要向上回朔，把这个parent插在父子链的中间。拥有属主控件的风格（控件的私有风格）不能作为其它风格的父风格
				if (pDst->GetOwnerControl() == NULL) // 仅仅处理非私有风格
				{
					CDuiStyle* cc = pSrc;
					CDuiStyle* pp = pSrc->GetParentStyle();
					int kindDst2;
					while (pp && (kindDst2=pp->GetKindOf(pDst))==CDuiStyle::kind_parent)
					{
						cc = pp;
						pp = pp->GetParentStyle();
					}
					ATLASSERT(pp); // 因为有根风格的存在，根风格是任何风格的父亲

					// 现在 cc 应该作为目标风格的下级
					cc->SetParentStyle(pDst);

					// pp 还不能马上作为目标风格的上级，要继续往上找
					while (kindDst2 != CDuiStyle::kind_child)
					{
						pp = pp->GetParentStyle();
						ATLASSERT(pp);
						kindDst2 = pp->GetKindOf(pDst);
					}
					pDst->SetParentStyle(pp);
				}
			}
		}
	}
}

CDuiImageResource* CDuiLayoutManager::CreateImageResource(LPCOLESTR lpszUrl, LPCOLESTR lpszId/* =NULL */)
{
	DELEGATE_TO_OWNER(CreateImageResource, (lpszUrl,lpszId));

	return NEW CDuiImageResource(this, lpszUrl, lpszId);
}

CDuiImageResource* CDuiLayoutManager::GetImageResource(LPCOLESTR szIdorUrl, BOOL bAutoCreate/*=FALSE*/)
{
	DELEGATE_TO_OWNER(GetImageResource, (szIdorUrl, bAutoCreate));
	if (szIdorUrl==NULL || *szIdorUrl==0)
		return NULL;

	CDuiImageResource* r = m_Images;
	while (r)
	{
		if (r->IsIdOrUrl(szIdorUrl))
			return r;
		r = r->Next();
	}

	if (bAutoCreate)
		return CreateImageResource(szIdorUrl);

	return NULL;
}

//CDuiEffectResource* CDuiLayoutManager::CreateEffectResource( LPCOLESTR lpszUrl, LPCOLESTR lpszData/*=NULL*/, LPCOLESTR lpszId/*=NULL*/ )
//{
//	DELEGATE_TO_OWNER(CreateEffectResource, (lpszUrl,lpszData,lpszId));
//
//	return NEW CDuiEffectResource(this, lpszUrl, lpszData, lpszId);
//}
//
//CDuiEffectResource* CDuiLayoutManager::GetEffectResource( LPCOLESTR szIdorUrl, BOOL bAutoCreate/*=FALSE*/ )
//{
//	DELEGATE_TO_OWNER(GetEffectResource, (szIdorUrl, bAutoCreate));
//	if (szIdorUrl==NULL || *szIdorUrl==0)
//		return NULL;
//
//	CDuiEffectResource* e = m_Effects;
//	while (e)
//	{
//		if (e->IsIdOrUrl(szIdorUrl))
//			return e;
//		e = e->Next();
//	}
//
//	if (bAutoCreate)
//		return CreateEffectResource(szIdorUrl);
//
//	return NULL;
//}

BOOL CDuiLayoutManager::GetAttributeBool(LPCOLESTR szAttr, BOOL bDef/* =FALSE */) const
{
	return m_aAttrs.GetAttributeBool(szAttr, bDef);
}

DuiNode* CDuiLayoutManager::CreateControl(DuiNode* pParent, LPCOLESTR lpszName)
{
	if (lpszName==NULL)
		return NULL;

	// 删除前面的空白
	while (::iswspace(*lpszName)) lpszName++;
	if (*lpszName==0) return NULL;

	// 不能创建多个根元素
	if (m_pRoot && pParent==NULL)
		return NULL;

	// 如果这是一段XML代码，则创建代码，此时pParent不可以是NULL
	if (*lpszName == L'<' && pParent)
	{
		CComBSTR code = L"<root>";
		code.Append(lpszName);
		code.Append(L"</root>");
		CMarkup xml(code);
		if (xml.IsValid())
		{
			CMarkupNode root = xml.GetRoot();
			if (!root.IsValid()) return NULL;
			CMarkupNode node = root.GetChild();
			if (!node.IsValid()) return NULL;
			CounterLock cl(m_lPaintLocked);
			DuiNode* pCtrl = _AddChild(&node, pParent);
			return pCtrl;
		}
		return NULL;
	}

	// 这是一个标签名
	DuiNode* pCtrl = DuiNode::New(lpszName, pParent, this);
	if (pCtrl)
	{
		if (pParent==NULL) // create root
			AttachControl(pCtrl);
		else
		{
			if (pParent == TempParent) // push to cache
			{
				m_aDelayedCleanup.Add(pCtrl);
				::PostMessage(m_hWndPaint, WM_APP+1, 0, 0);
				pParent = NULL;
			}
			InitControls(pCtrl, pParent);
		}

		pCtrl->SetReadyState(READYSTATE_LOADING);

		DisableOp dop(pCtrl, DUIDISABLED_INVALIDATE | DUIDISABLED_UPDATELAYOUT | DUIDISABLED_UPDATELAYOUT_FROMCHILD);
		if (pParent)
		{
			// 在容器控件中添加此控件
			if (!pParent->AddChildControl(pCtrl))
			{
				pCtrl->DeleteThis();
				return NULL;
			}
		}

		// 控件节点刚刚创建，在解析属性之前先执行扩展初始化，以及添加控件扩展
		struct ae
		{
			// 默认回调
			static DuiVoid /*CALLBACK*/ OnAttachExtensions(CallContext* pcc, HDE hde, IExtensionContainer* pContainer)
			{
				DuiNode* n = DuiNode::FromHandle(hde);
				for (ControlExtensionEntry** ppEntry=&__pctrlextEntryFirst; ppEntry < &__pctrlextEntryLast; ppEntry++)
				{
					if (*ppEntry != NULL)
					{
						ControlExtensionEntry* pEntry = *ppEntry;
						if (lstrcmpiW(pEntry->szName, n->m_tagName)==0)
						{
							IExtension* pExt = pEntry->pfnCreateControlExtension();
							pContainer->AttachExtension(pExt);
							return 0;
						}
					}
				}
				return 0;
			}
		};
		HDE hde = pCtrl->Handle();
		PluginCallContext(FALSE, hde)											// 创建插件调用上下文
			.DefProc(ae::OnAttachExtensions)									// 设置默认过程
			.Call(OpCode_AttachExtensions, hde, (IExtensionContainer*)pCtrl);	// 调用插件
	}

	return pCtrl;
}

void CDuiLayoutManager::CreateControlDispatch( DuiNode* pCtrl )
{
	DELEGATE_TO_OWNER(CreateControlDispatch, (pCtrl));

	CComPtr<IDispatch> disp;
	if (pCtrl && m_pObjectCreator && SUCCEEDED(m_pObjectCreator->CreateControlObject(pCtrl, &disp)))
		(pCtrl)->SetObject(disp.p);
}

HRESULT CDuiLayoutManager::ParseProcedure( LPCOLESTR lpszCode, IDispatch** ppDisp )
{
	DELEGATE_TO_OWNER(ParseProcedure, (lpszCode,ppDisp));

	if (!m_script.IsValid()) return E_FAIL;
	if (lpszCode==NULL) return E_INVALIDARG;
	if (ppDisp==NULL) return E_POINTER;
	*ppDisp = NULL;

	//static int i = 0;
	//CStdString code = L"function code(){";
	////code.AppendFormat(L"%d(){", i++);
	//code += lpszCode;
	//code += L"}";

#ifndef NO3D
	d3d::SetActiveScriptWindow(m_hWndPaint);
#endif
	HRESULT hr = m_script.ParseProcedure(lpszCode, ppDisp, FALSE, TRUE);
#ifndef NO3D
	d3d::SetActiveScriptWindow(NULL);
#endif
	return hr;
}

HRESULT CDuiLayoutManager::ParseExpression( LPCOLESTR lpszCode, IDispatch** ppDisp )
{
	DELEGATE_TO_OWNER(ParseExpression, (lpszCode, ppDisp));

	if (!m_script.IsValid()) return E_FAIL;
	if (lpszCode==NULL) return E_INVALIDARG;
	if (ppDisp==NULL) return E_POINTER;
	*ppDisp = NULL;

#ifndef NO3D
	d3d::SetActiveScriptWindow(m_hWndPaint);
#endif
	HRESULT hr = m_script.ParseProcedure(lpszCode, ppDisp, TRUE, TRUE);
#ifndef NO3D
	d3d::SetActiveScriptWindow(NULL);
#endif
	return hr;
}

BOOL CDuiLayoutManager::BuildControlPen( DuiNode* pCtrl, LPLOGPEN pLogPen, CDuiStyleSelector* proc/*=NULL*/ )
{
	if (pLogPen==NULL) return FALSE;
#ifdef GDI_ONLY
	if (pCtrl==NULL) return ::GetObject((HPEN)::GetCurrentObject(GetPaintDC(), OBJ_PEN), sizeof(LOGPEN), pLogPen), TRUE;
#else
	//CSurfaceDC dc(GetSurface());
	CClientDC dc(NULL/*m_hWndPaint*/);
	if (pCtrl==NULL) return ::GetObject((HPEN)::GetCurrentObject(dc.m_hDC, OBJ_PEN), sizeof(LOGPEN), pLogPen), TRUE;
	//if (pCtrl==NULL) return ::GetObject((HPEN)::GetCurrentObject(GetPaintDC(), OBJ_PEN), sizeof(LOGPEN), pLogPen), TRUE;
#endif // GDI_ONLY

	class style_callback : public CDuiStyleSelector
	{
	public:
		CStdPtrArray styles;
		BOOL SelectorHandler(CDuiStyle* pTestStyle, DuiStyleAttributeType satFor, DWORD dwMatchResult)
		{
			ATLASSERT(pTestStyle);
			styles.Add(pTestStyle);
			return FALSE;
		}
	};

	style_callback* cb = NULL;
	if (proc==NULL)
	{
		proc = cb = NEW style_callback;
		pLogPen->lopnColor = RGB(0,0,0);
		pLogPen->lopnStyle = PS_SOLID;
		pLogPen->lopnWidth.x = 1;
		pLogPen->lopnWidth.y = 0;
	}
	//if (pCtrl->GetPrivateStyle() && ((CDuiStyle*)pCtrl->GetPrivateStyle())->GetPrivateAttribute(SAT_BORDER)) proc->SelectorHandler(pCtrl->GetPrivateStyle(), SAT_BORDER, 0);
	__FindStyle(proc, pCtrl, SAT_BORDER);
	//BuildControlPen(rt(pCtrl)->GetParent(), pLogPen, proc);
	if (cb == NULL) return TRUE; // 递归调用不处理下面的

	if (cb->styles.GetSize()==0) return delete cb, FALSE;

	for (int i=cb->styles.GetSize()-1; i>=0; i--)
	{
		CDuiStyle* pStyle = (CDuiStyle*)cb->styles[i];
		CDuiStyleVariant* pVar = pStyle->GetPrivateAttribute(SAT_BORDER, FALSE);
		if (pVar) StyleVariantGetPen(pVar, pLogPen);
	}
	return delete cb, TRUE;
}

BOOL CDuiLayoutManager::BuildControlFont( DuiNode* pCtrl, LPLOGFONTW pLogFont, CDuiStyleSelector* proc/*=NULL*/ )
{
	if (pLogFont==NULL) return FALSE;

	if (pCtrl==NULL) return ::GetObject((HFONT)::GetStockObject(DEFAULT_GUI_FONT), sizeof(LOGFONTW), pLogFont), TRUE;
	//pLogFont->lfQuality = ANTIALIASED_QUALITY;

	class style_callback : public CDuiStyleSelector
	{
	public:
		CStdPtrArray styles;
		BOOL SelectorHandler(CDuiStyle* pTestStyle, DuiStyleAttributeType satFor, DWORD dwMatchResult)
		{
			ATLASSERT(pTestStyle);
			styles.Add(pTestStyle);
			return FALSE;
		}
	};

	style_callback* cb = NULL;
	if (proc==NULL) proc = cb = NEW style_callback;
	//if (pCtrl->GetPrivateStyle() && ((CDuiStyle*)pCtrl->GetPrivateStyle())->GetPrivateAttribute(SAT_FONT)) proc->SelectorHandler(pCtrl->GetPrivateStyle(), SAT_FONT, 0);
	__FindStyle(proc, pCtrl, SAT_FONT);
	BuildControlFont((pCtrl)->GetParent(), pLogFont, proc);
	if (cb == NULL) return TRUE; // 递归调用不处理下面的

	if (cb->styles.GetSize()==0) return delete cb, FALSE;

	for (int i=cb->styles.GetSize()-1; i>=0; i--)
	{
		CDuiStyle* pStyle = (CDuiStyle*)cb->styles[i];
		CDuiStyleVariant* pVar = pStyle->GetPrivateAttribute(SAT_FONT, FALSE);
		if (pVar) StyleVariantGetFont(pVar, pLogFont);
	}
	return delete cb, TRUE;
}
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//
CDuiStyleVariant::CDuiStyleVariant()
{
	__cache = NULL;
	StyleVariantInit(this);
}

CDuiStyleVariant::CDuiStyleVariant( DuiStyleAttributeType _sat )
{
	__cache = NULL;
	StyleVariantInit(this);
	sat = _sat;
}

CDuiStyleVariant::CDuiStyleVariant( DuiStyleAttributeType _sat, BOOL bVal )
{
	__cache = NULL;
	sat = _sat;
	svt = SVT_BOOL;
	boolVal = bVal;
}

CDuiStyleVariant::CDuiStyleVariant( DuiStyleAttributeType _sat, LONG lVal )
{
	__cache = NULL;
	sat = _sat;
	svt = SVT_LONG;
	longVal = lVal;
}

CDuiStyleVariant::CDuiStyleVariant( DuiStyleAttributeType _sat, DOUBLE dVal )
{
	__cache = NULL;
	sat = _sat;
	svt = SVT_DOUBLE;
	doubleVal = dVal;
}

CDuiStyleVariant::CDuiStyleVariant( DuiStyleAttributeType _sat, LPCOLESTR lpszVal, DuiStyleVariantType _svt/*=SVT_STRING*/ )
{
	__cache = NULL;
	sat = _sat;
	svt = _svt;
	strVal = NULL;
	if (lpszVal)
	{
		int len = lstrlenW(lpszVal) + 1;
		strVal = NEW OLECHAR[len];
		if (strVal)
			lstrcpyW((LPWSTR)strVal, lpszVal);
	}
}

CDuiStyleVariant::CDuiStyleVariant( DuiStyleAttributeType _sat, COLORREF clr )
{
	__cache = NULL;
	sat = _sat;
	svt = SVT_COLOR;
	colorVal = clr;
}

CDuiStyleVariant::CDuiStyleVariant( DuiStyleAttributeType _sat, HPEN hPen )
{
	__cache = NULL;
	sat = _sat;
	svt = SVT_PEN;
	if (hPen)
	{
		penVal = NEW LOGPEN;
		::GetObject(hPen, sizeof(LOGPEN), penVal);
	}
}

CDuiStyleVariant::CDuiStyleVariant(DuiStyleAttributeType _sat, LOGPEN* pLogPen)
{
	__cache = NULL;
	sat = _sat;
	svt = SVT_PEN;
	penVal = NULL;

	if (pLogPen)
	{
		penVal = NEW LOGPEN;
		*penVal = *pLogPen;
	}
}

CDuiStyleVariant::CDuiStyleVariant( DuiStyleAttributeType _sat, HFONT hFont )
{
	__cache = NULL;
	sat = _sat;
	svt = SVT_FONT;
	if (hFont)
	{
		fontVal = NEW LOGFONTW;
		::GetObject(hFont, sizeof(LOGFONTW), fontVal);
	}
}

CDuiStyleVariant::CDuiStyleVariant(DuiStyleAttributeType _sat, LOGFONT* pLogFont)
{
	__cache = NULL;
	sat = _sat;
	svt = SVT_FONT;
	fontVal = NULL;

	if (pLogFont)
	{
		fontVal = NEW LOGFONT;
		*fontVal = *pLogFont;
	}
}

CDuiStyleVariant::CDuiStyleVariant( DuiStyleAttributeType _sat, HBRUSH hBrush )
{
	__cache = NULL;
	sat = _sat;
	svt = SVT_BRUSH;
	brushVal = hBrush;
}

CDuiStyleVariant::CDuiStyleVariant(DuiStyleAttributeType _sat, LOGBRUSH* pLogBrush)
{
	__cache = NULL;
	sat = _sat;
	svt = SVT_BRUSH;
	brushVal = NULL;

	if (pLogBrush)
	{
		LOGBRUSH* lb = NEW LOGBRUSH;
		*lb = *pLogBrush;
		__cache = (UINT_PTR)lb;
	}
}

CDuiStyleVariant::CDuiStyleVariant( DuiStyleAttributeType _sat, HICON hIcon, DuiStyleVariantType _svt/*=SVT_ICON*/ )
{
	ATLASSERT(_svt==SVT_ICON || _svt==SVT_CURSOR);
	__cache = NULL;
	sat = _sat;
	svt = _svt;
	iconVal = hIcon;
}

CDuiStyleVariant::CDuiStyleVariant( DuiStyleAttributeType _sat, HBITMAP hBitmap )
{
	__cache = NULL;
	sat = _sat;
	svt = SVT_BITMAP;
	bitmapVal = hBitmap;
}

CDuiStyleVariant::CDuiStyleVariant(DuiStyleAttributeType _sat, BITMAP* pBitmap)
{
	__cache = NULL;
	sat = _sat;
	svt = SVT_BITMAP;
	bitmapVal = NULL;

	if (pBitmap)
	{
		BITMAP* bmp = NEW BITMAP;
		*bmp = *pBitmap;
		__cache = (UINT_PTR)bmp;
	}
}

CDuiStyleVariant::CDuiStyleVariant(DuiStyleAttributeType _sat, LPCRECT pRect)
{
	__cache = NULL;
	sat = _sat;
	svt = SVT_RECT;
	rectVal = NULL;

	if (pRect)
	{
		rectVal = NEW RECT;
		*rectVal = *pRect;
	}
}

CDuiStyleVariant::CDuiStyleVariant(DuiStyleAttributeType _sat, LPSIZE pSize)
{
	__cache = NULL;
	sat = _sat;
	svt = SVT_SIZE;
	sizeVal = NULL;

	if (pSize)
	{
		sizeVal = NEW SIZE;
		*sizeVal = *pSize;
	}
}

CDuiStyleVariant::CDuiStyleVariant(DuiStyleAttributeType _sat, LPPOINT pPoint)
{
	__cache = NULL;
	sat = _sat;
	svt = SVT_POINT;
	pointVal = NULL;

	if (pPoint)
	{
		pointVal = NEW POINT;
		*pointVal = *pPoint;
	}
}

CDuiStyleVariant::CDuiStyleVariant(DuiStyleAttributeType _sat, LPVOID pExternal, UINT_PTR uiData)
{
	__cache = uiData;
	sat = _sat;
	svt = SVT_EXTERNAL;
	extVal = pExternal;
}

//
//CDuiStyleVariant::CDuiStyleVariant(DuiStyleAttributeType _sat, HCURSOR hCursor)
//{
//	sat = _sat;
//	svt = SVT_CURSOR;
//	cursorVal = hCursor;
//}

CDuiStyleVariant::~CDuiStyleVariant()
{
	DuiStyleVariantType _svt = svt;
	StyleVariantClear(this);
	sat = SAT__UNKNOWN;

	if (__cache)
	{
		if (_svt == SVT_PEN) ::DeleteObject((HPEN)__cache);
		else if (_svt == SVT_BRUSH) delete (LOGBRUSH*)__cache;
		else if (_svt == SVT_FONT) ::DeleteObject((HFONT)__cache);
		else if (_svt == SVT_BITMAP) delete (BITMAP*)__cache;
	}
	__cache = NULL;
}

//CDuiStyleVariant::operator LPLOGPEN()
//{
//	if ((svt&SVT_MASK_ALL) != SVT_PEN)
//		return NULL;
//	return penVal;
//}
//
//CDuiStyleVariant::operator HBRUSH()
//{
//	if ((svt&SVT_MASK_ALL) != SVT_BRUSH)
//		return NULL;
//
//	if (__cache)
//	{
//		if (brushVal) DeleteObject(brushVal);
//		brushVal = CreateBrushIndirect((const LOGBRUSH*)__cache);
//		delete (LOGBRUSH*)__cache;
//		__cache = NULL;
//	}
//	return brushVal;
//}
//
//CDuiStyleVariant::operator LPLOGFONTW()
//{
//	if ((svt&SVT_MASK_ALL) != SVT_FONT)
//		return NULL;
//	return fontVal;
//}
//
//CDuiStyleVariant::operator HBITMAP()
//{
//	if ((svt&SVT_MASK_ALL) != SVT_BITMAP)
//		return NULL;
//
//	if (__cache)
//	{
//		if (bitmapVal) DeleteObject(bitmapVal);
//		bitmapVal = CreateBitmapIndirect((const BITMAP*)__cache);
//		delete (BITMAP*)__cache;
//		__cache = NULL;
//	}
//	return bitmapVal;
//}

//HPEN CDuiStyleVariant::GetPen(LOGPEN* pRefPen)
//{
//	if (pRefPen==NULL || (svt&SVT_MASK_ALL)!=SVT_PEN)
//		return NULL;
//
//	if (penVal==NULL) return NULL;
//
//	if (svt & SVT_PEN_MASK_WIDTH) pRefPen->lopnWidth.x = penVal->lopnWidth.x;
//	if (svt & SVT_PEN_MASK_COLOR) pRefPen->lopnColor = penVal->lopnColor;
//	if (svt & SVT_PEN_MASK_STYLE) pRefPen->lopnStyle = penVal->lopnStyle;
//
//	if (__cache) ::DeleteObject((HPEN)__cache);
//	__cache = (UINT_PTR)::CreatePenIndirect(pRefPen);
//	return (HPEN)__cache;
//}

void CDuiStyleVariant::SetPen(LOGPEN* pRefPen, LONG mask/* =SVT_PEN_MASK_ALL */)
{
	if (pRefPen == NULL)
		return;

	if ((svt&SVT_MASK_ALL)!=SVT_EMPTY && (svt&SVT_MASK_ALL)!=SVT_PEN)
		return;

	svt = (DuiStyleVariantType)(SVT_PEN | mask);
	if (penVal == NULL) penVal = NEW LOGPEN;
	*penVal = *pRefPen;
}

BOOL CDuiStyleVariant::GetLogBrush(LOGBRUSH* pLogBrush)
{
	if (pLogBrush==NULL || (svt&SVT_MASK_ALL)!=SVT_BRUSH)
		return FALSE;

	if (__cache)
		return *pLogBrush = *(LOGBRUSH*)__cache, TRUE;

	if (brushVal==NULL || ::GetObjectType(brushVal)!=OBJ_BRUSH)
		return FALSE;

	return ::GetObject(brushVal, sizeof(LOGBRUSH), pLogBrush), TRUE;
}

//HFONT CDuiStyleVariant::GetFont(LOGFONTW* pRefFont)
//{
//	if (pRefFont==NULL || (svt&SVT_MASK_ALL)!=SVT_FONT)
//		return NULL;
//	if (fontVal==NULL) return NULL;
//
//	if (svt & SVT_FONT_MASK_FAMILY) lstrcpyW(pRefFont->lfFaceName, fontVal->lfFaceName);
//	if (svt & SVT_FONT_MASK_SIZE) pRefFont->lfHeight = fontVal->lfHeight, pRefFont->lfWidth = 0;
//	if (svt & SVT_FONT_MASK_STYLE) pRefFont->lfWeight=fontVal->lfWeight, pRefFont->lfUnderline=fontVal->lfUnderline, pRefFont->lfItalic=fontVal->lfItalic, pRefFont->lfStrikeOut=fontVal->lfStrikeOut;
//
//	//return NULL;
//	if (__cache) ::DeleteObject((HFONT)__cache);
//	__cache = (UINT_PTR)::CreateFontIndirectW(pRefFont);
//	return (HFONT)__cache;
//}

void CDuiStyleVariant::SetFont(LOGFONTW* pRefFont, LONG mask/* =SVT_FONT_MASK_ALL */)
{
	if (pRefFont == NULL)
		return;

	if ((svt&SVT_MASK_ALL)!=SVT_EMPTY && (svt&SVT_MASK_ALL)!=SVT_FONT)
		return;

	svt = (DuiStyleVariantType)(SVT_FONT | mask);
	if (fontVal==NULL) fontVal = NEW LOGFONTW;
	*fontVal = *pRefFont;
}

BOOL CDuiStyleVariant::GetBitmap(BITMAP* pBitmap)
{
	if (pBitmap==NULL || (svt&SVT_MASK_ALL)!=SVT_BITMAP)
		return FALSE;

	if (__cache)
		return *pBitmap = *(BITMAP*)__cache, TRUE;

	if (bitmapVal==NULL || ::GetObjectType(bitmapVal)!=OBJ_BITMAP)
		return FALSE;

	return ::GetObject(bitmapVal, sizeof(BITMAP), pBitmap), TRUE;
}

//void CDuiStyleVariant::operator = (BOOL b)
//{
//	if (svt!=SVT_EMPTY && (svt&SVT_MASK_ALL)!=SVT_BOOL)
//		return;
//
//	svt = SVT_BOOL;
//	boolVal = b;
//}
//
//void CDuiStyleVariant::operator = (LONG l)
//{
//	if (svt!=SVT_EMPTY && (svt&SVT_MASK_ALL)!=SVT_LONG)
//		return;
//
//	svt = SVT_LONG;
//	longVal = l;
//}
//
//void CDuiStyleVariant::operator = (DOUBLE d)
//{
//	if (svt!=SVT_EMPTY && (svt&SVT_MASK_ALL)!=SVT_DOUBLE && (svt&SVT_MASK_ALL)!=SVT_PERCENT)
//		return;
//
//	svt = SVT_DOUBLE;
//	doubleVal = d;
//}
//
//void CDuiStyleVariant::operator = (COLORREF clr)
//{
//	if (svt!=SVT_EMPTY && (svt&SVT_MASK_ALL)!=SVT_COLOR)
//		return;
//
//	svt = SVT_COLOR;
//	colorVal = clr;
//}
//
//void CDuiStyleVariant::operator = (LPCOLESTR lpsz)
//{
//	if ((svt&SVT_MASK_ALL)!=SVT_EMPTY && (svt&SVT_MASK_ALL)!=SVT_STRING)
//		return;
//
//	if (svt==SVT_STRING && strVal) delete [] strVal;
//	svt = SVT_STRING;
//	strVal = NEW OLECHAR[lstrlenW(lpsz) + 1];
//	if (strVal)
//		lstrcpyW((LPWSTR)strVal, lpsz);
//}
//
//void CDuiStyleVariant::operator = (const LOGPEN* pLogPen)
//{
//	if (pLogPen == NULL)
//		return;
//
//	if ((svt&SVT_MASK_ALL)!=SVT_EMPTY && (svt&SVT_MASK_ALL)!=SVT_PEN)
//		return;
//
//	svt = (DuiStyleVariantType)(SVT_PEN | SVT_PEN_MASK_ALL);
//	if (penVal == NULL) penVal = NEW LOGPEN;
//	*penVal = *pLogPen;
//}
//
//void CDuiStyleVariant::operator = (const LOGBRUSH* pLogBrush)
//{
//	if (pLogBrush == NULL)
//		return;
//
//	if ((svt&SVT_MASK_ALL)!=SVT_EMPTY && (svt&SVT_MASK_ALL)!=SVT_BRUSH)
//		return;
//
//	svt = SVT_BRUSH;
//	if (__cache == NULL) __cache = (UINT_PTR)NEW LOGBRUSH;
//	*(LOGBRUSH*)__cache = *pLogBrush;
//}
//
//void CDuiStyleVariant::operator = (const LOGFONT* pLogFont)
//{
//	if (pLogFont == NULL)
//		return;
//
//	if ((svt&SVT_MASK_ALL)!=SVT_EMPTY && (svt&SVT_MASK_ALL)!=SVT_FONT)
//		return;
//
//	svt = (DuiStyleVariantType)(SVT_FONT | SVT_FONT_MASK_ALL);
//	if (fontVal==NULL) fontVal = NEW LOGFONTW;
//	*fontVal = *pLogFont;
//}
//
//void CDuiStyleVariant::operator = (const BITMAP* pBitmap)
//{
//	if (pBitmap == NULL)
//		return;
//
//	if ((svt&SVT_MASK_ALL)!=SVT_EMPTY && (svt&SVT_MASK_ALL)!=SVT_BITMAP)
//		return;
//
//	svt = SVT_BITMAP;
//	if (__cache == NULL) __cache = (UINT_PTR)NEW BITMAP;
//	*(BITMAP*)__cache = *pBitmap;
//}
//
//void CDuiStyleVariant::operator = (LPCRECT pRect)
//{
//	if (pRect == NULL)
//		return;
//
//	if ((svt&SVT_MASK_ALL)!=SVT_EMPTY && (svt&SVT_MASK_ALL)!=SVT_RECT)
//		return;
//
//	svt = SVT_RECT;
//	if (rectVal == NULL) rectVal = NEW RECT;
//	*rectVal = *pRect;
//}
//
//void CDuiStyleVariant::operator = (LPSIZE pSize)
//{
//	if (pSize == NULL) return;
//	if ((svt&SVT_MASK_ALL)!=SVT_EMPTY && (svt&SVT_MASK_ALL)!=SVT_SIZE) return;
//	svt = SVT_SIZE;
//	if (sizeVal == NULL) sizeVal = NEW SIZE;
//	*sizeVal = *pSize;
//}
//
//void CDuiStyleVariant::operator = (LPPOINT pPoint)
//{
//	if (pPoint == NULL) return;
//	if ((svt&SVT_MASK_ALL)!=SVT_EMPTY && (svt&SVT_MASK_ALL)!=SVT_POINT) return;
//	svt = SVT_POINT;
//	if (pointVal == NULL) pointVal = NEW POINT;
//	*pointVal = *pPoint;
//}

BOOL CDuiStyleVariant::IsEmpty()
{
	return (svt&SVT_MASK_ALL)==SVT_EMPTY;
}

LPVOID CDuiStyleVariant::GetExternal()
{
	return ((svt&SVT_MASK_ALL)==SVT_EXTERNAL) ? extVal : NULL;
}

BOOL CDuiStyleVariant::Parse(CDuiLayoutManager* pLayoutMgr, CDuiStyle* pStyle, LPCOLESTR lpszAttrName, CComVariant v )
{
	if (pLayoutMgr==NULL || pStyle==NULL || lpszAttrName==NULL || v.vt==VT_EMPTY)
		return FALSE;

	//LPCOLESTR p = lpszValue;
	//while (iswspace(*p)) p++;
	//if (*p == L'=') // 表达式类型
	//{
	//	CDuiStyleVariant* pRet = pStyle->GetPrivateAttribute(SAT_MARGIN, TRUE);
	//	ATLASSERT(pRet);
	//}

#define __StyleParser(name, classname, sat) else if (lstrcmpiW(lpszAttrName, L#name) == 0)  return CDuiStyleParser_##classname::Parse(pLayoutMgr, pStyle, v);
	__ForeachStyle(__StyleParser);
#undef __StyleParser

	return FALSE;
}

CDuiStyle::CDuiStyle( CDuiLayoutManager* pLayoutMgr,LPCOLESTR lpszTargetName/*=NULL*/, LPCOLESTR lpszTargetId/*=NULL*/, DWORD dwTargetState/*=0*/, DuiNode* pOwnerCtrl/*=NULL*/, CDuiStyle* pRefStyle/*=NULL*/ ) :
	m_pLayoutMgr(pLayoutMgr),
	m_pRefStyle(pRefStyle),
	m_strTargetName(lpszTargetName),
	m_strTargetId(lpszTargetId),
	m_dwTargetState(dwTargetState),
	m_pOwnerCtrl(pOwnerCtrl),
	m_pParent(NULL)
{
	ATLASSERT(m_pLayoutMgr);
	if (m_pOwnerCtrl) (m_pOwnerCtrl)->SetPrivateStyle(this);

	// 如果目标名称是别名，这里修改成标准名称
	if (!m_strTargetName.IsEmpty())
	{
		LPCWSTR NormalName = m_strTargetName;
		PluginCallContext(TRUE).Output(&NormalName).Call(OpCode_GetNormalName, (LPCWSTR)m_strTargetName, NULL);
		m_strTargetName = NormalName;
	}
}

CDuiStyle::~CDuiStyle()
{
	for (int i=0; i<m_StyleAttributes.GetSize(); i++)
	{
		CDuiStyleVariant* pVar = (CDuiStyleVariant*)m_StyleAttributes[i];
		if (pVar)
			delete pVar;
	}
	m_StyleAttributes.RemoveAll();
}

void CDuiStyle::CheckRefStyle()
{
	// 可能某个style已经被布局管理器删除了，此时检测一下
	if (m_pRefStyle && !m_pLayoutMgr->HasStyle(m_pRefStyle))
		m_pRefStyle = NULL;
}

CDuiStyleVariant* CDuiStyle::GetPrivateAttribute( DuiStyleAttributeType sat, BOOL bAutoCreate/*=FALSE*/ )
{
	for (int i=0; i<m_StyleAttributes.GetSize(); i++)
	{
		CDuiStyleVariant* pStyleVar = (CDuiStyleVariant*)m_StyleAttributes[i];
		if (pStyleVar && pStyleVar->sat==sat)
			return pStyleVar;
	}

	CDuiStyleVariant* p = NULL;
	if (bAutoCreate)
	{
		p = NEW CDuiStyleVariant(sat);
		m_StyleAttributes.Add(p);
	}
	return p;
}

CDuiStyleVariant* CDuiStyle::GetAttribute( DuiStyleAttributeType sat, DuiStyleVariantType svt/*=SVT_EMPTY*/ )
{
	return m_pOwnerCtrl ? (CDuiStyleVariant*)(m_pOwnerCtrl)->GetStyleAttribute(sat,svt) : GetAttributeNoControl(sat,svt);
}

CDuiStyleVariant* CDuiStyle::GetAttributeNoControl(DuiStyleAttributeType sat, DuiStyleVariantType svt/*=SVT_EMPTY*/)
{
	CDuiStyleVariant* pVar = GetPrivateAttribute(sat);
	if (pVar && (svt==SVT_EMPTY || svt==pVar->svt))
		return pVar;

	if (m_pRefStyle && (pVar=m_pRefStyle->GetAttribute(sat, svt))!=&svEmpty)
		return pVar;

	if (m_pParent && (pVar=m_pParent->GetAttribute(sat, svt))!=&svEmpty)
		return pVar;

	return &svEmpty;
}

void CDuiStyle::RemoveAttribute(DuiStyleAttributeType sat)
{
	for (int i=0; i<m_StyleAttributes.GetSize(); i++)
	{
		CDuiStyleVariant* pStyleVar = (CDuiStyleVariant*)m_StyleAttributes[i];
		if (pStyleVar && pStyleVar->sat==sat)
		{
			delete pStyleVar;
			m_StyleAttributes.RemoveAt(i);
			return;
		}
	}
}

BOOL CDuiStyle::ParseStyle( LPCOLESTR lpszValue )
{
	if (lpszValue==NULL) return FALSE;

	CStrArray strs;
	if (!SplitStringToArray(lpszValue, strs, L";"))
		return FALSE;

	for (int i=0; i<strs.GetSize(); i++)
	{
		CStrArray pairs;
		if (SplitStringToArray(strs[i], pairs, L":"/*, FALSE*/) && pairs.GetSize()==2)
		{
			CComVariant v = pairs[1];
			CDuiStyleVariant::Parse(m_pLayoutMgr, this, pairs[0].Trim(), v);
		}
	}
	return TRUE;
}

int CDuiStyle::GetKindOf(CDuiStyle* pDest)
{
	ATLASSERT(pDest);
	LPCOLESTR tname=GetTargetName(), tname2=pDest->GetTargetName();
	LPCOLESTR tid=GetTargetId(), tid2=pDest->GetTargetId();
	DWORD tstate=GetTargetState(), tstate2=pDest->GetTargetState();

	// 比较原则：无值为大
	int num_same = 0, num_parent = 0, num_child = 0, num_unrelated = 0;
	int kind_of_name = (tname==tname2 || (tname && tname2 && lstrcmpiW(tname,tname2)==0)) ? num_same++, kind_same :
		(tname && !tname2) ? num_parent++, kind_parent :
		(!tname && tname2) ? num_child++, kind_child :
		(num_unrelated++, kind_unrelated);
	int kind_of_id = (tid==tid2 || (tid && tid2 && lstrcmpiW(tid,tid2)==0)) ? num_same++, kind_same :
		(tid && !tid2) ? num_parent++, kind_parent :
		(!tid && tid2) ? num_child++, kind_child :
		(num_unrelated++, kind_unrelated);
	int kind_of_state = (tstate==tstate2) ? num_same++, kind_same :
		((tstate && !tstate2) || (tstate2 && (tstate&tstate2)==tstate2)) ? num_parent++, kind_parent :
		((!tstate && tstate2) || (tstate && (tstate&tstate2)==tstate)) ? num_child++, kind_child :
		(num_unrelated++, kind_unrelated);

	if (num_same == 3)
		return kind_same;
	else if (num_unrelated > 0)
		return kind_unrelated;
	else if (num_parent>0 && num_child==0)
		return kind_parent;
	else if (num_child>0 && num_parent==0)
		return kind_child;
	else // num_parent>0 && num_child>0
		return kind_unrelated;
}

LPCOLESTR CDuiStyle::GetTargetName() const
{
	if (m_pOwnerCtrl) return (m_pOwnerCtrl)->m_tagName;
	return !m_strTargetName.IsEmpty() ? (LPCOLESTR)m_strTargetName : NULL;
}

LPCOLESTR CDuiStyle::GetTargetId() const
{
	if (m_pOwnerCtrl) return (m_pOwnerCtrl)->GetID();
	return !m_strTargetId.IsEmpty() ? (LPCOLESTR)m_strTargetId : NULL;
}

DWORD CDuiStyle::GetTargetState() const
{
	return m_dwTargetState;
}

//BOOL CDuiStyle::IsTargetNameSame(LPCOLESTR name, BOOL bMustTargetName/*=FALSE*/) const
//{
//	ControlExtensionEntry* pEntry = CDuiLayoutManager::GetControlFactroy(name);
//	if (pEntry) name = pEntry->szName;
//
//	if (m_pOwnerCtrl) return name==m_pOwnerCtrl->GetName() || lstrcmpiW(name,m_pOwnerCtrl->GetName())==0;
//
//	if (bMustTargetName && m_strTargetName.IsEmpty()) return FALSE;
//	return m_strTargetName.IsEmpty() ? TRUE
//		: CDuiLayoutManager::GetControlFactroy((LPCOLESTR)m_strTargetName)==CDuiLayoutManager::GetControlFactroy(name);
//}

void CDuiStyle::FireChangeEvent( CDuiStyleVariant* newVal )
{
	// 仅针对私有样式触发事件
	if (m_pOwnerCtrl && newVal)
	{
		m_pOwnerCtrl->OnStyleChanged(newVal);
	}
}

BOOL CDuiStyle::OnHit( LPVOID pClient, LPVOID pServer, DWORD_PTR protocol )
{
#ifndef NO3D
	if ((protocol & cp_style_bind)!=0 && pServer)
	{
		DuiStyleAttributeType sat = (DuiStyleAttributeType)(LOWORD(protocol));
		CDuiStyleVariant* pRet = GetPrivateAttribute(sat, FALSE);
		if (pRet == pClient)
		{
			COLOR* c = (COLOR*)pServer;
			if (pRet)
			{
				*pRet = (COLORREF)*c;
				FireChangeEvent(pRet);
			}
		}
	}
#endif
	return FALSE;
}
//////////////////////////////////////////////////////////////////////////
CDuiImageResource::CDuiImageResource( CDuiLayoutManager* lyt, LPCOLESTR lpszUrl, LPCOLESTR lpszId/*=NULL*/ ) : UrlResources<CDuiImageResource,false>(lpszUrl, lpszId, &lyt->m_Images)
{
	pLayoutMgr = lyt;
	img = NULL;

	_ParseUrl(lpszUrl);
}

CDuiImageResource::~CDuiImageResource()
{
	if (img)
	{
#ifndef NO3D
		if (pLayoutMgr->Get3DDevice()==NULL)
#endif // NO3D
			delete img;
		//else img3d->Dispose();
	}
	img = NULL;

	if (Next()) delete Next();
}

void CDuiImageResource::_ParseUrl( LPCOLESTR lpszUrl )
{
	if (lpszUrl==NULL || *lpszUrl==0)
		return;

#ifndef NO3D
	if (pLayoutMgr->Get3DDevice())
	{
		img3d = ImageResource::New(pLayoutMgr->Get3DDevice(), lpszUrl, id);
	}
	else
#endif // NO3D
	{
		img = NEW CDuiImage;
		if (img && !LoadUrl())
		{
			delete img;
			img=NULL;
		}
	}
}

BOOL CDuiImageResource::IsNull() const
{
	if (!img || !isLoaded) return TRUE;

#ifndef NO3D
	if (pLayoutMgr->Get3DDevice())
		return !img3d->IsLoaded();
	else
#endif // NO3D
		return img->IsNull();
}

void CDuiImageResource::SetClipRect( RECT& rcClip )
{
#ifndef NO3D
	if (pLayoutMgr->Get3DDevice() && img3d)
	{
		img3d->SetClipRect(&rcClip);
	}
	else
#endif // NO3D
	if (img)
		img->SetClipRect(&rcClip);
}

//////////////////////////////////////////////////////////////////////////
BOOL CAttributeMap::SetAttribute( LPCOLESTR szAttr, /*LPCOLESTR*/CComVariant szVal, BOOL bHasSubProps/*=FALSE*/ )
{
	CAttributeMap* pSubProps = NULL;
	if (szVal.vt==VT_BSTR && szVal.bstrVal!=NULL && szVal.bstrVal[0]!=0 && bHasSubProps)
	{
		CStrArray astr;
		if (SplitStringToArray(szVal.bstrVal, astr, L"()"))
		{
			szVal = astr[0];
			if (astr.GetSize()>=2)
			{
				CStrArray astr2;
				if (SplitStringToArray(astr[1], astr2, L",;") && astr2.GetSize()>0)
				{
					pSubProps = NEW CAttributeMap;
					for (int i=0; i<astr2.GetSize(); i++)
					{
						CStrArray astr3;
						if (SplitStringToArray(astr2[i], astr3, L"=:") && astr3.GetSize()>=2)
						{
							pSubProps->SetAttribute(astr3[0], (LPCOLESTR)astr3[1], FALSE);
						}
					}
				}
			}
		}
	}
	CStdString k(szAttr);
	k.MakeLower();
	int nIndex = FindKey(k);
	BOOL bRet = TRUE;
	if (nIndex == -1)
		bRet = Add(k, AttrValue(szVal,pSubProps));
	else
	{
		ATLASSERT(nIndex >= 0 && nIndex < m_nSize);
		m_aKey[nIndex].~CStdString();
		m_aVal[nIndex].~AttrValue();
		InternalSetAtIndex(nIndex, k, AttrValue(szVal,pSubProps));
	}
	// notify
	if (_fnNotify) _fnNotify(FALSE, k/*szAttr*/, _data);
	return bRet;
}

void CAttributeMap::RemoveAttribute(LPCOLESTR szAttr)
{
	CStdString k(szAttr);
	k.MakeLower();
	int nIndex = FindKey(k);
	if (nIndex == -1) return;

	//RemoveAt(nIndex);
	// 不真的删除是为了 IDispatch 需要使用固定索引，一个名称一旦有一个索引，以后就一直使用此索引
	if ( !GetValueAt(nIndex)._deleted)
	{
		GetValueAt(nIndex)._deleted = TRUE;
		if (_fnNotify) _fnNotify(TRUE, szAttr, _data);
	}
}

STDMETHODIMP CAttributeMap::Read (LPCOLESTR pszPropName, VARIANT *pVar, IErrorLog *pErrorLog)
{
	LineLog dbg(L"Property Process('%s')", pszPropName);

	if (pszPropName==NULL || pVar==NULL) return E_POINTER;
	if (!HasAttribute(pszPropName)) return E_INVALIDARG;

	CComVariant v = GetAttribute(pszPropName);
	if (v.vt==VT_BSTR) dbg(L" - value('%s')", v.bstrVal);
	else dbg(L" - not string");
	VARTYPE vt = pVar->vt;
	::VariantInit(pVar); // SILVERLIGHT控件读取属性时传递的pVar居然没有初始化，必须首先把数值清0
	if (vt==VT_EMPTY || SUCCEEDED(v.ChangeType(vt)))
		return v.Detach(pVar);

	// 专门处理颜色转换，例如颜色名称或者#ababab类型的颜色
	if ((vt==VT_I4 || vt==VT_UI4) && v.vt==VT_BSTR)
	{
		COLORREF clr = CLR_INVALID;
		if (TryLoad_COLORREF_FromString(v.bstrVal, clr) && clr!=CLR_INVALID)
		{
			V_UI4(pVar) = (ULONG)clr;
			return S_OK;
		}
	}

	if (pErrorLog)
	{
		EXCEPINFO ei = {0,0,(BSTR)pszPropName,(BSTR)L"no property.",NULL,0,0,NULL,E_FAIL};
		pErrorLog->AddError(pszPropName, &ei);
	}
	return E_FAIL;
}

//////////////////////////////////////////////////////////////////////////
// CDuiAxContainer

STDMETHODIMP CDuiAxContainer::QueryInterface( REFIID riid, LPVOID *ppvObject )
{
	if (ppvObject==NULL) return E_POINTER;
	*ppvObject = NULL;
	if ( riid == IID_IUnknown ) *ppvObject = static_cast<IOleContainer*>(this);
	else if ( riid == IID_IOleContainer ) *ppvObject = static_cast<IOleContainer*>(this);
	else if ( riid == IID_IOleWindow ) *ppvObject = static_cast<IOleWindow*>(this);
	else if ( riid == IID_IOleInPlaceFrame ) *ppvObject = static_cast<IOleInPlaceFrame*>(this);
	else if ( riid == IID_IOleInPlaceUIWindow ) *ppvObject = static_cast<IOleInPlaceUIWindow*>(this);

	if ( *ppvObject != NULL ) AddRef();
	return *ppvObject == NULL ? E_NOINTERFACE : S_OK;
}

STDMETHODIMP CDuiAxContainer::GetWindow( HWND* phWnd )
{
	*phWnd = m_pLayoutMgr->GetPaintWindow();
	return S_OK;
}

STDMETHODIMP CDuiAxContainer::ContextSensitiveHelp( BOOL )
{
	return E_NOTIMPL;
}

STDMETHODIMP CDuiAxContainer::GetBorder( LPRECT )
{
	return E_NOTIMPL;
}

STDMETHODIMP CDuiAxContainer::RequestBorderSpace( LPCBORDERWIDTHS )
{
	return E_NOTIMPL;
}

STDMETHODIMP CDuiAxContainer::SetBorderSpace( LPCBORDERWIDTHS )
{
	return E_NOTIMPL;
}

STDMETHODIMP CDuiAxContainer::SetActiveObject( LPOLEINPLACEACTIVEOBJECT pActiveObject, LPCOLESTR )
{
	if (m_pInPlaceActiveObject.p == pActiveObject)
		return S_OK;

	// uideactive old
	if (m_pInPlaceActiveObject.p)
	{
		CComQIPtr<IOleInPlaceObject> spIPObject(m_pInPlaceActiveObject);
		if (spIPObject.p)
			spIPObject->UIDeactivate();
	}

	m_pInPlaceActiveObject = pActiveObject;
	for (int i=0, num=m_sites.GetSize(); i<num; i++)
	{
		CDuiAxSite* pSite = (CDuiAxSite*)m_sites[i];
		CComPtr<IOleInPlaceActiveObject> ipao;
		if (pSite && SUCCEEDED(pSite->QueryControl(&ipao)) && ipao.p==pActiveObject)
		{
			m_pLayoutMgr->SetFocus(pSite->owner());
			break;
		}
	}
	return S_OK;
}

STDMETHODIMP CDuiAxContainer::InsertMenus( HMENU, LPOLEMENUGROUPWIDTHS )
{
	return E_NOTIMPL;
}

STDMETHODIMP CDuiAxContainer::SetMenu( HMENU, HOLEMENU, HWND )
{
	return E_NOTIMPL;
}

STDMETHODIMP CDuiAxContainer::RemoveMenus( HMENU )
{
	return E_NOTIMPL;
}

STDMETHODIMP CDuiAxContainer::SetStatusText( LPCOLESTR )
{
	return E_NOTIMPL;
}

STDMETHODIMP CDuiAxContainer::EnableModeless( BOOL )
{
	return S_OK;
}

STDMETHODIMP CDuiAxContainer::TranslateAccelerator( LPMSG, WORD )
{
	return S_FALSE;
}

//////////////////////////////////////////////////////////////////////////
// IEnumUnknown helper
class CEnumUnknown : public IEnumUnknown
{
public:
	CEnumUnknown(IUnknown** pUnks, ULONG ulCount) : m_dwRef(1), m_ulPos(0), m_pUnks(NULL), m_ulCount(0)
	{
		if (ulCount==0 || ulCount>1000 || pUnks==NULL) return;

		m_ulCount = ulCount;
		m_pUnks = NEW IUnknown*[m_ulCount];
		if (m_pUnks==NULL) return;

		for (ULONG i=0; i<m_ulCount; i++)
		{
			m_pUnks[i] = pUnks[i];
			if (m_pUnks[i]) m_pUnks[i]->AddRef();
		}
	}
	~CEnumUnknown()
	{
		if (m_pUnks)
		{
			for (ULONG i=0; i<m_ulCount; i++)
			{
				if (m_pUnks[i]) m_pUnks[i]->Release();
			}
			delete[] m_pUnks;
		}
	}

	ULONG m_dwRef;
	IUnknown** m_pUnks;
	ULONG m_ulCount;
	ULONG m_ulPos;

	STDMETHOD_(ULONG,AddRef)()
	{
		return ++m_dwRef;
	}
	STDMETHOD_(ULONG,Release)()
	{
		LONG lRef = --m_dwRef;
		if( lRef == 0 ) delete this;
		return lRef;
	}
	STDMETHOD(QueryInterface)(REFIID riid, LPVOID *ppvObject)
	{
		if (ppvObject==NULL) return E_POINTER;
		*ppvObject = NULL;

		if( riid == IID_IUnknown || riid == IID_IEnumUnknown ) *ppvObject = static_cast<IEnumUnknown*>(this);

		if( *ppvObject != NULL ) AddRef();
		return *ppvObject == NULL ? E_NOINTERFACE : S_OK;
	}
	STDMETHOD(Next)(ULONG celt, IUnknown **rgelt, ULONG *pceltFetched)
	{
		if (celt==0) return E_UNEXPECTED;
		if (rgelt==NULL) return E_POINTER;

		celt = min(celt, m_ulCount-m_ulPos);
		if( pceltFetched != NULL ) *pceltFetched = celt;
		if (celt==0) return S_FALSE;

		for (ULONG i=0; i<celt; i++)
		{
			rgelt[i] = m_pUnks[m_ulPos++];
			if (rgelt[i]) rgelt[i]->AddRef();
		}
		return S_OK;
	}
	STDMETHOD(Skip)(ULONG celt)
	{
		m_ulPos += celt;
		m_ulPos = min(m_ulPos, m_ulCount);
		return S_OK;
	}
	STDMETHOD(Reset)(void)
	{
		m_ulPos = 0;
		return S_OK;
	}
	STDMETHOD(Clone)(IEnumUnknown **ppenum)
	{
		return E_NOTIMPL;
	}
};

STDMETHODIMP CDuiAxContainer::ParseDisplayName( LPBINDCTX, LPOLESTR, ULONG*, LPMONIKER* )
{
	return E_NOTIMPL;
}

STDMETHODIMP CDuiAxContainer::EnumObjects( DWORD dwFlags, LPENUMUNKNOWN* ppEnumUnknown )
{
	if (ppEnumUnknown==NULL) return E_POINTER;
	*ppEnumUnknown = NEW CEnumUnknown((IUnknown**)m_sites.GetData(), m_sites.GetSize());
	return *ppEnumUnknown==NULL ? E_OUTOFMEMORY : S_OK;
}

STDMETHODIMP CDuiAxContainer::LockContainer( BOOL )
{
	return E_NOTIMPL;
}

BOOL CDuiAxContainer::RegistSite( CDuiAxSite* pSite, BOOL bUnReg/*=FALSE*/ )
{
	if (bUnReg) return m_sites.Remove(pSite);
	else return m_sites.Add(pSite);
}

void CDuiAxContainer::CreateOleFont(HFONT hFont/*=NULL*/)
{
	if (!hFont) hFont = (HFONT)::GetStockObject(DEFAULT_GUI_FONT);
	if (!hFont) hFont = (HFONT)::GetStockObject(SYSTEM_FONT);
	_assert(hFont);

	LOGFONTW logfont;
	::GetObject(hFont, sizeof(LOGFONTW), &logfont);

	FONTDESC fd;
	fd.cbSizeofstruct = sizeof(FONTDESC);
	fd.lpstrName = const_cast<LPOLESTR>(logfont.lfFaceName);
	fd.sWeight = (short)logfont.lfWeight;
	fd.sCharset = logfont.lfCharSet;
	fd.fItalic = logfont.lfItalic;
	fd.fUnderline = logfont.lfUnderline;
	fd.fStrikethrough = logfont.lfStrikeOut;

	long lfHeight = logfont.lfHeight;
	if (lfHeight < 0)
		lfHeight = -lfHeight;

	CWindowDC dc(m_pLayoutMgr->GetPaintWindow());
	int ppi = dc.GetDeviceCaps(LOGPIXELSY);
	fd.cySize.Lo = lfHeight * 720000 / ppi;
	fd.cySize.Hi = 0;

	m_spOleFont = NULL;

	if (FAILED(::OleCreateFontIndirect(&fd, IID_IFontDisp, (void**)&m_spOleFont)))
		m_spOleFont = NULL;
}

BOOL CDuiAxContainer::OnAmbientProperty( CDuiAxSite* pSite, DISPID dispid, VARIANT* pvarResult )
{
	switch (dispid)
	{
	case DISPID_AMBIENT_AUTOCLIP:
	case DISPID_AMBIENT_MESSAGEREFLECT:
	case DISPID_AMBIENT_SUPPORTSMNEMONICS:
	case DISPID_AMBIENT_USERMODE:
		V_VT(pvarResult) = VT_BOOL;
		V_BOOL(pvarResult) = (VARIANT_BOOL)-1;
		return TRUE;

	case DISPID_AMBIENT_SHOWGRABHANDLES:
	case DISPID_AMBIENT_SHOWHATCHING:
	case DISPID_AMBIENT_UIDEAD:
		V_VT(pvarResult) = VT_BOOL;
		V_BOOL(pvarResult) = 0;
		return TRUE;

	case DISPID_AMBIENT_APPEARANCE:     // ambient appearance is 3D
		V_VT(pvarResult) = VT_I2;
		V_I2(pvarResult) = 1;
		return TRUE;

	case DISPID_AMBIENT_BACKCOLOR:
	case DISPID_AMBIENT_FORECOLOR:
		{
			// TODO: 这里要添加更多的控件计算，暂时用简单方法
			CWindow win(m_pLayoutMgr->GetPaintWindow());
			CWindowDC dc(win);
			win.SendMessage(WM_CTLCOLORSTATIC, (WPARAM)dc.m_hDC, (LPARAM)win.m_hWnd);

			V_VT(pvarResult) = VT_COLOR;
			V_I4(pvarResult) = (dispid == DISPID_AMBIENT_BACKCOLOR) ? dc.GetBkColor() : dc.GetTextColor();
		}
		return TRUE;

	case DISPID_AMBIENT_FONT:
		if (m_spOleFont == NULL)         // ambient font not initialized
			CreateOleFont(CWindow(m_pLayoutMgr->GetPaintWindow()).GetFont());

		_assert(m_spOleFont.p != NULL);
		if (m_spOleFont.p == NULL)         // failed to create font
			return FALSE;

		V_VT(pvarResult) = VT_FONT;
		m_spOleFont.p->AddRef();
		V_DISPATCH(pvarResult) = m_spOleFont.p;
		return TRUE;

	case DISPID_AMBIENT_DISPLAYASDEFAULT:
		V_VT(pvarResult) = VT_BOOL;
		V_BOOL(pvarResult) = (VARIANT_BOOL)0;
		return TRUE;

	case DISPID_AMBIENT_LOCALEID:
		V_VT(pvarResult) = VT_I4;
		V_I4(pvarResult) = GetThreadLocale();
		return TRUE;

	case DISPID_AMBIENT_DISPLAYNAME:
		{
			V_VT(pvarResult) = VT_BSTR;
			V_BSTR(pvarResult) = ::SysAllocString(L""); // return blank string
		}
		return TRUE;
	}

	return FALSE;
}

LRESULT CDuiAxContainer::MessageHandler( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
{
	LRESULT lRes = 0;

	CDuiAxSite* pAxSite = NULL;
	CDuiEditable* pEdit = NULL;
	if ((uMsg >= WM_MOUSEFIRST && uMsg <= WM_MOUSELAST) || uMsg==WM_MOUSEWHEEL || uMsg==guMsgMouseWheel)
	{
		DuiNode* pCtrl = m_pLayoutMgr->m_pEventCaptured;
		if (pCtrl)
		{
			//::DebugBreak();
			if ((pAxSite=CLASS_CAST(pCtrl, CDuiAxSite))==NULL && (pEdit=CLASS_CAST(pCtrl, CDuiEditable))==NULL)
				return bHandled=FALSE, 0;
		}

		POINT pt = {GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
		m_pLayoutMgr->__AdjustPoint(&pt);
		// 如果没有捕获鼠标的OCX控件，则获取鼠标指向的控件
		if (pAxSite==NULL && pEdit==NULL)
		{
			pCtrl = m_pLayoutMgr->FindControl(pt);
			if (pCtrl)
			{
				pAxSite = CLASS_CAST(pCtrl, CDuiAxSite);
				if (pAxSite==NULL)
					pEdit = CLASS_CAST(pCtrl, CDuiEditable);
			}
			if (pAxSite==NULL && pEdit==NULL)
				return bHandled=FALSE, 0;
		}

		// now pAxSite or pEdit are valid, first call default handler
		if (pEdit)
		{
			bHandled = m_pLayoutMgr->DefMessageHandler(uMsg, wParam, lParam, lRes);
			if (!bHandled)
				bHandled = pEdit->OnMessage(uMsg, wParam, lParam, lRes);
			return lRes;
		}

		if (!pAxSite->m_bWindowless)
			return bHandled=FALSE, 0;

		bHandled = TRUE;
		m_pLayoutMgr->DefMessageHandler(uMsg, wParam, lParam, lRes);

		DWORD dwHitResult = pAxSite->m_bCapture ? HITRESULT_HIT : HITRESULT_OUTSIDE;
		if (dwHitResult == HITRESULT_OUTSIDE && pAxSite->m_spViewObject != NULL)
		{
			RECT rc = pAxSite->GetPos();
			pAxSite->m_spViewObject->QueryHitPoint(DVASPECT_CONTENT, &rc, pt, 0, &dwHitResult);
		}
		if (dwHitResult == HITRESULT_HIT)
		{
			lParam = MAKELPARAM(pt.x, pt.y);
			switch (uMsg)
			{
			case WM_MOUSEMOVE:
			case WM_LBUTTONUP:
			case WM_RBUTTONUP:
			case WM_MBUTTONUP:
			case WM_LBUTTONDOWN:
			case WM_RBUTTONDOWN:
			case WM_MBUTTONDOWN:
			case WM_LBUTTONDBLCLK:
			case WM_RBUTTONDBLCLK:
			case WM_MBUTTONDBLCLK:
			//case WM_MOUSEWHEEL:
				if (pAxSite->m_bInPlaceActive && pAxSite->m_spInPlaceObject)
					pAxSite->m_spInPlaceObject->OnWindowMessage(uMsg, wParam, lParam, &lRes);
			}
			if ((uMsg==WM_MOUSEWHEEL || uMsg==guMsgMouseWheel) && pAxSite->m_bInPlaceActive && pAxSite->m_spInPlaceObject)
				pAxSite->m_spInPlaceObject->OnWindowMessage(uMsg, wParam, lParam, &lRes);
		}

		return lRes;
	}

	// 向聚焦的无窗口控件传递消息
	BOOL bWindowlessMsg = FALSE;
	//WCHAR log[2] = L"";
	switch (uMsg)
	{
	//case WM_IME_CHAR:
	//	log[0] = (WCHAR)wParam;
	//	// pass through
	case WM_KEYDOWN:
	case WM_KEYUP:
	case WM_CHAR:
	case WM_DEADCHAR:
	case WM_SYSKEYDOWN:
	case WM_SYSKEYUP:
	case WM_SYSDEADCHAR:
	case WM_HELP:
	case WM_CAPTURECHANGED:
	case WM_CANCELMODE:
	case WM_SETCURSOR:
	case WM_CONTEXTMENU:
		bWindowlessMsg = TRUE;
		break;
	default:;
		//if (uMsg>=WM_IME_SETCONTEXT && uMsg<=WM_IME_KEYUP) // 多次一举，处理IME消息导致很多问题，其实根本不用处理
		//	bWindowlessMsg = TRUE;
	}
	if (!bWindowlessMsg)
		return bHandled=FALSE, 0;

	DuiNode* pActiveCtrl = m_pLayoutMgr->GetFocus();
	//if (pActiveCtrl == NULL)
	//{
	//	//pActiveCtrl = m_pLayoutMgr->GetRoot();
	//	m_pLayoutMgr->SetFocus(m_pLayoutMgr->GetRoot());
	//}
	if (pActiveCtrl && (pEdit=CLASS_CAST(pActiveCtrl, CDuiEditable)))
	{
		return bHandled=pEdit->OnMessage(uMsg, wParam, lParam, lRes), lRes;
	}

	HRESULT hr = S_FALSE;
	if (pActiveCtrl && (pAxSite=CLASS_CAST(pActiveCtrl, CDuiAxSite)))
	{
		CComQIPtr<IOleInPlaceObjectWindowless> spIPObject(pAxSite->m_spInPlaceObject); //m_pInPlaceActiveObject);
		if (spIPObject.p)
			hr = spIPObject->OnWindowMessage(uMsg, wParam, lParam, &lRes);
	}
	bHandled = (hr==S_OK);
	return lRes;

	//CDuiControlExtensionBase* pCtrl = m_pLayoutMgr->m_pFocus;
	//CDuiAxSite* pAxSite = pCtrl ? CLASS_CAST(pCtrl, CDuiAxSite) : NULL;
	//if (pAxSite==NULL || !pAxSite->m_bWindowless)
	//	return bHandled=FALSE, 0;

	//HRESULT hr = S_FALSE;
	//if (pAxSite->m_bInPlaceActive && pAxSite->m_spInPlaceObject)
	//	hr = pAxSite->m_spInPlaceObject->OnWindowMessage(uMsg, wParam, lParam, &lRes);
	//bHandled = (hr==S_OK);
	//return lRes;
}

//////////////////////////////////////////////////////////////////////////
// CDuiAxSite
//class org_clip_helper
//{
//	CRenderContentOrg org;
//	CRenderClip clip;
//
//public:
//	org_clip_helper(HDC hdc, CDuiControlExtensionBase* pCtrl) :
//		org(hdc, CDuiControlExtension::GetOffsetParent(pCtrl)),
//		clip(hdc, rt(pCtrl)->GetPos())
//	{}
//};

CDuiAxSite::CDuiAxSite( ) //: m_pOwner(pOwner), m_dwMiscStatus(0), __cache(NULL),
		//m_bUIActivated(FALSE), m_bInPlaceActivated(FALSE), m_bWindowless(FALSE), m_bCaptured(FALSE)
{
	//_assert(m_pOwner);
	memset(this, 0, sizeof(CDuiAxSite));

	//m_bDCCached = FALSE;
	//m_bInPlaceActive = FALSE;
	//m_bUIActive = FALSE;
	//m_bMDIApp = FALSE;
	//m_bWindowless = FALSE;
	//m_bCapture = FALSE;
	//m_bHaveFocus = FALSE;
	//m_bReleaseAll = FALSE;
	//m_bLocked = FALSE;
}

CDuiAxSite::~CDuiAxSite()
{
	ReleaseSurface();
	ReleaseAll();
	if (m_pContainer) m_pContainer->RegistSite(this, TRUE);
	//if (__cache) __cache = delete (org_clip_helper*)__cache, NULL;
}

void CDuiAxSite::Init()
{
	if (m_pLayoutMgr == NULL)
	{
		m_pLayoutMgr = m_pOwner->m_pLayoutMgr;
		m_pContainer = &m_pLayoutMgr->m_AxContainer;
		m_pContainer->RegistSite(this, FALSE);
	}
}

void CDuiAxSite::ReleaseSurface()
{
	if (_memdc)
	{
		if (_hbmpOld)
		{
			::SelectObject(_memdc, _hbmpOld);
			_hbmpOld = NULL;
		}
		::DeleteDC(_memdc);
		_memdc = NULL;
	}
	if (_hbmp)
	{
		::DeleteObject(_hbmp);
		_hbmp = NULL;
	}
}

void CDuiAxSite::ResetSurface()
{
	//if (m_pSurface) m_pSurface = (m_pSurface->Delete(), NULL);

	//CRect rc = m_pOwner->GetPos();
	//m_pSurface = m_pLayoutMgr->GetSurface()->Clone(&rc);

	ReleaseSurface();

#ifndef NO3D
	Device* dev = m_pLayoutMgr->Get3DDevice();
#endif

	HDC dc = 
#ifndef NO3D
		dev ? ::GetDC(NULL/*m_pLayoutMgr->GetPaintWindow()*/) : 
#endif
		m_pLayoutMgr->GetSurface()->GetDC();

	_memdc = ::CreateCompatibleDC(dc);
	CRect rc = GetPos();
	_hbmp = ::CreateCompatibleBitmap(dc, rc.Width(), rc.Height());
	if (_memdc && _hbmp) _hbmpOld = (HBITMAP)::SelectObject(_memdc, _hbmp);

#ifndef NO3D
	if (dev) 
		::ReleaseDC(NULL/*m_pLayoutMgr->GetPaintWindow()*/, dc);
	else 
#endif
		m_pLayoutMgr->GetSurface()->ReleaseDC(dc);
}

void CDuiAxSite::GetControlInfo()
{
	memset(&m_ctlInfo, 0, sizeof(CONTROLINFO));
	m_ctlInfo.cb = sizeof(CONTROLINFO);

	CComQIPtr<IOleControl> spOleCtrl(m_spOleObject);
	if (spOleCtrl.p)
		spOleCtrl->GetControlInfo(&m_ctlInfo);
}

STDMETHODIMP CDuiAxSite::QueryInterface( REFIID riid, LPVOID *ppvObject )
{
	if (ppvObject==NULL) return E_POINTER;
	*ppvObject = NULL;
	if ( riid == IID_IUnknown ) *ppvObject = static_cast<IOleClientSite*>(this);
	else if ( riid == IID_IOleClientSite ) *ppvObject = static_cast<IOleClientSite*>(this);
	else if ( riid == IID_IOleInPlaceSiteWindowless ) *ppvObject = static_cast<IOleInPlaceSiteWindowless*>(this);
	else if ( riid == IID_IOleInPlaceSiteEx )         *ppvObject = static_cast<IOleInPlaceSiteEx*>(this);
	else if ( riid == IID_IOleInPlaceSite )           *ppvObject = static_cast<IOleInPlaceSite*>(this);
	else if ( riid == IID_IOleWindow )                *ppvObject = static_cast<IOleWindow*>(this);
	else if ( riid == IID_IOleControlSite ) *ppvObject = static_cast<IOleControlSite*>(this);
	else if ( riid == IID_IDispatch ) *ppvObject = static_cast<IDispatch*>(this);
	else if ( riid == IID_IServiceProvider ) *ppvObject = static_cast<IServiceProvider*>(this);
	else if ( riid == __uuidof(IXcpControlHost2) )	*ppvObject = static_cast<IXcpControlHost2*>(this);
	else if ( riid == __uuidof(IXcpControlHost) )	*ppvObject = static_cast<IXcpControlHost*>(this);

	if ( *ppvObject != NULL ) AddRef();
	return *ppvObject == NULL ? E_NOINTERFACE : S_OK;
}

// {1B36028E-B491-4BB2-8584-8A9E0A677D6E}
static const GUID GUID_IXcpControlHost = 
{ 0x1B36028E, 0xB491, 0x4BB2, { 0x85, 0x84, 0x8A, 0x9E, 0x0A, 0x67, 0x7D, 0x6E } };

STDMETHODIMP CDuiAxSite::QueryService(REFGUID guidService, REFIID riid, void** ppvObject)
{
	if (/*guidService == IID_IBindHost ||
		guidService == GUID_IWebBrowserApp ||*/
		guidService == GUID_IXcpControlHost)
	{
		return QueryInterface(riid, ppvObject);
	}

	return E_NOINTERFACE;
}

HRESULT CDuiAxSite::QueryControl( REFIID riid, LPVOID* ppv )
{
	if (m_spOleObject.p == NULL) return E_UNEXPECTED;
	return m_spOleObject->QueryInterface(riid, ppv);
}

STDMETHODIMP CDuiAxSite::SaveObject()
{
	return E_NOTIMPL;
}

STDMETHODIMP CDuiAxSite::GetMoniker( DWORD, DWORD, LPMONIKER* )
{
	return E_NOTIMPL;
}

STDMETHODIMP CDuiAxSite::GetContainer( LPOLECONTAINER* ppContainer )
{
	*ppContainer = m_pContainer;
	return S_OK;
}

STDMETHODIMP CDuiAxSite::ShowObject()
{
	//Device* dev = m_pLayoutMgr->Get3DDevice();
	CRect rc = GetPos();
	CRect rcClip = GetClip();

	HDC hdc = NULL; //dev ? dev->GetDC() : ::GetDC(m_pLayoutMgr->GetPaintWindow());
#ifndef NO3D
	if (m_pOwner->m_pSurface)
	{
		hdc = m_pOwner->m_pSurface->GetDC();
		SetOrg(hdc);
	}
	else
#endif
		hdc = ::GetDC(m_pLayoutMgr->GetPaintWindow());
	if (hdc == NULL) return E_FAIL;

	if (m_spViewObject.p)
	{
		HRESULT hr = m_spViewObject->Draw(m_dwAspect/*DVASPECT_CONTENT*/, -1, NULL, NULL, NULL, hdc, (RECTL*)&rc, (RECTL*)&rcClip, NULL, NULL); 
	}
	
#ifndef NO3D
	//if (dev) dev->ReleaseDC(hdc);
	if (m_pOwner->m_pSurface)
		m_pOwner->m_pSurface->ReleaseDC(hdc);
	else
#endif
		::ReleaseDC(m_pLayoutMgr->GetPaintWindow(), hdc);

	return S_OK;
}

STDMETHODIMP CDuiAxSite::OnShowWindow( BOOL )
{
	return E_NOTIMPL;
}

STDMETHODIMP CDuiAxSite::RequestNewObjectLayout()
{
	return E_NOTIMPL;
}

STDMETHODIMP CDuiAxSite::GetWindow( HWND* phWnd)
{
	*phWnd = m_pLayoutMgr->GetPaintWindow();
	return S_OK;
}

STDMETHODIMP CDuiAxSite::ContextSensitiveHelp( BOOL )
{
	return E_NOTIMPL;
}

STDMETHODIMP CDuiAxSite::CanInPlaceActivate()
{
	return S_OK;
}

STDMETHODIMP CDuiAxSite::OnInPlaceActivate()
{
	_assert(m_bInPlaceActive == FALSE);
	_assert(m_spInPlaceObject.p == NULL);

	m_bInPlaceActive = TRUE;
	OleLockRunning(m_spOleObject, TRUE, FALSE);
	m_bWindowless = FALSE;
	return m_spOleObject.p->QueryInterface(IID_IOleInPlaceObject, (LPVOID*)&m_spInPlaceObject);
	//return S_OK;
	//BOOL bDummy = FALSE;
	//return OnInPlaceActivateEx(&bDummy, 0);
}

STDMETHODIMP CDuiAxSite::OnUIActivate()
{
	m_bUIActive = TRUE;
	// 应该通知容器，调用前一个活动对象的 IOleInPlaceObject->UIDeactivate()
	return S_OK;
}

STDMETHODIMP CDuiAxSite::GetWindowContext( LPOLEINPLACEFRAME* ppFrame, LPOLEINPLACEUIWINDOW* ppDoc, LPRECT prectPos,
										  LPRECT prectClip, LPOLEINPLACEFRAMEINFO pFrameInfo )
{
	if (ppDoc) *ppDoc = m_pContainer;
	if (ppFrame) *ppFrame = m_pContainer;

	// 等待测试，这里获取的矩形应该是相对容器窗口客户区的矩形还是相对父控件客户区的矩形？
	::CopyRect(prectPos, &GetPos());
	::CopyRect(prectClip, &GetClip());

	pFrameInfo->cb = sizeof(OLEINPLACEFRAMEINFO);
	pFrameInfo->fMDIApp = FALSE;
	pFrameInfo->hwndFrame = m_pLayoutMgr->GetPaintWindow();
	pFrameInfo->haccel = NULL;
	pFrameInfo->cAccelEntries = 0;

	return S_OK;
}

STDMETHODIMP CDuiAxSite::Scroll( SIZE )
{
	return E_NOTIMPL;
	//return S_FALSE;
}

STDMETHODIMP CDuiAxSite::OnUIDeactivate( BOOL )
{
	m_bUIActive = FALSE;
	//m_pLayoutMgr->SetFocus(NULL);
	return S_OK;
}

STDMETHODIMP CDuiAxSite::OnInPlaceDeactivate()
{
	m_bInPlaceActive = FALSE;
	m_spInPlaceObject = NULL;
	return S_OK;
}

STDMETHODIMP CDuiAxSite::DiscardUndoState()
{
	return S_OK;
}

STDMETHODIMP CDuiAxSite::DeactivateAndUndo()
{
	if (m_spInPlaceObject.p)
		m_spInPlaceObject->UIDeactivate();
	return S_OK;
}

STDMETHODIMP CDuiAxSite::OnPosRectChange( LPCRECT lprcPosRect )
{
	::CopyRect(&__rcNeeded, lprcPosRect);
	RECT rc = GetPos();
	RECT rcClip = GetClip();
	if (m_spInPlaceObject.p /*&& !::EqualRect(lprcPosRect, &rc)*/)
		m_spInPlaceObject->SetObjectRects(&rc, &rcClip);
	return S_OK;
}

STDMETHODIMP CDuiAxSite::OnInPlaceActivateEx( BOOL* ptNoRedraw, DWORD dwFlags )
{
	_assert(m_bInPlaceActive == FALSE);
	_assert(m_spInPlaceObject.p == NULL);

	m_bInPlaceActive = TRUE;
	::OleLockRunning(m_spOleObject, TRUE, FALSE);

	HRESULT hr = E_FAIL;
	if( dwFlags&ACTIVATE_WINDOWLESS )
	{
		m_bWindowless = TRUE;
		hr = m_spOleObject.p->QueryInterface(IID_IOleInPlaceObjectWindowless, (LPVOID*)&m_spInPlaceObject);
	}

	if (FAILED(hr))
	{
		m_bWindowless = FALSE;
		hr = m_spOleObject.p->QueryInterface(IID_IOleInPlaceObject, (LPVOID*)&m_spInPlaceObject);
	}

	if (m_spInPlaceObject)
	{
		m_spInPlaceObject->SetObjectRects(&GetPos(), &GetClip());
		//HWND hwnd = NULL;
		//m_spInPlaceObject->GetWindow(&hwnd);
		//CDuiWindowBase::FromHWND(hwnd);
		////DuiNode* r = rt(m_pOwner);
		////if (r->m_pSurface && r->m_pSurface->GetDevice()->IsLayeredWindow())
		////	::SetParent(hwnd, rt(m_pOwner)->m_pSurface->GetDevice()->GetGhostWindow());
	}

	if (ptNoRedraw) *ptNoRedraw = FALSE;

	return S_OK;
}

STDMETHODIMP CDuiAxSite::OnInPlaceDeactivateEx( BOOL tNoRedraw )
{
	m_bInPlaceActive = FALSE;
	m_spInPlaceObject = NULL;
	return S_OK;
	//return OnInPlaceDeactivate();
}

STDMETHODIMP CDuiAxSite::RequestUIActivate()
{
	return S_OK;
}

STDMETHODIMP CDuiAxSite::AdjustRect( LPRECT prect )
{
	return S_OK;
}

STDMETHODIMP CDuiAxSite::CanWindowlessActivate()
{
	return S_OK;
}

STDMETHODIMP CDuiAxSite::GetCapture()
{
	return (m_bCapture) ? S_OK : S_FALSE;
}

STDMETHODIMP CDuiAxSite::GetDC( LPCRECT prect, DWORD grfFlags, HDC* phDC )
{
	if (phDC == NULL)
		return E_POINTER;
	if (m_bDCCached)
		return E_FAIL;

	CWindow win(m_pLayoutMgr->GetPaintWindow());
#ifndef NO3D
	if (m_pOwner->m_pSurface)
	{
		*phDC = m_pOwner->m_pSurface->GetDC();
		SetOrg(*phDC);
	}
	else
#endif
		*phDC = win.GetDC();
	if (*phDC == NULL)
		return E_FAIL;

	m_bDCCached = TRUE;

	if (grfFlags & OLEDC_NODRAW)
		return S_OK;

	RECT rect; // = m_pOwner->GetPos();
	win.GetClientRect(&rect);
	if (grfFlags & OLEDC_OFFSCREEN)
	{
		HDC hDCOffscreen = CreateCompatibleDC(*phDC);
		if (hDCOffscreen != NULL)
		{
			HBITMAP hBitmap = CreateCompatibleBitmap(*phDC, rect.right - rect.left, rect.bottom - rect.top);
			if (hBitmap == NULL)
				DeleteDC(hDCOffscreen);
			else
			{
				HGDIOBJ hOldBitmap = SelectObject(hDCOffscreen, hBitmap);
				if (hOldBitmap == NULL)
				{
					DeleteObject(hBitmap);
					DeleteDC(hDCOffscreen);
				}
				else
				{
					DeleteObject(hOldBitmap);
					m_hDCScreen = *phDC;
					*phDC = hDCOffscreen;
				}
			}
		}
	}

	if (grfFlags & OLEDC_PAINTBKGND)
		::FillRect(*phDC, &rect, (HBRUSH) (COLOR_WINDOW+1));
	return S_OK;

	//*phDC = _memdc; // m_pSurface->GetDC();
	//if (grfFlags & OLEDC_PAINTBKGND)
	//{
	//	RECT rc = m_pOwner->GetPos();
	//	::SetWindowOrgEx(*phDC, rc.left, rc.top, NULL);
	//	//::IntersectRect(&rc, &rc, &GetClip(FALSE));
	//	::FillRect(*phDC, &rc, (HBRUSH) (COLOR_WINDOW+1));
	//}

	////if (__cache == NULL)
	////	__cache = NEW org_clip_helper(*phDC, m_pOwner);
	//return S_OK;
}

STDMETHODIMP CDuiAxSite::ReleaseDC( HDC hDC )
{
	m_bDCCached = FALSE;

	CWindow win(m_pLayoutMgr->GetPaintWindow());
	if (m_hDCScreen != NULL)
	{
		//RECT rect;
		//win.GetClientRect(&rect);
		//// Offscreen DC has to be copied to screen DC before releasing the screen dc;
		//::BitBlt(m_hDCScreen, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, hDC, 0, 0, SRCCOPY);
		CRect rc = m_pOwner->GetClipRect();
		::BitBlt(m_hDCScreen, rc.left, rc.top, rc.Width(), rc.Height(), hDC, rc.left, rc.top, SRCCOPY);
		::DeleteDC(hDC);
		hDC = m_hDCScreen;
		m_hDCScreen = NULL;
	}

#ifndef NO3D
	if (m_pOwner->m_pSurface)
		m_pOwner->m_pSurface->ReleaseDC(hDC);
	else
#endif
		win.ReleaseDC(hDC);
	return S_OK;

	//CRect rc = GetClip(FALSE);
	//CSurface* pSurf = m_pLayoutMgr->GetSurface();
	//{
	//	CSurfaceDC dc(pSurf);
	//	::BitBlt(dc.m_hDC, rc.left, rc.top, rc.Width(), rc.Height(), hDC, rc.left, rc.top, SRCCOPY);
	//}
	//pSurf->BltToPrimary(&rc);

	//////if (__cache) __cache = delete (org_clip_helper*)__cache, NULL;
	////RECT rc = GetClip(FALSE);
	////m_pSurface->ReleaseDC(hDC);
	////m_pSurface->BltToPrimary(&rc);
	////CSurface* pSurf = m_pLayoutMgr->GetSurface();
	////pSurf->BltToPrimary(&rc);
	////pSurf->ReleaseDC(hDC);
	//return S_OK;
}

STDMETHODIMP CDuiAxSite::GetFocus()
{
	return m_bHaveFocus ? S_OK : S_FALSE;
}

STDMETHODIMP CDuiAxSite::InvalidateRect( LPCRECT pRect, BOOL bErase )
{
	return (
#ifndef NO3D
		m_pOwner->m_pSurface ? m_pOwner->m_pSurface->Invalidate(TRUE) : 
#endif
		::InvalidateRect(m_pLayoutMgr->GetPaintWindow(), pRect, bErase)), S_OK;
}

STDMETHODIMP CDuiAxSite::InvalidateRgn( HRGN hRgn, BOOL bErase )
{
	return (
#ifndef NO3D
		m_pOwner->m_pSurface ? m_pOwner->m_pSurface->Invalidate(TRUE) : 
#endif
		::InvalidateRgn(m_pLayoutMgr->GetPaintWindow(), hRgn, bErase)), S_OK;
}

STDMETHODIMP CDuiAxSite::OnDefWindowMessage( UINT msg, WPARAM wParam, LPARAM lParam, LRESULT* plResult )
{
	return m_pLayoutMgr->DefMessageHandler(msg, wParam, lParam, *plResult) ? S_OK : S_FALSE;
}

STDMETHODIMP CDuiAxSite::ScrollRect( int dx, int dy, LPCRECT prectScroll, LPCRECT prectClip )
{
	return S_OK;
}

STDMETHODIMP CDuiAxSite::SetCapture( BOOL bCapture )
{
	m_bCapture = bCapture;
	m_pLayoutMgr->SetCapture(bCapture ? m_pOwner : NULL);
	return S_OK;
}

STDMETHODIMP CDuiAxSite::SetFocus( BOOL bFocus )
{
	m_bHaveFocus = bFocus;
	return S_OK;
	//return m_pLayoutMgr->SetFocus(bFocus?m_pOwner:NULL), S_OK;
}

STDMETHODIMP CDuiAxSite::OnControlInfoChanged()
{
	return GetControlInfo(), S_OK;
}

STDMETHODIMP CDuiAxSite::LockInPlaceActive( BOOL fLock )
{
	return S_OK;
}

STDMETHODIMP CDuiAxSite::GetExtendedControl( LPDISPATCH* ppDisp )
{
	if (ppDisp == NULL)
		return E_POINTER;
	return m_spOleObject.QueryInterface(ppDisp);
}

STDMETHODIMP CDuiAxSite::TransformCoords( POINTL* pptHimetric, POINTF* pptContainer, DWORD dwFlags )
{
	HRESULT hr = S_OK;

	CClientDC dc(m_pLayoutMgr->GetPaintWindow());
	SIZE sz;

	if (dwFlags & XFORMCOORDS_HIMETRICTOCONTAINER)
	{
		sz.cx = pptHimetric->x;
		sz.cy = pptHimetric->y;
		dc.HIMETRICtoDP(&sz);

		if (dwFlags & XFORMCOORDS_SIZE)
		{
			pptContainer->x = (float)abs(sz.cx);
			pptContainer->y = (float)abs(sz.cy);
		}
		else if (dwFlags & XFORMCOORDS_POSITION)
		{
			pptContainer->x = (float)sz.cx;
			pptContainer->y = (float)sz.cy;
		}
		else
		{
			hr = E_INVALIDARG;
		}
	}
	else if (dwFlags & XFORMCOORDS_CONTAINERTOHIMETRIC)
	{
		sz.cx = (int)(pptContainer->x);
		sz.cy = (int)(pptContainer->y);
		dc.DPtoHIMETRIC(&sz);

		if (dwFlags & XFORMCOORDS_SIZE)
		{
			pptHimetric->x = abs(sz.cx);
			pptHimetric->y = abs(sz.cy);
		}
		else if (dwFlags & XFORMCOORDS_POSITION)
		{
			pptHimetric->x = sz.cx;
			pptHimetric->y = sz.cy;
		}
		else
		{
			hr = E_INVALIDARG;
		}
	}
	else
	{
		hr = E_INVALIDARG;
	}
	return hr;
}

STDMETHODIMP CDuiAxSite::TranslateAccelerator( LPMSG lpMsg, DWORD grfModifiers )
{
	return S_FALSE;
}

STDMETHODIMP CDuiAxSite::OnFocus( BOOL fGotFocus )
{
	m_bHaveFocus = fGotFocus;
	return S_OK;
}

STDMETHODIMP CDuiAxSite::ShowPropertyFrame()
{
	return E_NOTIMPL;
}

STDMETHODIMP CDuiAxSite::GetTypeInfoCount( unsigned int* )
{
	return E_NOTIMPL;
}

STDMETHODIMP CDuiAxSite::GetTypeInfo( unsigned int, LCID, ITypeInfo** )
{
	return E_NOTIMPL;
}

STDMETHODIMP CDuiAxSite::GetIDsOfNames( REFIID, LPOLESTR*, unsigned int cNames, LCID, DISPID* )
{
	return E_NOTIMPL;
}

STDMETHODIMP CDuiAxSite::Invoke( DISPID dispid, REFIID, LCID, unsigned short, DISPPARAMS*, VARIANT* pvarResult, EXCEPINFO*, unsigned int* )
{
	return m_pContainer->OnAmbientProperty(this, dispid, pvarResult) ? S_OK : DISP_E_MEMBERNOTFOUND;
}

HRESULT CDuiAxSite::CreateControl( REFCLSID clsid )
{
	if (clsid == GUID_NULL) return E_INVALIDARG;
	if (m_spOleObject.p) return E_UNEXPECTED;

	HRESULT hr = m_spOleObject.CoCreateInstance(clsid);
	if( FAILED(hr) ) return hr;

	// More control creation stuff
	hr = m_spOleObject->GetMiscStatus(DVASPECT_CONTENT, &m_dwMiscStatus);

	// Initialize control
	if( (m_dwMiscStatus & OLEMISC_SETCLIENTSITEFIRST) != 0 )
		m_spOleObject->SetClientSite(this);

	// TODO: 要改成用属性包来初始化
	CComQIPtr<IPersistPropertyBag> spPPB = m_spOleObject;
	if (spPPB.p)
	{
		hr = spPPB->Load(&m_pOwner->_attributes, NULL);
	}
	if ( FAILED(hr) )
	{
		CComQIPtr<IPersistStreamInit> spPersistStreamInit = m_spOleObject;
		if ( spPersistStreamInit.p != NULL )
			hr = spPersistStreamInit->InitNew();
	}
	if ( FAILED(hr) )
	{
		if (m_dwMiscStatus & OLEMISC_SETCLIENTSITEFIRST)
			m_spOleObject->SetClientSite(NULL);

		m_dwMiscStatus = 0;
		m_spOleObject.Release();
		return hr;
	}

	if ( (m_dwMiscStatus & OLEMISC_SETCLIENTSITEFIRST) == 0 )
		m_spOleObject->SetClientSite(this);

	m_dwViewObjectType = 0;
	m_dwAspect = DVASPECT_CONTENT;
	hr = m_spOleObject->QueryInterface(__uuidof(IViewObjectEx), (void**) &m_spViewObject);
	if (SUCCEEDED(hr))
	{
		m_dwViewObjectType = 7;
		//DWORD dwViewStatus;
		//m_spViewObject->GetViewStatus(&dwViewStatus);
		//if (dwViewStatus & VIEWSTATUS_DVASPECTTRANSPARENT)
		//	m_dwAspect = DVASPECT_TRANSPARENT;
	}

	if (FAILED(hr))
	{
		hr = m_spOleObject->QueryInterface(__uuidof(IViewObject2), (void**) &m_spViewObject);
		if (SUCCEEDED(hr))
			m_dwViewObjectType = 3;
	}

	if (FAILED(hr))
	{
		hr = m_spOleObject->QueryInterface(__uuidof(IViewObject), (void**) &m_spViewObject);
		if (SUCCEEDED(hr))
			m_dwViewObjectType = 1;
	}

	m_spOleObject->SetHostNames(OLESTR("DUIActiveX"), NULL);

	//// Activate and done...
	//// 第一次显示时才激活
	//if( (m_dwMiscStatus & OLEMISC_INVISIBLEATRUNTIME) == 0 )
	//{
	//	hr = m_spOleObject->DoVerb(OLEIVERB_INPLACEACTIVATE, NULL, this, 0, m_pLayoutMgr->GetPaintWindow(), &(m_pOwner)->GetPos());
	//	//::RedrawWindow(m_pLayoutMgr->GetPaintWindow(), &rt(m_pOwner)->GetPos(), NULL, RDW_INVALIDATE | RDW_UPDATENOW | RDW_ERASE | RDW_INTERNALPAINT | RDW_FRAME);
	//}

	CComQIPtr<IObjectWithSite> spSite = m_spOleObject;
	if( spSite )
		spSite->SetSite(static_cast<IOleClientSite*>(this));

	return hr;
}

HRESULT CDuiAxSite::ActivateAx()
{
	_assert(m_bInPlaceActive == FALSE);
	_assert(m_spOleObject != NULL);

	HRESULT hr = S_OK;
	if( (m_dwMiscStatus & OLEMISC_INVISIBLEATRUNTIME) == 0 )
	{
		ResetSurface();
		hr = m_spOleObject->DoVerb(OLEIVERB_INPLACEACTIVATE, NULL, this, 0, m_pLayoutMgr->GetPaintWindow(), &GetPos());
		//::RedrawWindow(m_pLayoutMgr->GetPaintWindow(), &rt(m_pOwner)->GetPos(), NULL, RDW_INVALIDATE | RDW_UPDATENOW | RDW_ERASE | RDW_INTERNALPAINT | RDW_FRAME);

		if (m_spInPlaceObject.p)
		{
			HWND hwnd = NULL;
			m_spInPlaceObject->GetWindow(&hwnd);
			if (::IsWindow(hwnd))
				::SetPropW(hwnd, L"Ole", m_spOleObject.p);
		}
	}
	return hr;
}

void CDuiAxSite::ReleaseAll()
{
	if (m_bReleaseAll) return;
	m_bReleaseAll = TRUE;

	//if (m_spViewObject != NULL)
	//	m_spViewObject->SetAdvise(DVASPECT_CONTENT, 0, NULL);

	//if(m_dwAdviseSink != 0xCDCDCDCD)
	//{
	//	AtlUnadvise(m_spUnknown, m_iidSink, m_dwAdviseSink);
	//	m_dwAdviseSink = 0xCDCDCDCD;
	//}

	if (m_spOleObject)
	{
		//m_spOleObject->Unadvise(m_dwOleObject);
		m_spOleObject->Close(OLECLOSE_NOSAVE);
		m_spOleObject->SetClientSite(NULL);
	}

	{
		CComQIPtr<IObjectWithSite> spSite(m_spOleObject);
		if (spSite != NULL)
			spSite->SetSite(NULL);
	}

	m_spViewObject = NULL;
	m_dwViewObjectType = 0;
	m_spInPlaceObject = NULL;
	m_spOleObject = NULL;

	m_bInPlaceActive = FALSE;
	m_bWindowless = FALSE;
	m_bInPlaceActive = FALSE;
	m_bUIActive = FALSE;
	m_bCapture = FALSE;

	//if (m_pSurface) m_pSurface = (m_pSurface->Delete(), NULL);
}

RECT CDuiAxSite::GetPos()
{
	return m_pOwner->GetClientRect();
}

RECT CDuiAxSite::GetClip(BOOL bEnsureVisible/*=TRUE*/)
{
	//static const RECT __rcClip = {-1,-1,0,0};
	RECT rcClip = m_pOwner->GetClipRect();
	////if (::IsRectEmpty(&rcClip) && bEnsureVisible)
	////	rcClip = __rcClip;

	// 有窗口控件的显示有一种现象，当窗口部分被裁剪时，能够正确地仅显示裁剪的部分
	// 当窗口被完全遮挡时，窗口并未隐藏，而是完全显示出来，并且以SPY++可探测的窗口形式存在，
	// 且遮住所有其它的内容。
	// 这里取巧一下，如果窗口被完全遮挡，则设置裁剪矩形的长宽都为1像素来欺骗窗口
	if (!m_bWindowless && m_spInPlaceObject.p && ::IsRectEmpty(&rcClip) && bEnsureVisible)
	{
		rcClip = GetPos();
		rcClip.right = rcClip.left + 1;
		rcClip.bottom = rcClip.top + 1;
	}
	return rcClip;
}

STDMETHODIMP CDuiAxSite::GetHostOptions(DWORD* pdwOptions)
{
	if ( pdwOptions == NULL )
		return E_POINTER;
	*pdwOptions = XcpHostOption_DisableFullScreen|XcpHostOption_EnableCrossDomainDownloads|
				XcpHostOption_EnableHtmlDomAccess|XcpHostOption_EnableScriptableObjectAccess;
	if (!m_pLayoutMgr->GetAttributeBool(L"EnableManaged"))
		*pdwOptions |= XcpHostOption_DisableManagedExecution;
	return S_OK;
}

STDMETHODIMP CDuiAxSite::NotifyLoaded (void)
{
	ATLTRACE(_T("CDuiAxSite::IXcpControlHost2::NotifyLoaded\n"));
	return S_OK;
}

STDMETHODIMP CDuiAxSite::NotifyError(BSTR bstrError, BSTR bstrSource, long nLine, long nColumn)
{
	LineLog(_T("CDuiAxSite::IXcpControlHost2::NotifyError (%s) on line %d\n"), bstrError, nLine);
	return S_OK;
}

STDMETHODIMP CDuiAxSite::InvokeHandler (BSTR bstrName, VARIANT varArg1, VARIANT varArg2, VARIANT* pvarResult)
{
	LineLog((L"CDuiAxSite::IXcpControlHost2::InvokeHandler (%s)\n"), bstrName);
	return E_NOTIMPL;
}

STDMETHODIMP CDuiAxSite::GetBaseUrl (BSTR* pbstrUrl)
{
	if ( pbstrUrl == NULL )
		return E_POINTER;

	// 布局管理器读取config中的BASEURL属性，如果未设置，默认为 file:///....../my.exe。
	// 注意：SILVERLIGHT仅支持正斜杠类型的file协议，如下面的第二种
	// 1. file://d:\\my.exe
	// 2. file:///d:/my.exe
	// 首先直接以Source作为baseurl，如果不存在source，则读取配置
	LPCOLESTR pSrc = (m_pOwner)->GetAttributeString(L"source");
	BOOL bValid = (pSrc!=NULL);
	if (bValid)
	{
		bValid = FALSE;
		LPCOLESTR p = pSrc;
		while (*p)
		{
			if (*p==L':')
			{
				bValid = TRUE;
				break;
			}
			p++;
		}
	}
	if (!bValid) pSrc = m_pLayoutMgr->GetAttributes().GetAttributeString(L"baseUrl");
	*pbstrUrl = ::SysAllocString(pSrc);
	return S_OK;
	//ATLTRACENOTIMPL(_T("CDuiAxSite::IXcpControlHost2::GetBaseUrl\n"));
}

STDMETHODIMP CDuiAxSite::GetNamedSource (BSTR bstrSourceName, BSTR* pbstrSource)
{
	ATLTRACENOTIMPL(_T("CDuiAxSite::IXcpControlHost2::GetNamedSource\n"));
}

STDMETHODIMP CDuiAxSite::DownloadUrl (BSTR bstrUrl, IXcpControlDownloadCallback *pCallback, IStream **ppStream)
{
	// 返回值说明：
	// S_OK - 同步下载成功。内容在流中返回。
	// S_FALSE - 不下载，始终使用寄宿的控件的固有下载程序。
	// E_FAIL - 不下载任何内容。寄宿的控件将引发相应的错误和事件，作为对失败的下载的响应。
	// E_PENDING - 采用异步下载，需保存作为回调的 pCallback，并在下载成功后调用它的 OnUrlDownloaded 方法。

	// 基于SILVERLIGHT安全限制，不允许使用res协议，这里只处理res协议，其它协议只做默认下载处理
	if (lstrlenW(bstrUrl)>4 && bstrUrl[0]==L'r' && bstrUrl[1]==L'e' && bstrUrl[2]==L's' && bstrUrl[3]==L':')
	{
		// res 协议采用同步方式下载
		return CDownload::LoadUrlToStream(bstrUrl, ppStream, NULL);
	}

	// 其它协议类型采用默认下载
	return S_FALSE;

	//HRESULT hr = S_OK;
	//if ( ppStream != NULL ) *ppStream = NULL;

	// TODO: ========================= PROGID: AgControl.AgControl
	//LPOLESTR pszPath = bstrUrl;
	//if ( _wcsnicmp(L"file:///", pszPath, 8) == 0 )    // 8 = wcslen(L"file:///")
	//	pszPath += 8;
	//else
	//	hr = S_FALSE;
	//if ( hr == S_OK )
	//{
	//	// Typically, you can parse the URL and find the correct resource
	//	// Url format: "file:///<applicationpath>#resource"
	//	// where: <applicationpath> is your application path (e.g.: c:/program files/yourapplication/yourapp.exe)
	//	//        #resource is your resource number (e.g.:#1001) which represents your .xap resource file.
	//	HGLOBAL hGlobal = GetResourceData(_Module.GetResourceInstance(), MAKEINTRESOURCE(IDR_BIN1), _T("BIN"));
	//	if ( hGlobal != NULL )
	//	{
	//		CComPtr<IStream> pStream;
	//		hr = CreateStreamOnHGlobal(hGlobal, TRUE, &pStream);
	//		if ( SUCCEEDED(hr) )
	//		{
	//			if ( pCallback != NULL ) {
	//				hr = pCallback->OnUrlDownloaded(hr, pStream);
	//			}
	//			else if ( ppStream != NULL ) {
	//				*ppStream = pStream.Detach();
	//			}
	//		}
	//		else
	//		{
	//			GlobalFree(hGlobal);
	//			hr = E_FAIL;
	//		}
	//	}
	//	else
	//	{
	//		hr = E_FAIL;
	//	}
	//}
	//return hr;
}

STDMETHODIMP CDuiAxSite::GetCustomAppDomain(IUnknown **ppAppDomain)
{
	ATLTRACENOTIMPL(_T("CDuiAxSite::IXcpControlHost2::GetCustomAppDomain\n"));
}

STDMETHODIMP CDuiAxSite::GetControlVersion (UINT * puMajorVersion, UINT * puMinorVersion)
{
	*puMajorVersion = MAKELONG(0,2);
	*puMinorVersion = MAKELONG(0,31005);
	return S_OK;
}

void CDuiAxSite::SetOrg( HDC hdc )
{
	POINT pt = DuiNode::GetOffset(/*CDuiControlExtension::GetOffsetParent*/(m_pOwner), NULL, CPoint(0,0));
	CRect rc = m_pOwner->GetPosBy(CRect(0,0,0,0));
	pt.x -= rc.left;
	pt.y -= rc.top;
	::SetWindowOrgEx(hdc, pt.x, pt.y, NULL);
}

void ControlExtension::__Use( HANDLE hSource )
{
	m_pOwner = DuiHandle<DuiNode>(hSource);
}

void ControlExtension::__Unuse( HANDLE )
{
	m_pOwner = NULL;
}
