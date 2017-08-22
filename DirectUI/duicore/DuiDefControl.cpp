#include "stdafx.h"
#include "DuiManager.h"
#include "../../common/theme.h"
#include "DuiHelper.h"
#include "DuiAlpha.h"
#include "DuiInline.h"
#include "DuiEdit.h"
#include "DuiPluginsManager.h"
#include "DuiWindowControl.h"

//////////////////////////////////////////////////////////////////////////
// 换行
#define IDD_RETURN(idd) \
{ \
	DuiNode::ReposInlineRow(idd); \
	idd.pt.x = idd.rc.left + idd.lLineIndent; \
	idd.pt.y += idd.cyMinHeight + idd.lLineDistance; \
	idd.cyTotalHeight += idd.cyMinHeight + idd.lLineDistance; \
	idd.cyMinHeight = idd.cyCurLine; \
}

//////////////////////////////////////////////////////////////////////////
// DUI ELEMENT API
#define IsHdeValid(hde) ((hde) && DuiHandle<DuiNode>(hde)->_sign==DuiNode::__sign)

#define __rt_verify(...) if (!IsHdeValid(hde)) return __VA_ARGS__;
#define __rt DuiHandle<DuiNode>(hde)

BOOL DUIAPI DuiIsElement(HDE hde)
{
	return IsHdeValid(hde);
}

HWND DUIAPI DuiGetContainerHwnd(HDE hde)
{
	__rt_verify(NULL);
	return __rt->m_pLayoutMgr->GetPaintWindow();
}

IExtensionContainer* DUIAPI DuiGetExtensionContainer(HDE hde)
{
	__rt_verify(NULL);
	return (IExtensionContainer*)(DuiNode*)__rt;
}

HDE DUIAPI DuiGetParent(HDE hde)
{
	__rt_verify(NULL);
	return DuiHandle<DuiNode>(__rt->GetParent());
}

HDE DUIAPI DuiGetTopParent(HDE hde)
{
	__rt_verify(NULL);
	return DuiHandle<DuiNode>(__rt->GetRoot());
}

LPCWSTR DUIAPI DuiGetID(HDE hde)
{
	__rt_verify(NULL);
	return __rt->GetID();
}

LPCWSTR DUIAPI DuiGetText(HDE hde)
{
	__rt_verify(NULL);
	return __rt->GetText();
}

void DUIAPI DuiSetText(HDE hde, LPCWSTR lpszText)
{
	__rt_verify();
	__rt->SetText(lpszText);
}

LPCWSTR DUIAPI DuiGetToolTip(HDE hde)
{
	__rt_verify(NULL);
	return __rt->GetToolTip(FALSE);
}

void DUIAPI DuiSetTooTip(HDE hde, LPCWSTR lpszToolTip)
{
	__rt_verify();
	__rt->SetToolTip(lpszToolTip);
}

WCHAR DUIAPI DuiGetShortcut(HDE hde)
{
	__rt_verify(0);
	return __rt->GetShortcut();
}

void DUIAPI DuiSetShortcut(HDE hde, WCHAR ch)
{
	__rt_verify();
	__rt->SetShortcut(ch);
}

DWORD DUIAPI DuiGetState(HDE hde)
{
	__rt_verify(0);
	return __rt->GetState();
}

void DUIAPI DuiModifyState(HDE hde, DWORD dwAdded, DWORD dwRemoved)
{
	__rt_verify();
	__rt->ModifyState(dwAdded, dwRemoved);
}

BOOL DUIAPI DuiHasState(HDE hde, DWORD dwState)
{
	__rt_verify(FALSE);
	return __rt->HasState(dwState);
}

BOOL DUIAPI DuiIsVisible(HDE hde)
{
	__rt_verify(FALSE);
	return __rt->IsVisible();
}

void DUIAPI DuiSetVisible(HDE hde, BOOL bVisible)
{
	__rt_verify();
	__rt->SetVisible(bVisible);
}

BOOL DUIAPI DuiIsEnabled(HDE hde)
{
	__rt_verify(FALSE);
	return __rt->IsEnabled();
}

void DUIAPI DuiSetEnabled(HDE hde, BOOL bEnabled)
{
	__rt_verify();
	__rt->SetEnabled(bEnabled);
}

BOOL DUIAPI DuiIsFocused(HDE hde)
{
	__rt_verify(FALSE);
	return __rt->IsFocused();
}

void DUIAPI DuiSetFocus(HDE hde)
{
	__rt_verify();
	__rt->SetFocus();
}

void DUIAPI DuiInvalidate(HDE hde)
{
	__rt_verify();
	__rt->Invalidate();
}

RECT DUIAPI DuiGetRect(HDE hde)
{
	__rt_verify(CRect());
	return __rt->GetPos(DUIRT_TOTAL);
}

RECT DUIAPI DuiGetClientRect(HDE hde)
{
	__rt_verify(CRect());
	return __rt->GetPos();
}

int DUIAPI DuiGetChildCount(HDE hde)
{
	__rt_verify(0);
	return __rt->GetChildCount();
}

HDE DUIAPI DuiGetChild(HDE hde, int index)
{
	__rt_verify(NULL);
	return DuiHandle<DuiNode>(__rt->GetChildControl(index));
}

BOOL DUIAPI DuiRemoveChild(HDE hde, HDE hdeChild)
{
	__rt_verify(FALSE);
	if (!IsHdeValid(hdeChild)) return FALSE;
	return __rt->RemoveChildControl(DuiHandle<DuiNode>(hdeChild));
}

void DUIAPI DuiRemoveAllChildren(HDE hde)
{
	__rt_verify();
	__rt->RemoveAllChildren();
}

BOOL DUIAPI DuiSetInnerXML(HDE hde, LPCWSTR lpszXml)
{
	__rt_verify(FALSE);
	return __rt->SetInnerXML(lpszXml);
}

BOOL DUIAPI DuiSetOuterXML(HDE hde, LPCWSTR lpszXml)
{
	__rt_verify(FALSE);
	return __rt->SetOuterXML(lpszXml);
}

HRGN DUIAPI DuiGetRegion(HDE hde)
{
	__rt_verify(NULL);
	return __rt->GetRegion();
}

BOOL DUIAPI DuiGetAttribute(HDE hde, LPCWSTR lpszAttr, VARIANT* pvResult)
{
	__rt_verify(FALSE);
	if (!__rt->_attributes.HasAttribute(lpszAttr)) return FALSE;
	if (pvResult)
		__rt->GetAttribute(lpszAttr).Detach(pvResult);
	return TRUE;
}

LPCWSTR DUIAPI DuiGetAttributeString(HDE hde, LPCWSTR lpszAttr)
{
	__rt_verify(NULL);
	return __rt->GetAttributeString(lpszAttr);
}

void DUIAPI DuiSetAttribute(HDE hde, LPCWSTR lpszAttr, VARIANT* value)
{
	__rt_verify();
	if (value==NULL) return;
	__rt->SetAttribute(lpszAttr, *value);
}

void DUIAPI DuiSetAttributeString(HDE hde, LPCWSTR lpszAttr, LPCWSTR lpszValue)
{
	__rt_verify();
	__rt->SetAttribute(lpszAttr, lpszValue);
}

void DUIAPI DuiRemoveAttribute(HDE hde, LPCWSTR lpszAttr)
{
	__rt_verify();
	__rt->RemoveAttribute(lpszAttr);
}

#undef __rt_verify
#undef __rt
// END API
//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
#define DelegateIOC(name, params, ret) ret;
	//if (m_ppIOC && *m_ppIOC && !m_ioc) \
	//{ \
	//	m_ioc = TRUE; \
	//	if ((*m_ppIOC)->name ## params ) { m_ioc=FALSE; return ret; } \
	//	m_ioc = FALSE; \
	//}


//////////////////////////////////////////////////////////////////////////
DuiNode::DuiNode( LPCOLESTR tagname, DuiNode* parent, CDuiLayoutManager* lyt ) :
	_sign(__sign), m_tagName(tagname), m_pLayoutMgr(lyt), m_pParent(parent), _dxt(NULL), m_hRgn(NULL),
	TOrderList<DuiNode>(&DuiNode::GetZIndex),
	m_defproc(this),
	m_offsetChildren(NULL),
	m_pStyle(NULL),
	//m_pController(NULL), m_ppIOC(NULL), m_ioc(FALSE),
#ifndef NO3D
	m_pSurface(NULL),
	m_pScene(NULL),
#endif
	m_scrollBar(Handle()),
	m_scrollBarH(Handle(), FALSE),
	m_bAutoDestroy(TRUE),
	m_readyState(READYSTATE_UNINITIALIZED),
	m_dblclick(0),
	//m_bNeedUpdateRect(TRUE),
	m_fnEventCallback(NULL),
	m_chShortcut(0),
	m_dwState(0)
{
	ZeroMemory(&m_rcItem, sizeof(RECT));
	ZeroMemory(&__cache, sizeof(__cache));
}

DuiNode::~DuiNode()
{
	m_eventHandler = (IDispatch*)NULL;

	if (__cache.layout)
	{
		__cache.layout->Unuse(Handle());
		__cache.layout = NULL;
	}
	for (int i=0; i<m_aExtensions.GetSize(); i++)
	{
		IExtension* pExt = static_cast<IExtension*>(m_aExtensions[i]);
		pExt->Unuse(Handle());
	}
	m_aExtensions.RemoveAll();
	_attributes.SetCallback(NULL, NULL);
	RemoveAllChildren();

#ifndef NO3D
	if (m_pSurface)
	{
		//m_pLayoutMgr->Get3DDevice()->UnRegisterNamedObject(GetObject());
		m_pSurface->Dispose();
	}
#endif

	RemoveFilter();
	gConnector.Disconnect(this);
	gConnector.Disconnect(NULL, this);
	if( m_pLayoutMgr != NULL ) m_pLayoutMgr->ReapObjects(this);
	if (m_pStyle && m_pStyle->GetOwnerControl()==this) m_pLayoutMgr->RemoveStyle(m_pStyle);
	if (m_hRgn) DeleteObject(m_hRgn); m_hRgn = NULL;

	CFont font = __cache.font; // auto destroy
}

void DuiNode::SetReadyState(READYSTATE rs)
{
	m_readyState = rs;
	switch (m_readyState)
	{
	case READYSTATE_LOADED:
		// 在这个阶段才设置属性改变通知回调，以避免初始化阶段不必要的回调处理，真正的回调延迟到 INTERACTIVE 阶段做
		_attributes.SetCallback(DuiNode::OnAttributeChanged, this);
		break;
	case READYSTATE_INTERACTIVE:
		{
			// 处理延迟的属性变化通知，先处理自己的属性，然后对子元素递归
			// 
			for (int i=0, num=_attributes.GetSize(); i<num; i++)
				DuiNode::OnAttributeChanged(FALSE, _attributes.GetKeyAt(i), this);

			// 递归设置子元素的 READYSTATE
			for (int i=0,num=GetChildCount(); i<num; i++)
				(GetChildControl(i))->SetReadyState((READYSTATE)m_readyState);
		}
		break;
	case READYSTATE_COMPLETE:
		{
			// 递归设置子元素的 READYSTATE
			for (int i=0,num=GetChildCount(); i<num; i++)
				(GetChildControl(i))->SetReadyState((READYSTATE)m_readyState);

			// 现在激发load事件
			DuiEvent event = { (DUI_EVENTTYPE)0 };
			event.Name = event_type_to_name(DUIET_load);
			event.pSender = DuiHandle<DuiNode>(this);
			event.dwTimestamp = ::GetTickCount();
			event.cancelBubble = TRUE;
			//pRet->OnEvent(event);
			m_pLayoutMgr->FireEvent(this, event);
		}
		break;
	}
}

void DuiNode::RemoveFilter()
{
	if (_dxt)
	{
		CComQIPtr<IPeerHolder> peer = GetObject();
		if (peer.p)
		{
			peer->DetachObject(_dxt->effect());
			peer->DetachObject(_dxt->disp());
		}
		Safe_Delete(_dxt);
	}
}

void DuiNode::SetFilter( LPCOLESTR progid/*=NULL*/ )
{
	if (progid==NULL) { RemoveFilter(); return; }

	if (_dxt==NULL || !_dxt->IsType(progid))
	{
		RemoveFilter();
		_dxt = DXT::create_transform(progid);
		if (_dxt)
		{
			// attach peer objects
			CComQIPtr<IPeerHolder> peer = GetObject(TRUE);
			if (peer.p)
			{
				peer->AttachObject(_dxt->disp());
				peer->AttachObject(_dxt->effect());
			}
		}
	}
	// init
	if (_dxt) SetFilterProps();
}

void DuiNode::SetFilterProps()
{
	if (_dxt)
	{
		DxtContext ctx;
		ctx._dc = m_pLayoutMgr->GetSurface()->GetDC(); // 只有针对系统内存的DC才可以调用GetDC，否则找不到合适的机会执行ReleaseDC
		ctx._rc = GetPos(DUIRT_TOTAL); //CDuiControlExtension::GetOffset(GetParent(), NULL, GetPos(DUIRT_TOTAL));
		//ctx._rcClip = CDuiControlExtension::GetClipRect(_ctrl);
		ctx._data = this;
		ctx._transformProc = DuiNode::TransformProc;
		ctx._drawToDC = DuiNode::DrawToDC;
		ctx._drawBkToDC = DuiNode::DrawBackgroundToDC;
		ctx._getWindowOrg = DuiNode::GetParentOrg;
		ctx._props = _attributes.GetSubAttributeMap(L"filter");
		_dxt->Init(ctx);
	}
}

/////////////////////////////////////////////////////////////////////////////////////
//
//


LPVOID DuiNode::CastToClass( LPCOLESTR szName/*=NULL*/ )
{
	if (szName==NULL) return NULL;
	LPVOID p = NULL;
	GetClassCollection(&p, szName, 1);
	//rt rtCtrl(this);
	//for (int i=0; i<rtCtrl->m_aExtensions.GetSize(); i++)
	//{
	//	CDuiExtension* pe = (CDuiExtension*)rtCtrl->m_aExtensions[i];
	//	if (pe)
	//	{
	//		p = pe->CastToClass(szName);
	//		if (p) return p;
	//	}
	//}
	//if (p==NULL && rtCtrl->GetLayout()) p = rtCtrl->GetLayout()->CastToClass(szName);
	//if (p==NULL && rtCtrl->GetVisual()) p = rtCtrl->GetVisual()->CastToClass(szName);
	return p;
}

int DuiNode::GetClassCollection(LPVOID* buf, LPCOLESTR szName, int bufSize/*=0*/)
{
	int num = 0;
	LPVOID p = NULL;
#define __assign_ptr(p) { buf[num++]=p; if(bufSize>0 && num>=bufSize) return num; }
	//p = _ctrl->CastToClass(szName);
	//if (p) buf[num++] = p;
	for (int i=0; i<m_aExtensions.GetSize(); i++)
	{
		IExtension* pe = (IExtension*)m_aExtensions[i];
		if (pe)
		{
			p = pe->CastToClass(szName);
			if (p) __assign_ptr(p);
		}
	}

	if (m_pLayoutMgr->m_lPaintLocked==0)
	{
		//if (GetLayout() && (p=GetLayout()->CastToClass(szName))) __assign_ptr(p);;
		//if (GetVisual() && (p=GetVisual()->CastToClass(szName))) __assign_ptr(p);;
	}
#undef __assign_ptr
	return num;
}

DWORD DuiNode::GetState()
{
	DWORD state;
	DelegateIOC(GetState, (this, &state), state);

	return m_dwState;
}

BOOL DuiNode::HasState(DWORD dwState)
{
	BOOL b;
	DelegateIOC(HasState, (this, dwState, &b), b);

	return (m_dwState & dwState) != 0;
}

DWORD DuiNode::SetState(DWORD dwNewState)
{
	DWORD state;
	DelegateIOC(SetState, (this, dwNewState, &state), state);

	StateChangedNotifyHelper scnh(this);
	DWORD dwOld = m_dwState;
	m_dwState = dwNewState;
	return dwOld;
}

void DuiNode::ModifyState(DWORD dwAdded, DWORD dwRemoved)
{
	DelegateIOC(ModifyState, (this, dwAdded, dwRemoved), ;);

	StateChangedNotifyHelper scnh(this);
	if (dwAdded) m_dwState |= dwAdded;
	if (dwRemoved) m_dwState &= ~dwRemoved;
}

void DuiNode::OnStateChanged(DWORD dwStateMask)
{
	LockReenter(OnStateChanged);

	if (dwStateMask & DUISTATE_MASK)
	{
		//if ((dwStateMask & DUISTATE_HIDDEN) && IsVisible())
		//{
		//	CDuiControlExtension::EstimateContentSize(_ctrl, CRect(GetPos()).Size());
		//}

		InvokeAllSinks(this, OnStateChanged, dwStateMask);

		//for (int i=0; i<m_aExtensions.GetSize(); i++)
		//{
		//	CDuiExtension* pExt = (CDuiExtension*)m_aExtensions[i];
		//	ATLASSERT(pExt);
		//	CDuiSinkOnStateChanged* pcb = CLASS_CAST(pExt, CDuiSinkOnStateChanged);
		//	if (pcb)
		//		pcb->OnStateChanged(dwStateMask);
		//}

#ifndef NO3D
		if ((dwStateMask & DUISTATE_HIDDEN) && m_pSurface)
		{
			m_pSurface->State(ost_visible, !HasState(DUISTATE_HIDDEN));
		}

		if ((dwStateMask & DUISTATE_DISABLED) && m_pSurface)
		{
			m_pSurface->State(ost_enable, !HasState(DUISTATE_DISABLED));
		}

		//if ((dwStateMask & DUISTATE_FOCUSED) && m_pSurface)
		//{
		//	CDuiEditable* pEdit = CLASS_CAST(_ctrl, CDuiEditable);
		//	if (pEdit && !HasState(DUISTATE_FOCUSED))
		//		pEdit->TxShowCaret(FALSE);
		//}

		Update3DState();
#endif

		Invalidate();
	}
}

BOOL DuiNode::OnChildStateChanged(HDE pChild, DWORD dwStateMask)
{
	LockReenter(OnChildStateChanged, FALSE);

	BOOL bDone = FALSE;
	InvokeAllSinksBool(this, OnChildStateChanged, pChild, dwStateMask);
	if (bDone) return bDone;
	//for (int i=0; i<m_aExtensions.GetSize(); i++)
	//{
	//	CDuiExtension* pExt = (CDuiExtension*)m_aExtensions[i];
	//	ATLASSERT(pExt);
	//	CDuiSinkOnChildStateChanged* pcb = CLASS_CAST(pExt, CDuiSinkOnChildStateChanged);
	//	if (pcb && pcb->OnChildStateChanged(pChild, dwStateMask))
	//		return TRUE;
	//}

	if ((dwStateMask & DUISTATE_HIDDEN) && m_readyState==READYSTATE_COMPLETE /*&& rt(pChild)->IsVisible()*/)
	{
		UpdateLayout();
	}

	//if (GetLayout()) return GetLayout()->OnChildStateChanged(pChild, dwStateMask);
	return FALSE;
}

BOOL DuiNode::IsVisible(BOOL bRealResult/*=FALSE*/)
{
	BOOL b;
	DelegateIOC(IsVisible, (this, bRealResult, &b), b);

	if (m_pParent && bRealResult)
		return (m_pParent)->IsVisible(bRealResult) && ((m_dwState & DUISTATE_HIDDEN) == 0);
	return (m_dwState & DUISTATE_HIDDEN) == 0;
}

BOOL DuiNode::IsEnabled(BOOL bRealResult/*=FALSE*/)
{
	BOOL b;
	DelegateIOC(IsEnabled, (this, bRealResult, &b), b);

	if (m_pParent && bRealResult)
		return (m_pParent)->IsEnabled(bRealResult) && ((m_dwState & DUISTATE_DISABLED) == 0);
	return (m_dwState & DUISTATE_DISABLED) == 0;
}

BOOL DuiNode::IsFocused()
{
	BOOL b;
	DelegateIOC(IsFocused, (this, &b), b);

	return (m_dwState & DUISTATE_FOCUSED);
}

void DuiNode::SetVisible(BOOL bVisible)
{
	DelegateIOC(SetVisible, (this, bVisible), );

	StateChangedNotifyHelper scnh(this);
	if( bVisible == IsVisible() ) return;
	if (bVisible)
		m_dwState &= ~DUISTATE_HIDDEN;
	else
		m_dwState |= DUISTATE_HIDDEN;
	//Invalidate();
	////if (m_pParent) UpdateLayout(m_pParent); // stack overflow!!!
	////else
		//if( m_pLayoutMgr != NULL ) m_pLayoutMgr->UpdateLayout();
}

void DuiNode::SetEnabled(BOOL bEnabled)
{
	DelegateIOC(SetEnabled, (this, bEnabled), );

	StateChangedNotifyHelper scnh(this);
	if (bEnabled)
		m_dwState &= ~DUISTATE_DISABLED;
	else
		m_dwState |= DUISTATE_DISABLED;
	Invalidate();
}

BOOL DuiNode::Activate()
{
	BOOL b;
	DelegateIOC(Activate, (this, &b), b);

	struct DefCallback
	{
		static BOOL /*CALLBACK*/ Activate(CallContext* pcc, HDE hde)
		{
			DuiNode* n = DuiNode::FromHandle(hde);

			if( !n->IsVisible() ) return FALSE;
			if( !n->IsEnabled() ) return FALSE;
			return TRUE;
		}
	};

	LockReenter(Activate, FALSE);
	BOOL bRet = FALSE;
	BOOL bHasInvocation = FALSE;
	InvokeOneSinkResult(bRet, this, Activate);
	if (!bHasInvocation)
		return DefCallback::Activate(NULL, Handle());
		//return (BOOL)m_defproc.CallProc(&bRet, OpCode_Activate, Handle());
	else
		return bRet;
}

