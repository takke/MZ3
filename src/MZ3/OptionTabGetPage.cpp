/*
 * Copyright (c) MZ3 Project Team All rights reserved.
 * Code licensed under the BSD License:
 * http://www.mz3.jp/license.txt
 */
// OptionTabGetPage.cpp : 実装ファイル
//

#include "stdafx.h"
#include "MZ3.h"
#include "util.h"
#include "OptionTabGetPage.h"

#ifndef SMARTPHONE2003_UI_MODEL

// COptionTabGetPage ダイアログ

IMPLEMENT_DYNAMIC(COptionTabGetPage, CPropertyPage)

COptionTabGetPage::COptionTabGetPage()
	: CPropertyPage(COptionTabGetPage::IDD)
{

}

COptionTabGetPage::~COptionTabGetPage()
{
}

void COptionTabGetPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_RECV_BUF_COMBO, mc_RecvBufCombo);
}


BEGIN_MESSAGE_MAP(COptionTabGetPage, CPropertyPage)
END_MESSAGE_MAP()


// COptionTabGetPage メッセージ ハンドラ

void COptionTabGetPage::OnOK()
{
	Save();

	CPropertyPage::OnOK();
}

BOOL COptionTabGetPage::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	// コンボボックスの初期化
	{
		int list[] = { 256, 512, 1024, 2048, 4096, 8192, 16384, 32768, -1 };

		mc_RecvBufCombo.ResetContent();

		for( int i=0; list[i] != -1; i++ ) {
			CString s;
			s.Format( L"%d Byte", list[i] );
			int idx = mc_RecvBufCombo.InsertString( i, s );
			mc_RecvBufCombo.SetItemData( i, list[i] );
		}
	}

	// オプションをダイアログに反映する
	Load();

	return TRUE;
}

/**
 * theApp.m_optionMng からダイアログに変換する
 */
void COptionTabGetPage::Load()
{
	// 取得種別
#ifdef BT_MZ3
	if (theApp.m_optionMng.GetPageType() == 0) {
		((CButton*)GetDlgItem(IDC_PAGE_GETALL_RADIO))->SetCheck(BST_CHECKED);
	} else {
		((CButton*)GetDlgItem(IDC_PAGE_GETLATEST_RADIO))->SetCheck(BST_CHECKED);
	}
#else
	GetDlgItem(IDC_PAGE_GET_GROUP)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_PAGE_GETALL_RADIO)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_PAGE_GETLATEST_RADIO)->ShowWindow(SW_HIDE);
#endif

	// 自動接続
	CheckDlgButton( IDC_USE_AUTOCONNECTION_CHECK, 
		theApp.m_optionMng.IsUseAutoConnection() ? BST_CHECKED : BST_UNCHECKED );

	// 受信バッファサイズ
	// リストになければ末尾に追加。
	const int recvBufSize = theApp.m_optionMng.GetRecvBufSize();
	bool bFound = false;
	for( int i=0; i<mc_RecvBufCombo.GetCount(); i++ ) {
		int bufSize = mc_RecvBufCombo.GetItemData( i );
		if( bufSize == recvBufSize ) {
			// 一致したので選択
			mc_RecvBufCombo.SetCurSel( i );
			bFound = true;
			break;
		}
	}
	if( !bFound ) {
		// 見つからなかったので末尾に追加、選択。
		CString s;
		s.Format( L"%d Byte", recvBufSize );
		int idx = mc_RecvBufCombo.InsertString( mc_RecvBufCombo.GetCount(), s );
		mc_RecvBufCombo.SetItemData( idx, recvBufSize );

		mc_RecvBufCombo.SetCurSel( idx );
	}

	// 定期取得間隔
	SetDlgItemText( IDC_INTERVAL_CHECK_SEC_EDIT, util::int2str(theApp.m_optionMng.m_nIntervalCheckSec) );
}

/**
 * ダイアログのデータを theApp.m_optionMng に変換する
 */
void COptionTabGetPage::Save()
{
	CString buf;

	// 再接続フラグ。
	// 状態が変化した場合に再接続を行う。
	bool bReConnect = false;


	// 取得種別
#ifdef BT_MZ3
	GETPAGE_TYPE type;
	if (((CButton*)GetDlgItem(IDC_PAGE_GETALL_RADIO))->GetCheck() == BST_CHECKED) {
		type = GETPAGE_ALL;
	}
	else {
		type = GETPAGE_LATEST20;
	}
	theApp.m_optionMng.SetPageType(type);
#endif

	// 受信バッファサイズ
	theApp.m_optionMng.SetRecvBufSize( mc_RecvBufCombo.GetItemData( mc_RecvBufCombo.GetCurSel() ) );

	// 自動接続
	bool bUseAutoConnectionUpdated = IsDlgButtonChecked( IDC_USE_AUTOCONNECTION_CHECK ) == BST_CHECKED ? true : false;
	if( theApp.m_optionMng.IsUseAutoConnection() != bUseAutoConnectionUpdated ) {
		bReConnect = true;
	}
	theApp.m_optionMng.SetUseAutoConnection( bUseAutoConnectionUpdated );

	if (bReConnect) {
		// 再接続フラグが ON なのでクローズする（再接続は必要時に行われる）
		theApp.m_inet.CloseInternetHandles();
	}

	// 定期取得間隔
	GetDlgItemText( IDC_INTERVAL_CHECK_SEC_EDIT, buf );
	theApp.m_optionMng.m_nIntervalCheckSec = option::Option::normalizeIntervalCheckSec( _wtoi( buf ) );
}

#endif