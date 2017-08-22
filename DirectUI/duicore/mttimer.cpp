#include "stdafx.h"
#include "mttimer.h"
#include "../../common/macro.h"

#ifndef NEW
#define NEW new
#endif // NEW

#define UserAlloc() \
public: \
	static void* operator new (size_t size) { return ::CoTaskMemAlloc(size); } \
	static void operator delete (void* p, size_t size) { ::CoTaskMemFree(p); }

struct TimerTask;
class ThreadData;
class TimerMgr;

// {CDF4F762-7C05-48e3-B1A9-4CD81C2DE42E}
static const GUID CLSID_MTTIMER = 
{ 0xcdf4f762, 0x7c05, 0x48e3, { 0xb1, 0xa9, 0x4c, 0xd8, 0x1c, 0x2d, 0xe4, 0x2e } };

class MtClassFactory : public IClassFactory
{
public:
	STDMETHOD_(ULONG, AddRef)() { return 1; }
	STDMETHOD_(ULONG, Release)() { return 1; }
	STDMETHOD(QueryInterface)(REFIID iid, LPVOID* ppvObj)
	{
		if (!ppvObj)
			return E_POINTER;

		*ppvObj = NULL;
		if (IsEqualIID(iid, __uuidof(IUnknown)) ||
			IsEqualIID(iid, __uuidof(IClassFactory)))
			*ppvObj = this;
		if (*ppvObj)
		{
			((LPUNKNOWN)(*ppvObj))->AddRef();
			return S_OK;
		}
		return E_NOINTERFACE;
	}
	STDMETHOD (CreateInstance)(IUnknown *pUnkOuter, REFIID riid, void **ppvObject);
	STDMETHOD (LockServer)(BOOL) { return S_OK; }
};

//////////////////////////////////////////////////////////////////////////
// CriticalSession
class CSObject
{
	CRITICAL_SECTION _cs;
public:
	CSObject() { ::InitializeCriticalSection(&_cs); }
	~CSObject() { ::DeleteCriticalSection(&_cs); }
	void Lock() { ::EnterCriticalSection(&_cs); }
	BOOL TryLock() { return ::TryEnterCriticalSection(&_cs); }
	void Unlock() { ::LeaveCriticalSection(&_cs); }
};

class CSLock
{
	CSObject& _obj;
	BOOL _locked;
public:
	CSLock(CSObject& obj, BOOL bTry=FALSE) : _obj(obj), _locked(TRUE) { if (bTry) _locked=_obj.TryLock(); else _obj.Lock(); }
	~CSLock() { /*if (_locked) _obj.*/Unlock(); }
	void Relock() { if (!_locked) _locked = (_obj.Lock(), TRUE); }
	void Unlock() { if (_locked) _locked = (_obj.Unlock(), FALSE); }
	BOOL IsLocked() { return _locked; }
};

//////////////////////////////////////////////////////////////////////////
// 这个类管理TLS。全局一个实例，每个线程一个独立的数据和窗口
class TLS
{
	DWORD _tlsIndex;
	//TimerMgr* _pMgr;

	//HANDLE _hMap;
	//struct MapData
	//{
	//	DWORD tlsIndex;
	//	TimerMgr* pMgr;
	//}* _pData;

	Begin_Disp_Map(TLS)
		//Disp_PropertyGet(111, Tls)
		Disp_PropertyGet(112, ThreadData)
		Disp_PropertyGet(113, RawThreadData)
		Disp_Method(200, NewCookie, ULONG, 0)
	End_Disp_Map()
	MtClassFactory _cf;
	DWORD _dwReg;
	HRESULT _hr;

public:
	BOOL bMain;
	CComDispatchDriver _disp;
	//TimerMgr& Mgr() { return *_pMgr; }
	void safe_init();

public:
	TLS(); //: _tlsIndex(::TlsAlloc());
	~TLS();

	void operator=(LPVOID data) { safe_init(); ::TlsSetValue(_tlsIndex, data); }
	operator LPVOID() { safe_init(); return ::TlsGetValue(_tlsIndex); }

	//HRESULT GetTls(VARIANT* pv)
	//{
	//	pv->vt = VT_UI4;
	//	pv->ulVal = (ULONG)(ULONG_PTR)this;
	//}
	HRESULT GetThreadData(VARIANT* pv);
	HRESULT GetRawThreadData(VARIANT* pv);
	ULONG NewCookie() { static ULONG cookie=0; return cookie++; }
};
TLS gTlsData;

