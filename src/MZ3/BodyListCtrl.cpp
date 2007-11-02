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

static const int OFFSET_FIRST	= 2*2;
static const int OFFSET_OTHER	= 6*2;

// CBodyListCtrl

IMPLEMENT_DYNAMIC(CBodyListCtrl, CListCtrl)

CBodyListCtrl::CBodyListCtrl()
	: m_bStopDraw(false)
{
}

CBodyListCtrl::~CBodyListCtrl()
{
}


BEGIN_MESSAGE_MAP(CBodyListCtrl, CListCtrl)
	ON_WM_ERASEBKGND()
	ON_NOTIFY_REFLECT_EX(LVN_ITEMCHANGED, &CBodyListCtrl::OnLvnItemchanged)
	ON_WM_VSCROLL()
END_MESSAGE_MAP()



// CBodyListCtrl ���b�Z�[�W �n���h��



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

	// �A�C�e���̃��x���̕����擾
	CRect rcLabel;
	this->GetItemRect(nItem, rcLabel, LVIR_LABEL);

	// ���̈ʒu�𓯂��ɂ���
	rcAllLabels.left = rcLabel.left;

	// �I������Ă���ꍇ�́A
	// �I������Ă��镶���݈̂�s��h��Ԃ�
	if (bSelected == TRUE) {
		pDC->FillRect(rcAllLabels, &CBrush(::GetSysColor(COLOR_HIGHLIGHT)));
	}else{
		// �w�i�̓h��Ԃ�
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
			util::DrawBitmap( pDC->GetSafeHdc(), theApp.m_bgImageMainBodyCtrl.getHandle(), x, y, w, h, x, y );
		}
	}

	// �A�C�e���̃��x����`���܂��B
	this->GetItemRect(nItem, rcItem, LVIR_LABEL);

	//--- �����J����

	pszText = szBuff;

	rcLabel = rcItem;
	rcLabel.left += OFFSET_FIRST;
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
		CMixiData* data = (CMixiData*)(lvi.lParam);

		COLORREF clrTextFg = theApp.m_skininfo.clrMainBodyListDefaultText;
		switch (data->GetAccessType()) {
		case ACCESS_BBS:
		case ACCESS_EVENT:
		case ACCESS_ENQUETE:
			// �R�~���j�e�B�A�C�x���g�A�A���P�[�g
			// ���ǐ��ɉ����ĐF�Â��B
			if (data->GetLastIndex() == -1) {
				// �S���̖���
				clrTextFg = theApp.m_skininfo.clrMainBodyListNonreadText;
			} else if (data->GetLastIndex() >= data->GetCommentCount()) {
				// ����
				clrTextFg = theApp.m_skininfo.clrMainBodyListDefaultText;
			} else {
				// ���Ǖ�����F�V���L��
				clrTextFg = theApp.m_skininfo.clrMainBodyListNewItemText;
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

		default:
			// �F�Â��Ȃ�
			// ���ɂ���
			clrTextFg = theApp.m_skininfo.clrMainBodyListDefaultText;
			break;
		}

		// �F�̐ݒ�
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
	// ���̐F�ɖ߂�
	for (int nColumn = 1; this->GetColumn(nColumn, &lvc); nColumn++) {
		rcItem.left = rcItem.right;
		rcItem.right = rcItem.left + lvc.cx;

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
			util::DrawBitmap( pDC->GetSafeHdc(), theApp.m_bgImageMainBodyCtrl.getHandle(), x, y, w, h, x, y );
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
	if( pNMLV->iItem < iStart || pNMLV->iItem == iEnd ) {
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
		static int s_nLastPos = nPos;
		if( s_nLastPos != nPos ) {
			Invalidate( FALSE );
			s_nLastPos = nPos;
		}
	}

	CListCtrl::OnVScroll(nSBCode, nPos, pScrollBar);
}
