#include "StdAfx.h"
#include "dxt.h"
#include "transform/dxbounds.h"
#include "transform/dxvector.h"
//#include "transform/dxhelper.h"

//#pragma comment(lib, "ddraw.lib")

//////////////////////////////////////////////////////////////////////////
// test multithread timer
#include "mttimer.h"


class ClipDC
{
public:
	ClipDC(HDC hdc, RECT rc) : hDC(hdc), rcItem(rc)
	{
		POINT pt = {0};
		::GetWindowOrgEx(hDC, &pt);

		RECT rcClip = { 0 };
		::GetClipBox(hDC, &rcClip);
		::OffsetRect(&rcClip, -pt.x, -pt.y);
		hOldRgn = ::CreateRectRgnIndirect(&rcClip);

		::OffsetRect(&rcItem, -pt.x, -pt.y);
		hRgn = ::CreateRectRgnIndirect(&rcItem);
		::ExtSelectClipRgn(hDC, hRgn, RGN_AND);
	}
	~ClipDC()
	{
		if (hDC) ::SelectClipRgn(hDC, hOldRgn);
		if (hOldRgn) ::DeleteObject(hOldRgn);
		if (hRgn) ::DeleteObject(hRgn);
	}
	RECT rcItem;
	HDC hDC;
	HRGN hRgn;
	HRGN hOldRgn;
};


BOOL SaveBitmapToFile(HBITMAP hBitmap, LPCWSTR lpFileName);

#define default_input_pixelformat DDPF_RGB32

const DWORD clr_parent = 0x00badbad; // 对于不支持ALPHA的变换，ALPHA通道值一定要设置成0，否则GDI操作不正常

const struct {const GUID* formatId; BOOL bAlpha; DWORD keyColor;} g_formatId[] =
{ 
	{&DDPF_PMARGB32, TRUE, 0},
	{&DDPF_ARGB32, TRUE, 0},
	{&DDPF_RGB32, FALSE, clr_parent},
	{&DDPF_RGB24, FALSE, clr_parent},
	{&DDPF_ARGB4444, TRUE, 0},
	{&DDPF_ARGB1555, TRUE, 0},
	{&DDPF_RGB565, FALSE, 0x00000bad},
	{&DDPF_RGB555, FALSE, 0x00000bad},
	{&DDPF_RGB8, FALSE, 0x00000bad},
};


void UpdateAlpha(IDXSurface* surf, COLORREF clrTransparent=clr_parent)
{
	if (surf==NULL) return;

	// 所有透明色部分的ALPHA设为0，其它设为255
	CComPtr<IDXARGBReadWritePtr> ptr;
	HRESULT hr = surf->LockSurface( NULL, INFINITE, DXLOCKF_READWRITE, 
		IID_IDXARGBReadWritePtr, (void **)&ptr, NULL);
	if( SUCCEEDED(hr) )
	{
		//ptr->FillRect(NULL, 0xffbadbad, FALSE);
		DXNATIVETYPEINFO nti;
		DXSAMPLEFORMATENUM fmtEnum = ptr->GetNativeType(&nti);
		CDXDBnds bnds;
		hr = surf->GetBounds(&bnds);
		bnds.SetToSize();
		SIZE sz;
		bnds.GetXYSize(sz);
		ptr->MoveToXY(0, 0);
		DXSAMPLE* pSample = ptr->Unpack(NULL, 0, FALSE);
		for (long y=0; y<sz.cy; y++)
		{
			for (long x=0; x<sz.cx; x++)
			{
				DXSAMPLE* p = pSample + y * sz.cx + x;
				if (RGB(p->Red, p->Green, p->Blue) == (clrTransparent&0x00ffffff)) p->Alpha = 0;
				//else /*if (p->Alpha==0)*/ p->Alpha = 255;
			}
		}

		ptr.Release();
	}
}

