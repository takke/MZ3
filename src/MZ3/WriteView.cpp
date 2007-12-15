/*
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */
// WriteView.cpp : �����t�@�C��
//

#include "stdafx.h"
#include "MZ3.h"
#include "WriteView.h"
#include "ReportView.h"
#include "MZ3View.h"
#include "WMMenu.h"
#include "kfm.h"
#include "HtmlArray.h"
#include "MainFrm.h"
#include "util.h"
#include "util_gui.h"
#include "url_encoder.h"
#include "PostDataGenerator.h"
#include "MixiParser.h"

#define GENERATE_POSTMSG_FAILED_MESSAGE L"���M�f�[�^�̍쐬�Ɏ��s���܂���"

// CWriteView

IMPLEMENT_DYNCREATE(CWriteView, CFormView)

// -----------------------------------------------------------------------------
// �R���X�g���N�^
// -----------------------------------------------------------------------------
CWriteView::CWriteView()
	: CFormView(CWriteView::IDD)
	, m_abort(FALSE)
{
	m_postData = new CPostData();
	m_sendEnd = TRUE;
}

// -----------------------------------------------------------------------------
// �f�X�g���N�^
// -----------------------------------------------------------------------------
CWriteView::~CWriteView()
{
	delete m_postData;
	m_postData = NULL;
}

void CWriteView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_WRITE_INFO_EDIT, m_infoEdit);
	DDX_Control(pDX, IDC_WRITE_SEND_BUTTON, m_sendButton);
	DDX_Control(pDX, IDC_WRITE_CANCEL_BUTTON, m_cancelButton);
	DDX_Control(pDX, IDC_WRITE_BODY_EDIT, m_bodyEdit);
	DDX_Control(pDX, IDC_WRITE_TITLE_EDIT, m_titleEdit);
}

BEGIN_MESSAGE_MAP(CWriteView, CFormView)
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_WRITE_SEND_BUTTON, &CWriteView::OnBnClickedWriteSendButton)
	ON_BN_CLICKED(IDC_WRITE_CANCEL_BUTTON, &CWriteView::OnBnClickedWriteCancelButton)
    ON_MESSAGE(WM_MZ3_GET_END, OnGetEnd)
    ON_MESSAGE(WM_MZ3_POST_CONFIRM, OnPostConfirm)
    ON_MESSAGE(WM_MZ3_POST_END, OnPostEnd)
    ON_MESSAGE(WM_MZ3_POST_ABORT, OnPostAbort)
    ON_MESSAGE(WM_MZ3_ABORT, OnAbort)
    ON_MESSAGE(WM_MZ3_ACCESS_INFORMATION, OnAccessInformation)
	ON_COMMAND(ID_EDIT_CUT, &CWriteView::OnEditCut)
	ON_COMMAND(ID_EDIT_COPY, &CWriteView::OnEditCopy)
	ON_COMMAND(ID_EDIT_PASTE, &CWriteView::OnEditPaste)
	ON_COMMAND(ID_EDIT_UNDO, &CWriteView::OnEditUndo)
	ON_COMMAND(ID_SELECT_ALL, &CWriteView::OnSelectAll)
	ON_COMMAND(ID_WRITE_SEND_MENU, &CWriteView::OnWriteSendMenu)
	ON_COMMAND(ID_WRITE_CANCEL_MENU, &CWriteView::OnWriteCancelMenu)
	ON_COMMAND(ID_WRITE_BACK_MENU, &CWriteView::OnWriteBackMenu)
    ON_MESSAGE(WM_MZ3_FIT, OnFit)
	ON_WM_SETFOCUS()
    ON_COMMAND(ID_IMAGE_BUTTON, OnImageButton)
	ON_COMMAND(ID_ATTACH_PHOTO1, &CWriteView::OnAttachPhoto1)
	ON_COMMAND(ID_ATTACH_PHOTO2, &CWriteView::OnAttachPhoto2)
	ON_COMMAND(ID_ATTACH_PHOTO3, &CWriteView::OnAttachPhoto3)
	ON_COMMAND(ID_CANCEL_ATTACH_PHOTO1, &CWriteView::OnCancelAttachPhoto1)
	ON_COMMAND(ID_CANCEL_ATTACH_PHOTO2, &CWriteView::OnCancelAttachPhoto2)
	ON_COMMAND(ID_CANCEL_ATTACH_PHOTO3, &CWriteView::OnCancelAttachPhoto3)
	ON_COMMAND(ID_ATTACH_PHOTO, &CWriteView::OnAttachPhoto)
	ON_COMMAND(ID_PREVIEW_ATTACHED_PHOTO1, &CWriteView::OnPreviewAttachedPhoto1)
	ON_COMMAND(ID_PREVIEW_ATTACHED_PHOTO2, &CWriteView::OnPreviewAttachedPhoto2)
	ON_COMMAND(ID_PREVIEW_ATTACHED_PHOTO3, &CWriteView::OnPreviewAttachedPhoto3)
	ON_COMMAND_RANGE(IDM_INSERT_EMOJI_BEGIN, IDM_INSERT_EMOJI_BEGIN+1000, &CWriteView::OnInsertEmoji)
END_MESSAGE_MAP()


// CWriteView �f�f

#ifdef _DEBUG
void CWriteView::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CWriteView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


// CWriteView ���b�Z�[�W �n���h��

void CWriteView::OnInitialUpdate()
{
	GetParentFrame()->RecalcLayout();
	CFormView::OnInitialUpdate();

	// �t�H���g�̕ύX
	{
		// �^�C�g��
		m_titleEdit.SetFont( &theApp.m_font );

		// �G�f�B�b�g
		m_bodyEdit.SetFont( &theApp.m_font );

		// �{�^��
		m_sendButton.SetFont( &theApp.m_font );
		m_cancelButton.SetFont( &theApp.m_font );

		// �ʒm�̈�
		m_infoEdit.SetFont( &theApp.m_font );
	}
}

