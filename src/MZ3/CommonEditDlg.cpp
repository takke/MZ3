/*
 * Copyright (c) MZ3 Project Team All rights reserved.
 * Code licensed under the BSD License:
 * http://www.mz3.jp/license.txt
 */
// CommonEditDlg.cpp : �����t�@�C��
//

#include "stdafx.h"
#include "MZ3.h"
#include "CommonEditDlg.h"


// CCommonEditDlg �_�C�A���O

IMPLEMENT_DYNAMIC(CCommonEditDlg, CDialog)

CCommonEditDlg::CCommonEditDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CCommonEditDlg::IDD, pParent)
	, mc_strEdit(_T(""))
	, mc_strMessage(_T(""))
	, mc_strEdit2(_T(""))
{

}

CCommonEditDlg::~CCommonEditDlg()
{
}

void CCommonEditDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, mc_strEdit);
	DDX_Text(pDX, IDC_EDIT2, mc_strEdit2);
	DDX_Text(pDX, IDC_MESSAGE_STATIC, mc_strMessage);
}


BEGIN_MESSAGE_MAP(CCommonEditDlg, CDialog)
	ON_BN_CLICKED(IDOK, &CCommonEditDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CCommonEditDlg ���b�Z�[�W �n���h��

void CCommonEditDlg::OnBnClickedOk()
{
	UpdateData();

	OnOK();
}

void CCommonEditDlg::SetTitle(LPCTSTR szTitle)
{
	m_strTitle = szTitle;
}

void CCommonEditDlg::SetMessage(LPCTSTR szMessage)
{
	mc_strMessage = szMessage;
}

BOOL CCommonEditDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// mc_strEdit2 �����ݒ�ł���� IDC_EDIT2 ���\����
	if (mc_strEdit2.IsEmpty()) {
		GetDlgItem(IDC_EDIT2)->ShowWindow( SW_HIDE );
	}

	SetWindowText( m_strTitle );

	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// ��O : OCX �v���p�e�B �y�[�W�͕K�� FALSE ��Ԃ��܂��B
}
