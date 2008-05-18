/*
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

/*
 * TouchListCtrl.cpp : CTouchListCtrlクラス実装
 */
#include "StdAfx.h"
#include "TouchListCtrl.h"
#include "MZ3.h"
#include "util.h"
#include "util_gui.h"
#include "MZ3View.h"

IMPLEMENT_DYNAMIC(CTouchListCtrl, CListCtrl)

CTouchListCtrl::CTouchListCtrl(void)
	: m_offsetPixelY(0)
	, m_bDragging(false)
#ifndef WINCE
	, m_memBMP(NULL)
	, m_memDC(NULL)
#endif
	, m_bTimerRedraw(false)
	, m_bDrawBk(true)
{
}

CTouchListCtrl::~CTouchListCtrl(void)
{
}

BEGIN_MESSAGE_MAP(CTouchListCtrl, CListCtrl)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_SIZE()
	ON_WM_DESTROY()
//#ifndef WINCE
	ON_WM_VSCROLL()
//#endif
	ON_WM_MOUSEWHEEL()
	ON_WM_TIMER()
	ON_NOTIFY_REFLECT(LVN_DELETEALLITEMS, &CTouchListCtrl::OnLvnDeleteallitems)
	ON_NOTIFY_REFLECT(LVN_DELETEITEM, &CTouchListCtrl::OnLvnDeleteitem)
	ON_NOTIFY_REFLECT(LVN_INSERTITEM, &CTouchListCtrl::OnLvnInsertitem)
	ON_WM_RBUTTONDOWN()
END_MESSAGE_MAP()

/**
 * PreTranslateMessage
 *  ・トラブルシュート用
 */
BOOL CTouchListCtrl::PreTranslateMessage(MSG* pMsg)
{
	switch (pMsg->message) {
	case WM_MOUSEMOVE:
		break;

	case WM_PAINT:
		MZ3_TRACE( L"CTouchListCtrl::PreTranslateMessage(WM_PAINT, 0x%04X, 0x%04X)\n", pMsg->message, pMsg->wParam, pMsg->lParam);
		break;
	case WM_MOUSEWHEEL:
		MZ3_TRACE( L"CTouchListCtrl::PreTranslateMessage(WM_MOUSEWHEEL, 0x%04X, 0x%04X)\n", pMsg->message, pMsg->wParam, pMsg->lParam);
		break;
	case WM_VSCROLL:
		MZ3_TRACE( L"CTouchListCtrl::PreTranslateMessage(WM_VSCROLL, 0x%04X, 0x%04X)\n", pMsg->message, pMsg->wParam, pMsg->lParam);
		break;
//	case WM_PAINT:
		// ちらつき防止のため遮断
//		return TRUE;

	default:
		MZ3_TRACE( L"CTouchListCtrl::PreTranslateMessage(0x%04X, 0x%04X, 0x%04X)\n", pMsg->message, pMsg->wParam, pMsg->lParam);
		break;
	}

	return CListCtrl::PreTranslateMessage(pMsg);
}

/**
 * WM_LBUTTONDOWN メッセージハンドラ
 *  ・ドラッグを開始する
 */
void CTouchListCtrl::OnLButtonDown(UINT nFlags, CPoint point)
{
	MZ3_TRACE( L"OnLButtonDown()\n");

	// フォーカスを設定する
	SetFocus();

	// 慣性スクロール停止
	MyResetAutoScrollTimer();

#ifdef WINCE
	// タップ長押しでソフトキーメニュー表示
	SHRGINFO RGesture;
	RGesture.cbSize     = sizeof(SHRGINFO);
	RGesture.hwndClient = m_hWnd;
	RGesture.ptDown     = point;
	RGesture.dwFlags    = SHRG_RETURNCMD;
	if (::SHRecognizeGesture(&RGesture) == GN_CONTEXTMENU) {
		ClientToScreen(&point);
		PopupContextMenu(point);
		return;
	}
#endif

	// ドラッグ開始
	m_bDragging = true;
	m_ptDragStart = point;
	m_iDragLine = 0;
	m_offsetPixelY = 0;

	// キャプチャ開始
	SetCapture();

	// 慣性スクロール情報取得
	m_autoScrollInfo.push( GetTickCount(), point );

	// 標準処理を呼ぶとキャプチャとかWM_LBUTTONUPが怪しいので呼ばない
	// とりあえずクリック処理として
	//  ・アイテムの選択
	//  ・フォーカスの設定
	// をやってるが、足りなかったら追加しましょ
	//CListCtrl::OnLButtonDown(nFlags, point);
}

