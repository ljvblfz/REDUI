#include "stdafx.h"
#include <mmintrin.h>
#include <emmintrin.h>
#include <WindowsX.h>
#include <atlcoll.h>
#include <atlpath.h>
#include "DuiD3DApi.h"
#include "DuiD3DImpl.h"
#include "Dui3DObject.h"
#include "Dui3DAnimation.h"
#include "DuiBase.h"
#include "DuiAlpha.h"
#include "DuiStyleParser.h"
//#include "../../../util/apihook.h"
#include "mttimer.h"
#include "script/pulsedef.h"
//#include <gdiplus.h>
//#include <atlimage.h>
//using namespace Gdiplus;
//

#pragma warning(push)
//#pragma warning(disable:4049)

Device* DeviceFromHWND(HWND hWnd);

BOOL gbDisableComposition = FALSE;

//////////////////////////////////////////////////////////////////////////

#if 0
ApiHook_s("dwmapi.dll", IsCompositionEnabled, HRESULT, (BOOL* pfEnabled))
{
	LockInHook(IsCompositionEnabled);

	if (gbDisableComposition)
		return (*pfEnabled=FALSE), S_OK;
	return CallOrigFunction(pfEnabled);
}
#endif

#if 0

ApiHook("user32.dll", GetDC, HDC, (HWND hWnd))
{
	LockInHook(GetDC);

	iDevice* dev = (iDevice*)DeviceFromHWND(hWnd);
	if (dev)
		return dev->GetDC();

	HDC hdc = CallOrigFunction(hWnd);
	return hdc;
}

ApiHook("user32.dll", ReleaseDC, int, (HWND hWnd, HDC hDC))
{
	LockInHook(ReleaseDC);

	iDevice* dev = (iDevice*)DeviceFromHWND(hWnd);
	if (dev)
		return dev->ReleaseDC(hDC), 1;

	int n = CallOrigFunction(hWnd, hDC);
	return n;
}
#endif

#if 0/*1*/

ApiHook_s("user32.dll", GetDC, HDC, (HWND hWnd))
{
	LockInHook(GetDC);

	HDC hdc = (HDC)::GetPropW(hWnd, AtomString(3333));
	if (hdc)
		return hdc;
	return CallOrigFunction(hWnd);
}
//ApiHook("user32.dll", WindowFromDC, HWND, (HDC hdc))
//{
//	LockInHook(WindowFromDC);
//
//	iDevice* dev = gt.devices;
//	while (dev)
//	{
//		iControl* ctrl = dev->m_controls;
//		while (ctrl)
//		{
//			if (ctrl->m_hdcSurf == hdc)
//				return ctrl->m_hwnd;
//			ctrl = ctrl->next;
//		}
//		dev = dev->next;
//	}
//
//	return CallOrigFunction(hdc);
//}
//
//ApiHook("user32.dll", GetWindowDC, HDC, (HWND hWnd))
//{
//	LockInHook(GetWindowDC);
//
//	iControl* c = (iControl*)::GetPropW(hWnd, AtomString(atom_child_window));
//	if (c)
//	{
//		return c->getDC();
//	}
//
//	return CallOrigFunction(hWnd);
//}

ApiHook_s("user32.dll", ReleaseDC, int, (HWND hWnd, HDC hDC))
{
	LockInHook(ReleaseDC);

	HDC hdc = (HDC)::GetPropW(hWnd, AtomString(3333));
	return hdc ? 1 : CallOrigFunction(hWnd, hDC);
}
#endif

#if 0/*1*/
ApiHook_s("user32.dll", BeginPaint, HDC, (HWND hWnd, LPPAINTSTRUCT lpPaint))
{
	LockInHook(BeginPaint);

	//iControl* c = (iControl*)::GetPropW(hWnd, AtomString(atom_child_window));
	//if (c)
	//{
	//	ZeroMemory(lpPaint, sizeof(PAINTSTRUCT));
	//	lpPaint->hdc = c->getDC();
	//	CRect rcWin;
	//	::GetWindowRect(hWnd, &rcWin);
	//	CPoint pt(0,0);
	//	::ClientToScreen(hWnd, &pt);
	//	pt -= rcWin.TopLeft();
	//	::SetWindowOrgEx(lpPaint->hdc, pt.x, pt.y, NULL);
	//	::GetClientRect(hWnd, &lpPaint->rcPaint);
	//	return lpPaint->hdc;
	//}
	//} 
	HDC hdc = (HDC)::GetPropW(hWnd, AtomString(3333));
	if (hdc)
	{
		ZeroMemory(lpPaint, sizeof(PAINTSTRUCT));
		lpPaint->hdc = hdc;
		CRect rcWin;
		::GetWindowRect(hWnd, &rcWin);
		CPoint pt(0,0);
		::ClientToScreen(hWnd, &pt);
		pt -= rcWin.TopLeft();
		::SetWindowOrgEx(lpPaint->hdc, pt.x, pt.y, NULL);
		::GetClientRect(hWnd, &lpPaint->rcPaint);
		return lpPaint->hdc;
	}
	HDC hdcRet = CallOrigFunction(hWnd, lpPaint);
//	iControl* c = (iControl*)::GetPropW(hWnd, AtomString(atom_child_window));
//	if (c)
//	{
//		WCHAR buf[256] = L"";
//		::GetClassNameW(hWnd, buf, 255);
//		LineLog(L"%s - BeginPaint", (LPCWSTR)buf);
//#if 1
//		::SetPropW(hWnd, AtomString(3333), hdcRet);
//#else
//		c->m_hdcOrig = hdcRet;
//		hdcRet = c->m_hdcSurf = c->getDC();
//#endif // 1
	//}
	return hdcRet;
}

ApiHook_s("user32.dll", EndPaint, BOOL, (HWND hWnd, CONST PAINTSTRUCT *lpPaint))
{
	LockInHook(EndPaint);

//	iControl* c = (iControl*)::GetPropW(hWnd, AtomString(atom_child_window));
//	if (c)
//	{
//		::SetWindowOrgEx(c->m_hdcSurf, 0, 0, NULL);
//#if 0
//		HBITMAP hbmp = (HBITMAP)::GetCurrentObject(c->m_hdcSurf, OBJ_BITMAP);
//		Bitmap* bmp = Bitmap::FromHBITMAP(hbmp, NULL);
//		if (bmp)
//		{
//			CLSID pngClsid;
//			GetEncoderClsid(L"image/png", &pngClsid);
//			bmp->Save(L"d:\\output.png", &pngClsid, NULL);
//			delete bmp;
//		}
//#endif
//		c->releaseDC(c->m_hdcSurf);
//		return TRUE;
//	}
	HDC hdc = (HDC)::GetPropW(hWnd, AtomString(3333));
	if (hdc)
	{
		WCHAR buf[256] = L"";
		::GetClassNameW(hWnd, buf, 255);
		LineLog(L"%s - EndPaint", (LPCWSTR)buf);
		iControl* c = (iControl*)::GetPropW(hWnd, AtomString(atom_child_window));
		if (c)
		{
			c->invalidate();	 
		}
		::ValidateRect(hWnd, NULL);
		return TRUE;
	}

	BOOL bRet = CallOrigFunction(hWnd, lpPaint);

//	iControl* c = (iControl*)::GetPropW(hWnd, AtomString(atom_child_window));
//	if (c)
//	{
//		WCHAR buf[256] = L"";
//		::GetClassNameW(hWnd, buf, 255);
//		LineLog(L"%s - EndPaint", (LPCWSTR)buf);
//#if 1
//		//HDC hdc = (HDC)::RemovePropW(hWnd, AtomString(3333));
//		//c->update(hdc);
//		c->invalidate();	 
//#else
//		//::BitBlt(c->m_hdcOrig, 0, 0, c->m_sz.cx, c->m_sz.cy, c->m_hdcSurf, 0, 0, SRCCOPY);
//		c->releaseDC(c->m_hdcSurf);
//		c->m_hdcOrig = c->m_hdcSurf = NULL;
//#endif // 1
//	}
	return bRet;
}
#endif

#if 0
ApiHook("user32.dll", InvalidateRect, BOOL, (HWND hWnd, CONST RECT* lpRect, BOOL bErase))
{
	LockInHook(InvalidateRect);

	BOOL bRet = ::InvalidateRect(hWnd, lpRect, bErase);
	iControl* c = (iControl*)::GetPropW(hWnd, AtomString(atom_child_window));
	if (c)
		c->invalidate();
	return bRet;
}
#endif

//#pragma comment(lib, "d3dx9.lib")
//#pragma comment(lib, "ddraw.lib")
#define CUSTOM_DEFINE_GUID(name, l, w1, w2, b1, b2, b3, b4, b5, b6, b7, b8) \
	EXTERN_C const GUID DECLSPEC_SELECTANY name \
	= { l, w1, w2, { b1, b2,  b3,  b4,  b5,  b6,  b7,  b8 } }

//CUSTOM_DEFINE_GUID( IID_IDirectDraw4,                  0x9c59509a,0x39bd,0x11d1,0x8c,0x4a,0x00,0xc0,0x4f,0xd9,0x30,0xc5 );
CUSTOM_DEFINE_GUID( IID_IDirectDraw7,                  0x15e65ec0,0x3b9c,0x11d2,0xb9,0x2f,0x00,0x60,0x97,0x97,0xea,0x5b );
CUSTOM_DEFINE_GUID( IID_IDirectDrawSurface7,           0x06675a80,0x3b9b,0x11d2,0xb9,0x2f,0x00,0x60,0x97,0x97,0xea,0x5b );

Thread __gTlsData gt = {E_FAIL, NULL, NULL, NULL, NULL};
id3d g_d3d;

class iDDSD : public DDSURFACEDESC2
{
public:
	iDDSD()
	{
		ZeroMemory((LPDDSURFACEDESC2)this, sizeof(DDSURFACEDESC2));
		dwSize = sizeof(DDSURFACEDESC2);
		dwFlags = DDSD_HEIGHT|DDSD_WIDTH|DDSD_CAPS|DDSD_PIXELFORMAT;
		dwWidth = 1;
		dwHeight = 1;
		//dwAlphaBitDepth = 8;
		ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_SYSTEMMEMORY;
		ddpfPixelFormat.dwSize = sizeof(DDPIXELFORMAT);
		ddpfPixelFormat.dwFlags = DDPF_RGB | DDPF_ALPHAPIXELS;
		ddpfPixelFormat.dwFourCC = 0;
		ddpfPixelFormat.dwRGBBitCount = 32;
		ddpfPixelFormat.dwRBitMask = 0x00ff0000;
		ddpfPixelFormat.dwGBitMask = 0x0000ff00;
		ddpfPixelFormat.dwBBitMask = 0x000000ff;
		ddpfPixelFormat.dwRGBAlphaBitMask = 0xff000000;
	}

	void SetExternalMemory(DWORD width, DWORD height, long pitch, LPVOID buf)
	{
		dwFlags = DDSD_HEIGHT|DDSD_WIDTH
				//| DDSD_CAPS
				| DDSD_PIXELFORMAT
				| DDSD_LPSURFACE | DDSD_PITCH
				//| DDSD_ALPHABITDEPTH	// 不能使用这个标志
				;
		dwWidth = width;
		dwHeight = height;
		ddsCaps.dwCaps = 0 
				//| DDSCAPS_OFFSCREENPLAIN
				//| DDSCAPS_VIDEOMEMORY
				| DDSCAPS_SYSTEMMEMORY
				;
		lPitch = pitch;
		lpSurface = buf;
	}
};
// 用于创建一个临时表面
const iDDSD g_ddsdDummy;

//////////////////////////////////////////////////////////////////////////
iGdiTexture::iGdiTexture( iDevice* device, ULONG width, ULONG height) : m_device(device), m_width(0), m_height(0), m_pitch(0), m_buffer(NULL)
	, _hdc(NULL), _lockCount(0), m_dirty(TRUE)
	, m_alpha(TRUE), m_wnd(NULL), m_oldProc(::DefWindowProc), m_changeWinClassName(NULL), m_oleObject(NULL), m_renderType(0)
{
	resize(width, height);
}

iGdiTexture::~iGdiTexture()
{
	bindWindow(NULL);
	m_surface = NULL;
	m_texture = NULL;
	if (m_buffer) m_buffer = (delete[] m_buffer, NULL);
}

//bool iGdiTexture::isValid()
//{
//	return m_texture.p != NULL;
//}

bool iGdiTexture::resize( ULONG width, ULONG height )
{
	if (width==0 || height==0) return false;

	m_texture = NULL;
	m_surface = NULL;

	m_width = width;
	m_height = height;
	m_pitch = (LONG)m_width * 4;
	m_dirty = TRUE;
	return true;

	// 保证每行的DWORD数是4的倍数，方便SSE2处理
	//ULONG numperline = ((m_width + 3) & ~3); // [1,4]=4, [5-8]=8, [9-12]=12, ...
	//m_pitch = (LONG)numperline * 4;
	//m_buffer = NEW DWORD[numperline * m_height];

	//m_pitch = (LONG)m_width * 4;
	//ULONG total = (m_width * m_height + 3) & ~3; // 保证 m_width * m_height 是 4 的倍数
	//m_buffer = NEW DWORD[total];

	//// 创建表面后定位至新内存
	//HRESULT hr = m_dd->CreateSurface((LPDDSURFACEDESC2)&g_ddsdDummy, &m_surface, NULL);
	//if (FAILED(hr)) return false;

	//iDDSD ddsd;
	//ddsd.SetExternalMemory(m_width, m_height, m_pitch, m_buffer);
	//hr = m_surface->SetSurfaceDesc(&ddsd, 0);
	//if (FAILED(hr)) m_surface.Release();

	//return SUCCEEDED(hr);
}

void iGdiTexture::_update_alpha(LPRECT pRect/*=NULL*/)
{
	if (m_buffer==NULL) return;

	DWORD dwBegin = ::timeGetTime();
	// 要区分用哪种技术更新ALPHA值
	// ======== 暂时只处理整个缓冲区
	//if (pRect == NULL)

	if (g_d3d.isSSE || g_d3d.isSSE2)
	{
		// 128bits 4DWORDs
		__m128i msub = _mm_set1_epi32((int)0x01000000);
		for (ULONG i=0, num=m_width*m_height; i<num; i+=4)
		{
			__m128i m = _mm_loadu_si128((__m128i*)&m_buffer[i]);
			m = _mm_sub_epi32(m, msub); // m -= msub, DWORD = DWORD - 0x01000000
			_mm_storeu_si128((__m128i*)&m_buffer[i], m); // *m_buffer = m
		}
	}
	else
	if (g_d3d.isMMX)
	{
		// 64bits 2DWORD
		__m64 msub = _m_from_int((int)0x01000000);
		for (ULONG i=0, num=m_width*m_height; i<num; i+=2)
		{
			__m64 m;
			m.m64_u64 = *(ULONGLONG*)&m_buffer[i]; // 赋值 2 个DWORD
			m = _mm_sub_pi32(m, msub); // m -= msub, DWORD = DWORD - 0x01000000
			*(ULONGLONG*)&m_buffer[i] = m.m64_u64; // *m_buffer = m
		}

		_m_empty(); // clear MMX states
	}
	else // no MMX
	{
		for (ULONG i=0, num=m_width*m_height; i<num; i++)
		{
			m_buffer[i] -= 0x01000000;
		}
	}
	DWORD dwTick = ::timeGetTime();
	if (dwTick-dwBegin > 16)
		ATLTRACE(L"MMX - TICKCOUNT: %d\n", dwTick-dwBegin);
}

HDC iGdiTexture::getDC()
{
	if (_hdc) return _lockCount++, _hdc;

	HRESULT hr = S_OK;
	if (m_surface == NULL)
	{
		// 创建表面后定位至新内存
		CheckHResult hr = m_device->m_dd->CreateSurface((LPDDSURFACEDESC2)&g_ddsdDummy, &m_surface, NULL);
		if (FAILED(hr)) return NULL;

		if (m_buffer==NULL)
		{
			ULONG total = (m_width * m_height + 3) & ~3; // 保证 m_width * m_height 是 4 的倍数
			m_buffer = NEW DWORD[total];
		}

		iDDSD ddsd;
		ddsd.SetExternalMemory(m_width, m_height, m_pitch, m_buffer);
		CheckHResult hr = m_surface->SetSurfaceDesc(&ddsd, 0);
		if (FAILED(hr)) return m_surface.Release(), NULL;
	}

	// 在GETDC锁定之前，先填充背景色作为以后的透明色
#define update_alpha

	DDBLTFX bfx = {sizeof(DDBLTFX)};
#ifdef update_alpha
	bfx.dwFillColor = KEYCOLOR_ARGB(1);
#else
	bfx.dwFillColor = D3DCOLOR_COLORVALUE(0,0,0,.5f);
#endif // update_alpha
	CheckHResult hr = m_surface->Blt(NULL, NULL, NULL, DDBLT_COLORFILL, &bfx);
	CheckHResult hr = m_surface->GetDC(&_hdc);
	if (SUCCEEDED(hr))
	{
		_lockCount = 1;

		// 需要初始化HDC吗？
		CDCHandle dc(_hdc);

		// * 选入默认GUI字体
		::SelectObject(_hdc, ::GetStockObject(DEFAULT_GUI_FONT));
		
		// * 设置透明模式
		::SetBkMode(_hdc, TRANSPARENT);
	}
	return _hdc;
}

void iGdiTexture::releaseDC( HDC hdc )
{
	if (_lockCount==0 || m_surface.p==NULL || _hdc!=hdc || _hdc==NULL || m_buffer==NULL) return;

	if (--_lockCount == 0)
	{
		m_surface->ReleaseDC(hdc);
		_hdc = NULL;
		m_surface = NULL;

#ifdef update_alpha
		// 检测并修复像素值
		_update_alpha();
#define src_format D3DFMT_A8R8G8B8
#else
#define src_format D3DFMT_X8R8G8B8
#endif // update_alpha

		m_dirty = FALSE;

		HRESULT hr = S_OK;

		// 现在要把表面数据传送到纹理中
		if (m_texture == NULL)
		{
			CheckHResult hr = m_device->m_device->CreateTexture(m_width, m_height, 0, 0/*D3DUSAGE_DYNAMIC*/ | D3DUSAGE_AUTOGENMIPMAP, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, &m_texture.p, NULL);
			if (FAILED(hr)) return;
		}

		CComPtr<IDirect3DSurface9> dst;
		CheckHResult hr = m_texture->GetSurfaceLevel(0, &dst);
		if (SUCCEEDED(hr))
		{
			//CheckHResult hr = m_device->m_device->ColorFill(dst, NULL, 0);

			RECT rc = {0,0,(LONG)m_width, (LONG)m_height};
			CheckHResult hr = D3DXLoadSurfaceFromMemory(dst, NULL, NULL, m_buffer, src_format, (UINT)m_pitch, NULL, &rc, D3DX_FILTER_BOX/*D3DX_FILTER_NONE*/ | D3DX_FILTER_DITHER, KEYCOLOR_ARGB(0));
		}

		m_buffer = (delete[] m_buffer, NULL);
	}
}

