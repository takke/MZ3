/*
 * Copyright (c) MZ3 Project Team All rights reserved.
 * Code licensed under the BSD License:
 * http://www.mz3.jp/license.txt
 */
// CategorySettingDlg.cpp : 実装ファイル
//

#include "stdafx.h"
#include "MZ3GroupFileEditor.h"
#include "CategorySettingDlg.h"
#include "../MZ3/util_base.h"
#include "../MZ3/util_mixi.h"


// CCategorySettingDlg ダイアログ

IMPLEMENT_DYNAMIC(CCategorySettingDlg, CDialog)

CCategorySettingDlg::CCategorySettingDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CCategorySettingDlg::IDD, pParent)
	, mc_strName(_T(""))
	, mc_strUrl(_T(""))
{

}

CCategorySettingDlg::~CCategorySettingDlg()
{
}

void CCategorySettingDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_NAME_EDIT, mc_strName);
	DDX_Control(pDX, IDC_TYPE_COMBO, mc_comboType);
	DDX_Text(pDX, IDC_URL_EDIT, mc_strUrl);
}


BEGIN_MESSAGE_MAP(CCategorySettingDlg, CDialog)
	ON_BN_CLICKED(IDC_TEMPLATE_BUTTON, &CCategorySettingDlg::OnBnClickedTemplateButton)
	ON_COMMAND_RANGE(ID_CATEGORY_TEMPLATE_START, ID_CATEGORY_TEMPLATE_START+1000, &CCategorySettingDlg::OnTemplateMenuItems)
END_MESSAGE_MAP()


// CCategorySettingDlg メッセージ ハンドラ

BOOL CCategorySettingDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	std::vector<ACCESS_TYPE> types = theApp.m_accessTypeInfo.getCategoryTypeList();
	for (int i=0; i<types.size(); i++) {
		int idx = mc_comboType.InsertString( i, theApp.m_accessTypeInfo.getShortText(types[i]) );

		mc_comboType.SetItemData( idx, types[i] );
		if (types[i]==m_item.m_mixi.GetAccessType()) {
			mc_comboType.SetCurSel(idx);
		}
	}

	// ロード
	MyLoadItem();

	return TRUE;  // return TRUE unless you set the focus to a control
	// 例外 : OCX プロパティ ページは必ず FALSE を返します。
}

void CCategorySettingDlg::OnOK()
{
	UpdateData();

	// 更新
	m_item.m_name = mc_strName;
	m_item.m_mixi.SetAccessType( (ACCESS_TYPE)mc_comboType.GetItemData(mc_comboType.GetCurSel()) );
	m_item.m_mixi.SetURL( mc_strUrl );
	m_item.m_bCruise = (IsDlgButtonChecked(IDC_CRUISE_CHECK)==BST_CHECKED) ? true : false;

	CDialog::OnOK();
}

void CCategorySettingDlg::OnBnClickedTemplateButton()
{
	POINT pt;
	GetCursorPos(&pt);

	CMenu menu;
	menu.LoadMenu(IDR_CATEGORY_TEMPLATE_MENU);

	// フルテンプレート生成
	Mz3GroupData template_data;
	template_data.initForTopPage(theApp.m_accessTypeInfo, Mz3GroupData::InitializeType());

	// メニュー生成
	CMenu* pcThisMenu = menu.GetSubMenu(0);
	int menuId = ID_CATEGORY_TEMPLATE_START;
	for (int groupIdx=0; groupIdx<template_data.groups.size(); groupIdx++) {
		CMenu subMenu;
		subMenu.CreatePopupMenu();

		CGroupItem& group = template_data.groups[groupIdx];

		// subMenu にカテゴリ名を追加
		for (int ic=0; ic<group.categories.size(); ic++) {
			subMenu.AppendMenuW( MF_STRING, menuId, group.categories[ic].m_name );
			menuId ++;
		}

		pcThisMenu->AppendMenuW( MF_POPUP, (UINT)subMenu.m_hMenu, group.name );
	}

	// ダミーを削除
	pcThisMenu->RemoveMenu( ID_DUMMY, MF_BYCOMMAND );

	menu.GetSubMenu(0)->TrackPopupMenu(TPM_LEFTALIGN | TPM_TOPALIGN, pt.x, pt.y, this);
}

void CCategorySettingDlg::OnTemplateMenuItems(UINT nID)
{
	int idx = nID - ID_CATEGORY_TEMPLATE_START;

	// フルテンプレート生成
	Mz3GroupData template_data;
	template_data.initForTopPage(theApp.m_accessTypeInfo, Mz3GroupData::InitializeType());

	int idxCounter = 0;
	for (int groupIdx=0; groupIdx<template_data.groups.size(); groupIdx++) {
		CGroupItem& group = template_data.groups[groupIdx];

		for (int ic=0; ic<group.categories.size(); ic++) {
			if (idxCounter==idx) {
				// この項目を反映させる
				CCategoryItem item = group.categories[ic];
				m_item = item;

				// ロード
				MyLoadItem();

				return;
			}

			idxCounter ++;
		}
	}

}

void CCategorySettingDlg::MyLoadItem(void)
{
	mc_strName = m_item.m_name;

	for (int i=0; i<mc_comboType.GetCount(); i++) {
		if (mc_comboType.GetItemData(i)==m_item.m_mixi.GetAccessType()) {
			mc_comboType.SetCurSel(i);
		}
	}

	mc_strUrl = m_item.m_mixi.GetURL();

	CheckDlgButton( IDC_CRUISE_CHECK, m_item.m_bCruise ? BST_CHECKED : BST_UNCHECKED );

	UpdateData(FALSE);
}