/**
 * WM_LBUTTONUP メッセージハンドラ
 *  ・ドラッグ中ならばドラッグを終了する
 */
void CTouchListCtrl::OnLButtonUp(UINT nFlags, CPoint point)
{
	MZ3_TRACE( L"OnLButtonUp(0x%X,%d,%d)\n", nFlags, point.x, point.y);

	if (m_bDragging) {
		// ドラッグ終了処理

		// キャプチャ終了
		ReleaseCapture();
		// フラグクリア
		m_bDragging = false;
		// 遅延描画タイマーのリセット
		MyResetRedrawTimer();

		// マウスカーソルを元に戻す
		::SetCursor( ::LoadCursor(NULL, IDC_ARROW) );

//#ifndef WINCE
//		// Win32では独自処理で描画する
//		// WMでは処理が追いつかないので標準処理に任せる
//		DrawDetail();
//		UpdateWindow();
//#else
//#ifndef TOUCHLIST_SCROLLWITHBK
//		// WMで、かつ背景同時スクロールでない場合は再描画
//		Invalidate();
//#endif
//#endif

		// 慣性スクロール情報取得
		m_autoScrollInfo.push( GetTickCount(), point );
		double speed = m_autoScrollInfo.calcMouseMoveSpeedY();
		MZ3_TRACE( L"! speed   : %5.3f [px/msec]\n", speed );
		// 慣性スクロール開始
		m_dwAutoScrollStartTick = GetTickCount();
		m_yAutoScrollMax = 0;
		MySetAutoScrollTimer( TIMER_INTERVAL_TOUCHLIST_AUTOSCROLL );

	}
	CListCtrl::OnLButtonUp(nFlags, point);
}

/**
 * WM_MOUSEMOVE メッセージハンドラ
 *  ・ドラッグ中ならば動的スクロール処理を行う
 */
void CTouchListCtrl::OnMouseMove(UINT nFlags, CPoint point)
{
	if( m_bDragging ) {
		// スクロール可能か？
		if ( GetItemCount() > GetCountPerPage() ) {
			// スクロール可能
			// グーのカーソルに変更
			::SetCursor( AfxGetApp()->LoadCursor(IDC_GRABBING_CURSOR) );

			// 縦ドラッグ処理
			ScrollByMoveY( point.y );
#ifdef WINCE
#ifndef TOUCHLIST_SCROLLWITHBK
			// WMで、かつ背景同時スクロールでない場合は遅延再描画
			MySetRedrawTimer( TIMER_INTERVAL_TOUCHLIST_SCROLLREDRAW );
#endif
#endif
			// 慣性スクロール情報取得
			m_autoScrollInfo.push( GetTickCount(), point );
		} else {
			::SetCursor( ::LoadCursor(NULL, IDC_ARROW) );
		}

	}

	CListCtrl::OnMouseMove(nFlags, point);
}

/**
 * WM_RESIZE メッセージハンドラ
 *	・描画領域の大きさ（クライアント領域からヘッダ領域を除く）の取得
 *  ・裏画面バッファの確保
 */
