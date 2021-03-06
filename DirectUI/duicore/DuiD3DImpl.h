#ifndef __DUID3DIMPL_H__
#define __DUID3DIMPL_H__

#include <ddraw.h>
#include "DuiD3D.h"
#include <d3dx9.h>
#include "../../common/mimefilter.h"
#include "DuiBase.h"
#include "DuiMarkup.h"
#include "DuiD3DHelper.h"
#include "Dui3DAuto.h"
#include <gdiplus.h>
//using namespace Gdiplus;

extern LPCWSTR prop_device_of_child_window;

//////////////////////////////////////////////////////////////////////////
class iGdiTexture;
class iObjectState;
  class iSurfaceBase;
	  class iImageLayer;
	  class iQuadObject;
	  class iRenderTarget;
  class iImageResource;
  class iImageAnimController;
  class iEffect;
  class iDevice;
  class iSubDevice;
class iCaret;
class iControl;
class iStoryBoard;
class iModelBase;
class iScene;
class iEntity;
class iModelBase;
class iMaterial;

//////////////////////////////////////////////////////////////////////////
#define UseSystemEffect

#define declare_allocator(x) \
	/*public: void DeleteThis() { delete this; }*/

#ifndef IsLayered
#define IsLayered(hwnd) ((::GetWindowLongW(hwnd, GWL_EXSTYLE) & WS_EX_LAYERED) == WS_EX_LAYERED)
#endif // IsLayered

#define __COLOR(type)			type(0xed, 0xee, 0xef)
#define __COLOR_AND_A(type, a)	type(a, 0xed, 0xee, 0xef)
#define KEYCOLOR_XRGB		__COLOR(D3DCOLOR_XRGB)
#define KEYCOLOR_ARGB(a)	__COLOR_AND_A(D3DCOLOR_ARGB, (a))
#define KEYCOLOR_RGB		__COLOR(RGB)


//#define USE_RHW

#ifdef USE_RHW
#define D3DFVF_VERTEX (D3DFVF_XYZRHW|D3DFVF_DIFFUSE|D3DFVF_TEX1)
#else
#define D3DFVF_VERTEX (D3DFVF_XYZ|D3DFVF_DIFFUSE|D3DFVF_TEX1)
#endif // USE_RHW

struct VERTEX
{
	float x, y, z; // position, 0 - width/height 当需要精确映射纹理到像素时，x y都应该减去0.5，即 x -= 0.5; y -= 0.5
#ifdef USE_RHW
	float rhw; // always 1.0，这个字段表明：系统认为顶点坐标已经是三大矩阵变换过的，因此设置的三大矩阵不再生效。如果需要变换，则设置顶点之前就需要自己先做好矩阵变换
#endif // USE_RHW
	D3DCOLOR color; // ARGB colour
	float u, v; // texture coordination, [0,1]，允许超过这个区间
};


//////////////////////////////////////////////////////////////////////////
#if 0
#define AdjustXY 0.5f
#else
#define AdjustXY 0.0f
#endif // USE_RHW

//#define DefaultZ 0.5f

#if 1
	#define ZNear 1.f
	#define ZFar 1000.f
	#define DefaultZ 1.f
#else
	// ZNEAR 千万别用 0，否则会使深度缓冲失效。此规则仅适用于投影矩阵，不能用于视口，视口的近面、远面必须设置为 0 和 1
	#define ZNear 0.f
	#define ZFar 1.f
	#define DefaultZ 0.5f
#endif // 1

