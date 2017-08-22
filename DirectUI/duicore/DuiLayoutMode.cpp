#include "stdafx.h"
#include "DuiLayoutMode.h"
#include "../../common/theme.h"
#include "DuiHelper.h"
#include "DuiPluginsManager.h"


ILayout* LayoutFromName( LPCOLESTR name )
{
	if (name==NULL || *name==0)
		return NULL;

	int len = lstrlenW(name);
	for (LayoutEntry** ppEntry = &__playoutEntryFirst; ppEntry < &__playoutEntryLast; ppEntry++)
	{
		if (*ppEntry != NULL)
		{
			LayoutEntry* pEntry = *ppEntry;
			if ((len==pEntry->lenName && lstrcmpiW(name,pEntry->szName)==0))
			{
				return pEntry->pLayout;
			}
		}
	}
	return NULL;
}

//////////////////////////////////////////////////////////////////////////
class DefLayout : public ILayout
{
	DECLARE_NO_DELETE_THIS(DefLayout)
	DECLARE_BASE_CLASS_CAST(DefLayout)

public:
	virtual BOOL OnUpdateLayout(HDE hde)
	{
		DuiNode* n = DuiHandle<DuiNode>(hde);

		CRect rcTotal = n->GetPos(DUIRT_TOTAL);
		SIZE sz = rcTotal.Size();
		n->EstimateSize(sz);

		return TRUE;
	}
};

DefLayout g_defLayout;

//////////////////////////////////////////////////////////////////////////
// API
ILayout* DUIAPI GetDefaultLayout()
{
	return &g_defLayout;
}

BOOL DUIAPI DefUpdateLayout(HDE hde)
{
	return g_defLayout.OnUpdateLayout(hde);
}



/////////////////////////////////////////////////////////////////////////////////////
//
//

BOOL LayoutVertical::OnUpdateLayout(HDE hde)
{
	DuiNode* node = DuiHandle<DuiNode>(hde);
	CRect rc = node->GetPos();
	rc.MoveToXY(0,0);
	CDuiScrollbar* pBar = node->GetScrollbar(), *pBarH = node->GetScrollbar(FALSE);
	//rc.right -= pBar->GetScrollbarWidth();
	// Determine the minimum size
	LONG minwidth = node->CalcContentMinWidth();
	SIZE szAvailable = { max(rc.Width(), minwidth), rc.Height() };
	int nAdjustables = 0;
	int cyFixed = 0;
	for( int i=0; i<node->GetChildCount(); i++ )
	{
		DuiNode* rr = node->GetChildControl(i);
		if( !rr->IsVisible() || rr->GetPosition()>DUIPOS_RELATIVE ) continue;
		SIZE sz = rr->EstimateSize(szAvailable);
		//if( sz.cy == 0 ) nAdjustables++;
		cyFixed += sz.cy;
	}
	// Place elements
	int cyNeeded = 0;
	int cyExpand = 0;
	if( nAdjustables > 0 )
		cyExpand = max(0, (szAvailable.cy - cyFixed) / nAdjustables);
	// Position the elements
	SIZE szRemaining = szAvailable;
	int iPosY = 0; //rc.top - pBar->GetScrollPos();
	int iAdjustable = 0;
	for( int i=0; i<node->GetChildCount(); i++ )
	{
		DuiNode* rr = node->GetChildControl(i);
		if( !rr->IsVisible() ) continue;
		DisableOp dop(rr, DUIDISABLED_INVALIDATE); // 临时禁止Invalidate操作
		SIZE sz = rr->EstimateSize(szRemaining);
		if (rr->GetPosition() > DUIPOS_RELATIVE) // 设置矩形，但不计入布局
		{
			RECT rcCtrl = { 0, 0, szAvailable.cx, sz.cy };
			rr->SetPos(rcCtrl);
			continue;
		}
		//if( sz.cy == 0 )
		//{
		//	iAdjustable++;
		//	sz.cy = cyExpand;
		//	// Distribute remaining to last element (usually round-off left-overs)
		//	if( iAdjustable == nAdjustables ) sz.cy += max(0, szAvailable.cy - (cyExpand * nAdjustables) - cyFixed);
		//}
		RECT rcCtrl = { 0, iPosY, szAvailable.cx, iPosY + sz.cy };
		rr->SetPos(rcCtrl);
		iPosY += sz.cy;
		cyNeeded += sz.cy;
		szRemaining.cy -= sz.cy;
	}

	// Handle overflow with scrollbars
	// 这里不用再设置滚动条了，在 CDuiControlExtension::UpdateLayout 里面会设置
	//CDuiControlExtension::SetScrollRange(_owner, minwidth, cyNeeded);
	////pBar->SetScrollRange(cyNeeded);
	////pBarH->SetScrollRange(minwidth);
	return TRUE;
}


