#include "stdafx.h"
#include "DuiVisual.h"
#include "../../common/theme.h"
#include "DuiStyleParser.h"
#include "DuiAlpha.h"
#include "DuiManager.h"
#include "DuiPluginsManager.h"

IVisual* VisualFromName( LPCOLESTR name )
{
	if (name==NULL || *name==0)
		return NULL;

	int len = lstrlenW(name);
	for (VisualEntry** ppEntry = &__pvisualEntryFirst; ppEntry < &__pvisualEntryLast; ppEntry++)
	{
		if (*ppEntry != NULL)
		{
			VisualEntry* pEntry = *ppEntry;
			if ((len==pEntry->lenName && lstrcmpiW(name,pEntry->szName)==0))
			{
				return pEntry->pVisual;
			}
		}
	}
	return NULL;
}


//////////////////////////////////////////////////////////////////////////
// layer images render helper
class CLayerRenderHelper
{
	CStdPtrArray& _layers;
	HDC _hdc;

public:
	CLayerRenderHelper(HDC hdc, CStdPtrArray& layers) : _hdc(hdc), _layers(layers) {}
	~CLayerRenderHelper()
	{
		for (int i=0; i<_layers.GetSize(); i++)
		{
			CDuiImageLayer* pLayer = static_cast<CDuiImageLayer*>(_layers[i]);
			if (pLayer) pLayer->DrawToDC(_hdc);
		}
	}
};

//////////////////////////////////////////////////////////////////////////
// 默认外观实现，以 API 和 Visual 两种方式公开
class DefVisual : public IVisual
{
public:
	virtual RECT GetAdjustRect(HDE hde) const
	{ return CRect(0,0,0,0); }

	virtual BOOL OnPaintBorder(HDE hde, HDC hdc, RECT rcBorder)
	{
		DuiNode* r = DuiHandle<DuiNode>(hde);
		CDCHandle dc(hdc);
		rcBorder = r->GetPos(DUIRT_BORDER);
		//CDuiControlExtension::AdjustOffset(r->GetParent(), NULL, rcBorder);
		// 重置原点的体制已经不需要再判断相交了
		//RECT _rc;
		//if (!::IntersectRect(&_rc, &rcBorder, &rc)) return;
		LOGPEN lp;
		if (r->m_pLayoutMgr->BuildControlPen(r, &lp))
		{
			if (IsGdi32(r->m_pLayoutMgr))
				//if (IsLayered(r->m_pLayoutMgr->GetPaintWindow()))
			{
				DrawBorder32(hdc, rcBorder, lp);
			}
			else
			{
				CPen pen;
				pen.CreatePenIndirect(&lp);
				if (pen)
				{
					HPEN hOlePen = dc.SelectPen(pen);
					HBRUSH hOldBrush = dc.SelectBrush((HBRUSH)::GetStockObject(NULL_BRUSH));
#ifndef NO3D
					if (r->m_pathClip.IsValid())
					{
						if (::BeginPath(hdc))
						{
							r->m_pathClip.Draw(hdc, rcBorder);
							::EndPath(hdc);
							::StrokePath(hdc);
						}
					}
					else
#endif
						dc.Rectangle(&rcBorder);
					dc.SelectBrush(hOldBrush);
					dc.SelectPen(hOlePen);
				}
			}
		}

		return TRUE;
	}

	virtual BOOL OnPaintFocus(HDE hde, HDC hdc, RECT rectFocus, HRGN hrgn)
	{
		DuiNode* r = DuiHandle<DuiNode>(hde);

		if (hrgn)
		{
			HBRUSH br = ::CreateSolidBrush(RGB(38,209,254));
			//HRGN dst = ::CreateRectRgn(0,0,0,0);
			//ATLASSERT(dst);
			//::CombineRgn(dst, dst, hrgn, RGN_OR);
			//POINT pt = {0};
			//pt = CDuiControlExtension::GetOffset(rt(pCtrl)->GetParent(), NULL, pt);
			//::OffsetRgn(dst, pt.x, pt.y);
			//::FrameRgn(hdc, dst, br, 1, 1);
			::FrameRgn(hdc, hrgn, br, 1, 1);
			//::DeleteObject(br);
			//::DeleteObject(dst);
			return TRUE;
		}

		CDCHandle dc(hdc);
		CRect rcFocus = r->GetPos(DUIRT_INSIDEBORDER);
		//CDuiControlExtension::AdjustOffset(r->GetParent(), NULL, rcFocus);
		rcFocus.DeflateRect(1,1);
		LOGPEN lp = {PS_DOT, 1, RGB(38,209,254)};
		if (IsGdi32(r->m_pLayoutMgr))
			//if (IsLayered(r->m_pLayoutMgr->GetPaintWindow()))
		{
			DrawBorder32(hdc, rcFocus, lp);
		}
		else
		{
			CPen pen;
			pen.CreatePenIndirect(&lp);
			if (pen)
			{
				HPEN hOlePen = dc.SelectPen(pen);
				HBRUSH hOldBrush = dc.SelectBrush((HBRUSH)::GetStockObject(NULL_BRUSH));
				dc.Rectangle(&rcFocus);
				dc.SelectBrush(hOldBrush);
				dc.SelectPen(hOlePen);
			}
		}

		return TRUE;
	}