void InitBackground(IDXSurface* surf)
{
	if (surf==NULL) return;
	CComPtr<IDXARGBReadWritePtr> ptr;
	HRESULT hr = surf->LockSurface( NULL, INFINITE, DXLOCKF_READWRITE, 
		IID_IDXARGBReadWritePtr, (void **)&ptr, NULL);
	if( SUCCEEDED(hr) )
	{
		ptr->FillRect(NULL, clr_parent/*|0xff000000*/, FALSE);
		ptr.Release();
	}
}

//////////////////////////////////////////////////////////////////////////
//

BOOL DXT::_init()
{
	if (_tf.p == NULL)
	{
		HRESULT hr = _tf.CoCreateInstance(CLSID_DXTransformFactory, NULL, CLSCTX_INPROC);
		if (SUCCEEDED(hr))
			hr = _tf->QueryService(SID_SDXSurfaceFactory, IID_IDXSurfaceFactory, (void**)&_sf.p);
		if (FAILED(hr))
			return FALSE;
	}
	return _tf.p != NULL;
}

void DXT::_clear()
{
	if (_sf.p) _sf.Release();
	if (_tf.p) _tf.Release();

#ifdef __TEST_TASKMANAGER
	if (_tm.p) _tm.Release();
#endif // __TEST_TASKMANAGER

	//OnEndOfThread();
}

DXT& DXT::instance()
{
	static DXT _dxt;
	return _dxt;
}

void DXT::clear()
{
	DXT::instance()._clear();
}

BOOL DXT::parse_to_clsid( LPCOLESTR szName, CLSID& clsid, LPCOLESTR szPrev/*=NULL*/ )
{
	if (szName==NULL || szName[0]==0) return FALSE;

	HRESULT hr = ::CLSIDFromProgID(szName, &clsid);
	if (FAILED(hr))
		hr = ::CLSIDFromString((LPOLESTR)szName, &clsid);
	if (FAILED(hr) && szPrev)
	{
		OLECHAR szTotal[256];
		lstrcpyW(szTotal, szPrev);
		lstrcatW(szTotal, szName);
		hr = ::CLSIDFromProgID(szTotal, &clsid);
	}
	return SUCCEEDED(hr);
}

DXTransform* DXT::create_transform( LPCOLESTR szProgId, IPropertyBag* pInitProp/*=NULL*/ )
{
	CLSID clsid;
	if (!parse_to_clsid(szProgId, clsid, L"DXImageTransform.Microsoft."))
		return NULL;

	DXT& dxt = instance();
	if (!dxt._init()) return NULL;

	CComPtr<IDXTransform> t;
	HRESULT hr = dxt._tf->CreateTransform(NULL,0,NULL,0, pInitProp, NULL, clsid, IID_IDXTransform, (void**)&t.p);
	if (FAILED(hr)) return NULL;

	DXTransform* tf = NEW DXTransform(t.p, clsid);
	ATLTRACE(L"make a transform '%s'\n", szProgId);

	return tf;
}

DXSurface* DXT::create_surface( DXTransform& t, BOOL writable )
{
	DXT& dxt = instance();
	if (!dxt._init()) return NULL;

	DXSurface* ps = NEW DXSurface(t, writable);
	//if (ps == NULL) return NULL;
	//if (ps)
	//{
	//	CComPtr<IDXARGBReadWritePtr> pPtr;
	//	hr = ps->ptr()->LockSurface( NULL, INFINITE, DXLOCKF_READWRITE, IID_IDXARGBReadWritePtr, (void **)&pPtr, NULL);
	//	if (SUCCEEDED(hr))
	//	{
	//		pPtr->FillRect(NULL, (DXPMSAMPLE)(DWORD)0xff00ff00/*0xff000000*/, TRUE);
	//	}
	//}
	return ps;
}

