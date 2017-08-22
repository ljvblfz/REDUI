#ifndef __DUIALPHA_H__
#define __DUIALPHA_H__

#pragma once

#include "GDIRect.h"
//#include <gdiplus.h>
using namespace Gdiplus;
//#pragma comment(lib, "gdiplus.lib")

#define call_drawText(bGdi32, params) {if (bGdi32) {DrawText32##params;} else {GdiDrawText##params;}}
#define call_getTextExtent(bGdi32, params) {if (bGdi32) {GetTextExtentPoint32P##params;} else {::GetTextExtentPoint32W##params;}}
#define call_getTextExtentExPoint(bGdi32, params) {if (bGdi32) {GetTextExtentExPointP##params;} else {::GetTextExtentExPointW##params;}}


#define FromRECT(rc) RectF((REAL)rc.left, (REAL)rc.top, (REAL)rc.right-rc.left, (REAL)rc.bottom-rc.top)
#define FromRECT4(rc) (INT)rc.left, (INT)rc.top, (INT)(rc.right-rc.left-1), (INT)(rc.bottom-rc.top-1)

#define FromCOLORREF(clr) Gdiplus::Color(GetRValue(clr), GetGValue(clr), GetBValue(clr))
#define FromCOLORREFwithAlpha(clr) Gdiplus::Color((BYTE)(clr>>24), GetRValue(clr), GetGValue(clr), GetBValue(clr))
#define FromCOLORREFandAlpha(clr, alpha) Gdiplus::Color(alpha, GetRValue(clr), GetGValue(clr), GetBValue(clr))

// 如果COLORREF自带有效的A，则忽略alpha
#define FromCOLORREFAutoAlpha(clr, alpha) Gdiplus::Color(((BYTE)(clr>>24)>0&&(BYTE)(clr>>24)<255)?(BYTE)(clr>>24):alpha, GetRValue(clr), GetGValue(clr), GetBValue(clr))


class EnterGraphicsContainer
{
	Graphics& _g;
	GraphicsContainer _gc;
public:
	EnterGraphicsContainer(Graphics& g) : _g(g) { _gc = _g.BeginContainer(); }
	~EnterGraphicsContainer() { _g.EndContainer(_gc); }
};

inline int   GetEncoderClsid(const   WCHAR*   format,   CLSID*   pClsid)  
{  
	UINT     num   =   0;                     //   number   of   image   encoders  
	UINT     size   =   0;                   //   size   of   the   image   encoder   array   in   bytes  

	ImageCodecInfo*   pImageCodecInfo   =   NULL;  

	GetImageEncodersSize(&num,   &size);  
	if(size   ==   0)  
		return   -1;     //   Failure  

	pImageCodecInfo   =   (ImageCodecInfo*)(malloc(size));  
	if(pImageCodecInfo   ==   NULL)  
		return   -1;     //   Failure  

	GetImageEncoders(num,   size,   pImageCodecInfo);  

	for(UINT   j   =   0;   j   <   num;   ++j)  
	{  
		if(   wcscmp(pImageCodecInfo[j].MimeType,   format)   ==   0   )  
		{  
			*pClsid   =   pImageCodecInfo[j].Clsid;  
			free(pImageCodecInfo);  
			return   j;     //   Success  
		}          
	}  

	free(pImageCodecInfo);  
	return   -1;     //   Failure  
}  

//inline void SaveDCtoFile(HDC hdc,)

inline void GdiPath2GraphicPath(HDC hdc, GraphicsPath& path)
{
	path.SetFillMode(FillModeWinding);
}

inline void CopyHDC32(HDC hdc, RECT& rc, HDC hdcSrc)
{
	Graphics g(hdc);
	EnterGraphicsContainer gc(g);

	HBITMAP hbmp = (HBITMAP)::GetCurrentObject(hdcSrc, OBJ_BITMAP);
	//HBITMAP hbmp = (HBITMAP)::SelectObject(hdcSrc, (HBITMAP)NULL);
	BITMAP bitmap = {0};
	int len1 = sizeof(BITMAP);
	int len2 = sizeof(DIBSECTION);
	int len = ::GetObjectW(hbmp, sizeof(BITMAP), &bitmap);
	DWORD dwError;
	if (len == 0) dwError = GetLastError();
	Bitmap bmp(bitmap.bmWidth, bitmap.bmHeight, bitmap.bmWidthBytes, PixelFormat24bppRGB, (BYTE*)bitmap.bmBits);
	//Bitmap bmp(hbmp, NULL);
	CLSID jpgClsid;
	GetEncoderClsid(L"image/jpeg", &jpgClsid);
	bmp.Save(L"d:\\test.jpg", &jpgClsid);
	CGDIRect grc(rc);
	g.DrawImage(&bmp, (RectF)grc);
	//::SelectObject(hdcSrc, hbmp);
}