	virtual BOOL OnPaintBackground(HDE hde, HDC hdc, RECT rectBkgnd)
	{
		DuiNode* r = DuiHandle<DuiNode>(hde);
		CDCHandle dc(hdc);

#ifndef NO3D
		if (r->m_pSurface) return TRUE; // 3D DO NOT ANYTHING
#endif
		CRect rcBkgnd = r->GetPos(DUIRT_INSIDEBORDER);
		//CRect rcBkgndClip;
		//if (!rcBkgndClip.IntersectRect(&rcBkgnd, &rc)) return;

		CLayerRenderHelper postRender(hdc, r->m_PostBackgroundImages); // 在函数返回前绘制后置背景层

		CDuiStyleVariant* sv = (CDuiStyleVariant*)(r)->GetStyleAttribute(SAT_BACKGROUND);
		BOOL isRoot = (r->GetParent()==NULL);
		BOOL isPendingImg = (sv && sv->svt==SVT_EXTERNAL && ((CDuiImageResource*)sv->extVal)->IsNull());

		if (sv == &svEmpty || sv==NULL || sv->svt==SVT_EMPTY || (isRoot && isPendingImg))
		{
			// 当根元素没有设置背景的时候，用白色填充
			if (!isRoot /*&& !isPendingImg*/)
				return TRUE;

			static CDuiStyleVariant rootBkgnd(SAT_BACKGROUND, (COLORREF)0xFFFFFFFF ); // 默认白色背景
			sv = &rootBkgnd;
		}

		BOOL bOver = FALSE;
		if (!isRoot)
		{
			bOver = TRUE;
			CComVariant v = r->GetAttribute(L"background-over");
			if (v.vt == VT_BSTR) bOver = ParseBoolString(v.bstrVal);
			else if ((v.vt!=VT_EMPTY) && SUCCEEDED(v.ChangeType(VT_BOOL))) bOver = (V_BOOL(&v) != VARIANT_FALSE);
		}

		if (r->__cache.imgBkgnd.IsNull())
		{
			r->__cache.imgBkgnd.Init(rcBkgnd.Width(), rcBkgnd.Height());
			if (sv->svt == SVT_COLOR)
			{
				BYTE bAlpha = 255;
				CComVariant v = (r)->GetAttribute(L"background-alpha");
				if (v.vt!=VT_EMPTY && SUCCEEDED(v.ChangeType(VT_UI1)))
					bAlpha = (BYTE)V_UI1(&v);

				COLORREF clr = FromStyleVariant(sv, COLORREF);
				r->__cache.imgBkgnd.FillFrom(clr, bAlpha);
				//if (IsLayered(((CDuiLayoutManager*)pCtrl->GetLayoutManager())->GetPaintWindow())/* || bAlpha!=255*/)
				//{
				//	//FillRect32(hdc, rcBkgnd, 0, 255); // 先用不透明黑色填充
				//	FillRect32(hdc, rcBkgnd, clr, bAlpha);
				//}
				//else if (clr != CLR_INVALID)
				//{
				//	dc.FillSolidRect(&rcBkgnd, clr);
				//}
			}
			else if (sv->svt == SVT_EXTERNAL)
			{
				// root and ispendingimg draw white
				if (isRoot)
				{
					r->__cache.imgBkgnd.FillFrom(RGB(255,255,255));
				}
				CDuiImageResource* pSrc = (CDuiImageResource*)sv->extVal;
				if (pSrc)
				{
					ExternalType et = (ExternalType)sv->__cache;
					switch (et)
					{
						//case ext_image_normal: pSrc->img->Draw(hdc, rcBkgnd, CRect(0,0,rcBkgnd.Width(),rcBkgnd.Height()), !bOver); break;
						//case ext_image_stretch: pSrc->img->Draw(hdc, rcBkgnd/*, rcBkgndClip*/, !bOver); break;
						//case ext_image_tile: pSrc->img->Tile(hdc, rcBkgnd, !bOver); break;
					case ext_image_center: // TODO:非3D模式下，暂时跟normal相同
					case ext_image_normal: r->__cache.imgBkgnd.DrawFrom(pSrc->img, FALSE); break;
					case ext_image_maxfit: // TODO:非3D模式下，暂时跟stretch相同
					case ext_image_stretch: r->__cache.imgBkgnd.DrawFrom(pSrc->img, TRUE); break;
					case ext_image_tile: r->__cache.imgBkgnd.TileFrom(pSrc->img); break;
					case ext_image_squares: r->__cache.imgBkgnd.DrawFrom(pSrc->img, TRUE, TRUE); break;
					default:; // ext_unknown
					}
				}
			}
		}
		if (!r->__cache.imgBkgnd.IsNull())
			r->__cache.imgBkgnd.Draw(hdc, rcBkgnd, !bOver);

		return TRUE;
	}

