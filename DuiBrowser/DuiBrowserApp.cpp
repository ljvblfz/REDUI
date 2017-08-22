// DuiBrowserApp.cpp : CDuiBrowserApp 的实现

#include "stdafx.h"
#include "DuiBrowserApp.h"
#include "atlstr.h"

extern CScript gScript;

// CDuiBrowserApp

ULONG CDuiBrowserApp::m_ulInstances = 0;
CPeer CDuiBrowserApp::m_peer(NULL);
IUnknown* CDuiBrowserApp::m_peerUnk = &CDuiBrowserApp::m_peer;


STDMETHODIMP CDuiBrowserApp::GetIDsOfNames( REFIID riid, LPOLESTR* rgszNames, UINT cNames, LCID lcid, DISPID* rgdispid )
{
	HRESULT hr;
	if (SUCCEEDED(hr=baseDispatch::GetIDsOfNames(riid, rgszNames, cNames, lcid, rgdispid)))
		return hr;

	return hr = m_peer.PeerGetDispId(*rgszNames, rgdispid);
}

STDMETHODIMP CDuiBrowserApp::Invoke( DISPID dispidMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS* pdispparams, VARIANT* pvarResult, EXCEPINFO* pexcepinfo, UINT* puArgErr )
{
	return m_peer.IsPeerDispId(dispidMember) ?
			m_peer.PeerInvoke(dispidMember, lcid, wFlags, pdispparams, pvarResult, pexcepinfo, puArgErr) :
			baseDispatch::Invoke(dispidMember, riid, lcid, wFlags, pdispparams, pvarResult, pexcepinfo, puArgErr);
}

STDMETHODIMP CDuiBrowserApp::load(BSTR xml)
{
	//UpdateInfo();
	// TODO: 在此添加实现代码
	if (gpDlg == NULL) return E_FAIL;

	gpDlg->m_wndShow.m_bstrXml = xml;
	gpDlg->DoDataExchange();
	gpDlg->m_wndShow.Reset();
	gpDlg->m_wndShow.UpdateXml();

	return S_OK;
}

STDMETHODIMP CDuiBrowserApp::loadUrl(BSTR url)
{
	//UpdateInfo();
	// TODO: 在此添加实现代码
	if (gpDlg == NULL) return E_FAIL;

	gpDlg->LoadUrl(url);
	gpDlg->m_wndShow.Reset();
	gpDlg->m_wndShow.UpdateXml();

	return S_OK;
}

STDMETHODIMP CDuiBrowserApp::activate(VARIANT* vUrl)
{
	//UpdateInfo();
	// TODO: 在此添加实现代码
	if (gpDlg == NULL) return E_FAIL;

	if (vUrl && V_VT(vUrl)==VT_BSTR)
	{
		gpDlg->LoadUrl(V_BSTR(vUrl));
		gpDlg->m_wndShow.Reset();
		gpDlg->m_wndShow.UpdateXml();
	}

	if (gpDlg->m_wndShow.IsWindow())
		::SetForegroundWindow(gpDlg->m_wndShow.m_hWnd);

	return S_OK;
}

//STDMETHODIMP CDuiBrowserApp::get_version(BSTR* pVal)
//{
//	//UpdateInfo();
//	// TODO: 在此添加实现代码
//	if (pVal==NULL) return E_POINTER;
//	*pVal = NULL;
//
//	VS_FIXEDFILEINFO ffvi;
//	GetFixedFileVersion((HINSTANCE)NULL, &ffvi);
//	CStringW str;
//	str.Format(L"%d.%d.%d.%d",
//		HIWORD(ffvi.dwFileVersionMS),
//		LOWORD(ffvi.dwFileVersionMS),
//		HIWORD(ffvi.dwFileVersionLS),
//		LOWORD(ffvi.dwFileVersionLS));
//	CComBSTR bstr = str;
//	*pVal = bstr.Detach();
//
//	return S_OK;
//}

STDMETHODIMP CDuiBrowserApp::get_visible(VARIANT_BOOL* pVal)
{
	//UpdateInfo();
	// TODO: 在此添加实现代码
	if (pVal==NULL) return E_POINTER;
	*pVal = (gpDlg ? VARIANT_TRUE : VARIANT_FALSE);

	return S_OK;
}


void CDuiBrowserApp::UpdateInfo()
{
	if (gpDlg)
	{
		CStringW str;
		str.Format(L"CONNECTIONS: %d", /*m_dwRef*/m_ulInstances);
		gpDlg->m_wndOutput.SetWindowText(str);
	}
}
//
//STDMETHODIMP CDuiBrowserApp::axo(BSTR progid, IDispatch** obj)
//{
//	// TODO: 在此添加实现代码
//	if (!gScript.IsValid()) return E_FAIL;
//	if (obj==NULL) return E_POINTER;
//	*obj = NULL;
//
//	CComDispatchDriver disp, func;
//	CComVariant vFunc;
//	HRESULT hr = gScript.GetScriptDispatch(NULL, &disp);
//	if (disp.p)
//		hr = disp.GetPropertyByName(L"new_axo", &vFunc);
//
//	if (vFunc.vt==VT_DISPATCH && vFunc.pdispVal)
//	{
//		func = vFunc.pdispVal;
//		CComVariant vp = progid, v;
//		hr = func.Invoke1((DISPID)0, &vp, &v);
//		if (v.vt==VT_DISPATCH && v.pdispVal)
//		{
//			*obj = v.pdispVal;
//			(*obj)->AddRef();
//		}
//	}
//
//	return hr;
//}
//
//STDMETHODIMP CDuiBrowserApp::httpLoad(BSTR url, VARIANT success, VARIANT* ret)
//{
//	// TODO: 在此添加实现代码
//	if (!gScript.IsValid()) return E_FAIL;
//
//	CComDispatchDriver disp, func;
//	CComVariant vFunc;
//	HRESULT hr = gScript.GetScriptDispatch(NULL, &disp);
//	if (disp.p)
//		hr = disp.GetPropertyByName(L"httpLoad", &vFunc);
//
//	if (vFunc.vt==VT_DISPATCH && vFunc.pdispVal)
//	{
//		func = vFunc.pdispVal;
//		CComVariant vUrl = url;
//		return func.Invoke2((DISPID)0, &vUrl, &success, ret);
//	}
//
//	return S_OK;
//}