HDC iGdiTexture::getTexDC()
{
	if (_hdc) return _lockCount++, _hdc;

	HRESULT hr = S_OK;
	if (m_texture == NULL)
	{
		CheckHResult hr = m_device->m_device->CreateTexture(m_width, m_height, 0, 0/*D3DUSAGE_DYNAMIC*/ | D3DUSAGE_AUTOGENMIPMAP, D3DFMT_X8R8G8B8, D3DPOOL_MANAGED, &m_texture.p, NULL);
		if (FAILED(hr)) return NULL;
	}

	CComPtr<IDirect3DSurface9> dst;
	CheckHResult hr = m_texture->GetSurfaceLevel(0, &dst);
	CheckHResult hr = dst->GetDC(&_hdc);
	if (SUCCEEDED(hr))
	{
		_lockCount = 1;

		// 需要初始化HDC吗？
		CDCHandle dc(_hdc);

		// * 选入默认GUI字体
		::SelectObject(_hdc, ::GetStockObject(DEFAULT_GUI_FONT));

		// * 设置透明模式
		::SetBkMode(_hdc, TRANSPARENT);
	}
	return _hdc;
}

void iGdiTexture::releaseTexDC( HDC hdc )
{
	if (_lockCount==0 || _hdc!=hdc || _hdc==NULL) return;

	HRESULT hr = S_OK;
	CComPtr<IDirect3DSurface9> dst;
	CheckHResult hr = m_texture->GetSurfaceLevel(0, &dst);
	if (--_lockCount == 0)
	{
		dst->ReleaseDC(hdc);
		_hdc = NULL;

		m_dirty = FALSE;
	}
}

BOOL iGdiTexture::Msgs::HasMessage( ULONG msg )
{
	if (msg==WM_PAINT) return TRUE;
	return Find(msg) >= 0;
}

void iGdiTexture::Msgs::ParseMessagesFromString( LPCOLESTR msgs )
{
	if (msgs == NULL)
		return;

	RemoveAll();

	CStrArray strs;
	if (::SplitStringToArray(msgs, strs, L" ,[]\t\r\n"))
	{
		for (int i=0; i<strs.GetSize(); i++)
		{
			long l;
			if (TryLoad_long_FromString(strs[i], l))
			{
				Add((ULONG)l);
				continue;
			}

#define msg2string(msg) L#msg
#define msgitem(msg) { msg2string(WM_##msg), L#msg, WM_##msg },
			static const struct {LPCOLESTR name; LPCOLESTR name2; UINT msg;} __mapitem_entry[] = {
				msgitem(TIMER)		msgitem(PAINT)
			};
#undef msgitem

			for (int j=0; j<sizeof(__mapitem_entry)/sizeof(__mapitem_entry[0]); j++)
			{
				if (lstrcmpiW(strs[i],__mapitem_entry[j].name)==0 || lstrcmpiW(strs[i],__mapitem_entry[j].name2)==0)
				{
					Add(__mapitem_entry[j].msg);
					break;
				}
			}
		}
	}
}

LPCWSTR g_szGdiTexture = L"GdiTexture";

void iGdiTexture::bindWindow( HWND hwnd, BOOL transparent/*=FALSE*/, BOOL bUpdateOleObject/*=TRUE*/ )
{
	if (m_wnd==hwnd && m_alpha==transparent)
		return;

	m_texture = NULL;
	m_surface = NULL;

	m_alpha = transparent;

	if (m_wnd)
	{
		// unsubclass
		SubclassWindow(m_wnd, m_oldProc);
		m_oldProc = ::DefWindowProc;
		//::SetPropW(hWnd, g_szGdiTexture, (HANDLE) NULL);
		::RemoveProp(m_wnd, g_szGdiTexture);
		m_wnd = NULL;
		if (bUpdateOleObject)
			m_oleObject = NULL;
	}
	m_wnd = hwnd;

	if (m_wnd)
	{
		// subclass
		m_oldProc = SubclassWindow(m_wnd, __WndProc);
		::SetPropW(m_wnd, g_szGdiTexture, this);
		if (bUpdateOleObject)
			m_oleObject = (IOleObject*)::GetPropW(m_wnd, L"Ole");
	}
}

BOOL iGdiTexture::__IsPaintMsg(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& lResult)
{
	if (!m_device->IsLayeredWindow() || !m_msgHooks.HasMessage(uMsg))
		return FALSE;

	HDC hdc = m_alpha ? getDC() : getTexDC();
	lResult = ::CallWindowProcW(m_oldProc, hWnd, uMsg, wParam, lParam);

	if (m_renderType==5 && m_oleObject) // Ole
	{
		// 浏览器控件的典型样式：
		//	<object progid="Shell.Explorer"				浏览器控件的PROGID
		//		bind-class="Internet Explorer_Server"	改变到HOOK这个类名的窗口
		//		bind-type="5"							绑定方式
		//		bind-messages="0x0118 timer"			HOOK这些消息
		//		style="width:100%;height:180;"
		//		location="www.baidu.com"				初始化打开的网址
		//		title="WebBrowser">haha</object>

		CComQIPtr<IViewObject> vo = m_oleObject;
		if (vo.p)
		{
			RECTL rc = {0,0,(LONG)m_width,(LONG)m_height};
			HRESULT hr = vo.p->Draw(DVASPECT_CONTENT, -1, NULL, NULL, NULL, hdc, (RECTL*) &rc, NULL, NULL, NULL); 
		}
	}
	else
	{
		switch (m_renderType)
		{
		case 0: // WM_PAINT
			::CallWindowProcW(m_oldProc, hWnd, WM_PAINT, (WPARAM)hdc, lParam);
			break;
		case 1: // WM_PRINT
			::CallWindowProcW(m_oldProc, hWnd, WM_PRINT, (WPARAM)hdc, (LPARAM)(PRF_CHECKVISIBLE|PRF_NONCLIENT|PRF_CLIENT|PRF_ERASEBKGND|PRF_CHILDREN|PRF_OWNED));
			break;
		case 2: // WM_PRINTCLIENT
			::CallWindowProcW(m_oldProc, hWnd, WM_PRINTCLIENT, (WPARAM)hdc, (LPARAM)(PRF_CHECKVISIBLE|PRF_NONCLIENT|PRF_CLIENT|PRF_ERASEBKGND|PRF_CHILDREN|PRF_OWNED));
			break;
		default:
			::CallWindowProcW(m_oldProc, hWnd, WM_PAINT, (WPARAM)hdc, lParam);
		}

		//::SendMessage(m_pWin->m_hWnd, WM_PAINT, (WPARAM)hdc, 0);
		//::SendMessage(hWnd, WM_PRINT, (WPARAM)hdc, (LPARAM)(PRF_CHECKVISIBLE|PRF_NONCLIENT|PRF_CLIENT|PRF_ERASEBKGND|PRF_CHILDREN|PRF_OWNED));
		//::SendMessage(m_hWnd, WM_PRINTCLIENT, (WPARAM)hdc, (LPARAM)(/*PRF_CHECKVISIBLE|*/PRF_NONCLIENT|PRF_CLIENT|PRF_ERASEBKGND|PRF_CHILDREN|PRF_OWNED));
	}

	if (m_alpha)
		releaseDC(hdc);
	else
		releaseTexDC(hdc);
	m_dirty = FALSE;
	m_device->render();

	return TRUE;
}

LRESULT CALLBACK iGdiTexture::__WndProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	iGdiTexture* pThis = (iGdiTexture*)::GetPropW(hWnd, g_szGdiTexture);
	if (pThis==NULL)
		return ::DefWindowProc(hWnd, uMsg, wParam, lParam);

	LRESULT lRes;
	if (pThis->__IsPaintMsg(hWnd, uMsg, wParam, lParam, lRes))
		return lRes;

	if (uMsg == WM_NCDESTROY)
	{
		LRESULT lRes = ::CallWindowProc(pThis->m_oldProc, hWnd, uMsg, wParam, lParam);

		// unsubclass
		SubclassWindow(hWnd, pThis->m_oldProc);
		pThis->m_oldProc = ::DefWindowProc;
		//::SetPropW(hWnd, g_szGdiTexture, (HANDLE) NULL);
		::RemoveProp(hWnd, g_szGdiTexture);
		pThis->m_wnd = NULL;
		return lRes;
	}
	else if (uMsg == WM_PARENTNOTIFY && LOWORD(wParam)==WM_CREATE && pThis->m_changeWinClassName)
	{
		HWND hwndCtrl = (HWND)lParam;
		WCHAR name[256] = L"";
		::GetClassNameW(hwndCtrl, name, 256);
		ATLTRACE(L"%s\n", name);
		LRESULT lRes = ::CallWindowProc(pThis->m_oldProc, hWnd, uMsg, wParam, lParam);
		pThis->bindWindow(hwndCtrl, pThis->m_alpha, FALSE);
		if (/*pThis->m_changeWinClassName &&*/ lstrcmpiW(pThis->m_changeWinClassName, name)==0)
		{
			pThis->m_changeWinClassName.Empty();
		}
		return lRes;
	}

	return ::CallWindowProc(pThis->m_oldProc, hWnd, uMsg, wParam, lParam);
}

//////////////////////////////////////////////////////////////////////////
iClipTexture::iClipTexture(iQuadObject* quad) : m_quad(quad), m_pitch(m_quad->m_width * 4), m_path(NULL)
			, m_dirty(TRUE), m_time(0)
{
	//
}

iClipTexture::~iClipTexture()
{
	m_texture = NULL;
}

void iClipTexture::resize()
{
	m_time = 0;
	m_texture = NULL;

	m_pitch = m_quad->m_width * 4;
	m_dirty = TRUE;
}

void iClipTexture::update_path()
{
	m_time = ::timeGetTime();
	m_dirty = FALSE;
	if (m_path==NULL || !m_path->IsValid()) return;

	ULONG total = (m_quad->m_width * m_quad->m_height + 3) & ~3; // 保证 m_width * m_height 是 4 的倍数
	LPDWORD buffer = NEW DWORD[total * 2]; // 这里不能分配 TOTAL 内存，必须增大，否则DDRAW可能会写超出的部分，引发CRT错误警告
	if (buffer==NULL)
		return;

	HRESULT hr = S_OK;
	CComPtr<IDirectDrawSurface7> ddsurface;
	// 创建表面后定位至新内存
	CheckHResult hr = m_quad->m_device->m_dd->CreateSurface((LPDDSURFACEDESC2)&g_ddsdDummy, &ddsurface, NULL);
	if (FAILED(hr))
	{
		delete[] buffer;
		return;
	}

	iDDSD ddsd;
	ddsd.SetExternalMemory(m_quad->m_width, m_quad->m_height, m_pitch, buffer);
	CheckHResult hr = ddsurface->SetSurfaceDesc(&ddsd, 0);
	if (FAILED(hr))
	{
		ddsurface = NULL;
		delete[] buffer;
		return;
	}

	DDBLTFX bfx = {sizeof(DDBLTFX)};
	bfx.dwFillColor = 0xffffffff;
	CheckHResult hr = ddsurface->Blt(NULL, NULL, NULL, DDBLT_COLORFILL, &bfx);
	HDC hdc = NULL;
	CheckHResult hr = ddsurface->GetDC(&hdc);
	if (SUCCEEDED(hr))
	{
		CRect rc(0,0,m_quad->m_width,m_quad->m_height);
		::BeginPath(hdc);
		m_path->Draw(hdc, rc);
		::EndPath(hdc);
		::FillPath(hdc); // 用任何默认色画刷填充路径，这里只需要填充后的 A 值变成 0 即可达到目标

		ddsurface->ReleaseDC(hdc);
		ddsurface = NULL; // DDRAW 表面只是一个工具，这里可以释放，我们需要的只是缓冲区中的数据

		// 现在要把表面数据传送到纹理中
		if (m_texture == NULL)
		{
			CheckHResult hr = m_quad->m_device->m_device->CreateTexture(m_quad->m_width, m_quad->m_height, 0, 0/*D3DUSAGE_DYNAMIC*/ | D3DUSAGE_AUTOGENMIPMAP, D3DFMT_A8, D3DPOOL_MANAGED, &m_texture.p, NULL);
			if (FAILED(hr))
			{
				ddsurface = NULL;
				delete[] buffer;
				return;
			}
		}

		CComPtr<IDirect3DSurface9> dst;
		CheckHResult hr = m_texture->GetSurfaceLevel(0, &dst);
		if (SUCCEEDED(hr))
		{
			//CheckHResult hr = m_device->m_device->ColorFill(dst, NULL, 0);

			RECT rc = {0,0,(LONG)m_quad->m_width, (LONG)m_quad->m_height};
			CheckHResult hr = D3DXLoadSurfaceFromMemory(dst, NULL, NULL, buffer, D3DFMT_A8R8G8B8, (UINT)m_pitch, NULL, &rc, D3DX_FILTER_LINEAR/*D3DX_FILTER_NONE*/, /*KEYCOLOR_ARGB*/(0));
		}
	}

	ddsurface = NULL;
	delete[] buffer;
}

IDirect3DTexture9* iClipTexture::get_texture()
{
	if (m_path==NULL) return NULL;

	if (m_dirty || m_path->Time() > m_time)
	{
		m_texture = NULL;

		update_path();
	}
	return m_texture;
}
//////////////////////////////////////////////////////////////////////////
id3d::id3d() : capsMMX(0), mstype(D3DMULTISAMPLE_NONE), mstype_d16(D3DMULTISAMPLE_NONE), msquality(0), msquality_d16(0)
{
	// check MMX caps
	__try {
		__asm {
			pxor mm0, mm0           // executing MMX instruction
				emms
		}
		isMMX = TRUE;

		__asm {
			xorps xmm0, xmm0        // executing SSE instruction
		}
		isSSE = TRUE;

		__asm {
			xorpd xmm0, xmm0        // executing SSE2 instruction
		}
		isSSE2 = TRUE;
	}
	#pragma warning (suppress: 6320)
	__except (EXCEPTION_EXECUTE_HANDLER) {
		if (_exception_code() == STATUS_ILLEGAL_INSTRUCTION) {}
	}

	// check 3DNow
	__try {
		__asm {
			pfrcp mm0, mm0          // executing 3DNow! instruction
				emms
		}
		is3DNow = TRUE;
	}
	__except (EXCEPTION_EXECUTE_HANDLER) {}
}

id3d::~id3d()
{
}

IDirect3D9* id3d::D3DCreate()
{
	if (gt.d3d9==NULL)
	{
		gt.hrInit = ::CoInitialize(NULL);
		gt.d3d9 = Direct3DCreate9(D3D_SDK_VERSION);
		if (gt.d3d9)
		{
			HRESULT hr;
			hr = gt.d3d9->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &displayMode);
			hr = gt.d3d9->GetDeviceCaps(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, &caps);
			mstype = mstype_d16 = D3DMULTISAMPLE_NONE;
#if 1
			for (DWORD i=D3DMULTISAMPLE_16_SAMPLES; i>=D3DMULTISAMPLE_2_SAMPLES; i--)
			{
				if (SUCCEEDED(hr=gt.d3d9->CheckDeviceMultiSampleType( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL , D3DFMT_A8R8G8B8, TRUE, (D3DMULTISAMPLE_TYPE)i, &msquality)))
				{
					mstype = (D3DMULTISAMPLE_TYPE)i;
					break;
				}
			}
			for (DWORD i=D3DMULTISAMPLE_16_SAMPLES; i>=D3DMULTISAMPLE_2_SAMPLES; i--)
			{
				if (SUCCEEDED(hr=gt.d3d9->CheckDeviceMultiSampleType( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL , D3DFMT_D16, TRUE, (D3DMULTISAMPLE_TYPE)i, &msquality_d16)))
				{
					mstype_d16 = (D3DMULTISAMPLE_TYPE)i;
					break;
				}
			}
#endif // 0
		}
	}
	return gt.d3d9;
}

void id3d::ThreadClear()
{
	if (gt.d3d9)
	{
		gt.d3d9->Release();
		gt.d3d9 = NULL;
	}

	if (SUCCEEDED(gt.hrInit))
	{
		::CoUninitialize();
		gt.hrInit = E_FAIL;
	}
}

bool id3d::isValid()
{
	D3DCreate();
	return /*hDll && pfn &&*/ g_d3dxapi_valid && (gt.d3d9!=NULL) && (caps.Caps2&D3DCAPS2_DYNAMICTEXTURES);
}
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////

iSurfaceBase::iSurfaceBase(iDevice* device, LONG x/*=0*/, LONG y/*=0*/, LONG width/*=0*/, LONG height/*=0*/ ) : m_device(device), /*m_width(width), m_height(height),*/ m_left(x), m_top(y)//, m_viewport(NULL)
{
	ZeroMemory(m_vertexs, sizeof(m_vertexs));
	//m_vertexs[0].color = m_vertexs[1].color = m_vertexs[2].color = m_vertexs[3].color = D3DCOLOR_ARGB(255,255,255,255);

	m_vertexs[0].z = m_vertexs[1].z = m_vertexs[2].z = m_vertexs[3].z = DefaultZ;
	//m_vertexs[0].z = m_vertexs[3].z = 0.0f;
#ifdef USE_RHW
	m_vertexs[0].rhw = m_vertexs[1].rhw = m_vertexs[2].rhw = m_vertexs[3].rhw = 1.0f;
	//m_vertexs[0].rhw = m_vertexs[3].rhw = 0.8f;
#endif // USE_RHW

	m_vertexs[0].u = m_vertexs[3].u = 0.0f;
	m_vertexs[1].u = m_vertexs[2].u = 1.0f;

	m_vertexs[0].v = m_vertexs[1].v = 0.0f;
	m_vertexs[2].v = m_vertexs[3].v = 1.0f;

	resize(width, height);
}

void iSurfaceBase::_updateVertexes()
{
	m_vertexs[0].x = m_vertexs[3].x = ((float)(m_left)) - AdjustXY;
	m_vertexs[1].x = m_vertexs[2].x = ((float)(m_left + m_width)) - AdjustXY;

	m_vertexs[0].y = m_vertexs[1].y = ((float)(m_top)) - AdjustXY;
	m_vertexs[2].y = m_vertexs[3].y = ((float)(m_top + m_height)) - AdjustXY;
}

void iSurfaceBase::resize( LONG width, LONG height )
{
	m_width = width;
	m_height = height;
	_updateVertexes();
}

void iSurfaceBase::move( LONG x, LONG y, LONG width, LONG height )
{
	m_left = x;
	m_top = y;
	resize(width, height);
}

