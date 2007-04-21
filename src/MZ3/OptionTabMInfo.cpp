// OptionTabMInfo.cpp : �����t�@�C��
//

#include "stdafx.h"
#include "MZ3.h"
#include "OptionTabMInfo.h"
#include "ReportView.h"
#include "ChooseFontDlg.h"
#include "util.h"

// COptionTabMInfo �_�C�A���O

IMPLEMENT_DYNAMIC(COptionTabMInfo, CPropertyPage)

COptionTabMInfo::COptionTabMInfo()
	: CPropertyPage(COptionTabMInfo::IDD)
{

}

COptionTabMInfo::~COptionTabMInfo()
{
}

void COptionTabMInfo::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(COptionTabMInfo, CPropertyPage)
END_MESSAGE_MAP()

// COptionTabMInfo ���b�Z�[�W �n���h��
BOOL COptionTabMInfo::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	// �N�����̐V���`�F�b�N
	((CButton*)GetDlgItem(IDC_BOOT_MNC_CHECK))->SetCheck(
		theApp.m_optionMng.IsBootCheckMnC() ? BST_CHECKED : BST_UNCHECKED );

	return TRUE;
}

void COptionTabMInfo::OnOK()
{
	// �N�����̐V���`�F�b�N
	theApp.m_optionMng.SetBootCheckMnC(
		((CButton*)GetDlgItem(IDC_BOOT_MNC_CHECK))->GetCheck() == BST_CHECKED ? TRUE : FALSE );

	CPropertyPage::OnOK();
}

