#include "StdAfx.h"
#include <WinInet.h>
#include "DuiImage.h"


// PathCreateFromUrl CreateURLMonikerEx CoInternetParseUrl PathIsXXX[URL] UrlApplyScheme
/*
 *	参考：http://wine.sourcearchive.com/documentation/1.1.5/shdocvw_2navigate_8c-source.html
 static HRESULT create_moniker(LPCWSTR url, IMoniker **mon)
 00558 {
 00559     WCHAR new_url[INTERNET_MAX_URL_LENGTH];
 00560     DWORD size;
 00561     HRESULT hres;
 00562 
 00563     if(PathIsURLW(url))
 00564         return CreateURLMoniker(NULL, url, mon);
 00565 
 00566     if(url[1] == ':') {
 00567         size = sizeof(new_url);
 00568         hres = UrlCreateFromPathW(url, new_url, &size, 0);
 00569         if(FAILED(hres)) {
 00570             WARN("UrlCreateFromPathW failed: %08x\n", hres);
 00571             return hres;
 00572         }
 00573     }else {
 00574         size = sizeof(new_url);
 00575         hres = UrlApplySchemeW(url, new_url, &size, URL_APPLY_GUESSSCHEME);
 00576         TRACE("got %s\n", debugstr_w(new_url));
 00577         if(FAILED(hres)) {
 00578             WARN("UrlApplyScheme failed: %08x\n", hres);
 00579             return hres;
 00580         }
 00581     }
 00582 
 00583     return CreateURLMoniker(NULL, new_url, mon);
 00584 }
 */

using namespace Gdiplus;

// 如果显示加载redui.dll，则必须在加载之前先初始化GDI+，否则无法卸载redui.dll
// 可以把这个类及全局变量定义复制到EXE中解决GDI+的卸载死锁问题
class __initGdiPlus
{
	CImage _img;
public:
	__initGdiPlus() { _img.Load((LPCTSTR)NULL); }
} ___initGdiPlus;

//

void CDuiImage::OnLoadStream(LPSTREAM pStream)
{
	img = Gdiplus::Bitmap::FromStream(pStream);
	loaded = TRUE;
	if (img && img->GetLastStatus()!=Gdiplus::Ok)
		Clear();

	// 	struct scb : public CDuiConnectorCallback
	// 	{
	// 	public:
	// 		BOOL OnHit(LPVOID pKey, LPVOID pVal, DWORD flag) { return ((CDuiConnectorCallback*)pKey)->OnHit(pKey, pVal, flag); }
	// 	};
	// 	scb cb;
	gConnector.Fire(NULL, this, cp_img_loaded/*, &cb*/);
}

CDuiImage::~CDuiImage()
{
	gConnector.Disconnect(NULL, this);
	Clear();
}

BOOL CDuiImage::Init(UINT nWidth, UINT nHeight)
{
	if (img)
	{
		if (nWidth==img->GetWidth() && nHeight==img->GetHeight()) return TRUE;
		Clear();
	}

	img = new Gdiplus::Bitmap(nWidth, nHeight);
	if (img && img->GetLastStatus()==Gdiplus::Ok) return loaded=TRUE, TRUE;
	return Clear(), FALSE;
}

BOOL CDuiImage::Tile( HDC hdc, const RECT& rcDst, BYTE bSrcAlpha/*=0xff*/, BOOL bCopy/*=FALSE*/ )
{
	if (img == NULL) return FALSE;
	Gdiplus::Graphics g(hdc);
	g.SetCompositingMode(CompositingModeSourceOver/*(CompositingMode)bCopy*/);
	TextureBrush br(img, WrapModeTile);
	g.FillRectangle(&br, Rect(rcDst.left,rcDst.top,rcDst.right-rcDst.left,rcDst.bottom-rcDst.top));
	//ImageAttributes ia;
	//ia.SetWrapMode(WrapModeTile);
	//g.DrawImage(img, Rect(rcDst.left,rcDst.top,rcDst.right-rcDst.left,rcDst.bottom-rcDst.top), 0, 0, img->GetWidth(), img->GetHeight(), UnitPixel, &ia);
	return TRUE;

	//if (IsNull())
	//	return FALSE;

	//CRect rc = rcDst;
	//for (int y=rc.top; y<rc.bottom; y+=GetHeight())
	//{
	//	int h = min(GetHeight(), rc.bottom-y);
	//	for (int x=rc.left; x<rc.right; x+=GetWidth())
	//	{
	//		int w = min(GetWidth(), rc.right-x);
	//		CRect _rc(CPoint(x,y), CSize(w,h));
	//		Draw(hdc, _rc, CRect(0,0,_rc.Width(),_rc.Height())/*, bSrcAlpha*/); // 为避免被拉伸，必须使用相同大小的srcRect参数
	//	}
	//}
	//return TRUE;
}

