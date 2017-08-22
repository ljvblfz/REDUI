#include "stdafx.h"
#include "Dui3DAuto.h"
#include "DuiStyleParser.h"

#ifndef minmax
#define minmax(v, minv, maxv) min(max(v, minv), maxv)
#endif // minmax

//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
class Console
{
	static ULONG __count;
public:
	Console()
	{
		if (__count==0)
		{
			AllocConsole();
			SetConsoleTitleW(L"REDirectUI Trace");
			SetConsoleCtrlHandler(ConsoleCtrlProc,TRUE);
		}
		__count++;
	}

	~Console()
	{
		if (--__count == 0)
		{
			SetConsoleCtrlHandler(ConsoleCtrlProc,FALSE);
			FreeConsole();
		}
	}

	static BOOL WINAPI ConsoleCtrlProc(DWORD dwCtrlType)
	{
		//if (dwCtrlType!=CTRL_C_EVENT && dwCtrlType!=CTRL_BREAK_EVENT) // 忽略Ctrl-C Ctrl-Break
		//{
		//	SetConsoleCtrlHandler(ConsoleCtrlProc,FALSE);
		//	FreeConsole();
		//}

		return TRUE; // 防止系统继续处理
	}

	HRESULT InitNamedItem(DISPPARAMS* pdispparams)
	{
		if (pdispparams->cArgs>0)
		{
			DWORD dwWrited;
			for (int i=(int)pdispparams->cArgs-1; i>=0; i--)
			{
				CComVariant v = pdispparams->rgvarg[i];
				if (S_OK == v.ChangeType(VT_BSTR))
					WriteConsoleW(GetStdHandle(STD_OUTPUT_HANDLE), V_BSTR(&v), (DWORD)lstrlenW(V_BSTR(&v)), &dwWrited, NULL);
				if (i>0)
					WriteConsoleW(GetStdHandle(STD_OUTPUT_HANDLE), L",", 1, &dwWrited, NULL);
			}
			WriteConsoleW(GetStdHandle(STD_OUTPUT_HANDLE), L"\n", 1, &dwWrited, NULL);
		}

		return S_OK;
	}

	BOOL __DispidOfName(LPCOLESTR szName, DISPID* pDispid)
	{
		if (lstrcmpiW(szName, L"trace") == 0)
			return *pDispid=1234, TRUE;
		return FALSE;
	}

	HRESULT __Invoke(DISPID dispid, WORD wFlags, DISPPARAMS* pdispparams, VARIANT *pVarResult)
	{
		if (dispid==1234 && (wFlags&DISPATCH_METHOD))
		{
			if (pdispparams->cArgs>0)
			{
				DWORD dwWrited;
				for (int i=(int)pdispparams->cArgs-1; i>=0; i--)
				{
					CComVariant v = pdispparams->rgvarg[i];
					if (S_OK == v.ChangeType(VT_BSTR))
						WriteConsoleW(GetStdHandle(STD_OUTPUT_HANDLE), V_BSTR(&v), (DWORD)lstrlenW(V_BSTR(&v)), &dwWrited, NULL);
					if (i>0)
						WriteConsoleW(GetStdHandle(STD_OUTPUT_HANDLE), L",", 1, &dwWrited, NULL);
				}
				WriteConsoleW(GetStdHandle(STD_OUTPUT_HANDLE), L"\n", 1, &dwWrited, NULL);
			}
			return S_OK;
		}

		return DISP_E_MEMBERNOTFOUND;
	}

	Begin_Auto_Disp_Map(Console)
	End_Disp_Map()
};
Implement_Creatable_NamedItem(Console)

ULONG Console::__count = 0;

//////////////////////////////////////////////////////////////////////////

D3DXMATRIX Matrix::GetIdentityMatrix()
{
	static D3DXMATRIX mtIdentity;
	if (!D3DXMatrixIsIdentity(&mtIdentity))
	{
		::D3DXMatrixIdentity(&mtIdentity);
	}
	return mtIdentity;
}

IDispatch* Matrix::Multiply( VARIANT mtOrFloat )
{
	CComVariant v;
	if (mtOrFloat.vt == VT_DISPATCH)
	{
		D3DXMATRIX* m = DISP_CAST(mtOrFloat.pdispVal, D3DXMATRIX);
		if (m)
		{
			operator *= (*m);
			return GetDispatch();
		}
	}
	else if (SUCCEEDED(v.ChangeType(VT_R4, &mtOrFloat)))
	{
		operator *= (V_R4(&v));
		return GetDispatch();
	}
	return NULL;
}

