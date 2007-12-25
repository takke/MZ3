/*
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
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
	DDX_Control(pDX, IDC_MINI_IMAGE_SIZE_COMBO, mc_comboMiniImageSize);
}


BEGIN_MESSAGE_MAP(COptionTabMainView, CPropertyPage)
	ON_BN_CLICKED(IDC_SHOW_MINI_IMAGE_DLG_CHECK, &COptionTabMainView::OnBnClickedShowMiniImageDlgCheck)
	ON_BN_CLICKED(IDC_SHOW_MINI_IMAGE_CHECK, &COptionTabMainView::OnBnClickedShowMiniImageCheck)
END_MESSAGE_MAP()


// COptionTabMainView ���b�Z�[�W �n���h��

BOOL COptionTabMainView::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	// ���[�U��R�~���j�e�B�̉摜
	CheckDlgButton( IDC_SHOW_MINI_IMAGE_CHECK, theApp.m_optionMng.m_bShowMainViewMiniImage ? BST_CHECKED : BST_UNCHECKED );

	// �ʉ�ʂɕ\��
	CheckDlgButton( IDC_SHOW_MINI_IMAGE_DLG_CHECK, theApp.m_optionMng.m_bShowMainViewMiniImageDlg ? BST_CHECKED : BST_UNCHECKED );

	// WINCE �ł̓J�[�\���ʒu�ɕ\�����Ȃ�
#ifdef WINCE
	// ����
#else
	CheckDlgButton( IDC_SHOW_MINI_IMAGE_DLG_ON_MOUSEOVER_CHECK, theApp.m_optionMng.m_bShowMainViewMiniImageOnMouseOver ? BST_CHECKED : BST_UNCHECKED );
#endif

	// �摜�̎����擾
	CheckDlgButton( IDC_AUTO_LOAD_MINI_IMAGE_CHECK, theApp.m_optionMng.m_bAutoLoadMiniImage ? BST_CHECKED : BST_UNCHECKED );

	// �g�s�b�N���̃A�C�R���\��
	CheckDlgButton( IDC_SHOW_ICON_CHECK, theApp.m_optionMng.m_bShowMainViewIcon ? BST_CHECKED : BST_UNCHECKED );

	// mini��ʃT�C�Y
#ifdef WINCE
	// ����
#else
	int sizes[] = { 25, 50, 75, 100, 125, 150, -1 };
	for (int i=0; sizes[i] != -1; i++) {
		CString s;
		s.Format( L"%dx%d", sizes[i], sizes[i] );
		int idx = mc_comboMiniImageSize.InsertString( i, s );
		mc_comboMiniImageSize.SetItemData( idx, sizes[i] );

		if (sizes[i] == theApp.m_optionMng.m_nMainViewMiniImageSize) {
			mc_comboMiniImageSize.SetCurSel(i);
		}
	}
	if (mc_comboMiniImageSize.GetCurSel() < 0) {
		mc_comboMiniImageSize.SetCurSel( 0 );
	}
	// �L���E����
	OnBnClickedShowMiniImageDlgCheck();
#endif

	UpdateControlItemStatus();

	return TRUE;  // return TRUE unless you set the focus to a control
	// ��O : OCX �v���p�e�B �y�[�W�͕K�� FALSE ��Ԃ��܂��B
}

void COptionTabMainView::OnOK()
{
	// ���[�U��R�~���j�e�B�̉摜
	theApp.m_optionMng.m_bShowMainViewMiniImage = IsDlgButtonChecked( IDC_SHOW_MINI_IMAGE_CHECK ) == BST_CHECKED;
	theApp.m_optionMng.m_bShowMainViewMiniImageDlg = IsDlgButtonChecked( IDC_SHOW_MINI_IMAGE_DLG_CHECK ) == BST_CHECKED;
	theApp.m_optionMng.m_bShowMainViewMiniImageOnMouseOver = IsDlgButtonChecked( IDC_SHOW_MINI_IMAGE_DLG_ON_MOUSEOVER_CHECK ) == BST_CHECKED;

	// �g�s�b�N���̃A�C�R���\��
	theApp.m_optionMng.m_bShowMainViewIcon = IsDlgButtonChecked( IDC_SHOW_ICON_CHECK ) == BST_CHECKED;

	// mini��ʃT�C�Y
#ifndef WINCE
	theApp.m_optionMng.m_nMainViewMiniImageSize = mc_comboMiniImageSize.GetItemData( mc_comboMiniImageSize.GetCurSel() );
#endif

	// �摜�̎����擾
	theApp.m_optionMng.m_bAutoLoadMiniImage = IsDlgButtonChecked( IDC_AUTO_LOAD_MINI_IMAGE_CHECK ) == BST_CHECKED;

	CPropertyPage::OnOK();
}

void COptionTabMainView::OnBnClickedShowMiniImageDlgCheck()
{
	UpdateControlItemStatus();
}

void COptionTabMainView::OnBnClickedShowMiniImageCheck()
{
	UpdateControlItemStatus();
}

void COptionTabMainView::UpdateControlItemStatus(void)
{
#ifdef WINCE
	// WINCE �Ȃ疳��
	GetDlgItem( IDC_SHOW_MINI_IMAGE_DLG_CHECK )->EnableWindow( FALSE );
	GetDlgItem( IDC_SHOW_MINI_IMAGE_DLG_ON_MOUSEOVER_CHECK )->EnableWindow( FALSE );
	GetDlgItem( IDC_SIZE_STATIC )->EnableWindow( FALSE );
	GetDlgItem( IDC_MINI_IMAGE_SIZE_COMBO )->EnableWindow( FALSE );

	BOOL bImageCheck = (IsDlgButtonChecked( IDC_SHOW_MINI_IMAGE_CHECK ) == BST_CHECKED) ? TRUE : FALSE;
	if (bImageCheck) {
		GetDlgItem( IDC_AUTO_LOAD_MINI_IMAGE_CHECK )->EnableWindow( TRUE );
	} else {
		GetDlgItem( IDC_AUTO_LOAD_MINI_IMAGE_CHECK )->EnableWindow( FALSE );
	}

#else
	BOOL bImageCheck = (IsDlgButtonChecked( IDC_SHOW_MINI_IMAGE_CHECK ) == BST_CHECKED) ? TRUE : FALSE;
	BOOL bImageDlgCheck = (IsDlgButtonChecked( IDC_SHOW_MINI_IMAGE_DLG_CHECK ) == BST_CHECKED) ? TRUE : FALSE;

	if (bImageCheck) {
		GetDlgItem( IDC_SHOW_MINI_IMAGE_DLG_CHECK )->EnableWindow( TRUE );
		GetDlgItem( IDC_AUTO_LOAD_MINI_IMAGE_CHECK )->EnableWindow( TRUE );
		if (bImageDlgCheck) {
			// �S�ėL��
			GetDlgItem( IDC_SHOW_MINI_IMAGE_DLG_ON_MOUSEOVER_CHECK )->EnableWindow( TRUE );
			GetDlgItem( IDC_SIZE_STATIC )->EnableWindow( TRUE );
			GetDlgItem( IDC_MINI_IMAGE_SIZE_COMBO )->EnableWindow( TRUE );
		} else {
			// �J�[�\���ʒu�ƃR���{�{�b�N�X�֘A�͖���
			GetDlgItem( IDC_SHOW_MINI_IMAGE_DLG_ON_MOUSEOVER_CHECK )->EnableWindow( FALSE );
			GetDlgItem( IDC_SIZE_STATIC )->EnableWindow( FALSE );
			GetDlgItem( IDC_MINI_IMAGE_SIZE_COMBO )->EnableWindow( FALSE );
		}
	} else {
		// �S�Ė���
		GetDlgItem( IDC_SHOW_MINI_IMAGE_DLG_CHECK )->EnableWindow( FALSE );
		GetDlgItem( IDC_SHOW_MINI_IMAGE_DLG_ON_MOUSEOVER_CHECK )->EnableWindow( FALSE );
		GetDlgItem( IDC_SIZE_STATIC )->EnableWindow( FALSE );
		GetDlgItem( IDC_MINI_IMAGE_SIZE_COMBO )->EnableWindow( FALSE );
		GetDlgItem( IDC_AUTO_LOAD_MINI_IMAGE_CHECK )->EnableWindow( FALSE );
	}
#endif
}

#endif