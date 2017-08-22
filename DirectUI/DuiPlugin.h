#ifndef __DUIPLUGIN_H__
#define __DUIPLUGIN_H__

#if (_MSC_VER < 1400) || !defined(_M_IX86)
	#error opcode.h requires Visual C++ 2005 and above, and MUST be compiled in X86 architecture.
#endif // _MSC_VER >= 1400

#include "DirectUIWindow.h"
#include "opcode.h"

typedef HANDLE HPLUGIN;

#define X86SIZE(n)   ( (sizeof(n) + sizeof(int) - 1) & ~(sizeof(int) - 1) )
#define Arg(type,name) ( *(type *)((params += X86SIZE(type)) - X86SIZE(type)) )

#define DuiVoid int

//////////////////////////////////////////////////////////////////////////
// 扩展接口
class IExtension
{
public:
	virtual void Use(HANDLE hSource) = 0; // hSource 表示开始使用此插件的源
	virtual void Unuse(HANDLE hSource) = 0; // hSource 表示不再使用此插件的源
	virtual void* CastToClass(LPCWSTR szName) = 0; // 扩展实现应该提供CAST到具体类型的机制
};

//////////////////////////////////////////////////////////////////////////
// 扩展容器
class IExtensionContainer
{
public:
	virtual void AttachExtension(IExtension* pExt) = 0;
	virtual void DetachExtension(IExtension* pExt) = 0;
};

//////////////////////////////////////////////////////////////////////////
// API for plugins

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

IExtensionContainer* DUIAPI DuiGetExtensionContainer(HDE hde);

#ifdef __cplusplus
}
#endif /* __cplusplus */

//////////////////////////////////////////////////////////////////////////
// 控件扩展，专用于扩展控件，每个实例都通过扩展容器附加到控件中
//class IControlExtension : public IExtension
//{
//	static LPCOLESTR __MyName() { return MakeIntName(100); }
//public:
//	// 控件扩展都应该设置属主控件句柄
//	virtual void SetOwner(HDE hde) = 0;
//	virtual HDE GetOwner() = 0;
//};

//////////////////////////////////////////////////////////////////////////
// 外观接口。插件通过实现外观接口，负责边框和背景的绘制。
// 外观接口对应样式表中的visual样式，框架通过查询插件，根据名称获得此接口
// IVisual 并非扩展接口

class IVisual
{
public:
	virtual RECT GetAdjustRect(HDE hde) const = 0;
	// 以下的绘制函数如果需要调用默认绘制，应该直接返回FALSE
	virtual BOOL OnPaintBorder(HDE hde, HDC hdc, RECT rcBorder) = 0;
	virtual BOOL OnPaintFocus(HDE hde, HDC hdc, RECT rcFocus, HRGN hrgn) = 0;
	virtual BOOL OnPaintBackground(HDE hde, HDC hdc, RECT rcBkgnd) = 0;
	virtual BOOL OnPaintForeground(HDE hde, HDC hdc, RECT rcFrgnd) = 0;
};

//////////////////////////////////////////////////////////////////////////
// 布局接口。插件通过实现布局接口，实现对子控件的布局排版
// 布局接口对应样式表中的 layout 样式，框架通过名称查询插件获得此接口
class ILayout : public IExtension
{
public:
	virtual BOOL OnUpdateLayout(HDE hde) = 0;
};

/*
 *	CallContext 结构表示当前操作的调用上下文，插件应该首先确认此上下文是否被自己关注，不关注的应该立即返回 FALSE
 */
struct CallContext
{
	DWORD cbSize;	// sizeof(CallContext)，结构体大小
	DWORD opcode;	// 操作码，代表将要执行的操作类型，它的值是 OpCode_ 为前缀的值
	LPVOID output;	// 返回值内存的指针，该内存由调用者创建。该值可以为 NULL，表示调用者不关心返回值
	HWND hwnd;		// 当前正在操作的窗口句柄，NULL 表示当前操作未关联窗口
	HDE hde;		// 当前正在操作的控件元素句柄，NULL 表示当前操作未关联控件
};