IDispatch* Matrix::Divide( float f )
{
	operator /= (f);
	return GetDispatch();
}

IDispatch* Matrix::Add( IDispatch* mt )
{
	D3DXMATRIX* m = DISP_CAST(mt, D3DXMATRIX);
	if (m)
	{
		operator += (*m);
		return GetDispatch();
	}
	return NULL;
}

IDispatch* Matrix::Minus( IDispatch* mt )
{
	D3DXMATRIX* m = DISP_CAST(mt, D3DXMATRIX);
	if (m)
	{
		operator -= (*m);
		return GetDispatch();
	}
	return NULL;
}

IDispatch* Matrix::Identity()
{
	D3DXMatrixIdentity(this);
	return GetDispatch();
}

bool Matrix::IsIdentity()
{
	return !!D3DXMatrixIsIdentity(this);
}

IDispatch* Matrix::Inverse( float Determinant )
{
	D3DXMatrixInverse(this, &Determinant, this);
	return GetDispatch();
}

IDispatch* Matrix::MultiplyTranspose( IDispatch* mt )
{
	D3DXMATRIX* m = DISP_CAST(mt, D3DXMATRIX);
	if (m)
	{
		D3DXMatrixMultiplyTranspose(this, this, m);
		return GetDispatch();
	}
	return NULL;
}

IDispatch* Matrix::Reflect( IDispatch* plane )
{
	D3DXPLANE* p = DISP_CAST(plane, D3DXPLANE);
	if (p)
	{
		D3DXMatrixReflect(this, p);
		return GetDispatch();
	}
	return NULL;
}

IDispatch* Matrix::RotationX( float Angle )
{
	D3DXMATRIX mt;
	D3DXMatrixRotationX(&mt, D3DXToRadian(Angle));
	operator*= (mt);
	return GetDispatch();
}

IDispatch* Matrix::RotationY( float Angle )
{
	D3DXMATRIX mt;
	D3DXMatrixRotationY(&mt, D3DXToRadian(Angle));
	operator*= (mt);
	return GetDispatch();
}

IDispatch* Matrix::RotationZ( float Angle )
{
	D3DXMATRIX mt;
	D3DXMatrixRotationZ(&mt, D3DXToRadian(Angle));
	operator*= (mt);
	return GetDispatch();
}

IDispatch* Matrix::RotationAxis( IDispatch* axis, float Angle )
{
	D3DXVECTOR3* v = DISP_CAST(axis, D3DXVECTOR3);
	if (v)
	{
		D3DXMATRIX mt;
		D3DXMatrixRotationAxis(&mt, v, D3DXToRadian(Angle));
		operator*= (mt);
		return GetDispatch();
	}
	return NULL;
}

IDispatch* Matrix::RotationQuaternion( IDispatch* q )
{
	D3DXQUATERNION* qq = DISP_CAST(q, D3DXQUATERNION);
	if (qq)
	{
		D3DXMATRIX mt;
		D3DXMatrixRotationQuaternion(&mt, qq);
		operator*= (mt);
		return GetDispatch();
	}
	return NULL;
}

IDispatch* Matrix::RotationYawPitchRoll( float yYaw, float xPitch, float zRoll )
{
	D3DXMATRIX mt;
	D3DXMatrixRotationYawPitchRoll(&mt, D3DXToRadian(yYaw), D3DXToRadian(xPitch), D3DXToRadian(zRoll));
	operator*= (mt);
	return GetDispatch();
}

IDispatch* Matrix::Scaling( float sx, float sy, float sz )
{
	D3DXMATRIX mt;
	D3DXMatrixScaling(&mt, sx, sy, sz);
	operator*= (mt);
	return GetDispatch();
}

IDispatch* Matrix::Translation( float x, float y, float z )
{
	D3DXMATRIX mt;
	D3DXMatrixTranslation(&mt, x, y, z);
	operator*= (mt);
	return GetDispatch();
}

IDispatch* Matrix::Transpose()
{
	D3DXMatrixTranspose(this, this);
	return GetDispatch();
}

