#include "stdafx.h"
#include "DuiScrollBar.h"
#include "DuiManager.h"
#include "../../common/theme.h"
#include "DuiPluginsManager.h"


#define CheckPosMin() if (m_nPos==0) return FALSE;
#define CheckPosMax() if (m_nPos==m_nRange-CRect(DuiHandle<DuiNode>(_owner)->GetPos()).Height()) return FALSE;
#define CheckPosMaxH() if (m_nPos==m_nRange-CRect(DuiHandle<DuiNode>(_owner)->GetPos()).Width()) return FALSE;


#define sb_timer_id_mask 0x00010000
#define sb_timer_id_mask_delay 0x00020000
#define sb_timer_id_mask_horz 0x80000000
#define sb_timer_id(partid) (sb_timer_id_mask | LOWORD(partid) | (m_bVert?0:sb_timer_id_mask_horz))
#define sb_timer_id_delay(partid) (sb_timer_id_mask_delay | LOWORD(partid) | (m_bVert?0:sb_timer_id_mask_horz))
#define is_sb_timer_id(id) (((id&0x00FF0000)==sb_timer_id_mask || (id&0x00FF0000)==sb_timer_id_mask_delay) && (LOWORD(id)>=sb_lower_arrow && LOWORD(id)<=sb_upper_track))
#define is_sb_timer_id_horz(id) (((id)&0xF0000000)==sb_timer_id_mask_horz)

//////////////////////////////////////////////////////////////////////////
//
void CDuiScrollbar::UpdateRects()
{
	if (m_bVisible || m_bForceVisible)
	{
		DuiNode* rtOwner = DuiHandle<DuiNode>(_owner);
		if (m_bVert)
		{
			int cxScroll = GetScrollbarWidth();
			CRect rcPadding = rtOwner->GetPos(DUIRT_INSIDEBORDER);
			//CDuiControlExtension::AdjustOffset(rtOwner->GetParent(), _owner, rcPadding);
			CRect rc = rtOwner->GetPos();
			//CDuiControlExtension::AdjustOffset(rtOwner->GetParent(), _owner, rc);
			int cyScroll = m_nRange - rc.Height();
			if (cyScroll<=0) cyScroll = 1;
			m_rc = CRect(rcPadding.right, rcPadding.top, rcPadding.right + cxScroll, rcPadding.bottom/* - rtOwner->GetScrollbar(FALSE)->GetScrollbarWidth()*/);
			// 计算上下箭头位置
			m_rcPart[sb_lower_arrow] = m_rc; m_rcPart[sb_lower_arrow].bottom = m_rcPart[sb_lower_arrow].top + min(cxScroll, m_rc.Height()/2);
			m_rcPart[sb_upper_arrow] = m_rc; m_rcPart[sb_upper_arrow].top = m_rcPart[sb_upper_arrow].bottom - min(cxScroll, m_rc.Height()/2);
			// 计算TRACK和THUMB位置
			CRect rcTrack = m_rc; rcTrack.top = m_rcPart[sb_lower_arrow].bottom; rcTrack.bottom = m_rcPart[sb_upper_arrow].top;
			int cyThumb = rcTrack.Height() * rc.Height() / m_nRange;
			int cyLowerTrack = (rcTrack.Height() - cyThumb) * m_nPos / cyScroll;
			m_rcPart[sb_lower_track] = m_rcPart[sb_thumb] = m_rcPart[sb_upper_track] = rcTrack;
			m_rcPart[sb_thumb].top = m_rcPart[sb_lower_track].bottom = m_rcPart[sb_lower_track].top + cyLowerTrack;
			m_rcPart[sb_upper_track].top = m_rcPart[sb_thumb].bottom = m_rcPart[sb_thumb].top + cyThumb;
			//SetScrollPos(m_nPos); // 如果在控件改变大小的过程中，调用此行能修正位置
		}
		else // horz
		{
			int cyScroll = GetScrollbarWidth();
			CRect rcPadding = rtOwner->GetPos(DUIRT_INSIDEBORDER);
			//CDuiControlExtension::AdjustOffset(rtOwner->GetParent(), _owner, rcPadding);
			CRect rc = rtOwner->GetPos();
			//CDuiControlExtension::AdjustOffset(rtOwner->GetParent(), _owner, rc);
			int cxScroll = m_nRange - rc.Width();
			if (cxScroll<=0) cxScroll = 1;
			m_rc = CRect(rcPadding.left, rcPadding.bottom, rcPadding.right /*- rtOwner->GetScrollbar()->GetScrollbarWidth()*/, rcPadding.bottom + cyScroll);
			// 计算上下箭头位置
			m_rcPart[sb_lower_arrow] = m_rc; m_rcPart[sb_lower_arrow].right = m_rcPart[sb_lower_arrow].left + min(cyScroll, m_rc.Width()/2);
			m_rcPart[sb_upper_arrow] = m_rc; m_rcPart[sb_upper_arrow].left = m_rcPart[sb_upper_arrow].right - min(cyScroll, m_rc.Width()/2);
			// 计算TRACK和THUMB位置
			CRect rcTrack = m_rc; rcTrack.left = m_rcPart[sb_lower_arrow].right; rcTrack.right = m_rcPart[sb_upper_arrow].left;
			int cxThumb = rcTrack.Width() * rc.Width() / m_nRange;
			int cxLowerTrack = (rcTrack.Width() - cxThumb) * m_nPos / cxScroll;
			m_rcPart[sb_lower_track] = m_rcPart[sb_thumb] = m_rcPart[sb_upper_track] = rcTrack;
			m_rcPart[sb_thumb].left = m_rcPart[sb_lower_track].right = m_rcPart[sb_lower_track].left + cxLowerTrack;
			m_rcPart[sb_upper_track].left = m_rcPart[sb_thumb].right = m_rcPart[sb_thumb].left + cxThumb;
			//SetScrollPos(m_nPos); // 如果在控件改变大小的过程中，调用此行能修正位置
		}
	}
}

