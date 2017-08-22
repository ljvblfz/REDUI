#include "StdAfx.h"
#include "DuiLayout.h"
#include "DuiLayoutMode.h"


void CDuiLayout::SetAttribute( HDE hde, LPCWSTR szName, VARIANT* value )
{
	if (lstrcmpiW(szName, L"type") == 0) szName = L"layout";
	__super::SetAttribute(hde, szName, value);
}

DuiStyleVariant* CDuiLayoutSplitter::GetStyleAttribute( HDE hde, DuiStyleAttributeType sat, DuiStyleVariantType svt, DWORD dwMatchMask )
{
	if (sat == SAT_CURSOR)
	{
		LayoutSplitter* pModeSplitter = CLASS_CAST(m_pOwner, LayoutSplitter);
		if (pModeSplitter && (pModeSplitter->GetState(m_pOwner->Handle()) & (DUISTATE_CAPTURED | DUISTATE_HOT)) == 0)
			return &svEmpty;
	}

	return __super::GetStyleAttribute(hde, sat, svt, dwMatchMask);
}