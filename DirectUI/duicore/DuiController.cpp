#include "stdafx.h"
#include "DuiController.h"
#include "DuiPluginsManager.h"


//////////////////////////////////////////////////////////////////////////
//
void CDuiController::OnConnect()
{
	ATLASSERT(m_pOwner);
	if (GetDispatch())
	{
		CComQIPtr<IPeerHolder> peer = (m_pOwner)->GetObject(TRUE);
		if (peer.p)
		{
			peer->AttachObject(GetDispatch());
		}
	}
}

CDuiController::~CDuiController()
{
	if (m_pOwner && GetDispatch())
	{
		CComQIPtr<IPeerHolder> peer = (m_pOwner)->GetObject(TRUE);
		if (peer.p)
		{
			peer->DetachObject(GetDispatch());
		}
	}
}

//////////////////////////////////////////////////////////////////////////
//
int CDuiControllerListBase::GetItemIndex( DuiNode* pItem )
{
	return (m_pOwner)->m_children.Find(pItem);
}

int CDuiControllerListBase::GetColumnCount() const
{
	return m_ColWidths.GetSize();
}

int CDuiControllerListBase::GetColumnWidth( int iIndex )
{
	if (iIndex<0 || iIndex>=m_ColWidths.GetSize()) return 0;
	return m_ColWidths[iIndex];
}

//////////////////////////////////////////////////////////////////////////
//
BOOL CDuiControllerListItemBase::IsSelected() const
{
	return (m_pOwner)->GetState() & DUISTATE_SELECTED;
}

void CDuiControllerListItemBase::select()
{
	(m_pOwner)->ModifyState(DUISTATE_SELECTED, 0);
}

void CDuiControllerListItemBase::unselect()
{
	(m_pOwner)->ModifyState(0, DUISTATE_SELECTED);
}

HRESULT CDuiControllerListItemBase::Getselected( VARIANT* pVar )
{
	V_VT(pVar) = VT_BOOL;
	V_BOOL(pVar) = (IsSelected() ? VARIANT_TRUE : VARIANT_FALSE);
	return S_OK;
}

HRESULT CDuiControllerListItemBase::Putselected( VARIANT* pVar )
{
	CComVariant v;
	HRESULT hr = v.ChangeType(VT_BOOL, pVar);
	if (SUCCEEDED(hr))
	{
		if (v.boolVal) select();
		else unselect();
	}
	return hr;
}

//////////////////////////////////////////////////////////////////////////
//
BOOL CDuiControllerList::OnChildStateChanged( HANDLE, HDE pChild, DWORD dwStateMask )
{
	DuiNode* child = DuiHandle<DuiNode>(pChild);
	if ((m_pOwner->GetControlFlags()&DUIFLAG_NOCHILDREN) == 0)
	if ((dwStateMask & DUISTATE_SELECTED) && ((child)->GetState() & DUISTATE_SELECTED))
	{
		for (int i=0, num=m_pOwner->GetChildCount(); i<num; i++)
		{
			DuiNode* p = m_pOwner->GetChildControl(i);
			if (child == p) continue;
			(p)->ModifyState(0, DUISTATE_SELECTED);
		}
	}
	return FALSE;
}

DuiNode* CDuiControllerList::GetCurSel() const
{
	if ((m_pOwner->GetControlFlags()&DUIFLAG_NOCHILDREN) == 0)
	{
		for (int i=0, num=m_pOwner->GetChildCount(); i<num; i++)
		{
			DuiNode* p = m_pOwner->GetChildControl(i);
			if (p && ((p)->GetState()&DUISTATE_SELECTED)) return p;
		}
	}
	return NULL;
}

HRESULT CDuiControllerList::Getselection(VARIANT* pVar)
{
	DuiNode* pCtrl = GetCurSel();
	if (pCtrl)
	{
		V_VT(pVar) = VT_DISPATCH;
		V_DISPATCH(pVar) = (pCtrl)->GetObject(TRUE);
	}
	return S_OK;
}

//////////////////////////////////////////////////////////////////////////
//
CDuiControllerListMulti::CDuiControllerListMulti() : m_pSelections(NULL)
{
}

HRESULT CDuiControllerListMulti::Getselections(VARIANT* pVar)
{
	if (!m_pSelections)
	{
		V_VT(pVar) = VT_DISPATCH;
		HRESULT hr = m_pSelections->CreateInstance(&V_DISPATCH(pVar), &m_pSelections);
		if (m_pSelections)
		{
			//m_pSelections->SetExternalRef((LPVOID*)&m_pSelections);
			UpdateSelection();
		}
		return hr;
	}

	V_VT(pVar) = VT_DISPATCH;
	V_DISPATCH(pVar) = m_pSelections->GetDispatch();
	if (V_DISPATCH(pVar)) V_DISPATCH(pVar)->AddRef();
	return S_OK;
}

void CDuiControllerListMulti::UpdateSelection(DuiNode* pExclude/*=NULL*/)
{
	if (m_pSelections)
	{
		m_pSelections->RemoveAll();

		if ((m_pOwner->GetControlFlags()&DUIFLAG_NOCHILDREN) == 0)
		{
			DuiNode* r = (m_pOwner);
			for (int i=0, num=r->GetChildCount(); i<num; i++)
			{
				DuiNode* rtChild = (r->GetChildControl(i));
				if ((rtChild->GetState()&DUISTATE_SELECTED) && (r->GetChildControl(i)!=pExclude))
				{
					m_pSelections->Add(rtChild->GetID(), rtChild->GetObject(TRUE));
				}
			}
		}
	}
}

BOOL CDuiControllerListMulti::OnChildStateChanged( HANDLE, HDE pChild, DWORD dwStateMask )
{
	if ((dwStateMask & DUISTATE_SELECTED))
		UpdateSelection();
	return FALSE;
}

void CDuiControllerListMulti::OnChildAdded( HANDLE, HDE pChild )
{
	UpdateSelection();
}

void CDuiControllerListMulti::OnChildRemoved( HANDLE, HDE pChild )
{
	UpdateSelection(DuiHandle<DuiNode>(pChild));
}