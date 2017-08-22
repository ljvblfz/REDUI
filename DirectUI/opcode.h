

#ifndef __OPCODE_H__
#define __OPCODE_H__

///////////////////////////////////////////////////////////////
// !!!本文件来自于“雨寒工作室”代码生成系统，请不要手动修改！
//   有任何疑问，请联系雨寒工作室室主、REDUI原创作者 胡柏华 的邮箱：
//				yuhanstudio@gmail.com 或 hubaihua@gmail.com

// 文件版本号定义
#define OPCODE_VERSION 101

// 文件第一部分是插件操作码定义

/*
 *==========================================================================================
	REDUI 插件可实现的操作函数说明：

		每个操作函数对应一个操作码(OpCode)，ReduiPlugin 依靠操作码来派发到正确的插件类实现的函数，并自动转换参数。
		对于插件制作者来说，绝大多数情况下操作码是一个无需关心的事物，插件只需要实现对应的操作函数即可
		实现方法非常简单，如果你想实现某个操作，只需根据下面的操作函数注释，找到所需的函数，
	从“函数原型”里复制函数原型到你的派生自 ReduiPlugin 的类中，然后实现这个函数即可。

	所有被支持的操作函数在下面列出，格式如下：
	* 【函数名称】  - 【类别名称】 类别，【函数功能简介】
			函数原型：
					【返回值类型】 【函数名称】(【参数类型 参数变量名】, 【参数类型 参数变量名】, ...);
			返回值：
					【返回值类型】  - 【返回值说明】
			参数：
					【参数类型 参数变量名】  - 【参数说明】
			备注：
					【函数备注说明】


	* Test  - Global 类别，
			函数原型：
					int Test(int n1, int n2, int n3);
			返回值：
					int
			参数：
					int n1  - 
					int n2  - 
					int n3  - 

	* GetNormalName  - Global 类别，根据别名获取标准名称，可用于拥有别名的 XML 标签、布局名、外观名等等
			函数原型：
					LPCWSTR GetNormalName(LPCWSTR szAlias, LPCWSTR szNameType);
			返回值：
					LPCWSTR  - 返回一个字符串指针，指向标准名称
			参数：
					LPCWSTR szAlias  - 名称，可以是标准名称或者别名
					LPCWSTR szNameType  - 命名系统类型
									"xml" 表示 XML 标签名。如果此参数为 NULL，系统默认是 "xml"。
									"layout" 表示布局名。
									"visual" 表示外观名。
									也可以是任意类型名称，系统不保证一定能识别，用户插件可自行识别。
			
			备注：
					一个 XML 标签名、或布局名、或外观名、或其它类型的命名系统，可能存在不同的别名，调用此操作获得标准名称

	* QueryVisual  - Global 类别，查询命名的外观样式接口
			函数原型：
					IVisual* QueryVisual(LPCWSTR szVisualName);
			返回值：
					IVisual*  - 返回外观样式接口。如果插件未实现此命名的外观样式，返回 NULL
			参数：
					LPCWSTR szVisualName  - 外观样式名称

	* QueryLayout  - Global 类别，查询命名的布局样式接口
			函数原型：
					ILayout* QueryLayout(LPCWSTR szLayoutName);
			返回值：
					ILayout*  - 返回布局样式接口。如果插件未实现此命名的布局样式，返回 NULL
			参数：
					LPCWSTR szLayoutName  - 布局样式名称

	* PaintBackground  - Paint 类别，绘制控件背景
			函数原型：
					BOOL PaintBackground(HDE hde, HDC hdc, RECT rcBkgnd);
			返回值：
					BOOL
			参数：
					HDE hde  - 控件句柄
					HDC hdc  - 设备上下文
					RECT rcBkgnd  - 绘制的矩形区域
			备注：
					系统默认实现将调用 DefPaintBackground。如果插件不希望调用默认实现，应该返回 TRUE。

	* PaintForeground  - Paint 类别，绘制控件前景，前景在背景、子控件绘制之后才绘制
			函数原型：
					BOOL PaintForeground(HDE hde, HDC hdc, RECT rcFrgnd);
			返回值：
					BOOL
			参数：
					HDE hde  - 控件句柄
					HDC hdc  - 设备上下文
					RECT rcFrgnd  - 绘制的矩形区域
			备注：
					系统默认实现将调用 DefPaintForeground。如果插件不希望调用默认实现，应该返回 TRUE。

	* PaintBorder  - Paint 类别，绘制控件边框
			函数原型：
					BOOL PaintBorder(HDE hde, HDC hdc, RECT rcBorder);
			返回值：
					BOOL
			参数：
					HDE hde  - 控件句柄
					HDC hdc  - 设备上下文
					RECT rcBorder  - 绘制的矩形区域
			备注：
					系统默认实现将调用 DefPaintBorder。如果插件不希望调用默认实现，应该返回 TRUE。

	* PaintFocus  - Paint 类别，绘制控件聚焦边框线，默认是比边框线稍小的虚线框
			函数原型：
					BOOL PaintFocus(HDE hde, HDC hdc, RECT rcFocus, HRGN hrgn);
			返回值：
					BOOL
			参数：
					HDE hde  - 控件句柄
					HDC hdc  - 设备上下文
					RECT rcFocus  - 绘制的矩形区域
					HRGN hrgn  - 在此区域内绘制。如果此参数是 NULL，则采用 rcFocus 指示的矩形，否则忽略 rcFocus
			备注：
					系统默认实现将调用 DefPaintFocus。如果插件不希望调用默认实现，应该返回 TRUE。

	* LoadXML  - Window 类别，加载 XML 描述
			函数原型：
					BOOL LoadXML(HWND hwnd, HDE hde);
			返回值：
					BOOL
			参数：
					HWND hwnd  - 窗口句柄
					HDE hde  - hde参数含义

	* AttachExtensions  - Element 类别，在此处为控件添加扩展
			函数原型：
					DuiVoid AttachExtensions(HDE hde, IExtensionContainer* pContainer);
			返回值：
					DuiVoid
			参数：
					HDE hde  - 控件句柄
					IExtensionContainer* pContainer  - 控件的扩展容器接口，利用该接口可附加控件扩展
			备注：
					当控件被创建且已解析所有的 XML 属性后，才执行此操作。
						* 请注意，所有控件扩展实例都是附加到控件上的，即必须一个扩展实例对应一个控件，不能把一个扩展实例同时附加到多个控件上。
						* 扩展实例被调用者创建，但由容器自动删除。当控件销毁时，或者 IExtensionContainer::DetachExtension() 被调用时，容器会调用 IExtension::DeleteThis() 方法删除控件。
			

	* OnUpdateLayout  - Element 类别，更新控件的布局
			函数原型：
					BOOL OnUpdateLayout(HDE hde);
			返回值：
					BOOL
			参数：
					HDE hde  - 控件句柄
			备注：
					系统默认实现将调用 DefUpdateLayout。如果插件不希望调用默认实现，应该返回 TRUE。

	* GetClipRegion  - Element 类别，获取控件裁剪区域
			函数原型：
					HRGN GetClipRegion(HDE hde, RECT rcBorder);
			返回值：
					HRGN
			参数：
					HDE hde  - 控件句柄
					RECT rcBorder  - 当前控件所属的边界矩形，区域需以此作为参照
			备注：
					当控件被绘制时，默认以 rcBorder 指示的矩形作为裁剪区域。如果不处理，或者返回 NULL 值，则使用默认值。

	* GetChildrenClipRegion  - Element 类别，获取控件的裁剪区域，此区域仅作用于作为子控件容器的部分，不包含控件自身的部分，例如边框。
			函数原型：
					HRGN GetChildrenClipRegion(HDE hde, RECT rcContent);
			返回值：
					HRGN
			参数：
					HDE hde  - 控件句柄
					RECT rcContent  - 当前控件所属的客户区矩形，客户区矩形是控件的一部分，用来绘制子控件。区域需以此作为参照
			备注：
					当控件被绘制时，默认以 rcContent 指示的矩形作为容器裁剪区域。如果不处理，或者返回 NULL 值，则使用默认值。

	* IsMyEvent  - Sink 类别，事件处理事件
			函数原型：
					BOOL IsMyEvent(HDE hde, DuiEvent* pEvent);
			返回值：
					BOOL
			参数：
					HDE hde  - 控件句柄
					DuiEvent* pEvent  - 
			备注：
					如果事件被处理，返回TRUE，否则返回FALSE

	* OnGetControlFlags  - Sink 类别，控件标志位获取事件
			函数原型：
					UINT OnGetControlFlags(HDE hde);
			返回值：
					UINT
			参数：
					HDE hde  - 控件句柄
			备注：
					获取控件的控制标志位

	* OnAddChildNode  - Sink 类别，如果某个XML子节点是父控件某要素的专属子节点，实现此方法的要素应该返回TRUE，将不再解析成子控件
			函数原型：
					BOOL OnAddChildNode(HDE hde, HANDLE hChild);
			返回值：
					BOOL
			参数：
					HDE hde  - 控件句柄
					HANDLE hChild  - 
			备注：
					父控件决定子节点是控件还是内部资源

	* OnStyleChanged  - Sink 类别，控件样式属性改变事件
			函数原型：
					DuiVoid OnStyleChanged(HDE hde, DuiStyleVariant* newval);
			返回值：
					DuiVoid
			参数：
					HDE hde  - 控件句柄
					DuiStyleVariant* newval  - 
			备注：
					控件私有样式属性发生了改变，改变的值在参数里

	* OnStateChanged  - Sink 类别，控件状态改变事件
			函数原型：
					DuiVoid OnStateChanged(HDE hde, DWORD dwStateMask);
			返回值：
					DuiVoid
			参数：
					HDE hde  - 控件句柄
					DWORD dwStateMask  - 
			备注：
					控件状态发生了改变

	* OnChildStateChanged  - Sink 类别，子控件状态改变事件
			函数原型：
					BOOL OnChildStateChanged(HDE hde, HDE hChild, DWORD dwStateMask);
			返回值：
					BOOL
			参数：
					HDE hde  - 控件句柄
					HDE hChild  - 
					DWORD dwStateMask  - 
			备注：
					子控件的状态发生了改变，参数掩码表示发生改变的状态位，并非状态位的值

	* OnAttributeChanged  - Sink 类别，控件属性改变事件
			函数原型：
					DuiVoid OnAttributeChanged(HDE hde, LPCWSTR szAttr);
			返回值：
					DuiVoid
			参数：
					HDE hde  - 控件句柄
					LPCWSTR szAttr  - 
			备注：
					控件属性发生了改变，参数表示改变的属性名称

	* OnChildAttributeChanged  - Sink 类别，子控件属性改变事件
			函数原型：
					DuiVoid OnChildAttributeChanged(HDE hde, HDE hChild, LPCWSTR szAttr);
			返回值：
					DuiVoid
			参数：
					HDE hde  - 控件句柄
					HDE hChild  - 
					LPCWSTR szAttr  - 
			备注：
					子控件的属性发生了改变

	* OnAttributeRemoved  - Sink 类别，控件属性已删除事件
			函数原型：
					DuiVoid OnAttributeRemoved(HDE hde, LPCWSTR szAttr);
			返回值：
					DuiVoid
			参数：
					HDE hde  - 控件句柄
					LPCWSTR szAttr  - 
			备注：
					控件属性已删除

	* OnChildAttributeRemoved  - Sink 类别，子控件属性已删除事件
			函数原型：
					DuiVoid OnChildAttributeRemoved(HDE hde, HDE hChild, LPCWSTR szAttr);
			返回值：
					DuiVoid
			参数：
					HDE hde  - 控件句柄
					HDE hChild  - 
					LPCWSTR szAttr  - 
			备注：
					子控件属性已删除

	* OnChildAdded  - Sink 类别，子控件增加事件
			函数原型：
					DuiVoid OnChildAdded(HDE hde, HDE hChild);
			返回值：
					DuiVoid
			参数：
					HDE hde  - 控件句柄
					HDE hChild  - 
			备注：
					子控件已经添加了

	* OnChildRemoved  - Sink 类别，子控件删除事件
			函数原型：
					DuiVoid OnChildRemoved(HDE hde, HDE hChild);
			返回值：
					DuiVoid
			参数：
					HDE hde  - 控件句柄
					HDE hChild  - 
			备注：
					子控件即将被删除

	* OnPrepareDC  - Sink 类别，准备设备上下文环境
			函数原型：
					BOOL OnPrepareDC(HDE hde, HDC hdc);
			返回值：
					BOOL
			参数：
					HDE hde  - 控件句柄
					HDC hdc  - 
			备注：
					控件自己决定如何准备HDC的初始设置，如果已处理返回TRUE， 否则返回FALSE

	* OnScroll  - Sink 类别，控件滚动条位置改变事件
			函数原型：
					DuiVoid OnScroll(HDE hde, HDE hScroll, BOOL bVert);
			返回值：
					DuiVoid
			参数：
					HDE hde  - 控件句柄
					HDE hScroll  - 发生滚动的控件句柄
					BOOL bVert  - TRUE 垂直滚动条，FALSE 水平滚动条
			备注：
					控件滚动位置发生了改变，这个事件会发给 hScroll 控件及其所有子控件

	* OnGetContentSize  - Sink 类别，控件大小估算事件
			函数原型：
					DuiVoid OnGetContentSize(HDE hde, SIZE* psz);
			返回值：
					DuiVoid
			参数：
					HDE hde  - 控件句柄
					SIZE* psz  - 
			备注：
					实现该事件的扩展必须自己提供内容大小的估算，内容大小指控件客户区大小，并非整个控件大小

	* OnUpdate3DSurface  - Sink 类别，3D表面更新事件
			函数原型：
					DuiVoid OnUpdate3DSurface(HDE hde, RECT rc);
			返回值：
					DuiVoid
			参数：
					HDE hde  - 控件句柄
					RECT rc  - 
			备注：
					扩展提供自己的创建/更新3D表面函数

	* On3DSurfaceCreated  - Sink 类别，3D表面已创建
			函数原型：
					DuiVoid On3DSurfaceCreated(HDE hde);
			返回值：
					DuiVoid
			参数：
					HDE hde  - 控件句柄
			备注：
					控件的3D表面已创建。此事件在 onload 之后触发

	* OnUpdateInlineRegion  - Sink 类别，内联控件更新区域事件
			函数原型：
					DuiVoid OnUpdateInlineRegion(HDE hde);
			返回值：
					DuiVoid
			参数：
					HDE hde  - 控件句柄
			备注：
					内联控件的扩展更新自己的区域和矩形

	* OnReposInlineRect  - Sink 类别，内联控件位置重新调整事件
			函数原型：
					DuiVoid OnReposInlineRect(HDE hde, int index, long cx, InDrawData* pidd);
			返回值：
					DuiVoid
			参数：
					HDE hde  - 控件句柄
					int index  - 
					long cx  - 
					InDrawData* pidd  - 
			备注：
					内联控件对当前行重新调整偏移和大小，index表示缓存矩形的索引，cx表示水平偏移值，高度调整参考idd.cyMinHeight

	* Activate  - Sink 类别，控件激活事件
			函数原型：
					BOOL Activate(HDE hde);
			返回值：
					BOOL
			参数：
					HDE hde  - 控件句柄
			备注：
					当鼠标点击控件或对聚焦控件按回车键时，控件将产生激活事件。
						如果控件已激活返回 TRUE，控件未激活返回 FALSE
						默认情况下，不可见或者已禁用的控件会返回 FALSE，其它返回 TRUE。

	* SetAttribute  - Sink 类别，设置控件属性
			函数原型：
					DuiVoid SetAttribute(HDE hde, LPCWSTR szName, VARIANT* value);
			返回值：
					DuiVoid
			参数：
					HDE hde  - 控件句柄
					LPCWSTR szName  - 属性名称
					VARIANT* value  - 属性值
			备注：
					当解析XML标签或者外部调用控件的设置属性方法时会触发此事件

	* GetAttribute  - Sink 类别，读取控件属性
			函数原型：
					DuiVoid GetAttribute(HDE hde, LPCWSTR szName, VARIANT* value);
			返回值：
					DuiVoid
			参数：
					HDE hde  - 控件句柄
					LPCWSTR szName  - 属性名称
					VARIANT* value  - 输出的属性值
			备注：
					当外部调用控件的读取属性方法时会触发此事件，如果插件或扩展拥有此名称的属性值，需要保存到 VARIANT 变量中。
						如果VARIANT变量已经有值，必须首先清空。

	* GetStyleAttribute  - Sink 类别，读取控件样式属性
			函数原型：
					DuiStyleVariant* GetStyleAttribute(HDE hde, DuiStyleAttributeType sat, DuiStyleVariantType svt, DWORD dwMatchMask);
			返回值：
					DuiStyleVariant*
			参数：
					HDE hde  - 控件句柄
					DuiStyleAttributeType sat  - 样式属性类型
					DuiStyleVariantType svt  - 样式变量类型。如果未强行指定，通常是 SVT_EMPTY
					DWORD dwMatchMask  - 用于样式匹配的条件掩码。如果未强行指定，通常是 0
			备注：
					当控件需要读取样式属性值时触发此事件。

	* OnPaintContent  - Sink 类别，绘制控件内容
			函数原型：
					DuiVoid OnPaintContent(HDE hde, HDC hdc, RECT rcPaint, BOOL bDraw, InDrawData* pidd);
			返回值：
					DuiVoid
			参数：
					HDE hde  - 控件句柄
					HDC hdc  - 设备上下文
					RECT rcPaint  - 控件绘制矩形区域
					BOOL bDraw  - 是实际绘制还是大小试算
					InDrawData* pidd  - 绘制上下文
			备注：
					控件内容绘制事件，不包括背景、边框、滚动条、前景。
						如果 bDraw 值为 FALSE，则无需真实绘制，只需试算后修正 pidd 中的值。
						绘制过程中通常需要调用子控件的绘制函数。

 *==========================================================================================
*/

