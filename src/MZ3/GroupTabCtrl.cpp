// GroupTabCtrl.cpp : �����t�@�C��
//

#include "stdafx.h"
#include "MZ3.h"
#include "GroupTabCtrl.h"
#include "MZ3View.h"


// CGroupTabCtrl

IMPLEMENT_DYNAMIC(CGroupTabCtrl, CTabCtrl)

CGroupTabCtrl::CGroupTabCtrl()
{

}

CGroupTabCtrl::~CGroupTabCtrl()
{
}


BEGIN_MESSAGE_MAP(CGroupTabCtrl, CTabCtrl)
	ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()



// CGroupTabCtrl ���b�Z�[�W �n���h��

void CGroupTabCtrl::OnLButtonDown(UINT nFlags, CPoint point)
{
#ifdef WINCE
	// �^�b�v�������Ń��j���[�\��
	SHRGINFO RGesture;
	RGesture.cbSize     = sizeof(SHRGINFO);
	RGesture.hwndClient = m_hWnd;
	RGesture.ptDown     = point;
	RGesture.dwFlags    = SHRG_RETURNCMD;
	if (::SHRecognizeGesture(&RGesture) == GN_CONTEXTMENU) {
		theApp.m_pMainView->PopupTabMenu(point, TPM_LEFTALIGN | TPM_TOPALIGN);
		return;
	}
#endif

	CTabCtrl::OnLButtonDown(nFlags, point);
}
