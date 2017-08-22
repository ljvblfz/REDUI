#ifndef __DUI3DAUTO_H__
#define __DUI3DAUTO_H__

//#include "DuiD3D.h"
#define D3D_DISABLE_9EX
#include <d3dx9.h>
//#include "DuiD3DApi.h"
#include "../../common/macro.h"

#define __foreach_object(v) \
	v(Vector2D) \
	v(Vector3D) \
	v(Vector4D) \
	v(Plane) \
	v(COLOR) \
	v(Quaternion) \

#define ZeroConstruct(theClass, baseClass) theClass() { ZeroMemory((baseClass*)this, sizeof(baseClass)); }

//////////////////////////////////////////////////////////////////////////
class Vector2D : public D3DXVECTOR2
{
public:
	ZeroConstruct(Vector2D, D3DXVECTOR2)

	HRESULT InitNamedItem(DISPPARAMS* params)
	{
		DispParams dp(params);
		if (dp.Count()<1 || !dp.GetFloat(0,x)) x=0.0f;
		if (dp.Count()<2 || !dp.GetFloat(1,y)) y=0.0f;

		return S_OK;
	}

	HRESULT InitFromString(LPCOLESTR szInit);
	//HRESULT Getvalue(VARIANT index, VARIANT* val)
	//{
	//	CComVariant v = GetDispatch();
	//	return v.Detach(val);
	//}

	Begin_Auto_Disp_Map(Vector2D)
		Disp_Property(1, x, float)
		Disp_Property(2, y, float)
		//Disp_ValueGet(value)
	End_Disp_Map()
};
Implement_Creatable_NamedItem(Vector2D)
Implement_Ref_Creator(D3DXVECTOR2, Vector2D) // 第二个参数表示参考 Vector2D 的映射表

//////////////////////////////////////////////////////////////////////////
class Vector3D : public D3DXVECTOR3
{
public:
	ZeroConstruct(Vector3D, D3DXVECTOR3)

	HRESULT InitNamedItem(DISPPARAMS* params)
	{
		DispParams dp(params);
		if (dp.Count()<1 || !dp.GetFloat(0,x)) x=0.0f;
		if (dp.Count()<2 || !dp.GetFloat(1,y)) y=0.0f;
		if (dp.Count()<3 || !dp.GetFloat(2,z)) z=0.0f;

		return S_OK;
	}

	HRESULT InitFromString(LPCOLESTR szInit);

	Begin_Auto_Disp_Map(Vector3D)
		Disp_Property(1, x, float)
		Disp_Property(2, y, float)
		Disp_Property(3, z, float)
	End_Disp_Map()
};
Implement_Creatable_NamedItem(Vector3D)
Implement_Ref_Creator(D3DXVECTOR3, Vector3D)
Implement_Ref_Creator(D3DVECTOR, D3DXVECTOR3, Vector3D)

//////////////////////////////////////////////////////////////////////////
class Vector4D : public D3DXVECTOR4
{
public:
	ZeroConstruct(Vector4D, D3DXVECTOR4)

	HRESULT InitNamedItem(DISPPARAMS* params)
	{
		DispParams dp(params);
		if (dp.Count()<1 || !dp.GetFloat(0,x)) x=0.0f;
		if (dp.Count()<2 || !dp.GetFloat(1,y)) y=0.0f;
		if (dp.Count()<3 || !dp.GetFloat(2,z)) z=0.0f;
		if (dp.Count()<4 || !dp.GetFloat(3,w)) w=0.0f;

		return S_OK;
	}

	HRESULT InitFromString(LPCOLESTR szInit);

	Begin_Auto_Disp_Map(Vector4D)
		Disp_Property(1, x, float)
		Disp_Property(2, y, float)
		Disp_Property(3, z, float)
		Disp_Property(4, w, float)
	End_Disp_Map()
};
Implement_Creatable_NamedItem(Vector4D)
Implement_Ref_Creator(D3DXVECTOR4, Vector4D)

//////////////////////////////////////////////////////////////////////////
class Plane : public D3DXPLANE
{
public:
	ZeroConstruct(Plane, D3DXPLANE)

