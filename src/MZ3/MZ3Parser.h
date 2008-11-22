/*
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */
#pragma once

#include "MyRegex.h"
#include "HtmlArray.h"
#include "MixiParserUtil.h"
#include "xml2stl.h"

/// MZ3用HTMLパーサ
namespace mz3parser {

/// リスト系HTMLの解析
bool MyDoParseMixiListHtml( ACCESS_TYPE aType, CMixiData& parent, CMixiDataList& body, CHtmlArray& html );
void MyDoParseMixiHtml( ACCESS_TYPE aType, CMixiData& mixi, CHtmlArray& html );

}//namespace mz3parser


//--- 以下は本来は実装のみ cpp に記述したい。。。

/// mixi 用HTMLパーサ
namespace mixi {

/// 各種パーサの基本クラス
class MZ3ParserBase 
{
public:
	static bool ExtractLinks( CMixiData& data_ );

	/**
	 * ログアウトしたかをチェックする
	 */
	static bool IsLogout( LPCTSTR szHtmlFilename )
	{
		// 最大で N 行目までチェックする
		const int CHECK_LINE_NUM_MAX = 1000;

		FILE* fp = _wfopen(szHtmlFilename, _T("r"));
		if( fp == NULL ) {
			// 取得失敗
			return false;
		}

		TCHAR buf[4096];
		for( int i=0; i<CHECK_LINE_NUM_MAX && fgetws(buf, 4096, fp) != NULL; i++ ) {
			// <form action="/login.pl" method="post">
			// があればログアウト状態と判定する。
			if (util::LineHasStringsNoCase( buf, L"<form", L"action=", L"login.pl" )) {
				// ログアウト状態
				fclose( fp );
				return true;
			}

			// API 対応（仮実装）
			if (i==0 && util::LineHasStringsNoCase( buf, L"WSSEによる認証が必要です" )) {
				fclose( fp );
				return true;
			}
		}
		fclose(fp);

		// ここにはデータがなかったのでログアウトとは判断しない
		return false;
	}
};

/// list 系ページに対するパーサの基本クラス
class MixiListParser : public MZ3ParserBase
{
protected:
	/**
	 * 正規表現 reg で HTML ソース str を解析し、「次へ」「前へ」のリンクを取得し、設定する。
	 *
	 * <ul>
	 * <li>正規表現 reg の仕様
	 *  <ul>解析成功時は 3 要素であること（"()"を２つ含むこと）。
	 *  <li>解析結果[1] はURLであること。
	 *  <li>解析結果[2] はリンク文字列であること。「次」または「前」を含むこと。
	 *  </ul>
	 * <li>解析成功時はリンク文字列に応じて、nextLink または backLink を生成する。
	 * <li>タイトルと名前に "<< %s >>" を設定する。%s は解析結果[2]を埋め込む。
	 * <li>アクセス種別は accessType を設定する。
	 * </ul>
	 *
	 * @return 成功時は true、失敗時は false
	 */
	static bool parseNextBackLinkBase( CMixiData& nextLink, CMixiData& backLink, CString str, 
									   MyRegex& reg, LPCTSTR strUrlHead, ACCESS_TYPE accessType )
	{
		bool bFound = false;
		for(;;) {
			// 探索
			if( reg.exec(str) == false || reg.results.size() != 3 ) {
				// 未発見。
				break;
			}else{
				// 発見。
				CString s( reg.results[2].str.c_str() );
				if( s.Find( L"前" ) != -1 ) {
					// 「前」
					s.Format( L"%s%s", strUrlHead, reg.results[1].str.c_str() );
					backLink.SetURL( s );
					s.Format( L"<< %s >>", reg.results[2].str.c_str() );
					backLink.SetTitle( s );
					backLink.SetName( s );
					backLink.SetAccessType( accessType );
					bFound = true;
				}else if( s.Find( L"次" ) != -1 ) {
					// 「次」
					s.Format( L"%s%s", strUrlHead, reg.results[1].str.c_str() );
					nextLink.SetURL( s );
					s.Format( L"<< %s >>", reg.results[2].str.c_str() );
					nextLink.SetTitle( s );
					nextLink.SetName( s );
					nextLink.SetAccessType( accessType );
					bFound = true;
				}
				str = str.Mid( reg.results[0].end );
			}
		}
		return bFound;
	}

};

/// contents 系ページに対するパーサの基本クラス
class MixiContentParser : public MZ3ParserBase
{
public:
	/**
	 * 記事投稿用URL等の取得
	 */
	static void parsePostURL(int startIndex, CMixiData& data_, const CHtmlArray& html_)
	{
		const int lastLine = html_.GetCount();

		for (int i=startIndex; i<lastLine; i++) {
			const CString& line = html_.GetAt(i);

			// <form name="bbs_comment_form" action="add_bbs_comment.pl?id=xxx&comm_id=yyy" enctype="multipart/form-data" method="post">
			if (line.Find(L"<form") != -1 &&
				line.Find(L"method=") != -1 &&
				line.Find(L"action=") != -1 &&
				line.Find(L"post") != -1) 
			{
				// delete_xxx.pl の場合は無視
				if (line.Find(L"delete_") != -1) {
					continue;
				}

				// Content-Type/enctype 解決
				if (line.Find(_T("multipart")) != -1) {
					data_.SetContentType(CONTENT_TYPE_MULTIPART);
				} else {
					data_.SetContentType(CONTENT_TYPE_FORM_URLENCODED);
				}

				// action/URL 取得
				CString action;
				if( util::GetBetweenSubString( line, L"action=\"", L"\"", action ) < 0 ) {
					continue;
				}
				data_.SetPostAddress( action );

				switch(data_.GetAccessType()) {
				case ACCESS_DIARY:
				case ACCESS_MYDIARY:
				case ACCESS_NEIGHBORDIARY:
					break;
				default:
					// 以降の解析は不要。
					return;
				}
//				continue;
			}
			
			// <input type="hidden" name="owner_id" value="yyy" />
			if (line.Find(L"<input") != -1 &&
				line.Find(L"hidden") != -1 &&
				line.Find(L"owner_id") != -1)
			{
				CString ownerId;
				if( util::GetBetweenSubString( line, L"value=\"", L"\"", ownerId ) < 0 ) {
					continue;
				}
				MZ3LOGGER_DEBUG( _T("owner_id = ") + ownerId );
				data_.SetOwnerID( _wtoi(ownerId) );
			}

		}
	}

