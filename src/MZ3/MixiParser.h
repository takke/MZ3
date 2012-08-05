/*
 * Copyright (c) MZ3 Project Team All rights reserved.
 * Code licensed under the BSD License:
 * http://www.mz3.jp/license.txt
 */
#pragma once

#include "MZ3Parser.h"
#include "xml2stl.h"

#ifdef BT_MZ3

/// mixi用パーサ
namespace mixi {

/// 各種mixiパーサの基本クラス
class MixiParserBase : public parser::MZ3ParserBase
{
public:
	/**
	 * ログアウトしたかをチェックする
	 */
	static bool IsLogout( LPCTSTR szHtmlFilename );
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
class MixiContentParser : public MixiParserBase, public parser::MZ3ContentParser
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
};


//■■■日記■■■
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
		 * - <dt>2007年10月02日&nbsp;22:22</dt>
		 *   のような日付見つかればそこから項目開始とみなす
		 *   <dd><a href="view_diary.pl?が見つかるまで行を飛ばし(通常日付の次の行)たら本文開始。
		 *   まずURLとIDを抜き出し、
		 * - 終了タグ</dd>が見つかるまで、行を結合して、最後に本文を抜き取る。
		 * - 以降に、"/listCommentArea"があれば、処理を終了する
		 *   
		 */
		int iLine = 180;		// とりあえず読み飛ばす
		for( ; iLine<count; iLine++ ) {
			const CString& str = html_.GetAt(iLine);
			//最初の日時を発見したら行検索ループ開始
			if(util::LineHasStringsNoCase( str, L"<dt>", L"</dt>" ) )  {
				// mixi データの作成
				{
					CMixiData data;
					data.SetAccessType(ACCESS_DIARY);

					//日時の抽出
					//<dt>2007年10月02日&nbsp;22:22</dt>
					ParserUtil::ParseDate(str, data);
					
					// 見出し
					//<dd><a href="view_diary.pl?
					{
						//<dd><a href="view_diary.pl?行が見つかるまで
						for( iLine; iLine<count; iLine++ ) {
							bool findFlag = false;
							const CString& line = html_.GetAt(iLine);
							//コメント開始行を発見したら
							if( util::LineHasStringsNoCase( line, L"<dd>", L"view_diary") )
							{
								// ＵＲＩ
								CString url;
								util::GetBetweenSubString( line, L"href=\"", L"\">", url );
								data.SetURL( url );
								
								// ＩＤを設定
								data.SetID( MixiUrlParser::GetID(url) );

								// 本文プレビュー
								CString target = L"";
								for( iLine; iLine<count; iLine++ ) {
									const CString& line2 = html_.GetAt(iLine);
									//</dd>が見つかったら次のコメントへ
									if( util::LineHasStringsNoCase( line2, L"</dd>") )
									{
										target += line2;
										//本文を抽出
										if( util::LineHasStringsNoCase( target, L"\">", L"</a>") )
										{
											CString title;
											util::GetBetweenSubString( target, L"\">", L"</a>", title );
											//改行を削除
											title.Replace(_T("\n"), _T(""));
											mixi::ParserUtil::ReplaceEntityReferenceToCharacter( title );
											//タイトルをセット
											data.SetTitle(title);
											//ループを抜けるフラグをセット
											findFlag = true;
										}
										//名前を抽出
										CString author;
										util::GetBetweenSubString( target, L"&nbsp;(", L")", author );
										mixi::ParserUtil::ReplaceEntityReferenceToCharacter( author );
										data.SetName( author );
										data.SetAuthor( author );
										break;
									} else {
										//</dd>が現れるまで結合
										target += line2;
									}
								}
							}
							//フラグがあったらループを抜ける
							if(findFlag == true) 
							{
								break;
							}
						}
					}
					out_.push_back(data);
				}
			}

			if( str.Find(_T("/listCommentArea")) != -1 ) {
				// 終了タグ発見
				break;
			}
		}

		MZ3LOGGER_DEBUG( L"ListCommentParser.parse() finished." );
		return true;
	}
};


