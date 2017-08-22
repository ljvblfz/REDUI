#include "stdafx.h"
#include "DuiInline.h"
#include "DuiAlpha.h"
#include "DuiPluginsManager.h"

DuiNode* CDuiInlineControlBase::FindControl( FINDCONTROLPROC Proc, LPVOID pData, UINT uFlags )
{
	if( (uFlags & DUIFIND_VISIBLE) != 0 && !m_pOwner->IsVisible() ) return NULL;
	if( (uFlags & DUIFIND_ENABLED) != 0 && !m_pOwner->IsEnabled() ) return NULL;
	if( (uFlags & DUIFIND_HITTEST) != 0 && !m_pOwner->PtInControl(* static_cast<LPPOINT>(pData)) ) return NULL;
	if ((uFlags & DUIFIND_HITTEST) /*&& (uFlags & DUIFIND_INLINE)*/)
	{
		if (uFlags & DUIFIND_ME_FIRST)
		{
			DuiNode* pCtrl = DuiHandle<DuiNode>(Proc(m_pOwner->Handle(), pData));
			if (pCtrl && CLASS_CAST(pCtrl, CDuiInlineText)) return (pCtrl)->GetParent(); // 内联文本块直接返回父控件
			return pCtrl;
		}

		if ((m_pOwner->GetControlFlags()&DUIFLAG_NOCHILDREN) == 0)
		{
			for (int i=0; i<m_pOwner->GetChildCount(); i++)
			{
				DuiNode* pChild = m_pOwner->GetChildControl(i);
				DuiNode* pCtrl = (pChild)->FindControl(Proc, pData, uFlags);
				if (pCtrl) return pCtrl;
			}
		}
	}
	DuiNode* pCtrl = m_pOwner;
	if (CLASS_CAST(pCtrl, CDuiInlineText)) pCtrl = (pCtrl)->GetParent(); // 内联文本块直接返回父控件
	return DuiHandle<DuiNode>(Proc((pCtrl->Handle()), pData));
	//return CDuiControlExtension::FindControl(Proc, pData, uFlags);
}

void CDuiInlineControlBase::OnUpdateInlineRegion(HANDLE)
{
	if ((m_pOwner->GetControlFlags() & DUIFLAG_NOCHILDREN)==0 && m_pOwner->GetChildCount()>0)
	{
		if (m_pOwner->m_hRgn) ::DeleteObject(m_pOwner->m_hRgn);
		m_pOwner->m_hRgn = m_pOwner->CombineChildrenRegion();
		m_pOwner->UpdateRectFromRgn();
	}
	else
		m_pOwner->UpdateRectFromRgn(m_pOwner->UpdateRgnFromRects()); // 获取RGN的边框矩形
}

