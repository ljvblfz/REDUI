#include "stdafx.h"
#include "Dui3DAnimation.h"
#include "Dui3DAuto.h"
#include "DuiStyleParser.h"

//////////////////////////////////////////////////////////////////////////
// 输入采样值，返回 CCubic 数组，此采样曲线非闭合，数组个数是 n-1
/* calculates the natural cubic spline that interpolates
y[0], y[1], ... y[n-1]
The first segment is returned as
C[0].a + C[0].b*u + C[0].c*u^2 + C[0].d*u^3 0<=u <1
the other segments are in C[1], C[2], ...  C[n-1] */
static CCubic* calcNaturalCubic(ULONG n, float* x)
{
	float* gamma = NEW float [n];
	float* delta = NEW float [n];
	float* D = NEW float [n];
	long i;
	/* We solve the equation
	[2 1       ] [D[0]]   [3(x[1] - x[0])  ]
	|1 4 1     | |D[1]|   |3(x[2] - x[0])  |
	|  1 4 1   | | .  | = |      .         |
	|    ..... | | .  |   |      .         |
	|     1 4 1| | .  |   |3(x[n-1] - x[n-3])|
	[       1 2] [D[n-1]]   [3(x[n-1] - x[n-2])]

	by using row operations to convert the matrix to upper triangular
	and then back sustitution.  The D[i] are the derivatives at the knots.
	*/
	gamma[0] = 1.0f/2.0f;
	for ( i = 1; i < (long)n-1; i++) 
	{
		gamma[i] = 1/(4-gamma[i-1]);
	}
	gamma[n-1] = 1/(2-gamma[n-2]);

	delta[0] = 3*(x[1]-x[0])*gamma[0];
	for ( i = 1; i < (long)n-1; i++) 
	{
		delta[i] = (3*(x[i+1]-x[i-1])-delta[i-1])*gamma[i];
	}
	delta[n-1] = (3*(x[n-1]-x[n-2])-delta[n-2])*gamma[n-1];

	D[n-1] = delta[n-1];
	for ( i = (long)n-2; i >= 0; i--) 
	{
		D[i] = delta[i] - gamma[i]*D[i+1];
	}

	/* now compute the coefficients of the cubics */
	CCubic* C = NEW CCubic [n-1];
	for ( i = 0; i < (long)n-1; i++)
	{
		C[i] = CCubic(x[i], D[i], 3*(x[i+1] - x[i]) - 2*D[i] - D[i+1],
			2*(x[i] - x[i+1]) + D[i] + D[i+1] , x[i+1]);
	}

	delete [] gamma;
	delete [] delta;
	delete [] D;

	return C;
};

// 输入采样值，返回 CCubic 数组，此采样曲线闭合，数组个数是 n
/* calculates the closed natural cubic spline that interpolates
x[0], x[1], ... x[n-1]
The first segment is returned as
C[0].a + C[0].b*u + C[0].c*u^2 + C[0].d*u^3 0<=u <1
the other segments are in C[1], C[2], ...  C[n-1] */
static CCubic* calcNaturalCubicClosed(ULONG n, float* x)
{
	float* w = NEW float[n];
	float* v = NEW float[n];
	float* y = NEW float[n];
	float* D = NEW float[n];
	float z, F, G, H;
	long k;
	/* We solve the equation
	[4 1      1] [D[0]]   [3(x[1] - x[n-1])  ]
	|1 4 1     | |D[1]|   |3(x[2] - x[0])  |
	|  1 4 1   | | .  | = |      .         |
	|    ..... | | .  |   |      .         |
	|     1 4 1| | .  |   |3(x[n-1] - x[n-3])|
	[1      1 4] [D[n-1]]   [3(x[0] - x[n-2])]

	by decomposing the matrix into upper triangular and lower matrices
	and then back sustitution.  See Spath "Spline Algorithms for Curves
	and Surfaces" pp 19--21. The D[i] are the derivatives at the knots.
	*/
	w[1] = v[1] = z = 1.0f/4.0f;
	y[0] = z * 3 * (x[1] - x[n-1]);
	H = 4;
	F = (3 * (x[0] - x[n-2]));
	G = 1;
	for ( k = 1; k < (long)n-1; k++) {
		v[k+1] = z = 1/(4 - v[k]);
		w[k+1] = -z * w[k];
		y[k] = z * (3*(x[k+1]-x[k-1]) - y[k-1]);
		H = H - G * w[k];
		F = F - G * y[k-1];
		G = -v[k] * G;
	}
	H = H - (G+1)*(v[n-1]+w[n-1]);
	y[n-1] = F - (G+1)*y[n-2];

	D[n-1] = y[n-1]/H;
	D[n-2] = y[n-2] - (v[n-1]+w[n-1])*D[n-1]; /* This equation is WRONG! in my copy of Spath */
	for ( k = (long)n-3; k >= 0; k--) {
		D[k] = y[k] - v[k+1]*D[k+1] - w[k+1]*D[n-1];
	}

	/* now compute the coefficients of the cubics */
	CCubic* C = NEW CCubic[n];
	for ( k = 0; k < (long)n-1; k++)
	{
		C[k] = CCubic(x[k], D[k], 3*(x[k+1] - x[k]) - 2*D[k] - D[k+1],
			2*(x[k] - x[k+1]) + D[k] + D[k+1], x[k+1]);
	}
	C[n-1] = CCubic(x[n-1], D[n-1], 3*(x[0] - x[n-1]) - 2*D[n-1] - D[0],
		2*(x[n-1] - x[0]) + D[n-1] + D[0], x[0]);

	delete [] w;
	delete [] v;
	delete [] y;
	delete [] D;

	return C;
};


//////////////////////////////////////////////////////////////////////////
iStoryBoard::iStoryBoard( iDevice* device/*=NULL*/ ) : m_device(device/*?device:iDevice::get_active_device()*/)
			, m_keyframe_sets(NULL)
			, m_started(false), m_stopped(false), m_paused(false)
			, m_current_time(0)
			, m_stop_behavior(stop_current), m_stop_pending(false), m_stop_time(1.f)
{
	//ATLASSERT(m_device);
	if (m_device)
		addToList(&m_device->m_stories, false);
}

iStoryBoard::~iStoryBoard()
{
	//removeFromList();
	if (m_keyframe_sets) m_keyframe_sets->removeAll();
}

bool iStoryBoard::isRunning( bool bMeOnly/*=false*/ )
{
	//if (m_device==NULL) return false;
	if ((m_started && !m_stopped && !m_paused) || m_stop_pending) return true;
	if (bMeOnly) return false;

	const iStoryBoard* sb = Next();
	while (sb)
	{
		if ((sb->m_started && !sb->m_stopped && !sb->m_paused) || sb->m_stop_pending) return true;
		sb = sb->Next();
	}
	return false;
}

