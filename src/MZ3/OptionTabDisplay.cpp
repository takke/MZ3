/*
 * Copyright (c) MZ3 Project Team All rights reserved.
 * Code licensed under the BSD License:
 * http://www.mz3.jp/license.txt
 */
// OptionTabDisplay.cpp : 実装ファイル
//

#include "stdafx.h"
#include "MZ3.h"
#include "OptionTabDisplay.h"
#include "ReportView.h"
#include "ChooseFontDlg.h"
#include "util.h"
#include "ChooseClientTypeDlg.h"

#ifndef SMARTPHONE2003_UI_MODEL

// COptionTabDisplay ダイアログ

IMPLEMENT_DYNAMIC(COptionTabDisplay, CPropertyPage)

COptionTabDisplay::COptionTabDisplay()
	: CPropertyPage(COptionTabDisplay::IDD)
{

}

COptionTabDisplay::~COptionTabDisplay()
{
}

void COptionTabDisplay::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(COptionTabDisplay, CPropertyPage)
	ON_BN_CLICKED(IDC_RESET_TAB_BUTTON, &COptionTabDisplay::OnBnClickedResetTabButton)
	ON_BN_CLICKED(IDC_SELECT_FONT_BUTTON, &COptionTabDisplay::OnBnClickedSelectFontButton)
END_MESSAGE_MAP()


// COptionTabDisplay メッセージ ハンドラ

BOOL COptionTabDisplay::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	// 背景On/Off
	CheckDlgButton( IDC_USE_BGIMAGE_CHECK, theApp.m_optionMng.IsUseBgImage() ? BST_CHECKED : BST_UNCHECKED );

	// ダウンロード後の確認画面
	CheckDlgButton( IDC_USE_RUN_CONFIRM_DLG_CHECK, theApp.m_optionMng.m_bUseRunConfirmDlg ? BST_CHECKED : BST_UNCHECKED );

	// 通知領域に新着TLをバルーン表示する
	CheckDlgButton( IDC_SHOW_BALLOON_ON_NEW_TL_CHECK, theApp.m_optionMng.m_bShowBalloonOnNewTL ? BST_CHECKED : BST_UNCHECKED );
	CWnd* pItem = NULL;
#ifdef WINCE
	pItem = GetDlgItem( IDC_SHOW_BALLOON_ON_NEW_TL_CHECK );
	if (pItem != NULL) {
		pItem->ShowWindow(SW_HIDE);
	}
#endif

	// ClearType
	CheckDlgButton( IDC_USE_CLEAR_TYPE_FONT_CHECK, theApp.m_optionMng.m_bUseClearTypeFont ? BST_CHECKED : BST_UNCHECKED );

	// ここまで用意したけど、たいていの場合はクリアタイプONで問題ないため非表示にしておく。
	pItem = GetDlgItem( IDC_USE_CLEAR_TYPE_FONT_CHECK );
	if (pItem != NULL) {
		pItem->ShowWindow(SW_HIDE);
	}

	return TRUE;
}

void COptionTabDisplay::OnOK()
{
	// 背景On/Off
	theApp.m_optionMng.SetUseBgImage(
		((CButton*)GetDlgItem(IDC_USE_BGIMAGE_CHECK))->GetCheck() == BST_CHECKED ? TRUE : FALSE );

	// ダウンロード後の確認画面
	theApp.m_optionMng.m_bUseRunConfirmDlg = IsDlgButtonChecked( IDC_USE_RUN_CONFIRM_DLG_CHECK ) == BST_CHECKED ? true : false;

	// 通知領域に新着TLをバルーン表示する
	theApp.m_optionMng.m_bShowBalloonOnNewTL = IsDlgButtonChecked( IDC_SHOW_BALLOON_ON_NEW_TL_CHECK ) == BST_CHECKED ? true : false;

	// ClearType
	theApp.m_optionMng.m_bUseClearTypeFont = IsDlgButtonChecked( IDC_USE_CLEAR_TYPE_FONT_CHECK ) == BST_CHECKED ? true : false;

	CPropertyPage::OnOK();
}

void COptionTabDisplay::OnBnClickedResetTabButton()
{
	LPCTSTR msg =  L"タブをリセットしてもよろしいですか？\n（初期化後に" MZ3_APP_NAME L"を終了します）";
	if( MessageBox( msg, 0, MB_YESNO | MB_ICONQUESTION ) != IDYES ) {
		return;
	}

	// 初期化
//	theApp.m_root.initForTopPage();
	// クライアント選択画面を表示
	CChooseClientTypeDlg dlg;
	if (dlg.DoModal()==IDOK) {
		// 設定された内容で初期化
		theApp.m_root.initForTopPage(theApp.m_accessTypeInfo, dlg.m_initType);
	} else {
		return;
	}

	// グループ定義ファイルの保存
	theApp.SaveGroupData();

	// 再起動
	// ・・・できないのでメッセージを表示して終了。
	MessageBox( L"初期化処理が完了したため" MZ3_APP_NAME L"を終了します" );
	theApp.m_pReportView->SaveIndex();
	ShowWindow(SW_HIDE);
	AfxGetMainWnd()->PostMessage(WM_CLOSE);
}

/// フォント変更
void COptionTabDisplay::OnBnClickedSelectFontButton()
{
	CChooseFontDlg dlg;
	if( dlg.DoModal() == IDOK ) {
		theApp.m_optionMng.m_fontFace = dlg.m_strSelectedFont;
	}
}

#endif