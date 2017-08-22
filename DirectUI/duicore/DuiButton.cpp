
#include "StdAfx.h"
#include "DuiButton.h"
#include "../../common/theme.h"

/////////////////////////////////////////////////////////////////////////////////////
//
//

//void CDuiButton::SetText(LPCOLESTR pstrText)
//{
//	CDuiControlExtension::SetText(pstrText);
//	// Automatic assignment of keyboard shortcut
//	if( wcschr(pstrText, L'&') != NULL ) rt(this)->m_chShortcut = *(wcschr(pstrText, L'&') + 1);
//}

BOOL CDuiButton::Activate(HDE hde)
{
	if( !__super::Activate(hde) ) return FALSE;
	if( m_pOwner->m_pLayoutMgr ) m_pOwner->m_pLayoutMgr->SendNotify(m_pOwner, _T("click"));
	return TRUE;
}


/////////////////////////////////////////////////////////////////////////////////////
//
//

BOOL CDuiOption::IsChecked()
{
	return m_pOwner->m_dwState & DUISTATE_CHECKED;
}

void CDuiOption::SetCheck(BOOL bSelected)
{
	if( bSelected == m_pOwner->HasState(DUISTATE_CHECKED) ) return;
	if (bSelected) m_pOwner->ModifyState(DUISTATE_CHECKED, 0);
	else m_pOwner->ModifyState(0, DUISTATE_CHECKED);
	if( m_pOwner->m_pLayoutMgr != NULL )
	{
		m_pOwner->m_pLayoutMgr->SendNotify(m_pOwner, L"changed");
		m_pOwner->Invalidate();
	}
}

BOOL CDuiOption::Activate(HDE hde)
{
	if( !__super::Activate(hde) ) return FALSE;
	SetCheck(TRUE);
	return TRUE;
}

void CDuiOption::SetAttribute(HDE hde, LPCWSTR szName, VARIANT* value)
{
	__super::SetAttribute(hde, szName, value);
	if( lstrcmpiW(szName, L"checked") == 0 ) SetCheck(ParseBoolString(Var2Bstr(CComVariant(*value))));
}

//////////////////////////////////////////////////////////////////////////
//
//

BOOL CDuiCheckBox::Activate(HDE hde)
{
	// 这里不能调用 __super::Activate，否则就调用 CDuiOption::Activate 去了
	if( !ControlExtension::Activate(hde) ) return FALSE;
	SetCheck(!IsChecked());
	return TRUE;
}

void CDuiCheckBox::SetCheck( BOOL bSelected )
{
	if( bSelected == m_pOwner->HasState(DUISTATE_CHECKED) ) return;
	if (bSelected) m_pOwner->ModifyState(DUISTATE_CHECKED, 0);
	else m_pOwner->ModifyState(0, DUISTATE_CHECKED);
	if( m_pOwner->m_pLayoutMgr != NULL ) m_pOwner->m_pLayoutMgr->SendNotify(m_pOwner, L"changed");
	m_pOwner->Invalidate();
}