void iSurfaceBase::setcolor( D3DCOLOR c0, D3DCOLOR c1/*=0*/, D3DCOLOR c2/*=0*/, D3DCOLOR c3/*=0*/ )
{
	m_vertexs[0].color = c0;
	m_vertexs[1].color = (c1 ? c1 : c0);
	m_vertexs[2].color = (c2 ? c2 : c0);
	m_vertexs[3].color = (c3 ? c3 : c0);
}

//////////////////////////////////////////////////////////////////////////

iDevice::iDevice( HWND hwnd ) : 
	m_hwnd(hwnd)
	, m_colorFill(D3DCOLOR_XRGB(255,255,255))
	, isLayered(FALSE)
	, isRendering(FALSE)
	, isReseting(FALSE)
	, isRelayout(FALSE)
	//, m_hbmp(NULL)
	//, m_hdc(NULL)
	//, m_pBits(NULL)
	//, m_pSurfaceBuffer(NULL)
	//, m_bufSize(0)
	, m_targets(NULL)
	, m_textures(NULL)
	, m_toptexture(NULL)
	, m_ordered_textures(NULL)
	//, m_dcOrig(this, 0, 0, 1, 1, NULL)
	, m_images(NULL)
	, m_effects(NULL)
//#ifdef UseSystemEffect
	, m_system_effect(this)
//#endif // UseSystemEffect
	, m_caret(this)
	, m_controls(NULL)
	, m_stories(NULL)
	, m_models(NULL)
	, m_scenes(NULL)
	//, m_current_target(NULL)
	//, m_current_scene(NULL)
	, m_curTime(0.0f)
	, m_elapsedTime(0.0f)
	, m_story_timerid(-1)
	, m_fps(30)
	, m_hscript(NULL)
	//, m_namedObjects(NULL)
{
	if (!::IsWindow(m_hwnd))
		return;

	HWND hwndFocus = m_hwnd;
	HWND hwndTmp;
	while ((hwndTmp = ::GetParent(hwndFocus)) != NULL)
		hwndFocus = hwndTmp;

	_initPresentParams();
	CComResult hr (E_FAIL);
	hr = gt.d3d9->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hwndFocus, D3DCREATE_HARDWARE_VERTEXPROCESSING, &_pp, &m_device.p);
	if (FAILED(hr))
		hr = gt.d3d9->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hwndFocus, D3DCREATE_MIXED_VERTEXPROCESSING, &_pp, &m_device.p);
	if (FAILED(hr))
		hr = gt.d3d9->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hwndFocus, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &_pp, &m_device.p);
#ifdef _DEBUG
	if (FAILED(hr))
	{
		::MessageBoxW(m_hwnd, L"Create REF Device!", L"NO HAL DEVICE", MB_OK|MB_ICONWARNING);
		CheckHResult hr = gt.d3d9->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_REF, hwndFocus, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &_pp, &m_device.p);
	}
#endif // _DEBUG

	isValid = SUCCEEDED(hr);
	if (!isValid)
	{
		ATLTRACE(L"==== CREATE DEVICE FAILED! ====\n");
		::MessageBoxW(m_hwnd, L"Create Device Failed!", L"NO 3D DEVICE", MB_OK|MB_ICONWARNING);
	}

	if (isValid)
	{
		// 更新至链表
		addToList(&gt.devices);

		//ATLASSERT(FALSE);
		CheckHResult hr = ::D3DXCreateEffectPool(&m_effectPool);
//#ifdef UseSystemEffect
		m_system_effect.init();
//#endif // UseSystemEffect

		// 创建DDRAW设备
		CheckHResult hr = ::DirectDrawCreateEx(NULL, (LPVOID*)&m_dd.p, IID_IDirectDraw7, NULL);
		if (m_dd.p == NULL) return;
		if (SUCCEEDED(hr))
			CheckHResult hr = m_dd->SetCooperativeLevel(m_hwnd, DDSCL_NORMAL);

		//// 创建顶点缓冲
		//hr = m_device->CreateVertexBuffer(1024*4*sizeof(VERTEX), D3DUSAGE_WRITEONLY, D3DFVF_VERTEX, D3DPOOL_MANAGED, &m_vb, NULL);
#if 0
		// 测试一个矩形
		VERTEX* pVertices = NULL;
		hr = m_vb->Lock(0, 4*sizeof(VERTEX), (void**)&pVertices, 0);
		if (pVertices)
		{
			pVertices[0].color = pVertices[1].color = pVertices[2].color = pVertices[3].color = D3DCOLOR_ARGB(240,255,0,0);

			pVertices[0].x = pVertices[3].x = 0.0f;
			pVertices[1].x = pVertices[2].x = 100.0f;

			pVertices[0].y = pVertices[1].y = 0.0f;
			pVertices[2].y = pVertices[3].y = 50.0f;

			pVertices[0].z = pVertices[1].z = pVertices[2].z = pVertices[3].z = DefaultZ;

#ifdef USE_RHW
			pVertices[0].rhw = pVertices[1].rhw = pVertices[2].rhw = pVertices[3].rhw = 1.0f;
#endif // USE_RHW

			pVertices[1].u = pVertices[2].u = 1.0f;
			pVertices[0].u = pVertices[3].u = 0.0f;

			pVertices[0].v = pVertices[1].v = 0.0f;
			pVertices[2].v = pVertices[3].v = 1.0f;
			hr = m_vb->Unlock();
		}
#endif // 0


		// 测试代码结束

		//_initDeviceState();
	}
}

iDevice::~iDevice()
{
	if (m_dd) m_dd.Release();

	relayout();

	if (m_toptexture) delete m_toptexture;

	// 从链表删除
	//if (isValid)
	{
		removeFromList(/*&gt.devices*/);
	}
}

void iDevice::relayout()
{
	isRelayout = TRUE;

	// remove timer
	if (m_story_timerid!=-1) m_story_timerid = (::KillMtTimer(m_story_timerid), -1);

	// images and effects
	if (m_images) m_images->removeAll(true);
	if (m_effects) m_effects->removeAll(true);
	//m_images = (_removeImages(m_images), NULL);

	// storyboard
	if (m_stories) m_stories->unload(), m_stories->removeAll();
	//m_stories = (_removeStories(m_stories), NULL);

	// model
	if (m_models) m_models->unload(), m_models->TList<iModelBase>::removeAll();

	// scene
	if (m_scenes) m_scenes->unload(), m_scenes->removeAll();
	//if (m_namedObjects) m_namedObjects->removeAll();
}

void iDevice::_initPresentParams()
{
	ZeroMemory(&_pp, sizeof(D3DPRESENT_PARAMETERS));
	_pp.Windowed = TRUE;
	_pp.hDeviceWindow = m_hwnd;

	isLayered = (IsLayered(m_hwnd) != 0);
	if (isLayered) m_colorFill = 0; //D3DCOLOR_ARGB(32,0,0,192);
	else m_colorFill = D3DCOLOR_XRGB(255,255,255);

	_pp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	_pp.Flags = 0; //D3DPRESENTFLAG_DISCARD_DEPTHSTENCIL;
	if (isLayered)
	{
		// 分层窗口增加A通道，后台缓冲必须支持锁定，以便更新图片，不可以调用Present
		_pp.BackBufferFormat = D3DFMT_A8R8G8B8;
		_pp.Flags |= D3DPRESENTFLAG_LOCKABLE_BACKBUFFER;
	}
	else
	{
		_pp.BackBufferFormat = D3DFMT_X8R8G8B8;
	}
	_pp.BackBufferCount = 1;
	//_pp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
	_pp.EnableAutoDepthStencil = TRUE;
	_pp.AutoDepthStencilFormat = /*D3DFMT_UNKNOWN; //*/ D3DFMT_D16;
	_pp.MultiSampleType = isLayered ? D3DMULTISAMPLE_NONE : g_d3d.mstype/*D3DMULTISAMPLE_4_SAMPLES*/;
	_pp.MultiSampleQuality = g_d3d.msquality - 1;

	RECT rc;
	//if (isLayered)
	//	::GetWindowRect(m_hwnd, &rc);
	//else
		::GetClientRect(m_hwnd, &rc);
	_pp.BackBufferWidth = (UINT)(rc.right - rc.left);
	_pp.BackBufferHeight = (UINT)(rc.bottom - rc.top);

	m_layeredSurf = NULL;
}

void iDevice::render()
{
	if (isRendering)
		return;

	//RECT rc;
	//::GetClientRect(m_hwnd, &rc);
	//if (_pp.BackBufferWidth!=(UINT)(rc.right - rc.left) || _pp.BackBufferHeight!=(UINT)(rc.bottom - rc.top))
	//	Reset();

	HRESULT hr = m_device->TestCooperativeLevel();
	if (hr == D3DERR_DEVICELOST)
	{
		ATLTRACE(L"===DEVICE LOST!===\n");
		on_story_started();
		//::InvalidateRect(m_hwnd, NULL, FALSE);
		return;
	}
	else if (hr == D3DERR_DEVICENOTRESET)
	{
		//ATLTRACE(L"===DEVICE NOT RESET!===\n");
		on_reset();
		::InvalidateRect(m_hwnd, NULL, TRUE);
		return;
	}
	else if (FAILED(hr))
	{
		ATLTRACE(L"===DEVICE ERROR(0x%08x)!===\n", hr);
		return;
	}

	if (!isValid) return;
	isRelayout = FALSE;

	// pre_render
	if (m_models) m_models->pre_render();
	if (m_ordered_textures) m_ordered_textures->ordered_render(false, true);
	if (m_textures) m_textures->pre_render();
	if (m_ordered_textures) m_ordered_textures->ordered_render(true, true);
	if (m_toptexture) m_toptexture->pre_render();

	_updateTime();

	// 推进动画
	if (m_stories) m_stories->step(m_elapsedTime);

	// 渲染到纹理阶段
	//{
	//	// render negative z-order
	//	if (m_ordered_textures)
	//		m_ordered_textures->ordered_render(false, true);

	//	if (m_textures)
	//		m_textures->pre_render();

	//	// render positive z-order
	//	if (m_ordered_textures)
	//		m_ordered_textures->ordered_render(true, true);
	//}
	//if (m_scenes)
	//	m_scenes->pre_render();

	isRendering = TRUE;

	CComPtr<IDirect3DSurface9> surf;
	CheckHResult hr = m_device->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &surf.p);
	CheckHResult hr = m_device->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER /*| D3DCLEAR_STENCIL*/, m_colorFill, 1.0f, 0);
	//CheckHResult hr = m_device->ColorFill(surf.p, NULL, m_colorFill);
	CheckHResult hr = m_device->BeginScene();
	_initDeviceState();
	m_system_effect.set_clip_texture();
	m_system_effect.set_device_params();

#if 0
	if (m_scenes)
	{
		m_scenes->render_scene();
	}
	else
#endif // 1
	{
		// render negative z-order
		if (m_ordered_textures)
			m_ordered_textures->ordered_render(false);

		if (m_textures)
			m_textures->render();

		// render positive z-order
		if (m_ordered_textures)
			m_ordered_textures->ordered_render(true);

		if (m_toptexture)
			m_toptexture->render();

		if (isLayered)
		{
			iControl* c = m_controls;
			while (c)
			{
				c->render();
				c = c->next;
			}
		}

		if (m_caret.owner == NULL)
			m_caret.render();
	}

	CheckHResult hr = m_device->EndScene();
	isRendering = FALSE;
	isReseting = FALSE;

	// 标准模式下执行 flip
	if (!isLayered)
		CheckHResult hr = m_device->Present(NULL, NULL, NULL, NULL);

	// 分层模式下更新分层窗口
	else
	{
		RECT rcClient, rc;
		::GetClientRect(m_hwnd, &rcClient);
		::GetWindowRect(m_hwnd, &rc);
		//::CopyRect(&rcClient, &rc);
		long w = rcClient.right - rcClient.left;
		long h = rcClient.bottom - rcClient.top;

		//if (m_pBits==NULL)
		//{
		//	BITMAPINFOHEADER bih = {0};
		//	bih.biSize = sizeof(BITMAPINFOHEADER);
		//	bih.biBitCount = 32;
		//	bih.biCompression = BI_RGB;
		//	bih.biPlanes = 1;
		//	bih.biWidth = w;
		//	bih.biHeight = -h;
		//	bih.biSizeImage = w * h * sizeof(DWORD);
		//	//LPDWORD pBits = NULL;
		//	m_hbmp = ::CreateDIBSection(NULL, (BITMAPINFO *)&bih, DIB_RGB_COLORS, (void **)&m_pBits, NULL, 0x0);
		//	m_hdc = ::CreateCompatibleDC(NULL);
		//	::SelectObject(m_hdc, m_hbmp);
		//	if (m_hbmp==NULL || m_hdc==NULL)
		//		return;
		//}

		// 尝试创建DD表面
		// 拷贝表面数据到缓冲区
		//D3DLOCKED_RECT lock;
		//hr = surf->LockRect(&lock, NULL, D3DLOCK_NOSYSLOCK | D3DLOCK_READONLY);
		////setBufferSize(lock.Pitch*h);
		////CopyMemory(m_pSurfaceBuffer, lock.pBits, lock.Pitch*h);
		////surf->UnlockRect();

		//// 如果需要，准备DD表面
		//if (m_ddSurface.p == NULL)
		//{
		//	if (m_dd.p == NULL)
		//	{
		//		hr = ::DirectDrawCreateEx(NULL, (LPVOID*)&m_dd.p, IID_IDirectDraw7, NULL);
		//		if (m_dd.p == NULL) return;
		//		if (SUCCEEDED(hr))
		//			hr = m_dd->SetCooperativeLevel(m_hwnd, DDSCL_NORMAL);
		//	}

		//	// 首先尝试用 IDirectDraw4 来直接创建表面
		//	//CComPtr<IDirectDraw4> dd4;
		//	//CComPtr<IDirectDrawSurface4> surf4;
		//	//hr = m_dd->QueryInterface(IID_IDirectDraw4, (LPVOID*)&dd4.p);
		//	//if (SUCCEEDED(hr))
		//	//	hr = dd4->CreateSurface(&ddsd, &surf4, NULL);
		//	//if (SUCCEEDED(hr))
		//	//	hr = dd4->QueryInterface(IID_IDirectDrawSurface7, (LPVOID*)&m_ddSurface.p);
		//	//// 再尝试创建临时表面，然后重设缓冲区
		//	//if (FAILED(hr))
		//		hr = m_dd->CreateSurface((LPDDSURFACEDESC2)&g_ddsdDummy, &m_ddSurface, NULL);
		//	if (FAILED(hr)) return;
		//}

		//iDDSD ddsd;
		//ddsd.SetExternalMemory(w,h,lock.Pitch,lock.pBits);
		//hr = m_ddSurface->SetSurfaceDesc(&ddsd, 0);
		//if (FAILED(hr)) return;

		//TimeCounter tc(L"UpdateLayeredWindow");
		if (m_layeredSurf.p==NULL)
		{
			CheckHResult hr = m_device->CreateOffscreenPlainSurface(rcClient.right-rcClient.left, rcClient.bottom-rcClient.top, D3DFMT_A8R8G8B8, D3DPOOL_SYSTEMMEM, &m_layeredSurf, NULL);
			ATLASSERT(SUCCEEDED(hr));
		}

		if (m_layeredSurf.p)
		{
			CheckHResult hr = m_device->GetRenderTargetData(surf, m_layeredSurf);

			HDC ddc = NULL;
			CheckHResult hr = m_layeredSurf->GetDC(&ddc);
			//hr = m_ddSurface->GetDC(&ddc);

			// update layered window
			POINT ptOffset = CLayerHelper::GetClientOffset(m_hwnd);
			POINT ptDst = {rc.left + ptOffset.x, rc.top + ptOffset.y};
			POINT ptSrc = {0, 0};
			SIZE sz = {w, h};
			BLENDFUNCTION bf = {AC_SRC_OVER, 0, 255, AC_SRC_ALPHA};
			BOOL bRet = ::UpdateLayeredWindow(m_hwnd, NULL, &ptDst, &sz, ddc, &ptSrc, 0, &bf, ULW_ALPHA /*::GetDeviceCaps(NULL,BITSPIXEL)==32 ? ULW_ALPHA : ULW_OPAQUE*/);

			//hr = m_ddSurface->ReleaseDC(ddc);
			//surf->UnlockRect();
			CheckHResult hr = m_layeredSurf->ReleaseDC(ddc);
		}
	}
}

void iDevice::reset()
{
	CRect rc;
	::GetClientRect(m_hwnd, &rc);
	if ((_pp.BackBufferWidth==(UINT)rc.Width() && _pp.BackBufferHeight==(UINT)rc.Height()) &&
		(isLayered == (IsLayered(m_hwnd) != 0)))
		return;

	isReseting = TRUE;
	_initPresentParams();
	m_device->Reset(&_pp);
	render();
}

void iDevice::on_reset()
{
	//if (!isValid) return;
	_initPresentParams();
	//m_dcOrig.on_reset();
	if (m_textures) m_textures->on_reset();
	if (m_toptexture) m_toptexture->on_reset();
	m_system_effect.on_reset();
	if (m_effects) m_effects->on_reset();
	if (m_scenes) m_scenes->on_reset();

	iControl* c = m_controls;
	while (c)
	{
		c->on_reset();
		c = c->next;
	}

	//m_depth_surface = NULL;

	LineLog dbg(L"=== Device is resetting...");
	CComResult hr;
	CheckHResult hr = m_device->Reset(&_pp);
	isValid = (/*hr==D3DERR_INVALIDCALL ||*/ SUCCEEDED(hr));
	if (!isValid)
	{
		dbg(L"FAILED!");
		isReseting = TRUE;
		if (hr == D3DERR_DEVICELOST)
		{
			dbg(L" - DeviceLost");
			//::InvalidateRect(m_hwnd, NULL, FALSE);
		}
		on_story_started(); // 为了启动定时器
	}
	else
	{
		dbg(L"SUCCEEDED.");
		isReseting = FALSE;
		on_story_stopped();
	}

	//m_dcOrig.move(rc.left, rc.top, rc.Width(), rc.Height());
	if (m_toptexture)
	{
		CRect rc;
		::GetClientRect(m_hwnd, &rc);
		m_toptexture->move(rc.left, rc.top, rc.Width(), rc.Height());
	}

	//_initDeviceState();
}

void iDevice::invalidate()
{
	if (m_story_timerid==-1)
		::InvalidateRect(m_hwnd, NULL, FALSE);
}