// 操作码 = 操作类别码 + 操作功能码
#define MAKEOPCODE(opcat, opnumber) ((DWORD)(((WORD)(((DWORD_PTR)(opnumber)) & 0xffff)) | ((DWORD)((WORD)(((DWORD_PTR)(opcat)) & 0xffff))) << 16))

// 操作类别码定义
enum
{
	OpCatalog_Global = 0,
	OpCatalog_Paint,
	OpCatalog_Window,
	OpCatalog_Element,
	OpCatalog_Xml,
	OpCatalog_Sink		// Sink 类别的所有操作都有对应的同名事件槽接口。接口用于控件扩展，这里的操作用于插件。扩展先于插件执行。
};

// 操作功能码定义
enum
{
	// Global
	OpNumber_Global_Query = 0,
	OpNumber_Test,
	OpNumber_GetNormalName,
	OpNumber_QueryVisual,
	OpNumber_QueryLayout,
	// Paint
	OpNumber_Paint_Query = 0,
	OpNumber_PaintBackground,
	OpNumber_PaintForeground,
	OpNumber_PaintBorder,
	OpNumber_PaintFocus,
	// Window
	OpNumber_Window_Query = 0,
	OpNumber_LoadXML,
	// Element
	OpNumber_Element_Query = 0,
	OpNumber_AttachExtensions,
	OpNumber_OnUpdateLayout,
	OpNumber_GetClipRegion,
	OpNumber_GetChildrenClipRegion,
	// Xml
	OpNumber_Xml_Query = 0,
	// Sink
	OpNumber_Sink_Query = 0,
	OpNumber_IsMyEvent,
	OpNumber_OnGetControlFlags,
	OpNumber_OnAddChildNode,
	OpNumber_OnStyleChanged,
	OpNumber_OnStateChanged,
	OpNumber_OnChildStateChanged,
	OpNumber_OnAttributeChanged,
	OpNumber_OnChildAttributeChanged,
	OpNumber_OnAttributeRemoved,
	OpNumber_OnChildAttributeRemoved,
	OpNumber_OnChildAdded,
	OpNumber_OnChildRemoved,
	OpNumber_OnPrepareDC,
	OpNumber_OnScroll,
	OpNumber_OnGetContentSize,
	OpNumber_OnUpdate3DSurface,
	OpNumber_On3DSurfaceCreated,
	OpNumber_OnUpdateInlineRegion,
	OpNumber_OnReposInlineRect,
	OpNumber_Activate,
	OpNumber_SetAttribute,
	OpNumber_GetAttribute,
	OpNumber_GetStyleAttribute,
	OpNumber_OnPaintContent,
};

