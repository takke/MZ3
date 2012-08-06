/*
 * Copyright (c) MZ3 Project Team All rights reserved.
 * Code licensed under the BSD License:
 * http://www.mz3.jp/license.txt
 */
// WriteView.cpp : �����t�@�C��
//

#include "stdafx.h"

#ifdef BT_MZ3

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
	, m_abort(false)
	, m_bWriteCompleted(true)
	, m_bFromMainView(false)
{
	m_postData = new CPostData();
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
	DDX_Control(pDX, IDC_WRITE_VIEWLIMIT_COMBO, m_viewlimitCombo);
}

BEGIN_MESSAGE_MAP(CWriteView, CFormView)
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_WRITE_SEND_BUTTON, &CWriteView::OnBnClickedWriteSendButton)
	ON_BN_CLICKED(IDC_WRITE_CANCEL_BUTTON, &CWriteView::OnBnClickedWriteCancelButton)
    ON_MESSAGE(WM_MZ3_GET_END, OnGetEnd)
	ON_MESSAGE(WM_MZ3_GET_ERROR, OnGetError)
	ON_MESSAGE(WM_MZ3_POST_ENTRY_END, OnPostEntryEnd)
	ON_MESSAGE(WM_MZ3_POST_CONFIRM_END, OnPostConfirmEnd)
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
	ON_COMMAND_RANGE(ID_LUA_MENU_BASE, ID_LUA_MENU_BASE+1000, OnLuaMenu)
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

		// ���J�͈̓R���{�{�b�N�X
		m_viewlimitCombo.SetFont( &theApp.m_font );
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

	int hEdit = theApp.GetInfoRegionHeight(theApp.m_optionMng.GetFontHeightByPixel(theApp.GetDPI()));

	// �R���{�{�b�N�X�̍������擾����
	int hCombo ;
	if( m_viewlimitCombo.m_hWnd ) {				// �������O�ɌĂ΂ꂽ�肷��̂Ŕ���
		// �R���{�{�b�N�X������
		// �R���{�{�b�N�X�̃T�C�Y�͎����I�Ɍ��܂�̂ł��������Ă���
		RECT rct;
		m_viewlimitCombo.GetWindowRect( &rct );
		hCombo = rct.bottom - rct.top ;
	} else {
		// �R���{�{�b�N�X���Ȃ�
		// ���傤���Ȃ��̂ŃG�f�B�b�g�{�b�N�X�̍������̗p����
		hCombo = hEdit;
	}
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

	util::MoveDlgItemWindow( this, IDC_WRITE_TITLE_EDIT,       0,              0,   cx,             hEdit);
	util::MoveDlgItemWindow( this, IDC_WRITE_VIEWLIMIT_COMBO,  0,          hEdit,   cx,            hCombo);
	util::MoveDlgItemWindow( this, IDC_WRITE_BODY_EDIT,        0, hEdit + hCombo,   cx, cy-hEdit*2-hCombo);
	util::MoveDlgItemWindow( this, IDC_WRITE_INFO_EDIT,        0,       cy-hEdit,   cx,             hEdit);
	util::MoveDlgItemWindow( this, IDC_WRITE_SEND_BUTTON,      0,       cy-hEdit, yBtn,             hEdit);
	util::MoveDlgItemWindow( this, IDC_WRITE_CANCEL_BUTTON, yBtn,       cy-hEdit, yBtn,             hEdit);
}

/**
 * �������݉�ʂ̊J�n
 *
 * ���r���[����̃G���g���|�C���g
 */
