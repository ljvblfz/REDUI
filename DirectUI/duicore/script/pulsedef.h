#pragma once

#ifndef __PULSEDEF_H__
#define __PULSEDEF_H__

#ifndef __activscp_h__
#include <ActivScp.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef HRESULT PSRESULT;

#if (!defined( PULSEAPI) || defined(NODLL))
	#ifdef PULSEAPI
		#undef PULSEAPI
	#endif
	#define PULSEAPI __stdcall
#endif // PULSEAPI

typedef LPVOID psHANDLE;

// script API
typedef psHANDLE psHSCRIPT;
typedef IActiveScript* LPACTIVESCRIPT;
typedef IActiveScriptError* LPACTIVESCRIPTERROR;

typedef void (CALLBACK *fnScriptNotifyCallback)(LPVOID);

typedef void (CALLBACK *fnScriptTerminateCallback)(const VARIANT*, const EXCEPINFO*, LPVOID);
typedef void (CALLBACK *fnStateChangeCallback)(SCRIPTSTATE, LPVOID);
typedef void (CALLBACK *fnScriptErrorCallback)(LPACTIVESCRIPTERROR, LPVOID);
typedef fnScriptNotifyCallback fnEnterScriptCallback;
typedef fnScriptNotifyCallback fnLeaveScriptCallback;

VOID
PULSEAPI
psCloseHandle(
	IN psHSCRIPT h);

BOOL
PULSEAPI
psIsValidHandle(
	IN psHANDLE h);

psHSCRIPT
PULSEAPI
psCreateJScript(
	IN BOOL bDebugEnabled);

psHSCRIPT
PULSEAPI
psCreateVBScript(
	 IN BOOL bDebugEnabled);

PSRESULT
PULSEAPI
psScriptSetHostHwnd(
	IN psHSCRIPT h,
	IN HWND hwnd);

PSRESULT
PULSEAPI
psScriptAddNamedUnknown(
	IN psHSCRIPT h,
	IN LPCOLESTR lpszNamedItem,
	IN LPUNKNOWN lpUnk);

VOID
PULSEAPI
psScriptSetActiveNamedItem(
	IN psHANDLE h,
	IN LPCOLESTR szNamedItem);

LPCOLESTR
PULSEAPI
psScriptGetActiveNamedItem(
	IN psHANDLE h);

PSRESULT
PULSEAPI
psScriptGetActiveScript(
	IN psHSCRIPT h,
	OUT LPACTIVESCRIPT* ppAs);

PSRESULT
PULSEAPI
psScriptExecute(
	IN psHSCRIPT h,
	IN LPCOLESTR lpszCode,
	IN BOOL bNeedDebug);

PSRESULT
PULSEAPI
psScriptSyncExecute(
	IN psHSCRIPT h,
	IN LPCOLESTR lpszCode,
	IN BOOL bNeedDebug);

PSRESULT
PULSEAPI
psScriptCalcExpression(
	IN psHSCRIPT h,
	IN LPCOLESTR lpszExpression,
	OUT VARIANT* pvarResult,
	IN IDispatch* pDispThis, /* = NULL */
	IN BOOL bNeedDebug);

PSRESULT
PULSEAPI
psScriptParseProcedure(
	IN psHSCRIPT h,
	IN LPCOLESTR lpszCode,
	IN BOOL bIsExpression,
	OUT IDispatch** ppDisp,
	IN BOOL bNeedDebug);

PSRESULT
PULSEAPI
psScriptCallFunction(
	IN psHSCRIPT h,
	IN LPCOLESTR lpszFunc,
	IN DISPPARAMS* pdp,
	OUT VARIANT* pvarResult,
	IN BOOL bNeedDebug);

PSRESULT
PULSEAPI
psScriptGetScriptDispatch(
	IN psHSCRIPT h,
	IN LPCOLESTR lpszName,
	IN LPDISPATCH* ppDisp);

PSRESULT
PULSEAPI
psScriptSetCallback(
	IN psHSCRIPT h,
	IN fnScriptNotifyCallback pfOnScriptTerminate,
	IN fnScriptNotifyCallback pfOnStateChange,
	IN fnScriptNotifyCallback pfOnScriptError,
	IN fnScriptNotifyCallback pfOnEnterScript,
	IN fnScriptNotifyCallback pfOnLeaveScript,
	IN LPVOID lpParam);

