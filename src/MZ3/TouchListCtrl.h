/*
 * Copyright (c) MZ3 Project Team All rights reserved.
 * Code licensed under the BSD License:
 * http://www.mz3.jp/license.txt
 */
#pragma once
#include "afxcmn.h"
#include "AutoScrollManager.h"

/**
 * CTouchListCtrlクラス
 *  ドラッグスクロール対応リストコントロール
 */

// タイマーID
#define TIMERID_TOUCHLIST_SCROLLREDRAW	1
#define TIMERID_TOUCHLIST_AUTOSCROLL	2
#define TIMERID_TOUCHLIST_PANSCROLL		3
// タイマーインターバル [msec]
#define TIMER_INTERVAL_TOUCHLIST_SCROLLREDRAW	10L
#define TIMER_INTERVAL_TOUCHLIST_SCROLLREDRAW_L	50L
#define TIMER_INTERVAL_TOUCHLIST_AUTOSCROLL		20L
#define TIMER_INTERVAL_TOUCHLIST_PANSCROLL		10L

/// カラムモードのスタイル
namespace COLUMN_MODE_STYLE {
static const int BOX_MARGIN_BOTTOM_PT = 2;	///< ボックス間マージン(下側)[pt]
static const int FIRST_MARGIN_LEFT	= 4;	///< 第1カラム、左側マージン
static const int FIRST_MARGIN_RIGHT	= 4;	///< 第1カラム、右側マージン
static const int OTHER_MARGIN_LEFT	= 12;	///< 第2カラム以降、左側マージン
static const int OTHER_MARGIN_RIGHT	= 12;	///< 第2カラム以降、右側マージン
}

/// 統合カラムモードのスタイル
namespace INTEGRATED_MODE_STYLE {
static const int BOX_MARGIN_BOTTOM_PT   = 2;	///< ボックス間マージン(下側)[pt]
static const int EACH_LINE_MARGIN_PT    = 1;	///< 行間マージン[pt]
static const int FIRST_LINE_MARGIN_LEFT = 4;	///< 1行目、左マージン
static const int OTHER_LINE_MARGIN_LEFT = 4+4;	///< 2行目以降、左マージン
};