void CWriteView::OnSize(UINT nType, int cx, int cy)
{
	// �O��̒l��ۑ����A(0,0) �̏ꍇ�͂��̒l�𗘗p����
	static int s_cx = 0;
	static int s_cy = 0;
	if (cx==0 && cy==0) {
		cx = s_cx;
		cy = s_cy;
	} else {
		s_cx = cx;
		s_cy = cy;
	}

	CFormView::OnSize(nType, cx, cy);

	int hEdit = theApp.GetInfoRegionHeight(theApp.m_optionMng.m_fontHeight);
	int yBtn  = 144;

	switch( theApp.GetDisplayMode() ) {
	case SR_VGA:
		yBtn  = 144;
		break;
	case SR_QVGA:
	default:
		yBtn  = 120;
		break;
	}

	util::MoveDlgItemWindow( this, IDC_WRITE_TITLE_EDIT,       0,        0,   cx, hEdit);
	util::MoveDlgItemWindow( this, IDC_WRITE_BODY_EDIT,        0,    hEdit,   cx, cy-hEdit*2);
	util::MoveDlgItemWindow( this, IDC_WRITE_INFO_EDIT,        0, cy-hEdit,   cx, hEdit);
	util::MoveDlgItemWindow( this, IDC_WRITE_SEND_BUTTON,      0, cy-hEdit, yBtn, hEdit);
	util::MoveDlgItemWindow( this, IDC_WRITE_CANCEL_BUTTON, yBtn, cy-hEdit, yBtn, hEdit);
}

// -----------------------------------------------------------------------------
// �������݃{�^���������̏���
// -----------------------------------------------------------------------------
void CWriteView::OnBnClickedWriteSendButton()
{
	CString msg;
	GetDlgItemText( IDC_WRITE_BODY_EDIT, msg );

	CString title;
	GetDlgItemText( IDC_WRITE_TITLE_EDIT, title );

	if (msg.GetLength() > 3000) {
		// �ő啶�����z���Ă���
		::MessageBox(m_hWnd, L"���M���������������܂�", MZ3_APP_NAME, MB_ICONERROR);
		return;
	}
	else if (title.GetLength() == 0) {
		::MessageBox(m_hWnd, L"�^�C�g������͂��Ă�������", MZ3_APP_NAME, MB_ICONERROR);
		return;
	}
	else if (msg.GetLength() == 0) {
		::MessageBox(m_hWnd, L"�{������͂��Ă�������", MZ3_APP_NAME, MB_ICONERROR);
		return;
	}

	{
		CString s;
		switch( m_writeViewType ) {
		case WRITEVIEW_TYPE_COMMENT:
			s = L"�R�����g�𓊍e���܂�\n��낵���ł����H";
			break;

		case WRITEVIEW_TYPE_NEWDIARY:
			s = L"���L�𓊍e���܂�\n��낵���ł����H";
			break;

		case WRITEVIEW_TYPE_REPLYMESSAGE:
			s = L"���b�Z�[�W��ԐM���܂�\n��낵���ł����H";
			break;

		case WRITEVIEW_TYPE_NEWMESSAGE:
			s = L"���b�Z�[�W�𑗐M���܂�\n��낵���ł����H";
			break;
		}

		// �Y�t�摜������΃t�@�C������ǉ��B
		if( !m_photo1_filepath.IsEmpty() ||
			!m_photo2_filepath.IsEmpty() ||
			!m_photo3_filepath.IsEmpty() ) 
		{
			s.Append( L"\n\n" );
			s.Append( L"�Y�t�摜�F\n" );
			if( !m_photo1_filepath.IsEmpty() ) {
				s.Append( m_photo1_filepath + L"\n" );
			}
			if( !m_photo2_filepath.IsEmpty() ) {
				s.Append( m_photo2_filepath + L"\n" );
			}
			if( !m_photo3_filepath.IsEmpty() ) {
				s.Append( m_photo3_filepath + L"\n" );
			}
		}

		int ret = ::MessageBox(m_hWnd, s, MZ3_APP_NAME, MB_ICONQUESTION | MB_OKCANCEL);
		if (ret == IDCANCEL) {
			// �����𒆎~
			return;
		}
	}

	m_access = TRUE;
	m_sendButton.EnableWindow(FALSE);
	m_cancelButton.EnableWindow(FALSE);
	m_infoEdit.ShowWindow(SW_SHOW);

	if (wcslen(theApp.m_loginMng.GetOwnerID()) == 0) {
		MZ3LOGGER_INFO( L"OwnerID�����擾�Ȃ̂ŁA���O�C�����A�擾����(1)" );

		theApp.m_mixi4recv.SetAccessType(ACCESS_LOGIN);
		theApp.EnableCommandBarButton( ID_STOP_BUTTON, TRUE);
		theApp.m_accessType = ACCESS_LOGIN;
		theApp.m_inet.Initialize( m_hWnd, &theApp.m_mixi4recv );
		theApp.m_inet.DoGet(theApp.MakeLoginUrl(), L"", CInetAccess::FILE_HTML );
		return;
	}

	m_infoEdit.ShowWindow(SW_SHOW);
	theApp.EnableCommandBarButton( ID_BACK_BUTTON, FALSE);
	theApp.EnableCommandBarButton( ID_WRITE_BUTTON, FALSE);

	StartConfirmPost( msg );

	// �L�[�����C�x���g��D�����߂Ƀt�H�[�J�X���擾����
	SetFocus();
}

// -----------------------------------------------------------------------------
// ���~�{�^���������̏���
// -----------------------------------------------------------------------------
void CWriteView::OnBnClickedWriteCancelButton()
{
	int ret;
	CString msg;
	((CEdit*)GetDlgItem(IDC_WRITE_BODY_EDIT))->GetWindowText(msg);

	if (msg.GetLength() > 0) {
		ret = ::MessageBox(m_hWnd, L"�����e�̃f�[�^������܂�\n�j������܂��������ł����H",
			MZ3_APP_NAME, MB_ICONQUESTION | MB_OKCANCEL);
		if (ret == IDCANCEL) {
			// �����𒆎~
			return;
		}
	}

	// ���e���N���A
	((CEdit*)GetDlgItem(IDC_WRITE_TITLE_EDIT))->SetWindowText(L"");
	((CEdit*)GetDlgItem(IDC_WRITE_BODY_EDIT))->SetWindowText(L"");

	// �O�̉�ʂɖ߂�
	if (IsWriteFromMainView() == FALSE) {
		::SendMessage(theApp.m_pReportView->m_hWnd, WM_MZ3_CHANGE_VIEW, NULL, NULL);
	}
	else {
		::SendMessage(theApp.m_pMainView->m_hWnd, WM_MZ3_CHANGE_VIEW, NULL, NULL);
	}
	theApp.EnableCommandBarButton( ID_FORWARD_BUTTON, FALSE);

	m_sendEnd = TRUE;
}

/**
 * �������݉�ʂ̓��e�i���m�F��ʂ̕\���j�����B�������݂̃V�~�����[�g�B
 */
