/*
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */
// CategoryListCtrl.cpp : �����t�@�C��
//

#include "stdafx.h"
#include "MZ3.h"
#include "MZ3View.h"
#include "CategoryListCtrl.h"
#include "util.h"
#include "util_gui.h"

static const int OFFSET_FIRST = 2;
static const int OFFSET_OTHER = 6;

// CCategoryListCtrl

IMPLEMENT_DYNAMIC(CCategoryListCtrl, CListCtrl)

CCategoryListCtrl::CCategoryListCtrl()
	: m_bStopDraw(false)
{
	// �F�̃f�t�H���g�l��ݒ�
	m_activeItem	= 0;
}

CCategoryListCtrl::~CCategoryListCtrl()
{
}


BEGIN_MESSAGE_MAP(CCategoryListCtrl, CListCtrl)
	ON_WM_ERASEBKGND()
	ON_WM_VSCROLL()
	ON_NOTIFY_REFLECT_EX(LVN_ITEMCHANGED, &CCategoryListCtrl::OnLvnItemchanged)
	ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()


// CCategoryListCtrl ���b�Z�[�W �n���h��

void CCategoryListCtrl::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	if( m_bStopDraw )
		return;
	CDC* pDC = CDC::FromHandle(lpDrawItemStruct->hDC);

	// �ĕ`�悷��Item�̍��W���擾
	CRect rcItem(lpDrawItemStruct->rcItem);

	// �w�i�𓧖��ɂ���t���O�i�C���[�W�f�[�^�p�j
//	UINT uiFlags = ILD_TRANSPARENT;

	// �C���[�W�I�u�W�F�N�g�i�[�A�h���X
//	CImageList* pImageList;

	// �A�C�e����ID
	int nItem = lpDrawItemStruct->itemID;

	// ���݃R���g���[���Ƀt�H�[�J�X�����邩�ۂ���
	// �t���O��ݒ�
	BOOL bFocus = FALSE;
	if (GetFocus() == this) {
		bFocus = TRUE;
	}

	// �A�C�e�� �f�[�^���擾���܂��B
	LV_ITEM lvi;

	lvi.mask = LVIF_IMAGE | LVIF_STATE | LVIF_PARAM;
	lvi.iItem = nItem;
	lvi.iSubItem = 0;
	lvi.stateMask = LVIS_SELECTED | LVIS_STATEIMAGEMASK;
	this->GetItem(&lvi);

	CString strText = this->GetItemText( nItem, 0 );

	// �A�C�e�����I����Ԃ��ۂ��̃t���O��ݒ�
	BOOL bSelected = (bFocus || (GetStyle() & LVS_SHOWSELALWAYS))
		&& lvi.state & LVIS_SELECTED;
	bSelected =	bSelected || (lvi.state & LVIS_DROPHILITED);

	// �A�C�e���̕\������Ă��镝���擾
	CRect rcAllLabels;
	this->GetItemRect(nItem, rcAllLabels, LVIR_BOUNDS);

	// �A�C�e���̃��x���̕����擾
	CRect rcLabel;
	this->GetItemRect(nItem, rcLabel, LVIR_LABEL);

	// ���̈ʒu�𓯂��ɂ���
	rcAllLabels.left = rcLabel.left;

	// �I������Ă���ꍇ�́A��s��h��Ԃ�
	if (bSelected == TRUE) {
		pDC->FillRect(rcAllLabels, &CBrush(::GetSysColor(COLOR_HIGHLIGHT)));
	}else{
		// �w�i�̓h��Ԃ�
		if( !theApp.m_optionMng.IsUseBgImage() || !theApp.m_bgImageMainCategoryCtrl.isEnableImage() ) {
			// �w�i�摜�Ȃ��̏ꍇ
			pDC->FillRect(rcAllLabels, &CBrush(RGB(0xFF, 0xFF, 0xFF)));
		}else{
			// �r�b�g�}�b�v�̕`��
			CRect rectClient;
			this->GetClientRect( &rectClient );

			int x = lpDrawItemStruct->rcItem.left;
			int y = lpDrawItemStruct->rcItem.top;
			int w = rectClient.Width();
			int h = lpDrawItemStruct->rcItem.bottom - y;
			util::DrawBitmap( pDC->GetSafeHdc(), theApp.m_bgImageMainCategoryCtrl.getHandle(), x, y, w, h, x, y );
		}
	}

	// �A�C�e���̃��x����`���܂��B
	this->GetItemRect(nItem, rcItem, LVIR_LABEL);

	rcLabel = rcItem;
	rcLabel.left += OFFSET_FIRST;
	rcLabel.right -= OFFSET_FIRST;

	// �����F�̕ύX
	COLORREF clrTextSave = (COLORREF)-1;
	COLORREF clrBkSave   = (COLORREF)-1;
	if (bSelected) {
		// �I����ԂȂ̂ŁA�V�X�e���W���̑I��F�œh��Ԃ�
		clrTextSave = pDC->SetTextColor(::GetSysColor(COLOR_HIGHLIGHTTEXT));
		clrBkSave = pDC->SetBkColor(::GetSysColor(COLOR_HIGHLIGHT));
	}else{
		// ��I����ԂȂ̂ŁA�A�N�e�B�u�Ȃ�ԁA�����łȂ���΍��ŕ`��
		COLORREF clrTextFg;
		if (lvi.iItem == GetActiveItem()) {
			clrTextFg = theApp.m_skininfo.clrMainCategoryListActiveText;
		} else {
			clrTextFg = theApp.m_skininfo.clrMainCategoryListInactiveText;
		}

		// �F�Â�����
		clrTextSave = pDC->SetTextColor(clrTextFg);
	}

	pDC->DrawText(strText,
		rcLabel,
		DT_LEFT | DT_SINGLELINE | DT_NOPREFIX | DT_NOCLIP | DT_VCENTER| DT_END_ELLIPSIS);

	// �J�����p�̃��x����`��
	LV_COLUMN lvc;
	lvc.mask = LVCF_FMT | LVCF_WIDTH;

	// ��2�J�����ȍ~�̕`��
	for (int nColumn = 1; this->GetColumn(nColumn, &lvc); nColumn++) {
		rcItem.left = rcItem.right;
		rcItem.right += lvc.cx;

		strText = this->GetItemText(nItem, nColumn);
		if (strText.GetLength() == 0) {
			continue;
		}

		UINT nJustify = DT_LEFT;

		switch(lvc.fmt & LVCFMT_JUSTIFYMASK) {
		case LVCFMT_RIGHT:
			nJustify = DT_RIGHT;
			break;
		case LVCFMT_CENTER:
			nJustify = DT_CENTER;
			break;
		default:
			break;
		}

		rcLabel = rcItem;
		rcLabel.left += OFFSET_OTHER;
		rcLabel.right -= OFFSET_OTHER;

		pDC->DrawText(strText,
			rcLabel,
			nJustify | DT_SINGLELINE | DT_NOPREFIX | DT_NOCLIP | DT_VCENTER | DT_END_ELLIPSIS);
	}

	// �F��߂�
	if (clrTextSave != (COLORREF)-1) {
		clrTextSave = pDC->SetTextColor(clrTextSave);
	}
	if (clrBkSave != (COLORREF)-1) {
		clrBkSave = pDC->SetBkColor(clrBkSave);
	}

	// �A�C�e�����t�H�[�J�X�������Ă���Ƃ���
	// ���̑I��g��`�悷��
	if ((lvi.state & LVIS_FOCUSED) == LVIS_FOCUSED && bFocus == TRUE) {
		pDC->DrawFocusRect(rcAllLabels);
	}
}

