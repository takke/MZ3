/*
 * Copyright (c) MZ3 Project Team All rights reserved.
 * Code licensed under the BSD License:
 * http://www.mz3.jp/license.txt
 */
// OptionTabTwitter.cpp : 実装ファイル
//

#include "stdafx.h"
#include "MZ3.h"
#include "OptionTabTwitter.h"
#include "util_base.h"

#ifndef SMARTPHONE2003_UI_MODEL

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

	// フッターマーク
	CheckDlgButton( IDC_ADD_SOURCE_TEXT_ON_POST_CHECK, 
					theApp.m_optionMng.m_bAddSourceTextOnTwitterPost ? BST_CHECKED : BST_UNCHECKED );
//	SetDlgItemText( IDC_ADD_SOURCE_TEXT_ON_POST_CHECK, L"文末に *" MZ3_APP_NAME L"* マークをつける" );

	// フッターマーク文字列
	SetDlgItemText( IDC_TWITTER_POST_FOOTER_TEXT_EDIT, theApp.m_optionMng.m_strTwitterPostFotterText );

	// ステータス行数
	{
		CComboBox* pComboBox = (CComboBox*)GetDlgItem(IDC_STATUS_LINE_COUNT_COMBO);
		int selectedNumber = theApp.m_optionMng.m_nTwitterStatusLineCount;
		int items[] = { 2, 3, -1 };	// -1 : terminater
		for (int i=0; items[i]!=-1; i++) {
			int n = items[i];
			int idx = pComboBox->InsertString(i, util::FormatString(L"%d", n));
			pComboBox->SetItemData(idx, n);
			if (selectedNumber==n) {
				pComboBox->SetCurSel(i);
			}
		}
	}

	// 取得ページ数
	{
		CComboBox* pComboBox = (CComboBox*)GetDlgItem(IDC_TWITTER_GET_PAGE_COUNT_COMBO);
		int selectedNumber = theApp.m_optionMng.m_nTwitterGetPageCount;
		int items[] = { 1, 2, 3, 4, 5, -1 };	// -1 : terminater
		for (int i=0; items[i]!=-1; i++) {
			int n = items[i];
			int idx = pComboBox->InsertString(i, util::FormatString(L"%d", n));
			pComboBox->SetItemData(idx, n);
			if (selectedNumber==n) {
				pComboBox->SetCurSel(i);
			}
		}
	}

	// 投稿後にタイムラインを取得する
	CheckDlgButton( IDC_TWITTER_RELOAD_TL_AFTER_POST_CHECK, 
					theApp.m_optionMng.m_bTwitterReloadTLAfterPost ? BST_CHECKED : BST_UNCHECKED );

	return TRUE;  // return TRUE unless you set the focus to a control
	// 例外 : OCX プロパティ ページは必ず FALSE を返します。
}

void COptionTabTwitter::OnOK()
{
	// フッターマーク
	theApp.m_optionMng.m_bAddSourceTextOnTwitterPost = IsDlgButtonChecked( IDC_ADD_SOURCE_TEXT_ON_POST_CHECK ) == BST_CHECKED ? true : false;

	// フッターマーク文字列
	GetDlgItemText(IDC_TWITTER_POST_FOOTER_TEXT_EDIT, theApp.m_optionMng.m_strTwitterPostFotterText);

	// ステータス行数
	{
		CComboBox* pComboBox = (CComboBox*)GetDlgItem(IDC_STATUS_LINE_COUNT_COMBO);
		int idx = pComboBox->GetCurSel();
		if (idx>=0) {
			int n = pComboBox->GetItemData(idx);
			theApp.m_optionMng.m_nTwitterStatusLineCount = option::Option::normalizeTwitterStatusLineCount(n);
		}
	}

	// 最大取得ページ数
	{
		CComboBox* pComboBox = (CComboBox*)GetDlgItem(IDC_TWITTER_GET_PAGE_COUNT_COMBO);
		int idx = pComboBox->GetCurSel();
		if (idx>=0) {
			int n = pComboBox->GetItemData(idx);
			theApp.m_optionMng.m_nTwitterGetPageCount = option::Option::normalizeTwitterGetPageCount(n);
		}
	}

	// 投稿後にタイムラインを取得する
	theApp.m_optionMng.m_bTwitterReloadTLAfterPost = IsDlgButtonChecked( IDC_TWITTER_RELOAD_TL_AFTER_POST_CHECK ) == BST_CHECKED ? true : false;

	CPropertyPage::OnOK();
}

#endif