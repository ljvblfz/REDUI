#pragma once

#ifndef __DISPATCHIMPL_H__
#define __DISPATCHIMPL_H__

#include <atlstr.h>

#ifndef NEW
	#define NEW new
#endif // NEW

#define SAFE_RELEASE(x) {if (x) x->Release(); x=NULL;}

template<class T, bool t_releaseSelf = false>
class CDispatchImpl : public IDispatch
{
	ULONG m_ulRef;
public:
	CDispatchImpl() : m_ulRef(0) {}

	STDMETHOD_(ULONG, AddRef)() { return ++m_ulRef; }
	STDMETHOD_(ULONG, Release)()
	{
		--m_ulRef;
		if (m_ulRef==0 && t_releaseSelf) delete static_cast<T*>(this);
		return m_ulRef;
	}
	STDMETHOD(QueryInterface)(REFIID iid, LPVOID* ppvObj)
	{
		if (!ppvObj)
			return E_POINTER;

		*ppvObj = NULL;
		if (IsEqualIID(iid, __uuidof(IUnknown)) ||
			IsEqualIID(iid, __uuidof(IDispatch)))
			*ppvObj = this;
		if (*ppvObj)
		{
			((LPUNKNOWN)(*ppvObj))->AddRef();
			return S_OK;
		}
		return E_NOINTERFACE;
	}
	template<class Q>
	HRESULT QueryInterface(Q** ppObj) { return QueryInterface(__uuidof(Q), (LPVOID*)ppObj); }
	STDMETHOD(GetTypeInfoCount)(UINT *pctinfo)
	{
		*pctinfo = 0;
		ATLTRACENOTIMPL(_T("CDispatchImpl::GetTypeInfoCount"));
	}
	STDMETHOD(GetTypeInfo)(UINT /*iTInfo*/, LCID /*lcid*/, ITypeInfo **ppTInfo)
	{
		*ppTInfo = NULL;
		ATLTRACENOTIMPL(_T("CDispatchImpl::GetTypeInfo"));
	}
	STDMETHOD(GetIDsOfNames)(REFIID riid, OLECHAR ** rgszNames, UINT cNames, LCID lcid, DISPID * rgDispId)
	{
		ATLTRACENOTIMPL(_T("CDispatchImpl::GetIDsOfNames"));
	}
	STDMETHOD(Invoke)(DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS * pdispparams, VARIANT *pVarResult, EXCEPINFO * pExcepInfo, UINT * puArgErr)
	{
		ATLASSERT(FALSE);
		return S_OK;
	}

	IDispatch* GetDispatch(bool bAddRef=true) { if(bAddRef)AddRef(); return (IDispatch*)this; }
	ULONG GetRefCount() { return m_ulRef; }
};

// 这是一个集合类，可通过索引或字符串ID作为下标来获取其中的成员
typedef HRESULT (CALLBACK* pfnOnAdd)(VARIANT* newVal, LPVOID data);
typedef HRESULT (CALLBACK* pfnOnUpdated)(long index, LPCOLESTR name, CComVariant& value, LPVOID data);
class CDispatchArray : public CDispatchImpl<CDispatchArray, true>
{
private:
	CDispatchArray(pfnOnAdd fnAdd, pfnOnUpdated fnUpdated, LPVOID data) : callback_add(fnAdd), callback_update(fnUpdated), callback_data(data) {} // 禁止直接构造
	struct __NamedDispItem
	{
		CComBSTR name;
		CComVariant value;
	};
	CSimpleArray<__NamedDispItem> items;
	pfnOnAdd callback_add; // 只有设置了此回调函数，集合中才能添加项 coll.Add(somevalue);
	pfnOnUpdated callback_update; // 只有设置了此回调函数，集合中的项才能被设置值 coll[i] = somevalue;
	LPVOID callback_data;

public:
	static CDispatchArray* New(IDispatch** ppDisp=NULL, pfnOnAdd fnAdd=NULL, pfnOnUpdated fnUpdated=NULL, LPVOID data=NULL)
	{
		CDispatchArray* p = NEW CDispatchArray(fnAdd, fnUpdated, data);
		if (p && ppDisp)
		{
			*ppDisp = p->GetDispatch(true);
		}
		return p;
	}
	static CDispatchArray* New(VARIANT* pVar, pfnOnAdd fnAdd=NULL, pfnOnUpdated fnUpdated=NULL, LPVOID data=NULL)
	{
		IDispatch** ppDisp = NULL;
		if (pVar)
		{
			pVar->vt = VT_DISPATCH;
			ppDisp = &pVar->pdispVal;
		}
		return New(ppDisp, fnAdd, fnUpdated, data);
	}

	template<typename T> void Add(T tVal, LPCOLESTR name=NULL)
	{
		__NamedDispItem item = {name, tVal};
		items.Add(item);
	}

