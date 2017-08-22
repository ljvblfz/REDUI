// DuiEvent.cpp : CDuiEvent 的实现

#include "stdafx.h"
#include "DuiEvent.h"
#include "DuiControl.h"
#include "duicore/DuiPluginsManager.h"


// CDuiEvent


STDMETHODIMP COleDuiEvent::get_name(BSTR* pVal)
{
	// TODO: 在此添加实现代码
	return *pVal = ::SysAllocString(DuiNode::NameToFinalName(m_event->Name)), S_OK;
}

STDMETHODIMP COleDuiEvent::get_src(IDispatch** pVal)
{
	// TODO: 在此添加实现代码
	if (pVal==NULL) return E_POINTER;
	*pVal = DuiHandle<DuiNode>(m_event->pSender)->GetObject(TRUE);
	if (*pVal) (*pVal)->AddRef();

	return S_OK;
}

STDMETHODIMP COleDuiEvent::get_x(LONG* pVal)
{
	// TODO: 在此添加实现代码
	if (pVal==NULL) return E_POINTER;
	*pVal = m_event->ptMouse.x;

	return S_OK;
}

STDMETHODIMP COleDuiEvent::get_y(LONG* pVal)
{
	// TODO: 在此添加实现代码
	if (pVal==NULL) return E_POINTER;
	*pVal = m_event->ptMouse.y;

	return S_OK;
}

STDMETHODIMP COleDuiEvent::get_cancelBubble(VARIANT_BOOL* pVal)
{
	// TODO: 在此添加实现代码
	if (pVal==NULL) return E_POINTER;
	*pVal = (m_event->cancelBubble) ? VARIANT_TRUE : VARIANT_FALSE;

	return S_OK;
}

STDMETHODIMP COleDuiEvent::put_cancelBubble(VARIANT_BOOL newVal)
{
	// TODO: 在此添加实现代码
	m_event->cancelBubble = (newVal!=VARIANT_FALSE);

	return S_OK;
}

STDMETHODIMP COleDuiEvent::get_returnValue(VARIANT_BOOL* pVal)
{
	// TODO: 在此添加实现代码
	if (pVal==NULL) return E_POINTER;
	*pVal = (m_event->returnValue) ? VARIANT_TRUE : VARIANT_FALSE;

	return S_OK;
}

STDMETHODIMP COleDuiEvent::put_returnValue(VARIANT_BOOL newVal)
{
	// TODO: 在此添加实现代码
	m_event->returnValue = (newVal!=VARIANT_FALSE);

	return S_OK;
}

STDMETHODIMP COleDuiEvent::get_propertyName(BSTR* pVal)
{
	// TODO: 在此添加实现代码
	if (pVal==NULL) return E_POINTER;
	*pVal = ::SysAllocString(m_event->pPropertyName);

	return S_OK;
}