/*
 *	插件回调函数
 */
typedef BOOL (__cdecl * fnPluginCallback)(LPVOID data, const CallContext* cc, char* params);

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*
 *	注册与卸载插件 API
 */
HPLUGIN DUIAPI RegisterPlugin(fnPluginCallback cb, LPVOID data, LPVOID reserved);
BOOL DUIAPI UnregisterPlugin(HPLUGIN hPlugin);
BOOL DUIAPI IsPlugin(HPLUGIN hPlugin);

// 以下API只能用在回调执行过程中，调用上下文必须有效
// CallNextPlugin 用来调用后续尚未调用的插件回调
// CallDefault 用来调用系统默认实现，它可以直接越过剩余的插件链
BOOL DUIAPI CallNextPlugin(const CallContext* cc, char* params);
BOOL DUIAPI CallDefault(const CallContext* cc, char* params);

#ifdef __cplusplus
}



//////////////////////////////////////////////////////////////////////////
// 插件工具类，必须在不低于VC2005上编译


//////////////////////////////////////////////////////////////////////////
// 工具模板类 ReduiPlugin，插件实现者可选择从此类继承，并选择性实现所需功能对应的成员函数
// 可实现的操作函数参见 opcode.h 文件中开始部分的注释
// 插件编写要遵循如下原则：
//		* 自我管理生命周期
//		* 插件在起作用前必须首先用 RegisterPlugin 注册自己。ReduiPlugin 已在构造函数中完成。
//		* 插件在中止前必须用 UnregisterPlugin 卸载自己。ReduiPlugin 已在析构函数中完成。
//		* 插件必须实现 fnPluginCallback 回调函数，这是插件功能调用的唯一入口。ReduiPlugin 已实现了 Call 回调函数并内部实现调用的派发。
//		* 插件实现的回调函数说明：
//			- 三个参数：
//				CallContext* cc，调用上下文，代表被调用时的当前窗口、控件元素、操作码。如果任何一项内容不符合插件的作用目标，则应该返回 FALSE。
//				char* params，调用参数集合，这是统一的调用约定，不同操作码对应不同的参数解释。
//				LPVOID output，返回值缓冲区指针，不同操作码需要不同的返回值类型，调用者提供缓冲区，如果此值是NULL，表示该操作不需要返回值
//			- 返回值 BOOL：TRUE 表示插件已处理该操作；FALSE 表示插件未处理或不关注该操作
// 
template<class t>
class ReduiPlugin
{
public:
	HPLUGIN m_hPlugin;

protected:
	typedef ReduiPlugin<t> thisClass;

	// 调用需要返回的是否已处理的信息缓存在这里
	BOOL m_bHandled;
	void SetHandled(BOOL bHandled)
	{
		m_bHandled = bHandled;
	}

	// 预检测调用上下文，如果条件不符合，应该返回 FALSE，将不再将调用派发给操作函数
	BOOL IsCallValid(const CallContext* cc)
	{
		switch (cc->opcode)
		{
#define __call_valid(ret, op, ...) __if_exists(t::op) { case OpCode_##op: }

			__foreach_op(__call_valid,,,,,)

#undef __call_valid
		case 0:
			return TRUE;
		}
		return FALSE;
	}

	// 回调参数缓存在这里，当调用 CallNextPlugin/CallDefault 等改变插件链调用次序的 API 时需要这些参数
	struct
	{
		const CallContext* _cc;
		char* _params;
	} m_cache;

public:
	ReduiPlugin() : m_hPlugin(NULL), m_bHandled(FALSE)
	{
		__if_not_exists(t::__NoAutoRegister)
		{
			m_hPlugin = ::RegisterPlugin((fnPluginCallback)&thisClass::_PluginEntry, (t*)this, NULL);
		}
	}
	virtual ~ReduiPlugin()
	{
		__if_not_exists(t::__NoAutoRegister)
		{
			::UnregisterPlugin(m_hPlugin);
		}
	}