/////////////////////////////////////////////////////////////////////////////////////
//
//

BOOL LayoutHorizontal::OnUpdateLayout(HDE hde)
{
	DuiNode* rtOwner = DuiHandle<DuiNode>(hde);
	CRect rc = (rtOwner)->GetPos();
	rc.MoveToXY(0,0);
	//CDuiScrollbarBase* pBar = _owner->GetScrollbar(FALSE);

	// Determine the width of elements that are sizeable
	SIZE szAvailable = { rc.right - rc.left, rc.bottom - rc.top };
	int nAdjustables = 0;
	int cxFixed = 0;
	for( int i = 0; i < rtOwner->GetChildCount(); i++ )
	{
		DuiNode* rr = rtOwner->GetChildControl(i);
		if( !rr->IsVisible() || rr->GetPosition()>DUIPOS_RELATIVE ) continue;
		SIZE sz = rr->EstimateSize(szAvailable);
		//if( sz.cx == 0 ) nAdjustables++;
		cxFixed += sz.cx;
	}
	int cxNeeded = 0;
	int cxExpand = 0;
	if( nAdjustables > 0 )
		cxExpand = max(0, (szAvailable.cx - cxFixed) / nAdjustables);
	// Position the elements
	SIZE szRemaining = szAvailable;
	int iPosX = rc.left;
	int iAdjustable = 0;
	for( int i = 0; i < rtOwner->GetChildCount(); i++ )
	{
		DuiNode* rr = rtOwner->GetChildControl(i);
		if( !rr->IsVisible() ) continue;
		DisableOp dop(rr, DUIDISABLED_INVALIDATE); // 临时禁止Invalidate操作
		SIZE sz = rr->EstimateSize(szRemaining);
		if (rr->GetPosition() > DUIPOS_RELATIVE) // 设置矩形，但不计入布局
		{
			RECT rcCtrl = { 0, rc.top, sz.cx, rc.bottom };
			rr->SetPos(rcCtrl);
			continue;
		}
		//if( sz.cx == 0 )
		//{
		//	iAdjustable++;
		//	sz.cx = cxExpand;
		//	if( iAdjustable == nAdjustables ) sz.cx += max(0, szAvailable.cx - (cxExpand * nAdjustables) - cxFixed);
		//}
		RECT rcCtrl = { iPosX, rc.top, iPosX + sz.cx, rc.bottom };
		rr->SetPos(rcCtrl);
		iPosX += sz.cx;
		cxNeeded += sz.cx;
		szRemaining.cx -= sz.cx;
	}

	// Handle overflow with scrollbars
	//pBar->SetScrollRange(cxNeeded);
	return TRUE;
}


/////////////////////////////////////////////////////////////////////////////////////
//
//

//LayoutTile::LayoutTile() : m_nColumns(2), m_cyNeeded(0)
//{
//   SetPadding(10);
//   SetMargin(CSize(10, 10));
//}