DuiNode* DuiNode::GetParent() const
{
	return m_pParent;
}

void DuiNode::SetFocus()
{
	DelegateIOC(SetFocus, (this), ;);

	if (m_pLayoutMgr)
	{
		if ((GetControlFlags() & DUIFLAG_TABSTOP) && IsEnabled(TRUE) && IsVisible(TRUE))
		{
			StateChangedNotifyHelper scnh(this);
			m_pLayoutMgr->SetFocus(this);
		}
		else if (m_pParent)
			(m_pParent)->SetFocus();
	}
}

void DuiNode::SetShortcut(OLECHAR ch)
{
	DelegateIOC(SetShortcut, (this, ch), ;);

	m_chShortcut = ch;
}

OLECHAR DuiNode::GetShortcut()
{
	OLECHAR c;
	DelegateIOC(GetShortcut, (this, &c), c);

	return m_chShortcut;
}

LPCOLESTR DuiNode::GetText()
{
	return GetChildCount()>0 ? (GetChildControl(0))->GetText() : (_attributes).GetAttributeString(L"text");
}

void DuiNode::SetText(LPCOLESTR pstrText)
{
	if (GetChildCount() > 0) (GetChildControl(0))->SetText(pstrText);
	else (_attributes).SetAttribute(L"text", pstrText);
	//CDuiControlExtension::UpdateLayout(CDuiControlExtension::GetOffsetParent(_ctrl, TRUE), _ctrl);
	//Invalidate();
}

void DuiNode::SetToolTip(LPCOLESTR pstrText)
{
	(_attributes).SetAttribute(L"title", pstrText);
}

LPCOLESTR GetDebugInfo(DuiNode* pCtrl, CStdString& dst)
{
	if (pCtrl == NULL) return dst;
	DuiNode* r = (pCtrl);

	CStdString rcs;
	POINT pt = {0};
	pt = DuiNode::GetOffset(pCtrl->GetOffsetParent(), NULL, pt);
	if (r->m_hRgn)
	{
		rcs.Format(L"[Inline] Total(%d,%d,%d,%d)", r->m_rcItem.top+pt.y, r->m_rcItem.bottom+pt.y, r->m_rcItem.left+pt.x, r->m_rcItem.right+pt.x);
		for (int i=0, num=r->__cache.rects.GetSize(); i<num; i++)
		{
			const DuiNode::RgnRect& rr = r->__cache.rects[i];
			rcs.AppendFormat(L"\n%d (%d,%d,%d,%d)", i, rr.rc.top+pt.y, rr.rc.bottom+pt.y, rr.rc.left+pt.x, rr.rc.right+pt.x);
		}
	}
	else
	{
		rcs.Format(L"[Block] Total(%d,%d,%d,%d)\nBorder(%d,%d,%d,%d)\nInsideBorder(%d,%d,%d,%d)\nContent(%d,%d,%d,%d)",
			r->__cache.pos.rcPart[DUIRT_TOTAL].top+pt.y, r->__cache.pos.rcPart[DUIRT_TOTAL].bottom+pt.y, r->__cache.pos.rcPart[DUIRT_TOTAL].left+pt.x, r->__cache.pos.rcPart[DUIRT_TOTAL].right+pt.x,
			//r->m_rcItem.top+pt.y, r->m_rcItem.bottom+pt.y, r->m_rcItem.left+pt.x, r->m_rcItem.right+pt.x,
			r->__cache.pos.rcPart[DUIRT_BORDER].top+pt.y, r->__cache.pos.rcPart[DUIRT_BORDER].bottom+pt.y, r->__cache.pos.rcPart[DUIRT_BORDER].left+pt.x, r->__cache.pos.rcPart[DUIRT_BORDER].right+pt.x,
			r->__cache.pos.rcPart[DUIRT_INSIDEBORDER].top+pt.y, r->__cache.pos.rcPart[DUIRT_INSIDEBORDER].bottom+pt.y, r->__cache.pos.rcPart[DUIRT_INSIDEBORDER].left+pt.x, r->__cache.pos.rcPart[DUIRT_INSIDEBORDER].right+pt.x,
			r->__cache.pos.rcPart[DUIRT_CONTENT].top+pt.y, r->__cache.pos.rcPart[DUIRT_CONTENT].bottom+pt.y, r->__cache.pos.rcPart[DUIRT_CONTENT].left+pt.x, r->__cache.pos.rcPart[DUIRT_CONTENT].right+pt.x
			);
	}
	dst.AppendFormat(L"Rects[top,bottom,left,right] of \"%s\":\n--------------------------------------------\n%s", pCtrl->m_tagName, rcs);
	return dst;
}

LPCOLESTR DuiNode::GetToolTip(BOOL bAllowDebug/*=TRUE*/)
{
	LPCOLESTR ptt = (_attributes).GetAttributeString(L"title");
	if (ptt==NULL) ptt = ((m_pParent) ? (m_pParent)->GetToolTip(FALSE) : NULL);

	if (bAllowDebug && m_pLayoutMgr->GetAttributeBool(L"debug-control"
//#ifdef _DEBUG
//		, TRUE
//#endif // _DEBUG
		))
	{
		static CStdString tt;
		if (ptt) 
		{
			tt = ptt;
			tt += L"\n\n>>>> Control Info <<<<\n======================\n";
			//tt.Format(L"%s\n\n>>>> Control Info <<<<\n======================\n", ptt);
		}
		else tt = L"";
		tt.AppendFormat(L"CURSOR:[%d,%d]\n", m_pLayoutMgr->m_ptLastMousePos.x, m_pLayoutMgr->m_ptLastMousePos.y);
		GetDebugInfo(this, tt);
		if (GetParent())
		{
			CStdString tt2;
			tt.AppendFormat(L"\n\nvvvvvvvvvv [PARENT] vvvvvvvvvv\n%s", GetDebugInfo(GetParent(), tt2));
		}
		return tt;
	}
	return ptt;
	//LPCOLESTR ptt = (_attributes).GetAttributeString(L"title");
	//if (ptt) return ptt;
	//return (m_pParent) ? rt(m_pParent)->GetToolTip() : NULL;
}

//void CDuiControlExtension::Init()
//{
//}

CDuiLayoutManager* DuiNode::GetLayoutManager() const
{
	return m_pLayoutMgr;
}

void DuiNode::SetManager(CDuiLayoutManager* pLayoutMgr, DuiNode* pParent)
{
	//BOOL bInit = (m_pLayoutMgr == NULL);
	m_pLayoutMgr = pLayoutMgr;
	if (pParent && m_pParent==NULL) m_pLayoutMgr->m_aDelayedCleanup.Remove(this);
	m_pParent = pParent;
	if (m_pParent)
	{
		InvokeAllSinks(m_pParent, OnChildAdded, DuiHandle<DuiNode>(this));
	}
	//if( bInit ) Init();

	for( int i=0; i<GetChildCount(); i++ )
		(GetChildControl(i))->SetManager(pLayoutMgr, this);
}

LPCOLESTR DuiNode::GetID() const
{
	if (m_id.IsEmpty())
		return NULL;
	return m_id;
	//return _attributes.GetAttributeString(L"id");
}

void DuiNode::SetID(LPCTSTR pstrId)
{
	m_id = pstrId;
	//_attributes.SetAttribute(L"id", pstrId);
}

/*
 *	pt 是窗口客户区相关
 */
BOOL DuiNode::PtInPos(POINT pt, DUI_RECTTYPE rt)
{
	if (m_hRgn) return FALSE;
	DuiNode::AdjustOffset(NULL, GetOffsetParent(), pt);
	return ::PtInRect(&GetPos(rt), pt);
}

/*
*	pt 是窗口客户区相关
*/
BOOL DuiNode::PtInControl(POINT pt)
{
	POINT pt2 = DuiNode::GetOffset(NULL, GetOffsetParent(), pt);
	return m_hRgn ? 
			::PtInRegion(m_hRgn,pt2.x,pt2.y) : 
			PtInPos(pt, DUIRT_BORDER); // ::PtInRect(&GetPos(DUIRT_BORDER), pt2); // GetPos 获取相对parent的矩形
}

/*
*	pt 是窗口客户区相关
*/
BOOL DuiNode::PtInBackground( POINT pt)
{
	POINT pt2 = pt;
	DuiNode::AdjustOffset(NULL, GetOffsetParent(), pt2);
	if (m_hRgn) return ::PtInRegion(m_hRgn, pt2.x, pt2.y);
	if (!::PtInRect(&GetPos(DUIRT_BORDER), pt2)) return FALSE;
	CRect rcbk = GetPos(DUIRT_INSIDEBORDER);
	if (!::PtInRect(&rcbk, pt2)) return TRUE; // at the border, not transparent

#ifndef NO3D
	if (m_pSurface) return TRUE;
#endif

	CDuiStyleVariant* sv = (CDuiStyleVariant*)GetStyleAttribute(SAT_BACKGROUND);
	if (sv==NULL || sv==&svEmpty || sv->svt!=SVT_EXTERNAL) return TRUE;
	CDuiImageResource* pSrc = (CDuiImageResource*)sv->extVal;
	if (pSrc==NULL || pSrc->img==NULL || pSrc->img->IsNull()) return TRUE;

	DuiNode::AdjustOffset(NULL, this, pt);
	//pt.x -= rcbk.left;
	//pt.y -= rcbk.top;
	return !pSrc->img->IsTransparent(pt);
}

/*
*	pt 是窗口客户区相关
*/
BOOL DuiNode::PtInScrollbar(POINT pt)
{
	return m_scrollBar.PtInScrollbar(pt) || m_scrollBarH.PtInScrollbar(pt);
}

LONG DuiNode::NcHittest(POINT pt)
{
	if (!PtInControl(pt) || PtInScrollbar(pt)) return HTNOWHERE;
	DuiStyleVariant* sv = (DuiStyleVariant*)GetStyleAttribute(SAT_HITTEST_AS);
	if (sv && sv!=&svEmpty) return FromStyleVariant(sv, LONG);//(LONG)(sv->operator ()<LONG>());
	return HTCLIENT;
}

DuiNode* DuiNode::FindControl(FINDCONTROLPROC Proc, LPVOID pData, UINT uFlags)
{
	CDuiInlineControlBase* icb = CLASS_CAST(this, CDuiInlineControlBase);
	if (icb)
		return icb->FindControl(Proc, pData, uFlags);

	if( (uFlags & DUIFIND_VISIBLE) != 0 && !IsVisible() ) return NULL;
	if( (uFlags & DUIFIND_ENABLED) != 0 && !IsEnabled() ) return NULL;
	if( (uFlags & DUIFIND_HITTEST) != 0 && !PtInControl(* static_cast<LPPOINT>(pData)) ) return NULL;
	if( (uFlags & DUIFIND_ME_FIRST) != 0 )
	{
		DuiNode* pCtrl = DuiHandle<DuiNode>(Proc(DuiHandle<DuiNode>(this), pData));
		if (pCtrl) return pCtrl;
	}

	// 当寻找子控件时，必须确保光标在内容区域内，PtInControl只能保证光标在边界区域
	if ((GetControlFlags()&DUIFLAG_NOCHILDREN)==0 || (uFlags & DUIFIND_HITTEST)==0 || m_hRgn || (PtInPos(*static_cast<LPPOINT>(pData), DUIRT_CONTENT))/* ::PtInRect(&GetPos(), *static_cast<LPPOINT>(pData))*/)
	{
		if ((uFlags & DUIFIND_HITTEST))
		{
			// 搜索正值的Z-ORDER
			DuiNode* pCtrl = COrderListFinder<false,true>::Find(m_offsetChildren, Proc, pData, uFlags);
			if (pCtrl) return pCtrl;
		}

		int num = m_children.GetSize();
		//for( int i=m_children.GetSize()-1; i>=0; i-- )
		for( int i=0; i<num; i++ )
		{
			int i2 = i;
			if ((uFlags & DUIFIND_HITTEST)) i2 = num-i-1; // 对于寻找光标下的控件应该采用反序
			DuiNode* r = (GetChildControl(i2));
			if ((uFlags & DUIFIND_HITTEST) && r->GetPosition()) continue;
			DuiNode* pCtrl = r->FindControl(Proc, pData, uFlags);
			if (pCtrl) return pCtrl;
		}

		if ((uFlags & DUIFIND_HITTEST))
		{
			// 搜索负值的Z-ORDER
			DuiNode* pCtrl = COrderListFinder<true,false>::Find(m_offsetChildren, Proc, pData, uFlags);
			if (pCtrl) return pCtrl;
		}
	}
	DuiNode* pCtrl = DuiHandle<DuiNode>(Proc(DuiHandle<DuiNode>(this), pData));
	if (pCtrl && m_pParent && (uFlags & DUIFIND_HITTEST) && Proc==&CDuiLayoutManager::__FindControlFromPoint)
		Proc(DuiHandle<DuiNode>(this), pData);
		//LineLog(_ctrl->__tag);
	return pCtrl;
}

DuiNode* DuiNode::GetOffsetParent(BOOL bIncludeSelf/*=FALSE*/)
{
	if (bIncludeSelf && IsBlock()) return this;
	DUI_POSITION pos = GetPosition();
	if (pos==DUIPOS_FIXED) return NULL;
	return __cache.offset.parent ? __cache.offset.parent : NULL;
	//CDuiControlExtensionBase* pRoot = rt(pCtrl)->GetRoot();
	//pCtrl = rt(pCtrl)->GetParent();
	//while (pCtrl && !CDuiControlExtension::IsBlock(pCtrl)) pCtrl = rt(pCtrl)->GetParent();
	//if (pos==DUIPOS_ABSOLUTE && (pCtrl==NULL || CDuiControlExtension::GetPosition(pCtrl)<=DUIPOS_RELATIVE))
	//	return pRoot;
	//return pCtrl;
}

SIZE DuiNode::GetOffsetParentSize(DUI_RECTTYPE rtp/* =DUIRT_CONTENT */)
{
	CSize sz;
	DuiNode* pOffsetParent = GetOffsetParent();
	if (pOffsetParent)
	{
		CRect rcParent = (pOffsetParent)->GetPos(rtp);
		sz = rcParent.Size();
	}
	else // get window client rect
	{
		CRect rc;
		::GetClientRect(m_pLayoutMgr->GetPaintWindow(), &rc);
		sz = rc.Size();
	}
	return sz;
}

DuiNode* DuiNode::GetNearestUpdateLayoutParent()
{
	DuiNode* parent = GetParent();
	while (parent && 
		((CLASS_CAST(parent, CDuiOnChildWidthChanged) && FromStyleVariant(parent->GetStyleAttribute(SAT_WIDTH), LONG, (LPVOID)(LONG_PTR)parent->GetOffsetParentSize().cx)==0) ||
		FromStyleVariant(parent->GetStyleAttribute(SAT_HEIGHT), LONG, (LPVOID)(LONG_PTR)parent->GetOffsetParentSize().cy)==0))
	{
		parent = (parent)->GetParent();
	}

	return parent ? parent : GetRoot();
}

BOOL DuiNode::IsChild(DuiNode* pParent, DuiNode* pChild, BOOL bIncludeSame/* =TRUE */)
{
	if (pParent==NULL || pChild==NULL) return FALSE;
	if (pParent==pChild && bIncludeSame) return TRUE;

	DuiNode* parent = (pChild)->GetParent();
	while (parent && parent!=pParent) parent = (parent)->GetParent();
	return parent==pParent;
}

void DuiNode::UpdateScrollRange()
{
	if (__cache.updateScroll )
	{
		__cache.updateScroll = FALSE;
		if (IsAllowScroll())
		{
			SIZE sz = GetContentSize();
			SetScrollRange(sz.cx, sz.cy);
#ifndef NO3D
			Update3DSurface();
#endif
		}
	}
}

void DuiNode::UpdateInlineRegion()
{
	LockReenter(OnUpdateInlineRegion);

	if (!IsInline()) return;

	// 先更新子元素
	// 
	if ((GetControlFlags() & DUIFLAG_NOCHILDREN) == 0)
	{
		for (int i=0, num=GetChildCount(); i<num; i++)
			GetChildControl(i)->UpdateInlineRegion();
	}

	// 计算自己的区域和矩形
	struct DefCallback
	{
		static DuiVoid /*CALLBACK*/ OnUpdateInlineRegion(CallContext* pcc, HDE hde)
		{
			DuiNode* n = DuiNode::FromHandle(hde);
			if ((n->GetControlFlags() & DUIFLAG_NOCHILDREN)==0 && n->GetChildCount()>0)
			{
				if (n->m_hRgn) ::DeleteObject(n->m_hRgn);
				n->m_hRgn = n->CombineChildrenRegion();
				n->UpdateRectFromRgn();
			}
			else
				n->UpdateRectFromRgn(n->UpdateRgnFromRects());
			return 0;
		}
	};

	//BOOL bHasInvocation = FALSE;
	InvokeOneSink(this, OnUpdateInlineRegion);
	//if (!bHasInvocation)
	//ISinkOnUpdateInlineRegion* uir = CLASS_CAST(this, ISinkOnUpdateInlineRegion);
	//if (uir)
	//{
	//	uir->OnUpdateInlineRegion(Handle());
	//}
	//else
	//{
	//	if ((GetControlFlags() & DUIFLAG_NOCHILDREN)==0 && GetChildCount()>0)
	//	{
	//		if (m_hRgn) ::DeleteObject(m_hRgn);
	//		m_hRgn = CombineChildrenRegion();
	//		UpdateRectFromRgn();
	//	}
	//	else
	//		UpdateRectFromRgn(UpdateRgnFromRects());
	//}
}

void DuiNode::ReposInlineRow(InDrawData& idd)
{
	if (idd.pvReserved == NULL) return;
	RowItems& rows = *(RowItems*)idd.pvReserved;
	if (rows.GetSize()==0) return;

	long cx = 0;
	long cxRemain = idd.rc.right - idd.pt.x;
	if (idd.uTextStyle & DT_CENTER) cx = cxRemain/2;
	else if (idd.uTextStyle & DT_RIGHT) cx = cxRemain;

	for (int i=0, num=rows.GetSize(); i<num; i++)
	{
		RowItem& row = rows[i];
		if (row.r && row.index>=0 && row.index<row.r->__cache.rects.GetSize())
		{
			//BOOL bHasInvocation = FALSE;
			InvokeOneSink(row.r, OnReposInlineRect, row.index, cx, &idd);
			if (!bHasInvocation)
			//ISinkOnReposInlineRect* rir = CLASS_CAST(row.r, ISinkOnReposInlineRect);
			//if (rir)
			//{
			//	rir->OnReposInlineRect(NULL, row.index, cx, &idd);
			//}
			//else
			{
				RECT& rc = row.r->__cache.rects[row.index].rc;
				//if ((idd.uTextStyle & DT_SINGLELINE) == 0) // 单行模式应该使用容器高度，非单行才需要裁剪高度
				//	rc.bottom = rc.top + idd.cyMinHeight;
				if (cx != 0)
				{
					rc.left += cx;
					rc.right += cx;
				}
			}
		}
	}

	rows.RemoveAll();
}

#ifndef NO3D
void DuiNode::Update3DSurface()
{
	LockReenter(OnUpdate3DSurface);
	if (m_pLayoutMgr->Get3DDevice())
	{
		RECT rcContent = GetClientRect(DUIRT_BORDER);
		InvokeOneSink(this, OnUpdate3DSurface, rcContent);
		if (!bHasInvocation)
		//ISinkOnUpdate3DSurface* cs = CLASS_CAST(this, ISinkOnUpdate3DSurface);
		//if (cs)
		//{
		//	cs->OnUpdate3DSurface(Handle(), rcContent);
		//}
		//else
		{
			if (m_pSurface==NULL)
			{
				m_pSurface = GdiLayer::New(m_pLayoutMgr->Get3DDevice(), rcContent.left, rcContent.top, (rcContent.right-rcContent.left), (rcContent.bottom-rcContent.top),
					m_pParent ? m_pParent->m_pSurface : NULL);
				if (m_pSurface)
				{
					//LPCOLESTR id = r->GetID();
					//if (id)
					//	r->m_pLayoutMgr->Get3DDevice()->RegisterNamedObject(id, r->GetObject(TRUE));
					//if (CLASS_CAST(this, CDuiWindowControl))
					//	m_pSurface->SetTransparent(FALSE);
					m_pSurface->SetPaintCallback(this);
					m_pSurface->SetExtensionContainer(this);
					m_pSurface->Clip(&m_pathClip);
					m_pSurface->SetName(GetID());
					Update3DState();
					InvokeAllSinks(this, On3DSurfaceCreated);
					//r->m_pSurface->ClipContent(&CDuiControlExtension::GetClipRect(pCtrl, DUIRT_CONTENT));
				}
			}
			else
			{
				m_pSurface->Move(rcContent.left, rcContent.top, rcContent.right, rcContent.bottom);
				m_pSurface->ClipContent(&GetClipRect(DUIRT_CONTENT));
			}
		}
	}
}
#endif