DXTransform::DXTransform( IDXTransform* t, CLSID clsid ) : _t(t),
			_effect(t),
			_scale(t),
			Enabled(true),
			_outFormatIndex(2), // DDPF_RGB32
			_outFormat(DDPF_ARGB32),
			_clsid(clsid),
			_inDesired(2),
			_misc(0)
{
	HRESULT hr = S_OK;
	DWORD dwFlags = 0;
	ULONG cGuids = 10;
	GUID aguid[10] = {0};

	hr = _t->GetInOutInfo(TRUE, 0, &dwFlags, aguid, &cGuids, NULL);
	//if (hr==S_OK && cGuids>0) _outFormat = aguid[0];

	_inMin = _inMax = 0;
	cGuids = 10;
	hr = _t->GetInOutInfo(FALSE, 0, &dwFlags, aguid, &cGuids, NULL);
	if (hr == S_OK) // S_FALSE 表示不支持此表面
	{
		BOOL b = ((dwFlags & DXINOUTF_OPTIONAL)==0);
		_inMin += b;
		_inMax ++;
	}
	cGuids = 10;
	hr = _t->GetInOutInfo(FALSE, 1, &dwFlags, aguid, &cGuids, NULL);
	if (hr == S_OK)
	{
		BOOL b = ((dwFlags & DXINOUTF_OPTIONAL)==0);
		_inMin += b;
		_inMax ++;
	}
	_inDesired = min(_inMax, max(_inMin, _inDesired));
	ATLTRACE("transform needs %d - %d inputs\n", _inMin, _inMax);

	// get misc flags
	hr = _t->GetMiscFlags(&_misc);

	//// test Effect::Duration
	//if (_effect.p)
	//{
	//	float d;
	//	hr = _effect->get_Duration(&d);
	//}

	//// test Quality
	//if (_misc & DXTMF_QUALITY_SUPPORTED)
	//{
	//	float fQuality;
	//	hr = _t->GetQuality(&fQuality);
	//}
}

BOOL DXTransform::IsType( LPCOLESTR szProgId ) const
{
	CLSID clsid;
	if (!DXT::parse_to_clsid(szProgId, clsid, L"DXImageTransform.Microsoft."))
		return FALSE;
	return _clsid==clsid;
}

HRESULT DXTransform::_setup()
{
	IUnknown* ppIn[2] = {_runtime._in?_runtime._in->ptr():NULL, _runtime._in2?_runtime._in2->ptr():NULL};
	IUnknown* pOut = _runtime._out->ptr();
	HRESULT hr = _t->Setup(ppIn, ppIn[1]?2:(ppIn[0]?1:0), &pOut, 1, 0);
	return hr;
}

HRESULT DXTransform::_init_1st_surface(BOOL bCapture/*=TRUE*/, BOOL bSetup/*=FALSE*/)
{
	if (_inMin == 0) return S_FALSE;
	if (!_runtime._ctx.IsValid()) return E_FAIL;
	if (_runtime._started) return E_UNEXPECTED;

	_capture_background();
	if (_runtime._in == NULL)
	{
		_runtime._in = DXT::create_surface(*this, TRUE);
		if (_runtime._in == NULL) return E_OUTOFMEMORY;

		if (bCapture) _capture_to_surface(_runtime._in);
		if (bSetup) _setup();
	}
	return S_OK;
}

HRESULT DXTransform::_capture_to_surface(DXSurface* surf, DWORD dwState/* =DXT_DS_ALL */)
{
	if (!_runtime._ctx.IsValid()) return E_FAIL;
	if (surf == NULL) return E_POINTER;

	HDC hdc = surf->GetDC();
	if (_runtime._ctx._drawToDC) _runtime._ctx._drawToDC(hdc, _runtime._ctx._data, dwState);
	surf->ReleaseDC();
	return S_OK;
}

HRESULT DXTransform::_capture_background()
{
	//if (!_runtime._ctx.IsValid()) return E_FAIL;
	//if (_runtime._bk == NULL)
	//{
	//	_runtime._bk = DXT::create_surface(*this, TRUE);
	//	if (_runtime._bk == NULL) return E_OUTOFMEMORY;

	//	if (_runtime._ctx._drawBkToDC)
	//	{
	//		HDC hdc = _runtime._bk->GetDC();
	//		_runtime._ctx._drawBkToDC(hdc, _runtime._ctx._data, _runtime._ctx._rc);
	//		_runtime._bk->ReleaseDC();
	//	}
	//}
	return S_OK;
}

