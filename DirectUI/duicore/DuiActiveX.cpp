
#include "StdAfx.h"
#include "DuiActiveX.h"
#include "../../common/theme.h"


/////////////////////////////////////////////////////////////////////////////////////
//
//

//class CActiveXCtrl;
//
//
///////////////////////////////////////////////////////////////////////////////////////
////
////
//
//class CActiveXWnd : public CDuiWindowBase
//{
//public:
//	HWND Init(CActiveXCtrl* pOwner, HWND hWndParent);
//
//	LPCTSTR GetWindowClassName() const;
//	void OnFinalMessage(HWND hWnd);
//
//	LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
//
//protected:
//	void DoVerb(LONG iVerb);
//
//	LRESULT OnMouseActivate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
//	LRESULT OnSetFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
//	LRESULT OnKillFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
//	LRESULT OnEraseBkgnd(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
//	LRESULT OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
//
//protected:
//	CActiveXCtrl* m_pOwner;
//};
//
//
///////////////////////////////////////////////////////////////////////////////////////
////
////
//
//class CActiveXEnum : public IEnumUnknown
//{
//public:
//	CActiveXEnum(IUnknown* pUnk) : m_pUnk(pUnk), m_dwRef(1), m_iPos(0)
//	{
//		m_pUnk->AddRef();
//	}
//	~CActiveXEnum()
//	{
//		m_pUnk->Release();
//	}
//
//	LONG m_iPos;
//	ULONG m_dwRef;
//	IUnknown* m_pUnk;
//
//	STDMETHOD_(ULONG,AddRef)()
//	{
//		return ++m_dwRef;
//	}
//	STDMETHOD_(ULONG,Release)()
//	{
//		LONG lRef = --m_dwRef;
//		if( lRef == 0 ) delete this;
//		return lRef;
//	}
//	STDMETHOD(QueryInterface)(REFIID riid, LPVOID *ppvObject)
//	{
//		*ppvObject = NULL;
//		if( riid == IID_IUnknown ) *ppvObject = static_cast<IEnumUnknown*>(this);
//		else if( riid == IID_IEnumUnknown ) *ppvObject = static_cast<IEnumUnknown*>(this);
//		if( *ppvObject != NULL ) AddRef();
//		return *ppvObject == NULL ? E_NOINTERFACE : S_OK;
//	}
//	STDMETHOD(Next)(ULONG celt, IUnknown **rgelt, ULONG *pceltFetched)
//	{
//		if( pceltFetched != NULL ) *pceltFetched = 0;
//		if( ++m_iPos > 1 ) return S_FALSE;
//		*rgelt = m_pUnk;
//		(*rgelt)->AddRef();
//		if( pceltFetched != NULL ) *pceltFetched = 1;
//		return S_OK;
//	}
//	STDMETHOD(Skip)(ULONG celt)
//	{
//		m_iPos += celt;
//		return S_OK;
//	}
//	STDMETHOD(Reset)(void)
//	{
//		m_iPos = 0;
//		return S_OK;
//	}
//	STDMETHOD(Clone)(IEnumUnknown **ppenum)
//	{
//		return E_NOTIMPL;
//	}
//};
//
//
///////////////////////////////////////////////////////////////////////////////////////
////
////
//
//class CActiveXFrameWnd : public IOleInPlaceFrame
//{
//public:
//	CActiveXFrameWnd(CDuiActiveX* pOwner) : m_dwRef(1), m_pOwner(pOwner), m_pActiveObject(NULL)
//	{
//	}
//	~CActiveXFrameWnd()
//	{
//		if( m_pActiveObject != NULL ) m_pActiveObject->Release();
//	}
//
//	ULONG m_dwRef;
//	CDuiActiveX* m_pOwner;
//	IOleInPlaceActiveObject* m_pActiveObject;
//
//	// IUnknown
//	STDMETHOD_(ULONG,AddRef)()
//	{
//		return ++m_dwRef;
//	}
//	STDMETHOD_(ULONG,Release)()
//	{
//		ULONG lRef = --m_dwRef;
//		if( lRef == 0 ) delete this;
//		return lRef;
//	}
//	STDMETHOD(QueryInterface)(REFIID riid, LPVOID *ppvObject)
//	{
//		*ppvObject = NULL;
//		if( riid == IID_IUnknown ) *ppvObject = static_cast<IOleInPlaceFrame*>(this);
//		else if( riid == IID_IOleWindow ) *ppvObject = static_cast<IOleWindow*>(this);
//		else if( riid == IID_IOleInPlaceFrame ) *ppvObject = static_cast<IOleInPlaceFrame*>(this);
//		else if( riid == IID_IOleInPlaceUIWindow ) *ppvObject = static_cast<IOleInPlaceUIWindow*>(this);
//		if( *ppvObject != NULL ) AddRef();
//		return *ppvObject == NULL ? E_NOINTERFACE : S_OK;
//	}  
//	// IOleInPlaceFrameWindow
//	STDMETHOD(InsertMenus)(HMENU /*hmenuShared*/, LPOLEMENUGROUPWIDTHS /*lpMenuWidths*/)
//	{
//		return S_OK;
//	}
//	STDMETHOD(SetMenu)(HMENU /*hmenuShared*/, HOLEMENU /*holemenu*/, HWND /*hwndActiveObject*/)
//	{
//		return S_OK;
//	}
//	STDMETHOD(RemoveMenus)(HMENU /*hmenuShared*/)
//	{
//		return S_OK;
//	}
//	STDMETHOD(SetStatusText)(LPCOLESTR /*pszStatusText*/)
//	{
//		return S_OK;
//	}
//	STDMETHOD(EnableModeless)(BOOL /*fEnable*/)
//	{
//		return S_OK;
//	}
//	STDMETHOD(TranslateAccelerator)(LPMSG /*lpMsg*/, WORD /*wID*/)
//	{
//		return S_FALSE;
//	}
//	// IOleWindow
//	STDMETHOD(GetWindow)(HWND* phwnd)
//	{
//		if( m_pOwner == NULL ) return E_UNEXPECTED;
//		*phwnd = rt(m_pOwner)->m_pLayoutMgr->GetPaintWindow();
//		return S_OK;
//	}
//	STDMETHOD(ContextSensitiveHelp)(BOOL /*fEnterMode*/)
//	{
//		return S_OK;
//	}
//	// IOleInPlaceUIWindow
//	STDMETHOD(GetBorder)(LPRECT /*lprectBorder*/)
//	{
//		return S_OK;
//	}
//	STDMETHOD(RequestBorderSpace)(LPCBORDERWIDTHS /*pborderwidths*/)
//	{
//		return INPLACE_E_NOTOOLSPACE;
//	}
//	STDMETHOD(SetBorderSpace)(LPCBORDERWIDTHS /*pborderwidths*/)
//	{
//		return S_OK;
//	}
//	STDMETHOD(SetActiveObject)(IOleInPlaceActiveObject* pActiveObject, LPCOLESTR /*pszObjName*/)
//	{
//		if( pActiveObject != NULL ) pActiveObject->AddRef();
//		if( m_pActiveObject != NULL ) m_pActiveObject->Release();
//		m_pActiveObject = pActiveObject;
//		return S_OK;
//	}
//};
//
//
///////////////////////////////////////////////////////////////////////////////////////
////
////
//
//class CActiveXCtrl :
//	public IOleClientSite,
//	public IOleInPlaceSiteWindowless,
//	public IOleControlSite,
//	public IObjectWithSite,
//	public IOleContainer
//{
//	friend CDuiActiveX;
//	friend CActiveXWnd;
//public:
//	CActiveXCtrl();
//	~CActiveXCtrl();
//
//	// IUnknown
//	STDMETHOD_(ULONG,AddRef)();
//	STDMETHOD_(ULONG,Release)();
//	STDMETHOD(QueryInterface)(REFIID riid, LPVOID *ppvObject);
//
//	// IObjectWithSite
//	STDMETHOD(SetSite)(IUnknown *pUnkSite);
//	STDMETHOD(GetSite)(REFIID riid, LPVOID* ppvSite);
//
//	// IOleClientSite
//	STDMETHOD(SaveObject)(void);       
//	STDMETHOD(GetMoniker)(DWORD dwAssign, DWORD dwWhichMoniker, IMoniker** ppmk);
//	STDMETHOD(GetContainer)(IOleContainer** ppContainer);        
//	STDMETHOD(ShowObject)(void);        
//	STDMETHOD(OnShowWindow)(BOOL fShow);        
//	STDMETHOD(RequestNewObjectLayout)(void);
//
//	// IOleInPlaceSiteWindowless
//	STDMETHOD(CanWindowlessActivate)(void);
//	STDMETHOD(GetCapture)(void);
//	STDMETHOD(SetCapture)(BOOL fCapture);
//	STDMETHOD(GetFocus)(void);
//	STDMETHOD(SetFocus)(BOOL fFocus);
//	STDMETHOD(GetDC)(LPCRECT pRect, DWORD grfFlags, HDC* phDC);
//	STDMETHOD(ReleaseDC)(HDC hDC);
//	STDMETHOD(InvalidateRect)(LPCRECT pRect, BOOL fErase);
//	STDMETHOD(InvalidateRgn)(HRGN hRGN, BOOL fErase);
//	STDMETHOD(ScrollRect)(INT dx, INT dy, LPCRECT pRectScroll, LPCRECT pRectClip);
//	STDMETHOD(AdjustRect)(LPRECT prc);
//	STDMETHOD(OnDefWindowMessage)(UINT msg, WPARAM wParam, LPARAM lParam, LRESULT* plResult);
//
//	// IOleInPlaceSiteEx
//	STDMETHOD(OnInPlaceActivateEx)(BOOL *pfNoRedraw, DWORD dwFlags);        
//	STDMETHOD(OnInPlaceDeactivateEx)(BOOL fNoRedraw);       
//	STDMETHOD(RequestUIActivate)(void);
//
//	// IOleInPlaceSite
//	STDMETHOD(CanInPlaceActivate)(void);       
//	STDMETHOD(OnInPlaceActivate)(void);        
//	STDMETHOD(OnUIActivate)(void);
//	STDMETHOD(GetWindowContext)(IOleInPlaceFrame** ppFrame, IOleInPlaceUIWindow** ppDoc, LPRECT lprcPosRect, LPRECT lprcClipRect, LPOLEINPLACEFRAMEINFO lpFrameInfo);
//	STDMETHOD(Scroll)(SIZE scrollExtant);
//	STDMETHOD(OnUIDeactivate)(BOOL fUndoable);
//	STDMETHOD(OnInPlaceDeactivate)(void);
//	STDMETHOD(DiscardUndoState)( void);
//	STDMETHOD(DeactivateAndUndo)( void);
//	STDMETHOD(OnPosRectChange)(LPCRECT lprcPosRect);
//
//	// IOleWindow
//	STDMETHOD(GetWindow)(HWND* phwnd);
//	STDMETHOD(ContextSensitiveHelp)(BOOL fEnterMode);
//
//	// IOleControlSite
//	STDMETHOD(OnControlInfoChanged)(void);      
//	STDMETHOD(LockInPlaceActive)(BOOL fLock);       
//	STDMETHOD(GetExtendedControl)(IDispatch** ppDisp);        
//	STDMETHOD(TransformCoords)(POINTL* pPtlHimetric, POINTF* pPtfContainer, DWORD dwFlags);       
//	STDMETHOD(TranslateAccelerator)(MSG* pMsg, DWORD grfModifiers);
//	STDMETHOD(OnFocus)(BOOL fGotFocus);
//	STDMETHOD(ShowPropertyFrame)(void);
//
//	// IOleContainer
//	STDMETHOD(EnumObjects)(DWORD grfFlags, IEnumUnknown** ppenum);
//	STDMETHOD(LockContainer)(BOOL fLock);
//
//	// IParseDisplayName
//	STDMETHOD(ParseDisplayName)(IBindCtx* pbc, LPOLESTR pszDisplayName, ULONG* pchEaten, IMoniker** ppmkOut);
//
//protected:
//	HRESULT CreateActiveXWnd();
//
//protected:
//	LONG m_dwRef;
//	CDuiActiveX* m_pOwner;
//	CActiveXWnd* m_pWindow;
//	IUnknown* m_pUnkSite;
//	IViewObject* m_pViewObject;
//	IOleInPlaceObjectWindowless* m_spInPlaceObject;
//	BOOL m_bLocked;
//	BOOL m_bFocused;
//	BOOL m_bCaptured;
//	BOOL m_bUIActivated;
//	BOOL m_bInPlaceActive;
//	BOOL m_bWindowless;
//};
//
//
//CActiveXCtrl::CActiveXCtrl() : 
//m_dwRef(1), 
//m_pOwner(NULL), 
//m_pWindow(NULL),
//m_pUnkSite(NULL), 
//m_pViewObject(NULL),
//m_spInPlaceObject(NULL),
//m_bLocked(FALSE), 
//m_bFocused(FALSE),
//m_bCaptured(FALSE),
//m_bWindowless(TRUE),
//m_bUIActivated(FALSE),
//m_bInPlaceActive(FALSE)
//{
//}
//
//CActiveXCtrl::~CActiveXCtrl()
//{
//	if( m_pWindow != NULL ) ::DestroyWindow(*m_pWindow);
//	if( m_pUnkSite != NULL ) m_pUnkSite->Release();
//	if( m_pViewObject != NULL ) m_pViewObject->Release();
//	if( m_spInPlaceObject != NULL ) m_spInPlaceObject->Release();
//}
//
//STDMETHODIMP CActiveXCtrl::QueryInterface(REFIID riid, LPVOID *ppvObject)
//{
//	*ppvObject = NULL;
//	if( riid == IID_IUnknown )                       *ppvObject = static_cast<IOleWindow*>(this);
//	else if( riid == IID_IOleClientSite )            *ppvObject = static_cast<IOleClientSite*>(this);
//	else if( riid == IID_IOleInPlaceSiteWindowless ) *ppvObject = static_cast<IOleInPlaceSiteWindowless*>(this);
//	else if( riid == IID_IOleInPlaceSiteEx )         *ppvObject = static_cast<IOleInPlaceSiteEx*>(this);
//	else if( riid == IID_IOleInPlaceSite )           *ppvObject = static_cast<IOleInPlaceSite*>(this);
//	else if( riid == IID_IOleWindow )                *ppvObject = static_cast<IOleWindow*>(this);
//	else if( riid == IID_IOleControlSite )           *ppvObject = static_cast<IOleControlSite*>(this);
//	else if( riid == IID_IOleContainer )             *ppvObject = static_cast<IOleContainer*>(this);
//	else if( riid == IID_IObjectWithSite )           *ppvObject = static_cast<IObjectWithSite*>(this);
//	if( *ppvObject != NULL ) AddRef();
//	return *ppvObject == NULL ? E_NOINTERFACE : S_OK;
//}
//
//STDMETHODIMP_(ULONG) CActiveXCtrl::AddRef()
//{
//	return ++m_dwRef;
//}
//
//STDMETHODIMP_(ULONG) CActiveXCtrl::Release()
//{
//	LONG lRef = --m_dwRef;
//	if( lRef == 0 ) delete this;
//	return lRef;
//}
//
//STDMETHODIMP CActiveXCtrl::SetSite(IUnknown *pUnkSite)
//{
//	TRACE(_T("AX: CActiveXCtrl::SetSite"));
//	if( m_pUnkSite != NULL ) {
//		m_pUnkSite->Release();
//		m_pUnkSite = NULL;
//	}
//	if( pUnkSite != NULL ) {
//		m_pUnkSite = pUnkSite;
//		m_pUnkSite->AddRef();
//	}
//	return S_OK;
//}
//
//STDMETHODIMP CActiveXCtrl::GetSite(REFIID riid, LPVOID* ppvSite)
//{
//	TRACE(_T("AX: CActiveXCtrl::GetSite"));
//	if( ppvSite == NULL ) return E_POINTER;
//	*ppvSite = NULL;
//	if( m_pUnkSite == NULL ) return E_FAIL;
//	return m_pUnkSite->QueryInterface(riid, ppvSite);
//}
//
//STDMETHODIMP CActiveXCtrl::SaveObject(void)
//{
//	TRACE(_T("AX: CActiveXCtrl::SaveObject"));
//	return E_NOTIMPL;
//}
//
//STDMETHODIMP CActiveXCtrl::GetMoniker(DWORD dwAssign, DWORD dwWhichMoniker, IMoniker** ppmk)
//{
//	TRACE(_T("AX: CActiveXCtrl::GetMoniker"));
//	if( ppmk != NULL ) *ppmk = NULL;
//	return E_NOTIMPL;
//}
//
//STDMETHODIMP CActiveXCtrl::GetContainer(IOleContainer** ppContainer)
//{
//	TRACE(_T("AX: CActiveXCtrl::GetContainer"));
//	if( ppContainer == NULL ) return E_POINTER;
//	*ppContainer = NULL;
//	HRESULT Hr = E_NOTIMPL;
//	if( m_pUnkSite != NULL ) Hr = m_pUnkSite->QueryInterface(IID_IOleContainer, (LPVOID*) ppContainer);
//	if( FAILED(Hr) ) Hr = QueryInterface(IID_IOleContainer, (LPVOID*) ppContainer);
//	return Hr;
//}
//
//STDMETHODIMP CActiveXCtrl::ShowObject(void)
//{
//	TRACE(_T("AX: CActiveXCtrl::ShowObject"));
//	if( m_pOwner == NULL ) return E_UNEXPECTED;
//	HDC hDC = ::GetDC(m_pOwner->m_hwndHost);
//	if( hDC == NULL ) return E_FAIL;
//	if( m_pViewObject != NULL ) m_pViewObject->Draw(DVASPECT_CONTENT, -1, NULL, NULL, NULL, hDC, (RECTL*) &rt(m_pOwner)->GetPos(DUIRT_BORDER), (RECTL*) &rt(m_pOwner)->GetPos(DUIRT_BORDER), NULL, NULL);
//	::ReleaseDC(m_pOwner->m_hwndHost, hDC);
//	return S_OK;
//}
//
//STDMETHODIMP CActiveXCtrl::OnShowWindow(BOOL fShow)
//{
//	TRACE(_T("AX: CActiveXCtrl::OnShowWindow"));
//	return E_NOTIMPL;
//}
//
//STDMETHODIMP CActiveXCtrl::RequestNewObjectLayout(void)
//{
//	return E_NOTIMPL;
//}
//
//STDMETHODIMP CActiveXCtrl::CanWindowlessActivate(void)
//{
//	return S_OK;  // Yes, we can!!
//}
//
//STDMETHODIMP CActiveXCtrl::GetCapture(void)
//{
//	if( m_pOwner == NULL ) return E_UNEXPECTED;
//	return m_bCaptured ? S_OK : S_FALSE;
//}
//
//STDMETHODIMP CActiveXCtrl::SetCapture(BOOL fCapture)
//{
//	if( m_pOwner == NULL ) return E_UNEXPECTED;
//	m_bCaptured = (fCapture == TRUE);
//	if( fCapture ) ::SetCapture(m_pOwner->m_hwndHost); else ::ReleaseCapture();
//	return S_OK;
//}
//
//STDMETHODIMP CActiveXCtrl::GetFocus(void)
//{
//	if( m_pOwner == NULL ) return E_UNEXPECTED;
//	return m_bFocused ? S_OK : S_FALSE;
//}
//
//STDMETHODIMP CActiveXCtrl::SetFocus(BOOL fFocus)
//{
//	if( m_pOwner == NULL ) return E_UNEXPECTED;
//	if( fFocus ) rt(m_pOwner)->SetFocus();
//	m_bFocused = (fFocus == TRUE);
//	return S_OK;
//}
//
//STDMETHODIMP CActiveXCtrl::GetDC(LPCRECT pRect, DWORD grfFlags, HDC* phDC)
//{
//	if( phDC == NULL ) return E_POINTER;
//	if( m_pOwner == NULL ) return E_UNEXPECTED;
//	*phDC = ::GetDC(m_pOwner->m_hwndHost);
//	if( (grfFlags & OLEDC_PAINTBKGND) != 0 ) {
//		CRect rcItem = rt(m_pOwner)->GetPos();
//		if( !m_bWindowless ) rcItem.OffsetRect(-rcItem.left, -rcItem.top);
//		::FillRect(*phDC, &rcItem, (HBRUSH) (COLOR_WINDOW + 1));
//	}
//	return S_OK;
//}
//
//STDMETHODIMP CActiveXCtrl::ReleaseDC(HDC hDC)
//{
//	if( m_pOwner == NULL ) return E_UNEXPECTED;
//	::ReleaseDC(m_pOwner->m_hwndHost, hDC);
//	return S_OK;
//}
//
//STDMETHODIMP CActiveXCtrl::InvalidateRect(LPCRECT pRect, BOOL fErase)
//{
//	if( m_pOwner == NULL ) return E_UNEXPECTED;
//	if( m_pOwner->m_hwndHost == NULL ) return E_FAIL;
//	return ::InvalidateRect(m_pOwner->m_hwndHost, pRect, fErase) ? S_OK : E_FAIL;
//}
//
//STDMETHODIMP CActiveXCtrl::InvalidateRgn(HRGN hRGN, BOOL fErase)
//{
//	if( m_pOwner == NULL ) return E_UNEXPECTED;
//	return ::InvalidateRgn(m_pOwner->m_hwndHost, hRGN, fErase) ? S_OK : E_FAIL;
//}
//
//STDMETHODIMP CActiveXCtrl::ScrollRect(INT dx, INT dy, LPCRECT pRectScroll, LPCRECT pRectClip)
//{
//	return S_OK;
//}
//
//STDMETHODIMP CActiveXCtrl::AdjustRect(LPRECT prc)
//{
//	return S_OK;
//}
//
//STDMETHODIMP CActiveXCtrl::OnDefWindowMessage(UINT msg, WPARAM wParam, LPARAM lParam, LRESULT* plResult)
//{
//	if( m_pOwner == NULL ) return E_UNEXPECTED;
//	*plResult = ::DefWindowProc(m_pOwner->m_hwndHost, msg, wParam, lParam);
//	return S_OK;
//}
//
//STDMETHODIMP CActiveXCtrl::OnInPlaceActivateEx(BOOL* pfNoRedraw, DWORD dwFlags)        
//{
//	ASSERT(m_spInPlaceObject==NULL);
//	if( m_pOwner == NULL ) return E_UNEXPECTED;
//	if( m_pOwner->m_pUnk == NULL ) return E_UNEXPECTED;
//	::OleLockRunning(m_pOwner->m_pUnk, TRUE, FALSE);
//	HWND hWndFrame = rt(m_pOwner)->m_pLayoutMgr->GetPaintWindow();
//	HRESULT Hr = E_FAIL;
//	if( (dwFlags & ACTIVATE_WINDOWLESS) != 0 ) {
//		m_bWindowless = TRUE;
//		Hr = m_pOwner->m_pUnk->QueryInterface(IID_IOleInPlaceObjectWindowless, (LPVOID*) &m_spInPlaceObject);
//		m_pOwner->m_hwndHost = hWndFrame;
//		rt(m_pOwner)->m_pLayoutMgr->AddMessageFilter(m_pOwner);
//	}
//	if( FAILED(Hr) ) {
//		m_bWindowless = FALSE;
//		Hr = CreateActiveXWnd();
//		if( FAILED(Hr) ) return Hr;
//		Hr = m_pOwner->m_pUnk->QueryInterface(IID_IOleInPlaceObject, (LPVOID*) &m_spInPlaceObject);
//	}
//	if( m_spInPlaceObject != NULL ) {
//		CRect rcItem = rt(m_pOwner)->GetPos(DUIRT_BORDER);
//		if( !m_bWindowless ) rcItem.OffsetRect(-rcItem.left, -rcItem.top);
//		m_spInPlaceObject->SetObjectRects(&rcItem, &rcItem);
//	}
//	m_bInPlaceActive = SUCCEEDED(Hr);
//	return Hr;
//}
//
//STDMETHODIMP CActiveXCtrl::OnInPlaceDeactivateEx(BOOL fNoRedraw)       
//{
//	m_bInPlaceActive = FALSE;
//	if( m_spInPlaceObject != NULL ) {
//		m_spInPlaceObject->Release();
//		m_spInPlaceObject = NULL;
//	}
//	if( m_pWindow != NULL ) {
//		::DestroyWindow(*m_pWindow);
//		m_pWindow = NULL;
//	}
//	return S_OK;
//}
//
//STDMETHODIMP CActiveXCtrl::RequestUIActivate(void)
//{
//	return S_OK;
//}
//
//STDMETHODIMP CActiveXCtrl::CanInPlaceActivate(void)       
//{
//	return S_OK;
//}
//
//STDMETHODIMP CActiveXCtrl::OnInPlaceActivate(void)
//{
//	BOOL bDummy = FALSE;
//	return OnInPlaceActivateEx(&bDummy, 0);
//}
//
//STDMETHODIMP CActiveXCtrl::OnUIActivate(void)
//{
//	m_bUIActivated = TRUE;
//	return S_OK;
//}
//
//STDMETHODIMP CActiveXCtrl::GetWindowContext(IOleInPlaceFrame** ppFrame, IOleInPlaceUIWindow** ppDoc, LPRECT lprcPosRect, LPRECT lprcClipRect, LPOLEINPLACEFRAMEINFO lpFrameInfo)
//{
//	if( ppDoc == NULL ) return E_POINTER;
//	if( ppFrame == NULL ) return E_POINTER;
//	if( lprcPosRect == NULL ) return E_POINTER;
//	if( lprcClipRect == NULL ) return E_POINTER;
//	*ppFrame = NEW CActiveXFrameWnd(m_pOwner);
//	*ppDoc = NULL;
//	ACCEL ac = { 0 };
//	HACCEL hac = ::CreateAcceleratorTable(&ac, 1);
//	lpFrameInfo->cb = sizeof(OLEINPLACEFRAMEINFO);
//	lpFrameInfo->fMDIApp = FALSE;
//	lpFrameInfo->hwndFrame = rt(m_pOwner)->m_pLayoutMgr->GetPaintWindow();
//	lpFrameInfo->haccel = hac;
//	lpFrameInfo->cAccelEntries = 1;
//	return S_OK;
//}
//
//STDMETHODIMP CActiveXCtrl::Scroll(SIZE scrollExtant)
//{
//	return E_NOTIMPL;
//}
//
//STDMETHODIMP CActiveXCtrl::OnUIDeactivate(BOOL fUndoable)
//{
//	m_bUIActivated = FALSE;
//	return S_OK;
//}
//
//STDMETHODIMP CActiveXCtrl::OnInPlaceDeactivate(void)
//{
//	return OnInPlaceDeactivateEx(TRUE);
//}
//
//STDMETHODIMP CActiveXCtrl::DiscardUndoState(void)
//{
//	return E_NOTIMPL;
//}
//
//STDMETHODIMP CActiveXCtrl::DeactivateAndUndo(void)
//{
//	return E_NOTIMPL;
//}
//
//STDMETHODIMP CActiveXCtrl::OnPosRectChange(LPCRECT lprcPosRect)
//{
//	return E_NOTIMPL;
//}
//
//STDMETHODIMP CActiveXCtrl::GetWindow(HWND* phwnd)
//{
//	if( m_pOwner == NULL ) return E_UNEXPECTED;
//	if( m_pOwner->m_hwndHost == NULL ) CreateActiveXWnd();
//	if( m_pOwner->m_hwndHost == NULL ) return E_FAIL;
//	*phwnd = m_pOwner->m_hwndHost;
//	return S_OK;
//}
//
//STDMETHODIMP CActiveXCtrl::ContextSensitiveHelp(BOOL fEnterMode)
//{
//	return S_OK;
//}
//
//STDMETHODIMP CActiveXCtrl::OnControlInfoChanged(void)      
//{
//	return S_OK;
//}
//
//STDMETHODIMP CActiveXCtrl::LockInPlaceActive(BOOL fLock)       
//{
//	return S_OK;
//}
//
//STDMETHODIMP CActiveXCtrl::GetExtendedControl(IDispatch** ppDisp)        
//{
//	if( ppDisp == NULL ) return E_POINTER;   
//	if( m_pOwner == NULL ) return E_UNEXPECTED;
//	if( m_pOwner->m_pUnk == NULL ) return E_UNEXPECTED;
//	return m_pOwner->m_pUnk->QueryInterface(IID_IDispatch, (LPVOID*) ppDisp);
//}
//
//STDMETHODIMP CActiveXCtrl::TransformCoords(POINTL* pPtlHimetric, POINTF* pPtfContainer, DWORD dwFlags)       
//{
//	return S_OK;
//}
//
//STDMETHODIMP CActiveXCtrl::TranslateAccelerator(MSG *pMsg, DWORD grfModifiers)
//{
//	return S_FALSE;
//}
//
//STDMETHODIMP CActiveXCtrl::OnFocus(BOOL fGotFocus)
//{
//	m_bFocused = (fGotFocus == TRUE);
//	return S_OK;
//}
//
//STDMETHODIMP CActiveXCtrl::ShowPropertyFrame(void)
//{
//	return E_NOTIMPL;
//}
//
//STDMETHODIMP CActiveXCtrl::EnumObjects(DWORD grfFlags, IEnumUnknown** ppenum)
//{
//	if( ppenum == NULL ) return E_POINTER;
//	if( m_pOwner == NULL ) return E_UNEXPECTED;
//	*ppenum = NEW CActiveXEnum(m_pOwner->m_pUnk);
//	return S_OK;
//}
//
//STDMETHODIMP CActiveXCtrl::LockContainer(BOOL fLock)
//{
//	m_bLocked = fLock != FALSE;
//	return S_OK;
//}
//
//STDMETHODIMP CActiveXCtrl::ParseDisplayName(IBindCtx *pbc, LPOLESTR pszDisplayName, ULONG* pchEaten, IMoniker** ppmkOut)
//{
//	return E_NOTIMPL;
//}
//
//HRESULT CActiveXCtrl::CreateActiveXWnd()
//{
//	if( m_pWindow != NULL ) return S_OK;
//	m_pWindow = NEW CActiveXWnd;
//	if( m_pWindow == NULL ) return E_OUTOFMEMORY;
//	m_pOwner->m_hwndHost = m_pWindow->Init(this, rt(m_pOwner)->m_pLayoutMgr->GetPaintWindow());
//	return S_OK;
//}
//
//
///////////////////////////////////////////////////////////////////////////////////////
////
////
//
//HWND CActiveXWnd::Init(CActiveXCtrl* pOwner, HWND hWndParent)
//{
//	m_pOwner = pOwner;
//	UINT uStyle = WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS;
//	Create(hWndParent, _T("UIActiveX"), uStyle, 0L, 0,0,0,0, NULL);
//	return m_hWnd;
//}
//
//LPCTSTR CActiveXWnd::GetWindowClassName() const
//{
//	return _T("ActiveXWnd");
//}
//
//void CActiveXWnd::OnFinalMessage(HWND hWnd)
//{
//	delete this;
//}
//
//void CActiveXWnd::DoVerb(LONG iVerb)
//{
//	if( m_pOwner == NULL ) return;
//	if( m_pOwner->m_pOwner == NULL ) return;
//	//CComPtr<IOleObject> spOleObject;
//	//m_pOwner->m_pOwner->GetControl(IID_IOleObject, (LPVOID*) &spOleObject);
//	if( m_pOwner->m_pOwner->m_pUnk == NULL ) return;
//	//CComQIPtr<IOleClientSite> spOleClientSite(spOleObject);
//	m_pOwner->m_pOwner->m_pUnk->DoVerb(iVerb, NULL, m_pOwner, 0, m_hWnd, &rt(m_pOwner->m_pOwner)->GetPos());
//}
//
//LRESULT CActiveXWnd::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
//{
//	LRESULT lRes;
//	BOOL bHandled = TRUE;
//	switch( uMsg )
//	{
//	case WM_PAINT:         lRes = OnPaint(uMsg, wParam, lParam, bHandled); break;
//	case WM_SETFOCUS:      lRes = OnSetFocus(uMsg, wParam, lParam, bHandled); break;
//	case WM_KILLFOCUS:     lRes = OnKillFocus(uMsg, wParam, lParam, bHandled); break;
//	case WM_ERASEBKGND:    lRes = OnEraseBkgnd(uMsg, wParam, lParam, bHandled); break;
//	case WM_MOUSEACTIVATE: lRes = OnMouseActivate(uMsg, wParam, lParam, bHandled); break;
//	default:
//		bHandled = FALSE;
//	}
//	if( !bHandled ) return CDuiWindowBase::HandleMessage(uMsg, wParam, lParam);
//	return lRes;
//}
//
//LRESULT CActiveXWnd::OnEraseBkgnd(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
//{
//	if( m_pOwner->m_pViewObject == NULL ) bHandled = FALSE;
//	return 1;
//}
//
//LRESULT CActiveXWnd::OnMouseActivate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
//{
//	//CComPtr<IOleObject> spOleObject;
//	//m_pOwner->m_pOwner->GetControl(IID_IOleObject, (LPVOID*) &spOleObject);
//	if( m_pOwner->m_pOwner->m_pUnk == NULL ) return 0;
//	DWORD dwMiscStatus = 0;
//	m_pOwner->m_pOwner->m_pUnk->GetMiscStatus(DVASPECT_CONTENT, &dwMiscStatus);
//	if( (dwMiscStatus & OLEMISC_NOUIACTIVATE) != 0 ) return 0;
//	if( !m_pOwner->m_bInPlaceActive ) DoVerb(OLEIVERB_INPLACEACTIVATE);
//	bHandled = FALSE;
//	return 0;
//}
//
//LRESULT CActiveXWnd::OnSetFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
//{
//	bHandled = FALSE;
//	m_pOwner->m_bFocused = TRUE;
//	if( !m_pOwner->m_bUIActivated ) DoVerb(OLEIVERB_UIACTIVATE);
//	return 0;
//}
//
//LRESULT CActiveXWnd::OnKillFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
//{
//	bHandled = FALSE;
//	m_pOwner->m_bFocused = FALSE;
//	return 0;
//}
//
//LRESULT CActiveXWnd::OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
//{
//	PAINTSTRUCT ps = { 0 };
//	::BeginPaint(m_hWnd, &ps);
//	::EndPaint(m_hWnd, &ps);
//	return 1;
//}


