#ifndef __DUIINLINE_H__
#define __DUIINLINE_H__

#pragma once

#include "DuiManager.h"

class CDuiInlineControlBase : public ControlExtension,
		public ISinkOnPrepareDC,
		public ISinkOnUpdateInlineRegion
{
	DECLARE_CLASS_NAME(CDuiInlineControlBase)
	DECLARE_CLASS_CAST(CDuiInlineControlBase, ControlExtension, ISinkOnPrepareDC, ISinkOnUpdateInlineRegion)
	DECLARE_BASE_CLASS_STYLE("display:inline")

	// ControlExtension
public:
	DuiNode* FindControl(FINDCONTROLPROC Proc, LPVOID pData, UINT uFlags);
	virtual BOOL OnPrepareDC(HANDLE, HDC) { return FALSE; }
	virtual void OnUpdateInlineRegion(HANDLE);
};

//////////////////////////////////////////////////////////////////////////
// InlineText 将把鼠标键盘消息交给父控件处理，同时style也使用父控件的

class CDuiInlineText : public CDuiInlineControlBase
{
	DECLARE_CONTROL_EXTENSION(CDuiInlineText)
	DECLARE_CLASS_CAST(CDuiInlineText, CDuiInlineControlBase)
	DECLARE_BASE_CLASS_STYLE("overflow:hidden")

public:
	virtual void OnPaintContent(HDE hde, HDC hdc, RECT rcPaint, BOOL bDraw, InDrawData* pidd);
	//virtual RECT GetPos(DUI_RECTTYPE rt=DUIRT_CONTENT) { return m_rcItem; }
	virtual void SetAttribute(HDE hde, LPCWSTR szName, VARIANT* value);
	virtual void GetAttribute(HDE hde, LPCWSTR szName, VARIANT* value);
	virtual DuiStyleVariant* GetStyleAttribute(HDE hde, DuiStyleAttributeType sat, DuiStyleVariantType svt=SVT_EMPTY, DWORD dwMatchMask=0);
};

THIS_IS_CONTROL_EXTENSION(CDuiInlineText, InlineText)


//////////////////////////////////////////////////////////////////////////
// Anchor

class CDuiAnchor : public CDuiInlineControlBase
{
	DECLARE_CONTROL_EXTENSION(CDuiAnchor)
	DECLARE_CONTROL_FLAGS(DUIFLAG_SETCURSOR|DUIFLAG_TABSTOP)
	DECLARE_CLASS_CAST(CDuiAnchor, CDuiInlineControlBase)

	BEGIN_CLASS_STYLE_MAP("color:#0000ff; cursor:hand; font-style:underline")
		STATE_STYLE(DUISTATE_HOT, "color:#ff0000")
	END_CLASS_STYLE_MAP()

public:
	virtual void OnPaintContent(HDE hde, HDC hdc, RECT rcPaint, BOOL bDraw, InDrawData* pidd);
	BOOL Activate(HDE hde);
};

THIS_IS_CONTROL_EXTENSION3(CDuiAnchor, Anchor, a, Link)

//////////////////////////////////////////////////////////////////////////
// Font  <font size=4 face="xxx" color="#xxx"></font>

class CDuiFont : public CDuiInlineControlBase
{
	DECLARE_CONTROL_EXTENSION(CDuiFont)
	DECLARE_CLASS_CAST(CDuiFont, CDuiInlineControlBase)

public:
	virtual void OnPaintContent(HDE hde, HDC hdc, RECT rcPaint, BOOL bDraw, InDrawData* pidd);
};

THIS_IS_CONTROL_EXTENSION2(CDuiFont, Font, f)


//////////////////////////////////////////////////////////////////////////
// Bold  <b>...</b>

class CDuiBold : public CDuiInlineControlBase
{
	DECLARE_CONTROL_EXTENSION(CDuiBold)
	DECLARE_CLASS_CAST(CDuiBold, CDuiInlineControlBase)

public:
	virtual void OnPaintContent(HDE hde, HDC hdc, RECT rcPaint, BOOL bDraw, InDrawData* pidd);
};

THIS_IS_CONTROL_EXTENSION3(CDuiBold, Bold, b, strong)

//////////////////////////////////////////////////////////////////////////
// Italic <i>...</i>

class CDuiItalic : public CDuiInlineControlBase
{
	DECLARE_CONTROL_EXTENSION(CDuiItalic)
	DECLARE_CLASS_CAST(CDuiItalic, CDuiInlineControlBase)

public:
	virtual void OnPaintContent(HDE hde, HDC hdc, RECT rcPaint, BOOL bDraw, InDrawData* pidd);
};

THIS_IS_CONTROL_EXTENSION2(CDuiItalic, Italic, i)


//////////////////////////////////////////////////////////////////////////
// Underline <u>...</u>

class CDuiUnderline : public CDuiInlineControlBase
{
	DECLARE_CONTROL_EXTENSION(CDuiUnderline)
	DECLARE_CLASS_CAST(CDuiUnderline, CDuiInlineControlBase)

public:
	virtual void OnPaintContent(HDE hde, HDC hdc, RECT rcPaint, BOOL bDraw, InDrawData* pidd);
};

THIS_IS_CONTROL_EXTENSION2(CDuiUnderline, Underline, u)


//////////////////////////////////////////////////////////////////////////
// Strike <s>...</s> 删除线 StrikeOut | Strike-through

class CDuiStrike : public CDuiInlineControlBase
{
	DECLARE_CONTROL_EXTENSION(CDuiStrike)
	DECLARE_CLASS_CAST(CDuiStrike, CDuiInlineControlBase)

public:
	virtual void OnPaintContent(HDE hde, HDC hdc, RECT rcPaint, BOOL bDraw, InDrawData* pidd);
};

