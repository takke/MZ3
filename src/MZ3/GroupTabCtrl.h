/*
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
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
};


