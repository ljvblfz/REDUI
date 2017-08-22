// DuiEvent.h : COleDuiEvent 的声明

#pragma once
#include "resource.h"       // 主符号

#include "DirectUI_i.h"
#include "duicore/DirectUI.h"
#include "duicore/DuiManager.h"

#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Windows CE 平台(如不提供完全 DCOM 支持的 Windows Mobile 平台)上无法正确支持单线程 COM 对象。定义 _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA 可强制 ATL 支持创建单线程 COM 对象实现并允许使用其单线程 COM 对象实现。rgs 文件中的线程模型已被设置为“Free”，原因是该模型是非 DCOM Windows CE 平台支持的唯一线程模型。"
#endif



// COleDuiEvent

class ATL_NO_VTABLE COleDuiEvent :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<COleDuiEvent, &CLSID_DuiEventObj>,
	public IDispatchImpl<IDuiEvent, &IID_IDuiEvent, &LIBID_DirectUILib, /*wMajor =*/ 0xFFFF, /*wMinor =*/ 0xFFFF>
{
public:
	DuiEvent* m_event;
	DuiEvent m_innerEvent; // 当脚本创建一个事件时，使用这个事件

public:
	COleDuiEvent() : m_event(&m_innerEvent)
	{
		ZeroMemory(&m_innerEvent, sizeof(DuiEvent));
	}
	~COleDuiEvent()
	{
		if (HIWORD(m_innerEvent.Name)) delete[] m_innerEvent.Name;
		m_innerEvent.Name = NULL;
	}

DECLARE_REGISTRY_RESOURCEID(IDR_DUIEVENT)


BEGIN_COM_MAP(COleDuiEvent)
	COM_INTERFACE_ENTRY(IDuiEvent)
	COM_INTERFACE_ENTRY(IDispatch)
END_COM_MAP()



	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct()
	{
		return S_OK;
	}

	void FinalRelease()
	{
	}

	// 从外部事件结构初始化一个事件对象
	BOOL Init(DuiEvent* pEvent)
	{
		m_event = pEvent;
		return m_event!=NULL;
	}

	// 直接使用内部事件结构时从这里初始化
	BOOL Init(LPCOLESTR szEventName)
	{
		if (szEventName==NULL) return FALSE;

#define _event_type(x) \
		else if (lstrcmpiW(szEventName, L#x)==0 || lstrcmpiW(szEventName,L"on" L#x)==0) m_innerEvent.Name = (LPCOLESTR)DUIET_##x;

		__foreach_event_type(_event_type)
#undef _event_type

		else
		{
			m_innerEvent.Name = NEW OLECHAR[lstrlenW(szEventName)+1];
			if (m_innerEvent.Name)
				lstrcpyW((LPOLESTR)m_innerEvent.Name, szEventName);
		}

		return m_innerEvent.Name!=NULL;
	}

public:

	STDMETHOD(get_name)(BSTR* pVal);
	STDMETHOD(get_src)(IDispatch** pVal);
	STDMETHOD(get_x)(LONG* pVal);
	STDMETHOD(get_y)(LONG* pVal);
	STDMETHOD(get_cancelBubble)(VARIANT_BOOL* pVal);
	STDMETHOD(put_cancelBubble)(VARIANT_BOOL newVal);
	STDMETHOD(get_returnValue)(VARIANT_BOOL* pVal);
	STDMETHOD(put_returnValue)(VARIANT_BOOL newVal);
	STDMETHOD(get_propertyName)(BSTR* pVal);
};

OBJECT_ENTRY_NON_CREATEABLE_EX_AUTO(__uuidof(DuiEventObj), COleDuiEvent)