void CDuiInlineText::OnPaintContent( HDE hde, HDC hdc, RECT rcPaint, BOOL bDraw, InDrawData* pidd )
{
	InDrawData& idd = *pidd;
	m_pOwner->UpdateDrawData(idd);

	if (bDraw && ::IsRectEmpty(&idd.rc)) return;
	// 父控件应该已经把DC环境设置好了
	LPCOLESTR p = m_pOwner->_attributes.GetAttributeString(L"text");
	if (p == NULL) return;
	CDCHandle dc(hdc);

	BOOL bGdi32 = IsGdi32(m_pOwner->m_pLayoutMgr);

	if (bDraw)
	{
		COLORREF glowcolor = FromStyleVariant(m_pOwner->GetStyleAttribute(SAT_GLOWCOLOR), COLORREF);
		long glowthick = FromStyleVariant(m_pOwner->GetStyleAttribute(SAT_GLOWTHICK), LONG, 0,0,1);
		if (m_pOwner->m_hRgn) // inline
		{
			for (int i=0, num=m_pOwner->__cache.rects.GetSize(); i<num; i++)
			{
				DuiNode::RgnRect& rr = m_pOwner->__cache.rects[i];
				call_drawText(bGdi32, (hdc, rr.str, rr.str_num, &rr.rc, m_pOwner->__cache.rects.text_style|DT_SINGLELINE, glowcolor, glowthick)) // vcenter bottom仅在SINGLELINE有效
			}
		}
		else // block
		{
			call_drawText(bGdi32, (hdc, p, lstrlenW(p), &idd.rc, idd.uTextStyle|DT_SINGLELINE, glowcolor, glowthick))
		}
		return;
	}

	// 现在是在试算状态 bDraw==FALSE
	if (m_pOwner->IsBlock())
	{
		CRect rc(idd.rc);
		long cy = max(dc.DrawText(p, lstrlenW(p), &rc, idd.uTextStyle|DT_CALCRECT), idd.cyMinHeight);
		idd.cyTotalHeight += cy + idd.lLineDistance;
		idd.pt.x = idd.rc.left + idd.lLineIndent;
		idd.pt.y += cy + idd.lLineDistance;
		m_pOwner->UpdateDrawData(idd);
		return;
	}

	// 现在是inline试算
	m_pOwner->__cache.rects.RemoveAll();
	m_pOwner->__cache.rects.text_style = idd.uTextStyle;

	// 为RGN做准备
	CPoint ptStart = idd.pt;
	CRect rc = idd.rc;
	//CDuiControlExtensionBase* parent = GetParent();
	//if (parent && !parent->IsRectDirty()) rc = parent->GetPos();
	BOOL bSingleLine = (idd.uTextStyle & DT_SINGLELINE);
	//CRect rcSingleLine = idd.rc;
	//rcSingleLine.bottom = rcSingleLine.top + idd.cyMinHeight;
	////if (rcSingleLine.Height() <= 0)
	////	rcSingleLine.bottom = max(rcSingleLine.bottom, idd.cyMinHeight);

	// 不处理所有回车换行TAB和空格
	while (*p != 0)
	{
		if (idd.pt.x >= rc.right) // 处理换行
		{
			DuiNode::ReposInlineRow(idd);
			//if (!bDraw) // 不绘制才更新布局
			//{
			//	CRect rc2(ptStart, CSize(rc.right-ptStart.x, bSingleLine ? idd.cyMinHeight : idd.cyCurLine));
			//	m_pOwner->AddRect(&rc2);
			//}
			if (bSingleLine) break;
			ptStart.SetPoint(rc.left, ptStart.y+idd.cyMinHeight + idd.lLineDistance);
			idd.pt = ptStart;
			idd.cyTotalHeight += idd.cyMinHeight + idd.lLineDistance;
			idd.cyMinHeight = idd.cyCurLine;
		}
		else if (iswspace(*p))
		{
			while (iswspace(*p) && *p!=L' ' && *p!=0) p++; // 排除所有非空格的空白
			if (*p == L' ')
			{
				SIZE sz;
				call_getTextExtent(bGdi32, (hdc, L" ", 1, &sz));
				if (idd.pt.x+sz.cx >= rc.right && rc.left+idd.lLineIndent+sz.cx<rc.right) {idd.pt.x+=sz.cx; continue;} // 需要换行处理
				//if (bDraw) /*dc.TextOut(idd.pt.x, idd.pt.y, L" ", 1);*/ // 空格不用添加到缓存
				//	call_drawText(bLayered, (hdc, L" ", 1, bSingleLine ? &idd.rc: &CRect(idd.pt,CSize(sz.cx,idd.cyMinHeight)), idd.uTextStyle));
				//	//call_drawText(bLayered, (hdc, L" ", 1, &CDuiControlExtension::GetOffset(idd.owner, NULL, bSingleLine ? idd.rc: CRect(idd.pt,CSize(sz.cx,idd.cyMinHeight))), idd.uTextStyle));
				idd.pt.x += sz.cx;
				p++;
				//while (iswspace(*p) && *p!=0) p++; // 排除剩余的空白
			}
		}
		else if (*p == L'&')
		{
			if ((idd.uTextStyle & DT_NOPREFIX))
			{
				SIZE sz;
				call_getTextExtent(bGdi32, (hdc, L"&", 1, &sz));
				if (idd.pt.x+sz.cx >= rc.right && rc.left+idd.lLineIndent+sz.cx<rc.right) {idd.pt.x+=sz.cx; continue;}
				m_pOwner->AddSpecChar(&CRect(idd.pt,CSize(sz.cx,idd.cyCurLine)), L'&', idd);
				//if (bDraw)
				//	call_drawText(bLayered, (hdc, L"&", 1, bSingleLine ? &idd.rc : &CRect(idd.pt,CSize(sz.cx,idd.cyMinHeight)), idd.uTextStyle));
				//	//call_drawText(bLayered, (hdc, L"&", 1, &CDuiControlExtension::GetOffset(idd.owner,NULL, bSingleLine ? idd.rc : CRect(idd.pt,CSize(sz.cx,idd.cyMinHeight))), idd.uTextStyle));
				//	//dc.TextOut(idd.pt.x, idd.pt.y, L"&", 1);
				idd.pt.x += sz.cx;
			}
			else
			{
				SIZE sz;
				call_getTextExtent(bGdi32, (hdc, L"_", 1, &sz));
				if (idd.pt.x+sz.cx >= rc.right && rc.left+idd.lLineIndent+sz.cx<rc.right) {idd.pt.x+=sz.cx; continue;}
				if (CDuiLayoutManager::GetSystemSettings().bShowKeyboardCues)
					m_pOwner->AddSpecChar(&CRect(idd.pt,CSize(sz.cx,idd.cyCurLine)), L'_', idd);
				//if (bDraw && CDuiLayoutManager::GetSystemSettings().bShowKeyboardCues)
				//	call_drawText(bLayered, (hdc, L"_", 1, bSingleLine ? &idd.rc : &CRect(idd.pt,CSize(sz.cx,idd.cyMinHeight)), idd.uTextStyle));
				//	//call_drawText(bLayered, (hdc, L"_", 1, &CDuiControlExtension::GetOffset(idd.owner,NULL,bSingleLine ? idd.rc : CRect(idd.pt,CSize(sz.cx,idd.cyMinHeight))), idd.uTextStyle));
				//	//dc.TextOut(idd.pt.x, idd.pt.y, L"_", 1);
				// 这里不要调用 idd.pt.x += sz.cx; 因为下一个字符继续在此位置绘制
			}
			p++;
		}
		else // normal char
		{
			// 采用更快速的计算方式
			int cchChars = 0; // 一次性可处理的非空白字符个数
			LPCOLESTR pEnd = p;
			while ((!iswspace(*pEnd) || *pEnd==L' ') && *pEnd!=L'&' && *pEnd!=0) pEnd++, cchChars++;
			int cchRealChars = 0; // 实际能绘制的字符个数
			SIZE sz;
			call_getTextExtentExPoint(bGdi32, (hdc, p, cchChars, rc.right-idd.pt.x, &cchRealChars, NULL, &sz));
			if (cchRealChars==0 && rc.left+idd.lLineIndent+sz.cx<rc.right) {idd.pt.x=rc.right; continue;} // 一个字符也装不下，修改一下当前位置，交给下次循环处理
			if (cchRealChars==0) /*(cchRealChars = 1),*/ call_getTextExtent(bGdi32, (hdc, p, 1, &sz)) // 保证下面要增加的值能够超出右边界，下次循环时将自动换行
			else
			{
				call_getTextExtent(bGdi32, (hdc, p, cchRealChars, &sz));
				m_pOwner->AddRect(&CRect(idd.pt,CSize(sz.cx,idd.cyCurLine)), p, cchRealChars, idd);
				//if (bDraw)
				//	call_drawText(bLayered, (hdc, p, cchRealChars, bSingleLine ? &idd.rc : &CRect(idd.pt,CSize(rc.right-idd.pt.x,idd.cyMinHeight)), idd.uTextStyle));
				//	//call_drawText(bLayered, (hdc, p, cchRealChars, &CDuiControlExtension::GetOffset(idd.owner,NULL,bSingleLine ? idd.rc : CRect(idd.pt,CSize(rc.right-idd.pt.x,idd.cyMinHeight))), idd.uTextStyle));
				//	//dc.TextOut(idd.pt.x, idd.pt.y, p, cchRealChars);
			}
			idd.pt.x += sz.cx;
			p += cchRealChars;
		}
	}

	m_pOwner->UpdateDrawData(idd);
}