	/**
	 * ページ変更リンクの抽出。
	 */
	static bool parsePageLink( CMixiData& data, const CHtmlArray& html, int startIndex=180 )
	{
		// startIndex 以降に下記を発見したら、解析開始。
/*
<div class="pageNavigation01">
<div class="pageList01">
<div>[<ul><li><a href="view_bbs.pl?page=1&comm_id=xxx&id=yyy">1</a></li><li><a href="view_bbs.pl?page=2&comm_id=xxx&id=23332197">2</a></li><li><a href="view_bbs.pl?page=3&comm_id=xxx&id=23332197">3</a></li><li><a href="view_bbs.pl?page=4&comm_id=xxx&id=23332197">4</a></li><li><a href="view_bbs.pl?page=5&comm_id=xxx&id=23332197">5</a></li><li><a href="view_bbs.pl?page=6&comm_id=xxx&id=23332197">6</a></li></ul>]</div>
</div>
<div class="pageList02">
<ul>
<li><a href="view_bbs.pl?id=23332197&comm_id=xxx&page=all">全てを表示</a></li>
<li>最新の20件を表示</li>
</ul>
</div>
</div>
*/
		const int count = html.GetCount();
		int i=startIndex;

		// pageList01
		for( ; i<count; i++ ) {
			const CString& line = html.GetAt(i);
			if( util::LineHasStringsNoCase( line, L"<div", L"class", L"pageList01" ) ) {
				break;
			}
		}
		if( i >= count ) {
			return false;
		}

		// </div> 発見するまでパターンマッチを繰り返す。
		for( i=i+1; i<count; i++ ) {
			const CString& line = html.GetAt(i);

			CString str = line;
			ParserUtil::ExtractURI( str, data.m_linkPage );

			if( util::LineHasStringsNoCase( line, L"</div>" ) ) {
				break;
			}
		}

		// pageList02
		for( ; i<count; i++ ) {
			const CString& line = html.GetAt(i);
			if( util::LineHasStringsNoCase( line, L"<div", L"class", L"pageList02" ) ) {
				break;
			}
		}
		if( i >= count ) {
			return false;
		}

		// </div> 発見するまでパターンマッチを繰り返す。
		for( i=i+1; i<count; i++ ) {
			const CString& line = html.GetAt(i);

			CString str = line;
			ParserUtil::ExtractURI( str, data.m_linkPage );

			if( util::LineHasStringsNoCase( line, L"</div>" ) ) {
				break;
			}
		}


		return !data.m_linkPage.empty();
	}