IDispatch* Matrix::Shadow( IDispatch* light, IDispatch* plane )
{
	D3DXVECTOR4* l = DISP_CAST(light, D3DXVECTOR4);
	D3DXPLANE* p = DISP_CAST(plane, D3DXPLANE);
	if (l && p)
	{
		D3DXMATRIX mt;
		D3DXMatrixShadow(&mt, l, p);
		operator*= (mt);
		return GetDispatch();
	}
	return NULL;
}

IDispatch* Matrix::LookAtLH( IDispatch* eye, IDispatch* at, IDispatch* up )
{
	D3DXVECTOR3* vEye = DISP_CAST(eye, D3DXVECTOR3);
	D3DXVECTOR3* vAt = DISP_CAST(at, D3DXVECTOR3);
	D3DXVECTOR3* vUp = DISP_CAST(up, D3DXVECTOR3);
	if (vEye && vAt && vUp)
	{
		D3DXMATRIX mt;
		D3DXMatrixLookAtLH(&mt, vEye, vAt, vUp);
		operator*= (mt);
		return GetDispatch();
	}
	return NULL;
}

IDispatch* Matrix::LookAtRH( IDispatch* eye, IDispatch* at, IDispatch* up )
{
	D3DXVECTOR3* vEye = DISP_CAST(eye, D3DXVECTOR3);
	D3DXVECTOR3* vAt = DISP_CAST(at, D3DXVECTOR3);
	D3DXVECTOR3* vUp = DISP_CAST(up, D3DXVECTOR3);
	if (vEye && vAt && vUp)
	{
		D3DXMATRIX mt;
		D3DXMatrixLookAtRH(&mt, vEye, vAt, vUp);
		operator*= (mt);
		return GetDispatch();
	}
	return NULL;
}

float Matrix::Determinant()
{
	return D3DXMatrixDeterminant(this);
}

//////////////////////////////////////////////////////////////////////////
HRESULT Vector2D::InitFromString( LPCOLESTR szInit )
{
	if (szInit)
	{
		CStrArray stds;
		if (SplitStringToArray(szInit, stds, L" \t\r\n,;"))
		{
			if (stds.GetSize()>=1) TryLoad_float_FromString(stds[0], x);
			if (stds.GetSize()>=2) TryLoad_float_FromString(stds[1], y);
			return S_OK;
		}
	}
	return E_FAIL;
}

HRESULT Vector3D::InitFromString( LPCOLESTR szInit )
{
	if (szInit)
	{
		CStrArray stds;
		if (SplitStringToArray(szInit, stds, L" \t\r\n,;"))
		{
			if (stds.GetSize()>=1) TryLoad_float_FromString(stds[0], x);
			if (stds.GetSize()>=2) TryLoad_float_FromString(stds[1], y);
			if (stds.GetSize()>=3) TryLoad_float_FromString(stds[2], z);
			return S_OK;
		}
	}
	return E_FAIL;
}

HRESULT Vector4D::InitFromString( LPCOLESTR szInit )
{
	if (szInit)
	{
		CStrArray stds;
		if (SplitStringToArray(szInit, stds, L" \t\r\n,;"))
		{
			if (stds.GetSize()>=1) TryLoad_float_FromString(stds[0], x);
			if (stds.GetSize()>=2) TryLoad_float_FromString(stds[1], y);
			if (stds.GetSize()>=3) TryLoad_float_FromString(stds[2], z);
			if (stds.GetSize()>=4) TryLoad_float_FromString(stds[3], w);
			return S_OK;
		}
	}
	return E_FAIL;
}

HRESULT Plane::InitFromString( LPCOLESTR szInit )
{
	if (szInit)
	{
		CStrArray stds;
		if (SplitStringToArray(szInit, stds, L" \t\r\n,;"))
		{
			if (stds.GetSize()>=1) TryLoad_float_FromString(stds[0], a);
			if (stds.GetSize()>=2) TryLoad_float_FromString(stds[1], b);
			if (stds.GetSize()>=3) TryLoad_float_FromString(stds[2], c);
			if (stds.GetSize()>=4) TryLoad_float_FromString(stds[3], d);
			return S_OK;
		}
	}
	return E_FAIL;
}

COLOR::~COLOR()
{
	gConnector.Disconnect(NULL, this);
}

