#ifndef __DXT_H__
#define __DXT_H__

#pragma once
//#include <ddraw.h>

//#include "DirectUI.h" // 为了能被多项目使用，不直接包含头文件，改成用宏定义来控制
#include "transform/dxtrans.h"
#include "../../common/macro.h"

#define Safe_Delete(x) if (x) (x)->Delete(&(x));

// DXT Draw State
#define DXT_DS_CONTENT		0x00000001	// content + scrollbar
#define DXT_DS_BACKGROUND	0x00000002
#define DXT_DS_BORDER		0x00000004
#define DXT_DS_FOREGROUND	0x00000008
#define DXT_DS_ALL			0x0000FFFF

class DXT;
class DXTransform;
class DXSurface;

class DXT
{
	friend DXTransform;
	friend DXSurface;
private:
	CComPtr<IDXTransformFactory> _tf;
	CComPtr<IDXSurfaceFactory> _sf;

	DXT() {}
	~DXT() { _clear(); }

	BOOL _init();
	void _clear();

	static BOOL parse_to_clsid(LPCOLESTR szName, CLSID& clsid, LPCOLESTR szPrev=NULL);

public:
	static DXT& instance();
	static void clear();

	static DXTransform* create_transform(LPCOLESTR szProgId, IPropertyBag* pInitProp=NULL);
	static DXSurface* create_surface(DXTransform& t, BOOL writable=FALSE);
};

//////////////////////////////////////////////////////////////////////////
// surface

class DXSurface
{
	friend DXT;
	friend DXTransform;
private:
	BOOL _bInput;
	DXTransform& _t;
	CComPtr<IDXSurface> _s;
	CComPtr<IDXSurface> _s32; // _s 的格式可能是不同的，全部转换成 ARGB32 格式再设置到 Transform 中
	CComPtr<IDXDCLock> _dclock;
	HDC _hdc;

	DXSurface(DXTransform& t, BOOL writable=FALSE);
	~DXSurface();

public:
	IDXSurface* ptr() { return _s32.p?_s32.p:_s.p; }

	void Delete(DXSurface** pps=NULL)
	{
		delete this;
		if (pps) *pps = NULL;
	}
	HDC GetDC();
	void ReleaseDC();
};

//////////////////////////////////////////////////////////////////////////
// transform

// 变换定时通知
typedef void (CALLBACK* pfnTransformProc)(LPVOID pData, DWORD dwState);
typedef void (CALLBACK* pfnDrawToDC)(HDC hdc, LPVOID pData, DWORD dwState);
typedef void (CALLBACK* pfnDrawBackgroundToDC)(HDC hdc, LPVOID pData, const RECT rc);
typedef POINT (CALLBACK* pfnGetWindowOrg)(LPVOID pData);

//class DXTransformNotify
//{
//public:
//	virtual void OnTransform() = 0;
//};

struct DxtContext
{
	HDC _dc;
	RECT _rc;
	RECT _rcClip;
	//DXTransformNotify* _transformProc;
	LPVOID _data;
	pfnTransformProc _transformProc;
	pfnDrawToDC _drawToDC;
	pfnDrawBackgroundToDC _drawBkToDC;
	pfnGetWindowOrg _getWindowOrg;
	IPropertyBag* _props;

	DxtContext() { memset(this, 0, sizeof(DxtContext)); }
	BOOL IsValid() const { return _dc && !::IsRectEmpty(&_rc); }
};