HRESULT DXTransform::_init_properties()
{
	CComQIPtr<IPersistPropertyBag> ppb = _t.p;
	HRESULT hr = S_OK;
	if (ppb.p && _runtime._ctx._props) hr = ppb->Load(_runtime._ctx._props, NULL);
	// test write
	//ppb->Save(_runtime._ctx._props, FALSE, TRUE);
	return hr;
}

void DXTransform::Init( DxtContext& ctx )
{
	if (!ctx.IsValid()) return;

	Stop();
	Safe_Delete(_runtime._out);
	Safe_Delete(_runtime._in);
	Safe_Delete(_runtime._in2);
	Safe_Delete(_runtime._bk);
	_runtime._ctx = ctx;

	IUnknown* pOut = NULL;
	if (_runtime._out == NULL)
	{
		_runtime._out = DXT::create_surface(*this, FALSE);
		if (_runtime._out == NULL) return ;//E_OUTOFMEMORY;
		pOut = _runtime._out->ptr();
	}
	_init_properties();
}

void DXTransform::Apply()
{
	if (_inDesired <= 1) return;
	//if (_runtime._in2) return ;//E_UNEXPECTED;

	if (_runtime._timerId != (DWORD)-1)
	{
		KillMtTimer(_runtime._timerId);
		_runtime._timerId = (DWORD)-1;
	}

	HRESULT hr = _init_1st_surface();
	if (FAILED(hr)) return ;//hr;

	if (_runtime._in2 == NULL)
	{
		_runtime._in2 = DXT::create_surface(*this, TRUE);
		if (_runtime._in2 == NULL) return ;//E_OUTOFMEMORY;
		_runtime._in2->GetDC();
	}
	return ;//S_OK;
}

void DXTransform::SetPercent(float percent)
{
	percent = min(1.0f, max(0.0f, percent));
	_runtime._percent = percent;
	if (_effect.p)
	{
		_effect->put_Progress(percent);
	}
	if (_t.p) _t->Execute(NULL, NULL, NULL);
}

#define dxt_period	100 // ms

void  DXTransform::Play( float fDuration /*=1.0f*/ )
{
	if (!Enabled || _inDesired<1) return;

	//float duration = 1.0f; // default 1 second.
	//CComVariant v;
	//if (SUCCEEDED(v.ChangeType(VT_R4, &vDuration)) && V_R4(&v)>0.1f)
	//{
	//	//_runtime._duration = V_R4(&v);
	//}
	if (_effect.p)
	{
		_effect->put_Duration(fDuration);
		_effect->get_Duration(&fDuration);
	}

	if (_runtime._in) _runtime._in->ReleaseDC();
	if (_runtime._in2) _runtime._in2->ReleaseDC();

	HRESULT hr = S_OK;
	if (_inMin>0 && _runtime._in==NULL)
	{
		hr = _init_1st_surface();
		if (FAILED(hr)) return ;//hr;
	}

	if (_runtime._in2==NULL) return;
	_capture_to_surface(_runtime._in2);
	_setup();

	SetPercent(0.0);
	// 启动定时器
	//if (_effect.p)
	//{
	//	//hr = _effect->put_Duration(_runtime._duration);
	//	//long cap = 0;
	//	//hr = _effect->get_Capabilities(&cap);
	//	//if (cap==0 || (cap==DXTET_MORPH && _inDesired==1)) return ;//E_FAIL;
	//	////float step;
	//	////hr = _effect->get_StepResolution(&step);
	//	// 先获取进度为 0.0 时的画面，否则在变换开始会出现一次黑屏
	//	hr = _effect->put_Progress(0.0f);
	//}
	////DXVEC tp = {DXBT_DISCRETE, 0};
	//hr = _t->Execute(NULL, NULL, NULL/*&tp*/);

	DWORD dwTotal = (DWORD)(fDuration * 1000 / dxt_period); // 如果 duration 为0，表示永久循环
	if (dwTotal != 0) dwTotal ++;
	_runtime._timerId = SetMtTimer(_runtime._timerId, dxt_period, dwTotal, &_timer_proc, this);
	if (_runtime._timerId == (DWORD)-1) return ;//E_FAIL;
	//ATLTRACE("定时器启动(%d)\n", _runtime._timerId);
	_runtime._started = TRUE;
}