void CWriteView::StartWriteView(WRITEVIEW_TYPE writeViewType, CMixiData* pMixi)
{
	theApp.m_access = false;
	m_abort  = false;

	m_data = pMixi;
	int iComboIndex;

	// ���e���N���A
	SetDlgItemText( IDC_WRITE_TITLE_EDIT, L"" );
	SetDlgItemText( IDC_WRITE_BODY_EDIT, L"" );
	m_titleEdit.SetReadOnly(FALSE);
	m_viewlimitCombo.ResetContent();
	m_viewlimitCombo.EnableWindow(FALSE);

	// �����t�H�[�J�X
	m_bodyEdit.SetFocus();

	// ��ʂ�ۑ�
	m_writeViewType = writeViewType;

	// ���������ł��邱�Ƃ�ݒ�
	m_bWriteCompleted = false;

	// MZ3 API : �t�b�N�֐��Ăяo��
	util::MyLuaDataList rvals;
	rvals.push_back(util::MyLuaData(0));	// m_bFromMainView
	rvals.push_back(util::MyLuaData(""));	// init_focus
	rvals.push_back(util::MyLuaData(0));	// enable_combo_box
	rvals.push_back(util::MyLuaData(0));	// enable_title_change
	if (util::CallMZ3ScriptHookFunctions2("init_write_view", &rvals, 
			util::MyLuaData(m_writeViewType),
			util::MyLuaData(m_data)
			)) {

		// �T�|�[�g���Ă���̂ő��s
		if (rvals[0].m_number) {
			m_bFromMainView = true;
		} else {
			m_bFromMainView = false;
		}

		// �����t�H�[�J�X
		if (rvals[1].m_strText=="body") {
			m_bodyEdit.SetFocus();
		} else if (rvals[1].m_strText=="title") {
			m_titleEdit.SetFocus();
		}

		// �R���{�{�b�N�X�L���E����
		if (rvals[2].m_number) {
			m_viewlimitCombo.EnableWindow(TRUE);
		} else {
			m_viewlimitCombo.EnableWindow(FALSE);
		}

		// �^�C�g���ύX�L���E����
		if (rvals[3].m_number) {
			m_titleEdit.SetReadOnly(FALSE);
		} else {
			m_titleEdit.SetReadOnly(TRUE);
		}

	} else {
		// Lua ���ŃT�|�[�g���Ă��Ȃ��̂ŏ]���̃R�[�h�B
		switch( writeViewType ) {
		case WRITEVIEW_TYPE_REPLYMESSAGE:
			// �^�C�g���ύX�F�L��
			m_titleEdit.SetReadOnly(FALSE);

			if (m_data != NULL) {
				// �^�C�g���̏����l��ݒ�
				SetDlgItemText( IDC_WRITE_TITLE_EDIT, L"Re : " + m_data->GetTitle() );
			}

			// ���J�͈̓R���{�{�b�N�X�F����
			m_viewlimitCombo.EnableWindow(FALSE);

			// �t�H�[�J�X�F�{������J�n
			m_bodyEdit.SetFocus();

			// �߂��ݒ�
			m_bFromMainView = false;
			break;

		case WRITEVIEW_TYPE_NEWMESSAGE:
			// �^�C�g���ύX�F�L��
			m_titleEdit.SetReadOnly(FALSE);

			// ���J�͈̓R���{�{�b�N�X�F����
			m_viewlimitCombo.EnableWindow(FALSE);

			// �t�H�[�J�X�F�^�C�g������J�n
			m_titleEdit.SetFocus();

			// �߂��ݒ�
			m_bFromMainView = true;
			break;

		case WRITEVIEW_TYPE_COMMENT:
			// �^�C�g���ύX�F����
			m_titleEdit.SetReadOnly(TRUE);

			if (m_data != NULL) {
				// �^�C�g����ݒ�
				SetDlgItemText( IDC_WRITE_TITLE_EDIT, m_data->GetTitle() );
			}

			// ���J�͈̓R���{�{�b�N�X�F����
			m_viewlimitCombo.EnableWindow(FALSE);

			// �t�H�[�J�X�F�{������J�n
			m_bodyEdit.SetFocus();

			// �߂��ݒ�
			m_bFromMainView = false;
			break;

		case WRITEVIEW_TYPE_NEWDIARY:
			// �^�C�g���ύX�F�L��
			m_titleEdit.SetReadOnly(FALSE);

			// ���J�͈̓R���{�{�b�N�X
			iComboIndex = m_viewlimitCombo.AddString( L"�W���̌��J�ݒ�" );
			m_viewlimitCombo.SetItemData( iComboIndex , 0 );
			iComboIndex = m_viewlimitCombo.AddString( L"����J" );
			m_viewlimitCombo.SetItemData( iComboIndex , 1 );
			iComboIndex = m_viewlimitCombo.AddString( L"�F�l�܂Ō��J" );
			m_viewlimitCombo.SetItemData( iComboIndex , 2 );
			iComboIndex = m_viewlimitCombo.AddString( L"�F�l�̗F�l�܂Ō��J" );
			m_viewlimitCombo.SetItemData( iComboIndex , 3 );
			iComboIndex = m_viewlimitCombo.AddString( L"�S�̂Ɍ��J" );
			m_viewlimitCombo.SetItemData( iComboIndex , 4 );
			m_viewlimitCombo.SetCurSel( 0 );
			m_viewlimitCombo.EnableWindow(TRUE);

			// �t�H�[�J�X�F�^�C�g������J�n
			m_titleEdit.SetFocus();

			// �߂��ݒ�
			m_bFromMainView = true;
			break;

		default:
			MessageBox( L"�A�v���P�[�V���������G���[�F���Ή��̏������݉�ʎ�ʂł�" );
			return;
		}
	}

	// �R���g���[����Ԃ̕ύX
	MyUpdateControlStatus();

	// �摜�Y�t�֘A�̏�����
	m_photo1_filepath = L"";
	m_photo2_filepath = L"";
	m_photo3_filepath = L"";

	// View ����ւ�
	theApp.ChangeView(theApp.m_pWriteView);
}

/**
 * �������݃{�^���������̏���
 *
 * ����n�����V�[�P���X�F
 *
 * <pre>
 *  StartEntryPost
 *    => StartConfirmPost()  [���b�Z�[�W�ȊO]
 *      => DoPost/WM_MZ3_POST_CONFIRM_END
 *        => �ʐM�J�n
 *    => DoPost/WM_MZ3_POST_ENTRY_END [���b�Z�[�W�̂�]
 *      => �ʐM�J�n
 *  OnPostEntryEnd (���b�Z�[�W�̂�)
 *    => StartConfirmPost()
 *      => DoPost/WM_MZ3_POST_CONFIRM_END
 *        => �ʐM�J�n
 *  OnPostConfirmEnd
 *    => StartRegistPost()
 *      => DoPost/WM_MZ3_POST_END
 *        => �ʐM�J�n
 *  OnPostEnd
 * </pre>
 */