	STDMETHOD(GetIDsOfNames)(REFIID riid, OLECHAR ** rgszNames, UINT cNames, LCID lcid, DISPID * rgDispId)
	{
		// Item DISPID == 0
		if (lstrcmpiW(*rgszNames, L"Item") == 0)
			return *rgDispId=0, S_OK;

		// length DISPID == 1
		if (lstrcmpiW(*rgszNames, L"length") == 0)
			return *rgDispId=1, S_OK;

		// Add DISPID == 2，仅当设置 callback_add 回调时此方法才有效
		if (lstrcmpiW(*rgszNames, L"Add") == 0 && callback_add)
			return *rgDispId=2, S_OK;

		// 先尝试名称是否十进制整数
		CComVariant vIdx = *rgszNames;
		if (/*vIdx.vt==VT_BSTR &&*/ SUCCEEDED(vIdx.ChangeType(VT_I4)) && vIdx.lVal>=0 && vIdx.lVal<items.GetSize())
			return *rgDispId=(DISPID)vIdx.lVal+10, S_OK;

		// 寻找名称
		for (int i=0, num=items.GetSize(); i<num; i++)
		{
			if (items[i].name.Length()>0 && items[i].name==*rgszNames)
				return *rgDispId=(DISPID)i+10, S_OK;
		}

		return *rgDispId=DISPID_UNKNOWN, DISP_E_UNKNOWNNAME;
	}
	STDMETHOD(Invoke)(DISPID dispid, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS * pdispparams, VARIANT *pVarResult, EXCEPINFO * pExcepInfo, UINT * puArgErr)
	{
		if (dispid<0 || dispid>=items.GetSize()+10 || (dispid>2 && dispid<10))
			return DISP_E_MEMBERNOTFOUND;

		HRESULT hr = S_OK;
		CComVariant vRet;
		switch (dispid)
		{
		case 0: // Item(idxOrName)
			if ((wFlags & DISPATCH_METHOD) && pdispparams->cArgs<=1)
			{
				int idx = 0;
				if (pdispparams->cArgs==1)
				{
					CComVariant v = pdispparams->rgvarg[0];
					if (SUCCEEDED(v.ChangeType(VT_I4))) idx = v.lVal;
					else if (v.vt == VT_BSTR)
					{
						idx = -1;
						for (int i=0, num=items.GetSize(); i<num; i++)
						{
							if (items[i].name.Length()>0 && items[i].name==v.bstrVal)
							{
								idx = i;
								break;
							}
						}
					}
				}
				if (idx>=0 && idx<items.GetSize())
					vRet = items[idx].value;
				else hr = E_INVALIDARG;
			}
			else if (wFlags==DISPATCH_PROPERTYGET)
			{
				if (items.GetSize()>0) vRet = items[0].value;
				else hr = E_UNEXPECTED;
			}
			else hr = E_UNEXPECTED;
			break;
		case 1: // length
			if (wFlags == DISPATCH_PROPERTYGET)
				vRet = (long)items.GetSize();
			else hr = E_UNEXPECTED;
			break;
		case 2: // Add(value)
			if (callback_add && (wFlags & DISPATCH_METHOD) && pdispparams->cArgs>=1)
			{
				hr = callback_add(pdispparams->rgvarg, callback_data);
			}
			break;
		default: // index
			hr = E_UNEXPECTED;
			if (dispid>=10 && dispid<items.GetSize()+10)
			{
				if (wFlags == DISPATCH_PROPERTYGET)
				{
					vRet = items[dispid-10].value;
					hr = S_OK;
				}
				else if ((wFlags==DISPATCH_PROPERTYPUT || wFlags==DISPATCH_PROPERTYPUTREF) && callback_update && pdispparams->cArgs==1)
				{
					items[dispid-10].value = pdispparams->rgvarg[0];
					hr = callback_update((long)dispid-10, items[dispid-10].name, items[dispid-10].value, callback_data);
				}
			}
		}

		if (SUCCEEDED(hr) && pVarResult) vRet.Detach(pVarResult);
		return hr;
	}
};

// 这是没有任何属性和方法调用的关闭者工具类，不要直接使用
template<class T>
class CDispatchCloserHelper : public CDispatchImpl<CDispatchCloserHelper<T>, true>
{
	T* _owner;
public:
	CDispatchCloserHelper(T* owner) : _owner(owner) { ATLASSERT(_owner); _owner->AddRef(); }
	~CDispatchCloserHelper() { _owner->__close(); _owner->Release(); }
};

// 这是有代理方法调用的关闭者工具类，不要直接使用
template<class T>
class CDispatchCloserWithProxyHelper : public CDispatchImpl<CDispatchCloserWithProxyHelper<T>, true>
{
	T* _owner;
public:
	CDispatchCloserWithProxyHelper(T* owner) : _owner(owner) { ATLASSERT(_owner); _owner->AddRef(); }
	~CDispatchCloserWithProxyHelper() { _owner->__close(); _owner->Release(); }

	STDMETHOD(GetTypeInfoCount)(UINT *pctinfo)
	{
		return _owner->GetTypeInfoCount(pctinfo);
	}
	STDMETHOD(GetTypeInfo)(UINT iTInfo, LCID lcid, ITypeInfo **ppTInfo)
	{
		return _owner->GetTypeInfo(iTInfo, lcid, ppTInfo);
	}
	STDMETHOD(GetIDsOfNames)(REFIID riid, OLECHAR ** rgszNames, UINT cNames, LCID lcid, DISPID * rgDispId)
	{
		return _owner->GetIDsOfNames(riid, rgszNames, cNames, lcid, rgDispId);
	}
	STDMETHOD(Invoke)(DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS * pdispparams, VARIANT *pVarResult, EXCEPINFO * pExcepInfo, UINT * puArgErr)
	{
		return _owner->Invoke(dispIdMember, riid, lcid, wFlags, pdispparams, pVarResult, pExcepInfo, puArgErr);
	}
};

// 关闭者类实现一个用于可预先关闭的代理组件，当代理组件被释放时，组件的__close方法被调用。
// 关闭者代理只产生一个实例，避免__close方法被多次调用
template<class T, class closerT = CDispatchCloserWithProxyHelper<T> >
class CDispatchCloserImpl
{
	closerT* _closer;
public:
	CDispatchCloserImpl() : _closer(NULL) {}
	// 派生类应该重载 __close()
	void __close() {}

	// 仅仅第一次调用GetCloser才能得到有效值
	HRESULT GetCloser(IDispatch** ppDisp)
	{
		if (ppDisp==NULL) return E_POINTER;
		*ppDisp = NULL;

		if (_closer==NULL)
		{
			_closer = NEW closerT((T*)this);
			return _closer->QueryInterface(IID_IDispatch, (LPVOID*)ppDisp);
		}

		return S_OK;
	}
};