	operator HPLUGIN () { return m_hPlugin; }

	BOOL IsPlugin()
	{
		return ::IsPlugin(m_hPlugin);
	}

	const CallContext* GetCallContext() const
	{
		return m_cache._cc;
	}

	LPVOID GetOutputPtr() const
	{
		return m_cache._cc.output;
	}

	/*
	 *	CallNextPlugin 用于改变插件链的执行顺序，在当前插件的回调过程中，先执行后续插件的回调。
	 *	这种方式可以获得后续回调的结果后再决定自己如何执行
	 */
	BOOL CallNextPlugin()
	{
		return ::CallNextPlugin(m_cache._cc, m_cache._params);
	}

	/*
	 *	在正常情况下，系统默认实现在插件调用链的最末端被调用，CallDefault 可使系统默认实现提前执行，然后返回继续执行调用链。
	 *	插件调用链的每个插件（包括系统默认实现）最多被执行一次，如果顺序被打乱，执行过的插件回调将不会再次执行
	 */
	BOOL CallDefault()
	{
		return ::CallDefault(m_cache._cc, m_cache._params);
	}

protected:
	/*
	 *	_PluginEntry 是插件接口的唯一调用入口，此实现将简化插件的制作
	 */
	static BOOL __cdecl _PluginEntry(LPVOID data, const CallContext* cc, char* params)
	{
		_assert(data && cc);
		t* pThis = (t*)data;

		/*
		 *	缓存参数，以便 CallNextPlugin 读取
		 */
		pThis->m_cache._cc = cc;
		pThis->m_cache._params = params;

		/*
		 *	检查调用上下文，不关注的不继续处理
		 */
		pThis->SetHandled(FALSE); // 检测有效性时，默认设置为不处理
		if (!pThis->IsCallValid(cc))
			return FALSE;

		/*
		 *	转换实际调用
		 */
		pThis->SetHandled(TRUE); // 实际调用时，默认设置为已处理

		/*
		 *	如果插件需要自定义处理过程，应该在派生类中实现以下函数：
		 *	
				 BOOL PluginProc(const CallContext* cc, char* params);
		 *	
		 *	在实现中如果属于不需要自己处理的条件，应该返回 FALSE，将继续执行默认的调用派发操作
		 */
		__if_exists(t::PluginProc)
		{
			if (pThis->PluginProc(cc, params))
				return TRUE;
		}

		return pThis->thisClass::_CallDispatch(cc, params);
	}

private:
	BOOL _CallDispatch(const CallContext* cc, char* params)
	{
		t* pThis = (t*)this;
		switch (cc->opcode)
		{
#define DoPreArg(...) __VA_ARGS__
#define PreArg(type,name) type* _##name = (type *)((params += X86SIZE(type)) - X86SIZE(type));
#define DoPreArg2(...)
#define PreArg2(type,name)
#define Arg2(type,name) *_##name
#define __op_dispatch(retype, op, prearg, prearg2, ...) \
	__if_exists(t::op) { \
		case OpCode_##op: \
		{ \
			prearg; \
			retype ret = pThis->op(__VA_ARGS__); \
			if (cc->output!=NULL && m_bHandled) *((retype*)cc->output) = ret; \
			break; \
		} \
	}

		__foreach_op(__op_dispatch, DoPreArg, PreArg, DoPreArg2, PreArg2, Arg2)

#undef __op_dispatch
#undef DoPreArg
#undef PreArg
#undef DoPreArg2
#undef PreArg2
#undef Arg2

		case 0: if (cc->output) *((DWORD*)cc->output) = OPCODE_VERSION; break; // 0 表示获取OPCODE版本号
		default: SetHandled(FALSE);
		}
		return m_bHandled;
	}
};


#endif // __cplusplus

#endif // __DUIPLUGIN_H__