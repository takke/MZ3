/*
 * Copyright (c) MZ3 Project Team All rights reserved.
 * Code licensed under the BSD License:
 * http://www.mz3.jp/license.txt
 */
#pragma once

#ifdef BT_MZ3

// CWriteEdit

class CWriteEdit : public CEdit
{
	DECLARE_DYNAMIC(CWriteEdit)

public:
	CWriteEdit();
	virtual ~CWriteEdit();

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
};

#endif
