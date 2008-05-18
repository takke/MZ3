/*
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */
#pragma once
#include "afxcmn.h"
#include "Ran2View.h"

/**
 * CTouchListCtrl�N���X
 *  �h���b�O�X�N���[���Ή����X�g�R���g���[��
 */

// �^�C�}�[ID
#define TIMERID_TOUCHLIST_SCROLLREDRAW	1
#define TIMERID_TOUCHLIST_AUTOSCROLL	2
// �^�C�}�[�C���^�[�o�� [msec]
#define TIMER_INTERVAL_TOUCHLIST_SCROLLREDRAW	10L
#define TIMER_INTERVAL_TOUCHLIST_AUTOSCROLL		20L
// �w�i�����X�N���[�����邩�H
#define TOUCHLIST_SCROLLWITHBK

class CTouchListCtrl :
	public CListCtrl
{
	DECLARE_DYNAMIC(CTouchListCtrl)

public:
	CTouchListCtrl(void);
	~CTouchListCtrl(void);

	virtual HBITMAP GetBgBitmapHandle() { return NULL; }
	virtual void PopupContextMenu( const CPoint ) { return; }
private:
	int		m_offsetPixelY;			///< �s�N�Z���P�ʃX�N���[���̃I�t�Z�b�g�s�N�Z����

	int		m_iItemHeight;			///< Item��s�̍���

	bool	m_bDragging;			///< �}�E�X�h���b�O��
	CPoint	m_ptDragStart;			///< �h���b�O�J�n�ʒu
	int		m_iDragLine;			///< �h���b�O�X�N���[���s��

	CRect	m_viewRect;				///< �`��̈�F��`
	int		m_screenWidth;			///< �`��̈�F����
	int		m_screenHeight;			///< �`��̈�F�c��

#ifndef WINCE
	CDC*		m_memDC;			///< �����DC
	CBitmap*	m_memBMP;			///< ����ʃo�b�t�@
	CBitmap*	m_oldBMP;			///< ����ʂ̏��
#endif

	int		m_drawStartTopOffset;	///< �`��J�n�I�t�Z�b�g(�s�N�Z��)

	bool	m_bDrawBk;				///< DrawItem�ɂ�����w�i�`��t���O
	bool	m_bTimerRedraw;			///< �x���`�撆�t���O

	Ran2::AutoScrollManager	m_autoScrollInfo;	///< �����X�N���[�����

	DWORD		m_dwAutoScrollStartTick;	///< �����X�N���[���J�n����
	int			m_yAutoScrollMax;			///< �����X�N���[�����̍ő�ړ���

public:
	// �w�i�`��t���O�̐ݒ�^�擾
	bool	GetDrawBk() { return m_bDrawBk; }
	void	SetDrawBk( const bool bDrawBk ) {
		m_bDrawBk = bDrawBk; 
	}
#ifndef WINCE
	// �`��
	int DrawDetail(bool bForceDraw=true);
#endif

	void ResetAllTimer();

private:
	bool	ScrollByMoveY( const int dy);
	bool	MyAdjustDrawOffset();
#ifndef WINCE
	bool	MyMakeBackBuffers(CPaintDC& cdc);
	void	DrawToScreen(CDC* pDC);
#endif

	// �x���`��^�C�}�[�̃Z�b�g�^���Z�b�g
	inline void	MySetRedrawTimer( const int iInterval = TIMER_INTERVAL_TOUCHLIST_SCROLLREDRAW ) {
		KillTimer( TIMERID_TOUCHLIST_SCROLLREDRAW );
		SetTimer( TIMERID_TOUCHLIST_SCROLLREDRAW, iInterval , NULL );
		m_bTimerRedraw = true ;
	}
	inline void	MyResetRedrawTimer(){
		KillTimer( TIMERID_TOUCHLIST_SCROLLREDRAW );
		m_bTimerRedraw = false ;
	}

	inline void	MySetAutoScrollTimer( const int iInterval = TIMER_INTERVAL_TOUCHLIST_AUTOSCROLL ) {
		KillTimer( TIMERID_TOUCHLIST_AUTOSCROLL );
		SetTimer( TIMERID_TOUCHLIST_AUTOSCROLL, iInterval , NULL );
	}
	inline void	MyResetAutoScrollTimer(){
		KillTimer( TIMERID_TOUCHLIST_AUTOSCROLL );
		m_autoScrollInfo.clear();
	}

protected:
	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnDestroy();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnLvnDeleteallitems(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLvnDeleteitem(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLvnInsertitem(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
};
