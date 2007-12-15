/*
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */
// WriteView.cpp : 実装ファイル
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

#define GENERATE_POSTMSG_FAILED_MESSAGE L"送信データの作成に失敗しました"

// CWriteView

IMPLEMENT_DYNCREATE(CWriteView, CFormView)

// -----------------------------------------------------------------------------
// コンストラクタ
// -----------------------------------------------------------------------------
CWriteView::CWriteView()
	: CFormView(CWriteView::IDD)
	, m_abort(FALSE)
{
	m_postData = new CPostData();
	m_sendEnd = TRUE;
}

// -----------------------------------------------------------------------------
// デストラクタ
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


// CWriteView 診断

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


// CWriteView メッセージ ハンドラ

void CWriteView::OnInitialUpdate()
{
	GetParentFrame()->RecalcLayout();
	CFormView::OnInitialUpdate();

	// フォントの変更
	{
		// タイトル
		m_titleEdit.SetFont( &theApp.m_font );

		// エディット
		m_bodyEdit.SetFont( &theApp.m_font );

		// ボタン
		m_sendButton.SetFont( &theApp.m_font );
		m_cancelButton.SetFont( &theApp.m_font );

		// 通知領域
		m_infoEdit.SetFont( &theApp.m_font );
	}
}

void CWriteView::OnSize(UINT nType, int cx, int cy)
{
	// 前回の値を保存し、(0,0) の場合はその値を利用する
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
// 書き込みボタン押下時の処理
// -----------------------------------------------------------------------------
void CWriteView::OnBnClickedWriteSendButton()
{
	CString msg;
	GetDlgItemText( IDC_WRITE_BODY_EDIT, msg );

	CString title;
	GetDlgItemText( IDC_WRITE_TITLE_EDIT, title );

	if (msg.GetLength() > 3000) {
		// 最大文字を越えている
		::MessageBox(m_hWnd, L"送信文字数が多すぎます", MZ3_APP_NAME, MB_ICONERROR);
		return;
	}
	else if (title.GetLength() == 0) {
		::MessageBox(m_hWnd, L"タイトルを入力してください", MZ3_APP_NAME, MB_ICONERROR);
		return;
	}
	else if (msg.GetLength() == 0) {
		::MessageBox(m_hWnd, L"本文を入力してください", MZ3_APP_NAME, MB_ICONERROR);
		return;
	}

	{
		CString s;
		switch( m_writeViewType ) {
		case WRITEVIEW_TYPE_COMMENT:
			s = L"コメントを投稿します\nよろしいですか？";
			break;

		case WRITEVIEW_TYPE_NEWDIARY:
			s = L"日記を投稿します\nよろしいですか？";
			break;

		case WRITEVIEW_TYPE_REPLYMESSAGE:
			s = L"メッセージを返信します\nよろしいですか？";
			break;

		case WRITEVIEW_TYPE_NEWMESSAGE:
			s = L"メッセージを送信します\nよろしいですか？";
			break;
		}

		// 添付画像があればファイル名を追加。
		if( !m_photo1_filepath.IsEmpty() ||
			!m_photo2_filepath.IsEmpty() ||
			!m_photo3_filepath.IsEmpty() ) 
		{
			s.Append( L"\n\n" );
			s.Append( L"添付画像：\n" );
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
			// 処理を中止
			return;
		}
	}

	m_access = TRUE;
	m_sendButton.EnableWindow(FALSE);
	m_cancelButton.EnableWindow(FALSE);
	m_infoEdit.ShowWindow(SW_SHOW);

	if (wcslen(theApp.m_loginMng.GetOwnerID()) == 0) {
		MZ3LOGGER_INFO( L"OwnerIDが未取得なので、ログインし、取得する(1)" );

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

	// キー押下イベントを奪うためにフォーカスを取得する
	SetFocus();
}

// -----------------------------------------------------------------------------
// 中止ボタン押下時の処理
// -----------------------------------------------------------------------------
void CWriteView::OnBnClickedWriteCancelButton()
{
	int ret;
	CString msg;
	((CEdit*)GetDlgItem(IDC_WRITE_BODY_EDIT))->GetWindowText(msg);

	if (msg.GetLength() > 0) {
		ret = ::MessageBox(m_hWnd, L"未投稿のデータがあります\n破棄されますがいいですか？",
			MZ3_APP_NAME, MB_ICONQUESTION | MB_OKCANCEL);
		if (ret == IDCANCEL) {
			// 処理を中止
			return;
		}
	}

	// 内容をクリア
	((CEdit*)GetDlgItem(IDC_WRITE_TITLE_EDIT))->SetWindowText(L"");
	((CEdit*)GetDlgItem(IDC_WRITE_BODY_EDIT))->SetWindowText(L"");

	// 前の画面に戻る
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
 * 書き込み画面の投稿（＝確認画面の表示）完了。書き込みのシミュレート。
 */
LRESULT CWriteView::OnPostConfirm(WPARAM wParam, LPARAM lParam)
{
	if (m_abort != FALSE) {
		::SendMessage(m_hWnd, WM_MZ3_POST_ABORT, NULL, NULL);
		return TRUE;
	}

	// ログアウトチェック
	bool bLogout = false;
	if (mixi::MixiParserBase::isLogout(theApp.m_filepath.temphtml) ) {
		bLogout = true;
	} else if (wcslen(theApp.m_loginMng.GetOwnerID())==0) {
		// オーナーID未取得の場合もログアウトとみなす。
		bLogout = true;
	}

	if (bLogout) {
		// ログアウト状態になっている
		// ログイン処理実施
		MZ3LOGGER_INFO(_T("ログインします。"));
		theApp.m_mixi4recv.SetAccessType(ACCESS_LOGIN);

		theApp.m_accessType = ACCESS_LOGIN;
		theApp.m_inet.Initialize( m_hWnd, &theApp.m_mixi4recv );
		theApp.m_inet.DoGet(theApp.MakeLoginUrl(), NULL, CInetAccess::FILE_HTML );

		return TRUE;
	}

	// 確認画面判定
	CHtmlArray html;
	html.Load( theApp.m_filepath.temphtml );

	// 解析用に確認画面を保存しておく
	CString strConfirmTempFile = theApp.m_filepath.temphtml + L".confirm.html";
	CopyFile( theApp.m_filepath.temphtml, strConfirmTempFile, FALSE );

	// 確認画面の解析
	if( html.GetPostConfirmData(m_postData) == false ) {
		CString msg = 
			L"エラーが発生しました\r\n"
			L"確認画面にエラー内容が表示されていますが、"
			L"本バージョンの" MZ3_APP_NAME L"ではエラー内容を表示できません・・・\r\n"
			L"お手数ですが、下記ファイルを直接解析してくださいm(_ _)m\r\n"
			+ strConfirmTempFile;
		MessageBox( msg );
		MZ3LOGGER_ERROR( msg );

		MZ3LOGGER_ERROR( L"書き込みに失敗したため、念のためOwnerIDを初期化します" );
		theApp.m_loginMng.SetOwnerID( L"" );

		return 0;
	}

	// 書き込み開始
	StartEntryPost();

	return TRUE;
}

/**
 * 確認画面｜書き込みボタン押下時の電文送信
 */
void CWriteView::StartEntryPost() 
{

	// 中断確認
	if (m_abort != FALSE) {
		::SendMessage(m_hWnd, WM_MZ3_POST_ABORT, NULL, NULL);
		return;
	}
	m_abort = FALSE;

	CString url;	// URL
	CString refUrl;	// リファラー用URL

	switch (m_writeViewType) {
	case WRITEVIEW_TYPE_COMMENT:
		{
			// 電文の生成
			if( !mixi::EntryCommentGenerator::generate( *m_postData, *m_data ) ) {
				MessageBox( GENERATE_POSTMSG_FAILED_MESSAGE );
				return;
			}

			// URL の生成
			switch (m_data->GetAccessType()) {
			case ACCESS_BBS:
			case ACCESS_EVENT:
			case ACCESS_ENQUETE:
				url.Format( L"http://mixi.jp/%s", m_data->GetPostAddress());
				break;

			case ACCESS_DIARY:
			case ACCESS_MYDIARY:
				url.Format( L"http://mixi.jp/%s&owner_id=%d", m_data->GetPostAddress(), m_data->GetOwnerID());
				//リファラ設定したら投稿で止まるようになったので保留
				//refUrl.Format( L"http://mixi.jp/%s", m_data->GetPostAddress());
				break;
			}
		}
		break;

	case WRITEVIEW_TYPE_NEWDIARY:
		{
			// タイトルを取得する
			CString title;
			GetDlgItemText( IDC_WRITE_TITLE_EDIT, title );

			// 電文の生成
			if( !mixi::EntryDiaryGenerator::generate( *m_postData, title ) ) {
				MessageBox( GENERATE_POSTMSG_FAILED_MESSAGE );
				return;
			}

			// URL の生成
			url = L"http://mixi.jp/add_diary.pl";
		}
		break;

	case WRITEVIEW_TYPE_REPLYMESSAGE:
		{
			// タイトルを取得する
			CString title;
			GetDlgItemText( IDC_WRITE_TITLE_EDIT, title );

			// 電文の生成
			if( !mixi::EntryReplyMessageGenerator::generate( *m_postData, title ) ) {
				MessageBox( GENERATE_POSTMSG_FAILED_MESSAGE );
				return;
			}

			// URL の生成
			url    = m_postData->GetConfirmUri();
			refUrl = m_postData->GetConfirmUri();
		}
		break;

	case WRITEVIEW_TYPE_NEWMESSAGE:
		{
			// タイトルを取得する
			CString title;
			GetDlgItemText( IDC_WRITE_TITLE_EDIT, title );

			// 電文の生成
			if( !mixi::EntryNewMessageGenerator::generate( *m_postData, title ) ) {
				MessageBox( GENERATE_POSTMSG_FAILED_MESSAGE );
				return;
			}

			// URL の生成
			url    = m_postData->GetConfirmUri();
			refUrl = m_postData->GetConfirmUri();
		}
		break;

	default:
		{
			CString s;
			s.Format( L"未サポートの送信種別です [%d]", m_writeViewType );
			MessageBox( s );

			return;
		}
		break;
	}

	// 成功時のメッセージを設定する
	m_postData->SetSuccessMessage( WM_MZ3_POST_END );

	// アクセス種別の設定
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
 * 書き込み完了（確認画面｜書き込みボタン押下⇒成功）
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
				::MessageBox(m_hWnd, L"メッセージを送信しました", MZ3_APP_NAME, MB_OK);
				break;
			default:
				::MessageBox(m_hWnd, L"投稿しました", MZ3_APP_NAME, MB_OK);
				break;
			}
		}
		else {
			// 中断が押されていた場合は上に返す
			return TRUE;
		}

		((CEdit*)GetDlgItem(IDC_WRITE_TITLE_EDIT))->SetWindowText(L"");
		((CEdit*)GetDlgItem(IDC_WRITE_BODY_EDIT))->SetWindowText(L"");

		// 進むボタンを使用不可にする
		theApp.EnableCommandBarButton( ID_FORWARD_BUTTON, FALSE);

		if( IsWriteFromMainView() ) {
			theApp.EnableCommandBarButton( ID_BACK_BUTTON, FALSE);
		} else {
			theApp.EnableCommandBarButton( ID_BACK_BUTTON, TRUE);
		}
		m_sendEnd = TRUE;

		// 前の画面に戻る
		if( IsWriteFromMainView() ) {
			::SendMessage(theApp.m_pMainView->m_hWnd, WM_MZ3_CHANGE_VIEW, NULL, NULL);
		}else{
			::SendMessage(theApp.m_pReportView->m_hWnd, WM_MZ3_CHANGE_VIEW, NULL, NULL);

			switch (m_writeViewType ) {
			case WRITEVIEW_TYPE_REPLYMESSAGE:
			case WRITEVIEW_TYPE_NEWMESSAGE:
				// メッセージならリロードしない
				break;
			default:
				::SendMessage(theApp.m_pReportView->m_hWnd, WM_MZ3_RELOAD, NULL, NULL);
				break;
			}
		}
	}
	else {
		LPCTSTR msg = L"投稿に失敗しました(1)";
		util::MySetInformationText( m_hWnd, msg );

		MZ3LOGGER_ERROR( msg );

		CString s;
		s.Format( 
			L"%s\n\n"
			L"%s に今回の書き込み内容を保存しました。", msg, theApp.m_filepath.tempdraftfile );
		::MessageBox(m_hWnd, s, MZ3_APP_NAME, MB_ICONERROR);

		// 失敗したのでダンプする
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
 * 書き込み内容をダンプする
 */
bool CWriteView::DumpToTemporaryDraftFile()
{
	FILE* fp = _wfopen( theApp.m_filepath.tempdraftfile, L"w" );

	if( fp == NULL ) {
		MZ3LOGGER_FATAL( L"ダンプファイルオープン失敗" );
		return false;
	}

	// 1行目：タイトル
	CString title;
	GetDlgItemText( IDC_WRITE_TITLE_EDIT, title );
	fwprintf( fp, L"%s\n", title );

	// 2行目以降：本文
	CString msg;
	GetDlgItemText( IDC_WRITE_BODY_EDIT, msg );
	fwprintf( fp, L"%s\n", msg );

	fclose( fp );

	return true;
}

// -----------------------------------------------------------------------------
// 中断
// -----------------------------------------------------------------------------
LRESULT CWriteView::OnPostAbort(WPARAM wParam, LPARAM lParam)
{
	// 中止ボタンを使用不可にする
	theApp.EnableCommandBarButton( ID_STOP_BUTTON, FALSE);
	theApp.EnableCommandBarButton( ID_BACK_BUTTON, TRUE);

	m_sendButton.EnableWindow(TRUE);
	m_cancelButton.EnableWindow(TRUE);

	m_infoEdit.ShowWindow(SW_HIDE);

	m_access = FALSE;

	return TRUE;
}

/**
 * 中断ボタン押下時の処理
 */
LRESULT CWriteView::OnAbort(WPARAM wParam, LPARAM lParam)
{
	if( theApp.m_inet.IsConnecting() ) {
		// 通信中ならば、Abort を呼び出す
		theApp.m_inet.Abort();
	}
	m_abort = TRUE;

	LPCTSTR msg = L"";
	if (m_postData->GetSuccessMessage() == WM_MZ3_POST_CONFIRM) {
		msg = L"中断しましたが、投稿された可能性があります";
	}
	else {
		msg = L"中断しました";
	}
	util::MySetInformationText( m_hWnd, msg );
	::MessageBox(m_hWnd, msg, MZ3_APP_NAME, MB_ICONSTOP | MB_OK);

	// 中止ボタンを使用不可にする
	theApp.EnableCommandBarButton( ID_STOP_BUTTON, FALSE);
	theApp.EnableCommandBarButton( ID_BACK_BUTTON, TRUE);

	m_sendButton.EnableWindow(TRUE);
	m_cancelButton.EnableWindow(TRUE);

	m_infoEdit.ShowWindow(SW_HIDE);

	// 中断後に書き込みボタンが押されると何故か強制終了してしまうため、
	// 本文領域にフォーカスを戻す。
	m_bodyEdit.SetFocus();

	return TRUE;
}

// -----------------------------------------------------------------------------
// アクセス情報通知
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
				// メインメニューのポップアップ
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
			// ヘルプ表示
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
				// アクセス中は中断処理
				::SendMessage(m_hWnd, WM_MZ3_ABORT, NULL, NULL);
				return TRUE;
			}
			break;
		}

	}

	return CFormView::PreTranslateMessage(pMsg);
}