BOOL CDuiScrollbar::SetScrollRange( int nRange, BOOL bRedraw/*=FALSE*/ )
{
	DuiNode* r = DuiHandle<DuiNode>(_owner);
	BOOL bScrollFlag = r->IsAllowScroll(); // 0 - hidden, 1 - auto, -1 - scroll

	BOOL bVisibleChanged = FALSE;
	if (nRange==0) // 取消滚动条
	{
		if (!m_bForceVisible)
		{
			bVisibleChanged = m_bVisible;
			m_bVisible = FALSE;
			r->SetRectDirty();
		}
		return bVisibleChanged;
	}
	if (nRange==-1) nRange = m_nRange; // -1 表示刷新
	m_nRange = max(1, nRange);

	CRect rc = r->GetPos();
	if ((m_bVert && rc.Height()<=0) || (!m_bVert && rc.Width()<=0)) { m_bVisible=FALSE; return FALSE;} // 还没有初始化

	//m_nRange = nRange;
	CDuiLayoutManager* pLayoutMgr = r->m_pLayoutMgr;
	int cbScroll = m_nRange - (m_bVert?rc.Height():rc.Width());

	if( cbScroll > 0 && !m_bVisible && bScrollFlag)
	{
		m_bVisible = TRUE;
		m_bEnabled = TRUE;
		bVisibleChanged = TRUE;
	}

	// Scroll not needed anymore?
	if( cbScroll <= 0 && !m_bForceVisible )
	{
		//cbScroll = 0;
		bVisibleChanged = m_bVisible;
		m_bVisible = FALSE;
		m_bEnabled = FALSE;
	}

	if (bVisibleChanged || bRedraw)
	{
		r->SetRectDirty();
		r->GetScrollbar(!m_bVert)->SetScrollRange(-1, bRedraw); // 刷新另一个滚动条
	}

	if (IsValid() && (bVisibleChanged || cbScroll || bRedraw))
		UpdateRects();

	SetScrollPos(m_nPos);

	/*rtOwner->*/Invalidate();
	return bVisibleChanged;
}

