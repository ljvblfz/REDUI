#ifndef __DUID3D_H__
#define __DUID3D_H__

#include "Dui3DAuto.h"
#include "DirectUI.h"

extern LPCWSTR prop_device;

// forward declaration
class Device;
class SubDevice;
class ImageResource;
class ImageLayer;
class GdiLayer;
class RenderTarget;
class Effect;

class Scene;
class Camera;

//class LightGroup;
struct Light;
	class AmbientLight;
	class PointLight;
	class SpotLight;
	class DirectionLight;
//class LightCollection;

class Material;

class Model;
class XModel;
class BoxModel;
class CylinderModel;
class PolygonModel;
class SphereModel;
class TorusModel;
class TextModel;
class TeapotModel;

class AnimationSet;
class Entity;

class KeyFrame;
class KeyFrameSet;
class FrameSet;
class StoryBoard;


#define disallow_copy_and_assign(x) \
	private: \
		x(const x& xref); \
		void operator=(const x& xref); \
	public:

#define disallow_implicit_constructors(x) \
	protected: x(){} virtual ~x(){} \
	disallow_copy_and_assign(x) \
	public:

#define declare_protect_constructor(x) protected: x(); public:

// ObjectStateType
enum ObjectStateType
{
	ost_visible = 0,
	ost_enable,

	ost_dword_align = 0x7fffffff
};

//////////////////////////////////////////////////////////////////////////
//
typedef enum
{
	ilt_unknown			= 0,

	ilt_normal,		// Image标准模式，左上对齐
	ilt_center,		// Image居中模式
	ilt_tile,		// Image平铺模式
	ilt_stretch,	// Image拉伸模式
	ilt_maxfit,		// Image拉伸模式，最大适合，保持图片宽高比例
	ilt_squares,	// Image九宫格模式

} ImageLayoutType;

typedef enum
{
	FRT_ALL			= 0,
	FRT_IMAGE		= 0x00000001,	// by image.name
	FRT_EFFECT		= 0x00000002,	// by effect.technique
	FRT_STORY		= 0x00000004,	// by story.name
	FRT_MODEL		= 0x00000008,	// by model.name
	FRT_SCENE		= 0x00000010,	// by scene.name
	FRT_ENTITY		= 0x00000020,	// by entity.name

	FRT_FORCE_DWORD	= 0x7fffffff
} FindResourceType;

namespace d3d
{
	bool IsValid(bool bAutoCreate=false);
	HWND SetActiveScriptWindow(HWND hwnd);

	// 这个函数应该在线程结束前调用
	void OnThreadFinish();

	COLORREF KeyColor();
};

//////////////////////////////////////////////////////////////////////////
class GdiPath
{
public:
	GdiPath();

	BOOL IsValid() { return _path_type!=pt_unknown; }
	DWORD Time() { return _timestamp; }
	BOOL Parse(LPCOLESTR strPath);

	BOOL Draw(HDC hdc, RECT rc);

private:
	// RelativePoint 是POINT类型，但 x/y 是相对值，非绝对值。相对方向由 xDir/yDir 指定。参照值运行时从参数传入，以构造真正的点值
	struct RelativePoint 
	{
		union
		{
			struct
			{
				DWORD xDir:1;	// 水平方向。FALSE - 相对于左边；TRUE - 相对于右边。默认 FALSE
				DWORD yDir:1;	// 垂直方向。FALSE - 相对于上边；TRUE - 相对于下边。默认 FALSE
				DWORD xPercent:1;	// fx 值是一个百分比数，此时 x 保存的是float类型，根据传入的宽度指定
				DWORD yPercent:1;	// fy 值是百分比
			};
			DWORD flags;
		};
		union
		{
			LONG x;			// 水平相对值
			float fx;
		};
		union
		{
			LONG y;			// 垂直相对值
			float fy;
		};

		RelativePoint(LONG _x=0, LONG _y=0) : flags(0), x(_x), y(_y) {}
		BOOL Parse(LPCOLESTR szx, LPCOLESTR szy);
		long GetX(const RECT& rc);
		long GetY(const RECT& rc);
	};

	CSimpleArray<RelativePoint> _points;
	CSimpleArray<BYTE> _types;

	DWORD _timestamp;

	enum PathType
	{
		pt_unknown			= 0,
		pt_rectangle,				// rectangle [left [top [right [bottom]]]] (0-2 points) 矩形，如果未指定参数，则效果跟 pt_unknown 相同
		pt_roundrect,				// roundrect [corner_cx [, corner_cy]] (0-1 point) 圆角矩形
		pt_polygon,					// polygon point1 point2 [point3 [... pointN]]  (2-N points) 多边形
		pt_ellipse,					// ellipse [left [top [right [bottom]]]] (0-2 points) 椭圆
		pt_chord,					// chord point1 point2 [left [top [right [bottom]]]] (2-4 points) 椭圆与线段相交
		pt_pie,						// pie point1 point2 [left [top [right [bottom]]]] (2-4 points) 椭圆与两个半径相交
		pt_polydraw,
	};
	PathType _path_type;

	BOOL Draw_Custom(HDC hdc, RECT rc);
	BOOL Draw_Rectangle(HDC hdc, RECT rc);
	BOOL Draw_RoundRect(HDC hdc, RECT rc);
	BOOL Draw_Polygon(HDC hdc, RECT rc);
	BOOL Draw_Ellipse(HDC hdc, RECT rc);
	BOOL Draw_Chord(HDC hdc, RECT rc);
	BOOL Draw_Pie(HDC hdc, RECT rc);
};

// 3D设备
class Device
{
public:
	// Allocator
	static Device* New(HWND hwnd);
	void Dispose();
	void Relayout(); // 窗口正准备导航到新的页面，调用它可以释放已有资源

	// Render one frame, call all RenderTarget::Render and ImageLayer::Render
	void Render();

	void Reset(); // notify that size of window is changed

	// for GDI operations, toplevel
	HDC GetDC();
	void ReleaseDC(HDC hdc);

	// caret for layed window
	BOOL CreateCaret(HBITMAP hBitmap, int nWidth, int nHeight, GdiLayer* pOwner);
	BOOL DestroyCaret();
	BOOL ShowCaret(BOOL bShow);
	BOOL SetCaretPos(int X, int Y);
	BOOL IsBlindTimer(UINT nID);

	BOOL IsLayeredWindow();
	HWND Handle();

	// children window
	void AddChild(HWND hWnd);
	void RemoveChild(HWND hWnd);
	void ResizeChild(HWND hWnd);
	//void UpdateChild(HWND hWnd, HDC hdc, RECT rc);

	HWND GetGhostWindow();

