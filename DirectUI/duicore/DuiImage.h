#pragma once

#include <atlimage.h>
#include "../../common/mimefilter.h"
#include "DuiBase.h"

class CDuiImage //: public CImage
{
private:
	//union
	//{
	//	CImage* img; // 用于GDI
	//};
	Gdiplus::Bitmap* img; // 用于GDI+
	BOOL loaded;

	RECT rcClip;
	//CDownload download;

	//void PreProcessTransparent();

public:
	RECT* prcClip;

	CDuiImage() : img(NULL), loaded(FALSE), prcClip(NULL)/*, download(OnDownloadComplete, this)*/ {}
	~CDuiImage();

	void Clear() { if (img) img = (delete img, NULL); loaded = FALSE; }

	BOOL IsNull() const { return !loaded; }
	UINT GetWidth() const { return loaded?img->GetWidth():0; }
	UINT GetHeight() const { return loaded?img->GetHeight():0; }
	BOOL IsTransparent(POINT pt) const;

	BOOL Init(UINT nWidth, UINT nHeight);

	void SetClipRect(LPCRECT pClip=NULL);

	BOOL Tile(HDC hdc, const RECT& rcDst, BYTE bSrcAlpha=0xff, BOOL bCopy=FALSE);
	//BOOL Draw( HDC hDestDC, int xDest, int yDest, int nDestWidth, int nDestHeight, BYTE bSrcAlpha=0xff );
	//BOOL Draw( HDC hDestDC, const RECT& rectDest, BYTE bSrcAlpha=0xff );
	BOOL Draw( HDC hdc, const RECT& rcDst, const RECT& rcSrc, BOOL bCopy=FALSE);
	BOOL Draw( HDC hdc, const RECT& rcDst, BOOL bSquares=FALSE, BOOL bCopy=FALSE);

	BOOL DrawFrom(CDuiImage* pImg, BOOL bStretch=FALSE, BOOL bSquares=FALSE);
	BOOL TileFrom(CDuiImage* pImg);
	BOOL FillFrom(COLORREF clr, BYTE bAlpha=255);

	void OnLoadStream(LPSTREAM pStream);

	//static const DWORD_PTR dwImgFlag = 0xFF000000;
};


//////////////////////////////////////////////////////////////////////////
//

class CDuiImageLayer
{
protected:
	CDuiImage _img;
	BOOL _bOver;
	RECT _rcDst;

public:
	CDuiImageLayer() : _bOver(FALSE) { ::SetRectEmpty(&_rcDst); }

	BOOL IsOver() { return _bOver; }
	void SetOver(BOOL bOver=TRUE) { _bOver = bOver; }

	void SetDestRect(RECT rc);
	void SetTopLeft(long l, long t);

	CDuiImage* operator->() { return &_img; }

	void DrawToDC(HDC hdc);
};