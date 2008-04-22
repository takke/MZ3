/*
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */
// ChooseClientTypeDlg.cpp : �����t�@�C��
//

#include "stdafx.h"
#include "MZ3.h"
#include "ChooseClientTypeDlg.h"


// CChooseClientTypeDlg �_�C�A���O

IMPLEMENT_DYNAMIC(CChooseClientTypeDlg, CDialog)

CChooseClientTypeDlg::CChooseClientTypeDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CChooseClientTypeDlg::IDD, pParent)
{

}

CChooseClientTypeDlg::~CChooseClientTypeDlg()
{
}

void CChooseClientTypeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CChooseClientTypeDlg, CDialog)
END_MESSAGE_MAP()


// CChooseClientTypeDlg ���b�Z�[�W �n���h��

BOOL CChooseClientTypeDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// �����I��
	CheckDlgButton( IDC_SERVICE_TYPE_MIXI_CHECK, BST_CHECKED );
	CheckDlgButton( IDC_SERVICE_TYPE_TWITTER_CHECK, BST_CHECKED );

	return TRUE;  // return TRUE unless you set the focus to a control
	// ��O : OCX �v���p�e�B �y�[�W�͕K�� FALSE ��Ԃ��܂��B
}

void CChooseClientTypeDlg::OnOK()
{
	// �I����Ԃ̎擾
	m_initType.bUseMixi    = (IsDlgButtonChecked( IDC_SERVICE_TYPE_MIXI_CHECK ) == BST_CHECKED);
	m_initType.bUseTwitter = (IsDlgButtonChecked( IDC_SERVICE_TYPE_TWITTER_CHECK ) == BST_CHECKED);

	// �����`�F�b�N����Ă��Ȃ���΃G���[
	if (m_initType.bUseMixi==false &&
		m_initType.bUseTwitter==false)
	{
		MessageBox(L"�I�����Ă�������");
		return;
	}

	CDialog::OnOK();
}