	//void RegisterNamedObject(LPCOLESTR name, IDispatch* disp);
	//void UnRegisterNamedObject(IDispatch* disp);
	void SetWindowObject(IDispatch* disp);
	void SetScriptObject(HANDLE handle);

	//IDispatch* FindModel(LPCOLESTR name);
	Scene* FindScene(LPCOLESTR nameorindex);
	BOOL FindNamedObject(LPCOLESTR name, DWORD findType=FRT_ALL, IDispatch** ppDisp=NULL);

	// XML Parser Filter
	BOOL IsChildNode(HANDLE pNode, IDispatch** ppDisp=NULL);

	disallow_implicit_constructors(Device)

	HRESULT GetStoryBoards(VARIANT* val); // 返回演示板集合
	HRESULT PutNewStoryBoard(VARIANT* val); // 增加一个新的演示板

	// 运行时上下文
	HRESULT GetCurrentScene(VARIANT* val);
	HRESULT GetCurrentEntity(VARIANT* val);
	HRESULT GetCurrentModel(VARIANT* val);
	HRESULT GetCurrentMaterial(VARIANT* val);
	HRESULT GetCurrentEffect(VARIANT* val);
	HRESULT GetCurrentTextureAspectRatio(VARIANT* val); // 获取当前纹理宽高比，如果当前纹理不存在，则返回默认值 1

	Begin_Disp_Map(Device)
		Disp_PropertyGet(10, StoryBoards)	Disp_Alias(10, Stories)
		Disp_PropertyPut(11, NewStoryBoard)
		Disp_PropertyGet(20, CurrentScene)
		Disp_PropertyGet(21, CurrentEntity)
		Disp_PropertyGet(22, CurrentModel)
		Disp_PropertyGet(23, CurrentMaterial)
		Disp_PropertyGet(24, CurrentEffect)
		Disp_PropertyGet(25, CurrentTextureAspectRatio)
	End_Disp_Map()
};

// subdevice
class SubDevice
{
public:
	static SubDevice* New(Device* parent, HWND hwnd);
	void Dispose();
};

// 原始图片资源，可异步加载URL
class ImageResource
{
public:
	// Allocator
	static ImageResource* New(Device* device, LPCOLESTR url, LPCOLESTR name=NULL);
	//void Dispose();

	bool IsLoaded();

	long GetWidth();
	long GetHeight();

	void SetClipRect(LPCRECT pClip=NULL);

	disallow_implicit_constructors(ImageResource)
};

// 2D静态纹理，可能仅包含顶点颜色，也可能包含图片纹理，这个层的大小跟GdiLayer完全相同，用作背景和前景
class ImageLayer
{
public:
	//// Allocator
	//static ImageLayer* New(GdiLayer* owner); // 使用此种方式创建的纹理表面仅用于前景。背景表面必须从 GdiLayer 中直接获取
	//void Dispose();
	void SetColor(COLORREF clrTopLeft, COLORREF clrTopRight=0, COLORREF clrBottomRight=0, COLORREF clrBottomLeft=0);
	void SetImage(ImageResource* img, int imageLayoutType = ilt_normal);
	void Clear();

	// texture is valid
	//bool isValid() {return false;}

	disallow_implicit_constructors(ImageLayer)
};

// 2D动态GDI层表面，可直接执行GDI绘制
class PaintCallback
{
public:
	virtual void Do3DPaint() = 0;
};

class GdiLayer
{
public:
	// Allocator
	static GdiLayer* New(Device* device, LONG x, LONG y, LONG width, LONG height, GdiLayer* parent=NULL);
	void Dispose();
	Device* GetDevice() const;

	void SetPaintCallback(PaintCallback* cbPaint);
	IExtensionContainer* GetExtensionContainer();
	void SetExtensionContainer(IExtensionContainer* pExtContainer);

	// for GDI operations
	HDC GetDC();
	void ReleaseDC(HDC hdc);

	bool Parent(GdiLayer* p, BOOL bOrder=false);
	GdiLayer* Parent();
	void ZIndex(long z);

	// background and foreground
	ImageLayer* Background();
	//void ForegroundCount(ULONG count);
	ImageLayer* Foreground();
	void RemoveForeground(ImageLayer* il);

	void Resize(LONG width, LONG height);
	void Move(LONG left, LONG top, LONG right, LONG bottom);
	void Move(LPCRECT prc);
	void Clip(GdiPath* path); // 用于裁剪自身
	void ClipContent(LPCRECT prcClip); // 用于裁剪子控件
	BOOL State(ObjectStateType ost);
	void State(ObjectStateType ost, BOOL value);
	void SetAsTarget(BOOL bTarget=FALSE);
	void SetScene(Scene* scene);
	void SetName(LPCOLESTR name);
	void BindWindow(HWND hwnd, IPropertyBag* pb=NULL/*, BOOL bTransparent=FALSE, LPCOLESTR anotherWinClassName=NULL, long lRenderType=0*/);

	void Invalidate(BOOL bDrawNow=FALSE);

	Begin_Disp_Map(GdiLayer)
		//Disp_PropertyGet(1, Camera, Camera)
		//Disp_PropertyGet(2, AmbientLight, AmbientLight)
		//Disp_PropertyGet(3, Lights)
		//Disp_PropertyGet(4, Entities)
		//Disp_PropertyPut(11, NewEntity)
	End_Disp_Map()

	disallow_implicit_constructors(GdiLayer)
};

// 渲染目标
//class RenderTarget
//{
//public:
//	// Allocator
//	static RenderTarget* New(Device* device, LONG width, LONG height);
//	virtual void Dispose();
//
//	// viewport
//	//void setViewport(ULONG x, ULONG y, ULONG w, ULONG h);
//	//void setViewport(const RECT& rc);
//
//
//	disallow_implicit_constructors(RenderTarget)
//};

// 特效，可异步下载

class Effect
{
public:
	// Allocator
	static Effect* New(Device* device, LPCOLESTR url=NULL, LPCOLESTR name=NULL, LPCOLESTR data=NULL);
	//void Dispose();

	bool IsLoaded();

	// 仅处理当前技术的可用参数，作为属性读写
	BOOL __DispidOfName(LPCOLESTR szName, DISPID* pDispid);
	HRESULT __Invoke(DISPID dispid, WORD wFlags, DISPPARAMS* pdispparams, VARIANT *pVarResult);

	Begin_Disp_Map(Effect)
	End_Disp_Map()

	disallow_implicit_constructors(Effect)
};