//////////////////////////////////////////////////////////////////////////
// TimerMgr
class TimerMgr
{
	//UserAlloc()
private:
	HANDLE _timer_thread;		// 时钟线程
	static const int _thread_num = 20;// 最多20个需要定时器的线程
	ThreadData** _threads;
	CSObject _cs;
	HANDLE _hStop;
	BOOL _stopped;

	BOOL _start()
	{
		if (_hStop == NULL) _hStop = ::CreateEventW(NULL, TRUE, FALSE, NULL);
		ATLASSERT(_hStop);
		if (_timer_thread == NULL) _timer_thread = CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)_thread_proc,(LPVOID)this,0,NULL);
		return _timer_thread!=NULL;
	}
	void _stop()
	{
		if (_timer_thread)
		{
			if (_hStop)
			{
				::SignalObjectAndWait(_hStop, _timer_thread, 500/*INFINITE*/, FALSE);
				if (!_stopped)
					::TerminateThread(_timer_thread, 1);
				//::SetEvent(_hStop);
				//::WaitForSingleObject(_timer_thread, INFINITE);
			}
			else
			{
				::TerminateThread(_timer_thread, 1);
			}
			::CloseHandle(_timer_thread);
			_timer_thread = NULL;
		}
		if (_hStop) ::CloseHandle(_hStop); _hStop=NULL;
	}
	static DWORD WINAPI _thread_proc(LPVOID lpParam);

public:

	TimerMgr() : _timer_thread(NULL), _hStop(NULL), _stopped(FALSE), _threads(NEW ThreadData*[_thread_num])
	{
		memset(_threads, 0, sizeof(ThreadData*) * _thread_num);
	}
	~TimerMgr()
	{
		_stop();
		delete [] _threads;
	}

	// 每个线程自动调用一次
	BOOL AddThreadData(ThreadData* td)
	{
		if (!_start()) return FALSE;
		CSLock lock(_cs);
		for (int i=0; i<_thread_num; i++)
		{
			if (_threads[i] == NULL)
			{
				_threads[i] = td;
				return TRUE;
			}
		}
		return FALSE;
	}
	void RemoveThreadData(ThreadData* td)
	{
		//if (!_start()) return;
		if (td==NULL) return;
		CSLock lock(_cs);
		for (int i=0; i<_thread_num; i++)
			if (_threads[i] == td)
			{
				_threads[i] = NULL;
				return;
			}
	}

	// 定时器线程调用
	static double frequency()
	{
		static double _f = 0.0;
		if (_f == 0.0)
		{
			LARGE_INTEGER _frequency;
			QueryPerformanceFrequency(&_frequency);
			_f = double(_frequency.QuadPart)/1000; // 每毫秒的TICK数，如果要提高精度（比如微秒），把1000改得更大（比如1000000）
		}
		return _f;
	}
	static __int64 tick_count(__int64 ms=0) // 如果ms==0则表示取当前的TICK数
	{
		if (ms == 0)
		{
			LARGE_INTEGER li;
			QueryPerformanceCounter(&li);
			return (__int64)li.QuadPart;
		}
		return (__int64)(double(ms) * frequency());
	}
};
TimerMgr gMgr;


//////////////////////////////////////////////////////////////////////////
// timer task

struct TimerTask
{
	//UserAlloc()
	ThreadData* _thread_data;

	__int64 _iStart;	// 开始时间
	__int64 _iPeriod;	// 定时周期
	pfnTimerProc _proc;	// 回调函数
	LPVOID _data;		// 回调数据
	DWORD _count;		// 触发次数
	DWORD _count_limit;	// 触发次数限制（0表示不限制）

	DWORD _cookie;
	BOOL _executing;
	BOOL _valid;
	BOOL _killed;

	TimerTask* _next;

	//TimerTask(ThreadData* thread_data);

	void Delete()
	{
		if (_next) _next->Delete();
		delete this;
	}
	TimerTask*& next() { return _next; }
};

//////////////////////////////////////////////////////////////////////////
// thread data

class ThreadData
{
	friend void OnEndOfThread();
	friend TimerMgr;
	//UserAlloc()
private:
	HWND _hwnd;		// 线程中接收消息的隐藏窗口句柄
	BOOL _stopPending;
	ULONG _running;
	TimerTask* _task;