HRESULT COLOR::InitFromString( LPCOLESTR szInit )
{
	if (szInit)
	{
		if (TryLoad_COLORREF_FromString(szInit, clr))
		{
			OnCOLORREFChanged();
			return S_OK;
		}
		CStrArray stds;
		if (SplitStringToArray(szInit, stds, L" \t\r\n,;"))
		{
			if (stds.GetSize()>=1) TryLoad_float_FromString(stds[0], r);
			if (stds.GetSize()>=2) TryLoad_float_FromString(stds[1], g);
			if (stds.GetSize()>=3) TryLoad_float_FromString(stds[2], b);
			if (stds.GetSize()>=4) TryLoad_float_FromString(stds[3], a);
			OnRGBAChanged();
			return S_OK;
		}
	}
	return E_FAIL;
}

void COLOR::_rgb2colorref()
{
	clr = RGB((BYTE)(r*255), (BYTE)(g*255), (BYTE)(b*255)) | (((BYTE)(a*255))<<24);
}

void COLOR::_colorref2rgb()
{
	r = (float)(GetRValue(clr)/255);
	g = (float)(GetGValue(clr)/255);
	b = (float)(GetBValue(clr)/255);
	a = (float)((clr>>24)/255);
}

void COLOR::_rgb2yuv()
{
	y =  0.299f * /*255 **/ r + 0.587f * /*255 **/ g + 0.114f * /*255 **/ b;
	u = -0.14713f * /*255 **/ r - 0.28886f * /*255 **/ g + 0.436f * /*255 **/ b;
	v =  0.615f * /*255 **/ r - 0.51499f * /*255 **/ g - 0.10001f * /*255 **/ b;
}

void COLOR::_yuv2rgb()
{
	r = minmax((y + 1.13983f * v) /*/ 255*/, 0, 1.f);
	g = minmax((y - 0.39465f * u - 0.58060f * v) /*/ 255*/, 0, 1.f);
	b = minmax((y + 2.03211f * u) /*/ 255*/, 0, 1.f);
}

void COLOR::OnPropertyChanged( DISPID dispid, LPCOLESTR name )
{
	switch(dispid)
	{
	case 1:
	case 2:
	case 3:
		OnRGBAChanged();
		break;
	case 5:
	case 6:
	case 7:
		OnYUVChanged();
		break;
	case 8:
		OnCOLORREFChanged();
		break;
	}
	gConnector.Fire(NULL, this);
}

void COLOR::OnRGBAChanged()
{
	_rgb2colorref();
	_rgb2yuv();
}

void COLOR::OnCOLORREFChanged()
{
	_colorref2rgb();
	_rgb2yuv();
}

void COLOR::OnYUVChanged()
{
	_yuv2rgb();
	_rgb2colorref();
}

HRESULT Quaternion::InitFromString( LPCOLESTR szInit )
{
	if (szInit)
	{
		CStrArray stds;
		if (SplitStringToArray(szInit, stds, L" \t\r\n,;"))
		{
			if (stds.GetSize()>=1) TryLoad_float_FromString(stds[0], x);
			if (stds.GetSize()>=2) TryLoad_float_FromString(stds[1], y);
			if (stds.GetSize()>=3) TryLoad_float_FromString(stds[2], z);
			if (stds.GetSize()>=4) TryLoad_float_FromString(stds[3], w);
			return S_OK;
		}
	}
	return E_FAIL;
}

//////////////////////////////////////////////////////////////////////////
FONT::FONT()
{
	::GetObject((HFONT)::GetStockObject(DEFAULT_GUI_FONT), sizeof(LOGFONTW), this);
}

void FONT::OnPropertyChanged( DISPID dispid, LPCOLESTR name )
{
	typedef WCHAR fface[LF_FACESIZE];

	if (dispid==1 && Name)
	{
		lstrcpynW(lfFaceName, Name, LF_FACESIZE);
		Validate();
	}
}

void FONT::Validate()
{
	static CStrArray ttf;
	if (ttf.GetSize()==0)
	{
		class EnumFontHelper
		{
		public:
			static int CALLBACK EnumProc(LPLOGFONTW lplf, LPNEWTEXTMETRIC lpntm, DWORD FontType, CStrArray* pttf)
			{
				if (FontType==TRUETYPE_FONTTYPE)
					pttf->Add(lplf->lfFaceName);
				return 1;
			}
		};

		HDC hdc = ::GetDC(NULL);
		::EnumFontFamiliesW(hdc, NULL, (FONTENUMPROC)EnumFontHelper::EnumProc, (LPARAM)&ttf);
		::ReleaseDC(NULL, hdc);
	}

	for (int i=0; i<ttf.GetSize(); i++)
	{
		if (ttf[i].CompareNoCase(lfFaceName)==0) return;
	}

	// 寻找一个合适的TTF字体
	lstrcpyW(lfFaceName, L"Arial");
}