//////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////// 
////////////////////////////////////////////////////////////////////////// 
// 以下用于3D场景
// 
// 场景用途，跟 GdiLayer 绑定时用于哪个层
enum SceneUsage
{
	SU_NONE				= 0,	// 场景无效
	SU_BACKGROUND		,		// 场景用于替换背景层
	SU_PRECONTENT		,		// 场景位于背景之上，内容之下
	SU_CONTENT			,		// 场景用于替换内容层，此为默认值
	SU_POSTCONTENT		,		// 场景位于内容之上，前景之下
	SU_FOREGROUND		,		// 场景用于替换前景层
	SU_ALL					// 场景替换所有内容（背景、内容、前景）
};

// 场景，作为2D和3D的纽带，也是一个独立的渲染目标，内部是一个纯粹的3D世界，在设备中必须绑定到一个 GdiLayer 表面
// 无论在DUI中作为一种样式还是作为一种独立控件，都以此类为入口
class Scene //: public GdiLayer
{
public:
	static Scene* New(Device* device);
	//void Dispose();

	HRESULT InitNamedItem(DISPPARAMS* params) { return S_OK; }

	// 解析XML子节点
	void ParseAttribute(HANDLE hNode);
	BOOL IsChildNode(HANDLE hChild);

	void SetMousePos(POINT* ppt); // ppt==NULL means that mouse leaves.

	static Scene* NewAutoInstance();
	static void DeleteAutoInstance(Scene* p);

	CComBSTR Name;
	bool LightEnabled;
	SceneUsage Usage;

	CComVariant RenderCallback; // callback. string or dispatch

	HRESULT GetScene(VARIANT* val);
	Camera& GetCameraRef();
	COLOR& GetMouseLightRef();
	AmbientLight& GetAmbientLightRef();
	//HRESULT GetAmbientLight(VARIANT* ret);
	HRESULT GetLights(VARIANT* ret); // 取光照集合，最多8个光源
	HRESULT GetEntities(VARIANT* ret); // 取实体对象集合
	HRESULT GetActiveEntity(VARIANT* ret); // 取光标下的实体对象
	void Update();
	
	HRESULT PutNewEntity(VARIANT* val); // 增加一个新实体，可以传递 Entity，或者 xxxModel，如果传递模型对象，则自动从模型中创建一个新实体

	Begin_Auto_Disp_Map(Scene)
		Disp_Property(1, Name, CComBSTR)
		Disp_PropertyGet(2, Camera, Camera)
		Disp_Property(3, LightEnabled, bool)
		Disp_PropertyGet(4, AmbientLight, AmbientLight)
		Disp_PropertyGet(5, Lights)
		Disp_PropertyGet(6, Entities)
		Disp_Property(7, Usage, long)		Disp_Alias(7, As)
		Disp_Property_Member(8, OnRender, RenderCallback, CComVariant)
		Disp_Property(9, MouseLight, COLOR)
		Disp_PropertyGet(10, Scene)
		Disp_PropertyPut(11, NewEntity)
		Disp_PropertyGet(12, ActiveEntity)

		Disp_Method(100, Update, void, 0)
	End_Disp_Map()

	disallow_implicit_constructors(Scene)
};
Implement_Creatable_NamedItem(Scene)

// 摄像机，用以决定视口矩阵和投影矩阵
class Camera
{
public:
	//static Camera* New(Scene* scene, float x, float y, float z);
	//void Dispose();

	D3DXVECTOR3 m_Position; // 摄像机位置

	float m_Pitch; // 抬头低头角度（绕X轴旋转），如15表示15度
	float m_Yaw; // 左转右转角度（绕Y轴旋转）
	float m_Roll; // 自身旋转角度（绕Z轴旋转）

	float m_Zoom; // 缩放比例，默认1
	float m_Fov; // 视野角度，默认45度
	//float Aspect; // 长宽比例，由渲染目标确定
	float m_NearPlane; // 视口可视近面，默认0
	float m_FarPlane; // 视口可视远面，默认1000
	bool m_Ortho; // 是否正交投影，默认false
	// 

	HRESULT SetProperties(VARIANT* val);

	void ParseAttribute(HANDLE hNode);

	Begin_Disp_Map(Camera)
		Disp_Property_Member(101, Zoom, m_Zoom, float)
		Disp_Property_Member(102, Fov, m_Fov, float)
		Disp_Property_Member(103, Pitch, m_Pitch, float)
		Disp_Property_Member(104, Yaw, m_Yaw, float)
		Disp_Property_Member(105, Roll, m_Roll, float)
		Disp_Property_Member(106, NearPlane, m_NearPlane, float)
		Disp_Property_Member(107, FarPlane, m_FarPlane, float)
		Disp_Property_Member(108, Ortho, m_Ortho, bool)
		Disp_Property_Member(109, Position, m_Position, D3DXVECTOR3)
		Disp_PropertyPut(200, Properties)
	End_Disp_Map()

	disallow_implicit_constructors(Camera)
};
Implement_Ref_Creator(Camera)

//////////////////////////////////////////////////////////////////////////
// 环境灯光
class AmbientLight
{
	disallow_implicit_constructors(AmbientLight)
public:
	bool m_Enabled;
	COLOR m_Color;

	void ParseAttribute(HANDLE hNode);

	Begin_Disp_Map(AmbientLight)
		Disp_Property_Member(100, Enable, m_Enabled, bool)
		//Disp_Property_Member(101, r, m_Color.r, float)
		//Disp_Property_Member(102, g, m_Color.g, float)
		//Disp_Property_Member(103, b, m_Color.b, float)
		//Disp_Property_Member(104, a, m_Color.a, float)
		//Disp_PropertyPut(200, Properties)
		Disp_Property_Member(110, Color, m_Color, COLOR)
	End_Disp_Map()
};
Implement_Ref_Creator(AmbientLight)

// 光源
struct Light
{
	bool m_Enabled;
	CComBSTR m_Name;
	D3DLIGHT9 m_Light;

	void ParseAttribute(HANDLE hNode);
	//HRESULT InitNamedItem(DISPPARAMS* params) { return S_OK; }

	Begin_Disp_Map(Light)
		Disp_Property_Member(1, Enable, m_Enabled, bool)
		Disp_Property_Member(2, Name, m_Name, CComBSTR)
	End_Disp_Map()
};
//Implement_DispCollection(LightCollection, Light);

// 点光源
class PointLight : public Light
{
	disallow_implicit_constructors(PointLight)
public:
	HRESULT InitNamedItem(DISPPARAMS* params)
	{
		DispParams dp(params);
		// 当前只支持第一个参数设置ENABLE
		if (dp.Count()>0) dp.GetBool(0,m_Enabled);

		return S_OK;
	}


	// 这两个方法被自动化的CreateInstance/DeleteInstance调用，实现这两个方法可以不用 NEW/delete 自己，而是实际创建派生类
	static PointLight* NewAutoInstance();
	static void DeleteAutoInstance(PointLight* p);