void iStoryBoard::step( float fTimeDelta, bool bNext/*=true*/ )
{
	if (isRunning(true) && m_keyframe_sets)
	{
		float delta = fTimeDelta;
		if (m_stop_pending && m_current_time+fTimeDelta>=m_stop_time)
		{
			delta = m_stop_time - m_current_time;
			m_current_time = m_stop_time;
		}
		else
			m_current_time += fTimeDelta;

		iFrameSetBase* kfs = m_keyframe_sets;
		while (kfs)
		{
			kfs->step(delta);
			kfs = kfs->Next();
		}
		m_device->fire_event(&m_onstep, GetDispatch());

		if ((m_stop_pending && m_current_time==m_stop_time) || (m_keyframe_sets && m_keyframe_sets->isFinished()))
		{
			m_started = false;
			m_stopped = true;
			m_paused = false;

			m_current_time = 0;
			m_stop_pending = false;
			m_device->on_story_stopped();

			// fire event
			m_device->fire_event(&m_onstop, GetDispatch());
		}
	}

	if (bNext)
	{
		iStoryBoard* sb = Next();
		while (sb)
		{
			sb->step(fTimeDelta);
			sb = sb->Next();
		}
	}
}

void iStoryBoard::start()
{
	if (m_device==NULL) return;
	if (m_started) return;
	m_started = true;
	m_stopped = false;
	m_paused = false;
	m_current_time = 0;
	m_stop_pending = false;

	// fire event
	m_device->fire_event(&m_onstart, GetDispatch());
	if (m_keyframe_sets) m_keyframe_sets->onstart();
	m_device->on_story_started();
	step(0, false);
}

void iStoryBoard::stop(int stopBehavior/*=0*/, float stopTime/*=1.f*/)
{
	if (m_device==NULL) return;
	if (!m_started || m_stop_pending || m_stopped) return;
	m_started = false;
	m_stopped = true;
	m_paused = false;

	m_stop_behavior = stopBehavior;
	m_stop_time = max(stopTime, 0.1f); // 最小0.1秒
	m_stop_pending = false;

	switch (m_stop_behavior)
	{
	case stop_begin:
	case stop_end:
		m_stop_pending = true;
		m_current_time = 0;
		if (m_keyframe_sets)
			m_keyframe_sets->onstopPending();
		return;
		break;
	case stop_beginNow:
	case stop_endNow:
		if (m_keyframe_sets)
			m_keyframe_sets->onstop();
		break;
	}

	m_device->on_story_stopped();

	// fire event
	m_device->fire_event(&m_onstop, GetDispatch());
}

void iStoryBoard::pause()
{
	if (m_device==NULL) return;
	if (!m_started || m_stopped || m_paused) return;
	m_paused = true;
	m_device->on_story_stopped();

	// fire event
	m_device->fire_event(&m_onpause, GetDispatch());
}

void iStoryBoard::resume()
{
	if (m_device==NULL) return;
	if (!m_started || m_stopped || !m_paused) return;
	m_paused = false;
	m_device->on_story_started();

	// fire event
	m_device->fire_event(&m_onresume, GetDispatch());
}

void iStoryBoard::unload()
{
	if (m_keyframe_sets) m_keyframe_sets->unload();
	m_onstart.Clear();
	m_onstop.Clear();
	m_onpause.Clear();
	m_onresume.Clear();
	m_onstep.Clear();

	if (next) next->unload();
}

//////////////////////////////////////////////////////////////////////////
iFrameSetBase::iFrameSetBase( iStoryBoard* sb/*=NULL*/ ) : m_story(sb)
			, m_reverse(false), m_closure(false), m_repeat(1), m_delay(0), m_idle(0)
{
	clearRuntime();
	if (m_story) addToList(&m_story->m_keyframe_sets, false);
}

iFrameSetBase::~iFrameSetBase()
{
}

void iFrameSetBase::clearRuntime()
{
	__runtime.~__tagRuntime();
	ZeroMemory(&__runtime, sizeof(__runtime));

	//iFrameSetBase* kfs = Next();
	//while (kfs)
	//{
	//	ZeroMemory(&kfs->__runtime, sizeof(__runtime));
	//	kfs = kfs->Next();
	//}
}

void iFrameSetBase::onstart()
{
	clearRuntime();

	if (Next()) Next()->onstart();
}

ATL::CComDispatchDriver iFrameSetBase::_get_object()
{
	if (m_target.vt == VT_DISPATCH)
		return V_DISPATCH(&m_target);
	else if (m_target.vt==VT_BSTR && V_BSTR(&m_target) && m_story->m_device->m_window_object)
	{
		CComVariant v;
		CComQIPtr<IDispatchEx> dispex(m_story->m_device->m_window_object);
		HRESULT hr;
		if (dispex)
		{
			DISPID did = DISPID_UNKNOWN;
			hr = dispex->GetDispID(V_BSTR(&m_target), 0, &did);
			if (SUCCEEDED(hr))
			{
				DISPPARAMS dp = {NULL, 0, NULL, 0};
				hr = dispex->InvokeEx(did, LOCALE_SYSTEM_DEFAULT, DISPATCH_PROPERTYGET, &dp, &v, NULL, NULL);
			}
		}
		else
			hr = m_story->m_device->m_window_object.GetPropertyByName(V_BSTR(&m_target), &v);

		// 尝试执行脚本获取对象
		if (FAILED(hr))
		{
			CScript script(m_story->m_device->m_hscript);
			if (script.IsValid())
			{
				hr = script.CalcExpression(V_BSTR(&m_target), &v);
			}
		}

		if (SUCCEEDED(hr) && v.vt==VT_DISPATCH)
		{
			m_target = v; // 现在可以直接保存对象
			return V_DISPATCH(&m_target);
		}
	}
	return CComDispatchDriver();
}