void iDevice::_initDeviceState()
{
	if (!isValid) return;
	// 设置全局设备状态
	CComResult hr;
	CheckHResult hr = m_device->SetFVF(D3DFVF_VERTEX);
	////CheckHResult hr = m_device->SetRenderState(D3DRS_ZENABLE, /*D3DZB_TRUE*/D3DZB_FALSE);	// disable depth buffering
	////CheckHResult hr = m_device->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
	CheckHResult hr = m_device->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE/*D3DZB_FALSE*/);	// disable depth buffering
	CheckHResult hr = m_device->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
	//CheckHResult hr = m_device->SetRenderState(D3DRS_STENCILENABLE, FALSE);
	CheckHResult hr = m_device->SetRenderState(D3DRS_MULTISAMPLEANTIALIAS, TRUE);	// disable depth buffering
	////CheckHResult hr = m_device->SetRenderState(D3DRS_LASTPIXEL, FALSE);		// disable drawing of the last pixel
	////CheckHResult hr = m_device->SetRenderState(D3DRS_LOCALVIEWER, FALSE);	// use orthogonal projection should specify false
	////CheckHResult hr = m_device->SetRenderState(D3DRS_POINTSCALEENABLE, TRUE);
	////CheckHResult hr = m_device->SetRenderState(D3DRS_POINTSPRITEENABLE, TRUE);

	////CheckHResult hr = m_device->SetRenderState(D3DRS_SLOPESCALEDEPTHBIAS, 0);
	////CheckHResult hr = m_device->SetRenderState(D3DRS_DEPTHBIAS, 0);

	////float fPointSize = 1.0f;
	////CheckHResult hr = m_device->SetRenderState(D3DRS_POINTSIZE, *(DWORD*)&fPointSize);
	////CheckHResult hr = m_device->SetRenderState(D3DRS_POINTSCALE_B, *(DWORD*)&fPointSize);

	// alpha blend
	CheckHResult hr = m_device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);	// enable alpha-blended transparency
	CheckHResult hr = m_device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	CheckHResult hr = m_device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	CheckHResult hr = m_device->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD); // select the arithmetic operation applied when the alpha blending render state

	//// alpha test
	//CheckHResult hr = m_device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE); 
	////CheckHResult hr = m_device->SetRenderState(D3DRS_ALPHAREF, (DWORD)0x00000001);
	////CheckHResult hr = m_device->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL);

	//CheckHResult hr = m_device->SetRenderState(D3DRS_CLIPPING, TRUE); // enable primitive clipping by Direct3D
	////CheckHResult hr = m_device->SetRenderState(D3DRS_INDEXEDVERTEXBLENDENABLE, TRUE); // disables indexed vertex blending

	CheckHResult hr = m_device->SetRenderState(D3DRS_LIGHTING, FALSE); // 没有使用光照，所以必须关闭光照，否则绘制的图元都是黑色
	////for (DWORD i=0; i<8; i++)
	////{
	////	CheckHResult hr = m_device->LightEnable(i, FALSE);
	////}
	//
	////CheckHResult hr = m_device->SetRenderState(D3DRS_AMBIENT, D3DCOLOR_ARGB(255,255,0,0));

	////if (g_d3d.caps.PrimitiveMiscCaps & D3DPMISCCAPS_SEPARATEALPHABLEND)
	////{
		//CheckHResult hr = m_device->SetRenderState(D3DRS_SEPARATEALPHABLENDENABLE, FALSE); // enables the separate blend mode for the alpha channel
	////	CheckHResult hr = m_device->SetRenderState(D3DRS_SRCBLENDALPHA, D3DBLEND_SRCALPHA);
	////	CheckHResult hr = m_device->SetRenderState(D3DRS_DESTBLENDALPHA, D3DBLEND_INVSRCALPHA);
	////	CheckHResult hr = m_device->SetRenderState(D3DRS_BLENDOPALPHA, D3DBLENDOP_ADD);
	////}

	////CheckHResult hr = m_device->SetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE);

	RECT rcClient;
	::GetClientRect(m_hwnd, &rcClient);
	long w = rcClient.right - rcClient.left;
	long h = rcClient.bottom - rcClient.top;
	float z = 0.0f;

	// 设置视口
	D3DVIEWPORT9 vp = {(DWORD)(0), (DWORD)(0),
		(DWORD)(w), (DWORD)(h),
		0.f, 1.f};
		//ZNear, ZFar};
	CheckHResult hr = m_device->SetViewport(&vp);

	D3DXMATRIX tmp;
	// 设置投影矩阵
#if 0
	float yScale = 1 / tanf( D3DXToRadian( 45.0f ) );
	z = yScale * h / 2;

	D3DXMATRIX matProj;
	//D3DXMatrixPerspectiveOffCenterLH(&tmp, 0, (float)w, 0, (float)h, 0.0f, 1.0f);
	D3DXMatrixPerspectiveFovLH(&matProj, D3DXToRadian( 90.0f ), (float)w/(float)h, z + 0.0f, z + 1000.0f);
	//D3DXMatrixPerspectiveOffCenterLH(&matProj, 0.0f, (float)w, (float)h, 0.0f, 0.0f, 1.0f);
#else
	// 这样设置投影矩阵，将可以直接使用屏幕坐标而无需转换到 [-1, 1]，也无需做0.5的偏移
	// 	m->_11 = 2.0f / w;
	// 	m->_22 = 2.0f / -h;
	// 	m->_33 = 1.0f / (zfar - znear);
	// 	m->_41 = (w + 1.0f) / -w;
	// 	m->_42 = (h + 1.0f) / h;
	// 	m->_43 = znear / (znear - zfar);
	// 	m->_44 = 1.0;
	D3DXMATRIX matProj(2.0f/w, 0, 0, 0,
						0, 2.0f/-h, 0, 0,
						0, 0, 1.0f/(ZFar - ZNear), 0,
						(w+1.0f)/-w, (h+1.0f)/h, ZNear/(ZFar-ZNear), 1); // 这种投影矩阵可以使非RHW的顶点直接使用屏幕坐标
	//// 等效方法2
	//D3DXMatrixOrthoOffCenterLH(&matProj, 0, (float)w, (float)h, 0, 0.0f, 1.0f); // 设置左上角为原点
	//D3DXMatrixTranslation(&tmp, -0.5f, -0.5f, 0); // 偏移0.5
	//matProj = tmp * matProj;

	//// 等效方法3,HGE的做法
	//float zoom = 1.0f; // 越大越近
	//D3DXMatrixScaling(&matProj, 1.0f, -1.0f, 1.0f);
	//D3DXMatrixTranslation(&tmp, -0.5f, (float)h+0.5f, 0.0f);
	//D3DXMatrixMultiply(&matProj, &matProj, &tmp);
	//D3DXMatrixOrthoOffCenterLH(&tmp, 0, (float)w/zoom, 0, (float)h/zoom, 0.0f, 1.0f);
	//D3DXMatrixMultiply(&matProj, &matProj, &tmp);

	///*g_d3d.d3dxApi.fn*/D3DXMatrixOrthoLH(&matProj, (float)w, (float)h, 0.0f, 1.0f);
	///*g_d3d.d3dxApi.fn*/D3DXMatrixOrthoLH(&matProj, D3DXToRadian( 45.0f ), (float)w/(float)h, 0.0f, 1.0f);
#endif // 0
	CheckHResult hr = m_device->SetTransform(D3DTS_PROJECTION, &matProj);


	D3DXMATRIX Identity;
	D3DXMatrixIdentity(&Identity);

	// 设置世界矩阵
	D3DXMATRIX matWorld = Identity;
#if 1
	//D3DXMatrixScaling(&matWorld, 1.0f, -1.0f, 1.0f);
	//D3DXMatrixTranslation(&tmp, -(float)w/2, (float)h/2, 0.0f);
	//matWorld *= tmp;
#else
	D3DXMatrixRotationY(&matWorld, D3DXToRadian(0.5f));
#endif // 0
	CheckHResult hr = m_device->SetTransform(D3DTS_WORLD, &matWorld);

	// 设置观察矩阵
	D3DXMATRIX matView = Identity;
#if 0
	D3DXVECTOR3 vEyePt(w/2.0f, -h/2.0f, -0.1f);
	D3DXVECTOR3 vLookatPt(w/2.0f, -h/2.0f, DefaultZ);
	D3DXVECTOR3 vUpVec(0.0f, 1.0f, 0.0f);
	D3DXMatrixLookAtLH(&matView, &vEyePt, &vLookatPt, &vUpVec);
#endif // 0
#if 0
	matView._22 = -1;
	matView._33 = -1;
	matView._41 = -( (float)w / 2 );
	matView._42 = ( (float)h / 2 );
	matView._43 = z;
#endif // 0
	CheckHResult hr = m_device->SetTransform(D3DTS_VIEW, &matView);
	//CheckHResult hr = m_device->SetTransform(D3DTS_TEXTURE0, &Identity);
}

//void iDevice::_removeImages( iImageResource* ir )
//{
//	if (ir)
//	{
//		_removeImages(ir->next);
//		delete ir;
//	}
//}
//
//void iDevice::_removeStories( iStoryBoard* story )
//{
//	if (story)
//	{
//		_removeStories(story->next);
//		delete story;
//	}
//}

ID3DXInclude* iDevice::getInclude()
{
	class DeviceInclude : public ID3DXInclude
	{
		LPCSTR data;
		UINT size;
		DeviceInclude() : data(NULL), size(0)
		{
			HRSRC hResource = ::FindResourceW( _AtlBaseModule.GetResourceInstance(), L"common", L"system" );
			ATLASSERT(hResource);

			HGLOBAL hGlobal = ::LoadResource( _AtlBaseModule.GetResourceInstance(), hResource );
			ATLASSERT(hGlobal);

			size = ::SizeofResource(_AtlBaseModule.GetResourceInstance(), hResource);
			data = (LPCSTR)::LockResource(hGlobal);
		}

	public:
		static ID3DXInclude* getInstance()
		{
			static DeviceInclude di;
			return di.data ? &di : NULL;
		}

		STDMETHOD(Open)(D3DXINCLUDE_TYPE IncludeType, LPCSTR pFileName, LPCVOID pParentData, LPCVOID *ppData, UINT *pBytes)
		{
			if (lstrcmpiA(pFileName, "redirectui") == 0 || lstrcmpiA(pFileName, "redui") == 0)
			{
				if (ppData) *ppData = data;
				if (pBytes) *pBytes = size;
			}
			else // try url
			{
				CComPtr<IStream> pStream;
				CComResult hr = CDownload::LoadUrlToStream(CA2W(pFileName), &pStream, NULL);
				if (SUCCEEDED(hr))
				{
					STATSTG ss;
					hr = pStream->Stat(&ss, STATFLAG_NONAME);
					ULONG cb = (ULONG)ss.cbSize.QuadPart;
					if (cb==0) return E_FAIL;
					if (pBytes) *pBytes = cb;

					if (ppData)
					{
						*ppData = NEW BYTE[cb];
						ULONG cbRead = 0;
						hr = pStream->Read((void*)*ppData, cb, &cbRead);
					}
				}
			}
			return S_OK;
		}
		STDMETHOD(Close)(LPCVOID pData)
		{
			if (pData != data) delete[] pData;
			return S_OK;
		}
	};

	return DeviceInclude::getInstance();
}

CONST D3DXMACRO* iDevice::getMacro()
{
	if (m_macros.GetSize()==0)
	{
		if (g_d3d.caps.VertexShaderVersion >= D3DVS_VERSION(3,0) && g_d3d.caps.PixelShaderVersion >= D3DPS_VERSION(3,0))
		{
			D3DXMACRO m = {"SHADER_VERSION", "3"};
			m_macros.Add(m);
		}
		else
		{
			if (g_d3d.caps.VertexShaderVersion >= D3DVS_VERSION(2,0) && g_d3d.caps.PixelShaderVersion >= D3DPS_VERSION(2,0))
			{
				::MessageBoxW(m_hwnd, L"您机器上的 DirectX 所支持的 Shader 版本过低，将影响显示效果。建议您升级到最新的 DirectX 运行版本。", L"提示", MB_OK | MB_ICONWARNING);
				D3DXMACRO m = {"SHADER_VERSION", "2"};
				m_macros.Add(m);
			}
			else
			{
				::MessageBoxW(m_hwnd, L"您机器上的显示驱动不支持 Shader，系统无法正常运行。", L"严重警告", MB_OK | MB_ICONERROR);
				D3DXMACRO m = {"SHADER_VERSION", "0"};
				m_macros.Add(m);
			}
		}

		D3DXMACRO mend = {0,0};
		m_macros.Add(mend);
	}
	return (CONST D3DXMACRO*)m_macros.GetData();
}

LONG iDevice::current_target_orig_x() const
{
	return (m_runtime.target && m_runtime.target->m_quad) ? m_runtime.target->m_quad->m_left : 0;
}

LONG iDevice::current_target_orig_y() const
{
	return (m_runtime.target && m_runtime.target->m_quad) ? m_runtime.target->m_quad->m_top : 0;
}

VERTEX* iDevice::update_target_vertex( VERTEX* vSrc, int num/*=4*/ )
{
	if (vSrc==NULL || num<=0) return vSrc;
	if (m_runtime.target==NULL || m_runtime.target->m_quad==NULL || (m_runtime.target->m_quad->m_left==0 && m_runtime.target->m_quad->m_top==0)) return vSrc;

	static CAtlArray<VERTEX> __vBuf;
	__vBuf.SetCount(num);
	for (int i=0; i<num; i++)
	{
		__vBuf[i] = vSrc[i];
		__vBuf[i].x -= m_runtime.target->m_quad->m_left;
		__vBuf[i].y -= m_runtime.target->m_quad->m_top;
	}
	return __vBuf.GetData();
}

void iDevice::_updateTime()
{
	if (m_curTime==0.0f)
	{
		m_curTime = (float)timeGetTime() * 0.001f;
		return;
	}

	float curTime = (float)timeGetTime() * 0.001f;
	m_elapsedTime = curTime - m_curTime;
	m_curTime = curTime;
}

IDirect3DTexture9* iDevice::find_texture( LPCOLESTR name )
{
	// 寻找图片资源
	iImageResource* img = m_images;
	while (img)
	{
		if (lstrcmpiW(img->m_name, name)==0)
		{
			return img->m_texture.p;
			//iImageAnimController* anim = img->getDefaultAnimation();
			//return img->getTexture(anim ? anim->m_currentFrame : 0);
		}

		img = img->Next();
	}

	return NULL;
}

iModelBase* iDevice::find_model( LPCOLESTR name )
{
	if (name==NULL) return NULL;

	iModelBase* m = m_models;
	while (m && lstrcmpiW(m->m_name, name)!=0) m = m->TList<iModelBase>::Next();
	return m;
}

iDevice* iDevice::get_active_device()
{
	if (gt.hwndActiveScript==NULL) return NULL;
	return (iDevice*)(Device*)::GetPropW(gt.hwndActiveScript, AtomString(atom_device));
}

void iDevice::on_story_started()
{
	if (m_story_timerid==-1)
	{
		m_story_timerid = ::SetMtTimer(m_story_timerid, (DWORD)(1000/m_fps), 0, (pfnTimerProc)&iDevice::story_callback, this);
	}
}

void iDevice::on_story_stopped()
{
	if (m_story_timerid!=-1 && m_stories && !m_stories->isRunning() && !isReseting && m_scenes==NULL)
	{
		m_story_timerid = (::KillMtTimer(m_story_timerid), -1);
	}
}

void CALLBACK iDevice::story_callback( LPVOID pData, DWORD dwId )
{
	iDevice* dev = (iDevice*)pData;
	if (dev==NULL || dev->m_story_timerid!=dwId) return;
	dev->render();
}

void iDevice::fire_event( VARIANT* callback, IDispatch* dispThis/*=NULL*/, VARIANT* pvarResult/*=NULL*/ )
{
	if (m_hscript==NULL) return;

	HWND h = d3d::SetActiveScriptWindow(m_hwnd);
	CScript(m_hscript).InvokeCallback(callback, dispThis, pvarResult);
	d3d::SetActiveScriptWindow(h);
}

//iNamedObject* iDevice::findNamedObject( LPCOLESTR name )
//{
//	iNamedObject* no = m_namedObjects;
//	while (no)
//	{
//		if (no->name.CompareNoCase(name) == 0) return no;
//		no = no->Next();
//	}
//	return NULL;
//}

//////////////////////////////////////////////////////////////////////////
iRenderTarget::iRenderTarget( ) : m_quad(NULL)
{
}

iRenderTarget::~iRenderTarget()
{
}

void iRenderTarget::on_reset()
{
	m_render_to_surface = NULL;
	m_depth_surface = NULL;
	m_rendertarget_surface = NULL;
	m_rendertarget = NULL;
}

void iRenderTarget::render()
{
	if (m_quad==NULL) return;

	CComResult hr;
	if (m_rendertarget_surface.p == NULL)
	{
		if (m_rendertarget.p == NULL)
		{
			CheckHResult hr = m_quad->m_device->m_device->CreateTexture(m_quad->m_width, m_quad->m_height, 0, D3DUSAGE_RENDERTARGET | D3DUSAGE_AUTOGENMIPMAP, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &m_rendertarget, NULL);
			if (FAILED(hr)) return;
		}
		CheckHResult hr = m_rendertarget->GetSurfaceLevel(0, &m_rendertarget_surface);
		if (FAILED(hr)) return;

		CheckHResult hr = m_quad->m_device->m_device->CreateDepthStencilSurface(m_quad->m_width, m_quad->m_height, D3DFMT_D16, g_d3d.mstype_d16, g_d3d.msquality_d16-1, TRUE, &m_depth_surface, NULL);
		if (FAILED(hr)) return;

		//CheckHResult hr = D3DXCreateRenderToSurface(m_quad->m_device->m_device, m_quad->m_width, m_quad->m_height, D3DFMT_A8R8G8B8, TRUE, D3DFMT_D16, &m_render_to_surface);
		//if (FAILED(hr)) return;
	}

	// 准备渲染目标环境
	iRenderTarget* rtOld = m_quad->m_device->m_runtime.set_target(this);
	IDirect3DDevice9* dev = m_quad->m_device->m_device.p;
	ATLASSERT(dev);

	D3DVIEWPORT9 vpOld;
	CheckHResult hr = dev->GetViewport(&vpOld);
	D3DXMATRIX mtOld;
	CheckHResult hr = dev->GetTransform(D3DTS_PROJECTION, &mtOld);

	CComPtr<IDirect3DSurface9> rtOldSurface, rtOldDepthSurface;
	CheckHResult hr = dev->GetRenderTarget(0, &rtOldSurface);
	CheckHResult hr = dev->GetDepthStencilSurface(&rtOldDepthSurface);
	CheckHResult hr = dev->SetRenderTarget(0, m_rendertarget_surface);
	CheckHResult hr = dev->SetDepthStencilSurface(m_depth_surface);
	CheckHResult hr = dev->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER /*| D3DCLEAR_STENCIL*/, D3DCOLOR_ARGB(0,0,0,0), 1.0f, 0);

	D3DVIEWPORT9 vp = {(DWORD)(0), (DWORD)(0),
		(DWORD)(m_quad->m_width), (DWORD)(m_quad->m_height),
		0.f, 1.f};
		//ZNear, ZFar};
	CheckHResult hr = dev->SetViewport(&vp); // 必须在SetRenderTarget之后设置

	//CheckHResult hr = m_render_to_surface->BeginScene(m_rendertarget_surface, &vp);
	//CheckHResult hr = dev->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER /*| D3DCLEAR_STENCIL*/, D3DCOLOR_ARGB(255,0,0,0), 1.0, 0);

	D3DXMATRIX tmp, matProj;
	D3DXMatrixScaling(&matProj, 1.0f, -1.0f, 1.0f);
