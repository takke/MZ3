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
#include "MouseGestureManager.h"
#include "MZ3.h"
#include "util.h"
#include "util_gui.h"
#include "MZ3View.h"

IMPLEMENT_DYNAMIC(CTouchListCtrl, CListCtrl)

CTouchListCtrl::CTouchListCtrl(void)
	: m_bStopDraw(false)
	, m_bBlackScrollMode(false)
	, m_offsetPixelY(0)
	, m_bDragging(false)
	, m_memBMP(NULL)
	, m_memDC(NULL)
	, m_bTimerRedraw(false)
	, m_bDrawBk(true)
	, m_bScrollWithBk(true)
	, m_bAutoScrolling(false)
	, m_bUseHorizontalDragMove(true)
	, m_bPanDragging(false)
	, m_bScrollDragging(false)
	, m_bCanSlide(false)
	, m_iDragStartItem(-1)
	, m_bUsePanScrollAnimation(true)
	, m_offsetPixelX(0)
	, m_drPanScrollDirection( PAN_SCROLL_DIRECTION_NONE )
	, m_bCanPanScroll(false)
	, m_hPanScrollEvent(NULL)
	, m_bStopDragging(false)
{
	// イベントオブジェクト作成
	m_hPanScrollEvent = CreateEvent( NULL , TRUE , TRUE , NULL );
}

CTouchListCtrl::~CTouchListCtrl(void)
{
	// イベントオブジェクトクローズ
	if(	m_hPanScrollEvent ){
		CloseHandle( m_hPanScrollEvent );
	}

}

BEGIN_MESSAGE_MAP(CTouchListCtrl, CListCtrl)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_SIZE()
	ON_WM_DESTROY()
	ON_WM_VSCROLL()
	ON_WM_MOUSEWHEEL()
	ON_WM_TIMER()
	ON_NOTIFY_REFLECT(LVN_DELETEALLITEMS, &CTouchListCtrl::OnLvnDeleteallitems)
	ON_NOTIFY_REFLECT(LVN_DELETEITEM, &CTouchListCtrl::OnLvnDeleteitem)
	ON_NOTIFY_REFLECT(LVN_INSERTITEM, &CTouchListCtrl::OnLvnInsertitem)
	ON_WM_RBUTTONDOWN()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_DRAWITEM()
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
//		MZ3_TRACE( L"CTouchListCtrl::PreTranslateMessage(WM_PAINT, 0x%04X, 0x%04X)\n", pMsg->message, pMsg->wParam, pMsg->lParam);
		break;
	case WM_MOUSEWHEEL:
//		MZ3_TRACE( L"CTouchListCtrl::PreTranslateMessage(WM_MOUSEWHEEL, 0x%04X, 0x%04X)\n", pMsg->message, pMsg->wParam, pMsg->lParam);
		break;
	case WM_VSCROLL:
//		MZ3_TRACE( L"CTouchListCtrl::PreTranslateMessage(WM_VSCROLL, 0x%04X, 0x%04X)\n", pMsg->message, pMsg->wParam, pMsg->lParam);
		break;
//	case WM_PAINT:
		// ちらつき防止のため遮断
//		return TRUE;

	case WM_DRAWITEM:
//		MZ3_TRACE(L"CTouchListCtrl::PreTranslateMessage(WM_VSCROLL, 0x%04X, 0x%04x)\n", pMsg->wParam, pMsg->lParam);
		break;

	default:
//		MZ3_TRACE( L"CTouchListCtrl::PreTranslateMessage(0x%04X, 0x%04X, 0x%04X)\n", pMsg->message, pMsg->wParam, pMsg->lParam);
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
	if (!m_bDragging && m_bStopDragging) {
		return;
	}

	MZ3_TRACE( L"OnLButtonDown()\n");

	// フォーカスを設定する
	SetFocus();

	// オフセットのずれを調整
	MyAdjustDrawOffset();
	// 慣性スクロール停止
	MyResetAutoScrollTimer();

	m_iDragStartItem = HitTest( point );

	//if( !m_bCanSlide ){
	//	SetSelectItem( m_iDragStartItem );
	//}
#ifdef WINCE
	// フォーカス矩形を表示
	DrawItemFocusRect( m_iDragStartItem );
	// タップ長押しでソフトキーメニュー表示
	SHRGINFO RGesture;
	RGesture.cbSize     = sizeof(SHRGINFO);
	RGesture.hwndClient = m_hWnd;
	RGesture.ptDown     = point;
	RGesture.dwFlags    = SHRG_RETURNCMD;
	if (::SHRecognizeGesture(&RGesture) == GN_CONTEXTMENU) {
		// 長押しした
		// フォーカス矩形を消す
		DrawItemFocusRect( m_iDragStartItem );
		// 項目を選択する
		SetSelectItem( m_iDragStartItem );
		RedrawItems( m_iDragStartItem , m_iDragStartItem );
		// メニューをポップアップ
		ClientToScreen(&point);
		PopupContextMenu(point);
		return;
	}
	// フォーカス矩形を消す
	DrawItemFocusRect( m_iDragStartItem );
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
	//  ・クリック通知メッセージを親に送る
	// をやってるが、足りなかったら追加しましょ
	//CListCtrl::OnLButtonDown(nFlags, point);
}

/**
 * WM_LBUTTONUP メッセージハンドラ
 *  ・ドラッグ中ならばドラッグを終了する
 */
