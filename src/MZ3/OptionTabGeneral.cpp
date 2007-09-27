// OptionTabMInfo.cpp : �����t�@�C��
//

#include "stdafx.h"
#include "MZ3.h"
#include "OptionTabGeneral.h"
#include "ReportView.h"
#include "ChooseFontDlg.h"
#include "util.h"

// this file is not used in smartphone edition
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

	// ���p����
	SetDlgItemText( IDC_QUOTE_MARK_EDIT, theApp.m_optionMng.GetQuoteMark() );

	// mixi ���o�C���Ή�
	CheckDlgButton( IDC_CONVERT_URL_FOR_MIXI_MOBILE_CHECK, 
		theApp.m_optionMng.m_bConvertUrlForMixiMobile ? BST_CHECKED : BST_UNCHECKED );

	return TRUE;
}

void COptionTabGeneral::OnOK()
{
	// �N�����̐V���`�F�b�N
	theApp.m_optionMng.SetBootCheckMnC(
		IsDlgButtonChecked( IDC_BOOT_MNC_CHECK ) == BST_CHECKED ? TRUE : FALSE );

	// ���p����
	CString mark;
	GetDlgItemText( IDC_QUOTE_MARK_EDIT, mark );
	theApp.m_optionMng.SetQuoteMark( mark );

	// mixi ���o�C���Ή�
	theApp.m_optionMng.m_bConvertUrlForMixiMobile = 
		IsDlgButtonChecked( IDC_CONVERT_URL_FOR_MIXI_MOBILE_CHECK ) == BST_CHECKED ? true : false;

	CPropertyPage::OnOK();
}

#endif