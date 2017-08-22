#pragma once

#ifndef __PSBASE_H__
#define __PSBASE_H__

#include <atlsimpcoll.h>

class CPulseBase
{
public:
	CPulseBase(CPulseBase* p)
	{
		if (m_ptrs.Find(p)<0)
			m_ptrs.Add(p);
	};
	virtual void ReleaseThis() = 0;

	static BOOL IsValid(CPulseBase* p) { return m_ptrs.Find(p)>=0; }
	static void DeleteThis(CPulseBase* p)
	{
		if (m_ptrs.Find(p)>=0)
		{
			m_ptrs.Remove(p);
			p->ReleaseThis();
		}
	}

private:
	static CSimpleArray<CPulseBase*> m_ptrs;
};
__declspec(selectany) CSimpleArray<CPulseBase*> CPulseBase::m_ptrs;

#define PulseBasePtr(p) ((CPulseBase*)(p))

#endif // __PSBASE_H__