void CWriteView::OnBnClickedWriteSendButton()
{
	// MZ3 API : �t�b�N�֐��Ăяo��
	util::MyLuaDataList rvals;
	if (util::CallMZ3ScriptHookFunctions2("click_write_view_send_button", &rvals, 
			util::MyLuaData(m_writeViewType),
			util::MyLuaData(m_data)
			)) {

		// �L�[�����C�x���g��D�����߂Ƀt�H�[�J�X���擾����
		SetFocus();
		return;
	}

	CString msg;
	GetDlgItemText( IDC_WRITE_BODY_EDIT, msg );

	CString title;
	GetDlgItemText( IDC_WRITE_TITLE_EDIT, title );

	if (msg.GetLength() > 3000) {
		// �ő啶�����z���Ă���
		::MessageBox(m_hWnd, L"���M���������������܂�", MZ3_APP_NAME, MB_ICONERROR);
		return;
	}
	
	if (title.GetLength() == 0) {
		::MessageBox(m_hWnd, L"�^�C�g������͂��Ă�������", MZ3_APP_NAME, MB_ICONERROR);
		return;
	}
	
	if (msg.GetLength() == 0) {
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

	theApp.m_access = true;
	m_abort  = false;

	// �R���g���[����Ԃ̕ύX
	MyUpdateControlStatus();

	if (wcslen(theApp.m_loginMng.GetMixiOwnerID()) == 0) {
		MZ3LOGGER_INFO( L"OwnerID�����擾�Ȃ̂ŁA���O�C�����A�擾����(1)" );

		// ���O�C�����s
		theApp.m_mixi4recv.SetAccessType(ACCESS_LOGIN);
		theApp.m_accessType = ACCESS_LOGIN;
		theApp.StartMixiLoginAccess(m_hWnd, &theApp.m_mixi4recv);
//		theApp.m_inet.Initialize( m_hWnd, &theApp.m_mixi4recv );
//		theApp.m_inet.DoGet(theApp.MakeLoginUrl(), L"", CInetAccess::FILE_HTML );
		return;
	}

	// POST �J�n
	StartEntryPost();

	// �L�[�����C�x���g��D�����߂Ƀt�H�[�J�X���擾����
	SetFocus();
}

/**
 * ���͉�ʂ̎擾(���b�Z�[�W�ȊO�̏ꍇ�͊m�F��ʂւ̑J��)
 */
void CWriteView::StartEntryPost()
{
	CString url;

	switch (m_writeViewType) {
	case WRITEVIEW_TYPE_COMMENT:		// �R�����g�̓��e�m�F
	case WRITEVIEW_TYPE_NEWDIARY:		// ���L�̓��e�m�F

		// �m�F��ʂւ̑J��
		StartConfirmPost();
		return;

	case WRITEVIEW_TYPE_REPLYMESSAGE:
		// ���b�Z�[�W�̕ԐM�m�F
		{
			// POST �d���̐���
			m_postData->ClearPostBody();

			// Content-Type ��ݒ肷��
			// Content-Type: application/x-www-form-urlencoded
			m_postData->SetContentType( CONTENT_TYPE_FORM_URLENCODED );

			// ���N�G�X�gURL����
			// http://mixi.jp/view_message.pl?id=xxx&box=inbox
			// ��
			// http://mixi.jp/reply_message.pl?reply_message_id=xxx&id=yyy
			int friend_id = m_data->GetOwnerID();
			CString reply_message_id = util::GetParamFromURL(m_data->GetURL(), L"id");
			url.Format(L"http://mixi.jp/reply_message.pl?reply_message_id=%s&id=%d",
				reply_message_id, friend_id);

			m_postData->SetConfirmUri(url);
		}
		break;

	case WRITEVIEW_TYPE_NEWMESSAGE:
		// �V�K���b�Z�[�W�̑��M�m�F
		{
			// POST �d���̐���
			m_postData->ClearPostBody();

			// Content-Type ��ݒ肷��
			// Content-Type: application/x-www-form-urlencoded
			m_postData->SetContentType( CONTENT_TYPE_FORM_URLENCODED );


			// ���N�G�X�gURL����

			// m_data ���� id ���擾����
			// http://mixi.jp/show_friend.pl?id=xxx
			int id = mixi::MixiUrlParser::GetID( m_data->GetURL() );
			if( id <= 0 ) {
				MessageBox( L"���M�惆�[�U�� ID ���s���ł��B���b�Z�[�W�𑗐M�ł��܂���B" );

				theApp.m_access = false;
				MyUpdateControlStatus();
				return;
			}

			// URL ����
			// http://mixi.jp/send_message.pl?id=xxx&mode=from_show_friend
			url = util::FormatString( L"http://mixi.jp/send_message.pl?id=%d&mode=from_show_friend", id );
			m_postData->SetConfirmUri(url);
		}

		break;

	default:
		{
			CString s;
			s.Format( L"���T�|�[�g�̑��M��ʂł� [%d]", m_writeViewType );
			MessageBox( s );
			MZ3LOGGER_ERROR(s);

			// �R���g���[����Ԃ̕��A
			theApp.m_access = false;
			MyUpdateControlStatus();
			return;
		}
		break;
	}

	// �������̃��b�Z�[�W��ݒ肷��
	m_postData->SetSuccessMessage( WM_MZ3_POST_ENTRY_END );

	// �A�N�Z�X��ʂ̐ݒ�
	switch (m_writeViewType) {
	case WRITEVIEW_TYPE_REPLYMESSAGE:	theApp.m_accessType = ACCESS_POST_REPLYMESSAGE_ENTRY;	break;
	case WRITEVIEW_TYPE_NEWMESSAGE:		theApp.m_accessType = ACCESS_POST_NEWMESSAGE_ENTRY;		break;
	default:							theApp.m_accessType = ACCESS_MAIN;						break;
	}

	theApp.m_access = true;
	m_abort  = false;

	// �R���g���[����Ԃ̕ύX
	MyUpdateControlStatus();

	LPCTSTR refUrl = L"";

	// �ʐM�J�n
	theApp.m_inet.Initialize( m_hWnd, NULL, theApp.GetInetAccessEncodingByAccessType(theApp.m_accessType) );
	theApp.m_inet.DoPost(
		url, 
		refUrl, 
		CInetAccess::FILE_HTML, 
		m_postData);
}

/**
 * �m�F��ʂւ̑J��
 */
void CWriteView::StartConfirmPost()
{
	CString wmsg;
	GetDlgItemText( IDC_WRITE_BODY_EDIT, wmsg );

	// ���[�U�����͂������b�Z�[�W�� EUC-JP URL Encoded String �ɕϊ�����
	CString euc_msg = URLEncoder::encode_euc(wmsg);
	TRACE(L"euc-jp url encoded string = [%s]\n", euc_msg);

	CString url;

	switch (m_writeViewType) {
	case WRITEVIEW_TYPE_COMMENT:
		// �R�����g�̓��e�m�F
		{
			// �d������
			if( !mixi::CommentConfirmGenerator::generate( *m_postData, *m_data, euc_msg, 
													      m_photo1_filepath, m_photo2_filepath, m_photo3_filepath ) ) 
			{
				MessageBox( GENERATE_POSTMSG_FAILED_MESSAGE );

				theApp.m_access = false;
				MyUpdateControlStatus();
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
			if( !mixi::DiaryConfirmGenerator::generate( *m_postData, theApp.m_loginMng.GetMixiOwnerID(), euc_msg, 
													    m_photo1_filepath, m_photo2_filepath, m_photo3_filepath ) )
			{
				MessageBox( GENERATE_POSTMSG_FAILED_MESSAGE );

				theApp.m_access = false;
				MyUpdateControlStatus();
				return;
			}

			// ���N�G�X�gURL����
			url = L"http://mixi.jp/add_diary.pl";
		}
		break;

	case WRITEVIEW_TYPE_REPLYMESSAGE:
		// ���b�Z�[�W�̕ԐM�m�F
		{
			// �^�C�g�����擾����
			CString title;
			GetDlgItemText( IDC_WRITE_TITLE_EDIT, title );

			// �d������
			int friend_id = m_data->GetOwnerID();
			CString reply_message_id = util::GetParamFromURL(m_data->GetURL(), L"id");
			if( !mixi::ReplyMessageConfirmGenerator::generate( *m_postData, title, euc_msg, friend_id, reply_message_id ) ) {
				MessageBox( GENERATE_POSTMSG_FAILED_MESSAGE );

				theApp.m_access = false;
				MyUpdateControlStatus();
				return;
			}

			// ���N�G�X�gURL����
			// http://mixi.jp/view_message.pl?id=xxx&box=inbox
			// ��
			// http://mixi.jp/reply_message.pl?id=yyy&message_id=xxx
			//url.Format(L"http://mixi.jp/reply_message.pl?reply_message_id=%s&id=%d",
			//	(LPCTSTR)util::GetParamFromURL(m_data->GetURL(), L"id"),
			//	m_data->GetOwnerID());
			url = L"http://mixi.jp/reply_message.pl";

			m_postData->SetConfirmUri(url);
		}
		break;

	case WRITEVIEW_TYPE_NEWMESSAGE:
		// �V�K���b�Z�[�W�̑��M�m�F
		{
			// �^�C�g�����擾����
			CString title;
			GetDlgItemText( IDC_WRITE_TITLE_EDIT, title );

			// �d������
			int friend_id = m_data->GetOwnerID();
			if( friend_id <= 0 ) {
				friend_id = mixi::MixiUrlParser::GetID( m_data->GetURL() );
				if( friend_id <= 0 ) {
					MessageBox( L"���M�惆�[�U�� ID ���s���ł��B���b�Z�[�W�𑗐M�ł��܂���B" );

					theApp.m_access = false;
					MyUpdateControlStatus();
					return;
				}
			}
			if( !mixi::NewMessageConfirmGenerator::generate( *m_postData, title, euc_msg, friend_id ) ) {
				MessageBox( GENERATE_POSTMSG_FAILED_MESSAGE );

				theApp.m_access = false;
				MyUpdateControlStatus();
				return;
			}

			// URL ����
			url = L"http://mixi.jp/send_message.pl";
			m_postData->SetConfirmUri(url);
		}

		break;

	default:
		{
			CString s;
			s.Format( L"���T�|�[�g�̑��M��ʂł� [%d]", m_writeViewType );
			MessageBox( s );
			MZ3LOGGER_ERROR(s);

			// �R���g���[����Ԃ̕��A
			theApp.m_access = false;
			MyUpdateControlStatus();
			return;
		}
		break;
	}

	// �������̃��b�Z�[�W��ݒ肷��
	m_postData->SetSuccessMessage( WM_MZ3_POST_CONFIRM_END );

	// �A�N�Z�X��ʂ̐ݒ�
	switch (m_writeViewType) {
	case WRITEVIEW_TYPE_COMMENT:		theApp.m_accessType = ACCESS_POST_COMMENT_CONFIRM;		break;
	case WRITEVIEW_TYPE_NEWDIARY:		theApp.m_accessType = ACCESS_POST_NEWDIARY_CONFIRM;		break;
	case WRITEVIEW_TYPE_REPLYMESSAGE:	theApp.m_accessType = ACCESS_POST_REPLYMESSAGE_CONFIRM;	break;
	case WRITEVIEW_TYPE_NEWMESSAGE:		theApp.m_accessType = ACCESS_POST_NEWMESSAGE_CONFIRM;	break;
	default:							theApp.m_accessType = ACCESS_MAIN;						break;
	}

	theApp.m_access = true;
	m_abort  = false;

	// �R���g���[����Ԃ̕ύX
	MyUpdateControlStatus();

	// ���t�@���ݒ�
	CString refUrl = L"";
	
	//���t�@���ݒ肵���瓊�e�Ŏ~�܂�悤�ɂȂ����̂ŕۗ�
	//LPCTSTR refUrl = m_data->GetURL();

	// �ʐM�J�n
	theApp.m_inet.Initialize( m_hWnd, NULL, theApp.GetInetAccessEncodingByAccessType(theApp.m_accessType) );
	theApp.m_inet.DoPost(
		url, 
		refUrl, 
		CInetAccess::FILE_HTML, 
		m_postData );
}

/**
* �A�N�Z�X�G���[�ʒm��M
*/
LRESULT CWriteView::OnGetError(WPARAM wParam, LPARAM lParam)
{
	// �ʐM�G���[�����������ꍇ�̏���
	LPCTSTR smsg = L"�G���[���������܂���";
	util::MySetInformationText( m_hWnd, smsg );

	CString msg;
	msg.Format( 
		L"%s\n\n"
		L"�����F%s", smsg, theApp.m_inet.GetErrorMessage() );
	::MessageBox(m_hWnd, msg, MZ3_APP_NAME, MB_ICONSTOP | MB_OK);
	MZ3LOGGER_ERROR( msg );

	theApp.m_access = false;

	// �R���g���[����Ԃ̕ύX
	MyUpdateControlStatus();

	return TRUE;
}

/**
 * ���~�{�^���������̏���
 */
void CWriteView::OnBnClickedWriteCancelButton()
{
	CString msg;
	GetDlgItem(IDC_WRITE_BODY_EDIT)->GetWindowText(msg);

	if (msg.GetLength() > 0) {
		if (MessageBox(L"�����e�̃f�[�^������܂�\n�j������܂�����낵���ł����H", 
			MZ3_APP_NAME,
			MB_ICONQUESTION | MB_OKCANCEL)==IDCANCEL)
		{
			// ���~�����̃L�����Z��(�ҏW�ɖ߂�)
			return;
		}
	}

	// ���e���N���A
	GetDlgItem(IDC_WRITE_TITLE_EDIT)->SetWindowText(L"");
	GetDlgItem(IDC_WRITE_BODY_EDIT)->SetWindowText(L"");

	// �R���g���[����Ԃ̕ύX
	MyUpdateControlStatus();

	// �O�̉�ʂɖ߂�
	if (IsWriteFromMainView()) {
		// ���C���r���[�ɖ߂邽�߁A�߂�{�^��������
		theApp.EnableCommandBarButton(ID_BACK_BUTTON, FALSE);
		::SendMessage(theApp.m_pMainView->m_hWnd, WM_MZ3_CHANGE_VIEW, NULL, NULL);
	} else {
		// ���|�[�g�r���[�ɖ߂邽�߁A�߂�{�^���L����
		theApp.EnableCommandBarButton(ID_BACK_BUTTON, TRUE);
		::SendMessage(theApp.m_pReportView->m_hWnd, WM_MZ3_CHANGE_VIEW, NULL, NULL);
	}

	m_bWriteCompleted = true;
}

/**
 * ���͉�ʂ̎擾�����B�m�F��ʃ{�^�������̃V�~�����[�g�B
 */
LRESULT CWriteView::OnPostEntryEnd(WPARAM wParam, LPARAM lParam)
{
	if (m_abort) {
		::SendMessage(m_hWnd, WM_MZ3_POST_ABORT, NULL, NULL);
		return TRUE;
	}

	// ���O�A�E�g�`�F�b�N
	if (theApp.IsMixiLogout(theApp.m_accessType)) {
		// ���O�A�E�g��ԂɂȂ��Ă���
		// ���O�C���������{
		MZ3LOGGER_INFO(_T("���O�C�����܂��B"));
		
		// ���O�C�����s
		theApp.m_mixi4recv.SetAccessType(ACCESS_LOGIN);
		theApp.m_accessType = ACCESS_LOGIN;
		theApp.StartMixiLoginAccess(m_hWnd, &theApp.m_mixi4recv);
//		theApp.m_inet.Initialize( m_hWnd, &theApp.m_mixi4recv );
//		theApp.m_inet.DoGet(theApp.MakeLoginUrl(), NULL, CInetAccess::FILE_HTML );
		return TRUE;
	}

	// �m�F��ʔ���
	CHtmlArray html;
	html.Load( theApp.m_filepath.temphtml );

	// ��͗p�ɓ��͉�ʂ�ۑ����Ă���
	CString strEntryTempFile = theApp.m_filepath.temphtml + L".entry.html";
	CopyFile( theApp.m_filepath.temphtml, strEntryTempFile, FALSE );

	// �m�F��ʂ̉��
	if (html.GetPostConfirmData(m_postData) == false) {
		CString msg = 
			L"�G���[���������܂���\r\n"
			L"�m�F��ʂɃG���[���e���\������Ă��܂����A"
			L"�{�o�[�W������" MZ3_APP_NAME L"�ł̓G���[���e��\���ł��܂���E�E�E\r\n"
			L"���萔�ł����A���L�t�@�C���𒼐ډ�͂��Ă�������m(_ _)m\r\n"
			+ strEntryTempFile;
		MessageBox( msg );
		MZ3LOGGER_ERROR( msg );

		MZ3LOGGER_ERROR( L"�������݂Ɏ��s�������߁A�O�̂���OwnerID�����������܂�" );
		theApp.m_loginMng.SetMixiOwnerID( L"" );

		theApp.m_access = false;
		MyUpdateControlStatus();

		return 0;
	}

	// �m�F��ʎ擾�J�n
	StartConfirmPost();

	return TRUE;
}

/**
 * �m�F��ʂ̕\�������B�������݂̃V�~�����[�g�B
 */
LRESULT CWriteView::OnPostConfirmEnd(WPARAM wParam, LPARAM lParam)
{
	if (m_abort) {
		::SendMessage(m_hWnd, WM_MZ3_POST_ABORT, NULL, NULL);
		return TRUE;
	}

	// ���O�A�E�g�`�F�b�N
	if (theApp.IsMixiLogout(theApp.m_accessType)) {
		// ���O�A�E�g��ԂɂȂ��Ă���
		// ���O�C���������{
		MZ3LOGGER_INFO(_T("���O�C�����܂��B"));

		// ���O�C�����s
		theApp.m_mixi4recv.SetAccessType(ACCESS_LOGIN);
		theApp.m_accessType = ACCESS_LOGIN;
		theApp.StartMixiLoginAccess(m_hWnd, &theApp.m_mixi4recv);
//		theApp.m_inet.Initialize( m_hWnd, &theApp.m_mixi4recv );
//		theApp.m_inet.DoGet(theApp.MakeLoginUrl(), NULL, CInetAccess::FILE_HTML );
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
		theApp.m_loginMng.SetMixiOwnerID( L"" );

		theApp.m_access = false;
		MyUpdateControlStatus();

		return 0;
	}

	// �������݊J�n
	StartRegistPost();

	return TRUE;
}

/**
 * �m�F��ʁb�������݃{�^���������̓d�����M
 */
void CWriteView::StartRegistPost() 
{
	// ���f�m�F
	if (m_abort) {
		::SendMessage(m_hWnd, WM_MZ3_POST_ABORT, NULL, NULL);
		return;
	}
	m_abort = false;

	CString url;	// URL
	CString refUrl;	// ���t�@���[�pURL

	switch (m_writeViewType) {
	case WRITEVIEW_TYPE_COMMENT:
		{
			// �d���̐���
			if( !mixi::CommentRegistGenerator::generate( *m_postData, *m_data ) ) {
				MessageBox( GENERATE_POSTMSG_FAILED_MESSAGE );

				// �R���g���[����Ԃ̕��A
				theApp.m_access = false;
				MyUpdateControlStatus();
				return;
			}

			// URL �̐���
			switch (m_data->GetAccessType()) {
			case ACCESS_BBS:
			case ACCESS_EVENT:
			case ACCESS_EVENT_JOIN:
			case ACCESS_ENQUETE:
				url.Format( L"http://mixi.jp/%s", m_data->GetPostAddress());
				break;

			case ACCESS_DIARY:
			case ACCESS_MYDIARY:
			case ACCESS_NEIGHBORDIARY:
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

			// ���J�͈͂��擾����
			CString viewlimit = L"0";
			int iIndex = m_viewlimitCombo.GetCurSel() ;
			if( iIndex >= 0 ) {
				viewlimit.Format( L"%d" , m_viewlimitCombo.GetItemData( iIndex ) ); 
#ifdef DEBUG
				wprintf( L"viewlimit = %s\n" , viewlimit);
#endif
			}

			// �d���̐���
			if( !mixi::DiaryRegistGenerator::generate( *m_postData, title, viewlimit ) ) {
				MessageBox( GENERATE_POSTMSG_FAILED_MESSAGE );

				// �R���g���[����Ԃ̕��A
				theApp.m_access = false;
				MyUpdateControlStatus();
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
			int friend_id = m_data->GetOwnerID();
			CString reply_message_id = util::GetParamFromURL(m_data->GetURL(), L"id");
			if( !mixi::ReplyMessageRegistGenerator::generate( *m_postData, title, friend_id, reply_message_id ) ) {
				MessageBox( GENERATE_POSTMSG_FAILED_MESSAGE );

				// �R���g���[����Ԃ̕��A
				theApp.m_access = false;
				MyUpdateControlStatus();
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
			int friend_id = m_data->GetOwnerID();
			if( friend_id <= 0 ) {
				friend_id = mixi::MixiUrlParser::GetID( m_data->GetURL() );
			}
			if( !mixi::NewMessageRegistGenerator::generate( *m_postData, title, friend_id ) ) {
				MessageBox( GENERATE_POSTMSG_FAILED_MESSAGE );

				// �R���g���[����Ԃ̕��A
				theApp.m_access = false;
				MyUpdateControlStatus();
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
			MZ3LOGGER_ERROR(s);

			// �R���g���[����Ԃ̕��A
			theApp.m_access = false;
			MyUpdateControlStatus();
			return;
		}
		break;
	}

	// �������̃��b�Z�[�W��ݒ肷��
	m_postData->SetSuccessMessage( WM_MZ3_POST_END );

	// �A�N�Z�X��ʂ̐ݒ�
	switch (m_writeViewType) {
	case WRITEVIEW_TYPE_COMMENT:		theApp.m_accessType = ACCESS_POST_COMMENT_REGIST;		break;
	case WRITEVIEW_TYPE_NEWDIARY:		theApp.m_accessType = ACCESS_POST_NEWDIARY_REGIST;		break;
	case WRITEVIEW_TYPE_REPLYMESSAGE:	theApp.m_accessType = ACCESS_POST_REPLYMESSAGE_REGIST;	break;
	case WRITEVIEW_TYPE_NEWMESSAGE:		theApp.m_accessType = ACCESS_POST_NEWMESSAGE_REGIST;	break;
	default:							theApp.m_accessType = ACCESS_MAIN;						break;
	}

	theApp.m_inet.Initialize( m_hWnd, NULL, theApp.GetInetAccessEncodingByAccessType(theApp.m_accessType) );
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

	theApp.m_access = false;
	m_abort  = false;

	// �R���g���[����Ԃ̕ύX
	MyUpdateControlStatus();

	// MZ3 API : �t�b�N�֐��Ăяo��
	util::MyLuaDataList rvals;
	if (util::CallMZ3ScriptHookFunctions2("post_end_write_view", &rvals, 
			util::MyLuaData(m_writeViewType),
			util::MyLuaData(m_data),
			util::MyLuaData(theApp.m_inet.m_dwHttpStatus),
			util::MyLuaData(CStringA(theApp.m_filepath.temphtml))
			))
	{
		m_bWriteCompleted = true;
		return TRUE;
	}

	// ���e�����`�F�b�N
	if (html.IsPostSucceeded(m_writeViewType) != FALSE) {
#ifdef	MZ3_DEBUG
		// only for Release_MZ3 configuration
		DumpToTemporaryDraftFile();
#endif
		if (m_abort == false) {
			switch (m_writeViewType ) {
			case WRITEVIEW_TYPE_REPLYMESSAGE:
			case WRITEVIEW_TYPE_NEWMESSAGE:
				::MessageBox(m_hWnd, L"���b�Z�[�W�𑗐M���܂���", MZ3_APP_NAME, MB_OK);
				break;
			default:
				::MessageBox(m_hWnd, L"���e���܂���", MZ3_APP_NAME, MB_OK);
				break;
			}
		} else {
			// ���f��������Ă����ꍇ�͏�ɕԂ�
			return TRUE;
		}

		GetDlgItem(IDC_WRITE_TITLE_EDIT)->SetWindowText(L"");
		GetDlgItem(IDC_WRITE_BODY_EDIT)->SetWindowText(L"");

		m_bWriteCompleted = true;

		// �O�̉�ʂɖ߂�
		if (IsWriteFromMainView()) {
			// ���C���r���[�ɖ߂邽�߁A�߂�{�^��������
			theApp.EnableCommandBarButton(ID_BACK_BUTTON, FALSE);
			::SendMessage(theApp.m_pMainView->m_hWnd, WM_MZ3_CHANGE_VIEW, NULL, NULL);
		} else {
			// ���|�[�g�r���[�ɖ߂邽�߁A�߂�{�^���L����
			theApp.EnableCommandBarButton(ID_BACK_BUTTON, TRUE);
			::SendMessage(theApp.m_pReportView->m_hWnd, WM_MZ3_CHANGE_VIEW, NULL, NULL);
		}

		if (!IsWriteFromMainView()) {
			// ���|�[�g�r���[�ɖ߂�����̃����[�h����
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
	} else {
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
	}

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

/**
 * ���f
 */
LRESULT CWriteView::OnPostAbort(WPARAM wParam, LPARAM lParam)
{
	theApp.m_access = false;

	// �R���g���[����Ԃ̕ύX
	MyUpdateControlStatus();

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
	m_abort = true;

	LPCTSTR msg = L"";
	if (m_postData->GetSuccessMessage() == WM_MZ3_POST_CONFIRM_END) {
		msg = L"���f���܂������A���e���ꂽ�\��������܂�";
	} else {
		msg = L"���f���܂���";
	}
	util::MySetInformationText( m_hWnd, msg );
	::MessageBox(m_hWnd, msg, MZ3_APP_NAME, MB_ICONSTOP | MB_OK);

	// ���f��ɏ������݃{�^�����������Ɖ��̂������I�����Ă��܂����߁A
	// �{���̈�Ƀt�H�[�J�X��߂��B
	m_bodyEdit.SetFocus();

	theApp.m_access = false;

	// �R���g���[����Ԃ̕ύX
	MyUpdateControlStatus();

	return TRUE;
}

/**
 * �A�N�Z�X���ʒm
 */
LRESULT CWriteView::OnAccessInformation(WPARAM wParam, LPARAM lParam)
{
	m_infoEdit.SetWindowText(*(CString*)lParam);
	return TRUE;
}

BOOL CWriteView::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYUP) {
		switch (pMsg->wParam) {
#ifndef WINCE
		case VK_F1:
			// �w���v�\��
			util::OpenByShellExecute( MZ4_MANUAL_URL );
			break;
#endif

#ifndef WINCE
		case VK_APPS:
			PopupWriteBodyMenu();
			break;
#endif

		case VK_BACK:
#ifndef WINCE
		case VK_ESCAPE:
#endif
			if (theApp.m_access) {
				// �A�N�Z�X���͒��f����
				::SendMessage(m_hWnd, WM_MZ3_ABORT, NULL, NULL);
				return TRUE;
			}
			break;
		}

	}

	return CFormView::PreTranslateMessage(pMsg);
}

/**
 * �O�̉�ʂɐ؂�ւ���
 */
void CWriteView::OnWriteBackMenu()
{
	CMainFrame* pMainFrame = (CMainFrame*)theApp.m_pMainWnd;
	pMainFrame->OnBackButton();
}

LRESULT CWriteView::OnFit(WPARAM wParam, LPARAM lParam)
{
	RECT rect;
	SystemParametersInfo(SPI_GETWORKAREA, 0, &rect, 0);
	if (theApp.m_Platforms.PocketPC) {
		OnSize(SIZE_RESTORED, rect.right - rect.left, rect.bottom - (rect.top*2));
	}

	return TRUE;
}

/**
 * GET ���\�b�h�������̏���
 */
LRESULT CWriteView::OnGetEnd(WPARAM wParam, LPARAM lParam)
{
	util::MySetInformationText( m_hWnd, L"��͒�" );

	CHtmlArray html;
	html.Load( theApp.m_filepath.temphtml );

	if (m_abort) {
		::SendMessage(m_hWnd, WM_MZ3_POST_ABORT, NULL, lParam);
		return TRUE;
	}

	theApp.m_access = false;
	m_abort  = false;

	// �R���g���[����Ԃ̕ύX
	MyUpdateControlStatus();

	// MZ3 API : �t�b�N�֐��Ăяo��
	ACCESS_TYPE access_type = ((CMixiData*)lParam)->GetAccessType();
	util::MyLuaDataList rvals;
	if (util::CallMZ3ScriptHookFunctions2("get_end_write_view", &rvals, 
			util::MyLuaData(m_writeViewType),
			util::MyLuaData(m_data),
			util::MyLuaData(theApp.m_inet.m_dwHttpStatus),
			util::MyLuaData(CStringA(theApp.m_filepath.temphtml)),
			util::MyLuaData(access_type)
			))
	{
		m_bWriteCompleted = true;
		return TRUE;
	}

	if (theApp.m_accessTypeInfo.getServiceType(access_type)=="mixi") {
		switch (access_type) {
		case ACCESS_LOGIN:
			// ���O�C���������ǂ����̊m�F
			if( mixi::HomeParser::IsLoginSucceeded(html) ) {
				// ���O�C������
				if (wcslen(theApp.m_loginMng.GetMixiOwnerID()) != 0) {
					MZ3LOGGER_DEBUG( L"OwnerID �擾�ς�" );
				} else {
					MZ3LOGGER_INFO( L"OwnerID�����擾�Ȃ̂ŁA���O�C�����A�擾���� (2)" );

					((CMixiData*)lParam)->SetAccessType(ACCESS_MAIN);
					theApp.m_accessType = ACCESS_MAIN;
					theApp.m_inet.DoGet(L"http://mixi.jp/check.pl?n=%2Fhome.pl", L"", CInetAccess::FILE_HTML, NULL );
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

			// �p�[�X
			MZ3Data data;
			theApp.DoParseMixiHomeHtml(&data, &html);

			if (wcslen(theApp.m_loginMng.GetMixiOwnerID()) == 0) {
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

		// POST �����𑱍s
		StartEntryPost();
	}

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
	// MZ3 API : �t�b�N�֐��Ăяo��
	util::MyLuaDataList rvals;
	rvals.push_back(util::MyLuaData((int)0));
	if (util::CallMZ3ScriptHookFunctions2("is_enable_write_view_attach_image_mode", &rvals, 
			util::MyLuaData(m_writeViewType),
			util::MyLuaData(m_data)
			)) {
		int is_enable_attach_image_mode = rvals[0].m_number;
		MZ3LOGGER_DEBUG(util::FormatString(L"estimated attach type by lua : %d", is_enable_attach_image_mode));
		return is_enable_attach_image_mode ? true : false;
	}
	MZ3LOGGER_INFO(L"Lua ���ŏ������Ȃ��̂Łu�Y�t�s�v�Ƃ݂Ȃ�");

	return false;
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

/**
 * �|�b�v�A�b�v���j���[
 */
void CWriteView::PopupWriteBodyMenu(void)
{
	POINT pt    = util::GetPopupPosForSoftKeyMenu2();
	int   flags = util::GetPopupFlagsForSoftKeyMenu2();

	CImageList* pGlobalImageList = &theApp.m_imageCache.GetImageList16();

	CWMMenu menu( pGlobalImageList );
	menu.LoadMenu( IDR_WRITE_MENU );
	CMenu* pcThisMenu = menu.GetSubMenu(0);

	// �ʐ^�Y�t���ł��Ȃ���ʂł́u�ʐ^��Y�t����v���j���[������
	if( !IsEnableAttachImageMode() ) {
		pcThisMenu->EnableMenuItem( ID_ATTACH_PHOTO, MF_GRAYED | MF_BYCOMMAND );
	}

	// �G�������j���[
	// mixi �̂݃T�|�[�g
	if (theApp.m_accessTypeInfo.getServiceType(m_writeViewType)=="mixi") {
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
						// ���T�C�Y���ĉ摜�L���b�V���ɒǉ�����B
						imageIndex = theApp.AddImageToImageCache(this, image, path);
					}
				}
//				MZ3LOGGER_DEBUG(util::FormatString(L" %4d:%d (%s)", i, imageIndex, path));
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

		// �ǉ����j���[
		util::MyLuaDataList rvals;
		if (util::CallMZ3ScriptHookFunctions2("popup_write_menu", &rvals, 
				util::MyLuaData(theApp.m_accessTypeInfo.getSerializeKey(m_writeViewType)),
				util::MyLuaData(m_data),
				util::MyLuaData(pcThisMenu)
				))
		{
		}

		// �Z�p���[�^�A�u�O�̉�ʂցv
		pcThisMenu->AppendMenu(MF_SEPARATOR);
		pcThisMenu->AppendMenu(MF_STRING, ID_WRITE_BACK_MENU, L"�O�̉�ʂ�");

		// ���j���[�\��(�R���e�L�X�g�̓s����A�����ŌĂяo��)
		pcThisMenu->TrackPopupMenu( flags, pt.x, pt.y, this );
	} else {
		// mixi �ȊO�ł͔�T�|�[�g
		pcThisMenu->DeleteMenu( 1, MF_BYPOSITION );

		// �ǉ����j���[
		util::MyLuaDataList rvals;
		if (util::CallMZ3ScriptHookFunctions2("popup_write_menu", &rvals, 
				util::MyLuaData(theApp.m_accessTypeInfo.getSerializeKey(m_writeViewType)),
				util::MyLuaData(m_data),
				util::MyLuaData(pcThisMenu)
				))
		{
		}

		// �Z�p���[�^�A�u�O�̉�ʂցv
		pcThisMenu->AppendMenu(MF_SEPARATOR);
		pcThisMenu->AppendMenu(MF_STRING, ID_WRITE_BACK_MENU, L"�O�̉�ʂ�");

		// ���j���[�\��
		pcThisMenu->TrackPopupMenu( flags, pt.x, pt.y, this );
	}
}

/**
 * ��Ԃɉ������R���g���[����Ԃ̕ύX
 */
void CWriteView::MyUpdateControlStatus(void)
{
	// �{�r���[�ł́A�i�ރ{�^���A�u���E�U�ŊJ���{�^���A�������݃{�^���͏�ɖ���
	theApp.EnableCommandBarButton(ID_FORWARD_BUTTON, FALSE);
	theApp.EnableCommandBarButton(ID_OPEN_BROWSER,   FALSE);
	theApp.EnableCommandBarButton(ID_WRITE_BUTTON,   FALSE);

	// �C���[�W�{�^���͉摜���e�\�ȏ�Ԃɂ̂ݗL��
	if (theApp.m_access) {
		theApp.EnableCommandBarButton(ID_IMAGE_BUTTON, FALSE);
	} else {
		theApp.EnableCommandBarButton(ID_IMAGE_BUTTON, IsEnableAttachImageMode() ? TRUE : FALSE);
	}

	// ���~�{�^���F�A�N�Z�X���͗L��
	theApp.EnableCommandBarButton(ID_STOP_BUTTON, theApp.m_access ? TRUE : FALSE);

	// �߂�{�^���F�A�N�Z�X���͖���
	theApp.EnableCommandBarButton(ID_BACK_BUTTON, theApp.m_access ? FALSE : TRUE);

	// ���M�{�^���F�A�N�Z�X���͖���
	m_sendButton.EnableWindow(theApp.m_access ? FALSE : TRUE);

	// �L�����Z���{�^���F�A�N�Z�X���͖���
	m_cancelButton.EnableWindow(theApp.m_access ? FALSE : TRUE);

	// ���̈�F�A�N�Z�X���͕\��
	m_infoEdit.ShowWindow(theApp.m_access ? SW_SHOW : SW_HIDE);
}

/**
 * MZ3 API �œo�^���ꂽ���j���[�̃C�x���g
 */
void CWriteView::OnLuaMenu(UINT nID)
{
	UINT idx = nID - ID_LUA_MENU_BASE;
	if (idx >= theApp.m_luaMenus.size()) {
		MZ3LOGGER_ERROR(util::FormatString(L"�s���ȃ��j���[ID�ł� [%d]", nID));
		return;
	}

	// Lua�֐����擾���Ăяo��
	const std::string& strFuncName = theApp.m_luaMenus[idx];
	util::CallMZ3ScriptHookFunction("", "select_menu", strFuncName.c_str(), &m_data);
}

#endif	// BT_MZ3
