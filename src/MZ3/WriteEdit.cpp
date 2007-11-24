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
END_MESSAGE_MAP()



// CWriteEdit メッセージ ハンドラ


void CWriteEdit::OnRButtonUp(UINT nFlags, CPoint point)
{
//	CEdit::OnRButtonUp(nFlags, point);
	// WriteView の独自メニューを表示する
	CWriteView* pWriteView = (CWriteView*) GetOwner();
	pWriteView->PopupWriteBodyMenu();
}