/////////////////////////////////////////////////////////////////////////////////////
//
//

CDuiActiveX::CDuiActiveX() : m_sink(this), m_clsid(GUID_NULL)
{
}

CDuiActiveX::~CDuiActiveX()
{
	ReleaseControl();
}

//SIZE CDuiActiveX::EstimateSize(SIZE szAvailable)
//{
//	SIZE sz = {0};
//	sz.cx = (LONG)*GetStyleAttribute(SAT_WIDTH);
//	sz.cy = (LONG)*GetStyleAttribute(SAT_HEIGHT);
//	return sz;
//}

BOOL CDuiActiveX::IsMyEvent( HANDLE, DuiEvent* pEvent )
{
	DuiEvent& event = *pEvent;
	DUI_EVENTTYPE e = DuiNode::NameToEventType(event.Name);
	if (e == DUIET__INVALID) return FALSE;

	//m_pOwner->OnEvent(event);

	if (e == DUIET_load)
	{
		HRESULT hr = CreateControl(m_clsid);
		if (SUCCEEDED(hr))
		{
			m_sink.Connect();

			// now attach ActiveX object to control peer.
			CComQIPtr<IPeerHolder2> peer(m_pOwner->GetObject(TRUE));
			if (peer.p)
			{
				peer->AttachObject2(GetDispatch(), __ClassName());

				CComQIPtr<IDispatch> obj(m_spOleObject);
				if (obj)
					peer->AttachObject(obj);
			}

			//m_pLayoutMgr->AddMessageFilter(this);
		}
	}
	else if (e == DUIET_unload)
	{
		ReleaseControl();
	}
	else
	{
		// 激活不能放在load事件里做，因为此时正在解析XML文件
		if (m_spOleObject.p && !m_bInPlaceActive)
		{
			ActivateAx();
			BindSurfaceWindow();
		}

		if (e == DUIET_setfocus)
		{
			m_bHaveFocus = TRUE;
			if (!m_bReleaseAll)
			{
				RECT rc = GetPos();
				if (m_spOleObject.p && !m_bUIActive)
					m_spOleObject->DoVerb(OLEIVERB_UIACTIVATE, NULL, this, 0, m_pLayoutMgr->GetPaintWindow(), &rc);

				if (!m_bWindowless && m_spInPlaceObject.p)
				{
					HWND hwnd = NULL;
					m_spInPlaceObject->GetWindow(&hwnd);
					if (::IsWindow(hwnd))
						::SetFocus(hwnd);
				}
			}
		}
		else if (e == DUIET_killfocus)
		{
			m_bHaveFocus = FALSE;
			if (m_spInPlaceObject.p)
				m_spInPlaceObject->UIDeactivate();

			if (!m_bWindowless && m_spInPlaceObject.p)
			{
				HWND hwnd = NULL;
				m_spInPlaceObject->GetWindow(&hwnd);
				if (::IsWindow(hwnd))
					::SetFocus(m_pLayoutMgr->GetPaintWindow());
			}
		}
		else if (e == DUIET_size)
		{
			//if ( m_spOleObject.p == NULL ) return TRUE;
			ResetSurface();

			CRect brc = GetPos();

			SIZEL hmSize = { 0 };
			SIZEL pxSize = { 0 };
			pxSize.cx = brc.Width(); // m_rcItem.right - m_rcItem.left;
			pxSize.cy = brc.Height(); // m_rcItem.bottom - m_rcItem.top;
			AtlPixelToHiMetric(&pxSize, &hmSize);
			if (m_spOleObject.p)
				m_spOleObject->SetExtent(DVASPECT_CONTENT, &hmSize);

			if ( m_spInPlaceObject.p )
			{
				CRect rcItem = brc;
				RECT rcClip = GetClip();
				m_spInPlaceObject->SetObjectRects(&rcItem, &rcClip);
			}
		}
	}
	return FALSE;
}