inline void FillRect32(HDC hdc, RECT& rc, COLORREF clr, BYTE alpha=255)
{
	Graphics g(hdc);
	EnterGraphicsContainer gc(g);
	g.SetCompositingMode(CompositingModeSourceOver);
	g.SetPageUnit(UnitPixel);
	SolidBrush br(FromCOLORREFAutoAlpha(clr, min(alpha,254)));
	g.FillRectangle(&br, (RectF)CGDIRect(rc));//FromRECT4(rc));

	//SolidBrush br2(Color::MakeARGB(0xFE, 0xFF, 0x7F, 0x50));
	//g.FillRectangle(&br2, (INT)rc.left+2, (INT)rc.top+2, (INT)30, 1);
	//g.FillRectangle(&br2, (INT)rc.left+2, (INT)rc.top+6, (INT)30, 2);
	//g.FillRectangle(&br2, (INT)rc.left+2, (INT)rc.top+10, (INT)30, 3);
}

inline void DrawBorder32(HDC hdc, RECT& rc, LOGPEN& lpen, BYTE alpha=255)
{
	Graphics g(hdc);
	EnterGraphicsContainer gc(g);
	g.SetCompositingMode(CompositingModeSourceCopy);
	g.SetPageUnit(UnitPixel);
	Pen pen(FromCOLORREFAutoAlpha(lpen.lopnColor, min(alpha,254)), (REAL)lpen.lopnWidth.x);
	pen.SetDashStyle((DashStyle)(lpen.lopnStyle<5 ? lpen.lopnStyle : DashStyleCustom));
	//PenAlignment pa = pen.GetAlignment();
	//pen.SetAlignment(PenAlignmentInset);
	//PenType pt = pen.GetPenType();
	//LineJoin lj = pen.GetLineJoin();
	//pen.SetLineJoin(LineJoinRound);
	CGDIRect grc(rc);
	grc.right -= 1.0f;
	grc.bottom -= 1.0f;
	g.DrawRectangle(&pen, (RectF)grc);//FromRECT4(rc));

	//Point pts[4];// = {{rc.left,rc.top}, {rc.right,rc.top}, {rc.right,rc.bottom}, {rc.left,rc.bottom}};
	//pts[0].X = rc.left; pts[0].Y = rc.top;
	//pts[1].X = rc.right; pts[1].Y = rc.top;
	//pts[2].X = rc.right; pts[2].Y = rc.bottom;
	//pts[3].X = rc.left; pts[3].Y = rc.bottom;
	//g.DrawPolygon(&pen, pts, 4);

	//g.DrawRectangle(&pen, (INT)rc.left+2, (INT)rc.top+2, (INT)30, 1);
	//g.DrawRectangle(&pen, (INT)rc.left+2, (INT)rc.top+6, (INT)30, 2);
	//g.DrawRectangle(&pen, (INT)rc.left+2, (INT)rc.top+10, (INT)30, 3);
}

inline void DrawLine32(HDC hdc, LONG x1, LONG y1, LONG x2, LONG y2, COLORREF clr, BYTE alpha=255)
{
	Graphics g(hdc);
	EnterGraphicsContainer gc(g);
	g.SetCompositingMode(CompositingModeSourceOver);
	g.SetPageUnit(UnitPixel);
	Pen pen(FromCOLORREFAutoAlpha(clr, min(alpha,254)));
	g.DrawLine(&pen, x1, y1, x2, y2);
}

inline void GdiDrawText(HDC hdc, LPCOLESTR lpszText, int cchText, LPRECT lpRect, UINT uFormat, COLORREF clrGlow, long lThick /*BYTE alpha=255*/)
{
	//if (clrGlow==0 || lThick<1)
	//{
	//	::DrawTextW(hdc, lpszText, cchText, lpRect, uFormat);
	//}
	//else
	if (clrGlow!=CLR_INVALID && lThick>=1)
	{
		HDC _hdc = hdc;
		
		//Bitmap bmp(lpRect->right-lpRect->left, lpRect->bottom-lpRect->top, PixelFormat32bppARGB);
		//{
		//	Graphics gr(&bmp);
		//	_hdc = gr.GetHDC();

			::BeginPath(_hdc);
			::DrawTextW(_hdc, lpszText, cchText, lpRect, uFormat);
			::EndPath(_hdc);

			HPEN pen = ::CreatePen(PS_SOLID, lThick, clrGlow);
			HPEN oldpen = (HPEN)::SelectObject(_hdc, pen);
			::StrokePath(_hdc);
			::SelectObject(_hdc, oldpen);
			::DeleteObject(pen);

		//	gr.ReleaseHDC(_hdc);
		//}

		//Graphics g(hdc);
		//Blur blur;
		//BlurParams myBlurParams;
		//myBlurParams.expandEdge = TRUE;
		//myBlurParams.radius = 3;
		//blur.SetParameters(&myBlurParams);
		//RECT rc = {0,0,lpRect->right-lpRect->left, lpRect->bottom-lpRect->top};
		//bmp.ApplyEffect(&blur, &rc);

		//g.DrawImage(&bmp, lpRect->left, lpRect->top);
	}
	::DrawTextW(hdc, lpszText, cchText, lpRect, uFormat);
}

