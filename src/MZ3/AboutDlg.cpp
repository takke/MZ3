/**
 * �o�[�W���������
 */
#include "stdafx.h"
#include "MZ3.h"
#include "AboutDlg.h"
#include "util.h"
#include "ForcedDebugDlg.h"

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BOOL CAboutDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

#ifndef WINCE
	// �o�[�W�����ԍ�
	CWnd* item = NULL;
	if( (item=GetDlgItem( IDC_STATIC_2 )) != NULL ) {
		// ���r�W���������񂩂�ԍ��݂̂𒊏o���A�o�[�W�����ԍ��ɖ��ߍ���

		CString version = MZ3_APP_NAME L" " MZ3_VERSION_TEXT;
		version += util::GetSourceRevision();
		item->SetWindowTextW( version );
	}

	// ���f�[�^��M��
	SetTotalBytes();

	// �A�C�R���̕ύX
	CStatic* icon = NULL;
	if( (icon=(CStatic*)GetDlgItem( IDC_STATIC_1 )) != NULL ) {
		icon->SetIcon( ::LoadIcon(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDR_MAINFRAME_WIN32)) );
	}
#endif

	return TRUE;	// �t�H�[�J�X���R���g���[���ɐݒ肵���ꍇ�������ATRUE ��Ԃ��܂��B
	// ��O: OCX �v���p�e�B �y�[�W�� FALSE ��Ԃ��Ȃ���΂Ȃ�܂���
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	ON_WM_SIZE()
	ON_STN_CLICKED(IDC_STATIC_1, &CAboutDlg::OnStnClickedStatic1)
	ON_BN_CLICKED(IDC_RESET_TOTAL_RECV_BYTES_BUTTON, &CAboutDlg::OnBnClickedResetTotalRecvBytesButton)
END_MESSAGE_MAP()

void CAboutDlg::OnSize(UINT /*nType*/, int /*cx*/, int /*cy*/)
{
#ifdef _DEVICE_RESOLUTION_AWARE
	DRA::RelayoutDialog(
						AfxGetInstanceHandle(), 
						this->m_hWnd, 
						DRA::GetDisplayMode() != DRA::Portrait ? MAKEINTRESOURCE(IDD_ABOUTBOX_WIDE) : MAKEINTRESOURCE(IDD_ABOUTBOX));
#endif

	CWnd* item = NULL;

	// �o�[�W�����ԍ�
	if( (item=GetDlgItem( IDC_STATIC_2 )) != NULL ) {
		// ���r�W���������񂩂�ԍ��݂̂𒊏o���A�o�[�W�����ԍ��ɖ��ߍ���

		CString version = MZ3_APP_NAME L" " MZ3_VERSION_TEXT;
		version += util::GetSourceRevision();
		item->SetWindowTextW( version );
	}

	// ���f�[�^��M��
	SetTotalBytes();
}

void CAboutDlg::OnStnClickedStatic1()
{
}

void CAboutDlg::OnBnClickedResetTotalRecvBytesButton()
{
	if( MessageBox( L"���f�[�^��M�ʂ����Z�b�g���܂����H", 0, MB_YESNO | MB_DEFBUTTON2 | MB_ICONQUESTION ) == IDYES ) {
		theApp.m_optionMng.ResetTotalRecvBytes();
		SetTotalBytes();
	}
}

void CAboutDlg::SetTotalBytes(void)
{
	// ���f�[�^��M��
	CWnd* item = NULL;
	if( (item=GetDlgItem( IDC_TOTAL_BYTES_STATIC )) != NULL ) {
		CString msg;
		msg.Format( L"Total Received : %s bytes", 
			util::int2comma_str( theApp.m_optionMng.GetTotalRecvBytes() ) );
		item->SetWindowTextW( msg );
	}
}

BOOL CAboutDlg::PreTranslateMessage(MSG* pMsg)
{
	switch( pMsg ->message ) {
	case WM_KEYUP:
		{
			// enter by k.c.
			int keys[] = { 046, 046, 050, 050, 045, 047, 045, 047, 0160, 0161 };
			static int pos = 0;
			pos = pos < 0 ? 0 : (pos >= 012 ? 0 : (pMsg->wParam==keys[pos] ? pos+1 : 0));
			if( pos==012 ) {
				CForcedDebugDlg dlg;
				dlg.DoModal();
				pos = 0;
				EndDialog(0);
				break;
			}
		}
#ifdef  MZ3_DEBUG
		{
			// enter by uuu
			static int pos = 0;
			pos = pos < 0 ? 0 : (pos >= 03 ? 0 : (pMsg->wParam==VK_UP?pos+1:0));
			if( pos == 03 ) {
				CForcedDebugDlg dlg;
				dlg.DoModal();
				pos = 0;
				EndDialog(0);
				break;
			}
		}
#endif
		break;
	}

	return CDialog::PreTranslateMessage(pMsg);
}
