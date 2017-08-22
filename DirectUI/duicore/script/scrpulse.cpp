// scrpulse.cpp : 定义 DLL 应用程序的入口点。
//

#include "stdafx.h"
//#include <dispex.h>

#ifndef NEW
#define NEW new
#endif // NEW

#define PULSEAPI __stdcall
#include "pulsedef.h"

#include "scripthost.h"

#define __CALLTYPE __stdcall
//#define __CALLTYPE 
//#define __CALLTYPE __cdecl

//typedef HRESULT (__CALLTYPE *pfnInvokeEx)(IDispatchEx *, DISPID , LCID , WORD , DISPPARAMS *, VARIANT *, EXCEPINFO *, IServiceProvider *);
//HRESULT __CALLTYPE Hook_InvokeEx(IDispatchEx *This, DISPID id, LCID lcid, WORD wFlags, DISPPARAMS *pdp, VARIANT *pVarRes, EXCEPINFO *pei, IServiceProvider *pspCaller);
//
//struct DispStr
//{
//	LPDWORD lpUnkl;
//	DWORD dwUnk[2];
//	LPVOID This;
//	LPDWORD lpVtbl;
//};
//
//class CInvokeExHook
//{
//protected:
//public:
//	IDispatchEx* m_pDispEx;
//
//
//public:
//	PROC m_pfnOrg;
//	CInvokeExHook() : m_pDispEx(NULL), m_pfnOrg(NULL) {}
//	~CInvokeExHook() { Release(); }
//	void Release()
//	{
//		if (m_pDispEx)
//		{
//			m_pDispEx->Release();
//			m_pDispEx = NULL;
//		}
//		m_pfnOrg = NULL;
//	}
//
//	PROC* GetOrgAddr(IDispatchEx* pDispEx)
//	{
//		DispStr* pStr = (DispStr*)pDispEx;
//		LPDWORD lpVtabl = pStr->lpVtbl;
//
//		PROC* ppfn = (PROC*)(lpVtabl + 8);
//
//		return ppfn;
//	}
//
//
//	void hook()
//	{
//		PROC* ppfn = GetOrgAddr(m_pDispEx);
//		if(*ppfn != (PROC)Hook_InvokeEx)
//		{
//			m_pfnOrg = (PROC)(*ppfn);
//			PROC pfnNew = (PROC)Hook_InvokeEx;
//			WriteProcessMemory(GetCurrentProcess(), ppfn, &pfnNew, sizeof(PROC), NULL);
//		}
//	}
//
//	void unhook()
//	{
//		if(m_pDispEx != 0)
//		{
//			PROC* ppfn = GetOrgAddr(m_pDispEx);
//			WriteProcessMemory(GetCurrentProcess(), ppfn, &m_pfnOrg, sizeof(PROC), NULL);
//		}
//		m_pfnOrg = NULL;
//	}
//
//	void SetDispatchEx(IUnknown* pDispEx)
//	{
//		if (pDispEx==NULL) return;
//		Release();
//
//		pDispEx->QueryInterface(IID_IDispatchEx, (void**)&m_pDispEx);
//		if (m_pDispEx==NULL) return;
//
//		DISPID eid;
//		if (S_OK==m_pDispEx->GetNextDispID(fdexEnumAll, DISPID_STARTENUM, &eid))
//		{
//			do 
//			{
//				CComBSTR name;
//				m_pDispEx->GetMemberName(eid, &name);
//				ATLTRACE("dispid:0x%x, name:%S\n", eid, name);
//			} while(S_OK==m_pDispEx->GetNextDispID(fdexEnumAll, eid, &eid));
//		}
//
//		hook();
//	}
//};
//
//CInvokeExHook m_hook;
//
//HRESULT __CALLTYPE Hook_InvokeEx(IDispatchEx *This, DISPID id, LCID lcid, WORD wFlags, DISPPARAMS *pdp, VARIANT *pVarRes, EXCEPINFO *pei, IServiceProvider *pspCaller)
//{
//	DispStr* pStr = (DispStr*)(m_hook.m_pDispEx);
//	if(pStr->This != This)
//	{
//		ATLTRACE("Warning:m_pDispEx != this !!!\n"); // __cdecl
//
//		HRESULT hr = ((pfnInvokeEx )m_hook.m_pfnOrg)(This, id, lcid, wFlags, pdp, pVarRes, pei, pspCaller);
//
//		return hr;
//	}
//
//	if(m_hook.m_pDispEx != NULL /*&& id != 0*/)
//	{
//		CComBSTR name;
//		m_hook.m_pDispEx->GetMemberName(id, &name);
//		ATLTRACE(_T("dispid:0x%x(%S), flag:%s\n"), id, name,
//			wFlags==DISPATCH_METHOD?_T("METHOD"):(wFlags==DISPATCH_PROPERTYGET?_T("PROPERTYGET"):(wFlags==DISPATCH_PROPERTYPUT?_T("PROPERTYPUT"):(wFlags==DISPATCH_PROPERTYPUTREF?_T("PROPERTYPUTREF"):_T("CONSTRUCT"))))
//			);
//	}
//	HRESULT hr = ((pfnInvokeEx )m_hook.m_pfnOrg)(This, id, lcid, wFlags, pdp, pVarRes, pei, pspCaller);
//
//	return hr;
//}



