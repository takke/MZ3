/*
 * Copyright (c) MZ3 Project Team All rights reserved.
 * Code licensed under the BSD License:
 * http://www.mz3.jp/license.txt
 */
#pragma once

#include "TouchListCtrl.h"

/**
 * カテゴリリスト（メイン画面2段目領域）
 */
class CCategoryListCtrl : public CTouchListCtrl
{
	DECLARE_DYNAMIC(CCategoryListCtrl)

public:
	CCategoryListCtrl();
	virtual ~CCategoryListCtrl();

	virtual HBITMAP GetBgBitmapHandle();
	virtual void PopupContextMenu( const CPoint );
	virtual void SetSelectItem( const int );
	virtual void MoveSlideRight();
	virtual void MoveSlideLeft();
protected:
	DECLARE_MESSAGE_MAP()
public:
	virtual void DrawItem(LPDRAWITEMSTRUCT /*lpDrawItemStruct*/);

	void SetActiveItem(int value) { m_activeItem = value; };
	int GetActiveItem() { return m_activeItem; };

	bool	m_bStopDraw;

private:
	int m_activeItem;				///< 赤いアイテムのインデックス

	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg BOOL OnLvnItemchanged(NMHDR *pNMHDR, LRESULT *pResult);
public:
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
};