	HRESULT InitNamedItem(DISPPARAMS* params)
	{
		DispParams dp(params);
		if (dp.Count()<1 || !dp.GetFloat(0,a)) a=0.0f;
		if (dp.Count()<2 || !dp.GetFloat(1,b)) b=0.0f;
		if (dp.Count()<3 || !dp.GetFloat(2,c)) c=0.0f;
		if (dp.Count()<4 || !dp.GetFloat(3,d)) d=0.0f;

		return S_OK;
	}

	HRESULT InitFromString(LPCOLESTR szInit);

	Begin_Auto_Disp_Map(Plane)
		Disp_Property(1, a, float)
		Disp_Property(2, b, float)
		Disp_Property(3, c, float)
		Disp_Property(4, d, float)
	End_Disp_Map()
};
Implement_Creatable_NamedItem(Plane)
Implement_Ref_Creator(D3DXPLANE, Plane)

//////////////////////////////////////////////////////////////////////////
class COLOR : public D3DCOLORVALUE
{
public:
	//ZeroConstruct(COLOR, D3DCOLORVALUE)
	COLOR() { r=g=b=a=y=u=v=0; clr=0; }
	COLOR(DWORD color) : clr(color) { OnCOLORREFChanged(); }
	~COLOR();

	void operator=(DWORD color) { clr=color; OnCOLORREFChanged(); }
	operator DWORD() { return clr; }

	HRESULT InitNamedItem(DISPPARAMS* params)
	{
		DispParams dp(params);
		if (dp.Count()<1 || !dp.GetFloat(0,r) || ((r>1.0f?r/=255:r),false)) r=0.0f;
		if (dp.Count()<2 || !dp.GetFloat(1,g) || ((g>1.0f?g/=255:g),false)) g=0.0f;
		if (dp.Count()<3 || !dp.GetFloat(2,b) || ((b>1.0f?b/=255:b),false)) b=0.0f;
		if (dp.Count()<4 || !dp.GetFloat(3,a) || ((a>1.0f?a/=255:a),false)) a=0.0f;
		_rgb2yuv();

		return S_OK;
	}

	HRESULT InitFromString(LPCOLESTR szInit);
	void OnPropertyChanged(DISPID dispid, LPCOLESTR name);

	Begin_Auto_Disp_Map(COLOR)
		Disp_Property(1, r, float)
		Disp_Property(2, g, float)
		Disp_Property(3, b, float)
		Disp_Property(4, a, float)
		Disp_Property(5, y, float)
		Disp_Property(6, u, float)
		Disp_Property(7, v, float)
		Disp_Property_Member(8, rgb, clr, ULONG)
	End_Disp_Map()

	void OnRGBAChanged();
	void OnCOLORREFChanged();
	void OnYUVChanged();
private:
	float y,u,v;
	COLORREF clr;
	void _rgb2colorref();
	void _colorref2rgb();
	void _rgb2yuv();
	void _yuv2rgb();
};
Implement_Creatable_NamedItem(COLOR)
Implement_Ref_Creator(D3DCOLORVALUE, COLOR)

// D3DCOLORVALUE
//Begin_Ref_Disp_Map(D3DCOLORVALUE)
//	Disp_Property(1, r, float)
//	Disp_Property(2, g, float)
//	Disp_Property(3, b, float)
//	Disp_Property(4, a, float)
//End_Ref_Disp_Map()

//////////////////////////////////////////////////////////////////////////
class Quaternion : public D3DXQUATERNION
{
public:
	ZeroConstruct(Quaternion, D3DXQUATERNION)

	HRESULT InitNamedItem(DISPPARAMS* params)
	{
		DispParams dp(params);
		if (dp.Count()<1 || !dp.GetFloat(0,x)) x=0.0f;
		if (dp.Count()<2 || !dp.GetFloat(1,y)) y=0.0f;
		if (dp.Count()<3 || !dp.GetFloat(2,z)) z=0.0f;
		if (dp.Count()<4 || !dp.GetFloat(3,w)) w=0.0f;

		return S_OK;
	}

	HRESULT InitFromString(LPCOLESTR szInit);

	Begin_Auto_Disp_Map(Quaternion)
		Disp_Property(1, x, float)
		Disp_Property(2, y, float)
		Disp_Property(3, z, float)
		Disp_Property(4, w, float)
	End_Disp_Map()
};
Implement_Creatable_NamedItem(Quaternion)
Implement_Ref_Creator(D3DXQUATERNION, Quaternion)

