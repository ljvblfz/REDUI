#include "stdafx.h"
#include "DuiD3DLayoutMode.h"
#include "DuiHelper.h"
#include "DuiPluginsManager.h"


BOOL CDuiLayoutModeCoverFlow::OnUpdateLayout(HANDLE h)
{
	DuiNode* r = DuiHandle<DuiNode>(h);
	CRect rc = (r)->GetPos();
	rc.MoveToXY(0,0);
	for (int i=0; i<r->GetChildCount(); i++)
	{
		DisableOp dop(r->GetChildControl(i), DUIDISABLED_CHILDSTATECHANGED);
		(r->GetChildControl(i))->SetPos(rc);
	}
	return TRUE;
}