//////////////////////////////////////////////////////////////////////////
#define BeginEffect(effect, technique) \
{ \
	iEffect* e = effect; \
	UINT numPass = e->begin(technique); \
	for (UINT __i=0; __i<numPass; __i++) \
	{ \
		e->beginPass(__i);

#define BeginSystemEffect(device, technique) \
{ \
	iSystemEffect* e = &device->m_system_effect; \
	UINT numPass = e->begin(technique); \
	for (UINT __i=0; __i<numPass; __i++) \
	{ \
		e->beginPass(__i);

#define EndEffect() \
		e->endPass(); \
	} \
	e->end(); \
}

#define __foreach_runtime(v) \
	v(target, iRenderTarget) \
	v(scene, iScene) \
	v(entity, iEntity) \
	v(model, iModelBase) \
	v(effect, iEffect) \
	v(material, iMaterial) \
	v(texture, IDirect3DBaseTexture9)

//////////////////////////////////////////////////////////////////////////
#define Thread __declspec(thread)

struct __gTlsData
{
	HRESULT hrInit;
	IDirect3D9* d3d9;
	iDevice* devices;
	HWND hwndGhost;
	HWND hwndActiveScript; // 当前正在执行脚本的窗口
};
extern Thread __gTlsData gt;

typedef IDirect3D9* ( WINAPI * pfnDirect3DCreate9) (UINT);
typedef D3DXMATRIX* (WINAPI * pfnD3DXMatrixOrthoLH) ( D3DXMATRIX *pOut, FLOAT w, FLOAT h, FLOAT zn, FLOAT zf );

class id3d
{
public:
	D3DDISPLAYMODE displayMode;
	D3DCAPS9 caps;
	D3DMULTISAMPLE_TYPE mstype, mstype_d16;
	DWORD msquality, msquality_d16;

	union
	{
		struct
		{
			DWORD isSSE2:1;
			DWORD isSSE:1;
			DWORD is3DNow:1;
			DWORD isMMX:1;
		};
		DWORD capsMMX;
	};

	id3d();
	~id3d();

	IDirect3D9* D3DCreate();

	// 这个函数应该在线程结束前调用
	void ThreadClear();

	bool isValid();
};

extern id3d g_d3d;

class PixEvent
{
public:
	PixEvent(LPCWSTR wszName, D3DCOLOR col=D3DCOLOR_XRGB(0,0,0))
	{
#ifdef _DEBUG
		__if_exists(D3DPERF_BeginEvent) {
			D3DPERF_BeginEvent(col, wszName);
		}
#endif // _DEBUG
	}
	~PixEvent()
	{
#ifdef _DEBUG
		__if_exists(D3DPERF_EndEvent) {
			D3DPERF_EndEvent();
		}
#endif // _DEBUG
	}
};

//////////////////////////////////////////////////////////////////////////
class iGdiTexture
{
public:
	iDevice* m_device;
	CComPtr<IDirect3DTexture9> m_texture;
	CComPtr<IDirectDrawSurface7> m_surface;
	LPDWORD m_buffer;
	LONG m_pitch;
	ULONG m_width, m_height;
	BOOL m_dirty;

	BOOL m_alpha;
	HWND m_wnd;	// 绑定的子窗口
	WNDPROC m_oldProc;
	CComBSTR m_changeWinClassName;
	IOleObject* m_oleObject;
	long m_renderType; // 0 - x, 表示渲染技术，默认是 0。如果默认不行，需尝试其它技术
	
	class Msgs : public CSimpleArray<ULONG>
	{
	public:
		BOOL HasMessage(ULONG msg);
		void ParseMessagesFromString(LPCOLESTR msgs);
	};
	Msgs m_msgHooks; // 所有需 HOOK 的消息列表，无需包含 WM_PAINT

	iGdiTexture(iDevice* device, ULONG width, ULONG height);

	~iGdiTexture();

	//bool isValid();
	bool resize(ULONG width, ULONG height);

	HDC getDC();
	void releaseDC(HDC hdc);

	HDC getTexDC();
	void releaseTexDC(HDC hdc);

	void bindWindow(HWND hwnd, BOOL transparent=FALSE, BOOL bUpdateOleObject=TRUE);

private:
	ULONG _lockCount;
	HDC _hdc;
	void _update_alpha(LPRECT pRect=NULL);

	BOOL __IsPaintMsg(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& lResult);
	static LRESULT CALLBACK __WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
};

//////////////////////////////////////////////////////////////////////////
class iClipTexture
{
public:
	iQuadObject* m_quad;
	CComPtr<IDirect3DTexture9> m_texture;
	LONG m_pitch;
	BOOL m_dirty;
	DWORD m_time;

	// 边缘裁剪路径
	GdiPath* m_path;

	iClipTexture(iQuadObject* quad);
	~iClipTexture();

	void resize();
	void update_path();
	IDirect3DTexture9* get_texture();
};

//////////////////////////////////////////////////////////////////////////
#define ostate(x) (1 << (x))
class iObjectState
{
public:
	DWORD isValid	:1;
	DWORD isTarget	:1;
	union
	{
		struct
		{
			DWORD isVisible		:1;	// 第一个成员在DWORD的最低位 (1<<0)
			DWORD isEnable		:1;	// (1<<1)
			DWORD isLoaded		:1; // (1<<2)
		};
		DWORD states;
	};

	iObjectState() : states(ostate(ost_visible) | ostate(ost_enable)), isValid(FALSE), isTarget(FALSE) {}

//protected:
//	void setValid(bool bValid=true) { isValid=bValid; }
};

//////////////////////////////////////////////////////////////////////////
class iCaret
{
public:
	LONG x,y,cx,cy; // position
	DWORD bValid:1;	// create/destroy
	DWORD bShow:1;	// show/hide
	DWORD bShowing:1;	// blinding state

	iQuadObject* owner;	// owned by
	iDevice* dev;	// device

	iCaret(iDevice* device);
	~iCaret();

	void render();
};

//////////////////////////////////////////////////////////////////////////
class iSurfaceBase : public iObjectState
{
public:
	iDevice* m_device;

	LONG m_left, m_top;			// 相对窗口客户区左上角
	LONG m_width, m_height;
	//D3DVIEWPORT9* m_viewport;
	VERTEX m_vertexs[4];

	iSurfaceBase(iDevice* device, LONG x=0, LONG y=0, LONG width=0, LONG height=0);

	void resize(LONG width, LONG height);
	void move(LONG x, LONG y, LONG width, LONG height);
	void setcolor(D3DCOLOR c0, D3DCOLOR c1=0, D3DCOLOR c2=0, D3DCOLOR c3=0);

private:
	void _updateVertexes();
};

#if 0
//////////////////////////////////////////////////////////////////////////
// iImageFrame
class iImageFrame : public TList<iImageFrame>
{
public:
	iImageResource* m_owner;
	CComPtr<IDirect3DTexture9> m_texture;
	LONG m_msec;

	iImageFrame(iImageResource* owner);
	~iImageFrame();
};
#endif // 0

//////////////////////////////////////////////////////////////////////////
// iImageResource
class iImageResource : public ImageResource
	//, public iObjectState
	, public UrlResources<iImageResource>
{
public:
	iDevice* m_device;
	CComBSTR m_name; // == id
	CComPtr<IDirect3DTexture9> m_texture;
	LONG m_width, m_height;
	// 图片切片，借用 RECT 结构，left 代表左边裁剪宽度，top 代表顶部裁剪高度
	// 在九宫格模式中，四个角不拉伸，四边一维拉伸，中间二维拉伸
	RECT _clip, *m_clip;
	// 如果图片有多帧，则 m_frames 有效
	//iImageFrame* m_frames;
	ULONG m_frameCount;
	//iImageFrame* m_currentFrame;

	LONG* m_msecs;
	ULONG m_currentFrame;
	Gdiplus::Image* m_img;
	GUID m_imgGuid;

	iImageAnimController* m_defAnim;
	iImageAnimController* getDefaultAnimation();

	iImageResource(iDevice* device, LPCOLESTR url, LPCOLESTR name=NULL);
	~iImageResource();
	void Dispose() { delete this; }

	long getClipWidth();
	long getClipHeight();
	float uLeft();
	float uRight();
	float vTop();
	float vBottom();
	long intLeft();
	long intRight();
	long intTop();
	long intBottom();

	void OnLoadStream(LPSTREAM pStream);
	static void CALLBACK onDownloadComplete(LPVOID pData, LPSTREAM pStream);

	IDirect3DTexture9* getTexture(ULONG index=0);

private:
	bool loadUrl(LPCOLESTR lpszUrl, HINSTANCE hMod=NULL);
};

class iImageAnimController : public CDuiConnectorCallback
{
public:
	iImageResource* m_image;
	DWORD m_timerid;
	ULONG m_currentFrame;

	iImageAnimController(iImageResource* img=NULL);
	~iImageAnimController();

	virtual BOOL OnHit( LPVOID pKey, LPVOID pVal, DWORD flag );
	void play();

	static void CALLBACK TimerProc(LPVOID pData, DWORD dwId);
};

//////////////////////////////////////////////////////////////////////////
// iImageLayer 用于静态纹理，通常用于加载图片，或者指定顶点颜色
class iImageLayer : public ImageLayer
	, public iImageAnimController
	, public TList<iImageLayer>
{
	declare_allocator(iImageLayer)
public:
	iQuadObject& m_owner;

	bool m_isValid;
	bool m_isLoaded;		// 仅针对图片纹理
	ImageLayoutType m_ilt; // 仅针对图片纹理
	VERTEX m_vertexs[16]; // 默认为NULL，表示使用 m_owner.m_vertexes，也能自行设定其它的顶点。仅针对颜色填充。九宫格模式需要16个顶点
	UINT m_number; // 图元数量，通常是 顶点数-2，扇形三角形列表，顺时针方向。仅针对颜色填充

	iImageLayer(iQuadObject& owner);
	~iImageLayer();
	
	void render();

	void _checkVertex();
	void _setPrimiteCount(bool bCopy=false, UINT num=2);

	void play();
};

//////////////////////////////////////////////////////////////////////////
// iQuadObject 用于每个块元素（不包括窗口跟元素）
class iQuadObject : public GdiLayer
	, public iSurfaceBase
	, public TOrderList<iQuadObject>
	, public TList<iQuadObject, 1>
{
	declare_allocator(iQuadObject)
public:
	iRenderTarget* m_rendertarget; // 如果表面是一个渲染目标，所有子元素都渲染到这个目标
	iScene* m_scene; // 表面应用的场景。如果存在场景，则表面是一个3D场景。场景内置渲染目标，无须单独设置

	LPCOLESTR m_name;

	iQuadObject* m_parent, *m_children, *m_ordered_children;
	iImageLayer* m_background;	// 背景图片纹理可有0个或1个，有多种贴图方式：标准、拉伸、平铺。
									// 如果存在背景纹理，则使用两个相同的四边形图元，一个使用背景纹理，另一个使用当前纹理，且当前纹理背景色为0
	iImageLayer* m_foregrounds;	// 如果存在前景，则使用这些前景，贴图方式跟背景纹理相同

	iEffect* m_effect;
	CStringA m_technique;

	iGdiTexture m_gdi;
	iClipTexture m_clip;
	RECT m_clipChildren;

	// callback data, DuiNode::Do3DPaint
	PaintCallback* _cbpaint;
	IExtensionContainer* m_exts;

	long m_zindex;

	iQuadObject(iDevice* device, LONG x, LONG y, LONG width, LONG height, iQuadObject* parent=NULL);
	virtual ~iQuadObject();

	bool isParent(iQuadObject* parent);

	void pre_render();
	void render(bool bNoNext=false);
	void ordered_render(bool positive, bool ispre=false);  // 执行排序渲染。positive: true:渲染大于等于0的; false:渲染小于0的
	void render_content();
	void render_children();

	void resize(LONG width, LONG height); // 参数允许小于0，这样会释放纹理资源，并设置成无效
	void move(LONG x, LONG y, LONG width, LONG height);
	virtual void on_reset();

	void set_as_target(BOOL bTarget);
	void set_scene(iScene* scene);

	long zindex() { return m_zindex; }

	virtual bool has_target();
};

//////////////////////////////////////////////////////////////////////////
// iRenderTarget 暂时不被使用，当需要执行变换时才需要使用
class iRenderTarget //: public RenderTarget
	//, public TList<iRenderTarget>
{
	declare_allocator(iRenderTarget)
public:
	iQuadObject* m_quad; // 所属表面
	CComPtr<IDirect3DTexture9> m_rendertarget; // 如果表面是一个渲染目标，所有子元素都渲染到这个目标
	CComPtr<IDirect3DSurface9> m_rendertarget_surface;
	CComPtr<IDirect3DSurface9> m_depth_surface;
	CComPtr<ID3DXRenderToSurface> m_render_to_surface;

	iRenderTarget();
	virtual ~iRenderTarget();

	IDirect3DTexture9* texture() const { return m_rendertarget.p; }

	void render();

	virtual void render_content();
	virtual void on_reset();
};

//////////////////////////////////////////////////////////////////////////
class iControl : public TList<iControl>
	, public iObjectState
{
	declare_allocator(iControl)
public:
	iDevice* m_device;
	CComPtr<IDirect3DTexture9> m_texture;
	CComPtr<IDirect3DSurface9> m_surface;
	HWND m_hwnd;
	SIZE m_sz;
	HDC m_hdcOrig, m_hdcSurf;
	ULONG m_lockCount;

	iControl(iDevice* device, HWND hwnd);
	~iControl();

	void render();
	void resize();
	void on_reset();
	void update(HDC hdc);
	void invalidate();

	HDC getDC();
	void releaseDC(HDC hdc);

	iControl* find(HWND hWnd);
};

//////////////////////////////////////////////////////////////////////////
struct EffectParamValue
{
	UINT Rows; // >1 - matrix(4x4)
	UINT Columns; // >1 - vector(4)
	UINT nElements; // 0 - not array, >0 - array
	UINT nBytes;
	union
	{
		//BOOL bVal;
		//INT iVal;
		//FLOAT fVal;

		LPBYTE pBuffer;
		BOOL* pbVal;
		INT* piVal;
		FLOAT* pfVal;
		D3DXMATRIX* pmVal;
		D3DXVECTOR4* pvVal;
	};

	EffectParamValue() : Rows(0), Columns(0), nElements(0), nBytes(0), pBuffer(NULL) {}
	~EffectParamValue() { clear(); }

	void clear()
	{
		if (pBuffer) delete[] pBuffer;
		//if (nElements>0 || nBytes>sizeof(float)) delete[] pBuffer;
		Rows = 0;
		Columns = 0;
		nElements = 0;
		nBytes = 0;
		pBuffer = NULL;
	}

	void set_size(D3DXPARAMETER_DESC* desc)
	{
		clear();
		Rows = desc->Rows;
		Columns = desc->Columns;
		nElements = desc->Elements;
		nBytes = desc->Bytes;
		if (Rows>1) pmVal = new D3DXMATRIX[max(1,nElements)];
		else if (Columns>1) pvVal = new D3DXVECTOR4[max(1, nElements)];
		else if (nBytes>0)
		//if (nElements>0 || nBytes>sizeof(float))
			pBuffer = new BYTE[nBytes];
	}
};

//////////////////////////////////////////////////////////////////////////
class iEffect : public Effect
			//, public iObjectState
			, public UrlResources<iEffect>
{
	declare_allocator(iEffect)
public:
	iDevice* m_device;
	CComPtr<ID3DXEffect> m_effect;
	//AutoStateDevice m_asd;
	BOOL isValid;
	D3DXHANDLE m_current_technique;

	enum
	{
		operator_none		=	0,
		operator_material	,
		operator_model		,
		operator_entity		,
	};
	int m_current_operator; // 当前操作者，用于运行时检测当前正在操作的对象类型，通常在 onPrepareEffect 事件中区分

	iEffect(iDevice* device, LPCOLESTR url=NULL, LPCOLESTR name=NULL, LPCOLESTR data=NULL, UINT length=0); // url 和 data 不能都为NULL
	~iEffect();

	void OnLoadData(LPCSTR pData, ULONG cbSize);
	static iEffect* findTechnique(iEffect* start, D3DXHANDLE technique);
	static iEffect* findTechnique(iEffect* start, LPCOLESTR technique);

	void clearParamBlock();
	void updateTechParams();

	bool setTechnique(LPCSTR technique);
	UINT begin(LPCSTR technique); // return count of passes
	UINT begin(LPCOLESTR technique);
	void end();
	bool beginPass(UINT pass);
	void endPass();

	bool on_reset(bool bNext=true);
	virtual void onloaded() {}

	HRESULT setParam(LPCOLESTR name, VARIANT* val);

	struct EffectParam
	{
		D3DXHANDLE handle;
		CComBSTR name;
		CComBSTR semantic;
		D3DXPARAMETER_DESC desc;
		/*
		 *	desc 信息备注：
		 *		Elements: 0 - 非数组类型；>0 - 数组类型，项个数
		 *		Rows/Columns: Object类型时为 0，普通类型时 > 0。例如 bool - [1,1], float3 - [1,3], float4 - [1,4], float4x4 - [4,4]
		 *		Bytes: 总字节数，无论是不是数组。纹理类型字节数为4，采样器类型字节数为0
		 */
		EffectParamValue saved_value; // 缓存旧值
		bool saved;

		EffectParam() : saved(false) { ZeroMemory(this, sizeof(EffectParam)); }
		//~EffectParam() { if (value) delete value; value=NULL; }

		HRESULT SetToEffect(ID3DXEffect* e, EffectParamValue* pValue);
		void Save(ID3DXEffect* e); // 缓存效果当前值
		void Restore(ID3DXEffect* e); // 用缓存的值恢复至效果中
	};
	CSimpleArray<EffectParam> m_params_cache;

	EffectParam* get_param_desc(D3DXHANDLE h, long* pIndex=NULL);
	D3DXHANDLE get_param_handle(LPCOLESTR name, long* pIndex=NULL);
	EffectParamValue* get_param_value(D3DXHANDLE h, bool bForceGet=false, bool bForPut=false);

protected:
	bool loadEffect(LPCSTR data, UINT length=0);
	bool _create();
	void _cacheParams(EffectParam* parent=NULL); // 缓存所有效果参数

	CDownload download;
	CStringA m_data;
	CStringA m_final_data;
	CComResult m_hr;
	iEffect* m_old_effect;
	D3DXHANDLE m_param_block;
};

//#ifdef UseSystemEffect
class iSystemEffect : public iEffect
{
public:
	iSystemEffect(iDevice* device);

	void init();
	virtual void onloaded();
	bool on_reset();

	void set_clip_texture(IDirect3DBaseTexture9* pTexture=NULL, IDirect3DBaseTexture9** ppOldTexture=NULL);

	void set_gray(BOOL bGray);
	//void set_transparent_color() {}

	void set_device_params();
	void set_scene_params();
	void set_scene_mirror_params();
	void set_model_params(CONST D3DXMATRIX* mtWorld);
	void set_texture_params();

	D3DXHANDLE gray_factor, hClip;
	CComPtr<IDirect3DTexture9> m_def_clip_texture; // 1 x 1

protected:
	bool _create();
};
//#endif // UseSystemEffect

//////////////////////////////////////////////////////////////////////////
// 命名对象表，用于根据名称检索IDispatch对象
//class iNamedObject : public TList<iNamedObject>
//{
//public:
//	CStringW name;
//	CComPtr<IDispatch> disp;
//
//	void Dispose() { delete this; }
//};

//////////////////////////////////////////////////////////////////////////
class iSubDevice : public Device
	, public TList<iSubDevice>
{
	declare_allocator(iSubDevice)
public:

	iDevice* m_device;
	HWND m_hwnd;
	iQuadObject* m_quad; // 绑定的QUAD对象

	iSubDevice(iDevice* device, HWND hwnd);
	~iSubDevice();
};

//////////////////////////////////////////////////////////////////////////
class iDevice : public Device
	, public iObjectState
	, public TList<iDevice>
{
	declare_allocator(iDevice)
public:
	//private:
	//bool isValid;
	CComPtr<IDirect3DDevice9> m_device;
	//CComPtr<IDirect3DSurface9> m_depth_surface;
	//CComPtr<IDirect3DVertexBuffer9> m_vb;
	CComPtr<IDirect3DSurface9> m_layeredSurf; // 仅在分层窗口中起作用
	HWND m_hwnd;	// 设备窗口
	D3DCOLOR m_colorFill;	// 背景填充色

	DWORD isLayered : 1;	// 设备窗口是否分层
	DWORD isRendering : 1;	// 是否正在渲染场景
	DWORD isReseting: 1;	// 设备正在复位
	DWORD isRelayout: 1;	// 设备正在重新载入

	// 如果窗口是分层的，创建一个DD表面缓存用于获得DC
	CComPtr<IDirectDraw7> m_dd;

	iRenderTarget* m_targets; // 渲染目标列表，不包含设备窗口主表面
	iQuadObject* m_textures; // 所有根级别的2D动态纹理集合，渲染时依次执行这些纹理
	iQuadObject* m_toptexture; // 最后显示的动态纹理，用于显示窗口或OCX控件，这些控件需要相对窗口客户区的位置信息
	iQuadObject* m_ordered_textures; // 排序的纹理，依据Z-ORDER，小于0的在m_textures之前渲染，大于等于0的在m_textures之后渲染

	// 这些静态资源在窗口导航至新布局时需删除
	iImageResource* m_images; // 静态图片纹理集合
	iModelBase* m_models; // 模型资源
	iStoryBoard* m_stories; // 动画资源集合
	DWORD m_story_timerid; // 动画轮询定时器ID
	DWORD m_fps; // 动画帧率，必须在30-100之间，默认30
	iScene* m_scenes; // 场景资源
	iEffect* m_effects; // 所有特效列表
	iSubDevice* m_subdevices; // 所有子设备列表

	//iNamedObject* m_namedObjects; // 命名对象表
	//iNamedObject* findNamedObject(LPCOLESTR name);
	CComDispatchDriver m_window_object;
	HANDLE m_hscript;

	CSimpleArray<D3DXMACRO> m_macros; // 用于效果的宏定义
	CComPtr<ID3DXEffectPool> m_effectPool;
//#ifdef UseSystemEffect
	iSystemEffect m_system_effect; // 内置系统特效
//#endif // UseSystemEffect

	//iQuadObject m_dcOrig; // 原始窗口DC内容，最先渲染
	iCaret m_caret;
	iControl* m_controls; // 所有子窗口（有窗口句柄）

	// 运行时信息，用来记录当前的状态和上下文
	struct RuntimeContext
	{
		RuntimeContext() { ZeroMemory(this, sizeof(RuntimeContext)); }

		template<typename T> T* set(T* newT) { return NULL; }

#define ImplementRuntime(name, type) \
	type* name; \
	type* set_##name(type* newval) \
	{ \
		type* pOld = name; \
		name = newval; \
		return pOld; \
	} \
	template<> type* set<type>(type* newval) { return set_##name(newval); } \
	type* get_##name() { return name; }

		// 渲染运行时设置，如果某个表面是一个渲染目标，当渲染它时必须先设置为当前目标，方便其子表面动态调整视口偏移值
		//iRenderTarget* target;
		//iScene* scene;
		//iMaterial* material;
		//IDirect3DBaseTexture9* texture;
		__foreach_runtime(ImplementRuntime)
#undef ImplementRuntime

	} m_runtime;

	float m_curTime;	// 单位秒
	float m_elapsedTime; // 单位秒

	static iDevice* get_active_device();

public:
	iDevice(HWND hwnd);
	~iDevice();

	//void clearLayeredState();
	//void setBufferSize(ULONG count);

	void reset();
	void render();
	void invalidate();
	void relayout(); // 布局即将切换，释放所有跟布局相关的资源

	LONG current_target_orig_x() const;
	LONG current_target_orig_y() const;
	VERTEX* update_target_vertex(VERTEX* vSrc, int num=4);

	ID3DXInclude* getInclude();
	CONST D3DXMACRO* getMacro();

	void fire_event(VARIANT* callback, IDispatch* dispThis=NULL, VARIANT* pvarResult=NULL);

	float current_time() { return m_curTime; }
	float elapsed_time() { return m_elapsedTime; }
	void on_story_started();
	void on_story_stopped();
	static void CALLBACK story_callback(LPVOID pData, DWORD dwId);

	IDirect3DTexture9* find_texture(LPCOLESTR name);
	iModelBase* find_model(LPCOLESTR name);

private:
	D3DPRESENT_PARAMETERS _pp;
	void _initPresentParams();
	void _initDeviceState();
	void on_reset();
	//void _removeImages(iImageResource* ir);
	//void _removeStories(iStoryBoard* story);
	void _updateTime();
};

#define RuntimeAutoContext(name, type) \

template<typename T>
class EnterRuntimeContext
{
	iDevice* _dev;
	T* _old;
public:
	EnterRuntimeContext(iDevice* dev, T* newT) : _dev(dev)
	{
		_old = _dev->m_runtime.set(newT);
	}
	~EnterRuntimeContext()
	{
		_dev->m_runtime.set(_old);
	}
};


#endif // __DUID3DIMPL_H__