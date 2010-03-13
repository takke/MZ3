/*
 * Copyright (c) MZ3 Project Team All rights reserved.
 * Code licensed under the BSD License:
 * http://www.mz3.jp/license.txt
 */
// OptionTabMainView.cpp : 実装ファイル
//

#include "stdafx.h"
#include "MZ3.h"
#include "OptionTabMainView.h"

#ifndef SMARTPHONE2003_UI_MODEL

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
	DDX_Control(pDX, IDC_ICON_SIZE_COMBO, mc_comboIconSize);
	DDX_Control(pDX, IDC_BODY_LIST_LINE_COUNT_COMBO, mc_comboBodyListLineCount);
}


BEGIN_MESSAGE_MAP(COptionTabMainView, CPropertyPage)
	ON_BN_CLICKED(IDC_SHOW_MINI_IMAGE_CHECK, &COptionTabMainView::OnBnClickedShowMiniImageCheck)
END_MESSAGE_MAP()


// COptionTabMainView メッセージ ハンドラ

BOOL COptionTabMainView::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	// ユーザやコミュニティの画像
	CheckDlgButton( IDC_SHOW_MINI_IMAGE_CHECK, theApp.m_optionMng.m_bShowMainViewMiniImage ? BST_CHECKED : BST_UNCHECKED );
#ifdef BT_TKTW
	SetDlgItemText(IDC_SHOW_MINI_IMAGE_CHECK, L"ユーザのアイコンを表示する");
#endif

	// 画像の自動取得
	CheckDlgButton( IDC_AUTO_LOAD_MINI_IMAGE_CHECK, theApp.m_optionMng.m_bAutoLoadMiniImage ? BST_CHECKED : BST_UNCHECKED );

	// トピック等のアイコン表示
#ifdef BT_MZ3
	CheckDlgButton( IDC_SHOW_ICON_CHECK, theApp.m_optionMng.m_bShowMainViewIcon ? BST_CHECKED : BST_UNCHECKED );
#else
	GetDlgItem(IDC_SHOW_ICON_CHECK)->ShowWindow(SW_HIDE);
#endif

	// 下ペインのリストを１行で表示する
//	CheckDlgButton( IDC_NO_INTEGRATED_MODE_CHECK, theApp.m_optionMng.m_bBodyListIntegratedColumnMode ? BST_UNCHECKED : BST_CHECKED);

	// 下ペインのリストの行数
	{
		mc_comboBodyListLineCount.ResetContent();
		int idx = 0;
		mc_comboBodyListLineCount.InsertString(idx, L"1行");
		mc_comboBodyListLineCount.SetItemData(idx++, 1);
		mc_comboBodyListLineCount.InsertString(idx, L"2行");
		mc_comboBodyListLineCount.SetItemData(idx++, 2);
		mc_comboBodyListLineCount.InsertString(idx, L"3行");
		mc_comboBodyListLineCount.SetItemData(idx++, 3);
		mc_comboBodyListLineCount.InsertString(idx, L"4行");
		mc_comboBodyListLineCount.SetItemData(idx++, 4);

		mc_comboBodyListLineCount.SetCurSel(0);	// default
		int nLine = 1;
		if (theApp.m_optionMng.m_bBodyListIntegratedColumnMode) {
			// 統合カラムモード
			nLine = theApp.m_optionMng.m_nBodyListIntegratedColumnModeLine;
		} else {
			// 非統合カラムモード
			nLine = 1;
		}
		for (int i=0; i<mc_comboBodyListLineCount.GetCount(); i++) {
			if (mc_comboBodyListLineCount.GetItemData(i) == nLine) {
				mc_comboBodyListLineCount.SetCurSel(i);
				break;
			}
		}
	}

	// 上ペインのリストクリック時に取得する
	CheckDlgButton( IDC_ONE_CLICK_CATEGORY_FETCH_MODE_CHECK, theApp.m_optionMng.m_bOneClickCategoryFetchMode ? BST_CHECKED : BST_UNCHECKED);
	
	// 起動時に上ペインを最大化する
	CheckDlgButton( IDC_MAGNIFY_MODE_CATEGORY_AT_START_CHECK, theApp.m_optionMng.m_bMagnifyModeCategoryAtStart ? BST_CHECKED : BST_UNCHECKED);

	// アイコンサイズ
	{
		mc_comboIconSize.ResetContent();
		int idx = 0;
		mc_comboIconSize.InsertString(idx, L"自動");
		mc_comboIconSize.SetItemData(idx++, 0);
		mc_comboIconSize.InsertString(idx, L"64px");
		mc_comboIconSize.SetItemData(idx++, 64);
		mc_comboIconSize.InsertString(idx, L"48px");
		mc_comboIconSize.SetItemData(idx++, 48);
		mc_comboIconSize.InsertString(idx, L"32px");
		mc_comboIconSize.SetItemData(idx++, 32);
		mc_comboIconSize.InsertString(idx, L"16px");
		mc_comboIconSize.SetItemData(idx++, 16);
		for (int i=0; i<mc_comboIconSize.GetCount(); i++) {
			if (mc_comboIconSize.GetItemData(i) == theApp.m_optionMng.m_bodyListIconSize) {
				mc_comboIconSize.SetCurSel(i);
				break;
			}
		}
	}

	UpdateControlItemStatus();

	return TRUE;  // return TRUE unless you set the focus to a control
	// 例外 : OCX プロパティ ページは必ず FALSE を返します。
}

