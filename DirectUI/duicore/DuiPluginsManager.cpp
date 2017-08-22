#include "StdAfx.h"
#include "DuiPluginsManager.h"
#include "DuiBase.h"
#include "DuiManager.h"

// 系统默认实现应该实现这个全局变量
extern PluginData g_defPlugin;

//////////////////////////////////////////////////////////////////////////
class CS
{
	CRITICAL_SECTION cs;
public:
	CS() { ::InitializeCriticalSection(&cs); }
	~CS() { ::DeleteCriticalSection(&cs); }

	operator CRITICAL_SECTION& () { return cs; }
};
class CSLocker
{
	CRITICAL_SECTION& _cs;
public:
	CSLocker(CRITICAL_SECTION& cs) : _cs(cs) { ::EnterCriticalSection(&_cs); }
	~CSLocker() { ::LeaveCriticalSection(&_cs); }
};

CS g_cs;
CSimpleArray<PluginData*> g_plugins;

//////////////////////////////////////////////////////////////////////////
/*
 *	插件调用缓存链
 */
class PluginsList : public TList<PluginsList>
{
public:
	PluginData* plugin;
	BOOL finished;

	static void BuildPluginsChain(PluginsList** ppHead)
	{
		if (ppHead==NULL) return;
		*ppHead = NULL;

		CSLocker locker(g_cs);
		int num = g_plugins.GetSize();
		if (num==0) return;
		PluginsList* p = NEW PluginsList[num];
		if (p==NULL) return;
		for (int i=0; i<num; i++)
		{
			p[i].plugin = g_plugins[num-1-i];
			p[i].finished = FALSE;
			p[i].addToList(ppHead, false);
		}
	}
};

/*
 *	API
 */
HPLUGIN DUIAPI RegisterPlugin(fnPluginCallback cb, LPVOID data, LPVOID reserved)
{
	return DuiHandle<PluginData>(PluginsManager::RegisterPlugin(cb, data, reserved));
}

BOOL DUIAPI UnregisterPlugin(HPLUGIN hPlugin)
{
	return PluginsManager::UnregisterPlugin(DuiHandle<PluginData>(hPlugin));
}

BOOL DUIAPI IsPlugin(HPLUGIN hPlugin)
{
	return PluginsManager::IsPlugin(DuiHandle<PluginData>(hPlugin));
}

/*
 *	CallNextPlugin 是一个很重要的API，用于随时改变插件链的执行次序
 *	插件执行顺序默认是按照注册顺序的反序，即晚注册的插件早执行，早注册的插件晚执行
 *	在某些特殊情形下，一个插件希望得到本该晚执行的插件回调的结果，然后根据结果做进一步处理，此时必须通过 CallNextPlugin 改变执行次序
 *	即在A插件的回调过程中调用此 API，将调用后续 B/C... 插件的回调过程，然后把结果返回给 A 插件，A 插件做处理之后再从回调中返回
 *	与此同时，A 回调完成之后，本该继续执行 B/C... 的回调，但因为 B/C... 已经被执行过，将不再重复调用 B/C... 的回调。
 */
BOOL DUIAPI CallNextPlugin(const CallContext* cc, char* params)
{
	PluginCallContext* ppcc = (PluginCallContext*)cc;
	if (ppcc==NULL || ppcc->curPlugin==NULL)
		return FALSE;

	BOOL bHandled = FALSE;
	PluginsList* plug = (PluginsList*)ppcc->curPlugin;
	//if (plug->Next()) // 不需要此条件，如果 Next 是空，表示调用默认实现
	{
		ppcc->curPlugin = plug->Next();
		bHandled = PluginsManager::_callPluginsChain(*ppcc, params);
		ppcc->curPlugin = plug;
	}

	return bHandled;
}

BOOL DUIAPI CallDefault(const CallContext* cc, char* params)
{
	PluginCallContext* ppcc = (PluginCallContext*)cc;
	if (ppcc==NULL || ppcc->curPlugin==NULL || ppcc->defPluginFinished)
		return FALSE;

	BOOL bHandled = FALSE;

	LPVOID oldPlug = ppcc->curPlugin;

	ppcc->curPlugin = NULL; // 置空即可调用默认实现
	bHandled = PluginsManager::_callPluginsChain(*ppcc, params);
	
	ppcc->curPlugin = oldPlug;

	return bHandled;
}