//■■■コミュニティ■■■


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
				// 「次を表示」、「前を表示」のリンクを抽出する
				if( parseNextBackLink( nextLink, backLink, line ) ) {
					// 抽出できたら終了せず。
				}

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

			// "/messageArea"が見つかれば終了
			if( util::LineHasStringsNoCase( line, L"/messageArea" ) ) {
				return false;
			}

			// view_community.pl で始まるなら、抽出する
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

				// Image 抽出
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
				// コミュ名と人数に分解
				CString userCount;
				int idxStart = name.ReverseFind( '(' );
				int idxEnd   = idxStart>0 ? (name.Find(')', idxStart+1)) : -1;
				if (idxStart > 0 && idxEnd > 0 && idxEnd-idxStart-1>0) {
					userCount = name.Mid(idxStart+1, idxEnd-idxStart-1) + L"人";
					name = name.Left(idxStart);
				}
				mixi::ParserUtil::ReplaceEntityReferenceToCharacter( name );
				mixi.SetName( name );
				mixi.SetDate( userCount );	// 仮に日付として登録する
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
		if( !util::CompileRegex( reg, L"<a href=[\"]?list_community.pl([?]page=[^\"^>]+)[\"]?>([^<]+)</a>" ) ) {
			MZ3LOGGER_FATAL( FAILED_TO_COMPILE_REGEX_MSG );
			return false;
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
		 *   "<ul><li>～件を表示"が現れたら（次へ、前へを含む）ナビゲーション行。
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
			if( util::LineHasStringsNoCase( line, L"<span", L"titleSpan" ) ) {
				// 解析
				CMixiData mixi;

				// タイトル抽出
				CString title;
				util::GetBetweenSubString( line, L"class=\"title\">", L"</a>", title );
				mixi::ParserUtil::ReplaceEntityReferenceToCharacter( title );
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
						if( util::LineHasStringsNoCase( line, L"<span", L"date") ) {
							ParserUtil::ParseDate(line, mixi);
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
		if( !util::CompileRegex( reg, L"<a href=[\"]?list_bbs.pl([?]page=[^\"^>]+)[\"]?>([^<]+)</a>" ) ) {
			MZ3LOGGER_FATAL( FAILED_TO_COMPILE_REGEX_MSG );
			return false;
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
		mixi.ClearChildren();

		// 名前を初期化
		mixi.SetName(L"");

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
			if( util::LineHasStringsNoCase( line, L"<span", L"class", L"date" ) ) {
				ParserUtil::ParseDate(line, mixi);
			}

			// タイトルを取得する
			// <span class="titleSpan"><span class="title">xxxxx</span>...
			if( util::LineHasStringsNoCase( line, L"<span", L"class", L"titleSpan", L"title" ) ) {
				CString title;
				util::GetBetweenSubString( line, L"titleSpan\">", L"</span>", title );
				// タグの除去
				ParserUtil::StripAllTags( title );
				mixi::ParserUtil::ReplaceEntityReferenceToCharacter( title );
				mixi.SetTitle( title );
				continue;
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

						ParserUtil::GetAuthor(line, &mixi);

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

					// 動画用scriptタグが見つかったらscriptタグ終了まで解析。
					if(! ParserUtil::ExtractVideoLinkFromScriptTag( mixi, i, html_ ) ) {
						// scriptタグ未発見なので、解析＆投入

						// 非表示のタグを削除
						if (isBbsHiddenLine(line)) {
							// この行を強制的にスキップ
							continue;
						}
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

		// コミュニティ名抽出
		parseCommunityName( mixi, html_ );

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

	static bool isBbsHiddenLine(const CString& line)
	{
		// <p class="reportLink01" style="display: none;"><!-- ... --></p>
		if (util::LineHasStringsNoCase(line, L"<p class=\"reportLink01\"", L"display: none;", L"</p")) {
			return true;
		}
		return false;
	}

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
				util::GetBetweenSubString( number, L">", L"</label", number );
				// &nbsp; を消す
				while(number.Replace(L"&nbsp;",L"")) {}
				cmtData.SetCommentIndex(_wtoi(number));
			}

			// 日付
			// <span class="date">2007年07月28日 21:09</span></dt>
			if (util::LineHasStringsNoCase( line, L"<span", L"class", L"date" ) ) {
				ParserUtil::ParseDate(line, cmtData);
			}

			// <div class="pageNavigation01"> を発見したら、コメント終了なので抜ける
			if (util::LineHasStringsNoCase( line, L"<div", L"class", L"pageNavigation01" )) {
				return -1;
			}

			// bbs_comment_formを発見したら、コメント終了なので抜ける 
			if (util::LineHasStringsNoCase( line, L"<form", L"bbs_comment_form", L"action" )) { 
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

				ParserUtil::GetAuthor( line, &cmtData );
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

			// 動画用scriptタグが見つかったらscriptタグ終了まで解析。
			if(! ParserUtil::ExtractVideoLinkFromScriptTag( cmtData, i, html_ ) ) {
				// scriptタグ未発見なので、解析＆投入

				// 非表示のタグを削除
				if (isBbsHiddenLine(line)) {
					// この行を強制的にスキップ
					continue;
				}

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

		// 名前を初期化
		data_.SetName(L"");

		INT_PTR lastLine = html_.GetCount();

		bool bInEnquete = false;
		int iLine=100;
		for( ; iLine<lastLine; iLine++ ) {
			const CString& line = html_.GetAt(iLine);

			// ●タイトル
			//<dt class="bbsTitle clearfix"><span class="titleSpan">XXXXXXXX</span>
			if( util::LineHasStringsNoCase( line, L"<dt", L"bbsTitle" ) )
			{
				CString title;
				util::GetBetweenSubString( line, L"titleSpan\">", L"</span>", title );
				// タグの除去
				ParserUtil::StripAllTags( title );
				mixi::ParserUtil::ReplaceEntityReferenceToCharacter( title );
				data_.SetTitle(title);
				continue;
			}
			
			// ●日時
			//<span class="date">2007年10月06日 17:20</span>	</dt>
			if( util::LineHasStringsNoCase( line, L"<span", L"date" ) )
			{
				ParserUtil::ParseDate(line, data_);
				continue;
			}

			// ●企画者解析
			if( util::LineHasStringsNoCase( line, L"<dt>", L"show_friend" ) )
			{
				ParserUtil::GetAuthor( line, &data_ );
				continue;
			}

			// ●質問内容解析
			if( util::LineHasStringsNoCase( line, L"<dd>" ) )	// (C4819回避のダミーコメントです)
			{
				//内容解析関数へ、<dd class="enqueteBlock">が見つかったら戻る
				if( !parseBody( data_, html_, iLine ) )
					return false;
				bInEnquete = true;
				continue;
			}

			// ●集計結果解析。
			// <td BGCOLOR=#FFD8B0 ALIGN=center><font COLOR=#996600>集計結果</font></td>
			if( util::LineHasStringsNoCase( line, L"<h3>", L"集計結果" ) )
			{
				if( !parseEnqueteResult( data_, html_, iLine ) )
					return false;
				continue;
			}

			// ●アンケート内容終了
			if( bInEnquete && 
				(util::LineHasStringsNoCase( line, L"<!-- COMMENT: start -->" ) ||
				 util::LineHasStringsNoCase( line, L"<div", L"id", L"enqueteComment") ||
				 // 送信ボタン
				 //<li><input type="submit" value="送信する" class="formBt01" /></li>
				 util::LineHasStringsNoCase( line, L"<input", L"type=", L"submit", L"送信する" )
				 ) )
			{
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

		// コミュニティ名抽出
		parseCommunityName( data_, html_ );

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
	 * (1) "<dd class=\"enqueteBlock\">" が現れるまで無視。その直後から、設問内容本文。解析して、AddBody する。
	 * (2) "</dd>" が現れたら終了。
	 */
	static bool parseBody( CMixiData& mixi, const CHtmlArray& html, int& iLine )
	{
		++iLine;

		mixi.AddBody(_T("\r\n"));
		const int lastLine = html.GetCount();

		for( ; iLine<lastLine; iLine++ ) {
			const CString& line = html.GetAt(iLine);

			// <dd class=\"enqueteBlock\"> があれば、その後ろを本文とする。
			CString target;
			if( util::GetAfterSubString( line, L"<dd class=\"enqueteBlock\">", target ) >= 0 ) {
				// <dd class=\"enqueteBlock\">発見。
			}else{
				// <dd class=\"enqueteBlock\">未発見。
				target = line;
			}

			// </dd>があれば、その前を追加し、終了。
			// なければ、その行を追加し、次の行へ。
			if( util::GetBeforeSubString( target, L"</dd>", target ) < 0 ) {
				// </dd> が見つからなかった。
				ParserUtil::AddBodyWithExtract( mixi, target );
			}else{
				// </dd>が見つかったので終了。
				ParserUtil::AddBodyWithExtract( mixi, target );
				break;
			}

			// reply_enquete.pl 用：</ul> があれば終了。
			if( target.Find(L"</ul>") != -1 ) {
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
	 * <!-- oneMeter -->
	 * <dl class="enqueteList">
	 * <dt>回答選択肢<br />
	 * <img src="http://img.mixi.jp/img/bar.gif" width="28" height="16" alt="" />
	 * </dt>
	 * <dd><span>9</span>(3%)</dd>
	 * </dl>
	 * <!-- oneMeter -->
	 * 解析、整形して、AddBody する。
	 *
	 * ● 下記の形式で、合計があるので、解析、整形して、AddBodyする。
	 * <dl class="enqueteTotal"><dt class="enqueteTotalNumber">合計<span>244</span></dt></dl>
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

			// <dd class="formButtons01">があれば、解析終了。
			if( util::LineHasStringsNoCase( line, L"<dd class=\"formButtons" ) ) {
				break;
			}

			// 選択肢、投票数、百分率を解析して、AddBody する。
			if( util::LineHasStringsNoCase( line, L"<dl class=\"enqueteList\">" ) ) {
				// 境界値チェック
				if( iLine+5 >= lastLine ) {
					break;
				}
				// 次の行に本文がある。
				const CString& line1 = html.GetAt(++iLine);
				CString item;
				util::GetBetweenSubString( line1, L"<dt>", L"<br />", item );
				
				// +3 行目に下記の形式で、投票数、百分率がある。
				// <dd><span>125</span>(51%)</dd>
				iLine += 3;
				const CString& line2 = html.GetAt( iLine );
				CString target;
				util::GetBetweenSubString( line2, L"<span>", L"</span>", target );

				CString target2;
				util::GetBetweenSubString( line2, L"</span>", L"</dd>", target2 );
				
				// xx (yy%)
				CString num_rate;
				num_rate = target + target2;

				CString str;
				str.Format( L"  ●%s\r\n", item );
				mixi.AddBody( str );
				str.Format( L"      %s\r\n", num_rate );
				mixi.AddBody( str );
			}

			// 合計を解析する。
			if( util::LineHasStringsNoCase( line, L"enqueteTotal" ) )
			{
				// 次の行に合計がある。
				const CString& line1 = html.GetAt( iLine );

				CString total;
				util::GetBetweenSubString( line1, L"<span>", L"</span>", total );

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
		int count = html_.GetCount();
		int retIndex = count;

		CMixiData cmtData;
		BOOL findFlag = FALSE;

		for (int i=sIndex; i<count; i++) {
			const CString& str = html_.GetAt(i);

			if (findFlag == FALSE) {

				if (str.Find(_T("<!-- ADD_COMMENT: start -->")) != -1 ||
				    str.Find(_T("<!-- COMMENT: end -->")) != -1 ||
				    str.Find(_T("add_enquete_comment.pl")) != -1) 
				{
					// コメント全体の終了タグ発見
					parsePostURL( i, *data, html_ );
					retIndex = -1;
					break;

				} else {

					const CString& line = html_.GetAt(i);
					// コメント番号を取得
					//<label for="commentCheck01">37</label>
					if( util::LineHasStringsNoCase( line, L"\">", L"</label>")) {
						CString number;
						util::GetAfterSubString( line, L"<label", number );
						util::GetBetweenSubString( number, L">", L"</label", number );
						// &nbsp; を消す
						while(number.Replace(L"&nbsp;",L"")) {}
						cmtData.SetCommentIndex(_wtoi(number));
					}
					
					// 日付を取得
					//<span class="date">2007年10月07日 11:25</span></dt>
					if( util::LineHasStringsNoCase( line, L"date\">", L"</span>")) {
						ParserUtil::ParseDate(line, cmtData);
					}
					
					// 名前を取得
					//<dt><a href="show_friend.pl?id=xxxxxx">なまえ</a></dt>
					if( util::LineHasStringsNoCase( line, L"<dt>", L"</dt>")) {
						CString Author;
						util::GetBetweenSubString( line, L"<dt>", L"</dt>", Author );
						ParserUtil::GetAuthor( Author, &cmtData );
						//コメント開始フラグをON
						findFlag = TRUE;
					}

				}
			}
			else {

				if( util::LineHasStringsNoCase( str, L"<dd>") ) {
					// コメントコメント本文取得
					const CString& str = html_.GetAt(i);

					// ----------------------------------------
					// アンケートのパターン
					// ----------------------------------------
					cmtData.AddBody(_T("\r\n"));

					CString buf;
					util::GetAfterSubString( str, L">", buf );

					if( util::GetBeforeSubString( buf, L"</dd>", buf ) > 0 ) {

						// 終了タグがあった場合
						ParserUtil::AddBodyWithExtract( cmtData, buf );
						retIndex = i + 5;
						break;
					}

					// それ以外の場合
					ParserUtil::AddBodyWithExtract( cmtData, buf );

					while( i<count ) {
						i++;
						const CString& line  = html_.GetAt(i);
						int index = line.Find(_T("</dd>"));
						if (index != -1) {
							// 終了タグ発見
							buf = line.Left(index);
							ParserUtil::AddBodyWithExtract( cmtData, buf );
							retIndex = i + 1;
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
		data_.ClearChildren();
		
		// 名前を初期化
		data_.SetName(L"");

		INT_PTR lastLine = html_.GetCount();

		int iLine=100;
		for( ; iLine<lastLine; iLine++ ) {
			const CString& line = html_.GetAt(iLine);

			// ●タイトル(pattern1)
			//<dt class="bbsTitle clearfix"><span class="titleSpan"><span class="title">xxxxx</span></span>
			if( util::LineHasStringsNoCase( line, L"<span", L"titleSpan" ) )
			{
				CString title;
				util::GetBetweenSubString( line, L"class=\"title\">", L"</span>", title );
				mixi::ParserUtil::ReplaceEntityReferenceToCharacter( title );
				data_.SetTitle(title);
				continue;
			}

			// ●日時
			//<dd>2007年09月27日(地域によっては遅れる場合あり)</dd>
			if( util::LineHasStringsNoCase( line, L"<dt>開催日時</dt>" ) )
			{
				const CString& line2 = html_.GetAt(iLine+1);
				CString date;
				util::GetBetweenSubString( line2, L"<dd>", L"</dd>", date );
				//ParserUtil::ParseDate(date, cmtData);
				//data_.SetDate(date);
				CString buf = _T("開催日時 ") + date;
				data_.AddBody(_T("\r\n"));
				data_.AddBody(buf);
				data_.AddBody(_T("\r\n"));
				
				continue;
			}

			// ●場所
			//<dd>都道府県未定(全国の書店など)</dd>
			if( util::LineHasStringsNoCase( line, L"<dt>開催場所</dt>" ) )
			{
				const CString& line2 = html_.GetAt(iLine+1);
				CString area;
				util::GetBetweenSubString( line2, L"<dd>", L"</dd>", area );
				CString buf = _T("開催場所 ") + area;
				data_.AddBody(_T("\r\n"));
				data_.AddBody(buf);
				data_.AddBody(_T("\r\n"));
				
				continue;
			}

			// ●企画者解析
			if( util::LineHasStringsNoCase( line, L"<dt>", L"show_friend" ) )
			{
				const CString& line2 = html_.GetAt(iLine );
				CString Author;
				util::GetBetweenSubString( line2, L"<dt>", L"</dt>", Author );
				ParserUtil::GetAuthor( Author, &data_ );

				// ●内容解析
				//内容解析関数へ、</dd>が見つかったら戻る
				if( !parseBody( data_, html_, iLine ) )
					return false;
				continue;
			}

			// ●募集期限
			//<dd>指定なし</dd>
			if( util::LineHasStringsNoCase( line, L"<dt>募集期限</dt>" ) )
			{
				const CString& line2 = html_.GetAt(iLine+1);
				CString limit;
				util::GetBetweenSubString( line2, L"<dd>", L"</dd>", limit );
				CString buf = _T("募集期限 ") + limit;
				data_.AddBody(_T("\r\n"));
				data_.AddBody(buf);
				data_.AddBody(_T("\r\n"));
				
				continue;
			}

			// ●参加者
			//<dd>NNN名</dd>
			if( util::LineHasStringsNoCase( line, L"<dt>参加者</dt>" ) )
			{
				const CString& line2 = html_.GetAt(iLine+1);
				CString people;
				util::GetBetweenSubString( line2, L"<dd>", L"</dd>", people );
				CString buf = _T("参加者 ") + people;
				data_.AddBody(_T("\r\n"));
				data_.AddBody(buf);
				data_.AddBody(_T("\r\n"));
				
				continue;
			}

			// ●イベント内容終了
			if( util::LineHasStringsNoCase( line, L"<!-- COMMENT: start -->" ) ||
				util::LineHasStringsNoCase( line, L"<div", L"id", L"eventComment") ) {
				break;
			}

		}

		// コメント解析
		while( iLine<lastLine ) {
			iLine = parseEventComment( iLine, &data_, html_ );
			if( iLine == -1 ) {
				break;
			}
		}

		// イベント参加メンバーへのリンク
		{
			CString url = util::FormatString(
							L"http://mixi.jp/list_event_member.pl?id=%s&comm_id=%s",
							(LPCTSTR)util::GetParamFromURL(data_.GetURL(), L"id"),
							(LPCTSTR)util::GetParamFromURL(data_.GetURL(), L"comm_id"));
			data_.m_linkPage.push_back( CMixiData::Link(url, L"★参加者一覧") );
		}

		// コミュニティ名抽出
		parseCommunityName( data_, html_ );

		// ページ移動リンクの抽出
		parsePageLink( data_, html_ );

		// 「最新のトピック」の抽出
		parseRecentTopics( data_, html_ );

		MZ3LOGGER_DEBUG( L"ViewEventParser.parse() finished." );
		return true;
	}

private:

	/**
	 * 設問内容を取得する。
	 *
	 * iLine は "設問内容" が存在する行。
	 *
	 * (1) <dd>が現れるまで無視。その直後から、設問内容本文。解析して、AddBody する。
	 * (2) "</dd>" が現れたら終了。
	 */
	static bool parseBody( CMixiData& mixi, const CHtmlArray& html, int& iLine )
	{
		++iLine;

		mixi.AddBody(_T("\r\n"));
		const int lastLine = html.GetCount();

		for( ; iLine<lastLine; iLine++ ) {
			const CString& line = html.GetAt(iLine);

			// <dd>があれば、その後ろを本文とする。
			CString target;
			if( util::GetAfterSubString( line, L"<dd>", target ) >= 0 ) {
				// <dd>発見。
			}else{
				// <dd>未発見。
				target = line;
			}

			// </dd>があれば、その前を追加し、終了。
			// なければ、その行を追加し、次の行へ。
			if( util::GetBeforeSubString( target, L"</dd>", target ) < 0 ) {
				// </dd> が見つからなかった。
				ParserUtil::AddBodyWithExtract( mixi, target );
			}else{
				// </dd>が見つかったので終了。
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
	 * イベントコメント取得
	 */
	static int parseEventComment(int sIndex, CMixiData* data, const CHtmlArray& html_ )
	{
		int count = html_.GetCount();
		int retIndex = count;

		CMixiData cmtData;
		BOOL findFlag = FALSE;

		for (int i=sIndex; i<count; i++) {
			const CString& str = html_.GetAt(i);

			if (findFlag == FALSE) {

				if (str.Find(_T("<!-- ADD_COMMENT: start -->")) != -1 ||
				    str.Find(_T("<!-- COMMENT: end -->")) != -1 ||
				    str.Find(_T("add_event_comment.pl")) != -1)
				{
					// コメント全体の終了タグ発見
					parsePostURL( i, *data, html_ );
					retIndex = -1;
					break;
				
				} else {

					const CString& line = html_.GetAt(i);
					//コメント番号を取得
					// <dt class="commentDate clearfix"><span class="senderId">183
					// or
					// <dt class="commentDate clearfix"><span class="senderId"><input type="checkbox" name="comment_id" id="commentCheck290675880" value="290675880" /><label for="commentCheck290675880">44</label></span>
					if( util::LineHasStringsNoCase( line, L"commentDate", L"senderId")) {
						CString number;
						if (util::GetAfterSubString( line, L"<label", number ) >= 0) {
							// 管理コミュ版
							util::GetBetweenSubString( number, L">", L"</label", number );
						} else {
							util::GetAfterSubString( line, L"<span", number );
							util::GetAfterSubString( number, L">", number );
						}
						// &nbsp; を消す
						while(number.Replace(L"&nbsp;",L"")) {}
						cmtData.SetCommentIndex( _wtoi(number) );
					}
					
					//日付を取得
					//<span class="date">2007年10月07日 11:25</span></dt>
					if( util::LineHasStringsNoCase( line, L"date\">", L"</span>")) {
						ParserUtil::ParseDate(line, cmtData);
					}
					
					//名前を取得
					//<dt><a href="show_friend.pl?id=xxxxxx">なまえ</a></dt>
					if( util::LineHasStringsNoCase( line, L"show_friend", L"</dt>")) {
						CString Author;
						util::GetBetweenSubString( line, L"<dt>", L"</dt>", Author );
						ParserUtil::GetAuthor( Author, &cmtData );
						//コメント開始フラグをON
						findFlag = TRUE;
					}

				}
			}
			else {

				if( util::LineHasStringsNoCase( str, L"<dd>") ) {
					// コメントコメント本文取得
					const CString& str = html_.GetAt(i);

					// ----------------------------------------
					// イベントのパターン
					// ----------------------------------------
					cmtData.AddBody(_T("\r\n"));

					CString buf;
					util::GetAfterSubString( str, L">", buf );

					if( util::GetBeforeSubString( buf, L"</dd>", buf ) > 0 ) {

						// 終了タグがあった場合
						ParserUtil::AddBodyWithExtract( cmtData, buf );
						retIndex = i + 1;
						break;
					}

					// それ以外の場合
					ParserUtil::AddBodyWithExtract( cmtData, buf );

					while( i<count ) {
						i++;
						const CString& line  = html_.GetAt(i);
						int index = line.Find(_T("</dd>"));
						if (index != -1) {
							// 終了タグ発見
							buf = line.Left(index);
							ParserUtil::AddBodyWithExtract( cmtData, buf );
							retIndex = i + 1;
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
		 * - <table class=..
		 *   が見つかればそこから項目開始とみなす
		 * - そこから+18行目以降に
		 *   <td ...><A HREF="view_news.pl?id=XXXXX&media_id=X"class="new_link">title</A>
		 *   という形式で「URL」と「タイトル」が存在する。
		 * - 次の1行後に
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
				if( str.Find( L"<table class=" ) == -1 ) {
					// 開始フラグ未発見
					continue;
				}
				bInItems = true;
			}
			if( str.Find( L"<a" ) == -1 || str.Find( L"view_news.pl" ) == -1 ) {
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
				if( !util::CompileRegex( reg, L"\"(view_news\\.pl\\?id=[0-9]+\\&media_id=[0-9]+).+>(.+)</a" ) ) {
					MZ3LOGGER_FATAL( FAILED_TO_COMPILE_REGEX_MSG );
					return false;
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
				if( !util::CompileRegex( reg, L"list_news_media\\.pl.+>([^<]+)</a" ) ) {
					MZ3LOGGER_FATAL( FAILED_TO_COMPILE_REGEX_MSG );
					return false;
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
			// <td WIDTH="1%" nowrap CLASS="f08">10月14日 16:47</td></tr>
			const CString& date = html_.GetAt(iLine);

			// mixi データの作成
			{
				CMixiData data;
				data.SetAccessType( ACCESS_NEWS );

				// 日付
				ParserUtil::ParseDate( date, data );
				TRACE(_T("%s\n"), data.GetDate());

				// 見出し
				CString wktitle = title.c_str();
				mixi::ParserUtil::ReplaceEntityReferenceToCharacter( wktitle );
				data.SetTitle( wktitle );
				TRACE(_T("%s\n"), data.GetTitle());

				// URL 生成
				CString url2 = L"http://news.mixi.jp/";
				url2 += url.c_str();
				data.SetURL( url2 );
				TRACE(_T("%s\n"), data.GetURL());

				// 名前
				CString name = author.c_str();
				mixi::ParserUtil::ReplaceEntityReferenceToCharacter( name );
				data.SetName( name );
				data.SetAuthor( name );
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
		if( !util::CompileRegex( reg, L"<a href=[\"]?list_news_category.pl([?]page=[^\"^>]+)[\"]?>([^<]+)</a>" ) ) {
			MZ3LOGGER_FATAL( FAILED_TO_COMPILE_REGEX_MSG );
			return false;
		}

		return parseNextBackLinkBase( nextLink, backLink, str, reg, 
					L"http://news.mixi.jp/list_news_category.pl", ACCESS_LIST_NEWS );
	}

};


/**
 * [content] view_news.pl 用パーサ
 * 【ニュース記事】
 * http://mixi.jp/view_news.pl
 */
class ViewNewsParser : public MixiContentParser
{
public:
	static bool parse( CMixiData& data_, const CHtmlArray& html_ )
	{
		MZ3LOGGER_DEBUG( L"ViewNewsParser.parse() start." );

		data_.ClearAllList();
		data_.ClearChildren();
		data_.SetName(L"");

		INT_PTR count = html_.GetCount();

		// タイトルは title タグから抽出する
		for (int iLine=0; iLine<20 && iLine<count; iLine++) {
			const CString& line = html_.GetAt(iLine);
	
			//<title>[mixi] たいとる（配信元名称）</title>
			if (util::LineHasStringsNoCase( line, L"<title>[mixi]", L"</title>" ) ) {
				CString title;
				util::GetBetweenSubString( line, L"<title>[mixi]", L"</title>", title );
				title.Trim();
				mixi::ParserUtil::ReplaceEntityReferenceToCharacter( title );
				data_.SetTitle(title);
				break;
			}
		}

		/**
		 * 方針：
		 * - <div class=\"article\">
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
				if( !util::LineHasStringsNoCase( line, L"<div class=\"article\">" ) ) {
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
			// 行頭に <div class=\"bottomContents clearfix\">があれば終了。
			LPCTSTR endTag = _T("<div class=\"bottomContents clearfix\">");
			if( wcsncmp( line, endTag, wcslen(endTag) ) == 0 ) {
				// 終了タグ発見
				break;
			}

			// 下記があれば末尾とみなす
			/*
			<a name="post-check"></a>
<p class="checkButton">
<a check_key="53be64390c2bf14912cc8ded09c28d346a7a28b0" check_button="button-5.gif">チェック</a>
<script data-prefix-uri="http://mixi.jp/" 
        src="http://static.mixi.jp/js/share.js" 
        type="text/javascript"></script>
</p>
			*/
			endTag = _T("<a name=\"post-check\"></a>");
			if( wcsncmp( line, endTag, wcslen(endTag) ) == 0 ) {
				// 終了タグ発見
				break;
			}

			ParserUtil::AddBodyWithExtract( data_, line );
		}

		int id = _wtoi(util::GetParamFromURL(data_.GetURL(), L"id"));
		data_.m_linkPage.push_back(
			MZ3Data::Link(util::FormatString(L"http://news.mixi.jp/list_quote_diary.pl?id=%d", id), 
						  L"関連日記"));

		MZ3LOGGER_DEBUG( L"ViewNewsParser.parse() finished." );
		return true;
	}

};



//■■■その他■■■

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

		INT_PTR lastLine = html_.GetCount();

		// 「次」、「前」のリンク
		CMixiData backLink;
		CMixiData nextLink;

		// 項目開始を探す
		bool bInItems = false;	// 項目開始？
		int iLine = 100;		// とりあえず読み飛ばす
		for( ; iLine<lastLine; iLine++ ) {
			const CString& line = html_.GetAt(iLine);

			// 項目開始？
			if( !bInItems ) {
				//<ul class="introListContents">
				if( util::LineHasStringsNoCase( line, L"<ul", L"class=", L"introListContents" ) ) {
					bInItems = true;
				}
			} else {

				//<div class="pageNavigation01"> で終了
				if( util::LineHasStringsNoCase( line, L"<div", L"class=", L"pageNavigation01" ) ) {
					break;
				}

				// show_friend.pl があれば項目っぽい
				if( util::LineHasStringsNoCase( line, L"<a", L"href=", L"show_friend.pl" ) ) {
					CMixiData mixi;
					if( parseOneIntro( mixi, html_, iLine ) ) {
						out_.push_back( mixi );
					}
				}
			}
		}

		// 終了タグが見つかったので、その後の行から次、前のリンクを抽出
		for( ; iLine<lastLine; iLine++ ) {
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

		MZ3LOGGER_DEBUG( L"ShowIntroParser.parse() finished." );
		return true;
	}

private:
	/**
	 * １つの紹介文を抽出する。
	 * iLine は show_friend.pl が現れた行。
	 */
	static bool parseOneIntro( CMixiData& mixi, const CHtmlArray& html, int& iLine )
	{
		const int lastLine = html.GetCount();

		if (iLine+2>lastLine) {
			return false;
		}

		// 画像抽出
		const CString& line = html.GetAt( iLine );
		CString image_url;
		if (util::GetBetweenSubString( line, L"src=\"", L"\"", image_url ) >= 0 ) {
			mixi.AddImage( image_url );
		}

		const CString& line2 = html.GetAt( ++iLine );

		// URL 抽出
		CString target;
		if( util::GetAfterSubString( line2, L"<a ", target ) < 0 ) {
			return false;
		}
		// target: href="show_friend.pl?id=xxx">なまえ</a></dt>
		CString url;
		if( util::GetBetweenSubString( target, L"href=\"", L"\"", url ) < 0 ) {
			return false;
		}
		mixi.SetURL( url );

		// URL 構築＆設定
		url.Insert( 0, L"http://mixi.jp/" );
		mixi.SetBrowseUri( url );

		// 名前抽出
		CString name;
		if( util::GetBetweenSubString( target, L">", L"<", name ) < 0 ) {
			return false;
		}
		mixi::ParserUtil::ReplaceEntityReferenceToCharacter( name );
		mixi.SetName( name );

		mixi.SetAccessType( ACCESS_PROFILE );

		// </div> が見つかるまで、紹介文を探す
		for( ++iLine; iLine<lastLine; iLine++ ) {
			const CString& line = html.GetAt( iLine );

			// <ul> or </div> があれば終了。
			if( util::LineHasStringsNoCase( line, L"</div>" ) ||
				util::LineHasStringsNoCase( line, L"<ul>" ) ) {
				break;
			}else{
				CString tagStripedLine = line;
				ParserUtil::StripAllTags( tagStripedLine );
				if (!tagStripedLine.IsEmpty()) {
					ParserUtil::AddBodyWithExtract( mixi, tagStripedLine );
					mixi.AddBody( L"\r\n" );
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
		if( !util::CompileRegex( reg, L"<a href=[\"]?show_intro.pl([?]page=[^\"^>]+)[\"]?>([^<]+)</a>" ) ) {
			MZ3LOGGER_FATAL( FAILED_TO_COMPILE_REGEX_MSG );
			return false;
		}

		return parseNextBackLinkBase( nextLink, backLink, str, reg, L"show_intro.pl", ACCESS_LIST_INTRO );
	}
};


/**
 * [list] show_calendar.pl 用パーサ。
 * 【カレンダー】
 * http://mixi.jp/show_calendar.pl
 */
class ShowCalendarParser : public MixiListParser
{
public:
	static bool parse( CMixiDataList& out_, const CHtmlArray& html_ )
	{
		MZ3LOGGER_DEBUG( L"ShowCalendarParser.parse() start." );

		INT_PTR count = html_.GetCount();

		BOOL findFlag = FALSE;
		BOOL findFlag2 = FALSE;
		CString YearMonth;
		CString strDate;

		// 「次」、「前」のリンク
		CMixiData backLink;
		CMixiData nextLink;
			
		for (int i=0; i<count; i++) {

			const CString& str = html_.GetAt(i);

			//<div class="calendarNavi bottom">が現れたら解析終了
			if( util::LineHasStringsNoCase( str, L"<div class=\"calendarNavi bottom\">" ) ) {
				break;
			}
			
			//年月を抽出
			if( util::LineHasStringsNoCase( str, L"<title>[mixi]", L"のカレンダー</title>" ) ) {
				util::GetBetweenSubString( str, L"<title>[mixi]", L"のカレンダー</title>", YearMonth );
				findFlag = TRUE;
			}
			
			//開始フラグ
			if( util::LineHasStringsNoCase( str, L"<div class=\"calendarTable\">" ) ) {
				findFlag2 = TRUE;
			}

			// 次・前の月リンクの抽出
			if (findFlag && !findFlag2) {
				parseNextBackLink(nextLink, backLink, str);
			}

			if (findFlag != FALSE && findFlag2 != FALSE ) {

				CString target = str;
				CString title;
				CString url;
				BOOL findFlag3 = FALSE;
				CMixiData data;
					
				//<a href="view_event.pl?id=nnnnn&comm_id=xxxxx">イベントタイトル</a> イベント
				if( util::LineHasStringsNoCase( target, L"view_event.pl" )) 
				{
					CString subtarget;
					util::GetAfterSubString( target , L"<a href=\"" , subtarget );
					util::GetBetweenSubString( subtarget, L"\">", L"</a>", title );
					util::GetBetweenSubString( target, L"<a href=\"", L"\">", url );
					if( util::LineHasStringsNoCase( target, L"<li class=\"join\">" )) {
						data.SetAccessType( ACCESS_EVENT_JOIN );
					} else {
						data.SetAccessType( ACCESS_EVENT );
					}
					findFlag3 = TRUE;
				}
				//<a href="show_friend.pl?id=xxxx">XXさん</a>　誕生日
				if( util::LineHasStringsNoCase( target, L"show_friend.pl" ) ) {
				
					//show_friend.pl以降を取り出す
					CString after;
					util::GetAfterSubString( target, L"show_friend.pl", after );
					util::GetBetweenSubString( after, L">", L"</a>", title );
					
					title = L"【誕生日】" + title;
					//飛ばし先のプロフィールURLを抽出
					util::GetBetweenSubString( target, L"<a href=\"", L"\">", url );
					data.SetURL( url );
					data.SetAccessType( ACCESS_BIRTHDAY );
					findFlag3 = TRUE;
				}
				//<a href="javascript:void(0);" onClick="MM_openBrWindow('view_schedule.pl?id=nnnnn','','width=760,height=640,toolbar=no,scrollbars=yes,left=10,top=10')">スケジュール</a>　自分スケジュール
				if( util::LineHasStringsNoCase( target, L"view_schedule.pl" ) ) {
					util::GetBetweenSubString( target, L"')\">", L"</a>", title );

					title = L"【予定】" + title;
					//飛ばし先のスケジュール詳細がまだ未実装のため保留
					util::GetBetweenSubString( target, L"MM_openBrWindow('", L"'", url );

					data.SetAccessType( ACCESS_SCHEDULE );
					findFlag3 = TRUE;
				}

				mixi::ParserUtil::ReplaceEntityReferenceToCharacter( title );
				if (findFlag3 != FALSE) {
					// オブジェクト生成

					data.SetTitle( title );
					data.SetURL( url );
					strDate.Trim();
					data.SetDate( strDate );
					out_.push_back( data );
				}
				
				//<td xxx>タグに日付があるのでその行の最後の<td>タグから切り出す
				CString td = target;
				bool bFindTd = false ;
				while( util::GetAfterSubString( td, L"<td", td ) != -1 ) {
					bFindTd = true;
				}
				if( bFindTd ){
					CString date = L"";
					// ">"と"<"の間を日付として抽出する
					if( util::GetBetweenSubString( td, L">", L"<", date ) == -1 ){
						// "<"が見つからなければ行の最後までを抽出する
						util::GetBetweenSubString( td, L">", L"\n" ,date );
					}
					strDate = YearMonth + date + L"日";
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

		MZ3LOGGER_DEBUG( L"ShowCalendarParser.parse() finished." );
		return true;
	}

private:
	/// 「次を表示」、「前を表示」のリンクを抽出する
	static bool parseNextBackLink( CMixiData& nextLink, CMixiData& backLink, CString str )
	{
		// 正規表現のコンパイル（一回のみ）
/*
	<a href="show_calendar.pl?year=2007&month=9&pref_id=13">&lt;&lt;&nbsp;前の月</a>
	<a href="show_calendar.pl?year=2007&month=10&pref_id=13">当月</a>
	<a href="show_calendar.pl?year=2007&month=11&pref_id=13">次の月&nbsp;&gt;&gt;</a>&nbsp;
*/
		static MyRegex reg;
		if( !util::CompileRegex( reg, L"<a href=\"show_calendar.pl([?].+?)\">.*?(前の月|当月|次の月).*?</a>" ) ) {
			MZ3LOGGER_FATAL( FAILED_TO_COMPILE_REGEX_MSG );
			return false;
		}

		return parseNextBackLinkBase( nextLink, backLink, str,
			reg, L"show_calendar.pl", ACCESS_LIST_CALENDAR );
	}
};

}//namespace mixi

#endif	// BT_MZ3
