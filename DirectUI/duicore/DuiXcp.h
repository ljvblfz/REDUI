#ifndef __DUIXCP_H__
#define __DUIXCP_H__


#pragma once

#include "DuiManager.h"
#include "../../common/dispatchimpl.h"


/////////////////////////////////////////////////////////////////////////////////////
//

class CDuiXcp : public CDuiAxSite
				, public ISinkIsMyEvent
				, public ISinkOnScroll
				, public INamedItemHost
{
public:
	CDuiXcp();
	virtual ~CDuiXcp();

	DECLARE_CONTROL_EXTENSION(CDuiXcp)
	DECLARE_CLASS_CAST(CDuiXcp, CDuiAxSite, ISinkIsMyEvent, ISinkOnScroll)
	DECLARE_CONTROL_FLAGS(DUIFLAG_NOFOCUSFRAME | DUIFLAG_NOCHILDREN)
	DECLARE_BASE_CLASS_STYLE("flag:tabstop; display:inlineblock; overflow:hidden;")

	void OnConnect();

	virtual BOOL IsMyEvent(HANDLE, DuiEvent* pEvent);
	virtual void OnScroll(HANDLE, HDE pScroll, BOOL bVert=TRUE);

	virtual void OnPaintContent(HDE hde, HDC hdc, RECT rcPaint, BOOL bDraw, InDrawData* pidd);
	virtual void SetAttribute(HDE hde, LPCWSTR szName, VARIANT* value);

	// CDuiAxSite::IXcpControlHost
	STDMETHOD (NotifyLoaded)(void);
	// INamedItemHost
	virtual BOOL GetNamedItem(LPCOLESTR name, VARIANT* pRet); // 如果有命名项，返回TRUE，否则返回FALSE

	//HRESULT Getobject(VARIANT* pVar);

protected:
	void ReleaseControl();

	BOOL m_bLoaded;

protected:

	Begin_Disp_Map(CDuiXcp)
		//Disp_PropertyGet(2, object)
	End_Disp_Map()
};

THIS_IS_CONTROL_EXTENSION(CDuiXcp, xaml)


#endif // __DUIXCP_H__