HRESULT iFrameSetBase::_set_property( iAttributeData* val )
{
	// 先判断是否跟缓存值相同
	if (val->vt == __runtime.vOld.vt)
	{
		if (__runtime.vOld.vt!=VT_DISPATCH && __runtime.vOld==*val) return S_OK;
		if (__runtime.vOld.vt==VT_DISPATCH)
		{
			if (__runtime.vOld.pdispVal == val->pdispVal) return S_OK;

#define __var_cmp(classname) \
			{ \
				classname* p = DISP_CAST(__runtime.vOld.pdispVal, classname); \
				classname* p2 = DISP_CAST(val->pdispVal, classname); \
				if (p && p2 && (memcmp(p,p2,sizeof(classname))==0)) return S_OK; \
			}

			__var_cmp(D3DVECTOR);
			__var_cmp(D3DXVECTOR2);
			__var_cmp(D3DXVECTOR4);
			__var_cmp(D3DXPLANE);
			__var_cmp(D3DCOLORVALUE);
			__var_cmp(D3DXQUATERNION);
#undef __var_cmp
		}
	}

	CComDispatchDriver obj = _get_object();
	if (obj.p==NULL) return E_FAIL;

	// 1.尝试直接赋值
	DISPID did = DISPID_UNKNOWN;
	HRESULT hr = obj.GetIDOfName(m_target_attribute, &did);
	if (SUCCEEDED(hr))
		return (__runtime.vOld=*val, obj.PutProperty(did, val));

	// 2.尝试 set 方法，DUI控件都有此方法
	hr = obj.GetIDOfName(L"set", &did);
	if (SUCCEEDED(hr))
	{
		CComVariant vRet, vP=m_target_attribute;
		hr = obj.Invoke2(did, &vP, val, &vRet);
		if (SUCCEEDED(hr))
			__runtime.vOld=*val;
		return hr;
	}

	// 3.尝试IDispatchEx，说明此对象是一个脚本环境对象，强行增加一个属性
	CComQIPtr<IDispatchEx> dispex(obj.p);
	if (dispex.p)
	{
		hr = dispex->GetDispID(m_target_attribute, fdexNameCaseInsensitive | fdexNameEnsure, &did);
		if (SUCCEEDED(hr))
		{
			CComVariant vRet;
			DISPPARAMS dp = {val, NULL, 1, 0};
			hr = dispex->InvokeEx(did, LOCALE_SYSTEM_DEFAULT, DISPATCH_PROPERTYPUT, &dp, &vRet, NULL, NULL);
			if (SUCCEEDED(hr))
				__runtime.vOld=*val;
		}
	}
	return hr;
}

HRESULT iFrameSetBase::_get_property( VARIANT* val )
{
	CComDispatchDriver obj = _get_object();
	if (obj.p==NULL) return E_FAIL;

	// 1.尝试直接读取
	DISPID did = DISPID_UNKNOWN;
	HRESULT hr = obj.GetIDOfName(m_target_attribute, &did);
	if (SUCCEEDED(hr))
		return obj.GetProperty(did, val);

	// 2.尝试 get 方法，DUI控件都有此方法
	hr = obj.GetIDOfName(L"get", &did);
	if (SUCCEEDED(hr))
	{
		CComVariant vP=m_target_attribute;
		hr = obj.Invoke1(did, &vP, val);
	}

	// 3.不需要尝试IDispatchEx
	return hr;
}

void iFrameSetBase::unload()
{
	m_target.Clear();

	if (next) next->unload();
}

bool iFrameSetBase::isFinished()
{
	if (!__runtime.finished) return false;

	if (next) return next->isFinished();
	return true;
}

//////////////////////////////////////////////////////////////////////////
void iAttributeData::Create( LPCOLESTR type )
{
	if (type==NULL)
	{
		adt = NULL;
		return;
	}

	// 如果已经创建过了，则不能再次创建。此处可保证 Create 能被多次安全调用
	if (adt) return;

	// 如果已经有缓存数据（可能是字符串），临时复制到另一个VARIANT中
	CComVariant vTmp;
	if (vt!=VT_EMPTY)
		Detach(&vTmp);

	if (type==(LPCOLESTR)VT_I4 || (((DWORD_PTR)type&0xffff0000) && (lstrcmpiW(type, L"number")==0 || lstrcmpiW(type, L"int")==0)))
	{
		vt = VT_I4; adt = (LPCOLESTR)VT_I4;
	}
	//else if (type==(LPCOLESTR)VT_UI4 || (((DWORD_PTR)type&0xffff0000) && (lstrcmpiW(type, L"colorref")==0 || lstrcmpiW(type, L"dword")==0)))
	//{
	//	vt = VT_UI4; adt = (LPCOLESTR)VT_UI4;
	//}
	else if (type==(LPCOLESTR)VT_R4 || (((DWORD_PTR)type&0xffff0000) && (lstrcmpiW(type, L"float")==0)))
	{
		vt = VT_R4; adt = (LPCOLESTR)VT_R4;
	}
	else if (type==(LPCOLESTR)VT_R8 || (((DWORD_PTR)type&0xffff0000) && (lstrcmpiW(type, L"double")==0)))
	{
		vt = VT_R8; adt = (LPCOLESTR)VT_R8;
	}

#define do_object(name) else if (type==name::__ClassName() || (((DWORD_PTR)type&0xffff0000) && (lstrcmpiW(type, L#name)==0))) { adt=name::__ClassName(); vt=VT_DISPATCH; name::CreateInstance(&pdispVal); }
	__foreach_object(do_object)
#undef do_object

	if (vTmp.vt != VT_EMPTY)
	{
		if (vTmp.vt == VT_BSTR) InitFromString(vTmp.bstrVal);
		else CopyFrom(&vTmp);
	}
}

bool iAttributeData::InitFromString( LPCOLESTR szInit )
{
	if (adt==NULL)
	{
		// 如果还未执行创建，则直接缓存数据，以备以后初始化
		CComVariant::operator = (szInit);
		return true;
	}

	if (adt==(LPCOLESTR)VT_I4) return !!TryLoad_long_FromString(szInit, lVal);
	//else if (adt==(LPCOLESTR)VT_UI4) return !!TryLoad_COLORREF_FromString(szInit, (COLORREF&)ulVal);
	else if (adt==(LPCOLESTR)VT_R4) return !!TryLoad_float_FromString(szInit, fltVal);
	else if (adt==(LPCOLESTR)VT_R8) return !!TryLoad_double_FromString(szInit, dblVal);
	else if (vt!=VT_DISPATCH) return false;
#define do_object(name) \
		else if (adt==name::__ClassName()) \
		{ \
			name* p = DISP_CAST(pdispVal, name); \
			if (p && SUCCEEDED(p->InitFromString(szInit))) return true; \
			return false; \
		}

		__foreach_object(do_object)
#undef do_object

	return false;
}