	virtual BOOL OnPaintForeground(HDE hde, HDC hdc, RECT rcFrgnd)
	{
		DuiNode* r = DuiHandle<DuiNode>(hde);
		CLayerRenderHelper(hdc, r->m_PreForegroundImages); // 立即绘制前置前景层
		CLayerRenderHelper postRender(hdc, r->m_PostForegroundImages); // 在函数返回前绘制后置前景层

		CDCHandle dc(hdc);
		CRect rcfgnd = r->GetPos(DUIRT_INSIDEBORDER);
		//CRect rcfgndClip;
		//if (!rcfgndClip.IntersectRect(&rcfgnd, &rc)) return;
		CDuiStyleVariant* sv = (CDuiStyleVariant*)(r)->GetStyleAttribute(SAT_FOREGROUND);
		if (sv == &svEmpty || sv==NULL) return TRUE;

		BYTE bAlpha = 50;
		CComVariant v = (r)->GetAttribute(L"foreground-alpha");
		if (v.vt!=VT_EMPTY && SUCCEEDED(v.ChangeType(VT_UI1)))
			bAlpha = (BYTE)V_UI1(&v);

		if (sv->svt == SVT_COLOR)
		{
			COLORREF clr = FromStyleVariant(sv, COLORREF);
			FillRect32(hdc, rcfgnd, clr, bAlpha);
		}
		else if (sv->svt == SVT_EXTERNAL)
		{
			CDuiImageResource* pSrc = (CDuiImageResource*)sv->extVal;
			if (pSrc)
			{
				ExternalType et = (ExternalType)sv->__cache;
				switch (et)
				{
				case ext_image_center: // TODO:非3D模式下，暂时跟normal相同
				case ext_image_normal: pSrc->img->Draw(hdc, rcfgnd, CRect(0,0,rcfgnd.Width(),rcfgnd.Height())); break;
				case ext_image_maxfit: // TODO:非3D模式下，暂时跟stretch相同
				case ext_image_stretch: pSrc->img->Draw(hdc, rcfgnd); break;
				case ext_image_tile: pSrc->img->Tile(hdc, rcfgnd); break;
				case ext_image_squares: pSrc->img->Draw(hdc, rcfgnd, TRUE); break;
				default:; // ext_unknown
				}
			}
		}

		return TRUE;
	}
};

DefVisual g_defVisual;

//////////////////////////////////////////////////////////////////////////
// Visual API
// 
IVisual* DUIAPI GetDefaultVisual()
{
	return &g_defVisual;
}

BOOL DUIAPI DefPaintBorder(HDE hde, HDC hdc, RECT rcBorder)
{
	return g_defVisual.OnPaintBorder(hde, hdc, rcBorder);
}

BOOL DUIAPI DefPaintFocus(HDE hde, HDC hdc, RECT rcFocus, HRGN hrgn)
{
	return g_defVisual.OnPaintFocus(hde, hdc, rcFocus, hrgn);
}