void CTouchListCtrl::OnLButtonUp(UINT nFlags, CPoint point)
{
	if (!m_bDragging && m_bStopDragging) {
		return;
	}

	MZ3_TRACE( L"OnLButtonUp(0x%X,%d,%d)\n", nFlags, point.x, point.y);
	int dx = point.x - m_ptDragStart.x;
	int dy = point.y - m_ptDragStart.y;

	if (m_bDragging) {
		// ドラッグ終了処理

		// キャプチャ終了
		ReleaseCapture();

		// dx,dyのドラッグ量に応じて、ドラッグ開始かどうかを判定する
		// m_bPanDragging, m_bScrollDragging, m_drPanScrollDirection が設定される
		MySetDragFlagWhenMovedPixelOverLimit(dx,dy);

		// 遅延描画タイマーのリセット
		MyResetRedrawTimer();

		// マウスカーソルを元に戻す
		::SetCursor( ::LoadCursor(NULL, IDC_ARROW) );

		if( !m_bPanDragging && !m_bScrollDragging ) {
			// 上下左右のドラッグがなければアイテムを選択する
			// 選択
			SetSelectItem( m_iDragStartItem );
			// クリック通知メッセージを親に送る
			int nItem = util::MyGetListCtrlSelectedItemIndex( *this );
			NMLISTVIEW nmlv;
			nmlv.hdr.hwndFrom = m_hWnd;
			nmlv.hdr.idFrom = 0 ;
			nmlv.hdr.code = NM_CLICK ;
			nmlv.iItem = nItem ;
			nmlv.iSubItem = 0 ;
			nmlv.ptAction = point ;
			nmlv.uChanged = 0;
			nmlv.uNewState = 0;
			nmlv.uOldState = LVIS_FOCUSED | LVIS_SELECTED;
			nmlv.lParam = NULL;
			GetParent()->SendMessage( WM_NOTIFY , NM_CLICK , (LPARAM)&nmlv );
		}

		// スクロール中か？
		if ( m_bScrollDragging ) {

			// 慣性スクロール情報取得
			m_autoScrollInfo.push( GetTickCount(), point );
			double speed = m_autoScrollInfo.calcMouseMoveSpeedY();
			MZ3_TRACE( L"! speed   : %5.3f [px/msec]\n", speed );
			if( abs(point.y - m_ptDragStart.y) > m_iItemHeight ){
				// 1行以上ドラッグしているので慣性スクロール開始
				MZ3_TRACE( L" 1行以上ドラッグしているので慣性スクロール開始\n" );
				m_dwAutoScrollStartTick = GetTickCount();
				m_yAutoScrollMax = 0;
				MySetAutoScrollTimer( TIMER_INTERVAL_TOUCHLIST_AUTOSCROLL );
			} else {
				// 1行未満のドラッグならばすぐに止める
				MyAdjustDrawOffset(MyAdjustDrawOffset_ADJUST_ONLY);
				m_autoScrollInfo.clear();

				// ★強制再描画
				MZ3_TRACE( L" ★強制再描画#0\n" );
				m_bDragging = false;
				m_bScrollDragging = false;
				DrawDetail(true);
				UpdateWindow();
			}
		} else if( m_bPanDragging ){ 
			// 横方向にドラッグ
			switch( m_drPanScrollDirection ) {
			case PAN_SCROLL_DIRECTION_RIGHT:
				MoveSlideRight();
				break;
			case PAN_SCROLL_DIRECTION_LEFT:
				MoveSlideLeft();
				break;
			}
		}
		// フラグクリア
		m_bDragging = false;
		m_bPanDragging = false;
		m_bScrollDragging = false;
		m_drPanScrollDirection = PAN_SCROLL_DIRECTION_NONE;
	}
	//CListCtrl::OnLButtonUp(nFlags, point);
}

/**
 * WM_MOUSEMOVE メッセージハンドラ
 *  ・ドラッグ中ならば動的スクロール処理を行う
 */
