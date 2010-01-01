/*
 * Copyright (c) MZ3 Project Team All rights reserved.
 * Code licensed under the BSD License:
 * http://www.mz3.jp/license.txt
 */
// WriteEdit.cpp : �����t�@�C��
//

#include "stdafx.h"
#ifdef BT_MZ3

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



// CWriteEdit ���b�Z�[�W �n���h��


void CWriteEdit::OnRButtonUp(UINT nFlags, CPoint point)
{
//	CEdit::OnRButtonUp(nFlags, point);
	// WriteView �̓Ǝ����j���[��\������
	CWriteView* pWriteView = (CWriteView*) GetOwner();
	pWriteView->PopupWriteBodyMenu();
}

void CWriteEdit::OnLButtonDown(UINT nFlags, CPoint point)
{
#ifdef WINCE
	// �^�b�v�������Ń\�t�g�L�[���j���[�\��
	SHRGINFO RGesture;
	RGesture.cbSize     = sizeof(SHRGINFO);
	RGesture.hwndClient = m_hWnd;
	RGesture.ptDown     = point;
	RGesture.dwFlags    = SHRG_RETURNCMD;
	if (::SHRecognizeGesture(&RGesture) == GN_CONTEXTMENU) {
		// WriteView �̓Ǝ����j���[��\������
		CWriteView* pWriteView = (CWriteView*) GetOwner();
		pWriteView->PopupWriteBodyMenu();
		return;
	}
#endif

	CEdit::OnLButtonDown(nFlags, point);
}

#endif