#define ScriptHostPtr(h) ((CScriptHostImpl*)reinterpret_cast<CPulseBase*>(h))


extern "C" VOID PULSEAPI psCloseHandle(psHANDLE h)
{
	//m_hook.Release();
	//PulseBasePtr(h)->Delete();
	CPulseBase::DeleteThis(reinterpret_cast<CPulseBase*>(h));
}

extern "C" BOOL PULSEAPI psIsValidHandle(psHANDLE h)
{
	return CPulseBase::IsValid(reinterpret_cast<CPulseBase*>(h));
}

extern "C" psHSCRIPT PULSEAPI psCreateVBScript(BOOL bDebugEnabled)
{
	CScriptHostImpl* p = NEW CScriptHostImpl(bDebugEnabled);
	if (p==NULL)
		return NULL;

	HRESULT hr = p->CreateScriptEngine(L"VBScript");
	if (FAILED(hr))
	{
		CPulseBase::DeleteThis(p);
		return NULL;
	}

	return reinterpret_cast<psHSCRIPT>((CPulseBase*)p);
}

extern "C" psHSCRIPT PULSEAPI psCreateJScript(BOOL bDebugEnabled)
{
	CScriptHostImpl* p = NEW CScriptHostImpl(bDebugEnabled);
	if (p==NULL)
		return NULL;

	HRESULT hr = p->CreateScriptEngine(L"JScript");
	if (FAILED(hr))
	{
		CPulseBase::DeleteThis(p);
		return NULL;
	}

	//// hook InvokeEx
	//CComPtr<IDispatch> pDisp;
	//p->m_as->GetScriptDispatch(NULL, &pDisp);
	//m_hook.SetDispatchEx(pDisp);

	return reinterpret_cast<psHSCRIPT>((CPulseBase*)p);
}

extern "C" PSRESULT PULSEAPI psScriptSetHostHwnd(psHSCRIPT h, HWND hwnd)
{
	if (!CPulseBase::IsValid(reinterpret_cast<CPulseBase*>(h)))
		return E_INVALIDARG;

	ScriptHostPtr(h)->SetHWND(hwnd);
	return S_OK;
}

extern "C" PSRESULT PULSEAPI psScriptAddNamedUnknown(psHSCRIPT h, LPCOLESTR lpszNamedItem, LPUNKNOWN lpUnk)
{
	if (!CPulseBase::IsValid(reinterpret_cast<CPulseBase*>(h)) || lpszNamedItem==NULL || lpUnk==NULL)
		return E_INVALIDARG;

	return ScriptHostPtr(h)->AddUnknown(lpszNamedItem, lpUnk);
}

extern "C" VOID PULSEAPI psScriptSetActiveNamedItem(psHANDLE h, LPCOLESTR szNamedItem)
{
	if (!CPulseBase::IsValid(reinterpret_cast<CPulseBase*>(h)))
		return;

	ScriptHostPtr(h)->SetActiveNamedItem(szNamedItem);
}

extern "C" LPCOLESTR PULSEAPI psScriptGetActiveNamedItem(psHANDLE h)
{
	if (!CPulseBase::IsValid(reinterpret_cast<CPulseBase*>(h)))
		return NULL;

	return ScriptHostPtr(h)->GetActiveNamedItem();
}

extern "C" PSRESULT PULSEAPI psScriptGetActiveScript(psHSCRIPT h, LPACTIVESCRIPT* ppAs)
{
	if (!CPulseBase::IsValid(reinterpret_cast<CPulseBase*>(h)) || ppAs==NULL)
		return E_INVALIDARG;

	*ppAs = NULL;
	if (ScriptHostPtr(h)->m_as==NULL)
		return S_OK;
	return ScriptHostPtr(h)->m_as.QueryInterface(ppAs);
}

extern "C" PSRESULT PULSEAPI psScriptExecute(psHSCRIPT h, LPCOLESTR lpszCode, BOOL bNeedDebug)
{
	if (!CPulseBase::IsValid(reinterpret_cast<CPulseBase*>(h)) || lpszCode==NULL)
		return E_INVALIDARG;

	HRESULT hr = ScriptHostPtr(h)->ParseCode(lpszCode, bNeedDebug);
	if (SUCCEEDED(hr))
		hr = ScriptHostPtr(h)->m_as->SetScriptState(SCRIPTSTATE_CONNECTED);
	return hr;
}

