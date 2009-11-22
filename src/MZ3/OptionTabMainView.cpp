/*
 * Copyright (c) MZ3 Project Team All rights reserved.
 * Code licensed under the BSD License:
 * http://www.mz3.jp/license.txt
 */
// OptionTabMainView.cpp : �����t�@�C��
//

#include "stdafx.h"
#include "MZ3.h"
#include "OptionTabMainView.h"

#ifndef SMARTPHONE2003_UI_MODEL

// COptionTabMainView �_�C�A���O

IMPLEMENT_DYNAMIC(COptionTabMainView, CPropertyPage)

COptionTabMainView::COptionTabMainView()
	: CPropertyPage(COptionTabMainView::IDD)
{

}

COptionTabMainView::~COptionTabMainView()
{
}

void COptionTabMainView::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(COptionTabMainView, CPropertyPage)
	ON_BN_CLICKED(IDC_SHOW_MINI_IMAGE_CHECK, &COptionTabMainView::OnBnClickedShowMiniImageCheck)
END_MESSAGE_MAP()


// COptionTabMainView ���b�Z�[�W �n���h��

BOOL COptionTabMainView::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	// ���[�U��R�~���j�e�B�̉摜
	CheckDlgButton( IDC_SHOW_MINI_IMAGE_CHECK, theApp.m_optionMng.m_bShowMainViewMiniImage ? BST_CHECKED : BST_UNCHECKED );

	// �摜�̎����擾
	CheckDlgButton( IDC_AUTO_LOAD_MINI_IMAGE_CHECK, theApp.m_optionMng.m_bAutoLoadMiniImage ? BST_CHECKED : BST_UNCHECKED );

	// �g�s�b�N���̃A�C�R���\��
	CheckDlgButton( IDC_SHOW_ICON_CHECK, theApp.m_optionMng.m_bShowMainViewIcon ? BST_CHECKED : BST_UNCHECKED );

	// ���y�C���̃��X�g���P�s�ŕ\������
	CheckDlgButton( IDC_NO_INTEGRATED_MODE_CHECK, theApp.m_optionMng.m_bBodyListIntegratedColumnMode ? BST_UNCHECKED : BST_CHECKED);

	// ��y�C���̃��X�g�N���b�N���Ɏ擾����
	CheckDlgButton( IDC_ONE_CLICK_CATEGORY_FETCH_MODE_CHECK, theApp.m_optionMng.m_bOneClickCategoryFetchMode ? BST_CHECKED : BST_UNCHECKED);

	UpdateControlItemStatus();

	return TRUE;  // return TRUE unless you set the focus to a control
	// ��O : OCX �v���p�e�B �y�[�W�͕K�� FALSE ��Ԃ��܂��B
}

void COptionTabMainView::OnOK()
{
	// ���[�U��R�~���j�e�B�̉摜
	theApp.m_optionMng.m_bShowMainViewMiniImage = IsDlgButtonChecked( IDC_SHOW_MINI_IMAGE_CHECK ) == BST_CHECKED;

	// �摜�̎����擾
	theApp.m_optionMng.m_bAutoLoadMiniImage = IsDlgButtonChecked( IDC_AUTO_LOAD_MINI_IMAGE_CHECK ) == BST_CHECKED;

	// �g�s�b�N���̃A�C�R���\��
	theApp.m_optionMng.m_bShowMainViewIcon = IsDlgButtonChecked( IDC_SHOW_ICON_CHECK ) == BST_CHECKED;

	// ���y�C���̃��X�g���P�s�ŕ\������
	theApp.m_optionMng.m_bBodyListIntegratedColumnMode = IsDlgButtonChecked( IDC_NO_INTEGRATED_MODE_CHECK ) != BST_CHECKED;

	// ��y�C���̃��X�g�N���b�N���Ɏ擾����
	theApp.m_optionMng.m_bOneClickCategoryFetchMode = IsDlgButtonChecked( IDC_ONE_CLICK_CATEGORY_FETCH_MODE_CHECK ) == BST_CHECKED;

	CPropertyPage::OnOK();
}

void COptionTabMainView::OnBnClickedShowMiniImageCheck()
{
	UpdateControlItemStatus();
}

void COptionTabMainView::UpdateControlItemStatus(void)
{
	BOOL bImageCheck = (IsDlgButtonChecked( IDC_SHOW_MINI_IMAGE_CHECK ) == BST_CHECKED) ? TRUE : FALSE;
	if (bImageCheck) {
		GetDlgItem( IDC_AUTO_LOAD_MINI_IMAGE_CHECK )->EnableWindow( TRUE );
	} else {
		GetDlgItem( IDC_AUTO_LOAD_MINI_IMAGE_CHECK )->EnableWindow( FALSE );
	}
}

#endif