//////////////////////////////////////////////////////////////////////////
class Matrix : public D3DXMATRIX
{
public:
	ZeroConstruct(Matrix, D3DXMATRIX)

	static D3DXMATRIX GetIdentityMatrix();

	HRESULT InitNamedItem(DISPPARAMS* params)
	{
		DispParams dp(params);
		if (dp.Count()< 1 || !dp.GetFloat( 0,_11)) _11=1.0f;
		if (dp.Count()< 2 || !dp.GetFloat( 1,_12)) _12=0.0f;
		if (dp.Count()< 3 || !dp.GetFloat( 2,_13)) _13=0.0f;
		if (dp.Count()< 4 || !dp.GetFloat( 3,_14)) _14=0.0f;
		if (dp.Count()< 5 || !dp.GetFloat( 4,_21)) _21=0.0f;
		if (dp.Count()< 6 || !dp.GetFloat( 5,_22)) _22=1.0f;
		if (dp.Count()< 7 || !dp.GetFloat( 6,_23)) _23=0.0f;
		if (dp.Count()< 8 || !dp.GetFloat( 7,_24)) _24=0.0f;
		if (dp.Count()< 9 || !dp.GetFloat( 8,_31)) _31=0.0f;
		if (dp.Count()<10 || !dp.GetFloat( 9,_32)) _32=0.0f;
		if (dp.Count()<11 || !dp.GetFloat(10,_33)) _33=1.0f;
		if (dp.Count()<12 || !dp.GetFloat(11,_34)) _34=0.0f;
		if (dp.Count()<13 || !dp.GetFloat(12,_41)) _41=0.0f;
		if (dp.Count()<14 || !dp.GetFloat(13,_42)) _42=0.0f;
		if (dp.Count()<15 || !dp.GetFloat(14,_43)) _43=0.0f;
		if (dp.Count()<16 || !dp.GetFloat(15,_44)) _44=1.0f;

		return S_OK;
	}

	IDispatch* Multiply(VARIANT mtOrFloat); // this *= mtOrFloat(IDispatch* or float)
	IDispatch* Divide(float f); // this /= f;
	IDispatch* Add(IDispatch* mt); // this += mt(matrix)
	IDispatch* Minus(IDispatch* mt); // this -= mt(matrix)

	IDispatch* Identity				();
	bool       IsIdentity			();
	IDispatch* Inverse				(float Determinant);
	IDispatch* MultiplyTranspose	(IDispatch* mt);	// mt:Matrix
	IDispatch* Reflect				(IDispatch* plane);	// plane:Plane
	IDispatch* RotationX			(float Angle);		// example: angel=15
	IDispatch* RotationY			(float Angle);		// example: angel=15
	IDispatch* RotationZ			(float Angle);		// example: angel=15
	IDispatch* RotationAxis			(IDispatch* axis, float Angle);		// axis:Vector3D
	IDispatch* RotationQuaternion	(IDispatch* q);		// q(Quaternion)
	IDispatch* RotationYawPitchRoll	(float yYaw, float xPitch, float zRoll); // float: radians, 15
	IDispatch* Scaling				(float sx, float sy, float sz); // float: scale factor, 1 is NO-SCALING
	IDispatch* Translation			(float x, float y, float z); // float: offset
	IDispatch* Transpose			();
	IDispatch* Shadow				(IDispatch* light, IDispatch* plane); // light:Vector4D, plane:Plane
	IDispatch* LookAtLH				(IDispatch* eye, IDispatch* at, IDispatch* up);	// eye|at|up:Vector3D
	IDispatch* LookAtRH				(IDispatch* eye, IDispatch* at, IDispatch* up);	// eye|at|up:Vector3D
	float      Determinant			();