	Begin_Auto_Disp_Map(PointLight, Light)
		//Disp_Property_Member(100, Enable, m_Enabled, bool)
		Disp_Property_Member(101, Diffuse, m_Light.Diffuse, D3DCOLORVALUE) // color
		Disp_Property_Member(102, Specular, m_Light.Specular, D3DCOLORVALUE) // color
		Disp_Property_Member(103, Ambient, m_Light.Ambient, D3DCOLORVALUE) // color
		Disp_Property_Member(104, Position, m_Light.Position, D3DVECTOR) // vector3
		Disp_Property_Member(105, Range, m_Light.Range, float) // 光照距离
		// 光线随距离的衰减算法：Atten = 1/( att0 + att1 * d + att2 * d^2)
		Disp_Property_Member(107, Attenuation0, m_Light.Attenuation0, float) // 常量衰减因子，默认0
		Disp_Property_Member(108, Attenuation1, m_Light.Attenuation1, float) // 线性衰减因子，默认1
		Disp_Property_Member(109, Attenuation2, m_Light.Attenuation2, float) // 平方衰减因子，默认0
		//Disp_PropertyPut(200, Properties)
	End_Disp_Map()
};
Implement_Creatable_NamedItem(PointLight)
Implement_Ref_Creator(PointLight)

// 聚光灯
class SpotLight : public Light
{
	disallow_implicit_constructors(SpotLight)
public:
	HRESULT InitNamedItem(DISPPARAMS* params)
	{
		DispParams dp(params);
		// 当前只支持第一个参数设置ENABLE
		if (dp.Count()>0) dp.GetBool(0,m_Enabled);

		return S_OK;
	}

	// 这两个方法被自动化的CreateInstance/DeleteInstance调用，实现这两个方法可以不用 NEW/delete 自己，而是实际创建派生类
	static SpotLight* NewAutoInstance();
	static void DeleteAutoInstance(SpotLight* p);

	Begin_Auto_Disp_Map(SpotLight, Light)
		//Disp_Property_Member(100, Enable, m_Enabled, bool)
		Disp_Property_Member(101, Diffuse, m_Light.Diffuse, D3DCOLORVALUE) // color
		Disp_Property_Member(102, Specular, m_Light.Specular, D3DCOLORVALUE) // color
		Disp_Property_Member(103, Ambient, m_Light.Ambient, D3DCOLORVALUE) // color
		Disp_Property_Member(104, Position, m_Light.Position, D3DVECTOR) // vector3
		Disp_Property_Member(105, Direction, m_Light.Direction, D3DVECTOR) // vector3
		Disp_Property_Member(106, Range, m_Light.Range, float)
		Disp_Property_Member(107, Attenuation0, m_Light.Attenuation0, float)
		Disp_Property_Member(108, Attenuation1, m_Light.Attenuation1, float)
		Disp_Property_Member(109, Attenuation2, m_Light.Attenuation2, float)
		Disp_Property_Member(110, Falloff, m_Light.Falloff, float) // default 1.0
		Disp_Property_Member(111, Theta, m_Light.Theta, float) // angel,in andians. range(0 - Phi)
		Disp_Property_Member(112, Phi, m_Light.Phi, float) // angel, in andias. range(0 - PI)
		//Disp_PropertyPut(200, Properties)
	End_Disp_Map()
};
Implement_Creatable_NamedItem(SpotLight)
Implement_Ref_Creator(SpotLight)

// 平行光源
class DirectionLight : public Light
{
	disallow_implicit_constructors(DirectionLight)
public:
	HRESULT InitNamedItem(DISPPARAMS* params)
	{
		DispParams dp(params);
		// 当前只支持第一个参数设置ENABLE
		if (dp.Count()>0) dp.GetBool(0,m_Enabled);

		return S_OK;
	}

	// 这两个方法被自动化的CreateInstance/DeleteInstance调用，实现这两个方法可以不用 NEW/delete 自己，而是实际创建派生类
	static DirectionLight* NewAutoInstance();
	static void DeleteAutoInstance(DirectionLight* p);

	Begin_Auto_Disp_Map(DirectionLight, Light)
		//Disp_Property_Member(100, Enable, m_Enabled, bool)
		Disp_Property_Member(101, Diffuse, m_Light.Diffuse, D3DCOLORVALUE) // color
		Disp_Property_Member(102, Specular, m_Light.Specular, D3DCOLORVALUE) // color
		Disp_Property_Member(103, Ambient, m_Light.Ambient, D3DCOLORVALUE) // color
		Disp_Property_Member(104, Direction, m_Light.Direction, D3DVECTOR) // vector3
		//Disp_PropertyPut(200, Properties)
	End_Disp_Map()
};
Implement_Creatable_NamedItem(DirectionLight)
Implement_Ref_Creator(DirectionLight)


//////////////////////////////////////////////////////////////////////////
class Material
{
	disallow_implicit_constructors(Material)
public:
	bool m_Enabled; // default:true，当用于实体时，default:false
	D3DMATERIAL9 material;

	CComVariant onTransform; // 设置变换之前的回调
	CComVariant onRender;
	CComVariant onPrepareEffect;

	void ParseAttribute(HANDLE hNode);

#ifdef _DEBUG
	void __DebugOnAddRef();
	void __DebugOnRelease();
#endif // _DEBUG

	CComBSTR& GetTextureNameRef();
	HRESULT PutTextureName(VARIANT* val);
	CComBSTR& GetTechniqueRef();

	void OnPropertyChanged(DISPID dispid, LPCOLESTR name);

	Begin_Disp_Map(Material) // 注意：材质不能创建，只能从其它对象中通过属性读取
		Disp_Property_Member(1, Enabled, m_Enabled, bool)
		Disp_Property(2, TextureName, CComBSTR)
		Disp_Property(3, Technique, CComBSTR)
		Disp_Property(4, onRender, CComVariant)
		Disp_Property(5, onPrepareEffect, CComVariant)
		Disp_Property_Member(101, Diffuse, material.Diffuse, D3DCOLORVALUE) // color
		Disp_Property_Member(102, Specular, material.Specular, D3DCOLORVALUE) // color
		Disp_Property_Member(103, Ambient, material.Ambient, D3DCOLORVALUE) // color
		Disp_Property_Member(104, Emissive, material.Emissive, D3DCOLORVALUE) // color
		Disp_Property_Member(105, Power, material.Power, float)
	End_Disp_Map()
};