void CDuiScrollbar::SetScrollPos( int nPos, BOOL bRedraw/*=FALSE*/ )
{
	DuiNode* rtOwner = DuiHandle<DuiNode>(_owner);
	CRect r = rtOwner->GetPos();
	nPos = minmax(nPos, 0, m_nRange-(m_bVert?r.Height():r.Width()));

	BOOL bUpdate = FALSE, bPosChanged = FALSE;
	if (IsValid() && m_nPos != nPos)
	{
		m_nPos = nPos;
		bPosChanged = TRUE;
		bUpdate = TRUE;
	}

	if (IsValid() && (bPosChanged || bRedraw))
	{
		UpdateRects();
		// 通知控件滚动位置已改变
		//if (bPosChanged)
			rtOwner->OnScrollPosChanged(m_bVert);
	}

	if (bUpdate || bRedraw)
	{
		////CDuiControlExtension::UpdateLayout(_owner);
		// 下面的代码将保证滚动条滚动是平滑的
		rtOwner->Invalidate();
		CDuiLayoutManager* pMgr = rtOwner->m_pLayoutMgr;
#ifndef NO3D
		if (pMgr->Get3DDevice())
		{
			//pMgr->Get3DDevice()->Render();
			pMgr->Invalidate(NULL, TRUE);
			return;
		}
#endif
		RECT rcTotal = rtOwner->GetPos(DUIRT_TOTAL);
		DuiNode::AdjustOffset(rtOwner->GetOffsetParent()/*rtOwner->GetParent()*/, NULL, rcTotal);
		::RedrawWindow(pMgr->GetPaintWindow(), &rcTotal, NULL, RDW_INVALIDATE|RDW_NOERASE|RDW_NOFRAME|RDW_NOINTERNALPAINT|RDW_UPDATENOW|RDW_NOCHILDREN);
	}
}

void CDuiScrollbar::SetScrollPosByPart(sb_part part)
{
	switch (part)
	{
	case sb_lower_arrow: SetScrollPos(m_nPos - GetScrollLine()); break;
	case sb_upper_arrow: SetScrollPos(m_nPos + GetScrollLine()); break;
	case sb_lower_track: SetScrollPos(m_nPos - GetScrollPage()); break;
	case sb_upper_track: SetScrollPos(m_nPos + GetScrollPage()); break;
	}
}

int CDuiScrollbar::GetScrollbarWidth() const
{
	return (m_bVisible||m_bForceVisible) ? CDuiLayoutManager::GetSystemMetrics().cxvscroll : 0;
}

/*
*	pt 是窗口客户区相关
*/
BOOL CDuiScrollbar::IsPtInside(POINT pt)
{
	ATLASSERT(IsValid());

	DuiNode::AdjustOffset(NULL, DuiHandle<DuiNode>(_owner)->GetParent(), pt);
	if (m_rc.PtInRect(pt))
	{
		m_bHover = TRUE;
		sb_part partHover = sb_null;
		if (m_rcPart[sb_lower_arrow].PtInRect(pt)) partHover = sb_lower_arrow;
		else if (m_rcPart[sb_upper_arrow].PtInRect(pt)) partHover = sb_upper_arrow;
		else if (m_rcPart[sb_lower_track].PtInRect(pt)) partHover = sb_lower_track;
		else if (m_rcPart[sb_thumb].PtInRect(pt)) partHover = sb_thumb;
		else if (m_rcPart[sb_upper_track].PtInRect(pt)) partHover = sb_upper_track;
		if (m_partHot != partHover) Invalidate();
		m_partHot = partHover;
		return TRUE;
	}
	else
	{
		if (m_bHover) Invalidate();
		m_bHover = FALSE;
		m_partHot = sb_null;
		return FALSE;
	}
}