#ifdef __cplusplus
}

//////////////////////////////////////////////////////////////////////////
// C++ wrapped class

#ifdef ATLASSERT
#define _assert ATLASSERT
#elif defined(ASSERT)
#define _assert ASSERT
#else
#define _assert __noop
#endif

class CScript
{
private:
	psHSCRIPT m_hScript;

public:
	CScript() : m_hScript(NULL) {}
	CScript(psHSCRIPT hScript) : m_hScript(hScript) {}

	operator psHSCRIPT() { return m_hScript; }

	BOOL IsValid()
	{ return psIsValidHandle(m_hScript); }
	void Close()
	{ _assert(IsValid()); psCloseHandle(m_hScript); m_hScript=NULL; }
	BOOL CreateJScript(BOOL bDebugEnabled=TRUE)
	{ _assert(m_hScript==NULL); m_hScript=psCreateJScript(bDebugEnabled); return IsValid(); }
	BOOL CreateVBScript(BOOL bDebugEnabled=TRUE)
	{ _assert(m_hScript==NULL); m_hScript=psCreateVBScript(bDebugEnabled); return IsValid(); }

	HRESULT SetHostHwnd(HWND hwnd)
	{ _assert(IsValid()); return psScriptSetHostHwnd(m_hScript, hwnd); }
	HRESULT AddNamedUnknown(LPCOLESTR lpszNamedItem, LPUNKNOWN lpUnk)
	{ _assert(IsValid()); return psScriptAddNamedUnknown(m_hScript, lpszNamedItem, lpUnk); }
	LPCOLESTR GetActiveNamedItem()
	{ _assert(IsValid()); return psScriptGetActiveNamedItem(m_hScript); }
	void SetActiveNamedItem(LPCOLESTR szNamedItem)
	{ _assert(IsValid()); psScriptSetActiveNamedItem(m_hScript, szNamedItem); }
	HRESULT SetCallback(fnScriptNotifyCallback pfOnScriptTerminate=NULL, fnScriptNotifyCallback pfOnStateChange=NULL, fnScriptNotifyCallback pfOnScriptError=NULL,
		fnScriptNotifyCallback pfOnEnterScript=NULL, fnScriptNotifyCallback pfOnLeaveScript=NULL, LPVOID lpParam=NULL)
	{ _assert(IsValid()); return psScriptSetCallback(m_hScript, pfOnScriptTerminate, pfOnStateChange, pfOnScriptError, pfOnEnterScript, pfOnLeaveScript, lpParam); }

	HRESULT GetActiveScript(LPACTIVESCRIPT* ppAs)
	{ _assert(IsValid()); return psScriptGetActiveScript(m_hScript, ppAs); }
	HRESULT GetScriptDispatch(LPCOLESTR lpszName, LPDISPATCH* ppDisp)
	{ _assert(IsValid()); return psScriptGetScriptDispatch(m_hScript, lpszName, ppDisp); }

	HRESULT Execute(LPCOLESTR lpszCode, BOOL bNeedDebug=TRUE)
	{ _assert(IsValid()); return psScriptExecute(m_hScript, lpszCode, bNeedDebug); }
	HRESULT SyncExecute(LPCOLESTR lpszCode, BOOL bNeedDebug=TRUE)
	{ _assert(IsValid()); return psScriptSyncExecute(m_hScript, lpszCode, bNeedDebug); }
	HRESULT ParseProcedure(LPCOLESTR lpszCode, IDispatch** ppDisp, BOOL bIsExpression=FALSE, BOOL bNeedDebug=TRUE)
	{ _assert(IsValid()); return psScriptParseProcedure(m_hScript, lpszCode, bIsExpression, ppDisp, bNeedDebug); }
	HRESULT CalcExpression(LPCOLESTR lpszExpression, VARIANT* pvarResult, IDispatch* pDispThis=NULL, BOOL bNeedDebug=TRUE)
	{ _assert(IsValid()); return psScriptCalcExpression(m_hScript, lpszExpression, pvarResult, pDispThis, bNeedDebug); }
	HRESULT CallFunction(LPCOLESTR lpszFunc, DISPPARAMS* pdp=NULL, VARIANT* pvarResult=NULL, BOOL bNeedDebug=TRUE)
	{ _assert(IsValid()); return psScriptCallFunction(m_hScript, lpszFunc, pdp, pvarResult, bNeedDebug); }

#ifdef __ATLCOMCLI_H__
	ATL::CComVariant CalcExpression(LPCOLESTR lpszExpression, IDispatch* pDispThis=NULL, BOOL bNeedDebug=TRUE)
	{
		ATL::CComVariant v;
		CalcExpression(lpszExpression, &v, pDispThis, bNeedDebug);
		return v;
	}

