/*
 * Copyright (c) MZ3 Project Team All rights reserved.
 * Code licensed under the BSD License:
 * http://www.mz3.jp/license.txt
 */
#pragma once


// CGroupTabCtrl

class CGroupTabCtrl : public CTabCtrl
{
	DECLARE_DYNAMIC(CGroupTabCtrl)

public:
	CGroupTabCtrl();
	virtual ~CGroupTabCtrl();

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
#ifndef WINCE
	bool   m_bDragging;
	CPoint m_ptDragStart;
#endif
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
};


