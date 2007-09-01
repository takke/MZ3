// ReportListCtrl.cpp : �����t�@�C��
//

#include "stdafx.h"
#include "MZ3.h"
#include "ReportListCtrl.h"
#include "util.h"

static const int OFFSET_FIRST = 2;
static const int OFFSET_OTHER = 6;

// CReportListCtrl

IMPLEMENT_DYNAMIC(CReportListCtrl, CListCtrl)

CReportListCtrl::CReportListCtrl()
{
	// �F�̃f�t�H���g�l��ݒ�
	m_clrBgFirst    = ::GetSysColor(COLOR_WINDOW);
	// �Ƃ肠�����Ԃɐݒ�
	m_clrBgSecond = RGB( 255,0,0);
	m_clrFgFirst = ::GetSysColor(COLOR_WINDOWTEXT);
	m_clrFgSecond = ::GetSysColor(COLOR_WINDOWTEXT);
}

CReportListCtrl::~CReportListCtrl()
{
}


BEGIN_MESSAGE_MAP(CReportListCtrl, CListCtrl)
	ON_WM_ERASEBKGND()
	ON_NOTIFY_REFLECT_EX(LVN_ITEMCHANGED, &CReportListCtrl::OnLvnItemchanged)
END_MESSAGE_MAP()



// CReportListCtrl ���b�Z�[�W �n���h��

void CReportListCtrl::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	CDC* pDC = CDC::FromHandle(lpDrawItemStruct->hDC);

	CRect rectClient;
	this->GetClientRect( &rectClient );

	// �r�b�g�}�b�v�̕`��
	if( theApp.m_optionMng.IsUseBgImage() ) {
		int x = lpDrawItemStruct->rcItem.left;
		int y = lpDrawItemStruct->rcItem.top;
		int w = rectClient.Width();
		int h = lpDrawItemStruct->rcItem.bottom - y;
		util::DrawBitmap( pDC->GetSafeHdc(), theApp.m_bgImageReportListCtrl.getHandle(), x, y, w, h, x, y );
	}

	// �ĕ`�悷��Item�̍��W���擾
	CRect rcItem(lpDrawItemStruct->rcItem);

	// �w�i�𓧖��ɂ���t���O�i�C���[�W�f�[�^�p�j
	UINT uiFlags = ILD_TRANSPARENT;

	// �C���[�W�I�u�W�F�N�g�i�[�A�h���X
	CImageList* pImageList;

	// �A�C�e����ID
	int nItem = lpDrawItemStruct->itemID;

	// ���݃R���g���[���Ƀt�H�[�J�X�����邩�ۂ���
	// �t���O��ݒ�
	BOOL bFocus = FALSE;
	if (GetFocus() == this) {
		bFocus = TRUE;
	}

	// �`�悷��F
	COLORREF clrTextSave, clrBkSave;
	COLORREF clrTextFg;
	COLORREF clrImage = m_clrBgFirst;

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

	// �A�C�e���̃��x���̕����擾
	CRect rcLabel;
	this->GetItemRect(nItem, rcLabel, LVIR_LABEL);

	// ���̈ʒu�𓯂��ɂ���
	rcAllLabels.left = rcLabel.left;

	// �����̉������擾
	CSize szSize = pDC->GetTextExtent(szBuff, (int)wcslen(szBuff));
	// �h��Ԃ��͈͂𕶎������ɂ���
	CRect rcItemStr;
	rcItemStr = rcLabel;
	rcItemStr.right =	rcItemStr.left + szSize.cx + OFFSET_OTHER;

	// �w�i�̓h��Ԃ��i�w�i�摜�Ȃ��̏ꍇ�j
	if( !theApp.m_optionMng.IsUseBgImage() ) {
		pDC->FillRect(rcAllLabels, &CBrush(RGB(0xFF, 0xFF, 0xFF)));
	}

	// �I������Ă���ꍇ�́A
	// �I������Ă��镶���݈̂�s��h��Ԃ�
	if (bSelected == TRUE) {
		pDC->FillRect(rcAllLabels, &CBrush(::GetSysColor(COLOR_HIGHLIGHT)));
	}

	// �F��ݒ肵�ăA�C�R�����}�X�N���܂��B
	if ((lvi.state & LVIS_CUT) == LVIS_CUT) {
		uiFlags |= ILD_BLEND50;
	}
	else if (bSelected == TRUE) {
		clrImage = ::GetSysColor(COLOR_HIGHLIGHT);
		uiFlags |= ILD_BLEND50;
	}

	// ��ԃA�C�R����`�悵�܂��B
	UINT nStateImageMask = lvi.state & LVIS_STATEIMAGEMASK;
	if ((nStateImageMask>>12) > 0) {

		int nImage = (nStateImageMask>>12) - 1;

		pImageList = this->GetImageList(LVSIL_STATE);
		if (pImageList) {
			pImageList->Draw(pDC, nImage,
				CPoint(rcItem.left, rcItem.top),
				ILD_TRANSPARENT);
		}
	}

	// �ʏ�̃A�C�R���ƃI�[�o�[���C�A�C�R����`�悵�܂��B
	CRect rcIcon;
	this->GetItemRect(nItem, rcIcon, LVIR_ICON);
	pImageList = this->GetImageList(LVSIL_SMALL);

	if (pImageList != NULL) {
		UINT nOvlImageMask = lvi.state & LVIS_OVERLAYMASK;
		if (rcItem.left < rcItem.right - 1) {
			if (lvi.iImage != 0) {
				ImageList_DrawEx(
					pImageList->m_hImageList, lvi.iImage,
					pDC->m_hDC,
					rcIcon.left, rcIcon.top, 16, 16,
					m_clrBgFirst, clrImage,
					uiFlags | nOvlImageMask);
			}
		}
	}

	// �A�C�e���̃��x����`���܂��B
	this->GetItemRect(nItem, rcItem, LVIR_LABEL);

	pszText = szBuff;

	rcLabel = rcItem;
	rcLabel.left += OFFSET_FIRST;
	rcLabel.right -= OFFSET_FIRST;

	// �V�X�e���W���̑I��F�œh��Ԃ�
	if (bSelected == TRUE) {
		clrTextSave = pDC->SetTextColor(::GetSysColor(COLOR_HIGHLIGHTTEXT));
		clrBkSave = pDC->SetBkColor(::GetSysColor(COLOR_HIGHLIGHT));
	}
	else {
		clrTextFg = RGB(0x00, 0x00, 0x00);

		// �F�Â�����
		clrTextSave = pDC->SetTextColor(clrTextFg);
	}

	pDC->DrawText(pszText,
		-1,
		rcLabel,
		DT_LEFT | DT_SINGLELINE | DT_NOPREFIX | DT_NOCLIP | DT_VCENTER | DT_END_ELLIPSIS);

	// �J�����p�̃��x����`��
	LV_COLUMN lvc;
	lvc.mask = LVCF_FMT | LVCF_WIDTH;
	// ���̐F�ɖ߂�
	for (int nColumn = 1; this->GetColumn(nColumn, &lvc); nColumn++) {
		rcItem.left = rcItem.right;
		rcItem.right += lvc.cx;

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

		rcLabel = rcItem;
		rcLabel.left += OFFSET_OTHER;
		rcLabel.right -= OFFSET_OTHER;

		pDC->DrawText(pszText,
			-1,
			rcLabel,
			nJustify | DT_SINGLELINE | DT_NOPREFIX | DT_NOCLIP | DT_VCENTER | DT_END_ELLIPSIS);
	}

	if (bSelected == TRUE) {
		clrTextSave = pDC->SetTextColor(clrTextSave);
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
	POSITION pos = GetFirstSelectedItemPosition();
	if( pos == NULL ) {
		return -1;
	}
	int idx = GetNextSelectedItem( pos );
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

		int x = rectClient.left;
		int y = rectClient.top;
		int w = rectClient.Width();
		int h = rectClient.Height();
		util::DrawBitmap( pDC->GetSafeHdc(), theApp.m_bgImageReportListCtrl.getHandle(), x, y, w, h, x, y );
		return TRUE;
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
			Invalidate( FALSE );
			s_nLastPos = nPos;
		}
	}

	CListCtrl::OnVScroll(nSBCode, nPos, pScrollBar);
}