BOOL DUIAPI DefPaintBackground(HDE hde, HDC hdc, RECT rcBkgnd)
{
	return g_defVisual.OnPaintBackground(hde, hdc, rcBkgnd);
}

BOOL DUIAPI DefPaintForeground(HDE hde, HDC hdc, RECT rcFrgnd)
{
	return g_defVisual.OnPaintForeground(hde, hdc, rcFrgnd);
}

//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
BOOL VisualButton::OnPaintBorder( HDE hde, HDC hdc, RECT rcBorder )
{
	DuiNode* r = DuiHandle<DuiNode>(hde);
	CRect _rcBorder = rcBorder; //_owner->GetPos(DUIRT_BORDER);
	DWORD dwState = r->GetState();
	int state = (dwState & DUISTATE_DISABLED) ? PBS_DISABLED :
		(dwState & DUISTATE_PUSHED) ? PBS_PRESSED :
		(dwState & DUISTATE_HOT) ? PBS_HOT : 
		(dwState & DUISTATE_FOCUSED) ? PBS_DEFAULTED : PBS_NORMAL;
	theme_ptr->PaintButton(hdc, BP_PUSHBUTTON, state, 0, &_rcBorder);
	return TRUE;
}

IMPLEMENT_SIMPLE_VISUAL_RECT(Option, (20,0,0,0))( HDE hde, HDC hdc, RECT rcBkgnd )
{
	DuiNode* r = DuiHandle<DuiNode>(hde);

	// 首先调用插件或默认的背景绘制过程
	PluginCallContext(TRUE).Call(OpCode_PaintBackground, hde, hdc, rcBkgnd);
	//g_defVisual.OnPaintBackground(hde, hdc, rcBkgnd); // 让控件定义的背景样式有机会重绘

	rcBkgnd.right = rcBkgnd.left + 20;
	DWORD dwState = r->GetState();
	int state;
	if (dwState & DUISTATE_CHECKED)
		state = (dwState & DUISTATE_DISABLED) ? RBS_CHECKEDDISABLED :
			(dwState & DUISTATE_PUSHED) ? RBS_CHECKEDPRESSED :
			(dwState & DUISTATE_HOT) ? RBS_CHECKEDHOT : RBS_CHECKEDNORMAL;
	else
		state = (dwState & DUISTATE_DISABLED) ? RBS_UNCHECKEDDISABLED :
			(dwState & DUISTATE_PUSHED) ? RBS_UNCHECKEDPRESSED :
			(dwState & DUISTATE_HOT) ? RBS_UNCHECKEDHOT : RBS_UNCHECKEDNORMAL;
	theme_ptr->PaintTheme(CThemeMgr::BUTTON, hdc, BP_RADIOBUTTON, state, &rcBkgnd);
	return TRUE;
}

IMPLEMENT_SIMPLE_VISUAL_RECT(CheckBox, (20,0,0,0))( HDE hde, HDC hdc, RECT rcBkgnd )
{
	DuiNode* r = DuiHandle<DuiNode>(hde);
	// 首先调用插件或默认的背景绘制过程
	PluginCallContext(TRUE).Call(OpCode_PaintBackground, hde, hdc, rcBkgnd);
	//g_defVisual.OnPaintBackground(hde, hdc, rcBkgnd); // 让控件定义的背景样式有机会重绘

	rcBkgnd.right = rcBkgnd.left + 20;
	DWORD dwState = r->GetState();
	int state;
	if (dwState & DUISTATE_CHECKED)
		state = (dwState & DUISTATE_DISABLED) ? CBS_CHECKEDDISABLED :
			(dwState & DUISTATE_PUSHED) ? CBS_CHECKEDPRESSED :
			(dwState & DUISTATE_HOT) ? CBS_CHECKEDHOT : CBS_CHECKEDNORMAL;
	else
		state = (dwState & DUISTATE_DISABLED) ? CBS_UNCHECKEDDISABLED :
			(dwState & DUISTATE_PUSHED) ? CBS_UNCHECKEDPRESSED :
			(dwState & DUISTATE_HOT) ? CBS_UNCHECKEDHOT : CBS_UNCHECKEDNORMAL;
	theme_ptr->PaintTheme(CThemeMgr::BUTTON, hdc, BP_CHECKBOX, state, &rcBkgnd);
	return TRUE;
}

