#ifndef __DUISURFACE_H__
#define __DUISURFACE_H__

#pragma once

//#include <atlapp.h>
//#include <atlgdi.h>
#include <ddraw.h>

#define DECLARE_DELETE() public: virtual void Delete() { delete this; }
//#define GDI_ONLY

#define SRC_BLT_KEYCOLOR	0xFF3D3E3F

#ifndef IsLayered
#define IsLayered(hwnd) ((::GetWindowLongW(hwnd, GWL_EXSTYLE) & WS_EX_LAYERED) == WS_EX_LAYERED)
#endif // IsLayered

class CPrimarySurface;
class COffscreenSurface;

class CSurface
{
	friend COffscreenSurface;

protected:
	CComPtr<IDirectDrawSurface> _ddSurface;
	CSurface* _owner;
	BOOL _isOffscrn;	// 是否离屏，针对GDI的HDC，或者表面的类型
	HDC _hdc;
	RECT _rc;
	ULONG _ulRef;

	CSurface(BOOL bOffscrn=FALSE) : _isOffscrn(bOffscrn), _hdc(NULL), _ulRef(0), _owner(NULL) { ::SetRectEmpty(&_rc); }
	//virtual ~CSurface()
	//{
	//	if (_ddSurface.p) _ddSurface.Release();
	//}

public:
	operator IDirectDrawSurface* () { return _ddSurface.p; }
	IDirectDrawSurface* operator->() { return _ddSurface.p; }

	CSurface* Owner() { return _owner; }

	virtual BOOL IsGDI() { return FALSE; }
	BOOL IsOffscreen() { return _isOffscrn; }

	virtual void Delete() = 0; // { delete this; } // = 0;
	virtual HDC GetDC();
	virtual void ReleaseDC(HDC hdc);
	virtual void BltToPrimary(LPRECT pRectSrc=NULL) {}		// 仅仅针对离屏表面有效
	void FillRect(LPRECT pRect, COLORREF clr, BYTE alpha=255);
	virtual CSurface* Clone(LPRECT pRect=NULL) = 0; // 为当前表面的全部或一个区域创建一个后备表面，离屏表面也可以有离屏表面
	virtual RECT Rect() { return _rc; } // 主表面读取窗口矩形，离屏表面直接返回 _rc;
};

class CSurfaceDC //: public CDCHandle
{
	CSurface* _surface;
	BOOL _bAutoBlt;
	int _savedc;

public:
	HDC m_hDC;

	CSurfaceDC(CSurface* surface=NULL, BOOL bAutoBlt=FALSE); // 如果第一个参数是NULL，则从屏幕获取DC
	~CSurfaceDC();

	operator HDC() { return m_hDC; }

	void Lock();
	void Unlock();
};

class CPrimarySurface : public CSurface
{
	friend COffscreenSurface;

	CComPtr<IDirectDraw> _dd; // ddraw
	CComPtr<IDirectDrawClipper> _ddClipper;
	HWND _hwnd;

	BOOL Init(HWND hwnd); // 初始化成主表面

	DECLARE_DELETE()

public:
	CPrimarySurface(HWND hwnd) { Init(hwnd); }
	~CPrimarySurface()
	{
		if (_ddSurface.p) _ddSurface.Release();
		if (_ddClipper.p) _ddClipper.Release();
		if (_dd.p) _dd.Release();
	};

	virtual CSurface* Clone(LPRECT pRect=NULL);
	virtual RECT Rect();
};

class COffscreenSurface : public CSurface
{
	friend CPrimarySurface;

	//RECT _rc;
	HDC _memdc;
	HBITMAP _hbmp;
	HBITMAP _hbmpOld;
	CComPtr<IDirectDrawSurface> _memdds;

	BOOL Init(CSurface* surface, LPCRECT lpRect=NULL); // 初始化成离屏表面

	DECLARE_DELETE()

	COffscreenSurface(CSurface* surface, LPCRECT lpRect=NULL); // 禁止直接new
public:
	~COffscreenSurface();

	virtual HDC GetDC();
	virtual void ReleaseDC(HDC hdc);
	virtual void BltToPrimary(LPRECT pRectSrc=NULL);

	virtual CSurface* Clone(LPRECT pRect=NULL);
};

class COffscreenGdiSurface;
class CGdiSurface : public CSurface
{
	friend COffscreenGdiSurface;

	HWND _hwnd;
	RECT _rc;

	DECLARE_DELETE()

public:
	CGdiSurface(HWND hwnd);
	~CGdiSurface();

	virtual BOOL IsGDI() { return TRUE; }

	virtual HDC GetDC() { return _hdc; }
	virtual void ReleaseDC(HDC hdc) {}
	virtual CSurface* Clone(LPRECT pRect=NULL) { return NULL; }
};

class COffscreenGdiSurface : public CSurface
{
	CGdiSurface& _primarySurface;
	RECT _rc;
	HBITMAP _hbmp;
	HBITMAP _hbmpOld;

	DECLARE_DELETE()

public:
	COffscreenGdiSurface(CGdiSurface& surface, LPCRECT lpRect=NULL);
	~COffscreenGdiSurface();

	virtual BOOL IsGDI() { return TRUE; }

	virtual HDC GetDC() { return _hdc; }
	virtual void ReleaseDC(HDC hdc) {}
	virtual void BltToPrimary(LPRECT pRectSrc=NULL);
	virtual CSurface* Clone(LPRECT pRect=NULL) { return NULL; }
};


#endif // __DUISURFACE_H__