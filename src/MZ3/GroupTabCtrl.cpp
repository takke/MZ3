// GroupTabCtrl.cpp : �����t�@�C��
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
		ClientToScreen(&point);
		theApp.m_pMainView->PopupTabMenu(point, TPM_LEFTALIGN | TPM_TOPALIGN);
		return;
	}
#else
	// �h���b�O�Ń^�u�̈ړ�(�����ύX)
	m_bDragging = true;

	// �L���v�`���J�n
	SetCapture();
#endif

	CTabCtrl::OnLButtonDown(nFlags, point);
}

void CGroupTabCtrl::OnMouseMove(UINT nFlags, CPoint point)
{
#ifndef WINCE
	if (m_bDragging) {
		// �}�E�X���W���E�܂��͍��̗v�f�𒴂�����ړ�
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
#endif

	CTabCtrl::OnMouseMove(nFlags, point);
}

void CGroupTabCtrl::OnLButtonUp(UINT nFlags, CPoint point)
{
#ifndef WINCE
	if (m_bDragging) {
		// �L���v�`���I��
		ReleaseCapture();

		// �t���O�N���A
		m_bDragging = false;
	}
#endif

	CTabCtrl::OnLButtonUp(nFlags, point);
}
