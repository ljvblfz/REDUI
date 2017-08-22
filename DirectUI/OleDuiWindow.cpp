// OleDuiWindow.cpp : COleDuiWindow 的实现

#include "stdafx.h"
#include "DuiEvent.h"
#include "OleDuiWindow.h"
#include "DirectUIWindow.h"
#include "DuiControl.h"
#include "duicore/mttimer.h"
#include "duicore/DuiPluginsManager.h"

#include "../common/mimefilter.h"

//LPCWSTR prop_layout = L"DuiOleWindow";
//LPCWSTR prop_device = L"d3d_device";

//////////////////////////////////////////////////////////////////////////
// API
HWND DUIAPI CreateDirectUIWindowA(HWND hwndParent, LPCSTR lpszWindowName, const RECT rc, DWORD dwStyle, DWORD dwExStyle, HMENU hMenu, HINSTANCE hInstance)
{
	LPCSTR name = lpszWindowName;
	if (::PathIsURLA(lpszWindowName) /*|| !::PathIsRelativeA(lpszWindowName)*/)
		name = NULL;
	HWND hwnd = CreateWindowExA(dwExStyle, "DirectUIHWND", name, dwStyle, rc.left, rc.top, rc.right-rc.left, rc.bottom-rc.top, hwndParent, hMenu, hInstance, NULL);
	if (hwnd == NULL) return NULL;
	if (name != lpszWindowName) // is url
		::SendMessage(hwnd, WM_LOADURL, (WPARAM)(LPCOLESTR)CA2W(lpszWindowName), (LPARAM)hInstance);
	return hwnd;
}

HWND DUIAPI CreateDirectUIWindowW(HWND hwndParent, LPCWSTR lpszWindowName, const RECT rc, DWORD dwStyle, DWORD dwExStyle, HMENU hMenu, HINSTANCE hInstance)
{
	LPCWSTR name = lpszWindowName;
	if (::PathIsURLW(lpszWindowName) /*|| !::PathIsRelativeW(lpszWindowName)*/)
		name = NULL;
	HWND hwnd = CreateWindowExW(dwExStyle, L"DirectUIHWND", name, dwStyle, rc.left, rc.top, rc.right-rc.left, rc.bottom-rc.top, hwndParent, hMenu, hInstance, NULL);
	if (hwnd == NULL) return NULL;
	if (name != lpszWindowName) // is url
		::SendMessage(hwnd, WM_LOADURL, (WPARAM)lpszWindowName, (LPARAM)hInstance);
	return hwnd;
}

BOOL DUIAPI IsDirectUIMessage(HWND hwndDirectUI, LPMSG lpMsg)
{
	return CDuiLayoutManager::PreTranslateMessage((const LPMSG)lpMsg);
	//if (lpMsg==NULL) return FALSE;

	//COleDuiWindow* pWin = COleDuiWindow::FromHandle(hwndDirectUI);
	//if (pWin == NULL)
	//	return FALSE;

	//return pWin->PreTranslateMessage(lpMsg);
}

BOOL DUIAPI IsDirectUIWindow(HWND hwnd)
{
	return COleDuiWindow::FromHandle(hwnd) != NULL;
}

void DUIAPI SetDirectUIWindowExternal(HWND hwndDirectUI, IDispatch* pDisp)
{
	COleDuiWindow* pwin = COleDuiWindow::FromHandle(hwndDirectUI);
	if (pwin) pwin->m_pExternal = pDisp;
}

HDE DUIAPI GetRootElement(HWND hwndDirectUI)
{
	COleDuiWindow* pwin = COleDuiWindow::FromHandle(hwndDirectUI);
	if (pwin==NULL || pwin->GetRoot()==NULL)
		return NULL;
	return pwin->GetRoot()->Handle();
}

HDE DUIAPI GetElementById(HWND hwndDirectUI, LPCWSTR lpszId)
{
	COleDuiWindow* pwin = COleDuiWindow::FromHandle(hwndDirectUI);
	DuiNode* pc = pwin ? pwin->GetControlById(lpszId) : NULL;
	return DuiHandle<DuiNode>(pc);
}