LRESULT CWriteView::OnPostConfirm(WPARAM wParam, LPARAM lParam)
{
	if (m_abort != FALSE) {
		::SendMessage(m_hWnd, WM_MZ3_POST_ABORT, NULL, NULL);
		return TRUE;
	}

	// ���O�A�E�g�`�F�b�N
	bool bLogout = false;
	if (mixi::MixiParserBase::isLogout(theApp.m_filepath.temphtml) ) {
		bLogout = true;
	} else if (wcslen(theApp.m_loginMng.GetOwnerID())==0) {
		// �I�[�i�[ID���擾�̏ꍇ�����O�A�E�g�Ƃ݂Ȃ��B
		bLogout = true;
	}

	if (bLogout) {
		// ���O�A�E�g��ԂɂȂ��Ă���
		// ���O�C���������{
		MZ3LOGGER_INFO(_T("���O�C�����܂��B"));
		theApp.m_mixi4recv.SetAccessType(ACCESS_LOGIN);

		theApp.m_accessType = ACCESS_LOGIN;
		theApp.m_inet.Initialize( m_hWnd, &theApp.m_mixi4recv );
		theApp.m_inet.DoGet(theApp.MakeLoginUrl(), NULL, CInetAccess::FILE_HTML );

		return TRUE;
	}

	// �m�F��ʔ���
	CHtmlArray html;
	html.Load( theApp.m_filepath.temphtml );

	// ��͗p�Ɋm�F��ʂ�ۑ����Ă���
	CString strConfirmTempFile = theApp.m_filepath.temphtml + L".confirm.html";
	CopyFile( theApp.m_filepath.temphtml, strConfirmTempFile, FALSE );

	// �m�F��ʂ̉��
	if( html.GetPostConfirmData(m_postData) == false ) {
		CString msg = 
			L"�G���[���������܂���\r\n"
			L"�m�F��ʂɃG���[���e���\������Ă��܂����A"
			L"�{�o�[�W������" MZ3_APP_NAME L"�ł̓G���[���e��\���ł��܂���E�E�E\r\n"
			L"���萔�ł����A���L�t�@�C���𒼐ډ�͂��Ă�������m(_ _)m\r\n"
			+ strConfirmTempFile;
		MessageBox( msg );
		MZ3LOGGER_ERROR( msg );

		MZ3LOGGER_ERROR( L"�������݂Ɏ��s�������߁A�O�̂���OwnerID�����������܂�" );
		theApp.m_loginMng.SetOwnerID( L"" );

		return 0;
	}

	// �������݊J�n
	StartEntryPost();

	return TRUE;
}

/**
 * �m�F��ʁb�������݃{�^���������̓d�����M
 */
void CWriteView::StartEntryPost() 
{

	// ���f�m�F
	if (m_abort != FALSE) {
		::SendMessage(m_hWnd, WM_MZ3_POST_ABORT, NULL, NULL);
		return;
	}
	m_abort = FALSE;

	CString url;	// URL
	CString refUrl;	// ���t�@���[�pURL

	switch (m_writeViewType) {
	case WRITEVIEW_TYPE_COMMENT:
		{
			// �d���̐���
			if( !mixi::EntryCommentGenerator::generate( *m_postData, *m_data ) ) {
				MessageBox( GENERATE_POSTMSG_FAILED_MESSAGE );
				return;
			}

			// URL �̐���
			switch (m_data->GetAccessType()) {
			case ACCESS_BBS:
			case ACCESS_EVENT:
			case ACCESS_ENQUETE:
				url.Format( L"http://mixi.jp/%s", m_data->GetPostAddress());
				break;

			case ACCESS_DIARY:
			case ACCESS_MYDIARY:
				url.Format( L"http://mixi.jp/%s&owner_id=%d", m_data->GetPostAddress(), m_data->GetOwnerID());
				//���t�@���ݒ肵���瓊�e�Ŏ~�܂�悤�ɂȂ����̂ŕۗ�
				//refUrl.Format( L"http://mixi.jp/%s", m_data->GetPostAddress());
				break;
			}
		}
		break;

	case WRITEVIEW_TYPE_NEWDIARY:
		{
			// �^�C�g�����擾����
			CString title;
			GetDlgItemText( IDC_WRITE_TITLE_EDIT, title );

			// �d���̐���
			if( !mixi::EntryDiaryGenerator::generate( *m_postData, title ) ) {
				MessageBox( GENERATE_POSTMSG_FAILED_MESSAGE );
				return;
			}

			// URL �̐���
			url = L"http://mixi.jp/add_diary.pl";
		}
		break;

	case WRITEVIEW_TYPE_REPLYMESSAGE:
		{
			// �^�C�g�����擾����
			CString title;
			GetDlgItemText( IDC_WRITE_TITLE_EDIT, title );

			// �d���̐���
			if( !mixi::EntryReplyMessageGenerator::generate( *m_postData, title ) ) {
				MessageBox( GENERATE_POSTMSG_FAILED_MESSAGE );
				return;
			}

			// URL �̐���
			url    = m_postData->GetConfirmUri();
			refUrl = m_postData->GetConfirmUri();
		}
		break;

	case WRITEVIEW_TYPE_NEWMESSAGE:
		{
			// �^�C�g�����擾����
			CString title;
			GetDlgItemText( IDC_WRITE_TITLE_EDIT, title );

			// �d���̐���
			if( !mixi::EntryNewMessageGenerator::generate( *m_postData, title ) ) {
				MessageBox( GENERATE_POSTMSG_FAILED_MESSAGE );
				return;
			}

			// URL �̐���
			url    = m_postData->GetConfirmUri();
			refUrl = m_postData->GetConfirmUri();
		}
		break;

	default:
		{
			CString s;
			s.Format( L"���T�|�[�g�̑��M��ʂł� [%d]", m_writeViewType );
			MessageBox( s );

			return;
		}
		break;
	}

	// �������̃��b�Z�[�W��ݒ肷��
	m_postData->SetSuccessMessage( WM_MZ3_POST_END );

	// �A�N�Z�X��ʂ̐ݒ�
	switch (m_writeViewType) {
	case WRITEVIEW_TYPE_COMMENT:		theApp.m_accessType = ACCESS_POST_ENTRY_COMMENT;		break;
	case WRITEVIEW_TYPE_NEWDIARY:		theApp.m_accessType = ACCESS_POST_ENTRY_NEWDIARY;		break;
	case WRITEVIEW_TYPE_REPLYMESSAGE:	theApp.m_accessType = ACCESS_POST_ENTRY_REPLYMESSAGE;	break;
	case WRITEVIEW_TYPE_NEWMESSAGE:		theApp.m_accessType = ACCESS_POST_ENTRY_NEWMESSAGE;		break;
	default:							theApp.m_accessType = ACCESS_MAIN;						break;
	}

	theApp.m_inet.Initialize( m_hWnd, NULL );
	theApp.m_inet.DoPost(
		url, 
		refUrl, 
		CInetAccess::FILE_HTML, 
		m_postData );
}

