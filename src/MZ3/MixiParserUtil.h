/*
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */
#pragma once

#include "MyRegex.h"
#include "HtmlArray.h"
#include "util_base.h"
#include "util_mixi.h"
#include "util_mz3.h"

/// mixi 用HTMLパーサ
namespace mixi {

/// パーサ用ユーティリティ群
class ParserUtil {
public:
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
		data->SetAuthorID( _wtoi(id) );

		// 名前抽出
		CString name;
		if( util::GetBetweenSubString( target, L">", L"<", name ) == -1 ) {
			CString msg;
			msg.Format( L"引数が '>', '<' を含みません str[%s]", str );
			MZ3LOGGER_ERROR(msg);
			return false;
		}
		// デコード
		mixi::ParserUtil::ReplaceEntityReferenceToCharacter( name );
		data->SetAuthor( name );

		return true;
	}

	/**
	 * URL に最後にアクセスしたときの、投稿件数を取得する
	 */
	static int GetLastIndexFromIniFile(LPCTSTR uri, const CMixiData& mixi)
	{
		// URIを分解
		CString strLogId = util::GetLogIdString(mixi);
		std::string idKey = util::my_wcstombs( (LPCTSTR)strLogId );

		// キー取得
		int lastIndex = -1;

		if (theApp.m_logfile.RecordExists(idKey.c_str(), "Log") ) {
			// レコードが在ればそれを使う
			lastIndex = atoi( theApp.m_logfile.GetValue(idKey.c_str(), "Log").c_str() );
		}
		else {
			lastIndex = -1;
		}

		return lastIndex;
	}

	/**
	 * URL に最後にアクセスしたときの、投稿件数を取得する
	 */
	static int GetLastIndexFromIniFile(const CMixiData& mixi)
	{
		return GetLastIndexFromIniFile( mixi.GetURL(), mixi );
	}

