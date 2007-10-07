#pragma once

#include "MyRegex.h"
#include "XmlParser.h"
#include "HtmlArray.h"
#include "MixiParserUtil.h"

/// mixi 用HTMLパーサ
namespace mixi {

/// mixi 用パーサの基本クラス
class MixiParserBase 
{
public:
};

/// list 系ページに対するパーサの基本クラス
class MixiListParser : public MixiParserBase
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
class MixiContentParser : public MixiParserBase
{
public:
	/** deprecated... */
	static void GetPostAddress(int sIndex, int eIndex, const CHtmlArray& html_, CMixiData& data_)
	{
		parsePostURL( sIndex, data_, html_ );
	}

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
					break;
				default:
					// 以降の解析は不要。
					return;
				}
				continue;
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
};

//■■■共通■■■
/**
 * home.pl ログイン画面用パーサ
 * 【ログイン画面】
 * http://mixi.jp/home.pl
 */
class LoginPageParser : public MixiParserBase
{
public:
	/**
	 * ログアウトしたかをチェックする
	 */
	static bool isLogout( LPCTSTR szHtmlFilename )
	{
		// 最大で N 行目までチェックする
		const int CHECK_LINE_NUM_MAX = 100;

		FILE* fp = _wfopen(szHtmlFilename, _T("r"));
		if( fp != NULL ) {
			TCHAR buf[4096];

			for( int i=0; i<CHECK_LINE_NUM_MAX && fgetws(buf, 4096, fp) != NULL; i++ ) {
				if( wcsstr( buf, L"regist.pl" ) != NULL ) {
					// ログアウト状態
					fclose( fp );
					return true;
				}
			}
			fclose(fp);
		}

		// ここにはデータがなかったのでログアウトとは判断しない
		return false;
	}

};

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
	 * </ul>
	 */
	static bool parse( const CHtmlArray& html_ )
	{
		MZ3LOGGER_DEBUG( L"HomeParser.parse() start." );

		INT_PTR count = html_.GetCount();

		int index = 0;

		// 新着メッセージ数の取得
		int messageNum = GetNewMessageCount( html_, 100, count, index);
		if (messageNum != 0) {
			theApp.m_newMessageCount = messageNum;

			// バイブしちゃう
			// NLED_SETTINGS_INFO led;
			//led.LedNum = ::NLedSetDevice(0, 
		}

		// 新着コメント数の取得
		int commentNum = GetNewCommentCount( html_, 100, index, index);
		if (commentNum != 0) {
			theApp.m_newCommentCount = commentNum;
		}

		if (wcslen(theApp.m_loginMng.GetOwnerID()) == 0) {
			// OwnerID が未取得なので解析する
			MZ3LOGGER_DEBUG( L"OwnerID が未取得なので解析します" );

			for (int i=index; i<count; i++) {
				const CString& line = html_.GetAt(i);
				if( util::LineHasStringsNoCase( line, L"<a", L"href=", L"list_community.pl" ) ) {
					CString buf;
					
					if( util::GetBetweenSubString( line, L"id=", L"\"", buf ) == -1 ) {
						MZ3LOGGER_ERROR( L"list_community.pl の引数に id 指定がありません。 line[" + line + L"]" );
					}else{
						MZ3LOGGER_DEBUG( L"OwnerID = " + buf );
						theApp.m_loginMng.SetOwnerID(buf);
						theApp.m_loginMng.Write();
					}
					break;
				}
			}

			if (wcslen(theApp.m_loginMng.GetOwnerID()) == 0) {
				MZ3LOGGER_ERROR( L"OwnerID が取得できませんでした" );
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
			else if (line.Find(_T("<!-- お知らせメッセージ ここまで -->")) != -1) {
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
			else if (line.Find(_T("<!-- お知らせメッセージ ここまで -->")) != -1) {
				retIndex = i;
				break;
			}
		}

		return commentNum;
	}

};


//■■■日記■■■
/**
 * [list] list_diary.pl 用パーサ
 * 【最近の日記一覧(自分の日記一覧)】
 * http://mixi.jp/list_diary.pl
 */
class ListDiaryParser : public MixiListParser
{
public:
	static bool parse( CMixiDataList& out_, const CHtmlArray& html_ )
	{
		MZ3LOGGER_DEBUG( L"ListDiaryParser.parse() start." );

		INT_PTR count = html_.GetCount();

		BOOL findFlag = FALSE;
		CMixiData backLink;
		CMixiData nextLink;

		CMixiData data;
		data.SetAccessType( ACCESS_MYDIARY );

		for (int i=170; i<count; i++) {
			const CString& str = html_.GetAt(i);

			if( findFlag ) {
				if( str.Find(_T("/bodyMainAreaMain")) != -1 ) {
					// 終了
					break;
				}

				// 「前を表示」「次を表示」の抽出
				LPCTSTR key = L"件を表示";
				if( str.Find( key ) != -1 ) {
					// リンクっぽいので正規表現マッチングで抽出
					if( parseNextBackLink( nextLink, backLink, str ) ) {
						continue;
					}
				}
			}

			// 名前の取得
			// "の日記</h2>" がある行。
			if (str.Find(_T("の日記")) != -1) {
				// "<h2>" と "の日記</h2>" で囲まれた部分を抽出する
				CString name;
				util::GetBetweenSubString( str, L"<h2>", L"の日記</h2>", name );

				// 名前
				data.SetName( name );
				data.SetAuthor( name );
			}

			const CString& key = _T("<dt><input");
			if (str.Find(key) != -1) {
				findFlag = TRUE;

				// タイトル
				// 解析対象：
        //<dt><input name="diary_id" type="checkbox" value="xxxxx"  /><a href="view_diary.pl?id=xxxx&owner_id=xxxx">タイトル</a><span><a href="edit_diary.pl?id=xxxx">編集する</a></span></dt>

				CString buf;
				util::GetBetweenSubString( str, key, L"</dt>", buf );

				CString title;
				util::GetBetweenSubString( buf, L"\">", L"</a><span><a", title );
				data.SetTitle( title );

				// 日付
        //<dd>2007年06月18日12:10</dd>
				const CString& str = html_.GetAt(i+1);	
				CString date;
				util::GetBetweenSubString( str, L">", L"<", date );
				ParserUtil::ChangeDate(date, &data);


        for (int j=i; j<count; j++) {
					const CString& str = html_.GetAt(j);

					LPCTSTR key = _T("<a href=\"view_diary.pl?id");
					if (str.Find(key) != -1) {
						CString uri;
						util::GetBetweenSubString( str, _T("<a href=\""), L"\">", uri );
						data.SetURL( uri );

						// ＩＤを設定
						CString id;
						util::GetBetweenSubString( uri, L"id=", L"&", id );
						data.SetID( _wtoi(id) );
						ParserUtil::GetLastIndexFromIniFile( data.GetURL(), &data );

						i = j;
						break;
					}
				}

				out_.push_back( data );
			}
		}
		// 前、次のリンクがあれば、追加する。
		if( !backLink.GetTitle().IsEmpty() ) {
			out_.insert( out_.begin(), backLink );
		}
		if( !nextLink.GetTitle().IsEmpty() ) {
			out_.push_back( nextLink );
		}

		MZ3LOGGER_DEBUG( L"ListDiaryParser.parse() finished." );
		return true;
	}

private:
	/// 「次を表示」、「前を表示」のリンクを抽出する
	static bool parseNextBackLink( CMixiData& nextLink, CMixiData& backLink, CString str )
	{
		// 正規表現のコンパイル（一回のみ）
		static MyRegex reg;
		if( !reg.isCompiled() ) {
			if(! reg.compile( L"<a href=list_diary.pl([?]page=[^>]+)>([^<]+)</a>" ) ) {
				MZ3LOGGER_FATAL( FAILED_TO_COMPILE_REGEX_MSG );
				return false;
			}
		}

		return parseNextBackLinkBase( nextLink, backLink, str, reg, L"list_diary.pl", ACCESS_LIST_MYDIARY );
	}

};

/**
 * [list] list_comment.pl 用パーサ
 * 【最近のコメント一覧】
 * http://mixi.jp/list_comment.pl
 */
class ListCommentParser : public MixiListParser
{
public:
	static bool parse( CMixiDataList& out_, const CHtmlArray& html_ )
	{
		MZ3LOGGER_DEBUG( L"ListCommentParser.parse() start." );

		INT_PTR count = html_.GetCount();

		/**
		 * 方針：
		 * - <img src=http://img.mixi.jp/img/pen_y.gif ALIGN=left WIDTH=14 HEIGHT=16>
		 *   が見つかればそこから項目開始とみなす
		 * - その行の
		 *   <img ...>2006年09月20日 07:47</td>
		 *   を正規表現でパースし、時刻を抽出する
		 * - 上記の2行下に
		 *   <a href="view_diary.pl?id=ZZZ&owner_id=ZZZ">内容</a> (Author)
		 *   という形式で項目が存在する。
		 * - これを正規表現でパースし、追加する。
		 * - 項目が見つかって以降に、
		 *   </table>
		 *   があれば、処理を終了する
		 */

		// 項目開始を探す
		bool bInItems = false;	// 項目が見つかったか。
		int iLine = 76;		// とりあえず読み飛ばす
		for( ; iLine<count; iLine++ ) {
			const CString& str = html_.GetAt(iLine);

			if( str.Find( L"<img" ) != -1 && str.Find( L"pen_y.gif" ) != -1 ) {
				// 項目発見。
				bInItems = true;

				//--- 時刻の抽出
				// 正規表現のコンパイル（一回のみ）
				static MyRegex regDate;
				if( !regDate.isCompiled() ) {
					if(! regDate.compile( L"<img.*>(.*)</" ) ) {
						MZ3LOGGER_FATAL( FAILED_TO_COMPILE_REGEX_MSG );
						return false;
					}
				}
				// 探索
				if( regDate.exec(str) == false || regDate.results.size() != 2 ) {
					// 未発見。
					continue;
				}
				// 発見。
				const std::wstring& date = regDate.results[1].str;	// 時刻文字列

				//--- 内容の抽出
				// 2行後ろ以降にある
				// 複数行にわたっている場合があるので、とりあえず１０行プリフェッチ。
				// 但し、</a> が見つかれば終了。
				CString target = L"";
				for( int i=0; i<10 && iLine+2+i < count; i++ ) {
					CString s = html_.GetAt(iLine+2+i);
					target += s;
					if( s.Find( L"</a>" ) >= 0 ) {
						break;
					}
				}
				// 改行は消す
				while( target.Replace( L"\n", L"" ) );

				// 正規表現のコンパイル（一回のみ）
				static MyRegex regBody;
				if( !regBody.isCompiled() ) {
					if(! regBody.compile( 
							L"<a href=\"view_diary.pl\\?id=([0-9]+)&owner_id=([0-9]+)\">(.*)</a> \\((.*)\\)" ) )
					{
						MZ3LOGGER_FATAL( FAILED_TO_COMPILE_REGEX_MSG );
						return false;
					}
				}

				// 探索
				if( regBody.exec(target) == false || regBody.results.size() != 5 ) {
					// 未発見。
					continue;
				}

				// 発見。
				const std::wstring& id        = regBody.results[1].str;	// id
				const std::wstring& ownder_id = regBody.results[2].str;	// ownder_id
				const std::wstring& body      = regBody.results[3].str;	// リンク文字列
				const std::wstring& author    = regBody.results[4].str;	// 投稿者

				// URL 再構築
				CString url;
				url.Format( L"view_diary.pl?id=%s&owner_id=%s", id.c_str(), ownder_id.c_str() );

				// mixi データの作成
				{
					CMixiData data;
					data.SetAccessType(ACCESS_DIARY);

					// 日付
					ParserUtil::ChangeDate( date.c_str(), &data );

					// 見出し
					data.SetTitle( body.c_str() );

					// ＵＲＩ
					data.SetURL( url );

					// ＩＤを設定
					data.SetID(_wtoi(id.c_str()));
					ParserUtil::GetLastIndexFromIniFile(data.GetURL(), &data);

					// 名前
					data.SetName( author.c_str() );
					data.SetAuthor( author.c_str() );

					out_.push_back(data);
				}

				// 5行読み飛ばす
				iLine += 5;
				continue;
			}

			if( bInItems && str.Find(_T("</table>")) != -1 ) {
				// 終了タグ発見
				break;
			}
		}

		MZ3LOGGER_DEBUG( L"ListCommentParser.parse() finished." );
		return true;
	}
};

/**
 * [list] new_friend_diary.pl 用パーサ
 * 【マイミク最新日記一覧】
 * http://mixi.jp/new_friend_diary.pl
 */
class ListNewFriendDiaryParser : public MixiListParser
{
public:
	static bool parse( CMixiDataList& out_, const CHtmlArray& html_ )
	{
		MZ3LOGGER_DEBUG( L"ListNewFriendDiaryParser.parse() start." );

		// 「次」、「前」のリンク
		CMixiData backLink;
		CMixiData nextLink;

		INT_PTR count = html_.GetCount();

		BOOL findFlag = FALSE;
		BOOL dataFind = FALSE;

		for (int i=170; i<count; i++) {

			const CString& line = html_.GetAt(i);

			if (findFlag == FALSE) {
				if (line.Find(_T("NewFriendDiaryArea")) != -1) {
					findFlag = TRUE;
				}
				continue;
			}
			else {

				// 「次を表示」、「前を表示」のリンクを抽出する
				if( parseNextBackLink( nextLink, backLink, line ) ) {
					continue;
				}

				if (util::LineHasStringsNoCase( line, L"<dt>", L"</dt>" ) ) {

					dataFind = TRUE;

					CMixiData data;
					data.SetAccessType(ACCESS_DIARY);

					//--- 時刻の抽出
					//<dt>2007年10月02日&nbsp;22:22</dt>
					CString date;
					util::GetBetweenSubString( line, L"<dt>", L"</dt>", date );
					date.Replace(_T("&nbsp;"), _T(" "));
					ParserUtil::ChangeDate(date, &data);

					//--- 見出しの抽出
					//<dd><a href="view_diary.pl?id=xxx&owner_id=xxx">タイトル</a> (なまえ)<div style="visibility: hidden;" class="diary_pop" id="xxx"></div>
					// or
					//<dd><a href="view_diary.pl?url=xxx&owner_id=xxx">タイトル</a> (なまえ)
					i++;
					const CString& line2 = html_.GetAt(i);

					CString after;
					util::GetAfterSubString( line2, L"<a", after );
					CString title;
					util::GetBetweenSubString( after, L">", L"<", title );
					data.SetTitle(title);

					// ＵＲＩ
					if (util::LineHasStringsNoCase( line2, L"list_diary.pl" ) ) {
						i += 5;
						continue;
					}
					CString url;
					util::GetBetweenSubString( after, L"\"", L"\"", url );
					data.SetURL(url);

					// ＩＤを設定
					CString id;
					util::GetBetweenSubString( url, L"id=", L"&", id );
					data.SetID(_wtoi(id));
					ParserUtil::GetLastIndexFromIniFile(data.GetURL(), &data);

					// 名前
					CString name;
					if (util::GetBetweenSubString( line2, L"</a> (", L")<div", name) < 0) {
						util::GetBetweenSubString( line2, L"</a> (", L")", name);
					}

					data.SetName(name);
					data.SetAuthor(name);

					out_.push_back( data );
					i += 5;
				}
				else if (line.Find(_T("/newFriendDiaryArea")) != -1 && dataFind != FALSE) {
					// 終了タグ発見
					break;
				}

			}
		}

		// 前、次のリンクがあれば、追加する。
		if( !backLink.GetTitle().IsEmpty() ) {
			out_.insert( out_.begin(), backLink );
		}
		if( !nextLink.GetTitle().IsEmpty() ) {
			out_.push_back( nextLink );
		}

		MZ3LOGGER_DEBUG( L"ListNewFriendDiaryParser.parse() finished." );
		return true;
	}

private:
	/// 「次を表示」、「前を表示」のリンクを抽出する
	static bool parseNextBackLink( CMixiData& nextLink, CMixiData& backLink, CString str )
	{
		// 正規表現のコンパイル（一回のみ）
		static MyRegex reg;
		if( !reg.isCompiled() ) {
			if(! reg.compile( L"<a href=new_friend_diary.pl([?]page=[^>]+)>([^<]+)</a>" ) ) {
				MZ3LOGGER_FATAL( FAILED_TO_COMPILE_REGEX_MSG );
				return false;
			}
		}

		if( parseNextBackLinkBase( nextLink, backLink, str, reg, L"new_friend_diary.pl", ACCESS_LIST_DIARY ) ) {
			// Name 要素は不要なので削除（詳細リストの右側に表示されてしまうのを回避するための暫定処置）
			nextLink.SetName( L"" );
			backLink.SetName( L"" );
			return true;
		}
		return false;
	}

};

/**
 * [list] new_comment.pl 用パーサ
 * 【日記コメント記入履歴一覧】
 * http://mixi.jp/new_comment.pl
 */
class NewCommentParser : public MixiListParser
{
public:
	static bool parse( CMixiDataList& out_, const CHtmlArray& html_ )
	{
		MZ3LOGGER_DEBUG( L"NewCommentParser.parse() start." );

		INT_PTR count = html_.GetCount();

		CString str;
		CString buf;
		CString key;

		BOOL findFlag = FALSE;
		BOOL dataFind = FALSE;

		//int index;
		for (int i=160; i<count; i++) {

			str = html_.GetAt(i);

			if (findFlag == FALSE) {
				if (util::LineHasStringsNoCase( str, L"entryList01") ) {
					// 開始フラグ発見。
					// とりあえずN行無視する。
					i+=3;
					findFlag = TRUE;
				}
				continue;
			}
			else {

				if (util::LineHasStringsNoCase( str, L"<dt>") ) {
					// 項目発見

					dataFind = TRUE;

					CMixiData data;
					data.SetAccessType(ACCESS_DIARY);

					// 日付
					// <dt>2007年10月01日&nbsp;01:11</dt>
					CString date;
					util::GetBetweenSubString( str, L">", L"<", date );
					date.Replace(L"&nbsp;",L"");
					ParserUtil::ChangeDate(date, &data);
					TRACE(_T("%s\n"), data.GetDate());

					// 見出しタイトル
					i+=1;
					str = html_.GetAt(i);

					CString title;
					util::GetBetweenSubString( str, L"\">", L"</a>", title );
					data.SetTitle(title);
					TRACE(_T("%s\n"), data.GetTitle());

					// ＵＲＩ
					buf = util::XmlParser::GetElement(str, 2);
					buf = util::XmlParser::GetAttribute(buf, _T("href="));
					buf = buf.Left(buf.GetLength() -1);
					buf = buf.Right(buf.GetLength() -1);
					data.SetURL(buf);
					TRACE(_T("%s\n"), data.GetURL());

					// ＩＤを設定
					buf = data.GetURL();
					buf = buf.Mid(buf.Find(_T("id=")) + wcslen(_T("id=")));
					buf = buf.Left(buf.Find(_T("&")));
					data.SetID(_wtoi(buf));
					ParserUtil::GetLastIndexFromIniFile(data.GetURL(), &data);

					// 名前
					CString author;
					if( util::GetBetweenSubString( str, L"&nbsp;(", L")</dd>", author ) < 0 ) {
						// not found.
						continue;
					}

					data.SetName( author );
					data.SetAuthor( author );
					TRACE(_T("%s\n"), data.GetName());

					out_.push_back(data);
				}
				else if (str.Find(_T("/newCommentArea")) != -1 && dataFind != FALSE) {
					// 終了タグ発見
					break;
				}

			}
		}

		MZ3LOGGER_DEBUG( L"NewCommentParser.parse() finished." );
		return true;
	}
};

/**
 * [content] view_diary.pl 用パーサ
 * 【日記詳細】
 * http://mixi.jp/view_diary.pl
 */
class ViewDiaryParser : public MixiContentParser
{
public:
	static bool parse( CMixiData& data_, const CHtmlArray& html_ )
	{
		MZ3LOGGER_DEBUG( L"ViewDiaryParser.parse() start." );

		data_.ClearAllList();

		INT_PTR lastLine = html_.GetCount();

		// 自分の日記かどうかを判断する（名前の取得のため）
		bool bMyDiary = true;
		for( int i=170; i<min(100,lastLine); i++ ) {
			const CString& line = html_.GetAt( i );
			if( util::LineHasStringsNoCase( line, L"diaryTitleFriend" ) ) {
				// 上記があるということは、自分の日記ではない。
				bMyDiary = false;
				break;
			}
		}

		// 最初の10行目までを検索し、<title> タグを解析することで、
		// タイトルを取得する
		for( int i=3; i<min(10,lastLine); i++ ) {
			const CString& line = html_.GetAt( i );
			CString title;
			if( util::GetBetweenSubString( line, L"<title>[mixi] ", L"</title>", title ) >= 0 ) {
				// 発見。
				// 自分の日記なら　　「<title>[mixi] タイトル</title>」
				// 自分以外の日記なら「<title>[mixi] 名前 | タイトル</title>」
				// という形式なので、タイトル部だけを抽出
				int idx = title.Find( L" | " );
				if( idx >= 0 ) {
					// タイトル部だけ抽出
					title = title.Mid( idx + wcslen(L" | ") );
				}else{
					// 見つからなければ自分の日記と判断し、そのままタイトルに。
				}
				data_.SetTitle( title );
			}
		}

		// 「最近の日記」の取得
		bool bStartRecentDiary = false;
		for( int iLine=400; iLine<lastLine; iLine++ ) {
			// <h3>最近の日記</h3>
			// があれば「最近の日記」開始とみなす。
			const CString& line = html_.GetAt( iLine );
			if( util::LineHasStringsNoCase( line, L"<h3>", L"最近の日記" ) ) {
				bStartRecentDiary = true;
				continue;
			}

			if( bStartRecentDiary ) {
				// </ul>
				// があれば「最近の日記」終了とみなす。
				if( util::LineHasStringsNoCase( line, L"</ul>" ) ) {
					break;
				}

				// 解析
				// 下記の形式で「最近の日記」リンクが存在する
				// <li><a href="view_diary.pl?id=xxx&owner_id=xxx">たいとる</a></li>
				if( util::LineHasStringsNoCase( line, L"view_diary.pl" ) ) {
					CString url;
					int idx = util::GetBetweenSubString( line, L"<a href=\"", L"\"", url );
					if( idx >= 0 ) {
						CString buf = line.Mid( idx );
						// buf:
						// >たいとる</a></li>

						// タイトル抽出
						CString title;
						if( util::GetBetweenSubString( buf, L">", L"<", title ) > 0 ) {
							CMixiData::Link link( url, title );
							data_.m_linkPage.push_back( link );
						}
					}
				}
			}
		}

		// 日記本文＆コメント解析
		bool bStartDiary = false;	// 日記本文開始フラグ
		bool bEndDiary   = false;	// 日記本文終了フラグ
		for (int i=75; i<lastLine; i++) {
			const CString& line = html_.GetAt(i);

			if (bStartDiary == false) {
				// 日記開始フラグを発見するまで廻す

				if (util::LineHasStringsNoCase( line, L"<div id=\"diary_body\">" ) )
				{
					// 日記開始フラグ発見（日記本文発見）
					bStartDiary = true;

					// とりあえず改行出力
					data_.AddBody(_T("\r\n"));

					// 日記の著者
					// ここで取得しなくても呼び出しもと（一覧）にて取得済みなので、コメントアウトしておく。
/*					{
						// フラグの５０行くらい前に、日記の著者があるはず。
						// <td WIDTH=490 background=http://img.mixi.jp/img/bg_w.gif><b><font COLOR=#605048>XXXの日記</font></b></td>
						// <td WIDTH=490 background=http://img.mixi.jp/img/bg_w.gif><b><font COLOR=#605048>XXXさんの日記</font></b></td>
						for( int iBack=-50; iBack<0; iBack++ ) {
							const CString& line = html_.GetAt( i+iBack );
							LPCTSTR pattern = L"show_friend.pl";
							int idx = line.Find( pattern );
							if( idx >= 0 ) {
								// 発見。
								CString target = line.Mid( idx+wcslen(pattern) );
								// 自分の日記なら「XXXの日記」、自分以外なら「XXXさんの日記」のはず。
								// この規則で著者を解析。
								CString author;
								if( bMyDiary ) {
									util::GetBetweenSubString( target, L">", L"の日記<", author );
								}else{
									util::GetBetweenSubString( target, L">", L"さんの日記<", author );
								}
								// 著者設定
								data_.SetAuthor( author );
								data_.SetName( author );
								break;
							}
						}
					}
*/
					// 公開レベル
					{
						// フラグの５０行くらい前に、「友人まで公開」といった情報があるはず。
						// <img src="http://img.mixi.jp/img/diary_icon1.gif" alt="友人まで公開" height="20" hspace="5" width="22"></td>
					}

					// 日記の添付画像取得
					parseImageLink( data_, html_, i );

					// "<div" の前を除去
					CString str = line.Mid( line.Find(L"<div") );

					// 現在の行を解析、追加。
					CString before;
					if( util::GetBeforeSubString( str, L"</div>", before ) > 0 ) {
						// この１行で終わり
						ParserUtil::AddBodyWithExtract( data_, before );
						bEndDiary = true;
					}else{
						ParserUtil::AddBodyWithExtract( data_, str );
					}
				}
				else if (line.Find(_T("さんは外部ブログを使われています。")) != -1) {
					// 外部ブログ解析
					parseExternalBlog( data_, html_, i );
					break;
				}
			}
			else {
				// 日記開始フラグ発見済み。

				// 終了タグまでデータ取得
				if (line.Find(_T("</div>")) != -1 ) {
					bEndDiary = true;
				}

				if( bEndDiary == false ) {
					// 本文終了タグ未発見
					// 日記本文解析

					// 動画用Scriptタグが見つかったらスクリプト用ループ開始
					if( util::LineHasStringsNoCase(line,L"<script") ) {
						while( i<lastLine ) {
							// 次の行をフェッチ
							const CString& nextLine = html_.GetAt( ++i );
							// 拡張子.flvが見つかったら投入
							if( util::LineHasStringsNoCase(nextLine,L".flv") ) {
								ParserUtil::AddBodyWithExtract( data_, nextLine );
							}
							// </script> があれば終了
							if( util::LineHasStringsNoCase( nextLine, L"</script>" ) ) {
								break;
							}
						}
					}
					else {
						// 本文終了タグ未発見なので、解析＆投入
						ParserUtil::AddBodyWithExtract( data_, line );
					}

				} else {
					// 終了タグ発見
					if (line.Find(_T("</div>")) != -1) {
						ParserUtil::AddBodyWithExtract( data_, line );
					}

					// コメント取得
					i += 10;
					data_.ClearChildren();

					int cmtNum = 0;		// コメント番号
					int index = i;
					while( index < lastLine ) {
						cmtNum++;
						index  = parseDiaryComment(index, lastLine, data_, html_, cmtNum);
						if (index == -1) {
							break;
						}
					}
					if (index == -1 || index >= lastLine) {
						break;
					}
				}
			}
		}

		MZ3LOGGER_DEBUG( L"ViewDiaryParser.parse() finished." );
		return true;
	}

private:
	/// 外部ブログ解析
	static bool parseExternalBlog( CMixiData& data_, const CHtmlArray& html_, int i )
	{
		CString str;

		data_.AddBody(_T("\r\n"));

		str = html_.GetAt(i-1);
		ParserUtil::UnEscapeHtmlElement(str);
		data_.AddBody(str);

		str = html_.GetAt(i);
		ParserUtil::UnEscapeHtmlElement(str);
		data_.AddBody(str);

		i += 5;
		str = html_.GetAt(i);
		data_.AddBody(_T("\r\n"));
		CString buf = str.Mid(str.Find(_T("href=\"")) + wcslen(_T("href=\"")));
		buf = buf.Left(buf.Find(_T("\"")));

		// 外部ブログフラグを立て、外部ブログのURLを設定しておく。
		data_.SetOtherDiary(TRUE);
		data_.SetBrowseUri(buf);

		data_.AddBody(buf);

		return true;
	}

	/// 日記の添付画像取得
	static bool parseImageLink( CMixiData& data_, const CHtmlArray& html_, int iLine_ )
	{
		// フラグのN行前に画像リンクがあるかも。
		int n_images_begin = -25;
		int n_images_end   = -1;
		bool bImageFound = false;
		for( int iBack=n_images_begin; iBack<=n_images_end; iBack++ ) {
			if( iLine_+iBack >= html_.GetCount() ) {
				break;
			}
			CString line = html_.GetAt( iLine_ +iBack );
			if (line.Find(_T("MM_openBrWindow('")) != -1) {
				// 画像発見。
				// 追加。

				// 先頭のタブを削除するために "<" 以前を削除
				int index = line.Find( L"<" );
				if( index > 0 ) {
					line = line.Mid( index );
				}

				ParserUtil::AddBodyWithExtract( data_, line );

				bImageFound = true;
			}
		}
		if( bImageFound ) {
			// 画像があれば、その後ろに改行を追加しておく。
			data_.AddBody(L"\r\n");
		}
		return bImageFound;
	}

	/**
	 * コメント取得
	 *
	 * @param sIndex [in] 開始インデックス
	 * @param eIndex [in] 終了インデックス
	 * @param data   [in/out] CMixiData* データ
	 * @param cmtNum [in] コメント番号
	 */
	static int parseDiaryComment(int sIndex, int eIndex, CMixiData& data_, const CHtmlArray& html_, int cmtNum)
	{
		CString name;
		CString date;
		CString comment;

		CString str;

		BOOL findFlag = FALSE;

		int retIndex = eIndex;

		CMixiData cmtData;		// コメントデータ

		int index;
		CString buf;

		for (int i=sIndex; i<eIndex; i++) {
			str = html_.GetAt(i);

			if (findFlag == FALSE) {
				if( util::LineHasStringsNoCase( str, L"add_comment.pl" ) ||	// コメントなし
					util::LineHasStringsNoCase( str, L"<!--/comment-->" ) ) // コメント全体の終了タグ発見
				{
					GetPostAddress(i, eIndex, html_, data_);
					return -1;
				}

				if ((index = str.Find(_T("show_friend.pl"))) != -1) {
					// コメントヘッダ取得

					// ＩＤ
					cmtData.SetCommentIndex(cmtNum);

					// 名前
					buf = str.Mid(index);
					MixiUrlParser::GetAuthor(buf, &cmtData);

					// 時刻
					for (int j=i+3; j>0; j--) {
						str = html_.GetAt(j);
						if (str.Find(_T("日&nbsp;")) != -1) {

							CString date;
							util::GetBetweenSubString( str, L">", L"<", date );
							date.Replace( L"&nbsp;", L"" );
							ParserUtil::ChangeDate(date, &cmtData);

							break;
						}
					}

					findFlag = TRUE;

				}
			}
			else {

				if (str.Find(_T("<dd>")) != -1) {
					// コメント本文取得
					i++;
					str = html_.GetAt(i);

					if (str.Find(_T("<dd>")) != 0) {
						cmtData.AddBody(_T("\r\n"));
					}

					ParserUtil::AddBodyWithExtract( cmtData, str );

					i++;
					for( ; i<eIndex; i++ ) {
						str = html_.GetAt(i);
						if (str.Find(_T("</dd>")) != -1) {
							// 終了タグが出てきたのでここで終わり
							retIndex = i+5;
							break;
						}

						ParserUtil::AddBodyWithExtract( cmtData, str );
					}
					break;
				}
			}

		}

		if( findFlag ) {
			data_.AddChild(cmtData);
		}
		return retIndex;
	}

};


//■■■コミュニティ■■■
/**
 * [list] new_bbs.pl 用パーサ
 * 【コミュニティ最新書き込み一覧】
 * http://mixi.jp/new_bbs.pl
 */
class NewBbsParser : public MixiListParser
{
public:
	static bool parse( CMixiDataList& out_, const CHtmlArray& html_ )
	{
		MZ3LOGGER_DEBUG( L"NewBbsParser.parse() start." );

		INT_PTR count = html_.GetCount();

		// 「次」、「前」のリンク
		CMixiData backLink;
		CMixiData nextLink;

		BOOL dataFind = FALSE;

		for (int i=140; i<count; i++) {
			const CString& str = html_.GetAt(i);

			// 「次」、「前」のリンク
			// 項目発見前にのみ存在する
			if( !dataFind ) {
				// 「次を表示」、「前を表示」のリンクを抽出する
				if( parseNextBackLink( nextLink, backLink, str ) ) {
					continue;
				}
			}

			// 項目探索
			// <dt class="iconTopic">2007年10月01日&nbsp;22:14</dt>
			if( util::LineHasStringsNoCase( str, L"<dt", L"class", L"iconTopic" ) ||
				util::LineHasStringsNoCase( str, L"<dt", L"class", L"iconEvent" ) ||
				util::LineHasStringsNoCase( str, L"<dt", L"class", L"iconEnquete" ) )
			{
				dataFind = TRUE;

				CMixiData data;

				// 日付
				CString date;
				util::GetBetweenSubString( str, L">", L"</", date );
				date.Replace( L"&nbsp;", L"" );
				ParserUtil::ChangeDate(date, &data);

				// 見出し
				i += 1;
				CString str = html_.GetAt(i);
				// <dd><a href="view_bbs.pl?id=20728968&comment_count=3&comm_id=1198460">【チャット】集え！xxx</a> (MZ3 -Mixi for ZERO3-)</dd>
				CString after;
				if (!util::GetAfterSubString( str, L"href=", after )) {
					MZ3LOGGER_ERROR(L"取得できません:" + str );
					return false;
				}
				CString title;
				util::GetBetweenSubString( after, L">", L"<", title );
				data.SetTitle(title);

				// ＵＲＩ
				CString href;
				util::GetBetweenSubString( str, L"\"", L"\"", href );

				// &で分解する
				while (href.Replace(_T("&amp;"), _T("&")));

				data.SetURL(href);
				data.SetCommentCount(
					MixiUrlParser::GetCommentCount( href ) );

				// URL に応じてアクセス種別を設定
				data.SetAccessType( util::EstimateAccessTypeByUrl(href) );

				// ＩＤを設定
				CString id;
				util::GetBetweenSubString( href, L"id=", L"&", id );
				data.SetID(_wtoi(id));

				// 前回のインデックスを取得
				ParserUtil::GetLastIndexFromIniFile(data.GetURL(), &data);

				// コミュニティ名
				CString communityName;
				util::GetBetweenSubString( str, L"</a>", L"</dd>", communityName );

				// 整形：最初と最後の括弧を取り除く
				communityName.Trim();
				util::GetBetweenSubString( communityName, L"(", L")", communityName );
				data.SetName(communityName);
				out_.push_back( data );
			}
			else if ( dataFind ) {
				if( str.Find(_T("</ul>")) != -1 ) {
					// 終了タグ発見
					break;
				}
			}
		}

		// 前、次のリンクがあれば、追加する。
		if( !backLink.GetTitle().IsEmpty() ) {
			out_.insert( out_.begin(), backLink );
		}
		if( !nextLink.GetTitle().IsEmpty() ) {
			out_.push_back( nextLink );
		}

		MZ3LOGGER_DEBUG( L"NewBbsParser.parse() finished." );
		return true;
	}

private:
	/// 「次を表示」、「前を表示」のリンクを抽出する
	static bool parseNextBackLink( CMixiData& nextLink, CMixiData& backLink, CString str )
	{
		// 正規表現のコンパイル（一回のみ）
		static MyRegex reg;
		if( !reg.isCompiled() ) {
			if(! reg.compile( L"<a href=new_bbs.pl([?]page=[^>]+)>([^<]+)</a>" ) ) {
				MZ3LOGGER_FATAL( FAILED_TO_COMPILE_REGEX_MSG );
				return false;
			}
		}

		if( parseNextBackLinkBase( nextLink, backLink, str, reg, L"new_bbs.pl", ACCESS_LIST_NEW_BBS ) ) {
			// Name 要素は不要なので削除（詳細リストの右側に表示されてしまうのを回避するための暫定処置）
			nextLink.SetName(L"");
			backLink.SetName(L"");
			return true;
		}
		return false;
	}

};

/**
 * [list] list_community.pl 用パーサ。
 * 【コミュニティ一覧】
 * http://mixi.jp/list_community.pl
 */
class ListCommunityParser : public MixiListParser
{
public:
	static bool parse( CMixiDataList& out_, const CHtmlArray& html_ )
	{
		MZ3LOGGER_DEBUG( L"ListCommunityParser.parse() start." );

		INT_PTR count = html_.GetCount();

		// 「次」、「前」のリンク
		CMixiData backLink;
		CMixiData nextLink;

		/**
		* 方針：
		* ★ <ul>で5件ずつ並んでいる。
		*   奇数行にリンク＆画像が、偶数行に名前が並んでいる。
		*   従って、「5件分のリンク抽出」「5件分の名前抽出」という手順で行う。
		*
		* ●iconListが見つかれば、そこから項目開始とみなす。/messageAreaが現れるまで以下を実行する。
		*   (1) view_community.plが見つかるまでの各行をパースし、所定の形式に一致していれば、URLと画像、コミュ名を取得する。
		*   (2) 抽出したデータを out_ に追加する。
		*/
		// 項目開始を探す
		bool bInItems = false;	// 項目開始？
		int iLine = 160;		// とりあえず読み飛ばす
		for( ; iLine<count; iLine++ ) {
			const CString& line = html_.GetAt(iLine);

			if( !bInItems ) {
				// 項目開始？
				if( util::LineHasStringsNoCase( line, L"iconList") ) {
					bInItems = true;
				}
			}

			if( bInItems ) {
				// 5件分取得＆追加
				if(! parseTwoTR( out_, html_, iLine ) ) {
					// 解析エラー
					break;
				}

				// pageNavigation が見つかれば終了
				const CString& line = html_.GetAt(iLine);
				if( util::LineHasStringsNoCase( line, L"pageNavigation" ) ) {
					break;
				}
			}
		}

		// pageNavigationが見つかったので、その後の行から次、前のリンクを抽出
		for( ; iLine<count; iLine++ ) {
			const CString& str = html_.GetAt(iLine-10);

			// 「次を表示」、「前を表示」のリンクを抽出する
			if( parseNextBackLink( nextLink, backLink, str ) ) {
				// 抽出できたら終了タグ
				break;
			}
		}

		// 前、次のリンクがあれば、追加する。
		if( !backLink.GetTitle().IsEmpty() ) {
			out_.insert( out_.begin(), backLink );
		}
		if( !nextLink.GetTitle().IsEmpty() ) {
			out_.push_back( nextLink );
		}

		MZ3LOGGER_DEBUG( L"ListCommunityParser.parse() finished." );
		return true;
	}

private:
	/**
	* 内容を抽出する
	*
	* (1) /messageArea が現れるまでの各行をパースし、のURL、名前を生成する。
	* (2) mixi_list に追加する。
	*/
	static bool parseTwoTR( CMixiDataList& mixi_list, const CHtmlArray& html, int& iLine )
	{
		const int lastLine = html.GetCount();

		bool bBreak = false;
		for( ; iLine < lastLine && bBreak == false; iLine++ ) {
			const CString& line = html.GetAt( iLine );

			//// /messageAreaが見つかれば終了
			if( util::LineHasStringsNoCase( line, L"/messageArea" ) ) {
				return false;
			}

			//view_community.pl で始まるなら、抽出する
			if( util::LineHasStringsNoCase( line, L"view_community.pl" ) ) {
			/* 行の形式：
			line1: <div class="iconListImage"><a href="view_community.pl?id=2640122" style="background: url(http://img-c3.mixi.jp/photo/comm/1/22/2640122_234s.jpg); text-indent: -9999px;" class="iconTitle" title="xxxxxx">xxxxxxの写真</a></div><span>xxxxxx(41)</span>
			line2: <div id="2640122" class="memo_pop"></div><p><a href="show_community_memo.pl?id=2640122" onClick="openMemo(event,'community',2640122);return false;"><img src="http://img.mixi.jp/img/basic/icon/memo001.gif" width="12" height="14" /></a></p>
			*/
				CMixiData mixi;

				// <a 以降のみにする
				CString target;
				if( util::GetAfterSubString( line, L"<a", target ) < 0 ) {
					// <a がなかったので次の行解析へ。
					continue;
				}

				// URL 抽出
				CString url;
				if( util::GetBetweenSubString( target, L"href=\"", L"\"", url ) < 0 ) {
					continue;
				}
				mixi.SetURL( url );

				// URL 構築＆設定
				url.Insert( 0, L"http://mixi.jp/" );
				mixi.SetBrowseUri( url );

				//// Image 抽出
				CString image_url;
				if( util::GetBetweenSubString( target, L"url(", L"); text-indent", image_url ) < 0 ) {
					continue;
				}
				mixi.AddImage( image_url );

				// <span>と</span>の間のコミュ名を抽出
				CString name;
				if( util::GetBetweenSubString( target, L"<span>", L"</span>", name ) < 0 ) {
					continue;
				}
				mixi.SetName( name );
				mixi.SetAccessType( ACCESS_COMMUNITY );

				// mixi_list に追加する。
				mixi_list.push_back( mixi );

			}
		}
		if( iLine >= lastLine ) {
			return false;
		}
		return true;
	}

	/// 「次を表示」、「前を表示」のリンクを抽出する
	/// <td align=right>1件〜50件を表示&nbsp;&nbsp;<a href=list_community.pl?page=2&id=xxx>次を表示</a></td>
	static bool parseNextBackLink( CMixiData& nextLink, CMixiData& backLink, CString str )
	{
		// 正規表現のコンパイル（一回のみ）
		static MyRegex reg;
		if( !reg.isCompiled() ) {
			if(! reg.compile( L"<a href=list_community.pl([?][^>]+)>([^<]+)</a>" ) ) {
				MZ3LOGGER_FATAL( FAILED_TO_COMPILE_REGEX_MSG );
				return false;
			}
		}

		return parseNextBackLinkBase( nextLink, backLink, str,
					reg, L"list_community.pl", ACCESS_LIST_COMMUNITY );
	}
};

/**
 * [list] list_comment.pl 用パーサ
 * 【コミュニティーのトピック一覧】
 * http://mixi.jp/list_bbs.pl
 */
class ListBbsParser : public MixiListParser
{
public:
	static bool parse( CMixiDataList& out_, const CHtmlArray& html_ )
	{
		MZ3LOGGER_DEBUG( L"ListBbsParser.parse() start." );

		INT_PTR count = html_.GetCount();

		// 「次」、「前」のリンク
		CMixiData backLink;
		CMixiData nextLink;

		/**
		 * 方針：
		 * - 下記のロジックで項目開始行を取得する。
		 *   - 無条件で200行読み飛ばす。
		 *   - "<dt class="bbsTitle clearfix">" の行が現れるまで読み飛ばす。
		 *   - 次の行以降が項目。
		 *
		 * - 下記の行をパースし、項目を生成する。
		 *   <span class="titleSpan"><a href="view_bbs.pl?id=23469005&comm_id=1198460" class="title">【報告】10/1リニューアルで動かない！</a>
		 *   </span>←この行は管理者の場合編集リンクなどが入る
		 *   <span class="date">2007年09月30日 16:40</span>
		 * - "/#bodyMainArea" が現れたら無条件で終了とする。
		 *   "<ul><li>〜件を表示"が現れたら（次へ、前へを含む）ナビゲーション行。
		 */

		// 項目開始行を探す
		int iLine = 200;		// とりあえず読み飛ばす
		bool bInItems = false;
		for( ; iLine<count; iLine++ ) {
			const CString& line = html_.GetAt(iLine);
			//<div class="pageTitle communityTitle002">
			if( util::LineHasStringsNoCase( line, L"<dt", L"bbsTitle" ) ) 
			{
				// 項目開始行発見。本文開始
				bInItems = true;
				break;
			}
		}

		if( !bInItems ) {
			return false;
		}

		// 項目の取得
		for( ; iLine<count; iLine++ ) {
			const CString& line = html_.GetAt(iLine);

			// /#bodyMainAreaがあったら解析を終了する。
			if( util::LineHasStringsNoCase( line, L"/#bodyMainArea") ) 
			{
				// 抽出の成否にかかわらず終了する。
				break;
			}

			// <ul><li>が現れたらナビゲーション行。
			// 「次」、「前」のリンクを含む。
			if( util::LineHasStringsNoCase( line, L"<ul><li>", L"件を表示" ) ) 
			{
				// 「次を表示」、「前を表示」のリンクを抽出する
				parseNextBackLink( nextLink, backLink, line );
			}

			// 項目
			//   <span class="titleSpan"><a href="view_bbs.pl?id=23469005&comm_id=1198460" class="title">【報告】10/1リニューアルで動かない！</a>
			//   <span class="date">2007年09月30日 16:40</span>
			if( util::LineHasStringsNoCase( line, L"<span", L"titleSpan", L"view_bbs") ) {
				// 解析
				CMixiData mixi;

				// タイトル抽出
				CString title;
				util::GetBetweenSubString( line, L"class=\"title\">", L"</a>", title );
				mixi.SetTitle(title);

				// URL 抽出
				CString url;
				util::GetBetweenSubString( line, L"href=\"", L"\" class=", url );
				mixi.SetURL( url );

				// URL に応じてアクセス種別を設定
				mixi.SetAccessType( util::EstimateAccessTypeByUrl(url) );

				// IDの抽出、設定
				mixi.SetID( MixiUrlParser::GetID(url) );

				// 日付解析
				{
					//<span class="date">2007年09月22日 12:55</span>
					//4行以内で<span class="date">行が見つかるまで
					for( iLine++; iLine<iLine+4; iLine++ ) {
						const CString& line = html_.GetAt(iLine);
						if( util::LineHasStringsNoCase( line, L"<span", L"date") )
						{
							CString date;
							util::GetBetweenSubString( line, L"class=\"date\">", L"</span>", date );
							ParserUtil::ChangeDate(date, &mixi);
							break;
						}
					}
				}

				// コメント数解析
				{
					// 下記の行を取得して解析する。
					// <em><a href="view_bbs.pl?id=14823636&comm_id=4043">37</a></em>
					int commentCount = -1;
					for( iLine++; iLine<count; iLine++ ) {
						const CString& line = html_.GetAt(iLine);

						if( util::LineHasStringsNoCase( line, L"<em>", url) )
						{
							// 発見。コメント数解析
							CString cc;
							util::GetBetweenSubString( line, L"\">", L"</a></em", cc );
							commentCount = _wtoi(cc);
							break;
						}
					}

					mixi.SetCommentCount(commentCount);

					// タイトルの末尾に付加する
					CString title = mixi.GetTitle();
					title.AppendFormat( L"(%d)", commentCount );
					mixi.SetTitle( title );
				}

				// 前回のインデックスを取得
				ParserUtil::GetLastIndexFromIniFile(mixi.GetURL(), &mixi);

				// コミュニティ名:とりあえず未設定
				mixi.SetName(L"");
				out_.push_back( mixi );
			}
		}

		// 前、次のリンクがあれば、追加する。
		if( !backLink.GetTitle().IsEmpty() ) {
			out_.insert( out_.begin(), backLink );
		}
		if( !nextLink.GetTitle().IsEmpty() ) {
			out_.push_back( nextLink );
		}

		MZ3LOGGER_DEBUG( L"ListBbsParser.parse() finished." );
		return true;
	}

private:
	/// 「次を表示」、「前を表示」のリンクを抽出する
	/// <td ALIGN=right BGCOLOR=#EED6B5>1件〜20件を表示&nbsp;&nbsp;<a href=list_bbs.pl?page=2&id=xxx>次を表示</a></td></tr>
	static bool parseNextBackLink( CMixiData& nextLink, CMixiData& backLink, CString str )
	{
		// 正規表現のコンパイル（一回のみ）
		static MyRegex reg;
		if( !reg.isCompiled() ) {
			if(! reg.compile( L"<a href=list_bbs.pl([?][^>]+)>([^<]+)</a>" ) ) {
				MZ3LOGGER_FATAL( FAILED_TO_COMPILE_REGEX_MSG );
				return false;
			}
		}

		return parseNextBackLinkBase( nextLink, backLink, str, reg, L"list_bbs.pl", ACCESS_LIST_BBS );
	}
};

/**
 * [content] view_bbs.pl 用パーサ
 * 【コミュニティートピック詳細】
 * http://mixi.jp/view_bbs.pl
 */
class ViewBbsParser : public MixiContentParser
{
public:
	static bool parse( CMixiData& mixi, const CHtmlArray& html_ ) 
	{
		MZ3LOGGER_DEBUG( L"ViewBbsParser.parse() start." );

		mixi.ClearAllList();

		INT_PTR lastLine = html_.GetCount();

		if (html_.GetAt(2).Find(_T("302 Moved")) != -1) {
			// 現在は見れない
			TRACE(_T("Moved\n"));
			return false;
		}

		bool bFoundMainText = false;	///< 本文解析成功？

		// 本文取得
		int i=100;
		for (; i<lastLine; i++) {
			const CString& line = html_.GetAt(i);

			if (bFoundMainText) {
				// 本文取得済みなので終了。
				break;
			}

			// 投稿日時を取得する
			// <span class="date">2007年07月14日 22:22</span></dt>
			if( util::LineHasStringsNoCase( line, L"span", L"class", L"date" ) ) {
				CString date;
				util::GetBetweenSubString( line, L">", L"<", date );
				ParserUtil::ChangeDate(date, &mixi);
			}


			if (util::LineHasStringsNoCase( line, L"<dd", L"class", L"bbsContent" ) ) {
				// 本文開始。トピック本文ね。
				bFoundMainText = true;

				// とりあえず改行
				mixi.AddBody(_T("\r\n"));

				// <dd>
				// から
				// </dd>
				// までを取得し、解析する。

				// <dd> を探す。
				for( ; i<lastLine; i++ ) {
					const CString& line = html_.GetAt( i );

					// "show_friend.pl" つまりトピ作成者のプロフィールリンクがあれば、
					// トピ作成者のユーザ名を取得する。
					if( util::LineHasStringsNoCase( line, L"show_friend.pl" ) ) {

						MixiUrlParser::GetAuthor(line, &mixi);

						// この行にはないので次の行へ。
						continue;
					}

					if (util::LineHasStringsNoCase( line, L"<dd>" )) {
						// <dd>以降を取得し、あれば つっこんで終了。
						CString after;
						util::GetAfterSubString( line, L"<dd>", after );
						ParserUtil::AddBodyWithExtract( mixi, after );
						++i;
						break;
					}
				}

				// </dd> が現れるまで、解析する。
				for( ; i<lastLine; i++ ) {
					const CString& line = html_.GetAt( i );

					if (util::LineHasStringsNoCase( line, L"</dd>" )) {
						// 終了。
						break;
					}

					// 動画用Scriptタグが見つかったらスクリプト用ループ開始
					if( util::LineHasStringsNoCase(line, L"<script") ) {
						while( i<lastLine ) {
							// 次の行をフェッチ
							const CString& nextLine = html_.GetAt( ++i );
							// 拡張子.flvが見つかったら投入
							if( util::LineHasStringsNoCase(nextLine,L".flv") ) {
								ParserUtil::AddBodyWithExtract( mixi, nextLine );
							}
							// </script> があれば終了
							if( util::LineHasStringsNoCase( nextLine, L"</script>" ) ) {
								break;
							}
						}
					} else {
						// ふつうに解析＆投入
						ParserUtil::AddBodyWithExtract( mixi, line );
					}
				}
			}
		}

		if (bFoundMainText) {
			// 本文取得済みなので、コメント取得処理を行う。
			for (; i<lastLine; i++) {
				const CString& line = html_.GetAt(i);

				// <dl class="commentList01"> を発見したら、コメント取得処理を行う。
				if (util::LineHasStringsNoCase( line, L"<dl", L"class", L"commentList01" )) {
					mixi.ClearChildren();

					int index = ++i;
					while( index < lastLine ) {
						index = parseBBSComment(index, lastLine, &mixi, html_);
						if( index == -1 ) {
							// エラーなので終了
							break;
						}
					}
					if (index == -1 || index >= lastLine) {
						break;
					}
				}
			}
		}

		// ページ移動リンクの抽出
		parsePageLink( mixi, html_ );

		// 「最新のトピック」の抽出
		parseRecentTopics( mixi, html_ );

		// 書き込み先URLの取得
		parsePostURL( 200, mixi, html_ );

		MZ3LOGGER_DEBUG( L"ViewBbsParser.parse() finished." );
		return true;
	}

private:

	/**
	 * ＢＢＳコメント取得 トピック コメント一覧
	 */
	static int parseBBSComment(int sIndex, int eIndex, CMixiData* data, const CHtmlArray& html_ ) 
	{
		INT_PTR lastLine = html_.GetCount(); //行数

		int retIndex = eIndex;
		CMixiData cmtData;
		bool findFlag = false;

/*
<dt class="commentDate clearfix"><span class="senderId">&nbsp;&nbsp;番号
</span>
<span class="date">2007年07月28日 21:09</span></dt>
<dd>
<dl class="commentContent01">
<dt><a href="show_friend.pl?id=XXX">なまえ</a></dt>
<dd>
...
</dd>
*/
		int i=0;
		for( i=sIndex; i<eIndex; i++ ) {
			const CString& line = html_.GetAt(i);
			// 番号取得
			// <dt class="commentDate clearfix"><span class="senderId">&nbsp;&nbsp;番号
			if (util::LineHasStringsNoCase( line, L"<dt", L"commentDate", L"senderId" )) {
				CString number;
				util::GetAfterSubString( line, L"senderId\">", number );
				// &nbsp; を消す
				while(number.Replace(L"&nbsp;",L"")) {}

				cmtData.SetCommentIndex(_wtoi(number));
			}

			// 番号取得２（編集可能なコミュ）
			// <span class="senderId"><input id="commentCheck01" name="comment_id" type="checkbox" value="291541807" /><label for="commentCheck01">&nbsp;12</label></span>
			if (util::LineHasStringsNoCase( line, L"<span", L"senderId", L"checkbox" )) {
				CString number;
				util::GetAfterSubString( line, L"<label", number );
				//  for="commentCheck01">&nbsp;12</label></span>

				util::GetBetweenSubString( number, L">", L"<", number );

				// &nbsp; を消す
				while(number.Replace(L"&nbsp;",L"")) {}

				cmtData.SetCommentIndex(_wtoi(number));
			}

			// 日付
			// <span class="date">2007年07月28日 21:09</span></dt>
			if (util::LineHasStringsNoCase( line, L"<span", L"class", L"date" ) ) {
				CString date;
				util::GetBetweenSubString( line, L">", L"<", date );
				ParserUtil::ChangeDate(date, &cmtData);
			}

			// <div class="pageNavigation01"> を発見したら、コメント終了なので抜ける
			if (util::LineHasStringsNoCase( line, L"<div", L"class", L"pageNavigation01" )) {
				return -1;
			}

			// <!-- ADD_COMMENT: start -->を発見したら、コメント終了なので抜ける
			if (util::LineHasStringsNoCase( line, L"<!", L"ADD_COMMENT", L"start" )) {
				return -1;
			}

			// <dl class="commentContent01"> を発見したらループ抜ける
			if (util::LineHasStringsNoCase( line, L"<dl", L"class", L"commentContent01" )) {
				findFlag = true;
				i++;
				break;
			}
		}

		for( ; i<eIndex; i++ ) {
			const CString& line = html_.GetAt(i);
			// なまえ
			// <dt><a href="show_friend.pl?id=XXX">なまえ</a></dt>
			if (util::LineHasStringsNoCase( line, L"<dt", L"show_friend.pl" )) {

				MixiUrlParser::GetAuthor( line, &cmtData );
			}

			// <dd> 発見したらループ抜ける
			if (util::LineHasStringsNoCase( line, L"<dd>" )) {
				i++;

				// 改行追加
				cmtData.AddBody(_T("\r\n"));
				break;
			}
		}

		// </dd> まで解析＆追加
		for( ; i<eIndex; i++ ) {
			const CString& line = html_.GetAt(i);

			// </dd> 発見したらループ抜ける
			if (util::LineHasStringsNoCase( line, L"</dd>" )) {
				i++;
				break;
			}

			// 動画用Scriptタグが見つかったらスクリプト用ループ開始
			if( util::LineHasStringsNoCase(line, L"<script") ) {
				while( i<lastLine ) {
					// 次の行をフェッチ
					const CString& nextLine = html_.GetAt( ++i );
					// 拡張子.flvが見つかったら投入
					if( util::LineHasStringsNoCase(nextLine,L".flv") ) {
						ParserUtil::AddBodyWithExtract( cmtData, nextLine );
					}
					// </script> があれば終了
					if( util::LineHasStringsNoCase( nextLine, L"</script>" ) ) {
						break;
					}
				}
			} else {
				// ふつうに解析＆投入
				ParserUtil::AddBodyWithExtract( cmtData, line );
			}
		}
		retIndex = i;

		if( findFlag ) {
			data->AddChild(cmtData);
		}
		return retIndex;
	}
};

/**
 * [content] view_enquete.pl 用パーサ
 * 【アンケート詳細】
 * http://mixi.jp/view_enquete.pl
 */
class ViewEnqueteParser : public MixiContentParser
{
public:
	static bool parse( CMixiData& data_, const CHtmlArray& html_ )
	{
		MZ3LOGGER_DEBUG( L"ViewEnqueteParser.parse() start." );

		data_.ClearAllList();
		data_.ClearChildren();

		INT_PTR lastLine = html_.GetCount();

		/*
		 * 解析方針：
		 * ●設問内容解析。
		 *   "BGCOLOR=#FFD8B0", "COLOR=#996600", "設問内容" がある行を見つけたら、設問内容解析を行う。
		 *   詳細は parseBody のコメント参照。
		 *   これ以降を「アンケート開始」とする。
		 * ●企画者解析。
		 *   "BGCOLOR=#FFD8B0", "COLOR=#996600", "企画者" がある行を見つけたら、企画者解析を行う。
		 *   次の行に、下記の形式で企画者がある。
		 *   <td BGCOLOR=#FFFFFF><a href="show_friend.pl?id=xxx">名前</a></td></tr>
		 * ●集計結果解析。
		 *   下記の形式の行から集計結果開始。
		 *   <td BGCOLOR=#FFD8B0 ALIGN=center><font COLOR=#996600>集計結果</font></td>
		 *   詳細は parseEnqueteResult 参照。
		 * ●</table> のみの行があれば、「アンケート終了」とし、コメント解析に移る。
		 */
		bool bInEnquete = false;
		int iLine=180;
		for( ; iLine<lastLine; iLine++ ) {
			const CString& line = html_.GetAt(iLine);

			// ●設問内容解析
			if( util::LineHasStringsNoCase( line, L"BGCOLOR=#FFD8B0", L"COLOR=#996600", L"設問内容" ) )
			{
				if( !parseBody( data_, html_, iLine ) )
					return false;
				bInEnquete = true;
				continue;
			}

			// ●企画者解析
			if( util::LineHasStringsNoCase( line, L"BGCOLOR=#FFD8B0", L"COLOR=#996600", L"企画者" ) )
			{
				// 次の行にある。
				const CString& line = html_.GetAt( ++iLine );
				MixiUrlParser::GetAuthor( line, &data_ );
				data_.SetDate(_T(""));
				continue;
			}

			// ●集計結果解析。
			// <td BGCOLOR=#FFD8B0 ALIGN=center><font COLOR=#996600>集計結果</font></td>
			if( util::LineHasStringsNoCase( line, L"BGCOLOR=#FFD8B0", L"COLOR=#996600", L"集計結果" ) )
			{
				if( !parseEnqueteResult( data_, html_, iLine ) )
					return false;
				continue;
			}

			// ●アンケート終了？
			if( bInEnquete && util::LineHasStringsNoCase( line, L"</table>" ) ) {
				bInEnquete = false;
				break;
			}
		}

		// コメント解析
		while( iLine<lastLine ) {
			iLine = parseEnqueteComment( iLine, &data_, html_ );
			if( iLine == -1 ) {
				break;
			}
		}

		// ページ移動リンクの抽出
		parsePageLink( data_, html_ );

		// 「最新のトピック」の抽出
		parseRecentTopics( data_, html_ );

		MZ3LOGGER_DEBUG( L"ViewEnqueteParser.parse() finished." );
		return true;
	}

private:

	/**
	 * 設問内容を取得する。
	 *
	 * iLine は "設問内容" が存在する行。
	 *
	 * (1) "</table>" が現れるまで無視。その直後から、設問内容本文。解析して、AddBody する。
	 * (2) "</td></tr>" が現れたら終了。
	 */
	static bool parseBody( CMixiData& mixi, const CHtmlArray& html, int& iLine )
	{
		++iLine;

		mixi.AddBody(_T("\r\n"));
		const int lastLine = html.GetCount();

		for( ; iLine<lastLine; iLine++ ) {
			const CString& line = html.GetAt(iLine);

			// </table> があれば、その後ろを本文とする。
			CString target;
			if( util::GetAfterSubString( line, L"</table>", target ) >= 0 ) {
				// </table> 発見。
			}else{
				// </table> 未発見。
				target = line;
			}

			// </td></tr> があれば、その前を追加し、終了。
			// なければ、その行を追加し、次の行へ。
			if( util::GetBeforeSubString( target, L"</td></tr>", target ) < 0 ) {
				// </td></tr> が見つからなかった。
				ParserUtil::AddBodyWithExtract( mixi, target );
			}else{
				// </td></tr> が見つかったので終了。
				ParserUtil::AddBodyWithExtract( mixi, target );
				break;
			}
		}
		if( iLine >= lastLine ) {
			return false;
		}
		return true;
	}

	/**
	 * 集計結果を取得する。
	 *
	 * iLine は "集計結果" が存在する行。
	 *
	 * ● "</table>" が現れたら終了。
	 *
	 * ● 下記の形式で、ある選択肢に対する要素がある。
<tr>
<td bgcolor="#fdf9f2">
アンケート選択肢内容<br>
<img alt="" src="http://img.mixi.jp/img/bar.gif" width="355" height="16" hspace="1" vspace="1">
</td>
<td width="50" bgcolor="#fff4e0" align="right" valign="bottom"><font color="#996600">投票数 (百分率%)</font></td>
</tr>
     * 解析、整形して、AddBody する。
	 *
	 * ● 下記の形式で、合計があるので、解析、整形して、AddBodyする。
<td height="26" bgcolor="#f7f0e6"><font color="#996600">合 計</font>&nbsp;</td>
<td width="50" bgcolor="#ffd8b0"><b><font color="#996600">34</font></b></td>
	 */
	static bool parseEnqueteResult( CMixiData& mixi, const CHtmlArray& html, int& iLine )
	{
		++iLine;

		mixi.AddBody( L"\r\n" );
		mixi.AddBody( L"\r\n" );
		mixi.AddBody( L"◆集計結果\r\n" );

		const int lastLine = html.GetCount();
		for( ; iLine<lastLine; iLine++ ) {
			const CString& line = html.GetAt(iLine);

			// </table> があれば、終了。
			if( util::LineHasStringsNoCase( line, L"</table>" ) ) {
				break;
			}

			// 選択肢、投票数、百分率を解析して、AddBody する。
			if( util::LineHasStringsNoCase( line, L"<td bgcolor=\"#fdf9f2\">" ) ) {
				// 境界値チェック
				if( iLine+5 >= lastLine ) {
					break;
				}
				// 次の行に本文がある。
				const CString& line1 = html.GetAt(++iLine);
				CString item;
				util::GetBeforeSubString( line1, L"<br>", item );

				// +3 行目に下記の形式で、投票数、百分率がある。
				// <td width="50" bgcolor="#fff4e0" align="right" valign="bottom"><font color="#996600">xx (yy%)</font></td>
				iLine += 3;
				const CString& line2 = html.GetAt( iLine );
				CString target;
				util::GetAfterSubString( line2, L"#996600\">", target );
				// xx (yy%)</font></td>
				CString num_rate;
				util::GetBeforeSubString( target, L"</font>", num_rate );

				CString str;
				str.Format( L"  ●%s\r\n", item );
				mixi.AddBody( str );
				str.Format( L"      %s\r\n", num_rate );
				mixi.AddBody( str );
			}

			// 合計を解析する。
			if( util::LineHasStringsNoCase( line, L"<td", L"#f7f0e6", L"#996600", L"合 計" ) )
			{
				// 次の行に合計がある。
				const CString& line1 = html.GetAt( ++iLine );

				CString total;
				util::GetBetweenSubString( line1, L"#996600\">", L"</font>", total );

				CString str;
				str.Format( L"  ●合計\r\n" );
				mixi.AddBody( str );
				str.Format( L"      %s\r\n", total );
				mixi.AddBody( str );
			}
		}

		if( iLine >= lastLine ) {
			return false;
		}
		return true;
	}

	/**
	 * アンケートコメント取得
	 */
	static int parseEnqueteComment(int sIndex, CMixiData* data, const CHtmlArray& html_ )
	{
		CString str;

		int eIndex = html_.GetCount();
		int retIndex = eIndex;

		CMixiData cmtData;
		BOOL findFlag = FALSE;
		CString buf;
		CString comment;
		int index;

		for (int i=sIndex; i<eIndex; i++) {
			str = html_.GetAt(i);

			if (findFlag == FALSE) {

				if (str.Find(_T("<!-- ADD_COMMENT: start -->")) != -1) {
					// コメントなし
					GetPostAddress(i, eIndex, html_, *data);
					retIndex = -1;
					break;
				}
				else if (str.Find(_T("<!-- COMMENT: end -->")) != -1) {
					// コメント全体の終了タグ発見
					GetPostAddress(i, eIndex, html_, *data);
					retIndex = -1;
					break;
				}
				//コメント終了条件追加(2006/11/19 icchu追加)
				else if (str.Find(_T("add_enquete_comment.pl")) != -1) {
					// コメント全体の終了タグ発見
					GetPostAddress(i, eIndex, html_, *data);
					retIndex = -1;
					break;
				}
				//2006/11/19 icchu追加ここまで
				else if ((index = str.Find(_T("show_friend.pl"))) != -1) {

					// 2行前に下記の形式でコメント番号が存在する。
					// &nbsp;<font color="#f8a448"><b>&nbsp;&nbsp;1</b>:</font>&nbsp;
					const CString& line1 = html_.GetAt( i-2 );

					CString strIndex;
					util::GetBetweenSubString( line1, L"<b>", L"</b>", strIndex );
					while( strIndex.Replace( L"&nbsp;", L"" ) );
					cmtData.SetCommentIndex( _wtoi(strIndex) );

					// この行に、コメント者が下記の形式で存在する。
					// <a href="show_friend.pl?id=xxxx">なまえ</a>
					const CString& line2 = html_.GetAt(i);
					MixiUrlParser::GetAuthor( line2, &cmtData );

					// 10行前に、下記の形式で時刻が存在する。
					// 2006年12月xx日<br>HH:MM<br>
					const CString& line3 = html_.GetAt(i-10);
					CString date = line3;
					while( date.Replace( L"<br>", L" " ) );

					ParserUtil::ChangeDate(date, &cmtData);

					findFlag = TRUE;
				}
			}
			else {

				if( util::LineHasStringsNoCase( str, L"<tr", L"<td", L"class=\"h120\"") ) {
					// コメントコメント本文取得
					str = html_.GetAt(i);

					// ----------------------------------------
					// アンケートのパターン
					// ----------------------------------------
					cmtData.AddBody(_T("\r\n"));

					// [    <tr><td class="h120" width="500">０１２３４５６７８９０]
					// ↓
					// [<td class="h120" width="500">０１２３４５６７８９０]

					// [    <tr><td class="h120" width="500">テストあげ</td></tr></table>]
					// ↓
					// [<td class="h120" width="500">テストあげ</td></tr></table>]
					util::GetAfterSubString( str, L">", buf );

					if( util::GetBeforeSubString( buf, L"</td></tr></table>", buf ) > 0 ) {
						// [<td class="h120" width="500">テストあげ</td></tr></table>]
						// ↓
						// [<td class="h120" width="500">テストあげ]

						// 終了タグがあった場合
						ParserUtil::AddBodyWithExtract( cmtData, buf );
						retIndex = i + 5;
						break;
					}

					// それ以外の場合
					buf.Replace(_T("\n"), _T("\r\n"));
					ParserUtil::AddBodyWithExtract( cmtData, buf );

					while( i<eIndex ) {
						i++;

						const CString& line  = html_.GetAt(i);
						const CString& line2 = html_.GetAt(i+1);
						if (util::LineHasStringsNoCase(line, L"</td>") &&
							util::LineHasStringsNoCase(line2,L"</tr>") ) 
						{
							// 一番最後の改行を削除
							buf = line;
							buf.Replace(_T("\n"), _T(""));
							ParserUtil::AddBodyWithExtract( cmtData, buf );

							// 終了タグが出てきたのでここで終わり
							retIndex = i + 5;
							break;
						}
						else if ((index = line.Find(_T("</td></tr>"))) != -1) {
							// 終了タグ発見
							buf = line.Left(index);
							ParserUtil::AddBodyWithExtract( cmtData, buf );
							retIndex = i + 5;
							break;
						}

						ParserUtil::AddBodyWithExtract( cmtData, line );
					}
					break;
				}
			}

		}

		if( findFlag ) {
			data->AddChild(cmtData);
		}
		return retIndex;
	}

};

/**
 * [content] view_event.pl 用パーサ
 * 【イベント詳細】
 * http://mixi.jp/view_event.pl
 */
class ViewEventParser : public MixiContentParser
{
public:
	static bool parse( CMixiData& data_, const CHtmlArray& html_ )
	{
		MZ3LOGGER_DEBUG( L"ViewEventParser.parse() start." );

		data_.ClearAllList();

		INT_PTR count = html_.GetCount();

		CString str;
		CString buf;
		BOOL findFlag = FALSE;
		BOOL endFlag = FALSE;

		int index;

		for (int i=180; i<count; i++) {
			str = html_.GetAt(i);

			if (findFlag == FALSE) {

				if (str.Find(_T("企画者")) != -1) {

					// 次の行に企画者（Author）があるはず。
					// <td bgcolor=#FDF9F2>&nbsp;<a href="show_friend.pl?id=xxx">なまえ</a>
					i++;
					str = html_.GetAt(i);

					index = str.Find(_T("show_friend.pl"));
					if( index >= 0 ) {
						buf = str.Mid(index);
						MixiUrlParser::GetAuthor( buf, &data_ );
					}else{
						MZ3LOGGER_ERROR( L"show_friend.pl が見つかりません [" + str + L"]" );
						// 退会済みの場合もあるのでそのまま続行
						ParserUtil::AddBodyWithExtract( data_, str );
						data_.SetAuthor( str );
					}

					// 開催日時の取得
					i += 6;
					str = html_.GetAt(i);
					ParserUtil::UnEscapeHtmlElement(str);
					buf = _T("開催日時 ") + str;
					data_.AddBody(_T("\r\n"));
					data_.AddBody(buf);
					data_.AddBody(_T("\r\n"));

					// 開催場所の取得
					i += 6;
					str = html_.GetAt(i);
					ParserUtil::UnEscapeHtmlElement(str);
					buf = _T("開催場所 ") + str;
					data_.AddBody(buf);
					data_.AddBody(_T("\r\n"));

					i++;
					continue;
				}

				// フラグを発見するまで廻す
				if (util::LineHasStringsNoCase( str, L"詳細") ) {
					findFlag = TRUE;

					// </td></tr>を探す
					i++;
					str = html_.GetAt(i);

					data_.AddBody(_T("\r\n"));

					// [<td bgcolor=#ffffff><table BORDER=0 CELLSPACING=0 CELLPADDING=5><tr><td CLASS=h120 width="410">テスト用です</td></tr></table></td>]

					if( util::GetBeforeSubString( str, L"</td></tr>", buf ) > 0 ) {
						// 1行なので終了。
						// [<td bgcolor=#ffffff><table BORDER=0 CELLSPACING=0 CELLPADDING=5><tr><td CLASS=h120 width="410">テスト用です]
						ParserUtil::AddBodyWithExtract( data_, buf );

						// ここで終了
						endFlag = TRUE;
					}else{
						// 未終了なので行をそのまま投入。
						ParserUtil::AddBodyWithExtract( data_, str );
					}
					continue;
				}
			}
			else {

				if (endFlag == FALSE) {
					// </td></tr> が現れるまで解析＆追加。
					if( util::GetBeforeSubString( str, L"</td></tr>", buf) >= 0 ) {
						// 詳細、終了
						ParserUtil::AddBodyWithExtract( data_, buf );
						endFlag = TRUE;
					}else{
						// 未終了なので行をそのまま投入。
						ParserUtil::AddBodyWithExtract( data_, str );
					}
				}
				else {
					data_.SetDate(_T(""));

					// コメントの開始まで探す
					data_.ClearChildren();

					index = i;
					while( index<count ) {
						index = parseEventComment(index, count, &data_, html_);
						if (index == -1) {
							break;
						}
					}
					if (index == -1 || index >= count) {
						break;
					}
				}
			}
		}

		// ページ移動リンクの抽出
		parsePageLink( data_, html_ );

		// 「最新のトピック」の抽出
		parseRecentTopics( data_, html_ );

		MZ3LOGGER_DEBUG( L"ViewEventParser.parse() finished." );
		return true;
	}

	/**
	 * イベントコメント取得
	 */
	static int parseEventComment(int sIndex, int eIndex, CMixiData* data, const CHtmlArray& html_ )
	{
		int retIndex = eIndex;
		CMixiData cmtData;
		BOOL bCommentFound = FALSE;

		for (int i=sIndex; i<eIndex; i++) {
			const CString& line = html_.GetAt(i);

			if (bCommentFound == FALSE) {

				if( util::LineHasStringsNoCase( line, L"<!-- end : Loop -->" ) ||
					util::LineHasStringsNoCase( line, L"<!-- ADD_COMMENT: start -->" ) ||
					util::LineHasStringsNoCase( line, L"<!-- COMMENT: end -->" ) ||
					util::LineHasStringsNoCase( line, L"add_event_comment.pl" ) ) 
				{
					// コメント全体の終了タグ発見
					GetPostAddress(i, eIndex, html_, *data);
					return -1;
				}
				
				int index = 0;
				if ((index = line.Find(_T("show_friend.pl"))) != -1) {

					// 名前の取得
					CString buf = line.Mid(index);
					MixiUrlParser::GetAuthor(buf, &cmtData);

					// コメント番号を前の行から取得する
					// [<font color="#f8a448"><b>&nbsp;25</b>&nbsp;:</font>]
					const CString& lineB1 = html_.GetAt(i-1);

					if( util::GetBetweenSubString( lineB1, L"<b>", L"</b>", buf ) < 0 ) {
						MZ3LOGGER_ERROR( L"コメント番号が取得できません。mixi 仕様変更？" );
						return -1;
					}

					while(buf.Replace(_T("&nbsp;"), _T("")));
					cmtData.SetCommentIndex(_wtoi(buf));

					// 時刻の取得
					// 時刻は4〜6行前にある
					if (html_.GetAt(i-4).Find(_T("checkbox")) != -1) {
						// 5, 6 行前にある
						// 自分管理コミュ
						CString date = util::XmlParser::GetElement(html_.GetAt(i-6), 1) + _T(" ")
							+ util::XmlParser::GetElement(html_.GetAt(i-5), 1);
						ParserUtil::ChangeDate(date, &cmtData);
					}
					else {
						// 4, 5 行前にある
						CString date = util::XmlParser::GetElement(html_.GetAt(i-5), 1) + _T(" ")
							+ util::XmlParser::GetElement(html_.GetAt(i-4), 1);
						ParserUtil::ChangeDate(date, &cmtData);
					}

					bCommentFound = TRUE;
				}
			}
			else {

				// コメント本文取得
				if( util::LineHasStringsNoCase( line, L"<td", L"class=h120" ) ||
					util::LineHasStringsNoCase( line, L"<td", L"class=\"h120\"" ) )
				{
					cmtData.AddBody(_T("\r\n"));

					// [<tr><td class="h120" width="500">てすと</td></tr>]

					// 終了タグが現れるまで解析＆追加
					for( ;i<eIndex+1; i++ ) {
						const CString& line1 = html_.GetAt(i);

						// 解析＆登録
						ParserUtil::AddBodyWithExtract( cmtData, line1 );

						if( util::LineHasStringsNoCase( line1, L"</tr>" ) ||
							util::LineHasStringsNoCase( line1, L"</table>" ) )
						{
							if( util::LineHasStringsNoCase( line1, L"</tr></table>" ) ) {
								// "</tr></table>" は終了タグではなく画像終了タグなので無視。
							}else{
								// 終了タグ発見
								retIndex = i + 5;
								break;
							}
						}
					}
					break;
				}
			}
		}

		if( bCommentFound ) {
			data->AddChild(cmtData);
		}
		return retIndex;
	}
};



//■■■ニュース■■■
/**
 * [list] list_news_category.pl 用パーサ。
 * 【ニュースのカテゴリ】
 * http://news.mixi.jp/list_news_category.pl?id=pickup&type=bn
 */
class ListNewsCategoryParser : public MixiListParser
{
public:
	static bool parse( CMixiDataList& out_, const CHtmlArray& html_ )
	{
		MZ3LOGGER_DEBUG( L"ListNewsCategoryParser.parse() start." );

		INT_PTR count = html_.GetCount();

		// 「次」、「前」のリンク
		CMixiData backLink;
		CMixiData nextLink;

		/**
		 * 方針：
		 * - <td WIDTH=35 background=http://img.mixi.jp/img/bg_w.gif>...
		 *   が見つかればそこから項目開始とみなす
		 * - そこから+18行目以降に
		 *   <td ...><A HREF="view_news.pl?id=XXXXX&media_id=X"class="new_link">title</A>
		 *   という形式で「URL」と「タイトル」が存在する。
		 * - 次の4行後に
		 *   <td ...><A HREF="list_news_media.pl?id=2">提供会社</A></td>
		 *   という形式で「提供会社」が存在する。
		 * - 次の行に
		 *   <td WIDTH="1%" nowrap CLASS="f08">11月30日 20:36</td></tr>
		 *   という形式で「配信時刻」が存在する。
		 * - 項目が見つかって以降に、
		 *   </table>
		 *   があれば、処理を終了する
		 */

		// 項目開始を探す
		bool bInItems = false;	// 「注目のピックアップ」開始？
		int iLine = 200;		// とりあえず読み飛ばす
		for( ; iLine<count; iLine++ ) {
			const CString& str = html_.GetAt(iLine);

			// 「次」、「前」のリンク
			// 項目発見後にのみ存在する
			if( bInItems ) {
				// 「次を表示」、「前を表示」のリンクを抽出する
				if( parseNextBackLink( nextLink, backLink, str ) ) {
					continue;
				}
			}

			if( !bInItems ) {
				if( str.Find( L"<img" ) == -1 || str.Find( L"bg_w.gif" ) == -1 ) {
					// 開始フラグ未発見
					continue;
				}
				bInItems = true;
			}
			if( str.Find( L"<A" ) == -1 || str.Find( L"view_news.pl" ) == -1 ) {
				// 項目未発見
				continue;
			}
			// 項目発見。
			if( str.Find(_T("</table>")) != -1 ) {
				// 終了タグ発見
				break;
			}

			//--- URL と タイトルの抽出
			std::wstring url, title;
			{
				// 正規表現のコンパイル（一回のみ）
				static MyRegex reg;
				if( !reg.isCompiled() ) {
					if(! reg.compile( L"\"(view_news.pl\\?id=[0-9]+\\&media_id=[0-9]+).+>(.+)</" ) ) {
						MZ3LOGGER_FATAL( FAILED_TO_COMPILE_REGEX_MSG );
						return false;
					}
				}
				// 探索
				if( reg.exec(str) == false || reg.results.size() != 3 ) {
					// 未発見。
					continue;
				}
				// 発見。
				url   = reg.results[1].str;	// URL
				title = reg.results[2].str;	// title
			}

			//--- 提供会社の抽出
			// +1 〜 +4 行目の辺りにあるはず。
			std::wstring author;
			for( int iInc=1; iInc<=4; iInc++ ) {
				const CString& line2 = html_.GetAt( ++iLine );

				// 正規表現のコンパイル（一回のみ）
				static MyRegex reg;
				if( !reg.isCompiled() ) {
					if(! reg.compile( L"list_news_media.pl.+>([^<]+)</" ) ) {
						MZ3LOGGER_FATAL( FAILED_TO_COMPILE_REGEX_MSG );
						return false;
					}
				}
				// 探索
				if( reg.exec(line2) == false || reg.results.size() != 2 ) {
					// 未発見。
					continue;
				}
				// 発見。
				author = reg.results[1].str;	// 提供会社
				break;
			}
			if( author.empty() ) {
				// 未発見のため終了
				continue;
			}

			//--- 配信時刻の抽出
			iLine += 1;
			const CString& line3 = html_.GetAt(iLine);

			std::wstring date;
			{
				// 正規表現のコンパイル（一回のみ）
				static MyRegex reg;
				if( !reg.isCompiled() ) {
					if(! reg.compile( L"<td[^>]+>([^<]+)</" ) ) {
						MZ3LOGGER_FATAL( FAILED_TO_COMPILE_REGEX_MSG );
						return false;
					}
				}
				// 探索
				if( reg.exec(line3) == false || reg.results.size() != 2 ) {
					// 未発見。
					continue;
				}
				// 発見。
				date = reg.results[1].str;	// 配信時刻
			}


			// mixi データの作成
			{
				CMixiData data;
				data.SetAccessType( ACCESS_NEWS );

				// 日付
				ParserUtil::ChangeDate( date.c_str(), &data );
				TRACE(_T("%s\n"), data.GetDate());

				// 見出し
				data.SetTitle( title.c_str() );
				TRACE(_T("%s\n"), data.GetTitle());

				// URL 生成
				CString url2 = L"http://news.mixi.jp/";
				url2 += url.c_str();
				data.SetURL( url2 );
				TRACE(_T("%s\n"), data.GetURL());

				// 名前
				data.SetName( author.c_str() );
				data.SetAuthor( author.c_str() );
				TRACE(_T("%s\n"), data.GetName());

				out_.push_back(data);
			}
		}

		// 前、次のリンクがあれば、追加する。
		if( !backLink.GetTitle().IsEmpty() ) {
			out_.insert( out_.begin(), backLink );
		}
		if( !nextLink.GetTitle().IsEmpty() ) {
			out_.push_back( nextLink );
		}

		MZ3LOGGER_DEBUG( L"ListNewsCategoryParser.parse() finished." );
		return true;
	}

private:
	/// 「次を表示」、「前を表示」のリンクを抽出する
	static bool parseNextBackLink( CMixiData& nextLink, CMixiData& backLink, CString str )
	{
		// 正規表現のコンパイル（一回のみ）
		static MyRegex reg;
		if( !reg.isCompiled() ) {
			if(! reg.compile( L"<a href=list_news_category.pl([?][^>]+)>([^<]+)</a>" ) ) {
				MZ3LOGGER_FATAL( FAILED_TO_COMPILE_REGEX_MSG );
				return false;
			}
		}

		return parseNextBackLinkBase( nextLink, backLink, str, reg, 
					L"http://news.mixi.jp/list_news_category.pl", ACCESS_LIST_NEWS );
	}

};

/**
 * [content] view_news.pl 用パーサ
 * 【ニュース詳細】
 * http://mixi.jp/view_news.pl
 */
class ViewNewsParser : public MixiContentParser
{
public:
	static bool parse( CMixiData& data_, const CHtmlArray& html_ )
	{
		MZ3LOGGER_DEBUG( L"ViewNewsParser.parse() start." );

		data_.ClearAllList();

		INT_PTR count = html_.GetCount();

		/**
		 * 方針：
		 * - CLASS="h130"
		 *   が見つかれば、その行に
		 *   <td CLASS="h130">title</td>
		 *   という形式で「タイトル」が存在する。
		 * - <td CLASS="h150">
		 *   が見つかればそこから項目開始とみなす。
		 * - 以降、
		 *   </td></tr>
		 *   が見つかるまで、パースを続ける。
		 */
		// 項目開始を探す
		bool bInItems = false;
		int iLine = 200;		// とりあえず読み飛ばす
		for( ; iLine<count; iLine++ ) {
			const CString& line = html_.GetAt(iLine);

			if( !bInItems ) {
				if( !util::LineHasStringsNoCase( line, L"<td CLASS=\"h150\">" ) ) {
					// 開始フラグ未発見
					continue;
				}
				bInItems = true;
				
				// とりあえず改行出力
				data_.AddBody(_T("\r\n"));

				// この行は無視する。
				continue;
			}
			
			// 項目発見。
			// 行頭に </td></tr> があれば終了。
			LPCTSTR endTag = _T("</td></tr>");
			if( wcsncmp( line, endTag, wcslen(endTag) ) == 0 ) {
				// 終了タグ発見
				break;
			}

			CString str = line;

			ParserUtil::AddBodyWithExtract( data_, str );
		}

		MZ3LOGGER_DEBUG( L"ViewNewsParser.parse() finished." );
		return true;
	}

};




//■■■メッセージ■■■
/**
 * [list] list_message.pl 用パーサ
 * 【メッセージ一覧】
 * http://mixi.jp/list_message.pl
 */
class ListMessageParser : public MixiListParser
{
public:
	static bool parse( CMixiDataList& out_, const CHtmlArray& html_ )
	{
		MZ3LOGGER_DEBUG( L"ListMessageParser.parse() start." );

		// 「次」、「前」のリンク
		CMixiData backLink;
		CMixiData nextLink;

		INT_PTR count = html_.GetCount();

		bool bInMessages = false;

		for (int i=100; i<count; i++) {
			const CString& line = html_.GetAt(i);

			// <td><a HREF="view_message.pl?id=xxx&box=inbox">件名</a></td>

			if (util::LineHasStringsNoCase( line, _T("view_message.pl")) ) {
				bInMessages = true;

				CMixiData data;
				data.SetAccessType(ACCESS_MESSAGE);

				// リンクを取得
				CString buf;
				if( util::GetAfterSubString( line, L"<a", buf ) < 0 ) {
					continue;
				}
				// buf: HREF="view_message.pl?id=xxx&box=inbox">件名</a></td>

				CString link;
				if( util::GetBetweenSubString( buf, L"HREF=\"", L"\"", link ) < 0 ) {
					continue;
				}
				data.SetURL(link);

				// 見だし
				CString title;
				if( util::GetBetweenSubString( buf, L">", L"<", title ) < 0 ) {
					continue;
				}
				data.SetTitle(title);

				// 名前
				// 前の行から取得する
				const CString& line0 = html_.GetAt( i-1 );
				// line0: <td>なまえ</td>
				if( util::GetBetweenSubString( line0, L"<td>", L"</td>", buf ) < 0 ) {
					continue;
				}
				data.SetName(buf);
				data.SetAuthor(buf);

				// 次の行から、日付を取得
				const CString& line2 = html_.GetAt( i+1 );
				// line2: <td>04月08日</td></tr>
				if( util::GetBetweenSubString( line2, L"<td>", L"</td>", buf ) < 0 ) {
					continue;
				}
				data.SetDate( buf );

				out_.push_back( data );
			}

			if( bInMessages && util::LineHasStringsNoCase( line, L"</table>" ) ) {
				// 検索終了
				break;
			}
		}

		MZ3LOGGER_DEBUG( L"ListMessageParser.parse() finished." );
		return true;
	}
};




/**
 * [content] view_message.pl 用パーサ
 * 【メッセージ詳細】
 * http://mixi.jp/view_message.pl
 */
class ViewMessageParser : public MixiContentParser
{
public:
	static bool parse( CMixiData& data_, const CHtmlArray& html_ )
	{
		MZ3LOGGER_DEBUG( L"ViewMessageParser.parse() start." );

		data_.ClearAllList();

		INT_PTR count = html_.GetCount();


		for (int i=0; i<count; i++) {
			CString str = html_.GetAt(i);

			// 日付抽出
			if( util::LineHasStringsNoCase( str, L"<font", L" COLOR=#996600", L">", L"日　付" ) ) {
				CString buf = str.Mid(str.Find(_T("</font>")) + wcslen(_T("</font>")));
				while( buf.Replace(_T("&nbsp;"), _T(" ")) );
				buf.Replace(_T(":"), _T(""));
				buf.Replace(_T(" "), _T(""));
				buf.Replace(_T("時"), _T(":"));
				ParserUtil::ChangeDate(buf, &data_);
				continue;
			}

			// 差出人ID抽出
			if( util::LineHasStringsNoCase( str, L"<font", L" COLOR=#996600", L">", L"差出人", L"show_friend.pl?id=" ) ) {
				CString buf;
				util::GetBetweenSubString( str, L"show_friend.pl?id=", L"\"", buf );
				data_.SetOwnerID(_wtoi(buf));
				continue;
			}

			// 本文抽出
			if( util::LineHasStringsNoCase( str, L"<td", L"CLASS=", L"h120" ) ) {

				data_.AddBody(_T("\r\n"));
				ParserUtil::AddBodyWithExtract( data_, str );

				// 同じ行に </td></tr> が存在すれば終了。
				if( util::LineHasStringsNoCase( str, L"</td></tr>" ) ) {
					break;
				}

				for (int j=i+1; j<count; j++) {
					str = html_.GetAt(j);
					ParserUtil::AddBodyWithExtract( data_, str );

					if( util::LineHasStringsNoCase( str, L"</td></tr>" ) ) {
						// 終了フラグ発見
						break;
					}
				}
				break;

			}
		}

		MZ3LOGGER_DEBUG( L"ViewMessageParser.parse() finished." );
		return true;
	}
};


//■■■その他■■■
/**
 * [list] list_friend.pl 用パーサ。
 * 【マイミク一覧】
 * http://mixi.jp/list_friend.pl
 */
class ListFriendParser : public MixiListParser
{
public:
	static bool parse( CMixiDataList& out_, const CHtmlArray& html_ )
	{
		MZ3LOGGER_DEBUG( L"ListFriendParser.parse() start." );

		INT_PTR count = html_.GetCount();

		// 「次」、「前」のリンク
		CMixiData backLink;
		CMixiData nextLink;

		/**
		 * 方針：
		 * ★ table で5人ずつ並んでいる。
		 *   奇数行にリンク＆画像が、偶数行に名前が並んでいる。
		 *   従って、「5人分のリンク抽出」「5人分の名前抽出」「突き合わせ」「データ追加」という手順で行う。
		 *
		 * ● <table ... CELLPADDING=2 ...>
		 *    が見つかれば、そこから項目開始とみなす。</table> が現れるまで以下を実行する。
		 *   (1) </tr> が見つかるまでの各行をパースし、所定の形式に一致していれば、URL と時刻を取得する。
		 *   (2) 次の </tr> が見つかるまでの各行をパースし、所定の形式に一致していれば、名前を抽出する。
		 *   (3) (1), (2) で抽出したデータを out_ に追加する。
		 */

		// 項目開始を探す
		bool bInItems = false;	// 項目開始？
		int iLine = 100;		// とりあえず読み飛ばす
		for( ; iLine<count; iLine++ ) {
			const CString& line = html_.GetAt(iLine);

			if( !bInItems ) {
				// 項目開始？
				if( util::LineHasStringsNoCase( line, L"<table", L"cellpadding=\"2\"", L">" ) ) {
					bInItems = true;
				}
			}

			if( bInItems ) {
				// </table> が見つかれば終了
				if( util::LineHasStringsNoCase( line, L"</table>" ) ) {
					break;
				}

				// </table> が見つからなかったので5人分取得＆追加
				if( parseTwoTR( out_, html_, iLine ) ) {
				}else{
					// 解析エラー
					break;
				}
			}
		}

		// </table> が見つかったので、その後の行から次、前のリンクを抽出
		for( ; iLine<count; iLine++ ) {
			const CString& line = html_.GetAt(iLine);

			// 「次を表示」、「前を表示」のリンクを抽出する
			if( parseNextBackLink( nextLink, backLink, line ) ) {
				// 抽出できたら終了タグ
				break;
			}
		}

		// 前、次のリンクがあれば、追加する。
		if( !backLink.GetTitle().IsEmpty() ) {
			out_.insert( out_.begin(), backLink );
		}
		if( !nextLink.GetTitle().IsEmpty() ) {
			out_.push_back( nextLink );
		}

		MZ3LOGGER_DEBUG( L"ListFriendParser.parse() finished." );
		return true;
	}

private:
	/**
	 * 5人分のユーザの内容を抽出する
	 *
	 * (1) </tr> が現れるまでの各行をパースし、5人分のURL、時刻を生成する。
	 * (2) </tr> が現れるまでの各行をパースし、5人分の名前を生成する。
	 * (3) mixi_list に追加する。
	 */
	static bool parseTwoTR( CMixiDataList& mixi_list, const CHtmlArray& html, int& iLine )
	{
		const int lastLine = html.GetCount();

		// 一時的な CMixiData のリスト
		CMixiDataList tmp_list;

		// 1つ目の </tr> までの解析
		bool bBreak = false;
		for( ; iLine < lastLine && bBreak == false; iLine++ ) {
			const CString& line = html.GetAt( iLine );
			if( util::LineHasStringsNoCase( line, L"</tr>" ) ) {
				// </tr> 発見、名前抽出に移る。
				bBreak = true;
			}
			if( util::LineHasStringsNoCase( line, L"</table>" ) ) {
				// </table> 発見、終了
				return false;
			}

			// <td で始まるなら、抽出する
			if( line.Left( 3 ).CompareNoCase( L"<td" ) == 0 ) {
				/* 行の形式：
line1 : <td width="20%" height="100" background="http://img.mixi.jp/img/bg_orange2-.gif">
line2 : <a href="show_friend.pl?id=xxx"><img src="http://img.mixi.jp/photo/member/xxx.jpg" alt=""  border="0" /></a></td>
*/
				CMixiData mixi;

			// 時刻判定
				{
					// "bg_orange1-.gif" があれば、「1時間以内」
					// "bg_orange2-.gif" があれば、「1日以内」
					// いずれもなければ、「1日以上」
					if( util::LineHasStringsNoCase( line, L"bg_orange1-.gif" ) ) {
						mixi.SetDate( L"1時間以内" );
					}else if( util::LineHasStringsNoCase( line, L"bg_orange2-.gif" ) ) {
						mixi.SetDate( L"1日以内" );
					}else{
						mixi.SetDate( L"-" );
					}
				}

				// line2 のフェッチ
				const CString& line2 = html.GetAt( ++iLine );
				if( line2.Find( L"</tr>" ) != -1 ) {
					// </tr> 発見、名前抽出に移る。
					bBreak = true;
				}
				if( util::LineHasStringsNoCase( line2, L"</table>" ) ) {
					// </table> 発見、終了
					return false;
				}

				// <a 以降のみにする
				CString target;
				if( util::GetAfterSubString( line2, L"<a", target ) < 0 ) {
					// <a がなかったので次の行解析へ。
					continue;
				}
				// target:  href="show_friend.pl?id=xxx"><img ...
				// URL 抽出
				CString url;
				if( util::GetBetweenSubString( target, L"href=\"", L"\"", url ) < 0 ) {
					continue;
				}
				mixi.SetURL( url );

				// URL 構築＆設定
				url.Insert( 0, L"http://mixi.jp/" );
				mixi.SetBrowseUri( url );

				// Image 抽出
				if( util::GetAfterSubString( target, L"<img", target ) < 0 ) {
					continue;
				}
				// target:  src="http://img.mixi.jp/photo/member/xxx.jpg" alt=""  border="0" /></a></td>
				CString image_url;
				if( util::GetBetweenSubString( target, L"src=\"", L"\"", image_url ) < 0 ) {
					continue;
				}
				mixi.AddImage( image_url );

				// 解析成功なので追加する。
				tmp_list.push_back( mixi );
			}
		}
		if( iLine >= lastLine ) {
			return false;
		}

		// 2つ目の </tr> までの解析
		int mixiIndex = 0;		// 何番目の項目を解析しているかを表すインデックス
		for( ; iLine < lastLine; iLine++ ) {
			const CString& line = html.GetAt( iLine );
			if( util::LineHasStringsNoCase( line, L"</tr>" ) ) {
				// </tr> 発見、終了
				break;
			}
			if( util::LineHasStringsNoCase( line, L"</table>" ) ) {
				// </table> 発見、終了
				return false;
			}

			if( mixiIndex >= (int)tmp_list.size() ) {
				// リスト数以上見つかったので、終了。
				break;
			}

			// <td があるなら、抽出する
			if( util::LineHasStringsNoCase( line, L"<td" ) ) {
				CMixiData& mixi = tmp_list[mixiIndex];
				mixiIndex ++;

				// 行の形式：
				// <tr align="center" bgcolor="#FFF4E0"><td valign="top">xxxさん(xx)
				// または
				// </td><td valign="top">xxxさん(xx)

				// 名前抽出
				CString name = line;

				// 全てのタグを除去
				ParserUtil::StripAllTags( name );

				// 末尾の \n を削除
				name.Replace( L"\n", L"" );

				mixi.SetName( name );
				mixi.SetAccessType( ACCESS_PROFILE );

				// mixi_list に追加する。
				mixi_list.push_back( mixi );
			}
		}
		if( iLine >= lastLine ) {
			return false;
		}


		return true;
	}

	/// 「次を表示」、「前を表示」のリンクを抽出する
	/// <td align="right" bgcolor="#EED6B5">1件〜50件を表示&nbsp;&nbsp;<a href=list_friend.pl?page=2&id=xxx>次を表示</a></td></tr>
	static bool parseNextBackLink( CMixiData& nextLink, CMixiData& backLink, CString str )
	{
		// 正規表現のコンパイル（一回のみ）
		static MyRegex reg;
		if( !reg.isCompiled() ) {
			if(! reg.compile( L"<a href=list_friend.pl([?][^>]+)>([^<]+)</a>" ) ) {
				MZ3LOGGER_FATAL( FAILED_TO_COMPILE_REGEX_MSG );
				return false;
			}
		}

		return parseNextBackLinkBase( nextLink, backLink, str, reg, L"list_friend.pl", ACCESS_LIST_FRIEND );
	}
};


/**
 * [list] show_intro.pl 用パーサ。
 * 【紹介文】
 * http://mixi.jp/show_intro.pl
 */
class ShowIntroParser : public MixiListParser
{
public:
	static bool parse( CMixiDataList& out_, const CHtmlArray& html_ )
	{
		MZ3LOGGER_DEBUG( L"ShowIntroParser.parse() start." );

		INT_PTR count = html_.GetCount();

		// 「次」、「前」のリンク
		CMixiData backLink;
		CMixiData nextLink;

		/**
		 * 方針：
		 * - bg_line.gif が見つかれば、そこから項目開始とみなす。
		 * - </tr> が現れるまでをパースし、項目を生成する。
		 * - </td></tr></table> が現れたら終了とする。
		 */

		// 項目開始を探す
		bool bInItems = false;	// 項目開始？
		int iLine = 100;		// とりあえず読み飛ばす
		for( ; iLine<count; iLine++ ) {
			const CString& str = html_.GetAt(iLine);

			// 「次」、「前」のリンク
			// 項目発見後にのみ存在する
			if( bInItems ) {
				// 「次を表示」、「前を表示」のリンクを抽出する
				if( parseNextBackLink( nextLink, backLink, str ) ) {
					// 抽出できたら終了タグとみなす。
					break;
				}
			}

			// 項目？
			if( str.Find( L"<td" ) != -1 && str.Find( L"bg_line.gif" ) != -1 ) {
				bInItems = true;

				// 解析
				CMixiData mixi;
				if( parseOneIntro( mixi, html_, iLine ) ) {
					out_.push_back( mixi );
				}
			}

			if( bInItems ) {
				// </td></tr></table> が見つかれば終了
				if( str.Find( L"</td></tr></table>" ) != -1 ) {
					break;
				}
			}
		}

		// 前、次のリンクがあれば、追加する。
		if( !backLink.GetTitle().IsEmpty() ) {
			out_.insert( out_.begin(), backLink );
		}
		if( !nextLink.GetTitle().IsEmpty() ) {
			out_.push_back( nextLink );
		}

		MZ3LOGGER_DEBUG( L"ShowIntroParser.parse() finished." );
		return true;
	}

private:
	/**
	 * １つの紹介文を抽出する。
	 * iLine は bg_line.gif が現れた行。
	 *
	 * (1) iLine からURL、名前を抽出する。
	 * (2) <td WIDTH=480>という行を探す。その次の行に下記の形式で「関係」が存在する。
     *     関係：かんけい<br>
	 * (3) 下記の形式で紹介文があるので、AddBodyする。
	 *     <div style="width:286px; margin:0; padding:0;">ほんぶん</div>
	 * (4) </tr> が見つかれば終了する。
	 */
	static bool parseOneIntro( CMixiData& mixi, const CHtmlArray& html, int& iLine )
	{
		const int lastLine = html.GetCount();

		const CString& line = html.GetAt( iLine );

		// 現在の行(1)と次の行(2)に、
		// 下記の形式で「URL」「画像URL」「名前」が存在するので、抽出する。
		// (1)：<td WIDTH=150 background=http://img.mixi.jp/img/bg_line.gif align="center">
		// (1)：<a href="show_friend.pl?id=xxx"><img src="http://img.mixi.jp/photo/member/xxx.jpg" border=0><br>
		// (2)：なまえ</td></a>

		// URL 抽出
		CString target;
		if( util::GetAfterSubString( line, L"<a ", target ) < 0 ) {
			return false;
		}
		// target: href="show_friend.pl?id=xxx"><img src="http://img.mixi.jp/photo/member/xxx.jpg" border=0><br>
		CString url;
		if( util::GetBetweenSubString( target, L"href=\"", L"\"", url ) < 0 ) {
			return false;
		}
		mixi.SetURL( url );

		// URL 構築＆設定
		url.Insert( 0, L"http://mixi.jp/" );
		mixi.SetBrowseUri( url );

		// 画像URL抽出
		if( util::GetAfterSubString( target, L"<img ", target ) < 0 ) {
			return false;
		}
		// target: src="http://img.mixi.jp/photo/member/xxx.jpg" border=0><br>
		CString image_url;
		if( util::GetBetweenSubString( target, L"src=\"", L"\"", image_url ) < 0 ) {
			return false;
		}
		mixi.AddImage( image_url );

		// 次の行から名前抽出
		const CString& line2 = html.GetAt( ++iLine );
		// </td> の前までを名前とする。
		CString name;
		if( util::GetBeforeSubString( line2, L"</td>", name ) < 0 ) {
			return false;
		}
		mixi.SetName( name );

		mixi.SetAccessType( ACCESS_PROFILE );

		// 関係：の抽出
		for( ++iLine; iLine+1<lastLine; iLine++ ) {
			const CString& line = html.GetAt( iLine );
			if( util::LineHasStringsNoCase( line, L"<td WIDTH=480>" ) ) {
				// 関係：かんけい<br>
				CString line1 = html.GetAt( ++iLine );

				if( line1.Find( L"関係：" ) == -1 ) {
					break;
				}

				// 半角スペース " " を追加しておく
				line1.Replace( L"<br>", L" <br>" );

				ParserUtil::AddBodyWithExtract( mixi, line1 );
				break;
			}
		}


		// </tr> が見つかるまで、紹介文を探す
		bool bInIntro = false;
		for( ++iLine; iLine< lastLine; iLine++ ) {
			const CString& line = html.GetAt( iLine );

			if( util::LineHasStringsNoCase( line, L"</tr>" ) ) {
				break;
			}
			if( !bInIntro ) {
				// <div style="width:286px; margin:0; padding:0;">ほんぶん</div>
				if( util::LineHasStringsNoCase( line, L"<div" ) ) {
					// この行以降に紹介文がある。
					bInIntro = true;

					// <div ... > を削除
					CString result;
					if( util::GetAfterSubString( line, L">", result ) < 0 ) {
						continue;
					}
					// </div> があれば削除＆紹介文終了
					if( util::GetBeforeSubString( result, L"</div>", result ) >= 0 ) {
						// 紹介文追加。
						mixi.AddBody( result );
						bInIntro = false;
					}else{
						// 紹介文追加。
						mixi.AddBody( result );
					}
				}
			}else{
				// </div> があればその前までを追加し、終了。
				// </div> がなければ、追加し、継続。
				if( util::LineHasStringsNoCase( line, L"</div>" ) ) {
					CString result;
					util::GetBeforeSubString( line, L"</div>", result );

					ParserUtil::AddBodyWithExtract( mixi, result );
					bInIntro = false;
				}else{
					ParserUtil::AddBodyWithExtract( mixi, line );
				}
			}
		}
		if( iLine >= lastLine ) {
			return false;
		}

		return true;
	}

	/// 「次を表示」、「前を表示」のリンクを抽出する
	/// <td align=right>1件〜50件を表示&nbsp;&nbsp;<a href=show_intro.pl?page=2&id=xxx>次を表示</a></td>
	static bool parseNextBackLink( CMixiData& nextLink, CMixiData& backLink, CString str )
	{
		// 正規表現のコンパイル（一回のみ）
		static MyRegex reg;
		if( !reg.isCompiled() ) {
			if(! reg.compile( L"<a href=show_intro.pl([?][^>]+)>([^<]+)</a>" ) ) {
				MZ3LOGGER_FATAL( FAILED_TO_COMPILE_REGEX_MSG );
				return false;
			}
		}

		return parseNextBackLinkBase( nextLink, backLink, str, reg, L"show_intro.pl", ACCESS_LIST_INTRO );
	}
};





/**
 * [list] show_log.pl 用パーサ
 * 【足あと】
 * http://mixi.jp/show_log.pl
 */
class ShowLogParser : public MixiListParser
{
public:
	static bool parse( CMixiDataList& out_, const CHtmlArray& html_ )
	{
		MZ3LOGGER_DEBUG( L"ShowLogParser.parse() start." );

		INT_PTR count = html_.GetCount();

		BOOL findFlag = FALSE;

		for (int i=0; i<count; i++) {

			CString str = html_.GetAt(i);

			if (findFlag != FALSE && str.Find(_T("</td></tr>")) != -1) {
				// 終了
				break;
			}

			if (str.Find(_T("show_friend.pl")) != -1) {
				findFlag = TRUE;

/* 解析対象文字列
<li style="padding: 0; margin: 0; list-style: none;">
2006年11月21日 18:10 <a href="show_friend.pl?id=ZZZZZ">XXXXX</a></li>
*/

				CString target = str;

				// URL を抽出
				// <a href="〜"> で解析
				CString url;
				int pos = util::GetBetweenSubString( target, L"<a href=\"", L"\">", url );
				if( pos < 0 ) 
					continue;

				// 日付を抽出
				// ">〜<a で解析
				target = str.Left( pos );
				CString strDate;
				pos = util::GetBetweenSubString( target, L"\">", L"<a", strDate );
				if( pos < 0 ) 
					continue;

				strDate.Trim();

				// 名前
				// ">〜</a で解析
				// 解析対象は、日付の終わり以降。
				target = str.Mid( pos );
				CString name;
				pos = util::GetBetweenSubString( target, L">", L"</a>", name );

				// "show_friend_memo.pl" と "http://img.mixi.jp/img/memo_s.gif" があればマイミクとみなす
				bool bMyMixi = false;
				if( util::LineHasStringsNoCase( target, L"show_friend_memo.pl", L"http://img.mixi.jp/img/memo_s.gif" ) ) {
					// マイミクとみなす
					bMyMixi = true;
				}

				// オブジェクト生成
				CMixiData data;
				data.SetAccessType( ACCESS_PROFILE );
				data.SetName( name );
				data.SetURL( url );
				data.SetBrowseUri( L"http://mixi.jp/" + url );
				ParserUtil::ChangeDate( strDate, &data );
				data.SetMyMixi( bMyMixi );

				out_.push_back( data );

			}
		}

		MZ3LOGGER_DEBUG( L"ShowLogParser.parse() finished." );
		return true;
	}
};






/**
 * [list] list_bookmark.pl 用パーサ。
 * 【お気に入り】
 * http://mixi.jp/list_bookmark.pl
 */
class ListBookmarkParser : public MixiListParser
{
public:
	static bool parse( CMixiDataList& out_, const CHtmlArray& html_ )
	{
		MZ3LOGGER_DEBUG( L"ListBookmarkParser.parse() start." );

		INT_PTR count = html_.GetCount();

		// 「次」、「前」のリンク
		CMixiData backLink;
		CMixiData nextLink;

		/**
		 * 方針：
		 * - <td ... background=http://img.mixi.jp/img/bg_line.gif>
		 *   が見つかればそこから項目開始とみなす
		 * - 1人分の項目を抽出する。（詳細は parseOneUser 参照）
		 * - 以降、上記を繰り返す。
		 * - 最初の項目が見つかった以降に、
		 *   <td ALIGN=right BGCOLOR=#EED6B5>1件〜30件を表示&nbsp;&nbsp;<a href=list_bookmark.pl?page=2>次を表示</a></td></tr>
		 *   という形式で、「次を表示」「前を表示」が存在する。
		 */

		// 項目開始を探す
		bool bInItems = false;	// 項目開始？
		int iLine = 150;		// とりあえず読み飛ばす
		for( ; iLine<count; iLine++ ) {
			const CString& str = html_.GetAt(iLine);

			// 「次」、「前」のリンク
			// 項目発見後にのみ存在する
			if( bInItems ) {
				// 「次を表示」、「前を表示」のリンクを抽出する
				if( parseNextBackLink( nextLink, backLink, str ) ) {
					// 抽出できたら終了タグとみなす。
					break;
				}
			}

			if( str.Find( L"<td" ) != -1 && str.Find( L"bg_line.gif" ) != -1 ) {
				// 項目フラグ発見
				CMixiData mixi;
				if( parseOneItem( mixi, html_, iLine ) ) {
					out_.push_back( mixi );
				}
				bInItems = true;
			}
		}

		// 前、次のリンクがあれば、追加する。
		if( !backLink.GetTitle().IsEmpty() ) {
			out_.insert( out_.begin(), backLink );
		}
		if( !nextLink.GetTitle().IsEmpty() ) {
			out_.push_back( nextLink );
		}

		MZ3LOGGER_DEBUG( L"ListBookmarkParser.parse() finished." );
		return true;
	}

private:
	/**
	 * 1人分のユーザの内容を抽出する
	 *
	 * - 次の行に
	 *   <a href="show_friend.pl?id=xxxxx">
	 *   という形式で「URL」が存在する。
	 * - そこから+3行目に
	 *   <td ><a ...>なまえ</a></td>
	 *   という形式で「名前」が存在する。
	 * - 次の行以降に
	 *   <td COLSPAN=2 BGCOLOR=#FFFFFF>...
	 *   という形式で「自己紹介」が存在する。
	 * - その行以降に
	 *   ...</td></tr>
	 *   があれば「自己紹介」終了。
	 * - 次の行以降に
	 *   <td BGCOLOR=#FFFFFF WIDTH=140>3日以上</td>
	 *   という形式で「最終ログイン」が存在する。
	 */
	static bool parseOneItem( CMixiData& mixi, const CHtmlArray& html, int& iLine )
	{
		const int lastLine = html.GetCount();

		// URL 抽出
		// bg_line.gif のある行から N 行以内に発見されなければエラーとして終了する。
		CString url;
		int endLine = iLine+3;
		iLine ++;
		for( ; iLine<endLine; iLine ++ ) {
			const CString line = html.GetAt( iLine );

			if( util::GetBetweenSubString( line, L"<a href=\"", L"\">", url ) >= 0 ) {
				// 発見
				mixi.SetURL( url );
				// URL 構築＆設定
				url.Insert( 0, L"http://mixi.jp/" );
				mixi.SetBrowseUri( url );
				break;
			}
		}
		if( iLine >= endLine ) {
			MZ3LOGGER_ERROR( L"<a href=\"...\"> が現れませんでした。 iLine[" + util::int2str(iLine) + L"]" );
			return false;
		}

		// 名前抽出
		{
			iLine += 2;
			for( ; iLine<lastLine; iLine++ ) {
				const CString line = html.GetAt( iLine );

				CString buf;
				// <td colspan="2" bgcolor="#FFFFFF"><a href="show_friend.pl?id=xxx">なまえ</a></td>
				// または
				// <a href="view_community.pl?id=xxx">コミュニティ名</a>
				// から名称を抽出する
				if( util::GetBetweenSubString( line, L"<a href=\"", L"</a>", buf ) > 0 ) {
					// buf : view_community.pl?id=xxx">なまえ
					// buf : show_friend.pl?id=xxx">コミュニティ名
					// 名前抽出
					CString name;
					if( util::GetAfterSubString( buf, L">", name ) > 0 ) {
						// 名前設定
						mixi.SetName( name );
						break;
					}
				}
			}
			if( iLine >= lastLine ) {
				MZ3LOGGER_ERROR( L"<a href=\"...\"> が現れませんでした。 iLine[" + util::int2str(iLine) + L"]" );
				return false;
			}
		}

		// 自己紹介抽出
		{
			iLine += 1;
			bool bInIntroduce = false;
			for( ; iLine<lastLine; iLine++ ) {
				const CString line = html.GetAt( iLine );

				if( !bInIntroduce ) {
					// 開始タグを探す
					CString after;
					if( util::GetAfterSubString( 
							line,
							L"<td colspan=\"2\" bgcolor=\"#FFFFFF\"", after ) < 0 )
					{
						continue;
					}
					// 発見。
					bInIntroduce = true;
					continue;
				}

				// 自己紹介終了？
				CString left;
				if( util::GetBeforeSubString( line, L"</td>", left ) >= 0 ) {
					// 自己紹介終了。
					// その左側を取得し、本文に追加し、ループ終了
					ParserUtil::AddBodyWithExtract( mixi, left );
					break;
				}else{
					// 自己紹介継続。
					// 本文にそのまま追加。
					ParserUtil::AddBodyWithExtract( mixi, line );
				}
			}
		}

		// 最終ログイン抽出
		{
			iLine += 1;
			for( ; iLine+1<lastLine; iLine++ ) {
				const CString line = html.GetAt( iLine );
				
				if( util::LineHasStringsNoCase( line, L"<td", L"bgcolor=\"#FFFFFF\"", L"width=\"250\"", L"align=\"left\">" ) ) {
					// 次の行が最終ログイン時刻文字列
					CString date = html.GetAt( iLine+1 );

					// 最終ログイン発見。
					date.Replace(_T("\n"), _T(""));
					mixi.SetDate( date );
					break;
				}
			}
		}

		// URL に応じてアクセス種別を設定する
		mixi.SetAccessType( util::EstimateAccessTypeByUrl( url ) );

		return true;
	}

	/// 「次を表示」、「前を表示」のリンクを抽出する
	static bool parseNextBackLink( CMixiData& nextLink, CMixiData& backLink, CString str )
	{
		// 正規表現のコンパイル（一回のみ）
		static MyRegex reg;
		if( !reg.isCompiled() ) {
			if(! reg.compile( L"<a href=list_bookmark.pl([?][^>]+)>([^<]+)</a>" ) ) {
				MZ3LOGGER_FATAL( FAILED_TO_COMPILE_REGEX_MSG );
				return false;
			}
		}

		return parseNextBackLinkBase( nextLink, backLink, str,
			reg, L"list_bookmark.pl", ACCESS_LIST_FAVORITE );
	}
};


//■■■MZ3独自■■■
/**
 * [content] Readme.txt 用パーサ
 * 【MZ3ヘルプ用】
 */
class HelpParser : public MixiContentParser
{
public:
	static bool parse( CMixiData& mixi, const CHtmlArray& html_ )
	{
		MZ3LOGGER_DEBUG( L"HelpParser.parse() start." );

		mixi.ClearAllList();

		INT_PTR count = html_.GetCount();

		int iLine = 0;

		int status = 0;		// 0 : start, 1 : 最初の項目, 2 : 2番目以降の項目解析中
		CMixiData child;

		for( ; iLine<count; iLine++ ) {
			const CString& line = html_.GetAt(iLine);

			CString head2 = line.Left(2);

			// - "--" で始まる行は無視する。
			if( head2 == "--" ) {
				continue;
			}

			// - "* " で始まる行があれば、ステータスに応じて処理を行う。
			if( head2 == L"* " ) {
				// 3 文字目以降を Author として登録する。
				// これにより、章目次としてレポート画面に表示される。
				CString chapter = line.Mid(2);
				chapter.Replace( L"\n", L"" );
				chapter.Replace( L"\t", L"    " );
				switch( status ) {
				case 0:
					// 最初の項目が見つかったので、トップ要素として追加する。
					mixi.SetAuthor( chapter );
					status = 1;
					break;
				case 1:
					// 2番目以降の項目が見つかったので、最初の子要素として追加する。
					child.SetAuthor( chapter );
					status = 2;
					break;

				case 2:
					// 3番目以降の項目が見つかったので、追加し、子要素を初期化する。
					child.SetCommentIndex( mixi.GetChildrenSize()+1 );
					mixi.AddChild( child );

					child.ClearBody();
					child.SetAuthor( chapter );
					break;
				}
				continue;
			}

			// - "** " で始まる行があれば、節項目として解析、追加する。
			if( line.Left(3) == L"** " ) {
				// 4 文字目以降を Author として登録する。
				// これにより、節目次としてレポート画面に表示される。
				CString section = line.Mid(3);
				section.Replace( L"\n", L"" );
				section.Replace( L"\t", L"    " );

				// 節名称の先頭に識別子を追加する。
				section.Insert( 0, L" " );

				switch( status ) {
				case 0:
					// 最初の項目としての節項目はありえないので無視する。
					break;
				case 1:
					// 1番目の解析中に見つかったので、最初の子要素として追加する。
					child.SetAuthor( section );
					status = 2;
					break;

				case 2:
					// 2番目以降の解析中に見つかったので、追加し、子要素を初期化する。
					child.SetCommentIndex( mixi.GetChildrenSize()+1 );
					mixi.AddChild( child );

					child.ClearBody();
					child.SetAuthor( section );
					break;
				}
				continue;
			}

			switch( status ) {
			case 1:
			case 2:
				{
					// 要素継続
					CString str = line;
					str.Replace( L"\n", L"" );
					str.Replace( L"\t", L"    " );

					// 要素追加
					if( status == 1 ) {
						mixi.AddBody( L"\r\n" + str );
					}else{
						child.AddBody( L"\r\n" + str );
					}
				}
				break;
			}
		}
		if( status == 2 ) {
			if( child.GetBodySize()>0 ) {
				child.SetCommentIndex( mixi.GetChildrenSize()+1 );
				mixi.AddChild( child );
			}
		}

		MZ3LOGGER_DEBUG( L"HelpParser.parse() finished." );
		return true;
	}

};

/**
 * [content] mz3log.txt 用パーサ
 * 【MZ3ヘルプ用】
 */
class ErrorlogParser : public MixiContentParser
{
public:
	static bool parse( CMixiData& mixi, const CHtmlArray& html_ )
	{
		MZ3LOGGER_DEBUG( L"ErrorlogParser.parse() start." );

		mixi.ClearAllList();
		INT_PTR count = html_.GetCount();

		int iLine = 0;

		int status = 0;		// 0 : start, 1 : 最初の項目, 2 : 2番目以降の項目解析中
		CMixiData child;

		mixi.SetAuthor( L"MZ3" );
		mixi.AddBody( L"the top element.\r\n" );

		CString msg;
		msg.Format( L"mz3log.txt has %d line(s).", count );
		mixi.AddBody( msg );

		// 【解析対象概要】
		// ---
		// [2007/07/13△12:30:03]△DEBUG△本文△[.\InetAccess.cpp:555]
		// ---
		// または
		// ---
		// [2007/07/13△12:06:33]△DEBUG△本文
		// ずっと本文
		// ずっと本文△[.\MZ3View.cpp:947]
		// ---
		// という形式。
		for( int iLine=0; iLine<count; iLine++ ) {
			CString target = html_.GetAt(iLine);
			target.Replace(_T("\n"), _T("\r\n"));	// 改行コード変換

			child.ClearBody();
			child.SetCommentIndex( iLine+1 );

			// "[〜]" を日付に。
			CString date;
			int index = 0;
			index = util::GetBetweenSubString( target, L"[", L"]", date );
			if( index == -1 ) {
				child.AddBody( L"★ '[〜]' が見つからないのでスキップ [" + target + L"]" );
				mixi.AddChild( child );
				continue;
			}
			child.SetDate( date );

			// "]" 以降を切り出し
			target = target.Mid( index );

			// "△〜△" をエラーレベル（名前）に。
			CString level;
			index = util::GetBetweenSubString( target, L" ", L" ", level );
			if( index == -1 ) {
				// "△〜△" が見つからないのでスキップ
				child.AddBody( L"★ '△〜△' が見つからないのでスキップ [" + target + L"]" );
				mixi.AddChild( child );
				continue;
			}

			// 2つ目の"△"以降を切り出し
			target = target.Mid( index );

			// 名前に「レベル△本文」を設定
			level += L" ";
			level += target;
			level = level.Left( 30 );
			child.SetAuthor( level );

			// 末尾が "]" なら本文に追加して終了。
			// 末尾が "]" 以外なら次の行以降を見つかるまで本文として解析。
			child.AddBody( L"\r\n" );
			child.AddBody( target );

			if( target.Right( 3 ) == L"]\r\n" ) {
				// 終了
			}else{
				// 末尾に "]" が現れるまで解析して終了。
				iLine ++;
				for( ;iLine<count; iLine++ ) {
					target = html_.GetAt( iLine );
					target.Replace(_T("\n"), _T("\r\n"));	// 改行コード変換
					child.AddBody( target );
					if( target.Right( 3 ) == L"]\r\n" ) {
						break;
					}
				}
			}
			mixi.AddChild( child );
		}

		MZ3LOGGER_DEBUG( L"ErrorlogParser.parse() finished." );
		return true;
	}

};


/**
 * 画像ダウンロードCGI 用パーサ
 *
 * show_diary_picture.pl
 * show_bbs_comment_picture.pl
 */
class ShowPictureParser : public MixiParserBase
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

/// リスト系HTMLの解析
inline void MyDoParseMixiHtml( ACCESS_TYPE aType, CMixiDataList& body, CHtmlArray& html )
{
	switch (aType) {
	case ACCESS_LIST_DIARY:			mixi::ListNewFriendDiaryParser::parse( body, html );	break;
	case ACCESS_LIST_NEW_COMMENT:	mixi::NewCommentParser::parse( body, html );			break;
	case ACCESS_LIST_COMMENT:		mixi::ListCommentParser::parse( body, html );			break;
	case ACCESS_LIST_NEW_BBS:		mixi::NewBbsParser::parse( body, html );				break;
	case ACCESS_LIST_MYDIARY:		mixi::ListDiaryParser::parse( body, html );				break;
	case ACCESS_LIST_FOOTSTEP:		mixi::ShowLogParser::parse( body, html );				break;
	case ACCESS_LIST_MESSAGE_IN:	mixi::ListMessageParser::parse( body, html );			break;
	case ACCESS_LIST_MESSAGE_OUT:	mixi::ListMessageParser::parse( body, html );			break;
	case ACCESS_LIST_NEWS:			mixi::ListNewsCategoryParser::parse( body, html );		break;
	case ACCESS_LIST_FAVORITE:		mixi::ListBookmarkParser::parse( body, html );			break;
	case ACCESS_LIST_FRIEND:		mixi::ListFriendParser::parse( body, html );			break;
	case ACCESS_LIST_COMMUNITY:		mixi::ListCommunityParser::parse( body, html );			break;
	case ACCESS_LIST_INTRO:			mixi::ShowIntroParser::parse( body, html );				break;
	case ACCESS_LIST_BBS:			mixi::ListBbsParser::parse( body, html );				break;
	}
}

/// View系HTMLの解析
inline void MyDoParseMixiHtml( ACCESS_TYPE aType, CMixiData& mixi, CHtmlArray& html )
{
	switch (aType) {
	case ACCESS_DIARY:		mixi::ViewDiaryParser::parse( mixi, html );		break;
	case ACCESS_BBS:		mixi::ViewBbsParser::parse( mixi, html );		break;
	case ACCESS_ENQUETE:	mixi::ViewEnqueteParser::parse( mixi, html );	break;
	case ACCESS_EVENT:		mixi::ViewEventParser::parse( mixi, html );		break;
	case ACCESS_MYDIARY:	mixi::ViewDiaryParser::parse( mixi, html );		break;
	case ACCESS_MESSAGE:	mixi::ViewMessageParser::parse( mixi, html );	break;
	case ACCESS_NEWS:		mixi::ViewNewsParser::parse( mixi, html );		break;
	case ACCESS_HELP:		mixi::HelpParser::parse( mixi, html );			break;
	case ACCESS_ERRORLOG:	mixi::ErrorlogParser::parse( mixi, html );		break;
	}
}

}//namespace mixi