// 代理类有自己独立的引用计数，但是IDispatch的相关调用都转移至属主。这个类不应该用NEW创建，而是定义成类成员或全局变量或静态变量。
template<bool tLock = true>
class CDispatchProxyT : public CDispatchImpl<CDispatchProxyT<tLock>, false>
{
	IDispatch* _owner;
public:
	CDispatchProxyT(IDispatch* owner) : _owner(owner) { ATLASSERT(owner); }
	~CDispatchProxyT() { if (_owner && tLock) _owner->Release(); _owner=NULL; }

	STDMETHOD_(ULONG, AddRef)()
	{
		ULONG lret = __super::AddRef();
		if (lret==1 && tLock) // 第一次添加引用才增加属主的引用计数
			_owner->AddRef();
		return lret;
	}
	STDMETHOD_(ULONG, Release)()
	{
		ULONG lret = __super::Release();
		if (lret==0 && tLock)
		{
			_owner->Release();
			_owner = NULL;
		}
		return lret;
	}
	STDMETHOD(GetTypeInfoCount)(UINT *pctinfo)
	{
		return _owner ? _owner->GetTypeInfoCount(pctinfo) : __super::GetTypeInfoCount(pctinfo);
	}
	STDMETHOD(GetTypeInfo)(UINT iTInfo, LCID lcid, ITypeInfo **ppTInfo)
	{
		return _owner ? _owner->GetTypeInfo(iTInfo, lcid, ppTInfo) : __super::GetTypeInfo(iTInfo, lcid, ppTInfo);
	}
	STDMETHOD(GetIDsOfNames)(REFIID riid, OLECHAR ** rgszNames, UINT cNames, LCID lcid, DISPID * rgDispId)
	{
		return _owner ? _owner->GetIDsOfNames(riid, rgszNames, cNames, lcid, rgDispId) : __super::GetIDsOfNames(riid, rgszNames, cNames, lcid, rgDispId);
	}
	STDMETHOD(Invoke)(DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS * pdispparams, VARIANT *pVarResult, EXCEPINFO * pExcepInfo, UINT * puArgErr)
	{
		return _owner ? _owner->Invoke(dispIdMember, riid, lcid, wFlags, pdispparams, pVarResult, pExcepInfo, puArgErr)
					: __super::Invoke(dispIdMember, riid, lcid, wFlags, pdispparams, pVarResult, pExcepInfo, puArgErr);
	}
};

typedef CDispatchProxyT<true> CDispatchProxy;
typedef CDispatchProxyT<false> CDispatchProxyNoLock;


#ifdef __dispex_h__

#ifndef DISPID_EXPANDO_BASE
#define DISPID_EXPANDO_BASE             3000000
#define DISPID_EXPANDO_MAX              3999999
#define IsExpandoDispid(dispid)         (DISPID_EXPANDO_BASE <= dispid && dispid <= DISPID_EXPANDO_MAX)
#endif // DISPID_EXPANDO_BASE

//////////////////////////////////////////////////////////////////////////
/*
 *	IDispatchEx 实现
 */

