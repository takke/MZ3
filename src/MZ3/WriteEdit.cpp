/*
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */
// WriteEdit.cpp : 実装ファイル
//

#include "stdafx.h"
#include "MZ3.h"
#include "WriteEdit.h"
#include "WriteView.h"


// CWriteEdit

IMPLEMENT_DYNAMIC(CWriteEdit, CEdit)

CWriteEdit::CWriteEdit()
{

}

CWriteEdit::~CWriteEdit()
{
}


BEGIN_MESSAGE_MAP(CWriteEdit, CEdit)
	ON_WM_RBUTTONUP()
	ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()



// CWriteEdit メッセージ ハンドラ


void CWriteEdit::OnRButtonUp(UINT nFlags, CPoint point)
{
//	CEdit::OnRButtonUp(nFlags, point);
	// WriteView の独自メニューを表示する
	CWriteView* pWriteView = (CWriteView*) GetOwner();
	pWriteView->PopupWriteBodyMenu();
}

void CWriteEdit::OnLButtonDown(UINT nFlags, CPoint point)
{
#ifdef WINCE
	// タップ長押しでソフトキーメニュー表示
	SHRGINFO RGesture;
	RGesture.cbSize     = sizeof(SHRGINFO);
	RGesture.hwndClient = m_hWnd;
	RGesture.ptDown     = point;
	RGesture.dwFlags    = SHRG_RETURNCMD;
	if (::SHRecognizeGesture(&RGesture) == GN_CONTEXTMENU) {
		// WriteView の独自メニューを表示する
		CWriteView* pWriteView = (CWriteView*) GetOwner();
		pWriteView->PopupWriteBodyMenu();
		return;
	}
#endif

	CEdit::OnLButtonDown(nFlags, point);
}