class DXTransform
{
	friend DXT;
	friend DXSurface;
public:
	Begin_Disp_Map(DXTransform)
		//Disp_Method(Apply, void, 71, DispFuncParams0)();		// 每调用一次就捕获一次HDC的内容，产生一个新的表面
		//Disp_Method(Play, void, 72, DispFuncParams1<VARIANT>)(VARIANT vDuration);	// 当调用时，产生一个输出表面，开始启动定时器。vDuration(VT_R4, >=0.0秒)
		//Disp_Method(Stop, void, 73, DispFuncParams0)();
		//Disp_Method(test, void, 74, DispFuncParams0)();
		//Disp_Property(Enabled, VARIANT_BOOL, 1)
		Disp_Property(1, Enabled, bool)
		Disp_Method(71, Apply, void, 0)		// 每调用一次就捕获一次HDC的内容，产生一个新的表面
		Disp_Method(72, Play, void, 1, float)	// 当调用时，产生一个输出表面，开始启动定时器。vDuration(VT_R4, >=0.0秒)
		Disp_Method(73, Stop, void, 0)
	End_Disp_Map()

	bool Enabled;

	void Apply();
	void Play(float fDuration = 1.0f);
	void Stop();

	void test();

private:
	CComQIPtr<IDXTransform> _t;
	CComQIPtr<IDXEffect> _effect;
	CComQIPtr<IDXTScaleOutput> _scale;
	int _outFormatIndex; // 格式索引
	GUID _outFormat;
	CLSID _clsid;
	DWORD _misc;	// GetMiscFlags
	ULONG _inMin, _inMax, _inDesired; // min:需要的最少输入表面数(0-2)；max:可选的最多输入表面数(<=2)；desired:期望的输入表面数(min-max)

	// 运行时数据
	struct DXTRuntime
	{
		DxtContext _ctx;
		BOOL _started;
		float _percent; // 0.0 - 1.0  当前执行完成的百分比
		//float _duration; // 单位秒
		//ULONG _fireTotal; // 定时器回调需要执行的总次数
		//ULONG _fireCount; // 定时器回调已经被执行的次数
		DXSurface* _out;
		DXSurface* _in;
		DXSurface* _in2;
		DXSurface* _bk;
		DWORD _timerId;

		DXTRuntime() { memset(this, 0, sizeof(DXTRuntime)); _timerId = (DWORD)-1; }
		~DXTRuntime()
		{
			Safe_Delete(_out);
			Safe_Delete(_in);
			Safe_Delete(_in2);
			Safe_Delete(_bk);
		}
	};
	DXTRuntime _runtime;

	DXTransform(IDXTransform* t, CLSID clsid);
	~DXTransform() { Stop(); }

	static void CALLBACK _timer_proc(LPVOID pData, DWORD dwId);

	HRESULT _init_1st_surface(BOOL bCapture=TRUE, BOOL bSetup=FALSE);
	HRESULT _capture_to_surface(DXSurface* surf, DWORD dwState=DXT_DS_ALL);
	HRESULT _capture_background();
	HRESULT _setup();
	HRESULT _init_properties();

public:
	IDXTransform* ptr() { return _t.p; }
	//IDispatch* DispatchPtr() { return _disp.p; }
	IDispatch* disp() { return GetDispatch(); }
	IDXEffect* effect() { return _effect.p; }
	//BOOL IsTypeSame(DXTransform* other) const { return this==other || _clsid==other->_clsid; }
	BOOL IsType(LPCOLESTR szProgId) const;

	void Delete(DXTransform** ppt=NULL)
	{
		delete this;
		if (ppt) *ppt = NULL;
	}

	void Init(DxtContext& ctx); // 如果变换区域的位置或大小发生变化，则必须重新调用此函数
	// Like ICSSFilterDispatch
	//HRESULT Apply();	// 每调用一次就捕获一次HDC的内容，产生一个新的表面
	//HRESULT Play(VARIANT vDuration);	// 当调用时，产生一个输出表面，开始启动定时器。vDuration(VT_R4, >=0.0秒)
	//HRESULT Stop();

	BOOL OnPaint(HDC hdc, const RECT& rect, DWORD stage); // stage 指绘制阶段，例如 DXT_DS_BACKGROUND 表示请求绘制背景，如果返回FALSE则调用默认处理

	void SetPercent(float percent);
};

#endif // __DXT_H__