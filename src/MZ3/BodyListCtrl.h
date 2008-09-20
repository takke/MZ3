/*
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */
#pragma once

#include "TouchListCtrl.h"

/**
 * �{�f�B�[���X�g�i���C����ʉ����̈�j
 */
class CBodyListCtrl : public CTouchListCtrl
{
	DECLARE_DYNAMIC(CBodyListCtrl)

public:
	bool	m_bStopDraw;					///< DrawItem ���s�ۃt���O�iSetDraw�j
	CString m_strIntegratedLinePattern1;	///< �����J�������[�h�A1�s�ڂ̃p�^�[��
	CString m_strIntegratedLinePattern2;	///< �����J�������[�h�A2�s�ڂ̃p�^�[��

	/// �A�C�R�����[�h�B�l�̓s�N�Z����
	enum ICON_MODE {
		ICON_MODE_NONE = 0,		///< �A�C�R���Ȃ�
		ICON_MODE_16 = 16,		///< 16x16 �A�C�R��
		ICON_MODE_32 = 32,		///< 32x32 �A�C�R��
		ICON_MODE_48 = 48,		///< 48x48 �A�C�R��
	};

private:
	ICON_MODE m_iconMode;		///< �A�C�R�����[�h

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
	void MySetIconMode( ICON_MODE iconMode ) { m_iconMode = iconMode; }
	bool IsEnableIcon() { return m_iconMode!=ICON_MODE_NONE; }

	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
#ifndef WINCE
	afx_msg void OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS* lpncsp);
#endif
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual void MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct);
	afx_msg LRESULT OnSetFont(WPARAM wParam, LPARAM lParam);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
};


