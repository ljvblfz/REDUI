#if !defined(__DUIBASE_H__)
#define __DUIBASE_H__

#pragma once

#include <atlstr.h>
#include "../../common/macro.h"
#include "../../common/mimefilter.h"

#ifndef NEW
#define NEW new
#endif // NEW

/////////////////////////////////////////////////////////////////////////////////////
//

class CDuiLayoutManager;


#define DUI_WNDSTYLE_CONTAINER  (0)
#define DUI_WNDSTYLE_FRAME      (WS_VISIBLE | WS_OVERLAPPEDWINDOW)
#define DUI_WNDSTYLE_CHILD      (WS_VISIBLE | WS_CHILD | WS_CLIPSIBLINGS | WS_CLIPCHILDREN)
#define DUI_WNDSTYLE_DIALOG     (WS_VISIBLE | WS_POPUPWINDOW | WS_CAPTION | WS_DLGFRAME | WS_CLIPSIBLINGS | WS_CLIPCHILDREN)

#define DUI_WNDSTYLE_EX_FRAME   (WS_EX_WINDOWEDGE)
#define DUI_WNDSTYLE_EX_DIALOG  (WS_EX_TOOLWINDOW | WS_EX_DLGMODALFRAME)

#define DUI_CLASSSTYLE_CONTAINER  (0)
#define DUI_CLASSSTYLE_FRAME      (CS_VREDRAW | CS_HREDRAW)
#define DUI_CLASSSTYLE_CHILD      (CS_VREDRAW | CS_HREDRAW | CS_DBLCLKS | CS_SAVEBITS)
#define DUI_CLASSSTYLE_DIALOG     (CS_VREDRAW | CS_HREDRAW | CS_DBLCLKS | CS_SAVEBITS)


/////////////////////////////////////////////////////////////////////////////////////
//

#define ASSERT(expr)  ATLASSERT(expr)

#ifdef _DEBUG
#define TRACE ATLTRACE //__Trace
//#define TRACEMSG __TraceMsg
#else
#define TRACE
//#define TRACEMSG _T("")
#endif

//void  __Trace(LPCTSTR pstrFormat, ...);
//LPCTSTR __TraceMsg(UINT uMsg);



/////////////////////////////////////////////////////////////////////////////////////
//

//typedef CSimpleArray<LPVOID> CStdPtrArray;
//class CStdPtrArray
//{
//public:
//	CStdPtrArray(){}
//
//protected:
//};

//#define OnConnectorHit(client,server,protocol,...) \
//{ \
//struct scb : public CDuiConnectorCallback \
//{ \
//public: \
//	BOOL OnHit(LPVOID pKey, LPVOID pVal, DWORD protocol) \
//	{ \
//		{ __VA_ARGS__; } \
//		return FALSE; \
//	} \
//}; \
//static scb cb; \
//gConnector.Connect(client, server, protocol, &cb); \
//}


/////////////////////////////////////////////////////////////////////////////////////
//

class  CStdValArray
{
public:
	CStdValArray(int iElementSize, int iPreallocSize = 0);
	virtual ~CStdValArray();

	BOOL IsEmpty() const;
	BOOL Insert(int iIndex, LPVOID pData);
	BOOL Add(LPCVOID pData);
	BOOL Remove(LPVOID pData);
	BOOL RemoveAt(int iIndex);
	void RemoveAll();
	int GetSize() const;
	LPVOID GetData();

	LPVOID GetAt(int iIndex) const;
	LPVOID operator[] (int nIndex) const;

protected:
	LPBYTE m_pVoid;
	int m_iElementSize;
	int m_nCount;
	int m_nAllocated;
};

class CStdPtrArray
{
public:
	CStdPtrArray(int iPreallocSize=0);
	virtual ~CStdPtrArray();

	BOOL IsEmpty() const;
	BOOL Insert(int iIndex, LPVOID pData);
	BOOL Add(LPCVOID pData);
	BOOL Remove(LPVOID pData);
	BOOL RemoveAt(int iIndex);
	void RemoveAll();
	int GetSize() const;
	LPVOID* GetData();
	int Find(const LPVOID& pData);

	BOOL Swap(LPVOID d1, LPVOID d2);
	BOOL MoveTo(LPVOID pData, int iIndex);

	LPVOID GetAt(int iIndex) const;
	LPVOID operator[] (int nIndex) const;

protected:
	LPVOID* m_ppVoid;
	int m_nCount;
	int m_nAllocated;
};

