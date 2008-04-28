/*
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */
#pragma once

#include "PostData.h"
#include "MixiData.h"

namespace mixi {

/// PostData を生成するクラス群の基底クラス
class PostDataGeneratorBase {
protected:

	/**
	 * 画像データを埋め込む
	 */
	static bool embedPhotoData( CPostData& post, const char* form_name, LPCTSTR photo_filepath ) {

		post.AppendPostBodyWithCRLF( "-----------------------------7d62ee108071e" );

		if( photo_filepath != NULL && wcslen(photo_filepath) > 0 ) {
			// ファイルから取得して埋め込む

			// Content-Disposition
			post.AppendPostBody( "Content-Disposition: form-data; name=\"" );
			post.AppendPostBody( form_name );
			post.AppendPostBody( "\"; " );
			post.AppendPostBody( "filename=\"photo.jpg\"" );
			post.AppendPostBodyWithCRLF( "" );

			// Content-Type
			post.AppendPostBodyWithCRLF( "Content-Type: image/jpeg" );

			// 空行
			post.AppendPostBodyWithCRLF( "" );

			// ファイルを読み込み、埋め込む
			if( !appendFile( post, photo_filepath ) ) {
				return false;
			}

			// 空行
			post.AppendPostBodyWithCRLF( "" );
		}else{
			// ファイル未指定なので埋め込まない
			// 空の電文を埋め込む

			// Content-Disposition
			post.AppendPostBody( "Content-Disposition: form-data; name=\"" );
			post.AppendPostBody( form_name );
			post.AppendPostBody( "\"; " );
			post.AppendPostBody( "filename=\"\"" );
			post.AppendPostBodyWithCRLF( "" );

			// Content-Type
			post.AppendPostBodyWithCRLF( "Content-Type: application/octet-stream" );

			// 空行
			post.AppendPostBodyWithCRLF( "" );
		}

		return true;
	}

	/**
	 * ファイルを読み込み、埋め込む
	 */
	static bool appendFile( CPostData& post, LPCTSTR photo_filepath ) {

		// ファイルを読み込み、そのまま埋め込む
		FILE* fp = _wfopen( photo_filepath, L"rb" );
		if( fp == NULL ) {
			return false;
		}

#define	BUF_SIZE 1024
		char buf[ BUF_SIZE ];
		while( !feof( fp ) ) {
			int len = fread( buf, 1, BUF_SIZE, fp );
			if( ferror( fp ) ) {
				// 読み込みエラー
				fclose( fp );
				return false;
			}

			post.AppendPostBody( buf, len );
		}

		fclose( fp );
		return true;
	}
};

/**
 * コメント投稿の確認画面用 POST 電文生成クラス（書き込み画面⇒確認画面ボタン）
 * 
 * add_bbs_comment.pl
 */
class PostCommentGenerator : public PostDataGeneratorBase {
public:

