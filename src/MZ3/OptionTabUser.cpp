// UserTab.cpp : �����t�@�C��
//

#include "stdafx.h"
#include "MZ3.h"
#include "OptionTabUser.h"


// COptionTabUser �_�C�A���O

IMPLEMENT_DYNAMIC(COptionTabUser, CPropertyPage)

COptionTabUser::COptionTabUser()
	: CPropertyPage(COptionTabUser::IDD)
{

}

COptionTabUser::~COptionTabUser()
{
}

void COptionTabUser::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(COptionTabUser, CPropertyPage)
END_MESSAGE_MAP()


// COptionTabUser ���b�Z�[�W �n���h��

BOOL COptionTabUser::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	((CEdit*)GetDlgItem(IDC_LOGIN_MAIL_EDIT))->SetWindowText(theApp.m_loginMng.GetEmail());
	((CEdit*)GetDlgItem(IDC_LOGIN_PASSWORD_EDIT))->SetWindowText(theApp.m_loginMng.GetPassword());

	return TRUE;  // return TRUE unless you set the focus to a control
	// ��O : OCX �v���p�e�B �y�[�W�͕K�� FALSE ��Ԃ��܂��B
}

void COptionTabUser::OnOK()
{
	CString buf;
	((CEdit*)GetDlgItem(IDC_LOGIN_MAIL_EDIT))->GetWindowText(buf);
	theApp.m_loginMng.SetEmail(buf);
	((CEdit*)GetDlgItem(IDC_LOGIN_PASSWORD_EDIT))->GetWindowText(buf);
	theApp.m_loginMng.SetPassword(buf);

	theApp.m_loginMng.Write();

	CPropertyPage::OnOK();
}