void CTouchListCtrl::OnSize(UINT nType, int cx, int cy)
{
	CListCtrl::OnSize(nType, cx, cy);

	CRect rctItem;								// 0番目のアイテムの領域
	CRect rctHeader;							// ヘッダ領域

	// クライアント領域を取得する
	GetClientRect( &m_viewRect );
	// ヘッダ領域を取得する
	GetHeaderCtrl()->GetWindowRect( &rctHeader );

	// 描画領域のtopをヘッダの高さ分縮める
	ScreenToClient( &rctHeader );
	m_viewRect.top = rctHeader.bottom ;

	// アイテム1個の高さを取得する
	if( GetItemCount() > 0 ){
		// アイテムが一つでもあれば高さを取得する
		GetItemRect( 0 , &rctItem , LVIR_BOUNDS);
		m_iItemHeight = rctItem.Height() ;
	} else {
		// ない時は取得しない（いいのか？
		m_iItemHeight = 1;
	}

	// 描画領域の幅と高さをメンバ変数に設定する
	m_screenWidth = cx;
	m_screenHeight = cy - rctHeader.Height();

#ifndef WINCE
	// 裏画面バッファが無ければここで確保する
	if( m_memBMP == NULL ) {
		CPaintDC	cdc(this);
		MyMakeBackBuffers(cdc);
	} else {
		// 裏画面バッファのサイズが小さい場合は再生成
		BITMAP bmp;
		GetObject(m_memBMP->m_hObject, sizeof(BITMAP), &bmp);
		if (bmp.bmWidth < m_screenWidth ||
			bmp.bmHeight < m_screenHeight*2) 
		{
			CPaintDC	cdc(this);
			MyMakeBackBuffers(cdc);
		}
	}
#endif
}

#ifndef WINCE
/**
 * 裏画面バッファの生成
 */
bool CTouchListCtrl::MyMakeBackBuffers(CPaintDC& cdc)
{
	//--- 解放
	// 裏画面バッファの解放
	if( m_memDC != NULL ){
		m_memDC->DeleteDC();
	}
	m_memDC = new CDC();

	if( m_memBMP != NULL ){
		m_memBMP->DeleteObject();
		delete m_memBMP;
	}
	m_memBMP = new CBitmap();
	
	//--- バッファ生成
	// 裏画面バッファの確保
	// 画面の高さを2倍して余裕をもたせてみた
	if (m_memBMP->CreateCompatibleBitmap( &cdc , m_screenWidth , m_screenHeight*2 ) != TRUE) {
		MessageBox(TEXT("CreateCompatibelBitmap error!"));
		return false;
	}
	// 実際に描画する領域の先頭オフセット
	m_drawStartTopOffset = m_screenHeight/2;

	// DCを生成
	m_memDC->CreateCompatibleDC(&cdc);
	m_memDC->SetBkMode(OPAQUE);					// 透過モードに設定する
	m_oldBMP = m_memDC->SelectObject(m_memBMP);

	return true;
}
#endif

/**
 * WM_DESTROY メッセージハンドラ
 *  ・裏画面バッファの解放
 */
void CTouchListCtrl::OnDestroy()
{
	CListCtrl::OnDestroy();
#ifndef WINCE
	//--- 解放
	// 裏画面バッファの解放
	if( m_memDC != NULL ){
		m_memDC->DeleteDC();
		delete m_memDC;
	}

	if( m_memBMP != NULL ){
		m_memBMP->DeleteObject();
		delete m_memBMP;
	}
#endif
}

#ifndef WINCE
/**
 * DrawToScreen() 描画
 *  裏画面バッファから画面物理デバイスへ転送
 */
void CTouchListCtrl::DrawToScreen(CDC* pDC)
{
	MZ3_TRACE( L"DrawToScreen(0x%X)\n", pDC!=NULL ? pDC->m_hDC : 0);

	// 変更後画面をm_offsetPixelY分ずらして表示する
	pDC->BitBlt( 
		0				, m_viewRect.top,
		m_screenWidth	, m_screenHeight, 
		m_memDC,
		0				, m_drawStartTopOffset + m_viewRect.top - m_offsetPixelY,
		SRCCOPY );
}
#endif

#ifndef WINCE
/**
 * DrawDetail()
 *  裏画面バッファにリスト項目を描画する
 */