bool iAttributeData::Calc( iAttributeData* from, iAttributeData* to, float percent, CCubic* c0/*=NULL*/, CCubic* c1/*=NULL*/, CCubic* c2/*=NULL*/, CCubic* c3/*=NULL*/ )
{
	if (from==NULL || percent<0 || percent>1.f || (to==NULL && percent!=0)) return false;

	if (percent==0)
	{
		Copy(from);
		adt = from->adt;
		return true;
	}
	if (from->adt != to->adt) return false;

	Create(from->adt);
	if (adt==NULL) return false;

#define PERCENT(x,y,p) ((x) * (1.f - p) + (y) * p)
	if (vt==VT_I4) lVal = c0 ? (long)c0->eval(percent) : (long)PERCENT(from->lVal, to->lVal, percent);
	//else if (vt==VT_UI4)
	//{
	//	BYTE r = c0 ? (BYTE)(long)c0->eval(percent) : (BYTE)PERCENT(GetRValue(from->ulVal), GetRValue(to->ulVal), percent);
	//	BYTE g = c1 ? (BYTE)(long)c1->eval(percent) : (BYTE)PERCENT(GetGValue(from->ulVal), GetGValue(to->ulVal), percent);
	//	BYTE b = c2 ? (BYTE)(long)c2->eval(percent) : (BYTE)PERCENT(GetBValue(from->ulVal), GetBValue(to->ulVal), percent);
	//	BYTE a = c3 ? (BYTE)(long)c3->eval(percent) : (BYTE)PERCENT((BYTE)(from->ulVal>>24), (BYTE)(to->ulVal>>24), percent);
	//	ulVal = RGB(r,g,b) | (a<<24);
	//}
	else if (vt==VT_R4) fltVal = c0 ? c0->eval(percent) : (float)PERCENT(from->fltVal, to->fltVal, percent);
	else if (vt==VT_R8) dblVal = c0 ? (double)c0->eval(percent) : (double)PERCENT(from->dblVal, to->dblVal, percent);
	else if (vt==VT_DISPATCH)
	{
		if (adt==Vector2D::__ClassName())
		{
			D3DXVECTOR2* p = DISP_CAST(pdispVal, D3DXVECTOR2);
			D3DXVECTOR2* p1 = DISP_CAST(from->pdispVal, D3DXVECTOR2);
			D3DXVECTOR2* p2 = DISP_CAST(to->pdispVal, D3DXVECTOR2);
			if (p==NULL || p1==NULL || p2==NULL) return false;
			p->x = c0 ? c0->eval(percent) : (float)PERCENT(p1->x, p2->x, percent);
			p->y = c1 ? c1->eval(percent) : (float)PERCENT(p1->y, p2->y, percent);
			return true;
		}
		else if (adt==Vector3D::__ClassName())
		{
			D3DVECTOR* p = DISP_CAST(pdispVal, D3DVECTOR);
			D3DVECTOR* p1 = DISP_CAST(from->pdispVal, D3DVECTOR);
			D3DVECTOR* p2 = DISP_CAST(to->pdispVal, D3DVECTOR);
			if (p==NULL || p1==NULL || p2==NULL) return false;
			p->x = c0 ? c0->eval(percent) : (float)PERCENT(p1->x, p2->x, percent);
			p->y = c1 ? c1->eval(percent) : (float)PERCENT(p1->y, p2->y, percent);
			p->z = c2 ? c2->eval(percent) : (float)PERCENT(p1->z, p2->z, percent);
			return true;
		}
		else if (adt==Vector4D::__ClassName())
		{
			D3DXVECTOR4* p = DISP_CAST(pdispVal, D3DXVECTOR4);
			D3DXVECTOR4* p1 = DISP_CAST(from->pdispVal, D3DXVECTOR4);
			D3DXVECTOR4* p2 = DISP_CAST(to->pdispVal, D3DXVECTOR4);
			if (p==NULL || p1==NULL || p2==NULL) return false;
			p->x = c0 ? c0->eval(percent) : (float)PERCENT(p1->x, p2->x, percent);
			p->y = c1 ? c1->eval(percent) : (float)PERCENT(p1->y, p2->y, percent);
			p->z = c2 ? c2->eval(percent) : (float)PERCENT(p1->z, p2->z, percent);
			p->w = c3 ? c3->eval(percent) : (float)PERCENT(p1->w, p2->w, percent);
			return true;
		}
		else if (adt==Plane::__ClassName())
		{
			D3DXPLANE* p = DISP_CAST(pdispVal, D3DXPLANE);
			D3DXPLANE* p1 = DISP_CAST(from->pdispVal, D3DXPLANE);
			D3DXPLANE* p2 = DISP_CAST(to->pdispVal, D3DXPLANE);
			if (p==NULL || p1==NULL || p2==NULL) return false;
			p->a = c0 ? c0->eval(percent) : (float)PERCENT(p1->a, p2->a, percent);
			p->b = c1 ? c1->eval(percent) : (float)PERCENT(p1->b, p2->b, percent);
			p->c = c2 ? c2->eval(percent) : (float)PERCENT(p1->c, p2->c, percent);
			p->d = c3 ? c3->eval(percent) : (float)PERCENT(p1->d, p2->d, percent);
			return true;
		}
		else if (adt==COLOR::__ClassName())
		{
			COLOR* p = DISP_CAST(pdispVal, COLOR);
			COLOR* p1 = DISP_CAST(from->pdispVal, COLOR);
			COLOR* p2 = DISP_CAST(to->pdispVal, COLOR);
			if (p==NULL || p1==NULL || p2==NULL) return false;
			p->r = c0 ? c0->eval(percent) : (float)PERCENT(p1->r, p2->r, percent);
			p->g = c1 ? c1->eval(percent) : (float)PERCENT(p1->g, p2->g, percent);
			p->b = c2 ? c2->eval(percent) : (float)PERCENT(p1->b, p2->b, percent);
			p->a = c3 ? c3->eval(percent) : (float)PERCENT(p1->a, p2->a, percent);
			p->OnRGBAChanged();
			return true;
		}
		else if (adt==Quaternion::__ClassName())
		{
			D3DXQUATERNION* p = DISP_CAST(pdispVal, D3DXQUATERNION);
			D3DXQUATERNION* p1 = DISP_CAST(from->pdispVal, D3DXQUATERNION);
			D3DXQUATERNION* p2 = DISP_CAST(to->pdispVal, D3DXQUATERNION);
			if (p==NULL || p1==NULL || p2==NULL) return false;
			p->x = c0 ? c0->eval(percent) : (float)PERCENT(p1->x, p2->x, percent);
			p->y = c1 ? c1->eval(percent) : (float)PERCENT(p1->y, p2->y, percent);
			p->z = c2 ? c2->eval(percent) : (float)PERCENT(p1->z, p2->z, percent);
			p->w = c3 ? c3->eval(percent) : (float)PERCENT(p1->w, p2->w, percent);
			return true;
		}
	}
	return false;
}