#if 1
	D3DXMatrixTranslation(&tmp, -0.5f, +0.5f, 0.0f);
	matProj *= tmp;
#endif // _DEBUG
	D3DXMatrixOrthoOffCenterLH(&tmp, (float)vp.X, (float)(vp.X+vp.Width), -((float)(vp.Y+vp.Height)), -((float)vp.Y), vp.MinZ, vp.MaxZ);
	matProj *= tmp;
	CheckHResult hr = dev->SetTransform(D3DTS_PROJECTION, &matProj);

	render_content();

	//CheckHResult hr = m_render_to_surface->EndScene(D3DX_FILTER_NONE);

	CheckHResult hr = dev->SetDepthStencilSurface(rtOldDepthSurface);
	rtOldDepthSurface = NULL;
	CheckHResult hr = dev->SetRenderTarget(0, rtOldSurface);
	rtOldSurface = NULL;

	CheckHResult hr = dev->SetViewport(&vpOld);
	CheckHResult hr = dev->SetTransform(D3DTS_PROJECTION, &mtOld);

	m_quad->m_device->m_runtime.set_target(rtOld);
}

void iRenderTarget::render_content()
{
	if (m_quad) m_quad->render_content();
}

//////////////////////////////////////////////////////////////////////////
#if 0
//////////////////////////////////////////////////////////////////////////
iImageFrame::iImageFrame( iImageResource* owner ) : m_owner(owner)
		, m_msec(0)
{
	ATLASSERT(m_owner);
	addToList(&m_owner->m_frames);
}

iImageFrame::~iImageFrame()
{
	if (next) delete next;
}
#endif // 0

//////////////////////////////////////////////////////////////////////////
extern CDuiConnector gConnector;

iImageResource::iImageResource( iDevice* device, LPCOLESTR url, LPCOLESTR name/*=NULL*/ ) : m_device(device), m_name(name)
		, m_width(0)
		, m_height(0)
		, m_clip(NULL)
		, m_frameCount(0)
		, m_img(NULL)
		, m_msecs(NULL)
		, m_currentFrame(0)
		, m_imgGuid(GUID_NULL)
		, m_defAnim(NULL)
		, UrlResources<iImageResource>(NULL, name)
		//, m_frames(NULL)
{
	if (m_device==NULL || url==NULL || *url==0) return;

	addToList(&device->m_images);

	//isValid = TRUE;

	LoadUrl(url);
}

iImageResource::~iImageResource()
{
	if (m_defAnim) m_defAnim = ((delete m_defAnim), NULL);

	gConnector.Disconnect(NULL, this);
	removeFromList();

	//if (m_frames) m_frames = (delete m_frames, NULL);
	m_frameCount = 0;
	m_currentFrame = 0;
	if (m_msecs) m_msecs = (delete[] m_msecs, NULL);
	if (m_img) m_img = (delete m_img, NULL);
}

D3DFORMAT gdip_to_d3d(PixelFormat pf)
{
	switch (pf)
	{
	case PixelFormat16bppRGB555:	return D3DFMT_X1R5G5B5;
	case PixelFormat16bppRGB565:	return D3DFMT_R5G6B5;
	case PixelFormat16bppARGB1555:	return D3DFMT_A1R5G5B5;
	case PixelFormat24bppRGB:		return D3DFMT_R8G8B8;
	case PixelFormat32bppRGB:		return D3DFMT_X8R8G8B8;
	case PixelFormat32bppARGB:		return D3DFMT_A8R8G8B8;
	case PixelFormat4bppIndexed:
	case PixelFormat8bppIndexed:	return D3DFMT_P8;
	}
	return D3DFMT_A8R8G8B8;
}

void iImageResource::OnLoadStream(LPSTREAM pStream)
{
	STATSTG ss;
	CComResult hr = pStream->Stat(&ss, STATFLAG_NONAME);
	ULONG cb = (ULONG)ss.cbSize.QuadPart;
	LPBYTE pBuf = NEW BYTE[cb];
	ULONG cbRead = 0;
	hr = pStream->Read(pBuf, cb, &cbRead);
	if (SUCCEEDED(hr))
	{
		// 如果使用 D3DXCreateTextureFromFileInMemory，则创建的纹理是正方形，且长度为2的N次方
		//CheckHResult hr = D3DXCreateTextureFromFileInMemory(m_device->m_device, pBuf, cb, &m_texture);
		//if (m_texture)
		//{
		//	isLoaded = TRUE;

		//	D3DSURFACE_DESC sd;
		//	CheckHResult hr = m_texture->GetLevelDesc(0, &sd);
		//	m_width = (LONG)sd.Width;
		//	m_height = (LONG)sd.Height;
		//}
		//else // try loading Image of GDI+
		{
			m_img = Image::FromStream(pStream);
			if (m_img && m_img->GetLastStatus() == Ok)
			{
				isLoaded = TRUE;
				m_width = (LONG)m_img->GetWidth();
				m_height = (LONG)m_img->GetHeight();

				PixelFormat pf = m_img->GetPixelFormat();

				m_img->GetFrameDimensionsList(&m_imgGuid, 1);

				//UINT count = m_img->GetFrameDimensionsCount();
				//GUID* pguid = NEW GUID[count];
				//m_img->GetFrameDimensionsList(pguid, count);
				//m_imgGuid = *pguid;
				m_frameCount = m_img->GetFrameCount(&m_imgGuid);
				if (m_frameCount > 1)
				{
					m_msecs = NEW LONG[m_frameCount];

					UINT size = m_img->GetPropertyItemSize(PropertyTagFrameDelay);
					PropertyItem* propItem = (PropertyItem*)NEW BYTE[size];
					if (propItem)
					{
						m_img->GetPropertyItem(PropertyTagFrameDelay, size, propItem);
						for (UINT i=0; i<m_frameCount; i++)
							m_msecs[i] = ((long*)propItem->value)[i] * 10;

						delete[] propItem;
					}
				}

				hr = m_device->m_device->CreateTexture(m_width, m_height, 0, 0/*D3DUSAGE_DYNAMIC*/ | D3DUSAGE_AUTOGENMIPMAP, gdip_to_d3d(pf), D3DPOOL_MANAGED, &m_texture, NULL);
				if (FAILED(hr))
					CheckHResult hr = m_device->m_device->CreateTexture(m_width, m_height, 0, 0 | D3DUSAGE_AUTOGENMIPMAP, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, &m_texture, NULL);
				if (FAILED(hr))
					LineLog(L"!!!!! ===== 创建动画纹理失败！");
				getTexture(0);
			}
		}
	}

	delete[] pBuf;

	gConnector.Fire(NULL, this, cp_img_loaded/*, &cb*/);
}

IDirect3DTexture9* iImageResource::getTexture( ULONG index/*=0*/ )
{
	if (m_img && m_texture.p)
	{
		if ((index!=m_currentFrame && index<m_frameCount) ||
			(m_frameCount==1 && index==0))
		{
			CComPtr<IDirect3DSurface9> surf;
			HRESULT hr = m_texture->GetSurfaceLevel(0, &surf);
			HDC hdc = NULL;
			hr = surf->GetDC(&hdc);
			if (SUCCEEDED(hr))
			{
				m_img->SelectActiveFrame(&m_imgGuid, index);

				Graphics g(hdc);
				//g.SetCompositingMode(CompositingModeSourceCopy);
				//g.SetPageUnit(UnitPixel);
				//Color c(Color::MakeARGB(0, 255,255,255/*0xed, 0xee, 0xef*/));
				//SolidBrush br(c);
				//g.FillRectangle(&br, 0, 0, m_width, m_height);
				g.DrawImage(m_img, 0, 0, m_width, m_height);
				surf->ReleaseDC(hdc);
				m_currentFrame = index;
			}
		}

		if (m_frameCount==1)
			m_img = (delete m_img, NULL);
	}
	return m_texture.p;
}

iImageAnimController* iImageResource::getDefaultAnimation()
{
	if (isLoaded && m_frameCount>1 && m_defAnim==NULL)
	{
		m_defAnim = NEW iImageAnimController(this);
		if (m_defAnim)
		{
			if (isLoaded)
				m_defAnim->play();
			else
				gConnector.Connect(m_defAnim, this, cp_img_loaded);
		}
	}
	return m_defAnim;
}

long iImageResource::getClipWidth()
{
	if (isLoaded && m_clip)
		return max(0, m_width - max(0,m_clip->left) - max(0,m_clip->right));
	return m_width;
}

long iImageResource::getClipHeight()
{
	if (isLoaded && m_clip)
		return max(0, m_height - max(0,m_clip->top) - max(0,m_clip->bottom));
	return m_height;
}

float iImageResource::uLeft()
{
	if (isLoaded && m_clip && m_width>0)
		return min(1.0f, (float)max(0,m_clip->left) / (float)m_width);
	return 0.0f;
}
float iImageResource::uRight()
{
	if (isLoaded && m_clip && m_width>0)
		return 1.0f - min(1.0f, (float)max(0,m_clip->right) / (float)m_width);
	return 1.0f;
}
float iImageResource::vTop()
{
	if (isLoaded && m_clip && m_height>0)
		return min(1.0f, (float)max(0,m_clip->top) / (float)m_height);
	return 0.0f;
}
float iImageResource::vBottom()
{
	if (isLoaded && m_clip && m_height>0)
		return 1.0f - min(1.0f, (float)max(0,m_clip->bottom) / (float)m_height);
	return 1.0f;
}
long iImageResource::intLeft()
{
	return (isLoaded && m_clip) ? m_clip->left : 0;
}
long iImageResource::intRight()
{
	return (isLoaded && m_clip) ? m_clip->right : 0;
}
long iImageResource::intTop()
{
	return (isLoaded && m_clip) ? m_clip->top : 0;
}
long iImageResource::intBottom()
{
	return (isLoaded && m_clip) ? m_clip->bottom : 0;
}

//////////////////////////////////////////////////////////////////////////

iImageAnimController::iImageAnimController( iImageResource* img/*=NULL*/ ) : m_image(img), m_timerid(-1), m_currentFrame(0)
{

}

iImageAnimController::~iImageAnimController()
{
	gConnector.Disconnect(this);
	if (m_timerid!=-1) m_timerid = (::KillMtTimer(m_timerid), -1);
	m_image = NULL;
}

BOOL iImageAnimController::OnHit( LPVOID pKey, LPVOID pVal, DWORD flag )
{
	if ((flag & cp_img_loaded))
	{
		gConnector.Disconnect(this);
		iImageAnimController* i = (iImageAnimController*)pKey;
		ATLASSERT(i==this);
		if (m_image == pVal)
		{
			play();
		}
	}
	return FALSE;
}

void iImageAnimController::play()
{
	if (m_image==NULL) return;

	m_currentFrame = 0;
	if (m_image->m_img && m_image->m_frameCount>1 && m_image->m_msecs[m_currentFrame]>0)
	{
		m_timerid = ::SetMtTimer(m_timerid, m_image->m_msecs[m_currentFrame], 1, iImageAnimController::TimerProc, this);
	}
}

void CALLBACK iImageAnimController::TimerProc( LPVOID pData, DWORD dwId )
{
	iImageAnimController* pThis = (iImageAnimController*)pData;
	if (pThis->m_timerid != dwId || pThis->m_image==NULL || pThis->m_image->m_device->isRelayout) return;

	pThis->m_currentFrame ++;
	if (pThis->m_currentFrame >= pThis->m_image->m_frameCount)
		pThis->m_currentFrame = 0;
	pThis->m_timerid = ::SetMtTimer(pThis->m_timerid, pThis->m_image->m_msecs[pThis->m_currentFrame], 1, iImageAnimController::TimerProc, pThis);
	pThis->m_image->m_device->invalidate();
}

//////////////////////////////////////////////////////////////////////////
iImageLayer::iImageLayer( iQuadObject& owner ) : m_owner(owner)
		//, m_vertexs(NULL)
		, m_ilt(ilt_normal)
		, m_number(0)
		, m_isLoaded(false)
		, m_isValid(false)
{
	/*
	 *	顶点顺序，用于九宫格。如果是简单的矩形，则直接用前4个顶点即可
			4	5	6	7
			15	0	1	8
			14	3	2	9
			13	12	11	10
	 */
	ZeroMemory(m_vertexs, 0);

	m_vertexs[4].u = m_vertexs[15].u = m_vertexs[14].u = m_vertexs[13].u = 0.0f;
	m_vertexs[7].u = m_vertexs[8].u = m_vertexs[9].u = m_vertexs[10].u = 1.0f;
	m_vertexs[4].v = m_vertexs[5].v = m_vertexs[6].v = m_vertexs[7].v = 0.0f;
	m_vertexs[13].v = m_vertexs[12].v = m_vertexs[11].v = m_vertexs[10].v = 1.0f;

	for (int i=0; i<16; i++)
		m_vertexs[i].z = DefaultZ;

	_setPrimiteCount(true, 2);
}

iImageLayer::~iImageLayer()
{
	//_setPrimiteCount(false, 0);
}

void iImageLayer::_setPrimiteCount( bool bCopy/*=false*/, UINT num/*=2*/ )
{
	if (num == m_number)
	{
		if (num>=2 && bCopy)
		{
			memcpy(m_vertexs, m_owner.m_vertexs, 4*sizeof(VERTEX));
			if (m_image)
			{
				m_vertexs[5].u = m_vertexs[0].u = m_vertexs[3].u = m_vertexs[12].u = m_image->uLeft();
				m_vertexs[6].u = m_vertexs[1].u = m_vertexs[2].u = m_vertexs[11].u = m_image->uRight();
				m_vertexs[15].v = m_vertexs[0].v = m_vertexs[1].v = m_vertexs[8].v = m_image->vTop();
				m_vertexs[14].v = m_vertexs[3].v = m_vertexs[2].v = m_vertexs[9].v = m_image->vBottom();

				m_vertexs[4].x = m_vertexs[15].x = m_vertexs[14].x = m_vertexs[13].x = (float)(m_owner.m_left) - AdjustXY;
				m_vertexs[7].x = m_vertexs[8].x = m_vertexs[9].x = m_vertexs[10].x = (float)(m_owner.m_left + m_owner.m_width) - AdjustXY;
				m_vertexs[5].x = /*m_vertexs[0].x = m_vertexs[3].x =*/ m_vertexs[12].x = (float)(m_owner.m_left + m_image->intLeft()) - AdjustXY;
				m_vertexs[6].x = /*m_vertexs[1].x = m_vertexs[2].x =*/ m_vertexs[11].x = (float)(m_owner.m_left + m_owner.m_width - m_image->intRight()) - AdjustXY;

				m_vertexs[4].y = m_vertexs[5].y = m_vertexs[6].y = m_vertexs[7].y = (float)(m_owner.m_top) - AdjustXY;
				m_vertexs[13].y = m_vertexs[12].y = m_vertexs[11].y = m_vertexs[10].y = (float)(m_owner.m_top + m_owner.m_height) - AdjustXY;
				m_vertexs[15].y = /*m_vertexs[0].y = m_vertexs[1].y =*/ m_vertexs[8].y = (float)(m_owner.m_top + m_image->intTop()) - AdjustXY;
				m_vertexs[14].y = /*m_vertexs[3].y = m_vertexs[2].y =*/ m_vertexs[9].y = (float)(m_owner.m_top + m_owner.m_height - m_image->intBottom()) - AdjustXY;
			}
		}
		return;
	}

	m_number = num;
	//if (m_vertexs)
	//	m_vertexs = ((delete[] m_vertexs), NULL);

	if (m_number>0)
	{
		//m_vertexs = NEW VERTEX[m_number+2];
		//if (m_vertexs==NULL) m_number = 0;
		if (m_number>=2 && bCopy)
		{
			memcpy(m_vertexs, m_owner.m_vertexs, 4*sizeof(VERTEX));
			if (m_image)
			{
				m_vertexs[5].u = m_vertexs[0].u = m_vertexs[3].u = m_vertexs[12].u = m_image->uLeft();
				m_vertexs[6].u = m_vertexs[1].u = m_vertexs[2].u = m_vertexs[11].u = m_image->uRight();
				m_vertexs[15].v = m_vertexs[0].v = m_vertexs[1].v = m_vertexs[8].v = m_image->vTop();
				m_vertexs[14].v = m_vertexs[3].v = m_vertexs[2].v = m_vertexs[9].v = m_image->vBottom();

				m_vertexs[4].x = m_vertexs[15].x = m_vertexs[14].x = m_vertexs[13].x = (float)(m_owner.m_left) - AdjustXY;
				m_vertexs[7].x = m_vertexs[8].x = m_vertexs[9].x = m_vertexs[10].x = (float)(m_owner.m_left + m_owner.m_width) - AdjustXY;
				m_vertexs[5].x = /*m_vertexs[0].x = m_vertexs[3].x =*/ m_vertexs[12].x = (float)(m_owner.m_left + m_image->intLeft()) - AdjustXY;
				m_vertexs[6].x = /*m_vertexs[1].x = m_vertexs[2].x =*/ m_vertexs[11].x = (float)(m_owner.m_left + m_owner.m_width - m_image->intRight()) - AdjustXY;

				m_vertexs[4].y = m_vertexs[5].y = m_vertexs[6].y = m_vertexs[7].y = (float)(m_owner.m_top) - AdjustXY;
				m_vertexs[13].y = m_vertexs[12].y = m_vertexs[11].y = m_vertexs[10].y = (float)(m_owner.m_top + m_owner.m_height) - AdjustXY;
				m_vertexs[15].y = /*m_vertexs[0].y = m_vertexs[1].y =*/ m_vertexs[8].y = (float)(m_owner.m_top + m_image->intTop()) - AdjustXY;
				m_vertexs[14].y = /*m_vertexs[3].y = m_vertexs[2].y =*/ m_vertexs[9].y = (float)(m_owner.m_top + m_owner.m_height - m_image->intBottom()) - AdjustXY;
			}
		}
	}
}