void  DXTransform::Stop()
{
	if (_runtime._timerId != (DWORD)-1)
	{
		KillMtTimer(_runtime._timerId);
		_runtime._timerId = (DWORD)-1;
	}
	_runtime._started = FALSE;
	_runtime._percent = 0.0f;

	//Safe_Delete(_runtime._out);
	Safe_Delete(_runtime._in);
	Safe_Delete(_runtime._in2);
	Safe_Delete(_runtime._bk);
}

void  DXTransform::test()
{
	LPVOID pThis = this;
	LPVOID prt = &_runtime;
	long l = sizeof(DXTRuntime);
	LPVOID pid = &_runtime._timerId;
	//WCHAR path[_MAX_PATH] = L"";
	//::GetTempPathW(_MAX_PATH, path);
	//lstrcatW(path, L"\\dxtest.bmp");
	////HWND hwnd = ::GetActiveWindow();
	////HDC hdc = ::GetDC(hwnd);
	//HBITMAP hbmp = (HBITMAP)::GetCurrentObject(_runtime._ctx._dc, OBJ_BITMAP);

	//SaveBitmapToFile(hbmp, path);
}

BOOL DXTransform::OnPaint( HDC hdc, const RECT& rect, DWORD stage )
{
	// 如果已经启动，此时应该直接画到DC上，然后返回TRUE
	if (!Enabled || (_inDesired>1 && !_runtime._started)) return FALSE;
	if (_runtime._out == NULL) return FALSE;

	BOOL bRet = TRUE;
	if (_inDesired <= 1)
	{
		if (_inDesired == 1)
		{
			_init_1st_surface(FALSE, TRUE);
			_capture_to_surface(_runtime._in);
			_t->Execute(NULL, NULL, NULL);
		}
		else if (stage == DXT_DS_CONTENT) // procedural surface is between background and content
		{
			// 1st - paint background
			//_capture_to_surface(_runtime._out, DXT_DS_BACKGROUND);
			// 2nd - paint procedural surface
			_t->Execute(NULL, NULL, NULL);
			// 3rd - paint content(include border and scrollbar)
			//_capture_to_surface(_runtime._out, DXT_DS_BORDER|DXT_DS_CONTENT);
			bRet = FALSE;
		}
		else
			return FALSE;
	}

	//RECT rcClip = _runtime._ctx._rcClip;
	//if (::IsRectEmpty(&rcClip)) rcClip = _runtime._ctx._rc;
	//ClipDC clip(hdc, rcClip);
	RECT rclip;
	::GetClipBox(hdc, &rclip);
	long cx = _runtime._ctx._rc.right-_runtime._ctx._rc.left;
	long cy = _runtime._ctx._rc.bottom-_runtime._ctx._rc.top;
	BLENDFUNCTION bf = {AC_SRC_OVER, 0, 255, AC_SRC_ALPHA};
	// background
	//if (_runtime._bk)
	//{
	//	::BitBlt(hdc, _runtime._ctx._rc.left, _runtime._ctx._rc.top, cx, cy,
	//		_runtime._bk->GetDC(), 0, 0, SRCCOPY);
	//	_runtime._bk->ReleaseDC();
	//}

	//POINT ptOld = {0};
	//::GetWindowOrgEx(hdc, &ptOld);
	//POINT ptOrg = {0};
	//if (_runtime._ctx._getWindowOrg)
	//{
	//	ptOrg = _runtime._ctx._getWindowOrg(_runtime._ctx._data);
	//	::SetWindowOrgEx(hdc, ptOrg.x, ptOrg.y, NULL);
	//}

	if (_misc & DXTMF_BLEND_WITH_OUTPUT)
		::AlphaBlend(hdc, _runtime._ctx._rc.left, _runtime._ctx._rc.top, cx, cy,
			_runtime._out->GetDC(), 0, 0, cx, cy, bf);
	else
		//::TransparentBlt(hdc, _runtime._ctx._rc.left, _runtime._ctx._rc.top, cx, cy,
		//	_runtime._out->GetDC(), 0, 0, cx, cy, clr_parent);
		::BitBlt(hdc, _runtime._ctx._rc.left, _runtime._ctx._rc.top, cx, cy,
			_runtime._out->GetDC(), 0, 0, SRCCOPY);
	
	//::SetWindowOrgEx(hdc, ptOld.x, ptOld.y, NULL);
	_runtime._out->ReleaseDC();

	return bRet;
}

