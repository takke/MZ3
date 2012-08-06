/*
 * Copyright (c) MZ3 Project Team All rights reserved.
 * Code licensed under the BSD License:
 * http://www.mz3.jp/license.txt
 */
// WriteView.cpp : 実装ファイル
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

#define GENERATE_POSTMSG_FAILED_MESSAGE L"送信データの作成に失敗しました"

// CWriteView

IMPLEMENT_DYNCREATE(CWriteView, CFormView)

// -----------------------------------------------------------------------------
// コンストラクタ
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

		// 公開範囲コンボボックス
		m_viewlimitCombo.SetFont( &theApp.m_font );
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

	int hEdit = theApp.GetInfoRegionHeight(theApp.m_optionMng.GetFontHeightByPixel(theApp.GetDPI()));

	// コンボボックスの高さを取得する
	int hCombo ;
	if( m_viewlimitCombo.m_hWnd ) {				// 初期化前に呼ばれたりするので判定
		// コンボボックスがある
		// コンボボックスのサイズは自動的に決まるのでそれを取ってくる
		RECT rct;
		m_viewlimitCombo.GetWindowRect( &rct );
		hCombo = rct.bottom - rct.top ;
	} else {
		// コンボボックスがない
		// しょうがないのでエディットボックスの高さを採用する
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
 * 書き込み画面の開始
 *
 * 他ビューからのエントリポイント
 */
void CWriteView::StartWriteView(WRITEVIEW_TYPE writeViewType, CMixiData* pMixi)
{
	theApp.m_access = false;
	m_abort  = false;

	m_data = pMixi;
	int iComboIndex;

	// 内容をクリア
	SetDlgItemText( IDC_WRITE_TITLE_EDIT, L"" );
	SetDlgItemText( IDC_WRITE_BODY_EDIT, L"" );
	m_titleEdit.SetReadOnly(FALSE);
	m_viewlimitCombo.ResetContent();
	m_viewlimitCombo.EnableWindow(FALSE);

	// 初期フォーカス
	m_bodyEdit.SetFocus();

	// 種別を保存
	m_writeViewType = writeViewType;

	// 書きかけであることを設定
	m_bWriteCompleted = false;

	// MZ3 API : フック関数呼び出し
	util::MyLuaDataList rvals;
	rvals.push_back(util::MyLuaData(0));	// m_bFromMainView
	rvals.push_back(util::MyLuaData(""));	// init_focus
	rvals.push_back(util::MyLuaData(0));	// enable_combo_box
	rvals.push_back(util::MyLuaData(0));	// enable_title_change
	if (util::CallMZ3ScriptHookFunctions2("init_write_view", &rvals, 
			util::MyLuaData(m_writeViewType),
			util::MyLuaData(m_data)
			)) {

		// サポートしているので続行
		if (rvals[0].m_number) {
			m_bFromMainView = true;
		} else {
			m_bFromMainView = false;
		}

		// 初期フォーカス
		if (rvals[1].m_strText=="body") {
			m_bodyEdit.SetFocus();
		} else if (rvals[1].m_strText=="title") {
			m_titleEdit.SetFocus();
		}

		// コンボボックス有効・無効
		if (rvals[2].m_number) {
			m_viewlimitCombo.EnableWindow(TRUE);
		} else {
			m_viewlimitCombo.EnableWindow(FALSE);
		}

		// タイトル変更有効・無効
		if (rvals[3].m_number) {
			m_titleEdit.SetReadOnly(FALSE);
		} else {
			m_titleEdit.SetReadOnly(TRUE);
		}

	} else {
		// Lua 側でサポートしていないので従来のコード。
		switch( writeViewType ) {
		case WRITEVIEW_TYPE_REPLYMESSAGE:
			// タイトル変更：有効
			m_titleEdit.SetReadOnly(FALSE);

			if (m_data != NULL) {
				// タイトルの初期値を設定
				SetDlgItemText( IDC_WRITE_TITLE_EDIT, L"Re : " + m_data->GetTitle() );
			}

			// 公開範囲コンボボックス：無効
			m_viewlimitCombo.EnableWindow(FALSE);

			// フォーカス：本文から開始
			m_bodyEdit.SetFocus();

			// 戻り先設定
			m_bFromMainView = false;
			break;

		case WRITEVIEW_TYPE_NEWMESSAGE:
			// タイトル変更：有効
			m_titleEdit.SetReadOnly(FALSE);

			// 公開範囲コンボボックス：無効
			m_viewlimitCombo.EnableWindow(FALSE);

			// フォーカス：タイトルから開始
			m_titleEdit.SetFocus();

			// 戻り先設定
			m_bFromMainView = true;
			break;

		case WRITEVIEW_TYPE_COMMENT:
			// タイトル変更：無効
			m_titleEdit.SetReadOnly(TRUE);

			if (m_data != NULL) {
				// タイトルを設定
				SetDlgItemText( IDC_WRITE_TITLE_EDIT, m_data->GetTitle() );
			}

			// 公開範囲コンボボックス：無効
			m_viewlimitCombo.EnableWindow(FALSE);

			// フォーカス：本文から開始
			m_bodyEdit.SetFocus();

			// 戻り先設定
			m_bFromMainView = false;
			break;

		case WRITEVIEW_TYPE_NEWDIARY:
			// タイトル変更：有効
			m_titleEdit.SetReadOnly(FALSE);

			// 公開範囲コンボボックス
			iComboIndex = m_viewlimitCombo.AddString( L"標準の公開設定" );
			m_viewlimitCombo.SetItemData( iComboIndex , 0 );
			iComboIndex = m_viewlimitCombo.AddString( L"非公開" );
			m_viewlimitCombo.SetItemData( iComboIndex , 1 );
			iComboIndex = m_viewlimitCombo.AddString( L"友人まで公開" );
			m_viewlimitCombo.SetItemData( iComboIndex , 2 );
			iComboIndex = m_viewlimitCombo.AddString( L"友人の友人まで公開" );
			m_viewlimitCombo.SetItemData( iComboIndex , 3 );
			iComboIndex = m_viewlimitCombo.AddString( L"全体に公開" );
			m_viewlimitCombo.SetItemData( iComboIndex , 4 );
			m_viewlimitCombo.SetCurSel( 0 );
			m_viewlimitCombo.EnableWindow(TRUE);

			// フォーカス：タイトルから開始
			m_titleEdit.SetFocus();

			// 戻り先設定
			m_bFromMainView = true;
			break;

		default:
			MessageBox( L"アプリケーション内部エラー：未対応の書き込み画面種別です" );
			return;
		}
	}

	// コントロール状態の変更
	MyUpdateControlStatus();

	// 画像添付関連の初期化
	m_photo1_filepath = L"";
	m_photo2_filepath = L"";
	m_photo3_filepath = L"";

	// View 入れ替え
	theApp.ChangeView(theApp.m_pWriteView);
}

/**
 * 書き込みボタン押下時の処理
 *
 * 正常系処理シーケンス：
 *
 * <pre>
 *  StartEntryPost
 *    => StartConfirmPost()  [メッセージ以外]
 *      => DoPost/WM_MZ3_POST_CONFIRM_END
 *        => 通信開始
 *    => DoPost/WM_MZ3_POST_ENTRY_END [メッセージのみ]
 *      => 通信開始
 *  OnPostEntryEnd (メッセージのみ)
 *    => StartConfirmPost()
 *      => DoPost/WM_MZ3_POST_CONFIRM_END
 *        => 通信開始
 *  OnPostConfirmEnd
 *    => StartRegistPost()
 *      => DoPost/WM_MZ3_POST_END
 *        => 通信開始
 *  OnPostEnd
 * </pre>
 */
void CWriteView::OnBnClickedWriteSendButton()
{
	// MZ3 API : フック関数呼び出し
	util::MyLuaDataList rvals;
	if (util::CallMZ3ScriptHookFunctions2("click_write_view_send_button", &rvals, 
			util::MyLuaData(m_writeViewType),
			util::MyLuaData(m_data)
			)) {

		// キー押下イベントを奪うためにフォーカスを取得する
		SetFocus();
		return;
	}

	CString msg;
	GetDlgItemText( IDC_WRITE_BODY_EDIT, msg );

	CString title;
	GetDlgItemText( IDC_WRITE_TITLE_EDIT, title );

	if (msg.GetLength() > 3000) {
		// 最大文字を越えている
		::MessageBox(m_hWnd, L"送信文字数が多すぎます", MZ3_APP_NAME, MB_ICONERROR);
		return;
	}
	
	if (title.GetLength() == 0) {
		::MessageBox(m_hWnd, L"タイトルを入力してください", MZ3_APP_NAME, MB_ICONERROR);
		return;
	}
	
	if (msg.GetLength() == 0) {
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

	theApp.m_access = true;
	m_abort  = false;

	// コントロール状態の変更
	MyUpdateControlStatus();

	if (wcslen(theApp.m_loginMng.GetMixiOwnerID()) == 0) {
		MZ3LOGGER_INFO( L"OwnerIDが未取得なので、ログインし、取得する(1)" );

		// ログイン実行
		theApp.m_mixi4recv.SetAccessType(ACCESS_LOGIN);
		theApp.m_accessType = ACCESS_LOGIN;
		theApp.StartMixiLoginAccess(m_hWnd, &theApp.m_mixi4recv);
//		theApp.m_inet.Initialize( m_hWnd, &theApp.m_mixi4recv );
//		theApp.m_inet.DoGet(theApp.MakeLoginUrl(), L"", CInetAccess::FILE_HTML );
		return;
	}

	// POST 開始
	StartEntryPost();

	// キー押下イベントを奪うためにフォーカスを取得する
	SetFocus();
}

/**
 * 入力画面の取得(メッセージ以外の場合は確認画面への遷移)
 */
void CWriteView::StartEntryPost()
{
	CString url;

	switch (m_writeViewType) {
	case WRITEVIEW_TYPE_COMMENT:		// コメントの投稿確認
	case WRITEVIEW_TYPE_NEWDIARY:		// 日記の投稿確認

		// 確認画面への遷移
		StartConfirmPost();
		return;

	case WRITEVIEW_TYPE_REPLYMESSAGE:
		// メッセージの返信確認
		{
			// POST 電文の生成
			m_postData->ClearPostBody();

			// Content-Type を設定する
			// Content-Type: application/x-www-form-urlencoded
			m_postData->SetContentType( CONTENT_TYPE_FORM_URLENCODED );

			// リクエストURL生成
			// http://mixi.jp/view_message.pl?id=xxx&box=inbox
			// ↓
			// http://mixi.jp/reply_message.pl?reply_message_id=xxx&id=yyy
			int friend_id = m_data->GetOwnerID();
			CString reply_message_id = util::GetParamFromURL(m_data->GetURL(), L"id");
			url.Format(L"http://mixi.jp/reply_message.pl?reply_message_id=%s&id=%d",
				reply_message_id, friend_id);

			m_postData->SetConfirmUri(url);
		}
		break;

	case WRITEVIEW_TYPE_NEWMESSAGE:
		// 新規メッセージの送信確認
		{
			// POST 電文の生成
			m_postData->ClearPostBody();

			// Content-Type を設定する
			// Content-Type: application/x-www-form-urlencoded
			m_postData->SetContentType( CONTENT_TYPE_FORM_URLENCODED );


			// リクエストURL生成

			// m_data から id を取得する
			// http://mixi.jp/show_friend.pl?id=xxx
			int id = mixi::MixiUrlParser::GetID( m_data->GetURL() );
			if( id <= 0 ) {
				MessageBox( L"送信先ユーザの ID が不明です。メッセージを送信できません。" );

				theApp.m_access = false;
				MyUpdateControlStatus();
				return;
			}

			// URL 生成
			// http://mixi.jp/send_message.pl?id=xxx&mode=from_show_friend
			url = util::FormatString( L"http://mixi.jp/send_message.pl?id=%d&mode=from_show_friend", id );
			m_postData->SetConfirmUri(url);
		}

		break;

	default:
		{
			CString s;
			s.Format( L"未サポートの送信種別です [%d]", m_writeViewType );
			MessageBox( s );
			MZ3LOGGER_ERROR(s);

			// コントロール状態の復帰
			theApp.m_access = false;
			MyUpdateControlStatus();
			return;
		}
		break;
	}

	// 成功時のメッセージを設定する
	m_postData->SetSuccessMessage( WM_MZ3_POST_ENTRY_END );

	// アクセス種別の設定
	switch (m_writeViewType) {
	case WRITEVIEW_TYPE_REPLYMESSAGE:	theApp.m_accessType = ACCESS_POST_REPLYMESSAGE_ENTRY;	break;
	case WRITEVIEW_TYPE_NEWMESSAGE:		theApp.m_accessType = ACCESS_POST_NEWMESSAGE_ENTRY;		break;
	default:							theApp.m_accessType = ACCESS_MAIN;						break;
	}

	theApp.m_access = true;
	m_abort  = false;

	// コントロール状態の変更
	MyUpdateControlStatus();

	LPCTSTR refUrl = L"";

	// 通信開始
	theApp.m_inet.Initialize( m_hWnd, NULL, theApp.GetInetAccessEncodingByAccessType(theApp.m_accessType) );
	theApp.m_inet.DoPost(
		url, 
		refUrl, 
		CInetAccess::FILE_HTML, 
		m_postData);
}

/**
 * 確認画面への遷移
 */
void CWriteView::StartConfirmPost()
{
	CString wmsg;
	GetDlgItemText( IDC_WRITE_BODY_EDIT, wmsg );

	// ユーザが入力したメッセージを EUC-JP URL Encoded String に変換する
	CString euc_msg = URLEncoder::encode_euc(wmsg);
	TRACE(L"euc-jp url encoded string = [%s]\n", euc_msg);

	CString url;

	switch (m_writeViewType) {
	case WRITEVIEW_TYPE_COMMENT:
		// コメントの投稿確認
		{
			// 電文生成
			if( !mixi::CommentConfirmGenerator::generate( *m_postData, *m_data, euc_msg, 
													      m_photo1_filepath, m_photo2_filepath, m_photo3_filepath ) ) 
			{
				MessageBox( GENERATE_POSTMSG_FAILED_MESSAGE );

				theApp.m_access = false;
				MyUpdateControlStatus();
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
			if( !mixi::DiaryConfirmGenerator::generate( *m_postData, theApp.m_loginMng.GetMixiOwnerID(), euc_msg, 
													    m_photo1_filepath, m_photo2_filepath, m_photo3_filepath ) )
			{
				MessageBox( GENERATE_POSTMSG_FAILED_MESSAGE );

				theApp.m_access = false;
				MyUpdateControlStatus();
				return;
			}

			// リクエストURL生成
			url = L"http://mixi.jp/add_diary.pl";
		}
		break;

	case WRITEVIEW_TYPE_REPLYMESSAGE:
		// メッセージの返信確認
		{
			// タイトルを取得する
			CString title;
			GetDlgItemText( IDC_WRITE_TITLE_EDIT, title );

			// 電文生成
			int friend_id = m_data->GetOwnerID();
			CString reply_message_id = util::GetParamFromURL(m_data->GetURL(), L"id");
			if( !mixi::ReplyMessageConfirmGenerator::generate( *m_postData, title, euc_msg, friend_id, reply_message_id ) ) {
				MessageBox( GENERATE_POSTMSG_FAILED_MESSAGE );

				theApp.m_access = false;
				MyUpdateControlStatus();
				return;
			}

			// リクエストURL生成
			// http://mixi.jp/view_message.pl?id=xxx&box=inbox
			// ↓
			// http://mixi.jp/reply_message.pl?id=yyy&message_id=xxx
			//url.Format(L"http://mixi.jp/reply_message.pl?reply_message_id=%s&id=%d",
			//	(LPCTSTR)util::GetParamFromURL(m_data->GetURL(), L"id"),
			//	m_data->GetOwnerID());
			url = L"http://mixi.jp/reply_message.pl";

			m_postData->SetConfirmUri(url);
		}
		break;

	case WRITEVIEW_TYPE_NEWMESSAGE:
		// 新規メッセージの送信確認
		{
			// タイトルを取得する
			CString title;
			GetDlgItemText( IDC_WRITE_TITLE_EDIT, title );

			// 電文生成
			int friend_id = m_data->GetOwnerID();
			if( friend_id <= 0 ) {
				friend_id = mixi::MixiUrlParser::GetID( m_data->GetURL() );
				if( friend_id <= 0 ) {
					MessageBox( L"送信先ユーザの ID が不明です。メッセージを送信できません。" );

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

			// URL 生成
			url = L"http://mixi.jp/send_message.pl";
			m_postData->SetConfirmUri(url);
		}

		break;

	default:
		{
			CString s;
			s.Format( L"未サポートの送信種別です [%d]", m_writeViewType );
			MessageBox( s );
			MZ3LOGGER_ERROR(s);

			// コントロール状態の復帰
			theApp.m_access = false;
			MyUpdateControlStatus();
			return;
		}
		break;
	}

	// 成功時のメッセージを設定する
	m_postData->SetSuccessMessage( WM_MZ3_POST_CONFIRM_END );

	// アクセス種別の設定
	switch (m_writeViewType) {
	case WRITEVIEW_TYPE_COMMENT:		theApp.m_accessType = ACCESS_POST_COMMENT_CONFIRM;		break;
	case WRITEVIEW_TYPE_NEWDIARY:		theApp.m_accessType = ACCESS_POST_NEWDIARY_CONFIRM;		break;
	case WRITEVIEW_TYPE_REPLYMESSAGE:	theApp.m_accessType = ACCESS_POST_REPLYMESSAGE_CONFIRM;	break;
	case WRITEVIEW_TYPE_NEWMESSAGE:		theApp.m_accessType = ACCESS_POST_NEWMESSAGE_CONFIRM;	break;
	default:							theApp.m_accessType = ACCESS_MAIN;						break;
	}

	theApp.m_access = true;
	m_abort  = false;

	// コントロール状態の変更
	MyUpdateControlStatus();

	// リファラ設定
	CString refUrl = L"";
	
	//リファラ設定したら投稿で止まるようになったので保留
	//LPCTSTR refUrl = m_data->GetURL();

	// 通信開始
	theApp.m_inet.Initialize( m_hWnd, NULL, theApp.GetInetAccessEncodingByAccessType(theApp.m_accessType) );
	theApp.m_inet.DoPost(
		url, 
		refUrl, 
		CInetAccess::FILE_HTML, 
		m_postData );
}

/**
* アクセスエラー通知受信
*/
LRESULT CWriteView::OnGetError(WPARAM wParam, LPARAM lParam)
{
	// 通信エラーが発生した場合の処理
	LPCTSTR smsg = L"エラーが発生しました";
	util::MySetInformationText( m_hWnd, smsg );

	CString msg;
	msg.Format( 
		L"%s\n\n"
		L"原因：%s", smsg, theApp.m_inet.GetErrorMessage() );
	::MessageBox(m_hWnd, msg, MZ3_APP_NAME, MB_ICONSTOP | MB_OK);
	MZ3LOGGER_ERROR( msg );

	theApp.m_access = false;

	// コントロール状態の変更
	MyUpdateControlStatus();

	return TRUE;
}

/**
 * 中止ボタン押下時の処理
 */
void CWriteView::OnBnClickedWriteCancelButton()
{
	CString msg;
	GetDlgItem(IDC_WRITE_BODY_EDIT)->GetWindowText(msg);

	if (msg.GetLength() > 0) {
		if (MessageBox(L"未投稿のデータがあります\n破棄されますがよろしいですか？", 
			MZ3_APP_NAME,
			MB_ICONQUESTION | MB_OKCANCEL)==IDCANCEL)
		{
			// 中止処理のキャンセル(編集に戻る)
			return;
		}
	}

	// 内容をクリア
	GetDlgItem(IDC_WRITE_TITLE_EDIT)->SetWindowText(L"");
	GetDlgItem(IDC_WRITE_BODY_EDIT)->SetWindowText(L"");

	// コントロール状態の変更
	MyUpdateControlStatus();

	// 前の画面に戻る
	if (IsWriteFromMainView()) {
		// メインビューに戻るため、戻るボタン無効化
		theApp.EnableCommandBarButton(ID_BACK_BUTTON, FALSE);
		::SendMessage(theApp.m_pMainView->m_hWnd, WM_MZ3_CHANGE_VIEW, NULL, NULL);
	} else {
		// レポートビューに戻るため、戻るボタン有効化
		theApp.EnableCommandBarButton(ID_BACK_BUTTON, TRUE);
		::SendMessage(theApp.m_pReportView->m_hWnd, WM_MZ3_CHANGE_VIEW, NULL, NULL);
	}

	m_bWriteCompleted = true;
}

/**
 * 入力画面の取得完了。確認画面ボタン押下のシミュレート。
 */
LRESULT CWriteView::OnPostEntryEnd(WPARAM wParam, LPARAM lParam)
{
	if (m_abort) {
		::SendMessage(m_hWnd, WM_MZ3_POST_ABORT, NULL, NULL);
		return TRUE;
	}

	// ログアウトチェック
	if (theApp.IsMixiLogout(theApp.m_accessType)) {
		// ログアウト状態になっている
		// ログイン処理実施
		MZ3LOGGER_INFO(_T("ログインします。"));
		
		// ログイン実行
		theApp.m_mixi4recv.SetAccessType(ACCESS_LOGIN);
		theApp.m_accessType = ACCESS_LOGIN;
		theApp.StartMixiLoginAccess(m_hWnd, &theApp.m_mixi4recv);
//		theApp.m_inet.Initialize( m_hWnd, &theApp.m_mixi4recv );
//		theApp.m_inet.DoGet(theApp.MakeLoginUrl(), NULL, CInetAccess::FILE_HTML );
		return TRUE;
	}

	// 確認画面判定
	CHtmlArray html;
	html.Load( theApp.m_filepath.temphtml );

	// 解析用に入力画面を保存しておく
	CString strEntryTempFile = theApp.m_filepath.temphtml + L".entry.html";
	CopyFile( theApp.m_filepath.temphtml, strEntryTempFile, FALSE );

	// 確認画面の解析
	if (html.GetPostConfirmData(m_postData) == false) {
		CString msg = 
			L"エラーが発生しました\r\n"
			L"確認画面にエラー内容が表示されていますが、"
			L"本バージョンの" MZ3_APP_NAME L"ではエラー内容を表示できません・・・\r\n"
			L"お手数ですが、下記ファイルを直接解析してくださいm(_ _)m\r\n"
			+ strEntryTempFile;
		MessageBox( msg );
		MZ3LOGGER_ERROR( msg );

		MZ3LOGGER_ERROR( L"書き込みに失敗したため、念のためOwnerIDを初期化します" );
		theApp.m_loginMng.SetMixiOwnerID( L"" );

		theApp.m_access = false;
		MyUpdateControlStatus();

		return 0;
	}

	// 確認画面取得開始
	StartConfirmPost();

	return TRUE;
}

/**
 * 確認画面の表示完了。書き込みのシミュレート。
 */
LRESULT CWriteView::OnPostConfirmEnd(WPARAM wParam, LPARAM lParam)
{
	if (m_abort) {
		::SendMessage(m_hWnd, WM_MZ3_POST_ABORT, NULL, NULL);
		return TRUE;
	}

	// ログアウトチェック
	if (theApp.IsMixiLogout(theApp.m_accessType)) {
		// ログアウト状態になっている
		// ログイン処理実施
		MZ3LOGGER_INFO(_T("ログインします。"));

		// ログイン実行
		theApp.m_mixi4recv.SetAccessType(ACCESS_LOGIN);
		theApp.m_accessType = ACCESS_LOGIN;
		theApp.StartMixiLoginAccess(m_hWnd, &theApp.m_mixi4recv);
//		theApp.m_inet.Initialize( m_hWnd, &theApp.m_mixi4recv );
//		theApp.m_inet.DoGet(theApp.MakeLoginUrl(), NULL, CInetAccess::FILE_HTML );
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
		theApp.m_loginMng.SetMixiOwnerID( L"" );

		theApp.m_access = false;
		MyUpdateControlStatus();

		return 0;
	}

	// 書き込み開始
	StartRegistPost();

	return TRUE;
}

/**
 * 確認画面｜書き込みボタン押下時の電文送信
 */
void CWriteView::StartRegistPost() 
{
	// 中断確認
	if (m_abort) {
		::SendMessage(m_hWnd, WM_MZ3_POST_ABORT, NULL, NULL);
		return;
	}
	m_abort = false;

	CString url;	// URL
	CString refUrl;	// リファラー用URL

	switch (m_writeViewType) {
	case WRITEVIEW_TYPE_COMMENT:
		{
			// 電文の生成
			if( !mixi::CommentRegistGenerator::generate( *m_postData, *m_data ) ) {
				MessageBox( GENERATE_POSTMSG_FAILED_MESSAGE );

				// コントロール状態の復帰
				theApp.m_access = false;
				MyUpdateControlStatus();
				return;
			}

			// URL の生成
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

			// 公開範囲を取得する
			CString viewlimit = L"0";
			int iIndex = m_viewlimitCombo.GetCurSel() ;
			if( iIndex >= 0 ) {
				viewlimit.Format( L"%d" , m_viewlimitCombo.GetItemData( iIndex ) ); 
#ifdef DEBUG
				wprintf( L"viewlimit = %s\n" , viewlimit);
#endif
			}

			// 電文の生成
			if( !mixi::DiaryRegistGenerator::generate( *m_postData, title, viewlimit ) ) {
				MessageBox( GENERATE_POSTMSG_FAILED_MESSAGE );

				// コントロール状態の復帰
				theApp.m_access = false;
				MyUpdateControlStatus();
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
			int friend_id = m_data->GetOwnerID();
			CString reply_message_id = util::GetParamFromURL(m_data->GetURL(), L"id");
			if( !mixi::ReplyMessageRegistGenerator::generate( *m_postData, title, friend_id, reply_message_id ) ) {
				MessageBox( GENERATE_POSTMSG_FAILED_MESSAGE );

				// コントロール状態の復帰
				theApp.m_access = false;
				MyUpdateControlStatus();
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
			int friend_id = m_data->GetOwnerID();
			if( friend_id <= 0 ) {
				friend_id = mixi::MixiUrlParser::GetID( m_data->GetURL() );
			}
			if( !mixi::NewMessageRegistGenerator::generate( *m_postData, title, friend_id ) ) {
				MessageBox( GENERATE_POSTMSG_FAILED_MESSAGE );

				// コントロール状態の復帰
				theApp.m_access = false;
				MyUpdateControlStatus();
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
			MZ3LOGGER_ERROR(s);

			// コントロール状態の復帰
			theApp.m_access = false;
			MyUpdateControlStatus();
			return;
		}
		break;
	}

	// 成功時のメッセージを設定する
	m_postData->SetSuccessMessage( WM_MZ3_POST_END );

	// アクセス種別の設定
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
 * 書き込み完了（確認画面｜書き込みボタン押下⇒成功）
 */
LRESULT CWriteView::OnPostEnd(WPARAM wParam, LPARAM lParam)
{
	CHtmlArray html;
	html.Load( theApp.m_filepath.temphtml );

	theApp.m_access = false;
	m_abort  = false;

	// コントロール状態の変更
	MyUpdateControlStatus();

	// MZ3 API : フック関数呼び出し
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

	// 投稿完了チェック
	if (html.IsPostSucceeded(m_writeViewType) != FALSE) {
#ifdef	MZ3_DEBUG
		// only for Release_MZ3 configuration
		DumpToTemporaryDraftFile();
#endif
		if (m_abort == false) {
			switch (m_writeViewType ) {
			case WRITEVIEW_TYPE_REPLYMESSAGE:
			case WRITEVIEW_TYPE_NEWMESSAGE:
				::MessageBox(m_hWnd, L"メッセージを送信しました", MZ3_APP_NAME, MB_OK);
				break;
			default:
				::MessageBox(m_hWnd, L"投稿しました", MZ3_APP_NAME, MB_OK);
				break;
			}
		} else {
			// 中断が押されていた場合は上に返す
			return TRUE;
		}

		GetDlgItem(IDC_WRITE_TITLE_EDIT)->SetWindowText(L"");
		GetDlgItem(IDC_WRITE_BODY_EDIT)->SetWindowText(L"");

		m_bWriteCompleted = true;

		// 前の画面に戻る
		if (IsWriteFromMainView()) {
			// メインビューに戻るため、戻るボタン無効化
			theApp.EnableCommandBarButton(ID_BACK_BUTTON, FALSE);
			::SendMessage(theApp.m_pMainView->m_hWnd, WM_MZ3_CHANGE_VIEW, NULL, NULL);
		} else {
			// レポートビューに戻るため、戻るボタン有効化
			theApp.EnableCommandBarButton(ID_BACK_BUTTON, TRUE);
			::SendMessage(theApp.m_pReportView->m_hWnd, WM_MZ3_CHANGE_VIEW, NULL, NULL);
		}

		if (!IsWriteFromMainView()) {
			// レポートビューに戻った後のリロード処理
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
	} else {
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
	}

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

/**
 * 中断
 */
LRESULT CWriteView::OnPostAbort(WPARAM wParam, LPARAM lParam)
{
	theApp.m_access = false;

	// コントロール状態の変更
	MyUpdateControlStatus();

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
	m_abort = true;

	LPCTSTR msg = L"";
	if (m_postData->GetSuccessMessage() == WM_MZ3_POST_CONFIRM_END) {
		msg = L"中断しましたが、投稿された可能性があります";
	} else {
		msg = L"中断しました";
	}
	util::MySetInformationText( m_hWnd, msg );
	::MessageBox(m_hWnd, msg, MZ3_APP_NAME, MB_ICONSTOP | MB_OK);

	// 中断後に書き込みボタンが押されると何故か強制終了してしまうため、
	// 本文領域にフォーカスを戻す。
	m_bodyEdit.SetFocus();

	theApp.m_access = false;

	// コントロール状態の変更
	MyUpdateControlStatus();

	return TRUE;
}

/**
 * アクセス情報通知
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
			// ヘルプ表示
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
				// アクセス中は中断処理
				::SendMessage(m_hWnd, WM_MZ3_ABORT, NULL, NULL);
				return TRUE;
			}
			break;
		}

	}

	return CFormView::PreTranslateMessage(pMsg);
}

/**
 * 前の画面に切り替える
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
 * GET メソッド完了時の処理
 */
LRESULT CWriteView::OnGetEnd(WPARAM wParam, LPARAM lParam)
{
	util::MySetInformationText( m_hWnd, L"解析中" );

	CHtmlArray html;
	html.Load( theApp.m_filepath.temphtml );

	if (m_abort) {
		::SendMessage(m_hWnd, WM_MZ3_POST_ABORT, NULL, lParam);
		return TRUE;
	}

	theApp.m_access = false;
	m_abort  = false;

	// コントロール状態の変更
	MyUpdateControlStatus();

	// MZ3 API : フック関数呼び出し
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
			// ログインしたかどうかの確認
			if( mixi::HomeParser::IsLoginSucceeded(html) ) {
				// ログイン成功
				if (wcslen(theApp.m_loginMng.GetMixiOwnerID()) != 0) {
					MZ3LOGGER_DEBUG( L"OwnerID 取得済み" );
				} else {
					MZ3LOGGER_INFO( L"OwnerIDが未取得なので、ログインし、取得する (2)" );

					((CMixiData*)lParam)->SetAccessType(ACCESS_MAIN);
					theApp.m_accessType = ACCESS_MAIN;
					theApp.m_inet.DoGet(L"http://mixi.jp/check.pl?n=%2Fhome.pl", L"", CInetAccess::FILE_HTML, NULL );
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

			// パース
			MZ3Data data;
			theApp.DoParseMixiHomeHtml(&data, &html);

			if (wcslen(theApp.m_loginMng.GetMixiOwnerID()) == 0) {
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

		// POST 処理を続行
		StartEntryPost();
	}

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
	// MZ3 API : フック関数呼び出し
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
	MZ3LOGGER_INFO(L"Lua 側で処理がないので「添付不可」とみなす");

	return false;
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

/**
 * ポップアップメニュー
 */
void CWriteView::PopupWriteBodyMenu(void)
{
	POINT pt    = util::GetPopupPosForSoftKeyMenu2();
	int   flags = util::GetPopupFlagsForSoftKeyMenu2();

	CImageList* pGlobalImageList = &theApp.m_imageCache.GetImageList16();

	CWMMenu menu( pGlobalImageList );
	menu.LoadMenu( IDR_WRITE_MENU );
	CMenu* pcThisMenu = menu.GetSubMenu(0);

	// 写真添付ができない画面では「写真を添付する」メニューを消す
	if( !IsEnableAttachImageMode() ) {
		pcThisMenu->EnableMenuItem( ID_ATTACH_PHOTO, MF_GRAYED | MF_BYCOMMAND );
	}

	// 絵文字メニュー
	// mixi のみサポート
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
						// リサイズして画像キャッシュに追加する。
						imageIndex = theApp.AddImageToImageCache(this, image, path);
					}
				}
//				MZ3LOGGER_DEBUG(util::FormatString(L" %4d:%d (%s)", i, imageIndex, path));
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

		// 追加メニュー
		util::MyLuaDataList rvals;
		if (util::CallMZ3ScriptHookFunctions2("popup_write_menu", &rvals, 
				util::MyLuaData(theApp.m_accessTypeInfo.getSerializeKey(m_writeViewType)),
				util::MyLuaData(m_data),
				util::MyLuaData(pcThisMenu)
				))
		{
		}

		// セパレータ、「前の画面へ」
		pcThisMenu->AppendMenu(MF_SEPARATOR);
		pcThisMenu->AppendMenu(MF_STRING, ID_WRITE_BACK_MENU, L"前の画面へ");

		// メニュー表示(コンテキストの都合上、ここで呼び出す)
		pcThisMenu->TrackPopupMenu( flags, pt.x, pt.y, this );
	} else {
		// mixi 以外では非サポート
		pcThisMenu->DeleteMenu( 1, MF_BYPOSITION );

		// 追加メニュー
		util::MyLuaDataList rvals;
		if (util::CallMZ3ScriptHookFunctions2("popup_write_menu", &rvals, 
				util::MyLuaData(theApp.m_accessTypeInfo.getSerializeKey(m_writeViewType)),
				util::MyLuaData(m_data),
				util::MyLuaData(pcThisMenu)
				))
		{
		}

		// セパレータ、「前の画面へ」
		pcThisMenu->AppendMenu(MF_SEPARATOR);
		pcThisMenu->AppendMenu(MF_STRING, ID_WRITE_BACK_MENU, L"前の画面へ");

		// メニュー表示
		pcThisMenu->TrackPopupMenu( flags, pt.x, pt.y, this );
	}
}

/**
 * 状態に応じたコントロール状態の変更
 */
void CWriteView::MyUpdateControlStatus(void)
{
	// 本ビューでは、進むボタン、ブラウザで開くボタン、書き込みボタンは常に無効
	theApp.EnableCommandBarButton(ID_FORWARD_BUTTON, FALSE);
	theApp.EnableCommandBarButton(ID_OPEN_BROWSER,   FALSE);
	theApp.EnableCommandBarButton(ID_WRITE_BUTTON,   FALSE);

	// イメージボタンは画像投稿可能な状態にのみ有効
	if (theApp.m_access) {
		theApp.EnableCommandBarButton(ID_IMAGE_BUTTON, FALSE);
	} else {
		theApp.EnableCommandBarButton(ID_IMAGE_BUTTON, IsEnableAttachImageMode() ? TRUE : FALSE);
	}

	// 中止ボタン：アクセス中は有効
	theApp.EnableCommandBarButton(ID_STOP_BUTTON, theApp.m_access ? TRUE : FALSE);

	// 戻るボタン：アクセス中は無効
	theApp.EnableCommandBarButton(ID_BACK_BUTTON, theApp.m_access ? FALSE : TRUE);

	// 送信ボタン：アクセス中は無効
	m_sendButton.EnableWindow(theApp.m_access ? FALSE : TRUE);

	// キャンセルボタン：アクセス中は無効
	m_cancelButton.EnableWindow(theApp.m_access ? FALSE : TRUE);

	// 情報領域：アクセス中は表示
	m_infoEdit.ShowWindow(theApp.m_access ? SW_SHOW : SW_HIDE);
}

/**
 * MZ3 API で登録されたメニューのイベント
 */
void CWriteView::OnLuaMenu(UINT nID)
{
	UINT idx = nID - ID_LUA_MENU_BASE;
	if (idx >= theApp.m_luaMenus.size()) {
		MZ3LOGGER_ERROR(util::FormatString(L"不正なメニューIDです [%d]", nID));
		return;
	}

	// Lua関数名取得＆呼び出し
	const std::string& strFuncName = theApp.m_luaMenus[idx];
	util::CallMZ3ScriptHookFunction("", "select_menu", strFuncName.c_str(), &m_data);
}

#endif	// BT_MZ3