bool iAttributeData::CopyFrom( const VARIANT* pSrc )
{
	if (adt==NULL)
	{
		// 如果还未执行创建，则直接缓存数据，以备以后重新初始化
		CComVariant::operator = (*pSrc);
		return true;
	}

	//if (adt==NULL || vt==VT_EMPTY || vt==VT_ERROR) return false;
	if (pSrc==NULL || pSrc->vt==VT_EMPTY || pSrc->vt==VT_ERROR) return false;
	CComVariant v;
	if (FAILED(v.ChangeType(vt, pSrc))) return false;

	switch (vt)
	{
	case VT_I4:
	//case VT_UI4:
	case VT_R4:
	case VT_R8:
		Copy(&v);
		return true;
	case VT_DISPATCH:
		if (adt==Vector2D::__ClassName())
		{
			D3DXVECTOR2* p = DISP_CAST(pdispVal, D3DXVECTOR2);
			D3DXVECTOR2* p1 = DISP_CAST(v.pdispVal, D3DXVECTOR2);
			if (p==NULL || p1==NULL) return false;
			p->x = p1->x;
			p->y = p1->y;
			return true;
		}
		else if (adt==Vector3D::__ClassName())
		{
			D3DVECTOR* p = DISP_CAST(pdispVal, D3DVECTOR);
			D3DVECTOR* p1 = DISP_CAST(v.pdispVal, D3DVECTOR);
			if (p==NULL || p1==NULL) return false;
			p->x = p1->x;
			p->y = p1->y;
			p->z = p1->z;
			return true;
		}
		else if (adt==Vector4D::__ClassName())
		{
			D3DXVECTOR4* p = DISP_CAST(pdispVal, D3DXVECTOR4);
			D3DXVECTOR4* p1 = DISP_CAST(v.pdispVal, D3DXVECTOR4);
			if (p==NULL || p1==NULL) return false;
			p->x = p1->x;
			p->y = p1->y;
			p->z = p1->z;
			p->w = p1->w;
			return true;
		}
		else if (adt==Plane::__ClassName())
		{
			D3DXPLANE* p = DISP_CAST(pdispVal, D3DXPLANE);
			D3DXPLANE* p1 = DISP_CAST(v.pdispVal, D3DXPLANE);
			if (p==NULL || p1==NULL) return false;
			p->a = p1->a;
			p->b = p1->b;
			p->c = p1->c;
			p->d = p1->d;
			return true;
		}
		else if (adt==COLOR::__ClassName())
		{
			COLOR* p = DISP_CAST(pdispVal, COLOR);
			COLOR* p1 = DISP_CAST(v.pdispVal, COLOR);
			if (p==NULL || p1==NULL) return false;
			p->r = p1->r;
			p->g = p1->g;
			p->b = p1->b;
			p->a = p1->a;
			p->OnRGBAChanged();
			return true;
		}
		else if (adt==Quaternion::__ClassName())
		{
			D3DXQUATERNION* p = DISP_CAST(pdispVal, D3DXQUATERNION);
			D3DXQUATERNION* p1 = DISP_CAST(v.pdispVal, D3DXQUATERNION);
			if (p==NULL || p1==NULL) return false;
			p->x = p1->x;
			p->y = p1->y;
			p->z = p1->z;
			p->w = p1->w;
			return true;
		}
	}
	return false;
}

bool iAttributeData::Sum( iAttributeData* src, iAttributeData* src2 )
{
	if (src==NULL || src2==NULL || src->adt==NULL) return false;

	ClearToZero();
	adt = NULL;
	Create(src->adt);

	CComVariant v;
	bool b;
	switch (src->vt)
	{
	// 数值情况下，src2也必须是数值
	case VT_I4: b = SUCCEEDED(v.ChangeType(src->vt, src2)); if (b) lVal = src->lVal + v.lVal; return b;
	//case VT_UI4: b = SUCCEEDED(v.ChangeType(src->vt, src2)); if (b) ulVal = src->ulVal + v.ulVal; return b;
	case VT_R4: b = SUCCEEDED(v.ChangeType(src->vt, src2)); if (b) fltVal = src->fltVal + v.fltVal; return b;
	case VT_R8: b = SUCCEEDED(v.ChangeType(src->vt, src2)); if (b) dblVal = src->dblVal + v.dblVal; return b;

	// 在结构体情况下，src2可以是相同的结构体，也可以是浮点数
	case VT_DISPATCH:
		if (adt==Vector2D::__ClassName())
		{
			D3DXVECTOR2* p = DISP_CAST(pdispVal, D3DXVECTOR2);
			D3DXVECTOR2* p1 = DISP_CAST(src->pdispVal, D3DXVECTOR2);
			if (p==NULL || p1==NULL) return false;
			if (SUCCEEDED(v.ChangeType(VT_R4, src2)))
			{
				p->x = p1->x + v.fltVal;
				p->y = p1->y + v.fltVal;
				return true;
			}
			else if (src2->vt==VT_DISPATCH)
			{
				D3DXVECTOR2* p2 = DISP_CAST(src2->pdispVal, D3DXVECTOR2);
				if (p2==NULL) return false;
				p->x = p1->x + p2->x;
				p->y = p1->y + p2->y;
				return true;
			}
		}
		else if (adt==Vector3D::__ClassName())
		{
			D3DVECTOR* p = DISP_CAST(pdispVal, D3DVECTOR);
			D3DVECTOR* p1 = DISP_CAST(src->pdispVal, D3DVECTOR);
			if (p==NULL || p1==NULL) return false;
			if (SUCCEEDED(v.ChangeType(VT_R4, src2)))
			{
				p->x = p1->x + v.fltVal;
				p->y = p1->y + v.fltVal;
				p->z = p1->z + v.fltVal;
				return true;
			}
			else if (src2->vt==VT_DISPATCH)
			{
				D3DVECTOR* p2 = DISP_CAST(src2->pdispVal, D3DVECTOR);
				if (p2==NULL) return false;
				p->x = p1->x + p2->x;
				p->y = p1->y + p2->y;
				p->z = p1->z + p2->z;
				return true;
			}
		}
		else if (adt==Vector4D::__ClassName())
		{
			D3DXVECTOR4* p = DISP_CAST(pdispVal, D3DXVECTOR4);
			D3DXVECTOR4* p1 = DISP_CAST(src->pdispVal, D3DXVECTOR4);
			if (p==NULL || p1==NULL) return false;
			if (SUCCEEDED(v.ChangeType(VT_R4, src2)))
			{
				p->x = p1->x + v.fltVal;
				p->y = p1->y + v.fltVal;
				p->z = p1->z + v.fltVal;
				p->w = p1->w + v.fltVal;
				return true;
			}
			else if (src2->vt==VT_DISPATCH)
			{
				D3DXVECTOR4* p2 = DISP_CAST(src2->pdispVal, D3DXVECTOR4);
				if (p2==NULL) return false;
				p->x = p1->x + p2->x;
				p->y = p1->y + p2->y;
				p->z = p1->z + p2->z;
				p->w = p1->w + p2->w;
				return true;
			}
		}
		else if (adt==Plane::__ClassName())
		{
			D3DXPLANE* p = DISP_CAST(pdispVal, D3DXPLANE);
			D3DXPLANE* p1 = DISP_CAST(src->pdispVal, D3DXPLANE);
			if (p==NULL || p1==NULL) return false;
			if (SUCCEEDED(v.ChangeType(VT_R4, src2)))
			{
				p->a = p1->a + v.fltVal;
				p->b = p1->b + v.fltVal;
				p->c = p1->c + v.fltVal;
				p->d = p1->d + v.fltVal;
				return true;
			}
			else if (src2->vt==VT_DISPATCH)
			{
				D3DXPLANE* p2 = DISP_CAST(src2->pdispVal, D3DXPLANE);
				if (p2==NULL) return false;
				p->a = p1->a + p2->a;
				p->b = p1->b + p2->b;
				p->c = p1->c + p2->c;
				p->d = p1->d + p2->d;
				return true;
			}
		}
		else if (adt==COLOR::__ClassName())
		{
			COLOR* p = DISP_CAST(pdispVal, COLOR);
			COLOR* p1 = DISP_CAST(src->pdispVal, COLOR);
			if (p==NULL || p1==NULL) return false;
			if (SUCCEEDED(v.ChangeType(VT_R4, src2)))
			{
				p->r = p1->r + v.fltVal;
				p->g = p1->g + v.fltVal;
				p->b = p1->b + v.fltVal;
				p->a = p1->a + v.fltVal;
				p->OnRGBAChanged();
				return true;
			}
			else if (src2->vt==VT_DISPATCH)
			{
				COLOR* p2 = DISP_CAST(src2->pdispVal, COLOR);
				if (p2==NULL) return false;
				p->r = p1->r + p2->r;
				p->g = p1->g + p2->g;
				p->b = p1->b + p2->b;
				p->a = p1->a + p2->a;
				p->OnRGBAChanged();
				return true;
			}
		}
		else if (adt==Quaternion::__ClassName())
		{
			D3DXQUATERNION* p = DISP_CAST(pdispVal, D3DXQUATERNION);
			D3DXQUATERNION* p1 = DISP_CAST(src->pdispVal, D3DXQUATERNION);
			if (p==NULL || p1==NULL) return false;
			if (SUCCEEDED(v.ChangeType(VT_R4, src2)))
			{
				p->x = p1->x + v.fltVal;
				p->y = p1->y + v.fltVal;
				p->z = p1->z + v.fltVal;
				p->w = p1->w + v.fltVal;
				return true;
			}
			else if (src2->vt==VT_DISPATCH)
			{
				D3DXQUATERNION* p2 = DISP_CAST(src2->pdispVal, D3DXQUATERNION);
				if (p2==NULL) return false;
				p->x = p1->x + p2->x;
				p->y = p1->y + p2->y;
				p->z = p1->z + p2->z;
				p->w = p1->w + p2->w;
				return true;
			}
		}
	}

	return false;
}