// 操作码定义(操作类别码和操作功能码的组合)
#define OpCode_Test			MAKEOPCODE(OpCatalog_Global, OpNumber_Test)
#define OpCode_GetNormalName			MAKEOPCODE(OpCatalog_Global, OpNumber_GetNormalName)
#define OpCode_QueryVisual			MAKEOPCODE(OpCatalog_Global, OpNumber_QueryVisual)
#define OpCode_QueryLayout			MAKEOPCODE(OpCatalog_Global, OpNumber_QueryLayout)
#define OpCode_PaintBackground			MAKEOPCODE(OpCatalog_Paint, OpNumber_PaintBackground)
#define OpCode_PaintForeground			MAKEOPCODE(OpCatalog_Paint, OpNumber_PaintForeground)
#define OpCode_PaintBorder			MAKEOPCODE(OpCatalog_Paint, OpNumber_PaintBorder)
#define OpCode_PaintFocus			MAKEOPCODE(OpCatalog_Paint, OpNumber_PaintFocus)
#define OpCode_LoadXML			MAKEOPCODE(OpCatalog_Window, OpNumber_LoadXML)
#define OpCode_AttachExtensions			MAKEOPCODE(OpCatalog_Element, OpNumber_AttachExtensions)
#define OpCode_OnUpdateLayout			MAKEOPCODE(OpCatalog_Element, OpNumber_OnUpdateLayout)
#define OpCode_GetClipRegion			MAKEOPCODE(OpCatalog_Element, OpNumber_GetClipRegion)
#define OpCode_GetChildrenClipRegion			MAKEOPCODE(OpCatalog_Element, OpNumber_GetChildrenClipRegion)
#define OpCode_IsMyEvent			MAKEOPCODE(OpCatalog_Sink, OpNumber_IsMyEvent)
#define OpCode_OnGetControlFlags			MAKEOPCODE(OpCatalog_Sink, OpNumber_OnGetControlFlags)
#define OpCode_OnAddChildNode			MAKEOPCODE(OpCatalog_Sink, OpNumber_OnAddChildNode)
#define OpCode_OnStyleChanged			MAKEOPCODE(OpCatalog_Sink, OpNumber_OnStyleChanged)
#define OpCode_OnStateChanged			MAKEOPCODE(OpCatalog_Sink, OpNumber_OnStateChanged)
#define OpCode_OnChildStateChanged			MAKEOPCODE(OpCatalog_Sink, OpNumber_OnChildStateChanged)
#define OpCode_OnAttributeChanged			MAKEOPCODE(OpCatalog_Sink, OpNumber_OnAttributeChanged)
#define OpCode_OnChildAttributeChanged			MAKEOPCODE(OpCatalog_Sink, OpNumber_OnChildAttributeChanged)
#define OpCode_OnAttributeRemoved			MAKEOPCODE(OpCatalog_Sink, OpNumber_OnAttributeRemoved)
#define OpCode_OnChildAttributeRemoved			MAKEOPCODE(OpCatalog_Sink, OpNumber_OnChildAttributeRemoved)
#define OpCode_OnChildAdded			MAKEOPCODE(OpCatalog_Sink, OpNumber_OnChildAdded)
#define OpCode_OnChildRemoved			MAKEOPCODE(OpCatalog_Sink, OpNumber_OnChildRemoved)
#define OpCode_OnPrepareDC			MAKEOPCODE(OpCatalog_Sink, OpNumber_OnPrepareDC)
#define OpCode_OnScroll			MAKEOPCODE(OpCatalog_Sink, OpNumber_OnScroll)
#define OpCode_OnGetContentSize			MAKEOPCODE(OpCatalog_Sink, OpNumber_OnGetContentSize)
#define OpCode_OnUpdate3DSurface			MAKEOPCODE(OpCatalog_Sink, OpNumber_OnUpdate3DSurface)
#define OpCode_On3DSurfaceCreated			MAKEOPCODE(OpCatalog_Sink, OpNumber_On3DSurfaceCreated)
#define OpCode_OnUpdateInlineRegion			MAKEOPCODE(OpCatalog_Sink, OpNumber_OnUpdateInlineRegion)
#define OpCode_OnReposInlineRect			MAKEOPCODE(OpCatalog_Sink, OpNumber_OnReposInlineRect)
#define OpCode_Activate			MAKEOPCODE(OpCatalog_Sink, OpNumber_Activate)
#define OpCode_SetAttribute			MAKEOPCODE(OpCatalog_Sink, OpNumber_SetAttribute)
#define OpCode_GetAttribute			MAKEOPCODE(OpCatalog_Sink, OpNumber_GetAttribute)
#define OpCode_GetStyleAttribute			MAKEOPCODE(OpCatalog_Sink, OpNumber_GetStyleAttribute)
#define OpCode_OnPaintContent			MAKEOPCODE(OpCatalog_Sink, OpNumber_OnPaintContent)