//////////////////////////////////////////////////////////////////////////
// 3D 模型
// 在XML定义中，使用相同的model标签，通过type指定模型类型，例如type="box"表示长方体模型，如果不指定则默认为普通模型
// 外部X模型也不需指定type属性，但须指定src属性指示外部文件路径

#define public_model_methods(model) \
public: \
	static model* New(Device* device); \
	static model* NewAutoInstance(); \
	static void DeleteAutoInstance(model* p); \
	HRESULT InitNamedItem(DISPPARAMS* params) { return S_OK; } \
	CComBSTR& GetNameRef(); \
	Transform3D& GetTransformRef(); \
	CComVariant& GetonTransformRef(); \
	CComVariant& GetonRenderRef(); \
	CComVariant& GetonPrepareEffectRef(); \
	CComVariant& GetonMouseEnterRef(); \
	CComVariant& GetonMouseLeaveRef(); \
	CComVariant& GetonClickRef(); \
	CComVariant& GetonDblClickRef(); \
	HRESULT GetMaterials(VARIANT* val); \
	HRESULT GetParent(VARIANT* val); \
	HRESULT GetChildren(VARIANT* val); \
	HRESULT GetEntities(VARIANT* val); \
	IDispatch* CreateEntity(); \
	IDispatch* SubmitChange(); \
	void Remove();

#define public_model_disp() \
	Disp_Property(1001, Name, CComBSTR) \
	Disp_PropertyGet(1002, Transform, Transform3D) \
	Disp_PropertyGet(1003, Materials) \
	Disp_PropertyGet(1004, Parent) \
	Disp_PropertyGet(1005, Children) \
	Disp_PropertyGet(1006, Entities) \
	Disp_Property(1050, onTransform, CComVariant) \
	Disp_Property(1051, onRender, CComVariant) \
	Disp_Property(1052, onPrepareEffect, CComVariant) \
	Disp_Property(1053, onMouseEnter, CComVariant) \
	Disp_Property(1054, onMouseLeave, CComVariant) \
	Disp_Property(1055, onClick, CComVariant) \
	Disp_Property(1056, onDblClick, CComVariant) \
	Disp_Method(2000, Remove, void, 0) \
	Disp_Method(2001, CreateEntity, IDispatch*, 0) \
	Disp_Method(2002, SubmitChange, IDispatch*, 0) /* 如果影响模型创建的属性被改变，必须调用此方法通知模型更新 */

// 普通模型，顶点等所有数据自定义
class Model
{
	disallow_implicit_constructors(Model)
	public_model_methods(Model)
public:
	/*
	*	如果未指定法线，则它们的生成将取决于是否为网格指定了三角形索引。 
	*	如果指定了三角形索引，则将考虑相邻面来生成法向量。 
	*	如果未指定三角形索引，则法线值只对对应的三角形起作用。 这可能导致网格外形呈小平面。
	 */
	Vector3DCollection m_Normals;		// 法向量集合，可选
	Vector3DCollection m_Points;		// 顶点集合，必选
	Point2DCollection m_TextureCoordinates;	// 纹理坐标集合，可选。通常取值范围 [0,1]
	IntCollection m_TriangleIndices;		// 顶点索引集合，非负整形数组，必须是3的倍数，必选。索引顺序决定某个三角形是正面还是背面

	void ParseAttribute(HANDLE hNode);

	// 以下几个方法当前只支持字符串类型赋值，例如 "1,0,1 2,1,0 3,4,6"，用逗号或空格分隔
	HRESULT PutNormals(VARIANT* val);
	HRESULT PutPoints(VARIANT* val);
	HRESULT PutTextureCoordinates(VARIANT* val);
	HRESULT PutTriangleIndices(VARIANT* val);

	Begin_Auto_Disp_Map(Model)
		public_model_disp()
		Disp_Property_Member(1, Normals, m_Normals, Vector3DCollection)
		Disp_Property_Member(2, Points, m_Points, Vector3DCollection)
		Disp_Property_Member(3, TextureCoordinates, m_TextureCoordinates, Point2DCollection)
		Disp_Property_Member(4, TriangleIndices, m_TriangleIndices, IntCollection)
	End_Disp_Map()
};
Implement_Creatable_NamedItem(Model)

// 空模型是一个有效的模型，但它不执行任何渲染
class NullModel
{
	disallow_implicit_constructors(NullModel)
	public_model_methods(NullModel)
public:

	Begin_Auto_Disp_Map(NullModel)
		public_model_disp()
	End_Disp_Map()
};
Implement_Creatable_NamedItem(NullModel)

// X模型，模型数据来自于外部X文件
class XModel
{
	disallow_implicit_constructors(XModel)
	public_model_methods(XModel)
public:

	void ParseAttribute(HANDLE hNode);

	Begin_Auto_Disp_Map(XModel)
		public_model_disp()
	End_Disp_Map()
};
Implement_Creatable_NamedItem(XModel)

// 矩形模型
class QuadModel
{
	disallow_implicit_constructors(QuadModel)
	public_model_methods(QuadModel)
public:

	float Width;	// 宽度，默认 2.0
	float Height;	// 高度，默认 2.0

	void ParseAttribute(HANDLE hNode);

	Begin_Auto_Disp_Map(QuadModel)
		Disp_Property(1, Width, float)
		Disp_Property(2, Height, float)
		public_model_disp()
	End_Disp_Map()
};
Implement_Creatable_NamedItem(QuadModel)

// 长方体模型
class BoxModel
{
	disallow_implicit_constructors(BoxModel)
	public_model_methods(BoxModel)
public:

	float Width;	// 宽度，默认 100.0
	float Height;	// 高度，默认 100.0
	float Depth;	// 深度，默认 100.0

	void ParseAttribute(HANDLE hNode);

	Begin_Auto_Disp_Map(BoxModel)
		Disp_Property(1, Width, float)
		Disp_Property(2, Height, float)
		Disp_Property(3, Depth, float)
		public_model_disp()
	End_Disp_Map()
};
Implement_Creatable_NamedItem(BoxModel)

// 圆柱体模型
class CylinderModel
{
	disallow_implicit_constructors(CylinderModel)
	public_model_methods(CylinderModel)
public:

	float Radius1;
	float Radius2;
	float Length;
	ULONG Slices;	// 主轴切片数量，默认 5
	ULONG Stacks;	// 主轴栈数量，默认 5

	void ParseAttribute(HANDLE hNode);

	Begin_Auto_Disp_Map(CylinderModel)
		Disp_Property(1, Radius1, float)
		Disp_Property(2, Radius2, float)
		Disp_Property(3, Length, float)
		Disp_Property(4, Slices, ULONG)
		Disp_Property(5, Stacks, ULONG)
		public_model_disp()
	End_Disp_Map()
};
Implement_Creatable_NamedItem(CylinderModel)