BOOL DUIAPI ParseStyle(HWND hwnd, LPCWSTR lpszTargetName, DWORD dwTargetState, LPCWSTR lpszStyle)
{
	COleDuiWindow* pwin = COleDuiWindow::FromHandle(hwnd);
	if (pwin == NULL) return FALSE;
	return NULL != pwin->m_pLayoutMgr->ParseStyle(lpszTargetName, dwTargetState, lpszStyle);
}

BOOL DUIAPI StreamFromUrl(LPCOLESTR lpszUrl, IStream** ppStream, HINSTANCE hMod)
{
	return SUCCEEDED(CDownload::LoadUrlToStream(lpszUrl, ppStream, hMod));
}

BOOL DUIAPI BstrFromStream(IStream* pStream, BSTR* pbstr)
{
	if (pStream==NULL || pbstr==NULL) return FALSE;
	*pbstr = NULL;

	CComBSTR str;
	CDownload::StreamToBSTR(pStream, str);
	if (str)
		return SUCCEEDED(str.CopyTo(pbstr));
	return FALSE;
}

//////////////////////////////////////////////////////////////////////////
// internal API
// 
#ifndef NO3D
Device* DeviceFromHWND(HWND hWnd)
{
	return (Device*)::GetPropW(hWnd, AtomString(atom_device));
}
#endif

struct ErrInfo
{
	CComBSTR err;
	static void CALLBACK OnScriptError(LPACTIVESCRIPTERROR pErr, LPVOID lpParam)
	{
		ErrInfo* pErrInfo = (ErrInfo*)lpParam;

		//// 取位置信息
		//DWORD sc;
		//ULONG ulLineNum;
		//LONG lCharPos;
		//pErr->GetSourcePosition(&sc, &ulLineNum, &lCharPos);

		// 取错误行源码
		CComBSTR bstrLine(256, L"");
		pErr->GetSourceLineText(&bstrLine);

		// 取异常信息
		EXCEPINFO ei;
		pErr->GetExceptionInfo(&ei);

		pErrInfo->err = L"======SCRIPT Error====== \r\nCode: \"";
		pErrInfo->err.Append(bstrLine);
		pErrInfo->err.Append(L"\"\r\nDescription: \"");
		pErrInfo->err.Append(ei.bstrDescription);
		pErrInfo->err.Append(L"\"\r\n");
		//wsprintfW(str, L"=========代码“%s”中位置为 %d 行 %d 列发生错误：%s。\r\n", bstrLine, ulLineNum, lCharPos, ei.bstrDescription);
	}
};
extern LPCOLESTR g_window_name;
BOOL DUIAPI CalcScriptExpressionA(HWND hwndDirectUI, LPCSTR lpszExpression, LPVARIANT lpVarResult)
{
	COleDuiWindow* pwin = COleDuiWindow::FromHandle(hwndDirectUI);
	if (pwin && pwin->m_pLayoutMgr && pwin->m_pLayoutMgr->m_script.IsValid())
	{
		ErrInfo ei;
		pwin->m_pLayoutMgr->m_script.SetCallback(NULL, NULL, (fnScriptNotifyCallback)&ErrInfo::OnScriptError, NULL, NULL, &ei);
#ifndef NO3D
		d3d::SetActiveScriptWindow(hwndDirectUI);
#endif
		BOOL bRet = SUCCEEDED(pwin->m_pLayoutMgr->m_script.CalcExpression((LPCOLESTR)CA2W(lpszExpression), lpVarResult));
#ifndef NO3D
		d3d::SetActiveScriptWindow(NULL);
#endif
		pwin->m_pLayoutMgr->m_script.SetCallback();
		if (ei.err.Length() > 0 && lpVarResult)
		{
			::VariantClear(lpVarResult);
			CComVariant v(ei.err);
			v.Detach(lpVarResult);
			return FALSE;
		}
		return bRet;
	}
	return FALSE;
}
BOOL DUIAPI CalcScriptExpressionW(HWND hwndDirectUI, LPCWSTR lpszExpression, LPVARIANT lpVarResult)
{
	COleDuiWindow* pwin = COleDuiWindow::FromHandle(hwndDirectUI);
	if (pwin && pwin->m_pLayoutMgr && pwin->m_pLayoutMgr->m_script.IsValid())
	{
		ErrInfo ei;
		pwin->m_pLayoutMgr->m_script.SetCallback(NULL, NULL, (fnScriptNotifyCallback)&ErrInfo::OnScriptError, NULL, NULL, &ei);
#ifndef NO3D
		d3d::SetActiveScriptWindow(hwndDirectUI);
#endif
		BOOL bRet = SUCCEEDED(pwin->m_pLayoutMgr->m_script.CalcExpression(lpszExpression, lpVarResult));
#ifndef NO3D
		d3d::SetActiveScriptWindow(NULL);
#endif
		pwin->m_pLayoutMgr->m_script.SetCallback();
		if (ei.err.Length() > 0 && lpVarResult)
		{
			::VariantClear(lpVarResult);
			CComVariant v(ei.err);
			v.Detach(lpVarResult);
			return FALSE;
		}
		return bRet;
	}
	return FALSE;
}