BOOL CDuiScrollbar::IsScrollEvent( DuiEvent& event )
{
	DuiNode* rtOwner = DuiHandle<DuiNode>(_owner);
	DUI_EVENTTYPE e = DuiNode::NameToEventType(event.Name);
	if (!IsValid() || HIWORD(event.Name))
		return FALSE;

	if (e!=(DUIET_vscroll) && e!=(DUIET_keydown) && e!=(DUIET_scrollwheel)
		&& e!=(DUIET_mousemove) && e!=(DUIET_mouseleave)
		&& e!=(DUIET_buttondown) && e!=(DUIET_buttonup) && e!=(DUIET_dblclick)
		&& e!=(DUIET_timer))
		return FALSE;

	switch (e)
	{
	case DUIET_vscroll:
	case DUIET_scrollwheel:
	case DUIET_mousemove:
	case DUIET_mouseleave:
	case DUIET_buttondown:
	case DUIET_buttonup:
	case DUIET_dblclick:
		if (m_partCapture == sb_null)
		{
			CDuiScrollbar* other = (CDuiScrollbar*)rtOwner->GetScrollbar(!m_bVert);
			if (other->m_partCapture != sb_null) // 当前正被另一个滚动条控制，消息应该交给它处理
			{
				return FALSE;
			}
		}
	}

	if( m_bVert && (e == (DUIET_vscroll) || e == (DUIET_scrollwheel)) ) 
	{
		switch( LOWORD(event.wParam) )
		{
		//case SB_THUMBPOSITION:
		//case SB_THUMBTRACK:
		//	{
		//		SCROLLINFO si = { 0 };
		//		si.cbSize = sizeof(SCROLLINFO);
		//		si.fMask = SIF_TRACKPOS;
		//		::GetScrollInfo(m_hWnd, SB_CTL, &si);
		//		SetScrollPos(si.nTrackPos);
		//	}
		//	break;
		case SB_LINEUP:		CheckPosMin(); SetScrollPosByPart(sb_lower_arrow); break;
		case SB_LINEDOWN:	CheckPosMax(); SetScrollPosByPart(sb_upper_arrow); break;
		case SB_PAGEUP:		CheckPosMin(); SetScrollPosByPart(sb_lower_track); break;
		case SB_PAGEDOWN:	CheckPosMax(); SetScrollPosByPart(sb_upper_track); break;
		case SB_THUMBPOSITION:	SetScrollPos(event.lParam); break; // 此消息为模拟发出
		}
		return TRUE;
	}
	else if( e == (DUIET_keydown) ) 
	{
		if (m_bVert)
		{
			switch( event.chKey )
			{
			case VK_DOWN:	CheckPosMax(); return SetScrollPosByPart(sb_upper_arrow), TRUE;
			case VK_UP:		CheckPosMin(); return SetScrollPosByPart(sb_lower_arrow), TRUE;
			case VK_NEXT:	CheckPosMax(); return SetScrollPosByPart(sb_upper_track), TRUE;
			case VK_PRIOR:	CheckPosMin(); return SetScrollPosByPart(sb_lower_track), TRUE;
			case VK_HOME:	return SetScrollPos(0), TRUE;
			case VK_END:	return SetScrollPos(99999), TRUE;
			}
		}
		else
		{
			switch( event.chKey )
			{
			case VK_LEFT:	CheckPosMin(); return SetScrollPosByPart(sb_lower_arrow), TRUE;
			case VK_RIGHT:	CheckPosMaxH(); return SetScrollPosByPart(sb_upper_arrow), TRUE;
			}
		}
	}
	else if (e == (DUIET_mousemove))
	{
		if (m_partCapture != sb_null)
		{
			if (m_partCapture == sb_thumb)
			{
				if (m_bVert)
				{
					int cy = event.ptMouse.y - m_ptCapture.y;
					if (cy > 0) cy = min(cy, m_rcPart[sb_upper_track].bottom - m_rcPart[sb_thumb].Height() - m_nCaptureThumbTopOrLeft);
					if (cy < 0) cy = max(cy, m_rcPart[sb_lower_track].top - m_nCaptureThumbTopOrLeft);
					int new_cy = (m_nCaptureThumbTopOrLeft + cy - m_rcPart[sb_lower_track].top);
					int total_cy = (m_rcPart[sb_lower_track].Height() + m_rcPart[sb_upper_track].Height());
					int cyRange = (m_nRange - CRect((rtOwner)->GetPos()).Height());
					if (total_cy > 0)
						SetScrollPos(cyRange * new_cy / total_cy);
				}
				else
				{
					int cx = event.ptMouse.x - m_ptCapture.x;
					if (cx > 0) cx = min(cx, m_rcPart[sb_upper_track].right - m_rcPart[sb_thumb].Width() - m_nCaptureThumbTopOrLeft);
					if (cx < 0) cx = max(cx, m_rcPart[sb_lower_track].left - m_nCaptureThumbTopOrLeft);
					int new_cx = (m_nCaptureThumbTopOrLeft + cx - m_rcPart[sb_lower_track].left);
					int total_cx = (m_rcPart[sb_lower_track].Width() + m_rcPart[sb_upper_track].Width());
					int cxRange = (m_nRange - CRect((rtOwner)->GetPos()).Width());
					if (total_cx > 0)
						SetScrollPos(cxRange * new_cx / total_cx);
				}
			}
			return TRUE;
		}
		return IsPtInside(event.ptMouse);
	}
	else if (e == (DUIET_buttondown))
	{
		if (!m_bHover || m_partHot==sb_null) return FALSE;
		m_partCapture = m_partHot;
		m_ptCapture = event.ptMouse;
		m_nCaptureThumbTopOrLeft = (m_bVert?m_rcPart[sb_thumb].top:m_rcPart[sb_thumb].left);

		//DuiEvent ev = { (DUI_EVENTTYPE)0 };
		//ev.Name = DUIET_vscroll;
		//ev.dwTimestamp = ::GetTickCount();
		switch (m_partCapture)
		{
		case sb_lower_arrow:
		case sb_upper_arrow:
		case sb_lower_track:
		case sb_upper_track: SetScrollPosByPart(m_partCapture); StartScrollTimer(m_partCapture); break;
		//default: Invalidate();
		}
		Invalidate();
		return TRUE;
	}
	else if (e == (DUIET_buttonup))
	{
		if (m_partCapture != sb_null) Invalidate();
		m_partCapture = sb_null;
		StopScrollTimer();
		IsPtInside(event.ptMouse); // 更新当前状态
		if (!m_bHover || m_partHot==sb_null) return FALSE;
		return TRUE;
	}
	else if (e == (DUIET_mouseleave))
	{
		if (m_partCapture != sb_null) return TRUE;
		if (m_bHover) Invalidate();
		m_bHover = FALSE;
		m_partHot = sb_null;
		return FALSE;
	}
	else if (e == (DUIET_dblclick))
	{
		return m_bHover;
	}
	else if (e == (DUIET_timer))
	{
		if (!is_sb_timer_id(event.wParam)) return FALSE;
		if (m_bVert && is_sb_timer_id_horz(event.wParam)) return FALSE;
		if (!m_bVert && !is_sb_timer_id_horz(event.wParam)) return FALSE;
		DWORD dwMask = (event.wParam & 0x00ff0000);
		sb_part part = (sb_part)LOWORD(event.wParam);
		if (part == m_partCapture && IsPtInside(event.ptMouse) && part==m_partHot)
		{
			if (dwMask == sb_timer_id_mask_delay) return SetScrollPosByPart(part), StartScrollTimer(part, FALSE), TRUE;
			SetScrollPosByPart(part);
		}
		return TRUE;
	}
	return FALSE;
}

