// OptionTabLog.cpp : 実装ファイル
//

#include "stdafx.h"
#include "MZ3.h"
#include "util.h"
#include "OptionTabLog.h"


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