// API END
//////////////////////////////////////////////////////////////////////////

CDuiLayoutManager* LayoutMgrFromHwnd(HWND hwnd)
{
	COleDuiWindow* pwin = COleDuiWindow::FromHandle(hwnd);
	return pwin ? pwin->m_pLayoutMgr : NULL;
}

// COleDuiWindow

COleDuiWindow* COleDuiWindow::FromHandle(HWND hwnd)
{
	if (!::IsWindow(hwnd) )
		return NULL;

	CComObject<COleDuiWindow>* pWin = (CComObject<COleDuiWindow>*)::GetPropW(hwnd, AtomString(atom_layout));
	if (pWin==NULL || ::IsBadReadPtr(pWin, sizeof(CComObject<COleDuiWindow>)))
		return NULL;
	return pWin;
}

BOOL COleDuiWindow::LoadXML( LPCOLESTR lpszXml, HINSTANCE hMod/*=NULL*/ )
{
	ATLASSERT(::IsWindow(m_hWnd));
	if (!::IsWindow(m_hWnd)) return FALSE;

	// clear all old timers
	for (int i=m_timers.GetSize()-1; i>=0; i--) KillMtTimer((DWORD)m_timers.GetKeyAt(i));
	m_timers.RemoveAll();
	m_timerOnce.RemoveAll();

	if (m_pLayoutMgr)
	{
		m_pLayoutMgr->RemoveNotifier(this);
		delete m_pLayoutMgr;
		m_pLayoutMgr = NULL;
	}

	m_pLayoutMgr = NEW CDuiLayoutManager;
	if (m_pLayoutMgr == NULL) return FALSE;
	m_pLayoutMgr->AddNotifier(this);
	m_pLayoutMgr->SetResourceInstance(hMod);
	m_pLayoutMgr->Init(m_hWnd, this);
	return m_pLayoutMgr->LoadFromXml(lpszXml);
}

BOOL COleDuiWindow::LoadFromUrl( LPCOLESTR lpszUrl, HINSTANCE hMod/*=NULL*/ )
{
	CComPtr<IStream> spStream;
	HRESULT hr = CDownload::LoadUrlToStream(lpszUrl, &spStream, hMod);
	if (FAILED(hr)) return FALSE;
	CComBSTR xml;
	CDownload::StreamToBSTR(spStream, xml);
	if (xml.Length() == 0) return FALSE;
	return LoadXML(xml, hMod);
}

void COleDuiWindow::OnFinalMessage( HWND hWnd )
{
	// clear all old timers
	for (int i=m_timers.GetSize()-1; i>=0; i--) KillMtTimer((DWORD)m_timers.GetKeyAt(i));
	m_timers.RemoveAll();
	m_timerOnce.RemoveAll();

#ifndef NO3D
	Device* dev = (Device*)::GetPropW(hWnd, AtomString(atom_device));
	if (dev)
		dev->Relayout();
#endif

	if (m_pLayoutMgr)
	{
		m_pLayoutMgr->RemoveNotifier(this);
		delete m_pLayoutMgr;
		m_pLayoutMgr = NULL;
	}
	if (m_pExternal) m_pExternal.Release();
}