extern "C" PSRESULT PULSEAPI psScriptSyncExecute(psHSCRIPT h, LPCOLESTR lpszCode, BOOL bNeedDebug)
{
	if (!CPulseBase::IsValid(reinterpret_cast<CPulseBase*>(h)) || lpszCode==NULL)
		return E_INVALIDARG;

	HRESULT hr = ScriptHostPtr(h)->ParseCode(lpszCode, bNeedDebug);
	if (SUCCEEDED(hr))
		hr = ScriptHostPtr(h)->m_as->SetScriptState(SCRIPTSTATE_CONNECTED);
	// wait for finished
	while (ScriptHostPtr(h)->m_ulWorking>0)
	{
		MSG msg;
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
	return hr;
}

extern "C" PSRESULT PULSEAPI psScriptParseProcedure(psHSCRIPT h, LPCOLESTR lpszCode, BOOL bIsExpression, IDispatch** ppDisp, BOOL bNeedDebug)
{
	if (!CPulseBase::IsValid(reinterpret_cast<CPulseBase*>(h)) || lpszCode==NULL)
		return E_INVALIDARG;

	DWORD dwFlag = SCRIPTPROC_IMPLICIT_THIS | SCRIPTPROC_IMPLICIT_PARENTS;
	if (bIsExpression) dwFlag |= SCRIPTPROC_ISEXPRESSION;
	HRESULT hr = ScriptHostPtr(h)->ParseProcedure(lpszCode, ppDisp, dwFlag, bNeedDebug);
	return hr;
}

extern "C" PSRESULT PULSEAPI psScriptCalcExpression(psHSCRIPT h, LPCOLESTR lpszExpression, VARIANT* pvarResult, IDispatch* pDispThis, BOOL bNeedDebug)
{
	IDispatch* disp = NULL;
	PSRESULT hr = psScriptParseProcedure(h, lpszExpression, TRUE, &disp, bNeedDebug);
	if (FAILED(hr) || disp==NULL) return hr;

	CComVariant vRet;
	if (pvarResult)
	{
		vRet.Attach(pvarResult);
		vRet.Clear();
	}

	DISPID didthis = DISPID_THIS;
	CComVariant vThis = pDispThis;
	DISPPARAMS dp = {pDispThis?&vThis:NULL, pDispThis?&didthis:NULL, pDispThis?1:0, pDispThis?1:0};
	hr = disp->Invoke(DISPID_VALUE, IID_NULL, LOCALE_CUSTOM_DEFAULT, DISPATCH_METHOD, &dp, &vRet, NULL, NULL);
	if (SUCCEEDED(hr) && pvarResult)
		vRet.Detach(pvarResult);
	disp->Release();
	return hr;
}

extern "C" PSRESULT PULSEAPI psScriptCallFunction(psHSCRIPT h, LPCOLESTR lpszFunc, DISPPARAMS* pdp, VARIANT* pvarResult, BOOL bNeedDebug)
{
	if (lpszFunc==NULL) return E_FAIL;

	ATL::CComVariant vFunc;
	PSRESULT hr = psScriptCalcExpression(h, lpszFunc, &vFunc, NULL, bNeedDebug);
	if (FAILED(hr)) return hr;
	if (vFunc.vt != VT_DISPATCH) return E_FAIL;

	ATL::CComVariant vRet;
	DISPPARAMS dp = {NULL, NULL, 0, 0};
	if (pdp==NULL) pdp = &dp;
	hr = vFunc.pdispVal->Invoke(DISPID_VALUE, IID_NULL, LOCALE_SYSTEM_DEFAULT, DISPATCH_METHOD, pdp, &vRet, NULL, NULL);
	if (FAILED(hr)) return hr;
	if (pvarResult)
		vRet.Detach(pvarResult);
	return S_OK;
}

extern "C" PSRESULT PULSEAPI psScriptGetScriptDispatch(psHSCRIPT h, LPCOLESTR lpszName, LPDISPATCH* ppDisp)
{
	if (!CPulseBase::IsValid(reinterpret_cast<CPulseBase*>(h)))
		return E_INVALIDARG;
	if (ppDisp==NULL)
		return E_POINTER;

	return ScriptHostPtr(h)->m_as->GetScriptDispatch(lpszName, ppDisp);
}

extern "C" PSRESULT PULSEAPI psScriptSetCallback(psHSCRIPT h, fnScriptNotifyCallback pfOnScriptTerminate, fnScriptNotifyCallback pfOnStateChange,
		fnScriptNotifyCallback pfOnScriptError, fnScriptNotifyCallback pfOnEnterScript, fnScriptNotifyCallback pfOnLeaveScript, LPVOID lpParam)
{
	if (!CPulseBase::IsValid(reinterpret_cast<CPulseBase*>(h)))
		return E_INVALIDARG;
	ScriptHostPtr(h)->pfn[sntOnScriptTerminate] = pfOnScriptTerminate;
	ScriptHostPtr(h)->pfn[sntOnStateChange] = pfOnStateChange;
	ScriptHostPtr(h)->pfn[sntOnScriptError] = pfOnScriptError;
	ScriptHostPtr(h)->pfn[sntOnEnterScript] = pfOnEnterScript;
	ScriptHostPtr(h)->pfn[sntOnLeaveScript] = pfOnLeaveScript;
	ScriptHostPtr(h)->lpParam = lpParam;
	return S_OK;
}