int	CTouchListCtrl::DrawDetail( bool bForceDraw )
{
	MZ3_TRACE( L"DrawDetail(%s)\n", bForceDraw ? L"true" : L"false");

	// 念のため遅延描画タイマーを停止する
	MyResetRedrawTimer();

	// 背景を塗りつぶす
	BITMAP bmp;
	GetObject(m_memBMP->m_hObject, sizeof(BITMAP), &bmp);

	HBITMAP hBgBitmap = GetBgBitmapHandle();
	if( !theApp.m_optionMng.IsUseBgImage() || hBgBitmap == NULL ) {
		// 背景画像なしの場合
		m_memDC->FillSolidRect( 0, 0, bmp.bmWidth, bmp.bmHeight, RGB(255,255,255) );
	}else{
		// 背景ビットマップの描画
		CRect rectViewClient;
		this->GetClientRect( &rectViewClient );
		rectViewClient.OffsetRect( 0 , m_drawStartTopOffset-m_offsetPixelY );
		int x = rectViewClient.left;
		int y = rectViewClient.top;
		int w = rectViewClient.Width();
		int h = rectViewClient.Height() + m_iItemHeight * 2;
#ifdef TOUCHLIST_SCROLLWITHBK
		int offset = ( m_iItemHeight * GetTopIndex()  - m_offsetPixelY) % bmp.bmHeight;
#else
		int offset = 0;
#endif
		util::DrawBitmap( m_memDC->GetSafeHdc(), hBgBitmap, x, y, w, h, 0, 0 + offset );
	}

	MZ3_TRACE( L"Top=%5d,offset=%5d\n" , GetTopIndex() , m_offsetPixelY );

	// 透過モードに設定
	m_memDC->SetBkMode(TRANSPARENT);

	// フォントを現在のフォントに置き換え
	CFont* oldFont = (CFont*)m_memDC->SelectObject( GetFont() );

	// 画面に収まるItemを順番に描画する
	// オフセットスクロール用に1行余分に描画する。
	const int N_OVER_OFFSET_LINES = 1;
	for(int i=-N_OVER_OFFSET_LINES; i <= GetCountPerPage()+N_OVER_OFFSET_LINES ; i++){
		int nItem = GetTopIndex() + i;

		// 範囲を越えたらスルー
		if (nItem < 0) {
			continue;
		}
		if( GetItemCount() <= nItem ){
			break;
		}

		// DRAWITEMSTRUCTをでっちあげて派生クラスのDrawItem()をだます
		DRAWITEMSTRUCT dis;
		CRect rctItem;
		GetItemRect( nItem , &rctItem , LVIR_BOUNDS );
		// 描画領域はバッファ先頭からオフセットした位置にある
		rctItem.OffsetRect( 0 , m_drawStartTopOffset );

		dis.CtlType		= ODT_LISTVIEW;
		dis.CtlID		= 0;
		dis.itemAction	= ODA_DRAWENTIRE;
		dis.hwndItem	= m_hWnd;
		dis.hDC			= m_memDC->GetSafeHdc();
		dis.rcItem		= rctItem;
		dis.itemID		= nItem;
		dis.itemData	= GetItemData( nItem );

		// 背景は描画済みなので描画しない
		// （DrawItem()で背景描画されると、ズレが生じるので）
		SetDrawBk( false );

		// 本物のアイテム描画を使用することで実際の画面と描画内容を一致させる
		DrawItem( &dis );

		SetDrawBk( true );
	}

	// フォントを元に戻す
	m_memDC->SelectObject( oldFont );

	// 画面に転送する
	if (bForceDraw) {
		// 強制的に描画する
		CDC* pDC = GetDC();
		DrawToScreen(pDC);
		ReleaseDC(pDC);

		ValidateRect( &m_viewRect );
	}

	return(1);
}
#endif

/**
 * WM_VSCROLL メッセージハンドラ
 * (縦スクロールバーによるスクロール)
 *  ・ちらつきをなくすために裏画面バッファによる描画を行う
 */
void CTouchListCtrl::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	MZ3_TRACE( L"OnVScroll(%d)\n", nPos );

	// 慣性スクロール停止
	MyResetAutoScrollTimer();

	// スクロール中の描画を禁止
#ifndef WINCE
	LockWindowUpdate();
#endif

	CListCtrl::OnVScroll(nSBCode, nPos, pScrollBar);

	// スクロール中の描画を再開
#ifndef WINCE
	UnlockWindowUpdate();
#endif

#ifndef WINCE
	// Win32では独自処理で描画する
	// WMでは処理が追いつかないので標準処理に任せる
	DrawDetail();
	UpdateWindow();
#else
#ifndef TOUCHLIST_SCROLLWITHBK
	// WMで、かつ背景同時スクロールでない場合は遅延再描画
	MySetRedrawTimer( TIMER_INTERVAL_TOUCHLIST_SCROLLREDRAW );
#endif
#endif
}