void COleDuiWindow::OnNotify( DuiNotify& msg )
{

}

BOOL COleDuiWindow::RegisterWindowClass()
{
	WNDCLASSW wc = { 0 };
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS | CS_GLOBALCLASS | CS_OWNDC;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hIcon = NULL;
	wc.lpfnWndProc = COleDuiWindow::__DuiWndProc;
	wc.hInstance = NULL; //_AtlBaseModule.GetResourceInstance();
	wc.hCursor = ::LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wc.lpszMenuName  = NULL;
	wc.lpszClassName = L"DirectUIHWND";
	ATOM ret = ::RegisterClassW(&wc);
	ATLASSERT(ret!=NULL || ::GetLastError()==ERROR_CLASS_ALREADY_EXISTS);
	return ret != NULL || ::GetLastError() == ERROR_CLASS_ALREADY_EXISTS;
}

BOOL COleDuiWindow::UnregisterWindowClass()
{
	return ::UnregisterClassW(L"DirectUIHWND", _AtlBaseModule.GetModuleInstance());
}

LRESULT CALLBACK COleDuiWindow::__DuiWndProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	// 阻止其它线程或其它进程发送消息
	if (::InSendMessage())
		return ::DefWindowProcW(hWnd, uMsg, wParam, lParam);
		//return ::ReplyMessage(0), 0;

	COleDuiWindow* pThis = NULL;
	if( uMsg == WM_NCCREATE )
	{
		//::SendMessage(hWnd, WM_SETFONT, (WPARAM)(HFONT)::GetStockObject(DEFAULT_GUI_FONT), 0);
		CComObject<COleDuiWindow>* pWin = NULL;
		pWin->CreateInstance(&pWin);
		if (pWin == NULL) return FALSE;
		pWin->AddRef();
		pWin->m_hWnd = hWnd;
		BOOL b = ::SetPropW(hWnd, AtomString(atom_layout)/*prop_layout*/, (HANDLE)pWin);
		pWin->m_winExt._win = hWnd;
		pWin->m_peer.AddObject(GetCreatableNamedItemPeer());
		pWin->m_peer.AddObject(pWin->m_winExt.GetDispatch(), pWin->m_winExt.__ClassName());
		pWin->m_peer.SetNamedItemHost(pWin);
		pThis = pWin;
	} 
	else
	{
		pThis = FromHandle(hWnd);
		if (pThis == NULL) return ::DefWindowProc(hWnd, uMsg, wParam, lParam);
		if( uMsg == WM_NCDESTROY )
		{
			::RemovePropW(hWnd, AtomString(atom_layout));
			pThis->OnFinalMessage(hWnd);
			pThis->m_peer.SetNamedItemHost(NULL);
			pThis->m_peer.RemoveObject(pThis->m_winExt.GetDispatch());
			pThis->m_peer.RemoveObject(GetCreatableNamedItemPeer());
			pThis->m_winExt._win.m_hWnd = NULL;
			pThis->m_hWnd = NULL;

#ifndef NO3D
			Device* dev = (Device*)::GetPropW(hWnd, AtomString(atom_device));
			if (dev)
			{
				pThis->m_peer.RemoveObject(dev->GetDispatch());
				dev->SetWindowObject(NULL);
				dev->Dispose();
				::RemovePropW(hWnd, AtomString(atom_device));
			}
#endif

			pThis->Release();

			return ::DefWindowProcW(hWnd, uMsg, wParam, lParam);
		}
	}

	//if (uMsg == WM_CREATE)
	//	::SendMessage(hWnd, WM_SETFONT, (WPARAM)(HFONT)::GetStockObject(DEFAULT_GUI_FONT), 0);
	//else
	switch (uMsg)
	{
	case WM_LOADXML:
	case WM_LOADURL:
		{
#ifndef NO3D
			if (d3d::IsValid(true))
			{
				Device* dev = (Device*)::GetPropW(hWnd, AtomString(atom_device));
				if (dev==NULL)
				{
					::SetPropW(hWnd, AtomString(atom_device), (HANDLE)(dev=Device::New(hWnd)));
					if (dev)
					{
						dev->SetWindowObject((IDuiWindow*)pThis);
						pThis->m_peer.AddObject(dev->GetDispatch(), dev->__ClassName());
					}
				}
				else // 设备已经存在，但需要释放原始布局中的资源
					dev->Relayout();
			}
#endif

			if (uMsg == WM_LOADXML)
				return pThis->LoadXML((LPCOLESTR)wParam);
			else // if (uMsg == WM_LOADURL)
				return pThis->LoadFromUrl((LPCOLESTR)wParam, (HINSTANCE)lParam);
		}
		break;
	}

	LRESULT lRes = 0;
	if (pThis->m_pLayoutMgr && pThis->m_pLayoutMgr->MessageHandler(uMsg, wParam, lParam, lRes))
		return lRes;
	return ::DefWindowProc(hWnd, uMsg, wParam, lParam);
}

