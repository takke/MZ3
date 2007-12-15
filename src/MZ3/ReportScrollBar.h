/*
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */
#pragma once


// ReportScrollBar

class ReportScrollBar : public CScrollBar
{
	DECLARE_DYNAMIC(ReportScrollBar)

public:
	ReportScrollBar();
	virtual ~ReportScrollBar();

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
};