void CDuiActiveX::OnScroll(HANDLE, HDE pScroll, BOOL bVert/*=TRUE*/)
{
	if ( m_spInPlaceObject.p )
	{
		RECT rcItem = GetPos();
		RECT rcClip = GetClip();

		//HWND hwnd = NULL;
		//if (!m_bWindowless && m_spInPlaceObject.p)
		//	m_spInPlaceObject->GetWindow(&hwnd);

		//if (hwnd && ::IsWindowVisible(hwnd) && ::IsRectEmpty(&rcClip))
		//	::ShowWindow(hwnd, SW_HIDE);

		m_spInPlaceObject->SetObjectRects(&rcItem, &rcClip);

		//if (hwnd && !::IsWindowVisible(hwnd) && !::IsRectEmpty(&rcClip))
		//	::ShowWindow(hwnd, SW_SHOW);

		//if (m_spInPlaceObject.p)
		//{
		//	if (::IsRectEmpty(&rcClip))
		//		m_spInPlaceObject->UIDeactivate();
		//}
	}
}

void CDuiActiveX::OnPaintContent( HDE hde, HDC hdc, RECT rcPaint, BOOL bDraw, InDrawData* pidd )
{
	InDrawData& idd = *pidd;
	// try activate control
	if (bDraw && m_spOleObject.p && !m_bInPlaceActive)
		ActivateAx();

	if ( m_spViewObject.p && m_bWindowless && bDraw)
	{
		RECT rc = GetPos();
		RECT rcClip = GetClip(FALSE);
		if (!::IsRectEmpty(&rcClip))
		{
#ifndef NO3D
			if (m_pOwner->m_pSurface)
			{
				HRESULT hr = m_spViewObject->Draw(m_dwAspect/*DVASPECT_CONTENT*/, -1, NULL, NULL, NULL, hdc, (RECTL*) &rc, (RECTL*) &rcClip, NULL, NULL); 
			}
			else
#endif
			{
				CRenderContentOrg org(hdc, NULL, m_pOwner);
				//POINT pt1;
				//::SetWindowOrgEx(hdc, 0, 0, &pt1);
				//CRenderClip clip(hdc, rcClip);
				HDC hdc2 = _memdc; //m_pSurface->GetDC();
				POINT ptOld;
				::SetWindowOrgEx(hdc2, rc.left, rc.top, &ptOld);
				HRESULT hr = m_spViewObject->Draw(m_dwAspect, -1, NULL, NULL, NULL, hdc2, (RECTL*) &rc, (RECTL*) &rcClip, NULL, NULL); 
				::BitBlt(hdc, rcClip.left, rcClip.top, rcClip.right-rcClip.left, rcClip.bottom-rcClip.top, hdc2, rcClip.left, rcClip.top, SRCCOPY);
				::SetWindowOrgEx(hdc2, ptOld.x, ptOld.y, NULL);
				//::SetWindowOrgEx(hdc, pt1.x, pt1.y, NULL);
				//m_pSurface->ReleaseDC(hdc2);
				//m_pSurface->BltToPrimary(&rcClip);
			}
		}

		m_pOwner->UpdateDrawData(idd);
	}
#if 0
	else if (bDraw && !m_bWindowless && m_spInPlaceObject.p && m_pOwner->m_pSurface && m_pOwner->m_pSurface->GetDevice()->IsLayeredWindow())
	{
		HWND hwnd = NULL;
		m_spInPlaceObject->GetWindow(&hwnd);
		if (::IsWindow(hwnd))
		{
			::SetWindowOrgEx(hdc, 0, 0, NULL);
			//BOOL bComp = FALSE;
			//HRESULT hr = ::DwmIsCompositionEnabled(&bComp);
			//if (SUCCEEDED(hr) && bComp)
			//	::DwmEnableComposition(DWM_EC_DISABLECOMPOSITION);
			extern BOOL gbDisableComposition;
			gbDisableComposition = TRUE;
			::SendMessage(hwnd, WM_PRINT, (WPARAM)hdc, (LPARAM)(PRF_CHECKVISIBLE|PRF_NONCLIENT|PRF_CLIENT|PRF_ERASEBKGND|PRF_CHILDREN|PRF_OWNED));
			gbDisableComposition = FALSE;
		}
	}
#endif // _DEBUG
	else if (!bDraw)
	{
		if (!::IsRectEmpty(&__rcNeeded))
		{
			// 如果控件自身在改变位置，这里提供估算值
			m_pOwner->UpdateDrawData(idd);
			if (idd.pt.x > idd.rc.left + idd.lLineIndent && (idd.rc.right-idd.pt.x < __rcNeeded.right - __rcNeeded.left))
			{
				idd.pt.x = idd.rc.left + idd.lLineIndent;
				idd.pt.y += idd.cyMinHeight + idd.lLineDistance;
				idd.cyTotalHeight += idd.cyMinHeight + idd.lLineDistance;
				idd.cyMinHeight = idd.cyCurLine;
			}
			idd.cyMinHeight = max(idd.cyMinHeight, __rcNeeded.bottom-__rcNeeded.top);
			idd.pt.x += __rcNeeded.right - __rcNeeded.left;
		}
		else
			__super::OnPaintContent(hde, hdc, rcPaint, bDraw, pidd);
	}
}