DuiNode* COleDuiWindow::GetControlById(LPCOLESTR szId)
{
	return m_pLayoutMgr ? m_pLayoutMgr->FindControl(szId) : NULL;
}

BOOL COleDuiWindow::GetNamedItem(LPCOLESTR name, VARIANT* pRet)
{
	DuiNode* pCtrl = GetControlById(name);
	if (pCtrl)
	{
		IDispatch* disp = pCtrl->GetObject(TRUE);
		if (disp)
		{
			if (pRet)
			{
				V_VT(pRet)=VT_DISPATCH;
				V_DISPATCH(pRet)=disp;
				disp->AddRef();
			}
			return TRUE;
		}
	}

#ifndef NO3D
	Device* dev;
	CComPtr<IDispatch> disp;
	if (m_pLayoutMgr && (dev=m_pLayoutMgr->Get3DDevice()) && dev->FindNamedObject(name, FRT_ALL, &disp) && disp.p)
	{
		if (pRet)
		{
			V_VT(pRet)=VT_DISPATCH;
			V_DISPATCH(pRet)=disp.Detach();
		}
		return TRUE;
	}
#endif

	return FALSE;
}

IDispatch* COleDuiWindow::GetWindowObject()
{
	CComPtr<IDispatch> disp;
	QueryInterface(IID_IDispatch, (void**)&disp);
	return disp.p;
}

HRESULT COleDuiWindow::CreateControlObject( DuiNode* pCtrl, IDispatch** ppDisp )
{
	ATLASSERT(pCtrl && ppDisp);
	if (pCtrl==NULL || ppDisp==NULL) return E_POINTER;
	*ppDisp = NULL;

	// ...
	CComObject<COleDuiControl>* ctl = NULL;
	ctl->CreateInstance(&ctl);
	if (ctl == NULL) return E_OUTOFMEMORY;
	if (!ctl->Init(pCtrl)) return E_FAIL;
	return ctl->QueryInterface(IID_IDispatch, (void**)ppDisp);
}

void COleDuiWindow::OnLayoutBegin()
{

}

void COleDuiWindow::OnLayoutReady()
{
#ifndef NO3D
	d3d::SetActiveScriptWindow(m_hWnd);
#endif
	DISPPARAMS dp = {NULL, NULL, 0, 0};
	for (int i=0, num=m_winExt.m_onReadys.GetSize(); i<num; i++)
	{
		CComPtr<IDispatch>& cb = m_winExt.m_onReadys[i];
		CComVariant vRet;
		if (cb)
			cb->Invoke(DISPID_VALUE, IID_NULL, LOCALE_SYSTEM_DEFAULT, DISPATCH_METHOD, &dp, &vRet, NULL, NULL);
	}

	// 调用完毕，没有继续存在的必要，删除所有回调
	m_winExt.m_onReadys.RemoveAll();
#ifndef NO3D
	d3d::SetActiveScriptWindow(NULL);
#endif
}

