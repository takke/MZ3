// OptionTabTwitter.cpp : �����t�@�C��
//

#include "stdafx.h"
#include "MZ3.h"
#include "OptionTabTwitter.h"


// COptionTabTwitter �_�C�A���O

IMPLEMENT_DYNAMIC(COptionTabTwitter, CPropertyPage)

COptionTabTwitter::COptionTabTwitter()
	: CPropertyPage(COptionTabTwitter::IDD)
{

}

COptionTabTwitter::~COptionTabTwitter()
{
}

void COptionTabTwitter::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(COptionTabTwitter, CPropertyPage)
END_MESSAGE_MAP()


// COptionTabTwitter ���b�Z�[�W �n���h��

BOOL COptionTabTwitter::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	// *MZ3* �}�[�N
	CheckDlgButton( IDC_ADD_SOURCE_TEXT_ON_POST_CHECK, theApp.m_optionMng.m_bAddSourceTextOnTwitterPost ? BST_CHECKED : BST_UNCHECKED );
	SetDlgItemText( IDC_ADD_SOURCE_TEXT_ON_POST_CHECK, L"������ *" MZ3_APP_NAME L"* �}�[�N������" );

	return TRUE;  // return TRUE unless you set the focus to a control
	// ��O : OCX �v���p�e�B �y�[�W�͕K�� FALSE ��Ԃ��܂��B
}

void COptionTabTwitter::OnOK()
{
	// *MZ3* �}�[�N
	theApp.m_optionMng.m_bAddSourceTextOnTwitterPost = IsDlgButtonChecked( IDC_ADD_SOURCE_TEXT_ON_POST_CHECK ) == BST_CHECKED ? true : false;

	CPropertyPage::OnOK();
}
