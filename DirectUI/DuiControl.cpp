// DuiControl.cpp : COleDuiControl 的实现

#include "stdafx.h"
#include <DispEx.h>
#include "DuiControl.h"
#include "duicore/DuiPluginsManager.h"

#ifndef DISPID_EXPANDO_BASE
#define DISPID_EXPANDO_BASE             3000000
#define DISPID_EXPANDO_MAX              3999999
#define IsExpandoDispid(dispid)         (DISPID_EXPANDO_BASE <= dispid && dispid <= DISPID_EXPANDO_MAX)
#endif // DISPID_EXPANDO_BASE

#define CheckOwner() if (m_pCtrl==NULL) return E_UNEXPECTED

// COleDuiControl


void CALLBACK COleDuiControl::OnControlEvent( HDE pCtrl, DuiEvent& event )
{
	COleDuiControl* pThis = FromControl(DuiHandle<DuiNode>(pCtrl));
	ATLASSERT(pThis);
	if (pThis) pThis->OnControlEvent(event);
}

void COleDuiControl::OnControlEvent(DuiEvent& event)
{
	if (m_pCtrl==NULL) return;

	DISPPARAMS* pdp = NULL;
	UINT cArgs = 0;
	DUI_EVENTTYPE et = DuiNode::NameToEventType(event.Name);
	// 用户事件
	if (HIWORD(event.Name) && et==DUIET__INVALID)
	{
		pdp = (DISPPARAMS*)event.wParam;
		if (pdp)
			cArgs = pdp->cArgs;
	}

	// 构造参数
	CComVariant* params = NEW CComVariant[cArgs+1];
	params[0] = (IDispatch*)this;
	for (UINT i=0; i<cArgs; i++)
	{
		ATLASSERT(pdp);
		params[i+1] = pdp->rgvarg[i];
	}
	DISPID didThis = DISPID_THIS;
	DISPPARAMS dp = {params, &didThis, cArgs+1, 1};
	CComVariant vRet;

	// 首先调用用户事件
	DuiNode* r = (m_pCtrl);
#ifndef NO3D
	d3d::SetActiveScriptWindow(r->m_pLayoutMgr->GetPaintWindow());
#endif // NO3D
	HRESULT hr = S_OK;
	CComBSTR name = DuiNode::NameToFinalName(event.Name);
	if (name && r->m_eventHandler.p) // TODO: 要把TYPE转换成NAME，可能会有多个NAME
	{
		CComQIPtr<IDispatchEx> dispex = r->GetEventHandler();
		if (dispex.p)
		{
			DISPID did = DISPID_UNKNOWN;
			hr = dispex->GetDispID(name, fdexNameCaseInsensitive, &did);
			if (SUCCEEDED(hr))
			{
				//CComVariant vHandler;
				//DISPPARAMS dp2 = {NULL, NULL, 0, 0};
				//hr = dispex->InvokeEx(did, LOCALE_SYSTEM_DEFAULT, DISPATCH_PROPERTYGET, &dp2, &vHandler, NULL, NULL);
				//if (SUCCEEDED(hr) && vHandler.vt==VT_DISPATCH && vHandler.pdispVal)
				//	hr = vHandler.pdispVal->Invoke(0, IID_NULL, LOCALE_SYSTEM_DEFAULT, DISPATCH_METHOD, &dp, &vRet, NULL, NULL);
				hr = dispex->InvokeEx(did, LOCALE_SYSTEM_DEFAULT, DISPATCH_METHOD, &dp, &vRet, NULL, NULL);
			}
		}
	}
	vRet.Clear();

	if (!event.returnValue && et>DUIET__FIRST && et<DUIET__LAST && m_events[et].p)
	{
		//VARIANTARG v;
		//v.vt = VT_DISPATCH;
		//v.pdispVal = (IDispatch*)this;
		//DISPID didThis = DISPID_THIS;
		//DISPPARAMS dp = {&v, &didThis, 1, 1};
		//CComVariant vRet;
		hr = m_events[et].p->Invoke(0, IID_NULL, LOCALE_SYSTEM_DEFAULT, DISPATCH_METHOD, &dp, &vRet, NULL, NULL);
		//CComQIPtr<IDispatchEx> dispex = m_events[event.Name].p;
		//HRESULT hr = dispex->InvokeEx(0, LOCALE_SYSTEM_DEFAULT, DISPATCH_METHOD, &dp, &vRet, NULL, NULL);
		ATLASSERT(TRUE);
	}
#ifndef NO3D
	d3d::SetActiveScriptWindow(NULL);
#endif
	delete[] params;

	// clear
	if (et==DUIET_unload && event.pSender==DuiHandle<DuiNode>(m_pCtrl)) m_pCtrl = NULL;
}