void COleDuiWindow::OnLayoutEnd()
{
#ifndef NO3D
	d3d::SetActiveScriptWindow(m_hWnd);
#endif
	DISPPARAMS dp = {NULL, NULL, 0, 0};
	for (int i=0, num=m_winExt.m_onEnds.GetSize(); i<num; i++)
	{
		CComPtr<IDispatch>& cb = m_winExt.m_onEnds[i];
		CComVariant vRet;
		if (cb)
			cb->Invoke(DISPID_VALUE, IID_NULL, LOCALE_SYSTEM_DEFAULT, DISPATCH_METHOD, &dp, &vRet, NULL, NULL);
	}

	// 调用完毕，没有继续存在的必要，删除所有回调
	m_winExt.m_onEnds.RemoveAll();
#ifndef NO3D
	d3d::SetActiveScriptWindow(NULL);
#endif
}

void COleDuiWindow::OnLayoutError()
{

}

STDMETHODIMP COleDuiWindow::get_window( IDispatch** pVal )
{
	return QueryInterface(IID_IDispatch, (void**)pVal);
}

STDMETHODIMP COleDuiWindow::get_event( IDispatch** pVal )
{
	if (pVal==NULL) return E_POINTER;
	*pVal = NULL;
	if (m_pLayoutMgr && m_pLayoutMgr->GetEvent())
	{
		CComObject<COleDuiEvent>* evt = NULL;
		evt->CreateInstance(&evt);
		if (evt == NULL) return E_OUTOFMEMORY;
		evt->Init(m_pLayoutMgr->GetEvent());
		return evt->QueryInterface(pVal);
	}
	return S_OK;
}

STDMETHODIMP COleDuiWindow::get_external( IDispatch** pVal )
{
	if (pVal == NULL) return E_POINTER;
	*pVal = NULL;
	if (m_pExternal) return m_pExternal.QueryInterface(pVal);
	return S_OK;
}
STDMETHODIMP COleDuiWindow::get_focus(IDispatch** pVal)
{
	// TODO: 在此添加实现代码
	if (pVal == NULL) return E_POINTER;
	*pVal = NULL;
	if (m_pLayoutMgr && m_pLayoutMgr->GetFocus())
	{
		*pVal = m_pLayoutMgr->GetFocus()->GetObject(TRUE);
		if (*pVal) (*pVal)->AddRef();
	}

	return S_OK;
}
STDMETHODIMP COleDuiWindow::get_body(IDispatch** pVal)
{
	if (pVal == NULL) return E_POINTER;
	*pVal = NULL;
	DuiNode* pRoot = GetRoot();
	if (pRoot)
	{
		*pVal = (pRoot)->GetObject(TRUE);
		if (*pVal) (*pVal)->AddRef();
	}

	return S_OK;
}

//STDMETHODIMP COleDuiWindow::GetIDsOfNames( REFIID riid, LPOLESTR* rgszNames, UINT cNames, LCID lcid, DISPID* rgdispid )
//{
//	HRESULT hr;
//	if (SUCCEEDED(hr=baseDispatch::GetIDsOfNames(riid, rgszNames, cNames, lcid, rgdispid)))
//		return hr;
//
//	if (hr == DISP_E_UNKNOWNNAME)
//	{
//		if (*rgdispid==DISPID_UNKNOWN && GetControlById(*rgszNames))
//		{
//			*rgdispid = DISPID_EXPANDO_BASE - 1;
//			m_curNamedItem = *rgszNames;
//			return S_OK;
//		}
//	}
//	return hr;
//}
//
//STDMETHODIMP COleDuiWindow::Invoke( DISPID dispidMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS* pdispparams, VARIANT* pvarResult, EXCEPINFO* pexcepinfo, UINT* puArgErr )
//{
//	if (dispidMember==DISPID_EXPANDO_BASE-1 && wFlags==DISPATCH_PROPERTYGET)
//	{
//		CDuiControlExtensionBase* pCtrl = GetControlById(m_curNamedItem);
//		if (pCtrl)
//		{
//			IDispatch* disp = pCtrl->GetObject(TRUE);
//			if (disp && pvarResult)
//				return V_VT(pvarResult)=VT_DISPATCH, V_DISPATCH(pvarResult)=disp, disp->AddRef(), S_OK;
//		}
//		return DISP_E_MEMBERNOTFOUND;
//	}
//	return baseDispatch::Invoke(dispidMember, riid, lcid, wFlags, pdispparams, pvarResult, pexcepinfo, puArgErr);
//}