	ThreadData() : _hwnd(NULL), _task(NULL), _stopPending(FALSE), _running(0)
	{
		if (GetClassAtom() != 0)
			_hwnd = ::CreateWindow(MAKEINTRESOURCE(GetClassAtom()), _T(""), 0,0,0,0,0, HWND_MESSAGE, 0,NULL,0);
		gTlsData = this;
		gMgr.AddThreadData(this);
	}
	~ThreadData() { if (_task) _task->Delete(); _task = NULL; }
	//void _clear()
	//{
	//	gTlsData.Mgr().RemoveThreadData(this);
	//	gTlsData = NULL;

	//	if (_task) _task->Delete(); _task = NULL;

	//	if (::IsWindow(_hwnd))
	//	{
	//		::DestroyWindow(_hwnd);
	//		_hwnd = NULL;
	//	}
	//}

public:
	CSObject cs;
	void Delete()
	{
		CSLock lock(cs);
		gMgr.RemoveThreadData(this);
		gTlsData = NULL;

		if (::IsWindow(_hwnd))
		{
			::DestroyWindow(_hwnd);
			_hwnd = NULL;
		}

		//if (_task) _task->Delete(); _task = NULL;

		if (_running>0) _stopPending = TRUE;
		else
		{
			lock.Unlock();
			delete this;
		}
	}
	HWND hwnd() { return _hwnd; }
	TimerTask*& task() { return _task; }

	static ThreadData* instance(bool bAutoCreate=true)
	{
		if ((LPVOID)gTlsData == NULL && bAutoCreate) NEW ThreadData;
		return (ThreadData*)(LPVOID)gTlsData;
	}
	static const UINT msg_id() { return WM_APP + 1; }

	DWORD AddTimerTask(DWORD dwCookie, DWORD dwPeriod, DWORD dwCount, pfnTimerProc proc, LPVOID pData)
	{
		if (dwPeriod==0 || proc==NULL) return (DWORD)-1;

		// dwCookie==-1 means that new cookie
		DWORD dwNewCookie = -1;
		if (dwCookie == (DWORD)-1)
		{
			CComVariant v;
			HRESULT hr = gTlsData._disp.Invoke0(L"NewCookie", &v);
			ATLASSERT(v.vt==VT_UI4);
			dwCookie = dwNewCookie = v.ulVal;
		}

		CSLock lock(cs);
		TimerTask* task = _task;
		// find exist
		while (task && task->_cookie!=dwCookie) task = task->next();
		if (task)
		{
			if (task->_proc!=proc || task->_data!=pData)
			{
				if (dwNewCookie == -1)
				{
					CComVariant v;
					HRESULT hr = gTlsData._disp.Invoke0(L"NewCookie", &v);
					ATLASSERT(v.vt==VT_UI4);
					dwNewCookie = v.ulVal;
				}
				dwCookie = dwNewCookie;
			}
		}
		//if (task == NULL)
		//{
		//	// find free
		//	while (task && (task->_valid || task->_executing)) task = task->next();
		//}
		if (task) // found
		{
			task->_iStart = TimerMgr::tick_count();
			task->_iPeriod = TimerMgr::tick_count((__int64)dwPeriod);
			task->_count = 0;
			task->_count_limit = dwCount;
			//task->_proc = proc;
			//task->_data = pData;
			task->_executing = FALSE;
			task->_valid = TRUE;
			task->_killed = FALSE;
			task->_cookie = dwCookie;
		}
		else // not found
		{
			TimerTask* last = _task;
			if (last)
				while (last->next()) last = last->next();

			task = NEW TimerTask;
			task->_thread_data = this;
			task->_iStart = TimerMgr::tick_count();
			task->_iPeriod = TimerMgr::tick_count((__int64)dwPeriod);
			task->_count = 0;
			task->_count_limit = dwCount;
			task->_proc = proc;
			task->_data = pData;
			task->_executing = FALSE;
			task->_valid = TRUE;
			task->_killed = FALSE;
			task->_next = NULL;
			task->_cookie = dwCookie;
			if (last)
			{
				//task->_cookie = last->_cookie + 1;
				last->_next = task;
			}
			else
				_task = task;
		}
		return task->_cookie;
	}