// 操作函数列表多用途工具宏
#define __foreach_op(opfunc, doprearg, prearg, doprearg2, prearg2, arg) \
	opfunc ( int, Test, doprearg( prearg(int,n1) prearg(int,n2) prearg(int,n3)), doprearg2( prearg2(int,n1) prearg2(int,n2) prearg2(int,n3)), arg(int,n1), arg(int,n2), arg(int,n3) ) \
	opfunc ( LPCWSTR, GetNormalName, doprearg( prearg(LPCWSTR,szAlias) prearg(LPCWSTR,szNameType)), doprearg2( prearg2(LPCWSTR,szAlias) prearg2(LPCWSTR,szNameType)), arg(LPCWSTR,szAlias), arg(LPCWSTR,szNameType) ) \
	opfunc ( IVisual*, QueryVisual, doprearg( prearg(LPCWSTR,szVisualName)), doprearg2( prearg2(LPCWSTR,szVisualName)), arg(LPCWSTR,szVisualName) ) \
	opfunc ( ILayout*, QueryLayout, doprearg( prearg(LPCWSTR,szLayoutName)), doprearg2( prearg2(LPCWSTR,szLayoutName)), arg(LPCWSTR,szLayoutName) ) \
	opfunc ( BOOL, PaintBackground, doprearg( prearg(HDE,hde) prearg(HDC,hdc) prearg(RECT,rcBkgnd)), doprearg2( prearg2(HDE,hde) prearg2(HDC,hdc) prearg2(RECT,rcBkgnd)), arg(HDE,hde), arg(HDC,hdc), arg(RECT,rcBkgnd) ) \
	opfunc ( BOOL, PaintForeground, doprearg( prearg(HDE,hde) prearg(HDC,hdc) prearg(RECT,rcFrgnd)), doprearg2( prearg2(HDE,hde) prearg2(HDC,hdc) prearg2(RECT,rcFrgnd)), arg(HDE,hde), arg(HDC,hdc), arg(RECT,rcFrgnd) ) \
	opfunc ( BOOL, PaintBorder, doprearg( prearg(HDE,hde) prearg(HDC,hdc) prearg(RECT,rcBorder)), doprearg2( prearg2(HDE,hde) prearg2(HDC,hdc) prearg2(RECT,rcBorder)), arg(HDE,hde), arg(HDC,hdc), arg(RECT,rcBorder) ) \
	opfunc ( BOOL, PaintFocus, doprearg( prearg(HDE,hde) prearg(HDC,hdc) prearg(RECT,rcFocus) prearg(HRGN,hrgn)), doprearg2( prearg2(HDE,hde) prearg2(HDC,hdc) prearg2(RECT,rcFocus) prearg2(HRGN,hrgn)), arg(HDE,hde), arg(HDC,hdc), arg(RECT,rcFocus), arg(HRGN,hrgn) ) \
	opfunc ( BOOL, LoadXML, doprearg( prearg(HWND,hwnd) prearg(HDE,hde)), doprearg2( prearg2(HWND,hwnd) prearg2(HDE,hde)), arg(HWND,hwnd), arg(HDE,hde) ) \
	opfunc ( DuiVoid, AttachExtensions, doprearg( prearg(HDE,hde) prearg(IExtensionContainer*,pContainer)), doprearg2( prearg2(HDE,hde) prearg2(IExtensionContainer*,pContainer)), arg(HDE,hde), arg(IExtensionContainer*,pContainer) ) \
	opfunc ( BOOL, OnUpdateLayout, doprearg( prearg(HDE,hde)), doprearg2( prearg2(HDE,hde)), arg(HDE,hde) ) \
	opfunc ( HRGN, GetClipRegion, doprearg( prearg(HDE,hde) prearg(RECT,rcBorder)), doprearg2( prearg2(HDE,hde) prearg2(RECT,rcBorder)), arg(HDE,hde), arg(RECT,rcBorder) ) \
	opfunc ( HRGN, GetChildrenClipRegion, doprearg( prearg(HDE,hde) prearg(RECT,rcContent)), doprearg2( prearg2(HDE,hde) prearg2(RECT,rcContent)), arg(HDE,hde), arg(RECT,rcContent) ) \
	opfunc ( BOOL, IsMyEvent, doprearg(prearg(HDE,hde) prearg(DuiEvent*,pEvent)), doprearg2(prearg2(HDE,hde) prearg2(DuiEvent*,pEvent)), arg(HDE,hde), arg(DuiEvent*,pEvent) ) \
	opfunc ( UINT, OnGetControlFlags, doprearg(prearg(HDE,hde)), doprearg2(prearg2(HDE,hde)), arg(HDE,hde) ) \
	opfunc ( BOOL, OnAddChildNode, doprearg(prearg(HDE,hde) prearg(HANDLE,hChild)), doprearg2(prearg2(HDE,hde) prearg2(HANDLE,hChild)), arg(HDE,hde), arg(HANDLE,hChild) ) \
	opfunc ( DuiVoid, OnStyleChanged, doprearg(prearg(HDE,hde) prearg(DuiStyleVariant*,newval)), doprearg2(prearg2(HDE,hde) prearg2(DuiStyleVariant*,newval)), arg(HDE,hde), arg(DuiStyleVariant*,newval) ) \
	opfunc ( DuiVoid, OnStateChanged, doprearg(prearg(HDE,hde) prearg(DWORD,dwStateMask)), doprearg2(prearg2(HDE,hde) prearg2(DWORD,dwStateMask)), arg(HDE,hde), arg(DWORD,dwStateMask) ) \
	opfunc ( BOOL, OnChildStateChanged, doprearg(prearg(HDE,hde) prearg(HDE,hChild) prearg(DWORD,dwStateMask)), doprearg2(prearg2(HDE,hde) prearg2(HDE,hChild) prearg2(DWORD,dwStateMask)), arg(HDE,hde), arg(HDE,hChild), arg(DWORD,dwStateMask) ) \
	opfunc ( DuiVoid, OnAttributeChanged, doprearg(prearg(HDE,hde) prearg(LPCWSTR,szAttr)), doprearg2(prearg2(HDE,hde) prearg2(LPCWSTR,szAttr)), arg(HDE,hde), arg(LPCWSTR,szAttr) ) \
	opfunc ( DuiVoid, OnChildAttributeChanged, doprearg(prearg(HDE,hde) prearg(HDE,hChild) prearg(LPCWSTR,szAttr)), doprearg2(prearg2(HDE,hde) prearg2(HDE,hChild) prearg2(LPCWSTR,szAttr)), arg(HDE,hde), arg(HDE,hChild), arg(LPCWSTR,szAttr) ) \
	opfunc ( DuiVoid, OnAttributeRemoved, doprearg(prearg(HDE,hde) prearg(LPCWSTR,szAttr)), doprearg2(prearg2(HDE,hde) prearg2(LPCWSTR,szAttr)), arg(HDE,hde), arg(LPCWSTR,szAttr) ) \
	opfunc ( DuiVoid, OnChildAttributeRemoved, doprearg(prearg(HDE,hde) prearg(HDE,hChild) prearg(LPCWSTR,szAttr)), doprearg2(prearg2(HDE,hde) prearg2(HDE,hChild) prearg2(LPCWSTR,szAttr)), arg(HDE,hde), arg(HDE,hChild), arg(LPCWSTR,szAttr) ) \
	opfunc ( DuiVoid, OnChildAdded, doprearg(prearg(HDE,hde) prearg(HDE,hChild)), doprearg2(prearg2(HDE,hde) prearg2(HDE,hChild)), arg(HDE,hde), arg(HDE,hChild) ) \
	opfunc ( DuiVoid, OnChildRemoved, doprearg(prearg(HDE,hde) prearg(HDE,hChild)), doprearg2(prearg2(HDE,hde) prearg2(HDE,hChild)), arg(HDE,hde), arg(HDE,hChild) ) \
	opfunc ( BOOL, OnPrepareDC, doprearg(prearg(HDE,hde) prearg(HDC,hdc)), doprearg2(prearg2(HDE,hde) prearg2(HDC,hdc)), arg(HDE,hde), arg(HDC,hdc) ) \
	opfunc ( DuiVoid, OnScroll, doprearg(prearg(HDE,hde) prearg(HDE,hScroll) prearg(BOOL,bVert)), doprearg2(prearg2(HDE,hde) prearg2(HDE,hScroll) prearg2(BOOL,bVert)), arg(HDE,hde), arg(HDE,hScroll), arg(BOOL,bVert) ) \
	opfunc ( DuiVoid, OnGetContentSize, doprearg(prearg(HDE,hde) prearg(SIZE*,psz)), doprearg2(prearg2(HDE,hde) prearg2(SIZE*,psz)), arg(HDE,hde), arg(SIZE*,psz) ) \
	opfunc ( DuiVoid, OnUpdate3DSurface, doprearg(prearg(HDE,hde) prearg(RECT,rc)), doprearg2(prearg2(HDE,hde) prearg2(RECT,rc)), arg(HDE,hde), arg(RECT,rc) ) \
	opfunc ( DuiVoid, On3DSurfaceCreated, doprearg(prearg(HDE,hde)), doprearg2(prearg2(HDE,hde)), arg(HDE,hde) ) \
	opfunc ( DuiVoid, OnUpdateInlineRegion, doprearg(prearg(HDE,hde)), doprearg2(prearg2(HDE,hde)), arg(HDE,hde) ) \
	opfunc ( DuiVoid, OnReposInlineRect, doprearg(prearg(HDE,hde) prearg(int,index) prearg(long,cx) prearg(InDrawData*,pidd)), doprearg2(prearg2(HDE,hde) prearg2(int,index) prearg2(long,cx) prearg2(InDrawData*,pidd)), arg(HDE,hde), arg(int,index), arg(long,cx), arg(InDrawData*,pidd) ) \
	opfunc ( BOOL, Activate, doprearg(prearg(HDE,hde)), doprearg2(prearg2(HDE,hde)), arg(HDE,hde) ) \
	opfunc ( DuiVoid, SetAttribute, doprearg(prearg(HDE,hde) prearg(LPCWSTR,szName) prearg(VARIANT*,value)), doprearg2(prearg2(HDE,hde) prearg2(LPCWSTR,szName) prearg2(VARIANT*,value)), arg(HDE,hde), arg(LPCWSTR,szName), arg(VARIANT*,value) ) \
	opfunc ( DuiVoid, GetAttribute, doprearg(prearg(HDE,hde) prearg(LPCWSTR,szName) prearg(VARIANT*,value)), doprearg2(prearg2(HDE,hde) prearg2(LPCWSTR,szName) prearg2(VARIANT*,value)), arg(HDE,hde), arg(LPCWSTR,szName), arg(VARIANT*,value) ) \
	opfunc ( DuiStyleVariant*, GetStyleAttribute, doprearg(prearg(HDE,hde) prearg(DuiStyleAttributeType,sat) prearg(DuiStyleVariantType,svt) prearg(DWORD,dwMatchMask)), doprearg2(prearg2(HDE,hde) prearg2(DuiStyleAttributeType,sat) prearg2(DuiStyleVariantType,svt) prearg2(DWORD,dwMatchMask)), arg(HDE,hde), arg(DuiStyleAttributeType,sat), arg(DuiStyleVariantType,svt), arg(DWORD,dwMatchMask) ) \
	opfunc ( DuiVoid, OnPaintContent, doprearg(prearg(HDE,hde) prearg(HDC,hdc) prearg(RECT,rcPaint) prearg(BOOL,bDraw) prearg(InDrawData*,pidd)), doprearg2(prearg2(HDE,hde) prearg2(HDC,hdc) prearg2(RECT,rcPaint) prearg2(BOOL,bDraw) prearg2(InDrawData*,pidd)), arg(HDE,hde), arg(HDC,hdc), arg(RECT,rcPaint), arg(BOOL,bDraw), arg(InDrawData*,pidd) ) \


