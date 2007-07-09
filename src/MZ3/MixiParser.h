#pragma once

#include "MyRegex.h"
#include "XmlParser.h"
#include "HtmlArray.h"

/// mixi 用HTMLパーサ
namespace mixi {

/// mixi の URL を分解するユーティリティ
class MixiUrlParser {
public:

	/**
	 * URL からオーナーIDを取得する。
	 *
	 * view_diary.pl?id=xxx&owner_id=xxx
	 */
	static CString GetOwnerID( CString url )
	{
		CString id;
		util::GetAfterSubString( url, _T("owner_id="), id );

		// '&' が含まれていれば、その後ろを削除する
		if( id.Find( '&' ) != -1 ) {
			util::GetBeforeSubString( id, L"&", id );
		}

		return id;
	}

	/**
	 * URL からIDを取得する。
	 *
	 * view_community.pl?id=1231285
	 */
	static int GetID( CString url )
	{
		CString id;
		util::GetAfterSubString( url, _T("id="), id );
		return _wtoi(id);
	}

	/**
	 * コメント数の取得。
	 *
	 * http://mixi.jp/view_bbs.pl?id=xxx&comment_count=yyy&comm_id=zzz
	 */
	static int GetCommentCount(LPCTSTR uri)
	{
		// "comment_count="と"&"に囲まれた文字列を数値変換したもの。
		CString strCommentCount;
		if( util::GetBetweenSubString( uri, L"comment_count=", L"&", strCommentCount ) == -1 ) {
			// not found.
			return -1;
		}

		return _wtoi(strCommentCount);
	}

	/**
	 * 名前の取得。
	 *
	 * show_friend.pl?id=xxx">なまえ</a>
	 *
	 * @param str A タグの部分文字列
	 */
	static bool GetAuthor(LPCTSTR str, CMixiData* data)
	{
		TRACE( L"GetAuthor, param[%s]\n", str );

		// show_friend.pl 以降に整形。
		CString target;
		if( util::GetAfterSubString( str, L"show_friend.pl", target ) == -1 ) {
			// not found.
			CString msg;
			msg.Format( L"引数が show_friend.pl を含みません str[%s]", str );
			MZ3LOGGER_ERROR(msg);
			return false;
		}

		// ID 抽出
		CString id;
		if( util::GetBetweenSubString( target, L"id=", L"\">", id ) == -1 ) {
			// not found.
			CString msg;
			msg.Format( L"引数が 'id=' を含みません str[%s]", str );
			MZ3LOGGER_ERROR(msg);
			return false;
		}
		data->SetAuthorID( id );

		// 名前抽出
		CString name;
		if( util::GetBetweenSubString( target, L">", L"<", name ) == -1 ) {
			CString msg;
			msg.Format( L"引数が '>', '<' を含みません str[%s]", str );
			MZ3LOGGER_ERROR(msg);
			return false;
		}
		data->SetAuthor( name );

		return true;
	}

};

/// パーサ用ユーティリティ群
class ParserUtil {
public:
	/**
	 * URL に最後にアクセスしたときの、投稿件数を取得する
	 */
	static void GetLastIndexFromIniFile(LPCTSTR uri, CMixiData* data)
	{
		// URIを分解
		CString buf = uri;

		switch (data->GetAccessType()) {
		case ACCESS_DIARY:
		case ACCESS_MYDIARY:
			buf = buf.Mid(buf.Find(_T("id="))+ wcslen(_T("id=")));
			buf = buf.Left(buf.Find(_T("&")));
			buf.Format(_T("d%s"), buf);
			break;
		case ACCESS_BBS:
			buf.Format(_T("b%d"), data->GetID());
			break;
		case ACCESS_EVENT:
			buf.Format(_T("v%d"), data->GetID());
			break;
		case ACCESS_ENQUETE:
			buf.Format(_T("e%d"), data->GetID());
			break;
		}

		std::string idKey = util::my_wcstombs( (LPCTSTR)buf );

		// キー取得
		int lastIndex = -1;

		if (theApp.m_logfile.RecordExists(idKey.c_str(), "Log") ) {
			// レコードが在ればそれを使う
			lastIndex = atoi( theApp.m_logfile.GetValue(idKey.c_str(), "Log").c_str() );
		}
		else {
			lastIndex = -1;
		}

		data->SetLastIndex(lastIndex);
	}

	/**
	 * 時刻変換
	 *
	 * @param strDate [in]  日付文字列。例："12月08日"
	 * @param strTime [in]  時刻文字列。例："23:31"
	 * @param data    [out] 解析結果を SetDate で保存する。
	 */
	static void ChangeDate( CString strDate, CString strTime, CMixiData* data )
	{
		int month  = _wtoi( strDate.Left(strDate.Find(_T("月")) ) );

		strDate = strDate.Mid(strDate.Find(_T("月")) + wcslen(_T("月")));
		int day    = _wtoi( strDate.Left(strDate.Find(_T("日"))) );

		int hour   = _wtoi( strTime.Left( strTime.Find(_T(":")) ) );
		int minute = _wtoi( strTime.Mid( strTime.Find(_T(":")) + wcslen(_T(":")) ) );

		// 全て 0 なら設定しない
		if( month!=0 || day!=0 || hour!=0 || minute!=0 ) {
			data->SetDate(month, day, hour, minute);
		}
	}

	/**
	 * 時刻変換
	 *
	 * @param buf  [in]  日時文字列。例："2006年11月19日 17:12"
	 * @param data [out] 解析結果を SetDate で保存する。
	 */
	static void ChangeDate(CString buf, CMixiData* data)
	{
		int year = _wtoi(buf.Left(buf.Find(_T("年"))));

		buf = buf.Mid(buf.Find(_T("年")) + wcslen(_T("年")));
		int month = _wtoi(buf.Left(buf.Find(_T("月"))));

		buf = buf.Mid(buf.Find(_T("月")) + wcslen(_T("月")));
		int day = _wtoi(buf.Left(buf.Find(_T("日"))));

		buf = buf.Mid(buf.Find(_T("日")) + wcslen(_T("日")));
		buf.Trim();
		int hour = _wtoi(buf.Left(buf.Find(_T(":"))));

		buf = buf.Mid(buf.Find(_T(":")) + wcslen(_T(":")));
		int minute = _wtoi(buf);

		// 全て 0 なら設定しない
		if( year!=0 || month!=0 || day!=0 || hour!=0 || minute!=0 ) {
			if( year==0 ) {
				data->SetDate(month, day, hour, minute);
			}else{
				data->SetDate(year, month, day, hour, minute);
			}
		}
	}

	/**
	 * str からリンク等を抽出、整形し、HTML の整形をして、data_ に AddBody する
	 */
	static void AddBodyWithExtract( CMixiData& data_, CString str )
	{
		ExtractReplaceLink( str, &data_ );
		UnEscapeHtmlElement( str );
		data_.AddBody( str );
	}

	/**
	 * 文字列中に含まれる全てのタグを除去する
	 */
	static bool StripAllTags( CString& str )
	{
		CString result;
		
		// '<' と '>' で囲まれた文字列を全て削除する

		bool bInTag = false;
		int len = str.GetLength();
		for( int i=0; i<len; i++ ) {
			if( bInTag ) {
				if( str[i] == '>' ) {
					bInTag = false;
				}
			}else{
				if( str[i] == '<' ) {
					bInTag = true;
				}else{
					result.AppendChar( str[i] );
				}
			}
		}

		str = result;

		// タグが閉じずに終わればエラーとする
		if( bInTag ) 
			return false;

		return true;
	}

	/**
	 * HTML 要素の置換。実体参照。
	 */
	static void UnEscapeHtmlElement(CString& line)
	{
		while( line.Replace(_T("\n"), _T("")) ) ;

		// table タグの除去
		{
			static MyRegex reg;
			if( !reg.isCompiled() ) 
				if(! reg.compile( L"<table[^>]*>" ) ) 
					return;
			if( line.Find( L"<table" ) != -1 ) 
				reg.replaceAll( line, L"" );
		}
		while( line.Replace(_T("</table>"), _T("")) );

		// tr タグの除去
		{
			static MyRegex reg;
			if( !reg.isCompiled() ) 
				if(! reg.compile( L"<tr[^>]*>" ) ) 
					return;
			if( line.Find( L"<tr" ) != -1 ) 
				reg.replaceAll( line, L"" );
		}
		while( line.Replace(_T("</tr>"), _T("")) );

		// td タグの除去
		{
			static MyRegex reg;
			if( !reg.isCompiled() ) 
				if(! reg.compile( L"<td[^>]*>" ) ) 
					return;
			if( line.Find( L"<td" ) != -1 ) 
				reg.replaceAll( line, L"" );
		}
		while( line.Replace(_T("</td>"), _T("")) );

		// div タグの除去
		{
			static MyRegex reg;
			if( !reg.isCompiled() ) 
				if(! reg.compile( L"<div[^>]*>" ) ) 
					return;
			if( line.Find( L"<div" ) != -1 ) 
				reg.replaceAll( line, L"" );
		}
		while( line.Replace(_T("</div>"), _T("")) );

		// br タグの置換
		{
			static MyRegex reg;
			if( !reg.isCompiled() ) 
				if(! reg.compile( L"<br[^>]*>" ) ) 
					return;
			if( line.Find( L"<br" ) != -1 ) 
				reg.replaceAll( line, L"\r\n" );
		}

		// "<p>" → 削除
		while( line.Replace(_T("<p>"), _T("")) ) ;

		// </p> → 改行
		while( line.Replace(_T("</p>"), _T("\r\n\r\n")) ) ;

		while( line.Replace(_T("&quot;"), _T("\"")) ) ;
		while( line.Replace(_T("&gt;"), _T(">")) ) ;
		while( line.Replace(_T("&lt;"), _T("<")) ) ;
		while( line.Replace(_T("&nbsp;"), _T(" ")) ) ;

		// &#xxxx; の実体参照の文字化
		// 例）&#3642; → char(3642)
		ReplaceEntityReferenceToCharacter( line );
	}

