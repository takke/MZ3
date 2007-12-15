/*
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */
#pragma once

/**
 * カテゴリリスト（メイン画面2段目領域）
 */
class CCategoryListCtrl : public CListCtrl
{
	DECLARE_DYNAMIC(CCategoryListCtrl)

public:
	CCategoryListCtrl();
	virtual ~CCategoryListCtrl();

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
};