void DuiNode::DelayedResize()
{
	// 在绝对定位情况下，元素无需真正请求父更新布局，仅需要更新滚动条
	if (GetPosition()>DUIPOS_RELATIVE)
	{
		__cache.hasOffset = FALSE;
		__cache.hasPos = __cache.hasClientPos = __cache.hasClipPos = FALSE;
		DuiNode* c = GetOffsetParent();
		if (c) c->UpdateScrollRange();
#ifndef NO3D
		Update3DSurface();
#endif
		return;
	}

	DuiNode* parent = GetNearestUpdateLayoutParent();
	if (parent == NULL) return;

	struct scb : public CDuiConnectorCallback
	{
	public:
		CSimpleArray<DuiNode*> ctrls;

		BOOL OnHit(LPVOID pClient, LPVOID pServer, DWORD_PTR protocol)
		{
			ctrls.Add((DuiNode*)pClient);
			return FALSE;
		}
	};
	scb _cb;
	gConnector.Fire(NULL, m_pLayoutMgr, cp_delayed_update_layout, &_cb);
	// 如果存在父，则不需要连接
	for (int i=0, num=_cb.ctrls.GetSize(); i<num; i++)
	{
		if (DuiNode::IsChild(_cb.ctrls[i], parent))
			return;
	}
	// 需要连接，要删除掉所有子
	for (int i=0, num=_cb.ctrls.GetSize(); i<num; i++)
	{
		if (DuiNode::IsChild(parent, _cb.ctrls[i]))
			gConnector.Disconnect(_cb.ctrls[i], m_pLayoutMgr, cp_delayed_update_layout);
	}
	// 连接
	struct scb2 : public CDuiConnectorCallback
	{
	public:
		BOOL OnHit(LPVOID pClient, LPVOID pServer, DWORD_PTR protocol)
		{
			DuiNode* ctrl = (DuiNode*)pClient;
			ctrl->UpdateLayout();
			gConnector.Disconnect(pClient, pServer, protocol);
			ctrl->Invalidate();
			return FALSE;
		}
	};
	static scb2 _cb2;
	gConnector.Connect(parent, m_pLayoutMgr, cp_delayed_update_layout, &_cb2);
	Invalidate();
}

/*
 *	一个POINT如果是相对于一个控件的，则表示它是以该控件的客户区左上角作为参照的
 *  注意，客户区不一定是可见的，可见区相当于一个窗口，只看到客户区的一部分内容，当一个控件的大小确定时，它的可见区就确定了
 *  可见区大小由窗口或者上级元素确定，客户区大小由下级元素确定
 *  如果 pSrc 或 pDst 的值是NULL，那么表示此 POINT 相对于容器窗口客户区的左上角，不是根元素
 */
void DuiNode::AdjustOffset(DuiNode* pSrc, DuiNode* pDst, POINT& pt)
{
	if (pSrc == pDst) return;
	
	// 如果 pSrc==NULL，表示 pt 相对于窗口客户区，需要首先转换成相对于根元素
	DuiNode* pRoot = NULL;
	if (pSrc==NULL)
	{
		pDst = pDst->GetOffsetParent(TRUE);
		//while (!CDuiControlExtension::IsBlock(pDst)) pDst = rt(pDst)->GetParent();

		// 首先把窗口相关的点坐标转换成根元素相关，并把 pSrc 直接替换成根元素
		pRoot = (pDst)->GetRoot();
		POINT pt2 = {0};
		(pRoot)->AdjustPoint(pt2); // 此时 pt2 应该变大，是 0 或者正值，表示相对于控件整体的左上角
		pt.x -= pt2.x;
		pt.y -= pt2.y;

		pt.x += pRoot->GetScrollbar(FALSE)->GetScrollPos();
		pt.y += pRoot->GetScrollbar()->GetScrollPos();
		if (pDst == pRoot) return;
		pSrc = pRoot;
	}
	// 内联元素跳过
	pSrc = pSrc->GetOffsetParent(TRUE);

	DuiNode* pDst2 = pDst;
	if (pDst2==NULL)
	{
		// 确保 pRoot 不为空
		if (pRoot == NULL) pRoot = (pSrc)->GetRoot();
		pDst2 = pRoot;
	}

	// 排除非相同窗口进行比较
	if ((pSrc)->m_pLayoutMgr != (pDst2)->m_pLayoutMgr) return;

	pDst2 = pDst2->GetOffsetParent(TRUE);

	// 现在开始，所有比较都是以元素客户区作为参照
	// child -> parent -> root
	do 
	{
		// 转换到相对于目标可见客户区原点
		pt.x -= pSrc->GetScrollbar(FALSE)->GetScrollPos();
		pt.y -= pSrc->GetScrollbar()->GetScrollPos();

		// 转换到父元素客户区左上角
		RECT rc = (pSrc)->GetPos();
		pt.x += rc.left;
		pt.y += rc.top;

		pSrc = pSrc->GetOffsetParent();
		if (pSrc == pDst2)
		{
			if (pDst == NULL)
			{
				(pRoot)->AdjustPoint(pt);
				pt.x -= pRoot->GetScrollbar(FALSE)->GetScrollPos();
				pt.y -= pRoot->GetScrollbar()->GetScrollPos();
			}
			return;
		}
	} while (pSrc);
	//CDuiControlExtensionBase* pParent = rt(pSrc)->GetParent();
	//while (pParent)
	//{
	//	while (!CDuiControlExtension::IsBlock(pParent)) pParent = rt(pParent)->GetParent();

	//	//// 必须首先把点坐标从客户区相关转换成整体相关
	//	//rt(pSrc)->AdjustPoint(pt);
	//	RECT rc = rt(pSrc)->GetPos(); // rc 是当前客户区相对于父元素客户区的矩形
	//	pt.x += rc.left;
	//	pt.y += rc.top;

	//	pt.x -= pParent->GetScrollbar(FALSE)->GetScrollPos();
	//	pt.y -= pParent->GetScrollbar()->GetScrollPos();
	//	if (pParent == pDst2)
	//	{
	//		// 是否还要转换到窗口客户区
	//		if (pDst == NULL)
	//		{
	//			rt(pRoot)->AdjustPoint(pt);
	//			//pt.x -= pRoot->GetScrollbar(FALSE)->GetScrollPos();
	//			//pt.y -= pRoot->GetScrollbar()->GetScrollPos();
	//		}
	//		return;
	//	}
	//	pSrc = pParent;
	//	pParent = rt(pParent)->GetParent();
	//}

	// 以目标元素客户区左上角作为基准点，转换成根元素客户区相关
	// root -> child
	POINT pt2 = {0}; // 这是目标元素客户区左上角
	do 
	{
		// 转换到相对于目标可见客户区原点
		pt2.x -= pDst2->GetScrollbar(FALSE)->GetScrollPos();
		pt2.y -= pDst2->GetScrollbar()->GetScrollPos();

		// 转换到父元素客户区左上角
		RECT rc = (pDst2)->GetPos();
		pt2.x += rc.left;
		pt2.y += rc.top;

		pDst2 = pDst2->GetOffsetParent();
	} while (pDst2);

	// 现在 pt 和 pt2 都是根元素客户区相关，做一个减法即可
	pt.x -= pt2.x;
	pt.y -= pt2.y;

	// 如果需要，转换回窗口客户区
	if (pDst == NULL)
	{
		(pRoot)->AdjustPoint(pt);
		pt.x -= pRoot->GetScrollbar(FALSE)->GetScrollPos();
		pt.y -= pRoot->GetScrollbar()->GetScrollPos();
	}
}

void DuiNode::AdjustOffset(DuiNode* pSrc, DuiNode* pDst, RECT& rc)
{
	POINT pt = {0};
	AdjustOffset(pSrc, pDst, pt);
	::OffsetRect(&rc, pt.x, pt.y);
}

POINT DuiNode::GetOffset(DuiNode* pSrc, DuiNode* pDst, POINT pt)
{
	return AdjustOffset(pSrc, pDst, pt), pt;
}

RECT DuiNode::GetOffset(DuiNode* pSrc, DuiNode* pDst, RECT rc)
{
	return AdjustOffset(pSrc, pDst, rc), rc;
}

RECT DuiNode::GetPosBy( RECT rcRefTotal, DUI_RECTTYPE rtp/* =DUIRT_CONTENT */)
{
	if (rtp==DUIRT_MARGIN)
		return rcRefTotal;

	CRect rc = rcRefTotal;
	if (__cache.hasPos)
	{
		LPCRECT prc = __cache.pos.rcPart;
		rc.DeflateRect(prc[rtp].left-prc[DUIRT_TOTAL].left, prc[rtp].top-prc[DUIRT_TOTAL].top, prc[DUIRT_TOTAL].right-prc[rtp].right, prc[DUIRT_TOTAL].bottom-prc[rtp].bottom);
		return rc;
	}

	// 除掉 margin
	//CDuiStyleVariant* pVar = GetStyleAttr(SAT_MARGIN);
	LPCRECT prc;
	if (GetPosition()==DUIPOS_DEFAULT && (prc=FromStyleVariant(GetStyleAttribute(SAT_MARGIN), LPCRECT)) != NULL)
		rc.DeflateRect(prc);
	if (rtp==DUIRT_BORDER)
		return rc;

	// 除掉 border
	DuiStyleVariant* sv = GetStyleAttribute(SAT_BORDER);
	LPLOGPEN plp = (sv ? FromStyleVariant(sv, LPLOGPEN) : NULL);
	if (plp && (sv->svt&SVT_PEN_MASK_WIDTH))
		rc.DeflateRect(plp->lopnWidth.x, plp->lopnWidth.x);
	else if (plp && (sv->svt&SVT_PEN_MASK_ALL)) // 没有设置宽度，但设置了其它项，则默认宽度是1
		rc.DeflateRect(1, 1);
	// 除掉滚动条
	rc.right -= GetScrollbar()->GetScrollbarWidth(); // scrollbar 在 PADDING 和 BORDER 之间
	rc.bottom -= GetScrollbar(FALSE)->GetScrollbarWidth();
	if (rtp==DUIRT_INSIDEBORDER)
		return rc;

	// 除掉外观样式
	if (GetVisual()) rc.DeflateRect(&GetVisual()->GetAdjustRect(DuiHandle<DuiNode>(this)));
	if (rtp==DUIRT_PADDING)
		return rc;

	// 除掉 padding
	if ((prc=FromStyleVariant(GetStyleAttribute(SAT_PADDING), LPCRECT)) != NULL)
		rc.DeflateRect(prc);
	return rc;
}

/*
 *	返回相对于窗口客户区的矩形
 */
RECT DuiNode::GetClientRect( DUI_RECTTYPE rtp/* =DUIRT_CONTENT */)
{
	if (!__cache.hasClientPos)
	{
		DuiNode* parent = GetOffsetParent();
		// client rect
		__cache.clientPos.rcPart[DUIRT_MARGIN] = GetOffset(parent, NULL, GetPos(DUIRT_MARGIN));
		__cache.clientPos.rcPart[DUIRT_BORDER] = GetOffset(parent, NULL, GetPos(DUIRT_BORDER));
		__cache.clientPos.rcPart[DUIRT_INSIDEBORDER] = GetOffset(parent, NULL, GetPos(DUIRT_INSIDEBORDER));
		__cache.clientPos.rcPart[DUIRT_PADDING] = GetOffset(parent, NULL, GetPos(DUIRT_PADDING));
		__cache.clientPos.rcPart[DUIRT_CONTENT] = GetOffset(parent, NULL, GetPos(DUIRT_CONTENT));
		__cache.hasClientPos = TRUE;
	}
	return __cache.clientPos.rcPart[rtp];
}

/*
 *	返回相对于窗口客户区的矩形
 */
RECT DuiNode::GetClipRect( DUI_RECTTYPE rtp/*=DUIRT_CONTENT*/)
{
	if (!__cache.hasClipPos)
	{
		CRect rc, rc2;
		DuiNode* parent = GetOffsetParent();
		if (parent) rc = parent->GetClipRect();
		else rc = m_pLayoutMgr->GetClientRect();
		__cache.clipPos.rcPart[DUIRT_MARGIN] = (rc2.IntersectRect(&rc, &GetClientRect((DUIRT_MARGIN))), rc2);
		__cache.clipPos.rcPart[DUIRT_BORDER] = (rc2.IntersectRect(&rc, &GetClientRect((DUIRT_BORDER))), rc2);
		__cache.clipPos.rcPart[DUIRT_INSIDEBORDER] = (rc2.IntersectRect(&rc, &GetClientRect((DUIRT_INSIDEBORDER))), rc2);
		__cache.clipPos.rcPart[DUIRT_PADDING] = (rc2.IntersectRect(&rc, &GetClientRect((DUIRT_PADDING))), rc2);
		__cache.clipPos.rcPart[DUIRT_CONTENT] = (rc2.IntersectRect(&rc, &GetClientRect((DUIRT_CONTENT))), rc2);
		__cache.hasClipPos = TRUE;
	}
	return __cache.clipPos.rcPart[rtp];
	//CRect rc;
	//if (r->GetParent()) rc = GetClipRect(r->GetParent());
	//else rc = r->m_pLayoutMgr->GetClientRect();
	//RECT rc2 = r->GetPos(rtp);
	//CDuiControlExtension::AdjustOffset(r->GetParent(), NULL, rc2);
	//rc.IntersectRect(&rc, &rc2);
	//return rc;
}

UINT DuiNode::GetControlFlags()
{
	LockReenter(OnGetControlFlags, 0);
	UINT flags = 0;
	InvokeOneSinkResult(flags, this, OnGetControlFlags);
	//ISinkOnGetControlFlags* gcf = CLASS_CAST(this, ISinkOnGetControlFlags);
	//if (gcf)
	//	flags = gcf->OnGetControlFlags(Handle());
	return flags | (UINT)FromStyleVariant(GetStyleAttribute(SAT_FLAG), LONG);
}

long DuiNode::GetZIndex()
{
	if (GetParent()==NULL) return 0;
	if (!__cache.hasZIndex)
	{
		__cache.hasZIndex = TRUE;
		CDuiStyleVariant* sv = (CDuiStyleVariant*)GetStyleAttribute(SAT_ZINDEX);
		__cache.offset.zindex = FromStyleVariant(sv, long);
	}
	return __cache.offset.zindex;
}

void DuiNode::UpdatePosition()
{
	DUI_POSITION pos = GetPosition();
	if (pos && !__cache.hasOffset)
	{
		__cache.hasOffset = TRUE;
		DuiNode* parent = GetOffsetParent();
		CRect rc;
		if (parent) rc = (parent)->GetPos();
		else ::GetClientRect(m_pLayoutMgr->GetPaintWindow(), &rc);
		rc.MoveToXY(0,0);
		if (pos==DUIPOS_RELATIVE)
			rc.MoveToXY(m_rcItem.left, m_rcItem.top);

		SIZE sz = rc.Size();
		// w/h 如果未设置就当作默认的 auto，l/r/t/b 如果未设置就当作默认值0
		long l=0,r=0,t=0,b=0,w=0,h=0;
		l = FromStyleVariant(GetStyleAttribute(SAT_LEFT),	long, (LPVOID)(LONG_PTR)sz.cx, 0, AutoValue);
		r = FromStyleVariant(GetStyleAttribute(SAT_RIGHT),	long, (LPVOID)(LONG_PTR)sz.cx, 0, AutoValue);
		w = FromStyleVariant(GetStyleAttribute(SAT_WIDTH),	long, (LPVOID)(LONG_PTR)sz.cx, 0, AutoValue);
		if (w==0 || w==AutoValue) w = FromStyleVariant(GetStyleAttribute(SAT_MINWIDTH), long, (LPVOID)(LONG_PTR)sz.cx, 0, AutoValue);
		//if (w==0) w = AutoValue;

		t = FromStyleVariant(GetStyleAttribute(SAT_TOP),		long, (LPVOID)(LONG_PTR)sz.cy, 0, AutoValue);
		b = FromStyleVariant(GetStyleAttribute(SAT_BOTTOM),	long, (LPVOID)(LONG_PTR)sz.cy, 0, AutoValue);
		h = FromStyleVariant(GetStyleAttribute(SAT_HEIGHT),	long, (LPVOID)(LONG_PTR)sz.cy, 0, AutoValue);
		if (h==0 || h==AutoValue) h = FromStyleVariant(GetStyleAttribute(SAT_MINHEIGHT), long, (LPVOID)(LONG_PTR)sz.cy, 0, AutoValue);
		//if (h==0) h = AutoValue;

		// l-r-w 通常最多只能一个AUTO值，如果多于一个，按优先顺序取默认值
		long autoNum = 0;
		if (l==AutoValue) autoNum++;
		if (r==AutoValue) autoNum++;
		if (w==AutoValue) autoNum++;
		if (autoNum==0)
		{
			__cache.offset.rcOffset.left = rc.left + l;
			__cache.offset.rcOffset.right = __cache.offset.rcOffset.left + w; // don't use r
		}
		else if (autoNum==1)
		{
			if (l==AutoValue)
			{
				__cache.offset.rcOffset.right = rc.right - r;		// 注意 r 是相对于右边的
				__cache.offset.rcOffset.left = __cache.offset.rcOffset.right - w;
			}
			else if (r==AutoValue)
			{
				__cache.offset.rcOffset.left = rc.left + l;
				__cache.offset.rcOffset.right = __cache.offset.rcOffset.left + w;
			}
			else // if (w==AutoValue)
			{
				__cache.offset.rcOffset.left = rc.left + l;
				__cache.offset.rcOffset.right = rc.right - r;
			}
		}
		else if (autoNum==2)
		{
			if (l!=AutoValue) // left only, use width of m_rcItem
			{
				__cache.offset.rcOffset.left = rc.left + l;
				__cache.offset.rcOffset.right = __cache.offset.rcOffset.left + (m_rcItem.right-m_rcItem.left);
			}
			else if (r!=AutoValue) // right only, use width of m_rcItem
			{
				__cache.offset.rcOffset.right = rc.right - r;
				__cache.offset.rcOffset.left = __cache.offset.rcOffset.right - (m_rcItem.right-m_rcItem.left);
			}
			else // if (w!=AutoValue) // width only, use left 0
			{
				__cache.offset.rcOffset.left = rc.left;
				__cache.offset.rcOffset.right = __cache.offset.rcOffset.left + w;
			}
		}
		else // if (autoNum==3)
		{
			__cache.offset.rcOffset.left = rc.left;
			__cache.offset.rcOffset.right = __cache.offset.rcOffset.left + (m_rcItem.right-m_rcItem.left);
		}

		// l-r-w 通常最多只能一个AUTO值，如果多于一个，按优先顺序取默认值
		autoNum = 0;
		if (t==AutoValue) autoNum++;
		if (b==AutoValue) autoNum++;
		if (h==AutoValue) autoNum++;
		if (autoNum==0)
		{
			__cache.offset.rcOffset.top = rc.top + t;
			__cache.offset.rcOffset.bottom = __cache.offset.rcOffset.top + h; // don't use b
		}
		else if (autoNum==1)
		{
			if (t==AutoValue)
			{
				__cache.offset.rcOffset.bottom = rc.bottom - b;		// 注意 b 是相对于底部的
				__cache.offset.rcOffset.top = __cache.offset.rcOffset.bottom - h;
			}
			else if (b==AutoValue)
			{
				__cache.offset.rcOffset.top = rc.top + t;
				__cache.offset.rcOffset.bottom = __cache.offset.rcOffset.top + h;
			}
			else // if (h==AutoValue)
			{
				__cache.offset.rcOffset.top = rc.top + t;
				__cache.offset.rcOffset.bottom = rc.bottom - b;
			}
		}
		else if (autoNum==2)
		{
			if (t!=AutoValue) // top only, use height of m_rcItem
			{
				__cache.offset.rcOffset.top = rc.top + t;
				__cache.offset.rcOffset.bottom = __cache.offset.rcOffset.top + (m_rcItem.bottom-m_rcItem.top);
			}
			else if (b!=AutoValue) // bottom only, use height of m_rcItem
			{
				__cache.offset.rcOffset.bottom = rc.bottom - b;
				__cache.offset.rcOffset.top = __cache.offset.rcOffset.bottom - (m_rcItem.bottom-m_rcItem.top);
			}
			else // if (h!=AutoValue) // height only, use top 0
			{
				__cache.offset.rcOffset.top = rc.top;
				__cache.offset.rcOffset.bottom = __cache.offset.rcOffset.top + h;
			}
		}
		else // if (autoNum==3)
		{
			__cache.offset.rcOffset.top = rc.top;
			__cache.offset.rcOffset.bottom = __cache.offset.rcOffset.top + (m_rcItem.bottom-m_rcItem.top);
		}
	}
}

/*
 *	返回相对于 parent 的矩形
 */
RECT DuiNode::GetPos(DUI_RECTTYPE rtp/*=DUIRT_CONTENT*/)
{
	RECT __rc;
	DelegateIOC(GetPos, (this, rtp, &__rc), __rc);

	//DUI_POSITION pos = CDuiControlExtension::GetPosition(_ctrl);
	UpdatePosition();
	if (m_hRgn || !m_pLayoutMgr) return __cache.hasOffset ? __cache.offset.rcOffset : m_rcItem;
	CRect rc = (__cache.hasOffset) ? __cache.offset.rcOffset : m_rcItem;
	if (__cache.tryContentWidth > 0)
	{
		SIZE sz = {__cache.tryContentWidth, 0}; // 如果流式布局中存在内联块，这个块的初始高度怎么计算又不导致无限嵌套调用呢？
		sz = AdjustSize(sz);
		rc.right = rc.left + sz.cx; // 试算中采用固定宽度。注意，_calcWidth 指的是内容宽度，不是整体宽度
	}
	if (!IsBlock()) return rc;
	if (__cache.tryContentWidth > 0) return GetPosBy(rc, rtp);
	if (!__cache.hasPos)
	{
		__cache.pos.rcPart[DUIRT_MARGIN] = rc;
		__cache.pos.rcPart[DUIRT_BORDER] = GetPosBy(rc, DUIRT_BORDER);
		__cache.pos.rcPart[DUIRT_INSIDEBORDER] = GetPosBy(rc, DUIRT_INSIDEBORDER);
		__cache.pos.rcPart[DUIRT_PADDING] = GetPosBy(rc, DUIRT_PADDING);
		__cache.pos.rcPart[DUIRT_CONTENT] = GetPosBy(rc, DUIRT_CONTENT);
		__cache.hasPos = TRUE;
	}
	return __cache.pos.rcPart[rtp];
}