//////////////////////////////////////////////////////////////////////////
PluginCallContext::PluginCallContext( /*DWORD _opcode,*/ BOOL _stopOnTrue/*=FALSE*/, HDE _hde/*=NULL*/, HWND _hwnd/*=NULL*/ )
	: unused(0)
	, stopOnTrue(_stopOnTrue)
	, lastPlugin(NULL)
	, curPlugin(NULL)
	, defPluginFinished(FALSE)
	, defProc(FALSE)
	, handled(FALSE)
{
	cbSize = sizeof(CallContext);
	opcode = -1;
	hde = _hde;
	hwnd = _hwnd;
	output = NULL;
}

PluginCallContext& PluginCallContext::Call( DWORD _opcode, ... )
{
	opcode = _opcode;
	if (opcode==-1) return *this;

	if (hwnd==NULL && hde)
	{
		DuiNode* n = DuiNode::FromHandle(hde);
		hwnd = n->m_pLayoutMgr->GetPaintWindow();
	}

	va_list params;
	va_start(params, _opcode);

	// 调用所有插件
	PluginsList* chain = NULL;
	chain->BuildPluginsChain(&chain);
	curPlugin = chain;

	handled = PluginsManager::_callPluginsChain(*this, params);

	if (chain)
	{
		delete[] chain;
	}

	va_end(params);

	return *this;
}

PluginCallContext& PluginCallContext::CallOne( PluginData* plug, DWORD _opcode, ... )
{
	if (plug==NULL) return *this;;
	opcode = _opcode;
	if (opcode==-1) return *this;

	if (hwnd==NULL && hde)
	{
		hwnd = DuiHandle<DuiNode>(hde)->m_pLayoutMgr->GetPaintWindow();
	}

	va_list params;
	va_start(params, _opcode);

	// 调用所有插件
	PluginsList chain;
	chain.plugin = plug;
	chain.finished = FALSE;
	curPlugin = &chain;

	PluginsManager::_callPluginsChain(*this, params, TRUE);

	va_end(params);
	return *this;
}

//////////////////////////////////////////////////////////////////////////
//PluginsManager::PluginsManager(void)
//{
//}
//
//PluginsManager::~PluginsManager(void)
//{
//}
//
//PluginsManager& PluginsManager::instance()
//{
//	static PluginsManager mgr;
//	return mgr;
//}

PluginData* PluginsManager::RegisterPlugin( fnPluginCallback cb, LPVOID data, LPVOID reserved )
{
	if (reserved) return NULL;
	if (cb==NULL) return NULL;

	CSLocker locker(g_cs);
	for (int i=0; i<g_plugins.GetSize(); i++)
	{
		if (g_plugins[i]->cb==cb && g_plugins[i]->data==data)
			return g_plugins[i];
	}

	PluginData* pd = NEW PluginData;
	if (pd==NULL) return NULL;
	pd->cb = cb;
	pd->data = data;

	// 检测插件版本号是否使用完全相同的 OPCODE 版本
	DWORD dwVer = 0;
	PluginCallContext().Output(&dwVer).CallOne(pd, 0);
	//CallOnePlugin(pd, PluginCallContext(0).Output(&dwVer));
	if (dwVer != OPCODE_VERSION)
	{
		delete pd;
		return NULL;
	}

	g_plugins.Add(pd);
	return pd;
}

BOOL PluginsManager::UnregisterPlugin( PluginData* pd )
{
	if (pd==NULL) return FALSE;

	CSLocker locker(g_cs);
	int index = g_plugins.Find(pd);
	if (-1 == index)
		return FALSE;

	delete pd;
	g_plugins.RemoveAt(index);
	return TRUE;
}

BOOL PluginsManager::IsPlugin( PluginData* pd )
{
	if (pd==NULL) return FALSE;

	CSLocker locker(g_cs);
	int index = g_plugins.Find(pd);
	return index >= 0;
}

/*
 *	调用插件链
 */