//BOOL CDuiImage::Draw( HDC hDestDC, int xDest, int yDest, int nDestWidth, int nDestHeight, BYTE bSrcAlpha/*=0xff */ )
//{
//	return AlphaBlend(hDestDC, xDest, yDest, bSrcAlpha);
//}
//
//BOOL CDuiImage::Draw( HDC hDestDC, const RECT& rectDest, BYTE bSrcAlpha/*=0xff */ )
//{
//	return Draw(hDestDC, rectDest.left, rectDest.top, rectDest.right-rectDest.left, rectDest.bottom-rectDest.top, bSrcAlpha);
//}

//void CDuiImage::PreProcessTransparent()
//{
//	if (IsNull() || !IsAlpha())
//		return;
//
//	for(int i = 0; i < GetWidth(); ++i)
//	{
//		for(int j = 0; j < GetHeight(); ++j)
//		{
//			unsigned char* pucColor = (unsigned char*)GetPixelAddress(i, j);
//			//if (pucColor[3] == 0)
//			{
//				//pucColor[0] = pucColor[1] = pucColor[2] = 0;
//				pucColor[0] = pucColor[0] * pucColor[3] / 255;
//				pucColor[1] = pucColor[1] * pucColor[3] / 255;
//				pucColor[2] = pucColor[2] * pucColor[3] / 255;
//			}
//		}
//	}
//}

BOOL CDuiImage::Draw( HDC hdc, const RECT& rcDst, const RECT& rcSrc, BOOL bCopy/*=FALSE*/ )
{
	if (img == NULL) return FALSE;
	Gdiplus::Graphics g(hdc);
	g.SetCompositingMode(CompositingModeSourceOver/*(CompositingMode)bCopy*/);
	g.DrawImage(img, Rect(rcDst.left,rcDst.top,rcDst.right-rcDst.left,rcDst.bottom-rcDst.top),
		rcSrc.left,rcSrc.top,rcSrc.right-rcSrc.left,rcSrc.bottom-rcSrc.top, UnitPixel);
	return TRUE;
}