	/**
	 * CPostData の生成
	 *
	 * @param post				CPostData オブジェクト
	 * @param mixi				CMixiData オブジェクト
	 * @param msg				ユーザが入力したメッセージ
	 * @param photo1_filepath	photo1 のファイルパス
	 * @param photo2_filepath	photo2 のファイルパス
	 * @param photo3_filepath	photo3 のファイルパス
	 */
	static bool generate( CPostData& post, const CMixiData& mixi, 
						  LPCTSTR msg,
						  LPCTSTR photo1_filepath = NULL, 
						  LPCTSTR photo2_filepath = NULL, 
						  LPCTSTR photo3_filepath = NULL )
	{
		// 次画面で hidden タグとして埋め込むメッセージを保存しておく
		post.SetComment(msg);

		// POST 電文の生成
		post.ClearPostBody();

		if (mixi.GetContentType() == CONTENT_TYPE_MULTIPART) {
			// マルチパートの時は下記電文を送信する
			post.AppendPostBodyWithCRLF( "-----------------------------7d62ee108071e" );
			post.AppendPostBodyWithCRLF( "Content-Disposition: form-data; name=\"comment\"" );
			post.AppendPostBodyWithCRLF( "" );
			post.AppendPostBodyWithCRLF( "msg" );

			if( !embedPhotoData( post, "photo1", photo1_filepath ) ) return false;
			if( !embedPhotoData( post, "photo2", photo2_filepath ) ) return false;
			if( !embedPhotoData( post, "photo3", photo3_filepath ) ) return false;

			post.AppendPostBodyWithCRLF( "-----------------------------7d62ee108071e--" );
		}
		else {
			// urlencodeの場合は、エンコードされたメッセージをそのまま設定
			CString body;
			switch (mixi.GetAccessType()) {
			case ACCESS_DIARY:
			case ACCESS_MYDIARY:
				body.Format(L"owner_id=%d&comment_body=%s", mixi.GetOwnerID(), msg);
				break;
			case ACCESS_ENQUETE:
				body.Format(L"submit=main&comment=%s", msg);
				break;
			}

			post.AppendPostBodyWithCRLF( body );
		}

		// Content-Type を設定する
		post.SetContentType( mixi.GetContentType() );

		return true;
	}
};

/**
 * コメント投稿の書き込み画面用 POST 電文生成クラス（確認画面｜書き込みボタン押下）
 * 
 * add_bbs_comment.pl
 */
class EntryCommentGenerator : public PostDataGeneratorBase {
public:

	/**
	 * CPostData の生成
	 *
	 * @param post				CPostData オブジェクト
	 * @param mixi				CMixiData オブジェクト
	 */
	static bool generate( CPostData& post, const CMixiData& mixi )
	{
		// POST 電文の生成
		post.ClearPostBody();

		switch( mixi.GetAccessType() ) {
		case ACCESS_BBS:
		case ACCESS_EVENT:
			post.AppendPostBody( "submit=confirm&packed=" );
			post.AppendPostBody( post.GetPacked() );
			post.AppendPostBody( "&comment=" );
			post.AppendPostBody( post.GetComment() );
			post.AppendPostBody( "&post_key=" );
			post.AppendPostBody( post.GetPostKey() );
			break;

		case ACCESS_ENQUETE:
			post.AppendPostBody( "submit=confirm&comment=" );
			post.AppendPostBody( post.GetComment() );
			post.AppendPostBody( "&post_key=" );
			post.AppendPostBody( post.GetPostKey() );
			break;

		case ACCESS_DIARY:
		case ACCESS_MYDIARY:
			post.AppendPostBody( "submit=confirm&comment_body=" );
			post.AppendPostBody( post.GetComment() );

      //2007/06/22 いっちゅう mixi仕様変更対応　owner_idが要らなくなったっぽい
			//post.AppendPostBody( "&owner_id=" );
			//post.AppendPostBody( util::int2str_a( mixi.GetOwnerID() ) );
      
			post.AppendPostBody( "&post_key=" );
			post.AppendPostBody( post.GetPostKey() );
			break;
		}

		// Content-Type を設定する
		// Content-Type: application/x-www-form-urlencoded
		post.SetContentType( CONTENT_TYPE_FORM_URLENCODED );

		return true;
	}
};

/**
 * 日記投稿の確認画面用 POST 電文生成クラス（書き込み画面⇒確認画面ボタン）
 * 
 * add_diary.pl
 */
class PostDiaryGenerator : public PostDataGeneratorBase {
public:

