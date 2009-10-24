/*
 * Copyright (c) MZ3 Project Team All rights reserved.
 * Code licensed under the BSD License:
 * http://www.mz3.jp/license.txt
 */
// OptionTabUI.cpp : �����t�@�C��
//

#include "stdafx.h"
#include "MZ3.h"
#include "OptionTabUI.h"
#include "util.h"

#ifndef SMARTPHONE2003_UI_MODEL

// COptionTabUI �_�C�A���O

IMPLEMENT_DYNAMIC(COptionTabUI, CPropertyPage)

COptionTabUI::COptionTabUI()
	: CPropertyPage(COptionTabUI::IDD)
{

}

COptionTabUI::~COptionTabUI()
{
}

void COptionTabUI::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(COptionTabUI, CPropertyPage)
END_MESSAGE_MAP()


// COptionTabUI ���b�Z�[�W �n���h��
BOOL COptionTabUI::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	// ���������莞��
	SetDlgItemText( IDC_LONG_RETURN_RANGE_MSEC_EDIT, util::int2str(theApp.m_optionMng.m_longReturnRangeMSec) );
#ifndef WINCE
	// Win32 �ł͖���
	GetDlgItem( IDC_LONG_RETURN_RANGE_MSEC_EDIT )->EnableWindow( FALSE );
#endif

	// Xcrawl �듮��h�~�@�\
	CheckDlgButton( IDC_USE_XCRAWL_CANCELER_CHECK, theApp.m_optionMng.m_bUseXcrawlExtension ? BST_CHECKED : BST_UNCHECKED );
#ifndef WINCE
	// Win32 �ł͖���
	GetDlgItem( IDC_USE_XCRAWL_CANCELER_CHECK )->EnableWindow( FALSE );
#endif

	// �����r���[�ƃJ�e�S�����X�g�̃p���X�N���[��
	CheckDlgButton( IDC_USE_RAN2_PAN_SCROLL_ANIMATION_CHECK, theApp.m_optionMng.m_bUseRan2PanScrollAnimation ? BST_CHECKED : BST_UNCHECKED );

	// ���h���b�O�ł̍��ڈړ�
	CheckDlgButton( IDC_USE_RAN2_HORIZONTAL_DRAG_MOVE_CHECK, theApp.m_optionMng.m_bUseRan2HorizontalDragMove ? BST_CHECKED : BST_UNCHECKED );

	return TRUE;
}

void COptionTabUI::OnOK()
{
	// ���������莞��
	{
		CString s;
		GetDlgItemText( IDC_LONG_RETURN_RANGE_MSEC_EDIT, s );
		theApp.m_optionMng.m_longReturnRangeMSec = option::Option::normalizeLongReturnRangeMSec( _wtoi(s) );
	}

	// Xcrawl �듮��h�~�@�\
	theApp.m_optionMng.m_bUseXcrawlExtension = IsDlgButtonChecked( IDC_USE_XCRAWL_CANCELER_CHECK ) == BST_CHECKED ? true : false;

	// �����r���[�ƃJ�e�S�����X�g�̃p���X�N���[��
	theApp.m_optionMng.m_bUseRan2PanScrollAnimation = (IsDlgButtonChecked(IDC_USE_RAN2_PAN_SCROLL_ANIMATION_CHECK) == BST_CHECKED) ? true : false;

	// ���h���b�O�ł̍��ڈړ�
	theApp.m_optionMng.m_bUseRan2HorizontalDragMove = (IsDlgButtonChecked(IDC_USE_RAN2_HORIZONTAL_DRAG_MOVE_CHECK) == BST_CHECKED) ? true : false;

	CPropertyPage::OnOK();
}

#endif