void CALLBACK DXTransform::_timer_proc( LPVOID pData, DWORD dwId )
{
	//ATLTRACE("定时器(%d)回调, DxTransform(0x%08x)\n", dwId, pData);
	DXTransform* pThis = (DXTransform*)pData;
	if (pThis == NULL) return;
	if (::IsBadWritePtr(pThis, sizeof(DXTransform))) return;

	float duration = 1.0f;
	HRESULT hr = S_OK;
	if (pThis->_effect.p)
	{
		pThis->_effect->get_Duration(&duration);
	}
	float step = dxt_period / (duration * 1000);
	pThis->SetPercent(pThis->_runtime._percent + step);

	if (pThis->_runtime._ctx._transformProc)
		pThis->_runtime._ctx._transformProc(pThis->_runtime._ctx._data, 0);

	if (pThis->_runtime._percent >= 1.0f)
	{
		pThis->Stop();
		//Safe_Delete(pThis->_runtime._out);
		//Safe_Delete(pThis->_runtime._in);
		//Safe_Delete(pThis->_runtime._in2);
		if (pThis->_runtime._ctx._transformProc) // 最后强制刷新一次，使之能按正常方式绘制
			pThis->_runtime._ctx._transformProc(pThis->_runtime._ctx._data, 1);
	}
}

DXSurface::DXSurface( DXTransform& t, BOOL bInput ) : _t(t), _bInput(bInput), _hdc(NULL)
{
	DXT& dxt = DXT::instance();
	if (!dxt._init() || !_t._runtime._ctx.IsValid()) return;

	CDXDBnds bnds(_t._runtime._ctx._rc);
	//CDXDBnds bnds(_t._runtime._ctx._rc.right-_t._runtime._ctx._rc.left, _t._runtime._ctx._rc.bottom-_t._runtime._ctx._rc.top);
	//DDSURFACEDESC sd = {sizeof(DDSURFACEDESC)};
	//sd.dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH /* | DDSD_LPSURFACE| DDSD_PIXELFORMAT*/;
	//sd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_SYSTEMMEMORY;
	//sd.dwHeight = rc.bottom - rc.top;
	//sd.dwWidth = rc.right - rc.left;
	////sd.lpSurface = m_pBits;
	////memset(&sd.ddpfPixelFormat, 0, sizeof(DDPIXELFORMAT)); // 用 DDSD_PIXELFORMAT 失败了
	////sd.ddpfPixelFormat.dwSize = sizeof(DDPIXELFORMAT);
	////sd.ddpfPixelFormat.dwFlags = DDPF_ALPHAPIXELS | DDPF_RGB;
	////sd.ddpfPixelFormat.dwRGBBitCount = 32;
	////sd.ddpfPixelFormat.dwRBitMask = 0x00ff0000;
	////sd.ddpfPixelFormat.dwGBitMask = 0x0000ff00;
	////sd.ddpfPixelFormat.dwBBitMask = 0x000000ff;
	////sd.ddpfPixelFormat.dwRGBAlphaBitMask = 0xff000000;
	/*
	 *	测试结果：
			1、用了DDSURFACEDESC参数后，LockSurfaceDC失败
	 */

	// 只能使用 RGB32 格式，不能使用ARGB32或者PMARGB32，否则显示不正常
	HRESULT hr = dxt._sf->CreateSurface(NULL, NULL, bInput?&default_input_pixelformat:&_t._outFormat, &bnds, 0, NULL, IID_IDXSurface, (void**)&_s.p);
	if (FAILED(hr)) return ;
	//hr = _s->SetColorKey(clr_parent);
	//if (!bInput) InitBackground(_s.p);
	// input 表面可能需要变换格式
	//if (_bInput)
	//{
	//	GUID fmtid;
	//	hr = _s->GetPixelFormat(&fmtid, NULL);
	//	if (fmtid == DDPF_RGB32)
	//		_s32 = _s;
	//	else
	//	{
	//		_s32 = NULL;
	//		hr = dxt._sf->CopySurfaceToNewFormat(_s.p, NULL, NULL, &DDPF_RGB32, &_s32.p);
	//	}
	//}
	//hr = _s->SetColorKey((DXSAMPLE)(clr_parent|0xff000000));
	//return;
}