	/**
	 * CPostData の生成
	 *
	 * @param post				CPostData オブジェクト
	 * @param ownerId			ユーザID
	 * @param msg				ユーザが入力したメッセージ
	 * @param photo1_filepath	photo1 のファイルパス
	 * @param photo2_filepath	photo2 のファイルパス
	 * @param photo3_filepath	photo3 のファイルパス
	 */
	static bool generate( CPostData& post, LPCTSTR ownerId, 
						  LPCTSTR msg,
						  LPCTSTR photo1_filepath = NULL, 
						  LPCTSTR photo2_filepath = NULL, 
						  LPCTSTR photo3_filepath = NULL )
	{
		// 次画面で hidden タグとして埋め込むメッセージを保存しておく
		post.SetComment(msg);

		// POST 電文の生成
		post.ClearPostBody();
		post.AppendPostBodyWithCRLF( "-----------------------------7d62ee108071e" );
		post.AppendPostBodyWithCRLF( "Content-Disposition: form-data; name=\"news_id\"" );
		post.AppendPostBodyWithCRLF( "" );
		post.AppendPostBodyWithCRLF( "-----------------------------7d62ee108071e" );
		post.AppendPostBodyWithCRLF( "Content-Disposition: form-data; name=\"id\"" );
		post.AppendPostBodyWithCRLF( "" );
		post.AppendPostBodyWithCRLF( ownerId ); // ユーザーＩＤ
		post.AppendPostBodyWithCRLF( "-----------------------------7d62ee108071e" );
		post.AppendPostBodyWithCRLF( "Content-Disposition: form-data; name=\"news_title\"" );
		post.AppendPostBodyWithCRLF( "" );
		post.AppendPostBodyWithCRLF( "-----------------------------7d62ee108071e" );
		post.AppendPostBodyWithCRLF( "Content-Disposition: form-data; name=\"news_url\"" );
		post.AppendPostBodyWithCRLF( "" );
		post.AppendPostBodyWithCRLF( "-----------------------------7d62ee108071e" );
		post.AppendPostBodyWithCRLF( "Content-Disposition: form-data; name=\"movie_id\"" );
		post.AppendPostBodyWithCRLF( "" );
		post.AppendPostBodyWithCRLF( "-----------------------------7d62ee108071e" );
		post.AppendPostBodyWithCRLF( "Content-Disposition: form-data; name=\"movie_title\"" );
		post.AppendPostBodyWithCRLF( "" );
		post.AppendPostBodyWithCRLF( "-----------------------------7d62ee108071e" );
		post.AppendPostBodyWithCRLF( "Content-Disposition: form-data; name=\"movie_url\"" );
		post.AppendPostBodyWithCRLF( "" );
		post.AppendPostBodyWithCRLF( "-----------------------------7d62ee108071e" );
		post.AppendPostBodyWithCRLF( "Content-Disposition: form-data; name=\"submit\"" );
		post.AppendPostBodyWithCRLF( "" );
		post.AppendPostBodyWithCRLF( "main" );
		post.AppendPostBodyWithCRLF( "-----------------------------7d62ee108071e" );
		post.AppendPostBodyWithCRLF( "Content-Disposition: form-data; name=\"diary_title\"" );
		post.AppendPostBodyWithCRLF( "" );
		post.AppendPostBodyWithCRLF( "title" );
		post.AppendPostBodyWithCRLF( "-----------------------------7d62ee108071e" );
		post.AppendPostBodyWithCRLF( "Content-Disposition: form-data; name=\"diary_body\"" );
		post.AppendPostBodyWithCRLF( "" );
		post.AppendPostBodyWithCRLF( "msg" );

		if( !embedPhotoData( post, "photo1", photo1_filepath ) ) return false;
		if( !embedPhotoData( post, "photo2", photo2_filepath ) ) return false;
		if( !embedPhotoData( post, "photo3", photo3_filepath ) ) return false;

		post.AppendPostBodyWithCRLF( "-----------------------------7d62ee108071e--" );

		// Content-Type を設定する
		// Content-Type: multipart/form-data
		post.SetContentType( CONTENT_TYPE_MULTIPART );

		return true;
	}
};

/**
 * 日記投稿の書き込み画面用 POST 電文生成クラス（確認画面｜書き込みボタン押下）
 * 
 * add_diary.pl
 */
class EntryDiaryGenerator : public PostDataGeneratorBase {
public:

	/**
	 * CPostData の生成
	 *
	 * @param post				CPostData オブジェクト
	 * @param title				タイトル文字列(wchar_t)
	 */
	static bool generate( CPostData& post, LPCTSTR title , LPCTSTR viewlimit)
	{
		// POST 電文の生成
		post.ClearPostBody();
		post.AppendPostBody( "submit=confirm&packed=" );
		post.AppendPostBody( post.GetPacked() );
		post.AppendPostBody( "&post_key=" );
		post.AppendPostBody( post.GetPostKey() );
		post.AppendPostBody( "&news_id=&id=" );
		post.AppendPostBody( theApp.m_loginMng.GetOwnerID() );
		post.AppendPostBody( "&diary_title=" );

		// タイトルを EUC-JP URL Encoded String に変換する
		post.AppendPostBody( URLEncoder::encode_euc(title) );
		post.AppendPostBody( "&diary_body=" );
		post.AppendPostBody( post.GetComment() );
		post.AppendPostBody( "&news_title=&news_url=&movie_id=&movie_title=&movie_url=" );
		post.AppendPostBody( "&tag_id=" );
		post.AppendPostBody( viewlimit );

		// Content-Type: application/x-www-form-urlencoded
		post.SetContentType( CONTENT_TYPE_FORM_URLENCODED );

		return true;
	}
};

/**
 * メッセージ返信の確認画面用 POST 電文生成クラス（書き込み画面⇒確認画面ボタン）
 * 
 * reply_message.pl
 */
class PostReplyMessageGenerator : public PostDataGeneratorBase {
public:

	/**
	 * CPostData の生成
	 *
	 * @param post				CPostData オブジェクト
	 * @param msg				ユーザが入力したメッセージ
	 */
	static bool generate( CPostData& post,
						  LPCTSTR msg )
	{
		// 次画面で hidden タグとして埋め込むメッセージを保存しておく
		post.SetComment(msg);

		// POST 電文の生成
		post.ClearPostBody();
		post.AppendPostBodyWithCRLF( "-----------------------------7d62ee108071e" );
		post.AppendPostBodyWithCRLF( "Content-Disposition: form-data; name=\"submit\"" );
		post.AppendPostBodyWithCRLF( "" );
		post.AppendPostBodyWithCRLF( "main" );
		post.AppendPostBodyWithCRLF( "-----------------------------7d62ee108071e" );
		post.AppendPostBodyWithCRLF( "Content-Disposition: form-data; name=\"post_key\"" );
		post.AppendPostBodyWithCRLF( "" );
		post.AppendPostBodyWithCRLF( "-----------------------------7d62ee108071e" );
		post.AppendPostBodyWithCRLF( "Content-Disposition: form-data; name=\"subject\"" );
		post.AppendPostBodyWithCRLF( "" );
		post.AppendPostBodyWithCRLF( "title" );
		post.AppendPostBodyWithCRLF( "-----------------------------7d62ee108071e" );
		post.AppendPostBodyWithCRLF( "Content-Disposition: form-data; name=\"body\"" );
		post.AppendPostBodyWithCRLF( "" );
		post.AppendPostBodyWithCRLF( "msg" );
		post.AppendPostBodyWithCRLF( "-----------------------------7d62ee108071e" );
		post.AppendPostBodyWithCRLF( "Content-Disposition: form-data; name=\"post_key\"" );
		post.AppendPostBodyWithCRLF( "" );
		post.AppendPostBodyWithCRLF( "4a960f4bbeb6f5cb7b28c8f82a55d0a5" );
		post.AppendPostBodyWithCRLF( "-----------------------------7d62ee108071e--" );

		// Content-Type を設定する
		// Content-Type: application/x-www-form-urlencoded
		post.SetContentType( CONTENT_TYPE_FORM_URLENCODED );

		return true;
	}
};

/**
 * メッセージ返信の書き込み画面用 POST 電文生成クラス（確認画面｜書き込みボタン押下）
 * 
 * reply_message.pl
 */
class EntryReplyMessageGenerator : public PostDataGeneratorBase {
public:

	/**
	 * CPostData の生成
	 *
	 * @param post				CPostData オブジェクト
	 * @param title				タイトル文字列(wchar_t)
	 */
	static bool generate( CPostData& post, LPCTSTR title )
	{
		CString msgId;
		msgId = post.GetConfirmUri();
		msgId = msgId.Mid(msgId.Find(L"message_id=") + wcslen(L"message_id="));

		// POST 電文の生成
		post.ClearPostBody();

		post.AppendPostBody( "submit=confirm&message_id=" );
		post.AppendPostBody( msgId );

		// 本文を埋め込む
		post.AppendPostBody( "&body=" );
		post.AppendPostBody( post.GetComment() );
		post.AppendPostBody( "&subject=" );

		// タイトルを EUC-JP URL Encoded String に変換して埋め込む
		post.AppendPostBody( URLEncoder::encode_euc( title ) );

		post.AppendPostBody( "&post_key=" );
		post.AppendPostBody( post.GetPostKey() );
		post.AppendPostBody( "&yes=%A1%A1%C1%F7%A1%A1%BF%AE%A1%A1" );

		// Content-Type: application/x-www-form-urlencoded
		post.SetContentType( CONTENT_TYPE_FORM_URLENCODED );

		return true;
	}
};

/**
 * 新規メッセージの確認画面用 POST 電文生成クラス（書き込み画面⇒確認画面ボタン）
 * 
 * send_message.pl
 */
class PostNewMessageGenerator : public PostDataGeneratorBase {
public:

	/**
	 * CPostData の生成
	 *
	 * @param post				CPostData オブジェクト
	 * @param msg				ユーザが入力したメッセージ
	 */
	static bool generate( CPostData& post,
						  LPCTSTR msg )
	{
		// 次画面で hidden タグとして埋め込むメッセージを保存しておく
		post.SetComment(msg);

		// POST 電文の生成
		post.ClearPostBody();
		post.AppendPostBody( "submit=main&" );
		post.AppendPostBody( "subject=title&" );
		post.AppendPostBody( "body=" );
		post.AppendPostBody( msg );
		post.AppendPostBody( "&post_key=" );
//		post.AppendPostBody( "74b630af81dfaae59bfb6352728844a7" );
		post.AppendPostBody( generateNewPostKey() );

		// Content-Type を設定する
		// Content-Type: application/x-www-form-urlencoded
		post.SetContentType( CONTENT_TYPE_FORM_URLENCODED );

		return true;
	}

	static CStringA generateNewPostKey() {
		CStringA postKey;

		const char chars[] = "abcdefghijklmnopqrstuvwxyz0123456789";
		srand( (int)GetTickCount() );
		for( int i=0; i<22; i++ ) {
			int idx = rand() % 36;

			postKey.AppendChar( chars[idx] );
		}

		return postKey;
	}
};

/**
 * 新規メッセージの書き込み画面用 POST 電文生成クラス（確認画面｜書き込みボタン押下）
 * 
 * send_message.pl
 */
class EntryNewMessageGenerator : public PostDataGeneratorBase {
public:

	/**
	 * CPostData の生成
	 *
	 * @param post				CPostData オブジェクト
	 * @param title				タイトル文字列(wchar_t)
	 */
	static bool generate( CPostData& post, LPCTSTR title )
	{
		post.ClearPostBody();

		post.AppendPostBody( "submit=confirm&" );
		post.AppendPostBody( "&subject=" );

		// タイトルを EUC-JP URL Encoded String に変換して埋め込む
		post.AppendPostBody( URLEncoder::encode_euc( title ) );

		// 本文を埋め込む
		post.AppendPostBody( "&body=" );
		post.AppendPostBody( post.GetComment() );

		post.AppendPostBody( "&post_key=" );
		post.AppendPostBody( post.GetPostKey() );
		post.AppendPostBody( "&yes=%A1%A1%C1%F7%A1%A1%BF%AE%A1%A1" );

		// Content-Type: application/x-www-form-urlencoded
		post.SetContentType( CONTENT_TYPE_FORM_URLENCODED );

		return true;
	}
};


}