void COptionTabMainView::OnOK()
{
	// ユーザやコミュニティの画像
	theApp.m_optionMng.m_bShowMainViewMiniImage = IsDlgButtonChecked( IDC_SHOW_MINI_IMAGE_CHECK ) == BST_CHECKED;

	// 画像の自動取得
	theApp.m_optionMng.m_bAutoLoadMiniImage = IsDlgButtonChecked( IDC_AUTO_LOAD_MINI_IMAGE_CHECK ) == BST_CHECKED;

	// トピック等のアイコン表示
	theApp.m_optionMng.m_bShowMainViewIcon = IsDlgButtonChecked( IDC_SHOW_ICON_CHECK ) == BST_CHECKED;

	// 下ペインのリストを１行で表示する
//	theApp.m_optionMng.m_bBodyListIntegratedColumnMode = IsDlgButtonChecked( IDC_NO_INTEGRATED_MODE_CHECK ) != BST_CHECKED;

	// 下ペインのリストの行数
	{
		int idx = mc_comboBodyListLineCount.GetCurSel();
		if (0 <= idx && idx <= mc_comboBodyListLineCount.GetCount()-1) {
			int nLine = mc_comboBodyListLineCount.GetItemData(idx);
			if (nLine == 1) {
				theApp.m_optionMng.m_bBodyListIntegratedColumnMode = false;
			} else {
				theApp.m_optionMng.m_bBodyListIntegratedColumnMode = true;
				theApp.m_optionMng.m_nBodyListIntegratedColumnModeLine = nLine;
			}
		}
	}

	// 上ペインのリストクリック時に取得する
	theApp.m_optionMng.m_bOneClickCategoryFetchMode = IsDlgButtonChecked( IDC_ONE_CLICK_CATEGORY_FETCH_MODE_CHECK ) == BST_CHECKED;

	// 起動時に上ペインを最大化する
	theApp.m_optionMng.m_bMagnifyModeCategoryAtStart = IsDlgButtonChecked( IDC_MAGNIFY_MODE_CATEGORY_AT_START_CHECK ) == BST_CHECKED;

	// アイコンサイズ
	{
		int idx = mc_comboIconSize.GetCurSel();
		if (0 <= idx && idx <= mc_comboIconSize.GetCount()-1) {
			theApp.m_optionMng.m_bodyListIconSize = (option::Option::BODYLIST_ICONSIZE)mc_comboIconSize.GetItemData(idx);
		}
	}

	CPropertyPage::OnOK();
}

void COptionTabMainView::OnBnClickedShowMiniImageCheck()
{
	UpdateControlItemStatus();
}

void COptionTabMainView::UpdateControlItemStatus(void)
{
	BOOL bImageCheck = (IsDlgButtonChecked( IDC_SHOW_MINI_IMAGE_CHECK ) == BST_CHECKED) ? TRUE : FALSE;
	if (bImageCheck) {
		GetDlgItem( IDC_AUTO_LOAD_MINI_IMAGE_CHECK )->EnableWindow( TRUE );
	} else {
		GetDlgItem( IDC_AUTO_LOAD_MINI_IMAGE_CHECK )->EnableWindow( FALSE );
	}
}

#endif