	/**
	 * 「最新のトピック」の抽出。
	 */
	static bool parseRecentTopics( CMixiData& data, const CHtmlArray& html, int startIndex=200 )
	{
		// startIndex 以降に下記を発見したら、解析開始。
		// <ul class="newTopicList01">

		const int count = html.GetCount();
		int i=startIndex;
		for( ; i<count; i++ ) {
			const CString& line = html.GetAt(i);
			if( util::LineHasStringsNoCase( line, L"<ul", L"class", L"newTopicList01" ) ) {
				break;
			}
		}
		if( i >= count ) {
			return false;
		}

		// 終了タグを発見するまでパターンマッチを繰り返す。
		for( i=i+1; i<count; i++ ) {
			const CString& line = html.GetAt(i);
			if( util::LineHasStringsNoCase( line, L"<p", L"class", L"utilityLinks03" ) ) {
				break;
			}

			CString str = line;
			if( line.Find( L"view_enquete.pl" ) != -1 ||
				line.Find( L"view_event.pl" ) != -1 ||
				line.Find( L"view_bbs.pl" ) != -1 )
			{
				ParserUtil::ExtractURI( str, data.m_linkPage );
			}
		}

		return !data.m_linkPage.empty();
	}

	/**
	 * コミュニティ名抽出
	 */
	static bool parseCommunityName( CMixiData& mixi, const CHtmlArray& html_ )
	{
		// <p class="utilityLinks03"><a href="view_community.pl?id=1198460">[MZ3 -Mixi for ZERO3-] コミュニティトップへ</a></p>
		const int lastLine = html_.GetCount();
		for (int i=0; i<lastLine; i++) {
			const CString& line = html_.GetAt(i);

			if (util::LineHasStringsNoCase( line, L"<p", L"utilityLinks03", L"view_community.pl" )) {
				CString name;
				if (util::GetBetweenSubString( line, L"<a", L"</a>", name ) >= 0) {
					if (util::GetBetweenSubString( name, L"[", L"]", name ) >= 0) {
						mixi::ParserUtil::ReplaceEntityReferenceToCharacter( name );
						mixi.SetName( name );
						return true;
					}
				}
			}
		}
		return false;
	}

};

//■■■共通■■■

/**
 * [content] home.pl ログイン後のメイン画面用パーサ
 * 【メイントップ画面】
 * http://mixi.jp/home.pl
 */
class HomeParser : public MixiContentParser
{
public:
	/**
	 * ログイン判定
	 *
	 * ログイン成功したかどうかを判定
	 *
	 * @return ログイン成功時は次のURL、失敗時は空の文字列を返す
	 */
	static bool IsLoginSucceeded( const CHtmlArray& html )
	{
		INT_PTR count = html.GetCount();

		for (int i=0; i<count; i++) {
			const CString& line = html.GetAt(i);

			if (util::LineHasStringsNoCase( line, L"refresh", L"check.pl" )) {
				// <html><head><meta http-equiv="refresh" content="0;url=/check.pl?n=%2Fhome.pl"></head></html>
				return true;
			}
			// <title>[mixi]</title>
			if (util::LineHasStringsNoCase(line, L"<title>[mixi]</title>")) {
				return true;
			}
		}

		return false;
	}