	ATL::CComVariant CallFunction(LPCOLESTR lpszFunc, ATL::CComVariant v1=ATL::CComVariant(), ATL::CComVariant v2=ATL::CComVariant(), 
					ATL::CComVariant v3=ATL::CComVariant(), ATL::CComVariant v4=ATL::CComVariant(), ATL::CComVariant v5=ATL::CComVariant())
	{
		VARIANTARG v[5];
		memcpy_s(&v[4], sizeof(VARIANTARG), &v1, sizeof(VARIANTARG));
		memcpy_s(&v[3], sizeof(VARIANTARG), &v2, sizeof(VARIANTARG));
		memcpy_s(&v[2], sizeof(VARIANTARG), &v3, sizeof(VARIANTARG));
		memcpy_s(&v[1], sizeof(VARIANTARG), &v4, sizeof(VARIANTARG));
		memcpy_s(&v[0], sizeof(VARIANTARG), &v5, sizeof(VARIANTARG));
		int nOptCnt = 0;
		if (v5.vt == VT_EMPTY) nOptCnt++;
		if (v4.vt == VT_EMPTY) nOptCnt++;
		if (v3.vt == VT_EMPTY) nOptCnt++;
		if (v2.vt == VT_EMPTY) nOptCnt++;
		if (v1.vt == VT_EMPTY) nOptCnt++;
		DISPPARAMS dp = {nOptCnt==5?NULL:&v[nOptCnt], NULL, 5-nOptCnt, 0};

		ATL::CComVariant vRet;
		CallFunction(lpszFunc, &dp, &vRet);
		return vRet;
	}
#endif // __ATLCOMCLI_H__
	
	HRESULT InvokeCallback(VARIANT* callback, IDispatch* pDispThis=NULL, VARIANT* pvarResult=NULL, BOOL bAllowCodeToDispatch=TRUE, BOOL bNeedDebug=TRUE)
	{
		_assert(IsValid());
		if (callback==NULL) return E_FAIL;
		CComVariant cb(*callback);
		if (cb.vt==VT_BSTR && cb.bstrVal)
		{
			CComPtr<IDispatch> disp;
			if (SUCCEEDED(ParseProcedure(cb.bstrVal, &disp)))
			{
				cb = disp;
				if (bAllowCodeToDispatch)
				{
					cb.Detach(callback);
					cb = disp;
				}
			}
		}
		HRESULT hr = E_FAIL;
		if (cb.vt==VT_DISPATCH && cb.pdispVal)
		{
			CComVariant vRet;
			if (pDispThis)
			{
				CComVariant vThis = pDispThis;
				DISPID didThis = DISPID_THIS;
				DISPPARAMS dp = {&vThis, &didThis, 1, 1};
				hr = cb.pdispVal->Invoke(DISPID_VALUE, IID_NULL, LOCALE_SYSTEM_DEFAULT, DISPATCH_METHOD, &dp, &vRet, NULL, NULL);
			}
			else
			{
				DISPPARAMS dp = {NULL, NULL, 0, 0};
				hr = cb.pdispVal->Invoke(DISPID_VALUE, IID_NULL, LOCALE_SYSTEM_DEFAULT, DISPATCH_METHOD, &dp, &vRet, NULL, NULL);
			}
			if (pvarResult) vRet.Detach(pvarResult);
		}
		return hr;
	}
};

#endif /* __cplusplus */

#endif // __PULSEDEF_H__