void CDuiScrollbar::Paint( HDC hdc )
{
	if (!m_bVisible && !m_bForceVisible) return;
	if (m_rc.IsRectEmpty()) UpdateRects();

	// 因为先画的垂直滚动条，所以当水平滚动条绘制完毕时多判断一下是否要画空白的部分
	if (m_bVert)
	{
		LONG vx = DuiHandle<DuiNode>(_owner)->GetScrollbar(FALSE)->GetScrollbarWidth();
		if (vx > 0)
		{
			//RECT rc = {m_rcPart[sb_upper_arrow].right, m_rcPart[sb_upper_arrow].top, m_rcPart[sb_upper_arrow].right + vx, m_rcPart[sb_upper_arrow].bottom};
			RECT rc = {m_rcPart[sb_upper_arrow].left, m_rcPart[sb_upper_arrow].bottom, m_rcPart[sb_upper_arrow].right, m_rcPart[sb_upper_arrow].bottom + vx};
			//draw_theme_scroll(hdc, SBP_SIZEBOX, SZB_HALFBOTTOMRIGHTALIGN, &rc);
			draw_theme_scroll(hdc, SBP_UPPERTRACKVERT, SCRBS_NORMAL, &rc);
		}
	}

	int state_lower_arrow = ((!m_bEnabled) ? (m_bVert?ABS_UPDISABLED:ABS_LEFTDISABLED) :
		((m_partCapture==sb_lower_arrow) ? (m_bVert?ABS_UPPRESSED:ABS_LEFTPRESSED) :
		((m_partHot==sb_lower_arrow) ? (m_bVert?ABS_UPHOT:ABS_LEFTHOT) :
		(m_bHover ? (m_bVert?ABS_UPHOVER:ABS_LEFTHOVER) : (m_bVert?ABS_UPNORMAL:ABS_LEFTNORMAL)))));
	int state_upper_arrow = ((!m_bEnabled) ? (m_bVert?ABS_DOWNDISABLED:ABS_RIGHTDISABLED) :
		((m_partCapture==sb_upper_arrow) ? (m_bVert?ABS_DOWNPRESSED:ABS_RIGHTPRESSED) :
		((m_partHot==sb_upper_arrow) ? (m_bVert?ABS_DOWNHOT:ABS_RIGHTHOT) :
		(m_bHover ? (m_bVert?ABS_DOWNHOVER:ABS_RIGHTHOVER) : (m_bVert?ABS_DOWNNORMAL:ABS_RIGHTNORMAL)))));
	int state_lower_track = ((!m_bEnabled) ? SCRBS_DISABLED :
		((m_partCapture==sb_lower_track) ? SCRBS_PRESSED :
		((m_partHot==sb_lower_track) ? SCRBS_HOT :
		(m_bHover ? SCRBS_HOVER : SCRBS_NORMAL))));
	int state_upper_track = ((!m_bEnabled) ? SCRBS_DISABLED :
		((m_partCapture==sb_upper_track) ? SCRBS_PRESSED :
		((m_partHot==sb_upper_track) ? SCRBS_HOT :
		(m_bHover ? SCRBS_HOVER : SCRBS_NORMAL))));
	int state_thumb = ((!m_bEnabled) ? SCRBS_DISABLED :
		((m_partCapture==sb_thumb) ? SCRBS_PRESSED :
		((m_partHot==sb_thumb) ? SCRBS_HOT :
		(m_bHover ? SCRBS_HOVER : SCRBS_NORMAL))));

	draw_theme_scroll(hdc, SBP_ARROWBTN, state_lower_arrow, &m_rcPart[sb_lower_arrow]);
	draw_theme_scroll(hdc, SBP_ARROWBTN, state_upper_arrow, &m_rcPart[sb_upper_arrow]);
	draw_theme_scroll(hdc, (m_bVert?SBP_LOWERTRACKVERT:SBP_LOWERTRACKHORZ), state_lower_track, &m_rcPart[sb_lower_track]);
	draw_theme_scroll(hdc, (m_bVert?SBP_THUMBBTNVERT:SBP_THUMBBTNHORZ), state_thumb, &m_rcPart[sb_thumb]);
	draw_theme_scroll(hdc, (m_bVert?SBP_GRIPPERVERT:SBP_GRIPPERHORZ), state_thumb, &m_rcPart[sb_thumb]);
	draw_theme_scroll(hdc, (m_bVert?SBP_UPPERTRACKVERT:SBP_UPPERTRACKHORZ), state_upper_track, &m_rcPart[sb_upper_track]);

	//CDuiControlExtensionBase* pParent = rt(_owner)->GetParent();
	//draw_theme_scroll(hdc, SBP_ARROWBTN, state_lower_arrow, &CDuiControlExtension::GetOffset(pParent, NULL, m_rcPart[sb_lower_arrow]));
	//draw_theme_scroll(hdc, SBP_ARROWBTN, state_upper_arrow, &CDuiControlExtension::GetOffset(pParent, NULL, m_rcPart[sb_upper_arrow]));
	//draw_theme_scroll(hdc, (m_bVert?SBP_LOWERTRACKVERT:SBP_LOWERTRACKHORZ), state_lower_track, &CDuiControlExtension::GetOffset(pParent, NULL, m_rcPart[sb_lower_track]));
	//draw_theme_scroll(hdc, (m_bVert?SBP_THUMBBTNVERT:SBP_THUMBBTNHORZ), state_thumb, &CDuiControlExtension::GetOffset(pParent, NULL, m_rcPart[sb_thumb]));
	//draw_theme_scroll(hdc, (m_bVert?SBP_GRIPPERVERT:SBP_GRIPPERHORZ), state_thumb, &CDuiControlExtension::GetOffset(pParent, NULL, m_rcPart[sb_thumb]));
	//draw_theme_scroll(hdc, (m_bVert?SBP_UPPERTRACKVERT:SBP_UPPERTRACKHORZ), state_upper_track, &CDuiControlExtension::GetOffset(pParent, NULL, m_rcPart[sb_upper_track]));
}