	void KillTimerTask(DWORD dwTimerId)
	{
		if (dwTimerId == (DWORD)-1) return;
		if (::IsBadReadPtr(_task, sizeof(TimerTask))) return;

		CSLock lock(cs);
		TimerTask* task = _task;
		while (task && task->_cookie!=dwTimerId) task = task->next();
		if (task)
		{
			task->_valid = FALSE;
			task->_killed = TRUE;
		}
	}

	// 窗口过程
private:
	static ATOM GetClassAtom()
	{
		static ATOM _atom = 0;
		if (_atom == 0)
		{
			WNDCLASSW wc;
			memset(&wc, 0, sizeof(wc));
			wc.lpfnWndProc = (WNDPROC)WindowProc;
			wc.lpszClassName = L"timer_hwnd";
			_atom = RegisterClassW(&wc);
		}
		return _atom;
	}

	static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		if (uMsg == msg_id())
		{
			TimerTask* task = (TimerTask*)wParam;
			ATLASSERT(task && task->_executing);
			if (::IsBadWritePtr(task, sizeof(TimerTask))) return 0;
			if (task->_thread_data->_stopPending) return 0;
			task->_thread_data->_running ++;

			if (task->_proc && !task->_killed)
				task->_proc(task->_data, task->_cookie);

			task->_thread_data->_running --;
			if (task->_thread_data->_running==0 && task->_thread_data->_stopPending)
				return delete task->_thread_data/*task->_thread_data->Delete()*/, 0;

			//if (!::IsBadWritePtr(task, sizeof(TimerTask)))
				task->_executing = FALSE;
		}
		return ::DefWindowProcW(hwnd, uMsg, wParam, lParam);
	}
};

//// auto release thread data
//class ThreadDataReleasor
//{
//public:
//	~ThreadDataReleasor()
//	{
//		ThreadData* p = (ThreadData*)(LPVOID)gTlsData;
//		if (p) p->Delete();
//	}
//};
//__declspec(thread) ThreadDataReleasor __releasor;


TLS::TLS() : _hr(::CoInitialize(NULL)), bMain(FALSE)
{
}

TLS::~TLS()
{
	if (bMain)
	{
		::TlsFree(_tlsIndex);
		CoRevokeClassObject(_dwReg);
	}
	if (_disp.p) _disp.Release();
	if (SUCCEEDED(_hr)) ::CoUninitialize();
}

HRESULT TLS::GetThreadData( VARIANT* pv )
{
	safe_init();
	pv->vt = VT_UI4;
	pv->ulVal = (ULONG)(ULONG_PTR)ThreadData::instance();
	return S_OK;
}

HRESULT TLS::GetRawThreadData( VARIANT* pv )
{
	safe_init();
	pv->vt = VT_UI4;
	pv->ulVal = (ULONG)(ULONG_PTR)ThreadData::instance(false);
	return S_OK;
}

void TLS::safe_init()
{
	if (_disp.p == NULL)
	{
		_disp.CoCreateInstance(CLSID_MTTIMER);
		if (_disp.p == NULL)
		{
			bMain = TRUE;
			_tlsIndex = ::TlsAlloc();
			CoRegisterClassObject(CLSID_MTTIMER, &_cf, CLSCTX_LOCAL_SERVER, REGCLS_MULTIPLEUSE, &_dwReg);
			//hr = CoGetClassObject(CLSID_MTTIMER, CLSCTX_INPROC, NULL, IID_IDispatch, (void**)&_disp);
			_disp.CoCreateInstance(CLSID_MTTIMER);
		}
	}
}
STDMETHODIMP MtClassFactory::CreateInstance(IUnknown *pUnkOuter, REFIID riid, void **ppvObject)
{
	//gTlsData.safe_init();
	return gTlsData.GetDispatch()->QueryInterface(riid, ppvObject);
}

//////////////////////////////////////////////////////////////////////////
// 实现

