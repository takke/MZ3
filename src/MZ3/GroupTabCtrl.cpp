/*
 * Copyright (c) MZ3 Project Team All rights reserved.
 * Code licensed under the BSD License:
 * http://www.mz3.jp/license.txt
 */
// GroupTabCtrl.cpp : 実装ファイル
//

#include "stdafx.h"
#include "MZ3.h"
#include "GroupTabCtrl.h"
#include "MZ3View.h"


// CGroupTabCtrl

IMPLEMENT_DYNAMIC(CGroupTabCtrl, CTabCtrl)

CGroupTabCtrl::CGroupTabCtrl()
#ifndef WINCE
	: m_bDragging(false)
#endif
{

}

CGroupTabCtrl::~CGroupTabCtrl()
{
}


BEGIN_MESSAGE_MAP(CGroupTabCtrl, CTabCtrl)
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
END_MESSAGE_MAP()



// CGroupTabCtrl メッセージ ハンドラ

void CGroupTabCtrl::OnLButtonDown(UINT nFlags, CPoint point)
{
#ifdef WINCE
	// タップ長押しでメニュー表示
	SHRGINFO RGesture;
	RGesture.cbSize     = sizeof(SHRGINFO);
	RGesture.hwndClient = m_hWnd;
	RGesture.ptDown     = point;
	RGesture.dwFlags    = SHRG_RETURNCMD;
	if (::SHRecognizeGesture(&RGesture) == GN_CONTEXTMENU) {
		ClientToScreen(&point);
		theApp.m_pMainView->PopupTabMenu(point, TPM_LEFTALIGN | TPM_TOPALIGN);
		return;
	}
#else
	// ドラッグでタブの移動(順序変更)
	m_bDragging = true;
	m_ptDragStart = point;

	// キャプチャ開始
	SetCapture();
#endif

	CTabCtrl::OnLButtonDown(nFlags, point);
}

void CGroupTabCtrl::OnMouseMove(UINT nFlags, CPoint point)
{
#ifndef WINCE
	if (m_bDragging) {
		// マウスカーソル変更
		SetCursor(LoadCursor(NULL, IDC_SIZEWE));

		// マウス座標が最初の位置より少しずれていて、かつ、右または左の要素を超えたら移動
		if (abs((point-m_ptDragStart).cx)>10) {
			int idx = GetCurSel();
			if (idx+1<GetItemCount()) {
				CRect r;
				GetItemRect(idx+1, r);
				CPoint ptCenter = r.CenterPoint();
				if (point.x > ptCenter.x) {
					theApp.m_pMainView->MoveTabItem(idx, idx+1);
				}
			}
			if (idx-1>=0) {
				CRect r;
				GetItemRect(idx-1, r);
				CPoint ptCenter = r.CenterPoint();
				if (point.x < ptCenter.x) {
					theApp.m_pMainView->MoveTabItem(idx, idx-1);
				}
			}
		}
	}
#endif

	CTabCtrl::OnMouseMove(nFlags, point);
}

void CGroupTabCtrl::OnLButtonUp(UINT nFlags, CPoint point)
{
#ifndef WINCE
	if (m_bDragging) {
		// キャプチャ終了
		ReleaseCapture();

		// マウスカーソルを元に戻す
		SetCursor(LoadCursor(NULL, IDC_ARROW));

		// フラグクリア
		m_bDragging = false;
	}
#endif

	CTabCtrl::OnLButtonUp(nFlags, point);
}