//////////////////////////////////////////////////////////////////////////
Transform3D::Transform3D() : m_pre(NULL), m_post(NULL), m_dirty(true)
{
	ZeroMemory(&m_factor, sizeof(TransfromFactor)); m_factor.m_scale=D3DXVECTOR3(1.f,1.f,1.f);
}

Transform3D::~Transform3D()
{
	if (m_pre) m_pre = (delete m_pre, NULL);
	if (m_post) m_post = (delete m_post, NULL);
}

bool Transform3D::_isDirty()
{
	return m_dirty ||
			(m_pre && m_pre->_isDirty()) ||
			(m_post && m_post->_isDirty());
}

void Transform3D::Update()
{
	m_dirty = true;
}

void Transform3D::Reset()
{
	if (m_pre) m_pre = (delete m_pre, NULL);
	if (m_post) m_post = (delete m_post, NULL);
	ZeroMemory(&m_factor, sizeof(TransfromFactor));
	m_factor.m_scale=D3DXVECTOR3(1.f,1.f,1.f);
	D3DXMatrixIdentity(&m_matrix);
	m_dirty = false;
}

D3DXMATRIX* Transform3D::GetMatrix()
{
	if (!_isDirty()) return &m_matrix;

	D3DXMATRIX mtTmp;

	// 计算前置转换
	if (m_pre)
		m_matrix = *m_pre->GetMatrix();
	else
		D3DXMatrixIdentity(&m_matrix);

	// 计算拉伸。如果三个因子存在0，或者三个因子都是1，则拉伸无效，忽略此种情况
	if ((m_factor.m_scale.x!=0 && m_factor.m_scale.y!=0 && m_factor.m_scale.z!=0) &&
		(m_factor.m_scale.x!=1.0f || m_factor.m_scale.y!=1.0f || m_factor.m_scale.z!=1.0f))
	{
		D3DXMatrixScaling(&mtTmp, m_factor.m_scale.x, m_factor.m_scale.y, m_factor.m_scale.z);
		D3DXMatrixMultiply(&m_matrix, &m_matrix, &mtTmp);
	}
	
	// 计算旋转
	switch (m_factor.m_rot_type)
	{
	case TransfromFactor::rot_x:
		D3DXMatrixRotationX(&mtTmp, D3DXToRadian(m_factor.m_rot_1d));
		D3DXMatrixMultiply(&m_matrix, &m_matrix, &mtTmp);
		break;
	case TransfromFactor::rot_y:
		D3DXMatrixRotationY(&mtTmp, D3DXToRadian(m_factor.m_rot_1d));
		D3DXMatrixMultiply(&m_matrix, &m_matrix, &mtTmp);
		break;
	case TransfromFactor::rot_z:
		D3DXMatrixRotationZ(&mtTmp, D3DXToRadian(m_factor.m_rot_1d));
		D3DXMatrixMultiply(&m_matrix, &m_matrix, &mtTmp);
		break;
	case TransfromFactor::rot_axis:
		D3DXMatrixRotationAxis(&mtTmp, &m_factor.m_rot_3d, D3DXToRadian(m_factor.m_rot_1d));
		D3DXMatrixMultiply(&m_matrix, &m_matrix, &mtTmp);
		break;
	case TransfromFactor::rot_yawpitchroll:
		D3DXMatrixRotationYawPitchRoll(&mtTmp, D3DXToRadian(m_factor.m_rot_3d.x), D3DXToRadian(m_factor.m_rot_3d.y), D3DXToRadian(m_factor.m_rot_3d.z));
		D3DXMatrixMultiply(&m_matrix, &m_matrix, &mtTmp);
		break;
	case TransfromFactor::rot_quaternion:
		D3DXMatrixRotationQuaternion(&mtTmp, &m_factor.m_rot_4d);
		D3DXMatrixMultiply(&m_matrix, &m_matrix, &mtTmp);
		break;
	}

	// 计算平移
	D3DXMatrixTranslation(&mtTmp, m_factor.m_translate.x, m_factor.m_translate.y, m_factor.m_translate.z);
	D3DXMatrixMultiply(&m_matrix, &m_matrix, &mtTmp);

	// 计算后置转换
	if (m_post)
		m_matrix *= *m_post->GetMatrix();

	m_dirty = false;

	return &m_matrix;
}