void CDuiActiveX::SetAttribute(HDE hde, LPCWSTR szName, VARIANT* value)
{
	if (lstrcmpiW(szName, L"clsid") == 0 || lstrcmpiW(szName, L"classid") == 0) szName = L"progid";
	__super::SetAttribute(hde, szName, value);

	CComVariant v = *value;
	if ( (lstrcmpiW(szName, L"progid") == 0) &&
		SUCCEEDED(v.ChangeType(VT_BSTR) && v.bstrVal && m_clsid==GUID_NULL) )
	{
		if( v.bstrVal[0] == L'{' ) ::CLSIDFromString((LPOLESTR)v.bstrVal, &m_clsid);
		else ::CLSIDFromProgID(v.bstrVal, &m_clsid);
	}
}

//LRESULT CDuiActiveX::MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
//{
//	if( m_spOleObject.p == NULL || !m_bWindowless ) return 0;
//
//	_assert(m_bWindowless);
//	//if( !m_pControl->m_bInPlaceActive ) return 0;
//	if( m_spInPlaceObject.p == NULL ) return 0;
//	BOOL bWasHandled = TRUE;
//	if( (uMsg >= WM_MOUSEFIRST && uMsg <= WM_MOUSELAST) || uMsg == WM_SETCURSOR )
//	{
//		// Mouse message only go when captured or inside rect
//		DWORD dwHitResult = m_bCaptured ? HITRESULT_HIT : HITRESULT_OUTSIDE;
//		if( dwHitResult == HITRESULT_OUTSIDE )
//		{
//			CComQIPtr<IViewObjectEx> spViewEx = m_spOleObject;
//			if( spViewEx )
//			{
//				 POINT ptMouse = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
//				 spViewEx->QueryHitPoint(DVASPECT_CONTENT, &rt(this)->GetPos(), ptMouse, 0, &dwHitResult);
//			}
//		}
//		if( dwHitResult != HITRESULT_HIT ) return 0;
//		if( uMsg == WM_SETCURSOR ) bWasHandled = FALSE;
//	}
//	else if( uMsg >= WM_KEYFIRST && uMsg <= WM_KEYLAST )
//	{
//		// Keyboard messages just go when we have focus
//		if( !m_bFocused ) return 0;
//	}
//	else
//	{
//		switch( uMsg )
//		{
//		case WM_HELP:
//		case WM_CONTEXTMENU:
//			bWasHandled = FALSE;
//			break;
//		default:
//			return 0;
//		}
//	}
//	LRESULT lResult = 0;
//	HRESULT Hr = m_spInPlaceObject->OnWindowMessage(uMsg, wParam, lParam, &lResult);
//	if( Hr == S_OK ) bHandled = bWasHandled;
//	return lResult;
//}

