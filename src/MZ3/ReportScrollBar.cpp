/*
 * Copyright (c) MZ3 Project Team All rights reserved.
 * Code licensed under the BSD License:
 * http://www.mz3.jp/license.txt
 */
// ReportScrollBar.cpp : �����t�@�C��
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



// ReportScrollBar ���b�Z�[�W �n���h��



void ReportScrollBar::OnLButtonDown(UINT nFlags, CPoint point)
{
	// Tap&Hold�}�[�J�[�̕\�����L�����Z������
	Default();

//	CScrollBar::OnLButtonDown(nFlags, point);
}
