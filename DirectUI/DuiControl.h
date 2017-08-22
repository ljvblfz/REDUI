// DuiControl.h : COleDuiControl 的声明

#pragma once
#include "resource.h"       // 主符号
#include <DispEx.h>

#include "DirectUI_i.h"
#include "duicore/DuiManager.h"
#include "../common/dispatchimpl.h"


#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Windows CE 平台(如不提供完全 DCOM 支持的 Windows Mobile 平台)上无法正确支持单线程 COM 对象。定义 _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA 可强制 ATL 支持创建单线程 COM 对象实现并允许使用其单线程 COM 对象实现。rgs 文件中的线程模型已被设置为“Free”，原因是该模型是非 DCOM Windows CE 平台支持的唯一线程模型。"
#endif


#define CheckTargetValid(ptr, ptr_type)  if (((ptr)==NULL) || (::IsBadReadPtr(ptr, (UINT_PTR)sizeof(ptr_type)))) return RPC_E_DISCONNECTED;



// {9EAC9EC2-1514-4403-84AB-624FD1E3E408}
EXTERN_C const GUID DECLSPEC_SELECTANY IID_IDuiControlSign = 
{ 0x9eac9ec2, 0x1514, 0x4403, { 0x84, 0xab, 0x62, 0x4f, 0xd1, 0xe3, 0xe4, 0x8 } };
MIDL_INTERFACE("9EAC9EC2-1514-4403-84AB-624FD1E3E408")
IDuiControlSign : public IUnknown
{
public:
	virtual DuiNode* STDMETHODCALLTYPE GetControl() = 0;
};


// COleDuiControl

