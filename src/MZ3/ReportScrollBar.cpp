/*
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */
// ReportScrollBar.cpp : 実装ファイル
//

#include "stdafx.h"
#include "MZ3.h"
#include "ReportScrollBar.h"


// ReportScrollBar

IMPLEMENT_DYNAMIC(ReportScrollBar, CScrollBar)

ReportScrollBar::ReportScrollBar()
{

}

ReportScrollBar::~ReportScrollBar()
{
}


BEGIN_MESSAGE_MAP(ReportScrollBar, CScrollBar)
	ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()



// ReportScrollBar メッセージ ハンドラ



void ReportScrollBar::OnLButtonDown(UINT nFlags, CPoint point)
{
	// Tap&Holdマーカーの表示をキャンセルする
	Default();

//	CScrollBar::OnLButtonDown(nFlags, point);
}
