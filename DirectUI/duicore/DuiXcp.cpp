#include "StdAfx.h"
#include "DuiXcp.h"
#include "DuiPluginsManager.h"

extern "C"
{
#define MIDL_DEFINE_GUID(type,name,l,w1,w2,b1,b2,b3,b4,b5,b6,b7,b8) \
	const type name = {l,w1,w2,{b1,b2,b3,b4,b5,b6,b7,b8}}
MIDL_DEFINE_GUID(CLSID, CLSID_XcpControl,0xDFEAF541,0xF3E1,0x4c24,0xAC,0xAC,0x99,0xC3,0x07,0x15,0x08,0x4A);
#undef MIDL_DEFINE_GUID
};

CDuiXcp::CDuiXcp() : m_bLoaded(FALSE)
{

}

CDuiXcp::~CDuiXcp()
{
	ReleaseControl();
}

void CDuiXcp::ReleaseControl()
{
	if (m_bReleaseAll) return;

	{
		CComQIPtr<IPeerHolder> peer(m_pOwner->GetObject());
		if (peer.p)
		{
			peer->SetNamedItemHost(NULL);

			if (m_bLoaded)
			{
				CComQIPtr<IXcpControl> spCtrl(m_spOleObject);
				if (spCtrl.p)
				{
					CComPtr<IDispatch> content;
					if (SUCCEEDED(spCtrl->get_Content(&content)) && content.p)
						peer->DetachObject(content.p);
				}
			}

			CComQIPtr<IDispatch> obj(m_spOleObject);
			if (obj)
				peer->DetachObject(obj);

			peer->DetachObject(GetDispatch());
		}
		// peer 应该在这里析构
	}

	ReleaseAll();
	m_bLoaded = FALSE;
}

void CDuiXcp::OnConnect()
{
	//CDuiControlExtension::Handle(hRuntime);
	Init();
}

void CDuiXcp::OnScroll( HANDLE, HDE pScroll, BOOL bVert/*=TRUE*/ )
{
	if ( m_spInPlaceObject.p )
	{
		RECT rcItem = GetPos();
		RECT rcClip = GetClip();

		//HWND hwnd = NULL;
		//if (!m_bWindowless && m_spInPlaceObject.p)
		//	m_spInPlaceObject->GetWindow(&hwnd);

		//if (hwnd && ::IsWindowVisible(hwnd) && ::IsRectEmpty(&rcClip))
		//	::ShowWindow(hwnd, SW_HIDE);

		m_spInPlaceObject->SetObjectRects(&rcItem, &rcClip);

		//if (hwnd && !::IsWindowVisible(hwnd) && !::IsRectEmpty(&rcClip))
		//	::ShowWindow(hwnd, SW_SHOW);

		//if (m_spInPlaceObject.p)
		//{
		//	if (::IsRectEmpty(&rcClip))
		//		m_spInPlaceObject->UIDeactivate();
		//}
	}
}

STDMETHODIMP CDuiXcp::NotifyLoaded (void)
{
	ATLASSERT(m_bLoaded == FALSE);
	m_bLoaded = TRUE;

	CComQIPtr<IPeerHolder> peer(m_pOwner->GetObject(TRUE));
	if (peer.p)
	{
		CComQIPtr<IXcpControl> spCtrl(m_spOleObject);
		if (spCtrl.p)
		{
			CComPtr<IDispatch> content;
			if (SUCCEEDED(spCtrl->get_Content(&content)) && content.p)
				peer->AttachObject(content.p);
		}
	}

	// 现在激发load事件
	DuiEvent event = { 0 };
	event.Name = event_type_to_name(DUIET_load);
	event.pSender = DuiHandle<DuiNode>(m_pOwner);
	event.dwTimestamp = ::GetTickCount();
	event.cancelBubble = TRUE;
	m_pOwner->m_pLayoutMgr->FireEvent(m_pOwner, event);

	return S_OK;
}

