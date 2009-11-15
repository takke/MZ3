/*
 * Copyright (c) MZ3 Project Team All rights reserved.
 * Code licensed under the BSD License:
 * http://www.mz3.jp/license.txt
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

/// �J�������[�h�̃X�^�C��
namespace COLUMN_MODE_STYLE {
static const int BOX_MARGIN_BOTTOM_PT = 2;	///< �{�b�N�X�ԃ}�[�W��(����)[pt]
static const int FIRST_MARGIN_LEFT	= 4;	///< ��1�J�����A�����}�[�W��
static const int FIRST_MARGIN_RIGHT	= 4;	///< ��1�J�����A�E���}�[�W��
static const int OTHER_MARGIN_LEFT	= 12;	///< ��2�J�����ȍ~�A�����}�[�W��
static const int OTHER_MARGIN_RIGHT	= 12;	///< ��2�J�����ȍ~�A�E���}�[�W��
}

/// �����J�������[�h�̃X�^�C��
namespace INTEGRATED_MODE_STYLE {
static const int BOX_MARGIN_BOTTOM_PT   = 2;	///< �{�b�N�X�ԃ}�[�W��(����)[pt]
static const int EACH_LINE_MARGIN_PT    = 1;	///< �s�ԃ}�[�W��[pt]
static const int FIRST_LINE_MARGIN_LEFT = 4;	///< 1�s�ځA���}�[�W��
static const int OTHER_LINE_MARGIN_LEFT = 4+16;	///< 2�s�ڈȍ~�A���}�[�W��
};

/* �����J�������[�h
 *            ------x-----------------------  x : FIRST_LINE_MARGIN_LEFT 
 *            ------xx----------------------  xx: OTHER_LINE_MARGIN_LEFT
 * 1st-line: |<icon> AAAAAAAAAAAAAAAAAAAAAAA|
 * 2nd-line: |<icon>  BBBBBBBBBBBBBBBBBBBBBB| A��B, B��C�̍s��: EACH_LINE_MARGIN_PT
 * 3rd-line: |<icon>  CCCCCCCCCCCCCCCCCCCCCC|
 *           |                              y y:  BOX_MARGIN_BOTTOM_PT
 * 1st-line: |<icon> DDDDDDDDDDDDDDDDDDDDDDD|
 * 2nd-line: |<icon>  EEEEEEEEEEEEEEEEEEEEEE|
 * 3rd-line: |<icon>  FFFFFFFFFFFFFFFFFFFFFF|
 */


// CBodyListCtrl

IMPLEMENT_DYNAMIC(CBodyListCtrl, CTouchListCtrl)

