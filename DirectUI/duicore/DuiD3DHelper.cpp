#include "stdafx.h"
#include "DuiD3DHelper.h"

AutoStateDevice::AutoStateDevice( IDirect3DDevice9* device, BOOL autoApply/*=TRUE*/ ) : m_device(device), m_items(NULL), m_autoApply(autoApply)
{

}

AutoStateDevice::~AutoStateDevice()
{
	Restore();
}

void AutoStateDevice::_apply( StateItemBase* si )
{
	if (si==NULL) return;
	si->Apply(m_device);
	if (si->Next()) _apply(si->Next());
}

void AutoStateDevice::_restore( StateItemBase* si )
{
	if (si==NULL) return;
	if (si->Next()) _restore(si->Next());
	if (m_device) si->Restore(m_device);
	si->Release();
}

void AutoStateDevice::Apply()
{
	if (m_device && !m_autoApply) _apply(m_items);
}

void AutoStateDevice::Restore()
{
	_restore(m_items);
	m_items = NULL;
}