inline void DrawText32(HDC hdc, LPCOLESTR lpszText, int cchText, LPRECT lpRect, UINT uFormat, COLORREF clrGlow, long lThick /*BYTE alpha=255*/)
{
	Graphics g(hdc);
	//g.SetCompositingQuality(CompositingQualityHighQuality);
	EnterGraphicsContainer gc(g);
	//g.SetCompositingMode(CompositingModeSourceCopy); // ??? 为何不能用COPY模式?
	g.SetPageUnit(UnitPixel);

	CGDIRect rc(*lpRect);
	if ((uFormat & (DT_RIGHT|DT_CENTER)) == 0) // 左对齐要调整
	{
		rc.left -= 2.0f;	// 左边要偏移2像素才能跟GDI的位置一样
		rc.right += (REAL)(rc.Width() * 0.04); // 宽度至少要增大4%才能完整显示整行文本
	}

	Font font(hdc);
	SolidBrush br(FromCOLORREFandAlpha(::GetTextColor(hdc), 254/*min(alpha,254)*/)); // alpha=255 有问题？部分字体的ALPHA竟然变成0了
	StringFormat sf;
	INT sff = sf.GetFormatFlags();
	sf.SetFormatFlags(StringFormatFlagsNoClip | StringFormatFlagsNoWrap | StringFormatFlagsNoFitBlackBox/*| StringFormatFlagsLineLimit*/);
	sf.SetTrimming(StringTrimmingCharacter);

	if (uFormat & DT_RIGHT) sf.SetAlignment(StringAlignmentFar);
	else if (uFormat & DT_CENTER) sf.SetAlignment(StringAlignmentCenter);
	else sf.SetAlignment(StringAlignmentNear);

	if (uFormat & DT_BOTTOM) sf.SetLineAlignment(StringAlignmentFar);
	else if (uFormat & DT_VCENTER) sf.SetLineAlignment(StringAlignmentCenter);
	else sf.SetLineAlignment(StringAlignmentNear);

	//LineLog dbg(L"DrawText32(0x%08x): %s", ::GetTextColor(hdc), lpszText);

	if (clrGlow!=CLR_INVALID && lThick>=1)
	{
		GraphicsPath path(FillModeWinding);
		FontFamily ff;
		font.GetFamily(&ff);
		path.AddString(lpszText, cchText, &ff, font.GetStyle(), font.GetSize(), (RectF)rc, &sf);
		Pen pen(FromCOLORREFandAlpha(clrGlow, 255), (REAL)lThick);
		pen.SetLineJoin(LineJoinRound);
		g.DrawPath(&pen, &path);
		//g.FillPath(&br, &path);
		//g.DrawString(lpszText, cchText, &font, (RectF)rc, &sf, &br);
	}
	g.DrawString(lpszText, cchText, &font, (RectF)rc, &sf, &br);
}

inline BOOL GetTextExtentPoint32P(HDC hdc, LPCOLESTR lpszString, int nCount, LPSIZE lpSize)
{
	Graphics g(hdc);
	EnterGraphicsContainer gc(g);
	g.SetPageUnit(UnitPixel);
	Font font(hdc);

	if (nCount < 0) nCount = lstrlenW(lpszString);

	PointF pt(0.0f, 0.0f);
	RectF rc;
	if (Ok != g.MeasureString(lpszString, nCount, &font, pt, &rc))
		return FALSE;

	if (lpSize)
	{
		lpSize->cx = (LONG)rc.Width;
		lpSize->cy = (LONG)rc.Height;
	}
	return TRUE;
}

inline BOOL GetTextExtentExPointP(HDC hdc, LPCOLESTR lpszString, int cchString, int nMaxExtent, LPINT lpnFit/* = NULL*/, LPINT alpDx/* = NULL*/, LPSIZE lpSize)
{
	if (lpSize==NULL) return FALSE;
	if (cchString < 0) cchString = lstrlenW(lpszString);

	if (lpnFit) *lpnFit = cchString;
	lpSize->cx = lpSize->cy = 0;
	if (!GetTextExtentPoint32P(hdc, lpszString, cchString, lpSize))
		return FALSE;
	if (lpSize->cx <= nMaxExtent)
		return TRUE;

	// 现在按照比例因子计算字符个数
	int cch = cchString * nMaxExtent / lpSize->cx;
	if (lpnFit) *lpnFit = cch;
	if (cch == 0) return TRUE;

	// 先往上试，再往下试
	SIZE sz = {0};
	BOOL bDown = TRUE;
	while (cch<cchString && GetTextExtentPoint32P(hdc, lpszString, cch, &sz) && sz.cx<=nMaxExtent)
	{
		bDown = FALSE;
		if (lpnFit) *lpnFit = cch;
		*lpSize = sz;
		cch++;
	}
	if (!bDown) return TRUE;

	// 现在开始往下
	cch--;
	if (lpnFit) *lpnFit = cch;
	while (cch>0 && GetTextExtentPoint32P(hdc, lpszString, cch, lpSize) && lpSize->cx>nMaxExtent)
	{
		cch--;
		if (lpnFit) *lpnFit = cch;
	}

	return TRUE;
}

#endif // __DUIALPHA_H__