	/**
	 * メインページからの情報取得。
	 *
	 * 下記の変数に情報を格納する。
	 * <ul>
	 * <li>theApp.m_loginMng
	 * <li>theApp.m_newMessageCount
	 * <li>theApp.m_newCommentCount
	 * <li>theApp.m_newApplyCount
	 * </ul>
	 */
	static bool parse( const CHtmlArray& html_ )
	{
		MZ3LOGGER_DEBUG( L"HomeParser.parse() start." );

		INT_PTR count = html_.GetCount();

		int index = 0;

		// 新着メッセージ数の取得
		int messageNum = GetNewMessageCount( html_, 350, count, index);
		if (messageNum != 0) {
			theApp.m_newMessageCount = messageNum;

			// バイブしちゃう
			// NLED_SETTINGS_INFO led;
			//led.LedNum = ::NLedSetDevice(0, 
		}

		// 新着コメント数の取得
		int commentNum = GetNewCommentCount( html_, 350, count, index);
		if (commentNum != 0) {
			theApp.m_newCommentCount = commentNum;
		}

		// 承認待ち数の取得
		int applyNum = GetNewAcknowledgmentCount( html_, 350, count, index);
		if (applyNum != 0) {
			theApp.m_newApplyCount = applyNum;
		}

		if (wcslen(theApp.m_loginMng.GetOwnerID()) == 0) {
			// OwnerID が未取得なので解析する
			MZ3LOGGER_DEBUG( L"OwnerID が未取得なので解析します" );

			for (int i=0; i<count; i++) {
				const CString& line = html_.GetAt(i);
				if( util::LineHasStringsNoCase( line, L"<a", L"href=", L"add_diary.pl?id=" ) ) {

					// add_diary.pl 以降を抽出
					CString after;
					util::GetAfterSubString( line, L"add_diary.pl", after );

					CString id;
					if( util::GetBetweenSubString( after, L"id=", L"\"", id ) == -1 ) {
						MZ3LOGGER_ERROR( L"add_diary.pl の引数に id 指定がありません。 line[" + line + L"], after[" + after + L"]" );
					}else{
						MZ3LOGGER_DEBUG( L"OwnerID = " + id );
						theApp.m_loginMng.SetOwnerID(id);
						theApp.m_loginMng.Write();
					}
					break;
				}
			}

			if (wcslen(theApp.m_loginMng.GetOwnerID()) == 0) {
				MZ3LOGGER_ERROR( L"OwnerID を取得できませんでした" );
			}
		}

		MZ3LOGGER_DEBUG( L"HomeParser.parse() finished." );
		return true;
	}

private:
	/**
	 * 新着メッセージ数の解析、取得
	 */
	static int GetNewMessageCount( const CHtmlArray& html, int sLine, int eLine, int& retIndex )
	{
		CString msg = _T("新着メッセージが");

		int messageNum = 0;

		for (int i=sLine; i<eLine; i++) {
			const CString& line = html.GetAt(i);

			int pos;
			if ((pos = line.Find(msg)) != -1) {
				// 新着メッセージあり
				// 不要部分を削除
				CString buf = line.Mid(pos + msg.GetLength());

				pos = buf.Find(_T("件"));
				// これより後ろを削除
				buf = buf.Mid(0, pos);

				TRACE(_T("メッセージ件数 = %s\n"), buf);

				messageNum = _wtoi(buf);
			}
			else if (line.Find(_T("<div id=\"bodySide\">")) != -1) {
				retIndex = i;
				break;
			}
		}

		return messageNum;
	}

