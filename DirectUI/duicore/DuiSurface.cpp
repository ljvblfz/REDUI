#include "stdafx.h"
#include "DuiSurface.h"
#include "../../common/apiloader.h"

//#include "DuiAlpha.h"

//#pragma comment(lib, "ddraw.lib")

//////////////////////////////////////////////////////////////////////////
// ddraw.dll
#include <ddraw.h>
//#pragma comment(linker, "/nodefaultlib[ddraw.lib]")

#define __foreach_ddrawapi(v) \
	v(HRESULT, DirectDrawCreate, (GUID FAR *lpGUID, LPDIRECTDRAW FAR *lplpDD, IUnknown FAR *pUnkOuter), (lpGUID,lplpDD,pUnkOuter)) \
	v(HRESULT, DirectDrawCreateEx, (GUID FAR * lpGuid, LPVOID  *lplpDD, REFIID  iid,IUnknown FAR *pUnkOuter), (lpGuid,lplpDD,iid,pUnkOuter))

ApiLoader ddapi(L"ddraw.dll");

#define do_api(ret, name, params_decl, params_var) \
	typedef ret (WINAPI * pfn##name) params_decl; \
	pfn##name fn##name = (pfn##name)ddapi(#name); \
	extern "C" ret WINAPI name params_decl \
{ \
	ATLASSERT(fn##name); \
	return fn##name params_var; \
} \

__foreach_ddrawapi(do_api)
#undef do_api


struct CDDSURFACEDESC : DDSURFACEDESC
{
	CDDSURFACEDESC(DWORD Caps, DWORD Flags = 0, int Width = 0, int Height = 0)
	{
		memset(this, 0,sizeof(CDDSURFACEDESC));
		this->dwSize = sizeof(DDSURFACEDESC);	
		this->dwFlags = (Flags & ~(DDSD_HEIGHT|DDSD_WIDTH)) | DDSD_CAPS;
		this->ddsCaps.dwCaps = Caps;
		if(Width > 0 && Height > 0)
		{
			this->dwHeight = Height; 
			this->dwWidth = Width; 
			this->dwFlags |= DDSD_HEIGHT|DDSD_WIDTH;
		}
	}
};

#define check_hr(hr) if (FAILED(hr)) return FALSE;

HDC CSurface::GetDC()
{
	if (_hdc == NULL)
	{
		if (_ddSurface.p)
			_ddSurface->GetDC(&_hdc);
	}
	if (_hdc) ++_ulRef;
	return _hdc;
}

void CSurface::ReleaseDC( HDC hdc )
{
	if (_hdc==NULL || _hdc!=hdc) return;
	--_ulRef;
	if (_ulRef==0 && _ddSurface.p)
		_hdc = (_ddSurface->ReleaseDC(_hdc), NULL);
}

void CSurface::FillRect( LPRECT pRect, COLORREF clr, BYTE alpha/*=255*/ )
{
	if (_ddSurface.p)
	{
		DDBLTFX bfx = {sizeof(DDBLTFX)};
		bfx.dwFillColor = ((clr & 0x00ffffff) | (alpha<<24));
		_ddSurface->Blt(pRect, NULL, NULL, DDBLT_COLORFILL, &bfx);
	}
}

CSurfaceDC::CSurfaceDC( CSurface* surface, BOOL bAutoBlt/*=FALSE*/ ) : _surface(surface), _bAutoBlt(bAutoBlt), m_hDC(NULL)
{
	Lock();
}

CSurfaceDC::~CSurfaceDC()
{
	Unlock();
	if (_surface && _bAutoBlt && _surface->IsOffscreen())
		_surface->BltToPrimary();
}

void CSurfaceDC::Lock()
{
	if (m_hDC == NULL)
	{
		if (_surface)
		{
			m_hDC = _surface->GetDC();
		}
		else
		{
			m_hDC = ::GetDC(NULL);
		}
		_savedc = ::SaveDC(m_hDC);
		::SelectObject(m_hDC, (HFONT)::GetStockObject(DEFAULT_GUI_FONT));
	}
}

void CSurfaceDC::Unlock()
{
	if (m_hDC)
	{
		::RestoreDC(m_hDC, _savedc);
		if (_surface)
			_surface->ReleaseDC(m_hDC);
		else
			::ReleaseDC(NULL, m_hDC);
		m_hDC = NULL;
	}
}

BOOL CPrimarySurface::Init( HWND hwnd )
{
	if (hwnd == NULL) hwnd = ::GetDesktopWindow();
	if (!::IsWindow(hwnd)) return FALSE;

	_hwnd = hwnd;

	_rc = Rect();

	// create DirectDrawObject
	HRESULT hr = ::DirectDrawCreate(NULL, &_dd.p, NULL); check_hr(hr);
	//HRESULT hr = CoCreateInstance(CLSID_DirectDraw, NULL, CLSCTX_INPROC, IID_IDirectDraw, (LPVOID*)&_dd.p); check_hr(hr);
	//hr = _dd->Initialize(NULL); check_hr(hr);
	hr = _dd->SetCooperativeLevel(hwnd, DDSCL_NORMAL); check_hr(hr);

	// create primary surface
	CDDSURFACEDESC dds(DDSCAPS_PRIMARYSURFACE);
	hr = _dd->CreateSurface(&dds, &_ddSurface.p, NULL); check_hr(hr);

	// create clipper
	hr = _dd->CreateClipper(0, &_ddClipper.p, NULL); check_hr(hr);
	// attach HWND to clipper
	hr = _ddClipper->SetHWnd(0, hwnd); check_hr(hr);
	// attach clipper to surface
	hr = _ddSurface->SetClipper(_ddClipper.p); check_hr(hr);

	return TRUE;
}

CSurface* CPrimarySurface::Clone(LPRECT pRect/* =NULL */)
{
	COffscreenSurface* pSurf = NEW COffscreenSurface(this, pRect);
	return pSurf;
}

RECT CPrimarySurface::Rect()
{
	RECT rc = {0};
	// 统一保存为屏幕尺寸
	//if (IsLayered(_hwnd)) // 对于分层窗口，必须使用窗口尺寸，而不是客户区尺寸
	//	::GetWindowRect(_hwnd, &rc);
	//else
	{
		::GetClientRect(_hwnd, &rc);
		::ClientToScreen(_hwnd, (LPPOINT)&rc);
		::ClientToScreen(_hwnd, ((LPPOINT)&rc)+1);
	}
	return rc;
}

COffscreenSurface::COffscreenSurface( CSurface* surface, LPCRECT lpRect/*=NULL*/ ) : CSurface(TRUE), _memdc(NULL), _hbmp(NULL), _hbmpOld(NULL)
{
	Init(surface, lpRect);
}

COffscreenSurface::~COffscreenSurface()
{
	if (_memdc)
	{
		if (_hbmpOld)
		{
			::SelectObject(_memdc, _hbmpOld);
			_hbmpOld = NULL;
		}
		::DeleteDC(_memdc);
		_memdc = NULL;
	}
	if (_hbmp)
	{
		::DeleteObject(_hbmp);
		_hbmp = NULL;
	}
}

BOOL COffscreenSurface::Init( CSurface* surface, LPCRECT lpRect/*=NULL*/ ) // lpRect应该是客户区坐标
{
	_owner = surface;
	if (_owner == NULL) return FALSE;

	while (surface->_owner) surface = surface->_owner;
	CPrimarySurface* primSurf = (CPrimarySurface*)surface;
	ATLASSERT(primSurf);

	if (_owner->_ddSurface.p==NULL || primSurf->_dd.p==NULL) return FALSE;

	if (lpRect)
	{
		::CopyRect(&_rc, lpRect);
		//::ClientToScreen(primSurf->_hwnd, (LPPOINT)&_rc);
		//::ClientToScreen(primSurf->_hwnd, ((LPPOINT)&_rc)+1);
	}
	else
	{
		//::CopyRect(&_rc, &_owner->_rc);
		CRect rc = _owner->Rect();
		_rc.left = _rc.top = 0;
		_rc.right = rc.Width(); //_owner->_rc.right - _owner->_rc.left;
		_rc.bottom = rc.Height(); //_owner->_rc.bottom - _owner->_rc.top;
	}

	//RECT rc;
	//::GetClientRect(primSurf->_hwnd, &rc);
	//if (IsLayered(primSurf->_hwnd)) // 对于分层窗口，必须使用窗口尺寸，而不是客户区尺寸
	//	::GetWindowRect(primSurf->_hwnd, &rc);
	int cx = _rc.right - _rc.left;
	int cy = _rc.bottom - _rc.top;

	CDDSURFACEDESC dds(DDSCAPS_OFFSCREENPLAIN /*| DDSCAPS_SYSTEMMEMORY*/, DDSD_PIXELFORMAT, cx, cy);
	dds.ddpfPixelFormat.dwSize = sizeof(DDPIXELFORMAT);
	dds.ddpfPixelFormat.dwFlags = DDPF_RGB | DDPF_ALPHAPIXELS;
	dds.ddpfPixelFormat.dwFourCC = 0;
	dds.ddpfPixelFormat.dwRGBBitCount = 32;
	dds.ddpfPixelFormat.dwRBitMask = 0x00ff0000;
	dds.ddpfPixelFormat.dwGBitMask = 0x0000ff00;
	dds.ddpfPixelFormat.dwBBitMask = 0x000000ff;
	dds.ddpfPixelFormat.dwRGBAlphaBitMask = 0xff000000;
	HRESULT hr = primSurf->_dd->CreateSurface(&dds, &_ddSurface.p, NULL); check_hr(hr);
	// 所有离屏表面都设置相同的关键色
	DDCOLORKEY clr = {SRC_BLT_KEYCOLOR, SRC_BLT_KEYCOLOR};
	hr = _ddSurface->SetColorKey(DDCKEY_SRCBLT, &clr);
	FillRect(NULL, SRC_BLT_KEYCOLOR);
	//DDPIXELFORMAT pf = {sizeof(DDPIXELFORMAT)};
	//_ddSurface->GetPixelFormat(&pf);

	//// 为了解决GDI操作显卡内存太慢的问题，这里专门创建一个系统内存DC用于GDI操作
	////HDC dc = ::GetDC(surface._hwnd);
	//HDC dc = CSurface::GetDC();
	//_memdc = ::CreateCompatibleDC(dc);
	//_hbmp = ::CreateCompatibleBitmap(dc, cx, cy);
	//if (_memdc && _hbmp) _hbmpOld = (HBITMAP)::SelectObject(_memdc, _hbmp);
	////::ReleaseDC(surface._hwnd, dc);
	//CSurface::ReleaseDC(dc);

	{
		CDDSURFACEDESC dds(DDSCAPS_OFFSCREENPLAIN | DDSCAPS_SYSTEMMEMORY, DDSD_PIXELFORMAT, cx, cy);
		dds.ddpfPixelFormat.dwSize = sizeof(DDPIXELFORMAT);
		dds.ddpfPixelFormat.dwFlags = DDPF_RGB | DDPF_ALPHAPIXELS;
		dds.ddpfPixelFormat.dwFourCC = 0;
		dds.ddpfPixelFormat.dwRGBBitCount = 32;
		dds.ddpfPixelFormat.dwRBitMask = 0x00ff0000;
		dds.ddpfPixelFormat.dwGBitMask = 0x0000ff00;
		dds.ddpfPixelFormat.dwBBitMask = 0x000000ff;
		dds.ddpfPixelFormat.dwRGBAlphaBitMask = 0xff000000;
		HRESULT hr = primSurf->_dd->CreateSurface(&dds, &_memdds.p, NULL); check_hr(hr);
		// 所有离屏表面都设置相同的关键色
		DDCOLORKEY clr = {SRC_BLT_KEYCOLOR, SRC_BLT_KEYCOLOR};
		hr = _memdds->SetColorKey(DDCKEY_SRCBLT, &clr);

		//HDC hdc = NULL;
		//if (SUCCEEDED(hr=_memdds->GetDC(&hdc)))
		//	_memdds->ReleaseDC(hdc);

		//DDBLTFX bfx = {sizeof(DDBLTFX)};
		//bfx.dwFillColor = SRC_BLT_KEYCOLOR;
		//_memdds->Blt(NULL, NULL, NULL, DDBLT_COLORFILL, &bfx);
	}

	//// 填充分层窗口非客户区
	//if (IsLayered(_primarySurface._hwnd))
	//	FillRect32(_memdc, lpRect?*(RECT*)lpRect:rc, RGB(0,0,0)/*, 254*/);

	return TRUE;
}

void COffscreenSurface::BltToPrimary(LPRECT pRectSrc/*=NULL*/)
{
	if (_ddSurface.p && _owner)
	{
		CSurface* surface = this;
		while (surface->_owner) surface = surface->_owner;
		CPrimarySurface* primSurf = (CPrimarySurface*)surface;
		ATLASSERT(primSurf);

		// 对于分层窗口，必须使用窗口尺寸，而不是客户区尺寸
		RECT rc = _owner->Rect();
		if (IsLayered(primSurf->_hwnd))
		{
			//::GetWindowRect(primSurf->_hwnd, &rc);
			POINT p = {rc.left, rc.top};
			POINT p2 = {0, 0};
			SIZE sz = {rc.right-rc.left, rc.bottom-rc.top};
			BLENDFUNCTION bf = {AC_SRC_OVER, 0, 255, AC_SRC_ALPHA};
			HDC hdc = GetDC();
			BOOL bRet = ::UpdateLayeredWindow(primSurf->_hwnd, NULL, &p, &sz, hdc, &p2, 0, &bf, /*::GetDeviceCaps(hdc,BITSPIXEL)==32 ?*/ ULW_ALPHA /*: ULW_OPAQUE*/);
			ReleaseDC(hdc);
			if (!bRet) ATLTRACE(L"UpdateLayeredWindow failed!\n");
		}
		else
		{
			// 如果 owner 是主表面（屏幕原点），则每次绘制必须重新计算屏幕位置，因为窗口可能随时移动
			POINT pt = {0};
			if (!_owner->IsOffscreen())
				::ClientToScreen(primSurf->_hwnd, &pt);

			//::GetClientRect(_primarySurface._hwnd, &rc);
			//::ClientToScreen(_primarySurface._hwnd, (LPPOINT)&rc);
			//::ClientToScreen(_primarySurface._hwnd, ((LPPOINT)&rc)+1);
			LPRECT prc = &_rc; //((!::IsRectEmpty(&_rc) ? &_rc : &rc));
			long x = pRectSrc ? pRectSrc->left : 0;
			long y = pRectSrc ? pRectSrc->top : 0;
			long cx = pRectSrc ? pRectSrc->right-pRectSrc->left : prc->right-prc->left;
			long cy = pRectSrc ? pRectSrc->bottom-pRectSrc->top : prc->bottom-prc->top;
			RECT rcSrc = {x, y, x+cx, y+cy};
			RECT rcDst = rcSrc;
			::OffsetRect(&rcDst, pt.x+prc->left, pt.y+prc->top);
			//if (_memdc)
			//{
			//	HDC hdc = CSurface::GetDC();
			//	::BitBlt(hdc, x, y, cx, cy, _memdc, x, y, SRCCOPY);
			//	CSurface::ReleaseDC(hdc);
			//}

			HRESULT hr;
			//if (_memdds.p)
			//{
			//	hr = _ddSurface->Blt(NULL, _memdds.p, NULL, DDBLT_WAIT, NULL);
			//}

			DDBLTFX bfx = {sizeof(DDBLTFX)};
			bfx.dwAlphaSrcConst = 0xff000000;
			//hr = (*_owner)->BltFast(pt.x+prc->left+x, pt.y+prc->top+y, _ddSurface.p, &rcSrc, DDBLTFAST_WAIT | DDBLTFAST_SRCCOLORKEY);
			//if (FAILED(hr))
				hr = (*_owner)->Blt(&rcDst, _ddSurface.p, &rcSrc, DDBLT_WAIT | DDBLT_KEYSRC, NULL);
				//hr = (*_owner)->Blt(&rcDst, _ddSurface.p, &rcSrc, DDBLT_WAIT | DDBLT_ALPHASRCCONSTOVERRIDE, &bfx);
			//ATLASSERT(SUCCEEDED(hr));
		}
	}
}

CSurface* COffscreenSurface::Clone(LPRECT pRect/* =NULL */)
{
	COffscreenSurface* pSurf = NEW COffscreenSurface(this, pRect);
	return pSurf;
}

HDC COffscreenSurface::GetDC()
{
	if (_memdds)
	{
		if (_hdc == NULL)
		{
			HRESULT hr;
			DDBLTFX bfx = {sizeof(DDBLTFX)};
			bfx.dwFillColor = SRC_BLT_KEYCOLOR;
			hr = _memdds->Blt(NULL, NULL, NULL, DDBLT_COLORFILL, &bfx);

			hr = _memdds->GetDC(&_hdc);
			if (_hdc==NULL)
				ATLASSERT(FALSE);
		}
		if (_hdc) ++_ulRef;
		return _hdc;
	}
	//if (_memdc) return _memdc;
	return CSurface::GetDC();
}

void COffscreenSurface::ReleaseDC( HDC hdc )
{
	if (_memdds)
	{
		if (_hdc==NULL || _hdc!=hdc) return;
		--_ulRef;
		if (_ulRef==0 && _memdds.p)
		{
			HRESULT hr = _memdds->ReleaseDC(_hdc);
			_hdc = NULL;
			
			//// update alpha--
			//DDSURFACEDESC sd = {sizeof(DDSURFACEDESC)};
			//hr = _memdds->Lock(NULL, &sd, DDLOCK_WAIT, NULL);
			//if (SUCCEEDED(hr))
			//{
			//	for (ULONG y=0; y<sd.dwHeight; y++)
			//	{
			//		DWORD* pdw = (DWORD*)(((char*)sd.lpSurface) + y * sd.lPitch);
			//		for (ULONG x=0; x<sd.dwWidth; x++)
			//		{
			//			if ((pdw[x]&0x00ffffff) != (SRC_BLT_KEYCOLOR&0x00ffffff)/* && (pdw[x]&0xff000000)==0*/)
			//			{
			//				pdw[x] -= 0x01000000;
			//			}
			//		}
			//	}
			//	hr = _memdds->Unlock(NULL);
			//}
			//// end update

			hr = _ddSurface->Blt(NULL, _memdds.p, NULL, DDBLT_WAIT, NULL);
		}
	}
	else
		CSurface::ReleaseDC(hdc);
	//if (hdc != _memdc) CSurface::ReleaseDC(hdc);
}

CGdiSurface::CGdiSurface( HWND hwnd ) : CSurface(FALSE), _hwnd(hwnd)
{
	_hdc = ::GetDC(hwnd);
	::GetClientRect(_hwnd, &_rc);
}

CGdiSurface::~CGdiSurface()
{
	if (_hdc)
	{
		::ReleaseDC(_hwnd, _hdc);
		_hdc = NULL;
	}
}

COffscreenGdiSurface::COffscreenGdiSurface( CGdiSurface& surface, LPCRECT lpRect/*=NULL*/ ) : CSurface(TRUE), _primarySurface(surface), _hbmp(NULL), _hbmpOld(NULL)
{
	::CopyRect(&_rc, lpRect ? lpRect : &_primarySurface._rc);
	if (_primarySurface._hdc)
	{
		_hbmp = ::CreateCompatibleBitmap(_primarySurface._hdc, _primarySurface._rc.right-_primarySurface._rc.left, _primarySurface._rc.bottom-_primarySurface._rc.top);
		_hdc = ::CreateCompatibleDC(_primarySurface._hdc);
		if (_hdc && _hbmp)
		{
			_hbmpOld = (HBITMAP)::SelectObject(_hdc, _hbmp);
		}
	}
}

COffscreenGdiSurface::~COffscreenGdiSurface()
{
	BltToPrimary();
	if (_hdc)
	{
		if (_hbmpOld)
		{
			::SelectObject(_hdc, _hbmpOld);
			_hbmpOld = NULL;
		}
		::DeleteDC(_hdc);
		_hdc = NULL;
	}
	if (_hbmp)
	{
		::DeleteObject(_hbmp);
		_hbmp = NULL;
	}
}

void COffscreenGdiSurface::BltToPrimary(LPRECT pRectSrc/*=NULL*/)
{
	if (_hdc && _primarySurface._hdc)
	{
		if (pRectSrc == NULL) pRectSrc = &_rc;
		::BitBlt(_primarySurface._hdc, pRectSrc->left, pRectSrc->top, pRectSrc->right-pRectSrc->left, pRectSrc->bottom-pRectSrc->top, _hdc, pRectSrc->left, pRectSrc->top, SRCCOPY);
	}
}