void CDuiInlineText::SetAttribute( HDE hde, LPCWSTR szName, VARIANT* value )
{
	// InlineText 只接受 text 属性，其它所有属性都禁止
	if (lstrcmpiW(szName, L"text") == 0 || m_pOwner->IsBlock())
		__super::SetAttribute(hde, szName, value);
	else
		m_pOwner->GetParent()->SetAttribute(szName, *value);
}

// 有时需要查询内联文本的样式和属性，但某些属性在XML中只能保存在父标签中，
// 因此需要查询父控件
void CDuiInlineText::GetAttribute( HDE hde, LPCWSTR szName, VARIANT* value )
{
	if (lstrcmpiW(szName, L"text") == 0 || m_pOwner->IsBlock())
		__super::GetAttribute(hde, szName, value);
	else
		m_pOwner->GetParent()->GetAttribute(szName).Detach(value);
}

// 有时需要查询内联文本的样式和属性，但某些属性在XML中只能保存在父标签中，
// 因此需要查询父控件
DuiStyleVariant* CDuiInlineText::GetStyleAttribute( HDE hde, DuiStyleAttributeType sat, DuiStyleVariantType svt/*=SVT_EMPTY*/, DWORD dwMatchMask/*=0*/ )
{
	DuiStyleVariant* pVar = __super::GetStyleAttribute(hde, sat, svt, dwMatchMask);
	if (pVar == &svEmpty && (/*sat==SAT_BACKGROUND ||*/ sat==SAT_HITTEST_AS))
		pVar = m_pOwner->GetParent()->GetStyleAttribute(sat, svt, dwMatchMask);
	return pVar;
}

//////////////////////////////////////////////////////////////////////////
void CDuiFont::OnPaintContent( HDE hde, HDC hdc, RECT rcPaint, BOOL bDraw, InDrawData* pidd )
{
	InDrawData& idd = *pidd;
	m_pOwner->UpdateDrawData(idd);

	CFontHandle hOldFont = NULL;
	if (!bDraw)
	{
		hOldFont = (HFONT)::GetCurrentObject(hdc, OBJ_FONT);
		LOGFONT lf;
		hOldFont.GetLogFont(&lf);

		LONG lSize = m_pOwner->_attributes.GetAttributeLong(L"size");
		CStdString strFace = m_pOwner->_attributes.GetAttribute(L"face");

		if (lSize!=0) lf.lfHeight = -(abs(lf.lfHeight) + lSize);
		if (strFace.GetLength()>0) lstrcpynW(lf.lfFaceName, strFace, LF_FACESIZE);
		// 	TEXTMETRIC tm_;
		// 	GetTextMetrics(hdc, &tm_);
		if (m_pOwner->__cache.font) ::DeleteObject(m_pOwner->__cache.font);
		m_pOwner->__cache.font = ::CreateFontIndirect(&lf);
		if (m_pOwner->__cache.font)
			m_pOwner->ProcessFont(m_pOwner->__cache.font);
	}
	if (m_pOwner->__cache.font)
	{
		hOldFont = (HFONT)::SelectObject(hdc, m_pOwner->__cache.font);
	}

// 	LOGFONT lfNew;
// 	if (__cache.font) __cache.font.GetLogFont(&lfNew);
// 	else hOldFont.GetLogFont(&lfNew);

	COLORREF clr = CLR_INVALID;
	if (m_pOwner->_attributes.HasAttribute(L"color")) TryLoad_COLORREF_FromString(m_pOwner->_attributes.GetAttributeString(L"color"), clr);
	if (clr != CLR_INVALID) clr &= 0x00ffffff;
	COLORREF clrOld = ::GetTextColor(hdc);
	if (clr != CLR_INVALID) ::SetTextColor(hdc, clr);

	InDrawData iddOld = idd;
	TEXTMETRIC tm;
	GetTextMetrics(hdc, &tm);
	idd.cyCurLine = tm.tmExternalLeading + tm.tmHeight;
	idd.cyMinHeight = max(idd.cyCurLine, idd.cyMinHeight);

	m_pOwner->DrawChildren(hdc, rcPaint, bDraw, idd);
	m_pOwner->UpdateDrawData(idd);

	// 还原idd的部分数据
	idd.cyCurLine = iddOld.cyCurLine;
	idd.cyMinHeight = max(iddOld.cyMinHeight, idd.cyMinHeight);

	if (clr != CLR_INVALID) ::SetTextColor(hdc, clrOld);
	if (m_pOwner->__cache.font)
	{
		::SelectObject(hdc, hOldFont);
	}

	//if (!bDraw)
	//{
	//	if (m_pOwner->m_hRgn) ::DeleteObject(m_pOwner->m_hRgn);
	//	m_pOwner->m_hRgn = m_pOwner->CombineChildrenRegion();
	//	m_pOwner->UpdateRectFromRgn();
	//}
}

void CDuiBold::OnPaintContent( HDE hde, HDC hdc, RECT rcPaint, BOOL bDraw, InDrawData* pidd )
{
	InDrawData& idd = *pidd;
	m_pOwner->UpdateDrawData(idd);

	HFONT hOldFont = NULL;
	if (!bDraw)
	{
		hOldFont = (HFONT)::GetCurrentObject(hdc, OBJ_FONT);
		LOGFONT lf;
		::GetObject(hOldFont, sizeof(LOGFONT), &lf);
		lf.lfWeight = FW_BOLD;

		if (m_pOwner->__cache.font) ::DeleteObject(m_pOwner->__cache.font);
		m_pOwner->__cache.font = ::CreateFontIndirect(&lf);
		if (m_pOwner->__cache.font)
			m_pOwner->ProcessFont(m_pOwner->__cache.font);
	}
	if (m_pOwner->__cache.font)
	{
		hOldFont = (HFONT)::SelectObject(hdc, m_pOwner->__cache.font);
	}

	m_pOwner->DrawChildren(hdc, rcPaint, bDraw, idd);
	m_pOwner->UpdateDrawData(idd);

	if (m_pOwner->__cache.font)
	{
		::SelectObject(hdc, hOldFont);
	}

	//if (!bDraw)
	//{
	//	if (m_pOwner->m_hRgn) ::DeleteObject(m_pOwner->m_hRgn);
	//	m_pOwner->m_hRgn = m_pOwner->CombineChildrenRegion();
	//	m_pOwner->UpdateRectFromRgn();
	//}
}