/*
 *	rc 是相对于 parent 的矩形
 */
void DuiNode::SetPos(RECT rc)
{
	DelegateIOC(SetPos, (this, rc), );

	// rc 是相对于父元素的相对位置
	//trace_c(__FUNCTION__);
	//ATLTRACE(L"%s SetPos(%d,%d,%d,%d)\n", GetName(), rc.left, rc.top, rc.right, rc.bottom);
	//if (GetParent()==NULL)
	//	ATLTRACE(L"root's SetPos()\n");
	CRect rcOrig = (m_rcItem);
	BOOL bChanged = !::EqualRect(&rcOrig, &rc);
	m_rcItem = rc;
	if (bChanged)
	{
		SetRectDirty();
		__cache.hasOffset = FALSE;
	}

#ifndef NO3D
	// 更新3D表面
	if (m_pLayoutMgr->Get3DDevice())
	{
		Update3DSurface();
	}

	if (m_pSurface==NULL)
		__cache.imgBkgnd.Clear();
#endif
	//m_bNeedUpdateRect = FALSE;
	//BOOL bSizeChanged = (rcOrig.Width()!=rc.right-rc.left || rcOrig.Height()!=rc.bottom-rc.top);

	//if (bChanged)
	//{
	//	RT(this,m_rcItem) = rc;
	//	if (bSizeChanged)
	//	{
	//		SetRectDirty();
	//		m_bNeedUpdateRect = FALSE;
	//		UpdateLayout(this);
	//	}
	//	else
	//	{
	//		POINT ptOffset = {rc.left-rcOrig.left, rc.top-rcOrig.top};
	//		UpdateLayout(this, &ptOffset);
	//	}
	//}
	//if (IsBlock(this))
	//	GetScrollbar()->SetScrollRange(rcOrig.Height()/*pCtrl->GetScrollbar()->SetScrollRange()*/);
	//Invalidate();
	//if (bChanged) CDuiControlExtension::UpdateLayout(_ctrl);
#ifdef GDI_ONLY
	if (bChanged && (_dxt))
	{
		DxtContext ctx;
		ctx._dc = m_pLayoutMgr->GetPaintDC();
		ctx._rc = DuiNode::GetOffset(GetOffsetParent(), NULL, m_rcItem);
		ctx._rcClip = GetClipRect();
		ctx._data = this;
		ctx._transformProc = DuiNode::TransformProc;
		ctx._drawToDC = DuiNode::DrawToDC;
		ctx._drawBkToDC = DuiNode::DrawBackgroundToDC;
		_dxt->Init(ctx);
	}
#else
	if (bChanged)
	{
		SetFilterProps ();
	}
#endif // GDI_ONLY

	//if (bChanged)
	{
		// 这将导致 UpdateLayout 被调用
		DuiEvent event = { (DUI_EVENTTYPE)0 };
		event.Name = event_type_to_name(DUIET_size);
		event.dwTimestamp = ::GetTickCount();
		event.cancelBubble = TRUE;
		m_pLayoutMgr->FireEvent(this, event);
	}

	//// 现在更新滚动条。滚动条更新应该放在 UpdateLayout之后调用，因为子控件的位置都在这里被计算，只有位置确定之后才能正确修复滚动条状态
	if (IsAllowScroll())
	{
		SIZE sz = GetContentSize();
		SetScrollRange(sz.cx, sz.cy);
	}

	// 更新3D表面
	//if (m_pSurface) Do3DPaint();

	//if (CDuiControlExtension::IsAllowScroll(_ctrl))
	//{
	//	SIZE sz = {0};
	//	for (int i=0; i<m_children.GetSize(); i++)
	//	{
	//		CDuiControlExtensionBase* child = GetChildControl(i);
	//		sz.cx = max(sz.cx, rt(child)->m_rcItem.right);
	//		sz.cy = max(sz.cy, rt(child)->m_rcItem.bottom);
	//	}
	//	CDuiControlExtension::SetScrollRange(_ctrl, sz.cx, sz.cy);
	//}
}

RECT DuiNode::AdjustRect(const RECT rcContent, DUI_RECTTYPE rtDst/* =DUIRT_TOTAL */)
{
	if (rtDst>DUIRT_TOTAL || rtDst<DUIRT_CONTENT) rtDst = DUIRT_TOTAL;

	if (rtDst == DUIRT_CONTENT) return rcContent;

	CRect rc = rcContent;
	if (__cache.hasPos)
	{
		LPCRECT prc = __cache.pos.rcPart;
		rc.DeflateRect(prc[rtDst].left-prc[DUIRT_CONTENT].left, 
						prc[rtDst].top-prc[DUIRT_CONTENT].top, 
						prc[DUIRT_CONTENT].right-prc[rtDst].right, 
						prc[DUIRT_CONTENT].bottom-prc[rtDst].bottom);
		return rc;
	}

	LPCRECT prc;
	// 增加padding
	if ((prc=FromStyleVariant(GetStyleAttribute(SAT_PADDING), LPCRECT)) != NULL)
		rc.InflateRect(prc);
	if (rtDst == DUIRT_PADDING) return rc;

	// 增加视觉样式
	if (__cache.visual) rc.InflateRect(&__cache.visual->GetAdjustRect(DuiHandle<DuiNode>(this)));
	if (rtDst == DUIRT_INSIDEBORDER) return rc;

	// 因为这只是估算，不需要额外添加滚动条所占空间
	// 增加border
	DuiStyleVariant* sv = GetStyleAttribute(SAT_BORDER);
	LPLOGPEN plp = (sv ? FromStyleVariant(sv, LPLOGPEN) : NULL);
	if (plp && (sv->svt&SVT_PEN_MASK_WIDTH))
		rc.InflateRect(plp->lopnWidth.x, plp->lopnWidth.x);
	else if (plp && (sv->svt&SVT_PEN_MASK_ALL)) // 没有设置宽度，但设置了其它项，则默认宽度是1
		rc.InflateRect(1, 1);
	if (rtDst == DUIRT_BORDER) return rc;

	// 增加margin
	if (GetPosition()==DUIPOS_DEFAULT && (prc=FromStyleVariant(GetStyleAttribute(SAT_MARGIN), LPCRECT)) != NULL)
		rc.InflateRect(prc);
	return rc;
}

SIZE DuiNode::AdjustSize(const SIZE szContent, DUI_RECTTYPE rtDst/* =DUIRT_TOTAL */)
{
	CRect rc(CPoint(0,0), szContent);
	rc = AdjustRect(rc, rtDst);
	return rc.Size();
}

POINT DuiNode::AdjustPoint(POINT& ptContent, DUI_RECTTYPE rtDst/* =DUIRT_TOTAL */)
{
	CRect rc(0,0,0,0);
	rc = AdjustRect(rc, rtDst);
	ptContent.x += -rc.left;
	ptContent.y += -rc.top;
	return ptContent;
}

void DuiNode::Invalidate()
{
	DuiNode* r = GetOffsetParent(TRUE);
#ifndef NO3D
	if (r && r->m_pSurface)
	{
		//r->Do3DPaint();
		r->Update3DState();
		//CDuiControlExtension::Update3DSurface(_ctrl);
		r->m_pSurface->Invalidate();
		DuiNode* rp = GetOffsetParent();
		if (rp)
		{
			if (rp->__cache.updateScroll)
				rp->m_pSurface->Invalidate();
		}
		return;
	}
#endif

	if ( m_pLayoutMgr && (((__cache.disabled) & DUIDISABLED_INVALIDATE)==0) )
	{
		RECT rc = GetPos(DUIRT_TOTAL);
		DuiNode::AdjustOffset(r, NULL, rc);
		m_pLayoutMgr->Invalidate(&rc);
	}
}

void DuiNode::SetRectDirty()
{
	__cache.hasPos = __cache.hasClientPos = __cache.hasClipPos = FALSE;

	for (int i=0; i<m_children.GetSize(); i++)
	{
		DuiNode* pCtrl = (DuiNode*)m_children[i];
		if (!(pCtrl)->IsVisible()) continue;
		(pCtrl)->SetRectDirty();
	}
}

//void CDuiControlExtension::UpdateOffset(CDuiControlExtensionBase* pCtrl, LPPOINT pptOffset)
//{
//	return;
//	if (pCtrl==NULL || pptOffset==NULL || (pptOffset->x==0 && pptOffset->y==0)) return;
//
//	rt rtCtrl(pCtrl);
//	::OffsetRect(&rtCtrl->m_rcItem, pptOffset->x, pptOffset->y);
//	if (rtCtrl->m_hRgn) ::OffsetRgn(rtCtrl->m_hRgn, pptOffset->x, pptOffset->y);
//
//	for (int i=0; i<rtCtrl->GetChildCount(); i++) UpdateOffset(rtCtrl->GetChildControl(i), pptOffset);
//}

SIZE DuiNode::GetContentSize()
{
	struct DefCallback
	{
		static DuiVoid OnGetContentSize(CallContext* pcc, HDE hde, LPSIZE psz)
		{
			DuiNode* n = DuiNode::FromHandle(hde);
			SIZE& sz = *psz;
			if ((n->GetControlFlags()&DUIFLAG_NOCHILDREN)) return 0;

			for (int i=0; i<n->GetChildCount(); i++)
			{
				DuiNode* child = n->GetChildControl(i);
				if (child->IsVisible() && child->GetPosition()==DUIPOS_DEFAULT)
				{
					sz.cx = max(sz.cx, child->m_rcItem.right);
					sz.cy = max(sz.cy, child->m_rcItem.bottom);
				}
			}
			if (FromStyleVariant(n->GetStyleAttribute(SAT_OVERFLOW_INCLUDE_OFFSET), BOOL))
			{
				for (DuiNode* rr=n->m_offsetChildren; rr; rr=rr->next)
				{
					if (rr->IsVisible())
					{
						sz.cx = max(sz.cx, rr->__cache.offset.rcOffset.right);
						sz.cy = max(sz.cy, rr->__cache.offset.rcOffset.bottom);
					}
				}
			}
			return 0;
		}
	};
	//ISinkOnGetContentSize* pSink = CLASS_CAST(this, ISinkOnGetContentSize);
	//if (pSink)
	//{
	//	SIZE sz={0};
	//	pSink->OnGetContentSize(Handle(), &sz);
	//	return sz;
	//}

	LockReenter(OnGetContentSize, CSize(0,0));
	SIZE sz = {0};
	BOOL bHasInvocation = FALSE;
	InvokeOneSink(this, OnGetContentSize, &sz);
	if (bHasInvocation)
		return sz;

	return DefCallback::OnGetContentSize(NULL, Handle(), &sz), sz;
}

void DuiNode::UpdateLayout(DuiNode* pFromChild/*=NULL*/)
{
	//trace_c(__FUNCTION__); trace_rect();
	DWORD dwDisabled = ((pFromChild && pFromChild!=this) ? DUIDISABLED_UPDATELAYOUT_FROMCHILD : DUIDISABLED_UPDATELAYOUT);

	// 往上寻找真正需要UpdateLayout的元素
	DuiNode* r = this;
	if (pFromChild)
		r = pFromChild->GetNearestUpdateLayoutParent();

	if (GetControlFlags()&DUIFLAG_NOCHILDREN) return;

	if (r->__cache.disabled & dwDisabled) return;

	DisableOp dop(r, dwDisabled); // 防止在 UpdateLayout 期间被 OnChildStateChanged 通知导致的重入

	//SIZE szOld = CRect(rtCtrl->m_rcItem).Size();
	//SIZE szStyle = {(long)*pCtrl->GetStyleAttribute(SAT_WIDTH), (long)*pCtrl->GetStyleAttribute(SAT_HEIGHT)};

	//BOOL bSizeChanged = FALSE;
	// 这里可能需要多次尝试，因为滚动条可视状态可能发生了变化，导致必须重新计算布局

	LockReenter(OnUpdateLayout);

	//SIZE szCalc = {0};
	BOOL bDone = FALSE;
	BOOL bAllowScroll = r->IsAllowScroll();
	BOOL bNeedUpdateScroll = TRUE;
	if (bAllowScroll) r->SetScrollRange(0, 0);

	// 循环不能超过3次
	// 
	int num=0;
	do
	{
		if (GetLayout()==NULL || !(bDone = GetLayout()->OnUpdateLayout(Handle())))
		{
			InvokeAllSinksBool(r, OnUpdateLayout);
		}

		if (!bDone || !bAllowScroll) // 没有布局样式或者没有滚动条，则无需重新计算布局
			break;

		SIZE sz = r->GetContentSize();
		if (!r->SetScrollRange(sz.cx, sz.cy)) // TRUE表示某个滚动条可视状态发生了改变
		{
			bNeedUpdateScroll = FALSE;
			break;
		}

		if (num==2) break;
		num++;
	} while (1);

	if (!bDone)
	{
		CRect rcTotal = r->GetPos(DUIRT_TOTAL);
		//DisableOp dop2(pCtrl, DUIDISABLED_UPDATELAYOUT_FROMCHILD);
		SIZE sz = rcTotal.Size();
		/*szCalc =*/ r->EstimateSize(sz);
	}

	// 现在更新滚动条。滚动条更新应该放在 UpdateLayout之后调用，因为子控件的位置都在这里被计算，只有位置确定之后才能正确修复滚动条状态
	if (bAllowScroll && bNeedUpdateScroll)
	{
		SIZE sz = r->GetContentSize();
		r->SetScrollRange(sz.cx, sz.cy);
	}

#ifndef NO3D
	if (r->m_pSurface)
	{
		// TODO: paint 3D content
		r->Update3DState();
		//r->Do3DPaint();
		r->m_pSurface->Invalidate();
		return;
	}
#endif

	if ((r->__cache.disabled & DUIDISABLED_INVALIDATE) == 0)
		r->Invalidate();
}

DUI_POSITION DuiNode::GetPosition()
{
	if (GetParent()==NULL) // 根元素只能是默认位置样式
		return __cache.offset.parent=NULL, DUIPOS_DEFAULT;
	if (!__cache.hasPosition)
	{
		__cache.hasPosition = TRUE;
		CDuiStyleVariant* sv = (CDuiStyleVariant*)GetStyleAttribute(SAT_POSITION);
		__cache.offset.position = (DUI_POSITION)FromStyleVariant(sv, long);

		// 同时更新offset.parent
		if (__cache.offset.position==DUIPOS_FIXED)
			__cache.offset.parent = NULL;
		else
		{
			DuiNode* pRoot = GetRoot();
			DuiNode* parent = GetParent();
			while (parent && !parent->IsBlock()) parent = (parent)->GetParent();
			if (__cache.offset.position==DUIPOS_ABSOLUTE && (parent==NULL || parent->GetPosition()<=DUIPOS_RELATIVE))
				__cache.offset.parent=(pRoot);
			else
				__cache.offset.parent=(parent);
		}
	}
	return __cache.offset.position;
}

//LONG CDuiControlExtension::GetZIndex(CDuiControlExtensionBase* pCtrl)
//{
//	if (pCtrl)
//	{
//		DuiNode* r = rt(pCtrl);
//		if (r->GetParent()==NULL) return 0;
//		if (!r->__cache.hasZIndex)
//		{
//			r->__cache.hasZIndex = TRUE;
//			CDuiStyleVariant* sv = (CDuiStyleVariant*)pCtrl->GetStyleAttribute(SAT_ZINDEX);
//			r->__cache.offset.zindex = FromStyleVariant(sv, long);
//		}
//		return r->__cache.offset.zindex;
//	}
//	return 0;
//}

BOOL DuiNode::IsBlock()
{
	if (!__cache.hasBlock)
	{
		do
		{
			if (GetLayout() || GetVisual()) {__cache.displayBlock=TRUE; break;} // 一个控件如果有布局模式或者有视觉样式，控件就是BLOCK
			if (GetPosition()>DUIPOS_DEFAULT) {__cache.displayBlock=TRUE; break;} // 一个控件如果不是默认定位，控件就是BLOCK
#ifndef NO3D
			if (m_pScene) {__cache.displayBlock=TRUE; break;} // 一个控件是一个3D场景，控件就是BLOCK
#endif
			DuiNode* parent = GetParent();
			if (parent==NULL) {__cache.displayBlock=TRUE; break;} // 根节点是BLOCK
			if (parent->GetLayout()) {__cache.displayBlock=TRUE; break;} // 如果父控件是一种布局控件，那么子控件都是BLOCK
			LONG display = FromStyleVariant(GetStyleAttribute(SAT_DISPLAY), LONG);
			__cache.displayBlock = ((display&DUIDISPLAY_BLOCK)!=0);
		} while(0);

		__cache.hasBlock = TRUE;
	}
	return __cache.displayBlock;

	//if (r->GetLayout() || r->GetVisual()) return TRUE; // 一个控件如果有布局模式或者有视觉样式，控件就是BLOCK
	//CDuiControlExtensionBase* parent = r->GetParent();
	//if (parent==NULL) return TRUE; // 根节点是BLOCK
	//if (rt(parent)->GetLayout()) return TRUE; // 如果父控件是一种布局控件，那么子控件都是BLOCK
	//LONG display = FromStyleVariant(pCtrl->GetStyleAttribute(SAT_DISPLAY), LONG);
	//return (display&DUIDISPLAY_BLOCK)!=0;
}

BOOL DuiNode::IsInline()
{
	if (!__cache.hasInline)
	{
		do
		{
			DuiNode* parent = GetParent();
			if (parent==NULL) {__cache.displayInline = FALSE; break;} // 根节点是BLOCK
			if (GetPosition()>DUIPOS_RELATIVE) {__cache.displayInline = FALSE; break;} // 绝对位置和固定位置的控件是BLOCK
			if (parent->GetLayout()) {__cache.displayInline=FALSE;break;} // 如果父控件是一种布局控件，那么子控件都是BLOCK
			LONG display = FromStyleVariant(GetStyleAttribute(SAT_DISPLAY), LONG);
			__cache.displayInline = (display==0 || (display&DUIDISPLAY_INLINE)!=0);
		} while(0);

		__cache.hasInline = TRUE;
	}
	return __cache.displayInline;

	//CDuiControlExtensionBase* parent = r->GetParent();
	//if (parent==NULL) return FALSE; // 根节点是BLOCK
	//if (rt(parent)->GetLayout()) return FALSE; // 如果父控件是一种布局控件，那么子控件都是BLOCK
	//LONG display = FromStyleVariant(pCtrl->GetStyleAttribute(SAT_DISPLAY), LONG);
	//return display==0 || (display&DUIDISPLAY_INLINE)!=0;
}

BOOL DuiNode::IsInlineBlock()
{
	return IsBlock() && IsInline();
}

BOOL DuiNode::IsAllowScroll()
{
	if (!IsBlock()) return FALSE;
	LONG overflow = FromStyleVariant(GetStyleAttribute(SAT_OVERFLOW), LONG);
	return (overflow != OVERFLOW_HIDDEN) ? (overflow==OVERFLOW_SCROLL ? -1 : TRUE) : FALSE;
}

BOOL DuiNode::SetScrollRange(LONG cx, LONG cy)
{
	if (IsAllowScroll())
	{
		BOOL b1 = GetScrollbar(FALSE)->SetScrollRange(cx);
		BOOL b2 = GetScrollbar()->SetScrollRange(cy);
		return b1 || b2;
	}
	return FALSE;
}

void DuiNode::OnScrollPosChanged(BOOL bVert/*=TRUE*/, DuiNode* pParent/* =NULL */)
{
	LockReenter(OnScroll);

	if (pParent == NULL)
		pParent = this;
	else
		SetRectDirty();

	InvokeOneSink(this, OnScroll, pParent->Handle(), bVert);
	//ISinkOnScroll* p = CLASS_CAST(this, ISinkOnScroll);
	//if (p)
	//	p->OnScroll(Handle(), pParent->Handle(), bVert);

	if ((GetControlFlags()&DUIFLAG_NOCHILDREN) == 0)
	{
		for (int i=0, num=GetChildCount(); i<num; i++)
		{
			GetChildControl(i)->OnScrollPosChanged(bVert, pParent);
		}
	}

#ifndef NO3D
	if (m_pSurface && pParent!=this)
	{
		RECT rc = GetClientRect(DUIRT_BORDER);
		m_pSurface->Move(&rc);
		m_pSurface->ClipContent(&GetClipRect(DUIRT_CONTENT));
		if (pParent == this)
		{
			// TODO: Paint 3D content
			//r->Do3DPaint();
		}
	}
#endif
}