void CTouchListCtrl::OnMouseMove(UINT nFlags, CPoint point)
{
	if (!m_bDragging && m_bStopDragging) {
		return;
	}

	if( m_bDragging ) {
		int dx = point.x - m_ptDragStart.x;
		int dy = point.y - m_ptDragStart.y;

		MZ3_TRACE(L"CTouchListCtrl::OnMouseMove\n");

		// dx,dyのドラッグ量に応じて、ドラッグ開始かどうかを判定する
		// m_bPanDragging, m_bScrollDragging, m_drPanScrollDirection が設定される
		MySetDragFlagWhenMovedPixelOverLimit(dx, dy);

		// 縦スクロール中か？
		if ( m_bScrollDragging ) {
			//if( m_bCanSlide ){
			//	SetSelectItem( m_iDragStartItem );
			//}
			// 縦スクロール中
			// グーのカーソルに変更
			::SetCursor( AfxGetApp()->LoadCursor(IDC_GRABBING_CURSOR) );

			// 縦ドラッグ処理
			ScrollByMoveY( point.y );
#ifdef WINCE
			if( !IsScrollWithBk() ){
				// WMで、かつ背景同時スクロールでない場合は遅延再描画
				MySetRedrawTimer( TIMER_INTERVAL_TOUCHLIST_SCROLLREDRAW_L );
			}
#endif
			// 慣性スクロール情報取得
			m_autoScrollInfo.push( GetTickCount(), point );
		} else if( m_bPanDragging ){
			// 横スクロール中
			// マウスポインタ変更
			switch( m_drPanScrollDirection ){
				case PAN_SCROLL_DIRECTION_LEFT:
					// 左方向
					::SetCursor( AfxGetApp()->LoadCursor(IDC_ARROW_LEFT_CURSOR) );
					break;
				case PAN_SCROLL_DIRECTION_RIGHT:
					// 右方向
					::SetCursor( AfxGetApp()->LoadCursor(IDC_ARROW_RIGHT_CURSOR) );
					break;
			}
		} else {
			::SetCursor( ::LoadCursor(NULL, IDC_ARROW) );
#ifdef WINCE
			// フォーカス矩形を表示
			DrawItemFocusRect( m_iDragStartItem );
			// タップ長押しでソフトキーメニュー表示
			// （指先が微動した時のため）
			SHRGINFO RGesture;
			RGesture.cbSize     = sizeof(SHRGINFO);
			RGesture.hwndClient = m_hWnd;
			RGesture.ptDown     = point;
			RGesture.dwFlags    = SHRG_RETURNCMD;
			if (::SHRecognizeGesture(&RGesture) == GN_CONTEXTMENU) {
				// 長押しした
				// フォーカス矩形を消す
				DrawItemFocusRect( m_iDragStartItem );

				// WM_LBUTTONUPと同様にドラッグ終了処理を行う

				// キャプチャ終了
				ReleaseCapture();
				// 遅延描画タイマーのリセット
				MyResetRedrawTimer();
				// 選択状態の設定
				SetSelectItem( m_iDragStartItem );
				RedrawItems( m_iDragStartItem , m_iDragStartItem );
				// オフセットの調整（念のため）
				MyAdjustDrawOffset();
				m_autoScrollInfo.clear();
				// ドラッグフラグクリア
				m_bDragging = false;
				m_bPanDragging = false;
				m_bScrollDragging = false;
				m_drPanScrollDirection = PAN_SCROLL_DIRECTION_NONE;

				// メニューポップアップ
				ClientToScreen(&point);
				PopupContextMenu(point);
				return;
			}
			// フォーカス矩形を消す
			DrawItemFocusRect( m_iDragStartItem );
#endif
		}

	}

	//CListCtrl::OnMouseMove(nFlags, point);
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

	// 裏画面バッファが無ければここで確保する
	if( m_memBMP == NULL ) {
		CDC* pDC = GetDC();
		MyMakeBackBuffers(pDC);
		ReleaseDC(pDC);
	} else {
		// 裏画面バッファのサイズが小さい場合は再生成
		BITMAP bmp;
		GetObject(m_memBMP->m_hObject, sizeof(BITMAP), &bmp);
		if (bmp.bmWidth < m_screenWidth ||
			bmp.bmHeight < m_screenHeight*2) 
		{
			CDC* pDC = GetDC();
			MyMakeBackBuffers(pDC);
			ReleaseDC(pDC);
		}
	}
}

/**
 * 裏画面バッファの生成
 */
bool CTouchListCtrl::MyMakeBackBuffers(CDC* pdc)
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
	if (m_memBMP->CreateCompatibleBitmap( pdc , m_screenWidth , m_screenHeight*2 ) != TRUE) {
		MessageBox(TEXT("CreateCompatibelBitmap error!"));
		return false;
	}
	// 実際に描画する領域の先頭オフセット
	m_drawStartTopOffset = m_screenHeight/2;

	// DCを生成
	m_memDC->CreateCompatibleDC(pdc);
	m_memDC->SetBkMode(OPAQUE);					// 透過モードに設定する
	m_oldBMP = m_memDC->SelectObject(m_memBMP);

	return true;
}

/**
 * WM_DESTROY メッセージハンドラ
 *  ・裏画面バッファの解放
 */
void CTouchListCtrl::OnDestroy()
{
	CListCtrl::OnDestroy();
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
}

/**
 * DrawToScreen() 描画
 *
 *  裏画面バッファから画面物理デバイスへ転送
 */
void CTouchListCtrl::DrawToScreen(CDC* pDC)
{
	MZ3_TRACE( L"★★DrawToScreen(0x%X)\n", pDC!=NULL ? pDC->m_hDC : 0);
	if( m_memDC == NULL ){
		MZ3_TRACE( L" invalid memory DC...\n");
		return;
	}

	if (!m_bBlackScrollMode) {
		int ys = m_drawStartTopOffset + m_viewRect.top - m_offsetPixelY;

		MZ3_TRACE( L" (%d,%d) => (%d,%d) [%d,%d]\n", 
			0				, ys,
			0				, m_viewRect.top,
			m_screenWidth	, m_screenHeight);

		pDC->BitBlt( 
			0				, m_viewRect.top,
			m_screenWidth	, m_screenHeight, 
			m_memDC,
			0				, ys,
			SRCCOPY );
	} else {
		int ys = 0;
		// ★スクロールドラッグ中、または慣性スクロール中であれば、描画範囲を計算する
		int offsetPixelY = 0;
		if (m_bScrollDragging) {
			// スクロールドラッグ中 ⇒カーソル座標とドラッグ開始座標から描画範囲を計算
			offsetPixelY = m_autoScrollInfo.getLastPoint().y - m_ptDragStart.y;
		} else if (m_bAutoScrolling) {
			// 慣性スクロール中     ⇒慣性スクロール座標とドラッグ開始位置から描画範囲を計算
			offsetPixelY = m_autoScrollInfo.getLastPoint().y - m_ptDragStart.y + m_yAutoScrollMax;
		} else {
			// m_offsetPixelY 分ずらして表示する
			offsetPixelY = m_offsetPixelY;
		}
		ys = m_drawStartTopOffset + m_viewRect.top - offsetPixelY;

		// 裏画面バッファの転送
		if (m_bScrollDragging || m_bAutoScrolling) {
			int h = m_screenHeight;
			int ydest = m_viewRect.top;

			MZ3_TRACE( L" offsetPixelY : %d\n", offsetPixelY);
			if (offsetPixelY > 0) {
				// 上側を塗りつぶす
				h     = m_screenHeight - offsetPixelY;
				ydest = m_viewRect.top + offsetPixelY;
				ys    = m_drawStartTopOffset + m_viewRect.top;
			} else if (offsetPixelY < 0) {
				// 下側を塗りつぶす
				h     = m_screenHeight + offsetPixelY;
				ydest = m_viewRect.top;
				ys    = m_drawStartTopOffset + m_viewRect.top - offsetPixelY;
			}

			// 描画範囲分だけ転送する
			MZ3_TRACE( L" (%d,%d) => (%d,%d) [%d,%d]\n", 
				0				, ys,
				0				, ydest,
				m_screenWidth	, h);
			pDC->BitBlt( 
				0				, ydest,
				m_screenWidth	, h, 
				m_memDC,
				0				, ys,
				SRCCOPY );

			// 描画範囲外を塗りつぶす
			if (ydest > m_viewRect.top) {
				// 上側を塗りつぶす
				int hFill = offsetPixelY;
				int yFill = 0;
				pDC->FillSolidRect(0, m_viewRect.top+yFill, m_screenWidth, hFill, RGB(0,0,0));
			} else if (h < m_screenHeight) {
				// 下側を塗りつぶす
				int hFill = m_screenHeight - h;
				int yFill = ydest + h;
				pDC->FillSolidRect(0, m_viewRect.top+yFill, m_screenWidth, hFill, RGB(0,0,0));
			}

		} else {
			MZ3_TRACE( L" (%d,%d) => (%d,%d) [%d,%d]\n", 
				0				, ys,
				0				, m_viewRect.top,
				m_screenWidth	, m_screenHeight);

			pDC->BitBlt( 
				0				, m_viewRect.top,
				m_screenWidth	, m_screenHeight, 
				m_memDC,
				0				, ys,
				SRCCOPY );
		}
	}
}