BOOL LayoutTile::OnUpdateLayout(HDE hde)
{
	DuiNode* r = DuiHandle<DuiNode>(hde);
	CRect rc = (r)->GetPos();
	rc.MoveToXY(0,0);
	//CDuiScrollbarBase* pBar = _owner->GetScrollbar();
	//rc.right -= pBar->GetScrollbarWidth();

	LONG cy = FromStyleVariant(r->GetStyleAttribute(SAT_HEIGHT), LONG, (LPVOID)(LONG_PTR)r->GetOffsetParentSize().cy); // 是否有固定高度
	if (cy <= 0) cy = FromStyleVariant(r->GetStyleAttribute(SAT_MINHEIGHT), LONG, (LPVOID)(LONG_PTR)r->GetOffsetParentSize().cy);
	LONG nColumns = FromStyleVariant(r->GetStyleAttribute(SAT_COLUMNS), LONG); // 是否有 Columns 属性
	if (nColumns <= 0) nColumns = 2;

	// Position the elements
	int cxWidth = (rc.right - rc.left) / nColumns;
	int cyHeight = 0;
	int iCount = 0;
	int cyNeeded = 0;
	POINT ptTile = { rc.left, rc.top };
	for( int i = 0; i < r->GetChildCount(); i++ )
	{
		DuiNode* rr = r->GetChildControl(i);
		if( !rr->IsVisible() ) continue;
		DisableOp dop(rr, DUIDISABLED_INVALIDATE); // 临时禁止Invalidate操作
		if (rr->GetPosition()>DUIPOS_RELATIVE)
		{
			RECT rcTile = { ptTile.x, ptTile.y, ptTile.x + cxWidth, ptTile.y };
			SIZE szAvailable = { rcTile.right - rcTile.left, 9999 };
			SIZE szTile = rr->EstimateSize(szAvailable);
			rcTile.bottom = rcTile.top + szTile.cy;
			rr->SetPos(rcTile);
			continue;
		}
		// Determine size
		RECT rcTile = { ptTile.x, ptTile.y, ptTile.x + cxWidth, ptTile.y };
		// If this panel expands vertically
		if( cy <= 0)
		{
			SIZE szAvailable = { rcTile.right - rcTile.left, 9999 };
			int iIndex = iCount;
			for( int j = 0; j < r->GetChildCount(); j++ )
			{
				DuiNode* rrr = r->GetChildControl(j);
				if (rrr->GetPosition()>DUIPOS_RELATIVE) continue;
				SIZE szTile = rrr->EstimateSize(szAvailable);
				cyHeight = max(cyHeight, szTile.cy);
				if( (++iIndex % nColumns) == 0) break;
			}
		}
		// Set position
		rcTile.bottom = rcTile.top + cyHeight;
		rr->SetPos(rcTile);
		// Move along...
		if( (++iCount % nColumns) == 0 )
		{
			ptTile.x = rc.left;
			ptTile.y += cyHeight;
			cyHeight = 0;
		}
		else
		{
			ptTile.x += cxWidth;
		}
		cyNeeded = rcTile.bottom - rc.top;
	}
	// Process the scrollbar
	//pBar->SetScrollRange(cyNeeded);
	return TRUE;
}


/////////////////////////////////////////////////////////////////////////////////////
//
//

//SIZE LayoutDialog::EstimateSize(SIZE szAvailable)
//{
//   RecalcArea();
//   return CSize(m_rcDialog.right - m_rcDialog.left, m_rcDialog.bottom - m_rcDialog.top);
//}