/**
 * WM_MOUSEWHEEL メッセージハンドラ
 * (マウスホイールによるクロール)
 *  ・ちらつきをなくすために裏画面バッファによる描画を行う
 *   ただし、この時点でスクロール位置が確定していないので遅延描画を行う
 */
BOOL CTouchListCtrl::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	// 慣性スクロール停止
	MyResetAutoScrollTimer();

	// 遅延描画を行う
	// 本来はスクロール位置が確定するメッセージで描画処理を行うべき
	if( !m_bTimerRedraw ){
		// 描画を停止
#ifndef WINCE
		LockWindowUpdate();
#else
		SetRedraw( FALSE );
#endif
		MySetRedrawTimer( TIMER_INTERVAL_TOUCHLIST_SCROLLREDRAW );
	}

	MZ3_TRACE( L"CTouchListCtrl::OnMouseWheel( %5d ),Top=%5d\n", zDelta, GetTopIndex());
	//return CListCtrl::OnMouseWheel(nFlags, zDelta, pt);
	// 右クリック＋マウスホイール処理のために親呼び出し
	return theApp.m_pMainView->OnMouseWheel(nFlags, zDelta, pt);
}

/**
 * WM_TIMER メッセージハンドラ
 *  ・TIMERID_TOUCHLIST_SCROLLREDRAW
 *    遅延描画処理
 */
void CTouchListCtrl::OnTimer(UINT_PTR nIDEvent)
{
	MZ3_TRACE( L"**OnTimer(0x%04X)\n", nIDEvent );
	switch( nIDEvent ){
/*
 * 遅延描画処理
*/
		case TIMERID_TOUCHLIST_SCROLLREDRAW:
			// 遅延描画処理
			// 遅延描画タイマーを停止
			MyResetRedrawTimer();

			// 描画を再開
#ifndef WINCE
			UnlockWindowUpdate();
#else
			SetRedraw( TRUE );
#endif
#ifndef WINCE
			// Win32では独自処理で描画する
			// WMでは処理が追いつかないので標準処理に任せる
			DrawDetail();
			UpdateWindow();
#else
			Invalidate();
			UpdateWindow();
#endif
			break;

/*
 * 慣性スクロール処理
*/
		case TIMERID_TOUCHLIST_AUTOSCROLL:
			{
				// 慣性スクロール
				// らんらんビューの丸パクリ

				// 遅延描画タイマーを停止
				MyResetRedrawTimer();

				// 仮想的な移動量算出
				int dt = GetTickCount() - m_dwAutoScrollStartTick;
				MZ3_TRACE( L"*************AUTOSCROLL:dt=%d\n", dt );

				// 擬似的なマイナスの加速度とする。
#ifdef WINCE
				double accel = -m_autoScrollInfo.calcMouseMoveAccelY() * 0.06;	// マイナスの加速度, スケーリングは適当ｗ
#else
				double accel = -m_autoScrollInfo.calcMouseMoveAccelY() * 0.06;	// マイナスの加速度, スケーリングは適当ｗ
#endif
				double speed = m_autoScrollInfo.calcMouseMoveSpeedY();

				int dyByAccel    = (int)((accel*dt*dt)/2.0);		// マイナスの加速度
				int dyByVelocity = (int)(dt * speed);				// 初速による移動	
				int dyAutoScroll = dyByAccel + dyByVelocity;		// LButtonUp からの移動量

				MZ3_TRACE( L"*************AUTOSCROLL:dyAutoScroll=%d\n", dyAutoScroll );
				MZ3_TRACE( L"*************AUTOSCROLL:m_yAutoScrollMax=%d\n", m_yAutoScrollMax );

				// 最大位置より戻った（極点を超えた）、
				// 加速度がしきい値より小さい、
				// またはN秒経過したなら終了
				if (speed == 0.0 ||
					(speed < 0 && dyAutoScroll > m_yAutoScrollMax) ||
					(speed > 0 && dyAutoScroll < m_yAutoScrollMax) ||
					(fabs(accel)<0.00005) ||
					dt > 5 * 1000)
				{
					if( m_offsetPixelY != 0 ){
						// オフセットのズレを解消
						MyAdjustDrawOffset();
					} else {
						// 停止状態で最後の描画
#ifndef WINCE
						// Win32では独自処理で描画する
						// WMでは処理が追いつかないので標準処理に任せる
						DrawDetail();
						UpdateWindow();
#else
#ifndef TOUCHLIST_SCROLLWITHBK
						// WMで、かつ背景同時スクロールでない場合は再描画
						Invalidate();
#endif
#endif
					}

					// 慣性スクロールの停止
					MZ3_TRACE( L"*************AUTOSCROLL:KillTimer#1\n" );
					MyResetAutoScrollTimer();

				} else {
					// dyAutoScroll 分だけ移動する。
					CPoint lastPoint = m_autoScrollInfo.getLastPoint();

					int dy = lastPoint.y + dyAutoScroll;

					MZ3_TRACE( L"*************AUTOSCROLL:ScrollByMoveY(%d)\n" , dy );
					if( ScrollByMoveY( dy ) ){
						// 先頭か最後尾に達した
						// 慣性スクロールの停止
						MZ3_TRACE( L"*************AUTOSCROLL:KillTimer#2\n" );
						MyResetAutoScrollTimer();
					}
				}

				m_yAutoScrollMax = dyAutoScroll;
				MZ3_TRACE( L"*************AUTOSCROLL:m_yAutoScrollMax=%d\n", m_yAutoScrollMax );
				break;
			}
		default:
			CListCtrl::OnTimer(nIDEvent);
			break;
	}
}

