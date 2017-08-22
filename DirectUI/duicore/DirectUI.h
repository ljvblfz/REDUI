#ifndef __DIRECTUI_H__
#define __DIRECTUI_H__

#pragma once

#include <atlcomcli.h>

#ifndef __DUIPLUGIN_H__
#include "../DuiPlugin.h"
#endif

//////////////////////////////////////////////////////////////////////////
//
//class CDuiLayoutModeBase;
class CDuiScrollbarBase;
//class CDuiVisualBase;
struct ControlExtensionEntry;
//struct DuiLayoutModeFactroyEntry;
//struct DuiVisualFactroyEntry;


//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
// Don't remove or edit this block 不要删除或修改下列内容
#pragma section("DirectUI$__a", read, shared)
#pragma section("DirectUI$__b", read, shared)
#pragma section("DirectUI$__c", read, shared)
extern "C"
{
	__declspec(selectany) __declspec(allocate("DirectUI$__a")) ControlExtensionEntry* __pctrlextEntryFirst = NULL;
	__declspec(selectany) __declspec(allocate("DirectUI$__c")) ControlExtensionEntry* __pctrlextEntryLast = NULL;
}

#pragma comment(linker, "/merge:DirectUI=.rdata")
// Don't remove or edit this block 不要删除或修改上列内容
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
// 基础工具宏
namespace __dui_base
{
#define __for_each_number(v, ...) \
	v(0, __VA_ARGS__) \
	v(1, __VA_ARGS__) \
	v(2, __VA_ARGS__) \
	v(3, __VA_ARGS__) \
	v(4, __VA_ARGS__) \
	v(5, __VA_ARGS__) \
	v(6, __VA_ARGS__) \
	v(7, __VA_ARGS__) \
	v(8, __VA_ARGS__) \
	v(9, __VA_ARGS__) \
	v(10, __VA_ARGS__) \
	v(11, __VA_ARGS__) \
	v(12, __VA_ARGS__) \
	v(13, __VA_ARGS__) \
	v(14, __VA_ARGS__) \
	v(15, __VA_ARGS__)

	// 数值减1的常数
#define __cntdec_0 0
#define __cntdec_1 0
#define __cntdec_2 1
#define __cntdec_3 2
#define __cntdec_4 3
#define __cntdec_5 4
#define __cntdec_6 5
#define __cntdec_7 6
#define __cntdec_8 7
#define __cntdec_9 8
#define __cntdec_10 9
#define __cntdec_11 10
#define __cntdec_12 11
#define __cntdec_13 12
#define __cntdec_14 13
#define __cntdec_15 14

#define __cntdec(n) __cntdec_##n

	// 连接两个符号
#define __connect2(x, y) x##y
#define __connect(x, y) __connect2(x, y)

	// 把符号变成字符串
#define __to_string2(x) #x
#define __to_string(x) __to_string2(x)

