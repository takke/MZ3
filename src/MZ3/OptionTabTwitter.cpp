// OptionTabTwitter.cpp : 実装ファイル
//

#include "stdafx.h"
#include "MZ3.h"
#include "OptionTabTwitter.h"


// COptionTabTwitter ダイアログ

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


// COptionTabTwitter メッセージ ハンドラ

BOOL COptionTabTwitter::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	// *MZ3* マーク
	CheckDlgButton( IDC_ADD_SOURCE_TEXT_ON_POST_CHECK, theApp.m_optionMng.m_bAddSourceTextOnTwitterPost ? BST_CHECKED : BST_UNCHECKED );
	SetDlgItemText( IDC_ADD_SOURCE_TEXT_ON_POST_CHECK, L"文末に *" MZ3_APP_NAME L"* マークをつける" );

	return TRUE;  // return TRUE unless you set the focus to a control
	// 例外 : OCX プロパティ ページは必ず FALSE を返します。
}

void COptionTabTwitter::OnOK()
{
	// *MZ3* マーク
	theApp.m_optionMng.m_bAddSourceTextOnTwitterPost = IsDlgButtonChecked( IDC_ADD_SOURCE_TEXT_ON_POST_CHECK ) == BST_CHECKED ? true : false;

	CPropertyPage::OnOK();
}