CBodyListCtrl::CBodyListCtrl()
{
#ifdef WINCE
	// �X�N���[������ iPhone ���̍ĕ`����s��
	// TODO 1��ʂł͂�͂葫��Ȃ��B�x���I�ɕ�����ʕ��m�ۂł���悤�ɂ��Ă��畜�������邱�ƁB
//	m_bBlackScrollMode = true;
#endif
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
	ON_MESSAGE(WM_SETFONT, OnSetFont)
	ON_WM_LBUTTONUP()
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

//	MZ3_TRACE(L"CBodyListCtrl::DrawItem(), nItem[%d]\n", lpDrawItemStruct->itemID);

	CDC* pDC = CDC::FromHandle(lpDrawItemStruct->hDC);

//	util::StopWatch sw_draw1, sw_draw2, sw_draw4, sw_draw3;

//sw_draw1.start();

	// �ĕ`�悷��Item�̍��W���擾
	CRect rcItem( lpDrawItemStruct->rcItem );

	// �A�C�e����ID
	int nItem = lpDrawItemStruct->itemID;

	// ���݃R���g���[���Ƀt�H�[�J�X�����邩�ۂ��̃t���O��ݒ�
	BOOL bFocus = FALSE;
	if (GetFocus() == this) {
		bFocus = TRUE;
	}

	// �A�C�e�� �f�[�^���擾���܂��B
	LV_ITEM lvi;
	lvi.mask = LVIF_IMAGE | LVIF_STATE;
	lvi.iItem = nItem;
	lvi.iSubItem = 0;
	lvi.stateMask = LVIS_SELECTED | LVIS_STATEIMAGEMASK;
	this->GetItem(&lvi);


	// �o�͑Ώۂ̗v�f���擾����
	CCategoryItem* pCategory = NULL;
	if (theApp.m_pMainView != NULL &&
		theApp.m_pMainView->m_selGroup != NULL &&
		theApp.m_pMainView->m_selGroup->getSelectedCategory() != NULL)
	{
		pCategory = theApp.m_pMainView->m_selGroup->getSelectedCategory();
	}

	// �o�͑Ώۂ̗v�f
	const CMixiData* pData = NULL;
	if (pCategory!=NULL && 0 <= nItem && nItem < (int)pCategory->m_body.size()) {
		pData = &pCategory->m_body[ nItem ];
	}

	// �I��v�f
	int selectedIdx = util::MyGetListCtrlSelectedItemIndex(*this);
	CMixiData* pSelectedData = NULL;
	if (pCategory!=NULL && 0 <= selectedIdx && selectedIdx < (int)pCategory->m_body.size()) {
		pSelectedData = &pCategory->m_body[ selectedIdx ];
	}

	// �o�͕�����̎擾(pData ����ϊ�)
	CString strTarget1;
	CString strTarget2;
	CString strTarget3;
	if (pCategory!=NULL && pData!=NULL) {
		// ��1�J����
		// �ǂ̍��ڂ�^���邩�́A�J�e�S�����ڃf�[�^���̎�ʂŌ��߂�B
		// ���s�̓X�y�[�X�ɒu������B
		strTarget1 = util::MyGetItemByBodyColType(pData, pCategory->m_bodyColType1, false);
		strTarget1.Replace(L"\r\n", L" ");

		// ��2�A��3�J����
		if (theApp.m_optionMng.m_bBodyListIntegratedColumnMode) {
			// �����J�������[�h�̏ꍇ�̓J�e�S���̃J�����^�C�v(�g�O���ŕύX�����)�ɖ��֌W��
			// �J�e�S����ʂɉ������������ݒ肷��
			AccessTypeInfo::BODY_INDICATE_TYPE bodyColType;
			bodyColType = theApp.m_accessTypeInfo.getBodyHeaderCol2Type(pCategory->m_mixi.GetAccessType());
			strTarget2 = util::MyGetItemByBodyColType(pData, bodyColType, false);
			bodyColType = theApp.m_accessTypeInfo.getBodyHeaderCol3Type(pCategory->m_mixi.GetAccessType());
			strTarget3 = util::MyGetItemByBodyColType(pData, bodyColType, false);
			
		} else {
			strTarget2 = util::MyGetItemByBodyColType(pData, pCategory->m_bodyColType2);
			strTarget3 = util::MyGetItemByBodyColType(pData, pCategory->m_bodyColType3);
		}
	}


	// �A�C�e�����I����Ԃ��ۂ��̃t���O��ݒ�
	BOOL bSelected =
		(bFocus || (GetStyle() & LVS_SHOWSELALWAYS))
		&& lvi.state & LVIS_SELECTED;
	bSelected = bSelected || (lvi.state & LVIS_DROPHILITED);

//sw_draw1.stop();
//sw_draw2.start();

	// �A�C�e���̕\������Ă��镝���擾
	CRect rcAllLabels;
	this->GetItemRect(nItem, rcAllLabels, LVIR_BOUNDS);
	// �\���ʒu��rcItem�ɍ��킹��
	rcAllLabels.MoveToY( rcItem.top );

	int nIconSize = 16;
	switch (m_iconMode) {
	case ICON_MODE_48:	nIconSize = 48;	break;
	case ICON_MODE_32:	nIconSize = 32;	break;
	default:			nIconSize = 16;	break;
	}

	// �A�C�e���̃��x���̕����擾
	CRect rcLabel;
	this->GetItemRect(nItem, rcLabel, LVIR_LABEL);
	// �\���ʒu��rcItem�ɍ��킹��
	rcLabel.MoveToY( rcItem.top );
	if (m_iconMode==ICON_MODE_NONE) {
		// �A�C�R���Ȃ��̏ꍇ�́A�A�C�R���������I�t�Z�b�g��������
		if (rcLabel.left > nIconSize) {
			rcLabel.left -= nIconSize;
		}
	}

	// ���̈ʒu�𓯂��ɂ���
	rcAllLabels.left = rcLabel.left;

	// �I������Ă���ꍇ�́A
	// �I������Ă��镶���݈̂�s��h��Ԃ�
	if (bSelected) {
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
	if (m_iconMode!=ICON_MODE_NONE) {
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

//sw_draw2.stop();
//sw_draw3.start();

	// �A�C�e���̃��x����`���܂��B
	CRect rcSubItem;
	this->GetItemRect(nItem, rcSubItem, LVIR_LABEL);
	// �\���ʒu��rcItem�ɍ��킹��
	rcSubItem.MoveToY( rcItem.top );

	//--- �����J����
//	rcLabel = rcItem;
	rcLabel.left  += COLUMN_MODE_STYLE::FIRST_MARGIN_LEFT;
	rcLabel.right -= COLUMN_MODE_STYLE::FIRST_MARGIN_RIGHT;

	// �����F�̕ύX
	COLORREF clrTextSave = (COLORREF)-1;
	COLORREF clrBkSave   = (COLORREF)-1;
	if (bSelected == TRUE) {
		// �I����ԂȂ̂ŁA�V�X�e���W���̑I��F�œh��Ԃ�
		clrTextSave = pDC->SetTextColor(::GetSysColor(COLOR_HIGHLIGHTTEXT));
		clrBkSave = pDC->SetBkColor(::GetSysColor(COLOR_HIGHLIGHT));
	} else {
		// ��I����ԂȂ̂ŁA��Ԃɉ����ĐF��ύX����
//		util::StopWatch sw_coloring;

//		sw_coloring.start();
		if (pData!=NULL) {

			COLORREF clrTextFg = theApp.m_skininfo.clrMainBodyListDefaultText;
			switch (pData->GetAccessType()) {
			case ACCESS_BBS:
			case ACCESS_EVENT:
			case ACCESS_EVENT_JOIN:
			case ACCESS_ENQUETE:
				// �R�~���j�e�B�A�C�x���g�A�A���P�[�g
				// ���ǐ��ɉ����ĐF�Â��B
				{
					int lastIndex = mixi::ParserUtil::GetLastIndexFromIniFile(*pData);
					if (lastIndex == -1) {
						// �S���̖���
						clrTextFg = theApp.m_skininfo.clrMainBodyListNonreadText;
					} else if (lastIndex >= pData->GetCommentCount()) {
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
			case ACCESS_NEIGHBORDIARY:
				// ���L
				// �O���u���O�͔����\��
				if( pData->GetURL().Find( L"?url=http" ) != -1 ) {
					// "?url=http" ���܂ނ̂ŊO���u���O�Ƃ݂Ȃ�
					clrTextFg = theApp.m_skininfo.clrMainBodyListExternalBlogText;
				} else {
					// mixi ���L
					// ���ǂȂ�A���ǂȂ獕
					if( util::ExistFile(util::MakeLogfilePath( *pData )) ) {
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
				if( util::ExistFile(util::MakeLogfilePath( *pData )) ) {
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
				if( pData->IsMyMixi() ) {
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
					CMixiData mixi = *pData;
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
					// �f�t�H���g�l�ݒ�
					clrTextFg = theApp.m_skininfo.clrMainBodyListDefaultText;

					// �������̔����������\������
					const CString& bodyText = pData->GetBody();
					LPCTSTR szMyTwitterID = theApp.m_loginMng.GetTwitterId();
					if (bodyText.Find(util::FormatString(L"@%s", szMyTwitterID))!=-1) {
						// �����Q
						clrTextFg = theApp.m_skininfo.clrMainBodyListEmphasis2;
						break;
					}

					if (pSelectedData==NULL) {
						break;
					}

					// �����̔����������\������
					const CString& pTargetName = pData->GetName();
					if (pTargetName==szMyTwitterID) {
						// �����I�[�i�[ID�F�����\��
						clrTextFg = theApp.m_skininfo.clrMainBodyListEmphasis4;
						break;
					}

					// �I�����ڂƓ����I�[�i�[ID�̍��ڂ������\������B
					if (pSelectedData->GetOwnerID()==pData->GetOwnerID()) {
						// �����I�[�i�[ID�F�����\��
						clrTextFg = theApp.m_skininfo.clrMainBodyListNonreadText;
						break;
					}

					// �I�����ړ��̈��p���[�U "@xxx @yyy" �̂����ꂩ�Ɠ������[�U�ł���΋����\������

					// �I�����ړ��̈��p���[�U���X�g���擾����B�Ȃ���΂����ō��B
					util::SetTwitterQuoteUsersWhenNotGenerated(pSelectedData);

					// �����ꂩ�ƈ�v����΋����\��
					int n = pSelectedData->GetTextArraySize(L"quote_users");
					for (int i=0; i<n; i++) {
						LPCTSTR szQuoteUser = pSelectedData->GetTextArrayValue(L"quote_users", i);
						
						// ��v����΋����\��
						if (pTargetName==szQuoteUser) {
							clrTextFg = theApp.m_skininfo.clrMainBodyListEmphasis3;
							break;
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
//		sw_coloring.stop();

//		MZ3LOGGER_DEBUG(
//			util::FormatString(L" *** coloring detect[%dms]",
//				sw_coloring.getElapsedMilliSecUntilStoped()));
	}

//sw_draw3.stop();


	// �e�J����(�e�s)�̕`��
//sw_draw4.start();

	// �G�����𕶎���ɕϊ�����
	if( LINE_HAS_EMOJI_LINK(strTarget1) ) {
		mixi::ParserUtil::ReplaceEmojiImageToText( strTarget1 );
	}

	if (theApp.m_optionMng.m_bBodyListIntegratedColumnMode) {
		// �����J�������[�h�̕`��

		//--- 1�s�ڂ̕`��

		// �p�^�[���̕ϊ�
		CString strLine1 = m_strIntegratedLinePattern1;
		bool bEmpty = true;
		if (m_strIntegratedLinePattern1.Find(L"%1")>=0 && !strTarget1.IsEmpty()) {
			bEmpty = false;
		}
		strLine1.Replace(L"%1", strTarget1);
		if (m_strIntegratedLinePattern1.Find(L"%2")>=0 && !strTarget2.IsEmpty()) {
			bEmpty = false;
		}
		strLine1.Replace(L"%2", strTarget2);
		if (m_strIntegratedLinePattern1.Find(L"%3")>=0 && !strTarget3.IsEmpty()) {
			bEmpty = false;
		}
		strLine1.Replace(L"%3", strTarget3);
		if (bEmpty) {
			strLine1 = L"";
		}

		// <small>...</small> ������Ώ���������
		// TODO �w�肳�ꂽ�͈͂�������������悤�ɁB�Ƃ肠�����S�̂������������Ⴄ
		CFont* pOriginalFont = NULL;
		if (strLine1.Find(L"<small>")>=0 && strLine1.Find(L"</small>")>=0) {
			strLine1.Replace(L"<small>", L"");
			strLine1.Replace(L"</small>", L"");
			pOriginalFont = pDC->SelectObject(&theApp.m_fontSmall);
		}
		// <big>...</big> ������Α傫������
		// TODO �w�肳�ꂽ�͈͂����傫������悤�ɁB�Ƃ肠�����S�̂�傫�������Ⴄ
		else if (strLine1.Find(L"<big>")>=0 && strLine1.Find(L"</big>")>=0) {
			strLine1.Replace(L"<big>", L"");
			strLine1.Replace(L"</big>", L"");
			pOriginalFont = pDC->SelectObject(&theApp.m_fontBig);
		}

		// �p�^�[����\t���܂܂�Ă���΍��E�ɕ�������
		CString strLine1Left = strLine1;
		CString strLine1Right = L"";
		int iSepPos = strLine1.Find(L"\t");
		if( iSepPos >=0 ){
			strLine1Left = strLine1.Left( iSepPos );
			strLine1Right = strLine1.Mid( iSepPos + 1);
			strLine1.Replace( L"\t" , L" " );
		}
		//MZ3_TRACE( L"��s�ځF%s>��%s��%s��\n" , strLine1 , strLine1Left ,strLine1Right  );

		// �`��
		CRect rcDraw = rcAllLabels;
		rcDraw.left += INTEGRATED_MODE_STYLE::FIRST_LINE_MARGIN_LEFT;
		// ���E�����`��
		CSize csDrawRight = pDC->GetOutputTextExtent( strLine1Right );
		CSize csDrawAllText = pDC->GetOutputTextExtent( strLine1 );
		if( csDrawAllText.cx > rcDraw.Width() && csDrawRight.cx < rcDraw.Width() ) {
			// ��ɕ�������������𗼒[�ɕ����ĕ`�悷��
			// ����������͉E�������񕪂��������̈�ɕ`�悷��
			CRect rcDrawLeft( rcDraw );
			rcDrawLeft.right -= csDrawRight.cx; 
			pDC->DrawText(strLine1Left,
				-1,
				rcDrawLeft,
				DT_LEFT | DT_SINGLELINE | DT_NOPREFIX | DT_NOCLIP | DT_TOP | DT_END_ELLIPSIS);
			// �E��������͉E�񂹂ŕ`�悷��
			pDC->DrawText(strLine1Right,
				-1,
				rcDraw,
				DT_RIGHT | DT_SINGLELINE | DT_NOPREFIX | DT_NOCLIP | DT_TOP | DT_END_ELLIPSIS);
		} else {
			// �E��������̕����`��̈���L���Ȃ炵�傤���Ȃ��̂ł��̂܂ܕ`�悷��
			pDC->DrawText(strLine1,
				-1,
				rcDraw,
				DT_LEFT | DT_SINGLELINE | DT_NOPREFIX | DT_NOCLIP | DT_TOP | DT_END_ELLIPSIS);
		}

		// �t�H���g��߂�
		if (pOriginalFont!=NULL) {
			pDC->SelectObject(pOriginalFont);
			pOriginalFont = NULL;
		}

		//--- 2�s�ڂ̕`��

		// �p�^�[���̕ϊ�
		CString strLine2 = m_strIntegratedLinePattern2;
		bEmpty = true;
		if (m_strIntegratedLinePattern2.Find(L"%1")>=0 && !strTarget1.IsEmpty()) {
			bEmpty = false;
		}
		strLine2.Replace(L"%1", strTarget1);
		if (m_strIntegratedLinePattern2.Find(L"%2")>=0 && !strTarget2.IsEmpty()) {
			bEmpty = false;
		}
		strLine2.Replace(L"%2", strTarget2);
		if (m_strIntegratedLinePattern2.Find(L"%3")>=0 && !strTarget3.IsEmpty()) {
			bEmpty = false;
		}
		strLine2.Replace(L"%3", strTarget3);
		if (bEmpty) {
			strLine2 = L"";
		}

		// <small>...</small> ������Ώ���������
		// TODO �w�肳�ꂽ�͈͂�������������悤�ɁB�Ƃ肠�����S�̂������������Ⴄ
		if (strLine2.Find(L"<small>")>=0 && strLine2.Find(L"</small>")>=0) {
			strLine2.Replace(L"<small>", L"");
			strLine2.Replace(L"</small>", L"");
			pOriginalFont = pDC->SelectObject(&theApp.m_fontSmall);
		}
		// <big>...</big> ������Α傫������
		// TODO �w�肳�ꂽ�͈͂����傫������悤�ɁB�Ƃ肠�����S�̂�傫�������Ⴄ
		else if (strLine2.Find(L"<big>")>=0 && strLine2.Find(L"</big>")>=0) {
			strLine2.Replace(L"<big>", L"");
			strLine2.Replace(L"</big>", L"");
			pOriginalFont = pDC->SelectObject(&theApp.m_fontBig);
		}

		// ������̍Ōオ"(����)"�ŏI����Ă���Ε�������
		CString strLine2Left = strLine2;
		CString strLine2Right = L"";
		mixi::ParserUtil::SepalateCountString( strLine2 , strLine2Left , strLine2Right );

		//MZ3_TRACE( L"��s�ځF%s>��%s��%s��%d\n" , strLine2 , strLine2Left ,strLine2Right , iSepPos );

		// �t�H���g�̍����擾
		LOGFONT lf;
		GetFont()->GetLogFont( &lf );
		// px�l�ɐ��K��
//		int lfHeightPx = lf.lfHeight < 0 ? -lf.lfHeight : theApp.pt2px(lf.lfHeight);
//		MZ3LOGGER_DEBUG(util::FormatString(
//			L"CBodyListCtrl::DrawItem(), %s, lfHeight : %d, lfHeightPx : %d, lfWidth : %d", 
//			(LPCTSTR)lf.lfFaceName,
//			(int)lf.lfHeight, (int)lfHeightPx,
//			(int)lf.lfWidth));

		CSize charSize = pDC->GetTextExtent(CString(L"��"));
		int lfHeightPx = charSize.cy;

		// �`��
		rcDraw = rcAllLabels;
		rcDraw.top    += lfHeightPx +theApp.pt2px(INTEGRATED_MODE_STYLE::EACH_LINE_MARGIN_PT);
		rcDraw.left   += INTEGRATED_MODE_STYLE::OTHER_LINE_MARGIN_LEFT;
		// ���E�����`��
		csDrawRight = pDC->GetOutputTextExtent( strLine2Right );
		csDrawAllText = pDC->GetOutputTextExtent( strLine2 );
		if( csDrawAllText.cx > rcDraw.Width() && csDrawRight.cx < rcDraw.Width() ) {
			// ��ɕ�������������𗼒[�ɕ����ĕ`�悷��
			// ����������͉E�������񕪂��������̈�ɕ`�悷��
			CRect rcDrawLeft( rcDraw );
			rcDrawLeft.right -= csDrawRight.cx; 
			pDC->DrawText(strLine2Left,
				-1,
				rcDrawLeft,
				DT_SINGLELINE | DT_NOPREFIX | DT_NOCLIP | DT_BOTTOM | DT_LEFT | DT_END_ELLIPSIS);
			// �E��������͉E�񂹂ŕ`�悷��
			pDC->DrawText(strLine2Right,
				-1,
				rcDraw,
				DT_SINGLELINE | DT_NOPREFIX | DT_NOCLIP | DT_BOTTOM | DT_RIGHT | DT_END_ELLIPSIS);

		} else {
			// �E��������̕����`��̈���L���Ȃ炵�傤���Ȃ��̂ł��̂܂܍��l�߂ŕ`�悷��
			pDC->DrawText(strLine2,
				-1,
				rcDraw,
				DT_SINGLELINE | DT_NOPREFIX | DT_NOCLIP | DT_BOTTOM | DT_LEFT | DT_END_ELLIPSIS);
		}

		// �t�H���g��߂�
		if (pOriginalFont!=NULL) {
			pDC->SelectObject(pOriginalFont);
			pOriginalFont = NULL;
		}

	} else {
		// �񓝍��J�������[�h�̏ꍇ�̕`�揈��

		// ��1�J����
		// ������̍Ōオ"(����)"�ŏI����Ă���Ε�������
		CString strTarget1Left = strTarget1;
		CString strTarget1Right = L"";
		mixi::ParserUtil::SepalateCountString( strTarget1 , strTarget1Left , strTarget1Right );

		// ���E�����`��
		CSize csLabelRight = pDC->GetOutputTextExtent( strTarget1Right );
		CSize csLabelAllText = pDC->GetOutputTextExtent( strTarget1 );
		if( csLabelAllText.cx > rcLabel.Width() && csLabelRight.cx < rcLabel.Width() ) {
			// ��ɕ�������������𗼒[�ɕ����ĕ`�悷��
			// ����������͉E�������񕪂��������̈�ɕ`�悷��
			CRect rcLabelLeft( rcLabel );
			rcLabelLeft.right -= csLabelRight.cx ; 
			pDC->DrawText(strTarget1Left,
				-1,
				rcLabelLeft,
				DT_LEFT | DT_SINGLELINE | DT_NOPREFIX | DT_NOCLIP | DT_TOP | DT_END_ELLIPSIS);
			// �E��������͉E�񂹂ŕ`�悷��
			pDC->DrawText(strTarget1Right,
				-1,
				rcLabel,
				DT_RIGHT | DT_SINGLELINE | DT_NOPREFIX | DT_NOCLIP | DT_TOP | DT_END_ELLIPSIS);
		} else {
			// �E��������̕����`��̈���L���Ȃ炵�傤���Ȃ��̂ł��̂܂܍��l�߂ŕ`�悷��
			pDC->DrawText(strTarget1,
				-1,
				rcLabel,
				DT_LEFT | DT_SINGLELINE | DT_NOPREFIX | DT_NOCLIP | DT_VCENTER | DT_END_ELLIPSIS);
		}

		// ��2�J�����ȍ~�̕`��
		LV_COLUMN lvc;
		lvc.mask = LVCF_FMT | LVCF_WIDTH;
		int nSubColumnStartX = 0;	// �e�J�����̗̈�J�n�ʒu(lvc.cx�����₷)
		if (this->GetColumn(0, &lvc)) {
			// ��1�J�����̕������Z
			nSubColumnStartX += lvc.cx;
		}
		for (int nColumn = 1; this->GetColumn(nColumn, &lvc); nColumn++) {
			LPCTSTR pszText = L"";
			switch (nColumn) {
			case 1:	
				pszText = strTarget2;
				break;
			case 2:	
				pszText = strTarget3;
				break;
			}

			// �� N �J����
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
			rcSubItem.left  = nSubColumnStartX;
			rcSubItem.right = rcSubItem.left + lvc.cx;

			CRect rcDraw = rcSubItem;
//			MZ3_TRACE(L"��[%d], w[%d]\n", nColumn, rcDraw.Width());
			if (rcDraw.Width()>=1) {
				// �`��̈悪1�ȏ�Ȃ̂ŕ`�悷��
				rcDraw.left  += COLUMN_MODE_STYLE::OTHER_MARGIN_LEFT;
				rcDraw.right -= COLUMN_MODE_STYLE::OTHER_MARGIN_RIGHT;

				pDC->DrawText(pszText,
					-1,
					rcDraw,
					nJustify | DT_SINGLELINE | DT_NOPREFIX | DT_NOCLIP | DT_VCENTER | DT_END_ELLIPSIS);
			}

			// �̈�J�n�ʒu�̍X�V
			nSubColumnStartX += lvc.cx;
		}
	}

	// �F��߂�
	if (clrTextSave != (COLORREF)-1) {
		clrTextSave = pDC->SetTextColor(clrTextSave);
	}
	if (clrBkSave != (COLORREF)-1) {
		clrBkSave = pDC->SetBkColor(clrBkSave);
	}

	// ���t��؂�o�[(���t�ύX��)�̕`��
	// ���݂̗v�f (nItem) �̏㑤�ɐ�������
	// ���菈��
	bool bDrawDayBreakBar = false;
	if (pCategory!=NULL) {
		CMixiDataList& list = pCategory->GetBodyList();
		if (list.size()>(size_t)nItem && (size_t)nItem<list.size()-1) {	// �ŏI�v�f�łȂ�����
			// ���t�擾
			CTime t0 = list[nItem  ].GetDateRaw();
			CTime t1 = list[nItem+1].GetDateRaw();
			if (t0.GetYear()  != t1.GetYear() ||
				t0.GetMonth() != t1.GetMonth() ||
				t0.GetDay()   != t1.GetDay())
			{
				// ���t���قȂ�
				bDrawDayBreakBar = true;
			}
		}
	}
	// �`�揈��
	if (bDrawDayBreakBar) {
		COLORREF clrDayBreakBar = theApp.m_skininfo.clrMainBodyListDayBreakLine;
		CPen penDayBreakBar(PS_SOLID, 1, clrDayBreakBar);

		CPen* pOldPen = pDC->SelectObject(&penDayBreakBar);

		pDC->MoveTo(rcAllLabels.left,  rcAllLabels.bottom-1);
		pDC->LineTo(rcAllLabels.right, rcAllLabels.bottom-1);

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

//sw_draw4.stop();

	// �x���`�}�[�N���ʏo��
	/*
	MZ3LOGGER_DEBUG(
		util::FormatString(L"DrawItem[%02d] draw[%dms][%dms][%dms][%dms]",
			nItem,
			sw_draw1.getElapsedMilliSecUntilStoped(),
			sw_draw2.getElapsedMilliSecUntilStoped(),
			sw_draw3.getElapsedMilliSecUntilStoped(),
			sw_draw4.getElapsedMilliSecUntilStoped()
			));
	*/
}

/**
 * �w�b�_�[������ύX����B
 *
 * NULL �Ȃ炻�̃C���f�b�N�X�̕����͕ύX���Ȃ��B
 */
void CBodyListCtrl::SetHeader(LPCTSTR col1, LPCTSTR col2, LPCTSTR col3)
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

	if( col3 != NULL ) {
		this->GetHeaderCtrl()->GetItem(2, &hdi);
		wcscpy(hdi.pszText, col3);
		this->GetHeaderCtrl()->SetItem(2, &hdi);
	}
}


BOOL CBodyListCtrl::OnEraseBkgnd(CDC* pDC)
{
	MZ3_TRACE( L"CBodyListCtrl::OnEraseBkgnd()\n");

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

/**
 * �e���ڂ̃T�C�Y�v�Z
 */
void CBodyListCtrl::MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct)
{
//	LOGFONT lf;
//	GetFont()->GetLogFont( &lf );

//	MZ3LOGGER_DEBUG(util::FormatString(L"CBodyListCtrl::MeasureItem(), lfHeight : %d, itemID : %d", 
//		(int)lf.lfHeight, (int)lpMeasureItemStruct->itemID));

	// px�l�ɕϊ�
//	int lfHeightPx = lf.lfHeight < 0 ? -lf.lfHeight : theApp.pt2px(lf.lfHeight);
	CDC* pDC = GetDC();
	CFont* pOldFont = pDC->SelectObject(&theApp.m_font);
	CSize charSize = pDC->GetTextExtent(CString(L"��"));
	pDC->SelectObject(pOldFont);
	ReleaseDC(pDC);

	int lfHeightPx = charSize.cy;
//	MZ3LOGGER_DEBUG(util::FormatString(L"CBodyListCtrl::MeasureItem(), lfHeight : %d, lfHeightPx : %d", 
//		(int)lf.lfHeight, (int)lfHeightPx));


	if (theApp.m_optionMng.m_bBodyListIntegratedColumnMode) {
		// �����J�������[�h�F������N�{����
		lpMeasureItemStruct->itemHeight = lfHeightPx*2 +theApp.pt2px(INTEGRATED_MODE_STYLE::BOX_MARGIN_BOTTOM_PT);
	} else {
		// �J�������[�h
		lpMeasureItemStruct->itemHeight = lfHeightPx   +theApp.pt2px(COLUMN_MODE_STYLE::BOX_MARGIN_BOTTOM_PT);
	}

//	MZ3_TRACE(L"CBodyListCtrl::MeasureItem(), itemHeight : %d\n", lpMeasureItemStruct->itemHeight);
}

LRESULT CBodyListCtrl::OnSetFont(WPARAM wParam, LPARAM lParam)
{
	MZ3_TRACE(L"CBodyListCtrl::OnSetFont()\n");

	LRESULT res =  Default();

	CRect rc;
	GetWindowRect( &rc );

	WINDOWPOS wp;
	wp.hwnd = m_hWnd;
	wp.cx = rc.Width();
	wp.cy = rc.Height();
	wp.flags = SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOOWNERZORDER | SWP_NOZORDER;
	SendMessage( WM_WINDOWPOSCHANGED, 0, (LPARAM)&wp );

	return res;
}

void CBodyListCtrl::OnLButtonUp(UINT nFlags, CPoint point)
{
	bool bDragging = m_bPanDragging || m_bScrollDragging;

	// ��ɐe���Ăяo���A�I��ύX����
	CTouchListCtrl::OnLButtonUp(nFlags, point);

	if (!bDragging) {
		// Twitter�ŃA�C�R���̈�Ȃ���p��ǉ�
		int idx = HitTest(point);
		if (m_iconMode != ICON_MODE_NONE && point.x < m_iconMode) {
			MZ3_TRACE(L"lbu [%d]\n", idx);

			CCategoryItem* pCategory = theApp.m_pMainView->m_selGroup->getSelectedCategory();
			if (pCategory!=NULL && 0 <= idx && idx < (int)pCategory->m_body.size()) {
				CMixiData* data = &pCategory->m_body[ idx ];

				// �A�C�R���̈�N���b�N�ʒm
				// TODO API ��
				if (data->GetAccessType()==ACCESS_TWITTER_USER) {

					// Lua �֐��Ăяo���ŉ�����
					theApp.MyLuaExecute(L"twitter.on_twitter_reply()");
				}
			}
		}
	}
}
