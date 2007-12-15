/*
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */
#pragma once

/**
 * ボディーリスト（メイン画面下側領域）
 */
class CBodyListCtrl : public CListCtrl
{
	DECLARE_DYNAMIC(CBodyListCtrl)

public:
	CBodyListCtrl();
	virtual ~CBodyListCtrl();

protected:
	DECLARE_MESSAGE_MAP()

public:
	virtual void DrawItem(LPDRAWITEMSTRUCT /*lpDrawItemStruct*/);

	void SetHeader(LPCTSTR, LPCTSTR);
	bool	m_bStopDraw;		///< DrawItem 実行可否フラグ（SetDraw）

private:
	BOOL	m_useColor;
	bool	m_bUseIcon;			///< アイコン利用フラグ

	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg BOOL OnLvnItemchanged(NMHDR *pNMHDR, LRESULT *pResult);
public:
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	void MyEnableIcon( bool bUseIcon ) { m_bUseIcon = bUseIcon; }
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
#ifndef WINCE
	afx_msg void OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS* lpncsp);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
#endif
};


