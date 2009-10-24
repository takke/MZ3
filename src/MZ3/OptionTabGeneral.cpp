/*
 * Copyright (c) MZ3 Project Team All rights reserved.
 * Code licensed under the BSD License:
 * http://www.mz3.jp/license.txt
 */
// OptionTabMInfo.cpp : �����t�@�C��
//

#include "stdafx.h"
#include "MZ3.h"
#include "OptionTabGeneral.h"
#include "ReportView.h"
#include "util.h"

#ifndef SMARTPHONE2003_UI_MODEL

// COptionTabGeneral �_�C�A���O

IMPLEMENT_DYNAMIC(COptionTabGeneral, CPropertyPage)

COptionTabGeneral::COptionTabGeneral()
	: CPropertyPage(COptionTabGeneral::IDD)
{

}

COptionTabGeneral::~COptionTabGeneral()
{
}

void COptionTabGeneral::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(COptionTabGeneral, CPropertyPage)
END_MESSAGE_MAP()

// COptionTabGeneral ���b�Z�[�W �n���h��
BOOL COptionTabGeneral::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	// �N�����̐V���`�F�b�N
	CheckDlgButton( IDC_BOOT_MNC_CHECK, theApp.m_optionMng.IsBootCheckMnC() ? BST_CHECKED : BST_UNCHECKED );

	// �o�[�W�����`�F�b�N�ɊJ���ł��܂߂�
	CheckDlgButton( IDC_USE_DEV_VER_CHECK_CHECK, theApp.m_optionMng.m_bUseDevVerCheck ? BST_CHECKED : BST_UNCHECKED );

	// ���p����
	SetDlgItemText( IDC_QUOTE_MARK_EDIT, theApp.m_optionMng.GetQuoteMark() );

	// URL���J���Ƃ��Ɋm�F����
	CheckDlgButton( IDC_CONFIRM_OPEN_URL_CHECK, 
		theApp.m_optionMng.m_bConfirmOpenURL ? BST_CHECKED : BST_UNCHECKED );

	// mixi ���o�C���Ή�
/*	CheckDlgButton( IDC_CONVERT_URL_FOR_MIXI_MOBILE_CHECK, 
		theApp.m_optionMng.m_bConvertUrlForMixiMobile ? BST_CHECKED : BST_UNCHECKED );
#ifndef WINCE
	// Win32 �ł͖���
	GetDlgItem( IDC_CONVERT_URL_FOR_MIXI_MOBILE_CHECK )->EnableWindow( FALSE );
#endif
*/
	return TRUE;
}

void COptionTabGeneral::OnOK()
{
	// �N�����̐V���`�F�b�N
	theApp.m_optionMng.SetBootCheckMnC(
		IsDlgButtonChecked( IDC_BOOT_MNC_CHECK ) == BST_CHECKED ? true : false );

	// �o�[�W�����`�F�b�N�ɊJ���ł��܂߂�
	theApp.m_optionMng.m_bUseDevVerCheck = IsDlgButtonChecked( IDC_USE_DEV_VER_CHECK_CHECK ) == BST_CHECKED ? true : false;

	// ���p����
	CString mark;
	GetDlgItemText( IDC_QUOTE_MARK_EDIT, mark );
	theApp.m_optionMng.SetQuoteMark( mark );

	// URL���J���Ƃ��Ɋm�F����
	theApp.m_optionMng.m_bConfirmOpenURL = 
		IsDlgButtonChecked( IDC_CONFIRM_OPEN_URL_CHECK ) == BST_CHECKED ? true : false;

	// mixi ���o�C���Ή�
/*	theApp.m_optionMng.m_bConvertUrlForMixiMobile = 
		IsDlgButtonChecked( IDC_CONVERT_URL_FOR_MIXI_MOBILE_CHECK ) == BST_CHECKED ? true : false;
*/
	CPropertyPage::OnOK();
}

#endif