template <class T, const IID* piid = &__uuidof(T), const GUID* plibid = &CAtlModule::m_libid, WORD wMajor = 1,
WORD wMinor = 0, class tihclass = CComTypeInfoHolder>
class ATL_NO_VTABLE IDispatchExImpl : public ATL::IDispatchImpl<T, piid, plibid, wMajor, wMinor, tihclass>,
					public IDispatchEx
{
	typedef ATL::IDispatchImpl<T, piid, plibid, wMajor, wMinor, tihclass> baseDispatch;

public:

protected:
	struct Item
	{
		OLECHAR name[256];
		CComVariant var;
		BOOL used;

		Item(LPCOLESTR n, CComVariant& v=CComVariant(), BOOL u=TRUE)
		{
			lstrcpyW(name, n);
			var = v;
			used = u;
		}
	};
	CSimpleArray<Item> items;

public:
	// IDispatch
	STDMETHOD(GetTypeInfoCount)(UINT* pctinfo)
	{
		return baseDispatch::GetTypeInfoCount(pctinfo);
	}
	STDMETHOD(GetTypeInfo)(UINT itinfo, LCID lcid, ITypeInfo** pptinfo)
	{
		return baseDispatch::GetTypeInfo(itinfo, lcid, pptinfo);
	}
	STDMETHOD(GetIDsOfNames)(REFIID riid, OLECHAR ** rgszNames, UINT cNames, LCID lcid, DISPID * rgDispId)
	{
		HRESULT hr;
		//if (SUCCEEDED(hr=baseDispatch::GetIDsOfNames(riid, rgszNames, cNames, lcid, rgDispId)))
		//	return hr;

		//if (hr == DISP_E_UNKNOWNNAME)
		{
			hr = S_OK;
			for (UINT i=0; i<cNames; i++)
			{
				if (rgDispId[i]==DISPID_UNKNOWN && FAILED(GetDispID(rgszNames[i], fdexNameCaseSensitive, &rgDispId[i])))
					hr = DISP_E_UNKNOWNNAME;
			}
		}
		return hr;
	}
	STDMETHOD(Invoke)(DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS * pdispparams, VARIANT *pVarResult, EXCEPINFO * pExcepInfo, UINT * puArgErr)
	{
		//if (IsExpandoDispid(dispIdMember))
			return InvokeEx(dispIdMember, lcid, wFlags, pdispparams, pVarResult, pExcepInfo, NULL);
		//return baseDispatch::Invoke(dispIdMember, riid, lcid, wFlags, pdispparams, pVarResult, pExcepInfo, puArgErr);
	}

	// IDispatchEx
	STDMETHOD(DeleteMemberByDispID)(DISPID id)
	{
		if (IsExpandoDispid(id) && id<(DISPID)items.GetSize()+DISPID_EXPANDO_BASE)
		{
			Item& item = items[id-DISPID_EXPANDO_BASE];
			item.var.Clear();
			item.used = FALSE;
		}
		return S_OK;
	}
	STDMETHOD(DeleteMemberByName)(BSTR bstrName, DWORD grfdex)
	{
		if (bstrName==NULL)
			return E_POINTER;

		for (int i=0; i<items.GetSize(); i++)
		{
			Item& item = items[i];
			if (((grfdex&fdexNameCaseInsensitive) && lstrcmpiW(bstrName, item.name)==0) ||
				lstrcmpW(bstrName, item.name)==0)
			{
				item.var.Clear();
				item.used = FALSE;
				return S_OK;
			}
		}
		return S_OK;
	}
	STDMETHOD(GetDispID)(BSTR bstrName, DWORD grfdex, DISPID *pid)
	{
		if (pid == NULL || bstrName==NULL)
			return E_POINTER;
		*pid = DISPID_UNKNOWN;

		HRESULT hr;
		if (SUCCEEDED(hr=baseDispatch::GetIDsOfNames(IID_NULL, &bstrName, 1, LOCALE_SYSTEM_DEFAULT, pid)))
			return hr;

		for (int i=0; i<items.GetSize(); i++)
		{
			Item& item = items[i];
			if (((grfdex&fdexNameCaseInsensitive) && lstrcmpiW(bstrName, item.name)==0) ||
				lstrcmpW(bstrName, item.name)==0)
			{
				*pid = (DISPID)(i+DISPID_EXPANDO_BASE);
				return S_OK;
			}
		}

		// not found
		if ((grfdex&fdexNameEnsure)) // auto create
		{
			items.Add(Item(bstrName));
			*pid = (DISPID)(items.GetSize()+DISPID_EXPANDO_BASE);
			return S_OK;
		}

		return DISP_E_UNKNOWNNAME;
	}
	STDMETHOD(GetMemberName)(DISPID id, BSTR *pbstrName)
	{
		if (pbstrName == NULL)
			return E_POINTER;
		*pbstrName = NULL;
		if (id<DISPID_EXPANDO_BASE || id>=(DISPID)items.GetSize()+DISPID_EXPANDO_BASE)
			return DISP_E_UNKNOWNNAME;

		*pbstrName = ::SysAllocString(items[id-DISPID_EXPANDO_BASE].name);
		return S_OK;
	}
	STDMETHOD(GetMemberProperties)(DISPID id, DWORD grfdexFetch, DWORD *pgrfdex)
	{
		if (pgrfdex==NULL)
			return E_POINTER;
		*pgrfdex = 0;
		if (id<DISPID_EXPANDO_BASE || id>=(DISPID)items.GetSize()+DISPID_EXPANDO_BASE)
			return DISP_E_UNKNOWNNAME;

		if (grfdexFetch&grfdexPropCanAll)
			*pgrfdex |= (grfdexFetch&grfdexPropCanAll);
		if (grfdexFetch&grfdexPropExtraAll)
			*pgrfdex |= (grfdexFetch&grfdexPropExtraAll);

		return S_OK;
	}
	STDMETHOD(GetNameSpaceParent)(IUnknown **ppunk)
	{
		return E_NOTIMPL;
	}
	STDMETHOD(GetNextDispID)(DWORD grfdex, DISPID id, DISPID *pid)
	{
		if (pid == NULL)
			return E_POINTER;

		if (id == DISPID_STARTENUM)
		{
			*pid = DISPID_EXPANDO_BASE;
			return S_OK;
		}
		else if (id<DISPID_EXPANDO_BASE || id>=(DISPID)(items.GetSize()+DISPID_EXPANDO_BASE))
		{
			*pid = DISPID_UNKNOWN;
			return S_FALSE;
		}
		else
		{
			*pid = id + 1;
			return S_OK;
		}
	}
	STDMETHOD(InvokeEx)(DISPID id, LCID lcid, WORD wFlags, DISPPARAMS *pdp, VARIANT *pvarRes, EXCEPINFO *pei, IServiceProvider *pspCaller)
	{
		if (!IsExpandoDispid(id))
			return baseDispatch::Invoke(id, IID_NULL, lcid, wFlags, pdp, pvarRes, pei, NULL);

		if (id>=(DISPID)items.GetSize()+DISPID_EXPANDO_BASE)
			return DISP_E_MEMBERNOTFOUND;

		Item& item = items[id-DISPID_EXPANDO_BASE];
		if (item.used == FALSE)
			return DISP_E_MEMBERNOTFOUND;

		switch(wFlags)
		{
		case DISPATCH_METHOD:
			break;
		case DISPATCH_CONSTRUCT:
			return OnConstructCall(pdp, pvarRes);
			break;
		case DISPATCH_PROPERTYGET:
			if (pvarRes)
				return ::VariantCopy(pvarRes, &item.var);
			break;
		case DISPATCH_PROPERTYPUT:
			if (pdp->cArgs != 1)
				return E_INVALIDARG;
			return ::VariantCopy(&item.var, pdp->rgvarg);
			break;
		default:
			return E_INVALIDARG;
		}
		return S_OK;
	}

	// overidable
public:
	virtual HRESULT OnConstructCall(DISPPARAMS *pdp, VARIANT *pvarRes) { return S_OK; }
};
#endif // __dispex_h__


//////////////////////////////////////////////////////////////////////////
// CPeer 是一个扩展对象集合类，用于实现添加外部 IDispatch 对象并能被脚本访问，同时它也实现了 IPeerHolder 接口，用来处理 Attach 和 Detach 请求
//// {276887CB-4F19-468d-AF41-D03070C53E68}
//EXTERN_C const GUID DECLSPEC_SELECTANY IID_INamedItemHost = { 0x276887cb, 0x4f19, 0x468d, { 0xaf, 0x41, 0xd0, 0x30, 0x70, 0xc5, 0x3e, 0x68 } };
//
//MIDL_INTERFACE("{276887CB-4F19-468d-AF41-D03070C53E68")
//INamedItemHost : public IUnknown
struct INamedItemHost
{
	virtual BOOL GetNamedItem(LPCOLESTR name, VARIANT* pRet) = 0; // 如果有命名项，返回TRUE，否则返回FALSE
};

