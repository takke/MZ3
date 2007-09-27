// OptionTabGetPage.cpp : �����t�@�C��
//

#include "stdafx.h"
#include "MZ3.h"
#include "util.h"
#include "OptionTabGetPage.h"

// this file is not used in smartphone edition
#ifndef SMARTPHONE2003_UI_MODEL


// COptionTabGetPage �_�C�A���O

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


// COptionTabGetPage ���b�Z�[�W �n���h��

void COptionTabGetPage::OnOK()
{
	Save();

	CPropertyPage::OnOK();
}

BOOL COptionTabGetPage::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	// �R���{�{�b�N�X�̏�����
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

	// �I�v�V�������_�C�A���O�ɔ��f����
	Load();

	return TRUE;
}

/**
 * theApp.m_optionMng ����_�C�A���O�ɕϊ�����
 */
void COptionTabGetPage::Load()
{
	// �擾���
	if (theApp.m_optionMng.GetPageType() == 0) {
		((CButton*)GetDlgItem(IDC_PAGE_GETALL_RADIO))->SetCheck(BST_CHECKED);
	}
	else {
		((CButton*)GetDlgItem(IDC_PAGE_GETLATEST_RADIO))->SetCheck(BST_CHECKED);
	}

	// �����ڑ�
	CheckDlgButton( IDC_USE_AUTOCONNECTION_CHECK, 
		theApp.m_optionMng.IsUseAutoConnection() ? BST_CHECKED : BST_UNCHECKED );

	// ��M�o�b�t�@�T�C�Y
	// ���X�g�ɂȂ���Ζ����ɒǉ��B
	const int recvBufSize = theApp.m_optionMng.GetRecvBufSize();
	bool bFound = false;
	for( int i=0; i<mc_RecvBufCombo.GetCount(); i++ ) {
		int bufSize = mc_RecvBufCombo.GetItemData( i );
		if( bufSize == recvBufSize ) {
			// ��v�����̂őI��
			mc_RecvBufCombo.SetCurSel( i );
			bFound = true;
			break;
		}
	}
	if( !bFound ) {
		// ������Ȃ������̂Ŗ����ɒǉ��A�I���B
		CString s;
		s.Format( L"%d Byte", recvBufSize );
		int idx = mc_RecvBufCombo.InsertString( mc_RecvBufCombo.GetCount(), s );
		mc_RecvBufCombo.SetItemData( idx, recvBufSize );

		mc_RecvBufCombo.SetCurSel( idx );
	}
}

/**
 * �_�C�A���O�̃f�[�^�� theApp.m_optionMng �ɕϊ�����
 */
void COptionTabGetPage::Save()
{
	CString buf;

	// �Đڑ��t���O�B
	// ��Ԃ��ω������ꍇ�ɍĐڑ����s���B
	bool bReConnect = false;


	// �擾���
	GETPAGE_TYPE type;
	if (((CButton*)GetDlgItem(IDC_PAGE_GETALL_RADIO))->GetCheck() == BST_CHECKED) {
		type = GETPAGE_ALL;
	}
	else {
		type = GETPAGE_LATEST10;
	}
	theApp.m_optionMng.SetPageType(type);

	// ��M�o�b�t�@�T�C�Y
	theApp.m_optionMng.SetRecvBufSize( mc_RecvBufCombo.GetItemData( mc_RecvBufCombo.GetCurSel() ) );

	// �����ڑ�
	bool bUseAutoConnectionUpdated = IsDlgButtonChecked( IDC_USE_AUTOCONNECTION_CHECK ) == BST_CHECKED ? true : false;
	if( theApp.m_optionMng.IsUseAutoConnection() != bUseAutoConnectionUpdated ) {
		bReConnect = true;
	}
	theApp.m_optionMng.SetUseAutoConnection( bUseAutoConnectionUpdated );

	if (bReConnect) {
		// �Đڑ��t���O�� ON �Ȃ̂ŃN���[�Y����i�Đڑ��͕K�v���ɍs����j
		theApp.m_inet.CloseInternetHandles();
	}
}

#endif