void CDuiScrollbar::Invalidate()
{
// 	RECT rc = m_rc;
// 	CDuiControlExtension::AdjustOffset(_owner, NULL, rc);
	DuiNode* r = DuiHandle<DuiNode>(_owner);
#ifndef NO3D
	if (r->m_pSurface)
		r->m_pSurface->Invalidate();
	else
#endif
		r->m_pLayoutMgr->Invalidate(&DuiNode::GetOffset(r->GetOffsetParent()/*r->GetParent()*/, NULL, m_rc));
}

void CDuiScrollbar::StartScrollTimer(sb_part nPartId, BOOL bDelayFirst/*=TRUE*/)
{
	DuiNode* r = DuiHandle<DuiNode>(_owner);
	StopScrollTimer();
	r->m_pLayoutMgr->SetTimer(r, bDelayFirst ? sb_timer_id_delay(nPartId) : sb_timer_id(nPartId), bDelayFirst ? 500 : 50);
}

void CDuiScrollbar::StopScrollTimer()
{
	DuiNode* r = DuiHandle<DuiNode>(_owner);
	CDuiLayoutManager* pLytMgr = r->m_pLayoutMgr;
	pLytMgr->KillTimer(r, sb_timer_id(sb_lower_arrow));
	pLytMgr->KillTimer(r, sb_timer_id(sb_upper_arrow));
	pLytMgr->KillTimer(r, sb_timer_id(sb_lower_track));
	pLytMgr->KillTimer(r, sb_timer_id(sb_upper_track));
	pLytMgr->KillTimer(r, sb_timer_id_delay(sb_lower_arrow));
	pLytMgr->KillTimer(r, sb_timer_id_delay(sb_upper_arrow));
	pLytMgr->KillTimer(r, sb_timer_id_delay(sb_lower_track));
	pLytMgr->KillTimer(r, sb_timer_id_delay(sb_upper_track));
}

int CDuiScrollbar::GetScrollLine() const
{
	return m_bVert ? min(CRect(DuiHandle<DuiNode>(_owner)->GetPos()).Height()/10, 20) :
					min(CRect(DuiHandle<DuiNode>(_owner)->GetPos()).Width()/10, 12);
}

int CDuiScrollbar::GetScrollPage() const
{
	return m_bVert ? max(CRect(DuiHandle<DuiNode>(_owner)->GetPos()).Height()-20, 5) :
					max(CRect(DuiHandle<DuiNode>(_owner)->GetPos()).Width()-20, 5);
}

/*
*	pt 是窗口客户区相关
*/
BOOL CDuiScrollbar::PtInScrollbar( POINT pt )
{
	if (!m_bVisible && !m_bForceVisible) return FALSE;
	DuiNode::AdjustOffset(NULL, DuiHandle<DuiNode>(_owner)->GetParent(), pt);
	return ::PtInRect(&m_rc, pt);
	//return m_bVisible ? ::PtInRect(&m_rc, pt) : FALSE;
}