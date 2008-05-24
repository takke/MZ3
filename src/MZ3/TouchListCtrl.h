/*
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */
#pragma once
#include "afxcmn.h"
#include "AutoScrollManager.h"

/**
 * CTouchListCtrl�N���X
 *  �h���b�O�X�N���[���Ή����X�g�R���g���[��
 */

// �^�C�}�[ID
#define TIMERID_TOUCHLIST_SCROLLREDRAW	1
#define TIMERID_TOUCHLIST_AUTOSCROLL	2
#define TIMERID_TOUCHLIST_PANSCROLL		3
// �^�C�}�[�C���^�[�o�� [msec]
#define TIMER_INTERVAL_TOUCHLIST_SCROLLREDRAW	10L
#define TIMER_INTERVAL_TOUCHLIST_SCROLLREDRAW_L	50L
#define TIMER_INTERVAL_TOUCHLIST_AUTOSCROLL		20L
#define TIMER_INTERVAL_TOUCHLIST_PANSCROLL		10L

class CTouchListCtrl :
	public CListCtrl
{
	DECLARE_DYNAMIC(CTouchListCtrl)

public:
	CTouchListCtrl(void);
	~CTouchListCtrl(void);

	virtual HBITMAP GetBgBitmapHandle() { return NULL; }
	virtual void PopupContextMenu( const CPoint ) { return; }
	virtual void SetSelectItem( const int ) { return; }
	virtual void MoveSlideRight() { Invalidate(); }
	virtual void MoveSlideLeft() { Invalidate(); }

	bool	m_bUseHorizontalDragMove;	///< ���h���b�O�ł̍��ڈړ�
	bool	m_bUsePanScrollAnimation;	///<���X�N���[���A�j���[�V�������邩

	/// �p���X�N���[���̕�����`
	enum PAN_SCROLL_DIRECTION
	{
		PAN_SCROLL_DIRECTION_NONE = 0,	///< �ǂ���ł��Ȃ�
		PAN_SCROLL_DIRECTION_RIGHT,		///< �E����
		PAN_SCROLL_DIRECTION_LEFT,		///< ������
	};
	void	StartPanScroll( PAN_SCROLL_DIRECTION );
private:
	int		m_offsetPixelY;			///< �s�N�Z���P�ʃX�N���[���̃I�t�Z�b�g�s�N�Z����

	int		m_iItemHeight;			///< Item��s�̍���

	bool	m_bDragging;			///< �}�E�X�h���b�O��
	CPoint	m_ptDragStart;			///< �h���b�O�J�n�ʒu
	int		m_iDragLine;			///< �h���b�O�X�N���[���s��
	int		m_iDragStartItem;		///< �h���b�O�J�n���̃N���b�N�A�C�e��

	CRect	m_viewRect;				///< �`��̈�F��`
	int		m_screenWidth;			///< �`��̈�F����
	int		m_screenHeight;			///< �`��̈�F�c��

	CDC*		m_memDC;			///< �����DC
	CBitmap*	m_memBMP;			///< ����ʃo�b�t�@
	CBitmap*	m_oldBMP;			///< ����ʂ̏��

	int		m_drawStartTopOffset;	///< �`��J�n�I�t�Z�b�g(�s�N�Z��)

	bool	m_bDrawBk;				///< DrawItem�ɂ�����w�i�`��t���O
	bool	m_bTimerRedraw;			///< �x���`�撆�t���O

	AutoScrollManager	m_autoScrollInfo;	///< �����X�N���[�����

	DWORD	m_dwAutoScrollStartTick;///< �����X�N���[���J�n����
	int		m_yAutoScrollMax;		///< �����X�N���[�����̍ő�ړ���
	bool	m_bAutoScrolling;		///< �����X�N���[����

	bool	m_bScrollWithBk;		///< �w�i�����X�N���[�����邩

	// �p���֘A���
	bool	m_bPanDragging;			///< �������}�E�X�h���b�O��
	bool	m_bScrollDragging;		///< �X�N���[����
	int		m_offsetPixelX;			///< �������I�t�Z�b�g�s�N�Z����
	int		m_dPxelX;				///< �������P�ʎ��Ԉړ��� [pixels/10msec]
	DWORD	m_dwPanScrollLastTick;	///< �p���X�N���[���J�n����
	bool	m_bCanSlide;			///< ���E�ړ��\��
	bool	m_bCanPanScroll;		///< �p���X�N���[���A�j���\��
	PAN_SCROLL_DIRECTION m_drPanScrollDirection;	///< �p���X�N���[������
public:
	// �w�i�`��t���O�̐ݒ�^�擾
	bool	IsDrawBk() { return m_bDrawBk; }
	void	SetDrawBk( const bool bDrawBk ) {
		m_bDrawBk = bDrawBk; 
	}
	// �`��
	int DrawDetail(bool bForceDraw=true);

	void ResetAllTimer();

	bool	IsScrollWithBk() { return m_bScrollWithBk ;}
	void	SetScrollWithBk( const bool bScrollWithBk ) {
		m_bScrollWithBk = bScrollWithBk;
	}

	void	SetCanSlide( const bool bCanSlide ) {
		m_bCanSlide = bCanSlide;
	}
	void	SetCanPanScroll( const bool bCanPanScroll ) {
		m_bCanPanScroll = bCanPanScroll;
	}
private:
	bool	ScrollByMoveY( const int dy);
	bool	MyAdjustDrawOffset();
	bool	MyMakeBackBuffers(CDC* pdc);
	void	DrawToScreen(CDC* pDC);
	void	PanDrawToScreen(CDC* pDC);
	void	DrawItemFocusRect( const int );

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

	// �c�X�N���[���^�C�}�[�̃Z�b�g�^���Z�b�g
	inline void	MySetAutoScrollTimer( const int iInterval = TIMER_INTERVAL_TOUCHLIST_AUTOSCROLL ) {
		KillTimer( TIMERID_TOUCHLIST_AUTOSCROLL );
		SetTimer( TIMERID_TOUCHLIST_AUTOSCROLL, iInterval , NULL );
		m_bAutoScrolling = true;
	}
	inline void	MyResetAutoScrollTimer(){
		KillTimer( TIMERID_TOUCHLIST_AUTOSCROLL );
		m_autoScrollInfo.clear();
		m_bAutoScrolling = false;
	}

	// �p���X�N���[���^�C�}�[�̃Z�b�g�^���Z�b�g
	inline void	MySetPanScrollTimer( const int iInterval = TIMER_INTERVAL_TOUCHLIST_PANSCROLL ) {
		KillTimer( TIMERID_TOUCHLIST_PANSCROLL );
		SetTimer( TIMERID_TOUCHLIST_PANSCROLL, iInterval , NULL );
	}
	inline void	MyResetPanScrollTimer(){
		KillTimer( TIMERID_TOUCHLIST_PANSCROLL );
		m_offsetPixelX = 0;
		m_drPanScrollDirection = PAN_SCROLL_DIRECTION_NONE;
	}

	void	MySetDragFlagWhenMovedPixelOverLimit( int , int );

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
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
};
