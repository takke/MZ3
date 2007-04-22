// OptionTabMInfo.cpp : 実装ファイル
//

#include "stdafx.h"
#include "MZ3.h"
#include "OptionTabGeneral.h"
#include "ReportView.h"
#include "ChooseFontDlg.h"
#include "util.h"

// COptionTabGeneral ダイアログ

IMPLEMENT_DYNAMIC(COptionTabGeneral, CPropertyPage)

COptionTabGeneral::COptionTabGeneral()
	: CPropertyPage(COptionTabGeneral::IDD)
{

}

COptionTabGeneral::~COptionTabGeneral()
{
}

void COptionTabGeneral::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(COptionTabGeneral, CPropertyPage)
END_MESSAGE_MAP()

// COptionTabGeneral メッセージ ハンドラ
BOOL COptionTabGeneral::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	// 起動時の新着チェック
	((CButton*)GetDlgItem(IDC_BOOT_MNC_CHECK))->SetCheck(
		theApp.m_optionMng.IsBootCheckMnC() ? BST_CHECKED : BST_UNCHECKED );

	return TRUE;
}

void COptionTabGeneral::OnOK()
{
	// 起動時の新着チェック
	theApp.m_optionMng.SetBootCheckMnC(
		((CButton*)GetDlgItem(IDC_BOOT_MNC_CHECK))->GetCheck() == BST_CHECKED ? TRUE : FALSE );

	CPropertyPage::OnOK();
}