void CDuiItalic::OnPaintContent(HDE hde, HDC hdc, RECT rcPaint, BOOL bDraw, InDrawData* pidd)
{
	InDrawData& idd = *pidd;
	m_pOwner->UpdateDrawData(idd);

	HFONT hOldFont = NULL;
	if (!bDraw)
	{
		hOldFont = (HFONT)::GetCurrentObject(hdc, OBJ_FONT);
		LOGFONT lf;
		::GetObject(hOldFont, sizeof(LOGFONT), &lf);
		lf.lfItalic = TRUE;

		if (m_pOwner->__cache.font) ::DeleteObject(m_pOwner->__cache.font);
		m_pOwner->__cache.font = ::CreateFontIndirect(&lf);
		if (m_pOwner->__cache.font)
			m_pOwner->ProcessFont(m_pOwner->__cache.font);
	}
	if (m_pOwner->__cache.font)
	{
		hOldFont = (HFONT)::SelectObject(hdc, m_pOwner->__cache.font);
	}

	m_pOwner->DrawChildren(hdc, rcPaint, bDraw, idd);
	m_pOwner->UpdateDrawData(idd);

	if (m_pOwner->__cache.font)
	{
		::SelectObject(hdc, hOldFont);
	}

	//if (!bDraw)
	//{
	//	if (m_pOwner->m_hRgn) ::DeleteObject(m_pOwner->m_hRgn);
	//	m_pOwner->m_hRgn = m_pOwner->CombineChildrenRegion();
	//	m_pOwner->UpdateRectFromRgn();
	//}
}

void CDuiUnderline::OnPaintContent(HDE hde, HDC hdc, RECT rcPaint, BOOL bDraw, InDrawData* pidd)
{
	InDrawData& idd = *pidd;
	m_pOwner->UpdateDrawData(idd);

	HFONT hOldFont = NULL;
	if (!bDraw)
	{
		hOldFont = (HFONT)::GetCurrentObject(hdc, OBJ_FONT);
		LOGFONT lf;
		::GetObject(hOldFont, sizeof(LOGFONT), &lf);
		lf.lfUnderline = TRUE;

		if (m_pOwner->__cache.font) ::DeleteObject(m_pOwner->__cache.font);
		m_pOwner->__cache.font = ::CreateFontIndirect(&lf);
		if (m_pOwner->__cache.font)
			m_pOwner->ProcessFont(m_pOwner->__cache.font);
	}
	if (m_pOwner->__cache.font)
	{
		hOldFont = (HFONT)::SelectObject(hdc, m_pOwner->__cache.font);
	}

	m_pOwner->DrawChildren(hdc, rcPaint, bDraw, idd);
	m_pOwner->UpdateDrawData(idd);

	if (m_pOwner->__cache.font)
	{
		::SelectObject(hdc, hOldFont);
	}

	//if (!bDraw)
	//{
	//	if (m_pOwner->m_hRgn) ::DeleteObject(m_pOwner->m_hRgn);
	//	m_pOwner->m_hRgn = m_pOwner->CombineChildrenRegion();
	//	m_pOwner->UpdateRectFromRgn();
	//}
}

void CDuiStrike::OnPaintContent(HDE hde, HDC hdc, RECT rcPaint, BOOL bDraw, InDrawData* pidd)
{
	InDrawData& idd = *pidd;
	m_pOwner->UpdateDrawData(idd);

	HFONT hOldFont = NULL;
	if (!bDraw)
	{
		hOldFont = (HFONT)::GetCurrentObject(hdc, OBJ_FONT);
		LOGFONT lf;
		::GetObject(hOldFont, sizeof(LOGFONT), &lf);
		lf.lfStrikeOut = TRUE;

		if (m_pOwner->__cache.font) ::DeleteObject(m_pOwner->__cache.font);
		m_pOwner->__cache.font = ::CreateFontIndirect(&lf);
		if (m_pOwner->__cache.font)
			m_pOwner->ProcessFont(m_pOwner->__cache.font);
	}
	if (m_pOwner->__cache.font)
	{
		hOldFont = (HFONT)::SelectObject(hdc, m_pOwner->__cache.font);
	}

	m_pOwner->DrawChildren(hdc, rcPaint, bDraw, idd);
	m_pOwner->UpdateDrawData(idd);

	if (m_pOwner->__cache.font)
	{
		::SelectObject(hdc, hOldFont);
	}

	//if (!bDraw)
	//{
	//	if (m_pOwner->m_hRgn) ::DeleteObject(m_pOwner->m_hRgn);
	//	m_pOwner->m_hRgn = m_pOwner->CombineChildrenRegion();
	//	m_pOwner->UpdateRectFromRgn();
	//}
}