ULONG iAttributeData::GetSplineDim(LPCOLESTR type)
{
	if (type==NULL) return 0;

	if (type==(LPCOLESTR)VT_I4 || (((DWORD_PTR)type&0xffff0000) && (lstrcmpiW(type, L"number")==0 || lstrcmpiW(type, L"int")==0))) return 1;
	//else if (type==(LPCOLESTR)VT_UI4 || (((DWORD_PTR)type&0xffff0000) && (lstrcmpiW(type, L"colorref")==0 || lstrcmpiW(type, L"dword")==0))) return 4;
	else if (type==(LPCOLESTR)VT_R4 || (((DWORD_PTR)type&0xffff0000) && (lstrcmpiW(type, L"float")==0))) return 1;
	else if (type==(LPCOLESTR)VT_R8 || (((DWORD_PTR)type&0xffff0000) && (lstrcmpiW(type, L"double")==0))) return 1;
	else if (type==Vector2D::__ClassName() || (((DWORD_PTR)type&0xffff0000) && (lstrcmpiW(type, L"Vector2D")==0))) return 2;
	else if (type==Vector3D::__ClassName() || (((DWORD_PTR)type&0xffff0000) && (lstrcmpiW(type, L"Vector3D")==0))) return 3;
	else if (type==Vector4D::__ClassName() || (((DWORD_PTR)type&0xffff0000) && (lstrcmpiW(type, L"Vector4D")==0))) return 4;
	else if (type==Plane::__ClassName() || (((DWORD_PTR)type&0xffff0000) && (lstrcmpiW(type, L"Plane")==0))) return 4;
	else if (type==COLOR::__ClassName() || (((DWORD_PTR)type&0xffff0000) && (lstrcmpiW(type, L"COLOR")==0))) return 4;
	else if (type==Quaternion::__ClassName() || (((DWORD_PTR)type&0xffff0000) && (lstrcmpiW(type, L"Quaternion")==0))) return 4;
	else return 0;
}

bool iAttributeData::SetSplineSample( float* s0, float* s1/*=NULL*/, float* s2/*=NULL*/, float* s3/*=NULL*/ )
{
	if (adt==NULL) return false;

	if (vt==VT_I4 && s0) return (*s0=(float)lVal), true;
	//else if (vt==VT_UI4 && s0 && s1 && s2 && s3)
	//{
	//	*s0 = (float)GetRValue(ulVal);
	//	*s1 = (float)GetGValue(ulVal);
	//	*s2 = (float)GetBValue(ulVal);
	//	*s3 = (float)(LOBYTE((ulVal)>>24));
	//	return true;
	//}
	else if (vt==VT_R4 && s0) return (*s0 = fltVal), true;
	else if (vt==VT_R8 && s0) return (*s0 = (float)dblVal), true;
	else if (vt==VT_DISPATCH)
	{
		if (adt==Vector2D::__ClassName() && s0 && s1)
		{
			D3DXVECTOR2* p = DISP_CAST(pdispVal, D3DXVECTOR2);
			if (p==NULL) return false;
			*s0 = p->x;
			*s1 = p->y;
			return true;
		}
		else if (adt==Vector3D::__ClassName() && s0 && s1 && s2)
		{
			D3DVECTOR* p = DISP_CAST(pdispVal, D3DVECTOR);
			if (p==NULL) return false;
			*s0 = p->x;
			*s1 = p->y;
			*s2 = p->z;
			return true;
		}
		else if (adt==Vector4D::__ClassName() && s0 && s1 && s2 && s3)
		{
			D3DXVECTOR4* p = DISP_CAST(pdispVal, D3DXVECTOR4);
			if (p==NULL) return false;
			*s0 = p->x;
			*s1 = p->y;
			*s2 = p->z;
			*s3 = p->w;
			return true;
		}
		else if (adt==Plane::__ClassName() && s0 && s1 && s2 && s3)
		{
			D3DXPLANE* p = DISP_CAST(pdispVal, D3DXPLANE);
			if (p==NULL) return false;
			*s0 = p->a;
			*s1 = p->b;
			*s2 = p->c;
			*s3 = p->d;
			return true;
		}
		else if (adt==COLOR::__ClassName() && s0 && s1 && s2 && s3)
		{
			D3DCOLORVALUE* p = DISP_CAST(pdispVal, D3DCOLORVALUE);
			if (p==NULL) return false;
			*s0 = p->r;
			*s1 = p->g;
			*s2 = p->b;
			*s3 = p->a;
			return true;
		}
		else if (adt==Quaternion::__ClassName() && s0 && s1 && s2 && s3)
		{
			D3DXQUATERNION* p = DISP_CAST(pdispVal, D3DXQUATERNION);
			if (p==NULL) return false;
			*s0 = p->x;
			*s1 = p->y;
			*s2 = p->z;
			*s3 = p->w;
			return true;
		}
	}
	return false;
}
//////////////////////////////////////////////////////////////////////////
iKeyFrameSet::iKeyFrameSet(iStoryBoard* sb/* =NULL */) : iFrameSetBase(sb)
		, m_keyframes(NULL)
		, m_spline(false)
{
	m_cubics[0] = m_cubics[1] = m_cubics[2] = m_cubics[3] = NULL;
}

