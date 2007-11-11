// OptionTabMainView.cpp : 実装ファイル
//

#include "stdafx.h"
#include "MZ3.h"
#include "OptionTabMainView.h"


// COptionTabMainView ダイアログ

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
END_MESSAGE_MAP()


// COptionTabMainView メッセージ ハンドラ

BOOL COptionTabMainView::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	// ユーザやコミュニティの画像
	CheckDlgButton( IDC_SHOW_MINI_IMAGE_DLG_CHECK, theApp.m_optionMng.m_bShowMainViewMiniImage ? BST_CHECKED : BST_UNCHECKED );

	// トピック等のアイコン表示
	CheckDlgButton( IDC_SHOW_ICON_CHECK, theApp.m_optionMng.m_bShowMainViewIcon ? BST_CHECKED : BST_UNCHECKED );

	return TRUE;  // return TRUE unless you set the focus to a control
	// 例外 : OCX プロパティ ページは必ず FALSE を返します。
}

void COptionTabMainView::OnOK()
{
	// ユーザやコミュニティの画像
	theApp.m_optionMng.m_bShowMainViewMiniImage = IsDlgButtonChecked( IDC_SHOW_MINI_IMAGE_DLG_CHECK ) == BST_CHECKED;

	// トピック等のアイコン表示
	theApp.m_optionMng.m_bShowMainViewIcon = IsDlgButtonChecked( IDC_SHOW_ICON_CHECK ) == BST_CHECKED;

	CPropertyPage::OnOK();
}
