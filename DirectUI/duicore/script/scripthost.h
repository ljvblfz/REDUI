//////////////////////////////////////////////////////////////////////////
//
// usage:
// CScriptHostImpl sh(lpUnk, L"someobject");
// // sh.SetHWND(hwnd);
// // sh.SetUnknown(lpUnk, L"someobject");
// sh.CreateScriptEngine();
// sh.ParseCode(L"xxxxx", L"someobject");
//
// sh.m_as->SetScriptState(SCRIPTSTATE_CONNECTED);
// // sh.m_as->SetScriptState(SCRIPTSTATE_DISCONNECTED);
//
//////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef __SCRIPTHOST_H__
#define __SCRIPTHOST_H__

#ifndef __cplusplus
#error This requires C++ compilation (use a .cpp suffix)
#endif

#ifndef __activscp_h__
#include <ActivScp.h>
#endif

#include "ActivDbg.h"

#include "psbase.h"

typedef enum tagScriptNotifyType
{
	sntOnScriptTerminate = 0,
	sntOnStateChange,
	sntOnScriptError,
	sntOnEnterScript,
	sntOnLeaveScript
} ScriptNotifyType;

//template<>
class CScriptHostImpl : public IActiveScriptSite,
	public IActiveScriptSiteWindow,
	public IActiveScriptSiteDebug,
	public CPulseBase
{
public:
	CScriptHostImpl(BOOL bDebugEnabled) : /*m_pUnk(NULL),*/ m_hwnd(NULL), /*m_as(NULL), m_asp(NULL),*/ lpParam(NULL), m_ulWorking(0)
		, CPulseBase((CPulseBase*)this), m_bDebugEnabled(bDebugEnabled)
		, m_pNamedItem(NULL)
	{
		memset(pfn, 0, sizeof(pfn));

		if (m_bDebugEnabled)
		{
			HRESULT hr = m_pdm.CoCreateInstance(CLSID_ProcessDebugManager, NULL, CLSCTX_INPROC_SERVER | CLSCTX_LOCAL_SERVER);
			if (m_pdm)
			{
				hr = m_pdm->CreateApplication(&m_da);
				if (m_da)
				{
					hr = m_da->SetName(OLESTR("Yuhan Studio Script Debugger"));
					hr = m_pdm->AddApplication(m_da, &m_dwDebugCookie);
					//m_da->StartDebugSession();
				}
				hr = m_pdm->CreateDebugDocumentHelper(NULL, &m_ddh);
				if (m_ddh)
				{
					hr = m_ddh->Init(m_da, L"JavaScript Codes", L"Debugable JavaScript Codes List", TEXT_DOC_ATTR_READONLY);
					hr = m_ddh->Attach(NULL);
				}
			}
			else
				m_bDebugEnabled = FALSE;
		}
	}
	virtual ~CScriptHostImpl()
	{
		if (m_pUnk)
			m_pUnk.Release();

		if (m_da) m_da.Release();
		if (m_ddh) m_ddh.Release();
		if (m_pdm)
			m_pdm.Release();

		ULONG l;
		if (m_as)
		{
			//HRESULT hr = m_as->SetScriptState(SCRIPTSTATE_CONNECTED);
			//hr = m_as->SetScriptState(SCRIPTSTATE_DISCONNECTED);
			//SCRIPTSTATE ss;
			//m_as->GetScriptState(&ss);
			//hr = m_as->SetScriptState(SCRIPTSTATE_CLOSED);
			/*hr =*/ m_as->Close();
		}
		if (m_asp)
		{
			l = m_asp.p->Release();
			//while (l>0)
			//	l = m_asp.p->Release();
			m_asp.p = NULL;
		}
		if (m_as)
		{
			l = m_as.p->Release();
			//while (l>0)
			//	l = m_as.p->Release();
			m_as.p = NULL;
		}
	}
	void ReleaseThis() { delete this; }

	void SetHWND(HWND hWnd) { m_hwnd=hWnd; }
	HRESULT AddUnknown(LPCOLESTR pNamedItem, LPUNKNOWN lpUnk)
	{
		m_pUnk = lpUnk;
		return m_as->AddNamedItem(pNamedItem, SCRIPTITEM_ISVISIBLE|SCRIPTITEM_ISSOURCE);
	}

	void SetActiveNamedItem(LPCOLESTR nameditem) { m_pNamedItem = nameditem; }
	LPCOLESTR GetActiveNamedItem() { return m_pNamedItem; }

	HRESULT CreateScriptEngine(LPCOLESTR progid=L"JScript") // other is L"VBScript"
	{
		CLSID clsid;
		HRESULT hr = CLSIDFromProgID(progid, &clsid);

		hr = ::CoCreateInstance(clsid,NULL,CLSCTX_INPROC_SERVER,IID_IActiveScript,(void**)&m_as);
		if (SUCCEEDED(hr))
		{
			// QI the IActiveScriptParse pointer
			m_asp = m_as;
			//hr = m_as->QueryInterface(IID_IActiveScriptParse,(void**)&m_asp);
			if (m_asp==NULL)
			{
				m_as.Release();
				return hr;
			}
			// set the script site
			hr = m_as->SetScriptSite(this);
			if ( FAILED( hr ) )
				return hr;
			// initiate the script engine
			hr = m_asp->InitNew();
			if (FAILED(hr))
				return hr;
			hr = m_as->SetScriptState(SCRIPTSTATE_INITIALIZED);
		}
		return hr;
	}

	//HRESULT ParseFile(const char* pszFileName,LPCOLESTR pstrItemName=NULL)
	//{
	//	HRESULT hr = S_OK;
	//	struct _stat stat;
	//	size_t cch;
	//	EXCEPINFO ei;
	//	FILE *pfile; 

	//	if(_stat(pszFileName,&stat))
	//		return E_FAIL;

	//	cch = stat.st_size;
	//	char* pszAlloc = new char[cch + 1];
	//	pszAlloc[cch] = '\0';// this is vitally important
	//	if(pszAlloc == NULL)
	//		return E_OUTOFMEMORY;

	//	memset(pszAlloc,0,cch);

	//	// get the script text into a memory block
	//	pfile = fopen(pszFileName,"rb");
	//	if ( !pfile ) 
	//	{
	//		hr = E_FAIL;
	//		return hr;
	//	}
	//	fread(pszAlloc,cch,1,pfile);
	//	fclose(pfile);

	//	LPOLESTR pwszCode;
	//	int CharCount = MultiByteToWideChar(CP_ACP,0,pszAlloc,-1,NULL,0);
	//	pwszCode = new WCHAR[CharCount];
	//	MultiByteToWideChar(CP_ACP,0,pszAlloc,-1,pwszCode,CharCount);

	//	size_t t = wcslen(pwszCode);
	//	hr = m_asp->ParseScriptText(pwszCode, pstrItemName,NULL, NULL, 0,0,0L,NULL,&ei);
	//	delete []pwszCode;
	//	delete []pszAlloc;

	//	return hr;
	//}

	HRESULT ParseCode(LPCOLESTR pszCode, BOOL bNeedDebug=FALSE)
	{
		if (m_ddh && bNeedDebug)
		{
			m_ddh->AddUnicodeText(pszCode);
			DWORD dw;
			m_ddh->DefineScriptBlock(0, lstrlenW(pszCode), m_as, FALSE, &dw);
		}
		EXCEPINFO ei;
		HRESULT hr = m_asp->ParseScriptText(pszCode, m_pNamedItem, NULL, NULL, 0,0,0L,NULL,&ei);
		//if (SUCCEEDED(hr)) m_ulWorking ++;
		return hr;
	}

	HRESULT ParseProcedure(LPCOLESTR pszCode, IDispatch** ppDisp, DWORD dwFlags=0, BOOL bNeedDebug=FALSE)
	{
		if (m_ddh && bNeedDebug)
		{
			m_ddh->AddUnicodeText(pszCode);
			DWORD dw;
			m_ddh->DefineScriptBlock(0, lstrlenW(pszCode), m_as, FALSE, &dw);
		}

		HRESULT hr = E_FAIL;
		CComQIPtr<IActiveScriptParseProcedure> aspp(m_asp);
		if (aspp)
			hr = aspp->ParseProcedureText(pszCode, NULL, NULL, m_pNamedItem, NULL, NULL, 0, 0, dwFlags | SCRIPTPROC_IMPLICIT_THIS, ppDisp);
		//if (SUCCEEDED(hr)) m_ulWorking ++;
		return hr;
	}

public:
	//IUnknown members
	STDMETHOD(QueryInterface)(REFIID iid,void** ppvObj)
	{
		if (!ppvObj)
			return E_POINTER;

		*ppvObj = NULL;
		if (IsEqualIID(iid, __uuidof(IUnknown)) || 
			IsEqualIID(iid, IID_IActiveScriptSite))
		{
			*ppvObj = (IActiveScriptSite*)this;
			return S_OK;
		}
		else if (IsEqualIID(iid, IID_IActiveScriptSiteWindow))
		{
			*ppvObj = (IActiveScriptSiteWindow*)this;
			return S_OK;
		}
		else if (IsEqualIID(iid, __uuidof(IActiveScriptSiteDebug)) && m_bDebugEnabled)
		{
			*ppvObj = (IActiveScriptSiteDebug*)this;
			return S_OK;
		}
		return E_NOINTERFACE;
	}

	STDMETHOD_(unsigned long,AddRef)(void) { return 1; }
	STDMETHOD_(unsigned long,Release)() { return 1; }

public:
	//IActiveScriptSite members
	STDMETHOD(GetLCID)(LCID  *plcid) { ATLTRACENOTIMPL(_T("CScriptHostImpl::GetLCID")); }
	STDMETHOD(GetItemInfo)(LPCOLESTR pstrName, DWORD dwReturnMask, IUnknown** ppiunkItem, ITypeInfo** ppti)
	{
		HRESULT hr = S_OK;
		// initialize the sent-in pointers
		if(dwReturnMask & SCRIPTINFO_ITYPEINFO)
		{
			if(ppti == NULL)
				return E_INVALIDARG;
			*ppti = NULL;
		}
		if(dwReturnMask & SCRIPTINFO_IUNKNOWN)
		{  
			if(ppiunkItem == NULL)
				return E_INVALIDARG;
			*ppiunkItem = NULL;
		}

		if(/*!lstrcmpiW(m_pNamedItem, pstrName) &&*/ m_pUnk)
		{
			if(dwReturnMask & SCRIPTINFO_IUNKNOWN)
			{
				// give out the object's IUnknown pointer
				m_pUnk.QueryInterface(ppiunkItem);
			}
			if(dwReturnMask & SCRIPTINFO_ITYPEINFO)
			{
				CComQIPtr<IProvideClassInfo> spClsInfo = m_pUnk;
				if(spClsInfo != NULL)
				{
					hr = spClsInfo->GetClassInfo(ppti);
				}
			}
		}
		return hr;
	}

	STDMETHOD(GetDocVersionString)(BSTR  *pbstrVersion) { ATLTRACENOTIMPL(_T("CScriptHostImpl::GetDocVersionString")); }
	STDMETHOD(OnScriptTerminate)(const VARIANT  *pvarResult,const EXCEPINFO  *pexcepinfo)
	{
		if (pfn[sntOnScriptTerminate])
		{
			(*(fnScriptTerminateCallback)pfn[sntOnScriptTerminate])(pvarResult, pexcepinfo, lpParam);
			return S_OK;
		}
		ATLTRACENOTIMPL(_T("CScriptHostImpl::OnScriptTerminate"));
	}
	STDMETHOD(OnStateChange)(SCRIPTSTATE ssScriptState)
	{
		ATLTRACE(_T("ScriptState: %d. "), ssScriptState);
		if (ssScriptState==SCRIPTSTATE_DISCONNECTED) m_ulWorking = 0;
		if (pfn[sntOnStateChange])
		{
			(*(fnStateChangeCallback)pfn[sntOnStateChange])(ssScriptState, lpParam);
			return S_OK;
		}
		ATLTRACENOTIMPL(_T("CScriptHostImpl::OnStateChange"));
	}
	STDMETHOD(OnScriptError)(IActiveScriptError  *pscripterror)
	{
		if (m_ulWorking>0) m_ulWorking--;
		if (pfn[sntOnScriptError])
		{
			(*(fnScriptErrorCallback)pfn[sntOnScriptError])(pscripterror, lpParam);
			return S_OK;
		}
		ATLTRACENOTIMPL(_T("CScriptHostImpl::OnScriptError"));
	}
	STDMETHOD(OnEnterScript)(void)
	{
		m_ulWorking++;
		if (pfn[sntOnEnterScript])
		{
			(*(fnEnterScriptCallback)pfn[sntOnEnterScript])(lpParam);
			return S_OK;
		}
		return E_NOTIMPL;
		ATLTRACENOTIMPL(_T("CScriptHostImpl::OnEnterScript"));
	}
	STDMETHOD(OnLeaveScript)(void)
	{
		if (m_ulWorking>0) m_ulWorking--;
		if (pfn[sntOnLeaveScript])
		{
			(*(fnLeaveScriptCallback)pfn[sntOnLeaveScript])(lpParam);
			return S_OK;
		}
		return E_NOTIMPL;
		ATLTRACENOTIMPL(_T("CScriptHostImpl::OnLeaveScript"));
	}

public:
	//IActiveScriptSiteWindow members
	STDMETHOD(GetWindow)(HWND  *phwnd)
	{
		//if (m_hwnd != NULL)
		{
			*phwnd = m_hwnd;
			return S_OK;
		}

		return E_FAIL;
	}
	STDMETHOD(EnableModeless)(BOOL fEnable) { return S_OK; }

public:
	// IActiveScriptSiteDebug methods
	STDMETHOD (GetDocumentContextFromPosition)(DWORD dwSourceContext, ULONG uCharacterOffset, ULONG uNumChars, IDebugDocumentContext **ppsc)
	{
		ULONG ulStartPos = 0;
		HRESULT hr = E_NOTIMPL;

		if (m_ddh)
		{
			hr = m_ddh->GetScriptBlockInfo(dwSourceContext, NULL, &ulStartPos, NULL);
			hr = m_ddh->CreateDebugDocumentContext(ulStartPos + uCharacterOffset, uNumChars, ppsc);
		}

		return hr;
	}

	STDMETHOD (GetApplication)(IDebugApplication **ppda)
	{
		if (ppda==NULL)
			return E_POINTER;
		*ppda = NULL;

		if (m_da)
			return m_da.QueryInterface(ppda);

		return S_OK;
	}

	STDMETHOD (GetRootApplicationNode)(IDebugApplicationNode **ppdanRoot)
	{
		if (ppdanRoot==NULL)
			return E_POINTER;
		*ppdanRoot = NULL;

		if (m_ddh)
			return m_ddh->GetDebugApplicationNode(ppdanRoot);
		return E_NOTIMPL;
	}

	STDMETHOD (OnScriptErrorDebug)(IActiveScriptErrorDebug *pErrorDebug, BOOL *pfEnterDebugger, BOOL *pfCallOnScriptErrorWhenContinuing)
	{
		if (pfEnterDebugger)
			*pfEnterDebugger = TRUE;
		if (pfCallOnScriptErrorWhenContinuing)
			*pfCallOnScriptErrorWhenContinuing = TRUE;
		return S_OK;
	}

public:
	CComPtr<IActiveScript> m_as;
	CComQIPtr<IActiveScriptParse> m_asp;
	ULONG m_ulWorking;

	fnScriptNotifyCallback pfn[5];
	LPVOID lpParam;

private:
	CComPtr<IUnknown> m_pUnk;
	LPCOLESTR m_pNamedItem;
	HWND m_hwnd;

public:
	CComPtr<IProcessDebugManager> m_pdm;
	CComPtr<IDebugApplication> m_da;
	CComPtr<IDebugDocumentHelper> m_ddh;
	DWORD m_dwDebugCookie;
	BOOL m_bDebugEnabled;
};

#endif // __SCRIPTHOST_H__