THIS_IS_CONTROL_EXTENSION2(CDuiStrike, Strike, s)


//////////////////////////////////////////////////////////////////////////
// Big <big>...</big> 比当前字体更大的字体

class CDuiBig : public CDuiInlineControlBase
{
	DECLARE_CONTROL_EXTENSION(CDuiBig)
	DECLARE_CLASS_CAST(CDuiBig, CDuiInlineControlBase)

public:
	virtual void OnPaintContent(HDE hde, HDC hdc, RECT rcPaint, BOOL bDraw, InDrawData* pidd);
};

THIS_IS_CONTROL_EXTENSION(CDuiBig, Big)


//////////////////////////////////////////////////////////////////////////
// Small <small>...</small> 比当前字体更小的字体

class CDuiSmall : public CDuiInlineControlBase
{
	DECLARE_CONTROL_EXTENSION(CDuiSmall)
	DECLARE_CLASS_CAST(CDuiSmall, CDuiInlineControlBase)

public:
	virtual void OnPaintContent(HDE hde, HDC hdc, RECT rcPaint, BOOL bDraw, InDrawData* pidd);
};

THIS_IS_CONTROL_EXTENSION(CDuiSmall, Small)


//////////////////////////////////////////////////////////////////////////
// WhiteSpace  <w width=20 /> 产生一个20像素宽的空白，默认12像素

class CDuiWhiteSpace : public CDuiInlineControlBase
{
	DECLARE_CONTROL_EXTENSION(CDuiWhiteSpace)
	DECLARE_CLASS_CAST(CDuiWhiteSpace, CDuiInlineControlBase)

public:
	virtual void OnPaintContent(HDE hde, HDC hdc, RECT rcPaint, BOOL bDraw, InDrawData* pidd);
};

THIS_IS_CONTROL_EXTENSION2(CDuiWhiteSpace, WhiteSpace, w)


//////////////////////////////////////////////////////////////////////////
// Paragraph  <p lineSpace="5" indent="10">...</p>
//		lineSpace; // 行间距，默认5
//		indent; // 首行缩进的像素, 0-100， 默认0

class CDuiParagraph : public CDuiInlineControlBase
{
	DECLARE_CONTROL_EXTENSION(CDuiParagraph)
	DECLARE_CLASS_CAST(CDuiParagraph, CDuiInlineControlBase)

public:
	virtual void OnPaintContent(HDE hde, HDC hdc, RECT rcPaint, BOOL bDraw, InDrawData* pidd);
};

THIS_IS_CONTROL_EXTENSION2(CDuiParagraph, Paragraph, p)


//////////////////////////////////////////////////////////////////////////
// LineBreak  <br />

class CDuiLineBreak : public CDuiInlineControlBase
{
	DECLARE_CONTROL_EXTENSION(CDuiLineBreak)
	DECLARE_CLASS_CAST(CDuiLineBreak, CDuiInlineControlBase)

public:
	virtual void OnPaintContent(HDE hde, HDC hdc, RECT rcPaint, BOOL bDraw, InDrawData* pidd);
	virtual void OnUpdateInlineRegion(HANDLE) {} // Do nothing
};

THIS_IS_CONTROL_EXTENSION2(CDuiLineBreak, LineBreak, br)


//////////////////////////////////////////////////////////////////////////
// HorizontalLine  <hr width="20" color="#8f8f8f" /> 画一个20像素长的水平线
//		width; // 线的长度，如果是0，将直接画到最右边，默认是0
//		color; // 画笔颜色，默认灰色

class CDuiHorizontalLine : public CDuiInlineControlBase
			, public ISinkOnReposInlineRect
{
	DECLARE_CONTROL_EXTENSION(CDuiHorizontalLine)
	DECLARE_CLASS_CAST(CDuiHorizontalLine, CDuiInlineControlBase, ISinkOnReposInlineRect)

public:
	virtual void OnPaintContent(HDE hde, HDC hdc, RECT rcPaint, BOOL bDraw, InDrawData* pidd);
	virtual void OnReposInlineRect(HANDLE, int index, long cx, InDrawData* pidd);
};

THIS_IS_CONTROL_EXTENSION2(CDuiHorizontalLine, HorizontalLine, hr)


//////////////////////////////////////////////////////////////////////////
// Img  <img src="bmp1" width="20" height="20" />
//		width;	// 如果宽度为0，则用默认图片宽度
//		height;	// 如果高度为0，则用默认图片高度
//		src;	// 指示图片资源的ID

class CDuiImg : public CDuiInlineControlBase
	, public CDuiConnectorCallback
	//, public SINK_CLASS(OnUpdate3DSurface)
{
	DECLARE_CONTROL_EXTENSION(CDuiImg)
	DECLARE_CLASS_CAST(CDuiImg, CDuiInlineControlBase/*, SINK_CLASS(OnUpdate3DSurface)*/)
	DECLARE_BASE_CLASS_STYLE("min-width:0;min-height:0;")

public:
	~CDuiImg();
	virtual void OnPaintContent(HDE hde, HDC hdc, RECT rcPaint, BOOL bDraw, InDrawData* pidd);
	virtual void SetAttribute(HDE hde, LPCWSTR szName, VARIANT* value);

	// CDuiConnectorCallback method
	virtual BOOL OnHit(LPVOID pKey, LPVOID pVal, DWORD flag);

	// SINK_CLASS(OnCreate3DSurface) method
	//virtual void OnUpdate3DSurface(HANDLE, RECT rc);

protected:
	void __updateSize();
};

THIS_IS_CONTROL_EXTENSION(CDuiImg, Img)


#endif // __DUIINLINE_H__