BOOL CDuiImage::Draw( HDC hdc, const RECT& rcDst, BOOL bSquares/*=FALSE*/, BOOL bCopy/*=FALSE*/ )
{
	if (img == NULL) return FALSE;
	Gdiplus::Graphics g(hdc);
	g.SetCompositingMode(CompositingModeSourceOver/*(CompositingMode)bCopy*/);
	if (bSquares && prcClip)
	{
		long w = (long)GetWidth();
		long h = (long)GetHeight();
		CRect rc = *prcClip;
		rc.left = min(w, max(0, rc.left));
		rc.right = min(w, max(0, rc.right));
		rc.top = min(h, max(0, rc.top));
		rc.bottom = min(h, max(0, rc.bottom));
		long cw = max(0, w - rc.left - rc.right);
		long ch = max(0, h - rc.top - rc.bottom);
		long mycw = max(0, (long)rcDst.right - rcDst.left - rc.left - rc.right);
		long mych = max(0, (long)rcDst.bottom - rcDst.top - rc.top - rc.bottom);

		if (rc.top > 0)
		{
			if (rc.left > 0)	g.DrawImage(img, Rect(rcDst.left,rcDst.top,rc.left,rc.top), 0,0,rc.left,rc.top, UnitPixel);
			if (cw>0 && mycw>0)	g.DrawImage(img, Rect(rcDst.left+rc.left,rcDst.top,mycw,rc.top), rc.left,0,cw,rc.top, UnitPixel);
			if (rc.right > 0)	g.DrawImage(img, Rect(rcDst.left+rc.left+mycw,rcDst.top,rc.right,rc.top), rc.left+cw,0,rc.right,rc.top, UnitPixel);
		}
		if (ch > 0 && mych > 0)
		{
			if (rc.left > 0)	g.DrawImage(img, Rect(rcDst.left,rcDst.top+rc.top,rc.left,mych), 0,rc.top,rc.left,ch, UnitPixel);
			if (cw>0 && mycw>0)	g.DrawImage(img, Rect(rcDst.left+rc.left,rcDst.top+rc.top,mycw,mych), rc.left,rc.top,cw,ch, UnitPixel);
			if (rc.right > 0)	g.DrawImage(img, Rect(rcDst.left+rc.left+mycw,rcDst.top+rc.top,rc.right,mych), rc.left+cw,rc.top,rc.right,ch, UnitPixel);
		}
		if (rc.bottom > 0)
		{
			if (rc.left > 0)	g.DrawImage(img, Rect(rcDst.left,rcDst.top+rc.top+mych,rc.left,rc.bottom), 0,rc.top+ch,rc.left,rc.bottom, UnitPixel);
			if (cw>0 && mycw>0)	g.DrawImage(img, Rect(rcDst.left+rc.left,rcDst.top+rc.top+mych,mycw,rc.bottom), rc.left,rc.top+ch,cw,rc.bottom, UnitPixel);
			if (rc.right > 0)	g.DrawImage(img, Rect(rcDst.left+rc.left+mycw,rcDst.top+rc.top+mych,rc.right,rc.bottom), rc.left+cw,rc.top+ch,rc.right,rc.bottom, UnitPixel);
		}
	}
	else if (prcClip)
	{
		long w = (long)GetWidth();
		long h = (long)GetHeight();
		CRect rc = *prcClip;
		rc.left = min(w, max(0, rc.left));
		rc.right = min(w, max(0, rc.right));
		rc.top = min(h, max(0, rc.top));
		rc.bottom = min(h, max(0, rc.bottom));
		long cw = max(0, w - rc.left - rc.right);
		long ch = max(0, h - rc.top - rc.bottom);
		if (cw>0 && ch>0)
			g.DrawImage(img, Rect(rcDst.left,rcDst.top,rcDst.right-rcDst.left,rcDst.bottom-rcDst.top),
				rc.left, rc.top, cw, ch, UnitPixel);
	}
	else
	{
		g.DrawImage(img, Rect(rcDst.left,rcDst.top,rcDst.right-rcDst.left,rcDst.bottom-rcDst.top),
			0, 0, img->GetWidth(), img->GetHeight(), UnitPixel);
	}
	return TRUE;
}