/////////////////////////////////////////////////////////////////////////////////
// 文件第二部分是扩展接口、基础结构定义

// ISink... 是事件响应槽接口定义。
// 事件响应槽接口通常不单独实现，而是在扩展中实现，并通过IExtension::CastToClass()获得
// 事件响应槽接口是公开接口，为提高字符串比较性能，采用数字式类名，范围是 500-999
// 扩展接口不同于插件，通常是多实例接口，而插件往往只需要一个全局实例即可。
// 为了使插件也能实现扩展，在扩展方法里设定第一个参数为一个句柄，代表当前来源调用者，普通扩展可忽略此参数

#ifndef HDE
typedef HANDLE HDE;
#endif

// 事件结构定义
typedef struct tagDuiEvent
{
	LPCOLESTR Name; // HIWORD(Name)==0 is DUI_EVENTTYPE, HIWORD(Name)!=0 is event name string
	HDE pSender;
	DWORD dwTimestamp;
	POINT ptMouse;
	OLECHAR chKey;
	WORD wKeyState;
	LPCOLESTR pPropertyName;
	WPARAM wParam; // 如果事件来自控件内部，这个参数值可能包含 DISPPARAMS* 作为回调函数的参数
	LPARAM lParam;
	BOOL cancelBubble;
	BOOL returnValue; // TRUE-stop continue, default is FALSE
} DuiEvent;