STDMETHODIMP COleDuiWindow::GetDispID( BSTR bstrName, DWORD grfdex, DISPID *pid )
{
	HRESULT hr;
	if (SUCCEEDED(hr=baseDispatch::GetDispID(bstrName, grfdex, pid)))
		return hr;

	hr = m_peer.PeerGetDispId(bstrName, pid);
	//if (SUCCEEDED(hr)) return hr;

	//if (hr==DISP_E_UNKNOWNNAME || hr==DISP_E_MEMBERNOTFOUND)
	//{
	//	int i = m_namedItems.Find(bstrName);
	//	if (i>=0)
	//	{
	//		*pid = DISPID_EXPANDO_BASE - i -1;
	//		return S_OK;
	//	}
	//	else if (GetControlById(bstrName) && m_namedItems.Add(bstrName))
	//	{
	//		*pid = DISPID_EXPANDO_BASE - m_namedItems.GetSize();
	//		return S_OK;
	//	}
	//}
	return hr;
}

STDMETHODIMP COleDuiWindow::InvokeEx( DISPID id, LCID lcid, WORD wFlags, DISPPARAMS *pdp, VARIANT *pvarRes, EXCEPINFO *pei, IServiceProvider *pspCaller )
{
	//if (id<DISPID_EXPANDO_BASE && id>=(DISPID_EXPANDO_BASE-m_namedItems.GetSize()) && wFlags==DISPATCH_PROPERTYGET)
	//{
	//	CDuiControlExtensionBase* pCtrl = GetControlById(m_namedItems[DISPID_EXPANDO_BASE-1-id]);
	//	if (pCtrl)
	//	{
	//		IDispatch* disp = rt(pCtrl)->GetObject(TRUE);
	//		if (disp && pvarRes)
	//			return V_VT(pvarRes)=VT_DISPATCH, V_DISPATCH(pvarRes)=disp, disp->AddRef(), S_OK;
	//	}
	//	return DISP_E_MEMBERNOTFOUND;
	//}

	return m_peer.IsPeerDispId(id) ?
		m_peer.PeerInvoke(id, lcid, wFlags, pdp, pvarRes, pei, NULL) :
		baseDispatch::InvokeEx(id, lcid, wFlags, pdp, pvarRes, pei, pspCaller);
}

STDMETHODIMP COleDuiWindow::alert( BSTR Content, BSTR Caption )
{
	::MessageBoxW(m_pLayoutMgr->GetPaintWindow(), Content, Caption, MB_OK|MB_ICONINFORMATION);

	return S_OK;
}
STDMETHODIMP COleDuiWindow::close(void)
{
	::PostMessage(m_pLayoutMgr->GetPaintWindow(), WM_CLOSE, 0, 0);
	return S_OK;
}

STDMETHODIMP COleDuiWindow::createElement(BSTR eTag, IDispatch** newElem)
{
	// TODO: 在此添加实现代码
	if (newElem==NULL) return E_POINTER;
	*newElem = NULL;

	DuiNode* pCtrl = m_pLayoutMgr->CreateControl(TempParent, eTag);
	if (pCtrl)
	{
		*newElem = (pCtrl)->GetObject(TRUE);
		if (*newElem) (*newElem)->AddRef();
	}

	return S_OK;
}

STDMETHODIMP COleDuiWindow::setInterval(VARIANT callback, LONG msec, LONG* timerId)
{
	*timerId = (long)SetMtTimer(-1, msec, 0, TimerProc, this);
	m_timers.Add(*timerId, callback);
	return S_OK;
}

STDMETHODIMP COleDuiWindow::setTimeout(VARIANT callback, LONG msec, LONG* timerId)
{
	*timerId = (long)SetMtTimer(-1, msec, 1, TimerProc, this);
	m_timers.Add(*timerId, callback);
	m_timerOnce.Add(*timerId);
	return S_OK;
}

STDMETHODIMP COleDuiWindow::clearInterval(LONG timerId)
{
	KillMtTimer((DWORD)timerId);
	m_timers.Remove(timerId);
	return S_OK;
}