//BOOL PluginsManager::CallPlugins( PluginCallContext& pcc, ... )
//{
//	va_list params;
//	va_start(params, pcc);
//
//	// 调用所有插件
//	BOOL bHandled = FALSE;
//	PluginsList* chain = NULL;
//	chain->BuildPluginsChain(&chain);
//	pcc.curPlugin = chain;
//
//	bHandled = _callPluginsChain(pcc, params);
//
//	if (chain)
//	{
//		delete[] chain;
//	}
//
//	va_end(params);
//
//	return bHandled;
//}
//
///*
// *	调用某个具体插件，不关心是否已处理
// */
//void PluginsManager::CallOnePlugin( PluginData* plug, PluginCallContext& pcc, ... )
//{
//	if (plug==NULL) return;
//
//	va_list params;
//	va_start(params, pcc);
//
//	// 调用所有插件
//	PluginsList chain;
//	chain.plugin = plug;
//	chain.finished = FALSE;
//	pcc.curPlugin = &chain;
//
//	_callPluginsChain(pcc, params, TRUE);
//
//	va_end(params);
//}

/*
 *	pcc.curPlugin 已经有效
 */
BOOL PluginsManager::_callPluginsChain( PluginCallContext& pcc, char* params, BOOL noDefault/*=FALSE*/ )
{
	BOOL bHandled = FALSE;

	PluginsList* plug = (PluginsList*)pcc.curPlugin;
	if (plug==NULL) // 调用默认实现
	{
		if (!noDefault && !pcc.defPluginFinished)
		{
			// 如果设置了默认回调，则调用该回调，否则调用系统默认实现
			if (pcc.defProc)
			{
				bHandled = TRUE; // 随身默认回调都被认为已处理
				switch (pcc.opcode)
				{
#define DoPreArg(...) __VA_ARGS__
#define PreArg(type,name) type* _##name = (type *)((params += X86SIZE(type)) - X86SIZE(type));
#define DoPreArg2(...) __VA_ARGS__
#define PreArg2(type,name) , type
#define Arg2(type,name) *_##name
#define __op_calldef(rettype, op, prearg, prearg2, ...) \
				case OpCode_##op: \
				{ \
					prearg; \
					rettype ret = ((rettype (/*CALLBACK*/ *)(CallContext* prearg2))pcc.defProc)((CallContext*)&pcc, __VA_ARGS__); \
					if (pcc.output!=NULL && bHandled) *((rettype*)pcc.output) = ret; \
					break; \
				}

					__foreach_op(__op_calldef, DoPreArg, PreArg, DoPreArg2, PreArg2, Arg2)

#undef __op_calldef
#undef DoPreArg
#undef PreArg
#undef DoPreArg2
#undef PreArg2
#undef Arg2
				}
			}
			else
			{
				bHandled = g_defPlugin.cb(g_defPlugin.data, &pcc, params);
			}

			pcc.defPluginFinished = TRUE;

			if (bHandled && pcc.stopOnTrue)
				return (pcc.lastPlugin = &g_defPlugin), TRUE;
			return bHandled;
		}
		return FALSE;
	}

	if (!plug->finished)
	{
		// 调用回调
		bHandled = plug->plugin->cb(plug->plugin->data, &pcc, params);

		plug->finished = TRUE;

		if (bHandled && pcc.stopOnTrue)
			return (pcc.lastPlugin = plug->plugin), TRUE;
	}

	// 调用下一个回调
	pcc.curPlugin = plug->Next();
	//if (pcc.curPlugin) // 允许为NULL，表示调用系统默认实现
		return _callPluginsChain(pcc, params);
	//return bHandled;
}

/*
 *	以下代码是一个完整的插件实现，功能就是把所有控件都裁剪成圆角矩形。
 *	这段代码可以放在在REDUI、应用程序、其它DLL中，作用相同。
 *	
 *	实现一个插件只需要几个简单的步骤：
 *		1. 创建一个类，从 ReduiPlugin<> 派生
 *		2. 实现注释中说明的任何一个或多个函数，比如此例中的 GetControlClipRgn，返回一个圆角矩形区域
 *		3. 为类构造一个实例
 */
//class CMyPlugin : public ReduiPlugin<CMyPlugin>
//{
//public:
//	HRGN GetControlClipRgn(HDE hCtrl, HDC hdc, RECT rcClip)
//	{
//		return ::CreateRoundRectRgn(...);
//	}
//};
//
//CMyPlugin myPlugin;