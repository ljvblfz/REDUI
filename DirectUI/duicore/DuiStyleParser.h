#ifndef __DUISTYLEPARSER_H__
#define __DUISTYLEPARSER_H__

#pragma once

#ifndef NO3D
#include <d3d9types.h>
#endif // NO3D
#include "DuiManager.h"

class CDuiStyle;

//////////////////////////////////////////////////////////////////////////
//
// 如果 bRead=TRUE，表示读取属性值，返回到 v 里
#define DECLARE_STYLE_PARSER(classname) \
	class CDuiStyleParser_##classname \
	{ \
	public: \
		static BOOL Parse(CDuiLayoutManager* pLayoutMgr, CDuiStyle* pStyle, CComVariant& v, BOOL bRead=FALSE); \
	};

#define IMPLEMENT_STYLE_PARSER(classname) \
	BOOL CDuiStyleParser_##classname::Parse

//////////////////////////////////////////////////////////////////////////
// type == BOOL/long/double/COLORREF
#define IMPLEMENT_SIMPLE_STYLE_PARSER(sat, type) \
	IMPLEMENT_STYLE_PARSER(sat)(CDuiLayoutManager* pLayoutMgr, CDuiStyle* pStyle, CComVariant& v, BOOL bRead/*=FALSE*/) \
	{ \
		if (bRead) return StyleToVariant(pStyle, SAT_##sat, v); \
		if (v.vt!=VT_BSTR) return VariantToStyle(pStyle, SAT_##sat, v); \
		CDuiStyleVariant* pRet = pStyle->GetPrivateAttribute(SAT_##sat, TRUE); \
		ATLASSERT(pRet); \
		type val; \
		if (!TryLoad_##type##_FromString(v.bstrVal, val)) \
			return FALSE; \
		return *pRet = val, pStyle->FireChangeEvent(pRet), TRUE; \
	}

#define IMPLEMENT_AUTONUMBER_STYLE_PARSER(sat) \
	IMPLEMENT_STYLE_PARSER(sat)(CDuiLayoutManager* pLayoutMgr, CDuiStyle* pStyle, CComVariant& v, BOOL bRead/*=FALSE*/) \
	{ \
		if (bRead) \
		{ \
			CDuiStyleVariant* pRet = pStyle->GetPrivateAttribute(SAT_##sat, FALSE); \
			if (pRet==NULL) return FALSE; \
			switch (pRet->svt) \
			{ \
			case SVT_AUTO: \
			case SVT_LONG: \
			case SVT_PERCENT: return StyleToVariant(pRet, v); \
			} \
			return FALSE; \
		} \
		CDuiStyleVariant* pRet = pStyle->GetPrivateAttribute(SAT_##sat, TRUE); \
		ATLASSERT(pRet); \
		if (v.vt==VT_BSTR) \
		{ \
			if (lstrcmpiW(V_BSTR(&v), L"auto") == 0) \
				return pRet->svt=SVT_AUTO, pStyle->FireChangeEvent(pRet), TRUE; \
			LONG l; \
			if (TryLoad_long_FromString(V_BSTR(&v), l)) \
				return *pRet = l, pStyle->FireChangeEvent(pRet), TRUE; \
			double d; \
			if (TryLoad_double_FromString(V_BSTR(&v), d) && d>=0.0 && d<=1.0) \
				return *pRet = d, pRet->svt=SVT_PERCENT, pStyle->FireChangeEvent(pRet), TRUE; \
		} \
		else switch (v.vt) \
		{ \
		case VT_R4: \
		case VT_R8: \
		case VT_I4: return VariantToStyle(pStyle, SAT_##sat, v); \
		} \
		return FALSE; \
	} 

//////////////////////////////////////////////////////////////////////////
//

#define __ForeachStyle(V) \
	V(background, Background, BACKGROUND) \
	V(foreground, Foreground, FOREGROUND) \
	V(color, Color, COLOR) \
	V(glowcolor, GLOWCOLOR, GLOWCOLOR) \
	V(glowthick, GLOWTHICK, GLOWTHICK) \
	V(font, Font, FONT) \
	V(font-family, FontFamily, _UNKNOWN) \
	V(font-size, FontSize, _UNKNOWN) \
	V(font-style, FontStyle, _UNKNOWN) \
	V(cursor, Cursor, CURSOR) \
	V(line-distance, LINE_DISTANCE, LINE_DISTANCE) \
	V(hittest-as, HitTestAs, HITTEST_AS) \
	V(flag, Flag, FLAG) \
	V(margin, Margin, MARGIN) \
	V(margin-left, MarginLeft, _UNKNOWN) \
	V(margin-top, MarginTop, _UNKNOWN) \
	V(margin_right, MarginRight, _UNKNOWN) \
	V(margin_bottom, MarginBottom, _UNKNOWN) \
	V(padding, Padding, PADDING) \
	V(padding-left, PaddingLeft, _UNKNOWN) \
	V(padding-top, PaddingTop, _UNKNOWN) \
	V(padding-right, PaddingRight, _UNKNOWN) \
	V(padding-bottom, PaddingBottom, _UNKNOWN) \
	V(border, Border, BORDER) \
	V(border-width, BorderWidth, _UNKNOWN) \
	V(border-style, BorderStyle, _UNKNOWN) \
	V(border-color, BorderColor, _UNKNOWN) \
	V(width, WIDTH, WIDTH) \
	V(height, HEIGHT, HEIGHT) \
	V(min-width, MINWIDTH, MINWIDTH) \
	V(min-height, MINHEIGHT, MINHEIGHT) \
	V(left, LEFT, LEFT) \
	V(right, RIGHT, RIGHT) \
	V(top, TOP, TOP) \
	V(bottom, BOTTOM, BOTTOM) \
	V(position, Position, POSITION) \
	V(z-index, ZINDEX, ZINDEX) \
	V(text-style, TextStyle, TEXT_STYLE) \
	V(stretch, Stretch, STRETCH) \
	V(dialog-item-pos, DialogItemPos, DIALOG_ITEM_POS) \
	V(columns, COLUMNS, COLUMNS) \
	V(overflow, Overflow, OVERFLOW) \
	V(overflow-include-offset, OVERFLOW_INCLUDE_OFFSET, OVERFLOW_INCLUDE_OFFSET) \
	V(display, Display, DISPLAY) \
	V(layout, Layout, LAYOUT) \
	V(visual, Visual, VISUAL) \
	V(filter, Filter, FILTER) \
	V(clip, Clip, CLIP) \

//////////////////////////////////////////////////////////////////////////
//
typedef CSimpleArray<CStdString> CStrArray;

BOOL SplitStringToArray(LPCOLESTR lpszData, CStrArray& aStrings, LPCOLESTR lpszTokens=NULL, BOOL bRemoveQuoto=TRUE);

HRESULT ObjectInitFromString(IDispatch* pDisp, LPCOLESTR szInit);

BOOL ParseBoolString(LPCOLESTR lpszData, BOOL bDef=FALSE);
DWORD TryLoadStateFromString(LPCOLESTR lpszValue);
BOOL StyleParseTextStyle(CDuiLayoutManager* pLayoutMgr, CDuiStyle* pStyle, LPCOLESTR lpszValue, LONG& l);
BOOL StyleParseRect(CDuiLayoutManager* pLayoutMgr, CDuiStyle* pStyle, LPCOLESTR lpszValue, RECT& rc, BOOL bNeedFull=FALSE);

BOOL TryLoad_long_FromString(LPCOLESTR lpszData, long& l);
BOOL TryLoad_float_FromString(LPCOLESTR lpszData, float& f);
BOOL TryLoad_double_FromString(LPCOLESTR lpszData, double& d);
BOOL TryLoad_COLORREF_FromString(LPCOLESTR lpszData, COLORREF& clr);

#ifndef NO3D
BOOL TryLoad_D3DCOLORVALUE_FromString(LPCOLESTR lpszData, D3DCOLORVALUE& clr);
#endif // NO3D

BOOL TryLoad_floatptr_FromString(LPCOLESTR lpszData, float* v, int num);
BOOL TryLoadFontFaceFromString(LPCOLESTR lpszData, LOGFONTW& lf, LONG& mask);
BOOL TryLoadFontStyleFromString(LPCOLESTR lpszData, LOGFONTW& lf, LONG& mask);
BOOL TryLoadFontSizeFromString(LPCOLESTR lpszData, LOGFONTW& lf, LONG& mask);
//BOOL TryLoad_COLORREF_FromString(LPCOLESTR lpszData, COLORREF& clr);

//////////////////////////////////////////////////////////////////////////
//

DECLARE_STYLE_PARSER(Color);
DECLARE_STYLE_PARSER(Background);
DECLARE_STYLE_PARSER(Foreground);

DECLARE_STYLE_PARSER(Font);
DECLARE_STYLE_PARSER(FontFamily);
DECLARE_STYLE_PARSER(FontSize);
DECLARE_STYLE_PARSER(FontStyle);

DECLARE_STYLE_PARSER(LINE_DISTANCE);

DECLARE_STYLE_PARSER(Margin);
DECLARE_STYLE_PARSER(MarginLeft);
DECLARE_STYLE_PARSER(MarginTop);
DECLARE_STYLE_PARSER(MarginRight);
DECLARE_STYLE_PARSER(MarginBottom);

DECLARE_STYLE_PARSER(Padding);
DECLARE_STYLE_PARSER(PaddingLeft);
DECLARE_STYLE_PARSER(PaddingTop);
DECLARE_STYLE_PARSER(PaddingRight);
DECLARE_STYLE_PARSER(PaddingBottom);

DECLARE_STYLE_PARSER(Border);
DECLARE_STYLE_PARSER(BorderWidth);
DECLARE_STYLE_PARSER(BorderStyle);
DECLARE_STYLE_PARSER(BorderColor);

//DECLARE_STYLE_PARSER(Width);
//DECLARE_STYLE_PARSER(Height);
//DECLARE_STYLE_PARSER(MinWidth);
//DECLARE_STYLE_PARSER(MinHeight);
DECLARE_STYLE_PARSER(GLOWCOLOR);
DECLARE_STYLE_PARSER(GLOWTHICK);
DECLARE_STYLE_PARSER(WIDTH);
DECLARE_STYLE_PARSER(HEIGHT);
DECLARE_STYLE_PARSER(MINWIDTH);
DECLARE_STYLE_PARSER(MINHEIGHT);
DECLARE_STYLE_PARSER(LEFT);
DECLARE_STYLE_PARSER(TOP);
DECLARE_STYLE_PARSER(RIGHT);
DECLARE_STYLE_PARSER(BOTTOM);
DECLARE_STYLE_PARSER(Position);
DECLARE_STYLE_PARSER(ZINDEX);

//DECLARE_STYLE_PARSER(Scroll);

DECLARE_STYLE_PARSER(TextStyle);

DECLARE_STYLE_PARSER(Cursor);
DECLARE_STYLE_PARSER(Flag);

DECLARE_STYLE_PARSER(HitTestAs);

DECLARE_STYLE_PARSER(Stretch);
DECLARE_STYLE_PARSER(DialogItemPos);
DECLARE_STYLE_PARSER(COLUMNS);

DECLARE_STYLE_PARSER(Overflow);
DECLARE_STYLE_PARSER(OVERFLOW_INCLUDE_OFFSET);
DECLARE_STYLE_PARSER(Display);
DECLARE_STYLE_PARSER(Layout);
DECLARE_STYLE_PARSER(Visual);
DECLARE_STYLE_PARSER(Filter);
DECLARE_STYLE_PARSER(Clip);


#endif // __DUISTYLEPARSER_H__