BOOL VisualGroupBox::OnPaintBackground( HDE hde, HDC hdc, RECT rcBkgnd )
{
	DuiNode* r = DuiHandle<DuiNode>(hde);
	// 首先调用插件或默认的背景绘制过程
	PluginCallContext(TRUE).Call(OpCode_PaintBackground, hde, hdc, rcBkgnd);
	//g_defVisual.OnPaintBackground(hde, hdc, rcBkgnd); // 让控件定义的背景样式有机会重绘

	CRect rc = rcBkgnd;
	LPCOLESTR szTitle = r->GetAttributeString(L"group-title");
	if (szTitle == NULL) szTitle = r->GetAttributeString(L"title");
	rc.DeflateRect(2, szTitle?10:2, 2, 2);
	theme_ptr->PaintTheme(CThemeMgr::BUTTON, hdc, BP_GROUPBOX, r->IsEnabled() ? GBS_NORMAL : GBS_DISABLED, &rc);

	// 现在绘制标题文本
	if (szTitle)
	{
		CDCHandle dc(hdc);
		int iOld = dc.SaveDC();
		// 标题文本仅仅使用普通GUI字体，但可以在属主控件属性中定义颜色和文本水平对齐，文本样式仅使用水平对齐的样式，然后加上单行和垂直对齐
		dc.SetBkMode(OPAQUE);
		HFONT font = (HFONT)::GetStockObject(DEFAULT_GUI_FONT);
		dc.SelectFont(font);

		COLORREF clr = CLR_INVALID;
		TryLoad_COLORREF_FromString(r->GetAttributeString(L"group-title-color"), clr);
		if (clr == CLR_INVALID) clr = RGB(0, 0, 159); // 默认浅蓝色
		dc.SetTextColor(clr&0x00ffffff);

		UINT style = 0;
		if (StyleParseTextStyle(NULL, NULL, r->GetAttributeString(L"group-title-align"), (LONG&)style))
			style &= (DT_LEFT | DT_CENTER | DT_RIGHT);
		style |= (DT_SINGLELINE | DT_VCENTER);

		CRect rcTitle = rcBkgnd;
		rcTitle.DeflateRect(20, 0);
		rcTitle.bottom = rcTitle.top + 20;
		BOOL bGdi32 = IsGdi32(r->m_pLayoutMgr);
		call_drawText(bGdi32, (hdc, szTitle, lstrlenW(szTitle), &rcTitle, style, 0,0));

		dc.RestoreDC(iOld);
	}
	return TRUE;
}

RECT VisualGroupBox::GetAdjustRect(HDE hde) const
{
	DuiNode* r = DuiHandle<DuiNode>(hde);
	return CRect(8, (r->GetAttributeString(L"group-title") || r->GetAttributeString(L"title")) ? 20 : 8, 8, 8);
}

//BOOL CDuiVisualHorzGradient::OnPaintBackground( HDC hdc, RECT rcBkgnd )
IMPLEMENT_SIMPLE_VISUAL(h_gradient)( HDE hde, HDC hdc, RECT rcBkgnd )
{
	DuiNode* r = DuiHandle<DuiNode>(hde);
	COLORREF clr[10];
	for (int i=0; i<10; i++) clr[i] = CLR_INVALID;
	CStrArray strs;
	int num = 0;
	if (SplitStringToArray(r->GetAttributeString(L"gradient-color"), strs, L" \t\r\n,"))
	{
		num = min(strs.GetSize(), 10);
		for (int i=0; i<num; i++)
		{
			TryLoad_COLORREF_FromString(strs[i], clr[i]);
			if (clr[i] == CLR_INVALID) clr[i] = RGB(255,255,255); // 默认白色
			clr[i] &= 0x00ffffff;
		}
	}
	if (num == 0) return FALSE;
	if (num == 1) return CDCHandle(hdc).FillSolidRect(&rcBkgnd, clr[0]), TRUE;

	int cx = (rcBkgnd.right - rcBkgnd.left) / (num - 1);
	TRIVERTEX triv[18];
	for (int i=0; i<num-1; i++)
	{
		triv[2*i].x = rcBkgnd.left + i * cx;
		triv[2*i].y = rcBkgnd.top;
		triv[2*i].Alpha = 0xFF00;
		triv[2*i].Red = GetRValue(clr[i]) << 8;
		triv[2*i].Green = GetGValue(clr[i]) << 8;
		triv[2*i].Blue = GetBValue(clr[i]) << 8;

		triv[2*i+1].x = (i==num-2) ? rcBkgnd.right : rcBkgnd.left + (i+1) * cx;
		triv[2*i+1].y = rcBkgnd.bottom;
		triv[2*i+1].Alpha = 0xFF00;
		triv[2*i+1].Red = GetRValue(clr[i+1]) << 8;
		triv[2*i+1].Green = GetGValue(clr[i+1]) << 8;
		triv[2*i+1].Blue = GetBValue(clr[i+1]) << 8;
	}
	GRADIENT_RECT grc[9];
	for (int i=0; i<num-1; i++)
	{
		grc[i].UpperLeft = 2*i;
		grc[i].LowerRight = 2*i + 1;
	}
	::GradientFill(hdc, triv, 2*(num-1), grc, num-1, GRADIENT_FILL_RECT_H);
	return TRUE;
}