/**
 * �������݊����i�m�F��ʁb�������݃{�^�������ː����j
 */
LRESULT CWriteView::OnPostEnd(WPARAM wParam, LPARAM lParam)
{
	CHtmlArray html;
	html.Load( theApp.m_filepath.temphtml );

	theApp.EnableCommandBarButton( ID_STOP_BUTTON, FALSE);

	if (html.IsPostSucceeded(m_writeViewType) != FALSE) {
#ifdef	MZ3_DEBUG
		// only for Release_MZ3 configuration
		DumpToTemporaryDraftFile();
#endif
		if (m_abort == FALSE) {
			switch (m_writeViewType ) {
			case WRITEVIEW_TYPE_REPLYMESSAGE:
			case WRITEVIEW_TYPE_NEWMESSAGE:
				::MessageBox(m_hWnd, L"���b�Z�[�W�𑗐M���܂���", MZ3_APP_NAME, MB_OK);
				break;
			default:
				::MessageBox(m_hWnd, L"���e���܂���", MZ3_APP_NAME, MB_OK);
				break;
			}
		}
		else {
			// ���f��������Ă����ꍇ�͏�ɕԂ�
			return TRUE;
		}

		((CEdit*)GetDlgItem(IDC_WRITE_TITLE_EDIT))->SetWindowText(L"");
		((CEdit*)GetDlgItem(IDC_WRITE_BODY_EDIT))->SetWindowText(L"");

		// �i�ރ{�^�����g�p�s�ɂ���
		theApp.EnableCommandBarButton( ID_FORWARD_BUTTON, FALSE);

		if( IsWriteFromMainView() ) {
			theApp.EnableCommandBarButton( ID_BACK_BUTTON, FALSE);
		} else {
			theApp.EnableCommandBarButton( ID_BACK_BUTTON, TRUE);
		}
		m_sendEnd = TRUE;

		// �O�̉�ʂɖ߂�
		if( IsWriteFromMainView() ) {
			::SendMessage(theApp.m_pMainView->m_hWnd, WM_MZ3_CHANGE_VIEW, NULL, NULL);
		}else{
			::SendMessage(theApp.m_pReportView->m_hWnd, WM_MZ3_CHANGE_VIEW, NULL, NULL);

			switch (m_writeViewType ) {
			case WRITEVIEW_TYPE_REPLYMESSAGE:
			case WRITEVIEW_TYPE_NEWMESSAGE:
				// ���b�Z�[�W�Ȃ烊���[�h���Ȃ�
				break;
			default:
				::SendMessage(theApp.m_pReportView->m_hWnd, WM_MZ3_RELOAD, NULL, NULL);
				break;
			}
		}
	}
	else {
		LPCTSTR msg = L"���e�Ɏ��s���܂���(1)";
		util::MySetInformationText( m_hWnd, msg );

		MZ3LOGGER_ERROR( msg );

		CString s;
		s.Format( 
			L"%s\n\n"
			L"%s �ɍ���̏������ݓ��e��ۑ����܂����B", msg, theApp.m_filepath.tempdraftfile );
		::MessageBox(m_hWnd, s, MZ3_APP_NAME, MB_ICONERROR);

		// ���s�����̂Ń_���v����
		DumpToTemporaryDraftFile();

		theApp.EnableCommandBarButton( ID_BACK_BUTTON, TRUE );
		m_sendButton.EnableWindow(TRUE);
		m_cancelButton.EnableWindow(FALSE);
	}

	m_infoEdit.ShowWindow( SW_HIDE );

	m_access = FALSE;

	return TRUE;
}

/**
 * �������ݓ��e���_���v����
 */
bool CWriteView::DumpToTemporaryDraftFile()
{
	FILE* fp = _wfopen( theApp.m_filepath.tempdraftfile, L"w" );

	if( fp == NULL ) {
		MZ3LOGGER_FATAL( L"�_���v�t�@�C���I�[�v�����s" );
		return false;
	}

	// 1�s�ځF�^�C�g��
	CString title;
	GetDlgItemText( IDC_WRITE_TITLE_EDIT, title );
	fwprintf( fp, L"%s\n", title );

	// 2�s�ڈȍ~�F�{��
	CString msg;
	GetDlgItemText( IDC_WRITE_BODY_EDIT, msg );
	fwprintf( fp, L"%s\n", msg );

	fclose( fp );

	return true;
}

// -----------------------------------------------------------------------------
// ���f
// -----------------------------------------------------------------------------
LRESULT CWriteView::OnPostAbort(WPARAM wParam, LPARAM lParam)
{
	// ���~�{�^�����g�p�s�ɂ���
	theApp.EnableCommandBarButton( ID_STOP_BUTTON, FALSE);
	theApp.EnableCommandBarButton( ID_BACK_BUTTON, TRUE);

	m_sendButton.EnableWindow(TRUE);
	m_cancelButton.EnableWindow(TRUE);

	m_infoEdit.ShowWindow(SW_HIDE);

	m_access = FALSE;

	return TRUE;
}

/**
 * ���f�{�^���������̏���
 */
LRESULT CWriteView::OnAbort(WPARAM wParam, LPARAM lParam)
{
	if( theApp.m_inet.IsConnecting() ) {
		// �ʐM���Ȃ�΁AAbort ���Ăяo��
		theApp.m_inet.Abort();
	}
	m_abort = TRUE;

	LPCTSTR msg = L"";
	if (m_postData->GetSuccessMessage() == WM_MZ3_POST_CONFIRM) {
		msg = L"���f���܂������A���e���ꂽ�\��������܂�";
	}
	else {
		msg = L"���f���܂���";
	}
	util::MySetInformationText( m_hWnd, msg );
	::MessageBox(m_hWnd, msg, MZ3_APP_NAME, MB_ICONSTOP | MB_OK);

	// ���~�{�^�����g�p�s�ɂ���
	theApp.EnableCommandBarButton( ID_STOP_BUTTON, FALSE);
	theApp.EnableCommandBarButton( ID_BACK_BUTTON, TRUE);

	m_sendButton.EnableWindow(TRUE);
	m_cancelButton.EnableWindow(TRUE);

	m_infoEdit.ShowWindow(SW_HIDE);

	// ���f��ɏ������݃{�^�����������Ɖ��̂������I�����Ă��܂����߁A
	// �{���̈�Ƀt�H�[�J�X��߂��B
	m_bodyEdit.SetFocus();

	return TRUE;
}

// -----------------------------------------------------------------------------
// �A�N�Z�X���ʒm
// -----------------------------------------------------------------------------
LRESULT CWriteView::OnAccessInformation(WPARAM wParam, LPARAM lParam)
{
	m_infoEdit.SetWindowText(*(CString*)lParam);
	return TRUE;
}