LONG DuiNode::CalcContentMinWidth()
{
	SIZE sz = GetOffsetParentSize();
	LONG width = FromStyleVariant(GetStyleAttribute(SAT_WIDTH), LONG, (LPVOID)sz.cx);
	if (width == 0)
		width = FromStyleVariant(GetStyleAttribute(SAT_MINWIDTH), LONG, (LPVOID)sz.cx);

	LONG cw = 0;
	if (width > 0)
	{
		CRect rc(0, 0, width, 0);
		rc = GetPosBy(rc);
		cw = rc.Width();
	}

	if ((GetControlFlags()&DUIFLAG_NOCHILDREN) == 0)
	{
		for (int i=0, num=GetChildCount(); i<num; i++)
		{
			DuiNode* child = GetChildControl(i);
			if ((child)->IsVisible() && !child->IsAllowScroll())
				cw = max(cw, child->GetMinWidth());
		}
	}
	return cw;
}

LONG DuiNode::GetMinWidth()
{
	LONG cx = GetOffsetParentSize().cx;
	LONG width = FromStyleVariant(GetStyleAttribute(SAT_WIDTH), LONG, (LPVOID)cx);
	if (width > 0) return width;
	width = FromStyleVariant(GetStyleAttribute(SAT_MINWIDTH), LONG, (LPVOID)cx);
	if (width > 0) return width;

	SIZE sz = {/*CDuiControlExtension::IsAllowScroll(pCtrl)?0:*/CalcContentMinWidth(),0};
	if (IsBlock())
		sz = AdjustSize(sz);
	return sz.cx;
}

LONG DuiNode::CalcContentHeight(LONG width)
{
	//trace_c(__FUNCTION__);
	if (width <= 0) return 0;

#ifdef GDI_ONLY
	CDCHandle dc = m_pLayoutMgr->GetPaintDC();
#else
	DuiNode* parent = GetOffsetParent();
	CSurfaceDC dc(
#ifndef NO3D
		m_pSurface ||
#endif
		parent==NULL
#ifndef NO3D
		|| (parent)->m_pSurface
#endif
		? NULL : m_pLayoutMgr->GetSurface());
#endif // GDI_ONLY
	
	// 试算时要设置这个固定宽度
	__cache.tryContentWidth = width;

	CRect rc = GetPos();
	rc.MoveToXY(0,0);
	//rc.right = rc.left + width;
	//rtCtrl->SetPos(rc);
	//rc.OffsetRect(0, m_scrollBar.GetScrollPos());
	int iOldCtx = ::SaveDC(dc.m_hDC);
	//PrepareDC(dc);

	InDrawData idd = {0};
	DuiNode::RowItems row;
	idd.pvReserved = &row;
	InitDrawData(dc.m_hDC, idd);
	//idd.rc.right = idd.rc.left + width;
	//rc.right = rc.left + width;
	//pCtrl->DoPaint(dc, rc, FALSE, idd);
	OnPaintContent(dc.m_hDC, rc, FALSE, idd);

	//idd.cyTotalHeight = idd.pt.y - idd.rc.top;
	UpdateDrawData(idd);
	DuiNode::ReposInlineRow(idd); // 这一句用来调整水平居中或右对齐的偏移量
	if ((GetControlFlags()&DUIFLAG_NOCHILDREN) == 0)
	{
		for (int i=0, num=GetChildCount(); i<num; i++)
		{
			GetChildControl(i)->UpdateInlineRegion();
		}
	}

	LONG h = idd.cyTotalHeight + ((idd.pt.x>idd.lLineIndent) ? idd.cyMinHeight : 0);

	// 通过滚动条的 CustomPos 属性来设置垂直方向的偏移值，模拟垂直居中的效果。注意，值应该为负值。
	if ((rc.Height()>h))
	{
		if ((idd.uTextStyle&DT_VCENTER))
			m_scrollBar.SetCustomPos(-(rc.Height() - h)/2);
		else if ((idd.uTextStyle&DT_BOTTOM))
			m_scrollBar.SetCustomPos(-(rc.Height() - h));
		else
			m_scrollBar.SetCustomPos(0);
	}
	else
		m_scrollBar.SetCustomPos(0);

	// 试算结束必须复位固定宽度
	__cache.tryContentWidth = 0;

	::RestoreDC(dc.m_hDC, iOldCtx);
	return h;
}

SIZE DuiNode::EstimateContentSize( SIZE szAvailable )
{
	//trace_c(__FUNCTION__); trace_rect();
	LONG mincx = CalcContentMinWidth();
	BOOL bAllowScroll = IsAllowScroll();

	// 先取消滚动条
	SetScrollRange(0, 0);
	
	SIZE sz = {0};
	sz.cx = max(szAvailable.cx, mincx);//rc.Width();
	// 首先更新水平滚动条
	//if (bAllowScroll) pCtrl->GetScrollbar(FALSE)->SetScrollRange(sz.cx);
	sz.cy = CalcContentHeight(sz.cx);

	if (szAvailable.cx>mincx && szAvailable.cy>0 && sz.cy > szAvailable.cy && bAllowScroll /*&& pCtrl->GetScrollbar()->GetScrollbarWidth()==0*/) // 需要滚动条
	{
		sz.cx -= CDuiLayoutManager::GetSystemMetrics().cxvscroll;
		sz.cx = max(sz.cx, mincx);
		//pCtrl->GetScrollbar(FALSE)->SetScrollRange(sz.cx); // 首先更新水平滚动条
		sz.cy = CalcContentHeight(sz.cx);
	}
	//if (/*szAvailable.cy>0 &&*/ bAllowScroll)
	//{
	//	pCtrl->GetScrollbar()->SetScrollRange(sz.cy);
	//}

	return sz;
}

SIZE DuiNode::EstimateSize(SIZE szAvailable)
{
	SIZE __sz;
	DelegateIOC(EstimateSize, (this, szAvailable, &__sz), __sz);

	//trace_c(__FUNCTION__); trace_rect();
	SIZE szParent = GetOffsetParentSize();
	SIZE sz = { FromStyleVariant(GetStyleAttribute(SAT_WIDTH), LONG, (LPVOID)(LONG_PTR)szParent.cx), FromStyleVariant(GetStyleAttribute(SAT_HEIGHT), LONG, (LPVOID)(LONG_PTR)szParent.cy)};
	SIZE szMin = { FromStyleVariant(GetStyleAttribute(SAT_MINWIDTH), LONG, (LPVOID)(LONG_PTR)szParent.cx), FromStyleVariant(GetStyleAttribute(SAT_MINHEIGHT), LONG, (LPVOID)(LONG_PTR)szParent.cy)};
	SIZE sz1 = sz;
	if (sz1.cx == 0) sz1.cx = szAvailable.cx;
	if (sz1.cy == 0) sz1.cy = szAvailable.cy;

	// 先取消滚动条
	SetScrollRange(0, 0);

	CRect rc(CPoint(0,0), sz1);
	rc = GetPosBy(rc); // TOTAL ==> CONTENT

	SIZE szCalc = rc.Size();
	SIZE szContent = EstimateContentSize(szCalc);
	SIZE sz2 = AdjustSize(szContent);
	if (sz.cx == 0) sz.cx = max(sz2.cx, szMin.cx);
	if (sz.cy == 0) sz.cy = max(sz2.cy, szMin.cy);
	return sz;
}

void DuiNode::InitDrawData(HDC hDC, InDrawData& idd)
{
	//CDCHandle dc(hDC);
	//int iOld = dc.SaveDC();
	PrepareDC(hDC);

	idd.owner = DuiHandle<DuiNode>(this);

	CRect rc = GetPos();
	rc.MoveToXY(0,0);
	//rc.top -= m_scrollBar.GetScrollPos();
	idd.rc = rc;
	idd.pt.x = rc.left;
	idd.pt.y = rc.top;
	if ((m_hRgn))
	{
		DWORD dwSize = ::GetRegionData((m_hRgn), 0, NULL);
		BYTE* pb = NEW BYTE[dwSize];
		LPRGNDATA pdata = (LPRGNDATA)pb;
		::GetRegionData((m_hRgn), dwSize, pdata);
		if (pdata->rdh.nCount > 0)
		{
			LPCRECT prc = (LPCRECT)(pb + sizeof(RGNDATAHEADER));
			idd.pt.x = prc->left;
			idd.pt.y = prc->top;
		}
		delete[] pb;
	}

	TEXTMETRIC tm;
	GetTextMetrics(hDC, &tm);
	idd.cyCurLine = max(tm.tmExternalLeading + tm.tmHeight, 0); //__cache.tryContentWidth>0?0:idd.cyCurLine);
	idd.cyMinHeight = max(idd.cyCurLine, 0); //__cache.tryContentWidth>0?0:idd.cyMinHeight);
	idd.cyTotalHeight = 0;
	idd.lLineIndent = 0;
	idd.lLineDistance = abs(FromStyleVariant(GetStyleAttribute(SAT_LINE_DISTANCE), LONG));
	idd.uTextStyle = (UINT)FromStyleVariant(GetStyleAttribute(SAT_TEXT_STYLE), LONG);
	//dc.RestoreDC(iOld);

	//idd.rc.bottom = 99999; //min(idd.rc.bottom, idd.cyMinHeight);
}

//class CRenderOffset
//{
//	HDC _hdc;
//	POINT _pt;
//public:
//	//CRenderOffset(HDC hdc, long cx, long cy) : _hdc(hdc)
//	//{
//	//	::GetWindowOrgEx(_hdc, &_pt);
//	//	::SetWindowOrgEx(_hdc, _pt.x+cx, _pt.y+cy, NULL);
//	//}
//	CRenderOffset(HDC hdc, CDuiControlExtensionBase* ctrl) : _hdc(hdc)
//	{
//		ATLASSERT(ctrl);
//		POINT pt = {0};
//		CDuiControlExtension::AdjustOffset(CDuiControlExtension::GetOffsetParent(ctrl), ctrl, pt);
//		::GetWindowOrgEx(_hdc, &_pt);
//		::SetWindowOrgEx(_hdc, _pt.x+pt.x, _pt.y+pt.y, NULL);
//	}
//	~CRenderOffset()
//	{
//		::SetWindowOrgEx(_hdc, _pt.x, _pt.y, NULL);
//	}
//};

class CDCSave
{
	HDC _hdc;
	int _saved;
public:
	CDCSave(HDC hdc) : _hdc(hdc) { ATLASSERT(_hdc); _saved = ::SaveDC(_hdc); }
	~CDCSave() { ::RestoreDC(_hdc, _saved); }
};

/*
 *	rcPaint 是相对于 root 的矩形
 */
void DuiNode::DoPaint(HDC hdc, const RECT& rcPaint, BOOL bDraw, InDrawData& idd)
{
	//trace_c(__FUNCTION__); ATLTRACE(L" draw(%d) ", bDraw); trace_rect(); trace_idd(idd);
	//if (bDraw)
	//	ATLTRACE(L"Paint %s\n", GetName());
	CDCSave savedc(hdc);

	HDE hde = DuiHandle<DuiNode>(this);
	// 绘制背景、边界、客户区、滚动条、焦点框和前景
	if (bDraw)
	{
		// 内联元素只绘制内容
		if (!IsBlock())
		{
			// 嵌套内联元素无需再设置原点偏移，它利用已有的原点
			//PrepareDC(hdc);
			OnPaintInlineBackground(hdc, m_hRgn);
			OnPaintContent(hdc, rcPaint, bDraw, idd);
			if (m_pLayoutMgr->GetFocus()==this && (GetControlFlags()&DUIFLAG_TABSTOP) &&
				(GetControlFlags()&DUIFLAG_NOFOCUSFRAME)==0 &&
				m_pLayoutMgr->GetAttributeBool(L"showfocus", TRUE))
			{
				RECT rc = GetPos(DUIRT_INSIDEBORDER);
				if (GetVisual()==NULL || !GetVisual()->OnPaintFocus(hde, hdc, rc, m_hRgn))
					PluginCallContext(TRUE, hde).Call(OpCode_PaintFocus, hde, hdc, rc, m_hRgn);
					//CDuiControlExtension::OnPaintFocus(_ctrl, hdc, rcPaint, (m_hRgn));
			}
			return;
		}

		RECT rc = GetPos(DUIRT_BORDER);
		CRenderClip clip(hdc, &rc, GetClipRgnBorder()
#ifndef NO3D
					, &m_pathClip
#endif
					);

		// 首先尝试交给DXTransform来绘制
		if ((_dxt) && (_dxt)->OnPaint(hdc, rcPaint, 0)) // 参数0表示所有绘制任务之前
			return;

		// 绘制背景
		if ((_dxt)==NULL || !(_dxt)->OnPaint(hdc, rcPaint, DXT_DS_BACKGROUND))
		{
			RECT rc = GetPos(DUIRT_INSIDEBORDER);
			if (GetVisual()==NULL || !GetVisual()->OnPaintBackground(hde, hdc, rc))
			{
				CRenderClip clip(hdc, &rc);
				PluginCallContext(TRUE, hde).Call(OpCode_PaintBackground, hde, hdc, rc);
				//CDuiControlExtension::OnPaintBackground(_ctrl, hdc, rcPaint);
			}
		}

		// 绘制边界
		if ((_dxt)==NULL || !(_dxt)->OnPaint(hdc, rcPaint, DXT_DS_BORDER))
		{
			//RECT rc = GetPos(DUIRT_BORDER);
			//CRenderClip clip(hdc, rc); // 如果hdc != hDC，那么不需要裁减，所以这里的参数还是原来的hDC

			if (GetVisual()==NULL || !GetVisual()->OnPaintBorder(hde, hdc, rc))
				PluginCallContext(TRUE, hde).Call(OpCode_PaintBorder, hde, hdc, rc);
				//CDuiControlExtension::OnPaintBorder(_ctrl, hdc, rcPaint);
		}

		// 绘制内容区
		if ((_dxt)==NULL || !(_dxt)->OnPaint(hdc, rcPaint, DXT_DS_CONTENT))
		{
			RECT rc = GetPos();
			CRenderClip clip(hdc, &rc, GetClipRgnChildren());

			InDrawData idd2 = {0} ; //= idd;
			InitDrawData(hdc, idd2); // 因为IDD数据必须在调用PrepareDC之后才有效，所以不信任DoPaint传入的idd参数，因为被UpdateLayout调用时并没有调用PrepareDC

			{
				CRenderContentOrg org(hdc, this, GetOffsetParent()); // 绘制子控件时重新修正原点
				OnPaintContent(hdc, rcPaint, bDraw, idd2);
			}
		}

		// 绘制滚动条
		m_scrollBar.Paint(hdc);
		m_scrollBarH.Paint(hdc);

		// 绘制焦点框
		if (bDraw && m_pParent!=NULL && m_pLayoutMgr->GetFocus()==this && (GetControlFlags()&DUIFLAG_TABSTOP) &&
			(GetControlFlags()&DUIFLAG_NOFOCUSFRAME)==0 &&
			m_pLayoutMgr->GetAttributeBool(L"showfocus", TRUE))
		{
			RECT rc = GetPos(DUIRT_INSIDEBORDER);
			if (GetVisual()==NULL || !GetVisual()->OnPaintFocus(hde, hdc, rc, m_hRgn))
				PluginCallContext(TRUE, hde).Call(OpCode_PaintFocus, hde, hdc, rc, m_hRgn);
				//CDuiControlExtension::OnPaintFocus(_ctrl, hdc, rcPaint, (m_hRgn));
		}

		// 绘制前景框
		if (bDraw && ((_dxt)==NULL || !(_dxt)->OnPaint(hdc, rcPaint, DXT_DS_FOREGROUND)))
		{
			RECT rc = GetPos(DUIRT_INSIDEBORDER);
			CRenderClip clip(hdc, &rc);
			PluginCallContext(TRUE, hde).Call(OpCode_PaintForeground, hde, hdc, rc);
			//CDuiControlExtension::OnPaintForeground(_ctrl, hdc, rcPaint);
		}
	}
	else // !bDraw，仅重新修正InDrawData数据，只需考虑客户区
	{
		BOOL bInline = IsInline(), bBlock = IsBlock();
		//UpdateDrawData(idd);
		if (!bBlock)
		{
			OnPaintContent(hdc, rcPaint, bDraw, idd);
		}
		else // is block
		{
			//// 这里要区分元素是否内联。内联元素在当前行开始估算宽度，非内联元素需要首先换行
			if (!bInline || idd.pt.x>=idd.rc.right /* || 还应该有个条件，就是最小宽度大于剩余宽度时也应该换行，以后再加 */)
				UpdateDrawData(idd);

			SIZE szParent = GetOffsetParentSize();
			SIZE szStyle = {FromStyleVariant(GetStyleAttribute(SAT_WIDTH),LONG,(LPVOID)(LONG_PTR)szParent.cx), FromStyleVariant(GetStyleAttribute(SAT_HEIGHT), LONG, (LPVOID)(LONG_PTR)szParent.cy)};
			SIZE szMin = szStyle;
			if (szMin.cx == 0) szMin.cx = FromStyleVariant(GetStyleAttribute(SAT_MINWIDTH), LONG, (LPVOID)(LONG_PTR)szParent.cx);
			if (szMin.cy == 0) szMin.cy = FromStyleVariant(GetStyleAttribute(SAT_MINHEIGHT), LONG, (LPVOID)(LONG_PTR)szParent.cy);

			SetScrollRange(0, 0);
			// 采用循环是因为可能需要重新计算
			while (TRUE)
			{
				// 试算期间，可能位置尚未建立，不能直接调用 GetPos。此时应该设置 tryContentWidth，GetPos 将以它作为基准执行高度估算
				__cache.tryContentWidth = T2CX(this, szStyle.cx>0 ? szStyle.cx : max(idd.rc.right - idd.pt.x, szMin.cx));

				InDrawData idd2 = {0};
				RowItems row;
				idd2.pvReserved = &row;
				InitDrawData(hdc, idd2);

				{
					CRenderContentOrg org(hdc, this, GetOffsetParent());
					OnPaintContent(hdc, rcPaint, bDraw, idd2);
					if (szStyle.cy == 0) szStyle.cy = max(C2TY(this, idd2.cyTotalHeight), szMin.cy);
				}

				// 获取子元素的最大位置，顺便更新内联区域和矩形
				SIZE szContent = {0};
				if ((GetControlFlags()&DUIFLAG_NOCHILDREN) == 0)
				{
					for (int i=0, num=m_children.GetSize(); i<num; i++)
					{
						DuiNode* child = GetChildControl(i);
						child->UpdateInlineRegion();

						DuiNode* rtchild = (child);
						if (rtchild->IsVisible())
						{
							szContent.cx = max(szContent.cx, rtchild->m_rcItem.right);
							szContent.cy = max(szContent.cy, rtchild->m_rcItem.bottom);
						}
					}
				}
				if (!bInline) szContent.cx = max(szContent.cx, __cache.tryContentWidth);
				SIZE szTotal = AdjustSize(szContent);
				if (szStyle.cx > 0) szTotal.cx = max(szStyle.cx, szMin.cx);
				if (szStyle.cy > 0) szTotal.cy = szStyle.cy;
				//if (bInline)
				{
					if (idd.pt.x>idd.rc.left+idd.lLineIndent && (idd.rc.right-idd.pt.x<max(szTotal.cx,C2TX(this,12))))
					{
						IDD_RETURN(idd);
						// 换行之后可能需要重新调整宽度
						//if (szMin.cx == 0) szTotal.cx = idd.rc.right - idd.pt.x;
						continue;
					}
				}
				SetPos(CRect(idd.pt, szTotal));
				
				//LONG cyTotal = idd2.cyTotalHeight + szTotal.cy;
				idd.cyMinHeight = max(idd.cyMinHeight, szTotal.cy);
				if (bInline)
					idd.pt.x += szTotal.cx;
				else
					IDD_RETURN(idd);
				//UpdateDrawData(idd);
				//idd.cyTotalHeight += cyTotal;
				__cache.tryContentWidth = 0;
				break;
			}
		}
	}
}

