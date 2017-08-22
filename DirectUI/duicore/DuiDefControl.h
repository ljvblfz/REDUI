#ifndef __DUIDEFCONTROL_H__
#define __DUIDEFCONTROL_H__

#pragma once

#include "DuiScrollBar.h"
#include "../DuiPlugin.h"
#include "DuiLayoutMode.h"
#include "DuiSurface.h"
#include "dxt.h"
#include "../../common/dispatchimpl.h"
#include "DuiImage.h"
#include "DuiPluginsManager.h"
#ifndef NO3D
#include "DuiD3D.h"
#endif // NO3D

#ifndef NO3D
#define IsGdi32(layout) ((layout)->Get3DDevice()==NULL && IsLayered((layout)->GetPaintWindow()))
#else
#define IsGdi32(layout) (IsLayered((layout)->GetPaintWindow()))
#endif // NO3D


#define LockReenter(sinkname, ...) \
	OpLock _lock(*this, OpCode_##sinkname); \
	if (!_lock.IsEntered()) { ::SetLastError(ERROR_ACCESS_DENIED); return __VA_ARGS__; }

class CDuiLayoutManager;
class CDuiStyleVariant;
class CDuiStyle;
class CDuiScrollbar;
class DuiNode;
class ControlExtension;

//////////////////////////////////////////////////////////////////////////
class CRenderClip
{
public:
	//CRenderClip() : hDC(NULL), hRgn(NULL), hOldRgn(NULL) {}
#ifndef NO3D
	CRenderClip(HDC hdc, LPCRECT rc, HRGN rgn=NULL, GdiPath* path=NULL);
#else
	CRenderClip(HDC hdc, LPCRECT rc, HRGN rgn=NULL);
#endif // NO3D
	~CRenderClip();
	RECT rcItem;
	HDC hDC;
	HRGN hRgn;
	HRGN hOldRgn;
};

//////////////////////////////////////////////////////////////////////////

// Structure for notifications to the outside world
typedef struct tagDuiNotify 
{
	CStdString sType;
	DuiNode* pSender;
	DWORD dwTimestamp;
	POINT ptMouse;
	WPARAM wParam;
	LPARAM lParam;
} DuiNotify;

//////////////////////////////////////////////////////////////////////////
// 
class CAttributeMap;

typedef void (CALLBACK* pfnAttributeChanged)(BOOL bDeleted, LPCOLESTR szAttr, LPVOID pData); // bDeleted: (TRUE - deleted; FALSE - changed)

struct AttrValue
{
	BOOL _deleted;
	CComVariant v;
	CAttributeMap* props; // 属性值里还可以继续嵌套属性包

	AttrValue(CComVariant _v, CAttributeMap* _props=NULL) : _deleted(FALSE), v(_v), props(_props) {}
	AttrValue(const AttrValue& other) : _deleted(FALSE) { v=other.v; props=other.props; ((AttrValue&)other).props=NULL;/*避免提早析构*/ }
	~AttrValue(); // { if (props) delete props; props = NULL; }
};

class CAttributeMap : public CSimpleMap<CStdString, AttrValue>,
	public CDispatchImpl<CAttributeMap>,
	public IPropertyBag
{
private:
	pfnAttributeChanged _fnNotify;
	LPVOID _data;

public:
	CAttributeMap() : _fnNotify(NULL), _data(NULL) {}

	void SetCallback(pfnAttributeChanged fn, LPVOID pdata)
	{
		_fnNotify = fn;
		_data = pdata;
	}

	BOOL SetAttribute(LPCOLESTR szAttr, /*LPCOLESTR*/CComVariant szVal, BOOL bHasSubProps=FALSE);
	void RemoveAttribute(LPCOLESTR szAttr);
	//BOOL SetAttributeByIndex(LONG idx, LPCOLESTR szVal)
	//{
	//	if (idx<0 || idx>=GetSize()) return FALSE;
	//	GetValueAt(idx) = szVal;
	//	return TRUE;
	//}
	CComVariant GetAttribute(LPCOLESTR szAttr) const
	{
		CStdString k(szAttr);
		k.MakeLower();
		int nIndex = FindKey(k);
		if (nIndex>=0 && !GetValueAt(nIndex)._deleted)
			return __super::GetValueAt(nIndex).v;
		return CComVariant(); // NULL;
	}
	CComVariant GetAttributeByIndex(LONG idx) const
	{
		if (idx>=0 && idx<__super::GetSize() && !GetValueAt(idx)._deleted)
			return __super::GetValueAt(idx).v;
		return CComVariant(); //NULL;
	}
	LPCOLESTR GetAttributeString(LPCOLESTR szAttr) const
	{
		CStdString k(szAttr);
		k.MakeLower();
		int nIndex = FindKey(k);
		if (nIndex>=0 && !GetValueAt(nIndex)._deleted)
		{
			CComVariant& v = __super::GetValueAt(nIndex).v;
			if (v.vt == VT_BSTR) return v.bstrVal;
		}
		return NULL;
	}
	LONG GetAttributeLong(LPCOLESTR szAttr, LONG lDef=0) const
	{
		CComVariant v = GetAttribute(szAttr);
		return (v.vt!=VT_EMPTY && SUCCEEDED(v.ChangeType(VT_I4))) ? V_I4(&v) : lDef;
	}
	BOOL GetAttributeBool(LPCOLESTR szAttr, BOOL bDef=FALSE) const;
	LONG FindAttribute(LPCOLESTR szAttr) const
	{
		CStdString k(szAttr);
		k.MakeLower();
		int nIndex = FindKey(k);
		if (nIndex>=0 && GetValueAt(nIndex)._deleted) nIndex = -1;
		return nIndex;
	}
	BOOL HasAttribute(LPCOLESTR szAttr) const
	{
		return (-1 != FindAttribute(szAttr));
	}
	CAttributeMap* GetSubAttributeMap(LPCOLESTR szAttr, BOOL bAutoCreate=FALSE)
	{
		int nIndex = FindAttribute(szAttr);
		if (nIndex == -1) return NULL;
		AttrValue& av = GetValueAt(nIndex);
		if (bAutoCreate && av.props==NULL)
			av.props = NEW CAttributeMap;
		return av.props;
	}

public:
	// IUnknown
	STDMETHOD_(ULONG, AddRef)() { return 2; }
	STDMETHOD_(ULONG, Release)() { return 1; }
	STDMETHOD(QueryInterface)(REFIID iid, LPVOID* ppvObj)
	{
		if (ppvObj == NULL) return E_POINTER;

		if (iid==IID_IPropertyBag)
			return (*ppvObj=(IPropertyBag*)this), S_OK;
		//else if (iid==IID_IUnknown || iid==IID_IDispatch)
		//	return (*ppvObj=(IDispatch*)this), S_OK;
		//else
		//	return (*ppvObj=NULL), E_NOINTERFACE;

		return CDispatchImpl<CAttributeMap>::QueryInterface(iid, ppvObj);
	}

	// IPropertyBag
	STDMETHOD (Read)(LPCOLESTR pszPropName, VARIANT *pVar, IErrorLog *pErrorLog);

	STDMETHOD (Write)(LPCOLESTR pszPropName, VARIANT *pVar)
	{
		return E_NOTIMPL; //E_FAIL; // 目前禁止写入属性值
		//if (pszPropName==NULL || pVar==NULL) return E_POINTER;
		//if (pVar->vt == VT_EMPTY) return E_FAIL;

		//return SetAttribute(pszPropName, *pVar) ? S_OK : E_FAIL;
	}

	// IDispatch
	STDMETHOD(GetIDsOfNames)(REFIID riid, OLECHAR ** rgszNames, UINT cNames, LCID lcid, DISPID * rgDispId)
	{
		CStdString k(*rgszNames);
		k.MakeLower();
		if (k.GetLength()>2 && k[0]==L'o' && k[1]==L'n') return *rgDispId=DISPID_UNKNOWN, DISP_E_UNKNOWNNAME;
		int nIndex = FindKey(k);
		if (nIndex == -1) return *rgDispId=DISPID_UNKNOWN, DISP_E_UNKNOWNNAME;
		return *rgDispId=(DISPID)nIndex, S_OK;
	}
	STDMETHOD(Invoke)(DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS * pdispparams, VARIANT *pVarResult, EXCEPINFO * pExcepInfo, UINT * puArgErr)
	{
		if (dispIdMember<0 || dispIdMember>=GetSize()) return DISP_E_MEMBERNOTFOUND;
		CStdString& k = GetKeyAt(dispIdMember);
		AttrValue& av = GetValueAt(dispIdMember);
		CComVariant vRet;

		switch (wFlags)
		{
		case DISPATCH_PROPERTYGET:
			if (!av._deleted)
				vRet = av.v;
			break;
		case DISPATCH_PROPERTYPUT:
		case DISPATCH_PROPERTYPUTREF:
			if (pdispparams->cArgs == 1)
			{
				//av.v = pdispparams->rgvarg[0];
				//av._deleted = FALSE;
				SetAttribute(k, pdispparams->rgvarg[0], k==L"filter");
			}
			break;
		}

		if (pVarResult) vRet.Detach(pVarResult);
		return S_OK;
	}
};

class CCustomMap : public CSimpleMap<LPVOID, LPVOID>
{
public:
	~CCustomMap()
	{
		for (int i=0; i<GetSize(); i++)
		{
			LPVOID v = GetValueAt(i);
			if (v) delete[] v;
		}
		RemoveAll();
	}

	LPVOID GetValue(LPVOID key, int nSize=0)
	{
		if (key==NULL)
			return NULL;

		int index = FindKey(key);
		if (index>=0)
			return GetValueAt(index);

		if (index==-1 && nSize<=0)
			return NULL;

		char* v = new char[nSize];
		if (v==NULL) return NULL;
		if (!Add(key, v))
			return (delete[] v), NULL;
		memset(v, 0, nSize);
		return v;
	}

	void DeleteKey(LPVOID key)
	{
		LPVOID v = Lookup(key);
		if (v) delete[] v;
		Remove(key);
	}
};

class CDefProc : public CSimpleMap<DWORD, fnDefPluginProc>
{
public:
	DuiNode* m_pOwner;
	CDefProc(DuiNode* owner) : m_pOwner(owner) {}

	template<typename tProc>
	void SetProc(DWORD opcode, tProc proc)
	{
		int index = FindKey(opcode);
		if (index < 0)
			Add(opcode, (const fnDefPluginProc&)proc);
		else
			SetAt(opcode, (const fnDefPluginProc&)proc);
	}

	template<typename T>
	T CallProcV(T* output, DWORD opcode, char* params)
	{
		int index = FindKey(opcode);
		if (index < 0) return output ? *output : 0;

		fnDefPluginProc proc = GetValueAt(index);
		Remove(opcode); // 调用后要从映射表中删除掉
		if (proc == NULL) return output ? *output : 0;

		switch (opcode)
		{
		case 0: return output ? *output : 0;
#define DoPreArg(...) __VA_ARGS__
#define PreArg(type,name) type* _##name = (type *)((params += X86SIZE(type)) - X86SIZE(type));
#define DoPreArg2(...) __VA_ARGS__
#define PreArg2(type,name) , type
#define Arg2(type,name) *_##name
#define __op_calldef(rettype, op, prearg, prearg2, ...) \
		case OpCode_##op: \
		{ \
			prearg; \
			rettype ret = ((rettype (/*CALLBACK*/ *)(CallContext* prearg2))proc)(NULL, __VA_ARGS__); \
			if (output) *((rettype*)output) = ret; \
			break; \
		}

		__foreach_op(__op_calldef, DoPreArg, PreArg, DoPreArg2, PreArg2, Arg2)

#undef __op_calldef
#undef DoPreArg
#undef PreArg
#undef DoPreArg2
#undef PreArg2
#undef Arg2
		}

		return output ? *output : 0;
	}

	template<typename T>
	T CallProc(T* output, DWORD opcode, ...)
	{

		va_list params;
		va_start(params, opcode);

		T t = CallProcV(output, opcode, params);

		va_end(params);

		return t;
	}
};

//////////////////////////////////////////////////////////////////////////
//

class DuiNode : 
#ifndef NO3D
						public PaintCallback, 
#endif // NO3D
						public TOrderList<DuiNode>, public IExtensionContainer
{
	friend CDuiLayoutManager;
	friend ControlExtension;
public:
	DWORD _sign;
	static const DWORD __sign = 0x20040620;

	LPCOLESTR m_tagName; // XML 标签名，仅使用指针
	CStdString m_id;

	HDE Handle();
	static DuiNode* FromHandle(HDE h);

private:

	DuiNode(LPCOLESTR tagname, DuiNode* parent, CDuiLayoutManager* lyt);
	~DuiNode();

	void DoNothing() {}

	CSimpleArray<DWORD> _invocations; // 正在调用的插件操作码集合，用于阻止重入
	BOOL _tryOpEnter(DWORD opcode);
	void _OpLeave(DWORD opcode);

	class OpLock
	{
		DuiNode& _owner;
		DWORD _opcode;
		BOOL _entered;
	public:
		OpLock(DuiNode& owner, DWORD opcode) : _owner(owner), _opcode(opcode), _entered(_owner._tryOpEnter(_opcode)) {}
		~OpLock() { if (_entered) _owner._OpLeave(_opcode); }

		BOOL IsEntered() const { return _entered; }
	};

public:
	CDuiLayoutManager* m_pLayoutMgr;
	CAttributeMap _attributes;
	CCustomMap m_customData;
	CDefProc m_defproc;
	RECT m_rcItem;	// 相对于 parent
	HRGN m_hRgn;	// 当被内联时此值有效，相对于 parent

#ifndef NO3D
	GdiPath m_pathClip;	// 仅当元素非内联时才有效

	GdiLayer* m_pSurface;
	Scene* m_pScene;
#endif // NO3D

	CDuiStyle* m_pStyle;
	DuiNode* m_pParent;
	//CDuiControllerBase* m_pController;
	//CDuiControllerBase** m_ppIOC; // 缓存的父控制器指针的指针
	CDuiScrollbar/*Wnd*/ m_scrollBar;
	CDuiScrollbar m_scrollBarH;
	CStdPtrArray m_aExtensions; // CDuiExtension

	//DWORD m_dwDisabled; // 被禁止的操作标志，例如阻止调用 UpdateLayout

	DXTransform* _dxt;

	//CAttributeMap m_attributes;
	CStdPtrArray m_children; // CDuiControlExtensionBase*
	DuiNode* m_offsetChildren; // 这是一个按照Z-ORDER排序的链表，在绘制时把它们放在其它子元素完成之后，通常具有定位样式或Z-ORDER的元素添加到此链表中
	OLECHAR m_chShortcut;
	//RECT m_rcItem;
	//HRGN m_hRgn;	// 当被内联时此值有效
	DWORD m_dwState;
	DWORD m_bAutoDestroy : 1;
	DWORD m_readyState : 3; // READYSTATE enum
	//DWORD m_ioc : 1; // 运行时值，指示当前是否正在被父控制器反转调用
	union
	{
		struct
		{
			DWORD m_dblclick_event : 1;
			DWORD m_dblclick_enable : 1;
		};
		DWORD m_dblclick : 2; // 控件是否需要双击响应，此值可通过设置双击事件或者标签属性指定
	};
	//BOOL m_bNeedUpdateRect;
	CStdPtrArray /*m_PreBackgroundImages,*/ m_PostBackgroundImages; // CDuiImageLayer，注意本类并不维护指针的释放，仅在绘制原始背景之前和之后顺序绘制这些层
	CStdPtrArray m_PreForegroundImages, m_PostForegroundImages; // CDuiImageLayer，注意本类并不维护指针的释放，仅在绘制原始前景之前和之后顺序绘制这些层

	// IDispatch
	CComPtr<IDispatch> m_disp;

	// external callback，是元素对象设置的回调，用于调用内置事件，优先级低于m_eventHandler中的同名事件回调
	EVENTCALLBACK m_fnEventCallback;

	// 脚本设置的事件处理器
	/*
	 *	用法：
			element.eventHandler({onload:function(){}, onunload:function(){}}); 一个参数：对象类型，同时设置多个事件回调。完全替换现有的m_eventHandler
			element.eventHandler("onload", function(){}); 两个参数：事件名称（串）+回调函数。仅设置/替换m_eventHandler中的一个事件回调
			element.eventHandler("onload"); 一个参数：事件名称（串）。返回事件对应的回调函数
	 */
	CComDispatchDriver m_eventHandler;

public:
	void DeleteThis() { delete this; }
	static DuiNode* New(LPCOLESTR tagname, DuiNode* parent, CDuiLayoutManager* lyt)
	{
		if (tagname==NULL || lyt==NULL) return NULL;
		return NEW DuiNode(tagname, parent, lyt);
	}

	void SetFilter(LPCOLESTR progid=NULL);
	void RemoveFilter();
	void SetFilterProps();

	LPVOID CastToClass( LPCOLESTR szName/*=NULL*/ );
	int GetClassCollection(LPVOID* buf, LPCOLESTR szName, int bufSize=0); // bufSize是buf个数
	
	void SetReadyState(READYSTATE rs);

public:
	// internal cache --- for inline
	struct RgnRect
	{
		RECT rc;
		LPCOLESTR str;
		LONG str_num;
		//UINT text_style;
	};
	class Rects : public CSimpleArray<RgnRect>
	{
	public:
		static LPCOLESTR specChars; // L"&_"
		UINT text_style;
	};
	struct RowItem
	{
		DuiNode* r;
		int index;
	};
	typedef CSimpleArray<RowItem> RowItems;

	struct StyleCacheItem
	{
		DWORD states;
		//DWORD states_mask;
		DuiStyleAttributeType sat;
		CDuiStyleVariant* val;
	};

	struct
	{
		union
		{
			struct
			{
				DWORD hasPos:1;			// pos
				DWORD hasClientPos:1;	// clientPos
				DWORD hasClipPos:1;		// clipPos
				DWORD hasBlock:1;		// displayBlock
				DWORD hasInline:1;		// displayInline
				DWORD displayBlock:1;
				DWORD displayInline:1;
				DWORD hasPosition:1;	// offset.position, offset.parent
				DWORD hasOffset:1;		// offset.rcOffset
				DWORD hasZIndex:1;		// offset.zindex
				DWORD updateScroll:1;	// must update scroll range
				DWORD hasVisual:1;		// visual is cached
				DWORD hasLayout:1;		// layout is cached
			};
			DWORD masks;
		};
		IVisual* visual;
		ILayout* layout;
		CDuiImage imgBkgnd;
		HFONT font;
		Rects rects;
		long tryContentWidth; // 如果这个值不是 0，那么表示估算大小时正在试算，试算中调用GetPos将使用此固定宽度
		DWORD disabled; // 被禁止的操作标志，例如阻止调用 UpdateLayout

		struct
		{
			RECT rcPart[DUIRT_TOTAL+1]; // 5 个
		} pos, clientPos, clipPos; // 缓存各个部分的矩形值，通过 hasPos/hasClientPos 指示有效性
		// 
		struct
		{
			DUI_POSITION position;
			DuiNode* parent; // 仅当位置样式不是默认样式时有效
			RECT rcOffset;
			LONG zindex;
		} offset;

		CSimpleArray<StyleCacheItem> styles;
		
	} __cache; // 所有缓存内容保存在这里

	//CSimpleArray<RgnRect> _pRects;
	void AddRect(LPCRECT pRect, LPCOLESTR psz, LONG num, InDrawData& idd);
	void AddSpecChar(LPCRECT pRect, OLECHAR c, InDrawData& idd); // c 必须存在于 specChars 中
	static void ReposInlineRow(InDrawData& idd);
	HRGN UpdateRgnFromRects();
	RECT UpdateRectFromRgn(HRGN hrgn=NULL);

	void UpdatePosition();
	long GetZIndex();

	void DrawChildren(HDC hdc, RECT rcPaint, BOOL bDraw, InDrawData& idd);
	HRGN CombineChildrenRegion();

	DuiNode* GetRoot();

	// IExtensionContainer methods
	virtual void AttachExtension(IExtension* pExt);
	virtual void DetachExtension(IExtension* pExt);

public:
	DuiNode* GetParent() const;

	LPCOLESTR GetID() const;
	void SetID(LPCTSTR pstrId);

	LPCOLESTR GetText();
	void SetText(LPCOLESTR pstrText);

	LPCOLESTR GetToolTip(BOOL bAllowDebug=TRUE);
	void SetToolTip(LPCOLESTR pstrText);

	OLECHAR GetShortcut();
	void SetShortcut(OLECHAR ch);

	void SetFocus();

	DWORD GetState();
	DWORD SetState(DWORD dwNewState);
	void ModifyState(DWORD dwAdded, DWORD dwRemoved);
	BOOL HasState(DWORD dwState);

	BOOL Activate();
	BOOL IsVisible(BOOL bRealResult=FALSE);
	BOOL IsEnabled(BOOL bRealResult=FALSE);
	BOOL IsFocused();
	void SetVisible(BOOL bVisible = TRUE);
	void SetEnabled(BOOL bEnable = TRUE);

	BOOL PtInPos(POINT pt, DUI_RECTTYPE rt);
	BOOL PtInControl(POINT pt);
	BOOL PtInScrollbar(POINT pt);
	LONG NcHittest(POINT pt);

	DuiNode* FindControl(FINDCONTROLPROC Proc, LPVOID pData, UINT uFlags);

	CDuiLayoutManager* GetLayoutManager() const;
	void SetManager(CDuiLayoutManager* pManager, DuiNode* pParent);

	RECT GetPos(DUI_RECTTYPE rt=DUIRT_CONTENT);
	void SetPos(RECT rc);
	RECT AdjustRect(const RECT rcContent, DUI_RECTTYPE rtDst=DUIRT_TOTAL);
	SIZE AdjustSize(const SIZE szContent, DUI_RECTTYPE rtDst=DUIRT_TOTAL);
	POINT AdjustPoint(POINT& ptContent, DUI_RECTTYPE rtDst=DUIRT_TOTAL);

	//BOOL IsRectDirty() const { return m_bNeedUpdateRect; }
	void SetRectDirty();

	void SetPrivateStyle(CDuiStyle* pStyle);
	CDuiStyle* GetPrivateStyle(BOOL bAutoCreate=FALSE) const;
	DuiStyleVariant* GetStyleAttribute(DuiStyleAttributeType sat, DuiStyleVariantType svt=SVT_EMPTY, DWORD dwMatchMask=0);

	void Invalidate();

	SIZE EstimateSize(SIZE szAvailable);
	void InitDrawData(HDC hDC, InDrawData& idd);
	void DoPaint(HDC hDC, const RECT& rcPaint, BOOL bDraw, InDrawData& idd);
	void OnPaintContent(HDC hdc, RECT rcPaint, BOOL bDraw, InDrawData& idd);

#ifndef NO3D
	// PaintCallback::Do3DPaint
	virtual void Do3DPaint();
	//static void Do3DPaint(LPVOID data) { ((DuiNode*)data)->Do3DPaint(); }
#endif // NO3D

	// for container
	int GetChildCount() const;
	DuiNode* GetChildControl(int index) const;
	BOOL AddChildControl(DuiNode* pChild, DuiNode* pRef=NULL);
	BOOL RemoveChildControl(DuiNode* pChild);
	void RemoveAllChildren();
	ILayout* GetLayout();
	void ResetLayout() { __cache.hasLayout = FALSE; }
	IVisual* GetVisual();
	void ResetVisual() { __cache.hasVisual = FALSE; }
	CDuiScrollbar* GetScrollbar(BOOL bVert=TRUE) { return (bVert?&m_scrollBar:&m_scrollBarH); }

	BOOL SetInnerXML(LPCOLESTR szXml);
	BOOL SetOuterXML(LPCOLESTR szXml);

	// for inline
	HRGN GetRegion() const { return m_hRgn; }

	//virtual void Init();
	void OnEvent(DuiEvent& event);
	void OnStateChanged(DWORD dwStateMask);
	BOOL OnChildStateChanged(HDE pChild, DWORD dwStateMask);
	void OnNotify(DuiNotify& msg);

	LONG GetAttributePos(LPCOLESTR szName) const;
	LONG GetAttributeCount() const;
	void SetAttribute(LPCOLESTR pstrName, CComVariant value);
	void SetAttributeByPos(LONG pos, CComVariant& value);
	void RemoveAttribute(LPCOLESTR szName);
	CComVariant GetAttribute(LPCOLESTR szName);
	CComVariant GetAttributeByPos(LONG pos);
	LPCOLESTR GetAttributeString(LPCOLESTR szName);

	static void CALLBACK OnAttributeChanged( BOOL bDeleted, LPCOLESTR szAttr, LPVOID pData );

	void OnAttributeChanged(LPCOLESTR szAttr);
	void OnChildAttributeChanged(DuiNode* pChild, LPCOLESTR szAttr);
	void OnAttributeRemoved(LPCOLESTR szAttr);
	void OnChildAttributeRemoved(DuiNode* pChild, LPCOLESTR szAttr);

	void OnStyleChanged(CDuiStyleVariant* newVal);

#ifndef NO3D
	// for D3D
	void Update3DState();
#endif
	void ProcessFont(HFONT hFont);

	// for IDispatch
	void SetObject(IDispatch* disp);
	IDispatch* GetObject(BOOL bAutoCreate=FALSE);

	// external callback
	void SetEventCallback(EVENTCALLBACK fnCallback);

	// eventHandler
	IDispatch* GetEventHandler(BOOL bAutoCreate=FALSE, BOOL bAddRef=FALSE);

	void PrepareDC(HDC hdc);
	void UpdateDrawData(InDrawData& idd);

	BOOL IsExtensionEvent(DuiEvent& event);

	// for container
	void SetAutoDestroy(BOOL bAuto) { m_bAutoDestroy = bAuto; }
	virtual int FindSelectable(int iIndex, BOOL bForward = TRUE);

	static void AdjustOffset(DuiNode* pSrc, DuiNode* pDst, POINT& pt);
	static void AdjustOffset(DuiNode* pSrc, DuiNode* pDst, RECT& rc);
	static POINT GetOffset(DuiNode* pSrc, DuiNode* pDst, POINT pt);
	static RECT GetOffset(DuiNode* pSrc, DuiNode* pDst, RECT rc);
	static BOOL IsChild(DuiNode* pParent, DuiNode* pChild, BOOL bIncludeSame=TRUE );

	static DUI_EVENTTYPE NameToEventType(LPCOLESTR lpszEventName);
	static LPCOLESTR NameToFinalName(LPCOLESTR lpszEventName);

	static void CALLBACK TransformProc( LPVOID pData, DWORD dwState );
	static void CALLBACK DrawToDC(HDC hdc, LPVOID lpData, DWORD dwState);
	static void CALLBACK DrawBackgroundToDC(HDC hdc, LPVOID lpData, const RECT rc);
	static POINT CALLBACK GetParentOrg(LPVOID lpData);

	UINT GetControlFlags();
	RECT GetClipRect( DUI_RECTTYPE rtp=DUIRT_CONTENT);
	RECT GetClientRect( DUI_RECTTYPE rtp=DUIRT_CONTENT );
	HRGN GetClipRgnBorder();
	HRGN GetClipRgnChildren();
	DuiNode* GetOffsetParent(BOOL bIncludeSelf=FALSE);
	SIZE GetOffsetParentSize(DUI_RECTTYPE rtp=DUIRT_CONTENT );
	DuiNode* GetNearestUpdateLayoutParent();
	void DelayedResize();
#ifndef NO3D
	void Update3DSurface();
#endif // NO3D
	void UpdateScrollRange();
	void UpdateInlineRegion();
	RECT GetPosBy( RECT rcRefTotal, DUI_RECTTYPE rtp=DUIRT_CONTENT );
	BOOL ParseStyle(LPCOLESTR lpszValue);
	BOOL TryParseAsStyle(LPCOLESTR lpszName, CComVariant v);
	BOOL TryRemoveAsStyle(LPCOLESTR lpszName);
	BOOL TryParseAsProcedure(LPCOLESTR lpszName, LPCOLESTR lpszValue);
	void OnPaintInlineBackground(HDC hdc, HRGN hrgn);
	POINT GetContentOrg(DuiNode* pRef=NULL);
	void UpdateLayout(DuiNode* pFromChild=NULL);
	SIZE GetContentSize();
	LONG GetMinWidth();
	LONG CalcContentMinWidth();
	LONG CalcContentHeight(LONG width);
	SIZE EstimateContentSize( SIZE szAvailable );
	BOOL SetScrollRange(LONG cx, LONG cy);
	void OnScrollPosChanged(BOOL bVert=TRUE, DuiNode* pParent=NULL );
	DUI_POSITION GetPosition();
	BOOL IsBlock();
	BOOL IsInline();
	BOOL IsInlineBlock();
	BOOL IsAllowScroll();
	BOOL PtInBackground( POINT pt);
};


//////////////////////////////////////////////////////////////////////////
// T2C/T2CX/T2CY - Total Size to Content Size
// C2T/C2TX/C2TY - Content Size to Total Size
class T2C
{
protected:
	SIZE _sz;
public:
	T2C(DuiNode* ctrl, LONG cx, LONG cy);
	operator SIZE() { return _sz; }
};
class T2CX : protected T2C
{
public:
	T2CX(DuiNode* ctrl, LONG cx) : T2C(ctrl, cx, 0) {}
	operator LONG() { return _sz.cx; }
};
class T2CY : protected T2C
{
public:
	T2CY(DuiNode* ctrl, LONG cy) : T2C(ctrl, 0, cy) {}
	operator LONG() { return _sz.cy; }
};

class C2T
{
protected:
	SIZE _sz;
public:
	C2T(DuiNode* ctrl);
	C2T(DuiNode* ctrl, LONG cx, LONG cy);
	operator SIZE() { return _sz; }
};
class C2TX : protected C2T
{
public:
	C2TX(DuiNode* ctrl, LONG cx) : C2T(ctrl, cx, 0) {}
	operator LONG() { return _sz.cx; }
};
class C2TY : protected C2T
{
public:
	C2TY(DuiNode* ctrl, LONG cy) : C2T(ctrl, 0, cy) {}
	operator LONG() { return _sz.cy; }
};

#endif // __DUIDEFCONTROL_H__