BOOL LayoutDialog::OnUpdateLayout(HDE hde)
{
	DuiNode* r = DuiHandle<DuiNode>(hde);
	CRect rc = (r)->GetPos();
	rc.MoveToXY(0,0);
	//CDuiScrollbarBase* pBar = _owner->GetScrollbar();
	//rc.right -= pBar->GetScrollbarWidth();
	//rc.top -= pBar->GetScrollPos();

	DialogData* pdd = (DialogData*)r->m_customData.GetValue(this, sizeof(DialogData));
	if (pdd==NULL) return FALSE;
	RecalcArea(hde, *pdd);
	//pBar->SetScrollRange(m_rcDialog.bottom);

	double d_cx = (double)(rc.Width() /*- m_rcDialog.left*/) / pdd->rcDialog.right; //Width();
	double d_cy = (double)(rc.Height() /*- m_rcDialog.top*/) / pdd->rcDialog.bottom; //Height();
	for( int i = 0; i < r->GetChildCount(); i++ )
	{
		DuiNode* child = r->GetChildControl(i);
		LONG uMode = FromStyleVariant(child->GetStyleAttribute(SAT_STRETCH), LONG);
		//if (uMode==0) uMode = DUISTRETCH_NO_MOVE_X | DUISTRETCH_NO_MOVE_Y | DUISTRETCH_NO_SIZE_X | DUISTRETCH_NO_SIZE_Y;

		LPCRECT prc = FromStyleVariant(child->GetStyleAttribute(SAT_DIALOG_ITEM_POS), LPCRECT);
		CRect rcPos(pdd->rcDialog.TopLeft(),CSize(100,25)); // 没有此属性，则创建一个默认相对矩形
		if (prc) rcPos.CopyRect(prc);
		rcPos.OffsetRect(rc.left, rc.top);

		if( (uMode & DUISTRETCH_NO_MOVE_X) == 0 ) rcPos.OffsetRect((int)((rcPos.left-rc.left) * d_cx - (rcPos.left-rc.left)), 0);
		if( (uMode & DUISTRETCH_NO_MOVE_Y) == 0 ) rcPos.OffsetRect(0, (int)((rcPos.top-rc.top) * d_cy - (rcPos.top-rc.top)));
		if( (uMode & DUISTRETCH_NO_SIZE_X) == 0 ) rcPos.right = rcPos.left + (long)(rcPos.Width() * d_cx);
		if( (uMode & DUISTRETCH_NO_SIZE_Y) == 0 ) rcPos.bottom = rcPos.top + (long)(rcPos.Height() * d_cy);

		DisableOp dop(child, DUIDISABLED_INVALIDATE); // 临时禁止Invalidate操作
		(child)->SetPos(rcPos);
	}
	return TRUE;

	//pBar->SetScrollRange(m_rcDialog.bottom - m_rcDialog.top);

	//// Determine how "scaled" the dialog is compared to the original size
	//int cxDiff = (rc.right - rc.left) - (m_rcDialog.right - m_rcDialog.left);
	//int cyDiff = (rc.bottom - rc.top) - (m_rcDialog.bottom - m_rcDialog.top);
	//if( cxDiff < 0 ) cxDiff = 0;
	//if( cyDiff < 0 ) cyDiff = 0;
	//// Stretch each control
	//// Controls are coupled in "groups", which determine a scaling factor.
	//// A "line" indicator is used to apply the same scaling to a new group of controls.
	//int nCount, cxStretch, cyStretch, cxMove, cyMove;
	//for( int i = 0; i < _owner->GetChildCount(); i++ )
	//{
	//	CDuiControlExtensionBase* pControl = _owner->GetChildControl(i);
	//	LONG uMode = *pControl->GetStyleAttribute(SAT_STRETCH);
	//	if (uMode == 0) uMode = DUISTRETCH_NEWGROUP;

	//	if( i == 0 || (uMode & DUISTRETCH_NEWGROUP) != 0 )
	//	{
	//		nCount = 0;
	//		for( int j = i + 1; j < _owner->GetChildCount(); j++ )
	//		{
	//			CDuiControlExtensionBase* pCtrl2 = _owner->GetChildControl(j);
	//			LONG uMode2 = *pCtrl2->GetStyleAttribute(SAT_STRETCH);
	//			if (uMode2 == 0) uMode2 = DUISTRETCH_NEWGROUP;

	//			if( (uMode2 & (DUISTRETCH_NEWGROUP | DUISTRETCH_NEWLINE)) != 0 ) break;
	//			if( (uMode2 & (DUISTRETCH_SIZE_X | DUISTRETCH_SIZE_Y)) != 0 ) nCount++;
	//		}
	//		if( nCount == 0 ) nCount = 1;
	//		cxStretch = cxDiff / nCount;
	//		cyStretch = cyDiff / nCount;
	//		cxMove = 0;
	//		cyMove = 0;
	//	}
	//	if( (uMode & DUISTRETCH_NEWLINE) != 0 )
	//	{
	//		cxMove = 0;
	//		cyMove = 0;
	//	}
	//	CRect rcPos = pControl->GetPos(DUIRT_TOTAL); // BUGBUG: 要加一个样式属性来指定相对于DIALOG的控件位置矩形
	//	if (rcPos.IsRectEmpty())
	//	{
	//		LPCRECT prc = *pControl->GetStyleAttribute(SAT_DIALOG_ITEM_POS);
	//		if (prc) rcPos.CopyRect(prc);
	//		else rcPos = CRect(0,0,100,25); // 没有此属性，则创建一个默认相对矩形
	//		rcPos.OffsetRect(rc.left, rc.top - pBar->GetScrollPos());
	//	}
	//	if( (uMode & DUISTRETCH_MOVE_X) != 0 ) ::OffsetRect(&rcPos, cxMove, 0);
	//	if( (uMode & DUISTRETCH_MOVE_Y) != 0 ) ::OffsetRect(&rcPos, 0, cyMove);
	//	if( (uMode & DUISTRETCH_SIZE_X) != 0 ) rcPos.right += cxStretch;
	//	if( (uMode & DUISTRETCH_SIZE_Y) != 0 ) rcPos.bottom += cyStretch;
	//	if( (uMode & (DUISTRETCH_SIZE_X | DUISTRETCH_SIZE_Y)) != 0 )
	//	{
	//		cxMove += cxStretch;
	//		cyMove += cyStretch;
	//	}
	//	pControl->SetPos(rcPos);
	//}
}