void iImageLayer::render()
{
	if (!m_isValid) return;

	// device 对象能自动记录设备状态的改变，并在析构时自动恢复原状态
	// 所有需记录的状态设置方法应该用 . 操作符调用，例如 device.SetFVF(...)
	// 所有无需记录的状态设置方法或者任何其它设备方法，应该用 -> 操作符调用，例如 device->SetFVF(...)
	AutoStateDevice device(m_owner.m_device->m_device);
	CComResult hr;

	if (m_owner.m_device->isLayered)
		CheckHResult hr = device.SetRenderState(D3DRS_BLENDOPALPHA, D3DBLENDOP_MAX);

	// render image
	if (m_image)
	{
		if (m_image->IsLoaded())
		{
			PixEvent pe(L"Image");
			_checkVertex();

#ifndef UseSystemEffect
			//CheckHResult hr = device.SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
			//CheckHResult hr = device.SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);
			CheckHResult hr = device.SetTexture(0, m_image->getTexture(m_currentFrame)); // m_currentFrame ? m_currentFrame->m_texture.p : m_image->m_texture.p);

			CheckHResult hr = device->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_BLENDTEXTUREALPHA);
			CheckHResult hr = device->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
			CheckHResult hr = device->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_CURRENT);

			CheckHResult hr = device->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
			CheckHResult hr = device->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
#else
			if (m_owner.m_device->m_system_effect.m_effect)
			{
				m_owner.m_device->m_system_effect.m_effect->SetTexture("g_tImage", m_image->getTexture(m_currentFrame));
				m_owner.m_device->m_system_effect.set_gray(!m_owner.isEnable);
			}
			BeginSystemEffect(m_owner.m_device, "ImageLayer");
#endif // UseSystemEffect
			
			if (m_image->m_clip && m_ilt==ilt_squares)
			{
				static const WORD wIndex[] =
				{
					0,1,3,	3,1,2,		4,5,15,	15,5,0,		5,6,0,	0,6,1,
					6,7,1,	1,7,8,		1,8,2,	2,8,9,		2,9,11,	11,9,10,
					3,2,12,	12,2,11,	14,3,13,13,3,12,	15,0,14,14,0,3
				};

				CheckHResult hr = device->DrawIndexedPrimitiveUP(D3DPT_TRIANGLELIST, 0, 16, 18, &wIndex[0], D3DFMT_INDEX16, 
					m_owner.m_device->update_target_vertex(m_vertexs, 16), sizeof(VERTEX));
			}
			else
				CheckHResult hr = device->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, m_number, m_owner.m_device->update_target_vertex(m_vertexs), sizeof(VERTEX));


#ifdef UseSystemEffect
			EndEffect();
#endif // UseSystemEffect
		}
	}

	// fill color
	else // m_image==NULL
	{
		PixEvent pe(L"ColorFill");
		_checkVertex();
#ifndef UseSystemEffect
		CheckHResult hr = device->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_BLENDTEXTUREALPHA);
		//CheckHResult hr = device->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
		CheckHResult hr = device->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_DIFFUSE);
		CheckHResult hr = device->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_CURRENT);
		CheckHResult hr = device->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
		CheckHResult hr = device->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE);
#else
		m_owner.m_device->m_system_effect.set_gray(!m_owner.isEnable);
		BeginSystemEffect(m_owner.m_device, "ColorFill");
#endif // UseSystemEffect
		
		CheckHResult hr = device->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, m_number, m_owner.m_device->update_target_vertex(m_vertexs), sizeof(VERTEX));
		
#ifdef UseSystemEffect
		EndEffect();
#endif // UseSystemEffect
	}
}

void iImageLayer::play()
{
	if (m_image==NULL) return;
	m_isLoaded = false;
	iImageAnimController::play();
	m_owner.m_device->invalidate();
}

void iImageLayer::_checkVertex()
{
	if (m_isValid /*&& !m_isLoaded*/)
	{
		_setPrimiteCount(true); // copy iQuadObject::m_vertexs to here
		if (m_image==NULL)
			m_isLoaded = true;

		else
		if (m_image && m_image->IsLoaded())
		{
			m_isLoaded = true;

			// update vertex
			if (m_ilt == ilt_stretch || (m_ilt==ilt_squares && m_image->m_clip==NULL))
			{
				m_vertexs[0].u = m_vertexs[3].u = m_image->uLeft();		// 0.0f;
				m_vertexs[1].u = m_vertexs[2].u = m_image->uRight();	// 1.0f;
				m_vertexs[0].v = m_vertexs[1].v = m_image->vTop();		// 0.0f;
				m_vertexs[2].v = m_vertexs[3].v = m_image->vBottom();	// 1.0f;
			}
			else if (m_ilt == ilt_maxfit)
			{
				m_vertexs[0].u = m_vertexs[3].u = m_image->uLeft();		// 0.0f;
				m_vertexs[1].u = m_vertexs[2].u = m_image->uRight();	// 1.0f;
				m_vertexs[0].v = m_vertexs[1].v = m_image->vTop();		// 0.0f;
				m_vertexs[2].v = m_vertexs[3].v = m_image->vBottom();	// 1.0f;

				bool vert = ((float)m_owner.m_width/(float)m_owner.m_height > (float)m_image->getClipWidth()/(float)m_image->getClipHeight()); // true:左右留空; false:上下留空
				if (vert)
				{
					LONG cx = (m_owner.m_width - (m_owner.m_height * m_image->getClipWidth() / m_image->getClipHeight())) / 2;
					m_vertexs[0].x = m_vertexs[3].x = ((float)(m_owner.m_left + cx)) - AdjustXY;
					m_vertexs[1].x = m_vertexs[2].x = ((float)(m_owner.m_left + m_owner.m_width - cx)) - AdjustXY;
				}
				else
				{
					LONG cy = (m_owner.m_height - (m_owner.m_width * m_image->getClipHeight() / m_image->getClipWidth())) / 2;
					m_vertexs[0].y = m_vertexs[1].y = ((float)(m_owner.m_top + cy)) - AdjustXY;
					m_vertexs[2].y = m_vertexs[3].y = ((float)(m_owner.m_top + m_owner.m_height - cy)) - AdjustXY;
				}
			}
			else if (m_ilt == ilt_tile) // TILE 模式下，图片CLIP失效
			{
				m_vertexs[0].u = m_vertexs[3].u = 0.0f;
				m_vertexs[1].u = m_vertexs[2].u = (float)m_owner.m_width / (float)m_image->m_width;
				m_vertexs[0].v = m_vertexs[1].v = 0.0f;
				m_vertexs[2].v = m_vertexs[3].v = (float)m_owner.m_height / (float)m_image->m_height;
			}
			else if (m_ilt == ilt_center)
			{
				LONG cx = (m_owner.m_width - m_image->getClipWidth()) / 2;
				LONG cy = (m_owner.m_height - m_image->getClipHeight()) / 2;

				m_vertexs[0].x = m_vertexs[3].x = ((float)(m_owner.m_left + max(0, cx))) - AdjustXY;
				m_vertexs[1].x = m_vertexs[2].x = ((float)(m_owner.m_left + m_owner.m_width - max(0, cx))) - AdjustXY;

				m_vertexs[0].y = m_vertexs[1].y = ((float)(m_owner.m_top + max(0, cy))) - AdjustXY;
				m_vertexs[2].y = m_vertexs[3].y = ((float)(m_owner.m_top + m_owner.m_height - max(0, cy))) - AdjustXY;

				m_vertexs[0].u = m_vertexs[3].u = 
					cx>=0 ? m_image->uLeft()/*0.0f*/ : ((float)(m_image->intLeft()-cx)/(float)m_image->m_width);
				m_vertexs[1].u = m_vertexs[2].u = 
					cx>=0 ? m_image->uRight()/*1.0f*/ : (1.0f - (float)(m_image->intRight()-cx)/(float)m_image->m_width);

				m_vertexs[0].v = m_vertexs[1].v = 
					cy>=0 ? m_image->vTop() : ((float)(m_image->intTop()-cy)/(float)m_image->m_height);
				m_vertexs[2].v = m_vertexs[3].v = 
					cy>=0 ? m_image->vBottom() : (1.0f - (float)(m_image->intBottom()-cy)/(float)m_image->m_height);
			}
			else if (m_ilt==ilt_squares && m_image->m_clip) // 九宫格模式
			{
				m_vertexs[0].x = m_vertexs[3].x = ((float)(m_owner.m_left + m_image->intLeft())) - AdjustXY;
				m_vertexs[1].x = m_vertexs[2].x = ((float)(m_owner.m_left + m_owner.m_width - m_image->intRight())) - AdjustXY;

				m_vertexs[0].y = m_vertexs[1].y = ((float)(m_owner.m_top + m_image->intTop())) - AdjustXY;
				m_vertexs[2].y = m_vertexs[3].y = ((float)(m_owner.m_top + m_owner.m_height - m_image->intBottom())) - AdjustXY;

				m_vertexs[0].u = m_vertexs[3].u = m_image->uLeft();		// 0.0f;
				m_vertexs[1].u = m_vertexs[2].u = m_image->uRight();	// 1.0f;
				m_vertexs[0].v = m_vertexs[1].v = m_image->vTop();		// 0.0f;
				m_vertexs[2].v = m_vertexs[3].v = m_image->vBottom();	// 1.0f;
			}
			else // m_ilt == ilt_normal or other
			{
				m_vertexs[1].x = m_vertexs[2].x = ((float)(m_owner.m_left + (LONG)min(m_owner.m_width,m_image->getClipWidth()))) - AdjustXY;

				m_vertexs[2].y = m_vertexs[3].y = ((float)(m_owner.m_top + (LONG)min(m_owner.m_height,m_image->getClipHeight()))) - AdjustXY;

				m_vertexs[0].u = m_vertexs[3].u = m_image->uLeft();
				m_vertexs[1].u = m_vertexs[2].u = 
					m_owner.m_width<m_image->getClipWidth() ? (float)(m_image->intLeft()+m_owner.m_width) / (float)m_image->m_width : m_image->uRight();

				m_vertexs[0].v = m_vertexs[1].v = m_image->vTop();
				m_vertexs[2].v = m_vertexs[3].v = 
					m_owner.m_height<m_image->getClipHeight() ? (float)(m_image->intTop()+m_owner.m_height) / (float)m_image->m_height : m_image->vBottom();
			}
		}
	}
}

//////////////////////////////////////////////////////////////////////////
iQuadObject::iQuadObject( iDevice* device, LONG x, LONG y, LONG width, LONG height, iQuadObject* parent/*=NULL*/ )
		: iSurfaceBase(device, x, y, width, height)
		, TOrderList<iQuadObject>(&iQuadObject::zindex)
		, TList<iQuadObject, 1>(&iQuadObject::m_children)
		//, m_device(device)
		, m_rendertarget(NULL)
		, m_scene(NULL)
		, m_name(NULL)
		, m_gdi(device, width, height)
		, m_clip(this)
		, _cbpaint(NULL)
		, m_exts(NULL)
		, m_parent(parent)
		, m_children(NULL)
		, m_ordered_children(NULL)
		, m_background(NULL)
		, m_foregrounds(NULL)
		, m_effect(NULL)
		, m_zindex(0)
{
	::SetRectEmpty(&m_clipChildren);
	if (m_device==NULL || m_width<=0 || m_height<=0) return;

	isValid = TRUE;

	TOrderList<iQuadObject>::addToList(m_parent ? &m_parent->m_children : &m_device->m_textures, false);
}

iQuadObject::~iQuadObject()
{
	if (m_background) m_background = ((delete m_background), NULL);
	if (m_foregrounds) m_foregrounds = ((delete m_foregrounds), NULL);
	if (m_rendertarget) m_rendertarget = ((delete m_rendertarget), NULL);

	//if (isValid)
	{
		TOrderList<iQuadObject>::removeFromList(false/*, m_parent ? m_parent->m_children : m_device->m_textures*/);
	}
}

void iQuadObject::render_content()
{
	CComResult hr;

	{
		// 记录所有更新的状态，析构时自动恢复到原始状态
		AutoStateDevice state(m_device->m_device);
		//CheckHResult hr = state.SetFVF(D3DFVF_VERTEX);

		//CComPtr<IDirect3DBaseTexture9> oldTex;
		//if (m_clip.m_texture)
		//	m_device->m_system_effect.set_clip_texture(m_clip.m_texture, &oldTex);
		m_device->m_system_effect.set_gray(!isEnable);

		if (m_scene && m_scene->Usage==SU_ALL)
			m_scene->render();
		else
		{
			// rendering background image or filling color
			if (m_scene && m_scene->Usage==SU_BACKGROUND)
				m_scene->render();
			else if (m_background)
			{
				PixEvent pe(L"QuadBackground");
				m_background->render();
			}

			if (m_scene && m_scene->Usage==SU_PRECONTENT)
				m_scene->render();

			// render GDI texture and children
			if (m_scene && m_scene->Usage==SU_CONTENT)
				m_scene->render();
			else
			{
				// GDI paint
				if (m_gdi.m_texture && m_device->m_system_effect.m_effect)
				{
					PixEvent pe(L"QuadGDI");
					m_device->m_system_effect.m_effect->SetTexture("g_tImage", m_gdi.m_texture.p);
					BeginSystemEffect(m_device, "ImageLayer");
					CheckHResult hr = m_device->m_device.p->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 2, m_device->update_target_vertex(m_vertexs), sizeof(VERTEX));
					EndEffect();
				}

				// render normal children
				if (!::IsRectEmpty(&m_clipChildren))
				{
					PixEvent pe(L"QuadChildren");
					// 设置视口和裁剪面时，必须重新计算(正交)投影矩阵，否则图元位置和大小会发生变化
					D3DVIEWPORT9 vp = {(DWORD)(m_clipChildren.left-m_device->current_target_orig_x()), (DWORD)(m_clipChildren.top-m_device->current_target_orig_y()),
						(DWORD)(m_clipChildren.right-m_clipChildren.left), (DWORD)(m_clipChildren.bottom-m_clipChildren.top),
						0.f, 1.f};
					CheckHResult hr = state.SetViewport(&vp);

					D3DXMATRIX tmp, matProj;
					D3DXMatrixScaling(&matProj, 1.0f, -1.0f, 1.0f);
					D3DXMatrixTranslation(&tmp, -0.5f, +0.5f, 0.0f);
					matProj *= tmp;
					D3DXMatrixOrthoOffCenterLH(&tmp, (float)vp.X, (float)(vp.X+vp.Width), -((float)(vp.Y+vp.Height)), -((float)vp.Y), vp.MinZ, vp.MaxZ);
					matProj *= tmp;
					CheckHResult hr = state.SetTransform(D3DTS_PROJECTION, &matProj);

					//// 画子元素之前要恢复旧的裁剪纹理
					//if (m_clip.m_texture)
					//	m_device->m_system_effect.set_clip_texture(oldTex);

					render_children();

					//if (m_clip.m_texture)
					//	m_device->m_system_effect.set_clip_texture(m_clip.m_texture);

					// render children 之后必须重新设置一次灰度，因为子控件可能会修改这个值
					m_device->m_system_effect.set_gray(!isEnable);
				}
			}

			if (m_scene && m_scene->Usage==SU_POSTCONTENT)
				m_scene->render();

			// render foreground images
			if (m_scene && m_scene->Usage==SU_FOREGROUND)
				m_scene->render();
			else if (m_foregrounds)
			{
				PixEvent pe(L"QuadForeground");
				iImageLayer* img = m_foregrounds;
				while (img)
				{
					img->render();
					img = img->next;
				}
			}
		}

		//if (m_clip.m_texture)
		//	m_device->m_system_effect.set_clip_texture(oldTex);
	}

	// 如果状态为 Disable，则覆盖一层灰色层
	//if (!isEnable)
	//{
	//	PixEvent pe(L"QuadDisabled");
	//	BeginSystemEffect(m_device, "DisabledLayer");
	//	CheckHResult hr = m_device->m_device.p->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 2, m_device->update_target_vertex(m_vertexs), sizeof(VERTEX));
	//	EndEffect();
	//}
}

void iQuadObject::render_children()
{
	// render negative z-index children
	if (m_ordered_children)
		m_ordered_children->ordered_render(false);

	// render caret
	if (m_device->m_caret.owner == this)
		m_device->m_caret.render();

	// render children
	if (m_children)
		m_children->render();
	//iQuadObject* child = m_children;
	//while (child)
	//{
	//	child->render();
	//	child = child->next;
	//}

	// render positive z-index children
	if (m_ordered_children)
		m_ordered_children->ordered_render(true);
}

void iQuadObject::pre_render()
{
	m_clip.get_texture();
	if (m_gdi.m_dirty && _cbpaint)
		_cbpaint->Do3DPaint();

	if (m_ordered_children) m_ordered_children->ordered_render(false, true);
	if (m_children) m_children->pre_render();
	if (m_ordered_children) m_ordered_children->ordered_render(true, true);

	if (next) next->pre_render();

	//if (has_target())
	//{
	//	m_rendertarget->render();
	//}
}

void iQuadObject::render(bool bNoNext/*=false*/)
{
	if (!isValid) return;

	// 这个调用来自于场景渲染 device->render()，直接绘制图元
	if (m_device->isRendering)
	{
		if (!isVisible) return;

		CComResult hr;
		// 设置渲染目标
		if (has_target())
		{
			PixEvent pe(L"Quad");
			m_rendertarget->render();
			// restore and render rendertarget

#if 0
			// for test
			CheckHResult hr = D3DXSaveTextureToFileW(L"d:\\output.png", D3DXIFF_PNG, m_rendertarget->texture(), NULL);
			// end test
#endif // 0

			AutoStateDevice device(m_device->m_device);
			if (m_device->isLayered)
				CheckHResult hr = device.SetRenderState(D3DRS_BLENDOPALPHA, D3DBLENDOP_MAX);

			CComPtr<IDirect3DBaseTexture9> oldTex;
			if (m_clip.get_texture())
				m_device->m_system_effect.set_clip_texture(m_clip.get_texture(), &oldTex);

			if (m_device->m_system_effect.m_effect)
			{
				m_device->m_system_effect.m_effect->SetTexture("g_tImage", m_rendertarget->texture());
				//if (!isEnable) m_device->m_system_effect.set_gray(0.f);
			}
			BeginSystemEffect(m_device, "ImageLayer");
			//CheckHResult hr = device.SetFVF(D3DFVF_VERTEX);
			CheckHResult hr = m_device->m_device->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 2, m_device->update_target_vertex(m_vertexs), sizeof(VERTEX));
			EndEffect();

			if (m_clip.get_texture())
				m_device->m_system_effect.set_clip_texture(oldTex);
		}
		else
			render_content();

		if (next && !bNoNext)
			next->render();
	}
}

