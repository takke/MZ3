// OptionTabUI.cpp : �����t�@�C��
//

#include "stdafx.h"
#include "MZ3.h"
#include "OptionTabUI.h"
#include "util.h"

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

	// ���\�t�g�L�[�̗L���E����
	CheckDlgButton( IDC_USE_LEFTSOFTKEY_CHECK, theApp.m_optionMng.m_bUseLeftSoftKey ? BST_CHECKED : BST_UNCHECKED );
#ifndef WINCE
	// Win32 �ł͖���
	GetDlgItem( IDC_USE_LEFTSOFTKEY_CHECK )->EnableWindow( FALSE );
#endif

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

	return TRUE;
}

void COptionTabUI::OnOK()
{
	// ���\�t�g�L�[�̗L���E����
	theApp.m_optionMng.m_bUseLeftSoftKey = IsDlgButtonChecked( IDC_USE_LEFTSOFTKEY_CHECK ) == BST_CHECKED ? true : false;

	// ���������莞��
	{
		CString s;
		GetDlgItemText( IDC_LONG_RETURN_RANGE_MSEC_EDIT, s );
		theApp.m_optionMng.m_longReturnRangeMSec = option::Option::normalizeLongReturnRangeMSec( _wtoi(s) );
	}

	// Xcrawl �듮��h�~�@�\
	theApp.m_optionMng.m_bUseXcrawlExtension = IsDlgButtonChecked( IDC_USE_XCRAWL_CANCELER_CHECK ) == BST_CHECKED ? true : false;

	CPropertyPage::OnOK();
}