COleDuiControl* COleDuiControl::FromControl( DuiNode* pCtrl, BOOL bAutoCreate/*=FALSE*/ )
{
	if (pCtrl == NULL) return NULL;
	IDispatch* disp = (pCtrl)->GetObject(bAutoCreate);
	if (disp == NULL) return NULL;
	return (CComObject<COleDuiControl>*)disp;
}

BOOL COleDuiControl::SetEventCallback( LPCOLESTR lpszEventName, IDispatch* pCallback )
{
	int et = DuiNode::NameToEventType(lpszEventName);
	if (et == DUIET__INVALID) return FALSE;
	m_events[et] = pCallback;
	if (et==DUIET_dblclick)
		(m_pCtrl)->m_dblclick_event = (pCallback ? TRUE : FALSE);
	return TRUE;
}

HRESULT COleDuiControl::GetEventCallback( LPCOLESTR lpszEventName, IDispatch** ppCallback )
{
	if (ppCallback==NULL) return E_POINTER;
	*ppCallback = NULL;

	int et = DuiNode::NameToEventType(lpszEventName);
	if (et == DUIET__INVALID) return E_INVALIDARG;
	return m_events[et].QueryInterface(ppCallback);
}

STDMETHODIMP COleDuiControl::get_tagName(BSTR* pVal)
{
	CheckOwner();

	// TODO: 在此添加实现代码
	if (pVal == NULL) return E_POINTER;
	*pVal = NULL;
	//CComVariant v = rt(m_pCtrl)->GetAttributeString(L"tagName");
	CComVariant v = m_pCtrl->m_tagName;
	return v.CopyTo(pVal);
// 	if (v) *pVal = ::SysAllocString(v);
// 
// 	return S_OK;
}

STDMETHODIMP COleDuiControl::get_ID(BSTR* pVal)
{
	CheckOwner();

	// TODO: 在此添加实现代码
	if (pVal == NULL) return E_POINTER;
	*pVal = NULL;
	LPCOLESTR v = (m_pCtrl)->GetID();
	if (v) *pVal = ::SysAllocString(v);

	return S_OK;
}

