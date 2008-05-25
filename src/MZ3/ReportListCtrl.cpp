/*
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */
// ReportListCtrl.cpp : �����t�@�C��
//

#include "stdafx.h"
#include "MZ3.h"
#include "ReportListCtrl.h"
#include "ReportView.h"
#include "util.h"
#include "util_gui.h"

static const int OFFSET_FIRST = 2;
static const int OFFSET_OTHER = 6;

// CReportListCtrl

IMPLEMENT_DYNAMIC(CReportListCtrl, CTouchListCtrl)

CReportListCtrl::CReportListCtrl()
{
}

CReportListCtrl::~CReportListCtrl()
{
}


BEGIN_MESSAGE_MAP(CReportListCtrl, CTouchListCtrl)
	ON_WM_ERASEBKGND()
	ON_NOTIFY_REFLECT_EX(LVN_ITEMCHANGED, &CReportListCtrl::OnLvnItemchanged)
	ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()



// CReportListCtrl ���b�Z�[�W �n���h��

void CReportListCtrl::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	CDC* pDC = CDC::FromHandle(lpDrawItemStruct->hDC);

	CRect rectClient;
	this->GetClientRect( &rectClient );

	if( IsDrawBk() ) {
		// �r�b�g�}�b�v�̕`��
		if( theApp.m_optionMng.IsUseBgImage() && theApp.m_bgImageReportListCtrl.isEnableImage() ) {
			int x = lpDrawItemStruct->rcItem.left;
			int y = lpDrawItemStruct->rcItem.top;
			int w = rectClient.Width();
			int h = lpDrawItemStruct->rcItem.bottom - y;
			int offset = 0;
			if( IsScrollWithBk() ){
				offset = ( h * GetTopIndex() ) % theApp.m_bgImageReportListCtrl.getBitmapSize().cy;
			}
			util::DrawBitmap( pDC->GetSafeHdc(), theApp.m_bgImageReportListCtrl.getHandle(), x, y, w, h, x, y + offset );
		}
	}

	// �ĕ`�悷��Item�̍��W���擾
	CRect rcItem(lpDrawItemStruct->rcItem);

	// �A�C�e����ID
	int nItem = lpDrawItemStruct->itemID;

	// ���݃R���g���[���Ƀt�H�[�J�X�����邩�ۂ���
	// �t���O��ݒ�
	BOOL bFocus = FALSE;
	if (GetFocus() == this) {
		bFocus = TRUE;
	}

	TCHAR szBuff[MAX_PATH];
	LPCTSTR pszText;

	// �A�C�e�� �f�[�^���擾���܂��B
	LV_ITEM lvi;

	lvi.mask = LVIF_TEXT | LVIF_IMAGE | LVIF_STATE | LVIF_PARAM;
	lvi.iItem = nItem;
	lvi.iSubItem = 0;
	lvi.pszText = szBuff;
	lvi.cchTextMax = sizeof(szBuff);
	lvi.stateMask = LVIS_SELECTED | LVIS_STATEIMAGEMASK;
	this->GetItem(&lvi);

	// �A�C�e�����I����Ԃ��ۂ��̃t���O��ݒ�
	BOOL bSelected =
		(bFocus || (GetStyle() & LVS_SHOWSELALWAYS))
		&& lvi.state & LVIS_SELECTED;
	bSelected = bSelected || (lvi.state & LVIS_DROPHILITED);

	// �A�C�e���̕\������Ă��镝���擾
	CRect rcAllLabels;
	this->GetItemRect(nItem, rcAllLabels, LVIR_BOUNDS);
	// �\���ʒu��rcItem�ɍ��킹��
	rcAllLabels.MoveToY( rcItem.top );

	// �A�C�e���̃��x���̕����擾
	CRect rcLabel;
	this->GetItemRect(nItem, rcLabel, LVIR_LABEL);
	// �\���ʒu��rcItem�ɍ��킹��
	rcLabel.MoveToY( rcItem.top );

	// ���̈ʒu�𓯂��ɂ���
	rcAllLabels.left = rcLabel.left;

	// �����̉������擾
	CSize szSize = pDC->GetTextExtent(szBuff, (int)wcslen(szBuff));
	// �h��Ԃ��͈͂𕶎������ɂ���
	CRect rcItemStr;
	rcItemStr = rcLabel;
	rcItemStr.right =	rcItemStr.left + szSize.cx + OFFSET_OTHER;

	// �w�i�̓h��Ԃ��i�w�i�摜�Ȃ��̏ꍇ�j
	if( !theApp.m_optionMng.IsUseBgImage() || !theApp.m_bgImageReportListCtrl.isEnableImage() ) {
		pDC->FillRect(rcAllLabels, &CBrush(RGB(0xFF, 0xFF, 0xFF)));
	}

	// �I������Ă���ꍇ�́A
	// �I������Ă��镶���݈̂�s��h��Ԃ�
	if (bSelected == TRUE) {
		pDC->FillRect(rcAllLabels, &CBrush(::GetSysColor(COLOR_HIGHLIGHT)));
	}

	// �F��ݒ肵�ăA�C�R�����}�X�N���܂��B
	UINT uiFlags = ILD_TRANSPARENT;						// �w�i�𓧖��ɂ���t���O�i�C���[�W�f�[�^�p�j
	COLORREF clrMaskFg = ::GetSysColor(COLOR_WINDOW);	// �`�悷��F
	COLORREF clrMaskBk = ::GetSysColor(COLOR_WINDOW);	// �`�悷��F
	if ((lvi.state & LVIS_CUT) == LVIS_CUT) {
		uiFlags |= ILD_BLEND50;
	} else {
		if (bSelected == TRUE) {
			clrMaskFg = ::GetSysColor(COLOR_HIGHLIGHT);
			uiFlags |= ILD_BLEND50;
		}
	}

	// ��ԃA�C�R����`�悵�܂��B
	UINT nStateImageMask = lvi.state & LVIS_STATEIMAGEMASK;
	if ((nStateImageMask>>12) > 0) {

		int nImage = (nStateImageMask>>12) - 1;

		CImageList* pImageList = this->GetImageList(LVSIL_STATE);
		if (pImageList) {
			pImageList->Draw(pDC, nImage,
				CPoint(rcItem.left, rcItem.top),
				ILD_TRANSPARENT);
		}
	}

	// �ʏ�̃A�C�R���ƃI�[�o�[���C�A�C�R����`�悵�܂��B
	CRect rcIcon;
	this->GetItemRect(nItem, rcIcon, LVIR_ICON);
	// �\���ʒu��rcItem�ɍ��킹��
	rcIcon.MoveToY( rcItem.top );

	CImageList* pImageList = this->GetImageList(LVSIL_SMALL);

	if (pImageList != NULL) {
		UINT nOvlImageMask = lvi.state & LVIS_OVERLAYMASK;
		if (rcItem.left < rcItem.right - 1) {
			if (lvi.iImage != 0) {
				ImageList_DrawEx(
					pImageList->m_hImageList, lvi.iImage,
					pDC->m_hDC,
					rcIcon.left, rcIcon.top, 16, 16,
					clrMaskBk, clrMaskFg,
					uiFlags | nOvlImageMask);
			}
		}
	}

	// �A�C�e���̃��x����`���܂��B
	CRect rcSubItem;
	this->GetItemRect(nItem, rcSubItem, LVIR_LABEL);
	// �\���ʒu��rcItem�ɍ��킹��
	rcSubItem.MoveToY( rcItem.top );

	//--- �����J����

	pszText = szBuff;

	//rcLabel = rcItem;
	rcLabel.left  += OFFSET_FIRST;
	rcLabel.right -= OFFSET_FIRST;

	// �����F�̕ύX
	COLORREF clrTextSave = (COLORREF)-1;
	COLORREF clrBkSave   = (COLORREF)-1;
	if (bSelected == TRUE) {
		// �I�����
		// �V�X�e���W���̑I��F�œh��Ԃ�
		clrTextSave = pDC->SetTextColor( ::GetSysColor(COLOR_HIGHLIGHTTEXT) );
		clrBkSave = pDC->SetBkColor( ::GetSysColor(COLOR_HIGHLIGHT) );
	} else {
		// ��I�����
		COLORREF clrTextFg = theApp.m_skininfo.clrReportListText;

		// �F�Â�����
		clrTextSave = pDC->SetTextColor(clrTextFg);
	}

	pDC->DrawText(pszText,
		-1,
		rcLabel,
		DT_LEFT | DT_SINGLELINE | DT_NOPREFIX | DT_NOCLIP | DT_VCENTER | DT_END_ELLIPSIS);

	//--- �E���J����

	// �J�����p�̃��x����`��
	LV_COLUMN lvc;
	lvc.mask = LVCF_FMT | LVCF_WIDTH;
	for (int nColumn = 1; this->GetColumn(nColumn, &lvc); nColumn++) {
		rcSubItem.left = rcSubItem.right;
		rcSubItem.right += lvc.cx;

		int nRetLen = this->GetItemText(nItem, nColumn, szBuff, sizeof(szBuff));
		if (nRetLen == 0) {
			continue;
		}

		pszText = szBuff;

		UINT nJustify = DT_LEFT;

		if (pszText == szBuff) {
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
		}

		rcLabel = rcSubItem;
		rcLabel.left  += OFFSET_OTHER;
		rcLabel.right -= OFFSET_OTHER;

		pDC->DrawText(pszText,
			-1,
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

/**
 * ���ݑI�𒆂̃A�C�e���̃C���f�b�N�X��Ԃ��B
 * �I�𒆂̃A�C�e�����Ȃ���� -1 ��Ԃ��B
 */
int CReportListCtrl::GetSelectedItem(void)
{
	int idx = util::MyGetListCtrlSelectedItemIndex( *this );
	if( GetItemCount() == 0 || idx < 0 ) {
		return -1;
	}
	return idx;
}

BOOL CReportListCtrl::OnEraseBkgnd(CDC* pDC)
{
	pDC->SetBkMode( TRANSPARENT );

	// �r�b�g�}�b�v�̏������ƕ`��
	if( theApp.m_optionMng.IsUseBgImage() ) {
		CRect rectClient;
		this->GetClientRect( &rectClient );

		theApp.m_bgImageReportListCtrl.load();

		if (theApp.m_bgImageReportListCtrl.isEnableImage()) {
			int x = rectClient.left;
			int y = rectClient.top;
			int w = rectClient.Width();
			int h = rectClient.Height();
			int offset = 0;
			if( IsScrollWithBk() ){
				if( GetItemCount() > 0) {
					CRect rcItem;
					GetItemRect( 0 , &rcItem , LVIR_BOUNDS );
					offset = ( rcItem.Height() * GetTopIndex() ) % theApp.m_bgImageReportListCtrl.getBitmapSize().cy;
				}
			}
			util::DrawBitmap( pDC->GetSafeHdc(), theApp.m_bgImageReportListCtrl.getHandle(), x, y, w, h, x, y + offset );
			return TRUE;
		}
	}

	return CListCtrl::OnEraseBkgnd(pDC);
}

BOOL CReportListCtrl::OnLvnItemchanged(NMHDR *pNMHDR, LRESULT *pResult)
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

void CReportListCtrl::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// �X�N���[�����̔w�i�摜�̂���h�~
	if (theApp.m_optionMng.IsUseBgImage()) {
		static int s_nLastPos = nPos;
		if( s_nLastPos != nPos ) {
			//Invalidate( FALSE );			// ������h�~�̂��߂����ł͍ĕ`�悵�Ȃ�
			s_nLastPos = nPos;
		}
	}

	CTouchListCtrl::OnVScroll(nSBCode, nPos, pScrollBar);
}

BOOL CReportListCtrl::PreTranslateMessage(MSG* pMsg)
{
#ifndef WINCE
	// Win32 �̏ꍇ�� VK_RETURN �� OnLvnKeydownReportList ����΂Ȃ��̂ł����ŏ�������
	switch (pMsg->message) {
	case WM_KEYDOWN:
		switch (pMsg->wParam) {
		case VK_RETURN:
			{
				CReportView* pView = (CReportView*)GetParent();
				pView->MyPopupReportMenu();
			}
			break;
		}
		break;
	}
#endif

	return CTouchListCtrl::PreTranslateMessage(pMsg);
}

void CReportListCtrl::OnLButtonDown(UINT nFlags, CPoint point)
{
/* CTouchListCtrl::OnLButtonDown()��CReportListCtrl::SetSelectItem()�Ɉڍs
	// �I��ύX
	int nItem = HitTest(point);
	if (nItem>=0) {
		util::MySetListCtrlItemFocusedAndSelected( *this, GetSelectedItem(), false );
		util::MySetListCtrlItemFocusedAndSelected( *this, nItem, true );
	}
*/
/* CTouchListCtrl::OnLButtonDown()��CReportListCtrl::PopupContextMenu()�Ɉڍs
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
		theApp.m_pReportView->MyPopupReportMenu(point, TPM_LEFTALIGN | TPM_TOPALIGN);
		return;
	}
#endif
*/

	CTouchListCtrl::OnLButtonDown(nFlags, point);
}

/**
 * virtual GetBgBitmapHandle()
 *  �w�iBitmap�̃n���h����Ԃ�
 */
HBITMAP CReportListCtrl::GetBgBitmapHandle()
{
	if( theApp.m_optionMng.IsUseBgImage() ) {
		theApp.m_bgImageReportListCtrl.load();
		if(theApp.m_bgImageReportListCtrl.isEnableImage()) {
			return theApp.m_bgImageReportListCtrl.getHandle();
		}
	}
	return NULL;
}

/**
 * virtual PopupContextMenu()
 *  �|�b�v�A�b�v���j���[���J��
 */
void CReportListCtrl::PopupContextMenu( const CPoint point )
{
	// TODO �{���� WM_COMMAND �Œʒm���ׂ��B
	theApp.m_pReportView->MyPopupReportMenu(point, TPM_LEFTALIGN | TPM_TOPALIGN);
}

/**
 * virtual SetSelectItem()
 *  �A�C�e���̑I����Ԃ�ύX����
 */
void CReportListCtrl::SetSelectItem( const int nItem )
{
	if ( nItem >= 0 ) {
		util::MySetListCtrlItemFocusedAndSelected( *this, GetSelectedItem(), false );
		util::MySetListCtrlItemFocusedAndSelected( *this, nItem, true );
	}
}