class CTouchListCtrl :
	public CListCtrl
{
	DECLARE_DYNAMIC(CTouchListCtrl)

public:
	bool	m_bStopDraw;					///< DrawItem 実行可否フラグ（SetDraw）

	bool	m_bBlackScrollMode;				///< ブラックスクロールモード
											///< スクロール中、描画範囲外にiPhone風の塗りつぶしを行う。
											///< WMだとリストの再描画が遅いので。
	DWORD	m_dwLastDrawTick;				///< 前回の描画時刻(TickTime)

	bool	m_bFullPageDrawForBlackScrollStart;	///< N画面分の描画(縦ドラッグ開始時に有効)

	CTouchListCtrl(void);
	~CTouchListCtrl(void);

	virtual HBITMAP GetBgBitmapHandle() { return NULL; }
	virtual void PopupContextMenu( const CPoint ) { return; }
	virtual void SetSelectItem( const int ) { return; }
	virtual void MoveSlideRight() { Invalidate(); }
	virtual void MoveSlideLeft() { Invalidate(); }

	bool	m_bUseHorizontalDragMove;	///< 横ドラッグでの項目移動
	bool	m_bUsePanScrollAnimation;	///< 横スクロールアニメーションするか

	/// パンスクロールの方向定義
	enum PAN_SCROLL_DIRECTION
	{
		PAN_SCROLL_DIRECTION_NONE = 0,	///< どちらでもない
		PAN_SCROLL_DIRECTION_RIGHT,		///< 右方向
		PAN_SCROLL_DIRECTION_LEFT,		///< 左方向
	};
	void	StartPanScroll( PAN_SCROLL_DIRECTION direction );
	BOOL	WaitForPanScroll( DWORD dwMilliseconds );

	bool	m_bStopDragging;		///< ドラッグ操作停止

protected:
	int		m_offsetPixelY;			///< ピクセル単位スクロールのオフセットピクセル数

	int		m_iItemHeight;			///< Item一行の高さ

	bool	m_bDragging;			///< マウスドラッグ中
	CPoint	m_ptDragStart;			///< ドラッグ開始位置
	int		m_iDragLine;			///< ドラッグスクロール行数
	int		m_iDragStartItem;		///< ドラッグ開始時のクリックアイテム

	CRect	m_viewRect;				///< 描画領域：矩形
	int		m_screenWidth;			///< 描画領域：横幅
	int		m_screenHeight;			///< 描画領域：縦幅

	CDC*		m_memDC;			///< 裏画面DC
	CBitmap*	m_memBMP;			///< 裏画面バッファ
	CBitmap*	m_oldBMP;			///< 旧画面の情報
	int			m_yBackSurfaceStart;///< 裏画面の描画開始位置(表に転送する境界判定用)
	int			m_yBackSurfaceEnd;	///< 裏画面の描画終了位置(表に転送する境界判定用)

	int		m_drawStartTopOffset;	///< 描画開始オフセット(ピクセル)

	bool	m_bDrawBk;				///< DrawItemにおける背景描画フラグ
	bool	m_bTimerRedraw;			///< 遅延描画中フラグ

	AutoScrollManager	m_autoScrollInfo;	///< 慣性スクロール情報

	DWORD	m_dwAutoScrollStartTick;///< 慣性スクロール開始時刻
	int		m_yAutoScrollMax;		///< 慣性スクロール中の最大移動量
	bool	m_bAutoScrolling;		///< 慣性スクロール中

	bool	m_bScrollWithBk;		///< 背景同時スクロールするか

	// パン関連情報
	bool	m_bPanDragging;			///< 横方向マウスドラッグ中
	bool	m_bVerticalDragging;	///< スクロール中

	int		m_offsetPixelX;			///< 横方向オフセットピクセル数
	int		m_dPxelX;				///< 横方向単位時間移動量 [pixels/10msec]
	DWORD	m_dwPanScrollLastTick;	///< パンスクロール開始時刻
	bool	m_bCanSlide;			///< 左右移動可能か
	bool	m_bCanPanScroll;		///< パンスクロールアニメ可能か
	PAN_SCROLL_DIRECTION m_drPanScrollDirection;	///< パンスクロール方向
	HANDLE	m_hPanScrollEvent;		///<

public:
	// 背景描画フラグの設定／取得
	bool	IsDrawBk() { return m_bDrawBk; }
	void	SetDrawBk( const bool bDrawBk ) {
		m_bDrawBk = bDrawBk; 
	}
	// 描画
	int DrawBackSurface(bool bForceDraw=true);

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
	void	DrawItemWithBackSurface(int nItem);
private:
	bool	ScrollByMoveY( const int dy);
#define MyAdjustDrawOffset_ADJUST_ONLY	true
	bool	MyAdjustDrawOffset(bool bAdjustOnly=false);
	bool	MyMakeBackBuffers(CDC* pdc);
	void	DrawToScreen(CDC* pDC);
	void	DrawToScreen(CDC* pDC, const CRect& rectDest);
	void	PanDrawToScreen(CDC* pDC);
	void	DrawItemFocusRect( const int );

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

	// 縦スクロールタイマーのセット／リセット
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

	// パンスクロールタイマーのセット／リセット
	inline void	MySetPanScrollTimer( const int iInterval = TIMER_INTERVAL_TOUCHLIST_PANSCROLL ) {
		KillTimer( TIMERID_TOUCHLIST_PANSCROLL );
		SetTimer( TIMERID_TOUCHLIST_PANSCROLL, iInterval , NULL );
	}
	inline void	MyResetPanScrollTimer(){
		KillTimer( TIMERID_TOUCHLIST_PANSCROLL );
		SetEvent( m_hPanScrollEvent );
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
	void DrawItemToBackSurface(int nItem, bool bDrawBk=false);
	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
};