iKeyFrameSet::~iKeyFrameSet()
{
	_clearSpline();
	if (m_keyframes) m_keyframes->removeAll();
}

void iKeyFrameSet::_clearSpline()
{
	if (m_cubics[0]) m_cubics[0] = (delete[] m_cubics[0], NULL);
	if (m_cubics[1]) m_cubics[1] = (delete[] m_cubics[1], NULL);
	if (m_cubics[2]) m_cubics[2] = (delete[] m_cubics[2], NULL);
	if (m_cubics[3]) m_cubics[3] = (delete[] m_cubics[3], NULL);

	if (m_keyframes) m_keyframes->setSpline(0, 0);
}

void iKeyFrameSet::step( float fTimeDelta )
{
	if (m_datatype==NULL) return; // 还没设置任何数据类型，无效动画
	float duration = get_duration();
	if (duration<=0) return; // 还没有关键帧，或者关键帧时间设置不正确
	if (__runtime.finished && !m_story->m_stop_pending) return;

	float current_time = m_story->m_current_time - m_delay;
	if (current_time<0 && !m_story->m_stop_pending) return;

	// 如果需要翻转，则周期要加倍
	float rev_duration = duration;
	if (m_reverse)
	{
		// 只有翻转模式才能使用闭合，闭合模式下最后一帧返回到第一帧的时间由第一帧保存，必须大于最后一帧的时间，否则默认为 lastframe->time + 1.0f
		if (m_closure)
		{
			if (m_keyframes->m_keytime <= duration) m_keyframes->m_keytime = duration + 1.0f;
			rev_duration = m_keyframes->m_keytime;
		}
		else rev_duration *= 2;
	}
	float total_duration = rev_duration + m_idle; // 增加重复前要暂停的时间

	if (m_target_attribute==NULL) return;
	CComDispatchDriver obj = _get_object();
	if (obj.p == NULL) return;

	iAttributeData v;
	if (m_story->m_stop_pending)
	{
		v.Calc(&__runtime.vCurr, &__runtime.vFinished, (m_story->m_current_time)/(m_story->m_stop_time));
		_set_property(&v);
		return;
	}

	// 已完成次数
	__runtime.count = (ULONG)((current_time + m_idle)/total_duration);
	if (m_repeat>0 && __runtime.count>=m_repeat)
	{
		if (!__runtime.finished)
		{
			// 最后一次设置对象属性
			iKeyFrame* kf = m_reverse ? m_keyframes : (iKeyFrame*)m_keyframes->getAt(m_keyframes->count()-1);
			v.Calc(&kf->m_keyvalue, NULL, 0);
			_set_property(&v);
		}
		__runtime.finished = true;
		return;
	}

	// 计算时间余数
	float rest = current_time - total_duration * __runtime.count;
	if (rest > rev_duration)
	{
		// 在 IDLE 阶段，不执行任何属性设置操作，但要复位翻转状态
		if (m_reverse) __runtime.reversing = false;
		return;
	}
	if (m_reverse)
	{
		__runtime.reversing = (rest>duration);
		if (__runtime.reversing)
		{
			//if (m_closure) rest -= duration;
			//else 
			if (!m_closure)
				rest = rev_duration - rest;
		}
	}

	// 计算命中哪个关键帧
	// 第一个关键帧的时间有特殊含义，仅当使用闭合模式时才有效，代表最后一帧直接返回到第一帧（并非原路返回）的时间，其它情况下被忽略。
	// 如果仅一个关键帧，没有任何动画。
	if (/*m_keyframes->m_keytime>rest ||*/ m_keyframes->Next()==NULL) return;

	iKeyFrame* kf = m_keyframes, *kf2=NULL;
	if (__runtime.reversing && m_closure)
	{
		kf = (iKeyFrame*)m_keyframes->getAt(m_keyframes->count()-1);
		kf2 = m_keyframes;
	}
	else
	{
		while (kf && kf->Next() && kf->Next()->m_keytime<rest) kf = kf->Next();
		ATLASSERT(kf && kf->Next());
		kf2 = kf->Next();
	}
	float firsttime = kf==m_keyframes ? 0 : kf->m_keytime;
	float percent = (rest - firsttime) / (kf2->m_keytime - firsttime);

	// 计算对象属性值，如果需要则创建样条
	_createSpline();
	v.Calc(&kf->m_keyvalue, &kf2->m_keyvalue, percent, kf->m_cubic[0], kf->m_cubic[1], kf->m_cubic[2], kf->m_cubic[3]);
	_set_property(&v);
}

void iKeyFrameSet::onstopPending()
{
	if (m_story->m_stop_pending && m_keyframes)
	{
		__runtime.vCurr = __runtime.vOld;
		if (m_story->m_stop_behavior==iStoryBoard::stop_begin /*|| m_reverse*/)
			__runtime.vFinished = m_keyframes->m_keyvalue;
		else // if (m_story->m_stop_behavior == iStoryBoard::stop_end)
			__runtime.vFinished = m_keyframes->getAt(m_keyframes->count()-1)->m_keyvalue;
	}

	if (Next()) Next()->onstopPending();
}

void iKeyFrameSet::onstart()
{
	if (m_datatype != NULL && m_keyframes)
	{
		m_keyframes->createAttributeData(m_datatype);
		_createSpline();
	}

	iFrameSetBase::onstart();
}

void iKeyFrameSet::onstop()
{
	if (m_keyframes)
	{
		switch (m_story->m_stop_behavior)
		{
		case iStoryBoard::stop_beginNow:
			_set_property(&m_keyframes->m_keyvalue);
			break;
		case iStoryBoard::stop_endNow:
			{
				iKeyFrame* kf = /*m_reverse ? m_keyframes :*/ (iKeyFrame*)m_keyframes->getAt(m_keyframes->count()-1);
				_set_property(&kf->m_keyvalue);
			}
			break;
		}
	}

	if (Next()) Next()->onstop();
}

