#ifndef __DUIHELPER_H__
#define __DUIHELPER_H__

#pragma once

#include "DuiManager.h"
#include "DuiPluginsManager.h"


//////////////////////////////////////////////////////////////////////////
// state changed notify helper

class StateChangedNotifyHelper
{
private:
	DuiNode* _owner;
	DWORD _oldState;
	DWORD _newState;

public:
	StateChangedNotifyHelper(DuiNode* owner)
	{
		_owner = owner;
		if (_owner)
			_oldState = _newState = (_owner)->GetState();
	}
	~StateChangedNotifyHelper()
	{
		if (_owner)
		{
			HDE hde = _owner->Handle();
			_newState = _owner->GetState();
			if (_newState != _oldState)
			{
				BOOL bDone = FALSE;
				// 首先由父控件处理通知。父控件可以通过返回值控制是否继续通知控件自身，这种机制有利于布局模式接管控制权
				if (_owner->GetParent() && (_owner->__cache.disabled&DUIDISABLED_CHILDSTATECHANGED)==0)
				{
					//InvokeAllSinksBool(_owner->GetParent(), OnChildStateChanged, hde, _newState ^ _oldState);
					//if (!bDone)
						bDone = (_owner->GetParent())->OnChildStateChanged(hde, _newState ^ _oldState);
				}
				// 然后由控件自身处理通知
				if (!bDone && (_owner->__cache.disabled&DUIDISABLED_STATECHANGED)==0)
				{
					InvokeAllSinks(_owner, OnStateChanged, _newState ^ _oldState);
					//{ 
					//	ISinkOnStateChanged* pBuf[128] = {0}; 
					//	for (int __idx=0, __cnt=(_owner)->GetClassCollection((LPVOID*)pBuf, CLASS_NAME(ISinkOnStateChanged), 128); __idx<__cnt; __idx++) 
					//	{ pBuf[__idx]->OnStateChanged((_owner)->Handle(), _newState ^ _oldState );  } 
					//	PluginsManager::CallPlugins(PluginCallContext(OpCode_OnStateChanged, FALSE, (_owner)->Handle()) 
					//		//__if_exists(abc) { .DefProc(NULL) }
					//		/*__if_exists (DefCallback::OnCall) { .DefProc( & DefCallback::OnCall ) },*/ 
					//		, NULL, (_owner)->Handle(), (_newState ^ _oldState) ); 
					//}
				}
			}
			_owner = NULL;
		}
	}
};

//////////////////////////////////////////////////////////////////////////
// DisabledOperationHelper

class DisableOp
{
private:
	DuiNode* _owner;
	DWORD _disabledFlags;
	DWORD _oldFlags;

public:
	DisableOp(DuiNode* owner, DWORD disabledFlags) : _owner(owner), _disabledFlags(disabledFlags), _oldFlags(0)
	{
		if (_owner)
		{
			_oldFlags=_owner->__cache.disabled;
			_owner->__cache.disabled |= _disabledFlags;
		}
	}
	~DisableOp()
	{
		if (_owner)
		{
			(_owner)->__cache.disabled = _oldFlags; /*RT(_owner, __cache.disabled) &= ~_disabledFlags;*/
			_owner = NULL;
		}
	}
};

#endif // __DUIHELPER_H__