void CDuiBig::OnPaintContent(HDE hde, HDC hdc, RECT rcPaint, BOOL bDraw, InDrawData* pidd)
{
	InDrawData& idd = *pidd;
	m_pOwner->UpdateDrawData(idd);

	HFONT hOldFont = NULL;
	if (!bDraw)
	{
		hOldFont = (HFONT)::GetCurrentObject(hdc, OBJ_FONT);
		LOGFONT lf;
		::GetObject(hOldFont, sizeof(LOGFONT), &lf);
		lf.lfHeight = -(abs(lf.lfHeight) * 6 / 5);

		if (m_pOwner->__cache.font) ::DeleteObject(m_pOwner->__cache.font);
		m_pOwner->__cache.font = ::CreateFontIndirect(&lf);
		if (m_pOwner->__cache.font)
			m_pOwner->ProcessFont(m_pOwner->__cache.font);
	}
	if (m_pOwner->__cache.font)
	{
		hOldFont = (HFONT)::SelectObject(hdc, m_pOwner->__cache.font);
	}

	//m_pOwner->DrawChildren(hdc, rcPaint, bDraw, idd);
	//m_pOwner->UpdateDrawData(idd);

	InDrawData iddOld = idd;
	TEXTMETRIC tm;
	GetTextMetrics(hdc, &tm);
	idd.cyCurLine = tm.tmExternalLeading + tm.tmHeight;
	idd.cyMinHeight = max(idd.cyCurLine, idd.cyMinHeight);

	m_pOwner->DrawChildren(hdc, rcPaint, bDraw, idd);
	m_pOwner->UpdateDrawData(idd);

	// 还原idd的部分数据
	idd.cyCurLine = iddOld.cyCurLine;
	idd.cyMinHeight = max(iddOld.cyMinHeight, idd.cyMinHeight);

	if (m_pOwner->__cache.font)
	{
		::SelectObject(hdc, hOldFont);
	}

	//if (!bDraw)
	//{
	//	if (m_pOwner->m_hRgn) ::DeleteObject(m_pOwner->m_hRgn);
	//	m_pOwner->m_hRgn = m_pOwner->CombineChildrenRegion();
	//	m_pOwner->UpdateRectFromRgn();
	//}
}

void CDuiSmall::OnPaintContent(HDE hde, HDC hdc, RECT rcPaint, BOOL bDraw, InDrawData* pidd)
{
	InDrawData& idd = *pidd;
	m_pOwner->UpdateDrawData(idd);

	HFONT hOldFont = NULL;
	if (!bDraw)
	{
		hOldFont = (HFONT)::GetCurrentObject(hdc, OBJ_FONT);
		LOGFONT lf;
		::GetObject(hOldFont, sizeof(LOGFONT), &lf);
		lf.lfHeight = -(abs(lf.lfHeight) * 4 / 5);

		if (m_pOwner->__cache.font) ::DeleteObject(m_pOwner->__cache.font);
		m_pOwner->__cache.font = ::CreateFontIndirect(&lf);
		if (m_pOwner->__cache.font)
			m_pOwner->ProcessFont(m_pOwner->__cache.font);
	}
	if (m_pOwner->__cache.font)
	{
		hOldFont = (HFONT)::SelectObject(hdc, m_pOwner->__cache.font);
	}

	InDrawData iddOld = idd;
	TEXTMETRIC tm;
	GetTextMetrics(hdc, &tm);
	idd.cyCurLine = tm.tmExternalLeading + tm.tmHeight;
	idd.cyMinHeight = max(idd.cyCurLine, idd.cyMinHeight);

	m_pOwner->DrawChildren(hdc, rcPaint, bDraw, idd);
	m_pOwner->UpdateDrawData(idd);

	// 还原idd的部分数据
	idd.cyCurLine = iddOld.cyCurLine;
	idd.cyMinHeight = max(iddOld.cyMinHeight, idd.cyMinHeight);

	if (m_pOwner->__cache.font)
	{
		::SelectObject(hdc, hOldFont);
	}

	//if (!bDraw)
	//{
	//	if (m_pOwner->m_hRgn) ::DeleteObject(m_pOwner->m_hRgn);
	//	m_pOwner->m_hRgn = m_pOwner->CombineChildrenRegion();
	//	m_pOwner->UpdateRectFromRgn();
	//}
}


void CDuiWhiteSpace::OnPaintContent( HDE hde, HDC hdc, RECT rcPaint, BOOL bDraw, InDrawData* pidd )
{
	InDrawData& idd = *pidd;
	m_pOwner->UpdateDrawData(idd);
	CPoint pt = idd.pt;
	LONG width = FromStyleVariant(m_pOwner->GetStyleAttribute(SAT_WIDTH), LONG, (LPVOID)(LONG_PTR)m_pOwner->GetOffsetParentSize().cx);
	if (width == 0) width = 12;
	idd.pt.x += min(width, idd.rc.right-idd.pt.x);
	m_pOwner->UpdateDrawData(idd);

	if (!bDraw)
	{
		m_pOwner->__cache.rects.RemoveAll();
		RECT rc = {pt.x, pt.y, idd.pt.x, idd.pt.y+idd.cyCurLine};
		m_pOwner->AddRect(&rc, NULL, 0, idd);
		//if (m_pOwner->m_hRgn) ::DeleteObject(m_pOwner->m_hRgn);
		//m_pOwner->m_hRgn = CreateRectRgn(pt.x, pt.y, idd.pt.x, idd.pt.y+idd.cyCurLine);
		//m_pOwner->UpdateRectFromRgn();
	}
}