// 控件绘制的上下文参数结构
typedef struct
{
	HDE owner; // rc 和 pt 是关联到此控件客户区的
	RECT rc;
	POINT pt;				// 当前绘制坐标
	LONG cyCurLine;			// 当前行的文字高度
	LONG cyMinHeight;		// 当前行的最小高度 >= 文字高度
	LONG cyTotalHeight;		// 总高度
	LONG lLineIndent;		// 缩进
	LONG lLineDistance;		// 行间距，换行时高度应加上 cyMinHeight + lLineDistance
	UINT uTextStyle;		// 文本样式

	LPVOID pvReserved;
} InDrawData;

//////////////////////////////////////////////////////////////////////////
// 样式属性类型
typedef enum
{
	SAT__UNKNOWN = 0,
	SAT_COLOR,				// color(COLOR)
	SAT_GLOWCOLOR,	// glow color(COLOR), default(0)
	SAT_GLOWTHICK,	// glow thick(LONG), default(1)
	SAT_FONT,				// font(FONT)
	SAT_TEXT_STYLE,			// text-style(LONG)
	SAT_CURSOR,				// cursor(CURSOR)
	SAT_LINE_DISTANCE,		// line-distance(LONG)

	SAT__NO_PARENT,			// ---- 以下的内容表示属性不可以从父元素中继承

	SAT_BACKGROUND,			// background(COLOR | ICON | BITMAP | EXTERNAL)
	SAT_FOREGROUND,			// foreground(COLOR | ICON | BITMAP | EXTERNAL)
	SAT_FLAG,				// flag(LONG) DUIFLAG_TABSTOP|WANTRETURN
	SAT_HITTEST_AS,			// hittest_as(LONG) // return while WM_NCHITTEST

	SAT_MARGIN,				// margin(RECT)
	SAT_PADDING,			// padding(RECT)
	SAT_BORDER,				// border(PEN)

	SAT_WIDTH,				// width(LONG/PERCENT/AUTO)
	SAT_HEIGHT,				// height(LONG/PERCENT/AUTO)
	SAT_MINWIDTH,			// min-width(LONG/PERCENT)
	SAT_MINHEIGHT,			// min-height(LONG/PERCENT)

	// left/top/right/bottom/width/height用于position样式，计算时参照被参考对象的各个方向的偏移值，例如left参照左边，top参照上边，right参照右边,bottom参照底边
	// 可以使用auto作为值，前提是能用其它的值算出来，例如 left:10;width:50;right:auto; 表示right = left + width
	SAT_LEFT,				// left(LONG/PERCENT/AUTO)
	SAT_TOP,				// top(LONG/PERCENT/AUTO)
	SAT_RIGHT,				// right(LONG/PERCENT/AUTO)
	SAT_BOTTOM,				// bottom(LONG/PERCENT/AUTO)
	SAT_POSITION,			// position(LONG), static(default)/relative/absolute/fixed
	SAT_ZINDEX,				// z-index(LONG), default:0

	SAT_STRETCH,			// stretch mode(LONG) when parent control's layout mode is dialog
	SAT_DIALOG_ITEM_POS,	// dailog item rect(RECT) when parent control's layout mode is dialog
	SAT_COLUMNS,			// columns(LONG) when parent control's layout mode is tile

	SAT_OVERFLOW,			// overflow(LONG) // auto(default) hidden scroll
	SAT_OVERFLOW_INCLUDE_OFFSET,	// overflow-include-offset(BOOL) // false(default)
	SAT_DISPLAY,			// display(LONG) // inline(default) inlineblock block none
	SAT_LAYOUT,				// layout(STRING) // horizontal vertical tile dialog inline(default) // only for block/inlineblock
	SAT_VISUAL,				// visual(STRING) // button checkbox ...
	SAT_FILTER,				// filter(STRING) // wipe ...
	SAT_CLIP,				// clip(STRING) // roundrect custom...
} DuiStyleAttributeType;