// 多边形模型
class PolygonModel
{
	disallow_implicit_constructors(PolygonModel)
	public_model_methods(PolygonModel)
public:

	float Length;	// 每条边的长度，默认 50
	ULONG Sides;	// 边数量，必须至少是3。默认是 5

	void ParseAttribute(HANDLE hNode);

	Begin_Auto_Disp_Map(PolygonModel)
		public_model_disp()
		Disp_Property(1, Length, float)
		Disp_Property(2, Sides, ULONG)
	End_Disp_Map()
};
Implement_Creatable_NamedItem(PolygonModel)

// 球体模型
class SphereModel
{
	disallow_implicit_constructors(SphereModel)
	public_model_methods(SphereModel)
public:

	float Radius;	// 半径，必须>=0，默认 50
	ULONG Slices;	// 主轴切片数量，默认 5
	ULONG Stacks;	// 主轴栈数量，默认 5

	void ParseAttribute(HANDLE hNode);

	Begin_Auto_Disp_Map(SphereModel)
		public_model_disp()
		Disp_Property(1, Radius, float)
		Disp_Property(4, Slices, ULONG)
		Disp_Property(5, Stacks, ULONG)
	End_Disp_Map()
};
Implement_Creatable_NamedItem(SphereModel)

// 圆环体（轮胎）模型
class TorusModel
{
	disallow_implicit_constructors(TorusModel)
	public_model_methods(TorusModel)
public:

    float InnerRadius;	// 内圈半径，默认 30
    float OuterRadius;	// 外圈半径，默认 50
    ULONG Sides;		// 交叉段边数，最小是3。默认 5
    ULONG Rings;		// 圆环数，最小是3。默认 5

	void ParseAttribute(HANDLE hNode);

	Begin_Auto_Disp_Map(TorusModel)
		public_model_disp()
		Disp_Property(1, InnerRadius, float)
		Disp_Property(2, OuterRadius, float)
		Disp_Property(3, Sides, ULONG)
		Disp_Property(4, Rings, ULONG)
	End_Disp_Map()
};
Implement_Creatable_NamedItem(TorusModel)

// 文字模型
class TextModel
{
	disallow_implicit_constructors(TextModel)
	public_model_methods(TextModel)
public:

	// 字体描述，必须是 TRUETYPE 字体
	FONT Font;

	CComBSTR Text;		// 要渲染的文本
    float Deviation;	// 字体轮廓的最大弦偏差，TrueType字体的一个属性，必须>=0，如果为0，则使用字体默认值。默认是 0
    float Extrusion;	// 文本字体在负 Z 轴方向的深度。默认 0.1
    bool Center;		// 是否让模型居中，默认 false，即原点位于文本左下角

	void ParseAttribute(HANDLE hNode);

	//D3DXVECTOR3& GetCenterRef();

	Begin_Auto_Disp_Map(TextModel)
		public_model_disp()
		Disp_Property(1, Text, CComBSTR)
		Disp_Property(2, Deviation, float)
		Disp_Property(3, Extrusion, float)
		Disp_Property(4, Font, FONT)
		Disp_Property(6, Center, bool)
		//Disp_Property(5, Center, D3DXVECTOR3)
	End_Disp_Map()
};
Implement_Creatable_NamedItem(TextModel)

// 茶壶模型
class TeapotModel
{
	disallow_implicit_constructors(TeapotModel)
	public_model_methods(TeapotModel)
public:

	void ParseAttribute(HANDLE hNode);

	Begin_Auto_Disp_Map(TeapotModel)
		public_model_disp()
	End_Disp_Map()
};
Implement_Creatable_NamedItem(TeapotModel)

// 动画集，只有 XModel 才可能有动画集
class AnimationSet
{
	disallow_implicit_constructors(AnimationSet)
public:
	static AnimationSet* NewAutoInstance();
	static void DeleteAutoInstance(AnimationSet* p);

	Begin_Auto_Disp_Map(AnimationSet)
	End_Disp_Map()
};

//////////////////////////////////////////////////////////////////////////
// 3D 对象实体，每个实体应该关联一个模型，实体不能直接创建，必须从模型中创建
class Entity
{
	disallow_implicit_constructors(Entity)
public:
	bool m_Enabled; // 默认 true
	CComBSTR m_Name;
	Transform3D m_WorldTransform; // 提供世界变换
	
	// 事件回调
	CComVariant onTransform; // 设置变换之前的回调
	CComVariant onRender;
	CComVariant onPrepareEffect;
	CComVariant onMouseEnter;
	CComVariant onMouseLeave;
	CComVariant onClick;
	CComVariant onDblClick;

	void SetEffectParameters(LPCOLESTR params);

	void Remove();		// 删除自己

	static void DeleteAutoInstance(Entity* p);

	Material& GetMaterialRef();
	CComBSTR& GetParametersRef();
	HRESULT GetScene(VARIANT* val);
	HRESULT GetModel(VARIANT* val);
	HRESULT GetParent(VARIANT* val);
	HRESULT GetParentIndex(VARIANT* val); // 属于父实体或场景的子实体集合的索引号
	HRESULT GetChildren(VARIANT* val);
	HRESULT PutParameters(VARIANT* val);

	// 动画集相关方法
	HRESULT GetAnimationSets(VARIANT* val); // 读取动画集集合
	void ResetTime(); // 复位动画时钟
	void SetAnimationSet(VARIANT as); // 设置当前动画集，参数可以是索引、名称或者 AnimationSet 对象

	Begin_Auto_Disp_Map(Entity)
		Disp_Property_Member(1, Enable, m_Enabled, bool)
		Disp_Property_Member(2, Name, m_Name, CComBSTR)
		Disp_PropertyGet_Member(3, Transform, m_WorldTransform, Transform3D)
		Disp_PropertyGet(4, Material, Material)
		Disp_PropertyGet(5, AnimationSets)
		Disp_Property(6, Parameters, CComBSTR)
		Disp_PropertyGet(19, Scene)
		Disp_PropertyGet(20, Model)
		Disp_PropertyGet(21, Parent)
		Disp_PropertyGet(22, ParentIndex)
		Disp_PropertyGet(23, Children)
		Disp_Property(50, onTransform, CComVariant)
		Disp_Property(51, onRender, CComVariant)
		Disp_Property(52, onPrepareEffect, CComVariant)
		Disp_Property(53, onMouseEnter, CComVariant)
		Disp_Property(54, onMouseLeave, CComVariant)
		Disp_Property(55, onClick, CComVariant)
		Disp_Property(56, onDblClick, CComVariant)
		Disp_Method(100, ResetTime, void, 0)
		Disp_Method(101, SetAnimationSet, void, 1, VARIANT)
		Disp_Method(201, Remove, void, 0)
	End_Disp_Map()
};