void LayoutDialog::RecalcArea(HDE hde, DialogData& dd)
{
	DuiNode* r = DuiHandle<DuiNode>(hde);
	CRect rc = (r)->GetPos();
	rc.MoveToXY(0,0);
	if (::EqualRect(&dd.rcOldContainer, &rc)) return;
	::CopyRect(&dd.rcOldContainer, &rc);

	LPCOLESTR w = r->GetAttributeString(L"dialog-width");
	LPCOLESTR h = r->GetAttributeString(L"dialog-height");
	CRect rcDlg(0,0, w?_wtoi(w):0, h?_wtoi(h):0);
	if (rcDlg.right!=0 && rcDlg.bottom!=0)
	{
		dd.rcDialog = rcDlg;
		return;
	}

	CRect rcBound = rcDlg;
	for( int i = 0; i < r->GetChildCount(); i++ )
	{
		DuiNode* child = r->GetChildControl(i);
		LPCRECT prc = FromStyleVariant(child->GetStyleAttribute(SAT_DIALOG_ITEM_POS), LPCRECT);
		CRect rcPos(0,0,100,25); // 没有此属性，则创建一个默认相对矩形
		if (prc) rcPos.CopyRect(prc);

		if (rcDlg.right==0) rcBound.right = max(rcBound.right, rcPos.right);
		if (rcDlg.bottom==0) rcBound.bottom = max(rcBound.bottom, rcPos.bottom);
	}
	if (rcBound.Width()==0) rcBound.right = rcBound.left + 1;
	if (rcBound.Height()==0) rcBound.bottom = rcBound.top + 1;
	dd.rcDialog = rcBound;
}

//////////////////////////////////////////////////////////////////////////
//

BOOL LayoutVerticalSplitter::OnUpdateLayout(HDE hde)
{
	DuiNode* r = DuiHandle<DuiNode>(hde);
	SplitterData* data = GetSplitterData(hde);
	if (data==NULL) return FALSE;

	CRect rc = (r)->GetPos();
	rc.MoveToXY(0,0);
	CRect rcLeft=rc, rcRight=rc;
	rcLeft.right = min(rcLeft.left + data->m_nPos, rc.right);
	rcRight.left = max(rcLeft.right + m_cxy, rc.left);

	// 首先隐藏所有子控件
	for (int i=0; i<r->GetChildCount(); i++)
	{
		DisableOp dop(r->GetChildControl(i), DUIDISABLED_INVALIDATE);
		(r->GetChildControl(i))->SetVisible(FALSE);
	}

	DuiNode* pCtrlLeft = r->GetChildControl(0);
	DuiNode* pCtrlRight = r->GetChildControl(1);
	//DisableOp dopL(pCtrlLeft, DUIDISABLED_CHILDSTATECHANGED);
	//DisableOp dopR(pCtrlRight, DUIDISABLED_CHILDSTATECHANGED);
	if (pCtrlLeft && !rcLeft.IsRectEmpty()) { (pCtrlLeft)->SetVisible(TRUE); (pCtrlLeft)->SetPos(rcLeft); }
	if (pCtrlRight && !rcRight.IsRectEmpty()) { (pCtrlRight)->SetVisible(TRUE); (pCtrlRight)->SetPos(rcRight); }
	return TRUE;
}