STDMETHODIMP COleDuiWindow::clearTimeout(LONG timerId)
{
	KillMtTimer((DWORD)timerId);
	m_timers.Remove(timerId);
	m_timerOnce.Remove(timerId);
	return S_OK;
}

void CALLBACK COleDuiWindow::TimerProc( LPVOID pData, DWORD dwId )
{
	COleDuiWindow* pThis = (COleDuiWindow*)pData;
	long id = (long)dwId;
	int idx = pThis->m_timers.FindKey(id);
	if (idx>=0)
	{
		CComVariant& v = pThis->m_timers.GetValueAt(idx);
#ifndef NO3D
		d3d::SetActiveScriptWindow(pThis->m_hWnd);
#endif
		if (v.vt==VT_BSTR)
			pThis->m_pLayoutMgr->m_script.Execute(v.bstrVal);
		else if (v.vt==VT_DISPATCH && v.pdispVal)
		{
			DISPPARAMS dp = {NULL,NULL,0,0};
			CComVariant ret;
			v.pdispVal->Invoke((DISPID)DISPID_VALUE, IID_NULL, LOCALE_CUSTOM_DEFAULT, DISPATCH_METHOD, &dp, &ret, NULL, NULL);
		}
#ifndef NO3D
		d3d::SetActiveScriptWindow(NULL);
#endif
	}

	if (pThis->m_timerOnce.Find(id)>0) // setTimeout 只需要执行一次，所以在这里删除
		pThis->clearTimeout(id);
}

STDMETHODIMP COleDuiWindow::createEvent( BSTR eventName, IDispatch** pVal )
{
	if (eventName==NULL) return E_INVALIDARG;
	if (pVal==NULL) return E_POINTER;
	*pVal = NULL;

	CComObject<COleDuiEvent>* evt = NULL;
	evt->CreateInstance(&evt);
	if (evt==NULL) return E_OUTOFMEMORY;
	evt->Init(eventName);
	return evt->QueryInterface(pVal);
}

STDMETHODIMP COleDuiWindow::fireEvent( IDuiControl* ctrl, IDuiEvent* eventObj )
{
	if (ctrl==NULL || eventObj==NULL) return E_INVALIDARG;

	CComObject<COleDuiControl>* pCtrl = (CComObject<COleDuiControl>*)ctrl;
	CComObject<COleDuiEvent>* pEvent = (CComObject<COleDuiEvent>*)eventObj;
	pEvent->m_innerEvent.pSender = DuiHandle<DuiNode>(pCtrl->GetControl());

	m_pLayoutMgr->FireEvent(NULL, pEvent->m_innerEvent);
	return S_OK;
}

//////////////////////////////////////////////////////////////////////////
bool CWinExt::$( IDispatch* callback )
{
	if (callback==NULL) return false;
	return !!m_onReadys.Add(callback);
}

bool CWinExt::$$( IDispatch* callback )
{
	if (callback==NULL) return false;
	return !!m_onEnds.Add(callback);
}

BOOL CWinExt::__DispidOfName( LPCOLESTR szName, DISPID* pDispid )
{
	if (lstrcmpiW(szName, L"trace") == 0)
		return *pDispid=1234, TRUE;
	return FALSE;
}

HRESULT CWinExt::__Invoke( DISPID dispid, WORD wFlags, DISPPARAMS* pdispparams, VARIANT *pVarResult )
{
	if (dispid==1234 && (wFlags&DISPATCH_METHOD))
	{
		if (pdispparams->cArgs>0)
		{
			OutputDebugStringW(L"[TRACE]");
			for (int i=(int)pdispparams->cArgs-1; i>=0; i--)
			{
				CComVariant v = pdispparams->rgvarg[i];
				if (S_OK == v.ChangeType(VT_BSTR))
					OutputDebugStringW(V_BSTR(&v));
				if (i>0)
					OutputDebugStringW(L",");
			}
			OutputDebugStringW(L"\n");
		}
		return S_OK;
	}

	return DISP_E_MEMBERNOTFOUND;
}