// -----------------------------------------------------------------------------
// 前の画面に切り替える
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
	util::MySetInformationText( m_hWnd, L"HTML解析中" );

	CHtmlArray html;
	html.Load( theApp.m_filepath.temphtml );

	if (m_abort != FALSE) {
		::SendMessage(m_hWnd, WM_MZ3_POST_ABORT, NULL, lParam);
		return TRUE;
	}

	switch( ((CMixiData*)lParam)->GetAccessType() ) {
	case ACCESS_LOGIN:
		// ログインしたかどうかの確認
		if( mixi::HomeParser::IsLoginSucceeded(html) ) {
			// ログイン成功
			if (wcslen(theApp.m_loginMng.GetOwnerID()) != 0) {
				MZ3LOGGER_DEBUG( L"OwnerID 取得済み" );
			} else {
				MZ3LOGGER_INFO( L"OwnerIDが未取得なので、ログインし、取得する (2)" );

				((CMixiData*)lParam)->SetAccessType(ACCESS_MAIN);
				theApp.m_accessType = ACCESS_MAIN;
				theApp.m_inet.DoGet(L"http://mixi.jp/check.pl?n=%2Fhome.pl", L"", CInetAccess::FILE_HTML );
				return TRUE;
			}
		} else {
			// ログイン失敗
			LPCTSTR msg = L"ログインに失敗しました";
			util::MySetInformationText( m_hWnd, msg );
			::MessageBox(m_hWnd, msg, MZ3_APP_NAME, MB_ICONERROR);

			::SendMessage(m_hWnd, WM_MZ3_POST_ABORT, NULL, lParam);
			return TRUE;
		}
		break;

	case ACCESS_MAIN:

		mixi::HomeParser::parse( html );

		if (wcslen(theApp.m_loginMng.GetOwnerID()) == 0) {
			LPCTSTR msg = L"投稿に失敗しました(2)";
			util::MySetInformationText( m_hWnd, msg );

			MZ3LOGGER_ERROR( msg );

			CString s;
			s.Format( 
				L"%s\n\n"
				L"%s に今回の書き込み内容を保存しました。", msg, theApp.m_filepath.tempdraftfile );
			::MessageBox(m_hWnd, s, MZ3_APP_NAME, MB_ICONERROR);

			// 失敗したのでダンプする
			DumpToTemporaryDraftFile();

			::SendMessage(m_hWnd, WM_MZ3_POST_ABORT, NULL, lParam);
			return TRUE;
		}
		break;
	}

	CString msg;
	GetDlgItemText( IDC_WRITE_BODY_EDIT, msg );

	// POST 処理を続行
	StartConfirmPost( msg );

	return TRUE;
}

