/*
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */
// UserDlg.cpp : 実装ファイル
//

#include "stdafx.h"
#include "MZ3.h"
#include "UserDlg.h"
#include "util_base.h"
#include "util_goo.h"

// CUserDlg ダイアログ

IMPLEMENT_DYNAMIC(CUserDlg, CDialog)

CUserDlg::CUserDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CUserDlg::IDD, pParent)
{
}

CUserDlg::~CUserDlg()
{
}

void CUserDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TYPE_COMBO, mc_comboType);
}


BEGIN_MESSAGE_MAP(CUserDlg, CDialog)
	ON_BN_CLICKED(IDOK, &CUserDlg::OnBnClickedOk)
	ON_CBN_SELCHANGE(IDC_TYPE_COMBO, &CUserDlg::OnCbnSelchangeTypeCombo)
END_MESSAGE_MAP()


// CUserDlg メッセージ ハンドラ

BOOL CUserDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	mc_comboType.InsertString( USER_DLG_COMBO_INDEX_TYPE_MIXI,    L"mixi" );
	mc_comboType.InsertString( USER_DLG_COMBO_INDEX_TYPE_TWITTER, L"Twitter" );
	mc_comboType.InsertString( USER_DLG_COMBO_INDEX_TYPE_WASSR,	  L"Wassr" );
	mc_comboType.InsertString( USER_DLG_COMBO_INDEX_TYPE_GOOHOME, L"gooホーム" );

	mc_comboType.SetCurSel( USER_DLG_COMBO_INDEX_TYPE_MIXI );
	m_idxSelectedCombo = USER_DLG_COMBO_INDEX_TYPE_MIXI;

	MyLoadControlData();

	return TRUE;  // return TRUE unless you set the focus to a control
	// 例外 : OCX プロパティ ページは必ず FALSE を返します。
}

void CUserDlg::OnBnClickedOk()
{
	if (MySaveControlData()) {
		OnOK();
	}
}

void CUserDlg::OnCbnSelchangeTypeCombo()
{
	MySaveControlData();

	m_idxSelectedCombo = (USER_DLG_COMBO_INDEX_TYPE)mc_comboType.GetCurSel();

	MyLoadControlData();
}

bool CUserDlg::MySaveControlData(void)
{
	CString buf;

	switch( m_idxSelectedCombo ) {
	case USER_DLG_COMBO_INDEX_TYPE_TWITTER:
		GetDlgItemText( IDC_LOGIN_MAIL_EDIT, buf );
		theApp.m_loginMng.SetTwitterId( buf );

		GetDlgItemText( IDC_LOGIN_PASSWORD_EDIT, buf );
		theApp.m_loginMng.SetTwitterPassword( buf );
		break;

	case USER_DLG_COMBO_INDEX_TYPE_WASSR:
		GetDlgItemText( IDC_LOGIN_MAIL_EDIT, buf );
		theApp.m_loginMng.SetWassrId( buf );

		GetDlgItemText( IDC_LOGIN_PASSWORD_EDIT, buf );
		theApp.m_loginMng.SetWassrPassword( buf );
		break;

	case USER_DLG_COMBO_INDEX_TYPE_GOOHOME:
		GetDlgItemText( IDC_LOGIN_MAIL_EDIT, buf );
		theApp.m_loginMng.SetGooId( buf );

		GetDlgItemText( IDC_LOGIN_PASSWORD_EDIT, buf );
		theApp.m_loginMng.SetGoohomeQuoteMailAddress( buf );

		// 投稿アドレス形式チェック
		if (!gooutil::IsValidQuoteMailAddress(buf)) {
			CString msg = L"gooホームひとこと投稿アドレスは下記の形式です。\n"
						  L" quote-XXXXXXXXXXXX@home.goo.ne.jp\n"
						  L"\n"
						  L"確認するURLを開きますか？";
			if (MessageBox(msg, NULL, MB_YESNO)==IDYES) {
				util::OpenUrlByBrowser(L"http://home.goo.ne.jp/config/quote");
			}
			return false;
		}
		break;

	case USER_DLG_COMBO_INDEX_TYPE_MIXI:
	default:
		GetDlgItemText( IDC_LOGIN_MAIL_EDIT, buf );
		theApp.m_loginMng.SetEmail( buf );
		
		GetDlgItemText( IDC_LOGIN_PASSWORD_EDIT, buf );
		theApp.m_loginMng.SetPassword( buf );
		break;
	}

	theApp.m_loginMng.Write();

	return true;
}

void CUserDlg::MyLoadControlData(void)
{
	switch( m_idxSelectedCombo ) {
	case USER_DLG_COMBO_INDEX_TYPE_TWITTER:
		SetDlgItemText( IDC_ID_STATIC, L"ID" );
		SetDlgItemText( IDC_LOGIN_MAIL_EDIT, theApp.m_loginMng.GetTwitterId() );
		SetDlgItemText( IDC_PASSWORD_STATIC, L"パスワード" );
		SetDlgItemText( IDC_LOGIN_PASSWORD_EDIT, theApp.m_loginMng.GetTwitterPassword() );
		break;

	case USER_DLG_COMBO_INDEX_TYPE_WASSR:
		SetDlgItemText( IDC_ID_STATIC, L"ID" );
		SetDlgItemText( IDC_LOGIN_MAIL_EDIT, theApp.m_loginMng.GetWassrId() );
		SetDlgItemText( IDC_PASSWORD_STATIC, L"パスワード" );
		SetDlgItemText( IDC_LOGIN_PASSWORD_EDIT, theApp.m_loginMng.GetWassrPassword() );
		break;

	case USER_DLG_COMBO_INDEX_TYPE_GOOHOME:
		SetDlgItemText( IDC_ID_STATIC, L"gooID" );
		SetDlgItemText( IDC_LOGIN_MAIL_EDIT, theApp.m_loginMng.GetGooId() );
		SetDlgItemText( IDC_PASSWORD_STATIC, L"gooホーム ひとことメール投稿アドレス" );
		SetDlgItemText( IDC_LOGIN_PASSWORD_EDIT, theApp.m_loginMng.GetGoohomeQuoteMailAddress() );
		break;

	case USER_DLG_COMBO_INDEX_TYPE_MIXI:
	default:
		SetDlgItemText( IDC_ID_STATIC, L"メールアドレス" );
		SetDlgItemText( IDC_LOGIN_MAIL_EDIT, theApp.m_loginMng.GetEmail() );
		SetDlgItemText( IDC_PASSWORD_STATIC, L"パスワード" );
		SetDlgItemText( IDC_LOGIN_PASSWORD_EDIT, theApp.m_loginMng.GetPassword() );
		break;
	}
}