void iQuadObject::ordered_render(bool positive, bool ispre/*=false*/)
{
	if ((positive && zindex()>=0) || (!positive && zindex()<0))
	{
		if (ispre)
			pre_render();
		else
			render(true);
	}
	if (next)
		next->ordered_render(positive, ispre);
}

bool iQuadObject::isParent( iQuadObject* parent )
{
	iQuadObject* p = m_parent;
	while (p && p!=parent) p = p->m_parent;
	return (p && p==parent);
}

void iQuadObject::resize( LONG width, LONG height )
{
	if (width==m_width && height==m_height) return;
	isValid = FALSE;

	if (width<=0 || height<=0) return;

	if (isTarget && m_rendertarget)
	{
		m_rendertarget->on_reset();
	}

	iSurfaceBase::resize(width, height);
	m_gdi.resize(width, height);

	if (m_background) m_background->m_isLoaded = false;
	iImageLayer* il = m_foregrounds;
	while (il)
	{
		il->m_isLoaded = false;
		il = il->next;
	}

	isValid = TRUE;
}

void iQuadObject::move( LONG x, LONG y, LONG width, LONG height )
{
	if (x==m_left && y==m_top && width==m_width && height==m_height) return;
	isValid = FALSE;

	if (width<=0 || height<=0) return;

	if (isTarget && m_rendertarget)
	{
		m_rendertarget->on_reset();
	}

	iSurfaceBase::move(x, y, width, height);
	m_gdi.resize((ULONG)width, (ULONG)height);

	if (m_background) m_background->m_isLoaded = false;
	iImageLayer* il = m_foregrounds;
	while (il)
	{
		il->m_isLoaded = false;
		il = il->next;
	}

	isValid = TRUE;
}

void iQuadObject::on_reset()
{
	if (m_children) m_children->on_reset();
	if (next) next->on_reset();
	if (m_ordered_children) m_ordered_children->on_reset();

	if (m_rendertarget) m_rendertarget->on_reset();
	if (m_scene) m_scene->m_target.on_reset();
	m_gdi.m_texture = NULL;
	m_gdi.m_surface = NULL;
	m_clip.resize();
	//m_clip.m_texture = NULL;
	//m_clip.m_surface = NULL;
}

void iQuadObject::set_as_target( BOOL bTarget )
{
	if (/*m_scene ||*/ m_clip.get_texture()) return; // 有场景情况下设置目标属性无效
	if (isTarget != bTarget) // changed
	{
		isTarget = bTarget;
		if (!isTarget && m_rendertarget)
		{
			m_rendertarget = ((delete m_rendertarget), NULL);
		}
	}
}

bool iQuadObject::has_target()
{
	if (!isTarget && m_clip.get_texture()==NULL)
	{
		if (m_rendertarget) m_rendertarget = ((delete m_rendertarget), NULL);
		return false;
	}
	if (m_rendertarget==NULL)
	{
		m_rendertarget = NEW iRenderTarget();
		if (m_rendertarget==NULL) return false;
		m_rendertarget->m_quad = this;
	}
	return true;
}

void iQuadObject::set_scene( iScene* scene )
{
	if (scene == m_scene) return;

	// 删除旧场景
	if (m_scene)
	{
		m_scene->m_target.on_reset();
		if (m_scene->m_target.m_quad && m_scene->m_target.m_quad->m_exts)
			m_scene->m_target.m_quad->m_exts->DetachExtension(m_scene);
		m_scene->m_target.m_quad = NULL;
		m_scene = NULL;
	}

	// 应用新场景
	m_scene = scene;

	if (m_scene)
	{
		m_scene->m_target.m_quad = this;
		if (m_exts)
			m_exts->AttachExtension(m_scene);
	}
}

//////////////////////////////////////////////////////////////////////////
//
iCaret::iCaret( iDevice* device ) : dev(device)
		, x(0), y(0), cx(0), cy(0)
		, owner(NULL)
		, bValid(FALSE), bShow(FALSE), bShowing(FALSE)
{

}

iCaret::~iCaret()
{

}

void iCaret::render()
{
	if (/*!dev->isLayered ||*/ cx<=0 || cy<=0 || !bValid || !bShow || !bShowing)
		return;

	VERTEX verts[4] = {
		{(float)x - AdjustXY,		(float)y - AdjustXY,		DefaultZ,
#ifdef USE_RHW
				1.0f,
#endif // USE_RHW
				D3DCOLOR_XRGB(0,0,0),	0.0f,	0.0f},
		{(float)(x+cx) - AdjustXY,	(float)y - AdjustXY,		DefaultZ,
#ifdef USE_RHW
				1.0f,
#endif // USE_RHW
				D3DCOLOR_XRGB(0,0,0),	1.0f,	0.0f},
		{(float)(x+cx) - AdjustXY,	(float)(y+cy) - AdjustXY,	DefaultZ,
#ifdef USE_RHW
				1.0f,
#endif // USE_RHW
				D3DCOLOR_XRGB(0,0,0),	1.0f,	1.0f},
		{(float)x - AdjustXY,		(float)(y+cy) - AdjustXY,	DefaultZ,
#ifdef USE_RHW
				1.0f,
#endif // USE_RHW
				D3DCOLOR_XRGB(0,0,0),	0.0f,	1.0f},
	};

	AutoStateDevice state(dev->m_device);
	CComResult hr;
	//CheckHResult hr = state.SetRenderState(D3DRS_SRCBLEND, D3DBLEND_INVDESTCOLOR);
	//CheckHResult hr = state.SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ZERO);
	//CheckHResult hr = state.SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD); // select the arithmetic operation applied when the alpha blending render state

	CheckHResult hr = state.SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
	CheckHResult hr = state.SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_DIFFUSE);
	CheckHResult hr = state.SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
	CheckHResult hr = state.SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE);

	CheckHResult hr = dev->m_device->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 2, dev->update_target_vertex(verts), sizeof(VERTEX));

	//CheckHResult hr = state.SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	//CheckHResult hr = state.SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	//CheckHResult hr = state.SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD); // select the arithmetic operation applied when the alpha blending render state
}

//////////////////////////////////////////////////////////////////////////
iControl::iControl( iDevice* device, HWND hwnd ) : m_device(device), m_hwnd(NULL), m_hdcOrig(NULL), m_hdcSurf(NULL), m_lockCount(0)
{
	if (m_device==NULL) return;
	if (::IsWindow(hwnd))
	{
		m_hwnd = hwnd;
		::SetPropW(m_hwnd, AtomString(atom_child_window), this);
		addToList(&m_device->m_controls);
		m_sz.cx = m_sz.cy = 0;
		resize();
	}
}

iControl::~iControl()
{
	if ((m_hwnd))
	{
		::RemovePropW(m_hwnd, AtomString(atom_child_window));
		removeFromList(/*m_device->m_controls*/);
	}
}

void iControl::render()
{
	//return;
	resize();
	if (m_sz.cx==0 || m_sz.cy==0 || /*m_texture==NULL ||*/ m_hwnd==NULL || !::IsWindowVisible(m_hwnd)) return;

	if (!isValid)
		update(NULL);
	CRect rc;
	::GetWindowRect(m_hwnd, &rc);
	CPoint pt = rc.TopLeft();
	//CPoint pt(0,0);
	//::ClientToScreen(m_hwnd, &pt);
	::ScreenToClient(m_device->m_hwnd, &pt);

	VERTEX verts[4] = {
		{(float)pt.x - AdjustXY,			(float)pt.y - AdjustXY,				DefaultZ,
#ifdef USE_RHW
				1.0f,
#endif // USE_RHW
				D3DCOLOR_XRGB(0,0,0),	0.0f,	0.0f},
		{(float)(pt.x+m_sz.cx) - AdjustXY,	(float)pt.y - AdjustXY,				DefaultZ,
#ifdef USE_RHW
				1.0f,
#endif // USE_RHW
				D3DCOLOR_XRGB(0,0,0),	1.0f,	0.0f},
		{(float)(pt.x+m_sz.cx) - AdjustXY,	(float)(pt.y+m_sz.cy) - AdjustXY,	DefaultZ,
#ifdef USE_RHW
				1.0f,
#endif // USE_RHW
				D3DCOLOR_XRGB(0,0,0),	1.0f,	1.0f},
		{(float)pt.x - AdjustXY,			(float)(pt.y+m_sz.cy) - AdjustXY,	DefaultZ,
#ifdef USE_RHW
				1.0f,
#endif // USE_RHW
				D3DCOLOR_XRGB(0,0,0),	0.0f,	1.0f},
	};

	AutoStateDevice state(m_device->m_device);
	CComResult hr;
	CheckHResult hr = state.SetTexture(0, m_texture.p);

	CheckHResult hr = state.SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
	CheckHResult hr = state.SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	CheckHResult hr = state.SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
	CheckHResult hr = state.SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE);

	CheckHResult hr = m_device->m_device->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 2, m_device->update_target_vertex(verts), sizeof(VERTEX));
}

void iControl::on_reset()
{
	isValid = FALSE;
	m_surface = NULL;
	m_texture = NULL;
}

void iControl::resize()
{
	CRect rc;
	//::GetClientRect(m_hwnd, &rc);
	::GetWindowRect(m_hwnd, &rc);
	if (rc.Width()==m_sz.cx && rc.Height()==m_sz.cy)
		return;

	m_sz = rc.Size();
	on_reset();
}

void iControl::update(HDC hdc)
{
	//if (hdc==NULL) return;

	HDC hdcSurf = getDC();
	if (hdcSurf)
	{
		//WCHAR buf[256] = L"";
		//::GetClassNameW(m_hwnd, buf, 255);
		//LineLog((LPCWSTR)buf);
		// 
		//::RedrawWindow(m_hwnd, NULL, NULL, RDW_ERASE | RDW_INVALIDATE | RDW_FRAME | RDW_ALLCHILDREN | RDW_UPDATENOW);
		//gbDisableComposition = TRUE;
		// 

		//::SendMessage(m_hwnd, WM_PRINT, (WPARAM)hdcSurf, (LPARAM)(PRF_CHECKVISIBLE|PRF_NONCLIENT|PRF_CLIENT|PRF_ERASEBKGND|PRF_CHILDREN|PRF_OWNED));

		//::SendMessage(m_hwnd, WM_PRINTCLIENT, (WPARAM)hdcSurf, (LPARAM)(PRF_CHECKVISIBLE|PRF_NONCLIENT|PRF_CLIENT|PRF_ERASEBKGND|PRF_CHILDREN|PRF_OWNED));
		//gbDisableComposition = FALSE;
		//::PrintWindow(m_hwnd, hdcSurf, PW_CLIENTONLY);
		////CClientDC dc(m_hwnd);
		HDC hdcSrc = (HDC)::GetPropW(m_hwnd, AtomString(3333));
		if (hdcSrc)
			::BitBlt(hdcSurf, 0, 0, m_sz.cx, m_sz.cy, hdcSrc, 0, 0, SRCCOPY);
		//CWindowDC dc(m_hwnd);
		//::BitBlt(hdcSurf, 0, 0, m_sz.cx, m_sz.cy, dc, 0, 0, SRCCOPY);
		//:: 

		//CDuiWindowBase* pwin = CDuiWindowBase::FromHWND(m_hwnd, FALSE);
		//if (pwin)
		//{
		//	pwin->DrawToDC32(hdcSurf, CRect(0,0,0,0), FALSE);
		//}
		releaseDC(hdcSurf);
		//m_device->render();
		isValid = TRUE;

#if 0
		HRESULT hr = D3DXSaveTextureToFileW(L"d:\\output.png", D3DXIFF_PNG, m_texture.p, NULL);
		hr = S_OK;
#endif
	}
}

iControl* iControl::find( HWND hWnd )
{
	iControl* c = this;
	while (c && c->m_hwnd!=hWnd) c = c->next;
	return c ? c : NULL;
}

HDC iControl::getDC()
{
	if (m_hdcSurf) return m_lockCount++, m_hdcSurf;

	CComResult hr = S_OK;
	if (m_surface==NULL)
	{
		if (m_texture==NULL)
		{
			CheckHResult hr = m_device->m_device->CreateTexture(m_sz.cx, m_sz.cy, 0, 0/*D3DUSAGE_DYNAMIC*/ | D3DUSAGE_AUTOGENMIPMAP, D3DFMT_X8R8G8B8, D3DPOOL_MANAGED, &m_texture, NULL);
			if (FAILED(hr)) return NULL;
		}
		CheckHResult hr = m_texture->GetSurfaceLevel(0, &m_surface);
		if (FAILED(hr)) return NULL;
	}

	CheckHResult hr = m_surface->GetDC(&m_hdcSurf);
	//::SelectClipRgn(m_hdcSurf, NULL);
	if (m_hdcSurf) m_lockCount = 1;
	return m_hdcSurf;
}

void iControl::releaseDC( HDC hdc )
{
	if (m_lockCount==0 || m_surface==NULL || hdc!=m_hdcSurf || m_hdcSurf==NULL) return;

	if (--m_lockCount == 0)
	{
		m_surface->ReleaseDC(hdc);
		m_hdcSurf = NULL;
		m_device->invalidate();
	}
}

void iControl::invalidate()
{
	isValid = FALSE;
	m_device->invalidate();
}

//#ifdef UseSystemEffect
//////////////////////////////////////////////////////////////////////////
iSystemEffect::iSystemEffect(iDevice* device) : iEffect(device)
		, gray_factor(NULL)
		, hClip(NULL)
{
	HRSRC hResource = ::FindResourceW( _AtlBaseModule.GetResourceInstance(), L"effect", L"system" );
	ATLASSERT(hResource);

	HGLOBAL hGlobal = ::LoadResource( _AtlBaseModule.GetResourceInstance(), hResource );
	ATLASSERT(hGlobal);

	DWORD dwSize = ::SizeofResource(_AtlBaseModule.GetResourceInstance(), hResource);
	m_data.Append((LPCSTR)::LockResource(hGlobal), (int)dwSize);
}

void iSystemEffect::init()
{
	if (loadEffect(m_data))
	{
		isValid = TRUE;
		isLoaded = TRUE;
	}
}

void iSystemEffect::set_gray( BOOL bGray )
{
	if (m_effect)
		m_effect->SetFloat(gray_factor, bGray ? 0.f : 1.f);
}

void iSystemEffect::onloaded()
{
	gray_factor = m_effect->GetParameterByName(NULL, "gray_factor");
	hClip = m_effect->GetParameterByName(NULL, "g_tClip");
}

void iSystemEffect::set_device_params()
{
	if (!_create()) return;
	if (m_effect==NULL) return;

	m_effect->SetFloat(m_effect->GetParameterByName(NULL, "g_fTime"), m_device->current_time());
	m_effect->SetFloat(m_effect->GetParameterByName(NULL, "g_fDelta"), m_device->elapsed_time());
}

void iSystemEffect::set_scene_params()
{
	if (!_create()) return;
	if (m_effect==NULL || m_device->m_runtime.scene==NULL) return;

	m_effect->SetMatrix(m_effect->GetParameterByName(NULL, "g_mView"), &m_device->m_runtime.scene->m_camera.get_view_matrix());
	m_effect->SetMatrix(m_effect->GetParameterByName(NULL, "g_mProjection"), &m_device->m_runtime.scene->m_camera.get_projection_matrix());
	m_effect->SetValue(m_effect->GetParameterByName(NULL, "g_vCameraPosition"), m_device->m_runtime.scene->m_camera.m_Position, sizeof(D3DXVECTOR3));
	//D3DXVECTOR4 v;
	//m_effect->SetVector(m_effect->GetParameterByName(NULL, "g_vCameraPosition"),
	//	D3DXVec3Transform(&v,&m_device->m_runtime.scene->m_camera.m_Position,&m_device->m_runtime.scene->get_world_matrix()), sizeof(D3DXVECTOR4));
}

void iSystemEffect::set_scene_mirror_params()
{
	if (!_create()) return;
	if (m_effect==NULL || m_device->m_runtime.scene==NULL) return;

	m_effect->SetMatrix(m_effect->GetParameterByName(NULL, "g_mMirrorWorld"), &m_device->m_runtime.scene->get_world_matrix());
}

void iSystemEffect::set_model_params(CONST D3DXMATRIX* mtWorld)
{
	if (!_create()) return;
	if (m_effect==NULL /*|| m_device->m_runtime.model==NULL*/) return;

	D3DXMATRIX world = * mtWorld * m_device->m_runtime.scene->get_world_matrix();
	m_effect->SetMatrix(m_effect->GetParameterByName(NULL, "g_mWorld"), &world);
	D3DXMATRIX mtWVP =	world
						* m_device->m_runtime.scene->m_camera.get_view_matrix()
						* m_device->m_runtime.scene->m_camera.get_projection_matrix();
	m_effect->SetMatrix(m_effect->GetParameterByName(NULL, "g_mWorldViewProjection"), &mtWVP);
}

void iSystemEffect::set_texture_params()
{
	if (!_create()) return;
	//if (m_effect==NULL) return;
	//D3DXHANDLE hAmbient = m_effect->GetParameterBySemantic( 0, "Ambient" );
	//D3DXHANDLE hDiffuse = m_effect->GetParameterBySemantic( 0, "Diffuse" );
	//D3DXHANDLE hSpecular = m_effect->GetParameterBySemantic( 0, "Specular" );
	////D3DXHANDLE hOpacity = m_effect->GetParameterBySemantic( 0, "Opacity" );
	//D3DXHANDLE hSpecularPower = m_effect->GetParameterBySemantic( 0, "SpecularPower" );

	//if (m_device->m_runtime.material)
	//{
	//	//if (hAmbient) hr2 = m_effect->SetValue(hAmbient, &m_device->m_runtime.material->material.Ambient, sizeof(D3DXVECTOR3));
	//	//if (hDiffuse) hr2 = m_effect->SetValue(hDiffuse, &m_device->m_runtime.material->material.Diffuse, sizeof(D3DXVECTOR3));
	//	//if (hSpecular) hr2 = m_effect->SetValue(hSpecular, &m_device->m_runtime.material->material.Specular, sizeof(D3DXVECTOR3));
	//	//if (hSpecularPower) hr2 = m_effect->SetInt(hSpecularPower, (int)m_device->m_runtime.material->material.Power);
	//}

	m_effect->SetTexture(m_effect->GetParameterByName(NULL, "g_MeshTexture"), m_device->m_runtime.texture);
	//m_effect->SetBool(m_effect->GetParameterByName(NULL, "g_hasTexture"), m_device->m_runtime.texture!=NULL);
}

void iSystemEffect::set_clip_texture( IDirect3DBaseTexture9* pTexture, IDirect3DBaseTexture9** ppOldTexture )
{
	if (!_create()) return;

	if (ppOldTexture)
		m_effect->GetTexture(hClip, ppOldTexture);

	if (pTexture==NULL) pTexture = m_def_clip_texture.p;
	//m_device->m_device->SetTexture(1, pTexture);
	HRESULT hr = m_effect->SetTexture(hClip, pTexture);
	ATLASSERT(SUCCEEDED(hr));
}