void CDuiParagraph::OnPaintContent( HDE hde, HDC hdc, RECT rcPaint, BOOL bDraw, InDrawData* pidd )
{
	InDrawData& idd = *pidd;
	LONG lineSpace = m_pOwner->_attributes.GetAttributeLong(L"linespace", 5);
	LONG indent = m_pOwner->_attributes.GetAttributeLong(L"indent");
	indent = min(abs(indent), 100); // 最大 100
	if (indent >= idd.rc.right-idd.rc.left-20)
		indent = (idd.rc.right-idd.rc.left)/2; // 如果太大，只取矩形宽度的一半

	InDrawData iddOld = idd;
	//LONG old_indent = idd.lLineIndent;
	BOOL bHasContent = idd.pt.x > idd.rc.left + idd.lLineIndent;

	idd.lLineIndent = indent;
	idd.pt.x = idd.rc.left + idd.lLineIndent;

	if (bHasContent) // 当前行已有内容，需要换行
	{
		DuiNode::ReposInlineRow(idd);
		idd.pt.y += lineSpace + idd.cyMinHeight + idd.lLineDistance;
		idd.cyTotalHeight += lineSpace + idd.cyMinHeight + idd.lLineDistance;
		idd.cyMinHeight = idd.cyCurLine;
	}
	else // 当前行本来就在开头，无需换行，但要调整行间隔
	{
		idd.pt.y += lineSpace;
		idd.cyTotalHeight += lineSpace;
	}
	idd.lLineDistance = abs(FromStyleVariant(m_pOwner->GetStyleAttribute(SAT_LINE_DISTANCE), LONG));

	m_pOwner->DrawChildren(hdc, rcPaint, bDraw, idd);

	if (idd.pt.x>idd.rc.left+idd.lLineIndent)
	{
		DuiNode::ReposInlineRow(idd);
		idd.pt.x = idd.rc.left + idd.lLineIndent;
		idd.pt.y += idd.cyMinHeight + idd.lLineDistance;
		idd.cyTotalHeight += idd.cyMinHeight + idd.lLineDistance;
		idd.cyMinHeight = idd.cyCurLine;
	}

	// restore old data
	idd.lLineIndent = iddOld.lLineIndent;
	idd.cyMinHeight = max(iddOld.cyMinHeight, idd.cyMinHeight);
	idd.lLineDistance = iddOld.lLineDistance;

	//if (!bDraw)
	//{
	//	if (m_pOwner->m_hRgn) ::DeleteObject(m_pOwner->m_hRgn);
	//	m_pOwner->m_hRgn = m_pOwner->CombineChildrenRegion();
	//	m_pOwner->UpdateRectFromRgn();
	//}
}

void CDuiLineBreak::OnPaintContent( HDE hde, HDC hdc, RECT rcPaint, BOOL bDraw, InDrawData* pidd )
{
	InDrawData& idd = *pidd;
	DuiNode::ReposInlineRow(idd);
	idd.pt.x = idd.rc.left + idd.lLineIndent;
	idd.pt.y += idd.cyMinHeight + idd.lLineDistance;
	idd.cyTotalHeight += idd.cyMinHeight + idd.lLineDistance;
	idd.cyMinHeight = idd.cyCurLine;

	if (!bDraw)
	{
		// br 没有RGN
		if (m_pOwner->m_hRgn) ::DeleteObject(m_pOwner->m_hRgn);
		m_pOwner->m_hRgn = NULL;
		m_pOwner->m_rcItem = CRect(idd.pt, CSize(0,idd.cyMinHeight));
		m_pOwner->__cache.hasPos = TRUE;
	}
}

void CDuiHorizontalLine::OnPaintContent( HDE hde, HDC hdc, RECT rcPaint, BOOL bDraw, InDrawData* pidd )
{
	InDrawData& idd = *pidd;
	m_pOwner->UpdateDrawData(idd);
	LONG width = FromStyleVariant(m_pOwner->GetStyleAttribute(SAT_WIDTH, SVT_EMPTY, DUISTYLEMATCH_NAME|DUISTYLEMATCH_ID), LONG, (LPVOID)(LONG_PTR)m_pOwner->GetOffsetParentSize().cx);
	COLORREF clr = FromStyleVariant(m_pOwner->GetStyleAttribute(SAT_COLOR, SVT_EMPTY, DUISTYLEMATCH_NAME|DUISTYLEMATCH_ID), COLORREF);
	if (clr == CLR_INVALID) clr = RGB(145,146,119); // 默认灰色

	if (bDraw && m_pOwner->__cache.rects.GetSize()>0)
	{
		CRect rc = m_pOwner->__cache.rects[0].rc;
		if (IsGdi32(m_pOwner->m_pLayoutMgr))
		{
			DrawLine32(hdc, rc.left, rc.top, rc.right, rc.bottom, clr);
		}
		else
		{
			HPEN hNewPen = ::CreatePen(PS_SOLID, 1, clr);
			HPEN hOldPen;
			if (hNewPen) hOldPen = (HPEN)::SelectObject(hdc, hNewPen);

			::MoveToEx(hdc, rc.left, rc.top, NULL);
			::LineTo(hdc, rc.right, rc.top);

			if (hNewPen)
			{
				::SelectObject(hdc, hOldPen);
				::DeleteObject(hNewPen);
			}
		}

		//if (width > 0) m_pOwner->UpdateDrawData(idd);
		return;
	}

	CPoint pt = idd.pt;
	LONG cx = width;
	if (cx<=0 || cx>idd.rc.right-pt.x) cx = idd.rc.right - pt.x;
	pt.y += idd.cyMinHeight/2;
	if (width <= 0)
	{
		idd.pt.x = idd.rc.right;

		DuiNode::ReposInlineRow(idd);
		idd.pt.x = idd.rc.left + idd.lLineIndent;
		idd.pt.y += idd.cyMinHeight + idd.lLineDistance;
		idd.cyTotalHeight += idd.cyMinHeight + idd.lLineDistance;
		idd.cyMinHeight = idd.cyCurLine;
	}
	else
	{
		idd.pt.x += width;
	}

	{
		m_pOwner->__cache.rects.RemoveAll();
		CRect rc(pt.x, pt.y, pt.x+cx, pt.y+1);
		m_pOwner->AddRect(&rc, NULL, 0, idd);
		////m_pOwner->AddRect(&CRect(pt, CSize(cx,idd.cyMinHeight)));
		//m_pOwner->UpdateRectFromRgn(m_pOwner->UpdateRgnFromRects());
	}
}

void CDuiHorizontalLine::OnReposInlineRect( HANDLE, int index, long cx, InDrawData* pidd )
{
	//InDrawData& idd = *pidd;
	if (index==0 && m_pOwner->__cache.rects.GetSize()>0)
	{
		LONG width = FromStyleVariant(m_pOwner->GetStyleAttribute(SAT_WIDTH, SVT_EMPTY, DUISTYLEMATCH_NAME|DUISTYLEMATCH_ID), LONG, (LPVOID)(LONG_PTR)m_pOwner->GetOffsetParentSize().cx);
		m_pOwner->__cache.rects[0].rc.left += cx;
		if (width > 0) m_pOwner->__cache.rects[0].rc.right += cx;
	}
}

