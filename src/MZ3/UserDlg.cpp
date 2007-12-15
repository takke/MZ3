/*
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */
// UserDlg.cpp : �����t�@�C��
//

#include "stdafx.h"
#include "MZ3.h"
#include "UserDlg.h"


// CUserDlg �_�C�A���O

IMPLEMENT_DYNAMIC(CUserDlg, CDialog)

CUserDlg::CUserDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CUserDlg::IDD, pParent)
{

}

CUserDlg::~CUserDlg()
{
}

void CUserDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CUserDlg, CDialog)
	ON_BN_CLICKED(IDOK, &CUserDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CUserDlg ���b�Z�[�W �n���h��

BOOL CUserDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	((CEdit*)GetDlgItem(IDC_LOGIN_MAIL_EDIT))->SetWindowText(theApp.m_loginMng.GetEmail());
	((CEdit*)GetDlgItem(IDC_LOGIN_PASSWORD_EDIT))->SetWindowText(theApp.m_loginMng.GetPassword());

	return TRUE;  // return TRUE unless you set the focus to a control
	// ��O : OCX �v���p�e�B �y�[�W�͕K�� FALSE ��Ԃ��܂��B
}

void CUserDlg::OnBnClickedOk()
{
	CString buf;
	((CEdit*)GetDlgItem(IDC_LOGIN_MAIL_EDIT))->GetWindowText(buf);
	theApp.m_loginMng.SetEmail(buf);
	((CEdit*)GetDlgItem(IDC_LOGIN_PASSWORD_EDIT))->GetWindowText(buf);
	theApp.m_loginMng.SetPassword(buf);

	theApp.m_loginMng.Write();

	OnOK();
}
