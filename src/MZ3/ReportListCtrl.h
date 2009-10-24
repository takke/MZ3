/*
 * Copyright (c) MZ3 Project Team All rights reserved.
 * Code licensed under the BSD License:
 * http://www.mz3.jp/license.txt
 */
#pragma once

#include "TouchListCtrl.h"

/**
 * レポートリスト（レポート画面上側領域）
 */
class CReportListCtrl : public CTouchListCtrl
{
	DECLARE_DYNAMIC(CReportListCtrl)

public:
	CReportListCtrl();
	virtual ~CReportListCtrl();

	virtual HBITMAP GetBgBitmapHandle();
	virtual void PopupContextMenu( const CPoint );
	virtual void SetSelectItem( const int );
	virtual void MoveSlideRight();
	virtual void MoveSlideLeft();
protected:
	DECLARE_MESSAGE_MAP()
public:
//	virtual BOOL PreTranslateMessage(MSG* pMsg);
public:
	virtual void DrawItem(LPDRAWITEMSTRUCT /*lpDrawItemStruct*/);

public:
	int GetSelectedItem(void);
public:
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
public:
	afx_msg BOOL OnLvnItemchanged(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
};


