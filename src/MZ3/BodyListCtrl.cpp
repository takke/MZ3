/*
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */
// BodyListCtrl.cpp : �����t�@�C��
//

#include "stdafx.h"
#include "MZ3.h"
#include "BodyListCtrl.h"
#include "MixiData.h"
#include "IniFile.h"
#include "util.h"
#include "util_gui.h"
#include "MixiParserUtil.h"
#include "MZ3View.h"

static const int OFFSET_FIRST	= 2*2;
static const int OFFSET_OTHER	= 6*2;

// CBodyListCtrl

IMPLEMENT_DYNAMIC(CBodyListCtrl, CTouchListCtrl)

CBodyListCtrl::CBodyListCtrl()
	: m_bStopDraw(false)
{
}

CBodyListCtrl::~CBodyListCtrl()
{
}


BEGIN_MESSAGE_MAP(CBodyListCtrl, CTouchListCtrl)
	ON_WM_ERASEBKGND()
	ON_NOTIFY_REFLECT_EX(LVN_ITEMCHANGED, &CBodyListCtrl::OnLvnItemchanged)
	ON_WM_VSCROLL()
	ON_WM_MOUSEWHEEL()
#ifndef WINCE
	ON_WM_NCCALCSIZE()
#endif
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_MEASUREITEM_REFLECT()
END_MESSAGE_MAP()



// CBodyListCtrl ���b�Z�[�W �n���h��



/**
 * PreTranslateMessage
 *  �E�g���u���V���[�g�p
 */
BOOL CBodyListCtrl::PreTranslateMessage(MSG* pMsg)
{
/*	switch (pMsg->message) {
	case WM_MOUSEMOVE:
		break;

	case WM_PAINT:
		// ������h�~�̂��ߎՒf
		return TRUE;

	default:
		MZ3_TRACE( L"CBodyListCtrl::PreTranslateMessage(0x%04X, 0x%04X, 0x%04X)\n", pMsg->message, pMsg->wParam, pMsg->lParam);
		break;
	}
*/
	return CTouchListCtrl::PreTranslateMessage(pMsg);
}