BOOL CCategoryListCtrl::OnEraseBkgnd(CDC* pDC)
{
	if( m_bStopDraw )
		return TRUE;

	pDC->SetBkMode( TRANSPARENT );

	// �r�b�g�}�b�v�̏������ƕ`��
	if( theApp.m_optionMng.IsUseBgImage() ) {
		CRect rectClient;
		this->GetClientRect( &rectClient );

		theApp.m_bgImageMainCategoryCtrl.load();

		if (theApp.m_bgImageMainCategoryCtrl.isEnableImage()) {
			int x = rectClient.left;
			int y = rectClient.top;
			int w = rectClient.Width();
			int h = rectClient.Height();
			util::DrawBitmap( pDC->GetSafeHdc(), theApp.m_bgImageMainCategoryCtrl.getHandle(), x, y, w, h, x, y );
			return TRUE;
		}
	}

	return CListCtrl::OnEraseBkgnd(pDC);
}

void CCategoryListCtrl::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// �X�N���[�����̔w�i�摜�̂���h�~
	if (theApp.m_optionMng.IsUseBgImage()) {
		static int s_nLastPos = nPos;
		if( s_nLastPos != nPos ) {
			Invalidate( FALSE );
			s_nLastPos = nPos;
		}
	}

	CListCtrl::OnVScroll(nSBCode, nPos, pScrollBar);
}

BOOL CCategoryListCtrl::OnLvnItemchanged(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);

	// �ŏ����Ō�̍��ڂł���ΑS�̂��ĕ`��i�w�i�摜�̂���h�~�j
	int iStart = GetTopIndex();
	int iEnd   = iStart + GetCountPerPage();
	if( pNMLV->iItem < iStart || pNMLV->iItem == iEnd ) {
		if( pNMLV->uNewState & LVIS_FOCUSED ) {
			Invalidate( FALSE );
		}
	}

	*pResult = 0;

	// FALSE ��Ԃ����ƂŁA�e�E�B���h�E�ɂ��C�x���g��n��
	return FALSE;
}


void CCategoryListCtrl::OnLButtonDown(UINT nFlags, CPoint point)
{
#ifdef WINCE
	// �^�b�v�������Ń\�t�g�L�[���j���[�\��
	SHRGINFO RGesture;
	RGesture.cbSize     = sizeof(SHRGINFO);
	RGesture.hwndClient = m_hWnd;
	RGesture.ptDown     = point;
	RGesture.dwFlags    = SHRG_RETURNCMD;
	if (::SHRecognizeGesture(&RGesture) == GN_CONTEXTMENU) {
		// TODO �{���� WM_COMMAND �Œʒm���ׂ��B
		ClientToScreen(&point);
		theApp.m_pMainView->PopupCategoryMenu(point, TPM_LEFTALIGN | TPM_TOPALIGN);
		return;
	}
#endif

	CListCtrl::OnLButtonDown(nFlags, point);
}