void CDuiActiveX::ReleaseControl()
{
	//m_sink.m_events.RemoveAll();
	// now detach ActiveX object from control peer.
	if (m_bReleaseAll) return;

	{
		CComQIPtr<IPeerHolder> peer(m_pOwner->GetObject());
		if (peer.p)
		{
			CComQIPtr<IDispatch> obj(m_spOleObject);
			if (obj)
				peer->DetachObject(obj);

			peer->DetachObject(GetDispatch());
		}
		// peer 应该在这里析构
	}

	m_sink.Disconnect();

	ReleaseAll();

	//m_pLayoutMgr->RemoveMessageFilter(this);
}

void CDuiActiveX::OnConnect()
{
	Init();
}

//void CDuiActiveX::OnControlEvent( LPCOLESTR szEventName, DISPPARAMS* dp, VARIANT* pResult )
//{
//}
//
//bool CDuiActiveX::onEvent( BSTR szEventName, VARIANT callback )
//{
//	if (szEventName==NULL)
//		return false;
//
//	CComVariant v(callback);
//	CDuiActiveXEvent::EventItem* item = m_sink._ItemByName(szEventName);
//	if (v.vt != VT_DISPATCH && item) return item->callback = (IDispatch*)NULL, true;
//	return item ? (item->callback=v.pdispVal, true) : false;
//}