	Begin_Auto_Disp_Map(Matrix)
		Disp_Property_Member( 1, m11, _11, float)
		Disp_Property_Member( 2, m12, _12, float)
		Disp_Property_Member( 3, m13, _13, float)
		Disp_Property_Member( 4, m14, _14, float)
		Disp_Property_Member( 5, m21, _21, float)
		Disp_Property_Member( 6, m22, _22, float)
		Disp_Property_Member( 7, m23, _23, float)
		Disp_Property_Member( 8, m24, _24, float)
		Disp_Property_Member( 9, m31, _31, float)
		Disp_Property_Member(10, m32, _32, float)
		Disp_Property_Member(11, m33, _33, float)
		Disp_Property_Member(12, m34, _34, float)
		Disp_Property_Member(13, m41, _41, float)
		Disp_Property_Member(14, m42, _42, float)
		Disp_Property_Member(15, m43, _43, float)
		Disp_Property_Member(16, m44, _44, float)

		Disp_Method(30, Multiply			, IDispatch*, 1, VARIANT)
		Disp_Method(31, Divide				, IDispatch*, 1, float)
		Disp_Method(32, Add					, IDispatch*, 1, IDispatch*)
		Disp_Method(33, Minus				, IDispatch*, 1, IDispatch*)

		Disp_Method(34, Identity			, IDispatch*, 0)
		Disp_Method(35, IsIdentity			, bool		, 0)
		Disp_Method(36, Inverse				, IDispatch*, 1, float)
		Disp_Method(37, MultiplyTranspose	, IDispatch*, 1, IDispatch*)
		Disp_Method(38, Reflect				, IDispatch*, 1, IDispatch*)
		Disp_Method(39, RotationX			, IDispatch*, 1, float)
		Disp_Method(40, RotationY			, IDispatch*, 1, float)
		Disp_Method(41, RotationZ			, IDispatch*, 1, float)
		Disp_Method(42, RotationAxis		, IDispatch*, 2, IDispatch*, float)
		Disp_Method(43, RotationQuaternion	, IDispatch*, 1, IDispatch*)
		Disp_Method(44, RotationYawPitchRoll, IDispatch*, 3, float, float, float)
		Disp_Method(45, Scaling				, IDispatch*, 3, float, float, float)
		Disp_Method(46, Translation			, IDispatch*, 3, float, float, float)
		Disp_Method(47, Transpose			, IDispatch*, 0)
		Disp_Method(48, Shadow				, IDispatch*, 2, IDispatch*, IDispatch*)
		Disp_Method(49, LookAtLH			, IDispatch*, 3, IDispatch*, IDispatch*, IDispatch*)
		Disp_Method(50, LookAtRH			, IDispatch*, 3, IDispatch*, IDispatch*, IDispatch*)
		Disp_Method(51, Determinant			, float		, 0)
	End_Disp_Map()
};
Implement_Creatable_NamedItem(Matrix)
Implement_Ref_Creator(D3DXMATRIX, Matrix)
Implement_Ref_Creator(D3DMATRIX, D3DXMATRIX, Matrix)

//////////////////////////////////////////////////////////////////////////
class Point2D
{
public:
	float x, y;  // [x,y] [u,v]...
	ZeroConstruct(Point2D, Point2D)

	HRESULT InitNamedItem(DISPPARAMS* params)
	{
		DispParams dp(params);
		if (dp.Count()<1 || !dp.GetFloat(0,x)) x=0.0f;
		if (dp.Count()<2 || !dp.GetFloat(1,y)) y=0.0f;

		return S_OK;
	}

	Begin_Auto_Disp_Map(Point2D)
		Disp_Property(1, x, float)
		Disp_Property(2, y, float)
		Disp_Property_Member(1, u, x, float)
		Disp_Property_Member(2, v, y, float)
	End_Disp_Map()
};
Implement_Creatable_NamedItem(Point2D)

//////////////////////////////////////////////////////////////////////////
class Point3D
{
public:
	float x, y, z;  // [x,y,z]
	ZeroConstruct(Point3D, Point3D)

	HRESULT InitNamedItem(DISPPARAMS* params)
	{
		DispParams dp(params);
		if (dp.Count()<1 || !dp.GetFloat(0,x)) x=0.0f;
		if (dp.Count()<2 || !dp.GetFloat(1,y)) y=0.0f;
		if (dp.Count()<3 || !dp.GetFloat(2,z)) z=0.0f;

		return S_OK;
	}

	Begin_Auto_Disp_Map(Point3D)
		Disp_Property(1, x, float)
		Disp_Property(2, y, float)
		Disp_Property(3, z, float)
	End_Disp_Map()
};
Implement_Creatable_NamedItem(Point3D)