BOOL CDuiXcp::IsMyEvent(HANDLE, DuiEvent* pEvent)
{
	DuiEvent& event = *pEvent;
	if (HIWORD(event.Name)) return FALSE;
	DUI_EVENTTYPE e = DuiNode::NameToEventType(event.Name);

	HRESULT hr = E_FAIL;
	if (e == (DUIET_load))
	{
		// onload事件与控件的事件同名冲突，如果脚本中设置，则使用控件加载文档完成的事件，不使用默认事件
		// 如果m_bLoaded为真，表示XAML文件已经加载完毕，控件的onload脚本应该得到调用，这里应该直接跳过
		if (!m_bLoaded)
		{
			hr = CreateControl(CLSID_XcpControl);
			if (SUCCEEDED(hr))
			{
				// now attach ActiveX object to control peer.
				CComQIPtr<IPeerHolder2> peer(m_pOwner->GetObject(TRUE));
				if (peer.p)
				{
					peer->AttachObject2(GetDispatch(), __ClassName());

					CComQIPtr<IDispatch> obj(m_spOleObject);
					if (obj)
						peer->AttachObject(obj);

					CComQIPtr<IXcpControl> spCtrl(m_spOleObject);
					if (spCtrl.p)
					{
						CComPtr<IDispatch> content;
						hr = spCtrl->get_Content(&content);
						if (SUCCEEDED(hr) && content.p)
							peer->AttachObject(content.p);
					}

					peer->SetNamedItemHost(this);
				}
			}
			event.returnValue = TRUE; // 阻止后续调用，包括脚本和冒泡
			return TRUE;
		}
	}
	else if (e == (DUIET_unload))
	{
		ReleaseControl();
	}
	else
	{
		// 激活不能放在load事件里做，因为此时正在解析XML文件
		if (m_spOleObject.p && !m_bInPlaceActive)
			ActivateAx();

		if (e == (DUIET_setfocus))
		{
			m_bHaveFocus = TRUE;
			if (!m_bReleaseAll)
			{
				RECT rc = GetPos();
				if (m_spOleObject.p && !m_bUIActive)
					hr = m_spOleObject->DoVerb(OLEIVERB_UIACTIVATE, NULL, this, 0, m_pLayoutMgr->GetPaintWindow(), &rc);

				if (!m_bWindowless && m_spInPlaceObject.p)
				{
					HWND hwnd = NULL;
					hr = m_spInPlaceObject->GetWindow(&hwnd);
					if (::IsWindow(hwnd))
						::SetFocus(hwnd);
				}
			}
		}
		else if (e == (DUIET_killfocus))
		{
			m_bHaveFocus = FALSE;
			if (m_spInPlaceObject.p)
				hr = m_spInPlaceObject->UIDeactivate();

			if (!m_bWindowless && m_spInPlaceObject.p)
			{
				HWND hwnd = NULL;
				hr = m_spInPlaceObject->GetWindow(&hwnd);
				if (::IsWindow(hwnd))
					::SetFocus(m_pLayoutMgr->GetPaintWindow());
			}
		}
		else if (e == (DUIET_size))
		{
			//if ( m_spOleObject.p == NULL ) return TRUE;
			ResetSurface();

			CRect brc = GetPos();

			SIZEL hmSize = { 0 };
			SIZEL pxSize = { 0 };
			pxSize.cx = brc.Width(); // m_rcItem.right - m_rcItem.left;
			pxSize.cy = brc.Height(); // m_rcItem.bottom - m_rcItem.top;
			AtlPixelToHiMetric(&pxSize, &hmSize);
			if (m_spOleObject.p)
				hr = m_spOleObject->SetExtent(DVASPECT_CONTENT, &hmSize);

			if ( m_spInPlaceObject.p )
			{
				CRect rcItem = brc;
				RECT rcClip = GetClip();
				hr = m_spInPlaceObject->SetObjectRects(&rcItem, &rcClip);
			}
		}
	}
	return FALSE;
}

