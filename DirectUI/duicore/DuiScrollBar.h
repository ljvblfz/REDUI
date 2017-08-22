#ifndef __DUISCROLLBAR_H__
#define __DUISCROLLBAR_H__

#pragma once

#include "DirectUI.h"


//////////////////////////////////////////////////////////////////////////
// 自绘滚动条

class CDuiScrollbar //: public CDuiScrollbarBase
{
public:
	CDuiScrollbar(HDE owner, BOOL bVert=TRUE) : _owner(owner), m_nPos(0), m_nRange(1), m_bVert(bVert), m_nCustomPos(0)
	{
		ATLASSERT(_owner);
		m_bVisible = FALSE;
		m_bEnabled = TRUE;
		m_bHover = FALSE;
		m_bForceVisible = FALSE;

		m_partHot = sb_null;
		m_partCapture = sb_null;

		m_rc.SetRectEmpty();
	}

public:
	virtual HDE GetOwner() const { return /*DuiHandle<DuiNode>*/(_owner); }
	virtual BOOL SetScrollRange(int nRange, BOOL bRedraw=FALSE);
	virtual int GetScrollPos() const { return ((m_bVisible || m_bForceVisible) && m_bEnabled) ? m_nPos : m_nCustomPos; }
	virtual void SetScrollPos(int nPos, BOOL bRedraw=FALSE);
	int GetCustomPos() const { return m_nCustomPos; }
	void SetCustomPos(int nPos) { m_nCustomPos = nPos; }
	virtual int GetScrollbarWidth() const;
	virtual void ShowScrollbar(BOOL bShow) { m_bForceVisible = bShow; if (!bShow) SetScrollRange(0); }
	virtual void EnableScrollbar(BOOL bEnable) { m_bEnabled = bEnable; }
	virtual BOOL IsValid() const { return (m_bVisible || m_bForceVisible) && m_bEnabled; }

	virtual BOOL IsScrollEvent(DuiEvent& event);
	void Paint(HDC hdc);
	void Invalidate();
	BOOL PtInScrollbar(POINT pt); // pt 是相对root的坐标;

protected:
	int GetScrollLine() const;
	int GetScrollPage() const;
	BOOL IsPtInside(POINT pt);

private:
	HDE _owner;
	int m_nRange;
	int m_nPos;

	// 当滚动条不可见时，允许设置一个用户自定义的偏移位置，用来调整所有子控件的偏移，默认是 0
	// 如果滚动条可见，此值无效
	int m_nCustomPos;

	enum sb_part
	{
		sb_lower_arrow,	// 上/左箭头
		sb_upper_arrow,	// 下/右箭头
		sb_lower_track,	// 上/左TRACK区
		sb_upper_track,	// 下/右TRACK区
		sb_thumb,		// 滑动条
		sb_count,

		sb_null = -1,
	};

	void StartScrollTimer(sb_part nPartId, BOOL bDelayFirst=TRUE);
	void StopScrollTimer();
	void SetScrollPosByPart(sb_part part);
	void UpdateRects();

	CRect m_rc;				// 滚动条总矩形，全部是相对于_owner 的 parent
	CRect m_rcPart[sb_count]; // 各个区域的矩形
	sb_part m_partHot;
	sb_part m_partCapture;
	POINT m_ptCapture; // 捕获鼠标时的初始位置，窗口客户区相关
	int m_nCaptureThumbTopOrLeft;

	DWORD m_bVisible:1;
	DWORD m_bEnabled:1;
	DWORD m_bVert:1;
	DWORD m_bHover:1;
	DWORD m_bForceVisible:1;
};


#endif // __DUISCROLLBAR_H__