void CBodyListCtrl::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	if( m_bStopDraw )
		return;

	CDC* pDC = CDC::FromHandle(lpDrawItemStruct->hDC);

	// �ĕ`�悷��Item�̍��W���擾
	CRect rcItem( lpDrawItemStruct->rcItem );

	// �A�C�e����ID
	int nItem = lpDrawItemStruct->itemID;

	// ���݃R���g���[���Ƀt�H�[�J�X�����邩�ۂ��̃t���O��ݒ�
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

	int nIconSize = 16;
	if (theApp.m_optionMng.m_bMainViewBodyListIntegratedColumnMode && 
		theApp.m_optionMng.GetFontHeight()>=16) 
	{
		nIconSize = 32;
	}

	// �A�C�e���̃��x���̕����擾
	CRect rcLabel;
	this->GetItemRect(nItem, rcLabel, LVIR_LABEL);
	// �\���ʒu��rcItem�ɍ��킹��
	rcLabel.MoveToY( rcItem.top );
	if (m_bUseIcon==false) {
		// �A�C�R���Ȃ��̏ꍇ�́A�A�C�R���������I�t�Z�b�g��������
		if (rcLabel.left > nIconSize) {
			rcLabel.left -= nIconSize;
		}
	}

	// ���̈ʒu�𓯂��ɂ���
	rcAllLabels.left = rcLabel.left;

	// �I������Ă���ꍇ�́A
	// �I������Ă��镶���݈̂�s��h��Ԃ�
	if (bSelected == TRUE) {
		pDC->FillRect(rcAllLabels, &CBrush(::GetSysColor(COLOR_HIGHLIGHT)));
	}else{
		// �w�i�̓h��Ԃ�
		if( IsDrawBk() ) {
			if( !theApp.m_optionMng.IsUseBgImage() || !theApp.m_bgImageMainBodyCtrl.isEnableImage() ) {
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
				int offset = 0;
				if( IsScrollWithBk() ){
					offset = ( h * GetTopIndex() ) % theApp.m_bgImageMainBodyCtrl.getBitmapSize().cy;
				}
				util::DrawBitmap( pDC->GetSafeHdc(), theApp.m_bgImageMainBodyCtrl.getHandle(), x, y , w, h, x, y + offset );
			}
		}
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
	if (m_bUseIcon) {
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
				if (lvi.iImage >= 0) {
					ImageList_DrawEx(
						pImageList->m_hImageList, lvi.iImage,
						pDC->m_hDC,
						rcIcon.left, rcIcon.top, nIconSize, nIconSize,
						clrMaskBk, clrMaskFg,
						uiFlags | nOvlImageMask);
				}
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

//	rcLabel = rcItem;
	rcLabel.left  += OFFSET_FIRST;
	rcLabel.right -= OFFSET_FIRST;

	// �����F�̕ύX
	COLORREF clrTextSave = (COLORREF)-1;
	COLORREF clrBkSave   = (COLORREF)-1;
	if (bSelected == TRUE) {
		// �I����ԂȂ̂ŁA�V�X�e���W���̑I��F�œh��Ԃ�
		clrTextSave = pDC->SetTextColor(::GetSysColor(COLOR_HIGHLIGHTTEXT));
		clrBkSave = pDC->SetBkColor(::GetSysColor(COLOR_HIGHLIGHT));
	}
	else {
		// ��I����ԂȂ̂ŁA��Ԃɉ����ĐF��ύX����
		CCategoryItem* pCategory = theApp.m_pMainView->m_selGroup->getSelectedCategory();
		if (pCategory!=NULL && 0 <= lvi.lParam && lvi.lParam < (int)pCategory->m_body.size()) {
			CMixiData* data = &pCategory->m_body[ lvi.lParam ];

			COLORREF clrTextFg = theApp.m_skininfo.clrMainBodyListDefaultText;
			switch (data->GetAccessType()) {
			case ACCESS_BBS:
			case ACCESS_EVENT:
			case ACCESS_EVENT_JOIN:
			case ACCESS_ENQUETE:
				// �R�~���j�e�B�A�C�x���g�A�A���P�[�g
				// ���ǐ��ɉ����ĐF�Â��B
				{
					int lastIndex = mixi::ParserUtil::GetLastIndexFromIniFile(*data);
					if (lastIndex == -1) {
						// �S���̖���
						clrTextFg = theApp.m_skininfo.clrMainBodyListNonreadText;
					} else if (lastIndex >= data->GetCommentCount()) {
						// ����
						clrTextFg = theApp.m_skininfo.clrMainBodyListDefaultText;
					} else {
						// ���Ǖ�����F�V���L��
						clrTextFg = theApp.m_skininfo.clrMainBodyListNewItemText;
					}
				}
				break;

			case ACCESS_DIARY:
			case ACCESS_MYDIARY:
				// ���L
				// �O���u���O�͔����\��
				if( data->GetURL().Find( L"?url=http" ) != -1 ) {
					// "?url=http" ���܂ނ̂ŊO���u���O�Ƃ݂Ȃ�
					clrTextFg = theApp.m_skininfo.clrMainBodyListExternalBlogText;
				} else {
					// mixi ���L
					// ���ǂȂ�A���ǂȂ獕
					if( util::ExistFile(util::MakeLogfilePath( *data )) ) {
						// ���O����:����
						clrTextFg = theApp.m_skininfo.clrMainBodyListDefaultText;
					}else{
						// ���O�Ȃ�:����
						clrTextFg = theApp.m_skininfo.clrMainBodyListNonreadText;
					}
				}
				break;

			case ACCESS_NEWS:
			case ACCESS_MESSAGE:
				// �j���[�X
				// ���O������΁i���ǂȂ�j���A���ǂȂ��
				if( util::ExistFile(util::MakeLogfilePath( *data )) ) {
					// ���O����:����
					clrTextFg = theApp.m_skininfo.clrMainBodyListDefaultText;
				}else{
					// ���O�Ȃ�:����
					clrTextFg = theApp.m_skininfo.clrMainBodyListNonreadText;
				}
				break;

			case ACCESS_PROFILE:
			case ACCESS_BIRTHDAY:
				// ���[�U�v���t�B�[��
				// �}�C�~�N�Ȃ�ɂ���B
				if( data->IsMyMixi() ) {
					clrTextFg = theApp.m_skininfo.clrMainBodyListFootprintMyMixiText;
				}else{
					clrTextFg = theApp.m_skininfo.clrMainBodyListDefaultText;
				}
				break;

			case ACCESS_COMMUNITY:
				// �R�~���j�e�B
				{
					// �b�菈�u�Ƃ��āA�g�s�b�N�ꗗ�̃��O���݃`�F�b�N�ŐF�ύX���s���B

					// �g�s�b�N�ꗗ�p mixi �I�u�W�F�N�g�𐶐�����
					CMixiData mixi = *data;
					mixi.SetAccessType( ACCESS_LIST_BBS );
					CString url;
					url.Format( L"list_bbs.pl?id=%d", mixi::MixiUrlParser::GetID(mixi.GetURL()) );
					mixi.SetURL(url);

					// ���݃`�F�b�N�B
					if( util::ExistFile(util::MakeLogfilePath(mixi) ) ) {
						// ���O����:����
						clrTextFg = theApp.m_skininfo.clrMainBodyListDefaultText;
					}else{
						// ���O�Ȃ�:����
						clrTextFg = theApp.m_skininfo.clrMainBodyListNonreadText;
					}
				}
				break;

			case ACCESS_TWITTER_USER:
				// Twitter ����
				{
					// �I�����ڂƓ����I�[�i�[ID�̍��ڂ������\������B
					int selectedIdx = util::MyGetListCtrlSelectedItemIndex(*this);
					CCategoryItem* pCategory = theApp.m_pMainView->m_selGroup->getSelectedCategory();
					if (pCategory!=NULL && 0 <= selectedIdx && selectedIdx < (int)pCategory->m_body.size()) {
						const CMixiData& selectedData = pCategory->m_body[ selectedIdx ];
						if (selectedData.GetOwnerID()==data->GetOwnerID()) {
							// �����I�[�i�[ID�F�����\��
							clrTextFg = theApp.m_skininfo.clrMainBodyListNonreadText;
						} else {
							// �قȂ�I�[�i�[ID
							clrTextFg = theApp.m_skininfo.clrMainBodyListDefaultText;
						}
					}
				}
				break;

			default:
				// �F�Â��Ȃ�
				// ���ɂ���
				clrTextFg = theApp.m_skininfo.clrMainBodyListDefaultText;
				break;
			}

			// �F�̐ݒ�
			clrTextSave = pDC->SetTextColor(clrTextFg);
		}
	}

	if (theApp.m_optionMng.m_bMainViewBodyListIntegratedColumnMode) {
		// �����J�������[�h�A1�s��
		CRect rcDraw = rcAllLabels;

		rcDraw.left += OFFSET_FIRST;
		pDC->DrawText(pszText,
			-1,
			rcDraw,
			DT_LEFT | DT_SINGLELINE | DT_NOPREFIX | DT_NOCLIP | DT_TOP | DT_END_ELLIPSIS);
	} else {
		// ��1�J����
		pDC->DrawText(pszText,
			-1,
			rcLabel,
			DT_LEFT | DT_SINGLELINE | DT_NOPREFIX | DT_NOCLIP | DT_VCENTER | DT_END_ELLIPSIS);
	}

	//--- �E���J����

	// �J�����p�̃��x����`��
	LV_COLUMN lvc;
	lvc.mask = LVCF_FMT | LVCF_WIDTH;
	// ���̐F�ɖ߂�
	for (int nColumn = 1; this->GetColumn(nColumn, &lvc); nColumn++) {
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

		if (theApp.m_optionMng.m_bMainViewBodyListIntegratedColumnMode) {
			// �����J�������[�h�AnColumn �s�ڂ̕`��
			// TODO �Ƃ肠����2�s����Ƃ��A�����̍����Ƃ���B
			CRect rcDraw = rcAllLabels;
			rcDraw.top    += rcDraw.Height()/2;
			rcDraw.bottom -= 2;
			rcDraw.left   += OFFSET_FIRST;
			rcDraw.left   += 16;

			pDC->DrawText(pszText,
				-1,
				rcDraw,
				nJustify | DT_SINGLELINE | DT_NOPREFIX | DT_NOCLIP | DT_BOTTOM | DT_RIGHT | DT_END_ELLIPSIS);
		} else {
			// �� N �J����
			rcSubItem.left  = rcSubItem.right;
			rcSubItem.right = rcSubItem.left + lvc.cx;

			CRect rcDraw = rcSubItem;
			rcDraw.left  += OFFSET_OTHER;
			rcDraw.right -= OFFSET_OTHER;

			pDC->DrawText(pszText,
				-1,
				rcDraw,
				nJustify | DT_SINGLELINE | DT_NOPREFIX | DT_NOCLIP | DT_VCENTER | DT_END_ELLIPSIS);
		}
	}

	// �F��߂�
	if (clrTextSave != (COLORREF)-1) {
		clrTextSave = pDC->SetTextColor(clrTextSave);
	}
	if (clrBkSave != (COLORREF)-1) {
		clrBkSave = pDC->SetBkColor(clrBkSave);
	}

	// ���t��؂�o�[�̕`��
	// ���݂̗v�f (nItem) �̏㑤�ɐ�������
	// ���菈��
	bool bDrawDayBreakBar = false;
	if (nItem>=1) { // 2�Ԗڈȍ~�̗v�f�ł��邱�ƁB
		if (theApp.m_pMainView != NULL &&
			theApp.m_pMainView->m_selGroup != NULL &&
			theApp.m_pMainView->m_selGroup->getSelectedCategory() != NULL) 
		{
			CCategoryItem* pCategory = theApp.m_pMainView->m_selGroup->getSelectedCategory();
			CMixiDataList& list = pCategory->GetBodyList();
			if (list.size()>(size_t)nItem) {
				// ���t�擾
				CTime t0 = list[nItem-1].GetDateRaw();
				CTime t1 = list[nItem  ].GetDateRaw();
				if (t0.GetYear()  != t1.GetYear() ||
					t0.GetMonth() != t1.GetMonth() ||
					t0.GetDay()   != t1.GetDay())
				{
					// ���t���قȂ�
					bDrawDayBreakBar = true;
				}
			}
		}
	}
	// �`�揈��
	if (bDrawDayBreakBar) {
		COLORREF clrDayBreakBar = theApp.m_skininfo.clrMainBodyListDayBreakLine;
		CPen penDayBreakBar(PS_SOLID, 1, clrDayBreakBar);

		CPen* pOldPen = pDC->SelectObject(&penDayBreakBar);

		pDC->MoveTo(rcAllLabels.left, rcAllLabels.top);
		pDC->LineTo(rcAllLabels.right, rcAllLabels.top);

		pDC->SelectObject(pOldPen);
//		wprintf( L"draw line (%d,%d) to (%d,%d)\n", 
//			rcAllLabels.left, rcAllLabels.top,
//			rcAllLabels.right, rcAllLabels.top);
	}

	// �A�C�e�����t�H�[�J�X�������Ă���Ƃ���
	// ���̑I��g��`�悷��
	if ((lvi.state & LVIS_FOCUSED) == LVIS_FOCUSED && bFocus == TRUE) {
		pDC->DrawFocusRect(rcAllLabels);
	}
}

/**
 * �w�b�_�[������ύX����B
 *
 * NULL �Ȃ炻�̃C���f�b�N�X�̕����͕ύX���Ȃ��B
 */
void CBodyListCtrl::SetHeader(LPCTSTR col1, LPCTSTR col2)
{
	HDITEM hdi;
	TCHAR lpBuffer[128];

	hdi.mask = HDI_TEXT;
	hdi.pszText = lpBuffer;
	hdi.cchTextMax = 128;

	if( col1 != NULL ) {
		this->GetHeaderCtrl()->GetItem(0, &hdi);
		wcscpy(hdi.pszText, col1);
		this->GetHeaderCtrl()->SetItem(0, &hdi);
	}

	if( col2 != NULL ) {
		this->GetHeaderCtrl()->GetItem(1, &hdi);
		wcscpy(hdi.pszText, col2);
		this->GetHeaderCtrl()->SetItem(1, &hdi);
	}
}


BOOL CBodyListCtrl::OnEraseBkgnd(CDC* pDC)
{
	//MZ3_TRACE( L" OnEraseBkgnd()\n");

	pDC->SetBkMode( TRANSPARENT );

	// �r�b�g�}�b�v�̏������ƕ`��
	if( theApp.m_optionMng.IsUseBgImage() ) {
		CRect rectClient;
		this->GetClientRect( &rectClient );

		theApp.m_bgImageMainBodyCtrl.load();

		if (theApp.m_bgImageMainBodyCtrl.isEnableImage()) {
			int x = rectClient.left;
			int y = rectClient.top;
			int w = rectClient.Width();
			int h = rectClient.Height();
			int offset = 0;
			if( IsScrollWithBk() ){
				if( GetItemCount() > 0) {
					CRect rcItem;
					GetItemRect( 0 , &rcItem , LVIR_BOUNDS );
					offset = ( rcItem.Height() * GetTopIndex() ) % theApp.m_bgImageMainBodyCtrl.getBitmapSize().cy;
				}
			}
			util::DrawBitmap( pDC->GetSafeHdc(), theApp.m_bgImageMainBodyCtrl.getHandle(), x, y, w, h, x, y + offset );
			return TRUE;
		}
	}

	return CListCtrl::OnEraseBkgnd(pDC);
}

BOOL CBodyListCtrl::OnLvnItemchanged(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);

	// �ŏ����Ō�̍��ڂł���ΑS�̂��ĕ`��i�w�i�摜�̂���h�~�j
	int iStart = GetTopIndex();
	int iEnd   = iStart + GetCountPerPage();
	if( pNMLV->iItem < iStart || pNMLV->iItem >= iEnd ) {
		if( pNMLV->uNewState & LVIS_FOCUSED ) {
			Invalidate( FALSE );
		}
	}

	*pResult = 0;

	// FALSE ��Ԃ����ƂŁA�e�E�B���h�E�ɂ��C�x���g��n��
	return FALSE;
}


