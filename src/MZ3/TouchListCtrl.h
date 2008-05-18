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
 * CTouchListCtrlクラス
 *  ドラッグスクロール対応リストコントロール
 */

// タイマーID
#define TIMERID_TOUCHLIST_SCROLLREDRAW	1
#define TIMERID_TOUCHLIST_AUTOSCROLL	2
// タイマーインターバル [msec]
#define TIMER_INTERVAL_TOUCHLIST_SCROLLREDRAW	10L
#define TIMER_INTERVAL_TOUCHLIST_AUTOSCROLL		20L
// 背景同時スクロールするか？
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
	int		m_offsetPixelY;			///< ピクセル単位スクロールのオフセットピクセル数

	int		m_iItemHeight;			///< Item一行の高さ

	bool	m_bDragging;			///< マウスドラッグ中
	CPoint	m_ptDragStart;			///< ドラッグ開始位置
	int		m_iDragLine;			///< ドラッグスクロール行数

	CRect	m_viewRect;				///< 描画領域：矩形
	int		m_screenWidth;			///< 描画領域：横幅
	int		m_screenHeight;			///< 描画領域：縦幅

#ifndef WINCE
	CDC*		m_memDC;			///< 裏画面DC
	CBitmap*	m_memBMP;			///< 裏画面バッファ
	CBitmap*	m_oldBMP;			///< 旧画面の情報
#endif

	int		m_drawStartTopOffset;	///< 描画開始オフセット(ピクセル)

	bool	m_bDrawBk;				///< DrawItemにおける背景描画フラグ
	bool	m_bTimerRedraw;			///< 遅延描画中フラグ

	Ran2::AutoScrollManager	m_autoScrollInfo;	///< 慣性スクロール情報

	DWORD		m_dwAutoScrollStartTick;	///< 慣性スクロール開始時刻
	int			m_yAutoScrollMax;			///< 慣性スクロール中の最大移動量

public:
	// 背景描画フラグの設定／取得
	bool	GetDrawBk() { return m_bDrawBk; }
	void	SetDrawBk( const bool bDrawBk ) {
		m_bDrawBk = bDrawBk; 
	}
#ifndef WINCE
	// 描画
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

	// 遅延描画タイマーのセット／リセット
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