void CDuiXcp::OnPaintContent( HDE hde, HDC hdc, RECT rcPaint, BOOL bDraw, InDrawData* pidd )
{
	InDrawData& idd = *pidd;
	// try activate control
	if (bDraw && m_spOleObject.p && !m_bInPlaceActive)
		ActivateAx();

	if ( m_spViewObject.p && m_bWindowless && bDraw)
	{
		RECT rc = GetPos();
		RECT rcClip = GetClip(FALSE);
		if (!::IsRectEmpty(&rcClip))
		{
#ifndef NO3D
			if (m_pOwner->m_pSurface)
			{
				HRESULT hr = m_spViewObject->Draw(m_dwAspect, -1, NULL, NULL, NULL, hdc, (RECTL*) &rc, (RECTL*) &rcClip, NULL, NULL); 
			}
			else
#endif
			{
				CRenderContentOrg org(hdc, NULL, m_pOwner);
				//POINT pt1;
				//::SetWindowOrgEx(hdc, 0, 0, &pt1);
				//CRenderClip clip(hdc, rcClip);
				HDC hdc2 = _memdc; //m_pSurface->GetDC();
				POINT ptOld;
				::SetWindowOrgEx(hdc2, rc.left, rc.top, &ptOld);
				HRESULT hr = m_spViewObject->Draw(m_dwAspect, -1, NULL, NULL, NULL, hdc2, (RECTL*) &rc, (RECTL*) &rcClip, NULL, NULL); 
				::BitBlt(hdc, rcClip.left, rcClip.top, rcClip.right-rcClip.left, rcClip.bottom-rcClip.top, hdc2, rcClip.left, rcClip.top, SRCCOPY);
				::SetWindowOrgEx(hdc2, ptOld.x, ptOld.y, NULL);
				//::SetWindowOrgEx(hdc, pt1.x, pt1.y, NULL);
				//m_pSurface->ReleaseDC(hdc2);
				//m_pSurface->BltToPrimary(&rcClip);
			}
		}

		m_pOwner->UpdateDrawData(idd);
	}
	else if (!bDraw)
	{
		if (!::IsRectEmpty(&__rcNeeded))
		{
			// 如果控件自身在改变位置，这里提供估算值
			m_pOwner->UpdateDrawData(idd);
			if (idd.pt.x > idd.rc.left + idd.lLineIndent && (idd.rc.right-idd.pt.x < __rcNeeded.right - __rcNeeded.left))
			{
				idd.pt.x = idd.rc.left + idd.lLineIndent;
				idd.pt.y += idd.cyMinHeight + idd.lLineDistance;
				idd.cyTotalHeight += idd.cyMinHeight + idd.lLineDistance;
				idd.cyMinHeight = idd.cyCurLine;
			}
			idd.cyMinHeight = max(idd.cyMinHeight, __rcNeeded.bottom-__rcNeeded.top);
			idd.pt.x += __rcNeeded.right - __rcNeeded.left;
		}
		else
			__super::OnPaintContent(hde, hdc, rcPaint, bDraw, pidd);
	}
}

void CDuiXcp::SetAttribute( HDE hde, LPCWSTR szName, VARIANT* value )
{
	if (lstrcmpiW(szName, L"onload") == 0)
	{
		if (value->vt == VT_BSTR)
			m_pOwner->TryParseAsProcedure(L"onload", value->bstrVal);
	}
	else
		__super::SetAttribute(hde, szName, value);
}

BOOL CDuiXcp::GetNamedItem( LPCOLESTR name, VARIANT* pRet )
{
	if (!m_bLoaded || m_spOleObject.p==NULL) return FALSE;

	CComQIPtr<IXcpControl> spCtrl(m_spOleObject);
	if (spCtrl.p==NULL) return FALSE;

	CComDispatchDriver content;
	if (FAILED(spCtrl->get_Content(&content)) || content.p==NULL)
		return FALSE;

	CComVariant vName = name;
	return SUCCEEDED(content.Invoke1(name, &vName, pRet));
}