/**
 * ScrollByMoveY(const int dy)
 * ドラッグ中の描画処理
 * 引数
 *   dy：マウスのY座標（point.y値、CTouchListCtrlコントロールのローカル座標、絶対値）
 * 戻り値
 *   描画位置が先頭か最後尾になった場合にtrue、そうでない場合にfalseを返す
 * 関連するメンバ変数
 *   m_ptDragStart	：(I)	：ドラッグ開始点
 *   m_iItemHeight	：(I)	：アイテム1個当たりの高さ（ピクセル）
 *   m_iDragLine	：(I/O)	：ドラッグ開始から現在までにスクロールした行数
 *   m_offsetPixelY	：(O)	：ピクセル単位スクロールのためのオフセット値
 * 機能
 * ・ドラッグ開始点（m_ptDragStart.y）からマウスの現在位置（dy）の移動量を基に
 *   画面に表示するItemの位置を決定し、スクロールして表示する
 */
bool CTouchListCtrl::ScrollByMoveY(const int dy)
{
	// 先頭or最後尾フラグ
	bool bLimitOver = false;

	// 現時点での表示アイテム先頭位置、オフセットを保存する
	int iTop = GetTopIndex();
	int iOffset = m_offsetPixelY;

	// Drag開始点からの移動行数を求める
	int iScrollLine = (( dy - m_ptDragStart.y ) / m_iItemHeight ) ;

#ifndef WINCE
	// PIXEL単位でスクロールさせる
	m_offsetPixelY = (( dy - m_ptDragStart.y ) % m_iItemHeight );
#else
	// WMではItem単位でスクロールさせる
	m_offsetPixelY = 0;
#endif

	// 前回の移動行数からの差分だけスクロールする
	//  Scroll()メソッドはピクセル数指定だが、ピクセル指定すると
	//  1行分の半分くらいで1行移動したりしてマウスの動きと同期が取れないので
	//  行数×高さで動かす
	if( abs(iScrollLine - m_iDragLine) > 0 ){
		CSize szScroll( 0 , -(( iScrollLine - m_iDragLine ) * m_iItemHeight) );

		MZ3_TRACE( L"ScrollByMoveY, Scroll(%5d)\n" , szScroll.cy );

		// Win32では独自処理で描画する
		// WMでは処理が追いつかないので標準処理に任せる
		// スクロール中の再描画を禁止
#ifndef WINCE
		LockWindowUpdate();
#endif

		// スクロール実行
		Scroll( szScroll );
				
		// スクロール中の再描画を再開
#ifndef WINCE
		UnlockWindowUpdate();
#endif
	}

	// スクロール後の先頭位置を取得する
	int iNextTop = GetTopIndex();

	// 上端より上または下端より下ならばオフセットを効かせない
	if( ( iNextTop < 0 ) ||
		( iNextTop  == 0 && m_offsetPixelY > 0) ||
		( GetItemCount() - GetCountPerPage() == iNextTop && m_offsetPixelY < 0) ||
		( GetItemCount() - GetCountPerPage() < iNextTop ) ){
		m_offsetPixelY = 0;
		// 先頭or最後尾フラグ
		bLimitOver = true;
	}

	if( iTop != iNextTop ) {
		// Item表示位置が変わった
		// トータル移動行数を蓄積する
		m_iDragLine += iTop - iNextTop;

#ifndef WINCE
		// 再描画
		MZ3_TRACE( L"ScrollByMoveY, DrawDetail() req1\n" );
		// Win32では独自処理で描画する
		// WMでは処理が追いつかないので標準処理に任せる
		DrawDetail();
		UpdateWindow();
#endif
	} else {
		// Item表示位置は変わらない
		if( iOffset != m_offsetPixelY ){
			// オフセットが変わった

#ifndef WINCE
			// 再描画
			MZ3_TRACE( L"ScrollByMoveY, DrawDetail() req2\n" );
			// Win32では独自処理で描画する
			// WMでは処理が追いつかないので標準処理に任せる
			DrawDetail();
			UpdateWindow();
#endif
		}
	}

	return bLimitOver;
}