BOOL CWriteView::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYUP) {
		switch (pMsg->wParam) {
		case VK_F1:
#ifdef WINCE
			if( theApp.m_optionMng.m_bUseLeftSoftKey ) {
				// ���C�����j���[�̃|�b�v�A�b�v
				RECT rect;
				int flags = TPM_CENTERALIGN | TPM_VCENTERALIGN;
				SystemParametersInfo(SPI_GETWORKAREA, 0, &rect, 0);

				CMenu menu;
				CMainFrame* pMainFrame = (CMainFrame*)theApp.m_pMainWnd;
				if( theApp.m_bPocketPC ) {
					menu.Attach( pMainFrame->m_wndCommandBar.GetMenu() );
				} else {
					menu.LoadMenu(IDR_MAINFRAME);
				}
				menu.GetSubMenu(0)->TrackPopupMenu(flags,
					rect.left,
					rect.bottom,
					pMainFrame );
				menu.Detach();
				return TRUE;
			}
#else
			// �w���v�\��
			util::OpenByShellExecute( MZ3_CHM_HELPFILENAME );
#endif
			break;

		case VK_F2:
		case VK_F10:
#ifndef WINCE
		case VK_APPS:
#endif
			PopupWriteBodyMenu();
			break;

		case VK_BACK:
#ifndef WINCE
		case VK_ESCAPE:
#endif
			if (m_access != FALSE) {
				// �A�N�Z�X���͒��f����
				::SendMessage(m_hWnd, WM_MZ3_ABORT, NULL, NULL);
				return TRUE;
			}
			break;
		}

	}

	return CFormView::PreTranslateMessage(pMsg);
}

// -----------------------------------------------------------------------------
// �O�̉�ʂɐ؂�ւ���
// -----------------------------------------------------------------------------
void CWriteView::OnWriteBackMenu()
{
	CMainFrame* pMainFrame = (CMainFrame*)theApp.m_pMainWnd;
	pMainFrame->OnBackButton();
}

LRESULT CWriteView::OnFit(WPARAM wParam, LPARAM lParam)
{
	RECT rect;
	SystemParametersInfo(SPI_GETWORKAREA, 0, &rect, 0);
	if( theApp.m_bPocketPC ) {
		OnSize(SIZE_RESTORED, rect.right - rect.left, rect.bottom - (rect.top*2));
	}

	return TRUE;
}

LRESULT CWriteView::OnGetEnd(WPARAM wParam, LPARAM lParam)
{
	util::MySetInformationText( m_hWnd, L"HTML��͒�" );

	CHtmlArray html;
	html.Load( theApp.m_filepath.temphtml );

	if (m_abort != FALSE) {
		::SendMessage(m_hWnd, WM_MZ3_POST_ABORT, NULL, lParam);
		return TRUE;
	}

	switch( ((CMixiData*)lParam)->GetAccessType() ) {
	case ACCESS_LOGIN:
		// ���O�C���������ǂ����̊m�F
		if( mixi::HomeParser::IsLoginSucceeded(html) ) {
			// ���O�C������
			if (wcslen(theApp.m_loginMng.GetOwnerID()) != 0) {
				MZ3LOGGER_DEBUG( L"OwnerID �擾�ς�" );
			} else {
				MZ3LOGGER_INFO( L"OwnerID�����擾�Ȃ̂ŁA���O�C�����A�擾���� (2)" );

				((CMixiData*)lParam)->SetAccessType(ACCESS_MAIN);
				theApp.m_accessType = ACCESS_MAIN;
				theApp.m_inet.DoGet(L"http://mixi.jp/check.pl?n=%2Fhome.pl", L"", CInetAccess::FILE_HTML );
				return TRUE;
			}
		} else {
			// ���O�C�����s
			LPCTSTR msg = L"���O�C���Ɏ��s���܂���";
			util::MySetInformationText( m_hWnd, msg );
			::MessageBox(m_hWnd, msg, MZ3_APP_NAME, MB_ICONERROR);

			::SendMessage(m_hWnd, WM_MZ3_POST_ABORT, NULL, lParam);
			return TRUE;
		}
		break;

	case ACCESS_MAIN:

		mixi::HomeParser::parse( html );

		if (wcslen(theApp.m_loginMng.GetOwnerID()) == 0) {
			LPCTSTR msg = L"���e�Ɏ��s���܂���(2)";
			util::MySetInformationText( m_hWnd, msg );

			MZ3LOGGER_ERROR( msg );

			CString s;
			s.Format( 
				L"%s\n\n"
				L"%s �ɍ���̏������ݓ��e��ۑ����܂����B", msg, theApp.m_filepath.tempdraftfile );
			::MessageBox(m_hWnd, s, MZ3_APP_NAME, MB_ICONERROR);

			// ���s�����̂Ń_���v����
			DumpToTemporaryDraftFile();

			::SendMessage(m_hWnd, WM_MZ3_POST_ABORT, NULL, lParam);
			return TRUE;
		}
		break;
	}

	CString msg;
	GetDlgItemText( IDC_WRITE_BODY_EDIT, msg );

	// POST �����𑱍s
	StartConfirmPost( msg );

	return TRUE;
}

void CWriteView::OnSetFocus(CWnd* pOldWnd)
{
	CFormView::OnSetFocus(pOldWnd);

}

/**
 * �摜�Y�t�m�F��ʂ̕\�����摜�t�@�C�����̎擾
 */
CString CWriteView::MyShowDlgToConfirmPostImage( CString selectedFilepath )
{
	// �I���ς݂ł���ΕύX���邩�K�˂�
	if( !selectedFilepath.IsEmpty() ) {
		CString msg;
		msg.Format( 
			L"�Y�t�t�@�C����ύX���܂����H\n"
			L"\n"
			L"�t�@�C���F%s\n"
			L"\n"
			L"�y�͂��z�ύX����\n"
			L"�y�������z�ύX���Ȃ�", (LPCTSTR) selectedFilepath );
		if( MessageBox( msg, 0, MB_YESNO ) == IDNO ) {
			// �������i�ύX���Ȃ��j�Ȃ̂ŁA���̃t�@�C����Ԃ�
			return selectedFilepath;
		}
	}

	WCHAR szFile[MAX_PATH] = L"\0";
	if( !selectedFilepath.IsEmpty() ) {
		wcscpy( szFile, selectedFilepath );
		szFile[ selectedFilepath.GetLength() +1 ] = '\0';
	}

	OPENFILENAME ofn;
	memset( &(ofn), 0, sizeof(ofn) );
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = m_hWnd;
	ofn.lpstrFile = szFile;
	ofn.nMaxFile = MAX_PATH; 
	ofn.lpstrTitle = L"JPEĢ�ق��J��...";
	//�t�B���^����ɂ���ƃz�[�~�������FileDialgChanger�C���X�g�[�����ɉ摜�T���l�C���ɑΉ��ł��邽�ߕύX
	//ofn.lpstrFilter = L"JPEĢ�� (*.jpg)\0*.jpg;*.jpeg\0���ׂĂ�̧�� (*.*)\0*.*\0\0";
	ofn.lpstrFilter = L"JPEĢ�� (*.jpg)\0*.jpg;*.jpeg\0\0";
	ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST;
	ofn.lpstrInitialDir = L"";
	if (GetOpenFileName(&ofn) == IDOK) {
		return szFile;
	}

	// �L�����Z�����ꂽ�̂Ō��̃t�@�C����Ԃ�
	return selectedFilepath;
}

