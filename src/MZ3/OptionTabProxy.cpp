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
END_MESSAGE_MAP()

#include "util.h"

// COptionTabProxy メッセージ ハンドラ

void COptionTabProxy::Load()
{
	if (theApp.m_optionMng.IsProxyUse() != FALSE) {
		m_useCheck.SetCheck(BST_CHECKED);
	}
	else {
		m_useCheck.SetCheck(BST_UNCHECKED);
	}


	m_serverEdit.SetWindowText(theApp.m_optionMng.GetProxyServer());

	m_portEdit.SetWindowText( util::int2str(theApp.m_optionMng.GetProxyPort()) );

	m_userEdit.SetWindowText(theApp.m_optionMng.GetProxyUser());
	m_passwordEdit.SetWindowText(theApp.m_optionMng.GetProxyPassword());

}

void COptionTabProxy::Save()
{
	CString buf;

	theApp.SetReConnect(FALSE);

	if (theApp.m_optionMng.IsProxyUse() != m_useCheck.GetCheck()) {
		theApp.SetReConnect(TRUE);
	}

	if (m_useCheck.GetCheck() == BST_CHECKED) {
		theApp.m_optionMng.SetProxyUse(TRUE);
	}
	else {
		theApp.m_optionMng.SetProxyUse(FALSE);
	}

	m_serverEdit.GetWindowText(buf);
	if (theApp.m_optionMng.GetProxyServer() != buf) {
		theApp.SetReConnect(TRUE);
	}
	theApp.m_optionMng.SetProxyServer(buf);

	m_portEdit.GetWindowText(buf);
	if (theApp.m_optionMng.GetProxyPort() != _wtoi(buf)) {
		theApp.SetReConnect(TRUE);
	}
	theApp.m_optionMng.SetProxyPort(_wtoi(buf));

	m_userEdit.GetWindowText(buf);
	theApp.m_optionMng.SetProxyUser(buf);

	m_passwordEdit.GetWindowText(buf);
	theApp.m_optionMng.SetProxyPassword(buf);

	if (theApp.IsReConnect()) {
		theApp.m_inet.Open();
	}

}

BOOL COptionTabProxy::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	Load();

	return TRUE;
}

void COptionTabProxy::OnOK()
{
	Save();

	CPropertyPage::OnOK();
}