#ifndef NO3D
void DuiNode::Do3DPaint()
{
	if (m_pSurface==NULL) return;
	HDE hde = DuiHandle<DuiNode>(this);

	// 如果需要，更新滚动条状态
	UpdateScrollRange();

	HDC hdc = m_pSurface->GetDC();

	{
		CDCSave savedc(hdc);

		CRect rcPaint = GetPos(DUIRT_BORDER);

		// 设置这个窗口原点非常重要，以后下面的所有绘制函数都是相对父控件客户区，而这里的HDC都是针对本控件创建的
		::SetWindowOrgEx(hdc, rcPaint.left, rcPaint.top, NULL);

		// 绘制边界、客户区、滚动条、焦点框和前景
		//if (bDraw)
		{
			// 内联元素不绘制
			if (!IsBlock()) return;

			// 如果有裁剪路径，则这里设置
			CRenderClip clip(hdc, &rcPaint, GetClipRgnBorder(), &m_pathClip);

			// 绘制背景
			{
				RECT rc = GetPos(DUIRT_INSIDEBORDER);
				if (GetVisual())
					GetVisual()->OnPaintBackground(hde, hdc, rc);
			}

			// 绘制边界
			if ((_dxt)==NULL || !(_dxt)->OnPaint(hdc, rcPaint, DXT_DS_BORDER))
			{
				//RECT rc = GetPos(DUIRT_BORDER);
				//CRenderClip clip(hdc, rc); // 如果hdc != hDC，那么不需要裁减，所以这里的参数还是原来的hDC

				if (GetVisual()==NULL || !GetVisual()->OnPaintBorder(hde, hdc, rcPaint))
					PluginCallContext(TRUE, hde).Call(OpCode_PaintBorder, hde, hdc, rcPaint);
					//CDuiControlExtension::OnPaintBorder(_ctrl, hdc, rcPaint);
			}

			// 绘制内容区
			if ((_dxt)==NULL || !(_dxt)->OnPaint(hdc, rcPaint, DXT_DS_CONTENT))
			{
				RECT rc = GetPos();
				CRenderClip clip(hdc, &rc, GetClipRgnChildren());

				InDrawData idd2 = {0};
				InitDrawData(hdc, idd2);

				POINT ptOld = {0};
				::GetWindowOrgEx(hdc, &ptOld);
				//CDuiAxSite* site = CLASS_CAST(_ctrl, CDuiAxSite);
				CDuiCustomOrg* org = CLASS_CAST(this, CDuiCustomOrg);
				if (org)
				{
					org->SetOrg(hdc);
					OnPaintContent(hdc, rcPaint, TRUE, idd2);
				}
				else
				{
					//POINT pt = {GetScrollbar(FALSE)->GetScrollPos(), GetScrollbar(TRUE)->GetScrollPos()};
					//CRenderContentOrg orgScroll(hdc, pt);
					CRenderContentOrg _org(hdc, this, GetOffsetParent()); // 绘制子控件时重新修正原点
					OnPaintContent(hdc, rcPaint, TRUE, idd2);
				}
				::SetWindowOrgEx(hdc, ptOld.x, ptOld.y, NULL);
			}

			// 绘制滚动条
			m_scrollBar.Paint(hdc);
			m_scrollBarH.Paint(hdc);

			// 绘制焦点框
			if (m_pParent!=NULL && m_pLayoutMgr->GetFocus()==this && (GetControlFlags()&DUIFLAG_TABSTOP) &&
				(GetControlFlags()&DUIFLAG_NOFOCUSFRAME)==0 &&
				m_pLayoutMgr->GetAttributeBool(L"showfocus", TRUE))
			{
				RECT rc = GetPos(DUIRT_INSIDEBORDER);
				if (GetVisual()==NULL || !GetVisual()->OnPaintFocus(hde, hdc, rc, m_hRgn))
					PluginCallContext(TRUE, hde).Call(OpCode_PaintFocus, hde, hdc, rc, m_hRgn);
					//CDuiControlExtension::OnPaintFocus(_ctrl, hdc, rcPaint, (m_hRgn));
			}

			//if (m_pParent==NULL)
			//{
			//	GdiPath p;
			//	p.Parse(L"roundrect 10");
			//	CRect rrrr(0,0,50,50);
			//	::BeginPath(hdc);
			//	p.Draw(hdc, rrrr);
			//	::EndPath(hdc);
			//	::StrokePath(hdc);
			//	//::StrokeAndFillPath(hdc);
			//}

			//// 绘制前景框
			//if (((_dxt)==NULL || !(_dxt)->OnPaint(hdc, rcPaint, DXT_DS_FOREGROUND)))
			//{
			//	RECT rc = GetPos(DUIRT_INSIDEBORDER);
			//	CRenderClip clip(hdc, rc);
			//	CDuiControlExtension::OnPaintForeground(_ctrl, hdc, rcPaint);
			//}
		}
	}

	m_pSurface->ReleaseDC(hdc);
}
#endif // NO3D


void CALLBACK DuiNode::DrawToDC(HDC hdc, LPVOID lpData, DWORD dwState)
{
	DuiNode* pCtrl = (DuiNode*)lpData;
	if (pCtrl==NULL || hdc==NULL) return;

	HDE hde = DuiHandle<DuiNode>(pCtrl);
	CRect rc = pCtrl->GetPos(DUIRT_TOTAL);
	//CDuiControlExtension::AdjustOffset(r->GetParent(), NULL, rc);

	CDCHandle dc(hdc);
	int iOldCtx = dc.SaveDC();

	{
		// 当前应该正在父容器原点
		DuiNode* parent = pCtrl->GetOffsetParent();
		//CRenderContentOrg orgParent(hdc, parent);

		// paint background
		if (dwState & DXT_DS_BACKGROUND)
		{
			DuiNode::DrawBackgroundToDC(hdc, parent, rc); // paint parent

			RECT rc2 = pCtrl->GetPos(DUIRT_INSIDEBORDER);
			//CDuiControlExtension::AdjustOffset(r->GetParent(), NULL, rc2);
			//CRenderClip clip(hdc, rc);
			if (pCtrl->GetVisual()==NULL || !pCtrl->GetVisual()->OnPaintBackground(hde, hdc, rc2))
			{
				CRenderClip clip(hdc, &rc2);
				PluginCallContext(TRUE, hde).Call(OpCode_PaintBackground, hde, hdc, rc);
				//OnPaintBackground(pCtrl, hdc, rc);
			}
		}

		// paint border
		if (dwState & DXT_DS_BORDER)
		{
			RECT rc2 = pCtrl->GetPos(DUIRT_BORDER);
			//CDuiControlExtension::AdjustOffset(r->GetParent(), NULL, rc2);
			CRenderClip clip(hdc, &rc, pCtrl->GetClipRgnBorder());
			if (pCtrl->GetVisual()==NULL || !pCtrl->GetVisual()->OnPaintBorder(hde, hdc, rc2))
				PluginCallContext(TRUE, hde).Call(OpCode_PaintBorder, hde, hdc, rc);
				//OnPaintBorder(pCtrl, hdc, rc);
		}

		// paint content
		if (dwState & DXT_DS_CONTENT)
		{
			((CDuiScrollbar*)pCtrl->GetScrollbar())->Paint(hdc);
			((CDuiScrollbar*)pCtrl->GetScrollbar(FALSE))->Paint(hdc);

			RECT rc2 = pCtrl->GetPos();
			//CDuiControlExtension::AdjustOffset(rtCtrl->GetParent(), NULL, rc2);

			CRenderClip clip(hdc, &rc2, pCtrl->GetClipRgnChildren());
			InDrawData idd = {0};
			pCtrl->InitDrawData(hdc, idd);
			{
				//CRenderContentOrg org(hdc, (pCtrl));
				CRenderContentOrg org(hdc, pCtrl, parent); // 原点参考系不一样，使用相对父元素偏移
				pCtrl->OnPaintContent(hdc, rc, TRUE, idd);
			}
			//idd.cyTotalHeight += idd.cyTotalHeight;
		}
	}

	dc.RestoreDC(iOldCtx);
}

void CALLBACK DuiNode::DrawBackgroundToDC(HDC hdc, LPVOID lpData, const RECT rc)
{
	DuiNode* pCtrl = (DuiNode*)lpData;
	if (pCtrl == NULL) return;

	// 当前原点在 pCtrl 的客户区，首先回退至父容器原点
	DuiNode* parent = pCtrl->GetOffsetParent();
	CRenderContentOrg orgParent(hdc, parent, pCtrl);
	{
		DuiNode::DrawBackgroundToDC(hdc, parent, rc);
	}

	CRect rcbk = pCtrl->GetPos(DUIRT_INSIDEBORDER);
	//CDuiControlExtension::AdjustOffset(r->GetParent(), NULL, rcbk);
	if (!rcbk.IntersectRect(&rcbk, &rc)) return;
	HDE hde = DuiHandle<DuiNode>(pCtrl);
	PluginCallContext(TRUE, hde).Call(OpCode_PaintBackground, hde, hdc, rc);
	//OnPaintBackground(pCtrl, hdc, rc);
}

void DuiNode::OnPaintInlineBackground(HDC hdc, HRGN hrgn)
{
	if (hrgn==NULL) return;

	CDCHandle dc(hdc);

	CDuiStyleVariant* sv = (CDuiStyleVariant*)GetStyleAttribute(SAT_BACKGROUND);
	if (sv==NULL || sv==&svEmpty || sv->svt!=SVT_COLOR)
	{
		//if (rt(pCtrl)->m_pLayoutMgr->Get3DDevice())
		//{
		//	CBrush br;
		//	dc.FillRgn(hrgn, br.CreateSolidBrush(d3d::KeyColor()));
		//}
		return;
	}

	BYTE bAlpha = 255;
	CComVariant v = GetAttribute(L"background-alpha");
	if (v.vt!=VT_EMPTY && SUCCEEDED(v.ChangeType(VT_UI1)))
		bAlpha = (BYTE)V_UI1(&v);

	COLORREF clr = FromStyleVariant(sv, COLORREF);
	if (bAlpha == 255)
	{
		CBrush br;
		dc.FillRgn(hrgn, br.CreateSolidBrush(clr));
	}
	else
	{
		Graphics g(hdc);
		g.SetCompositingMode(CompositingModeSourceOver);
		BYTE a = (BYTE) (clr>>24);
		if (a>0 && a<255) bAlpha = a; // 优先使用COLORREF中自带的A值
		Color c(Color::MakeARGB(min(bAlpha,254), GetRValue(clr), GetGValue(clr), GetBValue(clr)));
		SolidBrush br(c);

		Region rgn(hrgn);
		g.FillRegion(&br, &rgn);
	}
}


void DuiNode::OnPaintContent( HDC hdc, RECT rcPaint, BOOL bDraw, InDrawData& idd )
{
	DelegateIOC(OnPaintContent, (this, hdc,rcPaint,bDraw,idd), );

	//trace_c(__FUNCTION__); ATLTRACE(L"draw(%d) ", bDraw); trace_rect(); trace_idd(idd);
	LPCOLESTR p = NULL;
	if (m_children.GetSize()==0 && (p = (_attributes).GetAttributeString(L"text"))!=NULL && CLASS_CAST(this,CDuiInlineText)==NULL)
	{
		// 这里投机取巧一下，如果一个元素没有子元素，并且需要显示文本，就创建一个内联文本
		DuiNode* pChild = m_pLayoutMgr->CreateControl(this, L"InlineText");
		if (pChild)
		{
			if (!AddChildControl(pChild))
			{
				pChild->DeleteThis();
				return;
			}
			pChild->SetAttribute(L"text", p);
			_attributes.Remove(L"text");
		}
	}

	struct DefCallback
	{
		static DuiVoid /*CALLBACK*/ OnPaintContent(CallContext* pcc, HDE hde, HDC hdc, RECT rcPaint, BOOL bDraw, InDrawData* pidd)
		{
			DuiNode* n = DuiNode::FromHandle(hde);
			n->DrawChildren(hdc, rcPaint, bDraw, *pidd);

			//InDrawData& idd = *pidd;
			//if (!bDraw)
			//{
			//	if (!n->IsBlock())
			//	{
			//		if (n->m_hRgn) n->m_hRgn = (::DeleteObject(n->m_hRgn), NULL);
			//		n->m_hRgn = n->CombineChildrenRegion();
			//		n->UpdateRectFromRgn(n->m_hRgn);
			//	}
			//	// TODO: 如果是BLOCK，应该读取属性计算RECT，是否应该折行
			//	else
			//	{
			//		CRect rc = n->GetPos(); //(idd.rc);
			//		idd.cyTotalHeight += idd.cyCurLine; //::DrawTextW(hdc, p, lstrlenW(p), &rc, idd.uTextStyle|DT_CALCRECT);
			//		idd.pt.x = idd.rc.left + idd.lLineIndent;
			//		idd.pt.y += rc.Height();
			//		n->UpdateDrawData(idd);
			//	}
			//}
			return 0;
		}
	};

	LockReenter(OnPaintContent);

	BOOL bHasInvocation = FALSE;
	InvokeOneSink(this, OnPaintContent, hdc, rcPaint, bDraw, &idd);
	if (!bHasInvocation)
		DefCallback::OnPaintContent(NULL, Handle(), hdc, rcPaint, bDraw, &idd);
	
	if (!bDraw)
	{
		if (!IsBlock())
		{
			if (m_hRgn) m_hRgn = (::DeleteObject(m_hRgn), NULL);
			m_hRgn = CombineChildrenRegion();
			UpdateRectFromRgn(m_hRgn);
		}
		// TODO: 如果是BLOCK，应该读取属性计算RECT，是否应该折行
		else
		{
			//CRect rc = GetPos(); //(idd.rc);
			////if (idd.pt.x > idd.rc.left + idd.lLineIndent)
			//idd.cyTotalHeight += idd.cyCurLine; //::DrawTextW(hdc, p, lstrlenW(p), &rc, idd.uTextStyle|DT_CALCRECT);
			//idd.pt.x = idd.rc.left + idd.lLineIndent;
			//idd.pt.y += rc.Height();
			UpdateDrawData(idd);
		}
	}
}

void DuiNode::DrawChildren( HDC hdc, RECT rcPaint, BOOL bDraw, InDrawData& idd )
{
	if ((GetControlFlags() & DUIFLAG_NOCHILDREN) == 0)
	{
		if (bDraw)
		{
			for (DuiNode* r = m_offsetChildren; r; r = r->next)
			{
				if (!r->IsVisible() || r->GetZIndex()>=0) continue;
				r->DoPaint(hdc, rcPaint, bDraw, idd);
			}
		}

		for (int i=0; i<m_children.GetSize(); i++)
		{
			DuiNode* c = (DuiNode*)m_children[i];
			if (c)
			{
				if (/*bDraw &&*/ !c->IsVisible()) continue;
				if (bDraw && c->GetPosition()) continue;
#ifndef NO3D
				// 3D模式下非内联元素无需绘制
				if (!bDraw || (m_pLayoutMgr->Get3DDevice()==NULL || c->IsInline()))
#endif
					c->DoPaint(hdc, rcPaint, bDraw, idd);
			}
		}

		if (bDraw)
		{
			for (DuiNode* r = m_offsetChildren; r; r = r->next)
			{
				if (!r->IsVisible() || r->GetZIndex()<0) continue;
				r->DoPaint(hdc, rcPaint, bDraw, idd);
			}
		}
	}
	UpdateDrawData(idd);
}

void DuiNode::PrepareDC(HDC hdc)
{
	LockReenter(OnPrepareDC);
	//BOOL bDone = FALSE;
	//InvokeAllSinksBool(this, OnPrepareDC, hdc);
	//if (!bDone)
	BOOL bDone = FALSE;
	InvokeOneSinkResult(bDone, this, OnPrepareDC, hdc);
	if (!bDone)
	//ISinkOnPrepareDC * p = CLASS_CAST(this, ISinkOnPrepareDC);
	//if (!p || !p->OnPrepareDC(Handle(), hdc))
	{
		CDCHandle dc(hdc);
		COLORREF clr = FromStyleVariant(GetStyleAttribute(SAT_COLOR), COLORREF);
		if (clr != CLR_INVALID) dc.SetTextColor(clr);

		LOGFONTW lf;
		if (m_pLayoutMgr->BuildControlFont(this, &lf))
		{
			if (__cache.font) ::DeleteObject(__cache.font);
			__cache.font = ::CreateFontIndirectW(&lf);
			if (__cache.font)
			{
				ProcessFont(__cache.font);
				dc.SelectFont(__cache.font);
			}
		}

		dc.SetBkMode(TRANSPARENT);
	}
}

void DuiNode::SetEventCallback(EVENTCALLBACK fnCallback)
{
	m_fnEventCallback = fnCallback;
}

IDispatch* DuiNode::GetEventHandler(BOOL bAutoCreate/* =FALSE */, BOOL bAddRef/* =FALSE */)
{
	if (m_eventHandler.p==NULL && bAutoCreate)
	{
		m_pLayoutMgr->ParseExpression(L"{}", &m_eventHandler);
	}

	if (m_eventHandler.p && bAddRef)
		m_eventHandler.p->AddRef();

	return m_eventHandler.p;
}

BOOL DuiNode::IsExtensionEvent(DuiEvent& event)
{
	if (m_scrollBar.IsScrollEvent(event) ||
		m_scrollBarH.IsScrollEvent(event) )
		return TRUE;

	for (int i=0; i<m_aExtensions.GetSize(); i++)
	{
		IExtension* pe = (IExtension*)m_aExtensions[i];
		ISinkIsMyEvent* p = CLASS_CAST(pe, ISinkIsMyEvent);
		if (p && p->IsMyEvent(Handle(), &event))
			return TRUE;
	}

	//ISinkIsMyEvent* p = NULL;
	//if (GetLayout() && (p=CLASS_CAST(GetLayout(), ISinkIsMyEvent)) && p->IsMyEvent(Handle(), &event))
	//	return TRUE;

	BOOL b = FALSE;
	PluginCallContext(TRUE, Handle()).Output(&b).Call(OpCode_IsMyEvent, Handle(), &event);

	return b;
}

void DuiNode::OnEvent(DuiEvent& event)
{
	StateChangedNotifyHelper scnh(this);
	switch (DuiNode::NameToEventType(event.Name))
	{
	case DUIET_load:
		//control_func(this, SetFilterProps, ());
//#ifdef _DEBUG
//		lstrcpyW(_ctrl->__tag, _ctrl->GetName());
//#endif // _DEBUG
		break;
	case DUIET_setfocus:
		if (IsEnabled())
		{
			m_dwState |= DUISTATE_FOCUSED;
			Invalidate();
		}
		break;
	case DUIET_killfocus:
		if (IsEnabled())
		{
			m_dwState &= ~DUISTATE_FOCUSED;
			Invalidate();
		}
		break;
	case DUIET_timer:
		m_pLayoutMgr->SendNotify(this, _T("timer"), event.wParam, event.lParam);
		break;
	case DUIET_buttondown:
	case DUIET_dblclick:
		if( PtInControl(event.ptMouse) && IsEnabled() )
		{
			m_dwState |= DUISTATE_PUSHED | DUISTATE_CAPTURED;
			Invalidate();
		}
		break;
	case DUIET_mousemove:
		if( (m_dwState & DUISTATE_CAPTURED) != 0 && IsEnabled())
		{
			if( PtInControl(event.ptMouse) )
				m_dwState |= DUISTATE_PUSHED;
			else
				m_dwState &= ~DUISTATE_PUSHED;
			Invalidate();
		}

#ifndef NO3D
		if (m_pScene && IsEnabled())
		{
			POINT pt = DuiNode::GetOffset(NULL, GetOffsetParent(), event.ptMouse);
			RECT rc = GetPos(DUIRT_BORDER);
			pt.x -= rc.left;
			pt.y -= rc.top;
			m_pScene->SetMousePos(&pt);
			Invalidate();
		}
#endif
		break;
	case DUIET_buttonup:
		if( (m_dwState & DUISTATE_CAPTURED) != 0 && IsEnabled() )
		{
			//if( PtInControl(event.ptMouse) ) // Activeate() 调用放到click事件中处理，按ENTER键时也可以调用到
			//	_ctrl->Activate();
			m_dwState &= ~(DUISTATE_PUSHED | DUISTATE_CAPTURED);
			Invalidate();
		}
		break;
	case DUIET_click:
		if (IsEnabled())
			Activate();
		break;
	case DUIET_mouseenter:
		if (IsEnabled())
		{
			m_dwState |= DUISTATE_HOT;
			Invalidate();
		}
		break;
	case DUIET_mouseleave:
		if (IsEnabled())
		{
			m_dwState &= ~DUISTATE_HOT;
#ifndef NO3D
			if (m_pScene) m_pScene->SetMousePos(NULL);
#endif
			Invalidate();
		}
		break;
	case DUIET_size:
		UpdateLayout();
		break;
	}
}

void DuiNode::OnNotify(DuiNotify& /*msg*/)
{
}

void DuiNode::SetAttribute(LPCOLESTR pstrName, CComVariant v) //alue)
{
	LockReenter(SetAttribute);

	if (pstrName==NULL || v.vt==VT_EMPTY /*pstrValue==NULL*/) return;
	//BOOL bParseSubProps = (lstrcmpiW(pstrName, L"filter")==0);

	if (lstrcmpiW(pstrName, L"id")==0 || lstrcmpiW(pstrName, L"name")==0)
		m_id = v.bstrVal;
	else
	{
		struct DefCallback
		{
			static DuiVoid /*CALLBACK*/ SetAttribute(CallContext* pcc, HDE hde, LPCWSTR szName, VARIANT* value)
			{
				DuiNode* n = DuiNode::FromHandle(hde);
				n->_attributes.SetAttribute(szName, *value, lstrcmpiW(szName, L"filter")==0);
				return 0;
			}
		};

		BOOL bHasInvocation = FALSE;
		InvokeOneSink(this, SetAttribute, pstrName, (VARIANT*)&v);
		if (!bHasInvocation)
			DefCallback::SetAttribute(NULL, Handle(), pstrName, &v);
	}
	////CComVariant v;
	//if (SUCCEEDED(v.ChangeType(VT_BSTR/*, &value*/)))
	//{
	//	if( lstrcmpiW(pstrName, _T("enabled")) == 0 ) SetEnabled(ParseBoolString(v.bstrVal));
	//	else if( lstrcmpiW(pstrName, _T("visible")) == 0 ) SetVisible(ParseBoolString(v.bstrVal));
	//	else if( lstrcmpiW(pstrName, _T("shortcut")) == 0 ) SetShortcut(v.bstrVal[0]);
	//	else if( lstrcmpiW(pstrName, _T("style")) == 0 ) CDuiControlExtension::ParseStyle(_ctrl, v.bstrVal);
	//	else if (lstrcmpiW(pstrName, L"filter")==0) SetFilter(GetAttributeString(L"filter"));
	//	else if( !CDuiControlExtension::TryParseAsStyle(_ctrl, pstrName, v.bstrVal) ) CDuiControlExtension::TryParseAsProcedure(_ctrl, pstrName, v.bstrVal);
	//}
	//} 
}

void DuiNode::RemoveAttribute(LPCOLESTR szName)
{
	_attributes.RemoveAttribute(szName);
}