// Style variant type
typedef enum
{
	SVT_EMPTY = 0,
	SVT_BOOL = 0x00000001<<0,			// true|yes|1, false|no|0
	SVT_LONG = 0x00000001<<1,			// 123
	SVT_DOUBLE = 0x00000001<<2,			// 123.456
	SVT_PERCENT = 0x00000001<<3,		// 40%
	SVT_STRING = 0x00000001<<4,			// "abcd"
	SVT_EXPRESSION = 0x00000001<<5,		// "abc()"
	SVT_COLOR = 0x00000001<<6,			// [ThemeName ]ColorType, #080808
	SVT_PEN = 0x00000001<<7,			// {ThemeName ]ColorType
	SVT_FONT = 0x00000001<<8,			// [ThemeName ]FontType
	SVT_BRUSH = 0x00000001<<9,
	SVT_ICON = 0x00000001<<10,
	SVT_BITMAP = 0x00000001<<11,
	SVT_CURSOR = 0x00000001<<12,
	SVT_RECT = 0x00000001<<13,			// left top right bottom
	SVT_SIZE = 0x00000001<<14,			// cx cy
	SVT_POINT = 0x00000001<<15,			// x y
	SVT_EXTERNAL = 0x00000001<<16,		// external extension, such as GDI+::Image

	// auto value
	SVT_AUTO = 0x00000001<<31,			// auto is a "value", it has no real value.

	// font mask
	SVT_FONT_MASK_FAMILY	= 0x01000000,
	SVT_FONT_MASK_SIZE		= 0x02000000,
	SVT_FONT_MASK_STYLE		= 0x04000000,
	SVT_FONT_MASK_ALL		= (SVT_FONT_MASK_FAMILY | SVT_FONT_MASK_SIZE | SVT_FONT_MASK_STYLE),
	// pen mask
	SVT_PEN_MASK_WIDTH		= 0x01000000,
	SVT_PEN_MASK_STYLE		= 0x02000000,
	SVT_PEN_MASK_COLOR		= 0x04000000,
	SVT_PEN_MASK_ALL		= (SVT_PEN_MASK_WIDTH | SVT_PEN_MASK_STYLE | SVT_PEN_MASK_COLOR),
	// all mask
	SVT_MASK_ALL			= 0x00FFFFFF,
} DuiStyleVariantType;

// Style variant
typedef struct tagDuiStyleVariant
{
	DuiStyleAttributeType sat;
	DuiStyleVariantType svt;

	union
	{
		BOOL boolVal;		// svt == SVT_BOOL
		LONG longVal;		// svt == SVT_LONG
		DOUBLE doubleVal;	// svt == SVT_DOUBLE or svt == SVT_PERCENT
		LPCOLESTR strVal;	// svt == SVT_STRING or svt == SVT_EXPRESSION
		COLORREF colorVal;	// svt == SVT_COLOR
		LPLOGPEN penVal;		// svt == SVT_PEN
		LPLOGFONTW fontVal;		// svt == SVT_FONT
		HBRUSH brushVal;	// svt == SVT_BRUSH
		HICON iconVal;		// svt == SVT_ICON or svt == SVT_CURSOR
		HBITMAP bitmapVal;	// svt == SVT_BITMAP
		//HCURSOR cursorVal;	// svt == SVT_CURSOR
		LPRECT rectVal;		// svt == SVT_RECT
		LPSIZE sizeVal;		// svt == SVT_SIZE
		LPPOINT pointVal;	// svt == SVT_POINT
		LPVOID extVal;		// svt == SVT_EXTERNAL
	};
} DuiStyleVariant;

// style variant API
DuiStyleVariant* WINAPI StyleVariantCreate();
BOOL WINAPI StyleVariantInit(DuiStyleVariant* pVar);
BOOL WINAPI StyleVariantClear(DuiStyleVariant* pVar);

BOOL WINAPI StyleVariantIsEmpty(DuiStyleVariant* pVar);
HPEN WINAPI StyleVariantGetPen(DuiStyleVariant* pVar, LPLOGPEN pRefPen);
void WINAPI StyleVariantSetPen(DuiStyleVariant* pVal, LPLOGPEN pRefPen, LONG mask);
HFONT WINAPI StyleVariantGetFont(DuiStyleVariant* pVar, LPLOGFONTW pRefFont);
void WINAPI StyleVariantSetFont(DuiStyleVariant* pVar, LPLOGFONTW pRefFont, LONG mask);
BOOL WINAPI StyleVariantGetLogBrush(DuiStyleVariant* pVar, LPLOGBRUSH pLogBrush);
BOOL WINAPI StyleVariantGetBitmap(DuiStyleVariant* pVar, LPBITMAP pBitmap);
LPVOID WINAPI StyleVariantGetExternal(DuiStyleVariant* pVar);


// 事件响应槽数字式命名定义
enum SinkName
{
	IntName__First = 500, // 事件接口命名范围是 500 - 999
	IntName_IsMyEvent,
	IntName_OnGetControlFlags,
	IntName_OnAddChildNode,
	IntName_OnStyleChanged,
	IntName_OnStateChanged,
	IntName_OnChildStateChanged,
	IntName_OnAttributeChanged,
	IntName_OnChildAttributeChanged,
	IntName_OnAttributeRemoved,
	IntName_OnChildAttributeRemoved,
	IntName_OnChildAdded,
	IntName_OnChildRemoved,
	IntName_OnPrepareDC,
	IntName_OnScroll,
	IntName_OnGetContentSize,
	IntName_OnUpdate3DSurface,
	IntName_On3DSurfaceCreated,
	IntName_OnUpdateInlineRegion,
	IntName_OnReposInlineRect,
	IntName_Activate,
	IntName_SetAttribute,
	IntName_GetAttribute,
	IntName_GetStyleAttribute,
	IntName_OnPaintContent,

	IntName__Last = 999
};

#define MakeIntName(n) (LPCWSTR)((ULONG_PTR)(WORD)(n))
#define HasIntName(x) public: static LPCWSTR __MyName() { return MakeIntName(IntName_##x); }

// 系统支持的事件响应槽接口定义


// 事件处理事件
class ISinkIsMyEvent
{
	HasIntName(IsMyEvent)
public:
	/* --
		如果事件被处理，返回TRUE，否则返回FALSE
	-- */
	virtual BOOL IsMyEvent(HANDLE hSource, DuiEvent* pEvent) = 0;
};

// 控件标志位获取事件
class ISinkOnGetControlFlags
{
	HasIntName(OnGetControlFlags)
public:
	/* --
		获取控件的控制标志位
	-- */
	virtual UINT OnGetControlFlags(HANDLE hSource) = 0;
};

// 如果某个XML子节点是父控件某要素的专属子节点，实现此方法的要素应该返回TRUE，将不再解析成子控件
class ISinkOnAddChildNode
{
	HasIntName(OnAddChildNode)
public:
	/* --
		父控件决定子节点是控件还是内部资源
	-- */
	virtual BOOL OnAddChildNode(HANDLE hSource, HANDLE hChild) = 0;
};

