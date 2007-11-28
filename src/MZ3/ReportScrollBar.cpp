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
