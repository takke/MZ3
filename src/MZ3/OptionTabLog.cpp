/*
 * Copyright (c) MZ3 Project Team All rights reserved.
 * Code licensed under the BSD License:
 * http://www.mz3.jp/license.txt
 */
// OptionTabLog.cpp : 実装ファイル
//

#include "stdafx.h"
#include "MZ3.h"
#include "util.h"
#include "util_gui.h"
#include "OptionTabLog.h"
#include "MZ3FileCacheManager.h"

#ifndef SMARTPHONE2003_UI_MODEL

// COptionTabLog ダイアログ

IMPLEMENT_DYNAMIC(COptionTabLog, CPropertyPage)

COptionTabLog::COptionTabLog()
	: CPropertyPage(COptionTabLog::IDD)
{

}

COptionTabLog::~COptionTabLog()
{
}

void COptionTabLog::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(COptionTabLog, CPropertyPage)
	ON_BN_CLICKED(IDC_CHANGE_LOG_FOLDER_BUTTON, &COptionTabLog::OnBnClickedChangeLogFolderButton)
	ON_BN_CLICKED(IDC_CLEAN_LOG_BUTTON, &COptionTabLog::OnBnClickedCleanLogButton)
	ON_BN_CLICKED(IDC_DEBUG_MODE_CHECK, &COptionTabLog::OnBnClickedDebugModeCheck)
END_MESSAGE_MAP()


// COptionTabLog メッセージ ハンドラ

void COptionTabLog::OnOK()
{
	Save();

	CPropertyPage::OnOK();
}

BOOL COptionTabLog::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	// オプションをダイアログに反映する
	Load();

	return TRUE;
}

/**
 * theApp.m_optionMng からダイアログに変換する
 */
void COptionTabLog::Load()
{
	// ログの保存
	CheckDlgButton( IDC_SAVE_LOG_CHECK, theApp.m_optionMng.m_bSaveLog ? BST_CHECKED : BST_UNCHECKED );

	// ログのパス
	SetDlgItemText( IDC_LOGFOLDER_EDIT, theApp.m_optionMng.GetLogFolder() );

	// デバッグモード
	CheckDlgButton( IDC_DEBUG_MODE_CHECK, theApp.m_optionMng.IsDebugMode() ? BST_CHECKED : BST_UNCHECKED );
}

/**
 * ダイアログのデータを theApp.m_optionMng に変換する
 */
void COptionTabLog::Save()
{
	// ログの保存
	theApp.m_optionMng.m_bSaveLog = (IsDlgButtonChecked( IDC_SAVE_LOG_CHECK ) == BST_CHECKED);

	// ログのパス
	CString strFolderPath;
	GetDlgItemText( IDC_LOGFOLDER_EDIT, strFolderPath );

	// オプションに保存
	theApp.m_optionMng.SetLogFolder( strFolderPath );

	// 各種パスの再生成
	theApp.m_filepath.init_logpath();

	// デバッグモード
	theApp.m_optionMng.SetDebugMode( IsDlgButtonChecked( IDC_DEBUG_MODE_CHECK ) == BST_CHECKED );
}

/**
 * ログフォルダの変更
 */
void COptionTabLog::OnBnClickedChangeLogFolderButton()
{
	CString strFolderPath;

	// デフォルトのパスを画面から取得する
	GetDlgItemText( IDC_LOGFOLDER_EDIT, strFolderPath );

	// フォルダ選択ダイアログ起動
	if( util::GetOpenFolderPath( m_hWnd, L"ログフォルダの変更", strFolderPath ) ) {

		// MZ3専用ログディレクトリ名を追加
		strFolderPath.Append( L"\\mz3log" );

		CString msg;
		msg.Format( 
			L"ログファイルの出力先を\n"
			L" %s\n"
			L"に変更します。よろしいですか？",
			strFolderPath );

		if( MessageBox( msg, 0, MB_YESNO ) == IDYES ) {
			// 画面の再設定
			SetDlgItemText( IDC_LOGFOLDER_EDIT, strFolderPath );
		}
	}

}

/**
 * ログの削除
 */
void COptionTabLog::OnBnClickedCleanLogButton()
{
	MZ3FileCacheManager::CountupResult cd;		// ファイル数, ファイルサイズ

	int nTargetFileLastWriteDaysBack = 0;		// 更新日付条件：全てのファイルを対象とする

	// ログファイル数カウント
	MZ3FileCacheManager cacheManager;
	cacheManager.GetTargetFileCount( theApp.m_filepath.deleteTargetFolders, &cd, nTargetFileLastWriteDaysBack );

	if( cd.nFiles == 0 ) {
		MessageBox( L"削除対象ファイルがありません" );
		return;
	}

	CString msg;
	msg.Format( 
		L"ログファイルを削除します。よろしいですか？\n"
		L"・ファイル数が多い場合、数分程度かかります\n"
		L"・[%s]以下の全てのファイルを消去します\n\n"
		L"ファイル数：%d\n"
		L"総ファイルサイズ：%s Bytes"
		, (LPCTSTR)theApp.m_filepath.logFolder, cd.nFiles, util::int2comma_str(cd.dwSize) );
	if( MessageBox( msg, 0, MB_YESNO | MB_ICONQUESTION ) != IDYES ) {
		return;
	}

	// 削除実行
	int nDeleted = 0;	// 削除済みファイル数
	cacheManager.DeleteFiles( theApp.m_filepath.deleteTargetFolders, &nDeleted, nTargetFileLastWriteDaysBack );

	msg.Format( 
		L"%d 個のファイルを削除しました。\n"
		L"（対象ファイル：%d 個）", nDeleted, cd.nFiles );
	MessageBox( msg );
}

void COptionTabLog::OnBnClickedDebugModeCheck()
{
	MessageBox( L"デバッグモードの変更は再起動後に反映されます" );
}

#endif