typedef struct tagAutomationCollectionItem
{
	CComBSTR name;
	CComPtr<IDispatch> disp;
}AutomationCollectionItem;

class CAutomationCollection //: public CStdPtrArray
{
public:
	CStdPtrArray m_coll;

	Begin_Auto_Disp_Map(CAutomationCollection)
		Disp_PropertyGet(1, length)
		Disp_ValueGet(Item)
	End_Disp_Map()

	~CAutomationCollection();

	HRESULT InitNamedItem(DISPPARAMS* params) { return S_OK; }; // 这个方法不起作用，但是避免编译警告

	BOOL Add(LPCOLESTR name, IDispatch* disp);
	void RemoveAll();

	HRESULT Getlength(VARIANT* pVar);
	HRESULT GetItem(VARIANT vIndex, VARIANT* pVar);
};

//////////////////////////////////////////////////////////////////////////
// 连接器，关联任何两个类型的指针，可附带一个标志条件，多对多。例如多个控件与一个资源关联，可枚举

// 预置协议
#define cp_img_loaded 0x01000000
#define cp_img_loaded_foreground (cp_img_loaded+1)

#define cp_delayed_update_layout 0x02000000
#define cp_style_bind 0x04000000

class CDuiConnectorCallback
{
public:
	virtual BOOL OnHit(LPVOID pClient, LPVOID pServer, DWORD_PTR protocol) = 0; // 如果返回 TRUE 表示遍历操作应该停止
};

class CDuiConnector
{
	struct _ConnectorItem
	{
		LPVOID client; // 这里仅表示控件指针
		LPVOID server; // 任意类型指针
		DWORD_PTR protocol; // 关联标志，0 表示忽略此标志
		CDuiConnectorCallback* cb;
	};
	CSimpleArray<_ConnectorItem> records;

	CRITICAL_SECTION cs;
	class CSLocker
	{
		CRITICAL_SECTION& _cs;
	public:
		CSLocker(CRITICAL_SECTION& cs) : _cs(cs) { ::EnterCriticalSection(&_cs); }
		~CSLocker() { ::LeaveCriticalSection(&_cs); }
	};

	int Find(LPVOID pClient, LPVOID pServer=NULL, DWORD_PTR protocol=0);


public:
	CDuiConnector() { ::InitializeCriticalSection(&cs); }
	~CDuiConnector() { ::DeleteCriticalSection(&cs); }

	void Connect(LPVOID pClient, LPVOID pServer=NULL, DWORD_PTR protocol=0, CDuiConnectorCallback* cb=NULL);

	void Disconnect(LPVOID pClient, LPVOID pServer=NULL, DWORD_PTR protocol=0);

	void Fire(LPVOID pClient, LPVOID pServer=NULL, DWORD_PTR protocol=0, CDuiConnectorCallback* cb=NULL);
};

extern CDuiConnector gConnector;

/////////////////////////////////////////////////////////////////////////////////////
//
#define WM_CHILDPAINTED	(WM_APP+23)
//#define WM_CHILDCARET	(WM_APP+24)
#define WM_CHILDSIZED	(WM_APP+25)

class  CDuiWindowBase
{
public:
	CDuiWindowBase();

	HWND GetHWND() const;
	operator HWND() const;

	BOOL RegisterWindowClass(UINT style, LPCTSTR szClassName);
	BOOL RegisterSuperclass(LPCTSTR szSuperClassName, LPCTSTR szWindowClassName);

	HWND Create(HWND hwndParent, LPCTSTR pstrName, DWORD dwStyle, DWORD dwExStyle, const RECT rc, HMENU hMenu = NULL, HINSTANCE hInstance=NULL);
	HWND Create(HWND hwndParent, LPCTSTR pstrName, DWORD dwStyle, DWORD dwExStyle, int x = CW_USEDEFAULT, int y = CW_USEDEFAULT, int cx = CW_USEDEFAULT, int cy = CW_USEDEFAULT, HMENU hMenu = NULL, HINSTANCE hInstance=NULL);
	HWND Subclass(HWND hWnd);
	void Unsubclass();
	void ShowWindow(BOOL bShow = TRUE, BOOL bTakeFocus = TRUE);
	BOOL ShowModal();
	void Close();
	void CenterWindow();
	void DrawToDC32(HDC hdc, RECT rc, BOOL bUpdateAlpha=TRUE);

	void HookToDC();