	/**
	 * &#xxxx; の実体参照の文字化
	 * 例）&#3642; → char(3642)
	 */
	static void ReplaceEntityReferenceToCharacter( CString& str )
	{
		// 正規表現のコンパイル（一回のみ）
		static MyRegex reg;
		if( !reg.isCompiled() ) {
			if(! reg.compile( L"&#([0-9]{4,5});" ) ) {
				MZ3LOGGER_FATAL( FAILED_TO_COMPILE_REGEX_MSG );
				return;
			}
		}

		CString target = str;
		str = L"";
		for( int i=0; i<100; i++ ) {	// 100 は無限ループ防止
			std::vector<MyRegex::Result>* pResults = NULL;
			if( reg.exec(target) == false || reg.results.size() != 2 )
			{
				// 未発見。
				// 残りの文字列を代入して終了。
				str += target;
				break;
			}
			// 発見。
			// マッチ文字列の左側を出力
			str += target.Left( reg.results[0].start );

			// 実体化
			{
				int d = _wtoi( reg.results[1].str.c_str() );
				str.AppendFormat( L"%c", d );
			}

			// ターゲットを更新。
			target = target.Mid( reg.results[0].end );
		}
	}

	/**
	 * リンク変換。
	 */
	static void ExtractReplaceLink(CString& str, CMixiData* data)
	{
		// いずれの処理も、
		// すぐに ExtractXxx を実行してもいいが、
		// ExtractXxx はコストが高いので、事前に Find で最低限の探索を行っている

#define LINE_HAS_DIARY_IMAGE(str) util::LineHasStringsNoCase( str, L"<a", L"MM_openBrWindow('", L"'", L"</a>" )

		// 画像の抽出
		// アルバム画像、日記・トピック・アンケート画像の抽出

		if( str.Find( L"show_album_picture.pl" ) != -1 ) {
			// アルバム画像の抽出
			ExtractAlbumImage( str, *data );
		}else if( LINE_HAS_DIARY_IMAGE(str) ) {
			// 日記・トピック・アンケート画像の抽出
			ExtractDiaryImage( str, *data );
		}

		// 絵文字画像リンクの変換
		// <img src="http://img.mixi.jp/img/emoji/85.gif" alt="喫煙" width="16" height="16" class="emoji" border="0">
#define LINE_HAS_EMOJI_LINK(str)	util::LineHasStringsNoCase( str, L"<img", L"alt=", L"class=\"emoji\"", L">" )
		if( LINE_HAS_EMOJI_LINK(str) ) {
			ReplaceEmojiImageToText( str );
		}

		// その他の画像リンクの変換
		if( util::LineHasStringsNoCase( str, L"<img", L"src=" ) ) {
			ExtractGeneralImageLink( str, *data );
		}

		// リンクの抽出
		if (str.Find( L"href" ) != -1) {
			ExtractURI( str, *data );
		}

		// 2ch 形式のリンク抽出
		if( str.Find( L"ttp://" ) != -1 ) {
			Extract2chURL( str, *data );
		}
	}

private:

	/**
	 * 絵文字画像リンクの変換。
	 *
	 * とりあえず alt 文字列に置換する。
	 */
	static void ReplaceEmojiImageToText( CString& line )
	{
		// <img src="http://img.mixi.jp/img/emoji/85.gif" alt="喫煙" width="16" height="16" class="emoji" border="0">
		// のようなリンクを
		// "((喫煙))" に変換する

		// 正規表現のコンパイル（一回のみ）
		static MyRegex reg;
		if( !reg.isCompiled() ) {
			LPCTSTR szPattern = L"<img src=\"http://img.mixi.jp/img/emoji/([^\"]+).gif\" alt=\"([^\"]+)\" [^c]+ class=\"emoji\"[^>]*>";
			if(! reg.compile( szPattern ) ) {
				MZ3LOGGER_FATAL( FAILED_TO_COMPILE_REGEX_MSG );
				return;
			}
		}

		// ((喫煙)) に変換する
		reg.replaceAll( line, L"(({2}))" );

		// [m:xx] 形式に置換する場合は下記の replaceAll とする。
//		if( reg.replaceAll( line, L"[m:{1}]" ) ) {
	}

	/**
	 * アルバム画像変換
	 *
	 * str からアルバム画像リンクを抽出し、そのリンクを data に追加(AddImage)する。
	 * また、str から該当する画像リンクを削除する。
	 */
	static void ExtractAlbumImage(CString& str, CMixiData& data_)
	{
/* 想定文字列（改行なし）
<br>
<a href="javascript:void(0)" onClick="MM_openBrWindow('show_album_picture.pl?
album_id=ZZZ&number=ZZZ&owner_id=ZZZ&key=ZZZ
','','width=650,height=650,toolbar=no,scrollbars=yes,left=10,top=10')">
<img alt=写真 src=http://ic30.mixi.jp/p/ZZZ/ZZZ/album/ZZZ/ZZZ/ZZZ.jpg border=0></a>
<a href="javascript:void(0)" onClick="MM_openBrWindow('show_album_picture.pl?
album_id=ZZZ&number=ZZZ&owner_id=ZZZ&key=ZZZ
','','width=650,height=650,toolbar=no,scrollbars=yes,left=10,top=10')">
<img alt=写真 src=http://ic3.mixi.jp/p/ZZZ/ZZZ/album/ZZZ/ZZZ/ZZZ.jpg border=0></a>
*/

		CString ret = _T("");

		LPCTSTR tag_MMBegin		= _T("MM_openBrWindow('");
		LPCTSTR tag_MMEnd		= _T("'");
		CString tag_AlbumBegin	= L"<a href=\"javascript:void(0)\"";
		CString tag_AlbumEnd	= L"</a>";
		CString url_mixi		= _T("http://mixi.jp/");

		// まず、アルバム関連タグ開始前までを出力バッファに追加
		int pos = str.Find(tag_AlbumBegin);
		if( pos >= 0 ) {
			// アルバム関連タグ開始前までを出力バッファに追加。
			ret += str.Left( pos );
		}else{
			// 見つからなかったよorz
			// アルバムなさげ。
			// 変換せずに終了。
			return;
		}

		CString buf = str.Mid( pos );
		for( int i=0; i<100; i++ ) {	// 100 は無限ループ防止

			// タグ開始前までを追加
			CString left;
			if( util::GetBeforeSubString( buf, tag_MMBegin, left ) == -1 ) {
				// not found
				ret += buf;
				break;
			}
			ret += left;

			CString url_right;
			if( util::GetBetweenSubString( buf, tag_MMBegin, tag_MMEnd, url_right ) == -1 ) {
				// not found
				ret += buf;
				break;
			}

			TRACE(_T("%s\n"), url_right);
			CString url_image = url_mixi + url_right;
			data_.AddImage(url_image);

#define REPLACE_ALBUMURL_STRING
#ifdef  REPLACE_ALBUMURL_STRING
			ret.AppendFormat( L"<<画像%02d(album)>>", data_.GetImageCount() );
#else
			ret += url_image;
#endif
			ret += _T("<br>");

			// 次のサーチのためにbufを変更する
			int index = buf.Find(tag_AlbumEnd);
			buf = buf.Mid(index + wcslen(tag_AlbumEnd));
		}

		// 更新して終了。
		str = ret;
	}