BOOL LayoutVerticalSplitter::IsMyEvent( HANDLE hde, DuiEvent* pEvent )
{
	DuiEvent& event = *pEvent;
	DUI_EVENTTYPE e = DuiNode::NameToEventType(event.Name);
	DuiNode* r = DuiHandle<DuiNode>(hde);
	SplitterData* data = GetSplitterData(hde);
	if (data==NULL) return FALSE;

	if (e == (DUIET_mousemove))
	{
		if (data->m_dwState & DUISTATE_CAPTURED)
		{
			CRect rc = r->GetPos();
			data->m_nPos = data->m_nPosCapture + minmax(event.ptMouse.x - data->m_ptCapture.x, -data->m_nPosCapture, rc.Width()-data->m_nPosCapture-m_cxy);
			OnUpdateLayout(hde);
			return TRUE;
		}
		CRect rc = r->GetPos();
		DuiNode::AdjustOffset(r->GetOffsetParent(), NULL, rc);
		rc.left += data->m_nPos;
		rc.right = rc.left + m_cxy;
		if (rc.PtInRect(event.ptMouse)) return (data->m_dwState|=DUISTATE_HOT), TRUE;
		data->m_dwState &= ~DUISTATE_HOT;
		return FALSE;
	}
	else if (e == (DUIET_mouseleave))
	{
		if (data->m_dwState&DUISTATE_CAPTURED) return TRUE;
		data->m_dwState &= ~DUISTATE_HOT;
		return FALSE;
	}
	else if (e == (DUIET_buttondown))
	{
		if ((data->m_dwState & DUISTATE_HOT) == 0) return (data->m_dwState&=~DUISTATE_CAPTURED), FALSE;
		data->m_dwState |= DUISTATE_CAPTURED;
		data->m_ptCapture = event.ptMouse;
		data->m_nPosCapture = data->m_nPos;
		return FALSE; // 让控件有机会设置CAPTURE
	}
	else if (e == (DUIET_buttonup))
	{
		if ((data->m_dwState&DUISTATE_CAPTURED) == 0) return FALSE;
		data->m_dwState &= ~DUISTATE_CAPTURED;
		return FALSE;
	}

	return FALSE;
}

BOOL LayoutHorizontalSplitter::OnUpdateLayout(HDE hde)
{
	DuiNode* r = DuiHandle<DuiNode>(hde);
	SplitterData* data = GetSplitterData(hde);
	if (data==NULL) return FALSE;

	CRect rc = (r)->GetPos();
	rc.MoveToXY(0,0);
	CRect rcTop=rc, rcBottom=rc;
	rcTop.bottom = min(rcTop.top + data->m_nPos, rc.bottom);
	rcBottom.top = max(rcTop.bottom + m_cxy, rc.top);

	for (int i=0; i<r->GetChildCount(); i++)
	{
		DisableOp dop(r->GetChildControl(i), DUIDISABLED_INVALIDATE); // 临时禁止Invalidate操作
		(r->GetChildControl(i))->SetVisible(FALSE);
	}

	DuiNode* pCtrlTop = r->GetChildControl(0);
	DuiNode* pCtrlBottom = r->GetChildControl(1);
	//DisableOp dopT(pCtrlTop, DUIDISABLED_CHILDSTATECHANGED);
	//DisableOp dopB(pCtrlBottom, DUIDISABLED_CHILDSTATECHANGED);
	if (pCtrlTop && !rcTop.IsRectEmpty()) { (pCtrlTop)->SetVisible(TRUE); (pCtrlTop)->SetPos(rcTop); }
	if (pCtrlBottom && !rcBottom.IsRectEmpty()) { (pCtrlBottom)->SetVisible(TRUE); (pCtrlBottom)->SetPos(rcBottom); }
	return TRUE;
}