	static CDuiWindowBase* FromHWND(HWND hwnd, BOOL bAutoCreate=TRUE);

protected:
	virtual LPCTSTR GetWindowClassName() const; // = 0;
	virtual LPCTSTR GetSuperClassName() const;
	virtual UINT GetClassStyle() const;

	LRESULT SendMessage(UINT uMsg, WPARAM wParam = 0, LPARAM lParam = 0L);
	LRESULT PostMessage(UINT uMsg, WPARAM wParam = 0, LPARAM lParam = 0L);
	void ResizeClient(int cx = -1, int cy = -1);

	virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
	virtual void OnFinalMessage(HWND hWnd);

	static LRESULT CALLBACK __WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	static LRESULT CALLBACK __ControlProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	static CDuiWindowBase* __GetOwnerParent(HWND hWnd);

	void UpdateSize();
	void UpdateDib();
	void UpdateCaret(BOOL bForce=FALSE);
	void UpdateAlpha();
	void ReleaseDib32();

protected:
	HWND m_hWnd;
	WNDPROC m_OldWndProc;
	BOOL m_bSubclassed;
	BOOL m_bAutoDestroy;

	HDC m_hdc32;
	HBITMAP m_hdib;
	HBITMAP m_hbmpOld;
	LPDWORD m_pBits;
	BYTE m_bAlpha;
	BOOL m_bDisableNotify;
};


/////////////////////////////////////////////////////////////////////////////////////
//
typedef CStringW CStdString;


//////////////////////////////////////////////////////////////////////////
//
class CLayerHelper
{
public:
	static POINT GetClientOffset(HWND hwnd);
	static POINT OffsetPointInc(HWND hwnd, LPPOINT ppt);
	static POINT OffsetPointDec(HWND hwnd, LPPOINT ppt);
};

//////////////////////////////////////////////////////////////////////////
// LineLog 用于栈变量，用来输出一行调试信息，中间可以输出多次信息，析构时自动添加换行符
class LineLog
{
	LPVOID _data;
public:
	LineLog() {}
	LineLog(LPCSTR szFormat, ...);
	LineLog(LPCWSTR szFormat, ...);
	~LineLog();

	void outputV(LPCSTR szFormat, va_list vars);
	void outputV(LPCWSTR szFormat, va_list vars);

	void operator()(LPCSTR szFormat, ...);
	void operator()(LPCWSTR szFormat, ...);
};

//////////////////////////////////////////////////////////////////////////
// TimeCounter 用于统计一段时间，单位毫秒
// 
class TimeCounter
{
	LineLog _ll;
	DWORD _dwStart;
public:
	TimeCounter(LPCWSTR szPrompt=NULL);
	~TimeCounter();

	void operator()(LPCWSTR szFormat, ...);
};

//////////////////////////////////////////////////////////////////////////
template<typename T, typename cacheT>
class tCache
{
public:
	typedef tCache<T, cacheT> thisClass;
	T value;
	BOOL bValid;
	cacheT cache;

	tCache() : bValid(FALSE) {}
	tCache(const T& other) : bValid(FALSE), value(other) {}

	thisClass& operator=(const T& other) { value(other); bValid=FALSE; return *this; }

	operator T() { return value; }
	//operator T*() { return &value; }
	void Invalidate() { bValid = FALSE; }
	BOOL IsValid() const { return bValid; }
};

//////////////////////////////////////////////////////////////////////////
// Atom
// 
enum AtomTable
{
	atom_layout = 2010,
	atom_device,
	atom_child_window,
};

class AtomString
{
	UINT _at;
public:
	AtomString(UINT at) : _at(at) {}
	operator LPCWSTR() { return MAKEINTATOM(_at); }
};

//////////////////////////////////////////////////////////////////////////
// 实现类都从此类派生，实现简单单向链表，请注意，如果一个类要派生多个列表，应该用tIndex区分索引值
// 
template<class T, int Cookie>
class TListNext
{
public:
	T* T::* ref_next;
	TListNext(T* T::* next=0) : ref_next(next)
	{
#ifdef _DEBUG
		if (ref_next==0)
			DebugBreak();
#endif // _DEBUG
	}
	T*& Next() { return ((T*)this)->*ref_next; }
	const T* Next() const { return ((T*)this)->*ref_next; }
};

template<class T>
class TListNext<T, 0> // 索引为0表示此列表使用内置的next成员，其它使用T类的成员
{
public:
	T* next;
	TListNext(T* T::* dummy=0) : next(NULL) {}
	T*& Next() { return next; }
	const T* Next() const { return next; }
};