void CBodyListCtrl::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// �X�N���[�����̔w�i�摜�̂���h�~
	if (theApp.m_optionMng.IsUseBgImage()) {
		// �X�N���[���ʒu���ω����Ă�����ĕ`��
		static int s_nLastPos = nPos;
		if( s_nLastPos != nPos ) {
			//Invalidate( FALSE );			// ������h�~�̂��߂����ł͍ĕ`�悵�Ȃ�
			s_nLastPos = nPos;

			theApp.m_pMainView->MoveMiniImageDlg();
		}
	}

	CTouchListCtrl::OnVScroll(nSBCode, nPos, pScrollBar);
}

BOOL CBodyListCtrl::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	// �X�N���[�����̔w�i�摜�̂���h�~
	if (theApp.m_optionMng.IsUseBgImage()) {
		// �X�N���[���ʒu���ω����Ă�����ĕ`��
		int pos = GetScrollPos(SB_VERT);
		static int s_lastPos = pos;
		if (pos != s_lastPos) {
			//Invalidate( FALSE );			// ������h�~�̂��߂����ł͍ĕ`�悵�Ȃ�
			s_lastPos = pos;

			theApp.m_pMainView->MoveMiniImageDlg();
		}
	}

	return CTouchListCtrl::OnMouseWheel(nFlags, zDelta, pt);
	//// �E�N���b�N�{�}�E�X�z�C�[�������̂��߂ɐe�Ăяo�� �� CTouchListCtrl�Ɉڍs
	//return theApp.m_pMainView->OnMouseWheel(nFlags, zDelta, pt);
}