//////////////////////////////////////////////////////////////////////////

// 关键帧
class KeyFrame
{
	disallow_implicit_constructors(KeyFrame)
public:
	// 普通创建实例，必须提供有效的keyframeset参数
	static KeyFrame* New(KeyFrameSet* keyframeset);

	// 用于自动化创建实例，比如脚本环境，keyframeset参数可以后来调用 AddTo 增加
	static KeyFrame* NewAutoInstance();
	static void DeleteAutoInstance(KeyFrame* p);

	HRESULT InitNamedItem(DISPPARAMS* params)
	{
		return S_OK;
	}

	void ParseAttribute(HANDLE hNode);

	CComBSTR Name; // 名称

	HRESULT GetKeyTime(VARIANT* val);	// 读取关键帧时间值(float)
	HRESULT PutKeyTime(VARIANT* val);	// 设置关键帧时间值(float/string['h:m:s'])
	HRESULT GetKeyValue(VARIANT* val);	// 读取关键帧属性值(float/long/ulong/double/COLOR/Vector3d...)
	HRESULT PutKeyValue(VARIANT* val);	// 设置关键帧属性值(float/..., 或者string)

	IDispatch* AddTo(IDispatch* keyframeset);	// 增加到关键帧集中
	void Remove();

	Begin_Auto_Disp_Map(KeyFrame)
		Disp_Property(1, Name, CComBSTR)

		Disp_Property(10, KeyTime)		Disp_Alias(10, Time)
		Disp_Property(11, KeyValue)		Disp_Alias(11, Value)

		Disp_Method(200, AddTo, IDispatch*, 1, IDispatch*)
		Disp_Method(201, Remove, void, 0)
	End_Disp_Map()
};
Implement_Creatable_NamedItem(KeyFrame)

// 关键帧集
class KeyFrameSet
{
	disallow_implicit_constructors(KeyFrameSet)
public:
	// 普通创建实例，必须提供有效的story参数
	static KeyFrameSet* New(StoryBoard* story);

	// 用于自动化创建实例，比如脚本环境，story参数可以后来调用 AddTo 增加
	static KeyFrameSet* NewAutoInstance();
	static void DeleteAutoInstance(KeyFrameSet* p);

	HRESULT InitNamedItem(DISPPARAMS* params)
	{
		return S_OK;
	}

	void ParseAttribute(HANDLE hNode);
	BOOL IsChildNode(HANDLE hChild);

	void OnPropertyChanged(DISPID dispid, LPCOLESTR name);

	CComBSTR Name; // 名称

	CComBSTR& GetTargetTypeRef();				// 字符串，比如 "float"，此字段必填
	HRESULT GetTarget(VARIANT* val);			// 读取目标对象，可能是名称，也可能是对象
	HRESULT PutTarget(VARIANT* val);			// 设置目标对象，可以是名称，也可以是对象
	CComBSTR& GetTargetAttributeRef();			// 目标对象属性名称，字符串
	bool& GetReverseRef();						// 是否执行动画翻转，默认 false
	HRESULT PutReverse(VARIANT* val);			// 设置是否执行动画翻转，可以设置的值是 true/false/"closure"(闭合翻转)
	ULONG& GetRepeatRef();						// 动画重复次数，正整数，0 表示无限重复
	HRESULT PutRepeat(VARIANT* val);			// 设置动画重复次数，正整数或者"forever"，默认为 1
	bool& GetSplineRef();						// 是否使用 B 样条，默认 false
	float& GetDelayRef();						// 动画延迟启动时间，单位秒，默认 0
	HRESULT PutDelay(VARIANT* val);
	float& GetIdleRef();						// 每次重复之前的空闲暂停时间，默认 0
	HRESULT PutIdle(VARIANT* val);
	bool& GetIsClosureRef();					// 只读，是否闭合翻转

	IDispatch* AddTo(IDispatch* story);			// 增加到演示板中
	void Remove();

	HRESULT GetKeyFrames(VARIANT* val);
	HRESULT PutNewKeyFrame(VARIANT* val);

	Begin_Auto_Disp_Map(KeyFrameSet)
		Disp_Property(1, Name, CComBSTR)
		Disp_PropertyGet(2, KeyFrames)
		Disp_PropertyPut(3, NewKeyFrame)

		Disp_PropertyPut(10, TargetType, CComBSTR)
		Disp_Property(11, Target)
		Disp_Property(12, TargetAttribute, CComBSTR)
		Disp_Property(13, Reverse, bool)
		Disp_Property(14, Repeat, ULONG)
		Disp_Property(15, Spline, bool)
		Disp_Property(16, Delay, float)
		Disp_Property(17, Idle, float)
		Disp_PropertyGet(18, IsClosure, bool)

		Disp_Method(200, AddTo, IDispatch*, 1, IDispatch*)
		Disp_Method(201, Remove, void, 0)
	End_Disp_Map()
};
Implement_Creatable_NamedItem(KeyFrameSet)

// From/To/By动画帧，它是一种特殊的 KeyFrameSet
class FrameSet
{
	disallow_implicit_constructors(FrameSet)
public:
	// 普通创建实例，必须提供有效的story参数
	static FrameSet* New(StoryBoard* story);

	// 用于自动化创建实例，比如脚本环境，story参数可以后来调用 AddTo 增加
	static FrameSet* NewAutoInstance();
	static void DeleteAutoInstance(FrameSet* p);

	HRESULT InitNamedItem(DISPPARAMS* params)
	{
		return S_OK;
	}

	void ParseAttribute(HANDLE hNode);

	CComBSTR Name; // 名称

	CComBSTR& GetTargetTypeRef();				// 字符串，比如 "float"，此字段必填
	CComBSTR& GetTargetAttributeRef();			// 目标对象属性名称，字符串
	bool& GetReverseRef();						// 是否执行动画翻转，默认 false
	ULONG& GetRepeatRef();						// 动画重复次数，正整数，0 表示无限重复
	float& GetDurationRef();					// 动画时间周期，浮点数，单位秒
	float& GetDelayRef();						// 动画延迟启动时间，单位秒，默认 0
	HRESULT PutDelay(VARIANT* val);
	float& GetIdleRef();						// 每次重复之前的空闲暂停时间，默认 0
	HRESULT PutIdle(VARIANT* val);
	VARIANT& GetFromRef();						// From 值，类型取决于目标类型
	VARIANT& GetToRef();						// To 值，类型取决于目标类型
	VARIANT& GetByRef();						// By 值，类型取决于目标类型

