#ifndef DuiPluginsManager_h__
#define DuiPluginsManager_h__

#pragma once

#include "../DuiPlugin.h"

template<typename T>
class DuiHandle
{
public:
	HANDLE _h;
	T* _p;

	DuiHandle(HANDLE h) : _h(h), _p(NULL)
	{
		if (_h)
			__if_exists(T::unused) { _p = (T*)(((char*)_h) - offsetof(T,unused)); }
			__if_not_exists(T::unused) { _p = ((T*)(((char*)_h) + 4)) - 1; }
	}
	DuiHandle(T* p) : _p(p), _h(NULL)
	{
		if (_p)
			__if_exists(T::unused) { _h = (HANDLE)(((char*)_p) + offsetof(T,unused)); }
			__if_not_exists(T::unused) { _h = (HANDLE)(((char*)(_p + 1)) - 4); }
	}

	operator HANDLE() { return _h; }
	operator T* () { return _p; }
	T* operator ->() { return _p; }
};

/*
 *	插件数据
 */
struct PluginData
{
	fnPluginCallback cb;
	LPVOID data;
};

/*
 *	插件调用上下文
 */
typedef void (/*__cdecl*/ * fnDefPluginProc)();
struct PluginCallContext : public CallContext
{
public:
	BOOL stopOnTrue; // 是否在返回TRUE后停止下一个插件调用，默认FALSE，即调用所有插件
	PluginData* lastPlugin; // 如果 stopOnTrue == TRUE，那么这里保存最后返回TRUE的插件
	LPVOID curPlugin; // 插件链中当前正在调用的插件
	BOOL defPluginFinished; // 系统默认实现是否已调用
	fnDefPluginProc defProc; // 默认回调，如果实现了此回调，将用它代替系统默认实现，这种默认回调不能返回VOID，第一个参数为 CallContext*，后续参数为正常参数
	BOOL handled; // 调用是否已被处理

	DWORD unused; // 这个字段不被使用

	PluginCallContext(/*DWORD _opcode, */BOOL _stopOnTrue=FALSE, HDE _hde=NULL, HWND _hwnd=NULL);

	// 设置参数
	PluginCallContext& Output(LPVOID pOutput) // 设置返回值缓冲区指针
	{ output = pOutput; return *this; }
	PluginCallContext& StopOnTrue(BOOL bStopOnTrue) // 设置是否插件返回 TRUE 后停止调用后续插件
	{ stopOnTrue=bStopOnTrue; return *this; }
	template<typename tProc> PluginCallContext& DefProc(tProc proc) // 设置默认回调，用以取代系统默认实现
	{ defProc = (fnDefPluginProc)proc; return *this; }
	PluginCallContext& SetHde(HDE _hde) // 设置控件句柄
	{ hde = _hde; return *this; }
	PluginCallContext& SetHwnd(HWND _hwnd) // 设置窗口句柄
	{ hwnd = _hwnd; return *this; }

	PluginCallContext& Call(DWORD _opcode, ...); // 调用插件链
	PluginCallContext& CallOne(PluginData* plug, DWORD _opcode, ...); // 调用一个具体的插件回调

	operator BOOL() { return handled; }
	BOOL IsHandled() const { return handled; }

	template<typename T>
	T GetOutput() { return output ? *((T*)output) : T(0); }
};

/*
 *	插件管理器
 */
class PluginsManager
{
	friend BOOL DUIAPI CallNextPlugin(const CallContext* cc, char* params);
	friend BOOL DUIAPI CallDefault(const CallContext* cc, char* params);
	friend PluginCallContext;
private:
	//PluginsManager(void);
	//~PluginsManager(void);

public:
	//static PluginsManager& instance();

	static PluginData* RegisterPlugin(fnPluginCallback cb, LPVOID data, LPVOID reserved);
	static BOOL UnregisterPlugin(PluginData* pd);
	static BOOL IsPlugin(PluginData* pd);

	//static BOOL CallPlugins(PluginCallContext& pcc, ...);
	//static void CallOnePlugin(PluginData* plug, PluginCallContext& pcc, ...);

protected:
	static BOOL _callPluginsChain(PluginCallContext& pcc, char* params, BOOL noDefault=FALSE);
};

#endif // DuiPluginsManager_h__