#ifndef WINCE
void CBodyListCtrl::OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS* lpncsp)
{
	// �����X�N���[���o�[�̔�\��
	this->EnableScrollBarCtrl( SB_HORZ, FALSE );

	CListCtrl::OnNcCalcSize(bCalcValidRects, lpncsp);
}
#endif

void CBodyListCtrl::OnMouseMove(UINT nFlags, CPoint point)
{
#ifndef WINCE
//	TRACE( L"OnMouseMove %d,%d\n", point.x, point.y );
	// �I�v�V������On�Ȃ�mini�摜��ʂ��ړ�
	if (theApp.m_optionMng.m_bShowMainViewMiniImage &&
		theApp.m_optionMng.m_bShowMainViewMiniImageDlg &&
		theApp.m_optionMng.m_bShowMainViewMiniImageOnMouseOver ) 
	{
		int idx = HitTest(point);
		if (idx>=0) {
			TRACE( L" idx=%d\n", idx );

			// �摜�ʒu�ύX
			theApp.m_pMainView->MoveMiniImageDlg( idx, point.x, point.y );
		}
	}
#endif
	CTouchListCtrl::OnMouseMove(nFlags, point);
}

void CBodyListCtrl::OnLButtonDown(UINT nFlags, CPoint point)
{
/* CTouchListCtrl::OnLButtonDown()��CBodyListCtrl::SetSelectItem()�Ɉڍs
	// �I��ύX
	int nItem = HitTest(point);
	if (nItem>=0) {
		int idx = util::MyGetListCtrlSelectedItemIndex( *this );
		if( GetItemCount() > 0 && idx >= 0 ) {
			util::MySetListCtrlItemFocusedAndSelected( *this, idx, false );
			util::MySetListCtrlItemFocusedAndSelected( *this, nItem, true );
		}
	}
*/
/* CTouchListCtrl::OnLButtonDown()��CBodyListCtrl::PopupContextMenu()�Ɉڍs
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
		theApp.m_pMainView->PopupBodyMenu(point, TPM_LEFTALIGN | TPM_TOPALIGN);
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
HBITMAP CBodyListCtrl::GetBgBitmapHandle()
{
	if( theApp.m_optionMng.IsUseBgImage() ) {
		theApp.m_bgImageMainBodyCtrl.load();
		if(theApp.m_bgImageMainBodyCtrl.isEnableImage()) {
			return theApp.m_bgImageMainBodyCtrl.getHandle();
		}
	}
	return NULL;
}

/**
 * virtual PopupContextMenu()
 *  �|�b�v�A�b�v���j���[���J��
 */
void CBodyListCtrl::PopupContextMenu( const CPoint point )
{
	// TODO �{���� WM_COMMAND �Œʒm���ׂ��B
	theApp.m_pMainView->PopupBodyMenu(point, TPM_LEFTALIGN | TPM_TOPALIGN);
}

/**
 * virtual SetSelectItem()
 *  �A�C�e���̑I����Ԃ�ύX����
 */
void CBodyListCtrl::SetSelectItem( const int nItem )
{
	if (nItem>=0) {
		int idx = util::MyGetListCtrlSelectedItemIndex( *this );
		if( GetItemCount() > 0 && idx >= 0 ) {
			util::MySetListCtrlItemFocusedAndSelected( *this, idx, false );
		}
		util::MySetListCtrlItemFocusedAndSelected( *this, nItem, true );
	}
}

void CBodyListCtrl::MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct)
{
	LOGFONT lf;
	GetFont()->GetLogFont( &lf );

	if (lf.lfHeight < 0) {
		lpMeasureItemStruct->itemHeight = -lf.lfHeight;
	} else {
		lpMeasureItemStruct->itemHeight = lf.lfHeight;
	}

	if (theApp.m_optionMng.m_bMainViewBodyListIntegratedColumnMode) {
		// �����J�������[�h�̂��ߍ�����N�{����
		if (lpMeasureItemStruct->itemHeight < 0) {
			lpMeasureItemStruct->itemHeight = lpMeasureItemStruct->itemHeight*2 -3;
		} else {
			lpMeasureItemStruct->itemHeight = lpMeasureItemStruct->itemHeight*2 +3;
		}
	}
}