	HRESULT GetTarget(VARIANT* val);			// 读取目标对象，可能是名称，也可能是对象
	HRESULT PutTarget(VARIANT* val);			// 设置目标对象，可以是名称，也可以是对象
	HRESULT PutRepeat(VARIANT* val);			// 设置动画重复次数，正整数或者"forever"，默认为 1
	HRESULT PutDuration(VARIANT* val);			// 设置动画时间周期，浮点数，单位秒，默认 1 秒
	HRESULT PutFrom(VARIANT* val);				// 设置 From 值，类型必须匹配目标类型，或者字符串初始化。如果不设置，则表示从对象读取当前值
	HRESULT PutTo(VARIANT* val);				// 设置 To 值，类型必须匹配目标类型，或者字符串初始化。如果不设置，则表示 To = From + By，如果未设置By，则从对象读取当前值
	HRESULT PutBy(VARIANT* val);				// 设置 By 值，类型或者匹配目标类型，或者是一个整数或浮点数值，或者字符串。To 和 By 必须至少设置一个。

	IDispatch* AddTo(IDispatch* story);			// 增加到演示板中
	void Remove();								// 删除自己

	Begin_Auto_Disp_Map(FrameSet)
		Disp_Property(1, Name, CComBSTR)

		Disp_PropertyPut(10, TargetType, CComBSTR)
		Disp_Property(11, Target)
		Disp_Property(12, TargetAttribute, CComBSTR)
		Disp_Property(13, Reverse, bool)
		Disp_Property(14, Repeat, ULONG)
		Disp_Property(15, Delay, float)
		Disp_Property(16, Idle, float)
		
		Disp_Property(17, Duration, float)
		Disp_Property(18, From, VARIANT)
		Disp_Property(19, To, VARIANT)
		Disp_Property(20, By, VARIANT)

		Disp_Method(200, AddTo, IDispatch*, 1, IDispatch*)
		Disp_Method(201, Remove, void, 0)
	End_Disp_Map()
};
Implement_Creatable_NamedItem(FrameSet)

// 动画演示板，代表一个动画
class StoryBoard
{
	disallow_implicit_constructors(StoryBoard)
public:
	// 普通创建实例，必须提供有效的device参数
	static StoryBoard* New(Device* device);

	// 用于自动化创建实例，比如脚本环境，设备参数自动获得
	static StoryBoard* NewAutoInstance();
	static void DeleteAutoInstance(StoryBoard* p);

	HRESULT InitNamedItem(DISPPARAMS* params)
	{
		return S_OK;
	}

#ifdef _DEBUG
	void __DebugOnAddRef();
	void __DebugOnRelease();
#endif // _DEBUG

	void ParseAttribute(HANDLE hNode);
	BOOL IsChildNode(HANDLE hChild);

	CComBSTR Name; // 名称

	bool& GetStartedRef();
	bool& GetStoppedRef();
	bool& GetPausedRef();

	// 以下4个控制方法都返回对象自身
	/*
	 *	Stop 方法参数说明：
	 *		stopBehavior - 字符串类型，用来指定当停止动画时，演示板应该如何动作，可以是如下值之一：
	 *			* current  - 保持当前值。当 Stop 方法被执行时，动画控制的属性值保持当前运行时值，stopTime 参数忽略，onstop 回调立即执行。这是默认值。
	 *			* beginNow - 立即回到初始值。当 Stop 方法被执行时，动画控制的属性值立即设置到初始值，stopTime 参数忽略，onstop 回调立即执行。
	 *			* endNow   - 立即回到结束值。当 Stop 方法被执行时，动画控制的属性值立即设置到结束值，stopTime 参数忽略，onstop 回调立即执行。
	 *			* begin    - 以动画方式回到初始值。当 Stop 方法被执行时，动画控制的属性值在 stopTime 指定的时间内从当前值变化到初始值，onstop 回调也被延迟执行。
	 *			* end      - 以动画方式回到结束值。当 Stop 方法被执行时，动画控制的属性值在 stopTime 指定的时间内从当前值变化到结束值，onstop 回调也被延迟执行。
	 *			
	 *		stopTime - 浮点类型，表示以动画方式停止当前动画时，停止过程所需的周期，单位秒。默认值为 1 秒。
	 */
	IDispatch* Start();
	IDispatch* Stop(VARIANT stopBehavior=CComVariant(), float stopTime=1.0f);
	IDispatch* Pause();
	IDispatch* Resume();

	// 事件属性
	HRESULT GetOnStart(VARIANT* val);
	HRESULT PutOnStart(VARIANT* val);
	HRESULT GetOnStop(VARIANT* val);
	HRESULT PutOnStop(VARIANT* val);
	HRESULT GetOnPause(VARIANT* val);
	HRESULT PutOnPause(VARIANT* val);
	HRESULT GetOnResume(VARIANT* val);
	HRESULT PutOnResume(VARIANT* val);
	HRESULT GetOnStep(VARIANT* val);
	HRESULT PutOnStep(VARIANT* val);
	HRESULT PutEventHandler(VARIANT* val); // 所有事件可通过此方法一次性设置

	HRESULT GetFrameSets(VARIANT* val);
	HRESULT PutNewFrameSet(VARIANT* val);

	IDispatch* AddTo(IDispatch* window);				// 增加到窗口设备中，返回对象自身
	void Remove();			// 从设备中删除自己

	Begin_Auto_Disp_Map(StoryBoard)
		Disp_Property(1, Name, CComBSTR)
		Disp_PropertyGet(2, FrameSets)
		Disp_PropertyPut(3, NewFrameSet)

		Disp_PropertyGet(10, Started, bool)
		Disp_PropertyGet(11, Stopped, bool)
		Disp_PropertyGet(12, Paused, bool)
		Disp_PropertyPut(22, EventHandler)
		Disp_Property(23, OnStart)
		Disp_Property(24, OnStop)
		Disp_Property(25, OnPause)
		Disp_Property(26, OnResume)
		Disp_Property(27, OnStep)

		Disp_Method(100, Start, IDispatch*, 0)
		//Disp_Method(101, Stop, UseParamsMap)
		Disp_Method_Ext(101, Stop)
		Disp_Method(102, Pause, IDispatch*, 0)
		Disp_Method(103, Resume, IDispatch*, 0)

		Disp_Method(200, AddTo, IDispatch*, 1, IDispatch*)
		Disp_Method(201, Remove, void, 0)
	End_Disp_Map()

	Begin_Method_Params_Map(StoryBoard)
		Method_Params(Stop, 2, 2, IDispatch*, VARIANT, float)
	End_Method_Params_Map()
};
Implement_Creatable_NamedItem(StoryBoard)

#endif // __DUID3D_H__