	/**
	 * 日記、トピック等の画像変換。
	 *
	 * str から画像リンクを抽出し、そのリンクを data に追加(AddImage)する。
	 * また、str から該当する画像リンクを削除する。
	 */
	static void ExtractDiaryImage(CString& line, CMixiData& data_)
	{
/* 想定文字列（改行なし。Zは英数字）
<td><table><tr><td width="130" height="140" align="center" valign="middle">
<a href="javascript:void(0)" onClick="MM_openBrWindow('show_diary_picture.pl?
owner_id=ZZZZZ&id=ZZZZZ&number=ZZZZZ','pict','width=680,height=660,toolbar=
no,scrollbars=yes,left=5,top=5')"><img src="http://ic29.mixi.jp/p/ZZZZZZZ/
ZZZ/diary/ZZ/ZZ/ZZs.jpg" border="0"></a></td>
*/
		LPCTSTR url_mixi		= _T("http://mixi.jp/");

		// 入出力文字列を解析対象とする。
		CString target = line;

		// とりあえず入出力文字列を空にする。
		line = L"";

		for( int i=0; i<100; i++ ) {	// 100 は無限ループ防止
			if( LINE_HAS_DIARY_IMAGE(target) ) {

				// 左側をとりあえず出力
				CString left;
				if( util::GetBeforeSubString( target, L"<a", left ) == -1 ) {
					// not found
					// LineHasStringsNoCase でチェックしているのでここに来たら内部エラー。
					MZ3LOGGER_FATAL( L"画像リンク解析でエラーが発生しました。 line[" + target + L"]" );
					break;
				}
				line += left;

				CString url_right;
				if( util::GetBetweenSubString( target, L"MM_openBrWindow('", L"'", url_right ) == -1 ) {
					// not found
					// LineHasStringsNoCase でチェックしているのでここに来たら内部エラー。
					MZ3LOGGER_FATAL( L"画像リンク解析でエラーが発生しました。 line[" + target + L"]" );
					break;
				}

				TRACE(_T("%s\n"), url_right);
				CString url_image = url_mixi + url_right;
				data_.AddImage( url_image );

				MZ3LOGGER_DEBUG( L"画像URL [" + url_image + L"]" );

				// 画像リンクを置換する
				line.AppendFormat( L"<<画像%02d>><br>", data_.GetImageCount() );

				// 次のサーチのために str を更新する
				if( util::GetAfterSubString( target, L"</a>", target ) < 0 ) {
					// 終了タグがなかった。タグ仕様変更？
					MZ3LOGGER_ERROR( L"画像の終了タグが見つかりません。 line[" + target + L"]" );
					break;
				}else{
					// 更新OK
				}
			}else{
				// not found
				// 見つからなかったので、解析対象文字列を入出力文字列に追加する
				line += target;
				break;
			}
		}
	}

	/**
	 * 画像変換。
	 *
	 * str から画像リンクを抽出し、そのリンクを data に追加(AddImage)する。
	 * また、str から該当する画像リンクを削除する。
	 */
	static void ExtractGeneralImageLink(CString& line, CMixiData& data_)
	{
		// 正規表現のコンパイル（一回のみ）
		static MyRegex reg;
		if( !reg.isCompiled() ) {
			LPCTSTR szPattern = L"<img[^>]*src=\"([^\"]+)\" [^>]*>";
			if(! reg.compile( szPattern ) ) {
				MZ3LOGGER_FATAL( FAILED_TO_COMPILE_REGEX_MSG );
				return;
			}
		}

		CString target = line;
		line = L"";
		for( int i=0; i<100; i++ ) {	// 100 は無限ループ防止
			if( !reg.exec(target) || reg.results.size() != 2 ) {
				// 未発見。
				// 残りの文字列を代入して終了。
				line += target;
				break;
			}

			// 発見。
			std::vector<MyRegex::Result>& results = reg.results;

			// マッチ文字列全体の左側を出力
			line.Append( target, results[0].start );

			CString text = L"<<画像>>";

			// url を追加
			LPCTSTR url = results[1].str.c_str();
			data_.m_linkList.push_back( CMixiData::Link(url, text) );

			// 置換
			line.Append( text );

			// とりあえず改行
			line += _T("<br>");

			// ターゲットを更新。
			target.Delete( 0, results[0].end );
		}

	}

public:
	/**
	 * 全てのリンク文字列（<a...>XXX</a>）を、文字列のみ（XXX）に変更する
	 * また、href="url" を list_ に追加する。
	 */
	static void ExtractURI( CString& str, std::vector<CMixiData::Link>& list_ )
	{
		TRACE( L"ExtractURI:str[%s]\n", (LPCTSTR)str );

		// <wbr/> タグの除去
		while( str.Replace(_T("<wbr/>"), _T("")) );

		// 正規表現のコンパイル（一回のみ）
		static MyRegex reg;
		if( !reg.isCompiled() ) {
			if(! reg.compile( L"<a href='([^']+)'[^>]*>([^<]*)</a>" ) ) {
				MZ3LOGGER_FATAL( FAILED_TO_COMPILE_REGEX_MSG );
				return;
			}
		}
		static MyRegex reg2;
		if( !reg2.isCompiled() ) {
			if(! reg2.compile( L"<a href=\"([^\"]+)\"[^>]*>([^<]*)</a>" ) ) {
				MZ3LOGGER_FATAL( FAILED_TO_COMPILE_REGEX_MSG );
				return;
			}
		}
		static MyRegex reg3;
		if( !reg3.isCompiled() ) {
			if(! reg3.compile( L"<a href=([^>]+)>([^<]*)</a>" ) ) {
				MZ3LOGGER_FATAL( FAILED_TO_COMPILE_REGEX_MSG );
				return;
			}
		}

		CString target = str;
		str = L"";
		for( int i=0; i<100; i++ ) {	// 100 は無限ループ防止
			std::vector<MyRegex::Result>* pResults = NULL;
			if( reg.exec(target) && reg.results.size() == 3 ) {
				pResults = &reg.results;
			}
			if( pResults == NULL && reg2.exec(target) && reg2.results.size() == 3 ) {
				pResults = &reg2.results;
			}
			if( pResults == NULL && reg3.exec(target) && reg3.results.size() == 3 ) {
				pResults = &reg3.results;
			}
			if( pResults == NULL ) {
				// 未発見。
				// 残りの文字列を代入して終了。
				str += target;
				break;
			}
			// 発見。
			std::vector<MyRegex::Result>& results = *pResults;


			// マッチ文字列全体の左側を出力
			str += target.Left( results[0].start );

			// URL
			const std::wstring& url = results[1].str;
			TRACE( L"regex-match-URL  : %s\n", url.c_str() );

			// 文字列
			const std::wstring& text = results[2].str;
			TRACE( L"regex-match-TEXT : %s\n", text.c_str() );
			str += text.c_str();

			// データに追加
			list_.push_back( CMixiData::Link(url.c_str(),text.c_str()) );

			// ターゲットを更新。
			target = target.Mid( results[0].end );
		}
	}

private:
	/**
	 * 全てのリンク文字列（<a...>XXX</a>）を、文字列のみ（XXX）に変更する
	 * また、href="url" を data_ のリンクリストに追加する。
	 */
	static void ExtractURI( CString& str, CMixiData& data_ ) {
		ExtractURI( str, data_.m_linkList );
	}