class ATL_NO_VTABLE COleDuiControl :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<COleDuiControl, &CLSID_DuiControl>,
	public IDispatchImpl<IDuiControl, &IID_IDuiControl, &LIBID_DirectUILib, /*wMajor =*/ 0xffff, /*wMinor =*/ 0xffff>,
	public IDuiControlSign
{
public:
	COleDuiControl() : m_pCtrl(NULL), m_peer(NULL), m_peerUnk(&m_peer)
	{
	}

DECLARE_REGISTRY_RESOURCEID(IDR_DUICONTROL)


BEGIN_COM_MAP(COleDuiControl)
	COM_INTERFACE_ENTRY(IDuiControl)
	COM_INTERFACE_ENTRY(IDispatch)
	//COM_INTERFACE_ENTRY(IDispHost)
	COM_INTERFACE_ENTRY(IDuiControlSign)
	COM_INTERFACE_ENTRY_AGGREGATE_BLIND(m_peerUnk)
END_COM_MAP()



	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct()
	{
		return S_OK;
	}

	void FinalRelease()
	{
		if (m_pCtrl && !::IsBadReadPtr(m_pCtrl, sizeof(DuiNode)))
		{
			DuiNode* r = (m_pCtrl);
			r->m_eventHandler = (IDispatch*)NULL;
			r->SetEventCallback(NULL);
			m_peer.RemoveObject(r->_attributes.GetDispatch());
		}
		m_pCtrl = NULL;
	}

	BOOL Init(DuiNode* pCtrl)
	{
		ATLASSERT(pCtrl);
		m_pCtrl = pCtrl;
		DuiNode* rtc = (m_pCtrl);
		rtc->SetEventCallback(OnControlEvent);
		return m_pCtrl && m_peer.AddObject(rtc->_attributes.GetDispatch());
	}

	// 属性
protected:
	DuiNode* m_pCtrl;

	CComPtr<IDispatch> m_events[DUIET__LAST];

	CPeer m_peer;
	IUnknown* m_peerUnk; // 纯粹为了编译不出错

public:
	// IDuiControlSign
	virtual DuiNode* STDMETHODCALLTYPE GetControl() { return m_pCtrl; }

public:
	// IDispHost
//	virtual LPCOLESTR STDMETHODCALLTYPE GetClassName() { return L"DUICONTROL"; }
//	virtual LPVOID STDMETHODCALLTYPE GetOwner() { return m_pCtrl; }
//	virtual BOOL STDMETHODCALLTYPE GetOwnerAttribute(LPCOLESTR szName, OUT LPVOID pBuffer, LPCOLESTR szTypeName)
//	{
//		DuiNode* r = rt(m_pCtrl);
//		VARTYPE vt = VT_EMPTY;
//		DuiStyleVariantType svt = SVT_EMPTY;
//		size_t sz = 0;
//		switch ((DWORD_PTR)szTypeName) // 仅处理下列类型
//		{
//		case tn_byte: vt = VT_I1; svt = SVT_LONG; sz = 1; break;
//		case tn_word: vt = VT_I2; svt = SVT_LONG; sz = 2; break;
//		case tn_long: vt = VT_I4; svt = SVT_LONG; sz = sizeof(long); break;
//		case tn_float: vt = VT_R4; svt = SVT_DOUBLE; sz = sizeof(float); break;
//		case tn_float4d: vt = VT_ERROR; svt = SVT_COLOR; sz = sizeof(D3DXCOLOR); break;
//		}
//		if (vt==VT_EMPTY) return FALSE;
//		if (sz>0) ZeroMemory(pBuffer, sz);
//
//		// 首先处理STYLE属性
//		if (svt!=SVT_EMPTY)
//		{
//			if (szName==NULL || szName[0]==0) return FALSE;
//#define __StyleParser(name, classname, sat) \
//			else if (lstrcmpiW(szName, L#name) == 0) \
//			{ \
//				CDuiStyle* s = (CDuiStyle*)r->GetPrivateStyle(); \
//				if (s==NULL) return TRUE; \
//				CDuiStyleVariant* v = s->GetPrivateAttribute(SAT_##sat); \
//				if (v==NULL) return TRUE; \
//				if (v->svt != svt) return FALSE; \
//				if (svt==SVT_DOUBLE) *(float*)pBuffer = (float)v->doubleVal; \
//				else if (svt==SVT_COLOR) \
//				{ \
//					D3DXCOLOR* c = (D3DXCOLOR*)pBuffer; \
//					const float f = 1.f/255.f; \
//					c->a = f * ((FLOAT) (unsigned char) (v->colorVal >> 24)); \
//					c->r = f * ((FLOAT) GetRValue(v->colorVal)); \
//					c->g = f * ((FLOAT) GetGValue(v->colorVal)); \
//					c->b = f * ((FLOAT) GetBValue(v->colorVal)); \
//				} \
//				else memcpy_s(pBuffer, sz, &v->longVal, sz); \
//				return TRUE; \
//			}
//
//			__ForeachStyle(__StyleParser);
//#undef __StyleParser
//		}
//
//		// 处理普通属性
//		if (vt==VT_ERROR) return FALSE; // 不处理颜色
//		CComVariant v = r->GetAttribute(szName);
//
//		if (v.vt==VT_EMPTY || FAILED(v.ChangeType(vt))) return TRUE;
//		memcpy_s(pBuffer, sz, &v.lVal, sz);
//		return TRUE;
//	}
//	virtual void STDMETHODCALLTYPE SetOwnerAttribute(LPCOLESTR szName, LPVOID pValue, LPCOLESTR szTypeName)
//	{
//		DuiNode* r = rt(m_pCtrl);
//		CComVariant v;
//		CDuiStyleVariant sv;
//		switch ((DWORD_PTR)szTypeName)
//		{
//		case tn_byte: v = *(char*)pValue; sv=(LONG)*(char*)pValue; break;
//		case tn_word: v = *(short*)pValue; sv=(LONG)*(short*)pValue; break;
//		case tn_long: v = *(long*)pValue; sv=(LONG)*(long*)pValue; break;
//		case tn_float: v = *(float*)pValue; sv=(DOUBLE)*(float*)pValue; break;
//		case tn_float4d:
//			{
//				D3DXCOLOR* c = (D3DXCOLOR*)pValue;
//				COLORREF clr = RGB((BYTE)(c->r*255.f), (BYTE)(c->g*255.f), (BYTE)(c->b*255.f)) | ((DWORD)(c->a*255.f)<<24);
//				sv = clr;
//			}
//			break;
//		//default: return;
//		}
//
//		// 首先处理STYLE属性
//		if (sv.svt!=SVT_EMPTY)
//		{
//			if (szName==NULL || szName[0]==0) return;
//#define __StyleParser(name, classname, satname) \
//			else if (lstrcmpiW(szName, L#name) == 0) \
//			{ \
//				CDuiStyle* s = (CDuiStyle*)r->GetPrivateStyle(TRUE); \
//				CDuiStyleVariant* v = s->GetPrivateAttribute(SAT_##satname, TRUE); \
//				if (v==NULL) return; \
//				DuiStyleAttributeType sat = v->sat; \
//				StyleVariantClear(v); \
//				v->sat = sat; \
//				switch (sv.svt) \
//				{ \
//				case SVT_LONG: *v = FromStyleVariant(&sv, LONG); break; \
//				case SVT_DOUBLE: *v = FromStyleVariant(&sv, DOUBLE); break; \
//				case SVT_COLOR: *v = FromStyleVariant(&sv, COLORREF); break; \
//				} \
//				return; \
//			}
//
//			__ForeachStyle(__StyleParser);
//#undef __StyleParser
//		}
//
//		r->SetAttribute(szName, v);
//	}

public:
	// event callback
	static void CALLBACK OnControlEvent(HDE pCtrl, DuiEvent& event);
	// get object helper
	static COleDuiControl* FromControl(DuiNode* pCtrl, BOOL bAutoCreate=FALSE);

	BOOL SetEventCallback(LPCOLESTR lpszEventName, IDispatch* pCallback);
	HRESULT GetEventCallback(LPCOLESTR lpszEventName, IDispatch** ppCallback);
	void OnControlEvent(DuiEvent& event);

	//STDMETHOD(GetIDsOfNames)(REFIID riid, LPOLESTR* rgszNames, UINT cNames,
	//	LCID lcid, DISPID* rgdispid);
	//STDMETHOD(Invoke)(DISPID dispidMember, REFIID riid,
	//	LCID lcid, WORD wFlags, DISPPARAMS* pdispparams, VARIANT* pvarResult,
	//	EXCEPINFO* pexcepinfo, UINT* puArgErr);
	STDMETHOD(GetIDsOfNames)(REFIID riid, LPOLESTR* rgszNames, UINT cNames, LCID lcid, DISPID* rgdispid);
	STDMETHOD(Invoke)(DISPID dispidMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS* pdispparams, VARIANT* pvarResult, EXCEPINFO* pexcepinfo, UINT* puArgErr);

	// 专用处理事件属性的读取与设置
public:
#define _event_type(x) \
	STDMETHOD(get_on##x)(IDispatch** pVal) { return GetEventCallback(L"on" L#x, pVal); } \
	STDMETHOD(putref_on##x)(IDispatch* newVal) { SetEventCallback(L"on" L#x, newVal); return S_OK; }

	__foreach_event_type(_event_type)
#undef _event_type

public:

	STDMETHOD(get_tagName)(BSTR* pVal);
	STDMETHOD(get_ID)(BSTR* pVal);
	STDMETHOD(eventHandler)(VARIANT nameOrObject, VARIANT callback, IDispatch** handler);
	STDMETHOD(set)(BSTR name, VARIANT value);
	STDMETHOD(get)(BSTR name, VARIANT* val);
	STDMETHOD(setStyle)(BSTR stylename, BSTR stylevalue);
	STDMETHOD(get_parent)(IDispatch** pVal);
	STDMETHOD(get_text)(BSTR* pVal);
	STDMETHOD(put_text)(BSTR newVal);
	STDMETHOD(focus)(void);
	STDMETHOD(activate)(void);
	STDMETHOD(get_visible)(VARIANT_BOOL* pVal);
	STDMETHOD(put_visible)(VARIANT_BOOL newVal);
	STDMETHOD(get_enabled)(VARIANT_BOOL* pVal);
	STDMETHOD(put_enabled)(VARIANT_BOOL newVal);
	STDMETHOD(get_childCount)(LONG* pVal);
	STDMETHOD(getChild)(VARIANT index, IDispatch** child);
	STDMETHOD(get_filter)(IDispatch** pVal);
	STDMETHOD(attachObject)(IDispatch* extObject);
	STDMETHOD(detachObject)(IDispatch* extObject);
	STDMETHOD(appendChild)(IDispatch* newChild, IDispatch** node);
	STDMETHOD(insertBefore)(IDispatch* newChild, VARIANT refChild, IDispatch** node);
	STDMETHOD(put_innerXML)(BSTR newVal);
	STDMETHOD(put_outerXML)(BSTR newVal);
	STDMETHOD(remove)(BSTR name);
	STDMETHOD(get_children)(IDispatch** pVal);
};

OBJECT_ENTRY_NON_CREATEABLE_EX_AUTO(__uuidof(DuiControl), COleDuiControl)
