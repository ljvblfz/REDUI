#if !defined(__DUIMANAGER_H__)
#define __DUIMANAGER_H__

#pragma once

#include "DirectUI.h"
#include "DuiBase.h"
#include "DuiImage.h"
#include "DuiMarkup.h"
#include "DuiStyleParser.h"
#include "DuiDefControl.h"
#ifndef NO3D
#include "DuiD3D.h"
#endif // NO3D
#include "script/pulsedef.h"
#include "xcp/XcpCtrl.h"

/////////////////////////////////////////////////////////////////////////////////////
//

class CDuiLayoutManager;
//class CDuiDefaultThemeManager;
class CDuiStyleVariant;
class CDuiStyle;
class CDuiScrollbar;
class DuiNode;
class CDuiAxContainer;
class CDuiAxSite;
class CDuiActiveX;
class CRenderContentOrg;

CDuiLayoutManager* LayoutMgrFromHwnd(HWND hwnd);

/////////////////////////////////////////////////////////////////////////////////////
//

#define TempParent ((DuiNode*)(DWORD_PTR)(DWORD)-1)

#define AutoValue -20040620


#ifdef _DEBUG
#define trace_c(x) ATLTRACE(L"\n%s(ID:%s,%x) - %S", GetName(this), GetID(this), GetState(this), x)
#define trace_rect() \
{ \
	CRect __rc = GetPos(DUIRT_TOTAL); \
	CRect __rc2 = GetPos(); \
	ATLTRACE(L" - rc(%d,%d,%d,%d) content(%d,%d,%d,%d)", __rc.left,__rc.top,__rc.right,__rc.bottom, __rc2.left,__rc2.top,__rc2.right,__rc2.bottom); \
}
#define trace_idd(idd) ATLTRACE(L" idd(pt(%d,%d), curline(%d), minheight(%d), lineindent(%d) textstyle(%08x)", idd.pt.x,idd.pt.y,idd.cyCurLine,idd.cyMinHeight, idd.lLineIndent, idd.uTextStyle)
#else
#define trace_c(x)
#define trace_rect()
#define trace_idd(idd)
#endif // _DEBUG