/*
 * タイマー停止処理
*/
void CTouchListCtrl::ResetAllTimer()
{
	// 遅延描画タイマーを停止
	MyResetRedrawTimer();
	// 慣性スクロールタイマーを停止
	MyResetAutoScrollTimer();
}

// 項目に変化があればタイマー処理を停止する
// 思いつくまま並べてみたが、親からResetAllTimer()を呼んだ方がいいかも

void CTouchListCtrl::OnLvnDeleteallitems(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	ResetAllTimer();
	*pResult = 0;
}

void CTouchListCtrl::OnLvnDeleteitem(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	ResetAllTimer();
	*pResult = 0;
}

void CTouchListCtrl::OnLvnInsertitem(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	ResetAllTimer();
	*pResult = 0;
}

void CTouchListCtrl::OnRButtonDown(UINT nFlags, CPoint point)
{
	// オフセットのズレを解消
	MyAdjustDrawOffset();

	// 慣性スクロールの停止
	MyResetAutoScrollTimer();

	CListCtrl::OnRButtonDown(nFlags, point);
}

bool CTouchListCtrl::MyAdjustDrawOffset()
{
	bool bMove = false;
	if( m_offsetPixelY != 0 ){
		// オフセットが残っているなら
		bMove = true;

		// 基本はオフセット分引き戻す
		int dy = m_iDragLine * m_iItemHeight + m_ptDragStart.y;

		if( m_yAutoScrollMax > 0) {
			// 下方向移動中
			if( m_offsetPixelY > 0 ){
				// 下にはみ出ていれば１コ下に下げる
				dy += m_iItemHeight;
			}
		} else if( m_yAutoScrollMax < 0 ) {
			// 上方向移動中
			if( m_offsetPixelY < 0 ){
				// 上にはみ出ていれば１コ上に上げる
				dy -= m_iItemHeight;
			}
		} else {
			// 停止中
			if( m_offsetPixelY > m_iItemHeight / 2 ){
				// 1行の半分以上、下にはみ出ていれば1コ下に下げる
				dy += m_iItemHeight;
			} else if( m_offsetPixelY < - m_iItemHeight / 2 ){
				// 1行の半分以上、上にはみ出ていれば1コ上に上げる
				dy -= m_iItemHeight;
			}
		}
		// スクロール
		MZ3_TRACE( L"*************AUTOSCROLL:m_iDragLine=%d,m_ptDragStart.y=%d,m_iItemHeight=%d\n" , m_iDragLine , m_ptDragStart.y , m_iItemHeight );
		MZ3_TRACE( L"*************AUTOSCROLL:ScrollByMoveY(%d)\n" , dy );
		ScrollByMoveY( dy );
#ifdef WINCE
#ifndef TOUCHLIST_SCROLLWITHBK
		// WMで、かつ背景同時スクロールでない場合は再描画
		Invalidate();
#endif
#endif
	}

	return bMove;
}
