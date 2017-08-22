#if !defined(__DUIACTIVEX_H__)
#define __DUIACTIVEX_H__

#pragma once

#include "DuiManager.h"
#include "../../common/dispatchimpl.h"

/////////////////////////////////////////////////////////////////////////////////////
//

class CDuiActiveXEvent;
class CDuiActiveX;

class CDuiActiveXEvent : public IDispatch
{
	friend CDuiActiveX;

	CDuiActiveX* m_pOwner;
	IID m_iidEvent;
	DWORD m_dwCookie;

	typedef struct tagEventItem
	{
		DISPID dispid;
		CComBSTR name;
		//CComDispatchDriver callback;
	} EventItem;
	CSimpleArray<EventItem> m_events;

	void _CacheEventItems();

	EventItem* _ItemByDispid(DISPID dispid);
	EventItem* _ItemByName(LPCOLESTR name);

public:
	//CComDispatchDriver eventHandler;

	CDuiActiveXEvent(CDuiActiveX* pOwner);
	~CDuiActiveXEvent();

	/*static*/ HRESULT GetEventIID(IUnknown* pUnk, IID* piid);

	LPCOLESTR GetNameByDispid(DISPID dispid);
	DISPID GetDispidByName(LPCOLESTR name);

	BOOL Connect();
	void Disconnect();

	// IDispatch
public:
	STDMETHOD_(ULONG, AddRef)() { return 2; }
	STDMETHOD_(ULONG, Release)() { return 1; }
	STDMETHOD(QueryInterface)(REFIID iid, LPVOID* ppvObj)
	{
		if (!ppvObj) return E_POINTER;
		*ppvObj = NULL;

		if (IsEqualIID(iid, __uuidof(IUnknown)) ||
			IsEqualIID(iid, __uuidof(IDispatch)) ||
			(iid!=IID_NULL && iid==m_iidEvent))
			*ppvObj = (IDispatch*)this;

		return *ppvObj ? S_OK : E_NOINTERFACE;
	}
	STDMETHOD(GetTypeInfoCount)(UINT *pctinfo) { *pctinfo = 0; return E_NOTIMPL; }
	STDMETHOD(GetTypeInfo)(UINT /*iTInfo*/, LCID /*lcid*/, ITypeInfo **ppTInfo) { *ppTInfo = NULL; return E_NOTIMPL; }
	STDMETHOD(GetIDsOfNames)(REFIID riid, OLECHAR ** rgszNames, UINT cNames, LCID lcid, DISPID * rgDispId) { return E_NOTIMPL; }
	STDMETHOD(Invoke)(DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS * pdispparams, VARIANT *pVarResult, EXCEPINFO * pExcepInfo, UINT * puArgErr);
};

/////////////////////////////////////////////////////////////////////////////////////
//

class CDuiActiveX : public CDuiAxSite
					, public ISinkIsMyEvent
					, public ISinkOnScroll
					, public ISinkOn3DSurfaceCreated
					//, public ISinkOnGetControlFlags
					//, public CDuiMessageFilter
{
	friend CDuiActiveXEvent;
public:
	CDuiActiveX();
	virtual ~CDuiActiveX();

	DECLARE_CONTROL_EXTENSION(CDuiActiveX)
	DECLARE_CLASS_CAST(CDuiActiveX, CDuiAxSite, ISinkIsMyEvent, ISinkOnScroll, ISinkOn3DSurfaceCreated/*, ISinkOnGetControlFlags*/)
	DECLARE_CONTROL_FLAGS(DUIFLAG_NOFOCUSFRAME | DUIFLAG_NOCHILDREN)
	DECLARE_BASE_CLASS_STYLE("flag:tabstop; display:inlineblock; overflow:hidden;")

	virtual BOOL IsMyEvent(HANDLE, DuiEvent* pEvent);
	virtual void OnScroll(HANDLE, HDE pScroll, BOOL bVert=TRUE);
	void OnConnect();

	//void SetPos(RECT rc);
	//SIZE EstimateSize(SIZE szAvailable);
	virtual void OnPaintContent(HDE hde, HDC hdc, RECT rcPaint, BOOL bDraw, InDrawData* pidd);
	virtual void On3DSurfaceCreated(HANDLE);

	void BindSurfaceWindow();

	void SetAttribute(HDE hde, LPCWSTR szName, VARIANT* value);

	//LRESULT MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	//void OnControlEvent(LPCOLESTR szEventName, DISPPARAMS* dp, VARIANT* pResult);

	//// 脚本设置事件回调 obj.onEvent('Click', function(a,b){ this.tagName; });
	//bool onEvent(BSTR szEventName, VARIANT callback);
	HRESULT Getobject(VARIANT* pVar);
	//HRESULT GeteventHandler(VARIANT* pVar);
	//HRESULT PuteventHandler(VARIANT* pVar);

protected:
	void ReleaseControl();

protected:
	CLSID m_clsid;
	CDuiActiveXEvent m_sink;

	Begin_Disp_Map(CDuiActiveX)
		//Disp_Method(1, onEvent, bool, 2, BSTR, VARIANT)
		Disp_PropertyGet(2, object)
		//Disp_Property(3, eventHandler)
	End_Disp_Map()
};

THIS_IS_CONTROL_EXTENSION3(CDuiActiveX, ActiveX, Object, Embed)


#endif // !defined(__DUIACTIVEX_H__)