// {5F212D08-4B06-4765-A9DB-CD4189309721}
//DEFINE_GUID(IID_IPeerHolder, 0x5f212d08, 0x4b06, 0x4765, 0xa9, 0xdb, 0xcd, 0x41, 0x89, 0x30, 0x97, 0x21);
EXTERN_C const GUID DECLSPEC_SELECTANY IID_IPeerHolder = { 0x5f212d08, 0x4b06, 0x4765, { 0xa9, 0xdb, 0xcd, 0x41, 0x89, 0x30, 0x97, 0x21 } };

MIDL_INTERFACE("5f212d08-4b06-4765-a9db-cd4189309721")
IPeerHolder : public IUnknown
{
public:
	virtual HRESULT STDMETHODCALLTYPE AttachObject(/* [in] */ IDispatch* pDisp) = 0;
	virtual HRESULT STDMETHODCALLTYPE DetachObject(/* [in] */ IDispatch* pDisp) = 0;
	virtual HRESULT STDMETHODCALLTYPE SetNamedItemHost(/* [in] */ INamedItemHost* host) = 0;
};

EXTERN_C const GUID DECLSPEC_SELECTANY IID_IPeerHolder2 = { 0x5f212d08, 0x4b06, 0x4765, { 0xa9, 0xdb, 0xcd, 0x41, 0x89, 0x30, 0x97, 0x22 } };

MIDL_INTERFACE("5f212d08-4b06-4765-a9db-cd4189309722")
IPeerHolder2 : public IPeerHolder
{
public:
	virtual HRESULT STDMETHODCALLTYPE AttachObject2(/* [in] */ IDispatch* pDisp, LPCOLESTR szTypeName) = 0;
	virtual HRESULT STDMETHODCALLTYPE FindObject(/* [in] */ LPCOLESTR szTypeName, IDispatch** ppDisp) = 0;
};


#define DISPID_PEER_BASE 5000000
#define DISPID_PEER_NEXT_STEP 10000

class CPeer : public IPeerHolder2
{
	DISPID _base_dispid;	// 当前对象的DISPID基数
	CSimpleArray<DISPID> _real_dispids;		// 缓存的外部对象实际的DISPID的集合
	IDispatch* _disp;		// 外部对象
	LPCOLESTR _typename;	// 外部对象的类型名称
	INamedItemHost* _named_item_host;	// 命名项宿主
	CSimpleArray<CAtlStringW> _named_items; // 命名项缓存
	CPeer* _next_peer;		// 指向下一个外部对象

public:
	CPeer(IDispatch* disp, LPCOLESTR name=NULL, DISPID dispid_base = DISPID_PEER_BASE) : _disp(disp), _typename(name), _base_dispid(dispid_base), _named_item_host(NULL), _next_peer(NULL)
	{
		// 是否需要直接填写DISPID缓冲？
		if (_disp)
			_disp->AddRef();
	}

	~CPeer()
	{
		release_object();
		if (_next_peer) delete _next_peer;
	}

	//IUnknown* GetUnknown() { return this; }

	// 添加一个对象
	BOOL AddObject(IDispatch* disp, LPCOLESTR name=NULL)
	{
		if (disp == NULL) return FALSE;
		CPeer* peer = find_object(NULL);
		if (peer)
			peer->set_object(disp, name);
		else
			append_object(disp, name);
		return TRUE;
	}

	// 删除一个对象，链表位置继续保留，仅仅把对象指针和缓存清空
	BOOL RemoveObject(IDispatch* disp)
	{
		CPeer* peer = find_object(disp);
		if (peer) peer->release_object();
		return peer!=NULL;
	}

	// IUnknown methods，可以被聚合查询
	STDMETHOD_(ULONG, AddRef)() { /*ATLASSERT(FALSE);*/ return 1; }
	STDMETHOD_(ULONG, Release)() { /*ATLASSERT(FALSE);*/ return 1; }
	STDMETHOD(QueryInterface)(REFIID iid, LPVOID* ppvObj)
	{
		if (ppvObj == NULL) return E_POINTER;

		// 此类应该被聚合，所以不提供 IID_IUnknown 的查询
		if (iid==IID_IUnknown) return E_NOINTERFACE;
		if (iid==IID_IPeerHolder || iid==IID_IPeerHolder2)
			return *ppvObj=(IUnknown*)(IPeerHolder2*)this, S_OK;

		HRESULT hr = E_NOINTERFACE;
		if (_disp && SUCCEEDED(hr=_disp->QueryInterface(iid, ppvObj)))
			return hr;
		if (_next_peer)
			hr = _next_peer->QueryInterface(iid, ppvObj);
		return hr;
	}

	// IPeerHolder 方法
	virtual HRESULT STDMETHODCALLTYPE AttachObject(/* [in] */ IDispatch* pDisp) { return AddObject(pDisp), S_OK; }
	virtual HRESULT STDMETHODCALLTYPE DetachObject(/* [in] */ IDispatch* pDisp) { return RemoveObject(pDisp), S_OK; }
	virtual HRESULT STDMETHODCALLTYPE SetNamedItemHost(/* [in] */ INamedItemHost* host) { return _named_items.RemoveAll(), _named_item_host=host, S_OK; }
	// IPeerHolder2 方法
	virtual HRESULT STDMETHODCALLTYPE AttachObject2(/* [in] */ IDispatch* pDisp, LPCOLESTR szTypeName) { return AddObject(pDisp,szTypeName), S_OK; }
	virtual HRESULT STDMETHODCALLTYPE FindObject(/* [in] */ LPCOLESTR szTypeName, IDispatch** ppDisp)
	{
		if (szTypeName==NULL) return E_INVALIDARG;
		//if (ppDisp) *ppDisp = NULL;

		if (szTypeName==_typename)
		{
			if (ppDisp)
			{
				*ppDisp = _disp;
				if (*ppDisp) AddRef();
			}
			return S_OK;
		}
		if (_next_peer)
			return _next_peer->FindObject(szTypeName, ppDisp);
		return E_NOINTERFACE;
	}

