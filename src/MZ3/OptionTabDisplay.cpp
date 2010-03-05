/*
 * Copyright (c) MZ3 Project Team All rights reserved.
 * Code licensed under the BSD License:
 * http://www.mz3.jp/license.txt
 */
// OptionTabDisplay.cpp : �����t�@�C��
//

#include "stdafx.h"
#include "MZ3.h"
#include "OptionTabDisplay.h"
#include "ReportView.h"
#include "ChooseFontDlg.h"
#include "util.h"
#include "ChooseClientTypeDlg.h"

#ifndef SMARTPHONE2003_UI_MODEL

// COptionTabDisplay �_�C�A���O

IMPLEMENT_DYNAMIC(COptionTabDisplay, CPropertyPage)

COptionTabDisplay::COptionTabDisplay()
	: CPropertyPage(COptionTabDisplay::IDD)
{

}

COptionTabDisplay::~COptionTabDisplay()
{
}

void COptionTabDisplay::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(COptionTabDisplay, CPropertyPage)
	ON_BN_CLICKED(IDC_RESET_TAB_BUTTON, &COptionTabDisplay::OnBnClickedResetTabButton)
	ON_BN_CLICKED(IDC_SELECT_FONT_BUTTON, &COptionTabDisplay::OnBnClickedSelectFontButton)
END_MESSAGE_MAP()


// COptionTabDisplay ���b�Z�[�W �n���h��

BOOL COptionTabDisplay::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	// �w�iOn/Off
	CheckDlgButton( IDC_USE_BGIMAGE_CHECK, theApp.m_optionMng.IsUseBgImage() ? BST_CHECKED : BST_UNCHECKED );

	// �_�E�����[�h��̊m�F���
	CheckDlgButton( IDC_USE_RUN_CONFIRM_DLG_CHECK, theApp.m_optionMng.m_bUseRunConfirmDlg ? BST_CHECKED : BST_UNCHECKED );

	// �ʒm�̈�ɐV��TL���o���[���\������
	CheckDlgButton( IDC_SHOW_BALLOON_ON_NEW_TL_CHECK, theApp.m_optionMng.m_bShowBalloonOnNewTL ? BST_CHECKED : BST_UNCHECKED );
	CWnd* pItem = NULL;
#ifdef WINCE
	pItem = GetDlgItem( IDC_SHOW_BALLOON_ON_NEW_TL_CHECK );
	if (pItem != NULL) {
		pItem->ShowWindow(SW_HIDE);
	}
#endif

	// ClearType
	CheckDlgButton( IDC_USE_CLEAR_TYPE_FONT_CHECK, theApp.m_optionMng.m_bUseClearTypeFont ? BST_CHECKED : BST_UNCHECKED );

	// �����܂ŗp�ӂ������ǁA�����Ă��̏ꍇ�̓N���A�^�C�vON�Ŗ��Ȃ����ߔ�\���ɂ��Ă����B
	pItem = GetDlgItem( IDC_USE_CLEAR_TYPE_FONT_CHECK );
	if (pItem != NULL) {
		pItem->ShowWindow(SW_HIDE);
	}

	return TRUE;
}

void COptionTabDisplay::OnOK()
{
	// �w�iOn/Off
	theApp.m_optionMng.SetUseBgImage(
		((CButton*)GetDlgItem(IDC_USE_BGIMAGE_CHECK))->GetCheck() == BST_CHECKED ? TRUE : FALSE );

	// �_�E�����[�h��̊m�F���
	theApp.m_optionMng.m_bUseRunConfirmDlg = IsDlgButtonChecked( IDC_USE_RUN_CONFIRM_DLG_CHECK ) == BST_CHECKED ? true : false;

	// �ʒm�̈�ɐV��TL���o���[���\������
	theApp.m_optionMng.m_bShowBalloonOnNewTL = IsDlgButtonChecked( IDC_SHOW_BALLOON_ON_NEW_TL_CHECK ) == BST_CHECKED ? true : false;

	// ClearType
	theApp.m_optionMng.m_bUseClearTypeFont = IsDlgButtonChecked( IDC_USE_CLEAR_TYPE_FONT_CHECK ) == BST_CHECKED ? true : false;

	CPropertyPage::OnOK();
}

void COptionTabDisplay::OnBnClickedResetTabButton()
{
	LPCTSTR msg =  L"�^�u�����Z�b�g���Ă���낵���ł����H\n�i���������" MZ3_APP_NAME L"���I�����܂��j";
	if( MessageBox( msg, 0, MB_YESNO | MB_ICONQUESTION ) != IDYES ) {
		return;
	}

	// ������
//	theApp.m_root.initForTopPage();
	// �N���C�A���g�I����ʂ�\��
	CChooseClientTypeDlg dlg;
	if (dlg.DoModal()==IDOK) {
		// �ݒ肳�ꂽ���e�ŏ�����
		theApp.m_root.initForTopPage(theApp.m_accessTypeInfo, dlg.m_initType);
	} else {
		return;
	}

	// �O���[�v��`�t�@�C���̕ۑ�
	theApp.SaveGroupData();

	// �ċN��
	// �E�E�E�ł��Ȃ��̂Ń��b�Z�[�W��\�����ďI���B
	MessageBox( L"������������������������" MZ3_APP_NAME L"���I�����܂�" );
	theApp.m_pReportView->SaveIndex();
	ShowWindow(SW_HIDE);
	AfxGetMainWnd()->PostMessage(WM_CLOSE);
}

/// �t�H���g�ύX
void COptionTabDisplay::OnBnClickedSelectFontButton()
{
	CChooseFontDlg dlg;
	if( dlg.DoModal() == IDOK ) {
		theApp.m_optionMng.m_fontFace = dlg.m_strSelectedFont;
	}
}

#endif