BOOL LayoutHorizontalSplitter::IsMyEvent( HANDLE h, DuiEvent* pEvent )
{
	DuiEvent& event = *pEvent;
	DuiNode* r = DuiHandle<DuiNode>(h);
	SplitterData* data = GetSplitterData(h);
	if (data==NULL) return FALSE;

	DUI_EVENTTYPE e = DuiNode::NameToEventType(event.Name);
	if (e == (DUIET_mousemove))
	{
		if (data->m_dwState & DUISTATE_CAPTURED)
		{
			CRect rc = r->GetPos();
			data->m_nPos = data->m_nPosCapture + minmax(event.ptMouse.y - data->m_ptCapture.y, -data->m_nPosCapture, rc.Height()-data->m_nPosCapture-m_cxy);
			OnUpdateLayout(h);
			return TRUE;
		}
		CRect rc = r->GetPos();
		DuiNode::AdjustOffset(r->GetOffsetParent(), NULL, rc);
		rc.top += data->m_nPos;
		rc.bottom = rc.top + m_cxy;
		if (rc.PtInRect(event.ptMouse)) return (data->m_dwState|=DUISTATE_HOT), TRUE;
		data->m_dwState &= ~DUISTATE_HOT;
		return FALSE;
	}
	else if (e == (DUIET_mouseleave))
	{
		if (data->m_dwState&DUISTATE_CAPTURED) return TRUE;
		data->m_dwState &= ~DUISTATE_HOT;
		return FALSE;
	}
	else if (e == (DUIET_buttondown))
	{
		if ((data->m_dwState & DUISTATE_HOT) == 0) return (data->m_dwState&=~DUISTATE_CAPTURED), FALSE;
		data->m_dwState |= DUISTATE_CAPTURED;
		data->m_ptCapture = event.ptMouse;
		data->m_nPosCapture = data->m_nPos;
		return FALSE; // 让控件有机会设置CAPTURE
	}
	else if (e == (DUIET_buttonup))
	{
		if ((data->m_dwState&DUISTATE_CAPTURED) == 0) return FALSE;
		data->m_dwState &= ~DUISTATE_CAPTURED;
		return FALSE;
	}

	return FALSE;
}

DWORD LayoutSplitter::GetState(HDE hde)
{
	SplitterData* data = GetSplitterData(hde);
	if (data == NULL)
		return 0;
	return data->m_dwState;
}

void LayoutSplitter::Use( HANDLE hSource )
{
	if (!::DuiIsElement(hSource))
		return;

	DuiNode* n = DuiHandle<DuiNode>(hSource);
	SplitterData* data = (SplitterData*)n->m_customData.GetValue(this, sizeof(SplitterData));
	LPCOLESTR szPos = n->GetAttributeString(L"splitter-pos");
	if (szPos && data)
		data->m_nPos = _wtoi(szPos);
}

void LayoutSplitter::Unuse( HANDLE hSource )
{
	if (!::DuiIsElement(hSource))
		return;

	DuiNode* n = DuiHandle<DuiNode>(hSource);
	n->m_customData.DeleteKey(this);
}

LayoutSplitter::SplitterData* LayoutSplitter::GetSplitterData( HDE hde )
{
	if (!::DuiIsElement(hde))
		return NULL;

	DuiNode* n = DuiHandle<DuiNode>(hde);
	return (SplitterData*)n->m_customData.GetValue(this);
}

BOOL LayoutPage::OnUpdateLayout(HDE hde)
{
	DuiNode* r = DuiHandle<DuiNode>(hde);
	CRect rc = (r)->GetPos();
	rc.MoveToXY(0,0);
	for (int i=0; i<r->GetChildCount(); i++)
	{
		DisableOp dop(r->GetChildControl(i), DUIDISABLED_CHILDSTATECHANGED);
		(r->GetChildControl(i))->SetPos(rc);
	}
	return TRUE;
}