void iKeyFrameSet::_createSpline()
{
	if (m_datatype!=NULL && m_keyframes && m_spline && m_cubics[0]==NULL)
	{
		// 如果需要样条，在这里初始化，只有3个及以上的关键帧才能使用样条
		DWORD num = m_keyframes->count();
		if (num>2)
		{
			ULONG dim = iAttributeData::GetSplineDim(m_datatype);
			if (dim>=1 && dim<=4)
			{
				float* sample[4] = {NULL};
				if (dim>=1) sample[0] = NEW float[num];
				if (dim>=2) sample[1] = NEW float[num];
				if (dim>=3) sample[2] = NEW float[num];
				if (dim>=4) sample[3] = NEW float[num];

				for (ULONG i=0; i<num; i++)
				{
					((iKeyFrame*)m_keyframes->getAt(i))->m_keyvalue.SetSplineSample(sample[0]?&sample[0][i]:NULL, sample[1]?&sample[1][i]:NULL,
						sample[2]?&sample[2][i]:NULL, sample[3]?&sample[3][i]:NULL);
				}

				if (dim>=1) m_cubics[0] = (m_reverse && m_closure) ? calcNaturalCubicClosed(num, sample[0]) : calcNaturalCubic(num, sample[0]);
				if (dim>=2) m_cubics[1] = (m_reverse && m_closure) ? calcNaturalCubicClosed(num, sample[1]) : calcNaturalCubic(num, sample[1]);
				if (dim>=3) m_cubics[2] = (m_reverse && m_closure) ? calcNaturalCubicClosed(num, sample[2]) : calcNaturalCubic(num, sample[2]);
				if (dim>=4) m_cubics[3] = (m_reverse && m_closure) ? calcNaturalCubicClosed(num, sample[3]) : calcNaturalCubic(num, sample[3]);

				m_keyframes->setSpline(num, 0, m_cubics[0], m_cubics[1], m_cubics[2], m_cubics[3]);

				if (sample[3]) delete[] sample[3];
				if (sample[2]) delete[] sample[2];
				if (sample[1]) delete[] sample[1];
				if (sample[0]) delete[] sample[0];
			}
		}
	}
}

//////////////////////////////////////////////////////////////////////////
iFrameSet::iFrameSet( iStoryBoard* sb/*=NULL*/ ) : iFrameSetBase(sb)
		, m_duration(1.f)
		, m_has_from(FALSE), m_has_to(FALSE), m_has_by(FALSE)
{

}

iFrameSet::~iFrameSet()
{

}

void iFrameSet::onstart()
{
	if (m_datatype && (m_has_from || m_has_to || m_has_by)) // From/To/By必须至少要有一个
	{
		// 如果需要，则初始化数据
		// 如果没有FROM，则 FROM = CURRENT
		if (!m_has_from)
		{
			m_from.ClearToZero();
			m_from.adt = NULL;

			CComVariant v;
			HRESULT hr = _get_property(&v);
			if (SUCCEEDED(hr))
			{
				m_from.Create(m_datatype);
				m_from.CopyFrom(&v);
			}
		}
		else m_from.Create(m_datatype);

		if (!m_has_to)
		{
			m_to.ClearToZero();
			m_to.adt = NULL;

			// 如果有BY，则 TO = FROM+BY；如果没有BY，则 TO = CURRENT
			if (m_has_by)
			{
				m_to.Sum(&m_from, &m_by);
			}
			else
			{
				CComVariant v;
				HRESULT hr = _get_property(&v);
				if (SUCCEEDED(hr))
				{
					m_to.Create(m_datatype);
					m_to.CopyFrom(&v);
				}
			}
		}
		else m_to.Create(m_datatype);
	}

	iFrameSetBase::onstart();
}

void iFrameSet::step( float fTimeDelta )
{
	if (m_datatype==NULL || !(m_has_from || m_has_to || m_has_by)) return; // 还没设置任何数据类型，无效动画
	if (m_duration<=0) return; // 时间周期设置不正确
	if (__runtime.finished && !m_story->m_stop_pending) return;

	float current_time = m_story->m_current_time - m_delay;
	if (current_time<0 && !m_story->m_stop_pending) return;

	// 如果需要翻转，则周期要加倍
	float rev_duration = m_duration;
	if (m_reverse) rev_duration *= 2;
	float total_duration = rev_duration + m_idle;

	if (m_target_attribute==NULL) return;
	CComDispatchDriver obj = _get_object();
	if (obj.p == NULL) return;

	iAttributeData v;
	if (m_story->m_stop_pending)
	{
		v.Calc(&__runtime.vCurr, &__runtime.vFinished, (m_story->m_current_time)/(m_story->m_stop_time));
		_set_property(&v);
		return;
	}

	// 已完成次数
	__runtime.count = (ULONG)((current_time + m_idle)/total_duration);
	if (m_repeat>0 && __runtime.count>=m_repeat)
	{
		if (!__runtime.finished)
		{
			// 最后一次设置对象属性
			iAttributeData* ad = m_reverse ? &m_from : &m_to;
			v.Calc(ad, NULL, 0);
			_set_property(&v);
		}
		__runtime.finished = true;
		return;
	}

	// 计算时间余数
	float rest = current_time - total_duration * __runtime.count;
	if (rest>2*m_duration) // IDLE
	{
		if (m_reverse) __runtime.reversing = false;
		return;
	}
	if (m_reverse)
	{
		__runtime.reversing = (rest > m_duration);
		if (__runtime.reversing) rest = 2*m_duration - rest;
	}

	// 计算对象属性值
	v.Calc(&m_from, &m_to, rest / m_duration);
	_set_property(&v);
}

void iFrameSet::onstopPending()
{
	if (m_story->m_stop_pending)
	{
		__runtime.vCurr = __runtime.vOld;
		if (m_story->m_stop_behavior==iStoryBoard::stop_begin /*|| m_reverse*/)
			__runtime.vFinished = m_from;
		else
			__runtime.vFinished = m_to;
	}

	if (Next()) Next()->onstopPending();
}

void iFrameSet::onstop()
{
	if (m_story->m_stop_behavior==iStoryBoard::stop_beginNow)
		_set_property(&m_from);
	else if (m_story->m_stop_behavior==iStoryBoard::stop_endNow)
		_set_property(/*m_reverse ? &m_from :*/ &m_to);

	if (Next()) Next()->onstop();
}

//////////////////////////////////////////////////////////////////////////
iKeyFrame::iKeyFrame( iKeyFrameSet* kfs/*=NULL*/ ) : TOrderList<iKeyFrame,true,0,float>(&iKeyFrame::key_time)
		, m_keyframe_set(kfs)
		, m_keytime(0)
{
	m_cubic[0] = m_cubic[1] = m_cubic[2] = m_cubic[3] = NULL;
	if (m_keyframe_set)
		addToList(&m_keyframe_set->m_keyframes, false);
}

void iKeyFrame::createAttributeData( LPCOLESTR type )
{
	m_keyvalue.Create(type);

	if (Next()) Next()->createAttributeData(type);
}

void iKeyFrame::setSpline( ULONG count, ULONG index, CCubic* cubics0/*=NULL*/, CCubic* cubics1/*=NULL*/, CCubic* cubics2/*=NULL*/, CCubic* cubics3/*=NULL*/ )
{
	m_cubic[0] = (cubics0 ? &cubics0[index] : NULL);
	m_cubic[1] = (cubics1 ? &cubics1[index] : NULL);
	m_cubic[2] = (cubics2 ? &cubics2[index] : NULL);
	m_cubic[3] = (cubics3 ? &cubics3[index] : NULL);

	if (Next() && (count==0 || index+1<count))
		Next()->setSpline(count, index+1, cubics0, cubics1, cubics2, cubics3);
}