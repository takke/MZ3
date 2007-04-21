// OptionTabMInfo.cpp : 実装ファイル
//

#include "stdafx.h"
#include "MZ3.h"
#include "OptionTabMInfo.h"
#include "ReportView.h"
#include "ChooseFontDlg.h"
#include "util.h"

// COptionTabMInfo ダイアログ

IMPLEMENT_DYNAMIC(COptionTabMInfo, CPropertyPage)

COptionTabMInfo::COptionTabMInfo()
	: CPropertyPage(COptionTabMInfo::IDD)
{

}

COptionTabMInfo::~COptionTabMInfo()
{
}

void COptionTabMInfo::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(COptionTabMInfo, CPropertyPage)
END_MESSAGE_MAP()

// COptionTabMInfo メッセージ ハンドラ
BOOL COptionTabMInfo::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	// 起動時の新着チェック
	((CButton*)GetDlgItem(IDC_BOOT_MNC_CHECK))->SetCheck(
		theApp.m_optionMng.IsBootCheckMnC() ? BST_CHECKED : BST_UNCHECKED );

	return TRUE;
}

void COptionTabMInfo::OnOK()
{
	// 起動時の新着チェック
	theApp.m_optionMng.SetBootCheckMnC(
		((CButton*)GetDlgItem(IDC_BOOT_MNC_CHECK))->GetCheck() == BST_CHECKED ? TRUE : FALSE );

	CPropertyPage::OnOK();
}

