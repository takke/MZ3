// OptionTabUI.cpp : 実装ファイル
//

#include "stdafx.h"
#include "MZ3.h"
#include "OptionTabUI.h"
#include "util.h"

// COptionTabUI ダイアログ

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


// COptionTabUI メッセージ ハンドラ
BOOL COptionTabUI::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	// 左ソフトキーの有効・無効
	CheckDlgButton( IDC_USE_LEFTSOFTKEY_CHECK, theApp.m_optionMng.m_bUseLeftSoftKey ? BST_CHECKED : BST_UNCHECKED );
#ifndef WINCE
	// Win32 では無効
	GetDlgItem( IDC_USE_LEFTSOFTKEY_CHECK )->EnableWindow( FALSE );
#endif

	// 長押し判定時間
	SetDlgItemText( IDC_LONG_RETURN_RANGE_MSEC_EDIT, util::int2str(theApp.m_optionMng.m_longReturnRangeMSec) );
#ifndef WINCE
	// Win32 では無効
	GetDlgItem( IDC_LONG_RETURN_RANGE_MSEC_EDIT )->EnableWindow( FALSE );
#endif

	// Xcrawl 誤動作防止機能
	CheckDlgButton( IDC_USE_XCRAWL_CANCELER_CHECK, theApp.m_optionMng.m_bUseXcrawlExtension ? BST_CHECKED : BST_UNCHECKED );
#ifndef WINCE
	// Win32 では無効
	GetDlgItem( IDC_USE_XCRAWL_CANCELER_CHECK )->EnableWindow( FALSE );
#endif

	return TRUE;
}

void COptionTabUI::OnOK()
{
	// 左ソフトキーの有効・無効
	theApp.m_optionMng.m_bUseLeftSoftKey = IsDlgButtonChecked( IDC_USE_LEFTSOFTKEY_CHECK ) == BST_CHECKED ? true : false;

	// 長押し判定時間
	{
		CString s;
		GetDlgItemText( IDC_LONG_RETURN_RANGE_MSEC_EDIT, s );
		theApp.m_optionMng.m_longReturnRangeMSec = option::Option::normalizeLongReturnRangeMSec( _wtoi(s) );
	}

	// Xcrawl 誤動作防止機能
	theApp.m_optionMng.m_bUseXcrawlExtension = IsDlgButtonChecked( IDC_USE_XCRAWL_CANCELER_CHECK ) == BST_CHECKED ? true : false;

	CPropertyPage::OnOK();
}