/**
 * �C���[�W�{�^���������̏���
 */
void CWriteView::OnImageButton()
{
	POINT pt    = util::GetPopupPos();
	int   flags = util::GetPopupFlags();

	CMenu menu;
	menu.LoadMenu( IDR_ATTACH_IMAGE_MENU );
	CMenu* pcThisMenu = menu.GetSubMenu(0);

	// ���Y�t�ς݂̏ꍇ
	// �@�u�Y�t�t�@�C����ύX����v
	// �@�u�Y�t����߂�v
	// �����Y�t�̏ꍇ
	// �@�u�Y�t�t�@�C����I������v
	LPCTSTR photo_files[] = { m_photo1_filepath, m_photo2_filepath, m_photo3_filepath };
	for( int i=0; i<3; i++ ) {
		CMenu* pSubMenu = pcThisMenu->GetSubMenu(i);

		int id_attach = ID_ATTACH_PHOTO1+i;
		int id_cancel = ID_CANCEL_ATTACH_PHOTO1+i;
		int id_preview = ID_PREVIEW_ATTACHED_PHOTO1+i;
		if( wcslen(photo_files[i])==0 ) {
			// ���Y�t
			pSubMenu->ModifyMenu( id_attach, MF_BYCOMMAND, id_attach, _T("�t�@�C����I������") );
			pSubMenu->RemoveMenu( id_cancel, MF_BYCOMMAND );
			pSubMenu->RemoveMenu( id_preview, MF_BYCOMMAND );
		}else{
			// �Y�t�ς�
			pSubMenu->ModifyMenu( id_attach, MF_BYCOMMAND, id_attach, _T("�t�@�C����ύX����") );
			pSubMenu->ModifyMenu( id_cancel, MF_BYCOMMAND, id_cancel, _T("�Y�t����߂�") );
		}
	}

	pcThisMenu->TrackPopupMenu( flags, pt.x, pt.y, this );
}

/**
 * �������݉�ʂ��J�n����B
 */
void CWriteView::StartWriteView(WRITEVIEW_TYPE writeViewType, CMixiData* pMixi)
{
	m_data = pMixi;
	m_access = FALSE;

	// ���e���N���A
	SetDlgItemText( IDC_WRITE_TITLE_EDIT, L"" );
	SetDlgItemText( IDC_WRITE_BODY_EDIT, L"" );

	// ��ʂ�ۑ�
	m_writeViewType = writeViewType;

	// ���������ł��邱�Ƃ�ݒ�
	m_sendEnd = FALSE;

	switch( writeViewType ) {
	case WRITEVIEW_TYPE_REPLYMESSAGE:
		// �^�C�g���ύX�F�L��
		m_titleEdit.SetReadOnly(FALSE);

		if (m_data != NULL) {
			// �^�C�g���̏����l��ݒ�
			SetDlgItemText( IDC_WRITE_TITLE_EDIT, L"Re : " + m_data->GetTitle() );
		}

		// �t�H�[�J�X�F�{������J�n
		m_bodyEdit.SetFocus();
		break;

	case WRITEVIEW_TYPE_NEWMESSAGE:
		// �^�C�g���ύX�F�L��
		m_titleEdit.SetReadOnly(FALSE);

		// �t�H�[�J�X�F�^�C�g������J�n
		m_titleEdit.SetFocus();
		break;

	case WRITEVIEW_TYPE_COMMENT:
		// �^�C�g���ύX�F����
		m_titleEdit.SetReadOnly(TRUE);

		if (m_data != NULL) {
			// �^�C�g����ݒ�
			SetDlgItemText( IDC_WRITE_TITLE_EDIT, m_data->GetTitle() );
		}

		// �t�H�[�J�X�F�{������J�n
		m_bodyEdit.SetFocus();
		break;

	case WRITEVIEW_TYPE_NEWDIARY:
		// �^�C�g���ύX�F�L��
		m_titleEdit.SetReadOnly(FALSE);

		// �t�H�[�J�X�F�^�C�g������J�n
		m_titleEdit.SetFocus();
		break;

	default:
		MessageBox( L"�A�v���P�[�V���������G���[�F���Ή��̏������݉�ʎ�ʂł�" );
		return;
	}

	// �{�^������
	theApp.EnableCommandBarButton( ID_FORWARD_BUTTON, FALSE );
	theApp.EnableCommandBarButton( ID_BACK_BUTTON,    TRUE  );
	theApp.EnableCommandBarButton( ID_OPEN_BROWSER,   FALSE );

	// �C���[�W�{�^���͉摜���e�\�ȏ�Ԃɂ̂ݗL��
	theApp.EnableCommandBarButton( ID_IMAGE_BUTTON, IsEnableAttachImageMode() ? TRUE : FALSE );

	// �摜�Y�t�֘A�̏�����
	m_photo1_filepath = L"";
	m_photo2_filepath = L"";
	m_photo3_filepath = L"";

	// �{�^���̏�����
	m_sendButton.EnableWindow(TRUE);
	m_cancelButton.EnableWindow(TRUE);
	m_infoEdit.ShowWindow(SW_HIDE);

	// �u����������ւ�
	theApp.ChangeView(theApp.m_pWriteView);
}

/// �ʐ^�P�b�Y�t
void CWriteView::OnAttachPhoto1()
{
	m_photo1_filepath = MyShowDlgToConfirmPostImage( m_photo1_filepath );
}

/// �ʐ^�Q�b�Y�t
void CWriteView::OnAttachPhoto2()
{
	m_photo2_filepath = MyShowDlgToConfirmPostImage( m_photo2_filepath );
}

/// �ʐ^�R�b�Y�t
void CWriteView::OnAttachPhoto3()
{
	m_photo3_filepath = MyShowDlgToConfirmPostImage( m_photo3_filepath );
}

