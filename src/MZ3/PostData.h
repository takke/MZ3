/*
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */
#pragma once

// CPostData コマンド ターゲット

/**
 * POST 送信データ
 */
class CPostData
{
public:
	/// POST のボディ部の型
	typedef std::vector<char> post_array;

private:

	post_array	 m_postBody;	///< POSTのボディ部

	CString		 m_comment;		///< hidden タグとして書き込み時に送るユーザ入力メッセージの本文

	CString		 m_postkey;		///< 確認画面の post_type 値（mixi の認証キー？）
	CString		 m_packed;		///< 確認画面の packed 値（mixi の認証キー？）

	CString		 m_contentType;	///< Content-Type
	int			 m_procType;	///< 処理タイプ（MZ3_POST_* 定数）。
								///< 成功時に CInternetAccess スレッドから送信されるメッセージ種別になる。
	CString		 m_confirmUri;	///< 確認URI

	CString		 m_additionalHeaders;	///< 追加パラメータ

public:
	CPostData() { m_postBody.reserve(10 * 1024); }
	virtual ~CPostData() {}

	bool AppendAdditionalHeader( LPCTSTR szHeader ) {
		if (!m_additionalHeaders.IsEmpty()) {
			m_additionalHeaders.Append( L"\r\n" );
		}
		m_additionalHeaders.Append( szHeader );
		return true;
	}

	const CString& GetAdditionalHeaders() {
		return m_additionalHeaders;
	}

	/// POST のボディ部の取得（参照により設定も可能）
	post_array& GetPostBody() { return m_postBody; }

	/// POST のボディ部を初期化する
	void ClearPostBody() {
		m_postBody.clear();
		m_additionalHeaders = L"";
	}

	/// POST のボディ部に文字列を追加する。
	/// 末尾に CRLF を追加する。
	/// 文字列は EUC-JP URL Encoded String であること。
	void AppendPostBodyWithCRLF( LPCTSTR strEucUrlEncodedString ) {
		AppendPostBody( strEucUrlEncodedString );
		AppendPostBody( "\r\n" );
	}

	/// POST のボディ部に ANSI 文字列を追加する。
	/// 末尾に CRLF を追加する。
	void AppendPostBodyWithCRLF( const char* str ) {
		AppendPostBody( str );
		AppendPostBody( "\r\n" );
	}

	/// POST のボディ部に文字列を追加する。
	/// 文字列は EUC-JP URL Encoded String であること。
	void AppendPostBody( LPCTSTR strEucUrlEncodedString ) {

		if( strEucUrlEncodedString != NULL && wcslen(strEucUrlEncodedString) > 0 ) {
			static post_array buf;
			buf.resize( 10001 );
			memset( &buf[0], 0x00, sizeof(char) * 10001);

			// ANSI 文字列に変換
			wcstombs( &buf[0], strEucUrlEncodedString, 10000);

			// ANSI 文字列を追加
			AppendPostBody( &buf[0] );
		}
	}

	/// POST のボディ部に ANSI 文字列を追加する。
	void AppendPostBody( const char* str ) {

		AppendPostBody( str, strlen(str) );
	}

	/// POST のボディ部に char 配列を追加する。
	void AppendPostBody( const char* str, int len ) {

		if( len>0 ) {
			// 元のサイズ
			size_t n = m_postBody.size();

			// 末尾に追加できるだけのバッファを確保
			m_postBody.resize( n+len );

			// 末尾にコピー
			memcpy( &m_postBody[0] +n, str, len );
		}
	}

	/// post_key 値の設定（確認画面の解析時に設定される）
	void SetPostKey(LPCTSTR str) { m_postkey = str; }

	/// post_key 値の取得（書き込みボタン押下時のPOSTメッセージ作成時に利用される）
	LPCTSTR GetPostKey() { return m_postkey; }

	/// Content-Type の設定
	void SetContentType(CONTENT_TYPE value) {
		m_contentType = value;
	}

	/// Content-Type の取得
	CONTENT_TYPE GetContentType() { return m_contentType; }

	void SetSuccessMessage(int value) { m_procType = value; }
	int GetSuccessMessage() { return m_procType; }

	void SetComment(LPCTSTR str) { m_comment = str; }
	LPCTSTR GetComment() { return m_comment; }

	void SetConfirmUri(LPCTSTR str) { m_confirmUri = str; }
	LPCTSTR GetConfirmUri() { return m_confirmUri; }

	void SetPacked( LPCTSTR str ) { m_packed = str; }
	LPCTSTR GetPacked() { return m_packed; }
};