	/**
	 * 2ch 形式のURL(ttp://...)を抽出し、正規化して data のリンクリストに追加する。
	 */
	static void Extract2chURL( const CString& str, CMixiData& data_ )
	{
//		TRACE( L"Extract2chURL:str[%s]\n", (LPCTSTR)str );

		// 正規表現のコンパイル（一回のみ）
		static MyRegex reg;
		if( !reg.isCompiled() ) {
			LPCTSTR szPattern = L"[^h](ttps?://[-_.!~*'()a-zA-Z0-9;/?:@&=+$,%#]+)";
			if(! reg.compile( szPattern ) ) {
				MZ3LOGGER_FATAL( FAILED_TO_COMPILE_REGEX_MSG );
				return;
			}
		}

		CString target = str;
		for( int i=0; i<100; i++ ) {	// 100 は無限ループ防止
			if( reg.exec(target) == false || 
				reg.results.size() != 2 )
			{
				// 未発見。終了。
				break;
			}

			// 発見。

			// 2ch URL
			const std::wstring& url_2ch = reg.results[1].str;
			TRACE( L"regex-match-2chURL : %s\n", url_2ch.c_str() );

			// 2ch URL を正規化
			std::wstring url = L"h";
			url += url_2ch;

			// データに追加
			data_.m_linkList.push_back( CMixiData::Link(url.c_str(), url_2ch.c_str()) );

			// ターゲットを更新。
			target = target.Mid( reg.results[0].end );
		}
	}
};

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
	static void GetPostAddress(int sIndex, int eIndex, const CHtmlArray& html_, CMixiData& data_)
	{
		CString str;
		for (int i=sIndex; i<eIndex; i++) {
			str = html_.GetAt(i);

			// <form name="bbs_comment_form" action="add_bbs_comment.pl?id=xxx&comm_id=xxx"
			// method="post" enctype="multipart/form-data">
			if (str.Find(_T("<form")) != -1 &&
				str.Find(_T("method=")) != -1 &&
				str.Find(_T("action=")) != -1 &&
				str.Find(_T("post")) != -1) 
			{
				// Content-Type/enctype 解決
				if (str.Find(_T("multipart")) != -1) {
					data_.SetContentType(CONTENT_TYPE_MULTIPART);
				} else {
					data_.SetContentType(CONTENT_TYPE_FORM_URLENCODED);
				}

				// action 取得
				CString action;
				if( util::GetBetweenSubString( str, L"action=\"", L"\"", action ) < 0 ) {
					continue;
				}
				data_.SetPostAddress( action );

				if (data_.GetAccessType() != ACCESS_DIARY &&
					data_.GetAccessType() != ACCESS_MYDIARY)
				{
					break;
				}
				continue;
			}
			else if (str.Find(_T("owner_id")) != -1) {

				CString ownerId;
				if( util::GetBetweenSubString( str, L"value=\"", L"\"", ownerId ) < 0 ) {
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
		// 未発見なら終了。
		// <table ... bgcolor="#eed6b5">
		const int count = html.GetCount();
		int iLine=startIndex;
		for( ; iLine<count; iLine++ ) {
			const CString& line = html.GetAt(iLine);
			if( util::LineHasStringsNoCase( line, L"<table", L"bgcolor=\"#eed6b5\">" ) )
			{
				break;
			}
		}
		if( iLine >= count ) {
			return false;
		}

		// </table> 発見するまでパターンマッチを繰り返す。
		for( iLine=iLine+1; iLine<count; iLine++ ) {
			const CString& line = html.GetAt(iLine);
			if( util::LineHasStringsNoCase( line, L"</table>" ) ) {
				break;
			}

			CString str = line;
			ParserUtil::ExtractURI( str, data.m_linkPage );
		}

		return !data.m_linkPage.empty();
	}

	/**
	 * 「最新のトピック」の抽出。
	 */
	static bool parseRecentTopics( CMixiData& data, const CHtmlArray& html, int startIndex=200 )
	{
		// startIndex 以降に下記を発見したら、解析開始。
		// 未発見なら終了。
		// <table ... bgcolor="#E8C79B">
		const int count = html.GetCount();
		int iLine=startIndex;
		for( ; iLine<count; iLine++ ) {
			const CString& line = html.GetAt(iLine);
			if( util::LineHasStringsNoCase( line, L"<table", L"bgcolor=\"#E8C79B\">" ) )
			{
				break;
			}
		}
		if( iLine >= count ) {
			return false;
		}

		// </table> 発見するまでパターンマッチを繰り返す。
		for( iLine=iLine+1; iLine<count; iLine++ ) {
			const CString& line = html.GetAt(iLine);
			if( util::LineHasStringsNoCase( line, L"</table>" ) ) {
				break;
			}

			CString str = line;
			ParserUtil::ExtractURI( str, data.m_linkPage );
		}

		return !data.m_linkPage.empty();
	}
};

/**
 * [list] list_news_category.pl 用パーサ。
 * 
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
			// +1 ～ +4 行目の辺りにあるはず。
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
 * [list] list_bookmark.pl 用パーサ。
 * 
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
		 *   <td ALIGN=right BGCOLOR=#EED6B5>1件～30件を表示&nbsp;&nbsp;<a href=list_bookmark.pl?page=2>次を表示</a></td></tr>
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

/**
 * [list] list_friend.pl 用パーサ。
 * 
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
	/// <td align="right" bgcolor="#EED6B5">1件～50件を表示&nbsp;&nbsp;<a href=list_friend.pl?page=2&id=xxx>次を表示</a></td></tr>
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
 * [list] list_community.pl 用パーサ。
 * 
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
		 * ★ table で5件ずつ並んでいる。
		 *   奇数行にリンク＆画像が、偶数行に名前が並んでいる。
		 *   従って、「5件分のリンク抽出」「5件分の名前抽出」「突き合わせ」「データ追加」という手順で行う。
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
				// 5件分取得＆追加
				if(! parseTwoTR( out_, html_, iLine ) ) {
					// 解析エラー
					break;
				}

				// </table> が見つかれば終了
				const CString& line = html_.GetAt(iLine);
				if( util::LineHasStringsNoCase( line, L"</table>" ) ) {
					break;
				}
			}
		}

		// </table> が見つかったので、その後の行から次、前のリンクを抽出
		for( ; iLine<count; iLine++ ) {
			const CString& str = html_.GetAt(iLine);

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
	 * 5件分のユーザの内容を抽出する
	 *
	 * (1) </tr> が現れるまでの各行をパースし、5件分のURL、時刻を生成する。
	 * (2) </tr> が現れるまでの各行をパースし、5件分の名前を生成する。
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

			// </table> が見つかれば終了
			if( util::LineHasStringsNoCase( line, L"</table>" ) ) {
				return false;
			}

			if( util::LineHasStringsNoCase( line, L"</tr>" ) ) {
				// </tr> 発見、名前抽出に移る。
				bBreak = true;
			}

			// <td で始まるなら、抽出する
			if( line.Left( 3 ).CompareNoCase( L"<td" ) == 0 ) {
				/* 行の形式：
line1: <td width="20%" height="100" background="http://img.mixi.jp/img/bg_line.gif">
line2: <a href="view_community.pl?id=1231285"><img src="http://img-c1.mixi.jp/photo/comm/12/85/1231285_120s.jpg" alt=""  border="0" /></a></td>
*/
				CMixiData mixi;

				// 次の行をフェッチ
				const CString& line2 = html.GetAt( ++iLine );
				if( util::LineHasStringsNoCase( line2, L"</tr>" ) ) {
					// </tr> 発見、名前抽出に移る。
					bBreak = true;
				}

				// <a 以降のみにする
				CString target;
				if( util::GetAfterSubString( line2, L"<a", target ) < 0 ) {
					// <a がなかったので次の行解析へ。
					continue;
				}
				// target:  href="view_community.pl?id=1231285"><img src="http://img-c1.mixi.jp/photo/comm/12/85/1231285_120s.jpg" alt=""  border="0" /></a></td>
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
				// target:  src="http://img-c1.mixi.jp/photo/comm/12/85/1231285_120s.jpg" alt=""  border="0" /></a></td>
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
		bBreak = false;
		int mixiIndex = 0;		// 何番目の項目を解析しているかを表すインデックス
		for( ; iLine < lastLine && bBreak == false; iLine++ ) {
			const CString& line = html.GetAt( iLine );
			if( util::LineHasStringsNoCase( line, L"</tr>" ) ) {
				// </tr> 発見、終了
				bBreak = true;
			}

			if( mixiIndex >= (int)tmp_list.size() ) {
				// リスト数以上見つかったので、終了。
				break;
			}

			// </table> が見つかれば終了
			if( util::LineHasStringsNoCase( line, L"</table>" ) ) {
				return false;
			}

			// <td があれば、抽出する
			if( util::LineHasStringsNoCase( line, L"<td" ) ) {
				CMixiData& mixi = tmp_list[mixiIndex];
				mixiIndex ++;

				// 行の形式：
				// <tr align="center" bgcolor="#FFF4E0"><td valign="top">MZ3 Debuggers(50)
				// または
				// </td><td valign="top">MZ3 -Mixi for ZERO3-(1504)

				// 名前抽出

				// <td 以降を抽出
				CString name;
				if( util::GetAfterSubString( line, L"<td ", name ) < 0 ) {
					continue;
				}

				// > 以降を抽出
				if( util::GetBetweenSubString( name, L">", L"\n", name ) < 0 ) {
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
	/// <td align=right>1件～50件を表示&nbsp;&nbsp;<a href=list_community.pl?page=2&id=xxx>次を表示</a></td>
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
 * [list] show_intro.pl 用パーサ。
 * 
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
	/// <td align=right>1件～50件を表示&nbsp;&nbsp;<a href=show_intro.pl?page=2&id=xxx>次を表示</a></td>
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
 * [list] list_bbs.pl 用パーサ。
 * 
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
		 *   - "<table ... BGCOLOR="#EED6B5">" の行を探す。
		 *   - "</td></tr></table>" が現れるまで読み飛ばす。
		 *   - 次の行以降が項目。
		 *
		 * - 下記の行をパースし、項目を生成する。
		 *   <td ALIGN=center ROWSPAN=3 NOWRAP bgcolor=#FFD8B0>0X月XX日<br>XX:XX</td>
		 *   <td bgcolor=#FFF4E0>&nbsp;<a href="view_bbs.pl?id=xxx&comm_id=xxx">内容</a></td></tr>
		 * - 再度 "<table ... BGCOLOR=#EED6B5>" が現れたら終了とする。
		 *   この行は（次へ、前へを含む）ナビゲーション行。
		 */

		// 項目開始行を探す
		int iLine = 200;		// とりあえず読み飛ばす
		bool bInItems = false;
		for( ; iLine<count; iLine++ ) {
			const CString& line = html_.GetAt(iLine);

			if( util::LineHasStringsNoCase( line, L"<table", L"#EED6B5" ) ) 
			{
				// 項目開始行発見。
				bInItems = true;

				// さらに無駄な行をスキップするため、break しない。
			}

			if( bInItems ) {
				// 無駄な行のスキップ
				if( util::LineHasStringsNoCase( line, L"</td></tr></table>" ) ) {
					// 無駄な行のスキップ完了。
					iLine ++;
					break;
				}
			}
		}

		if( !bInItems ) {
			return false;
		}

		// 項目の取得
		for( ; iLine<count; iLine++ ) {
			const CString& line = html_.GetAt(iLine);

			// "<table ... BGCOLOR=#EED6B5>" が現れたらナビゲーション行。
			// 「次」、「前」のリンクを含む。
			// 解析を終了する。
			if( util::LineHasStringsNoCase( line, L"<td", L"#EED6B5" ) ) 
			{
				// 「次を表示」、「前を表示」のリンクを抽出する
				parseNextBackLink( nextLink, backLink, line );

				// 抽出の成否にかかわらず終了する。
				break;
			}

			// 項目？
			//   <td align="center" rowspan="3" nowrap="nowrap" bgcolor="#FFD8B0">0X月XX日<br />XX:XX</td>
			//   <td bgcolor="#FFF4E0">&nbsp;<a href="view_bbs.pl?id=xxx&comm_id=xxx">【報告】動作しました</a></td></tr>
			if( util::LineHasStringsNoCase( line, L"<td", L"#FFD8B0" ) ) {
				// 解析
				CMixiData mixi;

				// 日付
				CString date, time;
				util::GetBetweenSubString( line, L">", L"<br />", date );
				util::GetBetweenSubString( line, L"<br />", L"<", time );
				ParserUtil::ChangeDate( date, time, &mixi );

				// 次の行を取得し、見出しとリンクを抽出する
				iLine += 1;
				const CString& line2 = html_.GetAt(iLine);

				// 見出し抽出
				CString target;
				{
					// まず "<a href=" まで読み飛ばす
					util::GetAfterSubString( line2, L"<a href=", target );
					// buf : "view_bbs.pl?id=xxx&comm_id=xxx">内容</a></td></tr>
					CString title;
					util::GetBetweenSubString( target, L">", L"<", title );
					mixi.SetTitle(title);
				}

				// URL 抽出
				CString url;
				util::GetBetweenSubString( target, L"\"", L"\"", url );
				mixi.SetURL( url );

				// URL に応じてアクセス種別を設定
				mixi.SetAccessType( util::EstimateAccessTypeByUrl(url) );

				// IDの抽出、設定
				mixi.SetID( MixiUrlParser::GetID(url) );

				// コメント数解析
				{
					// 下記の行を取得して解析する。
					// <td ALIGN=right bgcolor="#FFFFFF"><a href=view_bbs.pl?id=xxx&comm_id=xxx>書き込み(19)</a>
					int commentCount = -1;
					for( iLine++; iLine<count; iLine++ ) {
						const CString& line = html_.GetAt(iLine);

						if( util::LineHasStringsNoCase( line, L"<td", url, L"書き込み(" ) )
						{
							// 発見。コメント数解析
							CString cc;
							util::GetBetweenSubString( line, L"書き込み(", L")", cc );
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
	/// <td ALIGN=right BGCOLOR=#EED6B5>1件～20件を表示&nbsp;&nbsp;<a href=list_bbs.pl?page=2&id=xxx>次を表示</a></td></tr>
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
 * [content] view_news.pl 用パーサ
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

/**
 * [content] view_diary.pl 用パーサ
 */
class ViewDiaryParser : public MixiContentParser
{
public:
	static bool parse( CMixiData& data_, const CHtmlArray& html_ )
	{
		MZ3LOGGER_DEBUG( L"ViewDiaryParser.parse() start." );

		data_.ClearAllList();

		INT_PTR count = html_.GetCount();

		CString str;
		BOOL findFlag = FALSE;
		BOOL endFlag = FALSE;
		int index;
		CString buf;

		// 自分の日記かどうかを判断する（名前の取得のため）
		bool bMyDiary = true;
		for( int i=75; i<min(100,count); i++ ) {
			const CString& line = html_.GetAt( i );
			if( util::LineHasStringsNoCase( line, L"f_np_0_message.gif')\">'" ) ) {
				// 上記があるということは、自分の日記ではない。
				bMyDiary = false;
				break;
			}
		}

		// 最初の10行目までを検索し、<title> タグを解析することで、
		// タイトルを取得する
		for( int i=3; i<min(10,count); i++ ) {
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
		for( int iLine=100; iLine<count; iLine++ ) {
			// <table BORDER=0 CELLSPACING=0 CELLPADDING=0 BGCOLOR=#F8A448>
			// があれば「最近の日記」開始とみなす。
			const CString& line = html_.GetAt( iLine );
			if( util::LineHasStringsNoCase( line, L"<table", L"#F8A448" ) )
			{
				bStartRecentDiary = true;
				continue;
			}

			if( bStartRecentDiary ) {
				// </table>
				// があれば「最近の日記」終了とみなす。
				if( util::LineHasStringsNoCase( line, L"</table>" ) ) {
					break;
				}

				// 解析
				// 下記の形式で「最近の日記」リンクが存在する
				// <td BGCOLOR=#FFFFFF><a HREF="view_diary.pl?id=xxx&owner_id=xxx" CLASS=wide>
				// <img src=http://img.mixi.jp/img/pt_or.gif ALIGN=left BORDER=0 WIDTH=8 HEIGHT=16>
				// タイトル</a></td>
				if( util::LineHasStringsNoCase( line, L"view_diary.pl" ) ) {
					CString url;
					int idx = util::GetBetweenSubString( line, L"<a HREF=\"", L"\"", url );
					if( idx > 0 ) {
						CString buf = line.Mid( idx );
						// buf:
						//  CLASS=wide>
						// <img src=http://img.mixi.jp/img/pt_or.gif ALIGN=left BORDER=0 WIDTH=8 HEIGHT=16>
						// タイトル</a></td>

						// > を読み飛ばす
						CString after;
						if( util::GetAfterSubString( buf, L">", after ) > 0 ) {
							CString title;
							if( util::GetBetweenSubString( after, L">", L"</a>", title ) > 0 ) {
								CMixiData::Link link( url, title );
								data_.m_linkPage.push_back( link );
							}
						}
					}
				}
			}
		}

		// 日記開始フラグ

		for (int i=75; i<count; i++) {
			str = html_.GetAt(i);

			if (findFlag == FALSE) {
				// 日記開始フラグを発見するまで廻す

				if (util::LineHasStringsNoCase( str, L"<td", L"class=\"h12\"" ) ||
					util::LineHasStringsNoCase( str, L"<td", L"CLASS=h12" ) )
				{
					// 日記開始フラグ発見（日記本文発見）
					findFlag = TRUE;

					// とりあえず改行出力
					data_.AddBody(_T("\r\n"));

					// 日記の著者
					{
						// フラグの５０行くらい前に、日記の著者があるはず。
						// <td WIDTH=490 background=http://img.mixi.jp/img/bg_w.gif><b><font COLOR=#605048>XXXの日記</font></b></td>
						// <td WIDTH=490 background=http://img.mixi.jp/img/bg_w.gif><b><font COLOR=#605048>XXXさんの日記</font></b></td>
						for( int iBack=-50; iBack<0; iBack++ ) {
							const CString& line = html_.GetAt( i+iBack );
							LPCTSTR pattern = L"bg_w.gif><b><font ";
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

					// 公開レベル
					{
						// フラグの５０行くらい前に、「友人まで公開」といった情報があるはず。
						// <img src="http://img.mixi.jp/img/diary_icon1.gif" alt="友人まで公開" height="20" hspace="5" width="22"></td>
					}

					// 日記の添付画像取得
					parseImageLink( data_, html_, i );

					// "<td" 以降に整形
					str = str.Mid( str.Find(L"<td") );

					// 現在の行を解析、追加。
					if( util::GetBeforeSubString( str, L"</td>", buf ) > 0 ) {
						// この１行で終わり
						ParserUtil::AddBodyWithExtract( data_, buf );
						endFlag = TRUE;
					}else{
						ParserUtil::AddBodyWithExtract( data_, str );
					}
				}
				else if (str.Find(_T("さんは外部ブログを使われています。<br>")) != -1) {
					// 外部ブログ解析
					parseExternalBlog( data_, html_, i );
					break;
				}
			}
			else {
				// 日記開始フラグ発見済み。

				// 終了タグまでデータ取得
				if (str.Find(_T("</td>")) != -1 ) {
					endFlag = TRUE;
				}

				if( endFlag == FALSE ) {
					// 終了タグ未発見
					// 日記本文解析
					ParserUtil::AddBodyWithExtract( data_, str );
				}
				else {
					// 終了タグ発見
					if (str.Find(_T("<br>")) != -1) {
						ParserUtil::AddBodyWithExtract( data_, str );
					}

					// コメント取得
					i += 10;
					data_.ClearChildren();

					int cmtNum = 0;

					index = i;
					while( index < count ) {
						cmtNum++;
						index  = parseDiaryComment(index, count, data_, html_, cmtNum);
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
		int n_images_end   = -5;
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
	 * @param id
	 */
	static int parseDiaryComment(int sIndex, int eIndex, CMixiData& data_, const CHtmlArray& html_, int id)
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
					util::LineHasStringsNoCase( str, L"<!-- ///// コメント : end ///// -->" ) ) // コメント全体の終了タグ発見
				{
					GetPostAddress(i, eIndex, html_, data_);
					return -1;
				}

				if ((index = str.Find(_T("show_friend.pl"))) != -1) {
					// コメントヘッダ取得

					// ＩＤ
					cmtData.SetCommentIndex(id);

					// 名前
					buf = str.Mid(index);
					MixiUrlParser::GetAuthor(buf, &cmtData);

					// 時刻
					for (int j=i; j>0; j--) {
						str = html_.GetAt(j);
						if (str.Find(_T("日<br>")) != -1) {

							ParserUtil::ChangeDate( 
								util::XmlParser::GetElement(str, 1) + _T(" ") + util::XmlParser::GetElement(str, 3),
								&cmtData);
							break;
						}
					}

					findFlag = TRUE;

				}
			}
			else {

				if (str.Find(_T("CLASS=h12")) != -1) {
					// コメント本文取得
					i++;
					str = html_.GetAt(i);

					if (str.Find(_T("<br>")) != 0) {
						cmtData.AddBody(_T("\r\n"));
					}

					ParserUtil::AddBodyWithExtract( cmtData, str );

					i++;
					for( ; i<eIndex; i++ ) {
						str = html_.GetAt(i);
						if (str.Find(_T("</td></tr>")) != -1) {
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

/**
 * [list] new_bbs.pl 用パーサ
 * 
 * コミュニティ一覧を取得
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
			if( str.Find(_T("/pen_r.gif")) != -1 && str.Find(_T("<img")) != -1 ) 
			{
				dataFind = TRUE;

				CMixiData data;

				// 日付
				ParserUtil::ChangeDate(util::XmlParser::GetElement(str, 3), &data);

				// 見出し
				i+=2;
				CString str = html_.GetAt(i);
				CString buf;
				{
					LPCTSTR key = _T("class=\"new_link\">");
					int index = str.Find(key);
					buf = str.Mid(index + wcslen(key));
				}
				str = buf;
				{
					int index = str.Find(_T("</a> ("));
					buf = str.Left(index);
				}
				data.SetTitle(buf);

				// ＵＲＩ
				str = html_.GetAt(i);
				buf = util::XmlParser::GetElement(str, 1);
				buf = util::XmlParser::GetAttribute(buf, _T("href="));
				// "を取り除く
				buf = buf.Right(buf.GetLength() -1);
				buf = buf.Left(buf.Find(_T("\"")));


				// ＵＲＩ
				// &で分解する
				while (buf.Replace(_T("&amp;"), _T("&")));

				data.SetURL(buf);
				data.SetCommentCount(
					MixiUrlParser::GetCommentCount( buf ) );

				// URL に応じてアクセス種別を設定
				data.SetAccessType( util::EstimateAccessTypeByUrl(buf) );

				// ＩＤを設定
				buf = buf.Mid(buf.Find(_T("id=")) + wcslen(_T("id=")));
				buf = buf.Left(buf.Find(_T("&")));
				data.SetID(_wtoi(buf));

				// 前回のインデックスを取得
				ParserUtil::GetLastIndexFromIniFile(data.GetURL(), &data);

				// コミュニティ名
				{
					LPCTSTR key = _T("</a> (");
					int index = str.Find(key);
					key = _T("</a>");
					buf = str.Mid(index + wcslen(key));
				}
				// 整形：最初と最後の括弧を取り除く
				buf.Trim();
				buf.Replace(_T("\n"), _T(""));
				buf = buf.Mid( 1, buf.GetLength()-2 );
				data.SetName(buf);
				out_.push_back( data );
			}
			else if ( dataFind ) {
				if( str.Find(_T("</table>")) != -1 ) {
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
 * [list] list_message.pl 用パーサ
 * 
 * メッセージ一覧を取得
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
 * [list] new_friend_diary.pl 用パーサ
 * 
 * 日記一覧を取得
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

		CString str;
		CString buf;
		CString key;

		BOOL findFlag = FALSE;
		BOOL dataFind = FALSE;
		int index;

		for (int i=76; i<count; i++) {

			str = html_.GetAt(i);

			if (findFlag == FALSE) {
				if (str.Find(_T("bg_w.gif")) != -1) {
					findFlag = TRUE;
				}
				continue;
			}
			else {

				// 「次を表示」、「前を表示」のリンクを抽出する
				if( parseNextBackLink( nextLink, backLink, str ) ) {
					continue;
				}

				if (util::LineHasStringsNoCase( str, L"<img", L"pen.gif" ) ) {

					dataFind = TRUE;

					CMixiData data;
					data.SetAccessType(ACCESS_DIARY);

					ParserUtil::ChangeDate(
						util::XmlParser::GetElement(str, 3) + _T("") + util::XmlParser::GetElement(str, 5), 
						&data);

					// 見出し
					i++;
					str = html_.GetAt(i);
					// 二つめの>までデータを捨てる
					index = str.Find(_T("><"));
					buf = str.Mid(index + 10);
					str = buf;
					index = str.Find('>');
					buf = str.Mid(index+1);
					str = buf;
					key = _T("</a> (");
					index = str.Find(key);
					buf = str.Left(index);
					data.SetTitle(buf);

					// ＵＲＩ
					str = html_.GetAt(i);
					if (util::LineHasStringsNoCase( str, L"list_diary.pl" ) ) {
						i += 5;
						continue;
					}
					buf = util::XmlParser::GetElement(str, 2);
					data.SetURL(util::XmlParser::GetAttribute(buf, _T("href=")));

					// ＩＤを設定
					buf = data.GetURL();
					buf = buf.Mid(buf.Find(_T("id=")) + wcslen(_T("id=")));
					buf = buf.Left(buf.Find(_T("&")));
					data.SetID(_wtoi(buf));
					ParserUtil::GetLastIndexFromIniFile(data.GetURL(), &data);

					// 名前
					key = _T("</a> (");
					index = str.Find(key);
					key = _T("</a>");
					buf = str.Mid(index + key.GetLength());
					// 最初と最後の括弧を取り除く
					buf.Trim();
					buf.Replace(_T("\n"), _T(""));
					buf = buf.Left(buf.GetLength() -1);
					buf = buf.Right(buf.GetLength() -1);
					data.SetName(buf);
					data.SetAuthor(buf);

					out_.push_back( data );
					i += 5;
				}
				else if (str.Find(_T("</table>")) != -1 && dataFind != FALSE) {
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
 * 
 * 日記コメント記入履歴を取得
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

		int index;
		for (int i=76; i<count; i++) {

			str = html_.GetAt(i);

			if (findFlag == FALSE) {
				if (util::LineHasStringsNoCase( str, L"img/bg_w.gif>") ) {
					// 開始フラグ発見。
					// とりあえずN行無視する。
					i+=10;
					findFlag = TRUE;
				}
				continue;
			}
			else {

				if (util::LineHasStringsNoCase( str, L"<img", L"pen_y.gif" ) ) {
					// 項目発見

					dataFind = TRUE;

					CMixiData data;
					data.SetAccessType(ACCESS_DIARY);

					// 日付
					ParserUtil::ChangeDate( util::XmlParser::GetElement(str, 3), &data );
					TRACE(_T("%s\n"), data.GetDate());

					// 見出し
					i+=2;
					str = html_.GetAt(i);
					key = _T("class=\"new_link\">");
					index = str.Find(key);
					buf = str.Mid(index + key.GetLength());
					buf.Trim();
					buf.Replace(_T("\n"), _T(""));
					data.SetTitle(buf);
					TRACE(_T("%s\n"), data.GetTitle());

					// ＵＲＩ
					buf = util::XmlParser::GetElement(str, 1);
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
					// +1～+2 行目にある
					for( int iInc=1; iInc<=2; iInc++ ) {
						str = html_.GetAt( ++i );

						CString author;
						if( util::GetBetweenSubString( str, L"(", L")", author ) < 0 ) {
							// not found.
							continue;
						}

						data.SetName( author );
						data.SetAuthor( author );
						TRACE(_T("%s\n"), data.GetName());
						break;
					}

					out_.push_back(data);
				}
				else if (str.Find(_T("</table>")) != -1 && dataFind != FALSE) {
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
 * [content] view_bbs.pl 用パーサ
 * 
 * ＢＢＳデータ取得
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

		BOOL findFlag = FALSE;
		BOOL bFound2ndTableEndTag = FALSE;

		const CString& tagBR  = _T("<br>");

		for (int i=180; i<lastLine; i++) {
			CString str = html_.GetAt(i);

			if (findFlag == FALSE) {
				// フラグを発見するまで廻す
				// フラグ：class="h120"

				// "show_friend.pl" つまりトピ作成者のプロフィールリンクがあれば、
				// トピ作成者のユーザ名と投稿日時を取得する。
				if ( util::LineHasStringsNoCase( str, L"show_friend.pl" ) ) {

					parseTopicAuthorAndCreateDate( i, html_, mixi );

					// この行にはないので次の行へ。
					continue;
				}

				if (util::LineHasStringsNoCase( str, L"class=\"h120\"" ) ) {
					// </table>を探す
					findFlag = TRUE;

					// とりあえず改行
					mixi.AddBody(_T("\r\n"));

					// 解析対象(str) のパターンは下記の２通り。
					// ●画像なしパターン1：（1行）
					// <table width="500" border="0" cellspacing="0" cellpadding="5"><tr><td class="h120">
					// <table><tr></tr></table>内容
					// ●画像なしパターン2：（1行）
					// <table width="500" border="0" cellspacing="0" cellpadding="5"><tr><td class="h120">
					// <table><tr></tr></table>内容</td></tr></table>
					// ●画像ありパターン1：（2行）
					// line1: <table width="500" border="0" cellspacing="0" cellpadding="5"><tr><td class="h120"><table>
					//        <tr><td width="130" height="140" align="center" valign="middle">
					//        <a href="javascript:void(0)" onClick="MM_openBrWindow('show_bbs_picture.pl?id=xxx&number=xxx',
					//        'pict','width=680,height=660,toolbar=no,scrollbars=yes,left=5,top=5')">
					//        <img src="http://ic52.mixi.jp/p/xxx.jpg" border="0"></a></td>
					// line2: </tr></table>写真付きトピック</td></tr></table>
					// ●画像ありパターン2：（2行）
					// line1: <table width="500" border="0" cellspacing="0" cellpadding="5"><tr><td class="h120"><table>
					//        <tr><td width="130" height="140" align="center" valign="middle">
					//        <a href="javascript:void(0)" onClick="MM_openBrWindow('show_bbs_picture.pl?id=xxx&number=xxx',
					//        'pict','width=680,height=660,toolbar=no,scrollbars=yes,left=5,top=5')">
					//        <img src="http://ic39.mixi.jp/p/xxx.jpg" border="0"></a></td>
					// line2: </tr></table>本文がちょっと長い
					// ●画像ありパターン3：（3行）
					// line1: <table width="500" border="0" cellspacing="0" cellpadding="5"><tr><td class="h120"><table>
					//        <tr><td width="130" height="140" align="center" valign="middle">
					//        <a href="javascript:void(0)" onClick="MM_openBrWindow('show_bbs_picture.pl?id=xxx&comm_id=xxx&number=xxx',
					//        'pict','width=680,height=660,toolbar=no,scrollbars=yes,left=5,top=5')">
					//        <img src="http://ic57.mixi.jp/p/xxx.jpg" border="0"></a></td>
					// line2: <td width="130" height="140" align="center" valign="middle">
					//        <a href="javascript:void(0)" onClick="MM_openBrWindow('show_bbs_picture.pl?id=xxx&comm_id=xxx&number=xxx',
					//        'pict','width=680,height=660,toolbar=no,scrollbars=yes,left=5,top=5')">
					//        <img src="http://ic43.mixi.jp/p/xxx.jpg" border="0"></a></td>
					// line3: </tr></table>本文表示される？

					// </table> が現れるまで各行をバッファリング。
					// これは、画像が複数あるような場合に、</table> が次の行に含まれるため。

					// 現在の行をバッファリング
					CString line = str;

					// </table> がなければ </table> が見つかるまでバッファリング
					if( !util::LineHasStringsNoCase( line, L"</table>" ) ) {

						while( i<lastLine ) {
							// 次の行をフェッチ
							const CString& nextLine = html_.GetAt( ++i );

							// バッファリング
							line += nextLine;

							// </table> があれば終了
							if( util::LineHasStringsNoCase( nextLine, L"</table>" ) ) {
								break;
							}
						}
					}

					// 最初の </table> までについて、
					// 画像リンクがあれば、解析し、投入。
					{
						CString strBeforeTableEndTag;
						util::GetBeforeSubString( line, L"</table>", strBeforeTableEndTag );
						if( LINE_HAS_DIARY_IMAGE(strBeforeTableEndTag) ) {
							ParserUtil::AddBodyWithExtract( mixi, strBeforeTableEndTag );
						}
					}

					// 最初の </table> 以降を取得。
					CString strAfterTableEndTag;
					util::GetAfterSubString( line, L"</table>", strAfterTableEndTag );

					// 最初の </table> 以降を解析＆投入
					ParserUtil::AddBodyWithExtract( mixi, strAfterTableEndTag );

					// さらに </table> がある(パターン2)なら、終了フラグを立てておく
					if( util::LineHasStringsNoCase( strAfterTableEndTag, L"</table>" ) ) {
						bFound2ndTableEndTag = TRUE;
					}

				}else{
					// フラグがないので読み飛ばす
				}
			}
			else {
				// フラグ発見済み。
				// 最初の </table> が見つかる行までは解析済み。
				// 「既にトピックの終了フラグ（2つ目の</table>タグ）が発見済み」
				// または
				// 「</table> を発見」すればコメント取得処理を行う。

				if( util::LineHasStringsNoCase(str,L"</table>") || bFound2ndTableEndTag ) {
					if (str.Find(tagBR) != -1) {
						str.Replace(_T("</td></tr>"), _T(""));
						str.Replace(_T("</tbody>"), _T(""));
						str.Replace(_T("</table>"), _T(""));
						ParserUtil::AddBodyWithExtract( mixi, str );
					}

					// コメントの開始まで探す
					mixi.ClearChildren();

					int index = i;
					while( index < lastLine ) {
						index = parseBBSComment(index, lastLine, &mixi, html_);
						if( index == -1 ) {
							break;
						}
					}
					if (index == -1 || index >= lastLine) {
						break;
					}
				}else{
					// 2つ目の</table>タグ未発見なので、解析＆投入
					ParserUtil::AddBodyWithExtract( mixi, str );
				}
			}
		}

		// ページ移動リンクの抽出
		parsePageLink( mixi, html_ );

		// 「最新のトピック」の抽出
		parseRecentTopics( mixi, html_ );

		MZ3LOGGER_DEBUG( L"ViewBbsParser.parse() finished." );
		return true;
	}

private:

	/**
	 * i 行目から投稿ユーザ名を取得し、上方向に投稿日時を探索する
	 */
	static bool parseTopicAuthorAndCreateDate( int i, const CHtmlArray& html_, CMixiData& mixi )
	{
		// 投稿ユーザ名
		const CString& line = html_.GetAt( i );
		MixiUrlParser::GetAuthor(line, &mixi);

		// 日付を取得
		// i 行目⇒2行目まで探索する。
		// 通常は3行程度前に下記がある。
		// <td rowspan="3" width="110" bgcolor="#ffd8b0" align="center" valign="top" nowrap>
		// 2007年05月10日<br>19:57</td>
		for (int j=i; j>0; j--) {
			const CString& line = html_.GetAt(j);
			if( util::LineHasStringsNoCase( line, L"年", L"月", L"日" ) ) {
				// TODO XmlParser 使わない方が柔軟では？(takke)
				ParserUtil::ChangeDate(
					util::XmlParser::GetElement(line, 2) + _T(" ") + util::XmlParser::GetElement(line, 4),
					&mixi);
				return true;
			}
		}

		// 未発見のため終了
		return false;
	}

	/**
	 * ＢＢＳコメント取得 トピック コメント一覧
	 */
	static int parseBBSComment(int sIndex, int eIndex, CMixiData* data, const CHtmlArray& html_ ) 
	{
		CString str;

		int retIndex = eIndex;
		CMixiData cmtData;
		BOOL findFlag = FALSE;
		CString date;
		CString comment;
		int index;

		for (int i=sIndex; i<eIndex; i++) {
			str = html_.GetAt(i);

			if (findFlag == FALSE) {
				CString buf;

				if (str.Find(_T("<!-- ADD_COMMENT: start -->")) != -1) {
					// コメントなし
					// 投稿アドレスの取得
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
				else if (str.Find(_T("add_bbs_comment.pl")) != -1) {
					// コメント全体の終了タグ発見
					GetPostAddress(i, eIndex, html_, *data);
					retIndex = -1;
					break;
				}
				//2006/11/19 icchu追加ここまで
				else if ((index = str.Find(_T("show_friend.pl"))) != -1) {

					str = html_.GetAt(i-1);
					util::GetAfterSubString( str, L"<b>", buf );
					buf = util::XmlParser::GetElement(buf, 1);
					while(buf.Replace(_T("&nbsp;"), _T("")));
					cmtData.SetCommentIndex(_wtoi(buf));

					str = html_.GetAt(i);
					buf = str.Mid(index);
					MixiUrlParser::GetAuthor(buf, &cmtData);

					if (html_.GetAt(i-4).Find(_T("checkbox")) != -1) {
						// 自分管理コミュ
						// 時刻
						str = html_.GetAt(i-6);
						buf = util::XmlParser::GetElement(str, 1);
						date = buf;
						str = html_.GetAt(i-5);
						buf = util::XmlParser::GetElement(str, 1);
						date += _T(" ");
						date += buf;
					}
					else {
						if (html_.GetAt(i-5).Find(_T(":")) != -1) {
							// 時刻
							str = html_.GetAt(i-5);
							buf = util::XmlParser::GetElement(str, 1);
							date = buf;
							buf = util::XmlParser::GetElement(str, 3);
							date += _T(" ");
							date += buf;
						}
						else {
							// 時刻
							str = html_.GetAt(i-5);
							buf = util::XmlParser::GetElement(str, 1);
							date = buf;
							str = html_.GetAt(i-4);
							buf = util::XmlParser::GetElement(str, 1);
							date += _T(" ");
							date += buf;
						}
					}

					ParserUtil::ChangeDate(date, &cmtData);

					findFlag = TRUE;

				}
			}
			else {

				if( util::LineHasStringsNoCase(str, L"class=\"h120\"") ) {
					// コメント本文取得
					str = html_.GetAt(i);

					cmtData.AddBody(_T("\r\n"));
					ParserUtil::AddBodyWithExtract( cmtData, str );

					i++;
					for( ; i+1<eIndex; i++ ) {
						const CString& line  = html_.GetAt(i);
						const CString& line2 = html_.GetAt(i+1);

						if (util::LineHasStringsNoCase( line, L"</tr>") && 
							util::LineHasStringsNoCase( line2,L"</table>") )
						{
							// 一番最後の改行を削除
							ParserUtil::AddBodyWithExtract( cmtData, line );

							// 終了タグが出てきたのでここで終わり
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
 * [content] view_enquete.pl 用パーサ
 * 
 * アンケートデータ取得
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
 * 
 * イベントデータ取得
 */
class ViewEventParser : public MixiContentParser
{
public:
	static bool parse( CMixiData& data_, const CHtmlArray& html_ )
	{
		MZ3LOGGER_DEBUG( L"ViewEventParser.parse() start." );

		data_.ClearAllList();

		INT_PTR count;
		count = html_.GetCount();

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
					if( index < 0 ) {
						MZ3LOGGER_ERROR( L"show_friend.pl が見つかりません [" + str + L"]" );
						break;
					}
					buf = str.Mid(index);
					MixiUrlParser::GetAuthor( buf, &data_ );

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
		CString str;

		int retIndex = eIndex;
		CMixiData cmtData;
		BOOL findFlag = FALSE;
		CString date;
		CString comment;
		int index;

		for (int i=sIndex; i<eIndex; i++) {
			str = html_.GetAt(i);

			if (findFlag == FALSE) {

				if( util::LineHasStringsNoCase( str, L"<!-- end : Loop -->" ) ||
					util::LineHasStringsNoCase( str, L"<!-- ADD_COMMENT: start -->" ) ||
					util::LineHasStringsNoCase( str, L"<!-- COMMENT: end -->" ) ||
					util::LineHasStringsNoCase( str, L"add_event_comment.pl" ) ) 
				{
					// コメント全体の終了タグ発見
					GetPostAddress(i, eIndex, html_, *data);
					return -1;
				}
				
				if ((index = str.Find(_T("show_friend.pl"))) != -1) {

					CString buf = str.Mid(index);
					MixiUrlParser::GetAuthor(buf, &cmtData);

					// コメント番号を前の行から取得する
					// [<font color="#f8a448"><b>&nbsp;25</b>&nbsp;:</font>]
					str = html_.GetAt(i-1);

					if( util::GetBetweenSubString( str, L"<b>", L"</b>", buf ) < 0 ) {
						MZ3LOGGER_ERROR( L"コメント番号が取得できません。mixi 仕様変更？" );
						return -1;
					}

					while(buf.Replace(_T("&nbsp;"), _T("")));
					cmtData.SetCommentIndex(_wtoi(buf));

					// 時刻
					if (html_.GetAt(i-4).Find(_T("checkbox")) != -1) {
						// 自分管理コミュ
						date = util::XmlParser::GetElement(html_.GetAt(i-6), 1) + _T(" ")
							+ util::XmlParser::GetElement(html_.GetAt(i-5), 1);
						ParserUtil::ChangeDate(date, &cmtData);
					}
					else {
						date = util::XmlParser::GetElement(html_.GetAt(i-5), 1) + _T(" ")
							+ util::XmlParser::GetElement(html_.GetAt(i-4), 1);
						ParserUtil::ChangeDate(date, &cmtData);
					}

					findFlag = TRUE;
				}
			}
			else {

				// コメントコメント本文取得

				if( util::LineHasStringsNoCase( str, L"<td", L"class=h120" ) ||
					util::LineHasStringsNoCase( str, L"<td", L"class=\"h120\"" ) )
				{
					cmtData.AddBody(_T("\r\n"));

					// [<tr><td class="h120" width="500">てすと</td></tr>]

					ParserUtil::AddBodyWithExtract( cmtData, str );

					CString line2 = html_.GetAt(i+1);
					if( util::LineHasStringsNoCase( str,   L"</td>" ) &&
						util::LineHasStringsNoCase( line2, L"</table>" ) )
					{
						// 終了タグがあった場合
						retIndex = i + 5;
					}else{
						// 終了タグ未発見の場合：
						// 終了タグが現れるまで解析＆追加。
						while( i<eIndex ) {
							i++;
							const CString& line = html_.GetAt(i);

							CString buf;
							if( util::GetBeforeSubString( line, L"</td>", buf ) >= 0 ||
								util::GetBeforeSubString( line, L"</table>", buf ) >= 0 ) 
							{
								// 終了タグ発見
								ParserUtil::AddBodyWithExtract( cmtData, buf );
								retIndex = i + 5;
								break;
							}

							ParserUtil::AddBodyWithExtract( cmtData, line );
						}
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
 * [content] view_message.pl 用パーサ
 * 
 * メッセージ取得
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

/**
 * [list] show_log.pl 用パーサ
 * 
 * 足あとの取得
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
				// <a href="～"> で解析
				CString url;
				int pos = util::GetBetweenSubString( target, L"<a href=\"", L"\">", url );
				if( pos < 0 ) 
					continue;

				// 日付を抽出
				// ">～<a で解析
				target = str.Left( pos );
				CString strDate;
				pos = util::GetBetweenSubString( target, L"\">", L"<a", strDate );
				if( pos < 0 ) 
					continue;

				strDate.Trim();

				// 名前
				// ">～</a で解析
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
 * [list] list_diary.pl 用パーサ
 * 
 * 自分の日記一覧取得
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

		for (int i=200; i<count; i++) {
			const CString& str = html_.GetAt(i);

			if( findFlag ) {
				if( str.Find(_T("</td></tr></table>")) != -1 ) {
					// 終了
					break;
				}

				// 「前を表示」「次を表示」の抽出
				LPCTSTR key = L"=#EED6B5>";
				if( str.Find( key ) != -1 ) {
					// リンクっぽいので正規表現マッチングで抽出
					if( parseNextBackLink( nextLink, backLink, str ) ) {
						continue;
					}
				}
			}

			// 名前の取得
			// "bg_w.gif" があり、"<font COLOR=#605048>" があり、"の日記" がある行。
			if (str.Find(_T("bg_w.gif")) != -1 && str.Find(_T("の日記")) != -1) {
				// "<font COLOR=#605048>" と "の日記" で囲まれた部分を抽出する
				CString name;
				util::GetBetweenSubString( str, L"<font COLOR=#605048>", L"の日記", name );

				// 名前
				data.SetName( name );
				data.SetAuthor( name );
			}

			const CString& key = _T("<td bgcolor=\"#FFF4E0\">&nbsp;");
			if (str.Find(key) != -1) {
				findFlag = TRUE;

				// タイトル
				// 解析対象：
				// <td bgcolor="#FFF4E0">&nbsp;<a href="view_diary.pl?id=xxx&owner_id=xxx">タイトル</a></td>
				CString buf;
				util::GetBetweenSubString( str, key, L"</td>", buf );

				CString title;
				util::GetBetweenSubString( buf, L">", L"</", title );
				data.SetTitle( title );

				// 日付
				const CString& str = html_.GetAt(i-1);
        //2007/06/20 いっちゅう日記投稿不具合対応
				ParserUtil::ChangeDate(util::XmlParser::GetElement(str, 5), util::XmlParser::GetElement(str, 7), &data);

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
 * 
 * コメント一覧の取得
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
 * [content] Readme.txt 用パーサ
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
 * [content] home.pl ログイン後のメイン画面用パーサ
 */
class HomeParser : public MixiContentParser
{
public:
	/**
	 * ログイン判定
	 *
	 * ログイン成功したかどうかを判定
	 *
	 * @return ログイン成功時はTRUE、失敗時はFALSEを返す
	 */
	static bool IsLoginSucceeded( const CHtmlArray& html )
	{
		INT_PTR count = html.GetCount();

		for (int i=0; i<count; i++) {
			const CString& line = html.GetAt(i);

			if (util::LineHasStringsNoCase( line, L"refresh", L"home.pl" )) {
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
	// 		NLED_SETTINGS_INFO led;
	//		led.LedNum = ::NLedSetDevice(0, 
		}

		// 新着コメント数の取得
		int commentNum = GetNewCommentCount( html_, 100, index, index);
		if (commentNum != 0) {
			theApp.m_newCommentCount = commentNum;
		}

		if (!wcscmp(theApp.m_loginMng.GetOwnerID(), _T(""))) {
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

/**
 * home.pl ログイン画面用パーサ
 */
class LoginPageParser : public MixiParserBase
{
public:
	/**
	 * ログアウトしたかをチェックする
	 */
	static bool isLogout( LPCTSTR szHtmlFilename )
	{
		// 最大で 30 行目までチェックする
		const int CHECK_LINE_NUM_MAX = 30;

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
		 * <img SRC="http://ic39.mixi.jp/p/xxx/xxx/diary/xx/x/xxx.jpg" BORDER=0>
		 */
		CString uri;
		for (int i=0; i<count; i++) {
			// 画像へのリンクを抽出
			if( util::GetBetweenSubString( html.GetAt(i), _T("<img SRC=\""), _T("\" BORDER=0>"), uri ) > 0 ) {
				break;
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
	}
}

}//namespace mixi