/// �ʐ^�P�b�Y�t����߂�
void CWriteView::OnCancelAttachPhoto1() { m_photo1_filepath = L""; }

/// �ʐ^�Q�b�Y�t����߂�
void CWriteView::OnCancelAttachPhoto2() { m_photo2_filepath = L""; }

/// �ʐ^�R�b�Y�t����߂�
void CWriteView::OnCancelAttachPhoto3() { m_photo3_filepath = L""; }

/// �ʐ^�P�b�v���r���[
void CWriteView::OnPreviewAttachedPhoto1()
{
	if( !m_photo1_filepath.IsEmpty() ) { 
		util::OpenByShellExecute( m_photo1_filepath );
	}
}

/// �ʐ^�Q�b�v���r���[
void CWriteView::OnPreviewAttachedPhoto2()
{
	if( !m_photo2_filepath.IsEmpty() ) { 
		util::OpenByShellExecute( m_photo2_filepath );
	}
}

/// �ʐ^�R�b�v���r���[
void CWriteView::OnPreviewAttachedPhoto3()
{
	if( !m_photo3_filepath.IsEmpty() ) { 
		util::OpenByShellExecute( m_photo3_filepath );
	}
}

/// �E�\�t�g�L�[���j���[�b�ʐ^��Y�t����
void CWriteView::OnAttachPhoto()
{
	// �u�摜�{�^���v�������̃��j���[������ɕ\������
	OnImageButton();
}

/// �摜���Y�t�\�ȃ��[�h�ł��邩��Ԃ�
bool CWriteView::IsEnableAttachImageMode(void)
{
	switch( m_writeViewType ) {
	case WRITEVIEW_TYPE_REPLYMESSAGE:
	case WRITEVIEW_TYPE_NEWMESSAGE:
		return false;

	case WRITEVIEW_TYPE_COMMENT:
		switch (m_data->GetAccessType()) {
		case ACCESS_BBS:
		case ACCESS_EVENT:
			return true;
		}
		return false;

	case WRITEVIEW_TYPE_NEWDIARY:
		return true;

	default:
		return false;
	}
}

/**
 * �m�F��ʂւ̑J�ڊJ�n
 */
void CWriteView::StartConfirmPost( CString wmsg )
{
	// ���[�U�����͂������b�Z�[�W�� EUC-JP URL Encoded String �ɕϊ�����
	CString euc_msg = URLEncoder::encode_euc(wmsg);
	TRACE(L"euc-jp url encoded string = [%s]\n", euc_msg);

	CString url;

	switch (m_writeViewType) {
	case WRITEVIEW_TYPE_COMMENT:
		// �R�����g�̓��e�m�F
		{
			// �d������
			if( !mixi::PostCommentGenerator::generate( *m_postData, *m_data, euc_msg, 
													   m_photo1_filepath, m_photo2_filepath, m_photo3_filepath ) ) 
			{
				MessageBox( GENERATE_POSTMSG_FAILED_MESSAGE );
				return;
			}

			// ���N�G�X�gURL����
			url.Format( L"http://mixi.jp/%s", m_data->GetPostAddress() );
		}
		break;

	case WRITEVIEW_TYPE_NEWDIARY:
		// ���L�̓��e�m�F
		{
			// �d������
			if( !mixi::PostDiaryGenerator::generate( *m_postData, theApp.m_loginMng.GetOwnerID(), euc_msg, 
													 m_photo1_filepath, m_photo2_filepath, m_photo3_filepath ) )
			{
				MessageBox( GENERATE_POSTMSG_FAILED_MESSAGE );
				return;
			}

			// ���N�G�X�gURL����
			url = L"http://mixi.jp/add_diary.pl";
		}
		break;

	case WRITEVIEW_TYPE_REPLYMESSAGE:
		// ���b�Z�[�W�̕ԐM�m�F
		{
			// �d������
			if( !mixi::PostReplyMessageGenerator::generate( *m_postData, euc_msg ) ) {
				MessageBox( GENERATE_POSTMSG_FAILED_MESSAGE );
				return;
			}

			// ���N�G�X�gURL����
			// http://mixi.jp/view_message.pl?id=xxx&box=inbox
			// ��
			// http://mixi.jp/reply_message.pl?id=yyy&message_id=xxx

			url.Format( L"http://mixi.jp/%s", m_data->GetURL() );
			url.Replace( L"&box=inbox", L"" );
			url.Replace( L"id=", L"message_id=" );

			CString subUri;
			subUri.Format(L"reply_message.pl?id=%d&", m_data->GetOwnerID());
			url.Replace(L"view_message.pl?", subUri);

			m_postData->SetConfirmUri(url);
		}
		break;

	case WRITEVIEW_TYPE_NEWMESSAGE:
		// �V�K���b�Z�[�W�̑��M�m�F
		{
			// �d������
			if( !mixi::PostNewMessageGenerator::generate( *m_postData, euc_msg ) ) {
				MessageBox( GENERATE_POSTMSG_FAILED_MESSAGE );
				return;
			}

			// ���N�G�X�gURL����

			// m_data ���� id ���擾����
			// http://mixi.jp/show_friend.pl?id=xxx
			int id = mixi::MixiUrlParser::GetID( m_data->GetURL() );
			if( id <= 0 ) {
				MessageBox( L"���M�惆�[�U�� ID ���s���ł��B���b�Z�[�W�𑗐M�ł��܂���B" );
				return;
			}

			// URL ����
			url = util::FormatString( L"http://mixi.jp/send_message.pl?id=%d", id );
			m_postData->SetConfirmUri(url);
		}

		break;

	default:
		{
			CString s;
			s.Format( L"���T�|�[�g�̑��M��ʂł� [%d]", m_writeViewType );
			MessageBox( s );

			return;
		}
		break;
	}

	// �������̃��b�Z�[�W��ݒ肷��
	m_postData->SetSuccessMessage( WM_MZ3_POST_CONFIRM );

	// �A�N�Z�X��ʂ̐ݒ�
	switch (m_writeViewType) {
	case WRITEVIEW_TYPE_COMMENT:		theApp.m_accessType = ACCESS_POST_CONFIRM_COMMENT;		break;
	case WRITEVIEW_TYPE_NEWDIARY:		theApp.m_accessType = ACCESS_POST_CONFIRM_NEWDIARY;		break;
	case WRITEVIEW_TYPE_REPLYMESSAGE:	theApp.m_accessType = ACCESS_POST_CONFIRM_REPLYMESSAGE;	break;
	case WRITEVIEW_TYPE_NEWMESSAGE:		theApp.m_accessType = ACCESS_POST_CONFIRM_NEWMESSAGE;	break;
	default:							theApp.m_accessType = ACCESS_MAIN;						break;
	}

	TRACE( L"Post URL = [%s]\n", url );

	m_abort = FALSE;

	m_infoEdit.ShowWindow( SW_SHOW );
	theApp.EnableCommandBarButton( ID_BACK_BUTTON, FALSE );
	theApp.EnableCommandBarButton( ID_STOP_BUTTON, TRUE );

	// ���t�@���ݒ�
	LPCTSTR refUrl = L"";
  //���t�@���ݒ肵���瓊�e�Ŏ~�܂�悤�ɂȂ����̂ŕۗ�
	//LPCTSTR refUrl = m_data->GetURL();

	// �ʐM�J�n
	theApp.m_inet.Initialize( m_hWnd, NULL );
	theApp.m_inet.DoPost(
		url, 
		refUrl, 
		CInetAccess::FILE_HTML, 
		m_postData );
}

