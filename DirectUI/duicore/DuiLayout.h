#pragma once

#include "DuiManager.h"

//////////////////////////////////////////////////////////////////////////
//

class CDuiLayout : public ControlExtension
{
public:
	DECLARE_CONTROL_EXTENSION(CDuiLayout)
	DECLARE_CLASS_CAST(CDuiLayout, ControlExtension)

	virtual void SetAttribute(HDE hde, LPCWSTR szName, VARIANT* value);
};

THIS_IS_CONTROL_EXTENSION(CDuiLayout, Layout)

//////////////////////////////////////////////////////////////////////////
//

class CDuiLayoutVertical : public ControlExtension
{
public:
	DECLARE_CONTROL_EXTENSION(CDuiLayoutVertical)
	DECLARE_CLASS_CAST(CDuiLayoutVertical, ControlExtension)
	BEGIN_CLASS_STYLE_MAP("layout:vertical")
	END_CLASS_STYLE_MAP()
};

THIS_IS_CONTROL_EXTENSION2(CDuiLayoutVertical, Layout.Vertical, VerticalLayout)

//////////////////////////////////////////////////////////////////////////
//

class CDuiLayoutHorizontal : public ControlExtension
{
public:
	DECLARE_CONTROL_EXTENSION(CDuiLayoutHorizontal)
	DECLARE_CLASS_CAST(CDuiLayoutHorizontal, ControlExtension)
	BEGIN_CLASS_STYLE_MAP("layout:horizontal")
	END_CLASS_STYLE_MAP()
};

THIS_IS_CONTROL_EXTENSION2(CDuiLayoutHorizontal, Layout.Horizontal, HorizontalLayout)

//////////////////////////////////////////////////////////////////////////
//

class CDuiLayoutTile : public ControlExtension
{
public:
	DECLARE_CONTROL_EXTENSION(CDuiLayoutTile)
	DECLARE_CLASS_CAST(CDuiLayoutTile, ControlExtension)
	BEGIN_CLASS_STYLE_MAP("layout:tile")
	END_CLASS_STYLE_MAP()
};

THIS_IS_CONTROL_EXTENSION2(CDuiLayoutTile, Layout.Tile, TileLayout)

//////////////////////////////////////////////////////////////////////////
//

class CDuiLayoutDialog : public ControlExtension
{
public:
	DECLARE_CONTROL_EXTENSION(CDuiLayoutDialog)
	DECLARE_CLASS_CAST(CDuiLayoutDialog, ControlExtension)
	BEGIN_CLASS_STYLE_MAP("layout:dialog")
	END_CLASS_STYLE_MAP()
};

THIS_IS_CONTROL_EXTENSION2(CDuiLayoutDialog, Layout.Dialog, DialogLayout)


/////////////////////////////////////////////////////////////////////////////////////
//

class  CDuiLayoutSplitter : public CDuiLayout
{
public:
	DECLARE_CONTROL_EXTENSION(CDuiLayoutSplitter)
	DECLARE_CLASS_CAST(CDuiLayoutSplitter, ControlExtension)

	//DECLARE_BASE_CLASS_STYLE("layout:v-splitter")
	virtual DuiStyleVariant* GetStyleAttribute(HDE hde, DuiStyleAttributeType sat, DuiStyleVariantType svt, DWORD dwMatchMask);
};

//THIS_IS_CONTROL_EXTENSION2(CDuiLayoutSplitter, Layout.Splitter, SplitterLayout)

//////////////////////////////////////////////////////////////////////////
//

class  CDuiLayoutVertSplitter : public CDuiLayoutSplitter
{
public:
	DECLARE_CONTROL_EXTENSION(CDuiLayoutVertSplitter)
	DECLARE_CLASS_CAST(CDuiLayoutVertSplitter, CDuiLayoutSplitter)
	DECLARE_BASE_CLASS_STYLE("layout:v-splitter; cursor:sizewe")
	//BEGIN_CLASS_STYLE_MAP("layout:v-splitter")
	//	STATE_STYLE(DUISTATE_HOT, "cursor:sizewe")
	//END_CLASS_STYLE_MAP()
};

THIS_IS_CONTROL_EXTENSION2(CDuiLayoutVertSplitter, Layout.VerticalSplitter, vSplitter)

//////////////////////////////////////////////////////////////////////////
//

class  CDuiLayoutHorzSplitter : public CDuiLayoutSplitter
{
public:
	DECLARE_CONTROL_EXTENSION(CDuiLayoutHorzSplitter)
	DECLARE_CLASS_CAST(CDuiLayoutHorzSplitter, CDuiLayoutSplitter)
	DECLARE_BASE_CLASS_STYLE("layout:h-splitter; cursor:sizens")
	//BEGIN_CLASS_STYLE_MAP("layout:h-splitter")
	//	STATE_STYLE(DUISTATE_HOT, "cursor:sizens")
	//END_CLASS_STYLE_MAP()
};

THIS_IS_CONTROL_EXTENSION2(CDuiLayoutHorzSplitter, Layout.HorizontalSplitter, hSplitter)

//////////////////////////////////////////////////////////////////////////
// Page 中只有一个子控件处于显示状态

class CDuiLayoutPage : public ControlExtension
{
public:
	DECLARE_CONTROL_EXTENSION(CDuiLayoutPage)
	DECLARE_CLASS_CAST(CDuiLayoutPage, ControlExtension)
	BEGIN_CLASS_STYLE_MAP("layout:page")
	END_CLASS_STYLE_MAP()
};

THIS_IS_CONTROL_EXTENSION2(CDuiLayoutPage, Layout.Page, PageLayout)