void CDuiImg::OnPaintContent( HDE hde, HDC hdc, RECT rcPaint, BOOL bDraw, InDrawData* pidd )
{
	InDrawData& idd = *pidd;
	m_pOwner->UpdateDrawData(idd);
#ifndef NO3D
	//// 3d 模式不须绘制
	if (m_pOwner->m_pLayoutMgr->Get3DDevice() && bDraw) return;
#endif

	CDuiImageResource* pSrc = m_pOwner->m_pLayoutMgr->GetImageResource(m_pOwner->_attributes.GetAttributeString(L"src"));
	if (pSrc==NULL || pSrc->img==NULL
#ifndef NO3D
		|| (m_pOwner->m_pLayoutMgr->Get3DDevice() && !pSrc->img3d->IsLoaded()) || (m_pOwner->m_pLayoutMgr->Get3DDevice()==NULL && pSrc->img->IsNull())
#endif
		)
		return;

	if (bDraw)
	{
		if (m_pOwner->__cache.rects.GetSize()>0)
			pSrc->img->Draw(hdc, m_pOwner->__cache.rects[0].rc);
		else
			pSrc->img->Draw(hdc, CRect(0,0,pSrc->img->GetWidth(),pSrc->img->GetHeight()));
		return;
	}

	long w = (
#ifndef NO3D
		m_pOwner->m_pLayoutMgr->Get3DDevice()?pSrc->img3d->GetWidth():
#endif
		pSrc->img->GetWidth());
	long h = (
#ifndef NO3D
		m_pOwner->m_pLayoutMgr->Get3DDevice()?pSrc->img3d->GetHeight():
#endif
		pSrc->img->GetHeight());

	SIZE szp = m_pOwner->GetOffsetParentSize();
	CSize sz(FromStyleVariant(m_pOwner->GetStyleAttribute(SAT_WIDTH), LONG, (LPVOID)(LONG_PTR)szp.cx), FromStyleVariant(m_pOwner->GetStyleAttribute(SAT_HEIGHT), LONG, (LPVOID)(LONG_PTR)szp.cy));
	if (!m_pOwner->IsInline())
	{
		m_pOwner->UpdateDrawData(idd);
		if (sz.cx <= 0) sz.cx = C2TX(m_pOwner, w);
		if (sz.cy <= 0) sz.cy = C2TY(m_pOwner, h);
		//CRect rc(idd.pt, sz);
		//m_pOwner->SetPos(rc);
		DuiNode::ReposInlineRow(idd);
		idd.pt.x = idd.rc.left + idd.lLineIndent;
		idd.pt.y += sz.cy + idd.lLineDistance;
		idd.cyTotalHeight += sz.cy + idd.lLineDistance;
		idd.cyMinHeight = idd.cyCurLine;
		return;
	}

	if (sz.cx <= 0) sz.cx = w;
	if (sz.cx >= idd.rc.right-idd.rc.left-idd.lLineIndent) sz.cx = idd.rc.right-idd.rc.left-idd.lLineIndent;
	if (sz.cy <= 0) sz.cy = h;
	if (idd.pt.x+sz.cx >= idd.rc.right)
	{
		DuiNode::ReposInlineRow(idd);
		idd.pt.x = idd.rc.left + idd.lLineIndent;
		idd.pt.y += idd.cyMinHeight + idd.lLineDistance;
		idd.cyTotalHeight += idd.cyMinHeight + idd.lLineDistance;
		idd.cyMinHeight = idd.cyCurLine;
	}

	CRect rc(idd.pt, sz);
	idd.pt.x += sz.cx;
	idd.cyMinHeight = max(idd.cyMinHeight, sz.cy);
	m_pOwner->UpdateDrawData(idd);

	//if (bDraw)
	//{
	//	//CDuiControlExtension::AdjustOffset(idd.owner, NULL, rc);
	//	pSrc->img->Draw(hdc, rc);
	//}

#ifndef NO3D
	//// 更新3D表面
	if (m_pOwner->m_pLayoutMgr->Get3DDevice())
	{
		//RECT rcContent = CDuiControlExtension::GetClientRect(this, DUIRT_TOTAL);
		CRect rcClient = rc;
		DuiNode::AdjustOffset(m_pOwner->GetOffsetParent(), NULL, rcClient);
		m_pOwner->Update3DSurface();
		//OnUpdate3DSurface(rcClient);
	}
#endif
	//else
	{
		m_pOwner->__cache.rects.RemoveAll();
		m_pOwner->AddRect(&rc, NULL, 0, idd);
		//r->UpdateRectFromRgn(r->UpdateRgnFromRects());
	}
}

void CDuiImg::SetAttribute( HDE hde, LPCWSTR szName, VARIANT* value )
{
	__super::SetAttribute(hde, szName, value);

	if (lstrcmpiW(szName, L"src") == 0)
	{
		gConnector.Disconnect(this, NULL, cp_img_loaded);
		CDuiImageResource* pSrc = m_pOwner->m_pLayoutMgr->GetImageResource(value->bstrVal, TRUE);
		if (pSrc && pSrc->img)
		{
			CDuiStyle* style = m_pOwner->GetPrivateStyle(TRUE);
			if (style)
			{
				CDuiStyleVariant* v = (CDuiStyleVariant*)style->GetPrivateAttribute(SAT_BACKGROUND, TRUE);
				StyleVariantClear(v);
				v->sat = SAT_BACKGROUND;
				v->svt = SVT_EXTERNAL;
				v->__cache = (UINT_PTR)ext_image_normal;
				v->extVal = pSrc;
				//style->FireChangeEvent(v);
			}

			gConnector.Connect(this, pSrc->img, cp_img_loaded, (CDuiConnectorCallback*)this);
			__updateSize();
		}
	}
	//if (!r->m_pLayoutMgr->Get3DDevice() || r->m_pSurface)
	//	CDuiControlExtension::DelayedResize(this);
	//	//CDuiControlExtension::UpdateLayout(CDuiControlExtension::GetOffsetParent(this));
}

