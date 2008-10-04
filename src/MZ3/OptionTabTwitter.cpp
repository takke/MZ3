/*
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */
// OptionTabTwitter.cpp : �����t�@�C��
//

#include "stdafx.h"
#include "MZ3.h"
#include "OptionTabTwitter.h"
#include "util_base.h"

#ifndef SMARTPHONE2003_UI_MODEL

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

	// �t�b�^�[�}�[�N
	CheckDlgButton( IDC_ADD_SOURCE_TEXT_ON_POST_CHECK, 
					theApp.m_optionMng.m_bAddSourceTextOnTwitterPost ? BST_CHECKED : BST_UNCHECKED );
//	SetDlgItemText( IDC_ADD_SOURCE_TEXT_ON_POST_CHECK, L"������ *" MZ3_APP_NAME L"* �}�[�N������" );

	// �t�b�^�[�}�[�N������
	SetDlgItemText( IDC_TWITTER_POST_FOOTER_TEXT_EDIT, theApp.m_optionMng.m_strTwitterPostFotterText );

	// �X�e�[�^�X�s��
	{
		CComboBox* pComboBox = (CComboBox*)GetDlgItem(IDC_STATUS_LINE_COUNT_COMBO);
		int selectedNumber = theApp.m_optionMng.m_nTwitterStatusLineCount;
		int items[] = { 2, 3, -1 };	// -1 : terminater
		for (int i=0; items[i]!=-1; i++) {
			int n = items[i];
			int idx = pComboBox->InsertString(i, util::FormatString(L"%d", n));
			pComboBox->SetItemData(idx, n);
			if (selectedNumber==n) {
				pComboBox->SetCurSel(i);
			}
		}
	}

	// �擾�y�[�W��
	{
		CComboBox* pComboBox = (CComboBox*)GetDlgItem(IDC_TWITTER_GET_PAGE_COUNT_COMBO);
		int selectedNumber = theApp.m_optionMng.m_nTwitterGetPageCount;
		int items[] = { 1, 2, 3, 4, 5, -1 };	// -1 : terminater
		for (int i=0; items[i]!=-1; i++) {
			int n = items[i];
			int idx = pComboBox->InsertString(i, util::FormatString(L"%d", n));
			pComboBox->SetItemData(idx, n);
			if (selectedNumber==n) {
				pComboBox->SetCurSel(i);
			}
		}
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// ��O : OCX �v���p�e�B �y�[�W�͕K�� FALSE ��Ԃ��܂��B
}

void COptionTabTwitter::OnOK()
{
	// �t�b�^�[�}�[�N
	theApp.m_optionMng.m_bAddSourceTextOnTwitterPost = IsDlgButtonChecked( IDC_ADD_SOURCE_TEXT_ON_POST_CHECK ) == BST_CHECKED ? true : false;

	// �t�b�^�[�}�[�N������
	GetDlgItemText(IDC_TWITTER_POST_FOOTER_TEXT_EDIT, theApp.m_optionMng.m_strTwitterPostFotterText);

	// �X�e�[�^�X�s��
	{
		CComboBox* pComboBox = (CComboBox*)GetDlgItem(IDC_STATUS_LINE_COUNT_COMBO);
		int idx = pComboBox->GetCurSel();
		if (idx>=0) {
			int n = pComboBox->GetItemData(idx);
			theApp.m_optionMng.m_nTwitterStatusLineCount = option::Option::normalizeTwitterStatusLineCount(n);
		}
	}

	// �ő�擾�y�[�W��
	{
		CComboBox* pComboBox = (CComboBox*)GetDlgItem(IDC_TWITTER_GET_PAGE_COUNT_COMBO);
		int idx = pComboBox->GetCurSel();
		if (idx>=0) {
			int n = pComboBox->GetItemData(idx);
			theApp.m_optionMng.m_nTwitterGetPageCount = option::Option::normalizeTwitterGetPageCount(n);
		}
	}

	CPropertyPage::OnOK();
}

#endif