Transform3D* Transform3D::get_pre()
{
	if (m_pre==NULL)
	{
		m_pre = NEW Transform3D;
	}
	return m_pre;
}

Transform3D* Transform3D::get_post()
{
	if (m_post==NULL)
	{
		m_post = NEW Transform3D;
	}
	return m_post;
}

HRESULT Transform3D::GetPreTransform( VARIANT* val )
{
	get_pre();
	if (m_pre==NULL) return E_OUTOFMEMORY;

	CComVariant v = m_pre->GetDispatch();
	return v.Detach(val);
}

HRESULT Transform3D::GetPostTransform( VARIANT* val )
{
	get_post();
	if (m_post==NULL) return E_OUTOFMEMORY;

	CComVariant v = m_post->GetDispatch();
	return v.Detach(val);
}

void Transform3D::OnPropertyChanged( DISPID dispid, LPCOLESTR name )
{
	m_dirty = true;
	switch(dispid)
	{
	case 10/*RotationX*/: m_factor.m_rot_type = TransfromFactor::rot_x; break;
	case 11/*RotationY*/: m_factor.m_rot_type = TransfromFactor::rot_y; break;
	case 12/*RotationZ*/: m_factor.m_rot_type = TransfromFactor::rot_z; break;
	case 13/*RotationAngle,		*/: m_factor.m_rot_type = TransfromFactor::rot_axis; break;
	case 14/*RotationAxis,		*/: m_factor.m_rot_type = TransfromFactor::rot_axis; break;
	case 15/*RotationQuaternion, */: m_factor.m_rot_type = TransfromFactor::rot_quaternion; break;
	case 16/*RotationYaw,		*/: m_factor.m_rot_type = TransfromFactor::rot_yawpitchroll; break;
	case 17/*RotationPitch,		*/: m_factor.m_rot_type = TransfromFactor::rot_yawpitchroll; break;
	case 18/*RotationRoll,		*/: m_factor.m_rot_type = TransfromFactor::rot_yawpitchroll; break;
	case 19/*RotationYawPitchRoll,*/: m_factor.m_rot_type = TransfromFactor::rot_yawpitchroll; break;
	}
}

BOOL Transform3D::OnPropertyCanAccess(BOOL bPut, DISPID dispid, LPCOLESTR name)
{
	if (!bPut)
	{
		switch (dispid)
		{
		case 10/*RotationX*/: if (m_factor.m_rot_type != TransfromFactor::rot_x) return FALSE; break;
		case 11/*RotationY*/: if (m_factor.m_rot_type != TransfromFactor::rot_y) return FALSE; break;
		case 12/*RotationZ*/: if (m_factor.m_rot_type != TransfromFactor::rot_z) return FALSE; break;
		case 13/*RotationAngle,		*/:
		case 14/*RotationAxis,		*/: if (m_factor.m_rot_type != TransfromFactor::rot_axis) return FALSE; break;
		case 15/*RotationQuaternion, */: if (m_factor.m_rot_type != TransfromFactor::rot_quaternion) return FALSE; break;
		case 16/*RotationYaw,		*/:
		case 17/*RotationPitch,		*/:
		case 18/*RotationRoll,		*/:
		case 19/*RotationYawPitchRoll,*/: if (m_factor.m_rot_type != TransfromFactor::rot_yawpitchroll) return FALSE; break;
		}
	}
	return TRUE;
}

bool Transform3D::RotateByX( float Angle )
{
	m_factor.m_rot_1d = Angle;
	m_factor.m_rot_type = TransfromFactor::rot_x;
	m_dirty = true;
	return true;
}

bool Transform3D::RotateByY( float Angle )
{
	m_factor.m_rot_1d = Angle;
	m_factor.m_rot_type = TransfromFactor::rot_y;
	m_dirty = true;
	return true;
}

bool Transform3D::RotateByZ( float Angle )
{
	m_factor.m_rot_1d = Angle;
	m_factor.m_rot_type = TransfromFactor::rot_z;
	m_dirty = true;
	return true;
}