	// 生成不同个数的顺序符号
#define __repeat_0(m, ...)
#define __repeat_1(m, ...)	__repeat_0(m, __VA_ARGS__)  m(1, __VA_ARGS__)
#define __repeat_2(m, ...)	__repeat_1(m, __VA_ARGS__)  m(2, __VA_ARGS__)
#define __repeat_3(m, ...)	__repeat_2(m, __VA_ARGS__)  m(3, __VA_ARGS__)
#define __repeat_4(m, ...)	__repeat_3(m, __VA_ARGS__)  m(4, __VA_ARGS__)
#define __repeat_5(m, ...)	__repeat_4(m, __VA_ARGS__)  m(5, __VA_ARGS__)
#define __repeat_6(m, ...)	__repeat_5(m, __VA_ARGS__)  m(6, __VA_ARGS__)
#define __repeat_7(m, ...)	__repeat_6(m, __VA_ARGS__)  m(7, __VA_ARGS__)
#define __repeat_8(m, ...)	__repeat_7(m, __VA_ARGS__)  m(8, __VA_ARGS__)
#define __repeat_9(m, ...)	__repeat_8(m, __VA_ARGS__)  m(9, __VA_ARGS__)
#define __repeat_10(m, ...) __repeat_9(m, __VA_ARGS__)  m(10, __VA_ARGS__)
#define __repeat_11(m, ...) __repeat_10(m, __VA_ARGS__)  m(11, __VA_ARGS__)
#define __repeat_12(m, ...) __repeat_11(m, __VA_ARGS__)  m(12, __VA_ARGS__)
#define __repeat_13(m, ...) __repeat_12(m, __VA_ARGS__)  m(13, __VA_ARGS__)
#define __repeat_14(m, ...) __repeat_13(m, __VA_ARGS__)  m(14, __VA_ARGS__)
#define __repeat_15(m, ...) __repeat_14(m, __VA_ARGS__)  m(15, __VA_ARGS__)

#define __last_repeat_0(m, ...)
#define __last_repeat_1(m, ...)	m(1, __VA_ARGS__)
#define __last_repeat_2(m, ...)	m(2, __VA_ARGS__)
#define __last_repeat_3(m, ...)	m(3, __VA_ARGS__)
#define __last_repeat_4(m, ...)	m(4, __VA_ARGS__)
#define __last_repeat_5(m, ...)	m(5, __VA_ARGS__)
#define __last_repeat_6(m, ...)	m(6, __VA_ARGS__)
#define __last_repeat_7(m, ...)	m(7, __VA_ARGS__)
#define __last_repeat_8(m, ...)	m(8, __VA_ARGS__)
#define __last_repeat_9(m, ...)	m(9, __VA_ARGS__)
#define __last_repeat_10(m, ...) m(10, __VA_ARGS__)
#define __last_repeat_11(m, ...)  m(11, __VA_ARGS__)
#define __last_repeat_12(m, ...)  m(12, __VA_ARGS__)
#define __last_repeat_13(m, ...)  m(13, __VA_ARGS__)
#define __last_repeat_14(m, ...)  m(14, __VA_ARGS__)
#define __last_repeat_15(m, ...)  m(15, __VA_ARGS__)

#define __repeat(n, m_begin, m_end, ...) __connect(__repeat_, __cntdec(n))(m_begin, __VA_ARGS__) __connect(__last_repeat_, n)(m_end, __VA_ARGS__)

// CastHelper
#define __tparam16(n,...) typename T##n=int,
#define __tparam16_end(n,...) typename T##n=int
#define __basecast(n, ...) \
	__if_exists(T##n::CastToClass) { if (!p) p = pExt->T##n::CastToClass(name); } \
	__if_exists(T##n::__HasName) { if (!p && (T##n::__HasName(name))) return (T##n *)pExt; } \
	__if_exists(T##n::__MyName) { if (!p && (name==T##n::__MyName() || (!isInt && (HIWORD(T##n::__MyName())!=0) && lstrcmpiW(name,T##n::__MyName())==0))) return (T##n *)pExt; }

template<class T, __repeat(15, __tparam16, __tparam16_end)>
class __CastHelperT
{
public:
	static LPVOID Cast2Class(T* pExt, LPCOLESTR name)
	{
		if (name==NULL) return NULL;
		BOOL isInt = (HIWORD(name)==0);
		__if_exists(T::__HasName) { if (T::__HasName(name)) return pExt; }
		__if_exists(T::__MyName) { if (name==(T::__MyName()) || (!isInt && (HIWORD(T::__MyName())!=0) && lstrcmpiW(name, T::__MyName())==0)) return pExt; }
		LPVOID p = NULL;
		__repeat(15, __basecast, __basecast)
		return p;
	}
};
} // end namespace __dui_base

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//


// 下面的宏定义用于控件制作辅助工具
typedef IExtension* (CALLBACK* fnCreateControlExtension)();
typedef void (CALLBACK* fnRegisterClassStyle)(HWND, LPCOLESTR);

typedef struct ControlExtensionEntry
{
	LPCOLESTR szName, szAlias, szAlias2;
	int lenName, lenAlias, lenAlias2;
	fnCreateControlExtension pfnCreateControlExtension;
	fnRegisterClassStyle pfnRegisterClassStyle;
} ControlExtensionEntry;

// 内部数字式类名: 100 - 499
enum
{
	InternalIntName__First = 100,
	InternalIntName_ControlExtension = 100,
	InternalIntName_CDuiInlineControlBase,
	InternalIntName_CDuiInlineText,
	InternalIntName_CDuiAnchor,
	InternalIntName_CDuiBig,
	InternalIntName_CDuiBold,
	InternalIntName_CDuiFont,
	InternalIntName_CDuiHorizontalLine,
	InternalIntName_CDuiImg,
	InternalIntName_CDuiItalic,
	InternalIntName_CDuiLineBreak,
	InternalIntName_CDuiParagraph,
	InternalIntName_CDuiUnderline,
	InternalIntName_CDuiWhiteSpace,
	InternalIntName_CDuiSmall,
	InternalIntName_CDuiStrike,

	InternalIntName_CDuiButton,
	InternalIntName_CDuiOption,
	InternalIntName_CDuiCheckBox,
	InternalIntName_CDuiGroupBox,

	InternalIntName_CDuiWindowControl,
	InternalIntName_CDuiAxSite,
	InternalIntName_CDuiActiveX,
	InternalIntName_CDuiXcp,

	InternalIntName_CDuiOnChildWidthChanged,
	InternalIntName_CDuiCustomOrg,

	InternalIntName_CDuiLayout,
	InternalIntName_CDuiLayoutDialog,
	InternalIntName_CDuiLayoutHorizontal,
	InternalIntName_CDuiLayoutHorzSplitter,
	InternalIntName_CDuiLayoutPage,
	InternalIntName_CDuiLayoutSplitter,
	InternalIntName_CDuiLayoutTile,
	InternalIntName_CDuiLayoutVertical,
	InternalIntName_CDuiLayoutVertSplitter,
	InternalIntName_CDuiLayoutModeCoverFlow,

	InternalIntName_CDuiEditable,
	InternalIntName_CDuiEdit,
	InternalIntName_CDuiRichEdit,

	InternalIntName_CDuiController,
	InternalIntName_CDuiControllerListBase,
	InternalIntName_CDuiControllerList,
	InternalIntName_CDuiControllerListMulti,
	InternalIntName_CDuiList,
	InternalIntName_CDuiListFooter,
	InternalIntName_CDuiListHeader,
	InternalIntName_CDuiControllerListItemBase,

	InternalIntName_DuiLayout,
	InternalIntName_LayoutSplitter,
	InternalIntName_DefLayout,

	InternalIntName_LayoutVertical,
	InternalIntName_LayoutHorizontal,
	InternalIntName_LayoutTile,
	InternalIntName_LayoutDialog,
	InternalIntName_LayoutVerticalSplitter,
	InternalIntName_LayoutHorizontalSplitter,
	InternalIntName_LayoutPage,

	//InternalIntName_,
	//InternalIntName_,
	//InternalIntName_,
	//InternalIntName_,

	InternalIntName__Last = 499
};

// 通用扩展类工具宏
#define DECLARE_CLASS_NAME(class, ...) public: static LPCWSTR __MyName() { return __if_exists(InternalIntName_##class){MakeIntName(InternalIntName_##class)} __if_not_exists(InternalIntName_##class){ L#class __pragma(message(#class " has no INT name.")) }; }
#define DECLARE_CLASS_NAME2(class, name) public: static LPCWSTR __MyName() { return L#name; }
#define DECLARE_CLASS_NAME_INT(n, ...) public: static LPCWSTR __MyName() { return MakeIntName(n); }	// 公开型接口应该使用数字式名称，可直接用 == 判断名称是否相同
//#define DECLARE_GET_CLASS_NAME(class) virtual LPCOLESTR GetName() const { return CLASS_NAME(class); }
//#define DECLARE_MODULE_HANDLE() virtual HINSTANCE GetModuleHandle() const { return _AtlBaseModule.GetModuleInstance(); }
#define DECLARE_DELETE_THIS(class) \
	virtual void Use(HANDLE h) \
	{ \
		__if_exists(class::__Use) { __Use(h); } \
		__if_exists(class::OnConnect) { OnConnect(); } \
	} \
	virtual void Unuse(HANDLE h) \
	{ \
		__if_exists(class::__Unuse) { __Unuse(h); } \
		__if_exists(class::OnDisconnect) { OnDisconnect(); } \
		delete this; \
	}

#define DECLARE_NO_DELETE_THIS(class) \
	virtual void Use(HANDLE h) \
	{ \
		__if_exists(class::__Use) { __Use(h); } \
		__if_exists(class::OnConnect) { OnConnect(); } \
	} \
	virtual void Unuse(HANDLE h) \
	{ \
		__if_exists(class::__Unuse) { __Unuse(h); } \
		__if_exists(class::OnDisconnect) { OnDisconnect(); } \
	}

#define DECLARE_HAS_NAME(class, factory, maptype) \
	public: static BOOL __HasName(LPCOLESTR name) \
	{ \
		extern factory maptype##class; \
		factory* f = &maptype##class; \
		if (name==NULL || *name==0) return FALSE; \
		BOOL isInt = (HIWORD(name)==0); \
		if (f->szName==name || (f->szAlias==name) || (f->szAlias2==name)) return TRUE; \
		if (!isInt && ((lstrcmpiW(f->szName,name)==0) || (f->szAlias && lstrcmpiW(f->szAlias,name)==0) || (f->szAlias2 && lstrcmpiW(f->szAlias2,name)==0))) return TRUE; \
		return FALSE; \
	}
#define CLASS_NAME(class) class::__MyName()
//#define CLASS_NAME_BASE(class) class::__MyName()

#define DECLARE_EXTENSION(class) \
public: \
	DECLARE_CLASS_NAME(class) \
	/*DECLARE_GET_CLASS_NAME(class)*/ \
	DECLARE_DELETE_THIS(class)

#define DECLARE_EXTENSION2(class, name) \
public: \
	DECLARE_CLASS_NAME2(class, name) \
	/*DECLARE_GET_CLASS_NAME(class)*/ \
	DECLARE_DELETE_THIS(class)

// 控件扩展实现类可以添加下列宏
#define DECLARE_CONTROL_EXTENSION(class) \
public: \
	DECLARE_EXTENSION(class) \
	/*DECLARE_EXTENSION2(class, control_name)*/ \
	/*__if_exists(__ctrlext_##class) {DECLARE_HAS_NAME(class, ControlExtensionEntry, __ctrlext_)}*/ \
	static IExtension* CALLBACK CreateMyself() { return NEW class; }

// 每个控件实现类应该在类定义外部添加这个宏
#define THIS_IS_CONTROL_EXTENSION_BASE(class, name, alias1, alias2) \
	__declspec(selectany) ControlExtensionEntry __ctrlext_##class = {name, alias1, alias2, lstrlenW(name), alias1?lstrlenW(alias1):0, alias2?lstrlenW(alias2):0, \
			class::CreateMyself, \
			__if_exists(class::__RegisterMyClassStyle){class::__RegisterMyClassStyle} __if_not_exists(class::__RegisterMyClassStyle){NULL}}; \
	extern "C" __declspec(allocate("DirectUI$__b")) __declspec(selectany) ControlExtensionEntry* const __pctrlext_##class = &__ctrlext_##class; \
	__pragma(comment(linker, "/include:___pctrlext_" #class));

#define THIS_IS_CONTROL_EXTENSION(class, name) THIS_IS_CONTROL_EXTENSION_BASE(class, L#name, NULL, NULL)
#define THIS_IS_CONTROL_EXTENSION2(class, name, alias) THIS_IS_CONTROL_EXTENSION_BASE(class, L#name, L#alias, NULL)
#define THIS_IS_CONTROL_EXTENSION3(class, name, alias, alias2) THIS_IS_CONTROL_EXTENSION_BASE(class, L#name, L#alias, L#alias2)


// 通常控件实现类具有初始化的STYLE，在类里用这个宏添加初始化风格串，参数必须使用UNICODE串或者NULL
#define BEGIN_CLASS_STYLE_MAP( default_class_style) \
public: \
	static void CALLBACK __RegisterMyClassStyle(HWND hwnd, LPCOLESTR lpszName=NULL) \
	{ \
		/*if (lpszName==NULL) lpszName = class::__MyName();*/ \
		if (lpszName==NULL || *lpszName==0) return; \
		static const struct {DWORD _state; LPCOLESTR _style;} __stylesEntry[] = { \
			{0, L##default_class_style},

#define STATE_STYLE(state, style) {state, L##style},

#define END_CLASS_STYLE_MAP() \
		}; \
		__super::__RegisterMyClassStyle(hwnd, lpszName); \
		for (int i=0; i<sizeof(__stylesEntry)/sizeof(__stylesEntry[0]); i++) \
			::ParseStyle(hwnd, lpszName, __stylesEntry[i]._state, __stylesEntry[i]._style); \
	}

#define DECLARE_BASE_CLASS_STYLE(default_class_style) \
	BEGIN_CLASS_STYLE_MAP(default_class_style) \
	END_CLASS_STYLE_MAP()

#define DECLARE_CONTROL_FLAGS(x) public: virtual UINT OnGetControlFlags(HANDLE) {return x;}

// CastToClass (class cast)
#define DECLARE_BASE_CLASS_CAST(class, ...) \
public: \
	DECLARE_CLASS_NAME(class) \
	DECLARE_CLASS_CAST(class, __VA_ARGS__)

	//virtual LPVOID CastToClass(LPCOLESTR szName=NULL) \
	//{ \
	//	if (szName==CLASS_NAME(class) || lstrcmpiW(szName, CLASS_NAME(class))==0) return (class*)this; \
	//	__if_exists(class::CastToExt) { LPVOID p=class::CastToExt(szName); if (p) return p; } \
	//	return NULL; \
	//}

#define DECLARE_CLASS_CAST(class, ...) \
public: \
	virtual void* CastToClass(LPCWSTR szName) \
	{ \
		return __dui_base::__CastHelperT<class, __VA_ARGS__>::Cast2Class(this, szName); \
	}

//#define DECLARE_CLASS_CAST2(class, base_class, base_class2) DECLARE_CLASS_CAST(class, base_class, base_class2)

#define CLASS_CAST(ctrl, class) static_cast<class*>((ctrl)->CastToClass(CLASS_NAME(class)))


//////////////////////////////////////////////////////////////////////////
// 布局模式实现类使用下面的宏
//typedef CDuiLayoutModeBase* (CALLBACK* fnCreateLayoutMode)(CDuiControlExtensionBase*);
//
//typedef struct DuiLayoutModeFactroyEntry 
//{
//	LPCOLESTR szName, szAlias, szAlias2;
//	int lenName, lenAlias, lenAlias2;
//	fnCreateLayoutMode pfnCreateLayoutMode;
//} DuiLayoutModeFactroyEntry;
//
//#define DECLARE_LAYOUT_MODE(class, mode) \
//public: \
//	DECLARE_EXTENSION2(class, mode) \
//	DECLARE_HAS_NAME(class, DuiLayoutModeFactroyEntry, __modeMap_) \
//	static CDuiLayoutModeBase* CALLBACK CreateMyself(CDuiControlExtensionBase* owner) { CDuiLayoutModeBase* p=NEW class; return p?(p->SetOwner(owner), p):NULL; }
//
//#define IMPLEMENT_LAYOUT_MODE_BASE(class, alias1, alias2) \
//	__declspec(selectany) DuiLayoutModeFactroyEntry __modeMap_##class = {CLASS_NAME(class), alias1, alias2, lstrlenW(CLASS_NAME(class)), alias1?lstrlenW(alias1):0, alias2?lstrlenW(alias2):0, class::CreateMyself}; \
//	extern "C" __declspec(allocate("DirectUI$__e")) __declspec(selectany) DuiLayoutModeFactroyEntry* const __pmodeMap_##class = &__modeMap_##class; \
//	__pragma(comment(linker, "/include:___pmodeMap_" #class));
//
//#define IMPLEMENT_LAYOUT_MODE(class) IMPLEMENT_LAYOUT_MODE_BASE(class, NULL, NULL)
//#define IMPLEMENT_LAYOUT_MODE2(class, alias) IMPLEMENT_LAYOUT_MODE_BASE(class, L#alias, NULL)
//#define IMPLEMENT_LAYOUT_MODE3(class, alias, alias2) IMPLEMENT_LAYOUT_MODE_BASE(class, L#alias, L#alias2)


//////////////////////////////////////////////////////////////////////////
// 视觉 实现类使用下面的宏
//typedef CDuiVisualBase* (CALLBACK* fnCreateVisual)(CDuiControlExtensionBase*);
//
//struct DuiVisualFactroyEntry
//{
//	LPCOLESTR szName;
//	int lenName;
//	fnCreateVisual pfnCreateVisual;
//};
//
//#define DECLARE_VISUAL(class, name) \
//public: \
//	DECLARE_EXTENSION2(class, name) \
//	static CDuiVisualBase* CALLBACK CreateMyself(CDuiControlExtensionBase* owner) { CDuiVisualBase* p=NEW class; return p?(p->SetOwner(owner), p):NULL; }
//
//#define IMPLEMENT_VISUAL(class) \
//	__declspec(selectany) DuiVisualFactroyEntry __visualMap_##class = {CLASS_NAME(class), lstrlenW(CLASS_NAME(class)), class::CreateMyself}; \
//	extern "C" __declspec(allocate("DirectUI$__h")) __declspec(selectany) DuiVisualFactroyEntry* const __pvisualMap_##class = &__visualMap_##class; \
//	__pragma(comment(linker, "/include:___pvisualMap_" #class));


//////////////////////////////////////////////////////////////////////////
// 注意：使用小写字母有特殊用途

typedef enum
{
	DUIET__FIRST = 0,	DUIET__INVALID = DUIET__FIRST,
	DUIET_mousemove,	DUIET_MOUSEBEGIN = DUIET_mousemove,
	DUIET_mouseleave,	DUIET_mouseout = DUIET_mouseleave,
	DUIET_mouseenter,	DUIET_mouseover = DUIET_mouseenter,
	DUIET_mousehover,
	DUIET_buttondown,
	DUIET_buttonup,
	DUIET_click,
	DUIET_dblclick,
	DUIET_contextmenu,
	DUIET_vscroll,
	DUIET_hscroll,
	DUIET_scrollwheel,	DUIET_MOUSEEND = DUIET_scrollwheel,
	DUIET_keydown,		DUIET_KEYBEGIN = DUIET_keydown,
	DUIET_keyup,
	DUIET_char,
	DUIET_syskey,		DUIET_KEYEND = DUIET_syskey,
	DUIET_killfocus,	DUIET_blur = DUIET_killfocus,
	DUIET_setfocus,		DUIET_focus = DUIET_setfocus,
	DUIET_size,
	//DUIET_setcursor,
	DUIET_measureitem,
	DUIET_drawitem,
	DUIET_timer,
	DUIET_notify,
	DUIET_command,
	DUIET_load,
	DUIET_unload,
	DUIET_propertychange,
	DUIET__LAST,

} DUI_EVENTTYPE;

#define event_type_to_name(type) (LPCOLESTR)(type)

typedef enum
{
	DUIRT_CONTENT	= 0,
	DUIRT_PADDING,
	DUIRT_INSIDEBORDER,	// 视觉样式往往占据边界和PADDING之间的空间，此标志对于填充背景是有用的
	DUIRT_BORDER,
	DUIRT_MARGIN,
	DUIRT_TOTAL = DUIRT_MARGIN,
} DUI_RECTTYPE;

typedef enum
{
	DUIPOS_DEFAULT	= 0,
	DUIPOS_STATIC	= DUIPOS_DEFAULT,	// 默认定位模式，由布局样式决定位置
	DUIPOS_RELATIVE,					// 相对位置，布局样式决定参考位置，left/top决定偏移量，块元素
	DUIPOS_ABSOLUTE,					// 绝对位置，相对于具备POSITION样式的父元素或根元素，left/top/bottom/right/width/height决定位置，非内联元素
	DUIPOS_FIXED,						// 固定位置，相对于窗口客户区，不受任何布局样式的影响，left/top/bottom/right/width/height决定位置，非内联元素
} DUI_POSITION;

//////////////////////////////////////////////////////////////////////////
//

// Flags for GetControlFlags()
#define DUIFLAG_TABSTOP			0x00000001
#define DUIFLAG_WANTRETURN		0x00000002
#define DUIFLAG_SETCURSOR		0x00000004
#define DUIFLAG_SELECTABLE		0x00000008
#define DUIFLAG_EXPANDABLE		0x00000010
#define DUIFLAG_NOFOCUSFRAME	0x00000020
#define DUIFLAG_NOCHILDREN		0x00000040 // 有些控件加载外部内容，忽略所有子控件，管理器仅解析子控件，不绘制或响应事件，但能访问子控件及其属性。此标志暂不支持XML描述。

// Flags for FindControl()
#define DUIFIND_ALL           0x00000000
#define DUIFIND_VISIBLE       0x00000001
#define DUIFIND_ENABLED       0x00000002
#define DUIFIND_HITTEST       0x00000004
#define DUIFIND_ME_FIRST      0x80000000

// Flags for GetStyleAttribute MATCH MASK
#define DUISTYLEMATCH_NAME		0x00000001
#define DUISTYLEMATCH_ID		0x00000002
#define DUISTYLEMATCH_STATE		0x00000004
#define DUISTYLEMATCH_PARENT	0x00000008
#define DUISTYLEMATCH_NAME_ID	(DUISTYLEMATCH_NAME | DUISTYLEMATCH_ID)
#define DUISTYLEMATCH_ALL		(DUISTYLEMATCH_NAME | DUISTYLEMATCH_ID | DUISTYLEMATCH_STATE | DUISTYLEMATCH_PARENT)

// Flags for overflow
typedef enum
{
	OVERFLOW_AUTO		= 0,	// default
	OVERFLOW_HIDDEN,
	OVERFLOW_SCROLL
} DuiOverflowType;

// Flags for display
#define DUIDISPLAY_NONE		0x00000001	// 暂不支持，因为跟DUISTATE冲突
#define DUIDISPLAY_INLINE	0x00000002	// default, HRGN is valid
#define DUIDISPLAY_BLOCK	0x00000004	// RECT is valid
#define DUIDISPLAY_INLINEBLOCK (DUIDISPLAY_INLINE | DUIDISPLAY_BLOCK) // RECT is valid


//////////////////////////////////////////////////////////////////////////
//

//////////////////////////////////////////////////////////////////////////
// style attributes
//typedef enum
//{
//	SAT__UNKNOWN = 0,
//	SAT_COLOR,				// color(COLOR)
//	SAT_FONT,				// font(FONT)
//	SAT_TEXT_STYLE,			// text-style(LONG)
//	SAT_CURSOR,				// cursor(CURSOR)
//	SAT_LINE_DISTANCE,		// line-distance(LONG)
//
//	SAT__NO_PARENT,			// >>>> 以下的内容表示属性不可以从父元素中继承
//
//	SAT_BACKGROUND,			// background(COLOR | ICON | BITMAP | EXTERNAL)
//	SAT_FOREGROUND,			// foreground(COLOR | ICON | BITMAP | EXTERNAL)
//	SAT_FLAG,				// flag(LONG) DUIFLAG_TABSTOP|WANTRETURN
//	SAT_HITTEST_AS,			// hittest_as(LONG) // return while WM_NCHITTEST
//
//	SAT_MARGIN,				// margin(RECT)
//	SAT_PADDING,			// padding(RECT)
//	SAT_BORDER,				// border(PEN)
//
//	SAT_WIDTH,				// width(LONG/PERCENT/AUTO)
//	SAT_HEIGHT,				// height(LONG/PERCENT/AUTO)
//	SAT_MINWIDTH,			// min-width(LONG/PERCENT)
//	SAT_MINHEIGHT,			// min-height(LONG/PERCENT)
//
//	// left/top/right/bottom/width/height用于position样式，计算时参照被参考对象的各个方向的偏移值，例如left参照左边，top参照上边，right参照右边,bottom参照底边
//	// 可以使用auto作为值，前提是能用其它的值算出来，例如 left:10;width:50;right:auto; 表示right = left + width
//	SAT_LEFT,				// left(LONG/PERCENT/AUTO)
//	SAT_TOP,				// top(LONG/PERCENT/AUTO)
//	SAT_RIGHT,				// right(LONG/PERCENT/AUTO)
//	SAT_BOTTOM,				// bottom(LONG/PERCENT/AUTO)
//	SAT_POSITION,			// position(LONG), static(default)/relative/absolute/fixed
//	SAT_ZINDEX,				// z-index(LONG), default:0
//
//	SAT_STRETCH,			// stretch mode(LONG) when parent control's layout mode is dialog
//	SAT_DIALOG_ITEM_POS,	// dailog item rect(RECT) when parent control's layout mode is dialog
//	SAT_COLUMNS,			// columns(LONG) when parent control's layout mode is tile
//
//	SAT_OVERFLOW,			// overflow(LONG) // auto(default) hidden scroll
//	SAT_OVERFLOW_INCLUDE_OFFSET,	// overflow-include-offset(BOOL) // false(default)
//	SAT_DISPLAY,			// display(LONG) // inline(default) inlineblock block none
//	SAT_LAYOUT,				// layout(STRING) // horizontal vertical tile dialog inline(default) // only for block/inlineblock
//	SAT_VISUAL,				// visual(STRING) // button checkbox ...
//	SAT_FILTER,				// filter(STRING) // wipe ...
//	SAT_CLIP,				// clip(STRING) // roundrect custom...
//} DuiStyleAttributeType;
//
//// Style variant type
//typedef enum
//{
//	SVT_EMPTY = 0,
//	SVT_BOOL = 0x00000001<<0,			// true|yes|1, false|no|0
//	SVT_LONG = 0x00000001<<1,			// 123
//	SVT_DOUBLE = 0x00000001<<2,			// 123.456
//	SVT_PERCENT = 0x00000001<<3,		// 40%
//	SVT_STRING = 0x00000001<<4,			// "abcd"
//	SVT_EXPRESSION = 0x00000001<<5,		// "abc()"
//	SVT_COLOR = 0x00000001<<6,			// [ThemeName ]ColorType, #080808
//	SVT_PEN = 0x00000001<<7,			// {ThemeName ]ColorType
//	SVT_FONT = 0x00000001<<8,			// [ThemeName ]FontType
//	SVT_BRUSH = 0x00000001<<9,
//	SVT_ICON = 0x00000001<<10,
//	SVT_BITMAP = 0x00000001<<11,
//	SVT_CURSOR = 0x00000001<<12,
//	SVT_RECT = 0x00000001<<13,			// left top right bottom
//	SVT_SIZE = 0x00000001<<14,			// cx cy
//	SVT_POINT = 0x00000001<<15,			// x y
//	SVT_EXTERNAL = 0x00000001<<16,		// external extension, such as GDI+::Image
//
//	// auto value
//	SVT_AUTO = 0x00000001<<31,			// auto is a "value", it has no real value.
//
//	// font mask
//	SVT_FONT_MASK_FAMILY	= 0x01000000,
//	SVT_FONT_MASK_SIZE		= 0x02000000,
//	SVT_FONT_MASK_STYLE		= 0x04000000,
//	SVT_FONT_MASK_ALL		= (SVT_FONT_MASK_FAMILY | SVT_FONT_MASK_SIZE | SVT_FONT_MASK_STYLE),
//	// pen mask
//	SVT_PEN_MASK_WIDTH		= 0x01000000,
//	SVT_PEN_MASK_STYLE		= 0x02000000,
//	SVT_PEN_MASK_COLOR		= 0x04000000,
//	SVT_PEN_MASK_ALL		= (SVT_PEN_MASK_WIDTH | SVT_PEN_MASK_STYLE | SVT_PEN_MASK_COLOR),
//	// all mask
//	SVT_MASK_ALL			= 0x00FFFFFF,
//} DuiStyleVariantType;
//
//// Style variant
//typedef struct tagDuiStyleVariant
//{
//	DuiStyleAttributeType sat;
//	DuiStyleVariantType svt;
//
//	union
//	{
//		BOOL boolVal;		// svt == SVT_BOOL
//		LONG longVal;		// svt == SVT_LONG
//		DOUBLE doubleVal;	// svt == SVT_DOUBLE or svt == SVT_PERCENT
//		LPCOLESTR strVal;	// svt == SVT_STRING or svt == SVT_EXPRESSION
//		COLORREF colorVal;	// svt == SVT_COLOR
//		LPLOGPEN penVal;		// svt == SVT_PEN
//		LPLOGFONTW fontVal;		// svt == SVT_FONT
//		HBRUSH brushVal;	// svt == SVT_BRUSH
//		HICON iconVal;		// svt == SVT_ICON or svt == SVT_CURSOR
//		HBITMAP bitmapVal;	// svt == SVT_BITMAP
//		//HCURSOR cursorVal;	// svt == SVT_CURSOR
//		LPRECT rectVal;		// svt == SVT_RECT
//		LPSIZE sizeVal;		// svt == SVT_SIZE
//		LPPOINT pointVal;	// svt == SVT_POINT
//		LPVOID extVal;		// svt == SVT_EXTERNAL
//	};
//} DuiStyleVariant;
//
//// style variant API
//DuiStyleVariant* WINAPI StyleVariantCreate();
//BOOL WINAPI StyleVariantInit(DuiStyleVariant* pVar);
//BOOL WINAPI StyleVariantClear(DuiStyleVariant* pVar);
//
//BOOL WINAPI StyleVariantIsEmpty(DuiStyleVariant* pVar);
//HPEN WINAPI StyleVariantGetPen(DuiStyleVariant* pVar, LPLOGPEN pRefPen);
//void WINAPI StyleVariantSetPen(DuiStyleVariant* pVal, LPLOGPEN pRefPen, LONG mask);
//HFONT WINAPI StyleVariantGetFont(DuiStyleVariant* pVar, LPLOGFONTW pRefFont);
//void WINAPI StyleVariantSetFont(DuiStyleVariant* pVar, LPLOGFONTW pRefFont, LONG mask);
//BOOL WINAPI StyleVariantGetLogBrush(DuiStyleVariant* pVar, LPLOGBRUSH pLogBrush);
//BOOL WINAPI StyleVariantGetBitmap(DuiStyleVariant* pVar, LPBITMAP pBitmap);
//LPVOID WINAPI StyleVariantGetExternal(DuiStyleVariant* pVar);
//
//
////////////////////////////////////////////////////////////////////////////
////
//
//// Structure for notifications from the system
//// to the control implementation.
//typedef struct tagDuiEvent
//{
//	LPCOLESTR Name; // HIWORD(Name)==0 is DUI_EVENTTYPE, HIWORD(Name)!=0 is event name string
//	HDE pSender;
//	DWORD dwTimestamp;
//	POINT ptMouse;
//	OLECHAR chKey;
//	WORD wKeyState;
//	LPCOLESTR pPropertyName;
//	WPARAM wParam; // 如果事件来自控件内部，这个参数值可能包含 DISPPARAMS* 作为回调函数的参数
//	LPARAM lParam;
//	BOOL cancelBubble;
//	BOOL returnValue; // TRUE-stop continue, default is FALSE
//} DuiEvent;
//
////////////////////////////////////////////////////////////////////////////
//typedef struct
//{
//	HDE owner; // rc 和 pt 是关联到此控件客户区的
//	RECT rc;
//	POINT pt;				// 当前绘制坐标
//	LONG cyCurLine;			// 当前行的文字高度
//	LONG cyMinHeight;		// 当前行的最小高度 >= 文字高度
//	LONG cyTotalHeight;		// 总高度
//	LONG lLineIndent;		// 缩进
//	LONG lLineDistance;		// 行间距，换行时高度应加上 cyMinHeight + lLineDistance
//	UINT uTextStyle;		// 文本样式
//
//	LPVOID pvReserved;
//} InDrawData;
//

//////////////////////////////////////////////////////////////////////////
// 事件响应类封装。
// 事件响应类通常不单独实现，而是在扩展中实现，并通过IExtension::CastToClass()获得
// 事件响应接口是公开的，为了提高字符串比较性能，采用数字式类名，范围是 500-999
// 扩展接口不同于插件，通常是多实例接口，而插件往往只需要一个全局实例即可。为了使插件也能实现扩展，在扩展方法里第一个参数为一个句柄，代表当前调用者

//#define SINK_CLASS(sinkname) ISink##sinkname
//
//#define DEFINE_SINK(sinkname, ...) \
//class SINK_CLASS(sinkname) \
//{ \
//	DECLARE_CLASS_NAME_INT(IntName_##sinkname) \
//public: virtual void sinkname( HANDLE h, __VA_ARGS__ ) = 0; \
//};
//
//#define DEFINE_SINK_TYPE(type, sinkname, ...) \
//class SINK_CLASS(sinkname) \
//{ \
//	DECLARE_CLASS_NAME_INT(IntName_##sinkname) \
//public: virtual type sinkname( HANDLE h, __VA_ARGS__ ) = 0; \
//};
//
//#define DEFINE_SINK_BOOL(sinkname, ...) DEFINE_SINK_TYPE(BOOL, sinkname, __VA_ARGS__)
//
//enum SinkName
//{
//	IntName_IsMyEvent = 500,
//	IntName_OnGetControlFlags,
//	IntName_OnAddChildNode,
//	IntName_OnStyleChanged,
//	IntName_OnStateChanged,
//	IntName_OnChildStateChanged,
//	IntName_OnAttributeChanged,
//	IntName_OnChildAttributeChanged,
//	IntName_OnAttributeRemoved,
//	IntName_OnChildAttributeRemoved,
//	IntName_OnChildAdded,
//	IntName_OnChildRemoved,
//	IntName_OnUpdateLayout,
//	IntName_OnPrepareDC,
//	IntName_OnScroll,
//	IntName_OnGetContentSize,
//	IntName_OnUpdate3DSurface,
//	IntName_OnUpdateInlineRegion,
//	IntName_OnReposInlineRect,
//	
//	IntName__Last = 999
//};
//
//// 事件处理事件
//DEFINE_SINK_BOOL(IsMyEvent, DuiEvent& event); // 如果事件被处理，返回TRUE，否则返回FALSE
////class CDuiSinkIsMyEvent : public CDuiSinkBase
////{
////	DECLARE_CLASS_NAME_INT(500)
////public:
////	virtual BOOL IsMyEvent(HANDLE h, DuiEvent& event) = 0; // 如果事件被处理，返回TRUE，否则返回FALSE
////};
//
//// 控件标志获取事件
//DEFINE_SINK_TYPE(UINT, OnGetControlFlags); // 获取控件的控制标志位
//
//// 如果某个XML子节点是父控件某要素的专属子节点，实现此方法的要素应该返回TRUE，将不再解析成子控件
//DEFINE_SINK_BOOL(OnAddChildNode, HANDLE hChild); // 父控件决定子节点是控件还是内部资源
//
//// 控件状态改变事件
//DEFINE_SINK(OnStateChanged, DWORD dwStateMask ); // 控件状态发生了改变
//
//// 控件样式属性改变事件
//DEFINE_SINK(OnStyleChanged, DuiStyleVariant* newval); // 控件私有样式属性发生了改变，改变的值在参数里
//
//// 子控件状态改变事件
//DEFINE_SINK_BOOL(OnChildStateChanged, HDE pChild, DWORD dwStateMask); // 子控件的状态发生了改变
//
//// 控件属性改变事件
//DEFINE_SINK(OnAttributeChanged, LPCOLESTR szAttr); // 控件属性发生了改变
//
//// 子控件属性改变事件
//DEFINE_SINK(OnChildAttributeChanged, HDE pChild, LPCOLESTR szAttr); // 子控件的属性发生了改变
//
//// 控件属性已删除事件
//DEFINE_SINK(OnAttributeRemoved, LPCOLESTR szAttr); // 控件属性已删除
//
//// 子控件属性已删除事件
//DEFINE_SINK(OnChildAttributeRemoved, HDE pChild, LPCOLESTR szAttr); // 子控件属性已删除
//
//// 子控件增加事件
//DEFINE_SINK(OnChildAdded, HDE pChild); // 子控件已经添加了
//
//// 子控件删除事件
//DEFINE_SINK(OnChildRemoved, HDE pChild); // 子控件即将被删除
//
//// 布局更新事件
//DEFINE_SINK_BOOL(OnUpdateLayout); // 需要更新自己内容的布局，如果被处理，返回TRUE，否则返回FALSE
//
////// 子控件大小改变事件
////DEFINE_SINK_BOOL(OnChildSizeChanged, CDuiControlExtensionBase* pChild); // 子控件大小需要调整
//
//// 准备设备上下文环境
//DEFINE_SINK_BOOL(OnPrepareDC, HDC hdc); // 控件自己决定如何准备HDC的初始设置，如果已处理返回TRUE， 否则返回FALSE
//
//// 控件滚动条位置改变事件
//DEFINE_SINK(OnScroll, HDE pScroll, BOOL bVert=TRUE); // 控件滚动位置发生了改变，这个事件会发给 pScroll 控件及其所有子控件
//
//// 如果控件自己能提供内容大小的估算，实现这个事件
//DEFINE_SINK(OnGetContentSize, SIZE& sz);
//
//// 控件提供自己的创建/更新3D表面函数
//DEFINE_SINK(OnUpdate3DSurface, RECT rc);
//
//// 内联控件更新自己的区域和矩形
//DEFINE_SINK(OnUpdateInlineRegion)
//
//// 内联控件对当前行重新调整偏移和大小，index表示缓存矩形的索引，cx表示水平偏移值，高度调整参考idd.cyMinHeight
//DEFINE_SINK(OnReposInlineRect, int index, long cx, InDrawData& idd)

//////////////////////////////////////////////////////////////////////////
//


//////////////////////////////////////////////////////////////////////////
//

//////////////////////////////////////////////////////////////////////////
// CDuiControllerBase 是控制器基类。控制器用来集中控制管理子控件的状态和行为，例如LIST/TREE/TABPAGE使用不同的方式管理子控件状态

//class CDuiControllerBase : public IControlExtension
//{
//	DECLARE_BASE_CLASS_CAST(CDuiControllerBase)
//public:
//	//virtual CDuiControlExtensionBase* GetOwner() const = 0;
//
//	virtual BOOL GetShortcut(CDuiControlExtensionBase* child, OLECHAR* pRet) { return FALSE; }
//	virtual BOOL SetShortcut(CDuiControlExtensionBase* child, OLECHAR ch) { return FALSE; }
//
//	virtual BOOL SetFocus(CDuiControlExtensionBase* child) { return FALSE; }
//
//	virtual BOOL GetState(CDuiControlExtensionBase* child, DWORD* pRet) { return FALSE; }
//	virtual BOOL SetState(CDuiControlExtensionBase* child, DWORD dwNewState, DWORD* pRet) { return FALSE; }
//	virtual BOOL ModifyState(CDuiControlExtensionBase* child, DWORD dwAdded, DWORD dwRemoved) { return FALSE; }
//	virtual BOOL HasState(CDuiControlExtensionBase* child, DWORD dwState, BOOL* pRet) { return FALSE; }
//
//	virtual BOOL Activate(CDuiControlExtensionBase* child, BOOL* pRet) { return FALSE; }
//	virtual BOOL IsVisible(CDuiControlExtensionBase* child, BOOL bRealResult, BOOL* pRet) { return FALSE; }
//	virtual BOOL IsEnabled(CDuiControlExtensionBase* child, BOOL bRealResult, BOOL* pRet) { return FALSE; }
//	virtual BOOL IsFocused(CDuiControlExtensionBase* child, BOOL* pRet) { return FALSE; }
//	virtual BOOL SetVisible(CDuiControlExtensionBase* child, BOOL bVisible) { return FALSE; }
//	virtual BOOL SetEnabled(CDuiControlExtensionBase* child, BOOL bEnable) { return FALSE; }
//
//	virtual BOOL GetPos(CDuiControlExtensionBase* child, DUI_RECTTYPE rt, RECT* pRet) { return FALSE; }
//	virtual BOOL SetPos(CDuiControlExtensionBase* child, RECT rc) { return FALSE; }
//
//	virtual BOOL EstimateSize(CDuiControlExtensionBase* child, SIZE szAvailable, SIZE* pRet) { return FALSE; }
//	virtual BOOL OnPaintContent(CDuiControlExtensionBase* child, HDC hdc, RECT rcPaint, BOOL bDraw, InDrawData& idd) { return FALSE; }
//};


//////////////////////////////////////////////////////////////////////////
//


//////////////////////////////////////////////////////////////////////////
//


//////////////////////////////////////////////////////////////////////////
// CDuiOnChildWidthChanged 这个类没有任何成员，唯一的作用就是如果某个控件希望接收直接子控件宽度变化的通知，它应该在CAST时返回一个有效的指针

class CDuiOnChildWidthChanged
{
	DECLARE_BASE_CLASS_CAST(CDuiOnChildWidthChanged)
};

//////////////////////////////////////////////////////////////////////////
//

typedef HDE (CALLBACK* FINDCONTROLPROC)(HDE, LPVOID);

typedef void (CALLBACK* EVENTCALLBACK)(HDE, DuiEvent&);


//////////////////////////////////////////////////////////////////////////
// 滚动条基类。每个容器都有一个滚动条属性。暂不扩展

class CDuiScrollbarBase
{
public:
	virtual HDE GetOwner() const = 0;
	virtual BOOL SetScrollRange(int nRange, BOOL bRedraw=FALSE) = 0; // 返回TRUE表示显示状态发生了变化
	virtual int GetScrollPos() const = 0;
	virtual void SetScrollPos(int nPos, BOOL bRedraw=FALSE) = 0;
	//virtual int GetScrollPage() const = 0;
	virtual int GetScrollbarWidth() const = 0;
	virtual void ShowScrollbar(BOOL bShow) = 0;
	virtual void EnableScrollbar(BOOL bEnable) = 0;
	virtual BOOL IsValid() const = 0;

	virtual BOOL IsScrollEvent(DuiEvent& event) = 0; // 不处理的事件应该返回FALSE
};

//////////////////////////////////////////////////////////////////////////
// 布局模式基类。每个容器类控件都有一个布局模式

//class CDuiLayoutModeBase : public IControlExtension,
//						public CDuiSinkIsMyEvent,
//						public CDuiSinkOnUpdateLayout,
//						public CDuiSinkOnChildStateChanged
//{
//	DECLARE_BASE_CLASS_CAST(CDuiLayoutModeBase, CDuiSinkIsMyEvent, CDuiSinkOnUpdateLayout, CDuiSinkOnChildStateChanged)
//public:
//
//	//virtual BOOL OnUpdateLayout() = 0;
//	//virtual BOOL IsLayoutEvent(DuiEvent& event) = 0; // 不处理的事件应该返回FALSE
//	//virtual BOOL OnChildStateChanged(CDuiControlExtensionBase* pChild, DWORD dwStateMask) = 0;
//};

//////////////////////////////////////////////////////////////////////////
// 外观视觉基类。控件可以使用视觉样式，它负责边框和背景的绘制

//class CDuiVisualBase : public CDuiExtension
//{
//	DECLARE_BASE_CLASS_CAST(CDuiVisualBase)
//public:
//	//virtual void DeleteThis() = 0; // delete this
//	virtual CDuiControlExtensionBase* GetOwner() const = 0;
//	virtual void SetOwner(CDuiControlExtensionBase* owner) = 0;
//
//	virtual RECT GetAdjustRect() const = 0;
//	// 以下的绘制函数如果需要调用默认绘制，应该直接返回FALSE
//	virtual BOOL OnPaintBorder(HDC hdc, RECT rcBorder) = 0;
//	virtual BOOL OnPaintBackground(HDC hdc, RECT rcBkgnd) = 0;
//	virtual BOOL OnPaintFocus(HDC hdc, RECT rcFocus) = 0;
//};

#endif // __DIRECTUI_H__