//////////////////////////////////////////////////////////////////////////
//
#define InvokeAllSinks(node, sinkname, ...) \
__if_exists(DefCallback::sinkname) { (node)->m_defproc.SetProc(OpCode_##sinkname, &DefCallback::sinkname); } \
{ \
	__if_exists(ISink##sinkname) \
	{ \
		ISink##sinkname* pBuf[128] = {0}; \
		for (int __idx=0, __cnt=(node)->GetClassCollection((LPVOID*)pBuf, CLASS_NAME(ISink##sinkname), 128); __idx<__cnt; __idx++) \
		{ pBuf[__idx]->sinkname((node)->Handle(), __VA_ARGS__ );  } \
	} \
	PluginCallContext(FALSE, (node)->Handle()) \
		__if_exists(DefCallback::sinkname){ .DefProc(&DefCallback::sinkname) } \
		.Call(OpCode_##sinkname, (node)->Handle(), __VA_ARGS__); \
}

#define InvokeAllSinksBool(node, sinkname, ...) \
__if_exists(DefCallback::sinkname) { (node)->m_defproc.SetProc(OpCode_##sinkname, &DefCallback::sinkname); } \
__if_not_exists(bDone) { BOOL bDone = FALSE; } \
{ \
	__if_exists(ISink##sinkname) \
	{ \
		ISink##sinkname* pBuf[128] = {0}; \
		for (int __idx=0, __cnt=(node)->GetClassCollection((LPVOID*)pBuf, CLASS_NAME(ISink##sinkname), 128); __idx<__cnt; __idx++) \
		{ if (pBuf[__idx]->sinkname((node)->Handle(), __VA_ARGS__ )) { bDone=TRUE; break; }  } \
		if (!bDone) \
	} \
		PluginCallContext(TRUE, (node)->Handle()).Output(&bDone) \
			__if_exists(DefCallback::sinkname){ .DefProc(&DefCallback::sinkname) } \
			.Call(OpCode_##sinkname, (node)->Handle(), __VA_ARGS__ ); \
}

#define InvokeOneSink(node, sinkname, ...) \
__if_exists(DefCallback::sinkname) { (node)->m_defproc.SetProc(OpCode_##sinkname, &DefCallback::sinkname); } \
__if_not_exists(bHasInvocation) { BOOL bHasInvocation = FALSE; } \
{ \
	__if_exists(ISink##sinkname) \
	{ \
		ISink##sinkname* pSink = CLASS_CAST(node, ISink##sinkname); \
		if (pSink) \
			(bHasInvocation=TRUE), pSink->sinkname((node)->Handle(), __VA_ARGS__); \
		else \
	} \
		bHasInvocation = PluginCallContext(TRUE, (node)->Handle()) \
			__if_exists(DefCallback::sinkname){ .DefProc(&DefCallback::sinkname) } \
			.Call(OpCode_##sinkname, (node)->Handle(), __VA_ARGS__ ).IsHandled(); \
}

#define InvokeOneSinkResult(retvar, node, sinkname, ...) \
__if_exists(DefCallback::sinkname) { (node)->m_defproc.SetProc(OpCode_##sinkname, &DefCallback::sinkname); } \
__if_not_exists(bHasInvocation) { BOOL bHasInvocation = FALSE; } \
{ \
	__if_exists(ISink##sinkname) \
	{ \
		ISink##sinkname* pSink = CLASS_CAST(node, ISink##sinkname); \
		if (pSink) \
			(bHasInvocation=TRUE), retvar = pSink->sinkname((node)->Handle(), __VA_ARGS__); \
		else \
	} \
		bHasInvocation = PluginCallContext(TRUE, (node)->Handle()).Output(&retvar) \
			__if_exists(DefCallback::sinkname){ .DefProc(&DefCallback::sinkname) } \
			.Call(OpCode_##sinkname, (node)->Handle(), __VA_ARGS__ ).IsHandled(); \
}

//////////////////////////////////////////////////////////////////////////
//
#define Var2Bstr(v) (SUCCEEDED((v).ChangeType(VT_BSTR)) ? (v).bstrVal : NULL)

#ifdef GDI_ONLY

#define __IsAlphaOrDD() __IsAlpha()

#else

//#ifndef NO3D
//#define __IsAlphaOrDD() (__IsAlpha() || __IsDD() || Get3DDevice())
//#else
#define __IsAlphaOrDD() (__IsAlpha() || __IsDD())
//#endif

#endif // GDI_ONLY

/////////////////////////////////////////////////////////////////////////////////////
//

// Flags for the dialog layout mode stretching
//#define DUISTRETCH_NEWGROUP   0x00000001
//#define DUISTRETCH_NEWLINE    0x00000002
#define DUISTRETCH_NO_MOVE_X     0x00000004
#define DUISTRETCH_NO_MOVE_Y     0x00000008
#define DUISTRETCH_NO_SIZE_X     0x00000010
#define DUISTRETCH_NO_SIZE_Y     0x00000020
#define DUISTRETCH_NO_MOVE (DUISTRETCH_NO_MOVE_X | DUISTRETCH_NO_MOVE_Y)
#define DUISTRETCH_NO_SIZE (DUISTRETCH_NO_SIZE_X | DUISTRETCH_NO_SIZE_Y)

// Flags for disabled operation
#define DUIDISABLED_INVALIDATE		0x00000001
#define DUIDISABLED_UPDATELAYOUT	0x00000002
#define DUIDISABLED_UPDATELAYOUT_FROMCHILD	0x00000004
#define DUIDISABLED_STATECHANGED	0x00000008
#define DUIDISABLED_CHILDSTATECHANGED	0x00000010


/////////////////////////////////////////////////////////////////////////////////////
//
//void fire_control_unload_events(CDuiControlExtensionBase* pCtrl, BOOL bIncludeChildren=TRUE, BOOL bIncludeSelf=TRUE);

#define __foreach_event_type(V) \
	V(mousemove) \
	V(mouseleave)	V(mouseout) \
	V(mouseenter)	V(mouseover) \
	V(mousehover) \
	V(keydown) \
	V(keyup) \
	V(char) \
	V(syskey) \
	V(killfocus)	V(blur) \
	V(setfocus)		V(focus) \
	V(buttondown) \
	V(buttonup) \
	V(click) \
	V(dblclick) \
	V(contextmenu) \
	V(vscroll) \
	V(hscroll) \
	V(scrollwheel) \
	V(size) \
	V(measureitem) \
	V(drawitem) \
	V(timer) \
	V(notify) \
	V(command) \
	V(load) \
	V(unload) \
	V(propertychange)


// System settings
typedef struct tagDuiSystemSettings
{
	BOOL bShowKeyboardCues;
} DuiSystemSettings;

// Various system settings
typedef struct tagDuiSystemMetrics
{
	INT cxvscroll;
} DuiSystemMetrics;


extern UINT guMsgMouseWheel;


// Listener interface
class CDuiNotifyListener
{
public:
	virtual void OnNotify(DuiNotify& msg) = 0;
};

// MessageFilter interface
class CDuiMessageFilter
{
public:
	virtual LRESULT MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) = 0;
};

// StyleSelector interface
class CDuiStyleSelector
{
public:
	virtual BOOL SelectorHandler(CDuiStyle* pTestStyle, DuiStyleAttributeType satFor, DWORD dwMatchResult) = 0;
};

// image/bitmap/icon resource
class CDuiImageResource : public UrlResources<CDuiImageResource, false>
{
public:
	CDuiImageResource(CDuiLayoutManager* lyt, LPCOLESTR lpszUrl, LPCOLESTR lpszId=NULL);
	~CDuiImageResource();

	BOOL IsNull() const;

	void SetClipRect(RECT& rcClip);

	union
	{
		CDuiImage* img;
#ifndef NO3D
		ImageResource* img3d;
#endif // NO3D
	};

	CDuiLayoutManager* pLayoutMgr;
	void OnLoadStream(LPSTREAM pStream) { img->OnLoadStream(pStream); }
private:

	void _ParseUrl(LPCOLESTR lpszUrl);
};


class CDuiObjectCreator
{
public:
	virtual IDispatch* GetWindowObject() = 0;
	virtual HRESULT CreateControlObject(DuiNode* pCtrl, IDispatch** ppDisp) = 0;
	virtual void OnLayoutBegin() = 0; // 开始加载一个新布局
	virtual void OnLayoutReady() = 0 ; // 布局解析完毕
	virtual void OnLayoutEnd() = 0;   // 布局即将卸载
	virtual void OnLayoutError() = 0 ; // 布局解析失败
};

//////////////////////////////////////////////////////////////////////////
//
class CounterLock
{
	LONG& __counter;
public:
	CounterLock(LONG& counter) : __counter(counter) { __counter++; }
	~CounterLock() { __counter--; }
};

//////////////////////////////////////////////////////////////////////////
// 附着到控件上的扩展

class ControlExtension : public IExtension
						, public ISinkOnGetControlFlags
						, public ISinkActivate
						, public ISinkSetAttribute
						, public ISinkGetAttribute
						, public ISinkGetStyleAttribute
						, public ISinkOnPaintContent
{
	DECLARE_BASE_CLASS_CAST(ControlExtension, ISinkOnGetControlFlags, ISinkActivate, ISinkSetAttribute, ISinkGetAttribute, ISinkGetStyleAttribute, ISinkOnPaintContent)
	DECLARE_NO_DELETE_THIS(ControlExtension)
public:
	DuiNode* m_pOwner;

	ControlExtension() : m_pOwner(NULL) {}

	static void CALLBACK __RegisterMyClassStyle(HWND hwnd, LPCOLESTR lpszName=NULL) {}

protected:
	void __Use(HANDLE hSource);
	void __Unuse(HANDLE);

public:
	virtual UINT OnGetControlFlags(HDE hde) { return 0; }
	virtual BOOL Activate(HDE hde) { BOOL b=FALSE; return m_pOwner->m_defproc.CallProc(&b, OpCode_Activate, hde); }
	virtual void SetAttribute(HDE hde, LPCWSTR szName, VARIANT* value) { m_pOwner->m_defproc.CallProc((DuiVoid*)NULL, OpCode_SetAttribute, hde, szName, value); }
	virtual void GetAttribute(HDE hde, LPCWSTR szName, VARIANT* value) { m_pOwner->m_defproc.CallProc((DuiVoid*)NULL, OpCode_GetAttribute, hde, szName, value); }
	virtual DuiStyleVariant* GetStyleAttribute(HDE hde, DuiStyleAttributeType sat, DuiStyleVariantType svt, DWORD dwMatchMask)
	{ DuiStyleVariant* sv=NULL; return m_pOwner->m_defproc.CallProc(&sv, OpCode_GetStyleAttribute, hde, sat, svt, dwMatchMask); }
	virtual void OnPaintContent(HDE hde, HDC hdc, RECT rcPaint, BOOL bDraw, InDrawData* pidd)
	{ m_pOwner->m_defproc.CallProc((DuiVoid*)0, OpCode_OnPaintContent, hde, hdc, rcPaint, bDraw, pidd); }
};


//////////////////////////////////////////////////////////////////////////
// ActiveX container and site
class CDuiAxContainer : public IOleInPlaceFrame,
						public IOleContainer,
						public CDuiMessageFilter
{
private:
	CDuiLayoutManager* m_pLayoutMgr;
	CStdPtrArray m_sites;

	// Ambient properties
	CComPtr<IFontDisp> m_spOleFont;

public:
	CDuiAxContainer(CDuiLayoutManager* pLayout) : m_pLayoutMgr(pLayout) {}

	BOOL RegistSite(CDuiAxSite* pSite, BOOL bUnReg=FALSE);

	BOOL OnAmbientProperty(CDuiAxSite* pSite, DISPID dispid, VARIANT* pvarResult);
	void CreateOleFont(HFONT hFont=NULL);
	virtual LRESULT MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	CComPtr<IOleInPlaceActiveObject> m_pInPlaceActiveObject;

public:
	// IUnknown
	STDMETHOD_(ULONG,AddRef)() { return 1; }
	STDMETHOD_(ULONG,Release)() { return 0; }
	STDMETHOD(QueryInterface)(REFIID riid, LPVOID *ppvObject);

	// IOleInPlaceFrame
	STDMETHOD(GetWindow)(HWND*);
	STDMETHOD(ContextSensitiveHelp)(BOOL);
	STDMETHOD(GetBorder)(LPRECT);
	STDMETHOD(RequestBorderSpace)(LPCBORDERWIDTHS);
	STDMETHOD(SetBorderSpace)(LPCBORDERWIDTHS);
	STDMETHOD(SetActiveObject)(LPOLEINPLACEACTIVEOBJECT, LPCOLESTR);
	STDMETHOD(InsertMenus)(HMENU, LPOLEMENUGROUPWIDTHS);
	STDMETHOD(SetMenu)(HMENU, HOLEMENU, HWND);
	STDMETHOD(RemoveMenus)(HMENU);
	STDMETHOD(SetStatusText)(LPCOLESTR);
	STDMETHOD(EnableModeless)(BOOL);
	STDMETHOD(TranslateAccelerator)(LPMSG, WORD);

	// IOleContainer
	STDMETHOD(ParseDisplayName)(LPBINDCTX, LPOLESTR, ULONG*, LPMONIKER*);
	STDMETHOD(EnumObjects)(DWORD, LPENUMUNKNOWN*);
	STDMETHOD(LockContainer)(BOOL);
};

class CDuiCustomOrg
{
	DECLARE_CLASS_NAME(CDuiCustomOrg)
public:
	virtual void SetOrg(HDC hdc) = 0; // 使用相对窗口客户区位置的控件应该继承此接口，用以提供自定义的内容原点
};

class CDuiAxSite : public IOleClientSite,
					public IOleInPlaceSiteWindowless,
					public IOleControlSite,
					public IXcpControlHost2, // for silverlight control
					public IServiceProvider,
					public IDispatch // AmbientProps
					, public CDuiCustomOrg
					, public ControlExtension
{
	friend CDuiAxContainer;

	DECLARE_BASE_CLASS_CAST(CDuiAxSite, CDuiCustomOrg, ControlExtension)
protected:
	CDuiLayoutManager* m_pLayoutMgr;
	CDuiAxContainer* m_pContainer;

	//LPVOID __cache;

	// 这个矩形是控件自身要求的位置，我们不用它设置实际位置，但是用它来估算大小
	// 这个矩形是相对窗口客户区的，不是相对父控件
	RECT __rcNeeded;

	CComPtr<IOleObject> m_spOleObject;
	CComPtr<IOleInPlaceObjectWindowless> m_spInPlaceObject; // IOleInPlaceObjectWindowless or IOleInPlaceObject
	CComPtr<IViewObjectEx> m_spViewObject;
	//CComQIPtr<IOleInPlaceActiveObject> m_pInPlaceActiveObject;
	DWORD m_dwMiscStatus;
	DWORD m_dwAspect;
	CONTROLINFO m_ctlInfo;
	DWORD m_dwViewObjectType;

	// state
	unsigned long m_bInPlaceActive:1;
	unsigned long m_bUIActive:1;
	unsigned long m_bMDIApp:1;
	unsigned long m_bWindowless:1;
	unsigned long m_bCapture:1;
	unsigned long m_bHaveFocus:1;
	unsigned long m_bReleaseAll:1;
	unsigned long m_bDCCached:1;

	//
	HDC m_hDCScreen;
	//CSurface* m_pSurface;
	// cache memdc
	HDC _memdc;
	HBITMAP _hbmp;
	HBITMAP _hbmpOld;

	void Init();

	void ReleaseSurface();
	void ResetSurface();

public:
	CDuiAxSite();
	~CDuiAxSite();

	DuiNode* owner() { return m_pOwner; }

	HRESULT CreateControl(REFCLSID clsid);
	HRESULT ActivateAx();
	void ReleaseAll();

	void GetControlInfo();

	RECT GetPos(); // 获取控件相对容器窗口客户区的位置
	RECT GetClip(BOOL bEnsureVisible=TRUE); // 相对于容器窗口客户区

	// PaintCallback::Do3DPaint
	//virtual void Do3DPaint();
	virtual void SetOrg(HDC hdc);

	HRESULT QueryControl(REFIID riid, LPVOID* ppv);
	template<typename Q> HRESULT QueryControl(Q** ppv) { return QueryControl(__uuidof(Q), (LPVOID*)ppv); }

public:
	// IUnknown
	STDMETHOD_(ULONG,AddRef)() { return 1; }
	STDMETHOD_(ULONG,Release)() { return 0; }
	STDMETHOD(QueryInterface)(REFIID riid, LPVOID *ppvObject);

	// IServiceProvider
	STDMETHOD(QueryService)(REFGUID guidService, REFIID riid, void **ppvObject);

	// IOleClientSite
	STDMETHOD(SaveObject)();
	STDMETHOD(GetMoniker)(DWORD, DWORD, LPMONIKER*);
	STDMETHOD(GetContainer)(LPOLECONTAINER*);
	STDMETHOD(ShowObject)();
	STDMETHOD(OnShowWindow)(BOOL);
	STDMETHOD(RequestNewObjectLayout)();

	// IOleInPlaceSiteWindowless
	STDMETHOD(GetWindow)(HWND*);
	STDMETHOD(ContextSensitiveHelp)(BOOL);

	STDMETHOD(CanInPlaceActivate)();
	STDMETHOD(OnInPlaceActivate)();
	STDMETHOD(OnUIActivate)();
	STDMETHOD(GetWindowContext)(LPOLEINPLACEFRAME*, LPOLEINPLACEUIWINDOW*, LPRECT, LPRECT, LPOLEINPLACEFRAMEINFO);
	STDMETHOD(Scroll)(SIZE);
	STDMETHOD(OnUIDeactivate)(BOOL);
	STDMETHOD(OnInPlaceDeactivate)();
	STDMETHOD(DiscardUndoState)();
	STDMETHOD(DeactivateAndUndo)();
	STDMETHOD(OnPosRectChange)(LPCRECT);

	STDMETHOD(OnInPlaceActivateEx)( BOOL* ptNoRedraw, DWORD dwFlags );
	STDMETHOD(OnInPlaceDeactivateEx)( BOOL tNoRedraw );
	STDMETHOD(RequestUIActivate)();

	STDMETHOD(AdjustRect)( LPRECT prect );
	STDMETHOD(CanWindowlessActivate)();
	STDMETHOD(GetCapture)();
	STDMETHOD(GetDC)( LPCRECT prect, DWORD dwFlags, HDC* phDC );
	STDMETHOD(GetFocus)();
	STDMETHOD(InvalidateRect)( LPCRECT prect, BOOL tErase );
	STDMETHOD(InvalidateRgn)( HRGN hRgn, BOOL tErase );
	STDMETHOD(OnDefWindowMessage)( UINT nMessage, WPARAM wParam, LPARAM lParam, LRESULT* plResult );
	STDMETHOD(ReleaseDC)( HDC hDC );
	STDMETHOD(ScrollRect)( int dx, int dy, LPCRECT prectScroll, LPCRECT prectClip );
	STDMETHOD(SetCapture)( BOOL tCapture );
	STDMETHOD(SetFocus)( BOOL tFocus );
	
	// IOleControlSite
	STDMETHOD(OnControlInfoChanged)();
	STDMETHOD(LockInPlaceActive)(BOOL fLock);
	STDMETHOD(GetExtendedControl)(LPDISPATCH* ppDisp);
	STDMETHOD(TransformCoords)(POINTL* lpptlHimetric, POINTF* lpptfContainer, DWORD flags);
	STDMETHOD(TranslateAccelerator)(LPMSG lpMsg, DWORD grfModifiers);
	STDMETHOD(OnFocus)(BOOL fGotFocus);
	STDMETHOD(ShowPropertyFrame)();
	
	// IDispatch // AmbientProps
	STDMETHOD(GetTypeInfoCount)(unsigned int*);
	STDMETHOD(GetTypeInfo)(unsigned int, LCID, ITypeInfo**);
	STDMETHOD(GetIDsOfNames)(REFIID, LPOLESTR*, unsigned int cNames, LCID, DISPID*);
	STDMETHOD(Invoke)(DISPID, REFIID, LCID, unsigned short, DISPPARAMS*, VARIANT*, EXCEPINFO*, unsigned int*);

 public:
	 // IXcpControlHost
	 STDMETHOD (GetHostOptions)(DWORD* pdwOptions);
	 STDMETHOD (NotifyLoaded)(void);
	 STDMETHOD (NotifyError)(BSTR bstrError, BSTR bstrSource, long nLine, long nColumn);
	 STDMETHOD (InvokeHandler)(BSTR bstrName, VARIANT varArg1, VARIANT varArg2, VARIANT* pvarResult);
	 STDMETHOD (GetBaseUrl)(BSTR* pbstrUrl);
	 STDMETHOD (GetNamedSource)(BSTR bstrSourceName, BSTR* pbstrSource);
	 STDMETHOD (DownloadUrl)(BSTR bstrUrl, IXcpControlDownloadCallback *pCallback, IStream **ppStream);

	 // IXcpControlHost2
	 STDMETHOD (GetCustomAppDomain)(IUnknown **ppAppDomain);
	 STDMETHOD (GetControlVersion)(UINT * puMajorVersion, UINT * puMinorVersion);
};


/////////////////////////////////////////////////////////////////////////////////////
//
class COleDuiWindow;

class  CDuiLayoutManager
{
	friend DuiNode;
	friend ControlExtension;
	friend COleDuiWindow;
	friend CDuiAxContainer;
	friend CDuiAxSite;
public:
	CDuiLayoutManager();
	~CDuiLayoutManager();

	// initialize
public:
	void Init(HWND hWnd, CDuiObjectCreator* pCreator=NULL, DuiNode* owner=NULL);
	BOOL LoadFromXml(LPCOLESTR lpszXml);
private:
	void _ParseConfig(CMarkupNode* pRoot);
	void _ParseResources(CMarkupNode* pRoot);
	BOOL _IsImageResource(CMarkupNode& r);
	BOOL _IsStyleResource(CMarkupNode& r);
	BOOL _IsScriptResource(CMarkupNode& r);
	BOOL _IsResource(CMarkupNode& r);
	DuiNode* _AddChild(CMarkupNode* pNode, DuiNode* pCtrlParent=NULL, BOOL bRemoveChildrenBefore=FALSE);

public:
	void UpdateLayout(BOOL bForce=FALSE);
	void Invalidate(LPRECT lpRect=NULL, BOOL bForce=FALSE);

#ifdef GDI_ONLY
	HDC GetPaintDC();
#else
	CSurface* GetSurface(BOOL bOffscreen=TRUE);
#endif // GDI_ONLY
	HWND GetPaintWindow() const;

#ifndef NO3D
	// D3D
	//static LPCWSTR prop_device;// = L"d3d_device";
	Device* Get3DDevice() const;
#endif // NO3D

	POINT GetMousePos() const;
	RECT GetClientRect() const;
	SIZE GetClientSize() const;

	void SetMinMaxInfo(int cx, int cy);

	HINSTANCE GetResourceInstance();
	HINSTANCE GetLanguageInstance();
	void SetResourceInstance(HINSTANCE hInst);
	void SetLanguageInstance(HINSTANCE hInst);


	BOOL AttachControl(DuiNode* pControl);
	BOOL InitControls(DuiNode* pControl, DuiNode* pParent = NULL);
	void ReapObjects(DuiNode* pControl);

	DuiNode* GetFocus() const;
	void SetFocus(DuiNode* pControl);
	DuiNode* GetCapture() const;
	void SetCapture(DuiNode* pCtrl);

	BOOL SetNextTabControl(BOOL bForward = TRUE);

	BOOL SetTimer(DuiNode* pControl, UINT nTimerID, UINT uElapse);
	BOOL KillTimer(DuiNode* pControl, UINT nTimerID);

	BOOL AddNotifier(CDuiNotifyListener* pControl);
	BOOL RemoveNotifier(CDuiNotifyListener* pControl);   
	void SendNotify(DuiNotify& Msg);
	void SendNotify(DuiNode* pControl, LPCOLESTR pstrMessage, WPARAM wParam = 0, LPARAM lParam = 0);

	BOOL AddMessageFilter(CDuiMessageFilter* pFilter);
	BOOL RemoveMessageFilter(CDuiMessageFilter* pFilter);

	//void SetEvent(DuiEvent* pEvent) {m_pEvent=pEvent;}
	DuiEvent* GetEvent() const {return m_pEvent;}
	BOOL FireEvent(DuiNode* pCtrl, DuiEvent& event);

	DuiNode* FindControl(POINT pt) const;
	DuiNode* FindControl(LPCOLESTR pstrId);

	static void MessageLoop();
	static BOOL PreTranslateMessage(const LPMSG pMsg);

	BOOL MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& lRes);
	BOOL DefMessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& lRes);
	BOOL PreMessageHandler(LPMSG pMsg);

	static DuiSystemMetrics GetSystemMetrics();
	static DuiSystemSettings GetSystemSettings();
	static void SetSystemSettings(const DuiSystemSettings Config);

	BOOL BuildControlPen(DuiNode* pCtrl, LPLOGPEN pLogPen, CDuiStyleSelector* proc=NULL);
	BOOL BuildControlFont(DuiNode* pCtrl, LPLOGFONTW pLogFont, CDuiStyleSelector* proc=NULL);

	// CDuiLayoutManagerBase
public:
	CDuiStyle* CreateStyle(LPCOLESTR lpszTargetName=NULL, LPCOLESTR lpszTargetId=NULL, DWORD dwTargetState=0, DuiNode* pOwnerCtrl=NULL, CDuiStyle* pRefStyle=NULL);
	CDuiStyle* ParseStyle(LPCOLESTR lpszTargetName, DWORD dwTargetState, LPCOLESTR lpszStyle);
	DuiNode* CreateControl(DuiNode* pParent, LPCOLESTR lpszName);

	void RemoveStyle(CDuiStyle* pStyle);
	BOOL HasStyle(CDuiStyle* pStyle);
	CDuiStyle* GetStyle(LPCOLESTR lpszTargetName=NULL, LPCOLESTR lpszTargetId=NULL, DWORD dwTargetState=0, BOOL bAutoCreate=FALSE, DuiStyleAttributeType satFor=SAT__UNKNOWN, DuiNode* pOwner=NULL);
	CDuiStyle* __FindStyle(CDuiStyleSelector* proc, DuiNode* pCtrl, DuiStyleAttributeType satFor=SAT__UNKNOWN);
	CDuiStyle* FindSimilarStyle(DuiNode* pCtrl, DWORD& dwMatch, DuiStyleAttributeType satFor=SAT__UNKNOWN);
	CDuiStyle* GetStyleById(LPCOLESTR lpszId);

	void InitCommonStyles();
	void UpdateStylesInherit();

	// resources
public:
	CDuiImageResource* CreateImageResource(LPCOLESTR lpszUrl, LPCOLESTR lpszId=NULL);
	CDuiImageResource* GetImageResource(LPCOLESTR szIdorUrl, BOOL bAutoCreate=FALSE);

	// attributes
public:
	inline const CAttributeMap& GetAttributes() const { return m_aAttrs; }
	BOOL GetAttributeBool(LPCOLESTR szAttr, BOOL bDef=FALSE) const;

	// object
public:
	void CreateControlDispatch(DuiNode* pCtrl);

	// 脚本相关
public:
	CScript m_script;
	//psHSCRIPT m_hScript;
	HRESULT ParseProcedure(LPCOLESTR lpszCode, IDispatch** ppDisp);
	HRESULT ParseExpression(LPCOLESTR lpszCode, IDispatch** ppDisp);

	// helper
public:
	inline void DoNothing() {}

private:
	void DrawChildrenHwnd(HDC hdc=NULL, HWND hwndChild = NULL);

private:
	static HDE CALLBACK __FindControlFromId(HDE pThis, LPVOID pData);
	static HDE CALLBACK __FindControlFromCount(HDE pThis, LPVOID pData);
	static HDE CALLBACK __FindControlFromPoint(HDE pThis, LPVOID pData);
	static HDE CALLBACK __FindControlFromTab(HDE pThis, LPVOID pData);
	static HDE CALLBACK __FindControlFromShortcut(HDE pThis, LPVOID pData);

#ifdef GDI_ONLY
	void __PrepareDC();
#else
	void __PrepareSurface();
	BOOL __IsDD() const
	{
		return _primarySurface!=NULL
#ifndef NO3D
			|| Get3DDevice()!=NULL
#endif // NO3D
			;
	}
#endif // GDI_ONLY
	BOOL __IsAlpha() const;
	void __Draw();
	void __AdjustPoint(LPPOINT ppt);

private:
	DuiNode* __owner_ctrl; // 当布局管理器嵌入其它布局管理器中时，此成员代表属主控件，资源类型的访问函数的调用都会委托给属主控件的布局管理器
	HWND m_hWndPaint;
	HWND __hwndTop; // 顶级父窗口或属主窗口
	HBITMAP m_hbmpOffscreen;
#ifdef GDI_ONLY
	HDC m_hDcPaint;
	HDC m_hDcOffscreen;
#else
	CSurface* m_pSurface;
	CSurface* m_pSurfaceOffscreen;

	CPrimarySurface* _primarySurface;
	COffscreenSurface* _offscrnSurface;
	CGdiSurface* _gdiSurface;
	COffscreenGdiSurface* _offscrnGdiSurface;
#endif // GDI_ONLY

#ifndef NO3D
	// D3D
	Device* m_pDevice;
	BOOL m_bD3DEnabled;
#endif // NO3D

	BOOL m_bDestroying;
	LONG m_lPaintLocked;

	//////////////////////////////////////////////////////////////////////////
	// for layered window
	LPDWORD m_pBits;
	BOOL m_bInvalid;
	BOOL m_bAlphaEnabled;

	HWND m_hwndTooltip;
	TOOLINFO m_ToolTip;
	//
	DuiNode* m_pRoot;
	DuiNode* m_pFocus;
	DuiNode* m_pEventHover;
	DuiNode* m_pEventCaptured;
	DuiNode* m_pEventKey;
	//
	POINT m_ptLastMousePos;
	BOOL m_bDblClick;
	DuiNode* m_pClick;
	SIZE m_szMinWindow;
	//UINT m_uMsgMouseWheel;
	UINT m_uTimerID;
	BOOL m_bFirstLayout;
	BOOL m_bResizeNeeded;
	BOOL m_bFocusNeeded;
	BOOL m_bOffscreenPaint;
	BOOL m_bMouseTracking;
	// XML
	CMarkup m_xml;
	CComBSTR m_strError;
	//
	static DuiSystemMetrics m_SystemMetrics;
	static DuiSystemSettings m_SystemConfig;
	//
	CStdPtrArray m_aNotifiers;
	CStdPtrArray m_aTimers;
	CStdPtrArray m_aMessageFilters;
	CStdPtrArray m_aDelayedCleanup;
	//
	HINSTANCE m_hLangInst;
	HINSTANCE m_hInstance;
	static CStdPtrArray m_aPreMessages;
	CDuiObjectCreator* m_pObjectCreator;
	//
	DuiEvent* m_pEvent;
	//
	CStdPtrArray m_aStyles; // CDuiStyle*
	//CStdPtrArray m_aResources; // CDuiImageResource*
	CStdPtrArray m_aChildHwnds; // HWND
	//
	CAttributeMap m_aAttrs;
	//
	CDuiAxContainer m_AxContainer;
	//
	DuiNode* m_fixedChildren;

public:
	CDuiImageResource* m_Images;
};


//////////////////////////////////////////////////////////////////////////
//
typedef enum
{
	ext_unknown			= 0,

	// DuiStyleVariant::extVal == CDuiImage*
	ext_image_normal,	// Image标准模式，左上对齐
	ext_image_center,	// Image居中模式
	ext_image_tile,		// Image平铺模式
	ext_image_stretch,	// Image拉伸模式
	ext_image_maxfit,	// Image拉伸模式，最大适合，保持图片宽高比例
	ext_image_squares,	// Image九宫格模式，四个角不拉伸，四条边单维拉伸，中间采用跟 stretch 相同的二维拉伸。注：九宫格属性属于图片
	// 如果图片含有九宫格数据，则上述图片绘制模式仅作用于中间格

} ExternalType;

template<typename T>
class svHelper
{
	//static T DuiStyleVariant::* pmField;
};

//template<> class svHelper<BOOL>
//{
//public:
//	static const DuiStyleVariantType svt = SVT_BOOL;
//	static BOOL DuiStyleVariant::* pmField;
//};
//__declspec(selectany) BOOL DuiStyleVariant::* svHelper<BOOL>::pmField = &DuiStyleVariant::boolVal;

#define DEFINE_STYLE_VARIANT_HELPER(_type, svt_def, svt_more, field, _default) \
template<> class svHelper<_type> \
{ \
public: \
	static _type defValue; \
	static const DuiStyleVariantType defsvt = (DuiStyleVariantType)(svt_def); \
	static const DuiStyleVariantType svt = (DuiStyleVariantType)((svt_def) | (svt_more)); \
	static _type DuiStyleVariant::* pmField; \
}; \
__declspec(selectany) _type svHelper<_type>::defValue = (_type)(_default); \
__declspec(selectany) _type DuiStyleVariant::* svHelper<_type>::pmField = (_type DuiStyleVariant::*)&DuiStyleVariant::field;

DEFINE_STYLE_VARIANT_HELPER(BOOL, SVT_BOOL, 0, boolVal, FALSE)
DEFINE_STYLE_VARIANT_HELPER(LONG, SVT_LONG, 0, longVal, 0)
DEFINE_STYLE_VARIANT_HELPER(DOUBLE, SVT_DOUBLE, SVT_PERCENT, doubleVal, 0.0)
DEFINE_STYLE_VARIANT_HELPER(LPCOLESTR, SVT_STRING, SVT_EXPRESSION, strVal, NULL)
DEFINE_STYLE_VARIANT_HELPER(COLORREF, SVT_COLOR, 0, colorVal, CLR_INVALID)
DEFINE_STYLE_VARIANT_HELPER(LPLOGPEN, SVT_PEN, 0, penVal, NULL)
DEFINE_STYLE_VARIANT_HELPER(LPLOGFONTW, SVT_FONT, 0, fontVal, NULL)
DEFINE_STYLE_VARIANT_HELPER(HBRUSH, SVT_BRUSH, 0, brushVal, NULL)
DEFINE_STYLE_VARIANT_HELPER(HBITMAP, SVT_BITMAP, 0, bitmapVal, NULL)
DEFINE_STYLE_VARIANT_HELPER(HICON, SVT_ICON, SVT_CURSOR, iconVal, NULL)
//DEFINE_STYLE_VARIANT_HELPER(HCURSOR, SVT_ICON|SVT_CURSOR, iconVal, NULL)
DEFINE_STYLE_VARIANT_HELPER(LPRECT, SVT_RECT, 0, rectVal, NULL)
DEFINE_STYLE_VARIANT_HELPER(LPCRECT, SVT_RECT, 0, rectVal, NULL)
DEFINE_STYLE_VARIANT_HELPER(LPSIZE, SVT_SIZE, 0, sizeVal, NULL)
DEFINE_STYLE_VARIANT_HELPER(LPPOINT, SVT_POINT, 0, pointVal, NULL)
DEFINE_STYLE_VARIANT_HELPER(LPVOID, SVT_EXTERNAL, 0, extVal, NULL)

#define OPERATOR_ASSIGN_POINTER(_type, svtmask) \
template<> inline void CDuiStyleVariant::operator=</*const*/ _type*> (/*const*/ _type* pVal) \
{ \
	if (pVal==NULL) return; \
	if ((svt&SVT_MASK_ALL)!=SVT_EMPTY && ((svt&SVT_MASK_ALL)&svHelper<_type*>::svt)==0) return; \
	svt = (DuiStyleVariantType)(svHelper<_type*>::defsvt | svtmask); \
	if (this->* svHelper<_type*>::pmField == NULL) this->* svHelper<_type*>::pmField = NEW _type; \
	*(this->* svHelper<_type*>::pmField) = *pVal; \
}

#define FromStyleVariant(sv, type, ...) (((CDuiStyleVariant*)(sv))->operator()<type>)(__VA_ARGS__)

class CDuiStyleVariant : public DuiStyleVariant
{
public:
	CDuiStyleVariant();
	CDuiStyleVariant(DuiStyleAttributeType _sat);
	CDuiStyleVariant(DuiStyleAttributeType _sat, BOOL bVal);
	CDuiStyleVariant(DuiStyleAttributeType _sat, LONG lVal);
	CDuiStyleVariant(DuiStyleAttributeType _sat, DOUBLE dVal);
	CDuiStyleVariant(DuiStyleAttributeType _sat, LPCOLESTR lpszVal, DuiStyleVariantType _svt=SVT_STRING);
	CDuiStyleVariant(DuiStyleAttributeType _sat, COLORREF clr);
	CDuiStyleVariant(DuiStyleAttributeType _sat, HPEN hPen);
	CDuiStyleVariant(DuiStyleAttributeType _sat, LOGPEN* pLogPen);
	CDuiStyleVariant(DuiStyleAttributeType _sat, HFONT hFont);
	CDuiStyleVariant(DuiStyleAttributeType _sat, LOGFONT* pLogFont);
	CDuiStyleVariant(DuiStyleAttributeType _sat, HBRUSH hBrush);
	CDuiStyleVariant(DuiStyleAttributeType _sat, LOGBRUSH* pLogBrush);
	CDuiStyleVariant(DuiStyleAttributeType _sat, HICON hIcon, DuiStyleVariantType _svt=SVT_ICON);
	CDuiStyleVariant(DuiStyleAttributeType _sat, HBITMAP hBitmap);
	CDuiStyleVariant(DuiStyleAttributeType _sat, BITMAP* pBitmap);
	//CDuiStyleVariant(DuiStyleAttributeType _sat, HCURSOR hCursor);
	CDuiStyleVariant(DuiStyleAttributeType _sat, LPCRECT pRect);
	CDuiStyleVariant(DuiStyleAttributeType _sat, LPSIZE pSize);
	CDuiStyleVariant(DuiStyleAttributeType _sat, LPPOINT pPoint);
	CDuiStyleVariant(DuiStyleAttributeType _sat, LPVOID pExternal, UINT_PTR uiData);

	~CDuiStyleVariant();

	// operators and condition
public:

	// 取消了操作符类型CAST，改成使用函数调用的操作符重载，好处是能够增加附加参数
	// sv->operator()<LONG> (pRef, reserved);，例如以前用 (long)*sv 等效于现在的 FromStyleVariant(sv, long [,pRef [,reserved]])
	template<typename T>
	T operator () (LPVOID pRef=NULL, LONG_PTR reserved=0, T defaultValue=0/*svHelper<T>::defValue*/)
	{
		return ((svt&SVT_MASK_ALL) & svHelper<T>::svt) ? (this->* svHelper<T>::pmField) : (defaultValue==0?svHelper<T>::defValue:defaultValue);
	}

	// operator =
	template<typename T>
	void operator = (T tVal)
	{
		if (svt!=SVT_EMPTY && ((svt&SVT_MASK_ALL) & svHelper<T>::svt)==0)
			return;

		svt = svHelper<T>::svt;
		(this->* svHelper<T>::pmField) = tVal;
	}


	BOOL GetLogBrush(LOGBRUSH* pLogBrush);
	BOOL GetBitmap(BITMAP* pBitmap);
	void SetPen(LOGPEN* pRefPen, LONG mask=SVT_PEN_MASK_ALL);
	void SetFont(LOGFONTW* pRefFont, LONG mask=SVT_FONT_MASK_ALL);

	BOOL IsEmpty();
	virtual LPVOID GetExternal();

public:
	static BOOL Parse(CDuiLayoutManager* pLayoutMgr, CDuiStyle* pStyle, LPCOLESTR lpszAttrName, CComVariant v);

public:
	UINT_PTR __cache;		// svt==SVT_FONT: __cache==HFONT
							// svt==SVT_PEN: __cache==HPEN
							// svt==SVT_BRUSH: __cache==LOGBRUSH*
							// svt==SVT_BITMAP: __cache==BITMAP* or DIBSECTION*
							// svt==SVT_EXPRESSION: __cache==CDuiLayoutManager*
							// svt==SVT_EXTERNAL: __cache==ExternalType
};

__declspec(selectany) CDuiStyleVariant svEmpty;

// 以下是内联特化实现，包括赋值和函数调用
// operator =
template<> inline void CDuiStyleVariant::operator =<LPCOLESTR> (LPCOLESTR tVal)
{
	if ((svt&SVT_MASK_ALL)!=SVT_EMPTY && (svt&SVT_MASK_ALL)!=SVT_STRING)
		return;
	//if (tVal==NULL) return;

	if (svt==SVT_STRING && strVal) delete [] strVal;
	svt = SVT_STRING;
	strVal = NEW OLECHAR[lstrlenW(tVal) + 1];
	if (strVal)
		lstrcpyW((LPWSTR)strVal, tVal);
}

OPERATOR_ASSIGN_POINTER(LOGPEN, SVT_PEN_MASK_ALL)
OPERATOR_ASSIGN_POINTER(LOGFONTW, SVT_FONT_MASK_ALL)
OPERATOR_ASSIGN_POINTER(RECT, 0)
OPERATOR_ASSIGN_POINTER(SIZE, 0)
OPERATOR_ASSIGN_POINTER(POINT, 0)

template<> inline void CDuiStyleVariant::operator=<LPLOGBRUSH> (LPLOGBRUSH pLogBrush)
{
	if (pLogBrush == NULL)
		return;

	if ((svt&SVT_MASK_ALL)!=SVT_EMPTY && (svt&SVT_MASK_ALL)!=SVT_BRUSH)
		return;

	svt = SVT_BRUSH;
	if (__cache == NULL) __cache = (UINT_PTR)NEW LOGBRUSH;
	*(LOGBRUSH*)__cache = *pLogBrush;
}

template<> inline void CDuiStyleVariant::operator=<LPBITMAP> (LPBITMAP pBitmap)
{
	if (pBitmap == NULL)
		return;

	if ((svt&SVT_MASK_ALL)!=SVT_EMPTY && (svt&SVT_MASK_ALL)!=SVT_BITMAP)
		return;

	svt = SVT_BITMAP;
	if (__cache == NULL) __cache = (UINT_PTR)NEW BITMAP;
	*(BITMAP*)__cache = *pBitmap;
}

// operator()
template<> inline HBRUSH CDuiStyleVariant::operator()<HBRUSH> (LPVOID pRef, LONG_PTR reserved, HBRUSH defaultValue)
{
	if ((svt&SVT_MASK_ALL) != SVT_BRUSH)
		return NULL;

	if (__cache)
	{
		if (brushVal) DeleteObject(brushVal);
		brushVal = CreateBrushIndirect((const LOGBRUSH*)__cache);
		delete (LOGBRUSH*)__cache;
		__cache = NULL;
	}
	return brushVal;
}

template<> inline HBITMAP CDuiStyleVariant::operator()<HBITMAP> (LPVOID pRef, LONG_PTR reserved, HBITMAP defaultValue)
{
	if ((svt&SVT_MASK_ALL) != SVT_BITMAP)
		return NULL;

	if (__cache)
	{
		if (bitmapVal) DeleteObject(bitmapVal);
		bitmapVal = CreateBitmapIndirect((const BITMAP*)__cache);
		delete (BITMAP*)__cache;
		__cache = NULL;
	}
	return bitmapVal;
}

template<> inline HPEN CDuiStyleVariant::operator()<HPEN> (LPVOID pRef, LONG_PTR reserved, HPEN defaultValue)
{
	LOGPEN* pRefPen = (LOGPEN*)pRef;
	if (pRefPen==NULL || (svt&SVT_MASK_ALL)!=SVT_PEN)
		return NULL;

	if (penVal==NULL) return NULL;

	if (svt & SVT_PEN_MASK_WIDTH) pRefPen->lopnWidth.x = penVal->lopnWidth.x;
	if (svt & SVT_PEN_MASK_COLOR) pRefPen->lopnColor = penVal->lopnColor;
	if (svt & SVT_PEN_MASK_STYLE) pRefPen->lopnStyle = penVal->lopnStyle;

	if (__cache) ::DeleteObject((HPEN)__cache);
	__cache = (UINT_PTR)::CreatePenIndirect(pRefPen);
	return (HPEN)__cache;
}

template<> inline HFONT CDuiStyleVariant::operator()<HFONT> (LPVOID pRef, LONG_PTR reserved, HFONT defaultValue)
{
	LPLOGFONTW pRefFont = (LPLOGFONTW)pRef;
	if (pRefFont==NULL || (svt&SVT_MASK_ALL)!=SVT_FONT)
		return NULL;
	if (fontVal==NULL) return NULL;

	if (svt & SVT_FONT_MASK_FAMILY) lstrcpyW(pRefFont->lfFaceName, fontVal->lfFaceName);
	if (svt & SVT_FONT_MASK_SIZE) pRefFont->lfHeight = fontVal->lfHeight, pRefFont->lfWidth = 0;
	if (svt & SVT_FONT_MASK_STYLE) pRefFont->lfWeight=fontVal->lfWeight, pRefFont->lfUnderline=fontVal->lfUnderline, pRefFont->lfItalic=fontVal->lfItalic, pRefFont->lfStrikeOut=fontVal->lfStrikeOut;

	//return NULL;
	if (__cache) ::DeleteObject((HFONT)__cache);
	__cache = (UINT_PTR)::CreateFontIndirectW(pRefFont);
	return (HFONT)__cache;
}

template<> inline LONG CDuiStyleVariant::operator()<LONG> (LPVOID pRef, LONG_PTR reserved, LONG defaultValue)
{
	// 变量类型是AUTO，表示此样式值动态计算，这里返回 AutoValue
	if ((svt&SVT_MASK_ALL)==SVT_AUTO) return AutoValue;

	// 当变量类型是百分比，且pRef是一个LONG型的正整数，以其为总数计算百分比
	if ((svt&SVT_MASK_ALL)==SVT_PERCENT && ((LONG)(LONG_PTR)pRef)>0 && doubleVal>=0.0 && doubleVal<=1.0)
	{
		return (LONG)(((LONG)(LONG_PTR)pRef) * doubleVal);
	}

	return ((svt&SVT_MASK_ALL) & svHelper<LONG>::svt) ? (this->* svHelper<LONG>::pmField) : (defaultValue==0?svHelper<LONG>::defValue:defaultValue);
}

// 内联特化结束
//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
//

class CDuiStyle : public CDuiConnectorCallback
{
public:
	CDuiStyle(CDuiLayoutManager* pLayoutMgr,LPCOLESTR lpszTargetName=NULL, LPCOLESTR lpszTargetId=NULL, DWORD dwTargetState=0, DuiNode* pOwnerCtrl=NULL, CDuiStyle* pRefStyle=NULL);
	~CDuiStyle();

	// CDuiStyle
public:
	LPCOLESTR GetTargetName() const;
	LPCOLESTR GetTargetId() const;
	DWORD GetTargetState() const;
	DuiNode* GetOwnerControl() const { return m_pOwnerCtrl; }
	LPCOLESTR GetId() const { return !m_strId.IsEmpty() ? (LPCOLESTR)m_strId : NULL; }

	//BOOL IsTargetNameSame(LPCOLESTR name, BOOL bMustTargetName=FALSE) const;

	// 级联样式链。一个样式不关注的样式属性可以从父样式中查找
	CDuiStyle* GetParentStyle() const { return m_pParent; }
	void SetParentStyle(CDuiStyle* pParentStyle)
	{
		m_pParent = pParentStyle;
	}


	// helper methods
public:
	CDuiStyleVariant* GetPrivateAttribute(DuiStyleAttributeType sat, BOOL bAutoCreate=FALSE);
	//CDuiStyleVariant* SetPrivateAttribute(CDuiStyleVariant* pStyleVar);
	CDuiStyleVariant* GetAttribute(DuiStyleAttributeType sat, DuiStyleVariantType svt=SVT_EMPTY);
	CDuiStyleVariant* GetAttributeNoControl(DuiStyleAttributeType sat, DuiStyleVariantType svt=SVT_EMPTY);
	void RemoveAttribute(DuiStyleAttributeType sat);

	BOOL ParseStyle(LPCOLESTR lpszValue);

	void SetId(LPCOLESTR szId) { m_strId = szId; }

	int GetKindOf(CDuiStyle* pDest); // 返回目标相对于我的关系

	void FireChangeEvent(CDuiStyleVariant* newVal);
	virtual BOOL OnHit(LPVOID pClient, LPVOID pServer, DWORD_PTR protocol);

	// members
public:
	enum
	{
		kind_same,
		kind_parent,
		kind_child,
		kind_unrelated
	};
	CStdPtrArray m_StyleAttributes; // array for CDuiStyleVariant*

	CDuiStyle* m_pParent;
	CStdString m_strId;

private:
	// 私有成员只允许构造时设置
	CStdString m_strTargetName;
	CStdString m_strTargetId;
	DWORD m_dwTargetState;
	DuiNode* m_pOwnerCtrl;

	CDuiStyle* m_pRefStyle; // 当克隆时，并不实际复制数据，而是保留来源指针，注意判断指针有效性
	CDuiLayoutManager* m_pLayoutMgr;

	void CheckRefStyle();
};


class CRenderContentOrg
{
	HDC _hdc;
	POINT _pt;
public:
	// refCtrl 是参考元素，元素内容原点相对于此参考元素的原点偏移，为 NULL 表示相对于窗口客户区原点
	CRenderContentOrg(HDC hdc, DuiNode* ctrl, DuiNode* refCtrl=NULL) : _hdc(hdc)
	{
		::GetWindowOrgEx(_hdc, &_pt);
		POINT pt = ctrl->GetContentOrg(refCtrl);
		pt.x += _pt.x;
		pt.y += _pt.y;
		::SetWindowOrgEx(_hdc, pt.x, pt.y, NULL);
	}
	CRenderContentOrg(HDC hdc, POINT pt) : _hdc(hdc)
	{
		::GetWindowOrgEx(_hdc, &_pt);
		pt.x += _pt.x;
		pt.y += _pt.y;
		::SetWindowOrgEx(_hdc, pt.x, pt.y, NULL);
	}
	~CRenderContentOrg()
	{
		::SetWindowOrgEx(_hdc, _pt.x, _pt.y, NULL);
	}
};

template<bool negative/*包含负数*/, bool positive/*包含正数*/>
struct COrderListFinder
{
	static DuiNode* Find(DuiNode* ri, FINDCONTROLPROC Proc, LPVOID pData, UINT uFlags)
	{
		if (!negative) // 不包含负数，直接跳过
		{
			while (ri && ri->GetZIndex()<0) ri=ri->next;
		}
		if (ri==NULL) return NULL;
		if (!positive && ri->GetZIndex()>=0) return NULL; // 不包含正数则停止

		DuiNode* p = NULL;
		if (ri->next) p = Find(ri->next, Proc, pData, uFlags);
		if (p) return p;
		return ri->FindControl(Proc, pData, uFlags);
	}
};


#endif // !defined(__DUIMANAGER_H__)