HRESULT CDuiActiveX::Getobject(VARIANT* pVar)
{
	_assert(pVar);
	CComQIPtr<IDispatch> spDisp(m_spOleObject);
	if (spDisp.p)
	{
		pVar->vt = VT_DISPATCH;
		pVar->pdispVal = spDisp.Detach();
		return S_OK;
	}
	return E_FAIL;
}

void CDuiActiveX::On3DSurfaceCreated( HANDLE )
{
	BindSurfaceWindow();
}

void CDuiActiveX::BindSurfaceWindow()
{
#ifndef NO3D
	if (m_pOwner->m_pSurface && !m_bWindowless && m_spInPlaceObject.p)
	{
		HWND hwnd = NULL;
		m_spInPlaceObject->GetWindow(&hwnd);
		if (::IsWindow(hwnd))
			m_pOwner->m_pSurface->BindWindow(hwnd, &m_pOwner->_attributes);
	}
#endif
}

//HRESULT CDuiActiveX::GeteventHandler( VARIANT* pVar )
//{
//	CComVariant v = m_sink.eventHandler.p;
//	return v.Detach(pVar);
//}
//
//HRESULT CDuiActiveX::PuteventHandler( VARIANT* pVar )
//{
//	if (pVar==NULL || V_VT(pVar)!=VT_DISPATCH) return E_INVALIDARG;
//	m_sink.eventHandler = V_DISPATCH(pVar);
//	return S_OK;
//}