bool Transform3D::RotateByAxis( IDispatch* axis, float Angle )
{
	D3DXVECTOR3* vec = DISP_CAST(axis, D3DXVECTOR3);
	if (vec==NULL) return false;
	m_factor.m_rot_3d = *vec;
	m_factor.m_rot_1d = /*D3DXToRadian*/(Angle);
	m_factor.m_rot_type = TransfromFactor::rot_axis;
	m_dirty = true;
	return true;
}

bool Transform3D::RotateByQuaternion( IDispatch* q )
{
	D3DXQUATERNION* qq = DISP_CAST(q, D3DXQUATERNION);
	if (qq==NULL) return false;
	m_factor.m_rot_4d = *qq;
	m_factor.m_rot_type = TransfromFactor::rot_quaternion;
	m_dirty = true;
	return true;
}

bool Transform3D::RotateByYawPitchRoll( float yYaw, float xPitch, float zRoll )
{
	m_factor.m_rot_3d = D3DXVECTOR3(yYaw, xPitch, zRoll);
	m_factor.m_rot_type = TransfromFactor::rot_yawpitchroll;
	m_dirty = true;
	return true;
}

bool Transform3D::Rotate( VARIANT v1, float f2/*=0*/, float f3/*=0*/ )
{
	if (v1.vt == VT_EMPTY) return false;

	if (v1.vt == VT_DISPATCH)
		return RotateByAxis(v1.pdispVal, f2) || RotateByQuaternion(v1.pdispVal);

	CComVariant v;
	if (SUCCEEDED(v.ChangeType(VT_R4, &v1)))
		return RotateByYawPitchRoll(v.fltVal, f2, f3);

	return false;
}

void Transform3D::ParseAttribute( HANDLE hNode )
{
	ATLASSERT(hNode);
	CMarkupNode* n = (CMarkupNode*)hNode;

	InitFromString(n->GetAttributeValue(L"Transform"));

	TryLoad_floatptr_FromString(n->GetAttributeValue(L"Scale"), m_factor.m_scale, 3);
	TryLoad_floatptr_FromString(n->GetAttributeValue(L"Position", L"Translate"), m_factor.m_translate, 3);

	// 旋转只能用一种类型，遇到一个即停止扫描
	BOOL bFound = FALSE;
	if (!bFound && (bFound = TryLoad_float_FromString(n->GetAttributeValue(L"RotationX", L"rX"), m_factor.m_rot_1d)))
		m_factor.m_rot_type = TransfromFactor::rot_x;
	if (!bFound && (bFound = TryLoad_float_FromString(n->GetAttributeValue(L"RotationY", L"rY"), m_factor.m_rot_1d)))
		m_factor.m_rot_type = TransfromFactor::rot_y;
	if (!bFound && (bFound = TryLoad_float_FromString(n->GetAttributeValue(L"RotationZ", L"rZ"), m_factor.m_rot_1d)))
		m_factor.m_rot_type = TransfromFactor::rot_z;
	if (!bFound && (bFound = TryLoad_floatptr_FromString(n->GetAttributeValue(L"RotationAxis", L"Axis"), m_factor.m_rot_3d, 3)))
	{
		m_factor.m_rot_type = TransfromFactor::rot_axis;
		TryLoad_float_FromString(n->GetAttributeValue(L"Angel"), m_factor.m_rot_1d);
	}
	if (!bFound && (bFound = TryLoad_floatptr_FromString(n->GetAttributeValue(L"RotationQuaternion", L"Quaternion", L"Quat"), m_factor.m_rot_4d, 4)))
		m_factor.m_rot_type = TransfromFactor::rot_quaternion;
	if (!bFound && (bFound = TryLoad_floatptr_FromString(n->GetAttributeValue(L"RotationYawPitchRoll", L"YawPitchRoll", L"YPR"), m_factor.m_rot_3d, 3)))
		m_factor.m_rot_type = TransfromFactor::rot_yawpitchroll;

	LPCOLESTR p = n->GetAttributeValue(L"Transform.Pre", L"PreTransform");
	if (p && get_pre()) m_pre->InitFromString(p);
	p = n->GetAttributeValue(L"Transform.Post", L"PostTransform");
	if (p && get_post()) m_post->InitFromString(p);
}

HRESULT Transform3D::InitFromString( LPCOLESTR szInit )
{
	return ObjectInitFromString(GetDispatch(), szInit);
}