//////////////////////////////////////////////////////////////////////////
class FONT : public LOGFONTW
{
public:
	FONT();

	HRESULT InitNamedItem(DISPPARAMS* params)
	{
		return S_OK;
	}

	CComBSTR Name;

	void Validate(); // 验证字体是否TTF字体，如果不是，设定一个默认字体
	
	void OnPropertyChanged(DISPID dispid, LPCOLESTR name);

	Begin_Auto_Disp_Map(FONT)
		Disp_Property_Member(1, Name, Name, CComBSTR)	Disp_Alias(1, FaceName)
		Disp_Property_Member(2, Height, lfHeight, long)
		Disp_Property_Member(3, Width, lfWidth, long)
		Disp_Property_Member(4, Escapement, lfEscapement, long)
		Disp_Property_Member(5, Orientation, lfOrientation, long)
		Disp_Property_Member(6, Weight, lfWeight, long)
		Disp_Property_Member(7, Italic, lfItalic, BYTE)
		Disp_Property_Member(8, Underline, lfUnderline, BYTE)
		Disp_Property_Member(9, StrikeOut, lfStrikeOut, BYTE)
		Disp_Property_Member(10, CharSet, lfCharSet, BYTE)
		Disp_Property_Member(11, OutPrecision, lfOutPrecision, BYTE)
		Disp_Property_Member(12, ClipPrecision, lfClipPrecision, BYTE)
		Disp_Property_Member(13, Quality, lfQuality, BYTE)
		Disp_Property_Member(14, PitchAndFamily, lfPitchAndFamily, BYTE)
	End_Disp_Map()
};
Implement_Creatable_NamedItem(FONT)

//////////////////////////////////////////////////////////////////////////
// 不可创建的矩阵变换对象

class Transform3D //: public D3DXMATRIX
{
public:
	struct TransfromFactor
	{
		// 变换步骤1：拉伸因子
		D3DXVECTOR3 m_scale;

		// 变换步骤2：旋转因子
		// 旋转类型只能使用如下一种，任何旋转设置都将放弃之前的旋转设置
		enum rot_type
		{
			rot_none	= 0,	// 不旋转
			rot_x,				// 绕 X 轴旋转
			rot_y,				// 绕 Y 轴旋转
			rot_z,				// 绕 Z 轴旋转
			rot_axis,			// 绕自定义轴旋转
			rot_yawpitchroll,	// 按 Yaw/Pitch/Roll旋转
			rot_quaternion,		// 按四元数旋转
		};
		rot_type m_rot_type;	// 旋转类型
		float m_rot_1d;			// 旋转类型为 rot_x/rot_y/rot_z/rot_axis 时此值有效
		D3DXVECTOR3 m_rot_3d;	// 旋转类型为 rot_axis/rot_yawpitchroll 时此值有效
		D3DXQUATERNION m_rot_4d;// 旋转类型为 rot_quaternion 时此值有效

		// 变换步骤3：平移因子
		D3DXVECTOR3 m_translate;
	} m_factor; // 变换因子
	D3DXMATRIX m_matrix; // 矩阵缓存

	// 前置转换和后置转换
	Transform3D* m_pre;
	Transform3D* m_post;
	bool m_dirty;

	Transform3D();
	~Transform3D();

	Transform3D* get_pre();
	Transform3D* get_post();

	void ParseAttribute(HANDLE hNode);
	HRESULT InitFromString(LPCOLESTR szInit);

	D3DXMATRIX* GetMatrix(); // 获取变换后的矩阵

	void OnPropertyChanged(DISPID dispid, LPCOLESTR name);
	BOOL OnPropertyCanAccess(BOOL bPut, DISPID dispid, LPCOLESTR name);

	void Update(); // 标记变换内容已发生变换，需要重新计算缓存
	void Reset(); // 复位当前变换，同时删除所有前置和后置变换

	HRESULT GetPreTransform(VARIANT* val);
	HRESULT GetPostTransform(VARIANT* val);