//////////////////////////////////////////////////////////////////////////
// CDuiActiveXEvent

CDuiActiveXEvent::CDuiActiveXEvent( CDuiActiveX* pOwner ) : m_pOwner(pOwner), m_iidEvent(IID_NULL), m_dwCookie(-1)
{
	_assert(m_pOwner);
}

CDuiActiveXEvent::~CDuiActiveXEvent()
{
	Disconnect();
}

HRESULT CDuiActiveXEvent::GetEventIID( IUnknown* pUnk, IID* piid )
{
	HRESULT hr = E_FAIL;
	if (pUnk==NULL) return E_INVALIDARG;
	if (piid==NULL) return E_POINTER;
	*piid = IID_NULL;

	// 1. 尝试 IProvideClassInfo2
	CComQIPtr<IProvideClassInfo2> spPci(pUnk);
	if (spPci.p && SUCCEEDED(spPci->GetGUID(GUIDKIND_DEFAULT_SOURCE_DISP_IID, piid)))
	{
		_assert(*piid != IID_NULL);
		return S_OK;
	}

	// 2. IProvideClassInfo
	CComPtr<ITypeInfo> spTypeInfo;
	if (spPci.p == NULL)
		hr = pUnk->QueryInterface(IID_IProvideClassInfo, (void**)&spPci.p);
	if (spPci.p)
		hr = spPci->GetClassInfo(&spTypeInfo);

	// 3. IDispatch
	if (spTypeInfo.p == NULL)
	{
		CComQIPtr<IDispatch> spDisp(pUnk);
		if (spDisp.p)
			hr = spDisp->GetTypeInfo(0, 0, &spTypeInfo);
	}

	if (spTypeInfo.p)
	{
		CComPtr<ITypeLib> spTypeLib;
		hr = spTypeInfo->GetContainingTypeLib(&spTypeLib, 0);
		if (SUCCEEDED(hr))
		{
			// 首先找到接口对应的CLSID，其实不需要找，直接用 CDuiActiveX::m_clsid 即可，这里作为验证
			CLSID clsid = CLSID_NULL;
			CComQIPtr<IPersist> spPersist(pUnk);
			if (spPersist.p)
			{
				hr = spPersist->GetClassID(&clsid);
				if (SUCCEEDED(hr))
				{
					_assert(clsid!=CLSID_NULL);
					_assert(clsid==m_pOwner->m_clsid);
				}
			}
			if (clsid==CLSID_NULL)
				clsid = m_pOwner->m_clsid;

			CComPtr<ITypeInfo> tiClass;
			hr = spTypeLib->GetTypeInfoOfGuid(clsid, &tiClass);
			if (SUCCEEDED(hr))
			{
				TYPEATTR* attr=NULL;
				hr = tiClass->GetTypeAttr(&attr);
				if (SUCCEEDED(hr))
				{
					for (WORD j=0; j<attr->cImplTypes; j++)
					{
						int nType;
						hr = tiClass->GetImplTypeFlags(j, &nType);
						if (SUCCEEDED(hr) && nType == (IMPLTYPEFLAG_FDEFAULT | IMPLTYPEFLAG_FSOURCE))
						{
							// found!!
							HREFTYPE hRef;
							hr = tiClass->GetRefTypeOfImplType(j, &hRef);
							if (SUCCEEDED(hr))
							{
								CComPtr<ITypeInfo> ti2;
								hr = tiClass->GetRefTypeInfo(hRef, &ti2);
								if (SUCCEEDED(hr))
								{
									TYPEATTR* pAttrIF;
									hr = ti2->GetTypeAttr(&pAttrIF);
									if (pAttrIF != NULL)
									{
										Checked::memcpy_s(piid, sizeof(GUID), &pAttrIF->guid, sizeof(GUID));
										ti2->ReleaseTypeAttr(pAttrIF);
									}
								}
							}
							break;
						}
					}
					tiClass->ReleaseTypeAttr(attr);
				}
			}
		}
	}

	// 4. IConnectionPoint
	if (FAILED(hr))
	{
		CComQIPtr<IConnectionPointContainer> cpc = pUnk;
		if (cpc)
		{
			CComPtr<IEnumConnectionPoints> ecp;
			hr = cpc->EnumConnectionPoints(&ecp);
			if (SUCCEEDED(hr))
			{
				CComPtr<IConnectionPoint> cp;
				hr = ecp->Next(1, &cp, NULL);
				if (SUCCEEDED(hr))
				{
					return cp->GetConnectionInterface(piid);
				}
			}
		}
	}
	return hr;
}