STDMETHODIMP COleDuiControl::eventHandler(VARIANT nameOrObject, VARIANT callback, IDispatch** handler)
{
	CheckOwner();

	if (handler) *handler=NULL;

	HRESULT hr = S_OK;
	CComBSTR name;
	if (nameOrObject.vt==VT_DISPATCH && nameOrObject.pdispVal)
	{
		// 替换现有的事件处理器
		m_pCtrl->m_eventHandler = nameOrObject.pdispVal;
		if (handler) *handler = m_pCtrl->GetEventHandler(FALSE, TRUE);
		// 重新设置别名事件到核心名称
		CComQIPtr<IDispatchEx> dispex = *handler;
		if (dispex.p)
		{
			CSimpleMap<DUI_EVENTTYPE, IDispatch*> maps;

			DISPID did;
			DISPPARAMS dp = {NULL,NULL,0,0};
			hr = dispex->GetNextDispID(fdexEnumAll, DISPID_STARTENUM, &did);
			while (hr == S_OK)
			{
				CComBSTR n;
				hr = dispex->GetMemberName(did, &n);
				CComVariant v;
				hr = dispex->InvokeEx(did, LOCALE_SYSTEM_DEFAULT, DISPATCH_PROPERTYGET, &dp, &v, NULL, NULL);
				DUI_EVENTTYPE et = DuiNode::NameToEventType(n);
				if (et!=DUIET__INVALID && v.vt==VT_DISPATCH)
				{
					if (!maps.SetAt(et, v.pdispVal))
						maps.Add(et, v.pdispVal);
				}

				hr = dispex->GetNextDispID(fdexEnumAll, did, &did);
			}

			for (int i=0; i<maps.GetSize(); i++)
			{
				DUI_EVENTTYPE& et = maps.GetKeyAt(i);
				IDispatch* disp = maps.GetValueAt(i);
				hr = dispex->GetDispID(CComBSTR(DuiNode::NameToFinalName((LPCOLESTR)et)), fdexNameEnsure | fdexNameCaseInsensitive, &did);
				if (SUCCEEDED(hr))
				{
					CComVariant vOld;
					hr = dispex->InvokeEx(did, LOCALE_SYSTEM_DEFAULT, DISPATCH_PROPERTYGET, &dp, &vOld, NULL, NULL);
					if (vOld.vt!=VT_DISPATCH || vOld.pdispVal!=disp)
					{
						CComVariant param = disp;
						DISPID didd = DISPID_PROPERTYPUT;
						DISPPARAMS dp2 = {&param, &didd, 1, 1};
						hr = dispex->InvokeEx(did, LOCALE_SYSTEM_DEFAULT, DISPATCH_PROPERTYPUTREF, &dp2,NULL, NULL, NULL);
					}
				}
			}
		}
	}
	else if (nameOrObject.vt==VT_BSTR)
	{
		if (callback.vt==VT_NULL)
		{
			callback.vt = VT_DISPATCH;
			callback.pdispVal = NULL;
		}

		name = DuiNode::NameToFinalName(nameOrObject.bstrVal);
		if (name)
		{
			CComQIPtr<IDispatchEx> dispex = m_pCtrl->GetEventHandler(TRUE);
			ATLASSERT(dispex.p);
			DISPID did = DISPID_UNKNOWN;
			hr = dispex->GetDispID(name, fdexNameEnsure | fdexNameCaseInsensitive, &did);
			if (SUCCEEDED(hr))
			{
				if (callback.vt==VT_DISPATCH)
				{
					DISPID didd = DISPID_PROPERTYPUT;
					DISPPARAMS dp2 = {&callback, &didd, 1, 1};
					//CComVariant vTmp;
					hr = dispex->InvokeEx(did, LOCALE_SYSTEM_DEFAULT, DISPATCH_PROPERTYPUTREF, &dp2, NULL, NULL, NULL);
					if (SUCCEEDED(hr) && handler && callback.pdispVal)
						hr = callback.pdispVal->QueryInterface(IID_IDispatch, (void**)handler);
				}
				else // get
				{
					DISPPARAMS dp2 = {NULL, NULL, 0, 0};
					CComVariant v;
					hr = dispex->InvokeEx(did, LOCALE_SYSTEM_DEFAULT, DISPATCH_PROPERTYGET, &dp2, &v, NULL, NULL);
					if (SUCCEEDED(hr) && v.vt==VT_DISPATCH && v.pdispVal && handler)
						hr = v.pdispVal->QueryInterface(IID_IDispatch, (void**)handler);
				}
			}
		}
	}
	else
		hr = E_INVALIDARG;
	
	return hr;
}