	// 管理 DISPID 映射，获取Peer对象的ID
	HRESULT PeerGetDispId(LPCOLESTR lpszName, DISPID* pdispid)
	{
		if (pdispid == NULL) return E_POINTER;
		*pdispid = DISPID_UNKNOWN;

		HRESULT hr = DISP_E_UNKNOWNNAME;
		if (_disp)
		{
			DISPID did = DISPID_UNKNOWN;
#ifdef __dispex_h__
			CComQIPtr<IDispatchEx> dispex = _disp;
			if (dispex)
				hr = dispex->GetDispID((BSTR)lpszName, fdexNameCaseSensitive, &did);
#endif // __dispex_h__

			if (FAILED(hr))
				hr = _disp->GetIDsOfNames(IID_NULL, (LPOLESTR*)&lpszName, 1, LOCALE_SYSTEM_DEFAULT, &did);

			if (SUCCEEDED(hr))
			{
				int idx = _real_dispids.Find(did);
				if (idx == -1)
					idx = (_real_dispids.Add(did), _real_dispids.GetSize()-1);
				return *pdispid=_base_dispid+idx, hr;
			}
		}

		if (_next_peer)
			hr = _next_peer->PeerGetDispId(lpszName, pdispid);

		if ((hr==DISP_E_UNKNOWNNAME || hr==DISP_E_MEMBERNOTFOUND) && _named_item_host)
		{
			CComVariant v;
			int i = _named_items.Find(lpszName);
			if (i>=0)
			{
				*pdispid = _base_dispid - i - 1;
				return S_OK;
			}
			else if (_named_item_host->GetNamedItem(lpszName, &v) && _named_items.Add(lpszName))
			{
				*pdispid = _base_dispid - _named_items.GetSize();
				return S_OK;
			}
		}
		return hr;
	}

	// 委托Peer对象调用
	HRESULT PeerInvoke(DISPID dispId, LCID lcid, WORD wFlags, DISPPARAMS * pdispparams, VARIANT *pVarResult, EXCEPINFO * pExcepInfo, UINT * puArgErr)
	{
		HRESULT hr = DISP_E_MEMBERNOTFOUND;
		if (_named_item_host && dispId>=_base_dispid-_named_items.GetSize() && dispId<_base_dispid && wFlags==DISPATCH_PROPERTYGET)
		{
			if (_named_item_host->GetNamedItem(_named_items[_base_dispid-dispId-1], pVarResult))
				return S_OK;
		}
		if (dispId>=_base_dispid && dispId<_base_dispid+DISPID_PEER_NEXT_STEP)
		{
			if (_disp && dispId<_base_dispid+_real_dispids.GetSize())
			{
#ifdef __dispex_h__
				CComQIPtr<IDispatchEx> dispex = _disp;
#endif // __dispex_h__
				hr =
#ifdef __dispex_h__
					(dispex.p) ? dispex.p->InvokeEx(_real_dispids[dispId-_base_dispid], lcid, wFlags, pdispparams, pVarResult, pExcepInfo, NULL) :
#endif // __dispex_h__
					_disp->Invoke(_real_dispids[dispId-_base_dispid], IID_NULL, lcid, wFlags, pdispparams, pVarResult, pExcepInfo, puArgErr);
			}
			return hr;
		}
		if (_next_peer)
			return _next_peer->PeerInvoke(dispId, lcid, wFlags, pdispparams, pVarResult, pExcepInfo, puArgErr);
		return hr;
	}

	// 判断 DISPID 是不是Peer对象的合法ID
	BOOL IsPeerDispId(DISPID id)
	{
		if (id>=_base_dispid-_named_items.GetSize() && id<_base_dispid+_real_dispids.GetSize() && _disp)
			return TRUE;
		return _next_peer && _next_peer->IsPeerDispId(id);
	}

private:
	void set_object(IDispatch* disp, LPCOLESTR name=NULL) { release_object(); _disp=disp; _typename=name; if (_disp) _disp->AddRef(); }
	void set_next(CPeer* peer) { ATLASSERT(_next_peer==NULL); _next_peer = peer; }
	DISPID get_base_dispid() { return _base_dispid; }
	CPeer* next() { return _next_peer; }
	CPeer* last()
	{
		CPeer* peer = this;
		while (peer->next()) peer = peer->next();
		return peer;
	}
	CPeer* find_object(IDispatch* disp)
	{
		for (CPeer* peer=this; peer; peer=peer->next())
			if (peer->_disp == disp)
				return peer;
		return NULL;
	}
	void release_object()
	{
		_named_item_host = NULL;
		_named_items.RemoveAll();
		SAFE_RELEASE(_disp);
		_real_dispids.RemoveAll();
	}
	void append_object(IDispatch* disp, LPCOLESTR name=NULL)
	{
		CPeer* lastpeer = last();
		CPeer* newpeer = NEW CPeer(disp, name, lastpeer->get_base_dispid() + DISPID_PEER_NEXT_STEP);
		lastpeer->set_next(newpeer);
	}
};