BOOL CDuiImg::OnHit( LPVOID pKey, LPVOID pVal, DWORD flag )
{
	if (flag & cp_img_loaded)
	{
		CDuiImg* i = (CDuiImg*)pKey;
		i->__updateSize();

		//CDuiControlExtension::UpdateLayout(CDuiControlExtension::GetOffsetParent(this));

		m_pOwner->DelayedResize();
	}
	return FALSE;
}

CDuiImg::~CDuiImg()
{
	gConnector.Disconnect(this);
}

//void CDuiImg::OnUpdate3DSurface(HANDLE, RECT rc)
//{
//	DuiNode* r = rt(this);
//	CRect rcClient = rc;
//	if (r->m_pSurface==NULL)
//	{
//		r->m_pSurface = GdiLayer::New(r->m_pLayoutMgr->Get3DDevice(), rcClient.left, rcClient.top, rcClient.Width(), rcClient.Height(),
//			r->m_pParent ? rt(r->m_pParent)->m_pSurface : NULL);
//		if (r->m_pSurface)
//		{
//			//r->m_pSurface->SetPaintCallback(this); // 没有GDI操作，所以无需设置回调
//			r->Update3DState();
//			CDuiImageResource* pSrc = r->m_pLayoutMgr->GetImageResource(r->_attributes.GetAttributeString(L"src"));
//			if (pSrc)
//				r->m_pSurface->Background()->SetImage(pSrc->img3d);
//			//r->m_pSurface->ClipContent(&CDuiControlExtension::GetClipRect(this, DUIRT_CONTENT));
//		}
//	}
//	else
//	{
//		r->m_pSurface->Move(rcClient.left, rcClient.top, rcClient.right, rcClient.bottom);
//	}
//}

void CDuiImg::__updateSize()
{
	CDuiImageResource* pSrc = m_pOwner->m_pLayoutMgr->GetImageResource(m_pOwner->_attributes.GetAttributeString(L"src"));
	if (pSrc==NULL) return;

	long w=0, h=0;
#ifndef NO3D
	if (m_pOwner->m_pLayoutMgr->Get3DDevice())
	{
		w = pSrc->img3d->GetWidth();
		h = pSrc->img3d->GetHeight();
	}
	else
#endif
	{
		w = (long)pSrc->img->GetWidth();
		h = (long)pSrc->img->GetHeight();
	}

	if (w>0 && h>0)
	{
		if (m_pOwner->IsBlock())
		{
			SIZE sz = {w,h};
			sz = m_pOwner->AdjustSize(sz);
			w = sz.cx;
			h = sz.cy;
		}
		CDuiStyle* style = m_pOwner->GetPrivateStyle(TRUE);
		if (style)
		{
			CDuiStyleVariant* v = (CDuiStyleVariant*)style->GetPrivateAttribute(SAT_MINWIDTH, TRUE);
			*v = w;
			style->FireChangeEvent(v);
			v = (CDuiStyleVariant*)style->GetPrivateAttribute(SAT_MINHEIGHT, TRUE);
			*v = h;
			style->FireChangeEvent(v);
		}
	}
}

//////////////////////////////////////////////////////////////////////////
void CDuiAnchor::OnPaintContent( HDE hde, HDC hdc, RECT rcPaint, BOOL bDraw, InDrawData* pidd )
{
	InDrawData& idd = *pidd;
	m_pOwner->UpdateDrawData(idd);

	CDCHandle dc(hdc);
	COLORREF _clr, clr = FromStyleVariant(m_pOwner->GetStyleAttribute(SAT_COLOR), COLORREF);
	if (clr != CLR_INVALID) _clr = dc.SetTextColor(clr);

	HFONT hOldFont = NULL;
	if (!bDraw)
	{
		LOGFONTW lf;
		if (m_pOwner->__cache.font) ::DeleteObject(m_pOwner->__cache.font); m_pOwner->__cache.font = NULL;
		if (m_pOwner->m_pLayoutMgr->BuildControlFont(m_pOwner, &lf))
		{
			m_pOwner->__cache.font = ::CreateFontIndirectW(&lf);
			if (m_pOwner->__cache.font)
				m_pOwner->ProcessFont(m_pOwner->__cache.font);
		}
	}
	if (m_pOwner->__cache.font)
	{
		hOldFont = (HFONT)::SelectObject(hdc, m_pOwner->__cache.font);
	}


	UINT _style, style = (UINT)FromStyleVariant(m_pOwner->GetStyleAttribute(SAT_TEXT_STYLE), LONG);
	if (style) _style=idd.uTextStyle, idd.uTextStyle=style;

	m_pOwner->DrawChildren(hdc, rcPaint, bDraw, idd);
	m_pOwner->UpdateDrawData(idd);
	if (!bDraw && !m_pOwner->IsBlock()) // 要修复矩形内容
	{
		if (m_pOwner->m_hRgn) ::DeleteObject(m_pOwner->m_hRgn);
		m_pOwner->m_hRgn = m_pOwner->CombineChildrenRegion();
		m_pOwner->UpdateRectFromRgn(m_pOwner->m_hRgn);
		//if (m_hRgn) ::DeleteObject(m_hRgn);
		//m_hRgn = NULL;
	}

	if (clr != CLR_INVALID) dc.SetTextColor(_clr);
	if (m_pOwner->__cache.font) ::SelectObject(hdc, (HGDIOBJ)hOldFont); // dc.SelectFont(oldfont);
	if (style) idd.uTextStyle = _style;
}

BOOL CDuiAnchor::Activate(HDE hde)
{
	if (!__super::Activate(hde)) return FALSE;
	LPCOLESTR url = m_pOwner->_attributes.GetAttributeString(L"href");
	if (url && ::PathIsURLW(url))
	{
		::ShellExecuteW(NULL, L"open", url, NULL, NULL, SW_SHOWNORMAL);
	}
	return TRUE;
}