/// �G�����}��
void CWriteView::OnInsertEmoji(UINT nID)
{
	int emojiIndex = nID - IDM_INSERT_EMOJI_BEGIN;
	// index check
	if (emojiIndex < 0 || emojiIndex >= (int)theApp.m_emoji.size()) {
		return;
	}

	// ���݂̃J�[�\���ʒu�ɑ}���i�܂��͑I��͈͂̒u���j
	m_bodyEdit.ReplaceSel( theApp.m_emoji[ emojiIndex ].code, /*bCanUndo=*/TRUE );
}


void CWriteView::PopupWriteBodyMenu(void)
{
	POINT pt    = util::GetPopupPos();
	int   flags = util::GetPopupFlags();

	CImageList* pGlobalImageList = &theApp.m_imageCache.GetImageList();

	CWMMenu menu( pGlobalImageList );
	menu.LoadMenu( IDR_WRITE_MENU );
	CMenu* pcThisMenu = menu.GetSubMenu(0);

	// �ʐ^�Y�t���ł��Ȃ���ʂł́u�ʐ^��Y�t����v���j���[������
	if( !IsEnableAttachImageMode() ) {
		pcThisMenu->EnableMenuItem( ID_ATTACH_PHOTO, MF_GRAYED | MF_BYCOMMAND );
	}

	// �G�������j���[
	const int SUBMENU_MAX = 20;
	int iSubMenu = 0;
	CWMMenu emojiSubMenu[SUBMENU_MAX] = {
		CWMMenu(pGlobalImageList), CWMMenu(pGlobalImageList), CWMMenu(pGlobalImageList), CWMMenu(pGlobalImageList), CWMMenu(pGlobalImageList), 
		CWMMenu(pGlobalImageList), CWMMenu(pGlobalImageList), CWMMenu(pGlobalImageList), CWMMenu(pGlobalImageList), CWMMenu(pGlobalImageList), 
		CWMMenu(pGlobalImageList), CWMMenu(pGlobalImageList), CWMMenu(pGlobalImageList), CWMMenu(pGlobalImageList), CWMMenu(pGlobalImageList), 
		CWMMenu(pGlobalImageList), CWMMenu(pGlobalImageList), CWMMenu(pGlobalImageList), CWMMenu(pGlobalImageList), CWMMenu(pGlobalImageList), 
	};

	std::vector<WMMENUBITMAP> menuBitmapArray;
	// �_�~�[���폜
	CMenu* emojiMenu = pcThisMenu->GetSubMenu(1);
	emojiMenu->DeleteMenu( IDM_INSERT_EMOJI_BEGIN, MF_BYCOMMAND );

	// N�����j���[�ɂ��Ēǉ�
	const int MENU_SPLIT_COUNT = 20;
	if (!theApp.m_emoji.empty()) {
		emojiSubMenu[iSubMenu].CreatePopupMenu();

		for (size_t i=0; i<theApp.m_emoji.size(); i++) {
			if (i%MENU_SPLIT_COUNT == 0 && 
				emojiSubMenu[iSubMenu].GetMenuItemCount()>0 && 
				iSubMenu<SUBMENU_MAX-1)
			{
				// �e�ɒǉ�
				emojiMenu->AppendMenu( MF_POPUP, (UINT)emojiSubMenu[iSubMenu].m_hMenu, util::FormatString(L"%d", iSubMenu+1) );
				iSubMenu++;
				emojiSubMenu[iSubMenu].CreatePopupMenu();
			}

			// �e�L�X�g�ݒ�
			int idItem = IDM_INSERT_EMOJI_BEGIN+i;
			emojiSubMenu[iSubMenu].AppendMenu( MF_STRING, idItem, theApp.m_emoji[i].text );

			// �G�������[�h
			// �Ƃ肠���� Win32 ����
			// �i�A�h�G�X�łȂ��� ToOwnerDraw ���ŗ����邽�߁E�E�E�j
#ifndef WINCE
			CString path = util::MakeImageLogfilePathFromUrl( theApp.m_emoji[i].url );

			// ���[�h�ς݂�����
			int imageIndex = theApp.m_imageCache.GetImageIndex( path );
			if (imageIndex<0) {
				// �����[�h�Ȃ̂Ń��[�h
				CMZ3BackgroundImage image(L"");
				image.load( path );
				if (image.isEnableImage()) {
					// 16x16 �Ƀ��T�C�Y����B
					CMZ3BackgroundImage resizedImage(L"");
					util::MakeResizedImage( this, resizedImage, image, 16, 16 );

					// �r�b�g�}�b�v�̒ǉ�
					CBitmap bm;
					bm.Attach( resizedImage.getHandle() );

					// �O���[�o���L���b�V���ɒǉ����A�C���f�b�N�X���擾����
					imageIndex = theApp.m_imageCache.Add( &bm, (CBitmap*) NULL, path );
				}
			}
			if (imageIndex>=0) {
				// ���j���[�p�r�b�g�}�b�v���\�[�X�ɒǉ�
				WMMENUBITMAP menuBmp = { idItem, imageIndex };
				menuBitmapArray.push_back(menuBmp);
			}
#endif
		}
		if (emojiSubMenu[iSubMenu].GetMenuItemCount() > 0) {
			// �e�ɒǉ�
			emojiMenu->AppendMenu( MF_POPUP, (UINT)emojiSubMenu[iSubMenu].m_hMenu, util::FormatString(L"%d", iSubMenu+1) );
		}
	}

	// �I�[�i�[�h���[�����i�r�b�g�}�b�v�̐ݒ�j
	if (!menuBitmapArray.empty() ) {
		menu.PrepareOwnerDraw( &menuBitmapArray[0], menuBitmapArray.size() );
	}

	// ���j���[�\��
	pcThisMenu->TrackPopupMenu( flags, pt.x, pt.y, this );
}