DXSurface::~DXSurface()
{
	ReleaseDC();
}

HDC DXSurface::GetDC()
{
	if (_hdc==NULL)
	{
		//if (!_bInput) UpdateAlpha(_s.p);
		if (_dclock.p == NULL)
			_s->LockSurfaceDC(NULL, INFINITE, _bInput?DXLOCKF_READWRITE:DXLOCKF_READ, &_dclock);
		if (_dclock.p) _hdc = _dclock->GetDC();

		CDCHandle dc(_hdc);
		if (_bInput && _hdc)
		{
			CRect rc = _t._runtime._ctx._rc;
			dc.SelectFont((HFONT)::GetStockObject(DEFAULT_GUI_FONT));
			// 控件绘制到外部DC时，将重设DC的SetWindowOrgEx，以便从DC的(0,0)位置开始绘制
			//POINT ptOrg = {0};
			//if (_t._runtime._ctx._getWindowOrg)
			//{
			//	ptOrg = _t._runtime._ctx._getWindowOrg(_t._runtime._ctx._data);
			//	dc.SetWindowOrg(ptOrg.x, ptOrg.y);
			//}
			dc.SetWindowOrg(rc.left, rc.top);

			//CRect rc2(rc);
			//rc2.OffsetRect(-rc2.left, -rc2.top);
			dc.FillSolidRect(&rc, clr_parent);
		}
	}
	return _hdc;
}

void DXSurface::ReleaseDC()
{
	_dclock = NULL;
	_hdc = NULL;

	if (_bInput)
	{
		DXT& dxt = DXT::instance();
		_s32 = NULL;
		GUID fmtid;
		HRESULT hr = _s->GetPixelFormat(&fmtid, NULL);
		if (fmtid == default_input_pixelformat)
			_s32 = _s;
		else
		{
			_s32 = NULL;
			hr = dxt._sf->CopySurfaceToNewFormat(_s.p, NULL, NULL, &default_input_pixelformat, &_s32.p);
		}
		//UpdateAlpha(_s32.p);
	}
}