STDMETHODIMP COleDuiControl::GetIDsOfNames( REFIID riid, LPOLESTR* rgszNames, UINT cNames, LCID lcid, DISPID* rgdispid )
{
	HRESULT hr = DISP_E_UNKNOWNNAME;
	//if (DuiNode::NameToEventType(*rgszNames) == DUIET__INVALID)
		hr = m_peer.PeerGetDispId(*rgszNames, rgdispid);
	if (FAILED(hr))
		hr = __super::GetIDsOfNames(riid, rgszNames, cNames, lcid, rgdispid);
		//hr = m_peer.PeerGetDispId(*rgszNames, rgdispid);
	//if (FAILED(hr) && m_pCtrl && cNames==1) // 搜索内部属性
	//{
	//	LONG idx = m_pCtrl->GetAttributePos(*rgszNames);
	//	if (idx >= 0)
	//	{
	//		*rgdispid = DISPID_EXPANDO_BASE + idx;
	//		hr = S_OK;
	//	}
	//}
	return SUCCEEDED(hr) ? S_OK : DISP_E_UNKNOWNNAME;
}

STDMETHODIMP COleDuiControl::Invoke( DISPID dispidMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS* pdispparams, VARIANT* pvarResult, EXCEPINFO* pexcepinfo, UINT* puArgErr )
{
	//if (IsExpandoDispid(dispidMember))
	//{
	//	if (wFlags&DISPATCH_PROPERTYGET)
	//	{
	//		LPCOLESTR sz = m_pCtrl->GetAttributeByPos(dispidMember-DISPID_EXPANDO_BASE);
	//		if (sz)
	//		{
	//			if (pvarResult)
	//			{
	//				V_VT(pvarResult) = VT_BSTR;
	//				V_BSTR(pvarResult) = ::SysAllocString(sz);
	//			}
	//			return S_OK;
	//		}
	//		return DISP_E_MEMBERNOTFOUND;
	//	}
	//	else if (wFlags == DISPATCH_PROPERTYPUT)
	//	{
	//		ATLASSERT(pdispparams->cArgs == 1);
	//		CComVariant v;
	//		if (SUCCEEDED(v.ChangeType(VT_BSTR, pdispparams->rgvarg)))
	//		{
	//			m_pCtrl->SetAttributeByPos(dispidMember-DISPID_EXPANDO_BASE, V_BSTR(&v));
	//			return S_OK;
	//		}
	//		return E_INVALIDARG;
	//	}
	//	return E_INVALIDARG;
	//}

	//HRESULT hr = __super::Invoke(dispidMember, riid, lcid, wFlags, pdispparams, pvarResult, pexcepinfo, puArgErr);
	//return hr;
	return m_peer.IsPeerDispId(dispidMember) ?
		m_peer.PeerInvoke(dispidMember, lcid, wFlags, pdispparams, pvarResult, pexcepinfo, puArgErr) :
		__super::Invoke(dispidMember, riid, lcid, wFlags, pdispparams, pvarResult, pexcepinfo, puArgErr);
}

STDMETHODIMP COleDuiControl::set(BSTR name, VARIANT value)
{
	CheckOwner();

	// TODO: 在此添加实现代码
	CComVariant v = value;
	m_pCtrl->SetAttribute(name, v);

	return S_OK;
}

STDMETHODIMP COleDuiControl::get(BSTR name, VARIANT* val)
{
	CheckOwner();

	// TODO: 在此添加实现代码
	CComVariant v = m_pCtrl->GetAttribute(name);
	return v.Detach(val);
}

STDMETHODIMP COleDuiControl::remove(BSTR name)
{
	CheckOwner();

	(m_pCtrl)->RemoveAttribute(name);
	return S_OK;
}

STDMETHODIMP COleDuiControl::setStyle(BSTR stylename, BSTR stylevalue)
{
	CheckOwner();

	// TODO: 在此添加实现代码
	if (m_pCtrl->TryParseAsStyle(stylename, stylevalue))
		return S_OK;
	return S_FALSE;
}