/**
 * DrawToScreen() 描画
 *
 *  裏画面バッファから画面物理デバイスへ転送
 */
void CTouchListCtrl::DrawToScreen(CDC* pDC, const CRect& rectDest)
{
//	MZ3_TRACE( L"DrawToScreen(0x%X, %d, %d, %d, %d)\n", pDC!=NULL ? pDC->m_hDC : 0, 
//		rectDest.left, rectDest.top, rectDest.right, rectDest.bottom);
	if( m_memDC == NULL ){
		return;
	}
	// 先頭アイテムの矩形取得
	// →1行表示モード時の「カラム」部分(y=18など)を除くため。2行表示モードの場合は y=0。
	CRect rectItem0;
	GetItemRect(GetTopIndex(), &rectItem0, LVIR_BOUNDS);
	int hHeaderColumn = rectItem0.top;

	// 変更後画面をm_offsetPixelY分ずらして表示する
	int x = rectDest.left;
	int y = rectDest.top +m_viewRect.top -hHeaderColumn;
	int w = rectDest.Width();
	int h = rectDest.Height();
	// y が描画範囲外なら描画しない。
	if (y>=rectItem0.top) {
		pDC->BitBlt( 
			x, y,
			w, h, 
			m_memDC,
			x, y +m_drawStartTopOffset -m_offsetPixelY,
			SRCCOPY );
	}
}

/**
 * DrawItemWithBackSurface() 描画
 *
 *  裏画面バッファから画面物理デバイスへ転送
 */
void CTouchListCtrl::DrawItemWithBackSurface(int nItem)
{
	MZ3_TRACE( L"DrawItemWithBackSurface(%d)\n", nItem);
	if( m_memDC == NULL ){
		return;
	}

	// アイテムの矩形取得
	CRect rectItem;
	GetItemRect(nItem, &rectItem, LVIR_BOUNDS);

	// 背景を塗りつぶす
	BITMAP bmp;
	GetObject(m_memBMP->m_hObject, sizeof(BITMAP), &bmp);

	HBITMAP hBgBitmap = GetBgBitmapHandle();
	CRect r = rectItem;
	r.OffsetRect( 0, m_drawStartTopOffset-m_offsetPixelY );
	int x = r.left;
	int y = r.top;
	int w = r.Width();
	int h = r.Height();
//	int h = r.Height() + m_iItemHeight*2;
	
	MZ3_TRACE( L"DrawToScreen(%d), [%d, %d, %d, %d], [%d,%d,%d,%d], %d, %d\n", 
		nItem, 
		rectItem.left, rectItem.top, rectItem.Width(), rectItem.Height(),
		x, y, w, h, m_drawStartTopOffset, m_offsetPixelY);

	if( !theApp.m_optionMng.IsUseBgImage() || hBgBitmap == NULL ) {
		// 背景画像なしの場合
		m_memDC->FillSolidRect(x, y, x+w, y+h, RGB(255,255,255));
	}else{
		// 背景ビットマップの描画
		int offset = 0;
		if( IsScrollWithBk() ){
			offset = ( m_iItemHeight * GetTopIndex()  - m_offsetPixelY) % bmp.bmHeight;
		}
		util::DrawBitmap( m_memDC->GetSafeHdc(), hBgBitmap, x, y, w, h, x, rectItem.top + offset );
	}

	// デバッグ用矩形
//	m_memDC->Rectangle(x, y, x+w, y+h);

	// 透過モードに設定
	m_memDC->SetBkMode(TRANSPARENT);

	// フォントを現在のフォントに置き換え
	CFont* oldFont = (CFont*)m_memDC->SelectObject( GetFont() );

	// バックバッファに描画
	DrawItemToBackSurface(nItem);

	// フォントを元に戻す
	m_memDC->SelectObject( oldFont );

	// 強制的に描画する
	CDC* pDC = GetDC();
	DrawToScreen(pDC, rectItem);
//	DrawToScreen(pDC);
	ReleaseDC(pDC);

	ValidateRect( &rectItem );
}

/**
 * PanDrawToScreen() 描画
 *  パンスクロール用に裏画面バッファから画面物理デバイスへ転送
 */
