// ProxyTab.cpp : 実装ファイル
//

#include "stdafx.h"
#include "MZ3.h"
#include "OptionTabProxy.h"


// COptionTabProxy ダイアログ

IMPLEMENT_DYNAMIC(COptionTabProxy, CPropertyPage)

COptionTabProxy::COptionTabProxy()
	: CPropertyPage(COptionTabProxy::IDD)
{

}

COptionTabProxy::~COptionTabProxy()
{
}

void COptionTabProxy::DoDataExchange(CDataExchange* pDX)
{
  CPropertyPage::DoDataExchange(pDX);
  DDX_Control(pDX, IDC_PROXY_USE_CHECK, m_useCheck);
  DDX_Control(pDX, IDC_PROXY_SERVER_EDIT, m_serverEdit);
  DDX_Control(pDX, IDC_PROXY_PORT_EDIT, m_portEdit);
  DDX_Control(pDX, IDC_PROXY_USER_EDIT, m_userEdit);
  DDX_Control(pDX, IDC_PROXY_PASSWPRD_EDIT, m_passwordEdit);
}


BEGIN_MESSAGE_MAP(COptionTabProxy, CPropertyPage)
	ON_BN_CLICKED(IDC_PROXY_USE_CHECK, &COptionTabProxy::OnBnClickedProxyUseCheck)
	ON_BN_CLICKED(IDC_USE_GLOBAL_PROXY_RADIO, &COptionTabProxy::OnBnClickedUseGlobalProxyRadio)
	ON_BN_CLICKED(IDC_USE_MANUAL_PROXY_RADIO, &COptionTabProxy::OnBnClickedUseManualProxyRadio)
END_MESSAGE_MAP()

#include "util.h"

// COptionTabProxy メッセージ ハンドラ

void COptionTabProxy::Load()
{
	// プロキシ
	m_useCheck.SetCheck( theApp.m_optionMng.IsUseProxy() ? BST_CHECKED : BST_UNCHECKED );

	// グローバルプロキシ
	CheckRadioButton( IDC_USE_GLOBAL_PROXY_RADIO, IDC_USE_MANUAL_PROXY_RADIO,
		theApp.m_optionMng.IsUseGlobalProxy() ? IDC_USE_GLOBAL_PROXY_RADIO : IDC_USE_MANUAL_PROXY_RADIO );

	// プロキシサーバ
	m_serverEdit.SetWindowText( theApp.m_optionMng.GetProxyServer() );

	// プロキシポート番号
	m_portEdit.SetWindowText( util::int2str(theApp.m_optionMng.GetProxyPort()) );

	// プロキシユーザ
	m_userEdit.SetWindowText( theApp.m_optionMng.GetProxyUser() );

	// プロキシパスワード
	m_passwordEdit.SetWindowText( theApp.m_optionMng.GetProxyPassword() );
}

void COptionTabProxy::Save()
{
	CString buf;

	// 再接続フラグ。
	// 状態が変化した場合に再接続を行う。
	bool bReConnect = false;

	// プロキシ
	bool bUseProxyUpdated = (m_useCheck.GetCheck() == BST_CHECKED) ? true : false;
	if (theApp.m_optionMng.IsUseProxy() != bUseProxyUpdated ) {
		bReConnect = true;
	}
	theApp.m_optionMng.SetUseProxy( bUseProxyUpdated );

	// グローバルプロキシ
	bool bUseGlobalProxyUpdated = IsDlgButtonChecked( IDC_USE_GLOBAL_PROXY_RADIO ) ? true : false;
	if( theApp.m_optionMng.IsUseGlobalProxy() != bUseGlobalProxyUpdated ) {
		bReConnect = true;
	}
	theApp.m_optionMng.SetUseGlobalProxy( bUseGlobalProxyUpdated );

	// プロキシサーバ
	m_serverEdit.GetWindowText(buf);
	if (theApp.m_optionMng.GetProxyServer() != buf) {
		bReConnect = true;
	}
	theApp.m_optionMng.SetProxyServer(buf);

	// プロキシポート番号
	m_portEdit.GetWindowText(buf);
	if (theApp.m_optionMng.GetProxyPort() != _wtoi(buf)) {
		bReConnect = true;
	}
	theApp.m_optionMng.SetProxyPort(_wtoi(buf));

	// プロキシユーザ
	m_userEdit.GetWindowText(buf);
	theApp.m_optionMng.SetProxyUser(buf);

	// プロキシパスワード
	m_passwordEdit.GetWindowText(buf);
	theApp.m_optionMng.SetProxyPassword(buf);

	if (bReConnect) {
		// 再接続フラグが ON なので再オープン
		theApp.m_inet.Open();
	}

}

BOOL COptionTabProxy::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	Load();

	DoUpdateControlViewStatus();

	return TRUE;
}

void COptionTabProxy::OnOK()
{
	Save();

	CPropertyPage::OnOK();
}

/**
 * プロキシチェックボックス、ラジオボタンの状態に応じて表示状態を変更する
 */
bool COptionTabProxy::DoUpdateControlViewStatus(void)
{
	int radio_buttons[] = { 
		IDC_USE_GLOBAL_PROXY_RADIO, IDC_USE_MANUAL_PROXY_RADIO, -1 };
	int manual_setting_controls[] = { 
		IDC_PROXY_SERVER_EDIT, IDC_PROXY_PORT_EDIT, IDC_PROXY_USER_EDIT, IDC_PROXY_PASSWPRD_EDIT, -1 };

	BOOL bEnableRadioButtons = FALSE;
	BOOL bEnableManualSettings = FALSE;

	// 判定
	if( IsDlgButtonChecked( IDC_PROXY_USE_CHECK ) == BST_CHECKED ) {
		// プロキシ有効 ⇒ ラジオボタンを Enable に。
		bEnableRadioButtons = TRUE;

		if( IsDlgButtonChecked( IDC_USE_GLOBAL_PROXY_RADIO ) == BST_CHECKED ) {
			// グローバルプロキシ有効 ⇒ 手動設定項目を Disable に。
			bEnableManualSettings = FALSE;
		}else{
			// 手動設定有効 ⇒ 手動設定項目を Enable に。
			bEnableManualSettings = TRUE;
		}
	}else{
		// プロキシ無効 ⇒ ラジオボタンと手動設定項目を Disable に。
		bEnableRadioButtons = FALSE;
		bEnableManualSettings = FALSE;
	}

	// 状態変更

	// ラジオボタン
	for( int i=0; radio_buttons[i] != -1; i++ ) {
		CWnd* p = GetDlgItem( radio_buttons[i] );
		if( p != NULL ) 
			p->EnableWindow( bEnableRadioButtons );
	}
	// 手動設定項目
	for( int i=0; manual_setting_controls[i] != -1; i++ ) {
		CWnd* p = GetDlgItem( manual_setting_controls[i] );
		if( p != NULL ) 
			p->EnableWindow( bEnableManualSettings );
	}

	return false;
}

void COptionTabProxy::OnBnClickedProxyUseCheck()
{
	// コントロールの表示状態の更新
	DoUpdateControlViewStatus();
}

void COptionTabProxy::OnBnClickedUseGlobalProxyRadio()
{
	// コントロールの表示状態の更新
	DoUpdateControlViewStatus();
}

void COptionTabProxy::OnBnClickedUseManualProxyRadio()
{
	// コントロールの表示状態の更新
	DoUpdateControlViewStatus();
}