void CDuiActiveXEvent::_CacheEventItems()
{
	if (m_iidEvent == IID_NULL && m_pOwner->m_spOleObject.p==NULL) return;

	HRESULT hr = E_FAIL;
	CComQIPtr<IProvideClassInfo> spPci(m_pOwner->m_spOleObject);
	CComPtr<ITypeInfo> spTypeInfo;
	if (spPci.p)
		hr = spPci->GetClassInfo(&spTypeInfo);

	if (spTypeInfo.p == NULL)
	{
		CComQIPtr<IDispatch> spDisp(m_pOwner->m_spOleObject);
		if (spDisp.p)
			hr = spDisp->GetTypeInfo(0, 0, &spTypeInfo);
	}

	if (SUCCEEDED(hr))
	{
		CComPtr<ITypeLib> spTypeLib;
		hr = spTypeInfo->GetContainingTypeLib(&spTypeLib, 0);
		if (SUCCEEDED(hr))
		{
			CComPtr<ITypeInfo> ti;
			hr = spTypeLib->GetTypeInfoOfGuid(m_iidEvent, &ti);
			if (SUCCEEDED(hr))
			{
				TYPEATTR* attr=NULL;
				hr = ti->GetTypeAttr(&attr);
				if (SUCCEEDED(hr))
				{
					for (WORD i=0; i<attr->cFuncs; i++)
					{
						FUNCDESC* fd =NULL;
						ti->GetFuncDesc(i, &fd);
						if (fd)
						{
							CComBSTR name;
							ti->GetDocumentation(fd->memid, &name, NULL, NULL, NULL);
							EventItem ei = {fd->memid, name};
							m_events.Add(ei);
							ti->ReleaseFuncDesc(fd);
							fd = NULL;
						}
					}
					ti->ReleaseTypeAttr(attr);
				}
			}
		}
	}
}

CDuiActiveXEvent::EventItem* CDuiActiveXEvent::_ItemByDispid(DISPID dispid)
{
	if (dispid==DISPID_UNKNOWN) return NULL;

	for (int i=0, num=m_events.GetSize(); i<num; i++)
	{
		if (m_events[i].dispid == dispid)
			return &m_events[i];
	}
	return NULL;
}

CDuiActiveXEvent::EventItem* CDuiActiveXEvent::_ItemByName(LPCOLESTR name)
{
	if (name==NULL) return NULL;

	for (int i=0, num=m_events.GetSize(); i<num; i++)
	{
		if (lstrcmpiW(name, m_events[i].name) == 0)
			return &m_events[i];
	}
	return NULL;
}

LPCOLESTR CDuiActiveXEvent::GetNameByDispid( DISPID dispid )
{
	EventItem* item = _ItemByDispid(dispid);
	return item ? item->name.m_str : NULL;
}

DISPID CDuiActiveXEvent::GetDispidByName( LPCOLESTR name )
{
	EventItem* item = _ItemByName(name);
	return item ? item->dispid : DISPID_UNKNOWN;
}

BOOL CDuiActiveXEvent::Connect()
{
	if (m_pOwner==NULL || m_pOwner->m_spOleObject.p==NULL)
		return FALSE;

	if (m_dwCookie != -1) return TRUE;

	HRESULT hr = E_FAIL;
	if (m_iidEvent==IID_NULL && FAILED(GetEventIID(m_pOwner->m_spOleObject.p, &m_iidEvent)))
		return FALSE;

	_CacheEventItems();
	return SUCCEEDED(AtlAdvise(m_pOwner->m_spOleObject.p, (IDispatch*)this, m_iidEvent, &m_dwCookie));
}

void CDuiActiveXEvent::Disconnect()
{
	if (m_pOwner==NULL || m_pOwner->m_spOleObject.p==NULL) return;
	if (m_dwCookie == -1) return;
	AtlUnadvise(m_pOwner->m_spOleObject.p, m_iidEvent, m_dwCookie);
	m_dwCookie = -1;
	m_events.RemoveAll();
	//eventHandler = (IDispatch*)NULL;
}

STDMETHODIMP CDuiActiveXEvent::Invoke( DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS * pdispparams, VARIANT *pVarResult, EXCEPINFO * pExcepInfo, UINT * puArgErr )
{
	DuiNode* r = m_pOwner->m_pOwner;
	LPCOLESTR name = GetNameByDispid(dispIdMember);
	if (name)
	{
		//ATLTRACE(L"ACTIVEX EVENT - %s [id:%s, clsid:%s]\n", name, r->GetAttributeString(L"id"), r->GetAttributeString(L"progid"));
	}
	DuiEvent ev = {name};
	ev.cancelBubble = TRUE;
	ev.wParam = (WPARAM)pdispparams;
	r->m_pLayoutMgr->FireEvent(r, ev);

	//EventItem* item = _ItemByDispid(dispIdMember);
	//if (item)
	//{
	//	IDispatch* cb = item->callback.p;
	//	if (cb == NULL && eventHandler.p) // 如果没有单独设置回调，则寻找eventHandler的同名回调属性
	//	{
	//		CComVariant v;
	//		if (SUCCEEDED(eventHandler.GetPropertyByName(name, &v)) && v.vt==VT_DISPATCH)
	//			cb = v.pdispVal;
	//	}

	//	if (cb)
	//	{
	//		VARIANT* vs = NEW VARIANT[pdispparams->cArgs + 1];
	//		if (vs == NULL) return E_OUTOFMEMORY;

	//		vs[0].vt = VT_DISPATCH;
	//		vs[0].pdispVal = rt(m_pOwner)->GetObject();
	//		memcpy_s(&vs[1], sizeof(VARIANT)*pdispparams->cArgs, pdispparams->rgvarg, sizeof(VARIANT)*pdispparams->cArgs);

	//		DISPID didThis = DISPID_THIS;
	//		DISPPARAMS dp = {vs, &didThis, pdispparams->cArgs+1, 1};
	//		HRESULT hr = cb->Invoke(0, IID_NULL, LOCALE_SYSTEM_DEFAULT, DISPATCH_METHOD, &dp, pVarResult, NULL, NULL);
	//		delete[] vs;
	//	}
	//}
	return S_OK;
}