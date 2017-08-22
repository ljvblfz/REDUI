#ifndef __DUID3DLAYOUTMODE_H__
#define __DUID3DLAYOUTMODE_H__

#pragma once

#include "DuiLayoutMode.h"
#include "DuiD3DImpl.h"

//////////////////////////////////////////////////////////////////////////
// 3D desktop
// 

class  CDuiLayoutModeCoverFlow : public ILayout
{
public:
	DECLARE_NO_DELETE_THIS(CDuiLayoutModeCoverFlow)
	DECLARE_BASE_CLASS_CAST(CDuiLayoutModeCoverFlow)

	virtual BOOL OnUpdateLayout(HANDLE);
};
THIS_IS_LAYOUT(CDuiLayoutModeCoverFlow, CoverFlow)



#endif // __DUID3DLAYOUTMODE_H__