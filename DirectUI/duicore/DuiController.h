#ifndef __DUICONTROLLER_H__
#define __DUICONTROLLER_H__

#pragma once

#include "DuiManager.h"
#include "DuiLayout.h"

//////////////////////////////////////////////////////////////////////////
//

class CDuiController : public ControlExtension // CDuiControllerBase
{
	DECLARE_BASE_CLASS_CAST(CDuiController, ControlExtension)
protected:
	virtual IDispatch* GetDispatch() { return NULL; }

public:
	virtual ~CDuiController();

	void OnConnect();
	// CDuiExtension
	//virtual HINSTANCE GetModuleHandle() const { return _AtlBaseModule.GetModuleInstance(); }

	//virtual void SetOwner(DuiNode* owner);
	//virtual DuiNode* GetOwner() const { return m_pOwner; }

	//// CDuiControllerBase
	//virtual BOOL IsControllerEvent(DuiEvent& event) { return FALSE; }
	//virtual void OnStateChanged(DWORD dwStateMask) {}
	//virtual BOOL OnChildStateChanged(HDE pChild, DWORD dwStateMask) { return FALSE; }
	//virtual void OnAttributeChanged(LPCOLESTR szAttr) {}
	//virtual void OnChildAttributeChanged(HDE pChild, LPCOLESTR szAttr) {}
};

//////////////////////////////////////////////////////////////////////////
// List controller base

class CDuiControllerListBase : public CDuiController
{
	DECLARE_CLASS_NAME(CDuiControllerListBase)
	DECLARE_CLASS_CAST(CDuiControllerListBase, CDuiController)

public:
	// CDuiController

	Begin_Disp_Map(CDuiControllerListBase)
		//Disp_PropertyGet
	End_Disp_Map()

public:
	//virtual BOOL SelectItem(int iIndex) = 0;
	//virtual BOOL SelectItem(CDuiControlExtensionBase* pItem) = 0;
	virtual int GetItemIndex(DuiNode* pItem);
	virtual int GetColumnCount() const;
	virtual int GetColumnWidth(int iIndex);

protected:
	CSimpleArray<int> m_ColWidths;
};

//////////////////////////////////////////////////////////////////////////
// List Item controller base

class CDuiControllerListItemBase : public CDuiController
{
	DECLARE_EXTENSION(CDuiControllerListItemBase)
	DECLARE_CLASS_CAST(CDuiControllerListItemBase, CDuiController)

public:
	//virtual BOOL IsControllerEvent(DuiEvent& event) { return FALSE; }
	virtual BOOL IsSelected() const;
	virtual void select();
	virtual void unselect();

	Begin_Disp_Map(CDuiControllerListItemBase)
		Disp_Property(1, selected)
		Disp_Method(2, select, void, 0)
		Disp_Method(3, unselect, void, 0)
	End_Disp_Map()

	HRESULT Getselected(VARIANT* pVar);
	HRESULT Putselected(VARIANT* pVar);
};

//////////////////////////////////////////////////////////////////////////
// controller list single-selection

class CDuiControllerList : public CDuiControllerListBase,
						public ISinkOnChildStateChanged
{
	DECLARE_EXTENSION(CDuiControllerList)
	DECLARE_CLASS_CAST(CDuiControllerList, CDuiControllerListBase, ISinkOnChildStateChanged)

public:
	virtual BOOL OnChildStateChanged(HANDLE, HDE pChild, DWORD dwStateMask);

	virtual DuiNode* GetCurSel() const;

	Begin_Disp_Map(CDuiControllerList, CDuiControllerListBase)
		Disp_PropertyGet(1, selection)
	End_Disp_Map()

	HRESULT Getselection(VARIANT* pVar);
};

//////////////////////////////////////////////////////////////////////////
// controller list multi-selection

class CDuiControllerListMulti : public CDuiControllerListBase,
						public ISinkOnChildStateChanged,
						public ISinkOnChildAdded,
						public ISinkOnChildRemoved
{
	DECLARE_EXTENSION(CDuiControllerListMulti)
	DECLARE_CLASS_CAST(CDuiControllerListMulti, CDuiControllerListBase, ISinkOnChildStateChanged, ISinkOnChildAdded, ISinkOnChildRemoved)

public:
	CDuiControllerListMulti();

	virtual BOOL OnChildStateChanged(HANDLE, HDE pChild, DWORD dwStateMask);
	virtual void OnChildAdded(HANDLE, HDE pChild);
	virtual void OnChildRemoved(HANDLE, HDE pChild);
	//virtual int GetSelCount() const { return 0; }
	//virtual DuiNode* GetSel(int iIndex) { return NULL; }

	Begin_Disp_Map(CDuiControllerListMulti, CDuiControllerListBase)
		Disp_PropertyGet(1, selections) // 返回选择的项集合
	End_Disp_Map()

	HRESULT Getselections(VARIANT* pVar);

protected:
	CAutomationCollection* m_pSelections; // cache

	void UpdateSelection(DuiNode* pExclude=NULL);
};


//////////////////////////////////////////////////////////////////////////
// Header

class CDuiListHeader : public CDuiLayoutHorizontal
{
	DECLARE_CONTROL_EXTENSION(CDuiListHeader)
	DECLARE_CLASS_CAST(CDuiListHeader, CDuiLayoutHorizontal)

public:
};

THIS_IS_CONTROL_EXTENSION2(CDuiListHeader, List.Header, Header)


//////////////////////////////////////////////////////////////////////////
// Footer

class CDuiListFooter : public CDuiLayoutHorizontal
{
	DECLARE_CONTROL_EXTENSION(CDuiListFooter)
	DECLARE_CLASS_CAST(CDuiListFooter, CDuiLayoutHorizontal)

public:
};

THIS_IS_CONTROL_EXTENSION2(CDuiListFooter, List.Footer, Footer)


//////////////////////////////////////////////////////////////////////////
// List control

class CDuiList : public CDuiLayoutVertical//, public CDuiControllerListBase
{
	DECLARE_CONTROL_EXTENSION(CDuiList)
	//DECLARE_CLASS_CAST2(CDuiList, CDuiLayoutVertical, CDuiControllerListBase)

public:

protected:
	int m_iCurSel;
	//int m_iExpandedItem;
	//CDuiListCallbackBase* m_pCallback;
	//CDuiLayoutVertical* m_pList;
	CDuiListHeader* m_pHeader;
	CDuiListFooter* m_pFooter;
	//DuiListInfo m_ListInfo;
};

THIS_IS_CONTROL_EXTENSION(CDuiList, List)

#endif // __DUICONTROLLER_H__