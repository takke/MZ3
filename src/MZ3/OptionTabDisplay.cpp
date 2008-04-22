/*
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
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

	// �t�H���g�i��j
	SetDlgItemText( IDC_FONT_BIG_EDIT, util::int2str(theApp.m_optionMng.m_fontHeightBig) );
	// �t�H���g�i���j
	SetDlgItemText( IDC_FONT_MEDIUM_EDIT, util::int2str(theApp.m_optionMng.m_fontHeightMedium) );
	// �t�H���g�i���j
	SetDlgItemText( IDC_FONT_SMALL_EDIT, util::int2str(theApp.m_optionMng.m_fontHeightSmall) );

	// �_�E�����[�h��̊m�F���
	CheckDlgButton( IDC_USE_RUN_CONFIRM_DLG_CHECK, theApp.m_optionMng.m_bUseRunConfirmDlg ? BST_CHECKED : BST_UNCHECKED );

	return TRUE;
}

void COptionTabDisplay::OnOK()
{
	// �w�iOn/Off
	theApp.m_optionMng.SetUseBgImage(
		((CButton*)GetDlgItem(IDC_USE_BGIMAGE_CHECK))->GetCheck() == BST_CHECKED ? TRUE : FALSE );

	// �t�H���g�i��j
	{
		CString s;
		GetDlgItemText( IDC_FONT_BIG_EDIT, s );
		theApp.m_optionMng.m_fontHeightBig = option::Option::normalizeFontSize( _wtoi(s) );;
	}
	// �t�H���g�i���j
	{
		CString s;
		GetDlgItemText( IDC_FONT_MEDIUM_EDIT, s );
		theApp.m_optionMng.m_fontHeightMedium = option::Option::normalizeFontSize( _wtoi(s) );
	}
	// �t�H���g�i���j
	{
		CString s;
		GetDlgItemText( IDC_FONT_SMALL_EDIT, s );
		theApp.m_optionMng.m_fontHeightSmall = option::Option::normalizeFontSize( _wtoi(s) );
	}

	// �_�E�����[�h��̊m�F���
	theApp.m_optionMng.m_bUseRunConfirmDlg = IsDlgButtonChecked( IDC_USE_RUN_CONFIRM_DLG_CHECK ) == BST_CHECKED ? true : false;

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
		theApp.m_root.initForTopPage(dlg.m_initType);
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