//////////////////////////////////////////////////////////////////////////
template<typename T, int Cookie=0>
class TList : public TListNext<T, Cookie>
{
	T** __head;
public:
	TList(T* T::* refnext=NULL) : __head(NULL), TListNext<T, Cookie>(refnext) {}

	bool isHead(T** head) const { return head && (head==__head); } // 此态表示某个外部的指针是否是本链表的头

	bool isHead() // 此态表示this是不是本链表的头
	{
		return (__head && (*__head == (T*)this));
	}

	//bool isInList(T** head=NULL)
	//{
	//	if (head==NULL) head = __head;
	//	if (head==NULL) return false;

	//	T* pThis = (T*)this;
	//	T* p = *head;
	//	while (p && p!=pThis) p = p->TList<T, Cookie>::Next();
	//	return (p==pThis);
	//}

	void addToList(T** head, bool useRefCount=true, bool append=true)
	{
		if (head==NULL || head==__head) return;
		if (__head && __head!=head)
		{
			removeFromList(false, __head);
			useRefCount = false;
		}
		__head = head;

		T* pThis = (T*)this;
		if (append)
		{
			if (*__head == NULL)
				*__head = pThis;
			else
			{
				T* p = *__head;
				while (p->TList<T, Cookie>::Next()) p = p->TList<T, Cookie>::Next();
				p->TList<T, Cookie>::Next() = pThis;
			}
		}
		else
		{
			pThis->TList<T, Cookie>::Next() = *__head;
			*__head = pThis;
		}

		if (useRefCount)
		{
			__if_exists(T::GetDispatch) { pThis->GetDispatch()->AddRef(); }
			__if_not_exists(T::GetDispatch) {
				__if_exists(T::getDispatch) { pThis->getDispatch()->AddRef(); }
			}
		}
	}

	void removeFromList(bool useRefCount=true, T** head=NULL)
	{
		if (head==NULL) head = __head;
		if (head==NULL) return;

		T* pThis = (T*)this;
		if (*head == pThis)
			*head = pThis->TList<T, Cookie>::Next();
		else
		{
			T* p = *head;
			while (p && p->TList<T, Cookie>::Next()!=pThis)
				p = p->TList<T, Cookie>::Next();
			if (p)
				p->TList<T, Cookie>::Next() = pThis->TList<T, Cookie>::Next();
		}

		pThis->TList<T, Cookie>::Next() = NULL;
		if (head==__head) __head = NULL;

		if (useRefCount)
		{
			__if_exists(T::GetDispatch) { pThis->GetDispatch()->Release(); }
			__if_not_exists(T::GetDispatch)
			{
				__if_exists(T::getDispatch) { pThis->getDispatch()->Release(); }
				//__if_not_exists(T::getDispatch) {
				//	__if_exists(T::Dispose) { pThis->Dispose(); }
				//}
			}
		}
	}

	// 派生类应该实现 Dispose 方法来释放自己
	void removeAll(bool deleteThis=false)
	{
		if (Next()) Next()->TList<T, Cookie>::removeAll(deleteThis);
		Next() = NULL;

		removeFromList();
		if (deleteThis) delete (T*)this;
	}

	DWORD count()
	{
		DWORD num = 1;
		const T* p = Next();
		while (p)
		{
			num++;
			p = p->TList<T, Cookie>::Next();
		}
		return num;
	}

	const T* getAt(DWORD index)
	{
		if (index==0) return (T*)this;
		const T* p = Next();
		while (--index>0 && p) p = p->TList<T, Cookie>::Next();
		return p;
	}

	// 获取自身在集合中的索引
	long getIndex()
	{
		if (__head==NULL) return -1;
		long l = 0;
		T* pThis = (T*)this;
		T* p = *__head;
		while (p && p!=pThis)
		{
			l++;
			p = p->TList<T, Cookie>::Next();
		}
		return p ? l : -1;
	}
};

//////////////////////////////////////////////////////////////////////////
// 简单单向链表，支持排序
// 

template<typename T, bool bAscend=true, int Cookie=0, typename ordertype=long>
class TOrderList : public TListNext<T, Cookie>
{
	T** __head;
	ordertype (T::* __order)();
public:

	TOrderList(ordertype (T::*fnGetOrder)() = 0, T* T::* refnext=NULL) : __head(NULL), TListNext<T, Cookie>(refnext), __order(fnGetOrder)/*, order(0)*/ {}

	bool isHead(T** head) const { return head && (head==__head); } // 此态表示某个外部的指针是否是本链表的头

