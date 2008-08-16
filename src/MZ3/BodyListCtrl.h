/*
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */
#pragma once

#include "TouchListCtrl.h"

/**
 * ボディーリスト（メイン画面下側領域）
 */
class CBodyListCtrl : public CTouchListCtrl
{
	DECLARE_DYNAMIC(CBodyListCtrl)

public:
	bool	m_bStopDraw;		///< DrawItem 実行可否フラグ（SetDraw）
private:
	BOOL	m_useColor;
	bool	m_bUseIcon;			///< アイコン利用フラグ

public:
	CBodyListCtrl();
	virtual ~CBodyListCtrl();

	virtual HBITMAP GetBgBitmapHandle();
	virtual void PopupContextMenu( const CPoint );
	virtual void SetSelectItem( const int );
protected:
	DECLARE_MESSAGE_MAP()

public:
	virtual void DrawItem(LPDRAWITEMSTRUCT /*lpDrawItemStruct*/);
	void	SetHeader(LPCTSTR col1, LPCTSTR col2, LPCTSTR col3);

private:
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg BOOL OnLvnItemchanged(NMHDR *pNMHDR, LRESULT *pResult);
public:
	void MyEnableIcon( bool bUseIcon ) { m_bUseIcon = bUseIcon; }
	bool IsEnableIcon() { return m_bUseIcon; }

	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
#ifndef WINCE
	afx_msg void OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS* lpncsp);
#endif
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual void MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct);
};