void CTouchListCtrl::PanDrawToScreen(CDC* pDC)
{
	MZ3_TRACE( L"PanDrawToScreen(0x%X)\n", pDC!=NULL ? pDC->m_hDC : 0);
	if( m_memDC == NULL ){
		return;
	}
	int sx = 0;
	int dx = 0;
	int wid = m_screenWidth;

	// パンスクロール処理
	if( m_offsetPixelX > 0 ){
		// 右にずれている場合
		sx = 0;
		dx = m_offsetPixelX;
		wid = m_screenWidth - m_offsetPixelX;

		// 変更前画面を左側に表示する
		pDC->BitBlt( 0 , m_viewRect.top , m_offsetPixelX , m_screenHeight , m_memDC , wid , m_drawStartTopOffset + m_viewRect.top - m_offsetPixelY, SRCCOPY );

		// 移動がわかるように縦線を引く
		pDC->MoveTo( m_offsetPixelX - 1 , m_viewRect.top );
		pDC->LineTo( m_offsetPixelX - 1 , m_viewRect.bottom );
	} else if( m_offsetPixelX < 0 ) {
		// 左にずれている場合
		sx = -m_offsetPixelX;
		dx = 0;
		wid = m_screenWidth + m_offsetPixelX;

		// 変更前画面を右側に表示する
		pDC->BitBlt( wid , m_viewRect.top , sx , m_screenHeight , m_memDC , 0 , m_drawStartTopOffset + m_viewRect.top - m_offsetPixelY, SRCCOPY );

		// 移動がわかるように縦線を引く
		pDC->MoveTo( wid + 1 , m_viewRect.top);
		pDC->LineTo( wid + 1 , m_viewRect.bottom );
	}

	// オフセットが一画面分あれば表示不要
	if( abs( m_offsetPixelX ) > abs( m_screenWidth ) ){
		return;
	}

	// 背景をオフセットに合わせて表示する
	BITMAP bmp;
	GetObject(m_memBMP->m_hObject, sizeof(BITMAP), &bmp);

	HBITMAP hBgBitmap = GetBgBitmapHandle();
	if( !theApp.m_optionMng.IsUseBgImage() || hBgBitmap == NULL ) {
		// 背景画像なしの場合
		pDC->FillSolidRect( dx, m_viewRect.top, wid, m_screenHeight, RGB(255,255,255) );
	}else{
		// 背景ビットマップの描画
		CRect rectViewClient;
		this->GetClientRect( &rectViewClient );
		rectViewClient.OffsetRect( 0 , m_drawStartTopOffset-m_offsetPixelY );
		int x = dx;
		int y = m_viewRect.top;
		int w = wid;
		int h = m_screenHeight;
		int offset = 0;
		if( IsScrollWithBk() ){
			offset = ( m_iItemHeight * GetTopIndex()  - m_offsetPixelY) % bmp.bmHeight;
		}
		util::DrawBitmap( pDC->GetSafeHdc(), hBgBitmap, x, y , w, h, sx, m_viewRect.top + offset );
	}
}

/**
 * DrawDetail()
 *  裏画面バッファにリスト項目を描画する
 */
int	CTouchListCtrl::DrawDetail( bool bForceDraw )
{
	MZ3_TRACE( L"DrawDetail(%s)\n", bForceDraw ? L"true" : L"false");
	if( m_memDC == NULL ){
		return 0;
	}

	// 念のため遅延描画タイマーを停止する
	MyResetRedrawTimer();

	// 背景を塗りつぶす
	BITMAP bmp;
	GetObject(m_memBMP->m_hObject, sizeof(BITMAP), &bmp);

	if (m_bBlackScrollMode && (m_bScrollDragging || m_bAutoScrolling)) {
		// ★ドラッグ中は背景を再描画しない
	} else {

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
			int offset = 0;
			if( IsScrollWithBk() ){
				offset = ( m_iItemHeight * GetTopIndex()  - m_offsetPixelY) % bmp.bmHeight;
			}
			util::DrawBitmap( m_memDC->GetSafeHdc(), hBgBitmap, x, y, w, h, 0, 0 + offset );
		}

		MZ3_TRACE( L" Top=%5d,offset=%5d\n" , GetTopIndex() , m_offsetPixelY );
	}

	// 透過モードに設定
	m_memDC->SetBkMode(TRANSPARENT);

	if (m_bBlackScrollMode && (m_bScrollDragging || m_bAutoScrolling)) {
		// ★ドラッグ中はアイテムを再描画しない
	} else {
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

			// バックバッファにアイテムを描画する
			DrawItemToBackSurface(nItem);
		}

		// フォントを元に戻す
		m_memDC->SelectObject( oldFont );
	}

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

/**
 * WM_VSCROLL メッセージハンドラ
 * (縦スクロールバーによるスクロール)
 *  ・ちらつきをなくすために裏画面バッファによる描画を行う
 */
void CTouchListCtrl::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	MZ3_TRACE( L"OnVScroll(%d)\n", nPos );

	// オフセットのズレを調整
	MyAdjustDrawOffset();
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
	// => なぜか背景色塗りつぶしになるので標準処理に任せる(ちらつくが仕方ない)