void DuiNode::SetAttributeByPos(LONG pos, CComVariant& value)
{
	CStdString k = (_attributes).GetKeyAt(pos);
	SetAttribute(k, value);
}

LONG DuiNode::GetAttributePos(LPCOLESTR szName) const
{
	return (_attributes).FindAttribute(szName);
}

LONG DuiNode::GetAttributeCount() const
{
	return (_attributes).GetSize();
}

CComVariant DuiNode::GetAttribute(LPCOLESTR szName)
{
	struct DefCallback
	{
		static DuiVoid /*CALLBACK*/ GetAttribute(CallContext* pcc, HDE hde, LPCWSTR szName, VARIANT* value)
		{
			::VariantClear(value);
			DuiNode* n = DuiNode::FromHandle(hde);
			CComVariant v;
			// 首先尝试从样式表里读取值
			CDuiStyle* pStyle = n->GetPrivateStyle(TRUE);

			if (szName==NULL) return 0;

#define __StyleParser(name, classname, sat) else if (lstrcmpiW(szName, L#name) == 0 && CDuiStyleParser_##classname::Parse(n->m_pLayoutMgr, pStyle, v, TRUE)) return v.Detach(value), 0;
			__ForeachStyle(__StyleParser);
#undef __StyleParser

			v = n->_attributes.GetAttribute(szName);
			v.Detach(value);
			return 0;
		}
	};

	LockReenter(GetAttribute, CComVariant());
	CComVariant v;
	BOOL bHasInvocation = FALSE;
	InvokeOneSink(this, GetAttribute, szName, (VARIANT*)&v);
	if (!bHasInvocation)
		DefCallback::GetAttribute(NULL, Handle(), szName, &v);
	return v;
}

CComVariant DuiNode::GetAttributeByPos(LONG pos)
{
	return (_attributes).GetAttributeByIndex(pos);
}

LPCOLESTR DuiNode::GetAttributeString(LPCOLESTR szName)
{
	return _attributes.GetAttributeString(szName);
}

BOOL DuiNode::TryParseAsProcedure(LPCOLESTR lpszName, LPCOLESTR lpszValue)
{
	if (lstrlenW(lpszName) > 2 &&
		(lpszName[0]==L'o') && (lpszName[1]==L'n'))
	{
		CComPtr<IDispatch> spThis = GetObject(TRUE);
		ATLASSERT(spThis);
		CComPtr<IDispatch> spProc;
		if (SUCCEEDED((m_pLayoutMgr)->ParseProcedure(lpszValue, &spProc)))
		{
			CComVariant v = spProc;
			return SUCCEEDED(spThis.PutPropertyByName(lpszName, &v));
		}
	}
	return FALSE;
}

BOOL DuiNode::ParseStyle(LPCOLESTR lpszValue)
{
	if (lpszValue==NULL)
		return FALSE;

	//// 把 style 里的项也存入属性中，方便脚本调用
	//CStrArray strs;
	//if (!SplitStringToArray(lpszValue, strs, L";"))
	//	return FALSE;

	//for (int i=0; i<strs.GetSize(); i++)
	//{
	//	CStrArray pairs;
	//	if (SplitStringToArray(strs[i], pairs, L":"/*, FALSE*/) && pairs.GetSize()==2)
	//	{
	//		m_attributes.SetAttribute(pairs[0].Trim(), pairs[1]);
	//	}
	//}

	return GetPrivateStyle(TRUE)->ParseStyle(lpszValue);
}

BOOL DuiNode::TryParseAsStyle(LPCOLESTR lpszName, CComVariant v)
{
	//if (lstrcmpiW(lpszName, L"visual")==0)
	//	r->ResetVisual();
	//else if (lstrcmpiW(lpszName, L"layout")==0)
	//	r->ResetLayout();
	////else if (lstrcmpiW(lpszName, L"filter")==0) control_func(pCtrl, SetFilter, (lpszValue));

	if (lpszName==NULL || v.vt==VT_EMPTY)
		return FALSE;

#define __StyleParser(name, classname, sat) else if (lstrcmpiW(lpszName, L#name) == 0)  return CDuiStyleParser_##classname::Parse(m_pLayoutMgr, GetPrivateStyle(TRUE), v);
	__ForeachStyle(__StyleParser);
#undef __StyleParser

	return FALSE;
}

BOOL DuiNode::TryRemoveAsStyle(LPCOLESTR lpszName)
{
	//if (lstrcmpiW(lpszName, L"visual")==0)
	//	return r->ResetVisual(), TRUE;
	//else if (lstrcmpiW(lpszName, L"layout")==0)
	//	return r->ResetLayout(), TRUE;

	if (lpszName == NULL) return FALSE;

#define __StyleParser(name, classname, sat) else if (lstrcmpiW(lpszName, L#name) == 0 && SAT_##sat!=SAT__UNKNOWN) {  CDuiStyle* p = GetPrivateStyle(); if (p) p->RemoveAttribute(SAT_##sat); return TRUE; }
	__ForeachStyle(__StyleParser);
#undef __StyleParser
	
	return FALSE;
}

void DuiNode::SetPrivateStyle( CDuiStyle* pStyle )
{
	m_pStyle = pStyle;
}

CDuiStyle* DuiNode::GetPrivateStyle(BOOL bAutoCreate/*=FALSE*/) const
{
	if (m_pStyle==NULL && bAutoCreate)
		m_pLayoutMgr->CreateStyle(NULL,NULL,0,(DuiNode*)this,NULL);
	return m_pStyle;
}

DuiStyleVariant* DuiNode::GetStyleAttribute(DuiStyleAttributeType sat, DuiStyleVariantType svt/*=SVT_EMPTY*/, DWORD dwMatchMask/*=0*/)
{
	struct DefCallback
	{
		static DuiStyleVariant* /*CALLBACK*/ GetStyleAttribute(CallContext* pcc, HDE hde, DuiStyleAttributeType sat, DuiStyleVariantType svt, DWORD dwMatchMask)
		{
			class StyleMonitor
			{
				DuiNode* _r;
				DWORD _states;
				DuiStyleAttributeType _sat;
				CDuiStyleVariant* __val;
				CDuiStyleVariant*& _val;
			public:
				// 在构造函数里查询样式表缓存，如果存在，直接赋值给_val
				StyleMonitor(DuiNode* r, DuiStyleAttributeType sat, CDuiStyleVariant*& val) : _r(r), _sat(sat), _val(val), __val(NULL)
				{
					ATLASSERT(_r);
					_states = _r->GetState();
					for (int i=0; i<_r->__cache.styles.GetSize(); i++)
					{
						StyleCacheItem& item = _r->__cache.styles[i];
						if (_states==item.states && _sat==item.sat)
						{
							_val = __val = item.val;
							return;
						}
					}
				}

				// 缓存样式表项，以便下次读取
				~StyleMonitor()
				{
					if (_val && _val!=&svEmpty && _val!=__val)
					{
						StyleCacheItem item = {_states, _sat, _val};
						_r->__cache.styles.Add(item);
					}
				}
			};

			CDuiStyleVariant* pVar = NULL;
			DuiNode* n = DuiNode::FromHandle(hde);

			StyleMonitor sm(n, sat, pVar);
			if (pVar /*&& pVar!=&svEmpty*/) return pVar; // 如果有缓存，pVal将有值

			CDuiStyle* pStyle = n->m_pStyle;

			DWORD dwMatch = DUISTYLEMATCH_ALL;
			if (n->GetState() != 0)
			{
				//ATLASSERT(sat != SAT_BORDER);
				pStyle = n->m_pLayoutMgr->FindSimilarStyle(n, dwMatch, sat);
			}

			// 对于状态不匹配的，如果没有匹配到同名或同ID的，那么应该直接引用无状态的样式
			if (((dwMatch&dwMatchMask)==dwMatchMask) && pStyle && (pVar=pStyle->GetAttributeNoControl(sat,svt))!=&svEmpty)
				return pVar;

			//if ((dwMatch&dwMatchMask)==dwMatchMask)
			{
				DWORD dwOld = n->GetState();
				n->m_dwState = 0;
				pStyle = n->m_pLayoutMgr->FindSimilarStyle(n, dwMatch, sat);
				n->m_dwState = dwOld;
			}

			if (((dwMatch&dwMatchMask)==dwMatchMask) && pStyle && (pVar=pStyle->GetAttributeNoControl(sat,svt))!=&svEmpty)
				return pVar;

			if ((dwMatchMask==0 || (dwMatchMask&DUISTYLEMATCH_PARENT)) && sat<SAT__NO_PARENT && n->m_pParent && (pVar=(CDuiStyleVariant*)n->m_pParent->GetStyleAttribute(sat,svt))!=&svEmpty)
				return pVar;

			return &svEmpty;
		}
	};

	LockReenter(GetStyleAttribute, NULL);
	BOOL bHasInvocation = FALSE;
	DuiStyleVariant* sv = &svEmpty;
	InvokeOneSinkResult(sv, this, GetStyleAttribute, sat, svt, dwMatchMask);
	if (!bHasInvocation)
		sv = DefCallback::GetStyleAttribute(NULL, Handle(), sat, svt, dwMatchMask);

	return sv;
}

int DuiNode::GetChildCount() const
{
	return m_children.GetSize();
}

DuiNode* DuiNode::GetChildControl( int index ) const
{
	if (index<0 || index>=m_children.GetSize()) return NULL;
	return (DuiNode*)m_children[index];
}

BOOL DuiNode::AddChildControl( DuiNode* pChild, DuiNode* pRef/*=NULL*/ )
{
	if (pChild==NULL) return FALSE;
	BOOL b;
	//if (pRef == NULL)
	//	b = m_children.Add(pChild);
	//else
		b = m_children.Insert(m_children.Find(pRef), pChild);
	if( m_pLayoutMgr )
	{
		if ((pChild)->GetParent()==NULL)
			m_pLayoutMgr->m_aDelayedCleanup.Remove(pChild);
		m_pLayoutMgr->InitControls(pChild, this);
		m_pLayoutMgr->UpdateLayout();
	}
	return b;
}

BOOL DuiNode::RemoveChildControl( DuiNode* pChild )
{
	LockReenter(OnChildRemoved, FALSE);
	if (pChild && m_bAutoDestroy && m_children.Find(pChild)>=0)
	{
		InvokeAllSinks(this, OnChildRemoved, pChild->Handle());
		pChild->DeleteThis();
	}
	BOOL b = m_children.Remove(pChild);
	if( m_pLayoutMgr ) m_pLayoutMgr->UpdateLayout();
	return b;
}

void DuiNode::RemoveAllChildren()
{
	LockReenter(OnChildRemoved);
	if (m_bAutoDestroy) for (int i=0; i<m_children.GetSize(); i++)
	{
		InvokeAllSinks(this, OnChildRemoved, static_cast<DuiNode*>(m_children[i])->Handle());
		static_cast<DuiNode*>(m_children[i])->DeleteThis();
	}
	m_children.RemoveAll();
	m_scrollBar.SetScrollPos(m_scrollBar.GetScrollPos());
	if( m_pLayoutMgr != NULL ) m_pLayoutMgr->UpdateLayout();
}

int DuiNode::FindSelectable( int iIndex, BOOL bForward /*= TRUE*/ )
{
	// NOTE: This is actually a helper-function for the list/combo/ect controls
	//       that allow them to find the next enabled/available selectable item
	if ((GetControlFlags()&DUIFLAG_NOCHILDREN)) return -1;
	int num = GetChildCount();
	if( num == 0 ) return -1;
	iIndex = min(max(iIndex, 0), num - 1);
	if( bForward )
	{
		for( int i = iIndex; i < num; i++ )
		{
			DuiNode* r = GetChildControl(i);
			if( (r->GetControlFlags() & DUIFLAG_SELECTABLE)
				&& r->IsVisible()
				&& r->IsEnabled() ) return i;
		}
		return -1;
	}
	else
	{
		for( int i = iIndex; i >= 0; --i )
		{
			DuiNode* r = GetChildControl(i);
			if( (r->GetControlFlags() & DUIFLAG_SELECTABLE)
				&& r->IsVisible()
				&& r->IsEnabled() ) return i;
		}
		return FindSelectable(0, TRUE);
	}
}

ILayout* DuiNode::GetLayout()
{
	if (!__cache.hasLayout)
	{
		__cache.hasLayout = TRUE;

		ILayout* pLayout = NULL;
		LPCOLESTR szName = FromStyleVariant(GetStyleAttribute(SAT_LAYOUT, SVT_EMPTY, DUISTYLEMATCH_NAME/*_ID*/), LPCOLESTR);
		if (szName)
			PluginCallContext(TRUE).Output(&pLayout).Call(OpCode_QueryLayout, szName);
		if (pLayout != __cache.layout)
		{
			// 当布局接口停止使用时，同时删除扩展
			if (__cache.layout)
			{
				DetachExtension(__cache.layout);
				//__cache.layout->Unuse(Handle());
				__cache.layout = NULL;
			}

			__cache.layout = pLayout;

			// 当布局接口被使用时，同时添加扩展
			if (__cache.layout)
				AttachExtension(__cache.layout);
				//__cache.layout->Use(Handle());
		}
	}
	return __cache.layout;
}


DUI_EVENTTYPE DuiNode::NameToEventType(LPCOLESTR lpszEventName)
{
	if (HIWORD(lpszEventName) == 0)
	{
		DUI_EVENTTYPE et = (DUI_EVENTTYPE)(UINT_PTR)(lpszEventName);
		if (et>DUIET__FIRST && et<DUIET__LAST) return et;
		return DUIET__INVALID;
	}

#define _event_type(x) {L"on" L#x, lstrlenW(L"on" L#x), DUIET_##x},
	static const struct {LPCOLESTR name; int len; int et;} _et_entry[] = {
		__foreach_event_type(_event_type)
	};
#undef _event_type

	int len = lstrlenW(lpszEventName);
	for (int i=0; i<sizeof(_et_entry)/sizeof(_et_entry[0]); i++)
	{
		if (len==_et_entry[i].len && lstrcmpiW(lpszEventName, _et_entry[i].name)==0) // found
			return (DUI_EVENTTYPE)_et_entry[i].et;
	}
	return DUIET__INVALID;
}

LPCOLESTR DuiNode::NameToFinalName(LPCOLESTR lpszEventName)
{
#define _event_type(x) {DUIET_##x, L"on" L#x},
	static const struct {int et; LPCOLESTR name;} _et_entry[] = {
		__foreach_event_type(_event_type)
	};
#undef _event_type

	DUI_EVENTTYPE et = DuiNode::NameToEventType(lpszEventName);

	if (HIWORD(lpszEventName) && et==DUIET__INVALID)
		return lpszEventName;

	for (int i=0; i<sizeof(_et_entry)/sizeof(_et_entry[0]); i++)
	{
		if (_et_entry[i].et == et)
			return _et_entry[i].name;
	}
	return NULL;
}

IVisual* DuiNode::GetVisual()
{
	if (!__cache.hasVisual)
	{
		__cache.hasVisual = TRUE;
		__cache.visual = NULL;
		LPCOLESTR szName = FromStyleVariant(GetStyleAttribute(SAT_VISUAL, SVT_EMPTY, DUISTYLEMATCH_NAME/*_ID*/), LPCOLESTR);
		if (szName)
			PluginCallContext(TRUE).Output(&__cache.visual).Call(OpCode_QueryVisual, szName);
	}
	return __cache.visual;
}

LPCOLESTR DuiNode::Rects::specChars = L"&_";

void DuiNode::AddRect( LPCRECT pRect, LPCOLESTR psz, LONG num, InDrawData& idd )
{
	RgnRect rr = {*pRect, psz, num};
	BOOL b = __cache.rects.Add(rr);

	if (b && idd.pvReserved)
	{
		RowItems& row = *(RowItems*)idd.pvReserved;
		RowItem ri = {this, __cache.rects.GetSize()-1};
		row.Add(ri);
	}
}

void DuiNode::AddSpecChar(LPCRECT pRect, OLECHAR c, InDrawData& idd)
{
	LPCOLESTR p = Rects::specChars;
	while (*p && *p!=c) p++;
	if (*p)
	{
		RgnRect rr = {*pRect, p, 1};
		BOOL b = __cache.rects.Add(rr);

		if (b && idd.pvReserved)
		{
			RowItems& row = *(RowItems*)idd.pvReserved;
			RowItem ri = {this, __cache.rects.GetSize()-1};
			row.Add(ri);
		}
	}
}

HRGN DuiNode::UpdateRgnFromRects()
{
	if (__cache.rects.GetSize()==0) return (m_hRgn);
	if ((m_hRgn)) DeleteObject((m_hRgn));
	(m_hRgn) = NULL;

	size_t nSize = sizeof(RECT) * __cache.rects.GetSize();
	RGNDATAHEADER* pHead = (RGNDATAHEADER*)NEW BYTE[sizeof(RGNDATAHEADER) + nSize];
	if (pHead==NULL) return (m_hRgn);
	ZeroMemory(pHead, sizeof(RGNDATAHEADER));
	pHead->dwSize = sizeof(RGNDATAHEADER);
	pHead->iType = RDH_RECTANGLES;
	pHead->nCount = __cache.rects.GetSize();
	LPRECT pRect = (LPRECT)(pHead + 1);
	for (LONG i=0, num=__cache.rects.GetSize(); i<num; i++)
		pRect[i] = __cache.rects[i].rc;
	//memcpy_s(pRect, nSize, __cache.rects.GetData(), nSize);
	//__cache.rects.RemoveAll();

	(m_hRgn) = ExtCreateRegion(NULL, sizeof(RGNDATAHEADER)+nSize, (const RGNDATA*)pHead);

	delete[] (BYTE*)pHead;
	return (m_hRgn);
}

RECT DuiNode::UpdateRectFromRgn(HRGN hrgn/*=NULL*/)
{
	if (hrgn==NULL) hrgn = (m_hRgn);
	if (hrgn)
	{
		::GetRgnBox(hrgn, &(m_rcItem));
		//m_scrollBar.SetScrollRange(m_rcItem.bottom - m_rcItem.top);
		__cache.hasOffset = FALSE;
	}
	return (m_rcItem);
}

void DuiNode::UpdateDrawData(InDrawData& idd)
{
	if (idd.pt.x>idd.rc.left+idd.lLineIndent && IsBlock())
	{
		ReposInlineRow(idd);
		idd.pt.x = idd.rc.left + idd.lLineIndent;
		idd.pt.y += idd.cyMinHeight + idd.lLineDistance;
		idd.cyTotalHeight += idd.cyMinHeight + idd.lLineDistance;
		idd.cyMinHeight = idd.cyCurLine;
	}
}

HRGN DuiNode::CombineChildrenRegion()
{
	HRGN dst = CreateRectRgn(0,0,0,0);
	ATLASSERT(dst);
	for (int i=0; i<m_children.GetSize(); i++)
	{
		DuiNode* pCtrl = (DuiNode*)m_children[i];
		if (pCtrl) ::CombineRgn(dst, dst, (pCtrl)->GetRegion(), RGN_OR);
	}
	return dst;
}

void DuiNode::SetObject( IDispatch* disp )
{
	m_disp = disp;
}

IDispatch* DuiNode::GetObject( BOOL bAutoCreate/*=FALSE*/ )
{
	if (m_disp==NULL && bAutoCreate)
		m_pLayoutMgr->CreateControlDispatch(this);
	return m_disp;
}

DuiNode* DuiNode::GetRoot()
{
	return m_pLayoutMgr->m_pRoot;
}

BOOL DuiNode::SetInnerXML( LPCOLESTR szXml )
{
	if (CLASS_CAST(this, CDuiInlineText))
		return SetText(szXml), TRUE;

	CComBSTR code = L"<root>";
	code.Append(szXml);
	code.Append(L"</root>");
	CMarkup xml(code);
	if (!xml.IsValid()) return FALSE;
	CMarkupNode root = xml.GetRoot();
	if (!root.IsValid()) return FALSE;

	{
		CounterLock cl(m_pLayoutMgr->m_lPaintLocked);
		RemoveAllChildren();
		for (CMarkupNode node=root.GetChild(FALSE); node.IsValid(); node=node.GetSibling(FALSE))
		{
			m_pLayoutMgr->_AddChild(&node, this);
		}

		for (int i=0,num=GetChildCount(); i<num; i++)
		{
			DuiNode* r = GetChildControl(i);
			r->SetReadyState(READYSTATE_INTERACTIVE);
			r->SetReadyState(READYSTATE_COMPLETE);
		}
	}
	UpdateLayout();
	return TRUE;
}

BOOL DuiNode::SetOuterXML(LPCOLESTR szXml)
{
	DuiNode* pParent = GetParent();
	if (GetRoot()==this || pParent==NULL) return FALSE; // 根元素或者临时元素不能设置 outerXML

	DuiNode* r = (pParent);
	DuiNode* pNewCtrl = m_pLayoutMgr->CreateControl(TempParent, szXml);
	if (pNewCtrl==NULL) return FALSE;

	{
		CounterLock cl(m_pLayoutMgr->m_lPaintLocked);
		if (!r->AddChildControl(pNewCtrl, this)) return FALSE;
		r->RemoveChildControl(this);
		DuiNode* r2 = (pNewCtrl);
		r2->SetReadyState(READYSTATE_INTERACTIVE);
		r2->SetReadyState(READYSTATE_COMPLETE);
	}
	m_pLayoutMgr->UpdateLayout();
	return TRUE;
}

void DuiNode::AttachExtension( IExtension* pExt )
{
	if (!pExt) return;
	if (m_aExtensions.Find(pExt) >= 0) return;
	m_aExtensions.Add(pExt);
	pExt->Use(Handle());
}