BOOL CDuiImage::DrawFrom( CDuiImage* pImg, BOOL bStretch/*=FALSE*/, BOOL bSquares/*=FALSE*/ )
{
	if (img==NULL || pImg==NULL) return FALSE;
	Gdiplus::Graphics g(img);
	g.SetCompositingMode(CompositingModeSourceOver);
	if (bSquares && pImg->prcClip) // 九宫格绘制
	{
		long w = (long)pImg->GetWidth();
		long h = (long)pImg->GetHeight();
		CRect rc = *pImg->prcClip;
		rc.left = min(w, max(0, rc.left));
		rc.right = min(w, max(0, rc.right));
		rc.top = min(h, max(0, rc.top));
		rc.bottom = min(h, max(0, rc.bottom));
		long cw = max(0, w - rc.left - rc.right);
		long ch = max(0, h - rc.top - rc.bottom);
		long mycw = max(0, (long)GetWidth() - rc.left - rc.right);
		long mych = max(0, (long)GetHeight() - rc.top - rc.bottom);

		if (rc.top > 0)
		{
			if (rc.left > 0)	g.DrawImage(pImg->img, Rect(0,0,rc.left,rc.top), 0,0,rc.left,rc.top, UnitPixel);
			if (cw>0 && mycw>0)	g.DrawImage(pImg->img, Rect(rc.left,0,mycw,rc.top), rc.left,0,cw,rc.top, UnitPixel);
			if (rc.right > 0)	g.DrawImage(pImg->img, Rect(rc.left+mycw,0,rc.right,rc.top), rc.left+cw,0,rc.right,rc.top, UnitPixel);
		}
		if (ch > 0 && mych > 0)
		{
			if (rc.left > 0)	g.DrawImage(pImg->img, Rect(0,rc.top,rc.left,mych), 0,rc.top,rc.left,ch, UnitPixel);
			if (cw>0 && mycw>0)	g.DrawImage(pImg->img, Rect(rc.left,rc.top,mycw,mych), rc.left,rc.top,cw,ch, UnitPixel);
			if (rc.right > 0)	g.DrawImage(pImg->img, Rect(rc.left+mycw,rc.top,rc.right,mych), rc.left+cw,rc.top,rc.right,ch, UnitPixel);
		}
		if (rc.bottom > 0)
		{
			if (rc.left > 0)	g.DrawImage(pImg->img, Rect(0,rc.top+mych,rc.left,rc.bottom), 0,rc.top+ch,rc.left,rc.bottom, UnitPixel);
			if (cw>0 && mycw>0)	g.DrawImage(pImg->img, Rect(rc.left,rc.top+mych,mycw,rc.bottom), rc.left,rc.top+ch,cw,rc.bottom, UnitPixel);
			if (rc.right > 0)	g.DrawImage(pImg->img, Rect(rc.left+mycw,rc.top+mych,rc.right,rc.bottom), rc.left+cw,rc.top+ch,rc.right,rc.bottom, UnitPixel);
		}
	}
	else
	{
		Gdiplus::Bitmap* src = bStretch?pImg->img:img;
		g.DrawImage(pImg->img, Rect(0,0,img->GetWidth(),img->GetHeight()), 0,0,src->GetWidth(),src->GetHeight(), UnitPixel);
	}
	return TRUE;
}

BOOL CDuiImage::TileFrom( CDuiImage* pImg )
{
	if (img==NULL || pImg==NULL) return FALSE;
	Gdiplus::Graphics g(img);
	g.SetCompositingMode(CompositingModeSourceOver);
	TextureBrush br(pImg->img, WrapModeTile);
	g.FillRectangle(&br, Rect(0,0,img->GetWidth(),img->GetHeight()));
	return TRUE;
}

BOOL CDuiImage::IsTransparent(POINT pt) const
{
	if (img==NULL || !IsAlphaPixelFormat(img->GetPixelFormat())) return FALSE;
	Color clr;
	if (img->GetPixel(pt.x, pt.y, &clr) != Gdiplus::Ok) return FALSE;
	return clr.GetAlpha() == 0;
}

BOOL CDuiImage::FillFrom( COLORREF clr, BYTE bAlpha/*=255*/ )
{
	if (img==NULL) return FALSE;
	Graphics g(img);
	g.SetCompositingMode(CompositingModeSourceCopy);
	g.SetPageUnit(UnitPixel);
	BYTE a = (BYTE) (clr>>24);
	if (a>0 && a<255) bAlpha = a; // 优先使用COLORREF中自带的A值
	Color c(Color::MakeARGB(min(bAlpha,254), GetRValue(clr), GetGValue(clr), GetBValue(clr)));
	SolidBrush br(c);
	g.FillRectangle(&br, 0, 0, img->GetWidth(), img->GetHeight());
	return TRUE;
}

void CDuiImage::SetClipRect( LPCRECT pClip/*=NULL*/ )
{
	if (pClip)
	{
		::CopyRect(&rcClip, pClip);
		prcClip = &rcClip;
	}
	else
		prcClip = NULL;
}

//////////////////////////////////////////////////////////////////////////
//

void CDuiImageLayer::SetDestRect( RECT rc )
{
	if (!::IsRectEmpty(&_rcDst))
	{
		_img.Clear();
	}

	_rcDst = rc;
	_img.Init(rc.right-rc.left, rc.bottom-rc.top);
}

void CDuiImageLayer::SetTopLeft( long l, long t )
{
	::OffsetRect(&_rcDst, l-_rcDst.left, t-_rcDst.top);
}

void CDuiImageLayer::DrawToDC( HDC hdc )
{
	if (!_img.IsNull())
		_img.Draw(hdc, _rcDst, !_bOver);
}
