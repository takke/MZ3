/*
 * Copyright (c) MZ3 Project Team All rights reserved.
 * Code licensed under the BSD License:
 * http://www.mz3.jp/license.txt
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