//	DrawDetail();
//	UpdateWindow();
#else
	// WMでは処理が追いつかないので標準処理に任せる
	if( !IsScrollWithBk() ){
		// WMで、かつ背景同時スクロールでない場合は遅延再描画
		MySetRedrawTimer( TIMER_INTERVAL_TOUCHLIST_SCROLLREDRAW_L );
	}
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
	if ( !theApp.m_pMouseGestureManager->IsGestureMode() ) {
		// オフセットのズレを調整
		MyAdjustDrawOffset();
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
 *  ・TIMERID_TOUCHLIST_AUTOSCROLL
 *    慣性スクロール処理
 *  ・TIMERID_TOUCHLIST_PANSCROLL:
 *    パンスクロール処理
 */
void CTouchListCtrl::OnTimer(UINT_PTR nIDEvent)
{
	MZ3_TRACE( L"**OnTimer(0x%04X)\n", nIDEvent );
	switch( nIDEvent ){
/****************************************************
*			 遅延描画処理
****************************************************/
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

/****************************************************
*			 慣性スクロール処理
****************************************************/
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

				MZ3_TRACE( L" *************AUTOSCROLL:dyAutoScroll=%d\n", dyAutoScroll );
				MZ3_TRACE( L" *************AUTOSCROLL:m_yAutoScrollMax=%d\n", m_yAutoScrollMax );

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
						// オフセットのズレを調整
						MyAdjustDrawOffset(MyAdjustDrawOffset_ADJUST_ONLY);
					} else {
						// 停止状態で最後の描画
#ifndef WINCE
						// Win32では独自処理で描画する
						// WMでは処理が追いつかないので標準処理に任せる
//						DrawDetail();
//						UpdateWindow();

//#else
//						if( !IsScrollWithBk() ){
//							// WMで、かつ背景同時スクロールでない場合は再描画
//							Invalidate();
//						}
#endif
					}

					// 慣性スクロールの停止
					MZ3_TRACE( L" *************AUTOSCROLL:KillTimer#1\n" );
					MyResetAutoScrollTimer();

					// ★強制再描画
					MZ3_TRACE( L" ★強制再描画#1\n" );
					DrawDetail(true);
					UpdateWindow();
				} else {
					// dyAutoScroll 分だけ移動する。
					CPoint lastPoint = m_autoScrollInfo.getLastPoint();

					int dy = lastPoint.y + dyAutoScroll;

					MZ3_TRACE( L" *************AUTOSCROLL:ScrollByMoveY(%d)\n" , dy );
					if( ScrollByMoveY( dy ) ){
						// 先頭か最後尾に達した
						// 慣性スクロールの停止
						MZ3_TRACE( L" *************AUTOSCROLL:KillTimer#2\n" );
						MyResetAutoScrollTimer();

						// ★強制再描画
						MZ3_TRACE( L" ★強制再描画#2\n" );
						DrawDetail(true);
						UpdateWindow();
					}
				}

				m_yAutoScrollMax = dyAutoScroll;
				MZ3_TRACE( L" *************AUTOSCROLL:m_yAutoScrollMax=%d\n", m_yAutoScrollMax );
#ifdef WINCE
				if( !IsScrollWithBk() ){
					// WMで、かつ背景同時スクロールでない場合は遅延再描画
					MySetRedrawTimer( TIMER_INTERVAL_TOUCHLIST_SCROLLREDRAW_L );
				}
#endif
				break;
			}