STDMETHODIMP COleDuiControl::get_parent(IDispatch** pVal)
{
	CheckOwner();

	// TODO: 在此添加实现代码
	if (pVal == NULL) return E_POINTER;
	*pVal = NULL;
	DuiNode* parent = (m_pCtrl)->GetParent();
	if (parent)
	{
		*pVal = (parent)->GetObject(TRUE);
		if (*pVal) (*pVal)->AddRef();
	}

	return S_OK;
}

STDMETHODIMP COleDuiControl::get_text(BSTR* pVal)
{
	CheckOwner();

	return (*pVal = ::SysAllocString((m_pCtrl)->GetText())), S_OK;
}

STDMETHODIMP COleDuiControl::put_text(BSTR newVal)
{
	CheckOwner();

	return (m_pCtrl)->SetText(newVal), S_OK;
}

STDMETHODIMP COleDuiControl::focus(void)
{
	CheckOwner();

	return (m_pCtrl)->SetFocus(), S_OK;
}

STDMETHODIMP COleDuiControl::activate(void)
{
	CheckOwner();

	return m_pCtrl->Activate() ? S_OK : S_FALSE;
}

STDMETHODIMP COleDuiControl::get_visible(VARIANT_BOOL* pVal)
{
	CheckOwner();

	return (*pVal = ((m_pCtrl)->IsVisible() ? VARIANT_TRUE : VARIANT_FALSE)), S_OK;
}

STDMETHODIMP COleDuiControl::put_visible(VARIANT_BOOL newVal)
{
	CheckOwner();

	return (m_pCtrl)->SetVisible(newVal!=VARIANT_FALSE), S_OK;
}

STDMETHODIMP COleDuiControl::get_enabled(VARIANT_BOOL* pVal)
{
	CheckOwner();

	return (*pVal = ((m_pCtrl)->IsEnabled() ? VARIANT_TRUE : VARIANT_FALSE)), S_OK;
}

STDMETHODIMP COleDuiControl::put_enabled(VARIANT_BOOL newVal)
{
	CheckOwner();

	return (m_pCtrl)->SetEnabled(newVal!=VARIANT_FALSE), S_OK;
}

STDMETHODIMP COleDuiControl::get_childCount(LONG* pVal)
{
	CheckOwner();

	return (*pVal = (m_pCtrl)->GetChildCount()), S_OK;
}

STDMETHODIMP COleDuiControl::getChild(VARIANT index, IDispatch** child)
{
	CheckOwner();

	// TODO: 在此添加实现代码
	if (child == NULL) return E_POINTER;
	*child = NULL;

	if ((m_pCtrl->GetControlFlags()&DUIFLAG_NOCHILDREN) == 0)
	{
		CComVariant v;
		if (SUCCEEDED(v.ChangeType(VT_I4, &index))) // by index
		{
			if (v.lVal>=0 && v.lVal<(m_pCtrl)->GetChildCount())
			{
				*child = ((m_pCtrl)->GetChildControl(v.lVal))->GetObject(TRUE);
				if (*child) (*child)->AddRef();
				return S_OK;
			}
			// not found, changed to BSTR
			v.ChangeType(VT_BSTR);
		}

		if (v.vt == VT_BSTR) // by ID
		{
			// 首先精确匹配ID
			for (int i=0; i<(m_pCtrl)->GetChildCount(); i++)
			{
				if (lstrcmpW(((m_pCtrl)->GetChildControl(i))->GetID(), v.bstrVal) == 0)
				{
					*child = ((m_pCtrl)->GetChildControl(i))->GetObject(TRUE);
					if (*child) (*child)->AddRef();
					return S_OK;
				}
			}
			// 大小写不敏感的匹配
			for (int i=0; i<(m_pCtrl)->GetChildCount(); i++)
			{
				if (lstrcmpiW(((m_pCtrl)->GetChildControl(i))->GetID(), v.bstrVal) == 0)
				{
					*child = ((m_pCtrl)->GetChildControl(i))->GetObject(TRUE);
					if (*child) (*child)->AddRef();
					return S_OK;
				}
			}
		}
	}

	return S_OK;
}