	/**
	 * 新着コメント数の解析、取得
	 *
	 * @todo 本来はこのメソッド内で、コメントのリンクも取得すべき。
	 *       日記数・コメント数に対してどんな HTML になるのかよく分からないので保留。
	 */
	static int GetNewCommentCount( const CHtmlArray& html, int sLine, int eLine, int& retIndex)
	{
	/* 対象文字列（改行なし）
	<td><font COLOR=#605048> <font COLOR=#CC9933>・</font> <font color=red><b>1件の日記に対して新着コメントがあります！</b></font></td>
	*/
		CString msg = _T("新着コメントが");

		int commentNum = 0;

		for (int i=sLine; i<eLine; i++) {
			const CString& line = html.GetAt(i);

			int pos;
			if ((pos = line.Find(msg)) != -1) {
				// 新着コメントあり

				// msg の後ろ側を削除。
				CString str = line.Left( pos );

				// "<b>" と "件" に囲まれた部分文字列を、件数とする。
				CString result;
				util::GetBetweenSubString( str, L"<b>", L"件", result );

				TRACE(_T("コメント数 = %s\n"), result);

				commentNum = _wtoi(result);
			}
			else if (line.Find(_T("<div id=\"bodySide\">")) != -1) {
				retIndex = i;
				break;
			}
		}

		return commentNum;
	}

	/**
	 * 承認待ち数の解析、取得
	 */
	static int GetNewAcknowledgmentCount( const CHtmlArray& html, int sLine, int eLine, int& retIndex )
	{
		CString msg = _T("承認待ちの友人が");

		int applyNum = 0;

		for (int i=sLine; i<eLine; i++) {
			const CString& line = html.GetAt(i);

			int pos;
			if ((pos = line.Find(msg)) != -1) {
				// 承認待ちあり
				// 不要部分を削除
				CString buf = line.Mid(pos + msg.GetLength());

				pos = buf.Find(_T("名"));
				// これより後ろを削除
				buf = buf.Mid(0, pos);

				TRACE(_T("承認待ち件数 = %s\n"), buf);

				applyNum = _wtoi(buf);
			}
			else if (line.Find(_T("<div id=\"bodySide\">")) != -1) {
				retIndex = i;
				break;
			}
		}

		return applyNum;
	}
};


/**
 * 画像ダウンロードCGI 用パーサ
 *
 * show_diary_picture.pl
 * show_bbs_comment_picture.pl
 */
class ShowPictureParser : public MZ3ParserBase
{
public:

	/**
	 * 画像URL取得
	 */
	static CString GetImageURL( const CHtmlArray& html )
	{
		INT_PTR count = html.GetCount();

		/* 
		 * 解析対象文字列：
		 * <img SRC="http://ic76.mixi.jp/p/xxx/xxx/diary/xx/x/xxx.jpg" BORDER=0>
		 */
		CString uri;
		for (int i=0; i<count; i++) {
			// 画像へのリンクを抽出
			const CString& line = html.GetAt(i);
			if( util::LineHasStringsNoCase( line, L"<img src=\"", L"\"" ) ) {
				// " から " までを取得する。
				if( util::GetBetweenSubString( line, L"\"", L"\"", uri ) > 0 ) {
					MZ3LOGGER_DEBUG( L"画像へのリンク抽出OK, url[" + uri + L"]" );
					break;
				}
			}
		}
		return uri;
	}
};

}//namespace mixi

namespace mz3parser {

/**
 * [content] RSS パーサ
 */
class RssFeedParser : public mixi::MixiContentParser
{
public:
	static bool parse( CMixiDataList& out_, const CHtmlArray& html_ ) {
		// html_ の文字列化
		std::vector<TCHAR> text;
		html_.TranslateToVectorBuffer( text );

		return RssFeedParser::parse( out_, text );
	}
	static bool parse( CMixiDataList& out_, const std::vector<TCHAR>& text_, CString* pStrTitle=NULL );
	static void setDescriptionTitle( CMixiData& data, CString description, CString title );
};

/**
 * [content] RSS AutoDiscovery パーサ
 */
class RssAutoDiscoveryParser : public mixi::MixiContentParser
{
public:
	static bool parse( CMixiDataList& out_, const std::vector<TCHAR>& text_ );
	static bool parseLinkRecursive( CMixiDataList& out_, const xml2stl::Node& node );
};

}//namespace mz3parser