	bool isHead() // 此态表示this是不是本链表的头
	{
		return (__head && (*__head == (T*)this));
	}

	//bool isInList(T** head=NULL)
	//{
	//	if (head==NULL) head = __head;
	//	if (head==NULL) return false;

	//	T* pThis = (T*)this;
	//	T* p = *head;
	//	while (p && p!=pThis) p = p->TOrderList<T, bAscend, Cookie, ordertype>::Next();
	//	return (p==pThis);
	//}

	void addToList(T** head, bool useRefCount=true)
	{
		if (head==NULL || head==__head) return;
		if (__head && __head!=head)
		{
			removeFromList(false, __head);
			useRefCount = false;
		}
		__head = head;

		T* pThis = (T*)this;
		if (*__head == NULL)
			*__head = pThis;
		else
		{
			T* p = *__head;
			while (p->TOrderList<T, bAscend, Cookie, ordertype>::Next())
			{
				if (__order &&
					((pThis->*__order)() - (p->*__order)()) * (ordertype)(bAscend?1:-1) >= 0 &&
					((pThis->*__order)() - (p->TOrderList<T, bAscend, Cookie, ordertype>::Next()->*__order)()) * (ordertype)(bAscend?1:-1) < 0)
				//if (TCompare<bAscend>::Subtract(order,p->order)>=0 && TCompare<bAscend>::Subtract(order,p->next->order)<0)
				{
					Next() = p->TOrderList<T, bAscend, Cookie, ordertype>::Next();
					//p->TOrderList<T, bAscend, Cookie, ordertype>::Next() = pThis;
					break;
				}
				p = p->TOrderList<T, bAscend, Cookie, ordertype>::Next();
			}
			p->TOrderList<T, bAscend, Cookie, ordertype>::Next() = pThis;
		}

		if (useRefCount)
		{
			__if_exists(T::GetDispatch) { pThis->GetDispatch()->AddRef(); }
			__if_not_exists(T::GetDispatch) {
				__if_exists(T::getDispatch) { pThis->getDispatch()->AddRef(); }
			}
		}
	}

	void reorder()
	{
		if (__head)
		{
			T** h = __head;
			removeFromList(false);
			addToList(h, false);
		}
	}

	void removeFromList(bool useRefCount=true, T** head=NULL)
	{
		if (head==NULL) head = __head;
		if (head==NULL) return;

		T* pThis = (T*)this;
		if (*head == pThis)
			*head = pThis->TOrderList<T, bAscend, Cookie, ordertype>::Next();
		else
		{
			T* p = *head;
			while (p && p->TOrderList<T, bAscend, Cookie, ordertype>::Next()!=pThis)
				p = p->TOrderList<T, bAscend, Cookie, ordertype>::Next();
			if (p)
				p->TOrderList<T, bAscend, Cookie, ordertype>::Next() = pThis->TOrderList<T, bAscend, Cookie, ordertype>::Next();
		}

		pThis->TOrderList<T, bAscend, Cookie, ordertype>::Next() = NULL;
		if (head==__head) __head = NULL;

		if (useRefCount)
		{
			__if_exists(T::GetDispatch) { pThis->GetDispatch()->Release(); }
			__if_not_exists(T::GetDispatch)
			{
				__if_exists(T::getDispatch) { pThis->getDispatch()->Release(); }
				//__if_not_exists(T::getDispatch) {
				//	__if_exists(T::Dispose) { pThis->Dispose(); }
				//}
			}
		}
	}

	// 派生类应该实现 Dispose 方法来释放自己
	void removeAll(bool deleteThis=false)
	{
		if (Next()) Next()->TOrderList<T, bAscend, Cookie, ordertype>::removeAll(deleteThis);
		Next() = NULL;

		removeFromList();
		if (deleteThis) delete (T*)this;
	}

	DWORD count()
	{
		DWORD num = 1;
		const T* p = Next();
		while (p)
		{
			num++;
			p = p->TOrderList<T, bAscend, Cookie, ordertype>::Next();
		}
		return num;
	}

	const T* getAt(DWORD index)
	{
		if (index==0) return (T*)this;
		const T* p = Next();
		while (--index>0 && p) p = p->TOrderList<T, bAscend, Cookie, ordertype>::Next();
		return p;
	}

	// 获取自身在集合中的索引
	long getIndex()
	{
		if (__head==NULL) return -1;
		long l = 0;
		T* pThis = (T*)this;
		T* p = *__head;
		while (p && p!=pThis)
		{
			l++;
			p = p->TOrderList<T, bAscend, Cookie, ordertype>::Next();
		}
		return p ? l : -1;
	}
};