DWORD WINAPI TimerMgr::_thread_proc( LPVOID lpParam )
{
#define is_exit() (::WaitForSingleObject(mgr->_hStop,0) == WAIT_OBJECT_0)
#define is_valid() (mgr->_threads && mgr->_threads[i] && !::IsBadReadPtr(mgr->_threads[i], sizeof(ThreadData)) && !mgr->_threads[i]->_stopPending)
#define check_valid() if (!is_valid()) break;

	TimerMgr* mgr = (TimerMgr*)lpParam;
	while (!is_exit())
	{
		__int64 sleep_count = tick_count(100); // 默认每次循环结束后睡眠100毫秒

		// 寻找有效的线程数据
		CSLock processLock(mgr->_cs);
		for (int i=0; i<_thread_num && !is_exit(); i++)
		{
			if (is_valid())
			{
				CSLock threadLock(mgr->_threads[i]->cs, TRUE); // try lock
				TimerTask* prev = NULL;
				for (TimerTask* task=mgr->_threads[i]->task(); task && !is_exit() && is_valid(); /*prev=task, task=task->next()*/)
				{//try {
					if (task->_valid)
					{
						__int64 ticks = tick_count(); check_valid();
						if (!task->_executing && (ticks-task->_iStart)>=task->_iPeriod )
						{
							task->_executing = TRUE;
							::PostMessage(mgr->_threads[i]->hwnd(), ThreadData::msg_id(), (WPARAM)task, 0); check_valid();

							task->_iStart = tick_count(); //check_valid();
							task->_count ++; check_valid();
							if (task->_count_limit!=0 && task->_count>=task->_count_limit)
								task->_valid = FALSE;
						}
						check_valid();
						if (task->_valid)
							sleep_count = min(sleep_count, max(task->_iPeriod - (tick_count()-task->_iStart), 1));
					}//}catch(...) { ATLTRACE(L"========Timer Thread Exception!!!\n"); }
					else if (task->_count_limit>0 && task->_count>=task->_count_limit && !task->_executing)
					{
						// 这个任务可以删除
						if (prev==NULL)
						{
							mgr->_threads[i]->task() = (TimerTask*)task->next();
							task->next() = NULL;
							task->Delete();
							task = mgr->_threads[i]->task();
						}
						else
						{
							prev->next() = (TimerTask*)task->next();
							task->next() = NULL;
							task->Delete();
							task = prev->next();
						}
						continue;
					}

					prev = task;
					task = task->next();
				}
			}
		}

		if (is_exit()) break;
		DWORD dwSleep = (DWORD)(double(sleep_count)/frequency());
		if ((long)dwSleep >= 5) // 只有5毫秒以上的空闲才执行睡眠
		{
			processLock.Unlock(); // 睡眠之前必须解锁
			::Sleep(dwSleep);
		}
	}

	mgr->_stopped = TRUE;
	ATLTRACE(L"timer thread exit.\n");
	return 0;

#undef check_valid
#undef is_valid
#undef is_exit
}

//////////////////////////////////////////////////////////////////////////
// API
DWORD SetMtTimer( DWORD dwCookie, DWORD dwPeriod, DWORD dwCount, pfnTimerProc proc, LPVOID pData )
{
	gTlsData.safe_init();
	CComVariant v;
	HRESULT hr = gTlsData._disp.GetPropertyByName(L"ThreadData", &v);
	ATLASSERT(v.vt==VT_UI4);
	ThreadData* ptd = (ThreadData*)(ULONG_PTR)v.ulVal;
	return ptd->AddTimerTask(dwCookie, dwPeriod, dwCount, proc, pData);
}

void KillMtTimer( DWORD dwTimerId )
{
	//ATLTRACE(L"KillMtTimer(%d)\n", dwTimerId);
	gTlsData.safe_init();
	CComVariant v;
	HRESULT hr = gTlsData._disp.GetPropertyByName(L"ThreadData", &v);
	ATLASSERT(v.vt==VT_UI4);
	ThreadData* ptd = (ThreadData*)(ULONG_PTR)v.ulVal;
	ptd->KillTimerTask(dwTimerId);
}

void OnEndOfThread()
{
	if (!gTlsData.bMain || !gTlsData._disp.p) return;
	CComVariant v;
	HRESULT hr = gTlsData._disp.GetPropertyByName(L"RawThreadData", &v);
	ATLASSERT(v.vt==VT_UI4);
	ThreadData* p = (ThreadData*)(ULONG_PTR)v.ulVal;
	//ThreadData* p = (ThreadData*)(LPVOID)gTlsData;
	if (p)
	{
		//gTlsData.Mgr().RemoveThreadData(p);
		//p->_stopPending = TRUE;
		if (p->_running==0) p->Delete();
	}
}