// 控件样式属性改变事件
class ISinkOnStyleChanged
{
	HasIntName(OnStyleChanged)
public:
	/* --
		控件私有样式属性发生了改变，改变的值在参数里
	-- */
	virtual void OnStyleChanged(HANDLE hSource, DuiStyleVariant* newval) = 0;
};

// 控件状态改变事件
class ISinkOnStateChanged
{
	HasIntName(OnStateChanged)
public:
	/* --
		控件状态发生了改变
	-- */
	virtual void OnStateChanged(HANDLE hSource, DWORD dwStateMask) = 0;
};

// 子控件状态改变事件
class ISinkOnChildStateChanged
{
	HasIntName(OnChildStateChanged)
public:
	/* --
		子控件的状态发生了改变，参数掩码表示发生改变的状态位，并非状态位的值
	-- */
	virtual BOOL OnChildStateChanged(HANDLE hSource, HDE hChild, DWORD dwStateMask) = 0;
};

// 控件属性改变事件
class ISinkOnAttributeChanged
{
	HasIntName(OnAttributeChanged)
public:
	/* --
		控件属性发生了改变，参数表示改变的属性名称
	-- */
	virtual void OnAttributeChanged(HANDLE hSource, LPCWSTR szAttr) = 0;
};

// 子控件属性改变事件
class ISinkOnChildAttributeChanged
{
	HasIntName(OnChildAttributeChanged)
public:
	/* --
		子控件的属性发生了改变
	-- */
	virtual void OnChildAttributeChanged(HANDLE hSource, HDE hChild, LPCWSTR szAttr) = 0;
};

// 控件属性已删除事件
class ISinkOnAttributeRemoved
{
	HasIntName(OnAttributeRemoved)
public:
	/* --
		控件属性已删除
	-- */
	virtual void OnAttributeRemoved(HANDLE hSource, LPCWSTR szAttr) = 0;
};

// 子控件属性已删除事件
class ISinkOnChildAttributeRemoved
{
	HasIntName(OnChildAttributeRemoved)
public:
	/* --
		子控件属性已删除
	-- */
	virtual void OnChildAttributeRemoved(HANDLE hSource, HDE hChild, LPCWSTR szAttr) = 0;
};

// 子控件增加事件
class ISinkOnChildAdded
{
	HasIntName(OnChildAdded)
public:
	/* --
		子控件已经添加了
	-- */
	virtual void OnChildAdded(HANDLE hSource, HDE hChild) = 0;
};

// 子控件删除事件
class ISinkOnChildRemoved
{
	HasIntName(OnChildRemoved)
public:
	/* --
		子控件即将被删除
	-- */
	virtual void OnChildRemoved(HANDLE hSource, HDE hChild) = 0;
};

// 准备设备上下文环境
class ISinkOnPrepareDC
{
	HasIntName(OnPrepareDC)
public:
	/* --
		控件自己决定如何准备HDC的初始设置，如果已处理返回TRUE， 否则返回FALSE
	-- */
	virtual BOOL OnPrepareDC(HANDLE hSource, HDC hdc) = 0;
};

// 控件滚动条位置改变事件
class ISinkOnScroll
{
	HasIntName(OnScroll)
public:
	/* --
		控件滚动位置发生了改变，这个事件会发给 hScroll 控件及其所有子控件
	-- */
	virtual void OnScroll(HANDLE hSource, HDE hScroll, BOOL bVert) = 0;
};

// 控件大小估算事件
class ISinkOnGetContentSize
{
	HasIntName(OnGetContentSize)
public:
	/* --
		实现该事件的扩展必须自己提供内容大小的估算，内容大小指控件客户区大小，并非整个控件大小
	-- */
	virtual void OnGetContentSize(HANDLE hSource, SIZE* psz) = 0;
};

// 3D表面更新事件
class ISinkOnUpdate3DSurface
{
	HasIntName(OnUpdate3DSurface)
public:
	/* --
		扩展提供自己的创建/更新3D表面函数
	-- */
	virtual void OnUpdate3DSurface(HANDLE hSource, RECT rc) = 0;
};

// 3D表面已创建
class ISinkOn3DSurfaceCreated
{
	HasIntName(On3DSurfaceCreated)
public:
	/* --
		控件的3D表面已创建。此事件在 onload 之后触发
	-- */
	virtual void On3DSurfaceCreated(HANDLE hSource) = 0;
};

// 内联控件更新区域事件
class ISinkOnUpdateInlineRegion
{
	HasIntName(OnUpdateInlineRegion)
public:
	/* --
		内联控件的扩展更新自己的区域和矩形
	-- */
	virtual void OnUpdateInlineRegion(HANDLE hSource) = 0;
};

// 内联控件位置重新调整事件
class ISinkOnReposInlineRect
{
	HasIntName(OnReposInlineRect)
public:
	/* --
		内联控件对当前行重新调整偏移和大小，index表示缓存矩形的索引，cx表示水平偏移值，高度调整参考idd.cyMinHeight
	-- */
	virtual void OnReposInlineRect(HANDLE hSource, int index, long cx, InDrawData* pidd) = 0;
};

// 控件激活事件
class ISinkActivate
{
	HasIntName(Activate)
public:
	/* --
		当鼠标点击控件或对聚焦控件按回车键时，控件将产生激活事件。
						如果控件已激活返回 TRUE，控件未激活返回 FALSE
						默认情况下，不可见或者已禁用的控件会返回 FALSE，其它返回 TRUE。
	-- */
	virtual BOOL Activate(HANDLE hSource) = 0;
};

// 设置控件属性
class ISinkSetAttribute
{
	HasIntName(SetAttribute)
public:
	/* --
		当解析XML标签或者外部调用控件的设置属性方法时会触发此事件
	-- */
	virtual void SetAttribute(HANDLE hSource, LPCWSTR szName, VARIANT* value) = 0;
};

// 读取控件属性
class ISinkGetAttribute
{
	HasIntName(GetAttribute)
public:
	/* --
		当外部调用控件的读取属性方法时会触发此事件，如果插件或扩展拥有此名称的属性值，需要保存到 VARIANT 变量中。
						如果VARIANT变量已经有值，必须首先清空。
	-- */
	virtual void GetAttribute(HANDLE hSource, LPCWSTR szName, VARIANT* value) = 0;
};

// 读取控件样式属性
class ISinkGetStyleAttribute
{
	HasIntName(GetStyleAttribute)
public:
	/* --
		当控件需要读取样式属性值时触发此事件。
	-- */
	virtual DuiStyleVariant* GetStyleAttribute(HANDLE hSource, DuiStyleAttributeType sat, DuiStyleVariantType svt, DWORD dwMatchMask) = 0;
};

// 绘制控件内容
class ISinkOnPaintContent
{
	HasIntName(OnPaintContent)
public:
	/* --
		控件内容绘制事件，不包括背景、边框、滚动条、前景。
						如果 bDraw 值为 FALSE，则无需真实绘制，只需试算后修正 pidd 中的值。
						绘制过程中通常需要调用子控件的绘制函数。
	-- */
	virtual void OnPaintContent(HANDLE hSource, HDC hdc, RECT rcPaint, BOOL bDraw, InDrawData* pidd) = 0;
};



#endif // __OPCODE_H__