void DuiNode::DetachExtension( IExtension* pExt )
{
	if (!pExt) return;
	if (m_aExtensions.Find(pExt) >= 0)
	{
		pExt->Unuse(Handle());
		m_aExtensions.Remove(pExt);
		m_customData.DeleteKey(pExt); // 删除此扩展可能创建的用户数据
	}
}

void DuiNode::OnAttributeChanged( LPCOLESTR szAttr )
{
	LockReenter(OnAttributeChanged);

	BOOL bResizeNeeded = FALSE;
	if (lstrcmpiW(szAttr, L"text") == 0)
		bResizeNeeded = TRUE;
	else if (lstrcmpiW(szAttr, L"dblclick_enable") == 0)
		m_dblclick_enable = _attributes.GetAttributeBool(szAttr, FALSE);

	CComVariant v = _attributes.GetAttribute(szAttr);
	//if (v.vt == VT_BSTR)
	//if (SUCCEEDED(v.ChangeType(VT_BSTR/*, &value*/)))
	{
		if( lstrcmpiW(szAttr, _T("enabled")) == 0 ) SetEnabled((v.vt==VT_BSTR)?ParseBoolString(v.bstrVal):SUCCEEDED(v.ChangeType(VT_BOOL))?v.boolVal!=0:FALSE);
		else if( lstrcmpiW(szAttr, _T("visible")) == 0 ) SetVisible((v.vt==VT_BSTR)?ParseBoolString(v.bstrVal):SUCCEEDED(v.ChangeType(VT_BOOL))?v.boolVal!=0:FALSE);
		else if( lstrcmpiW(szAttr, _T("shortcut")) == 0 && v.vt==VT_BSTR) SetShortcut(v.bstrVal[0]);
		else if( lstrcmpiW(szAttr, _T("style")) == 0  && v.vt==VT_BSTR) ParseStyle(v.bstrVal);
		else if (lstrcmpiW(szAttr, L"filter")==0 && v.vt==VT_BSTR) SetFilter(GetAttributeString(L"filter"));
		else if( !TryParseAsStyle(szAttr, v) && v.vt==VT_BSTR)
			TryParseAsProcedure(szAttr, v.bstrVal);
	}

	if (lstrcmpiW(szAttr, L"background") == 0)
	{
		__cache.imgBkgnd.Clear();

		struct scb : public CDuiConnectorCallback
		{
		public:
			BOOL OnHit(LPVOID pClient, LPVOID pServer, DWORD_PTR protocol)
			{
				DuiNode* r = (DuiNode*)pClient;
				r->__cache.imgBkgnd.Clear();
#ifndef NO3D
				r->Update3DState();
#endif
				r->Invalidate();
				return FALSE;
			}
		};
		static scb _cb;
		gConnector.Disconnect(this, NULL, cp_img_loaded);
		CDuiImageResource* pRes = m_pLayoutMgr->GetImageResource(GetAttributeString(szAttr));
		if (pRes)
			gConnector.Connect(this, pRes->img, cp_img_loaded, &_cb);
	}
	else if (lstrcmpiW(szAttr, L"foreground") == 0)
	{
		struct scb : public CDuiConnectorCallback
		{
		public:
			BOOL OnHit(LPVOID pClient, LPVOID pServer, DWORD_PTR protocol)
			{
				DuiNode* r = (DuiNode*)pClient;
#ifndef NO3D
				r->Update3DState();
#endif
				r->Invalidate();
				return FALSE;
			}
		};
		static scb _cb;
		gConnector.Disconnect(this, NULL, cp_img_loaded_foreground);
		CDuiImageResource* pRes = m_pLayoutMgr->GetImageResource(GetAttributeString(szAttr));
		if (pRes)
			gConnector.Connect(this, pRes->img, cp_img_loaded_foreground, &_cb);
	}
	else if (lstrcmpiW(szAttr, L"style")==0 || lstrcmpiW(szAttr, L"overflow")==0)
	{
		m_scrollBar.ShowScrollbar(IsAllowScroll() == -1); // 强迫显示，仅垂直滚动条
	}

	InvokeAllSinks(this, OnAttributeChanged, szAttr);
	//for (int i=0; i<m_aExtensions.GetSize(); i++)
	//{
	//	CDuiExtension* pExt = (CDuiExtension*)m_aExtensions[i];
	//	ATLASSERT(pExt);
	//	CDuiSinkOnAttributeChanged* pss = CLASS_CAST(pExt, CDuiSinkOnAttributeChanged);
	//	if (pss) pss->OnAttributeChanged(szAttr);
	//}

	if (bResizeNeeded)
		DelayedResize();
	else
	{
		if (m_readyState==READYSTATE_COMPLETE)
			Invalidate();
#ifndef NO3D
		Update3DState();
#endif
	}
}

void DuiNode::OnChildAttributeChanged( DuiNode* pChild, LPCOLESTR szAttr )
{
	LockReenter(OnChildAttributeChanged);

	InvokeAllSinks(this, OnChildAttributeChanged, pChild->Handle(), szAttr);
	//for (int i=0; i<m_aExtensions.GetSize(); i++)
	//{
	//	CDuiExtension* pExt = (CDuiExtension*)m_aExtensions[i];
	//	ATLASSERT(pExt);
	//	CDuiSinkOnChildAttributeChanged* pss = CLASS_CAST(pExt, CDuiSinkOnChildAttributeChanged);
	//	if (pss) pss->OnChildAttributeChanged(pChild, szAttr);
	//}
}

void DuiNode::OnAttributeRemoved(LPCOLESTR szAttr)
{
	LockReenter(OnAttributeRemoved);

	if( lstrcmpiW(szAttr, _T("enabled")) == 0 ) SetEnabled(TRUE);
	else if( lstrcmpiW(szAttr, _T("visible")) == 0 ) SetVisible(TRUE);
	else if( lstrcmpiW(szAttr, _T("shortcut")) == 0 ) SetShortcut(0);
	else if (lstrcmpiW(szAttr, L"filter")==0) SetFilter(NULL);
	else TryRemoveAsStyle(szAttr);

	InvokeAllSinks(this, OnAttributeRemoved, szAttr);
	//for (int i=0; i<m_aExtensions.GetSize(); i++)
	//{
	//	CDuiExtension* pExt = (CDuiExtension*)m_aExtensions[i];
	//	ATLASSERT(pExt);
	//	CDuiSinkOnAttributeRemoved* pss = CLASS_CAST(pExt, CDuiSinkOnAttributeRemoved);
	//	if (pss) pss->OnAttributeRemoved(szAttr);
	//}

	if (lstrcmpiW(szAttr, L"background") == 0)
	{
		__cache.imgBkgnd.Clear();
	}
#ifndef NO3D
	Update3DState();
#endif
	Invalidate();
}

void DuiNode::OnChildAttributeRemoved(DuiNode* pChild, LPCOLESTR szAttr)
{
	LockReenter(OnChildAttributeRemoved);

	InvokeAllSinks(this, OnChildAttributeRemoved, pChild->Handle(), szAttr);
	//for (int i=0; i<m_aExtensions.GetSize(); i++)
	//{
	//	CDuiExtension* pExt = (CDuiExtension*)m_aExtensions[i];
	//	ATLASSERT(pExt);
	//	CDuiSinkOnChildAttributeRemoved* pss = CLASS_CAST(pExt, CDuiSinkOnChildAttributeRemoved);
	//	if (pss) pss->OnChildAttributeRemoved(pChild, szAttr);
	//}
}

void DuiNode::OnStyleChanged( CDuiStyleVariant* newVal )
{
	LockReenter(OnStyleChanged);

	// 删除所有CACHE
	// 
	for (int i=__cache.styles.GetSize()-1; i>=0; i--)
	{
		StyleCacheItem& item = __cache.styles[i];
		if (item.sat==newVal->sat && item.val!=newVal)
			__cache.styles.RemoveAt(i);
	}

	switch (newVal->sat)
	{
	case SAT_WIDTH:
	case SAT_HEIGHT:
	case SAT_MINWIDTH:
	case SAT_MINHEIGHT:
	case SAT_LEFT:
	case SAT_RIGHT:
	case SAT_TOP:
	case SAT_BOTTOM:
		{
			__cache.hasOffset = FALSE;
			__cache.hasPos = __cache.hasClientPos = __cache.hasClipPos = FALSE;
			DUI_POSITION pos = GetPosition();
			if (pos>DUIPOS_DEFAULT && pos<DUIPOS_FIXED)
			{
				if (FromStyleVariant(__cache.offset.parent->GetStyleAttribute(SAT_OVERFLOW_INCLUDE_OFFSET), BOOL) &&
					__cache.offset.parent->IsAllowScroll())
					__cache.offset.parent->__cache.updateScroll = TRUE;
			}
		}
		//if (m_pSurface)
		//{
		//	m_pSurface->Parent(__cache.offset.parent ? __cache.offset.parent->m_pSurface : NULL);
		//	m_pSurface->Move(&CDuiControlExtension::GetClientRect(_ctrl,DUIRT_TOTAL));
		//}
		break;
	case SAT_POSITION:
		{
			__cache.hasPos = __cache.hasClientPos = __cache.hasClipPos = FALSE;
			__cache.hasPosition = FALSE;
			DUI_POSITION pos = GetPosition();
			if (pos) addToList(__cache.offset.parent ? &__cache.offset.parent->m_offsetChildren : &m_pLayoutMgr->m_fixedChildren);
			else removeFromList();

			//if (m_pSurface)
			//{
			//	m_pSurface->Parent(__cache.offset.parent ? __cache.offset.parent->m_pSurface : NULL);
			//	m_pSurface->Move(&CDuiControlExtension::GetClientRect(_ctrl,DUIRT_TOTAL));
			//}
		}
		break;
	case SAT_ZINDEX:
		{
			__cache.hasZIndex = FALSE;
			DUI_POSITION pos = GetPosition();
			removeFromList(); // 因为Z序变化，必须先从列表中删除再重新添加
			if (pos) addToList(__cache.offset.parent ? &__cache.offset.parent->m_offsetChildren : &m_pLayoutMgr->m_fixedChildren);

			//if (m_pSurface)
			//	m_pSurface->ZIndex(GetZIndex());
		}
		break;
	case SAT_CLIP:
		{
#ifndef NO3D
			m_pathClip.Parse(FromStyleVariant(newVal, LPCOLESTR));
#endif
		}
		break;
	case SAT_VISUAL:
		ResetVisual();
		break;
	case SAT_LAYOUT:
		ResetLayout();
		break;
	}

	InvokeAllSinks(this, OnStyleChanged, newVal);
	//Update3DState();
	Invalidate();
}

#ifndef NO3D
void DuiNode::Update3DState()
{
	if (m_pSurface == NULL || m_readyState<READYSTATE_COMPLETE) return;

	// state
	m_pSurface->State(ost_visible, IsVisible(FALSE));
	m_pSurface->State(ost_enable, IsEnabled(FALSE));
	m_pSurface->ZIndex(GetZIndex());
	m_pSurface->Parent(__cache.offset.parent ? __cache.offset.parent->m_pSurface : NULL, GetPosition()!=DUIPOS_DEFAULT);
	m_pSurface->Move(&GetClientRect(DUIRT_BORDER));
	m_pSurface->ClipContent(&GetClipRect(DUIRT_CONTENT));
	m_pSurface->SetScene(m_pScene);
	m_pSurface->SetAsTarget(_attributes.GetAttributeBool(L"3d-target"));
	//m_pSurface->SetName(GetID());

	// background
	CDuiStyleVariant* sv = (CDuiStyleVariant*)GetStyleAttribute(SAT_BACKGROUND);
	if (sv && sv!=&svEmpty)
	{
		if (sv->svt == SVT_COLOR)
		{
			COLORREF clr = FromStyleVariant(sv, COLORREF);
			if ((clr>>24) == 0)
			{
				BYTE bAlpha = 255;
				CComVariant v = GetAttribute(L"background-alpha");
				if (v.vt!=VT_EMPTY && SUCCEEDED(v.ChangeType(VT_UI1)))
					bAlpha = (BYTE)V_UI1(&v);
				clr = (clr) | (bAlpha<<24);
			}

			m_pSurface->Background()->SetColor(clr);
		}
		else if (sv->svt == SVT_EXTERNAL)
		{
			CDuiImageResource* pSrc = (CDuiImageResource*)sv->extVal;
			if (pSrc)
			{
				ExternalType et = (ExternalType)sv->__cache;
				m_pSurface->Background()->SetImage(pSrc->img3d, et);
			}
		}
	}
	else
		m_pSurface->Background()->Clear();
		//m_pSurface->Background()->SetColor(0); // 清理背景

	// foreground
	sv = (CDuiStyleVariant*)GetStyleAttribute(SAT_FOREGROUND);
	if (sv && sv!=&svEmpty)
	{
		if (sv->svt == SVT_COLOR)
		{
			COLORREF clr = FromStyleVariant(sv, COLORREF);
			if ((clr>>24) == 0)
			{
				BYTE bAlpha = 255;
				CComVariant v = GetAttribute(L"foreground-alpha");
				if (v.vt!=VT_EMPTY && SUCCEEDED(v.ChangeType(VT_UI1)))
					bAlpha = (BYTE)V_UI1(&v);
				clr = (clr) | (bAlpha<<24);
			}

			m_pSurface->Foreground()->SetColor(clr);
		}
		else if (sv->svt == SVT_EXTERNAL)
		{
			CDuiImageResource* pSrc = (CDuiImageResource*)sv->extVal;
			if (pSrc)
			{
				ExternalType et = (ExternalType)sv->__cache;
				m_pSurface->Foreground()->SetImage(pSrc->img3d, et);
			}
		}
	}
	else
		m_pSurface->Foreground()->Clear();
	//	m_pSurface->Foreground()->SetColor(0); // 清理前景
}
#endif

void DuiNode::ProcessFont(HFONT hFont)
{
	//if (m_pLayoutMgr->Get3DDevice()) // ddraw::GetDC获得的DC不能对文本反锯齿，必须特殊处理一下HFONT
	//{
	//	CClientDC dcScreen(NULL);
	//	HFONT oldFont = dcScreen.SelectFont(hFont);
	//	dcScreen.SelectFont(oldFont);
	//}
}

void CALLBACK DuiNode::TransformProc( LPVOID pData, DWORD dwState )
{
	if (pData)
	{
		//((CDuiControlExtensionBase*)pData)->Invalidate();
		DuiNode* r = (DuiNode*)pData;
		CDuiLayoutManager* pMgr = r->m_pLayoutMgr;
		////pMgr->Invalidate(((CDuiControlExtensionBase*)pData)->GetPos(DUIRT_TOTAL), TRUE);
		RECT rc = r->GetPos(DUIRT_TOTAL);
		CSurface* psurf = pMgr->GetSurface();
		HDC hdc = psurf->GetDC();
		{
			CRenderContentOrg orgParent(hdc, r->GetOffsetParent());
			r->_dxt->OnPaint(hdc, rc, 0);
		}
		psurf->ReleaseDC(hdc);
		psurf->BltToPrimary(&rc);
		if (dwState == 1) r->Invalidate();
	}
}

POINT DuiNode::GetContentOrg(DuiNode* pRef/*=NULL*/)
{
	POINT ptOrg = {0};
	return DuiNode::GetOffset(pRef, this, ptOrg);
}

POINT CALLBACK DuiNode::GetParentOrg(LPVOID lpData)
{
	DuiNode* pCtrl = (DuiNode*)lpData;
	if (pCtrl==NULL) return CPoint(0,0);
	DuiNode* parent = pCtrl->GetOffsetParent();
	if (parent==NULL) return CPoint(0,0);
	return parent->GetContentOrg();
}

void CALLBACK DuiNode::OnAttributeChanged( BOOL bDeleted, LPCOLESTR szAttr, LPVOID pData )
{
	DuiNode* r = (DuiNode*)pData;
	if (r)
	{
		//if (r->m_pLayoutMgr->m_lPaintLocked>0) return;

		if (bDeleted) r->OnAttributeRemoved(szAttr);
		else r->OnAttributeChanged(szAttr);

		DuiNode* parent = r->GetParent();
		if (parent)
		{
			if (bDeleted) parent->OnChildAttributeRemoved(r, szAttr);
			else parent->OnChildAttributeChanged(r, szAttr);
		}

		// 内联文本的事件都转移给父控件
		if (CLASS_CAST(r, CDuiInlineText))
			r = parent;
		if (r && (r)->m_readyState>=READYSTATE_INTERACTIVE)
		{
			DuiEvent event = {event_type_to_name(DUIET_propertychange)};
			event.pSender = DuiHandle<DuiNode>(r);
			event.pPropertyName = szAttr;
			(r)->m_pLayoutMgr->FireEvent(r, event);
		}
	}
}

HDE DuiNode::Handle()
{
	return DuiHandle<DuiNode>(this);
}

DuiNode* DuiNode::FromHandle( HDE h )
{
	DuiNode* n = DuiHandle<DuiNode>(h);
	if (n && n->_sign==DuiNode::__sign)
		return n;
	return NULL;
}

BOOL DuiNode::_tryOpEnter( DWORD opcode )
{
	if (_invocations.Find(opcode) >= 0)
		return FALSE;
	return _invocations.Add(opcode);
}

void DuiNode::_OpLeave( DWORD opcode )
{
	_invocations.Remove(opcode);
}

HRGN DuiNode::GetClipRgnBorder()
{
	//struct DefCallback
	//{
	//	static HRGN GetClipRegion(CallContext* pcc, HDE hde, RECT rcBorder)
	//	{
	//		//DuiNode* n = DuiNode::FromHandle(hde);
	//		return NULL;
	//	}
	//};

	HRGN hrgn = NULL;
	InvokeOneSinkResult(hrgn, this, GetClipRegion, GetPos(DUIRT_BORDER));
	return hrgn;
}

HRGN DuiNode::GetClipRgnChildren()
{
	//struct DefCallback
	//{
	//	static HRGN GetChildrenClipRegion(CallContext* pcc, HDE hde, RECT rcBorder)
	//	{
	//		//DuiNode* n = DuiNode::FromHandle(hde);
	//		return NULL;
	//	}
	//};

	HRGN hrgn = NULL;
	InvokeOneSinkResult(hrgn, this, GetChildrenClipRegion, GetPos(DUIRT_CONTENT));
	return hrgn;
}

T2C::T2C( DuiNode* ctrl, LONG cx, LONG cy )
{
	RECT rc = {0,0,cx,cy};
	rc = ctrl->GetPosBy(rc);
	_sz.cx = rc.right - rc.left;
	_sz.cy = rc.bottom - rc.top;
}

C2T::C2T( DuiNode* ctrl )
{
	SIZE sz = ctrl->GetContentSize();
	_sz = ctrl->AdjustSize(sz, DUIRT_TOTAL);
}

C2T::C2T( DuiNode* ctrl, LONG cx, LONG cy )
{
	SIZE sz = {cx,cy};
	_sz = (ctrl)->AdjustSize(sz, DUIRT_TOTAL);
}

BOOL CAttributeMap::GetAttributeBool( LPCOLESTR szAttr, BOOL bDef/*=FALSE*/ ) const
{
	CComVariant v = GetAttribute(szAttr);
	if (v.vt == VT_BSTR) return ParseBoolString(v.bstrVal);
	return (v.vt!=VT_EMPTY && SUCCEEDED(v.ChangeType(VT_BOOL))) ? (V_BOOL(&v) != VARIANT_FALSE) : bDef;
}

//////////////////////////////////////////////////////////////////////////
CRenderClip::CRenderClip( HDC hdc, LPCRECT rc, HRGN rgn/*=NULL*/
#ifndef NO3D
						 , GdiPath* path/*=NULL*/ 
#endif // NO3D
						 ) : hDC(hdc), hOldRgn(NULL), hRgn(rgn)
{
	if (rc==NULL && hRgn==NULL)
		return;

	hOldRgn = ::CreateRectRgn(0,0,1,1);
	::GetClipRgn(hDC, hOldRgn);

	if (hRgn==NULL)
		rcItem = *rc;

	POINT pt = {0};
	::GetWindowOrgEx(hDC, &pt);

	//RECT rcClip = { 0 };
	//::GetClipBox(hDC, &rcClip);
	//::OffsetRect(&rcClip, -pt.x, -pt.y);
	//hOldRgn = ::CreateRectRgnIndirect(&rcClip);

#ifndef NO3D
	if (path && path->IsValid() && ::BeginPath(hDC))
	{
		path->Draw(hDC, rcItem);
		BOOL b=::EndPath(hDC);
		b=::SelectClipPath(hDC, RGN_AND);
		//b = ::StrokePath(hDC);
		//if (!b)
		//{
		//	DWORD dw = GetLastError();
		//	ATLASSERT(1);
		//}
	}
	else
#endif // NO3D
	if (hRgn)
	{
		::OffsetRgn(hRgn, -pt.x, -pt.y);
		::ExtSelectClipRgn(hDC, hRgn, RGN_AND);
	}
	else
	{
		::OffsetRect(&rcItem, -pt.x, -pt.y);
		hRgn = ::CreateRectRgnIndirect(&rcItem);
		::ExtSelectClipRgn(hDC, hRgn, RGN_AND);
	}
}

CRenderClip::~CRenderClip()
{
	if (hDC && hOldRgn) ::SelectClipRgn(hDC, hOldRgn);
	if (hOldRgn) ::DeleteObject(hOldRgn);
	if (hRgn) ::DeleteObject(hRgn);
}