/*
 * Copyright (c) MZ3 Project Team All rights reserved.
 * Code licensed under the BSD License:
 * http://www.mz3.jp/license.txt
 */
#pragma once

#include "MZ3.h"
#include "constants.h"
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
	 * URL に最後にアクセスしたときの、投稿件数を取得する
	 */
	static int GetLastIndexFromIniFile(LPCTSTR uri, const CMixiData& mixi)
	{
		// URIを分解
		CString strLogId = util::GetLogIdString(mixi);
		std::string idKey = util::my_wcstombs( (LPCTSTR)strLogId );

		// キー取得
		int lastIndex = -1;

		if (theApp.m_readlog.RecordExists(idKey.c_str(), "Log") ) {
			// レコードが在ればそれを使う
			lastIndex = atoi( theApp.m_readlog.GetValue(idKey.c_str(), "Log").c_str() );
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

	static bool ParseDate(LPCTSTR line, CMixiData& mixi);
	static bool ParseDate(LPCTSTR line, CTime& t_result);

	/**
	 * 3文字の「月」文字列を数値に変換する。
	 *
	 * "Jan" の場合は数値 1 を返す。
	 * 未解決時は 1 を返す。
	 */
	static int ThreeCharMonthToInteger( const CString& strMonth )
	{
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
		return month;
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

		// "<p>" → 削除
		while( line.Replace(_T("<p>"), _T("")) ) ;

		// </p> → 改行
		while( line.Replace(_T("</p>"), _T("\r\n\r\n")) ) ;

		// 実体参照の文字化
		ReplaceEntityReferenceToCharacter(line);

	}

	static void ReplaceEntityReferenceToCharacter( CString& str )
	{
		// &quot; などの定義済み実体参照の文字化
		ReplaceDefinedEntityReferenceToCharacter( str );

		// &#xxxx; の実体参照の文字化
		// 例）&#3642; → char(3642)
		if (str.Find(L"&#") >= 0) {
			ReplaceNumberEntityReferenceToCharacter( str );
			ReplaceNumberEntityReferenceToCharacter2( str );
		}
	}

	/**
	 * &quot; などの定義済み実体参照の文字化
	 */
	static void ReplaceDefinedEntityReferenceToCharacter( CString& str )
	{
		str.Replace(_T("&quot;"), _T("\""));
		str.Replace(_T("&gt;"),   _T(">"));
		str.Replace(_T("&lt;"),   _T("<"));
		str.Replace(_T("&nbsp;"), _T(" "));
		str.Replace(_T("&amp;"),  _T("&"));
	}

	/**
	 * &#xxxx; の実体参照の文字化
	 * 例）&#3642; → char(3642)
	 */
	static void ReplaceNumberEntityReferenceToCharacter( CString& str )
	{
		// 正規表現のコンパイル（一回のみ）
		static MyRegex reg;
		if( !util::CompileRegex( reg, L"&#([0-9]{2,5});" ) ) {
			return;
		}

		CString target = str;
		str = L"";
		for( int i=0; i<MZ3_INFINITE_LOOP_MAX_COUNT; i++ ) {	// MZ3_INFINITE_LOOP_MAX_COUNT は無限ループ防止
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
	 * &#xxxxx; の実体参照の文字化
	 * 例）&#x3642; → char(3642)
	 */
	static void ReplaceNumberEntityReferenceToCharacter2( CString& str )
	{
		// 正規表現のコンパイル（一回のみ）
		static MyRegex reg;
		if( !util::CompileRegex( reg, L"&#x([0-9A-F]{4});" ) ) {
			return;
		}

		CString target = str;
		str = L"";
		for( int i=0; i<MZ3_INFINITE_LOOP_MAX_COUNT; i++ ) {	// MZ3_INFINITE_LOOP_MAX_COUNT は無限ループ防止
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
				// hex => dec
				int d = 0;
				if (swscanf(reg.results[1].str.c_str(), L"%X", &d)==1) {
					str.AppendFormat( L"%c", d );
				}
//				MZ3LOGGER_DEBUG(util::FormatString(L"%s : %d\n", reg.results[1].str.c_str(), d));
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
		// <img src="http://img.mixi.net/img/emoji/85.gif" alt="喫煙" width="16" height="16" class="emoji" border="0">
#define LINE_HAS_EMOJI_LINK(str)	util::LineHasStringsNoCase( str, L"<img", L"alt=", L"class=\"emoji\"", L">" )
		if( LINE_HAS_EMOJI_LINK(str) ) {
			ReplaceEmojiImageToCode( str );
		}

		// その他の画像リンクの変換
		//if( util::LineHasStringsNoCase( str, L"<img", L"src=" ) ) {
		//	ExtractGeneralImageLink( str, *data );
		//}

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
		// GoogleMapの抽出(下記が複数行で存在する)
		//map.push({
		//  map_id : "map_1",
		//	size   : { width : 480, height : 480 },
		//  url    : { embed : "http://maps.google.co.jp/maps?spn=xxxx,xxxx&ie=UTF8&ll=xx,xx&output=embed&z=15&s=xx", link : "http://maps.google.co.jp/maps?source=embed&spn=xxx,xxx&ie=UTF8&ll=xxx,xxx&z=15" }
		//});
#define LINE_HAS_GOOGLEMAP_LINK(line)	util::LineHasStringsNoCase( line, L"url", L"embed", L"maps.google.co.jp" )
		// ニコニコ動画リンクの抽出(下記が1行で存在する)
		//<script type="text/javascript" src="http://ext.nicovideo.jp/thumb_watch/smxxxxxxxx?w=450&h=357&n=1" charset="utf-8"></script>
#define LINE_HAS_NICOVIDEO_LINK(line)	util::LineHasStringsNoCase( line, L"<script", L"src=", L"ext.nicovideo.jp" )

		// リンクの抽出
		if ( ( str.Find( L"href" ) != -1 ) ||
			 (str.Find( L"ttp://" ) != -1) ||
			 LINE_HAS_YOUTUBE_LINK(str) ||
			 LINE_HAS_GOOGLEMAP_LINK(str) ||
			 LINE_HAS_NICOVIDEO_LINK(str) ||
			 ( util::LineHasStringsNoCase( str, L"<img", L"src=" ) ) ) {
			ExtractURI( str, *data );
		}

		// 2ch 形式のリンク抽出
		//if( str.Find( L"ttp://" ) != -1 ) {
		//	//Extract2chURL( str, *data );
		//}
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
				// 行をフェッチ
				const CString& nextLine = html_.GetAt( iLine_ );
				// 拡張子.flvが見つかったら投入
				if( util::LineHasStringsNoCase( nextLine, L".flv" ) ) {
					ParserUtil::AddBodyWithExtract( mixi_, nextLine );
				}
				// Youtube対応
				if( LINE_HAS_YOUTUBE_LINK(nextLine) ) {
					ParserUtil::AddBodyWithExtract( mixi_, nextLine );
				}
				// ニコニコ動画対応
				if( LINE_HAS_NICOVIDEO_LINK(nextLine) ) {
					ParserUtil::AddBodyWithExtract( mixi_, nextLine );
				}
				// </script> があれば終了
				if( util::LineHasStringsNoCase( nextLine, L"</script>" ) ) {
					// </script>の後ろに何かあればBodyに追加しておく
					CString scripttag = L"</script>";
					int idx = nextLine.Find(scripttag);
					if( idx>=0 ){
						idx += scripttag.GetLength();
						if( nextLine.GetLength() > idx ){
							ParserUtil::AddBodyWithExtract( mixi_, nextLine.Mid( idx ) );
						}
					}
					break;
				}
				// 次の行へ進める
				++iLine_;
			}
			return true;
		} else {
			return false;
		}
	}

	/**
	 * 絵文字画像リンクの変換
	 *
	 * ((文字)) 文字列に置換する。
	 */
	static void ReplaceEmojiImageToText( CString& line )
	{
		// <img src="http://img.mixi.net/img/emoji/xx.gif" alt="文字" width="16" height="16" class="emoji" border="0">
		// のようなリンクを
		// "((文字))" に変換する

		// 正規表現のコンパイル（一回のみ）
		static MyRegex reg;
		if (!util::CompileRegex( reg, L"<img src=\"http://img.mixi.net/img/emoji/([^\"]+).gif\" alt=\"([^\"]+)\" [^c]+ class=\"emoji\"[^>]*>" )) {
			return;
		}

		// ((文字)) に変換する
		reg.replaceAll( line, L"(({2}))" );

	}

	/**
	 * 件数文字列の分割
	 *
	 * 文字列の最後が(数字)ならばその前までの文字列と(数字)文字列に分割する。
	 */
	static void SepalateCountString( const CString strTarget , CString& strLeft , CString& strRight )
	{
		// 文字列の最後が"(数字)"で終わっていれば分離する
		strLeft = strTarget;
		strRight = L"";
		if ( strTarget.Right(1) == ")" && strTarget.Find(L"(") >= 0) {
			// 正規表現のコンパイル（一回のみ）
			static MyRegex reg;
			util::CompileRegex(reg, L"(\\([0-9]+\\))$");

			std::vector<MyRegex::Result>* pResults = NULL;
			if (reg.exec(strTarget) == true && reg.results.size() == 2) {
				strLeft = strTarget.Left( reg.results[1].start );
				strRight = reg.results[1].str.c_str();
			}
		}

	}

private:

	/**
	 * 絵文字画像リンクの変換
	 *
	 * [m:xxx] 文字列に置換する。
	 */
	static void ReplaceEmojiImageToCode( CString& line )
	{
		// <img src="http://img.mixi.net/img/emoji/85.gif" alt="喫煙" width="16" height="16" class="emoji" border="0">
		// のようなリンクを
		// "((喫煙))" に変換する

		// 正規表現のコンパイル（一回のみ）
		static MyRegex reg;
		if (!util::CompileRegex( reg, L"<img src=\"http://img.mixi.net/img/emoji/([^\"]+).gif\" alt=\"([^\"]+)\" [^c]+ class=\"emoji\"[^>]*>" )) {
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
		for( int i=0; i<MZ3_INFINITE_LOOP_MAX_COUNT; i++ ) {	// MZ3_INFINITE_LOOP_MAX_COUNT は無限ループ防止

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
			ret.AppendFormat( L"<_img><<画像%02d(album)>></_img>", data_.GetImageCount() );
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
	 * 日記、トピック等の画像変換
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

		for( int i=0; i<MZ3_INFINITE_LOOP_MAX_COUNT; i++ ) {	// MZ3_INFINITE_LOOP_MAX_COUNT は無限ループ防止
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
				line.AppendFormat( L"<_img><<画像%02d>></_img><br>", data_.GetImageCount() );

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
	 * 動画変換
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

		static MyRegex reg2;
		if( !util::CompileRegex( reg2, L"player\\.push\\(\\{([^\\)]+)\\}\\);" ) ) {
			MZ3LOGGER_FATAL( FAILED_TO_COMPILE_REGEX_MSG );
			return;
		}

		static MyRegex reg3;
		if( !util::CompileRegex( reg3, L"Event\\.observe\\(.*\\)" ) ) {
			MZ3LOGGER_FATAL( FAILED_TO_COMPILE_REGEX_MSG );
			return;
		}

		CString target = line;
		line = L"";
		for( int i=0; i<MZ3_INFINITE_LOOP_MAX_COUNT; i++ ) {	// MZ3_INFINITE_LOOP_MAX_COUNT は無限ループ防止
			// player.push({云々});を探す
			if( !reg2.exec(target) || reg2.results.size() != 2 ) {
				// 未発見。
				// 文字列の中で動画リンクをさがす
				if( reg.exec( target ) && reg.results.size() == 2 ) {
					// 発見
					std::vector<MyRegex::Result>& results = reg.results;

					// マッチ文字列全体の左側を出力
					line.Append( target, results[0].start );

					// 動画リンクの表示
					CString text = L"<<動画>>";

					// url を追加
					LPCTSTR url = results[1].str.c_str();
					
					//動画を追加
					data_.AddMovie( url );

					// 置換ｆ
					line.Append( L"<_mov>" + text + L"</_mov>" );

					// とりあえず改行
					line += _T("<br>");

					// ターゲットを更新。
					target.Delete( 0, results[0].end );
				} else {
					// 未発見
					line += target;
					break;
				}

			} else {
				// 発見。
				std::vector<MyRegex::Result>& results2 = reg2.results;

				// マッチ文字列全体の左側を出力
				line.Append( target, results2[0].start );

				// マッチ文字列の中で動画リンクをさがす
				if( reg.exec( results2[1].str.c_str() ) && reg.results.size() == 2 ) {
					// 発見
					std::vector<MyRegex::Result>& results = reg.results;

					// 動画リンクの表示
					CString text = L"<<動画>>";

					// url を追加
					LPCTSTR url = results[1].str.c_str();
					
					//動画を追加
					data_.AddMovie( url );

					// 置換ｆ
					line.Append( L"<_mov>" + text + L"</_mov>" );

					// とりあえず改行
					line += _T("<br>");
				} else {
					// 未発見
					line += results2[1].str.c_str();
				}

				// ターゲットを更新。
				target.Delete( 0, results2[0].end );
			}

			// Event.observe(云々) があれば消す
			if( reg3.exec(target) && reg3.results.size() != 0 ) {
				// 発見。
				std::vector<MyRegex::Result>& results3 = reg3.results;
				// ターゲットを更新。
				target.Delete( results3[0].start , results3[0].end );
			}


		}
	}

public:
	/**
	 * 全てのリンク文字列（<a...>XXX</a>）を、文字列のみ（XXX）に変更する
	 * また、href="url" を list_ に追加する。
	 * さらに、2ch 形式のURL(ttp://...)も抽出し、正規化して data のリンクリストに追加する。
	 * YouTube動画リンクも抽出する
	 * GoogleMapリンクも抽出する
	 * <img>タグからの画像リンクも抽出する
	 */
	static void ExtractURI( CString& str, std::vector<CMixiData::Link>& list_ )
	{
		TRACE( L"ExtractURI:str[%s]\n", (LPCTSTR)str );

		// <wbr/> タグの除去
		while( str.Replace(_T("<wbr/>"), _T("")) );
		while( str.Replace(_T("<wbr />"), _T("")) );

		// 正規表現のコンパイル（一回のみ）
		static MyRegex reg;
		if( !util::CompileRegex( reg, L"<a .*?href *?='([^']+)'[^>]*>(.*?)</a>" ) ) {
			MZ3LOGGER_FATAL( FAILED_TO_COMPILE_REGEX_MSG );
			return;
		}
		static MyRegex reg2;
		if( !util::CompileRegex( reg2, L"<a .*?href *?=\"([^\"]+)\"[^>]*>(.*?)</a>" ) ) {
			MZ3LOGGER_FATAL( FAILED_TO_COMPILE_REGEX_MSG );
			return;
		}
		static MyRegex reg3;
		if( !util::CompileRegex( reg3, L"<a .*?href *?=([^>]+)>(.*?)</a>" ) ) {
			MZ3LOGGER_FATAL( FAILED_TO_COMPILE_REGEX_MSG );
			return;
		}
		// 2ch形式リンク抽出用
		static MyRegex reg4;
		if( !util::CompileRegex( reg4, L"([^h]|^)(ttps?://[-_.!~*'()a-zA-Z0-9;/?:@&=+$,%#]+)" ) ) {
			MZ3LOGGER_FATAL( FAILED_TO_COMPILE_REGEX_MSG );
			return;
		}
		// YouTube動画リンク抽出用
		static MyRegex reg5;
		if( !util::CompileRegex( reg5, L"youtube_write.*src=\"(.*?)\".*?;" ) ) {
			MZ3LOGGER_FATAL( FAILED_TO_COMPILE_REGEX_MSG );
			return;
		}
		// GoogleMapリンク抽出用
		static MyRegex reg6;
		if( !util::CompileRegex( reg6, L".*url *: \\{ embed : \"([^\"]+)\", *link *: \"([^\"]+)\" *\\}" ) ) {
			MZ3LOGGER_FATAL( FAILED_TO_COMPILE_REGEX_MSG );
			return;
		}
		static MyRegex reg7;
		if( !util::CompileRegex( reg7, L"map\\.push\\(\\{([^\\)]+)\\}\\);" ) ) {
			MZ3LOGGER_FATAL( FAILED_TO_COMPILE_REGEX_MSG );
			return;
		}
		static MyRegex reg8;
		if( !util::CompileRegex( reg8, L"Event\\.observe\\(.*\\)" ) ) {
			MZ3LOGGER_FATAL( FAILED_TO_COMPILE_REGEX_MSG );
			return;
		}
		// ニコニコ動画リンク抽出用
		static MyRegex reg9;
		if( !util::CompileRegex( reg9, L"<script *type=\"text/javascript\" *src=\"http://ext\\.nicovideo\\.jp/thumb_watch/([a-z0-9]*)\\?.*?\".*?>(</script>)?" ) ) {
			MZ3LOGGER_FATAL( FAILED_TO_COMPILE_REGEX_MSG );
			return;
		}

		// <img>タグ抽出用
		static MyRegex reg10;
		if( !util::CompileRegex( reg10, L"<img[^>]*src=\"([^\"]+)\"[^>]*>" ) ) {
			return;
		}

		// <span>タグ抽出用
		static MyRegex regs;
		if( !util::CompileRegex( regs, L"<span[^>]*>" ) ) {
			MZ3LOGGER_FATAL( FAILED_TO_COMPILE_REGEX_MSG );
			return;
		}

		CString target = str;
		str = L"";
		for( int i=0; i<MZ3_INFINITE_LOOP_MAX_COUNT; i++ ) {	// MZ3_INFINITE_LOOP_MAX_COUNT は無限ループ防止
			int offset = -1;
			u_int offsetend = 0;
			std::wstring url = L"";
			std::wstring text = L"";
			std::wstring url2 = L"";
			std::wstring text2 = L"";
			bool bCrLf = false;
			bool bEventObserve = false;

			CString regtarget = target;
			// <a href=xx>
			if( regtarget.Find( L"href" ) != -1 ) {
				if( reg.exec(regtarget) && reg.results.size() == 3 ) {
					offset = reg.results[0].start ;
					offsetend = reg.results[0].end;
					url = reg.results[1].str;
					text = reg.results[2].str;
					regtarget = regtarget.Left(offset);				// 次のサーチ範囲を限定する
				}
				if( reg2.exec(regtarget) && reg2.results.size() == 3 ) {
					//if( offset < 0 || ( reg2.results[0].start < offset ) ){
						offset = reg2.results[0].start ;
						offsetend = reg2.results[0].end;
						url = reg2.results[1].str;
						text = reg2.results[2].str;
						regtarget = regtarget.Left(offset);			// 次のサーチ範囲を限定する
					//}
				}
				if( reg3.exec(regtarget) && reg3.results.size() == 3 ) {
					//if( offset < 0 || ( reg3.results[0].start < offset ) ){
						offset = reg3.results[0].start ;
						offsetend = reg3.results[0].end;
						url = reg3.results[1].str;
						text = reg3.results[2].str;
						regtarget = regtarget.Left(offset);			// 次のサーチ範囲を限定する
					//}
				}
			}
			// 2ch URL
			if( regtarget.Find( L"ttp://" ) != -1 ) {
				if( reg4.exec(regtarget) && reg4.results.size() == 3 ) {
					// 2ch URL
					//if( offset < 0 || ( reg4.results[2].start < offset ) ){
						offset = reg4.results[2].start;
						offsetend = reg4.results[0].end;
						// 2ch URL を正規化
						url = L"h";
						url += reg4.results[2].str;
						text = reg4.results[2].str;
						regtarget = regtarget.Left(offset);			// 次のサーチ範囲を限定する
					//}
				}
			}
			// YouTube動画リンク
			if( LINE_HAS_YOUTUBE_LINK(regtarget) ) {
				if( reg5.exec(regtarget) && reg5.results.size() == 2 ) {
					//if( offset < 0 || ( reg5.results[0].start < offset ) ){
						// YouTube動画リンク
						offset = reg5.results[0].start ;
						offsetend = reg5.results[0].end;
						url = reg5.results[1].str;
						text = L"<<Youtube動画>>";
						bCrLf = true;					// YouTubeリンクの場合は改行する
						regtarget = regtarget.Left(offset);			// 次のサーチ範囲を限定する
					//}
				}
			}
			// GoogleMapリンク
			if( LINE_HAS_GOOGLEMAP_LINK(regtarget) ) {
				if( reg7.exec(regtarget) && reg7.results.size() == 2 ) {
					// 発見。
					//if( offset < 0 || ( reg7.results[0].start < offset ) ){
						// マッチ文字列の中でGoogleMapリンクをさがす
						if( reg6.exec( reg7.results[1].str.c_str() ) && reg6.results.size() == 3 ) {
							// 発見
							// GoogleMapリンク
							offset = reg7.results[0].start ;
							offsetend = reg7.results[0].end ;
#ifndef WINCE
							// Win32はlink:からURLを取得
							url = reg6.results[2].str;
#else
							// WMはembed:からURLを取得
							url = reg6.results[1].str;
#endif
							text = L"<<Googleマップ>>";
							bCrLf = true;					// GoogleMapリンクの場合は改行する
							bEventObserve = true;			// Event.Observeがあれば消す指示
							regtarget = regtarget.Left(offset);			// 次のサーチ範囲を限定する
						}
					//}
				}
			}
			// ニコニコ動画リンク
			if( LINE_HAS_NICOVIDEO_LINK(regtarget) ) {
				if( reg9.exec(regtarget) && reg9.results.size() >= 2 ) {
					//if( offset < 0 || ( reg9.results[0].start < offset ) ){
						// ニコニコ動画リンク
						offset = reg9.results[0].start ;
						offsetend = reg9.results[0].end;
						url = L"http://www.nicovideo.jp/watch/" + reg9.results[1].str;
						text = L"<<ニコニコ動画>>";
						bCrLf = true;					// ニコニコ動画リンクの場合は改行する
						regtarget = regtarget.Left(offset);			// 次のサーチ範囲を限定する
					//}
				}
			}
			// <img>画像リンク
			if( regtarget.Find( L"<img" ) != -1 ) {
				if( reg10.exec(regtarget) && reg10.results.size() == 2 ) {
					//if( offset < 0 || ( reg10.results[0].start < offset ) ){
						// class="emoji" が含まれていれば、絵文字と判断し、無視する。
						if( util::LineHasStringsNoCase( reg10.results[0].str.c_str(), L"class=\"emoji\"" ) ) {
						} else {
							// <img>画像リンク
							offset = reg10.results[0].start ;
							offsetend = reg10.results[0].end;
							url = reg10.results[1].str;
							CString csWork;
							csWork.Format( L"<<link%02d(画像)>>" , list_.size() + 1 );
							text = csWork;
							bCrLf = true;					// <img>画像リンクの場合は改行する
							regtarget = regtarget.Left(offset);			// 次のサーチ範囲を限定する
						}
					//}
				}
			}
			// リンクテキストの中に<img>があれば抽出する
			{
				regtarget = text.c_str();
				if( regtarget.Find( L"<img" ) != -1 ) {
					if( reg10.exec(regtarget) && reg10.results.size() == 2 ) {
						// class="emoji" が含まれていれば、絵文字と判断し、無視する。
						if( util::LineHasStringsNoCase( reg10.results[0].str.c_str(), L"class=\"emoji\"" ) ) {
						} else {
							// <img>画像リンク
							CString csWork;
							csWork.Format( L"<<link%02d>>" , list_.size() + 1 );
							text = csWork;
							csWork.Format( L"<<link%02d(link%02dの画像)>>" , list_.size() + 2 , list_.size() + 1 );
							url2 = reg10.results[1].str;
							text2 = csWork;
							bCrLf = true;					// <img>画像リンクの場合は改行する
						}
					}
				}
			}
			if( offset < 0 ) {
				// 未発見。
				// 残りの文字列を代入して終了。
				str += target;
				break;
			}
			// 発見。
			// マッチ文字列全体の左側を出力
			str += target.Left( offset );

			// URL
			TRACE( L"regex-match-URL  : %s\n", url.c_str() );

			// 文字列
			TRACE( L"regex-match-TEXT : %s\n", text.c_str() );
			str += L"<_a>";
			str += text.c_str();
			str += L"</_a>";

			// データに追加
			// span,strong,br タグの除去
			CString csText = text.c_str();
			if( csText.Find( L"<span" ) != -1 ) 
				regs.replaceAll( csText, L"" );
			while( csText.Replace(_T("</span>"), _T("")) );
			while( csText.Replace(_T("<strong>"), _T("")) );
			while( csText.Replace(_T("</strong>"), _T("")) );
			while( csText.Replace(_T("<br>"), _T("")) );
			while( csText.Replace(_T("<br />"), _T("")) );

			list_.push_back( CMixiData::Link(url.c_str(),csText) );

			// リンク内に画像リンクがあればデータに追加
			if( url2 != L"" ){
				// 文字列
				str += L":<_a>";
				str += text2.c_str();
				str += L"</_a>";

				// データに追加
				list_.push_back( CMixiData::Link(url2.c_str(),text2.c_str() ) );
			}

			// 改行
			if( bCrLf ){
				str += L"<br>";
			}

			// ターゲットを更新。
			target = target.Mid( offsetend );

			if( bEventObserve ) {
				// Event.observe(云々) があれば消す
				if( reg8.exec(target) && reg8.results.size() != 0 ) {
					// 発見。
					// ターゲットを更新。
					target.Delete( reg8.results[0].start , reg8.results[0].end );
				}
			}
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

};

}