BOOL SaveBitmapToFile(HBITMAP hBitmap, LPCWSTR lpFileName)
{    	 //lpFileName 为位图文件名
	HDC     hDC;         
	//设备描述表
	int     iBits;      
	//当前显示分辨率下每个像素所占位数
	WORD    wBitCount = 32;   
	//位图中每个像素所占字节数
	//定义调色板大小， 位图中像素字节大小 ，  位图文件大小 ， 写入文件字节数
	DWORD           dwPaletteSize=0,dwBmBitsSize,dwDIBSize, dwWritten;
	BITMAP          Bitmap;        
	//位图属性结构
	BITMAPFILEHEADER   bmfHdr;        
	//位图文件头结构
	BITMAPINFOHEADER   bi;            
	//位图信息头结构 
	LPBITMAPINFOHEADER lpbi;          
	//指向位图信息头结构
	HANDLE          fh, hDib, hPal;
	HPALETTE     hOldPal=NULL;
	//定义文件，分配内存句柄，调色板句柄

	//计算位图文件每个像素所占字节数
	hDC = CreateDCW(L"DISPLAY",NULL,NULL,NULL);
	iBits = GetDeviceCaps(hDC, BITSPIXEL) * 
		GetDeviceCaps(hDC, PLANES);
	DeleteDC(hDC);
	if (iBits <= 1)
		wBitCount = 1;
	else if (iBits <= 4)
		wBitCount = 4;
	else if (iBits <= 8)
		wBitCount = 8;
	else if (iBits <= 24)
		wBitCount = 24;
	//计算调色板大小
	if (wBitCount <= 8)
		dwPaletteSize=(1<<wBitCount)*sizeof(RGBQUAD);

	//设置位图信息头结构
	GetObject(hBitmap, sizeof(BITMAP), &Bitmap);
	bi.biSize            = sizeof(BITMAPINFOHEADER);
	bi.biWidth           = Bitmap.bmWidth;
	bi.biHeight          = Bitmap.bmHeight;
	bi.biPlanes          = 1;
	bi.biBitCount         = wBitCount;
	bi.biCompression      = BI_RGB;
	bi.biSizeImage        = 0;
	bi.biXPelsPerMeter     = 0;
	bi.biYPelsPerMeter     = 0;
	bi.biClrUsed         = 0;
	bi.biClrImportant      = 0;

	dwBmBitsSize = ((Bitmap.bmWidth*wBitCount+31)/32)*4*Bitmap.bmHeight;
	//为位图内容分配内存
	hDib  = GlobalAlloc(GHND,dwBmBitsSize+dwPaletteSize+sizeof(BITMAPINFOHEADER));
	lpbi = (LPBITMAPINFOHEADER)GlobalLock(hDib);
	*lpbi = bi;
	// 处理调色板   
	hPal = GetStockObject(DEFAULT_PALETTE);
	if (hPal)
	{
		hDC = GetDC(NULL);
		hOldPal=SelectPalette(hDC,(HPALETTE)hPal,FALSE);
		RealizePalette(hDC);
	}
	// 获取该调色板下新的像素值
	GetDIBits(hDC,hBitmap,0,(UINT)Bitmap.bmHeight,(LPSTR)lpbi+sizeof(BITMAPINFOHEADER)+dwPaletteSize, (BITMAPINFO *)lpbi,DIB_RGB_COLORS);
	//恢复调色板   
	if (hOldPal)
	{
		SelectPalette(hDC, hOldPal, TRUE);
		RealizePalette(hDC);
		ReleaseDC(NULL, hDC);
	}
	//创建位图文件    
	fh=CreateFileW(lpFileName, GENERIC_WRITE,0, NULL, CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN, NULL);
	if (fh==INVALID_HANDLE_VALUE)
		return FALSE;
	// 设置位图文件头
	bmfHdr.bfType = ((WORD) ('M' << 8) | 'B');  // "BM"
	dwDIBSize=sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER)+dwPaletteSize+dwBmBitsSize;  
	bmfHdr.bfSize = dwDIBSize;
	bmfHdr.bfReserved1 = 0;
	bmfHdr.bfReserved2 = 0;
	bmfHdr.bfOffBits = (DWORD)sizeof(BITMAPFILEHEADER)+(DWORD)sizeof(BITMAPINFOHEADER)+dwPaletteSize;
	// 写入位图文件头
	WriteFile(fh, (LPSTR)&bmfHdr, sizeof(BITMAPFILEHEADER), &dwWritten, NULL);
	// 写入位图文件其余内容
	WriteFile(fh, (LPSTR)lpbi, dwDIBSize, 
		&dwWritten, NULL);
	//清除   
	GlobalUnlock(hDib);
	GlobalFree(hDib);
	CloseHandle(fh);
	return TRUE;
}