void CWriteView::OnSetFocus(CWnd* pOldWnd)
{
	CFormView::OnSetFocus(pOldWnd);

}

/**
 * 画像添付確認画面の表示＆画像ファイル名の取得
 */
CString CWriteView::MyShowDlgToConfirmPostImage( CString selectedFilepath )
{
	// 選択済みであれば変更するか訪ねる
	if( !selectedFilepath.IsEmpty() ) {
		CString msg;
		msg.Format( 
			L"添付ファイルを変更しますか？\n"
			L"\n"
			L"ファイル：%s\n"
			L"\n"
			L"【はい】変更する\n"
			L"【いいえ】変更しない", (LPCTSTR) selectedFilepath );
		if( MessageBox( msg, 0, MB_YESNO ) == IDNO ) {
			// いいえ（変更しない）なので、元のファイルを返す
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
	ofn.lpstrTitle = L"JPEGﾌｧｲﾙを開く...";
	//フィルタを一個にするとホーミンさんのFileDialgChangerインストール時に画像サムネイルに対応できるため変更
	//ofn.lpstrFilter = L"JPEGﾌｧｲﾙ (*.jpg)\0*.jpg;*.jpeg\0すべてのﾌｧｲﾙ (*.*)\0*.*\0\0";
	ofn.lpstrFilter = L"JPEGﾌｧｲﾙ (*.jpg)\0*.jpg;*.jpeg\0\0";
	ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST;
	ofn.lpstrInitialDir = L"";
	if (GetOpenFileName(&ofn) == IDOK) {
		return szFile;
	}

	// キャンセルされたので元のファイルを返す
	return selectedFilepath;
}

/**
 * イメージボタン押下時の処理
 */
void CWriteView::OnImageButton()
{
	POINT pt    = util::GetPopupPos();
	int   flags = util::GetPopupFlags();

	CMenu menu;
	menu.LoadMenu( IDR_ATTACH_IMAGE_MENU );
	CMenu* pcThisMenu = menu.GetSubMenu(0);

	// ●添付済みの場合
	// 　「添付ファイルを変更する」
	// 　「添付をやめる」
	// ●未添付の場合
	// 　「添付ファイルを選択する」
	LPCTSTR photo_files[] = { m_photo1_filepath, m_photo2_filepath, m_photo3_filepath };
	for( int i=0; i<3; i++ ) {
		CMenu* pSubMenu = pcThisMenu->GetSubMenu(i);

		int id_attach = ID_ATTACH_PHOTO1+i;
		int id_cancel = ID_CANCEL_ATTACH_PHOTO1+i;
		int id_preview = ID_PREVIEW_ATTACHED_PHOTO1+i;
		if( wcslen(photo_files[i])==0 ) {
			// 未添付
			pSubMenu->ModifyMenu( id_attach, MF_BYCOMMAND, id_attach, _T("ファイルを選択する") );
			pSubMenu->RemoveMenu( id_cancel, MF_BYCOMMAND );
			pSubMenu->RemoveMenu( id_preview, MF_BYCOMMAND );
		}else{
			// 添付済み
			pSubMenu->ModifyMenu( id_attach, MF_BYCOMMAND, id_attach, _T("ファイルを変更する") );
			pSubMenu->ModifyMenu( id_cancel, MF_BYCOMMAND, id_cancel, _T("添付をやめる") );
		}
	}

	pcThisMenu->TrackPopupMenu( flags, pt.x, pt.y, this );
}

/**
 * 書き込み画面を開始する。
 */
void CWriteView::StartWriteView(WRITEVIEW_TYPE writeViewType, CMixiData* pMixi)
{
	m_data = pMixi;
	m_access = FALSE;

	// 内容をクリア
	SetDlgItemText( IDC_WRITE_TITLE_EDIT, L"" );
	SetDlgItemText( IDC_WRITE_BODY_EDIT, L"" );

	// 種別を保存
	m_writeViewType = writeViewType;

	// 書きかけであることを設定
	m_sendEnd = FALSE;

	switch( writeViewType ) {
	case WRITEVIEW_TYPE_REPLYMESSAGE:
		// タイトル変更：有効
		m_titleEdit.SetReadOnly(FALSE);

		if (m_data != NULL) {
			// タイトルの初期値を設定
			SetDlgItemText( IDC_WRITE_TITLE_EDIT, L"Re : " + m_data->GetTitle() );
		}

		// フォーカス：本文から開始
		m_bodyEdit.SetFocus();
		break;

	case WRITEVIEW_TYPE_NEWMESSAGE:
		// タイトル変更：有効
		m_titleEdit.SetReadOnly(FALSE);

		// フォーカス：タイトルから開始
		m_titleEdit.SetFocus();
		break;

	case WRITEVIEW_TYPE_COMMENT:
		// タイトル変更：無効
		m_titleEdit.SetReadOnly(TRUE);

		if (m_data != NULL) {
			// タイトルを設定
			SetDlgItemText( IDC_WRITE_TITLE_EDIT, m_data->GetTitle() );
		}

		// フォーカス：本文から開始
		m_bodyEdit.SetFocus();
		break;

	case WRITEVIEW_TYPE_NEWDIARY:
		// タイトル変更：有効
		m_titleEdit.SetReadOnly(FALSE);

		// フォーカス：タイトルから開始
		m_titleEdit.SetFocus();
		break;

	default:
		MessageBox( L"アプリケーション内部エラー：未対応の書き込み画面種別です" );
		return;
	}

	// ボタン制御
	theApp.EnableCommandBarButton( ID_FORWARD_BUTTON, FALSE );
	theApp.EnableCommandBarButton( ID_BACK_BUTTON,    TRUE  );
	theApp.EnableCommandBarButton( ID_OPEN_BROWSER,   FALSE );

	// イメージボタンは画像投稿可能な状態にのみ有効
	theApp.EnableCommandBarButton( ID_IMAGE_BUTTON, IsEnableAttachImageMode() ? TRUE : FALSE );

	// 画像添付関連の初期化
	m_photo1_filepath = L"";
	m_photo2_filepath = L"";
	m_photo3_filepath = L"";

	// ボタンの初期化
	m_sendButton.EnableWindow(TRUE);
	m_cancelButton.EnableWindow(TRUE);
	m_infoEdit.ShowWindow(SW_HIDE);

	// Ｖｉｅｗ入れ替え
	theApp.ChangeView(theApp.m_pWriteView);
}

/// 写真１｜添付
void CWriteView::OnAttachPhoto1()
{
	m_photo1_filepath = MyShowDlgToConfirmPostImage( m_photo1_filepath );
}

/// 写真２｜添付
void CWriteView::OnAttachPhoto2()
{
	m_photo2_filepath = MyShowDlgToConfirmPostImage( m_photo2_filepath );
}

/// 写真３｜添付
void CWriteView::OnAttachPhoto3()
{
	m_photo3_filepath = MyShowDlgToConfirmPostImage( m_photo3_filepath );
}

/// 写真１｜添付をやめる
void CWriteView::OnCancelAttachPhoto1() { m_photo1_filepath = L""; }

/// 写真２｜添付をやめる
void CWriteView::OnCancelAttachPhoto2() { m_photo2_filepath = L""; }

/// 写真３｜添付をやめる
void CWriteView::OnCancelAttachPhoto3() { m_photo3_filepath = L""; }

/// 写真１｜プレビュー
void CWriteView::OnPreviewAttachedPhoto1()
{
	if( !m_photo1_filepath.IsEmpty() ) { 
		util::OpenByShellExecute( m_photo1_filepath );
	}
}

/// 写真２｜プレビュー
void CWriteView::OnPreviewAttachedPhoto2()
{
	if( !m_photo2_filepath.IsEmpty() ) { 
		util::OpenByShellExecute( m_photo2_filepath );
	}
}

/// 写真３｜プレビュー
void CWriteView::OnPreviewAttachedPhoto3()
{
	if( !m_photo3_filepath.IsEmpty() ) { 
		util::OpenByShellExecute( m_photo3_filepath );
	}
}

/// 右ソフトキーメニュー｜写真を添付する
void CWriteView::OnAttachPhoto()
{
	// 「画像ボタン」押下時のメニューをさらに表示する
	OnImageButton();
}

/// 画像が添付可能なモードであるかを返す
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
 * 確認画面への遷移開始
 */
void CWriteView::StartConfirmPost( CString wmsg )
{
	// ユーザが入力したメッセージを EUC-JP URL Encoded String に変換する
	CString euc_msg = URLEncoder::encode_euc(wmsg);
	TRACE(L"euc-jp url encoded string = [%s]\n", euc_msg);

	CString url;

	switch (m_writeViewType) {
	case WRITEVIEW_TYPE_COMMENT:
		// コメントの投稿確認
		{
			// 電文生成
			if( !mixi::PostCommentGenerator::generate( *m_postData, *m_data, euc_msg, 
													   m_photo1_filepath, m_photo2_filepath, m_photo3_filepath ) ) 
			{
				MessageBox( GENERATE_POSTMSG_FAILED_MESSAGE );
				return;
			}

			// リクエストURL生成
			url.Format( L"http://mixi.jp/%s", m_data->GetPostAddress() );
		}
		break;

	case WRITEVIEW_TYPE_NEWDIARY:
		// 日記の投稿確認
		{
			// 電文生成
			if( !mixi::PostDiaryGenerator::generate( *m_postData, theApp.m_loginMng.GetOwnerID(), euc_msg, 
													 m_photo1_filepath, m_photo2_filepath, m_photo3_filepath ) )
			{
				MessageBox( GENERATE_POSTMSG_FAILED_MESSAGE );
				return;
			}

			// リクエストURL生成
			url = L"http://mixi.jp/add_diary.pl";
		}
		break;

	case WRITEVIEW_TYPE_REPLYMESSAGE:
		// メッセージの返信確認
		{
			// 電文生成
			if( !mixi::PostReplyMessageGenerator::generate( *m_postData, euc_msg ) ) {
				MessageBox( GENERATE_POSTMSG_FAILED_MESSAGE );
				return;
			}

			// リクエストURL生成
			// http://mixi.jp/view_message.pl?id=xxx&box=inbox
			// ↓
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
		// 新規メッセージの送信確認
		{
			// 電文生成
			if( !mixi::PostNewMessageGenerator::generate( *m_postData, euc_msg ) ) {
				MessageBox( GENERATE_POSTMSG_FAILED_MESSAGE );
				return;
			}

			// リクエストURL生成

			// m_data から id を取得する
			// http://mixi.jp/show_friend.pl?id=xxx
			int id = mixi::MixiUrlParser::GetID( m_data->GetURL() );
			if( id <= 0 ) {
				MessageBox( L"送信先ユーザの ID が不明です。メッセージを送信できません。" );
				return;
			}

			// URL 生成
			url = util::FormatString( L"http://mixi.jp/send_message.pl?id=%d", id );
			m_postData->SetConfirmUri(url);
		}

		break;

	default:
		{
			CString s;
			s.Format( L"未サポートの送信種別です [%d]", m_writeViewType );
			MessageBox( s );

			return;
		}
		break;
	}

	// 成功時のメッセージを設定する
	m_postData->SetSuccessMessage( WM_MZ3_POST_CONFIRM );

	// アクセス種別の設定
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

	// リファラ設定
	LPCTSTR refUrl = L"";
  //リファラ設定したら投稿で止まるようになったので保留
	//LPCTSTR refUrl = m_data->GetURL();

	// 通信開始
	theApp.m_inet.Initialize( m_hWnd, NULL );
	theApp.m_inet.DoPost(
		url, 
		refUrl, 
		CInetAccess::FILE_HTML, 
		m_postData );
}

/// 絵文字挿入
void CWriteView::OnInsertEmoji(UINT nID)
{
	int emojiIndex = nID - IDM_INSERT_EMOJI_BEGIN;
	// index check
	if (emojiIndex < 0 || emojiIndex >= (int)theApp.m_emoji.size()) {
		return;
	}

	// 現在のカーソル位置に挿入（または選択範囲の置換）
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

	// 写真添付ができない画面では「写真を添付する」メニューを消す
	if( !IsEnableAttachImageMode() ) {
		pcThisMenu->EnableMenuItem( ID_ATTACH_PHOTO, MF_GRAYED | MF_BYCOMMAND );
	}

	// 絵文字メニュー
	const int SUBMENU_MAX = 20;
	int iSubMenu = 0;
	CWMMenu emojiSubMenu[SUBMENU_MAX] = {
		CWMMenu(pGlobalImageList), CWMMenu(pGlobalImageList), CWMMenu(pGlobalImageList), CWMMenu(pGlobalImageList), CWMMenu(pGlobalImageList), 
		CWMMenu(pGlobalImageList), CWMMenu(pGlobalImageList), CWMMenu(pGlobalImageList), CWMMenu(pGlobalImageList), CWMMenu(pGlobalImageList), 
		CWMMenu(pGlobalImageList), CWMMenu(pGlobalImageList), CWMMenu(pGlobalImageList), CWMMenu(pGlobalImageList), CWMMenu(pGlobalImageList), 
		CWMMenu(pGlobalImageList), CWMMenu(pGlobalImageList), CWMMenu(pGlobalImageList), CWMMenu(pGlobalImageList), CWMMenu(pGlobalImageList), 
	};

	std::vector<WMMENUBITMAP> menuBitmapArray;
	// ダミーを削除
	CMenu* emojiMenu = pcThisMenu->GetSubMenu(1);
	emojiMenu->DeleteMenu( IDM_INSERT_EMOJI_BEGIN, MF_BYCOMMAND );

	// N個ずつメニューにして追加
	const int MENU_SPLIT_COUNT = 20;
	if (!theApp.m_emoji.empty()) {
		emojiSubMenu[iSubMenu].CreatePopupMenu();

		for (size_t i=0; i<theApp.m_emoji.size(); i++) {
			if (i%MENU_SPLIT_COUNT == 0 && 
				emojiSubMenu[iSubMenu].GetMenuItemCount()>0 && 
				iSubMenu<SUBMENU_MAX-1)
			{
				// 親に追加
				emojiMenu->AppendMenu( MF_POPUP, (UINT)emojiSubMenu[iSubMenu].m_hMenu, util::FormatString(L"%d", iSubMenu+1) );
				iSubMenu++;
				emojiSubMenu[iSubMenu].CreatePopupMenu();
			}

			// テキスト設定
			int idItem = IDM_INSERT_EMOJI_BEGIN+i;
			emojiSubMenu[iSubMenu].AppendMenu( MF_STRING, idItem, theApp.m_emoji[i].text );

			// 絵文字ロード
			// とりあえず Win32 限定
			// （アドエスでなぜか ToOwnerDraw 内で落ちるため・・・）
#ifndef WINCE
			CString path = util::MakeImageLogfilePathFromUrl( theApp.m_emoji[i].url );

			// ロード済みか判定
			int imageIndex = theApp.m_imageCache.GetImageIndex( path );
			if (imageIndex<0) {
				// 未ロードなのでロード
				CMZ3BackgroundImage image(L"");
				image.load( path );
				if (image.isEnableImage()) {
					// 16x16 にリサイズする。
					CMZ3BackgroundImage resizedImage(L"");
					util::MakeResizedImage( this, resizedImage, image, 16, 16 );

					// ビットマップの追加
					CBitmap bm;
					bm.Attach( resizedImage.getHandle() );

					// グローバルキャッシュに追加し、インデックスを取得する
					imageIndex = theApp.m_imageCache.Add( &bm, (CBitmap*) NULL, path );
				}
			}
			if (imageIndex>=0) {
				// メニュー用ビットマップリソースに追加
				WMMENUBITMAP menuBmp = { idItem, imageIndex };
				menuBitmapArray.push_back(menuBmp);
			}
#endif
		}
		if (emojiSubMenu[iSubMenu].GetMenuItemCount() > 0) {
			// 親に追加
			emojiMenu->AppendMenu( MF_POPUP, (UINT)emojiSubMenu[iSubMenu].m_hMenu, util::FormatString(L"%d", iSubMenu+1) );
		}
	}

	// オーナードロー準備（ビットマップの設定）
	if (!menuBitmapArray.empty() ) {
		menu.PrepareOwnerDraw( &menuBitmapArray[0], menuBitmapArray.size() );
	}

	// メニュー表示
	pcThisMenu->TrackPopupMenu( flags, pt.x, pt.y, this );
}