STDMETHODIMP COleDuiControl::attachObject(IDispatch* extObject)
{
	CheckOwner();

	return m_peer.AttachObject(extObject);
}

STDMETHODIMP COleDuiControl::detachObject(IDispatch* extObject)
{
	CheckOwner();

	return m_peer.DetachObject(extObject);
}

STDMETHODIMP COleDuiControl::get_filter(IDispatch** pVal)
{
	CheckOwner();

	// TODO: 在此添加实现代码
	if (pVal == NULL) return E_POINTER;
	*pVal = NULL;
	if (m_pCtrl->_dxt)
	{
		*pVal = m_pCtrl->_dxt->GetDispatch();
		if (*pVal) (*pVal)->AddRef();
	}

	return S_OK;
}

STDMETHODIMP COleDuiControl::appendChild(IDispatch* newChild, IDispatch** node)
{
	CheckOwner();

	if (newChild==NULL) return E_UNEXPECTED;
	if (node==NULL) return E_POINTER;
	*node = NULL;

	// 检验子元素是否合法
	CComQIPtr<IDuiControlSign> ctl(newChild);
	if (ctl.p == NULL) return E_UNEXPECTED;
	DuiNode* pCtrl = ctl->GetControl();
	if (!pCtrl) return E_UNEXPECTED;

	if (m_pCtrl->AddChildControl(pCtrl))
	{
		*node = newChild;
		newChild->AddRef();
	}
	
	return S_OK;
}

STDMETHODIMP COleDuiControl::insertBefore(IDispatch* newChild, VARIANT refChild, IDispatch** node)
{
	CheckOwner();

	if (newChild==NULL) return E_UNEXPECTED;
	if (node==NULL) return E_POINTER;
	*node = NULL;

	// 检验子元素是否合法
	CComQIPtr<IDuiControlSign> ctl(newChild);
	if (ctl.p == NULL) return E_UNEXPECTED;
	DuiNode* pCtrl = ctl->GetControl();
	if (!pCtrl) return E_UNEXPECTED;

	// 参考元素
	DuiNode* pRef = NULL;
	CComVariant v;
	if (refChild.vt==VT_DISPATCH && refChild.pdispVal)
	{
		CComQIPtr<IDuiControlSign> refSign(refChild.pdispVal);
		if (refSign.p)
			pRef = refSign->GetControl();
	}
	else if (refChild.vt!=VT_EMPTY && refChild.vt!=VT_ERROR && SUCCEEDED(v.ChangeType(VT_I4, &refChild))) // 这是索引
		pRef = m_pCtrl->GetChildControl(v.lVal);

	if (m_pCtrl->AddChildControl(pCtrl, pRef))
	{
		*node = newChild;
		newChild->AddRef();
	}

	return S_OK;
}

STDMETHODIMP COleDuiControl::put_innerXML(BSTR newVal)
{
	CheckOwner();

	return (m_pCtrl)->SetInnerXML(newVal) ? S_OK : E_FAIL;
}

STDMETHODIMP COleDuiControl::put_outerXML(BSTR newVal)
{
	CheckOwner();

	return (m_pCtrl)->SetOuterXML(newVal) ? S_OK : E_FAIL;
}

STDMETHODIMP COleDuiControl::get_children(IDispatch** pVal)
{
	CheckOwner();
	if (pVal == NULL) return E_POINTER;
	*pVal = NULL;
	CDispatchArray* pArr = CDispatchArray::New(pVal);
	if (pArr==NULL) return E_OUTOFMEMORY;

	if ((m_pCtrl->GetControlFlags()&DUIFLAG_NOCHILDREN) == 0)
	{
		for (int i=0, num=m_pCtrl->GetChildCount(); i<num; i++)
		{
			DuiNode* child = (m_pCtrl->GetChildControl(i));
			pArr->Add(child->GetObject(TRUE), child->GetID());
		}
	}

	//*pVal = pArr->GetDispatch(true);
	return S_OK;
}