/****************************************************
*			 パンスクロール処理
****************************************************/
		case TIMERID_TOUCHLIST_PANSCROLL:
			{
				int dwDt = GetTickCount() - m_dwPanScrollLastTick;
				m_dwPanScrollLastTick = GetTickCount();

				if( m_dPxelX == 0 ) {
					// 移動量ゼロなら無限ループ防止のため中止
					MyResetPanScrollTimer();
				} else {
					// 移動処理
					m_offsetPixelX += dwDt * m_dPxelX / 10;

					MZ3_TRACE( L" m_offsetPixelX = %5d, dwDt = %5d\n"  , m_offsetPixelX , dwDt );

					// 終了判定
					if( m_dPxelX > 0 ){
						if( m_offsetPixelX > 0 ){
							m_offsetPixelX = 0;
						}
					} else {
						if( m_offsetPixelX < 0 ){
							m_offsetPixelX = 0;
						}
					}
					// 強制的に描画する
					CDC* pDC = GetDC();
					PanDrawToScreen(pDC);
					ReleaseDC(pDC);
					if( m_offsetPixelX == 0 ){
						// 一画面分移動した
						// パンスクロール終了
						MyResetPanScrollTimer();
						m_dPxelX = 0;
					}
				}
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

	// ★
	if (m_bBlackScrollMode) {
		// PIXEL単位でスクロールさせる
		m_offsetPixelY = (( dy - m_ptDragStart.y ) % m_iItemHeight );
	} else {
#ifndef WINCE
		// PIXEL単位でスクロールさせる
		m_offsetPixelY = (( dy - m_ptDragStart.y ) % m_iItemHeight );
#else
		// WMではItem単位でスクロールさせる
		m_offsetPixelY = 0;
#endif
	}

	// 前回の移動行数からの差分だけスクロールする
	//  Scroll()メソッドはピクセル数指定だが、ピクセル指定すると
	//  1行分の半分くらいで1行移動したりしてマウスの動きと同期が取れないので
	//  行数×高さで動かす
	if( abs(iScrollLine - m_iDragLine) > 0 ){
		CSize szScroll( 0 , -(( iScrollLine - m_iDragLine ) * m_iItemHeight) );

		MZ3_TRACE( L" ScrollByMoveY, Scroll(%5d)\n" , szScroll.cy );

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
	}

	if( ( iNextTop <= 0 ) ||
		( GetItemCount() - GetCountPerPage() <= iNextTop ) ){
		// 先頭or最後尾フラグ
		bLimitOver = true;
	}

	bool bDoRedraw = false;
	if( iTop != iNextTop ) {
		// Item表示位置が変わった
		// トータル移動行数を蓄積する
		m_iDragLine += iTop - iNextTop;

		MZ3_TRACE( L" ScrollByMoveY, DrawDetail() req1\n" );
		bDoRedraw = true;
	} else {
		// Item表示位置は変わらない
		if( iOffset != m_offsetPixelY ){
			// オフセットが変わった

			MZ3_TRACE( L" ScrollByMoveY, DrawDetail() req2\n" );
			bDoRedraw = true;
		}
	}

	if (bDoRedraw) {
		if (m_bBlackScrollMode) {
			// 再描画
			DrawDetail();
			UpdateWindow();
		} else {
#ifndef WINCE
			// 再描画
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
	// オフセットのズレを調整
	MyAdjustDrawOffset();
	// 慣性スクロールタイマーを停止
	MyResetAutoScrollTimer();
	// パンスクロールタイマーはここでは停止しない
	//MyResetPanScrollTimer();
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

/*
 * WM_RBUTTONDOWN メッセージハンドラ
 *  ・慣性スクロールしていれば停止する
*/
void CTouchListCtrl::OnRButtonDown(UINT nFlags, CPoint point)
{
	// オフセットのズレを調整
	MyAdjustDrawOffset();
	// 慣性スクロールの停止
	MyResetAutoScrollTimer();

	CListCtrl::OnRButtonDown(nFlags, point);
}

/*
 * MyAdjustDrawOffset()
 * m_offsetPixelYが0になるように調整する
 *  ・移動中なら進行方向に1個進めて止める
 *  ・移動中でなければ近い方に寄せて止める
*/
bool CTouchListCtrl::MyAdjustDrawOffset(bool bAdjustOnly)
{
//	MZ3_TRACE(L"MyAdjustDrawOffset()\n");
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
		MZ3_TRACE( L" *************AUTOSCROLL:m_iDragLine=%d,m_ptDragStart.y=%d,m_iItemHeight=%d\n" , m_iDragLine , m_ptDragStart.y , m_iItemHeight );
		MZ3_TRACE( L" *************AUTOSCROLL:ScrollByMoveY(%d)\n" , dy );
		if (!bAdjustOnly) {
			ScrollByMoveY( dy );
#ifdef WINCE
			if( !IsScrollWithBk() ){
				// WMで、かつ背景同時スクロールでない場合は再描画
				Invalidate();
			}
#endif
		}
	}

	return bMove;
}

/*
 * WM_LBUTTONDBLCLK メッセージハンドラ
 *  ・最初のクリック位置と二回目がずれたらダブルクリックを効かせない
 */
void CTouchListCtrl::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	// 選択変更
	int nItem = HitTest(point);
	if (nItem>=0) {
		int idx = util::MyGetListCtrlSelectedItemIndex( *this );
		if( GetItemCount() > 0 && idx >= 0 ) {
			util::MySetListCtrlItemFocusedAndSelected( *this, idx, false );
		}
		util::MySetListCtrlItemFocusedAndSelected( *this, nItem, true );
		if( idx != nItem ){
			return;
		}
	}

	// ダブルタップの暴発防止（できてるかどうか不明だが念のため
	if( m_bAutoScrolling ) {
		return;
	}

	CListCtrl::OnLButtonDblClk(nFlags, point);
}

/**
 * dx,dyのドラッグ量に応じて、ドラッグ開始かどうかを判定し、変数を設定する
 *
 * ドラッグ開始時は m_bPanDragging, m_bScrollDragging を設定する
 */
void CTouchListCtrl::MySetDragFlagWhenMovedPixelOverLimit(int dx, int dy)
{
	// 縦ドラッグ開始判断用オフセット値
#ifndef WINCE
	// win32の場合半行以内の移動はドラッグとみなさない
	int dyMinLimit = theApp.m_optionMng.GetFontHeight() / 2 + 1 ;
#else
	// WMの場合一行以内の移動はドラッグとみなさない
	int dyMinLimit = theApp.m_optionMng.GetFontHeight();
#endif

	if (m_bPanDragging) {
		// 横ドラッグ中
		if( abs( dx ) < m_screenWidth / 3 ){
			// マウスを元に戻したら横ドラッグをキャンセル
			m_bPanDragging = false ;
		}
	} else if (m_bScrollDragging) {
		// 縦ドラッグ中

	} else {
		// ドラッグ方向が確定していない
		if( m_bCanSlide &&
			m_bUseHorizontalDragMove &&
			( abs(dx) > abs(dy) && abs(dx) > m_screenWidth / 3 ) ) {
				// 横方向の移動量が大きくて移動量が画面の1/3以上の場合]
				// ドラッグ方向設定
				if( dx > 0 ){
					m_drPanScrollDirection = PAN_SCROLL_DIRECTION_RIGHT;
				} else {
					m_drPanScrollDirection = PAN_SCROLL_DIRECTION_LEFT;
				}
				// 横ドラッグ開始
				m_bPanDragging = true;
		} else if( abs(dx) < abs(dy) && abs(dy) > dyMinLimit ) {
			// 縦方向の移動量が大きくて移動量がドラッグ開始オフセット以上の場合
			if( GetItemCount()-GetCountPerPage() > 0 ) {
				// 縦スクロール可能ならば
				// 縦ドラッグ開始
				m_bScrollDragging = true;
			}
		}
	}
}

/**
 * パンスクロール開始
 *
 * スクロール方向（direction）に従いオフセット値と差分を設定してタイマーを起動する
 */
void CTouchListCtrl::StartPanScroll(PAN_SCROLL_DIRECTION direction)
{
#ifndef WINCE
#define PANSCROLL_DIVIDE 18
#else
#define PANSCROLL_DIVIDE 15
#endif
	if ( !m_bUsePanScrollAnimation || !m_bCanPanScroll ) {
		// オプションでオフになっているのでアニメーションしない
		Invalidate();
		return;
	}

	if( m_memDC == NULL ){
		return;
	}
	// パンスクロール用に直前の表示状態のコピーを取る（一画面分）
	CDC* pDC = GetDC();
	m_memDC->BitBlt( m_viewRect.left , m_viewRect.top + m_drawStartTopOffset , m_screenWidth , m_screenHeight , pDC , m_viewRect.left , m_viewRect.top , SRCCOPY );

	MyResetPanScrollTimer();

	switch (direction) {
	case PAN_SCROLL_DIRECTION_RIGHT:
		{
			// 右方向へスクロール
			HBITMAP hBmp;
			CBitmap cBmp;
			int rc = cBmp.LoadBitmap( IDB_SLIDE_RIGHT_BITMAP );
			hBmp = (HBITMAP) cBmp;
			BITMAP	bmp;
			GetObject( hBmp , sizeof(bmp), &bmp );
			CDC cTempDC;
			cTempDC.CreateCompatibleDC( m_memDC );
			HBITMAP oldBmp = (HBITMAP)SelectObject( cTempDC.m_hDC , hBmp );
			m_memDC->BitBlt( 0 , m_drawStartTopOffset + ( m_screenHeight - bmp.bmHeight ) / 2 , bmp.bmWidth , bmp.bmHeight , &cTempDC , 0 , 0 , SRCCOPY );
			SelectObject( cTempDC.m_hDC , oldBmp );
			
			// 左へ一画面ずれたところから開始
			m_offsetPixelX = - m_screenWidth;
			// 移動差分
			m_dPxelX = m_screenWidth / PANSCROLL_DIVIDE + 1 ;
			break;
		}
	case PAN_SCROLL_DIRECTION_LEFT:
		{
			// 左方向へスクロール
			HBITMAP hBmp;
			CBitmap cBmp;
			int rc = cBmp.LoadBitmap( IDB_SLIDE_LEFT_BITMAP );
			hBmp = (HBITMAP) cBmp;
			BITMAP	bmp;
			GetObject( hBmp , sizeof(bmp), &bmp );
			CDC cTempDC;
			cTempDC.CreateCompatibleDC( m_memDC );
			HBITMAP oldBmp = (HBITMAP)SelectObject( cTempDC.m_hDC , hBmp );
			m_memDC->BitBlt( m_screenWidth - bmp.bmWidth , m_drawStartTopOffset + ( m_screenHeight - bmp.bmHeight ) / 2 , bmp.bmWidth , bmp.bmHeight , &cTempDC , 0 , 0 , SRCCOPY );
			SelectObject( cTempDC.m_hDC , oldBmp );

			// 右へ一画面ずれたところから開始
			m_offsetPixelX = m_screenWidth;
			// 移動差分
			m_dPxelX = - m_screenWidth / PANSCROLL_DIVIDE + 1 ;
			break;
		}
	}

	// パンスクロール方向設定
	m_drPanScrollDirection = direction;

	// パンスクロール開始時刻
	m_dwPanScrollLastTick = GetTickCount();

	// パンスクロール開始
	ResetEvent( m_hPanScrollEvent );
	MZ3_TRACE( L" ResetEvent(0X%08X)\n" , m_hPanScrollEvent );
	MySetPanScrollTimer( TIMER_INTERVAL_TOUCHLIST_PANSCROLL );
}

/**
 * DrawItemFocusRect()
 *
 * 指定されたアイテムにフォーカス矩形を表示する
 * ・XORで描画するので2回呼べば矩形を消す
 *   ここは描画か消去か意識できないので呼び出し側で管理すること
 * ・アイテムがスクロールされたら消される
 *
 */
void CTouchListCtrl::DrawItemFocusRect( const int nItem )
{
	CRect rctItem;
	GetItemRect( nItem , rctItem , LVIR_BOUNDS);
	CDC* pdc = GetDC();
	pdc->DrawFocusRect( rctItem );
	ReleaseDC(pdc);
}

/**
 * パンスクロール終了待ち
 *
 * 横スクロールが終了するのを待つ
 * dwMilliseconds：(I)：タイムアウト時間(ms)
 * 戻り値：	TRUE：パンスクロールが終了した
 *			FALSE：タイムアウトかエラー
 */
BOOL CTouchListCtrl::WaitForPanScroll( DWORD dwMilliseconds )
{
	BOOL bRtn = TRUE;
	if( m_bCanPanScroll && m_bUsePanScrollAnimation ){
		DWORD dwRtn = WaitForSingleObject( m_hPanScrollEvent , dwMilliseconds );
		MZ3_TRACE( L"WaitForSingleObject(0X%08X):%d\n" , m_hPanScrollEvent , dwRtn );
		if( dwRtn != WAIT_OBJECT_0 ) {
			bRtn = FALSE;
		}
	}
	return bRtn;
}

void CTouchListCtrl::DrawItemToBackSurface(int nItem)
{
	// 背景は描画済みなので描画しない
	// （DrawItem()で背景描画されると、ズレが生じるので）
	SetDrawBk( false );

	// DRAWITEMSTRUCTをでっちあげて派生クラスのDrawItem()をだます
	DRAWITEMSTRUCT dis;
	CRect rctItem;
	GetItemRect( nItem , &rctItem , LVIR_BOUNDS );

//	MZ3_TRACE(L" DrawItemToBackSurface, %d, %d, %d, %d, %d\n",
//		rctItem.left, rctItem.top, rctItem.right, rctItem.bottom, m_drawStartTopOffset);

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

	// 本物のアイテム描画を使用することで実際の画面と描画内容を一致させる
	DrawItem( &dis );

	SetDrawBk( true );
}

void CTouchListCtrl::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct)
{
//	MZ3_TRACE(L"CTouchListCtrl::OnDrawItem, %d, %d\n",
//		nIDCtl, lpDrawItemStruct->itemID);

	CListCtrl::OnDrawItem(nIDCtl, lpDrawItemStruct);
}