	/**
	 * 時刻変換(日付・時刻っぽい文字列を抽出し、mixi にSetDateする)
	 *
	 * @param line [in]  日時文字列を含む文字列。
	 *                   例："2006年11月19日 17:12"
	 *                       "<span class="date">2007年07月05日 21:55</span></dt>"
	 *                       "<td>10月08日</td></tr>"
	 * @param mixi [out] 解析結果を SetDate で保存する。
	 */
	static void ParseDate(LPCTSTR line, CMixiData& mixi)
	{
		// 汎用 形式
		{
			// 正規表現のコンパイル
			static MyRegex reg;
			if( !util::CompileRegex( reg, L"([0-9]{2,4})?年?([0-9]{1,2}?)月([0-9]{1,2})日[^0-9]*([0-9]{1,2})?:?時?([0-9]{2})?" ) ) {
				return;
			}
			// 検索
			if( reg.exec(line) && reg.results.size() == 6 ) {
				// 抽出
				int year   = _wtoi( reg.results[1].str.c_str() );
				int month  = _wtoi( reg.results[2].str.c_str() );
				int day    = _wtoi( reg.results[3].str.c_str() );
				int hour   = _wtoi( reg.results[4].str.c_str() );
				int minute = _wtoi( reg.results[5].str.c_str() );
				mixi.SetDate(year, month, day, hour, minute);
				return;
			}
		}

		// RSS 形式 (YYYY-MM-DDT00:00:00Z)
		{
			// 正規表現のコンパイル
			static MyRegex reg;
			if( !util::CompileRegex( reg, L"([0-9]{4})-([0-9]{2})-([0-9]{2})T([0-9]{2}):([0-9]{2}):([0-9]{2})Z" ) ) {
				return;
			}
			// 検索
			if( reg.exec(line) && reg.results.size() == 7 ) {
				// 抽出
				int year   = _wtoi( reg.results[1].str.c_str() );
				int month  = _wtoi( reg.results[2].str.c_str() );
				int day    = _wtoi( reg.results[3].str.c_str() );
				int hour   = _wtoi( reg.results[4].str.c_str() );
				int minute = _wtoi( reg.results[5].str.c_str() );

				CTime t(year, month, day, hour, minute, 0);
				t += CTimeSpan(0, 9, 0, 0);

				mixi.SetDate(t.GetYear(), t.GetMonth(), t.GetDay(), t.GetHour(), t.GetMinute());
				return;
			}
		}

		// RSS 形式 (Sun Dec 16 09:00:00 +0000 2007)
		{
			// 正規表現のコンパイル
			static MyRegex reg;
			if( !util::CompileRegex( reg, L"([a-zA-Z]{3}) ([a-zA-Z]{3}) ([0-9]{2}) ([0-9]{2}):([0-9]{2}):([0-9]{2}) \\+([0-9]{4}) ([0-9]{4})" ) ) {
				return;
			}
			// 検索
			if( reg.exec(line) && reg.results.size() == 9 ) {
				// 抽出

				// 年
				int year   = _wtoi( reg.results[8].str.c_str() );

				// 月
				CString strMonth = reg.results[2].str.c_str();
				int month = 1;
				if (strMonth == L"Jan")			month = 1;
				else if (strMonth == L"Feb")	month = 2;
				else if (strMonth == L"Mar")	month = 3;
				else if (strMonth == L"Apr")	month = 4;
				else if (strMonth == L"May")	month = 5;
				else if (strMonth == L"Jun")	month = 6;
				else if (strMonth == L"Jul")	month = 7;
				else if (strMonth == L"Aug")	month = 8;
				else if (strMonth == L"Sep")	month = 9;
				else if (strMonth == L"Oct")	month = 10;
				else if (strMonth == L"Nov")	month = 11;
				else if (strMonth == L"Dec")	month = 12;
				else {
					// ??
				}

				int day    = _wtoi( reg.results[3].str.c_str() );
				int hour   = _wtoi( reg.results[4].str.c_str() );
				int minute = _wtoi( reg.results[5].str.c_str() );
				int sec    = _wtoi( reg.results[6].str.c_str() );

				CTime t(year, month, day, hour, minute, sec);
				t += CTimeSpan(0, 9, 0, 0);

				mixi.SetDate(t.GetYear(), t.GetMonth(), t.GetDay(), t.GetHour(), t.GetMinute());
				return;
			}
		}

		CString msg = L"文字列内に日付・時刻が見つかりません : [";
		msg += line;
		msg += L"]";
		MZ3LOGGER_DEBUG( msg );
		return;
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
		// 改行コードの除去
		while( line.Replace(_T("\n"), _T("")) ) ;

		// table タグの除去
		{
			static MyRegex reg;
			if( !util::CompileRegex( reg, L"<table[^>]*>" ) ) {
				return;
			}
			if( line.Find( L"<table" ) != -1 ) 
				reg.replaceAll( line, L"" );
		}
		while( line.Replace(_T("</table>"), _T("")) );

		// tr タグの除去
		{
			static MyRegex reg;
			if( !util::CompileRegex( reg, L"<tr[^>]*>" ) ) {
				return;
			}
			if( line.Find( L"<tr" ) != -1 ) 
				reg.replaceAll( line, L"" );
		}
		while( line.Replace(_T("</tr>"), _T("")) );

		// td タグの除去
		{
			static MyRegex reg;
			if( !util::CompileRegex( reg, L"<td[^>]*>" ) ) {
				return;
			}
			if( line.Find( L"<td" ) != -1 ) 
				reg.replaceAll( line, L"" );
		}
		while( line.Replace(_T("</td>"), _T("")) );

		// div タグの除去
		{
			static MyRegex reg;
			if( !util::CompileRegex( reg, L"<div[^>]*>" ) ) {
				return;
			}
			if( line.Find( L"<div" ) != -1 ) 
				reg.replaceAll( line, L"" );
		}
		while( line.Replace(_T("</div>"), _T("")) );

		// span タグの除去
		{
			static MyRegex reg;
			if( !util::CompileRegex( reg, L"<span[^>]*>" ) ) {
				return;
			}
			if( line.Find( L"<span" ) != -1 ) 
				reg.replaceAll( line, L"" );
		}
		while( line.Replace(_T("</span>"), _T("")) );

		// br タグの置換
		{
			static MyRegex reg;
			if( !util::CompileRegex( reg, L"<br[^>]*?>" ) ) {
				return;
			}
			if( line.Find( L"<br" ) != -1 ) 
				reg.replaceAll( line, L"\r\n" );
		}

		// コメントの置換
		{
			static MyRegex reg;
			if( !util::CompileRegex( reg, L"<!--.*?-->" ) ) {
				return;
			}
			if( line.Find( L"<!--" ) != -1 ) 
				reg.replaceAll( line, L"" );
		}

		if (theApp.m_optionMng.m_bRenderByIE) {
			return;
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
		if( !util::CompileRegex( reg, L"&#([0-9]{3,5});" ) ) {
			return;
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
			ReplaceEmojiImageToCode( str );
		}

		// その他の画像リンクの変換
		if( util::LineHasStringsNoCase( str, L"<img", L"src=" ) ) {
			ExtractGeneralImageLink( str, *data );
		}

		// 動画ファイルの抽出
		if (str.Find( L".flv" ) != -1) {
			ExtractGeneralVideoLink( str, *data );
		}

		// Youtube 動画の抽出(下記が1行で存在する)
		//youtube_write('<div class="insert_video"><object width="450" height="373"><param name="movie" value="
		//http://www.youtube.com/v/xxx"></param><param name="wmode" value="transparent"></param>
		//<embed src="http://www.youtube.com/v/xxx" type="application/x-shockwave-flash" wmode="transparent" 
		//width="450" height="373"></embed></object></div>');
#define LINE_HAS_YOUTUBE_LINK(line)		util::LineHasStringsNoCase( line, L"<embed", L"src=", L"youtube.com" )
		if (LINE_HAS_YOUTUBE_LINK(str)) {
			ExtractYoutubeVideoLink( str, *data );
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

	/**
	 * 動画リンクの抽出
	 *
	 * <script> タグを抽出するため、複数行のパースを行う。
	 * iLine_ パラメータが参照である点に注意！
	 */
	static bool ExtractVideoLinkFromScriptTag( CMixiData& mixi_, int& iLine_, const CHtmlArray& html_ )
	{
		const int lastLine = html_.GetCount();
		if( iLine_>=lastLine ) {
			return false;
		}

		// 動画用Scriptタグが見つかったらスクリプト用ループ開始
		const CString& line = html_.GetAt( iLine_ );
		if( util::LineHasStringsNoCase( line, L"<script") ) {
			while( iLine_<lastLine ) {
				// 次の行をフェッチ
				const CString& nextLine = html_.GetAt( ++iLine_ );
				// 拡張子.flvが見つかったら投入
				if( util::LineHasStringsNoCase( nextLine, L".flv" ) ) {
					ParserUtil::AddBodyWithExtract( mixi_, nextLine );
				}
				// Youtube対応
				if( LINE_HAS_YOUTUBE_LINK(nextLine) ) {
					ParserUtil::AddBodyWithExtract( mixi_, nextLine );
				}
				// </script> があれば終了
				if( util::LineHasStringsNoCase( nextLine, L"</script>" ) ) {
					break;
				}
			}
			return true;
		} else {
			return false;
		}
	}

private:

	/**
	 * 絵文字画像リンクの変換。
	 *
	 * [m:xxx] 文字列に置換する。
	 */
	static void ReplaceEmojiImageToCode( CString& line )
	{
		// <img src="http://img.mixi.jp/img/emoji/85.gif" alt="喫煙" width="16" height="16" class="emoji" border="0">
		// のようなリンクを
		// "((喫煙))" に変換する

		// 正規表現のコンパイル（一回のみ）
		static MyRegex reg;
		if (!util::CompileRegex( reg, L"<img src=\"http://img.mixi.jp/img/emoji/([^\"]+).gif\" alt=\"([^\"]+)\" [^c]+ class=\"emoji\"[^>]*>" )) {
			return;
		}

		// ((喫煙)) に変換する
//		reg.replaceAll( line, L"(({2}))" );

		// [m:xx] 形式に置換する
		reg.replaceAll( line, L"[m:{1}]" );
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
			if( util::GetBeforeSubString( buf, tag_AlbumBegin, left ) == -1 ) {
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
			MZ3LOGGER_DEBUG( L"アルバム画像URL [" + url_image + L"]" );

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
/*
<td><a href="javascript:void(0);" onClick="MM_openBrWindow('show_diary_picture.pl?
owner_id=zzz&id=zzz&number=zz','pict','width=680,height=660,toolbar=no,scrollbars=
yes,left=5,top=5');"><img src="http://ic77.mixi.jp/p/zzz/zzz/diary/zz/zz/zzs.jpg"
alt="" /></a></td>
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
		if( !util::CompileRegex( reg, L"<img[^>]*src=\"([^\"]+)\" [^>]*>" ) ) {
			return;
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

			// class="emoji" が含まれていれば、絵文字と判断し、無視する。
			if( util::LineHasStringsNoCase( results[0].str.c_str(), L"class=\"emoji\"" ) ) {
				line.Append( results[0].str.c_str() );
			} else {
				CString text = L"<<画像>>";
				// url を追加
				LPCTSTR url = results[1].str.c_str();
				data_.m_linkList.push_back( CMixiData::Link(url, text) );

				// 置換
				line.Append( text );

				// とりあえず改行
				line += _T("<br>");
			}

			// ターゲットを更新。
			target.Delete( 0, results[0].end );
		}

	}

	/**
	 * 動画変換。
	 *
	 * line から動画リンクを抽出し、そのリンクを data に追加する。
	 * また、line から該当する動画リンクを削除する。
	 */
	static void ExtractGeneralVideoLink(CString& line, CMixiData& data_)
	{

		// 正規表現のコンパイル（一回のみ）
		static MyRegex reg;
		if( !util::CompileRegex( reg, L".*video *: '([^']+)',.*" ) ) {
			MZ3LOGGER_FATAL( FAILED_TO_COMPILE_REGEX_MSG );
			return;
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

			CString text = L"<<動画>>";

			// url を追加
			LPCTSTR url = results[1].str.c_str();
			
			//動画を追加
			data_.AddMovie( url );

			// 置換
			line.Append( text );

			// とりあえず改行
			line += _T("<br>");

			// ターゲットを更新。
			target.Delete( 0, results[0].end );
		}
	}

	/**
	 * Youtube 動画変換。
	 *
	 * line から動画リンクを抽出し、そのリンクを data に追加する。
	 * また、line から該当する動画リンクを削除する。
	 */
	static void ExtractYoutubeVideoLink(CString& line, CMixiData& data_)
	{

		// 正規表現のコンパイル（一回のみ）
		static MyRegex reg;
		if( !util::CompileRegex( reg, L"youtube_write.*src=\"(.*?)\".*?;" ) ) {
			MZ3LOGGER_FATAL( FAILED_TO_COMPILE_REGEX_MSG );
			return;
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

			CString text = L"<<Youtube動画>>";

			// url を追加
			LPCTSTR url = results[1].str.c_str();
			
			//動画を追加
			data_.m_linkList.push_back( CMixiData::Link(url,url) );

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
		while( str.Replace(_T("<wbr />"), _T("")) );

		// 正規表現のコンパイル（一回のみ）
		static MyRegex reg;
		if( !util::CompileRegex( reg, L"<a href='([^']+)'[^>]*>([^<]*)</a>" ) ) {
			MZ3LOGGER_FATAL( FAILED_TO_COMPILE_REGEX_MSG );
			return;
		}
		static MyRegex reg2;
		if( !util::CompileRegex( reg2, L"<a href=\"([^\"]+)\"[^>]*>([^<]*)</a>" ) ) {
			MZ3LOGGER_FATAL( FAILED_TO_COMPILE_REGEX_MSG );
			return;
		}
		static MyRegex reg3;
		if( !util::CompileRegex( reg3, L"<a href=([^>]+)>([^<]*)</a>" ) ) {
			MZ3LOGGER_FATAL( FAILED_TO_COMPILE_REGEX_MSG );
			return;
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
		if( !util::CompileRegex( reg, L"[^h](ttps?://[-_.!~*'()a-zA-Z0-9;/?:@&=+$,%#]+)" ) ) {
			MZ3LOGGER_FATAL( FAILED_TO_COMPILE_REGEX_MSG );
			return;
		}

		CString target = str;
		for( int i=0; i<100; i++ ) {	// 100 は無限ループ防止
			if( reg.exec(target) == false || reg.results.size() != 2 ) {
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

}