bool iSystemEffect::on_reset()
{
	//m_def_clip_texture = NULL;
	bool b = iEffect::on_reset(false);
	return b;
}

bool iSystemEffect::_create()
{
#ifdef _DEBUG
	if (m_effect==NULL && m_hr==S_OK)
	{
		DWORD dwShaderFlags = D3DXFX_NOT_CLONEABLE | D3DXSHADER_DEBUG | D3DXSHADER_SKIPOPTIMIZATION;

		CComPtr<ID3DXBuffer> err;
		WCHAR cfile[MAX_PATH];
		::GetModuleFileNameW(NULL, cfile, MAX_PATH);
		CPathW path(cfile);
		path.RemoveFileSpec();
		path.Append(L"\\..\\DirectUI\\system.fx");
		m_hr = D3DXCreateEffectFromFileW(m_device->m_device.p, path, m_device->getMacro(), m_device->getInclude(), dwShaderFlags, m_device->m_effectPool.p, &m_effect, &err);
		if (SUCCEEDED(m_hr))
		{
			_cacheParams();
			onloaded();
		}
		else if (err)
		{
			LPCSTR msg = (LPCSTR)err->GetBufferPointer();
			ATLTRACE("\n=====system effect compiler error=======\n%s\n", msg);
			::MessageBoxA(m_device->Handle(), msg, "System effect compiling error", MB_OK | MB_ICONERROR);
		}
		else
		{
			static bool first = true;
			if (first)
			{
				first = false;
				::MessageBoxW(m_device->Handle(), L"Failed to create system effect!", L"Error", MB_OK | MB_ICONERROR);
			}
		}
	}
	if (FAILED(m_hr)) return false;
#else
	if (!iEffect::_create()) return false;
#endif // _DEBUG

	if (m_def_clip_texture==NULL)
	{
		HRESULT hr = m_device->m_device->CreateTexture(1,1,1,0,D3DFMT_A8R8G8B8,D3DPOOL_MANAGED,&m_def_clip_texture,NULL);
		if (SUCCEEDED(hr))
		{
			//CComPtr<IDirect3DSurface9> surf;
			//hr = m_def_clip_texture->GetSurfaceLevel(0, &surf);
			//hr = m_device->m_device->ColorFill(surf, NULL, 0);
			D3DLOCKED_RECT lock;
			hr = m_def_clip_texture->LockRect(0, &lock, NULL, D3DLOCK_DISCARD);
			if (SUCCEEDED(hr))
			{
				//BYTE* p = (BYTE*)lock.pBits;
				DWORD* p = (DWORD*)lock.pBits;
				*p = 0; // set alpha=0
				m_def_clip_texture->UnlockRect(0);
			}
		}
	}
	//set_clip_texture(m_def_clip_texture,NULL);

	return true;
}
//#endif // UseSystemEffect

//////////////////////////////////////////////////////////////////////////
void iEffect::EffectParam::Save( ID3DXEffect* e )
{
	ATLASSERT(e);
	HRESULT hr = S_OK;
	saved = true;
	switch (desc.Type)
	{
	case D3DXPT_BOOL:
		if (desc.Elements>0) hr = e->GetBoolArray(handle, saved_value.pbVal, desc.Elements);
		else hr = e->GetBool(handle, saved_value.pbVal);
		break;
	case D3DXPT_INT:
		if (desc.Elements>0) hr = e->GetIntArray(handle, saved_value.piVal, desc.Elements);
		else hr = e->GetInt(handle, saved_value.piVal);
		break;
	case D3DXPT_FLOAT:
		if (desc.Rows>1)
		{
			if (desc.Elements>0) hr = e->GetMatrixArray(handle, saved_value.pmVal, desc.Elements);
			else hr = e->GetMatrix(handle, saved_value.pmVal);
		}
		else if (desc.Columns>1)
		{
			if (desc.Elements>0) hr = e->GetVectorArray(handle, saved_value.pvVal, desc.Elements);
			else hr = e->GetVector(handle, saved_value.pvVal);
		}
		else //if (desc.Rows==1 && desc.Columns==1)
		{
			if (desc.Elements>0) hr = e->GetFloatArray(handle, saved_value.pfVal, desc.Elements);
			else hr = e->GetFloat(handle, saved_value.pfVal);
		}
		break;
	}
}

HRESULT iEffect::EffectParam::SetToEffect( ID3DXEffect* e, EffectParamValue* pValue )
{
	ATLASSERT(e && pValue);
	HRESULT hr = S_OK;
	Save(e); // 缓存旧值
	switch (desc.Type)
	{
	case D3DXPT_BOOL:
		if (desc.Elements>0) hr = e->SetBoolArray(handle, pValue->pbVal, pValue->nElements);
		else hr = e->SetBool(handle, *pValue->pbVal);
		break;
	case D3DXPT_INT:
		if (desc.Elements>0) hr = e->SetIntArray(handle, pValue->piVal, pValue->nElements);
		else hr = e->SetInt(handle, *pValue->piVal);
		break;
	case D3DXPT_FLOAT:
		if (desc.Rows>1)
		{
			if (desc.Elements>0) hr = e->SetMatrixArray(handle, pValue->pmVal, pValue->nElements);
			else hr = e->SetMatrix(handle, pValue->pmVal);
		}
		else if (desc.Columns>1)
		{
			if (desc.Elements>0) hr = e->SetVectorArray(handle, pValue->pvVal, pValue->nElements);
			else hr = e->SetVector(handle, pValue->pvVal);
		}
		else //if (desc.Rows==1 && desc.Columns==1)
		{
			if (desc.Elements>0) hr = e->SetFloatArray(handle, pValue->pfVal, pValue->nElements);
			else hr = e->SetFloat(handle, *pValue->pfVal);
		}
		break;
	}
	return hr;
}

void iEffect::EffectParam::Restore( ID3DXEffect* e )
{
	if (saved)
	{
		saved = false;

		ATLASSERT(e);
		HRESULT hr = S_OK;
		switch (desc.Type)
		{
		case D3DXPT_BOOL:
			if (desc.Elements>0) hr = e->SetBoolArray(handle, saved_value.pbVal, desc.Elements);
			else hr = e->SetBool(handle, *saved_value.pbVal);
			break;
		case D3DXPT_INT:
			if (desc.Elements>0) hr = e->SetIntArray(handle, saved_value.piVal, desc.Elements);
			else hr = e->SetInt(handle, *saved_value.piVal);
			break;
		case D3DXPT_FLOAT:
			if (desc.Rows>1)
			{
				if (desc.Elements>0) hr = e->SetMatrixArray(handle, saved_value.pmVal, desc.Elements);
				else hr = e->SetMatrix(handle, saved_value.pmVal);
			}
			else if (desc.Columns>1)
			{
				if (desc.Elements>0) hr = e->SetVectorArray(handle, saved_value.pvVal, desc.Elements);
				else hr = e->SetVector(handle, saved_value.pvVal);
			}
			else //if (desc.Rows==1 && desc.Columns==1)
			{
				if (desc.Elements>0) hr = e->SetFloatArray(handle, saved_value.pfVal, desc.Elements);
				else hr = e->SetFloat(handle, *saved_value.pfVal);
			}
			break;
		}
	}
}

//////////////////////////////////////////////////////////////////////////
iEffect::iEffect( iDevice* device, LPCOLESTR url/*=NULL*/, LPCOLESTR name/*=NULL*/, LPCOLESTR data/*=NULL*/, UINT length/*=0*/ ) : m_device(device)
		//, m_asd(device->m_device)
		, isValid(FALSE)
		, m_current_technique(NULL)
		, m_data(data)
		, m_hr(S_OK)
		, m_old_effect(NULL)
		, m_param_block(NULL)
{
	if (m_device==NULL || ((url==NULL || *url==0) && m_data.IsEmpty())) return;
	addToList(&m_device->m_effects);
	if (!m_data.IsEmpty() && length>0)
	{
		CStringA sdata(data, length);
		m_data = sdata;
	}

	isValid = TRUE;
	isLoaded = FALSE;

	if (url && *url!=0)
		LoadUrl(url);
	else
		isLoaded = !!loadEffect(/*"technique ___avoid_compile_error { pass _0{} }\n\n" +*/ m_data);
}

iEffect::~iEffect()
{
	//removeFromList();
	clearParamBlock();
}

void iEffect::OnLoadData(LPCSTR pData, ULONG cbSize)
{
	CStringA strBuf = pData;
	isLoaded = !!loadEffect(strBuf + "\n\n" + m_data);
}

bool iEffect::loadEffect( LPCSTR data, UINT length/*=0*/ )
{
	m_final_data = data;
	return on_reset(false);
}

iEffect* iEffect::findTechnique( iEffect* start, D3DXHANDLE technique )
{
	if (start==NULL || technique==NULL) return NULL;

	if (start->m_effect && SUCCEEDED(start->m_hr) && SUCCEEDED(start->m_effect->ValidateTechnique(technique)))
		return start;

	return findTechnique(start->Next(), technique);
}

iEffect* iEffect::findTechnique(iEffect* start, LPCOLESTR technique)
{
	if (technique==NULL) return NULL;
	return findTechnique(start, CStringA(technique));
}

void iEffect::updateTechParams()
{
	//if (m_param_block == NULL)
	{
		HRESULT hr;
		//m_effect->BeginParameterBlock();
		for (int i=0; i<m_params_cache.GetSize(); i++)
		{
			D3DXPARAMETER_DESC& desc = m_params_cache[i].desc;
			D3DXHANDLE h = m_params_cache[i].handle;
			EffectParamValue* pValue;
			if ((pValue = get_param_value(h)) &&
				m_effect->IsParameterUsed(h, m_current_technique))
			{
				hr = m_params_cache[i].SetToEffect(m_effect.p, pValue);
				//hr = m_effect->SetValue(h, pValue->pBuffer, pValue->nBytes);
			}
		}
		//m_param_block = m_effect->EndParameterBlock();
	}

	//if (m_param_block)
	//	m_effect->ApplyParameterBlock(m_param_block);
}

void iEffect::_cacheParams(EffectParam* parent/*=NULL*/)
{
	if (parent==NULL)
	{
		D3DXEFFECT_DESC ed;
		m_effect->GetDesc(&ed);
		for (UINT i=0; i<ed.Parameters; i++)
		{
			D3DXHANDLE param = m_effect->GetParameter(NULL, i);
			EffectParam ep;
			ep.handle = param;
			m_effect->GetParameterDesc(param, &ep.desc);
			ep.name = ep.desc.Name;
			ep.semantic = ep.desc.Semantic;
			if (ep.desc.StructMembers > 0) // 结构体变量无需缓冲，但要缓冲其成员
				_cacheParams(&ep);
			else if (m_params_cache.Add(ep))
			{
				EffectParam& epp = m_params_cache[m_params_cache.GetSize()-1];
				epp.saved_value.set_size(&epp.desc);
			}
		}
	}
	else
	{
		for (UINT i=0; i<parent->desc.StructMembers; i++)
		{
			D3DXHANDLE param = m_effect->GetParameter(parent->handle, i);
			EffectParam ep;
			ep.handle = param;
			m_effect->GetParameterDesc(param, &ep.desc);
			ep.name = parent->name;
			ep.name.Append(L".");
			ep.name.Append(ep.desc.Name);
			ep.semantic = ep.desc.Semantic;
			if (m_params_cache.Add(ep))
			{
				EffectParam& epp = m_params_cache[m_params_cache.GetSize()-1];
				epp.saved_value.set_size(&epp.desc);
			}
		}
	}
}

bool iEffect::setTechnique( LPCSTR technique )
{
	if (/*m_effect.p==NULL && m_hr==S_OK &&*/ !_create()) return false;
	if (m_effect.p == NULL) return false;
	D3DXHANDLE tech;
	if (technique)
		tech = m_effect->GetTechniqueByName(technique);
	else
		m_effect->FindNextValidTechnique(NULL, &tech);
	if (tech==NULL) return false;

#if 0
	D3DXHANDLE hPass = m_effect->GetPass(tech, 0);
	D3DXPASS_DESC pd;
	m_effect->GetPassDesc(hPass, &pd);
#endif

	if (tech==m_current_technique) return true;
	CComResult hr = m_effect->SetTechnique(tech);
	if (SUCCEEDED(hr))
	{
		m_current_technique = tech;
	}
	return SUCCEEDED(hr);
}

UINT iEffect::begin( LPCSTR technique )
{
	if (!setTechnique(technique)) return 0;

	m_old_effect = m_device->m_runtime.set_effect(this);
	updateTechParams();
	UINT numPasses = 0;
	CComResult hr = m_effect->Begin(&numPasses, 0);
	return numPasses;
}

UINT iEffect::begin(LPCOLESTR technique)
{
	if (technique==NULL) return 0;
	return begin(CStringA(technique));
}

void iEffect::end()
{
	if (m_effect)
		m_effect->End();

	for (int i=0; i<m_params_cache.GetSize(); i++) m_params_cache[i].Restore(m_effect.p);
	//clearParamBlock();

	m_device->m_runtime.set_effect(m_old_effect);
	m_old_effect = NULL;
}

bool iEffect::beginPass( UINT pass )
{
	if (m_effect == NULL) return false;
	return SUCCEEDED(m_effect->BeginPass(pass));
}

void iEffect::endPass()
{
	if (m_effect)
		m_effect->EndPass();
}

bool iEffect::on_reset(bool bNext/*=true*/)
{
	if (m_effect)
		m_effect->OnResetDevice();
	m_effect = NULL;
	m_hr = S_OK;
	m_params_cache.RemoveAll();

	//if (m_effect) hr = m_effect->OnResetDevice();
	if (bNext && Next()) Next()->on_reset(bNext);

	return SUCCEEDED(m_hr);
}

bool iEffect::_create()
{
	if (m_effect==NULL && m_hr==S_OK)
	{
		DWORD dwShaderFlags = D3DXFX_NOT_CLONEABLE
#ifdef _DEBUG
			| D3DXSHADER_DEBUG
			| D3DXSHADER_SKIPOPTIMIZATION
			//| D3DXSHADER_FORCE_VS_SOFTWARE_NOOPT // 这两个标志不能使用，否则效果不会启动着色器
			//| D3DXSHADER_FORCE_PS_SOFTWARE_NOOPT
#endif // _DEBUG
			| 0;

		CComPtr<ID3DXBuffer> err;
		m_hr = D3DXCreateEffect(m_device->m_device.p, m_final_data, m_final_data.GetLength(), m_device->getMacro(), m_device->getInclude(), dwShaderFlags, m_device->m_effectPool.p, &m_effect, &err);
		if (SUCCEEDED(m_hr))
		{
			_cacheParams();
			onloaded();
			//CComPtr<ID3DXEffectStateManager> esm;
			//hr = m_effect->GetStateManager(&esm);
			//hr = m_effect->SetStateManager(NULL);
			//ATLASSERT("hello");
			//m_effect->SetStateManager(&m_asd);
		}
		else if (err)
		{
			LPCSTR msg = (LPCSTR)err->GetBufferPointer();
			ATLTRACE("\n=====effect compiler error=======\n%s\n", msg);
			::MessageBoxA(m_device->Handle(), msg, "Effect compiling error", MB_OK | MB_ICONERROR);
		}
		else
		{
			static bool first = true;
			if (first)
			{
				first = false;
				::MessageBoxW(m_device->Handle(), L"Failed to create effect!", L"Error", MB_OK | MB_ICONERROR);
			}
		}
	}
	return SUCCEEDED(m_hr);
}

iEffect::EffectParam* iEffect::get_param_desc( D3DXHANDLE h, long* pIndex/*=NULL*/ )
{
	if (m_effect==NULL) return NULL;

	for (int i=0; i<m_params_cache.GetSize(); i++)
	{
		if (m_params_cache[i].handle==h)
		{
			if (pIndex) *pIndex = i;
			return &m_params_cache[i];
		}
	}
	return NULL;
}

D3DXHANDLE iEffect::get_param_handle( LPCOLESTR name, long* pIndex/*=NULL*/ )
{
	if (m_effect==NULL) return NULL;

	for (int i=0; i<m_params_cache.GetSize(); i++)
	{
		// 可以直接取变量名称，例如 g_mWorld，或者取语义名称，但需要加前缀([$&%#]中任意一个符号)，例如 $World
		if (lstrcmpiW(name, m_params_cache[i].name)==0 ||
			((*name==L'$' || *name==L'&' || *name==L'%' || *name==L'#') && lstrcmpiW(name+1, m_params_cache[i].semantic)==0))
		{
			if (pIndex) *pIndex = i;
			return m_params_cache[i].handle;
		}
	}
	// 如果没有变量名称，允许寻找不带前缀的语义名称
	if (!(*name==L'$' || *name==L'&' || *name==L'%' || *name==L'#'))
	{
		for (int i=0; i<m_params_cache.GetSize(); i++)
		{
			// 可以直接取变量名称，例如 g_mWorld，或者取语义名称，但需要加前缀([$&%#]中任意一个符号)，例如 $World
			if (lstrcmpiW(name, m_params_cache[i].semantic)==0)
			{
				if (pIndex) *pIndex = i;
				return m_params_cache[i].handle;
			}
		}
	}
	return NULL;
}

EffectParamValue* iEffect::get_param_value( D3DXHANDLE h, bool bForceGet/*=false*/, bool bForPut/*=false*/ )
{
	if (m_effect==NULL) return NULL;

	EffectParamValue* v = NULL;
	bool bNormal = (!bForPut && !bForceGet);
	if ((bNormal || m_current_operator>=operator_entity) && m_device->m_runtime.entity && (v=m_device->m_runtime.entity->effect_params.get_param_value(this, h, bForPut)))
		return v;
	if ((bNormal || m_current_operator>=operator_model) && m_device->m_runtime.model && (v=m_device->m_runtime.model->effect_params.get_param_value(this, h, bForPut)))
		return v;
	if ((bNormal || m_current_operator>=operator_material) && m_device->m_runtime.material && (v=m_device->m_runtime.material->effect_params.get_param_value(this, h, bForPut)))
		return v;

	if (bForceGet)
	{
		for (int i=0; i<m_params_cache.GetSize(); i++)
		{
			if (m_params_cache[i].handle == h)
			{
				m_params_cache[i].Save(m_effect.p);
				return &m_params_cache[i].saved_value;
			}
		}
	}
	return NULL;
}

void iEffect::clearParamBlock()
{
	if (m_effect && m_param_block)
		m_effect->DeleteParameterBlock(m_param_block);
	m_param_block = NULL;
}
#pragma warning(pop)
