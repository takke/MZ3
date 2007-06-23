// ProxyTab.cpp : �����t�@�C��
//

#include "stdafx.h"
#include "MZ3.h"
#include "OptionTabProxy.h"


// COptionTabProxy �_�C�A���O

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

// COptionTabProxy ���b�Z�[�W �n���h��

void COptionTabProxy::Load()
{
	// �v���L�V
	m_useCheck.SetCheck( theApp.m_optionMng.IsUseProxy() ? BST_CHECKED : BST_UNCHECKED );

	// �O���[�o���v���L�V
	CheckRadioButton( IDC_USE_GLOBAL_PROXY_RADIO, IDC_USE_MANUAL_PROXY_RADIO,
		theApp.m_optionMng.IsUseGlobalProxy() ? IDC_USE_GLOBAL_PROXY_RADIO : IDC_USE_MANUAL_PROXY_RADIO );

	// �v���L�V�T�[�o
	m_serverEdit.SetWindowText( theApp.m_optionMng.GetProxyServer() );

	// �v���L�V�|�[�g�ԍ�
	m_portEdit.SetWindowText( util::int2str(theApp.m_optionMng.GetProxyPort()) );

	// �v���L�V���[�U
	m_userEdit.SetWindowText( theApp.m_optionMng.GetProxyUser() );

	// �v���L�V�p�X���[�h
	m_passwordEdit.SetWindowText( theApp.m_optionMng.GetProxyPassword() );
}

void COptionTabProxy::Save()
{
	CString buf;

	// �Đڑ��t���O�B
	// ��Ԃ��ω������ꍇ�ɍĐڑ����s���B
	bool bReConnect = false;

	// �v���L�V
	bool bUseProxyUpdated = (m_useCheck.GetCheck() == BST_CHECKED) ? true : false;
	if (theApp.m_optionMng.IsUseProxy() != bUseProxyUpdated ) {
		bReConnect = true;
	}
	theApp.m_optionMng.SetUseProxy( bUseProxyUpdated );

	// �O���[�o���v���L�V
	bool bUseGlobalProxyUpdated = IsDlgButtonChecked( IDC_USE_GLOBAL_PROXY_RADIO ) ? true : false;
	if( theApp.m_optionMng.IsUseGlobalProxy() != bUseGlobalProxyUpdated ) {
		bReConnect = true;
	}
	theApp.m_optionMng.SetUseGlobalProxy( bUseGlobalProxyUpdated );

	// �v���L�V�T�[�o
	m_serverEdit.GetWindowText(buf);
	if (theApp.m_optionMng.GetProxyServer() != buf) {
		bReConnect = true;
	}
	theApp.m_optionMng.SetProxyServer(buf);

	// �v���L�V�|�[�g�ԍ�
	m_portEdit.GetWindowText(buf);
	if (theApp.m_optionMng.GetProxyPort() != _wtoi(buf)) {
		bReConnect = true;
	}
	theApp.m_optionMng.SetProxyPort(_wtoi(buf));

	// �v���L�V���[�U
	m_userEdit.GetWindowText(buf);
	theApp.m_optionMng.SetProxyUser(buf);

	// �v���L�V�p�X���[�h
	m_passwordEdit.GetWindowText(buf);
	theApp.m_optionMng.SetProxyPassword(buf);

	if (bReConnect) {
		// �Đڑ��t���O�� ON �Ȃ̂ōăI�[�v��
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
 * �v���L�V�`�F�b�N�{�b�N�X�A���W�I�{�^���̏�Ԃɉ����ĕ\����Ԃ�ύX����
 */
bool COptionTabProxy::DoUpdateControlViewStatus(void)
{
	int radio_buttons[] = { 
		IDC_USE_GLOBAL_PROXY_RADIO, IDC_USE_MANUAL_PROXY_RADIO, -1 };
	int manual_setting_controls[] = { 
		IDC_PROXY_SERVER_EDIT, IDC_PROXY_PORT_EDIT, IDC_PROXY_USER_EDIT, IDC_PROXY_PASSWPRD_EDIT, -1 };

	BOOL bEnableRadioButtons = FALSE;
	BOOL bEnableManualSettings = FALSE;

	// ����
	if( IsDlgButtonChecked( IDC_PROXY_USE_CHECK ) == BST_CHECKED ) {
		// �v���L�V�L�� �� ���W�I�{�^���� Enable �ɁB
		bEnableRadioButtons = TRUE;

		if( IsDlgButtonChecked( IDC_USE_GLOBAL_PROXY_RADIO ) == BST_CHECKED ) {
			// �O���[�o���v���L�V�L�� �� �蓮�ݒ荀�ڂ� Disable �ɁB
			bEnableManualSettings = FALSE;
		}else{
			// �蓮�ݒ�L�� �� �蓮�ݒ荀�ڂ� Enable �ɁB
			bEnableManualSettings = TRUE;
		}
	}else{
		// �v���L�V���� �� ���W�I�{�^���Ǝ蓮�ݒ荀�ڂ� Disable �ɁB
		bEnableRadioButtons = FALSE;
		bEnableManualSettings = FALSE;
	}

	// ��ԕύX

	// ���W�I�{�^��
	for( int i=0; radio_buttons[i] != -1; i++ ) {
		CWnd* p = GetDlgItem( radio_buttons[i] );
		if( p != NULL ) 
			p->EnableWindow( bEnableRadioButtons );
	}
	// �蓮�ݒ荀��
	for( int i=0; manual_setting_controls[i] != -1; i++ ) {
		CWnd* p = GetDlgItem( manual_setting_controls[i] );
		if( p != NULL ) 
			p->EnableWindow( bEnableManualSettings );
	}

	return false;
}

void COptionTabProxy::OnBnClickedProxyUseCheck()
{
	// �R���g���[���̕\����Ԃ̍X�V
	DoUpdateControlViewStatus();
}

void COptionTabProxy::OnBnClickedUseGlobalProxyRadio()
{
	// �R���g���[���̕\����Ԃ̍X�V
	DoUpdateControlViewStatus();
}

void COptionTabProxy::OnBnClickedUseManualProxyRadio()
{
	// �R���g���[���̕\����Ԃ̍X�V
	DoUpdateControlViewStatus();
}
