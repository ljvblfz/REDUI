#ifndef __DUIVISUAL_H__
#define __DUIVISUAL_H__

#pragma once

#include "../DuiPlugin.h"

struct VisualEntry
{
	LPCOLESTR szName;
	int lenName;
	IVisual* pVisual;
};

#pragma section("ReduiVisual$__a", read, shared)
#pragma section("ReduiVisual$__b", read, shared)
#pragma section("ReduiVisual$__c", read, shared)
extern "C"
{
	__declspec(selectany) __declspec(allocate("ReduiVisual$__a")) VisualEntry* __pvisualEntryFirst = NULL;
	__declspec(selectany) __declspec(allocate("ReduiVisual$__c")) VisualEntry* __pvisualEntryLast = NULL;
}
#pragma comment(linker, "/merge:ReduiVisual=.rdata")


#define THIS_IS_VISUAL(class, name) \
	__declspec(selectany) class __visual_##class; \
	__declspec(selectany) VisualEntry __visualEntry_##class = {L#name, lstrlenW(L#name), &__visual_##class}; \
	extern "C" __declspec(allocate("ReduiVisual$__b")) __declspec(selectany) VisualEntry* const __pvisual_##class = &__visualEntry_##class; \
	__pragma(comment(linker, "/include:___pvisual_" #class));


//////////////////////////////////////////////////////////////////////////
class VisualBase : public IVisual
{
public:
	virtual RECT GetAdjustRect(HDE hde) const { return CRect(0,0,0,0); }
	virtual BOOL OnPaintBorder(HDE hde, HDC hdc, RECT rcBorder) { return FALSE; }
	virtual BOOL OnPaintFocus(HDE hde, HDC hdc, RECT rcFocus, HRGN hrgn) { return FALSE; }
	virtual BOOL OnPaintBackground(HDE hde, HDC hdc, RECT rcBkgnd) { return FALSE; }
	virtual BOOL OnPaintForeground(HDE hde, HDC hdc, RECT rcFrgnd) { return FALSE; }
};

IVisual* VisualFromName(LPCOLESTR name);

// API function
IVisual* DUIAPI GetDefaultVisual();


//#include "DuiManager.h"


//////////////////////////////////////////////////////////////////////////
//

class VisualButton : public VisualBase
{
public:
	virtual BOOL OnPaintBorder(HDE hde, HDC hdc, RECT rcBorder);
	virtual BOOL OnPaintBackground(HDE hde, HDC hdc, RECT rcBkgnd) { return TRUE; }
	virtual BOOL OnPaintFocus(HDE hde, HDC hdc, RECT rcFocus, HRGN hrgn) { return TRUE; }
};

THIS_IS_VISUAL(VisualButton, Button)

//////////////////////////////////////////////////////////////////////////
//

class VisualGroupBox : public VisualBase
{
public:
	virtual RECT GetAdjustRect(HDE hde) const;
	virtual BOOL OnPaintBackground(HDE hde, HDC hdc, RECT rcBkgnd);
};

THIS_IS_VISUAL(VisualGroupBox, GroupBox)



//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
// ¼òÒ×¹¤¾ßºê

#define IMPLEMENT_SIMPLE_VISUAL(visualname) \
	class Visual##visualname : public VisualBase \
	{ \
	public: \
		virtual BOOL OnPaintBackground(HDE hde, HDC hdc, RECT rcBkgnd); \
	}; \
	THIS_IS_VISUAL(Visual##visualname, visualname) \
	BOOL Visual##visualname::OnPaintBackground

#define IMPLEMENT_SIMPLE_VISUAL_RECT(visualname, rect) \
	class Visual##visualname : public VisualBase \
	{ \
	public: \
		virtual RECT GetAdjustRect(HDE hde) const { return CRect##rect; } \
		virtual BOOL OnPaintBackground(HDE hde, HDC hdc, RECT rcBkgnd); \
	}; \
	THIS_IS_VISUAL(Visual##visualname, visualname) \
	BOOL Visual##visualname::OnPaintBackground


#endif // __DUIVISUAL_H__