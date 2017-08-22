#ifndef __DUILAYOUTMODE_H__
#define __DUILAYOUTMODE_H__

#pragma once

#include "DirectUI.h"
//#include "../DuiPlugin.h"

struct LayoutEntry
{
	LPCOLESTR szName;
	int lenName;
	ILayout* pLayout;
};

#pragma section("ReduiLayout$__a", read, shared)
#pragma section("ReduiLayout$__b", read, shared)
#pragma section("ReduiLayout$__c", read, shared)
extern "C"
{
	__declspec(selectany) __declspec(allocate("ReduiLayout$__a")) LayoutEntry* __playoutEntryFirst = NULL;
	__declspec(selectany) __declspec(allocate("ReduiLayout$__c")) LayoutEntry* __playoutEntryLast = NULL;
}
#pragma comment(linker, "/merge:ReduiLayout=.rdata")


#define THIS_IS_LAYOUT(class, name) \
	__declspec(selectany) class __layout_##class; \
	__declspec(selectany) LayoutEntry __layoutEntry_##class = {L#name, lstrlenW(L#name), &__layout_##class}; \
	extern "C" __declspec(allocate("ReduiLayout$__b")) __declspec(selectany) LayoutEntry* const __playout_##class = &__layoutEntry_##class; \
	__pragma(comment(linker, "/include:___playout_" #class));



ILayout* LayoutFromName(LPCOLESTR name);

// API function
ILayout* DUIAPI GetDefaultLayout();



//////////////////////////////////////////////////////////////////////////
//#include "DuiManager.h"

//////////////////////////////////////////////////////////////////////////
//
class DuiLayout : public ILayout
{
public:
	DECLARE_BASE_CLASS_CAST(DuiLayout)
	DECLARE_NO_DELETE_THIS(DuiLayout)
};

/////////////////////////////////////////////////////////////////////////////////////
//

class  LayoutVertical : public DuiLayout
{
	DECLARE_BASE_CLASS_CAST(LayoutVertical, DuiLayout)
public:
	virtual BOOL OnUpdateLayout(HDE hde);
};
THIS_IS_LAYOUT(LayoutVertical, Vertical)


/////////////////////////////////////////////////////////////////////////////////////
//

class  LayoutHorizontal : public DuiLayout,
			public CDuiOnChildWidthChanged
{
public:
	DECLARE_BASE_CLASS_CAST(LayoutHorizontal, DuiLayout, CDuiOnChildWidthChanged)

	virtual BOOL OnUpdateLayout(HDE hde);
};

THIS_IS_LAYOUT(LayoutHorizontal, Horizontal)


/////////////////////////////////////////////////////////////////////////////////////
//

class  LayoutTile : public DuiLayout
{
	DECLARE_BASE_CLASS_CAST(LayoutTile, DuiLayout)
public:

	virtual BOOL OnUpdateLayout(HDE hde);
};

THIS_IS_LAYOUT(LayoutTile, Tile)


/////////////////////////////////////////////////////////////////////////////////////
//

class  LayoutDialog : public DuiLayout
{
	DECLARE_BASE_CLASS_CAST(LayoutDialog, DuiLayout)
public:
	virtual BOOL OnUpdateLayout(HDE hde);

protected:
	struct DialogData
	{
		CRect rcDialog;
		CRect rcOldContainer;
	};

	void RecalcArea(HDE hde, DialogData& dd);
};

THIS_IS_LAYOUT(LayoutDialog, Dialog)

/////////////////////////////////////////////////////////////////////////////////////
//

class LayoutSplitter : public DuiLayout
{
protected:
	static const int m_cxy = 6; // 分隔条的宽度或高度（像素）
	struct SplitterData
	{
		LONG m_nPos;

		DWORD m_dwState;
		POINT m_ptCapture;
		LONG m_nPosCapture;
	};

public:
	DECLARE_BASE_CLASS_CAST(LayoutSplitter, DuiLayout)

	virtual void Use(HANDLE hSource);
	virtual void Unuse(HANDLE hSource);

	SplitterData* GetSplitterData(HDE hde);
	DWORD GetState(HDE hde);
};

class  LayoutVerticalSplitter : public LayoutSplitter,
								public ISinkIsMyEvent
{
public:
	DECLARE_BASE_CLASS_CAST(LayoutVerticalSplitter, LayoutSplitter, ISinkIsMyEvent)

	virtual BOOL OnUpdateLayout(HDE hde);
	virtual BOOL IsMyEvent(HANDLE, DuiEvent* pEvent);
};

THIS_IS_LAYOUT(LayoutVerticalSplitter, V-Splitter)

class  LayoutHorizontalSplitter : public LayoutSplitter, public CDuiOnChildWidthChanged
{
public:
	DECLARE_BASE_CLASS_CAST(LayoutHorizontalSplitter, LayoutSplitter, CDuiOnChildWidthChanged)

	virtual BOOL OnUpdateLayout(HDE hde);
	virtual BOOL IsMyEvent(HANDLE, DuiEvent* pEvent);
};

THIS_IS_LAYOUT(LayoutHorizontalSplitter, H-Splitter)


/////////////////////////////////////////////////////////////////////////////////////
//

class  LayoutPage : public DuiLayout
{
	DECLARE_BASE_CLASS_CAST(LayoutPage, DuiLayout)
public:

	virtual BOOL OnUpdateLayout(HDE hde);
};

THIS_IS_LAYOUT(LayoutPage, Page)


#endif // __DUILAYOUTMODE_H__