//BOOL CDuiVisualVertGradient::OnPaintBackground( HDC hdc, RECT rcBkgnd )
IMPLEMENT_SIMPLE_VISUAL(v_gradient)( HDE hde, HDC hdc, RECT rcBkgnd )
{
	DuiNode* r = DuiHandle<DuiNode>(hde);
	COLORREF clr[10];
	for (int i=0; i<10; i++) clr[i] = CLR_INVALID;
	CStrArray strs;
	int num = 0;
	if (SplitStringToArray(r->GetAttributeString(L"gradient-color"), strs, L" \t\r\n,"))
	{
		num = min(strs.GetSize(), 10);
		for (int i=0; i<num; i++)
		{
			TryLoad_COLORREF_FromString(strs[i], clr[i]);
			if (clr[i] == CLR_INVALID) clr[i] = RGB(255,255,255); // 默认白色
			clr[i] &= 0x00ffffff;
		}
	}
	if (num == 0) return FALSE;
	if (num == 1) return CDCHandle(hdc).FillSolidRect(&rcBkgnd, clr[0]), TRUE;

	int cy = (rcBkgnd.bottom - rcBkgnd.top) / (num - 1);
	TRIVERTEX triv[18];
	for (int i=0; i<num-1; i++)
	{
		triv[2*i].x = rcBkgnd.left;
		triv[2*i].y = rcBkgnd.top + i * cy;
		triv[2*i].Alpha = 0xFF00;
		triv[2*i].Red = GetRValue(clr[i]) << 8;
		triv[2*i].Green = GetGValue(clr[i]) << 8;
		triv[2*i].Blue = GetBValue(clr[i]) << 8;

		triv[2*i+1].x = rcBkgnd.right;
		triv[2*i+1].y = (i==num-2) ? rcBkgnd.bottom : rcBkgnd.top + (i+1) * cy;
		triv[2*i+1].Alpha = 0xFF00;
		triv[2*i+1].Red = GetRValue(clr[i+1]) << 8;
		triv[2*i+1].Green = GetGValue(clr[i+1]) << 8;
		triv[2*i+1].Blue = GetBValue(clr[i+1]) << 8;
	}
	GRADIENT_RECT grc[9];
	for (int i=0; i<num-1; i++)
	{
		grc[i].UpperLeft = 2*i;
		grc[i].LowerRight = 2*i + 1;
	}
	::GradientFill(hdc, triv, 2*(num-1), grc, num-1, GRADIENT_FILL_RECT_V);
	return TRUE;
}

IMPLEMENT_SIMPLE_VISUAL_RECT(cmdlink, (30,0,0,0))(HDE hde, HDC hdc, RECT rcBkgnd)
{
	DuiNode* r = DuiHandle<DuiNode>(hde);
	// 首先调用插件或默认的背景绘制过程
	PluginCallContext(TRUE).Call(OpCode_PaintBackground, hde, hdc, rcBkgnd);
	//g_defVisual.OnPaintBackground(hde, hdc, rcBkgnd); // 让控件定义的背景样式有机会重绘

	rcBkgnd.right = rcBkgnd.left + 30;
	DWORD dwState = r->GetState();
	int state = (dwState & DUISTATE_DISABLED) ? CMDLGS_DISABLED :
		(dwState & DUISTATE_PUSHED) ? CMDLGS_PRESSED :
		(dwState & DUISTATE_HOT) ? CMDLGS_HOT : CMDLGS_NORMAL;
	theme_ptr->PaintTheme(CThemeMgr::BUTTON, hdc, BP_COMMANDLINKGLYPH, state, &rcBkgnd);
	return TRUE;
}