	bool Rotate(VARIANT v1, float f2=0, float f3=0);
	bool RotateByX				(float Angle);		// x:float
	bool RotateByY				(float Angle);		// y:float
	bool RotateByZ				(float Angle);		// z:float
	bool RotateByAxis			(IDispatch* axis, float Angle);		// axis:Vector3D
	bool RotateByQuaternion		(IDispatch* q);		// q(Quaternion)
	bool RotateByYawPitchRoll	(float yYaw, float xPitch, float zRoll); // float: radians, 15

	Begin_Disp_Map(Transform3D)
		Disp_Property_Member(1, Scale, m_factor.m_scale, D3DXVECTOR3)
		Disp_Property_Member(2, Translate, m_factor.m_translate, D3DXVECTOR3)		Disp_Alias(2, Position)

		// 绕 X/Y/Z 轴旋转的属性
		Disp_Property_Member(10, RotationX,			m_factor.m_rot_1d, float)				Disp_Alias(10, rx)
		Disp_Property_Member(11, RotationY,			m_factor.m_rot_1d, float)				Disp_Alias(11, ry)
		Disp_Property_Member(12, RotationZ,			m_factor.m_rot_1d, float)				Disp_Alias(12, rz)
		// 绕自定义轴旋转的属性
		Disp_Property_Member(13, RotationAngle,		m_factor.m_rot_1d, float)				Disp_Alias(13, Angle)
		Disp_Property_Member(14, RotationAxis,		m_factor.m_rot_3d, D3DXVECTOR3)			Disp_Alias(14, Axis)
		// 按四元数旋转的属性
		Disp_Property_Member(15, RotationQuaternion, m_factor.m_rot_4d, D3DXQUATERNION)		Disp_Alias(15, Quaternion) Disp_Alias(15, Quat)
		// 按 Yaw/Pitch/Roll旋转的属性
		Disp_Property_Member(16, RotationYaw,		m_factor.m_rot_3d.x, float)				Disp_Alias(16, Yaw)
		Disp_Property_Member(17, RotationPitch,		m_factor.m_rot_3d.y, float)				Disp_Alias(17, Pitch)
		Disp_Property_Member(18, RotationRoll,		m_factor.m_rot_3d.z, float)				Disp_Alias(18, Roll)
		Disp_Property_Member(19, RotationYawPitchRoll,	m_factor.m_rot_3d, D3DXVECTOR3)		Disp_Alias(19, YawPitchRoll) Disp_Alias(19, YPR)

		// 旋转方式也可用对应方法一次设置完毕
		Disp_Method(30, Rotate				,,)
		Disp_Method(20, RotateByX			, bool, 1, float)
		Disp_Method(21, RotateByY			, bool, 1, float)
		Disp_Method(22, RotateByZ			, bool, 1, float)
		Disp_Method(23, RotateByAxis		, bool, 2, IDispatch*, float)
		Disp_Method(24, RotateByQuaternion	, bool, 1, IDispatch*)
		Disp_Method(25, RotateByYawPitchRoll, bool, 3, float, float, float)

		// 每个变换都拥有一个前置变换和后置变换
		Disp_PropertyGet(100, PreTransform)		Disp_Alias(100, Pre)
		Disp_PropertyGet(101, PostTransform)	Disp_Alias(101, Post)

		Disp_Method(200, Update, void, 0)
		Disp_Method(201, Reset, void, 0)
	End_Disp_Map()

	Begin_Method_Params_Map(Transform3D)
		Method_Params(Rotate, 3, 2, bool, VARIANT, float, float)
	End_Method_Params_Map()

private:
	bool _isDirty();
};

//////////////////////////////////////////////////////////////////////////
// 简单集合类，不可自动化创建，不可写，通常作为成员获取，有Add方法，可直接作为成员变量，方便访问数组 CSimpleArray
//Implement_DispCollection(BoolCollection, bool)
Implement_DispCollection(IntCollection, long)
Implement_DispCollection(FloatCollection, float)
//Implement_DispCollection(Vector2DCollection, D3DXVECTOR2)
Implement_DispCollection(Vector3DCollection, D3DVECTOR)
//Implement_DispCollection(Vector4DCollection, D3DXVECTOR4)
Implement_DispCollection(Point2DCollection, Point2D)
//Implement_DispCollection(Point3DCollection, Point3D) // 用 Vector3D 代替

#endif // __DUI3DAUTO_H__