#if 0
//////////////////////////////////////////////////////////////////////////
// DispHelper 用于为一个普通的类创建简单的 IDispatch 对象，使用的是 CreateDispTypeInfo 和 CreateStdDispatch
// 工作原理是添加属性和方法，构造一个接口表，交给两个API完成 IDispatch 的创建。
/*
 *	有下列注意事项：
	1、一个需要实现作 IDispatch 的类，不要从其它类派生。（派生的情况尚未测试）
	2、类通过如下的宏定义添加属性和方法
		Begin_Dispatch_Map(CSomeClass)
			Disp_PropertyGet(abc, long, 1)		// 添加一个只读属性 abc，long类型，DISPID是1
			Disp_PropertyPut(hello, long, 2)	// 添加一个只写属性 hello，long类型，DISPID是2
			Disp_Property(world, long, 3)		// 添加一个读写属性 world，long类型，DISPID是3
			Disp_Method(print, void, 4, DispFuncParams0) () { // do something }		// 添加一个 print 方法，void返回类型，DISPID是4，没有参数
			Disp_Method(foo, long, 5, DispFuncParams1<float>) (float f) { return (long)5; }		// 添加一个 foo 方法，long返回类型，DISPID是5，一个参数（float类型）
		End_Dispatch_Map()
	3、上述宏映射必须放在类申明的最开始，也就是说所有其它自定义成员函数（包括构造和析构函数）都应该放到后面
	4、这个宏里实现了一个 IDispatch* DispatchPtr() 函数，用来获取生成的 IDispatch 对象
	5、所有参数类型（比如long、float等等）必须是 CVarTypeInfo<> 模板类支持的类型，因为它能提供数据类型到 VARTYPE 的转换。
		如果需要不支持的类型，必须手工添加新的 CVarTypeInfo<> 特化模板类，例如可以定义 CString CComBSTR std::string等类型
	6、方法的参数个数只支持3个，通过 DispFuncParamsN<> 模板类提供，如果超过此数，需要自己添加定义，例如 DispFuncParams8<t1,...,t8>
	7、需要包含 atlcomcli.h 头文件

	简单的类示例：
	#include <atlcomcli.h>
	#include "<thisfile>.h"

	class CTest
	{
	public:
		Begin_Dispatch_Map(CTest)
			Disp_Property(abc, long, 1)
			Disp_Method(print, void, 2, DispFuncParams0) ()
			{  ATLTRACE(_T("result: %d\n"), abc); }
		End_Dispatch_Map()

		CTest()
		{
			abc = 3;
			CComPtr<IDispatch> disp = DispatchPtr();
			disp.Invoke0(L"print");				// 调用 print 方法，将打印 result: 3
			CComVariant v = (long)123;
			disp.PutPropertyByName(L"abc", &v);	// 设置 abc 属性值为 123
			disp.Invoke0(L"print");				// 调用 print 方法，将打印 result: 123
		}
	}

 */
#define Max_Methods_Count 256	// 接口方法的最大个数

// 扩充 CVarTypeInfo 模板类的定义
template<>
class CVarTypeInfo< void >
{
public:
	static const VARTYPE VT = VT_EMPTY;
	//static char VARIANT::* const pmField;
};

// 函数参数模板类定义，如果需要也需扩充
struct DispFuncParams0
{
	static PARAMDATA* GetParamData(UINT* num = NULL) { if (num) *num=0; return NULL; }
};

template<typename T1>
struct DispFuncParams1
{
	static PARAMDATA* GetParamData(UINT* num = NULL)
	{
		static PARAMDATA _pd[] = {{NULL, CVarTypeInfo<T1>::VT}};
		if (num) *num = 1;
		return _pd;
	}
};

template<typename T1, typename T2>
struct DispFuncParams2
{
	static PARAMDATA* GetParamData(UINT* num = NULL)
	{
		static PARAMDATA _pd[] = {{NULL, CVarTypeInfo<T1>::VT}, {NULL, CVarTypeInfo<T2>::VT}};
		if (num) *num = 2;
		return _pd;
	}
};

template<typename T1, typename T2, typename T3>
struct DispFuncParams3
{
	static PARAMDATA* GetParamData(UINT* num = NULL)
	{
		static PARAMDATA _pd[] = {{NULL, CVarTypeInfo<T1>::VT}, {NULL, CVarTypeInfo<T2>::VT}, {NULL, CVarTypeInfo<T3>::VT}};
		if (num) *num = 3;
		return _pd;
	}
};

// 工具基类，提供基本位置计算
class DispHelperBase
{
public:
	virtual ULONG count() = 0; // 方法数
	virtual LPVOID data_ptr() = 0; // 数据成员的地址
	virtual LPVOID next() = 0; // 下一个数据成员的地址
	virtual LPCOLESTR get_name() {return NULL;} // 成员名称
	virtual ULONG fill_methoddata(METHODDATA* pmd) = 0; // 填充1或2个方法数据的成员

	enum offset_type
	{
		offset_get	= 0,
		offset_remove,
		offset_next,
		offset_prev,
	};

	static DispHelperBase* offset(offset_type otype, LPVOID origThis, DispHelperBase* offsetThis=NULL)
	{
		static CSimpleMap<LPVOID, DispHelperBase*> _map;

		switch (otype)
		{
		case offset_remove: return _map.Remove(origThis), NULL; // 第三个参数无效
		case offset_get:
			{
				int i = _map.FindKey(origThis);
				if (i >= 0) return _map.GetValueAt(i);
				if (offsetThis) _map.Add(origThis, offsetThis);
				return offsetThis;
			}
			break;
		case offset_prev: // 第二个参数无效
			{
				if (offsetThis->data_ptr() == NULL) return NULL;
				int i = _map.FindVal(offsetThis);
				if (i == -1) return NULL;
				origThis = offsetThis->data_ptr();
				for (i=0; i<_map.GetSize(); i++)
				{
					if (_map.GetValueAt(i)->next() == origThis)
						return _map.GetValueAt(i);
				}
			}
			break;
		case offset_next: // 第二个参数无效
			{
				int i = _map.FindVal(offsetThis);
				if (i == -1) return NULL;
				origThis = _map.GetValueAt(i)->next();
				if (origThis==NULL) return NULL;
				for (i=0; i<_map.GetSize(); i++)
				{
					if (_map.GetValueAt(i)->data_ptr() == origThis)
						return _map.GetValueAt(i);
				}
			}
			break;
		}
		return NULL;
	}
};

