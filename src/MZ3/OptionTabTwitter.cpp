// OptionTabTwitter.cpp : 実装ファイル
//

#include "stdafx.h"
#include "MZ3.h"
#include "OptionTabTwitter.h"
#include "util_base.h"


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

	// ステータス行数
	CComboBox* pStatusLineCountCombo = (CComboBox*)GetDlgItem(IDC_STATUS_LINE_COUNT_COMBO);
	int selectedNumber = theApp.m_optionMng.m_nTwitterStatusLineCount;
	int items[] = { 2, 3, -1 };	// -1 : terminater
	for (int i=0; items[i]!=-1; i++) {
		int n = items[i];
		int idx = pStatusLineCountCombo->InsertString(i, util::FormatString(L"%d", n));
		pStatusLineCountCombo->SetItemData(idx, n);
		if (selectedNumber==n) {
			pStatusLineCountCombo->SetCurSel(i);
		}
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// 例外 : OCX プロパティ ページは必ず FALSE を返します。
}

void COptionTabTwitter::OnOK()
{
	// *MZ3* マーク
	theApp.m_optionMng.m_bAddSourceTextOnTwitterPost = IsDlgButtonChecked( IDC_ADD_SOURCE_TEXT_ON_POST_CHECK ) == BST_CHECKED ? true : false;

	// ステータス行数
	CComboBox* pStatusLineCountCombo = (CComboBox*)GetDlgItem(IDC_STATUS_LINE_COUNT_COMBO);
	int idx = pStatusLineCountCombo->GetCurSel();
	if (idx>=0) {
		int n = pStatusLineCountCombo->GetItemData(idx);
		theApp.m_optionMng.m_nTwitterStatusLineCount = option::Option::normalizeTwitterStatusLineCount(n);
	}

	CPropertyPage::OnOK();
}