// 实现类以此类包装，实现简单单向链表
template<typename T>
class TContainedList
{
	struct listItem
	{
		T* dest;
		listItem* next;

		listItem(T* t) : dest(t), next(NULL) {}
	};
	listItem* head;
public:

	TContainedList() : head(NULL) {}

	bool exist(T* t)
	{
		if (t==NULL) return false;
		for (listItem* p=head; p; p=p->next)
		{
			if (p->dest == t) return true;
		}
		return false;
	}

	bool add(T* t, bool append=true)
	{
		if (t==NULL || exist(t)) return true;

		listItem* tt = NEW listItem(t);
		if (tt==NULL) return false;

		if (append)
		{
			if (head==NULL)
				head = tt;
			else
			{
				listItem* p = head;
				while (p->next) p = p->next;
				p->next = tt;
			}
		}
		else
		{
			tt->next = head;
			head = tt;
		}
		return true;
	}

	void remove(T* t)
	{
		if (t==NULL || head==NULL) return;
		if (head->dest == t)
		{
			listItem* next = head->next;
			delete head;
			head = next;
		}
		else
		{
			listItem* p = head;
			while (p && p->next && p->next->dest!=t) p = p->next;
			if (p->next)
			{
				listItem* next = p->next->next;
				delete p->next;
				p->next = next;
			}
		}
	}
};

//////////////////////////////////////////////////////////////////////////
template<class T, bool bAutoLoad=true>
class UrlResources : public TList<T>
{
	typedef UrlResources<T,bAutoLoad> thisClass;
public:
	UrlResources(LPCOLESTR szUrl=NULL, LPCOLESTR szId=NULL, T** head=NULL) : id(szId), url(szUrl), isLoaded(false), download(_OnDownloadComplete, this)
	{
		if (head)
			addToList(head);

		if (bAutoLoad) LoadUrl(szUrl);
	}
	~UrlResources()
	{
		//if (Next()) delete Next();
		//if (isHead()) removeFromList(); // 仅链表头才调用remove，目的是把头指针清零
	}

	//bool IsLoaded() const { return isLoaded; }
	LPCOLESTR GetId() const { return id.GetLength()>0 ? (LPCOLESTR)id : NULL; }
	LPCOLESTR GetUrl() const { return url.GetLength()>0 ? (LPCOLESTR)url : NULL; }
	BOOL IsIdOrUrl(LPCOLESTR id_or_url) { return (id.CompareNoCase(id_or_url)==0 || url.CompareNoCase(id_or_url)==0); }
	BOOL LoadUrl(LPCOLESTR szUrl=NULL)
	{
		if (szUrl==NULL) szUrl = url;
		if (szUrl==NULL) return FALSE;
		url = szUrl;

		// 使用异步下载
		if (::PathIsURLW(url))
			return SUCCEEDED(download.Download(url));

		// 使用同步下载
		CComPtr<IStream> spStream;
		HRESULT hr = CDownload::LoadUrlToStream(url, &spStream, NULL);
		if (SUCCEEDED(hr))
			_OnDownloadComplete(this, spStream.p);
		return !!isLoaded;
	}

	bool isLoaded;

protected:
	CStdString id, url;

private:
	CDownload download;

	static void CALLBACK _OnDownloadComplete(LPVOID pData, LPSTREAM pStream)
	{
		thisClass* pThis = (thisClass*)pData;
		if (pThis==NULL || pStream==NULL) return;
		T* pT = (T*)pThis;
		pThis->isLoaded = true;

		__if_exists(T::OnLoadStream) { pT->OnLoadStream(pStream); }
		__if_not_exists(T::OnLoadStream) {
			__if_exists(T::OnLoadData)
			{
				STATSTG ss;
				HRESULT hr = pStream->Stat(&ss, STATFLAG_NONAME);
				ULONG cb = (ULONG)ss.cbSize.QuadPart;
				LPSTR pBuf = NEW CHAR[cb+1];
				if (pBuf==NULL) return;
				pBuf[cb] = 0;
				ULONG cbRead = 0;
				hr = pStream->Read(pBuf, cb, &cbRead);
				if (SUCCEEDED(hr))
				{
					CStringA strBuf = pBuf;
					pT->OnLoadData(pBuf, cb);
				}

				delete[] pBuf;
			}
		}
	}
};

#endif // !defined(__DUIBASE_H__)