// 属性模板类，每个属性都会从它派生一个类
template<class T, typename tType, DISPID dispid, bool tGet, bool tPut>
class DispPropertyHelper : public DispHelperBase
{
public:
	DispPropertyHelper() { ATLASSERT(tGet||tPut); offset(offset_get, (T*)this, this); }
	~DispPropertyHelper() { offset(offset_remove, (T*)this); }

	ULONG count() { return (tGet&&tPut) ? 2 : 1; }
	LPVOID data_ptr() { return reinterpret_cast<tType*>((T*)this)-1; }
	LPVOID next() { return (T*)this + 1; }
	ULONG fill_methoddata(METHODDATA* pmd)
	{
		if (tGet)
		{
			pmd->szName = (LPOLESTR)get_name();
			pmd->ppdata = NULL;
			pmd->dispid = dispid;
			pmd->cArgs = 0;
			pmd->wFlags = DISPATCH_PROPERTYGET;
			pmd->vtReturn = CVarTypeInfo<tType>::VT;
			pmd++;
		}
		if (tPut)
		{
			pmd->szName = (LPOLESTR)get_name();
			pmd->ppdata = DispFuncParams1<tType>::GetParamData();
			pmd->dispid = dispid;
			pmd->cArgs = 1;
			pmd->wFlags = DISPATCH_PROPERTYPUT;
			pmd->vtReturn = CVarTypeInfo<tType>::VT;
			if (pmd->vtReturn == VT_DISPATCH) pmd->wFlags = DISPATCH_PROPERTYPUTREF;
		}
		return (tGet&&tPut) ? 2 : 1;
	}
};

// 方法模板类，每个方法都会从它派生一个类
template<class T, DISPID dispid, typename tReturn, class tFuncParams>
class DispMethodHelper : public DispHelperBase
{
public:
	DispMethodHelper() { offset(offset_get, (T*)this, this); }
	~DispMethodHelper() { offset(offset_remove, (T*)this, this); }

	ULONG count() { return 1; }
	LPVOID data_ptr() { return (T*)this; }
	LPVOID next() { return (T*)this + 1; }
	ULONG fill_methoddata(METHODDATA* pmd)
	{
		pmd->szName = (LPOLESTR)get_name();
		pmd->ppdata = tFuncParams::GetParamData(&pmd->cArgs);
		pmd->dispid = dispid;
		pmd->wFlags = DISPATCH_METHOD;
		pmd->vtReturn = CVarTypeInfo<tReturn>::VT;
		return 1;
	}
};

// 边界控制类，用来设置开始成员和结束成员
template<bool tBegin>
class DispBoundHelper : public DispHelperBase
{
public:
	DispBoundHelper() { offset(offset_get, this, this); }
	~DispBoundHelper() { offset(offset_remove, this); }

	ULONG count() { return 0; }
	LPVOID data_ptr() { return tBegin ? NULL : this; }
	LPVOID next() { return tBegin ? this + 1 : NULL; }
	ULONG fill_methoddata(METHODDATA* pmd) { return 0; }
};

// 宏定义
#define Begin_Dispatch_Map(classname) \
public: \
	DispBoundHelper<true> _map__begin; \

#define Disp_PropertyGet(name, type, dispid) \
	type name; struct __##name##__helper : public DispPropertyHelper<__##name##__helper, type, dispid, true, false> {LPCOLESTR get_name(){return OLESTR(#name);}} name##__helper; \
	STDMETHOD_(type, get_##name) () { return name; } \

#define Disp_PropertyPut(name, type, dispid) \
	type name; struct __##name##__helper : public DispPropertyHelper<__##name##__helper, type, dispid, false, true> {LPCOLESTR get_name(){return OLESTR(#name);}} name##__helper; \
	STDMETHOD_(void, put_##name) (type _##name) { name = _##name; } \

#define Disp_Property(name, type, dispid) \
	type name; struct __##name##__helper : public DispPropertyHelper<__##name##__helper, type, dispid, true, true> {LPCOLESTR get_name(){return OLESTR(#name);}} name##__helper; \
	STDMETHOD_(type, get_##name) () { return name; } \
	STDMETHOD_(void, put_##name) (type _##name) { name = _##name; } \

#define Disp_Method(name, type, dispid, params) \
	struct __##name##__helper : public DispMethodHelper<__##name##__helper, dispid, type, params> {LPCOLESTR get_name(){return OLESTR(#name);}} name##__helper; \
	STDMETHOD_(type, name)

#define End_Dispatch_Map() \
	DispBoundHelper<false> _map__end; \
	CComPtr<IDispatch> __spDisp; \
	IDispatch* DispatchPtr() \
	{ \
		if (__spDisp.p == NULL) \
		{ \
			static METHODDATA _mdata[Max_Methods_Count] = {0}; \
			static UINT _num = 0; \
			static INTERFACEDATA _id = {_mdata, _num}; \
			if (_num == 0) \
			{ \
				DispHelperBase* p = DispHelperBase::offset(DispHelperBase::offset_next, NULL, &_map__begin); \
				while (p && p->next()) \
				{ \
					_num += p->fill_methoddata(&_mdata[_num]); \
					p = DispHelperBase::offset(DispHelperBase::offset_next, NULL, p); \
				} \
				for (UINT i=0; i<_num; i++) {_mdata[i].iMeth=i; _mdata[i].cc=CC_STDCALL;} \
				_id.cMembers = _num; \
			} \
			CComPtr<ITypeInfo> ti; \
			CComPtr<IUnknown> unk; \
			HRESULT hr = ::CreateDispTypeInfo(&_id, LOCALE_SYSTEM_DEFAULT, &ti); \
			if (SUCCEEDED(hr)) hr = ::CreateStdDispatch(NULL, this, ti.p, &unk.p); \
			if (SUCCEEDED(hr)) hr = unk->QueryInterface(IID_IDispatch, (void**)&__spDisp.p); \
		} \
		return __spDisp.p; \
	} \

#endif
// End of DispHelper
//////////////////////////////////////////////////////////////////////////

#endif // __DISPATCHIMPL_H__