/*
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */
#pragma once

#include "MZ3Parser.h"
#include "xml2stl.h"

/// mixi 用HTMLパーサ
namespace mixi {

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
				ParserUtil::ParseDate(str, data);

				for (int j=i; j<count; j++) {
					const CString& str = html_.GetAt(j);

					LPCTSTR key = _T("<a href=\"view_diary.pl?id");
					if (str.Find(key) != -1) {
						CString uri;
						util::GetBetweenSubString( str, _T("<a href=\""), L"\">", uri );
						data.SetURL( uri );

						// ＩＤを設定
						data.SetID( MixiUrlParser::GetID(uri) );

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
		if( !util::CompileRegex( reg, L"<a href=[\"]?list_diary.pl([?]page=[^\"^>]+)[\"]?>([^<]+)</a>" ) ) {
			MZ3LOGGER_FATAL( FAILED_TO_COMPILE_REGEX_MSG );
			return false;
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
											//タイトルをセット
											data.SetTitle(title);
											//ループを抜けるフラグをセット
											findFlag = true;
										}
										//名前を抽出
										CString author;
										util::GetBetweenSubString( target, L"&nbsp;(", L")", author );
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

		// 日記開始フラグの探索
		int iLine = 100;
		for (; iLine<count; iLine++) {
			const CString& line = html_.GetAt(iLine);

			if (util::LineHasStringsNoCase( line, _T("newFriendDiaryArea"))) {
				// 日記開始
				break;
			}
		}

		// 各日記項目の取得
		// 日記が開始していなければ最終行なのでループしない。
		bool bDataFound   = false;
		for (; iLine<count; iLine++) {
			const CString& line = html_.GetAt(iLine);

			// 「次を表示」、「前を表示」のリンクを抽出する
			if( parseNextBackLink( nextLink, backLink, line ) ) {
				continue;
			}

			if (util::LineHasStringsNoCase( line, L"<dt>", L"</dt>" ) ) {

				bDataFound = true;

				CMixiData data;
				data.SetAccessType(ACCESS_DIARY);

				//--- 時刻の抽出
				//<dt>2007年10月02日&nbsp;22:22</dt>
				ParserUtil::ParseDate(line, data);

				//--- 見出しの抽出
				//<dd><a href="view_diary.pl?id=xxx&owner_id=xxx">タイトル</a> (なまえ)<div style="visibility: hidden;" class="diary_pop" id="xxx"></div>
				// or
				//<dd><a href="view_diary.pl?url=xxx&owner_id=xxx">タイトル</a> (なまえ)
				iLine++;
				const CString& line2 = html_.GetAt(iLine);

				CString after;
				util::GetAfterSubString( line2, L"<a", after );
				CString title;
				util::GetBetweenSubString( after, L">", L"<", title );
				data.SetTitle(title);

				// ＵＲＩ
				if (util::LineHasStringsNoCase( line2, L"list_diary.pl" ) ) {
					iLine += 5;
					continue;
				}
				CString url;
				util::GetBetweenSubString( after, L"\"", L"\"", url );
				data.SetURL(url);

				// ＩＤを設定
				data.SetID( MixiUrlParser::GetID(url) );

				// 名前
				CString name;
				if (util::GetBetweenSubString( line2, L"</a> (", L")<div", name) < 0) {
					// 失敗したので ")" までを抽出
					util::GetBetweenSubString( line2, L"</a> (", L")", name);
				}

				data.SetName(name);
				data.SetAuthor(name);

				out_.push_back( data );
				iLine += 5;
			}
			else if (bDataFound && line.Find(_T("pageNavigation01")) != -1) {
				// 終了タグ発見
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

		MZ3LOGGER_DEBUG( L"ListNewFriendDiaryParser.parse() finished." );
		return true;
	}

private:
	/// 「次を表示」、「前を表示」のリンクを抽出する
	static bool parseNextBackLink( CMixiData& nextLink, CMixiData& backLink, CString str )
	{
		// 正規表現のコンパイル（一回のみ）
		static MyRegex reg;
		if( !util::CompileRegex( reg, L"<a href=[\"]?new_friend_diary.pl([?]page=[^\"^>]+)[\"]?>([^<]+)</a>" ) ) {
			MZ3LOGGER_FATAL( FAILED_TO_COMPILE_REGEX_MSG );
			return false;
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

		BOOL bEntryStarted = FALSE;
		BOOL bItemFound = FALSE;

		//int index;
		for (int i=160; i<count; i++) {

			const CString& line = html_.GetAt(i);

			if (bEntryStarted == FALSE) {
				if (util::LineHasStringsNoCase( line, L"entryList01") ) {
					// 開始フラグ発見。
					// とりあえずN行無視する。
					i += 3;
					bEntryStarted = TRUE;
				}
				continue;
			}
			else {

				if (util::LineHasStringsNoCase( line, L"<dt>") ) {
					// 項目発見
					bItemFound = TRUE;

					CMixiData data;
					data.SetAccessType(ACCESS_DIARY);

					// 日付
					// <dt>2007年10月01日&nbsp;01:11</dt>
					ParserUtil::ParseDate(line, data);
					MZ3LOGGER_DEBUG( L"date : " + data.GetDate() );

					// 見出しタイトル
					//<dd><a href="view_diary.pl?id=xxx&owner_id=yyy&comment_count=2">タイトル(2)</a>&nbsp;(なまえ)</dd>
					i += 1;
					const CString& line2 = html_.GetAt(i);

					CString title;
					util::GetBetweenSubString( line2, L"\">", L"</a>", title );
					data.SetTitle(title);
					MZ3LOGGER_DEBUG( L"title : " + data.GetTitle() );

					// URL
					CString url;
					util::GetBetweenSubString( line2, L"href=\"", L"\"", url );
					data.SetURL(url);
					MZ3LOGGER_DEBUG( L"URL : " + data.GetURL() );

					// ＩＤを設定
					data.SetID( MixiUrlParser::GetID(url) );

					// 名前
					CString author;
					if( util::GetBetweenSubString( line2, L"&nbsp;(", L")</dd>", author ) < 0 ) {
						// not found.
						continue;
					}

					data.SetName( author );
					data.SetAuthor( author );
					TRACE(_T("%s\n"), data.GetName());

					out_.push_back(data);
				}
				else if (line.Find(_T("/newCommentArea")) != -1 && bItemFound) {
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
		data_.ClearChildren();

		// html_ の文字列化
		std::vector<TCHAR> text;
		html_.TranslateToVectorBuffer( text );

		// XML 解析
		xml2stl::Container root;
		if (!xml2stl::SimpleXmlParser::loadFromText( root, text )) {
			MZ3LOGGER_ERROR( L"XML 解析失敗" );
			return false;
		}

		try {
			const xml2stl::Node& body = root.getNode( L"html" )
											.getNode( L"body" );

			const xml2stl::Node& bodyMainArea = body.getNode( L"div", L"id=bodyArea" )
													.getNode( L"div", L"id=bodyMainArea" );

			// <title> タグからタイトルを取得する
			try {
				// /html/head/title
				const xml2stl::Node& titleNode = root.getNode(L"html")
													 .getNode(L"head")
													 .getNode(L"title");

				CString title = titleNode.getTextAll().c_str();

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

			} catch (xml2stl::NodeNotFoundException& e) {
				MZ3LOGGER_ERROR( util::FormatString( L"タイトル取得エラー : %s", e.getMessage().c_str()) );
			}

			// 「最近の日記」の取得
			try {
				// /html/body/div[2]/div/div[3]/div[2]/div[2]/ul
				const xml2stl::Node& ul = bodyMainArea.getNode(L"div", L"id=bodyMainAreaSub")
													  .getNode(L"div", L"class=newDiary")
													  .getNode(L"div", L"class=contents")
													  .getNode(L"ul");

				for (size_t i=0; i<ul.getChildrenCount(); i++) {
					const xml2stl::Node& li = ul.getNodeByIndex(i);
					if (li.getName()==L"li") {
						const xml2stl::Node& a = li.getNode(L"a");
						CString title = a.getTextAll().c_str();
						CString url   = a.getProperty(L"href").c_str();

						CMixiData::Link link( url, title );
						data_.m_linkPage.push_back( link );
					}
				}

			} catch (xml2stl::NodeNotFoundException& e) {
				MZ3LOGGER_ERROR( util::FormatString( L"最近の日記 取得エラー : %s", e.getMessage().c_str()) );
			}

			// 日時の取得
			try {
				// /html/body/div[2]/div/div[2]/div[2]/div/dl/dd
				const xml2stl::Node& dd = bodyMainArea.getNode(L"div", L"id=bodyMainAreaMain")
													  .getNode(L"div", L"class=viewDiaryBox")
													  .getNode(L"div", L"class=listDiaryTitle")
													  .getNode(L"dl")
													  .getNode(L"dd");

				data_.SetDate( dd.getTextAll().c_str() );

			} catch (xml2stl::NodeNotFoundException& e) {
				MZ3LOGGER_ERROR( util::FormatString( L"日時の取得エラー : %s", e.getMessage().c_str()) );
			}

			// 公開範囲の取得
			try {
				// /html/body/div[2]/div/div[2]/div[2]/div/dl/dd
				const xml2stl::Node& img = bodyMainArea.getNode(L"div", L"id=bodyMainAreaMain")
													  .getNode(L"div", L"class=viewDiaryBox")
													  .getNode(L"div", L"class=listDiaryTitle")
													  .getNode(L"dl")
													  .getNode(L"dt")
													  .getNode(L"img");

				data_.SetOpeningRange( img.getProperty(L"alt").c_str() );

			} catch (xml2stl::NodeNotFoundException& e) {
				MZ3LOGGER_ERROR( util::FormatString( L"公開範囲の取得エラー : %s", e.getMessage().c_str()) );
			}

			// 日記の著者
			try {
				// 自分の日記なら「XXXの日記」、自分以外なら「XXXさんの日記」のはず。
				// この規則で著者を解析。
				CString author;
				for (size_t i=0; i<bodyMainArea.getChildrenCount(); i++) {
					const xml2stl::Node& node = bodyMainArea.getNodeByIndex(i);
					try {
						const xml2stl::XML2STL_STRING& class_value = node.getProperty(L"class");

						if (wcsstr(class_value.c_str(), L"diaryTitleFriend") != NULL) {
							CString line = bodyMainArea.getNodeByIndex(i).getTextAll().c_str();
							util::GetBetweenSubString( line, L">", L"さんの日記<", author );
							break;
						}
						if (wcsstr(class_value.c_str(), L"diaryTitle") != NULL) {
							CString line = bodyMainArea.getNodeByIndex(i).getTextAll().c_str();
							util::GetBetweenSubString( line, L">", L"の日記<", author );
							break;
						}
					} catch (...) {
					}
				}

				// 著者設定
				data_.SetAuthor( author );
				data_.SetName( author );

			} catch (xml2stl::NodeNotFoundException& e) {
				MZ3LOGGER_ERROR( util::FormatString( L"日記の著者の取得エラー : %s", e.getMessage().c_str()) );
			}

			// とりあえず改行出力
			data_.AddBody(_T("\r\n"));

			// 日記の添付写真
			try {
				const xml2stl::Node& div = bodyMainArea.getNode(L"div", L"id=bodyMainAreaMain")
													   .getNode(L"div", L"class=viewDiaryBox")
													   .getNode(L"div", L"class=txtconfirmArea")
													   .getNode(L"div", L"class=diaryPhoto");

				CString subHtml = div.getTextAll().c_str();
				ParserUtil::AddBodyWithExtract( data_, subHtml );

				// 改行
				data_.AddBody(_T("\r\n"));
			} catch (xml2stl::NodeNotFoundException& e) {
				// 写真がなかったと判断する
				MZ3LOGGER_INFO( util::FormatString( L"写真の取得エラー : %s", e.getMessage().c_str()) );
			}

			// 前の日記へのリンク
			try {
				const xml2stl::Node& div = bodyMainArea.getNode(L"div", L"id=bodyMainAreaMain")
													   .getNode(L"div", L"class=diaryPaging01 clearfix")
													   .getNode(L"div", L"class=diaryPagingLeft");

				CString PrevLink = div.getTextAll().c_str();
				mixi::ParserUtil::ReplaceDefinedEntityReferenceToCharacter( PrevLink );
				mixi::ParserUtil::ExtractURI( PrevLink , data_.m_linkList );

				data_.SetPrevDiary( PrevLink );
			} catch (xml2stl::NodeNotFoundException& e) {
				// リンクがなかったと判断する
				MZ3LOGGER_INFO( util::FormatString( L"前の日記へのリンク取得エラー : %s", e.getMessage().c_str()) );
			}

			// 次の日記へのリンク
			try {
				const xml2stl::Node& div = bodyMainArea.getNode(L"div", L"id=bodyMainAreaMain")
													   .getNode(L"div", L"class=diaryPaging01 clearfix")
													   .getNode(L"div", L"class=diaryPagingRight");

				CString NextLink = div.getTextAll().c_str();
				mixi::ParserUtil::ReplaceDefinedEntityReferenceToCharacter( NextLink );
				mixi::ParserUtil::ExtractURI( NextLink , data_.m_linkList );

				data_.SetNextDiary( NextLink );
			} catch (xml2stl::NodeNotFoundException& e) {
				// リンクがなかったと判断する
				MZ3LOGGER_INFO( util::FormatString( L"次の日記へのリンク取得エラー : %s", e.getMessage().c_str()) );
			}

			// 本文取得
			try {
				const xml2stl::Node& div = bodyMainArea.getNode(L"div", L"id=bodyMainAreaMain")
													   .getNode(L"div", L"class=viewDiaryBox")
													   .getNode(L"div", L"class=txtconfirmArea")
													   .getNode(L"div", L"id=diary_body");

				CString subHtml = div.getTextAll().c_str();

				// script タグの除去
				{
					static MyRegex reg;
					if( !util::CompileRegex( reg, L"<script *type=\"[^\"]*\">" ) ) {
						return false;
					}
					if( subHtml.Find( L"<script" ) != -1 ) 
						reg.replaceAll( subHtml, L"" );
				}
				while( subHtml.Replace(_T("</script>"), _T("")) );

				ParserUtil::AddBodyWithExtract( data_, subHtml );
			} catch (xml2stl::NodeNotFoundException& e) {
				// 外部ブログかもしれない
				try {
					const xml2stl::Node& messageAlert = bodyMainArea.getNode(L"div", L"class=messageArea")
														   .getNode(L"div", L"class=contents")
														   .getNode(L"div")
														   .getNode(L"p", L"class=messageAlert");
					
					CString subHtml = messageAlert.getTextAll().c_str();
					ParserUtil::AddBodyWithExtract( data_, subHtml );

				} catch (xml2stl::NodeNotFoundException& e) {
					MZ3LOGGER_ERROR( util::FormatString( L"外部ブログの取得エラー : %s", e.getMessage().c_str()) );
				}
				MZ3LOGGER_ERROR( util::FormatString( L"本文の取得エラー : %s", e.getMessage().c_str()) );
			}

			// コメント取得
			parseDiaryComment(data_, bodyMainArea);
		} catch (xml2stl::NodeNotFoundException& e) {
			MZ3LOGGER_ERROR( util::FormatString( L"body not found... : %s", e.getMessage().c_str()) );
			return false;
		}

		MZ3LOGGER_DEBUG( L"ViewDiaryParser.parse() finished." );
		return true;
	}

private:
	/**
	 * コメントの取得
	 */
	static bool parseDiaryComment(CMixiData& data_, const xml2stl::Node& bodyMainArea)
	{
		try {
			const xml2stl::Node& div = bodyMainArea.getNode(L"div", L"id=bodyMainAreaMain")
												   .getNode(L"div", L"id=diaryComment")
												   .getNode(L"div", L"class=diaryMainArea02 commentList");
			// throw なし => div class=diaryMainArea02 commentList があるのでdiv#diaryComment 等がある。
			parseDiaryComment_sub(data_, div);

		} catch (...) {
			// throw あり => div class=diaryMainArea02 commentList がないのでdiv class=diaryMainArea02 deleteButtonを探す。
			try {
				const xml2stl::Node& form = bodyMainArea.getNode(L"div", L"id=bodyMainAreaMain")
													   .getNode(L"div", L"id=diaryComment")
													   .getNode(L"div", L"class=diaryMainArea02 deleteButton")
													   .getNode(L"form");
				// throw なし => form があるので、form 配下に div#diaryCommentbox 等がある（自分の日記）。
				parseDiaryComment_sub(data_, form);
			} catch (xml2stl::NodeNotFoundException& e) {
				MZ3LOGGER_INFO( util::FormatString( L"コメントの取得エラー : %s", e.getMessage().c_str()) );
			}
		}

		try {
			// <form name="comment_form" ...> がある div 要素を探索する
			const xml2stl::Node& divDiaryComment = bodyMainArea.getNode(L"div", L"id=bodyMainAreaMain")
															   .getNode(L"div", L"id=diaryComment");
			for (size_t i=0; i<divDiaryComment.getChildrenCount(); i++) {
				const xml2stl::Node& div2 = divDiaryComment.getNodeByIndex(i);

				try {
					// div2 から form を探す
					div2.getNode(L"form", L"name=comment_form");
					// throw なし => 対象の form
				} catch (...) {
					// form なし

					// 自分の日記の場合は、
					// <div class="diaryMainArea02">
					//  <div class="diaryCommentboxReply">
					//   <form name="comment_form"
					// となる。
					try {
						div2.getNode(L"div", L"class=diaryCommentboxReply");
						// throw なし => 対象の div
					} catch (...) {
						// diaryCommentboxReply なし
						continue;
					}
				}

				CHtmlArray html;
				html.Add( div2.getTextAll().c_str() );
				parsePostURL( 0, data_, html );
			}

		} catch (xml2stl::NodeNotFoundException& e) {
			MZ3LOGGER_INFO( util::FormatString( L"コメントの取得エラー : %s", e.getMessage().c_str()) );
		}
		return true;
	}

	static bool parseDiaryComment_sub(CMixiData& data_, const xml2stl::Node& div)
	{
		int comment_number = 1;

		size_t n = div.getChildrenCount();
		for (size_t i=0; i<n; i++) {
			const xml2stl::Node& divDiaryCommentBox = div.getNodeByIndex(i);
			try {
				if (divDiaryCommentBox.getProperty(L"class") == L"diaryCommentbox" ||
					divDiaryCommentBox.getProperty(L"class") == L"diaryCommentboxLast")
				{
					try {
						const xml2stl::Node& dl = divDiaryCommentBox.getNode(L"dl");
						
						const xml2stl::Node& dt = dl.getNode(L"dt");
						const xml2stl::Node& spanName = dt.getNode(L"span", L"class=commentTitleName");
						const xml2stl::Node& spanDate = dt.getNode(L"span", L"class=commentTitleDate");
						const xml2stl::Node& dd = dl.getNode(L"dd");

						CMixiData comment_data;
						
						// コメント番号
						comment_data.SetCommentIndex(comment_number++);

						// 名前
						ParserUtil::GetAuthor(spanName.getTextAll().c_str(), &comment_data);

						// 時刻
						ParserUtil::ParseDate(spanDate.getTextAll().c_str(), comment_data);

						// コメント本文
						comment_data.AddBody(_T("\r\n"));		// 改行出力
						ParserUtil::AddBodyWithExtract(comment_data, dd.getTextAll().c_str());

						data_.AddChild( comment_data );

					} catch (xml2stl::NodeNotFoundException& e) {
						MZ3LOGGER_INFO( util::FormatString( L"コメントの取得エラー : %s", e.getMessage().c_str()) );
					}
				}
			} catch (...) {
			}
		}

		return true;
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
				ParserUtil::ParseDate(str, data);

				// 見出し
				i += 1;
				CString str2 = html_.GetAt(i);
				// <dd><a href="view_bbs.pl?id=20728968&comment_count=3&comm_id=1198460">【チャット】集え！xxx</a> (MZ3 -Mixi for ZERO3-)</dd>
				CString after;
				if (!util::GetAfterSubString( str2, L"href=", after )) {
					MZ3LOGGER_ERROR(L"取得できません:" + str );
					return false;
				}
				CString title;
				util::GetBetweenSubString( after, L">", L"<", title );
				
				//アンケート、イベントの場合はタイトルの前にマークを付ける
				if(util::LineHasStringsNoCase( str, L"<dt", L"class", L"iconEvent" )){
					data.SetTitle(L"【☆】" + title);
				}else if(util::LineHasStringsNoCase( str, L"<dt", L"class", L"iconEnquete" )){
					data.SetTitle(L"【＠】" + title);
				}else{
					data.SetTitle(title);
				}

				// ＵＲＩ
				CString href;
				util::GetBetweenSubString( str2, L"\"", L"\"", href );

				// &で分解する
				while (href.Replace(_T("&amp;"), _T("&")));

				data.SetURL(href);
				data.SetCommentCount(
					MixiUrlParser::GetCommentCount( href ) );

				// URL に応じてアクセス種別を設定
				data.SetAccessType( util::EstimateAccessTypeByUrl(href) );

				// ＩＤを設定
				data.SetID( MixiUrlParser::GetID(href) );

				// コミュニティ名
				CString communityName;
				util::GetBetweenSubString( str2, L"</a>", L"</dd>", communityName );

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
		if( !util::CompileRegex( reg, L"<a href=[\"]?new_bbs.pl([?]page=[^\"^>]+)[\"]?>([^<]+)</a>" ) ) {
			MZ3LOGGER_FATAL( FAILED_TO_COMPILE_REGEX_MSG );
			return false;
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
 * [list] new_bbs_comment.pl 用パーサ
 * 【コミュニティコメント記入履歴】
 * http://mixi.jp/new_bbs_comment.pl
 */
class ListNewBbsCommentParser : public MixiListParser
{
public:
	static bool parse( CMixiDataList& out_, const CHtmlArray& html_ )
	{
		MZ3LOGGER_DEBUG( L"ListNewBbsCommentParser.parse() start." );

		// NewBbsParser に委譲
		bool rval = NewBbsParser::parse( out_, html_ );

		MZ3LOGGER_DEBUG( L"ListNewBbsCommentParser.parse() finished." );
		return rval;
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
	/// <td align=right>1件〜50件を表示&nbsp;&nbsp;<a href=list_community.pl?page=2&id=xxx>次を表示</a></td>
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
			if( util::LineHasStringsNoCase( line, L"<span", L"titleSpan" ) ) {
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
	/// <td ALIGN=right BGCOLOR=#EED6B5>1件〜20件を表示&nbsp;&nbsp;<a href=list_bbs.pl?page=2&id=xxx>次を表示</a></td></tr>
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


/**
 * [content] list_event_member.pl 用パーサ
 * 【イベント参加者一覧】
 * http://mixi.jp/list_event_member.pl
 */
class ListEventMemberParser : public MixiContentParser
{
public:
	static bool parse( CMixiData& data_, const CHtmlArray& html_ )
	{
		MZ3LOGGER_DEBUG( L"ListEventMemberParser.parse() start." );

		data_.ClearAllList();
		data_.ClearChildren();

		// html_ の文字列化
		std::vector<TCHAR> text;
		html_.TranslateToVectorBuffer( text );

		// XML 解析
		xml2stl::Container root;
		if (!xml2stl::SimpleXmlParser::loadFromText( root, text )) {
			MZ3LOGGER_ERROR( L"XML 解析失敗" );
			return false;
		}

		try {
			const xml2stl::Node& body = root.getNode( L"html" )
											.getNode( L"body" );

			const xml2stl::Node& bodyMainArea = body.getNode( L"div", L"id=bodyArea" )
													.getNode( L"div", L"id=bodyMainArea" );

			// タイトルを取得する
			try {
				const xml2stl::Node& div = bodyMainArea.getNode(L"div", L"class=pageTitle communityTitle002")
													   .getNode(L"h2");

				data_.SetTitle(div.getTextAll().c_str());
				data_.SetName(div.getTextAll().c_str());

			} catch (xml2stl::NodeNotFoundException& e) {
				MZ3LOGGER_ERROR( util::FormatString( L"タイトルエラー : %s", e.getMessage().c_str()) );
			}

			// 参加者一覧を取得する
			try {
				// /html/body/div[2]/div/div[4]
				const xml2stl::Node& div = bodyMainArea.getNode(L"div", L"class=iconList03");

				int user_num = 1;

				for (size_t i=0; i<div.getChildrenCount(); i++) {
					const xml2stl::Node& ul = div.getNodeByIndex(i);
					if (ul.getName() != L"ul") {
						continue;
					}

					for (size_t j=0; j<ul.getChildrenCount(); j++) {
						const xml2stl::Node& li = ul.getNodeByIndex(j);
						if (li.getName() != L"li") {
							continue;
						}

						const xml2stl::Node& li_div = li.getNode(L"div");
						const xml2stl::Node& a_node = li_div.getNode(L"div").getNode(L"a");

						// name
						CString name = li_div.getNode(L"span").getTextAll().c_str();

						// href
						CString url = a_node.getProperty(L"href").c_str();

						// 画像URL
						CString image_url = a_node.getProperty(L"style").c_str();
						util::GetBetweenSubString(image_url, L"url(", L")", image_url);

						// 登録
						CMixiData user;
						user.SetCommentIndex(user_num++);
						user.SetName(name);
						user.SetTitle(name);
						user.SetAuthor(name);
						user.SetAuthorID( mixi::MixiUrlParser::GetID(url) );
						user.SetURL( url );
						user.SetAccessType( ACCESS_PROFILE );

						user.m_linkList.push_back( CMixiData::Link(image_url, L"ユーザ画像") );

						data_.AddChild(user);
					}
				}

			} catch (xml2stl::NodeNotFoundException& e) {
				MZ3LOGGER_ERROR( util::FormatString( L"参加者一覧取得エラー : %s", e.getMessage().c_str()) );
			}

		} catch (xml2stl::NodeNotFoundException& e) {
			MZ3LOGGER_ERROR( util::FormatString( L"body not found... : %s", e.getMessage().c_str()) );
			return false;
		}

		MZ3LOGGER_DEBUG( L"ListEventMemberParser.parse() finished." );
		return true;
	}

private:
};


/**
 * [content] show_friend.pl 用パーサ
 * 【プロフィール】
 * http://mixi.jp/show_friend.pl
 */
class ShowFriendParser : public MixiContentParser
{
public:
	static bool parse( CMixiData& mixi, const CHtmlArray& html_ )
	{
		MZ3LOGGER_DEBUG( L"ShowFriendParser.parse() start." );

		mixi.ClearAllList();
		mixi.ClearChildren();
		mixi.SetName( L"" );
		mixi.SetDate( L"" );

		// html_ の文字列化
		std::vector<TCHAR> text;
		html_.TranslateToVectorBuffer( text );

		// XML 解析
		xml2stl::Container root;
		if (!xml2stl::SimpleXmlParser::loadFromText( root, text )) {
			MZ3LOGGER_ERROR( L"XML 解析失敗" );
			return false;
		}

		// 名前
		// /html/body/div[2]/div/div/div/div/h3
		try {
			const xml2stl::Node& h3 = root.getNode( L"html" )
										  .getNode( L"body" )
										  .getNode( L"div", 1 )
										  .getNode( L"div" )
										  .getNode( L"div" )
										  .getNode( L"div" )
										  .getNode( L"div" )
										  .getNode( L"h3" );
			CString name = h3.getTextAll().c_str();
			mixi.SetName( name );
			mixi.SetTitle( name );
			mixi.SetAuthor( name );
		} catch (xml2stl::NodeNotFoundException& e) {
			MZ3LOGGER_ERROR( util::FormatString( L"h3 not found... : %s", e.getMessage().c_str()) );
		}

		// ユーザ画像
		// /html/body/div[2]/div/div/div/div/img
		try {
			const xml2stl::Node& img = root.getNode( L"html" )
										   .getNode( L"body" )
										   .getNode( L"div", 1 )
										   .getNode( L"div" )
										   .getNode( L"div" )
										   .getNode( L"div" )
										   .getNode( L"div" )
										   .getNode( L"img" );
			LPCTSTR url = img.getProperty( L"src" ).c_str();
			mixi.m_linkList.push_back( CMixiData::Link( url, L"ユーザ画像" ) );

			// プロフィール画面にユーザ画像リンクを表示する
			mixi.AddBody(_T("<_a><<ユーザ画像>></_a>"));

			MZ3LOGGER_DEBUG( util::FormatString( L"user image : [%s]", url ) );
		} catch (xml2stl::NodeNotFoundException& e) {
			MZ3LOGGER_ERROR( util::FormatString( L"img not found... : %s", e.getMessage().c_str()) );
		}

		// 友人関係
		// /html/body/div[2]/div/div/div/p
		try {
			const xml2stl::Node& friendPath = root.getNode( L"html" )
										   .getNode( L"body" )
										   .getNode( L"div", 1 )
										   .getNode( L"div" )
										   .getNode( L"div" )
										   .getNode( L"div" )
										   .getNode( L"p" , xml2stl::Property(L"class", L"friendPath"));
			CString path = friendPath.getTextAll().c_str();

			if( path != L"" ) {
				// とりあえず改行
				mixi.AddBody(_T("\r\n"));

				// プロフィール画面に友人関係を表示する
				ParserUtil::AddBodyWithExtract( mixi, path );

				MZ3LOGGER_DEBUG( util::FormatString( L"user path : [%s]", path ) );
			}
		} catch (xml2stl::NodeNotFoundException& e) {
			MZ3LOGGER_ERROR( util::FormatString( L"user path not found... : %s", e.getMessage().c_str()) );
		}

		// プロフィールを全て取得し、本文に設定する。
		// /html/body/div[2]/div/div#bodyContents/div#profile/ul
		try {
			const xml2stl::Node& ul = root.getNode( L"html" )
										  .getNode( L"body" )
										  .getNode( L"div", xml2stl::Property(L"id", L"bodyArea") )
										  .getNode( L"div", xml2stl::Property(L"id", L"bodyMainArea") )
										  .getNode( L"div", xml2stl::Property(L"id", L"bodyContents") )
										  .getNode( L"div", xml2stl::Property(L"id", L"profile") )
										  .getNode( L"ul" );

			// とりあえず改行
			mixi.AddBody(_T("\r\n"));

			// 各liを追加していく
			int nChildren = ul.getChildrenCount();
			for (int i=0; i<nChildren; i++) {
				const xml2stl::Node& li = ul.getNodeByIndex(i);
				if (li.getName()!=L"li")
					continue;

				const xml2stl::Node& dl = li.getNode( L"dl" );

				// 項目の名称
				CString target = util::FormatString( L"■ %s", dl.getNode(L"dt").getTextAll().c_str() );
				ParserUtil::AddBodyWithExtract( mixi, target );
				mixi.AddBody(_T("\r\n"));

				// 項目の内容
				const xml2stl::Node& dd = dl.getNode( L"dd" );
				target = dd.getTextAll().c_str();
				ParserUtil::AddBodyWithExtract( mixi, target );
				mixi.AddBody(_T("\r\n"));
				mixi.AddBody(_T("\r\n"));
			}
		} catch (xml2stl::NodeNotFoundException& e) {
			MZ3LOGGER_ERROR( util::FormatString( L"(プロフィール本文) not found... : %s", e.getMessage().c_str()) );
		}

		int nChildItemNumber = 1;

		// 最新の日記取得
		// /html/body/div[2]/div/div[2]/div[3]/div/div[2]/dl
		try {
			const xml2stl::Node& dl = root.getNode( L"html" )
										  .getNode( L"body" )
										  .getNode( L"div", xml2stl::Property(L"id", L"bodyArea") )
										  .getNode( L"div" )
										  .getNode( L"div", xml2stl::Property(L"id", L"bodyContents") )
										  .getNode( L"div", xml2stl::Property(L"id", L"mymixiUpdate") )
										  .getNode( L"div", xml2stl::Property(L"id", L"newFriendDiary") )
										  .getNode( L"div", xml2stl::Property(L"class", L"contents") )
										  .getNode( L"dl" );

			// dt/span, dd/a が交互に出現する。
			int n = dl.getChildrenCount();

			CMixiData diaryItem;
			CMixiData::Link link( L"", L"" );

			// とりあえず改行
			diaryItem.AddBody(_T("\r\n"));

			for (int i=0; i<n; i++) {
				const xml2stl::Node& node = dl.getNodeByIndex(i);
				if (node.getName() == L"dt") {
					CString date = node.getNode(L"span").getTextAll().c_str();
					
					// リンク名設定
					link.text += date;

					// 本文設定
					diaryItem.AddBody( util::FormatString(L"■ %s", date ) );
				}
				if (node.getName() == L"dd") {
					const xml2stl::Node& a = node.getNode(L"a");

					// リンク名設定
					link.text += L" : ";
					link.text += a.getTextAll().c_str();

					// URL, IDを設定
					link.url = a.getProperty( L"href" ).c_str();
					diaryItem.m_linkList.push_back(link);

					// 本文に追加
					ParserUtil::AddBodyWithExtract( diaryItem, util::FormatString(L" : %s", a.getTextAll().c_str()) );
					diaryItem.AddBody( L"\r\n" );
					ParserUtil::AddBodyWithExtract( diaryItem, util::FormatString(L" (<_a>%s</_a>)", link.url) );
					diaryItem.AddBody( L"\r\n" );
					diaryItem.AddBody( L"\r\n" );

					// 初期化
					link = CMixiData::Link( L"", L"" );
				}
			}
			// 登録
			diaryItem.SetCommentIndex( nChildItemNumber++ );
			diaryItem.SetAuthor( L"最新の日記" );
			mixi.AddChild( diaryItem );

		} catch (xml2stl::NodeNotFoundException& e) {
			MZ3LOGGER_ERROR( util::FormatString( L"(最新の日記) not found... : %s", e.getMessage().c_str()) );
		}

		// 紹介文の取得
		// /html/body/div[2]/div/div[2]/div[4]/div[2]
		try {
			const xml2stl::Node& div = root.getNode( L"html" )
										   .getNode( L"body" )
										   .getNode( L"div", xml2stl::Property(L"id", L"bodyArea") )
										   .getNode( L"div" )
										   .getNode( L"div", xml2stl::Property(L"id", L"bodyContents") )
										   .getNode( L"div", xml2stl::Property(L"id", L"intro") )
										   .getNode( L"div", xml2stl::Property(L"class", L"contents") );

			// dl が続く。
			int n = div.getChildrenCount();
			for (int i=0; i<n; i++) {
				const xml2stl::Node& dl = div.getNodeByIndex(i);
				if (dl.getName() != L"dl") {
					continue;
				}

				CMixiData introItem;
				introItem.AddBody( L"\r\n" );
				
				// dt/a[2] : 名前
				// dt/a[1]/img/@src : 画像
				const xml2stl::Node& dt = dl.getNode(L"dt");
				CString name = dt.getNode( L"a", 1 ).getTextAll().c_str();
				CString url  = dt.getNode( L"a", 1 ).getProperty(L"href").c_str();

				// dd/p class=relation  : 関係
				// dd/p class=userInput : 紹介文
				CString intro, relation;
				const xml2stl::Node& dd = dl.getNode(L"dd");
				size_t n_dd_children = dd.getChildrenCount();
				for (size_t j=0; j<n_dd_children; j++) {
					const xml2stl::Node& dd_sub = dd.getNodeByIndex(j);
					if (dd_sub.isNode() && dd_sub.getName() == L"p") {
						CString className = dd_sub.getProperty(L"class").c_str();
						if (className==L"relation") {
							relation = dd_sub.getTextAll().c_str();
						} else if (className==L"userInput") {
							intro    = dd_sub.getTextAll().c_str();
						}
					}
				}

				if (!relation.IsEmpty()) {
					ParserUtil::AddBodyWithExtract( introItem, relation );
					introItem.AddBody( L"\r\n" );
				}
				if (!intro.IsEmpty()) {
					ParserUtil::AddBodyWithExtract( introItem, intro );

					// 登録
					introItem.SetCommentIndex( nChildItemNumber++ );
					introItem.SetAuthor( util::FormatString( L"紹介文(%s)", name ) );
					introItem.SetAuthorID( mixi::MixiUrlParser::GetID(url) );
					introItem.SetURL( url );
					introItem.SetAccessType( ACCESS_PROFILE );
					mixi.AddChild( introItem );
				}
			}
		} catch (xml2stl::NodeNotFoundException& e) {
			MZ3LOGGER_ERROR( util::FormatString( L"(紹介文) not found... : %s", e.getMessage().c_str()) );
		}

		MZ3LOGGER_DEBUG( L"ShowFriendParser.parse() finished." );
		return true;
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
			// +1 〜 +4 行目の辺りにあるはず。
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

			ParserUtil::AddBodyWithExtract( data_, line );
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
					const CString& line1 = html_.GetAt( i-2 );
					if( util::GetBetweenSubString( line1, L"<td>", L" \n", buf ) < 0 ) {
						continue;
					}
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
			// <font COLOR=#996600>日　付</font>&nbsp;:&nbsp;2007年10月08日 21時52分&nbsp;&nbsp;
			if( util::LineHasStringsNoCase( str, L"<font", L" COLOR=#996600", L">", L"日　付" ) ) {
				CString buf = str;
				buf.Replace(_T("時"), _T(":"));
				ParserUtil::ParseDate(buf, data_);
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
 * http://mixi.jp/list_friend.pl または http://mixi.jp/ajax_friend_setting.pl
 */
class ListFriendParser : public MixiListParser
{
public:
	static bool parse( CMixiDataList& out_, const CHtmlArray& html_ )
	{
		bool rval = false;

		MZ3LOGGER_DEBUG( L"ListFriendParser.parse() start." );

		// "{" から始まっていれば ajax_friend_setting.pl とみなす。
		bool ajax_friend_setting_mode = false;
		INT_PTR lastLine = html_.GetCount();
		for (int i=0; i<1 && i<lastLine; i++) {
			const CString& line = html_.GetAt(i);
			if (line.Left(1)==L"{") {
				ajax_friend_setting_mode = true;
			}
		}

		if (ajax_friend_setting_mode) {
			MZ3LOGGER_INFO(L"JSON 解析開始");

			// JSON の解析を行う
			
			// ...
			// {"new_friend_diary":null,"nickname":"xxx",
			//  "photo":"http://profile.img.mixi.jp/photo/member/54/75/xxx",
			//  "member_count":"112",
			//  "relation_id":null,"tag_ids":null,"lastlogin_level":"3","member_id":"xxx"},
			// {"new_friend_diary":null, ...

			CString line;
			if (lastLine>=1) {
				for (int i=0; i<lastLine; i++) {
					line.Append(html_.GetAt(i));
				}
			}
			int iStart = 0;
			for (;;) {
				iStart = line.Find(L"{\"new_friend_diary\"", iStart);
				if (iStart<0) {
					break;
				}
				int iEnd = line.Find(L"}", iStart);
				CString target = line.Mid(iStart, iEnd-iStart);
				MZ3LOGGER_DEBUG(util::FormatString(L"[%d,%d] : ", iStart, iEnd) + target);
				iStart = iEnd;

				CMixiData data;

				// url
				CString member_id;
				util::GetBetweenSubString(target, L"\"member_id\":\"", L"\"", member_id);
				CString url = L"show_friend.pl?id=";
				url.Append(member_id);
				data.SetURL(url);

				// URL 構築＆設定
				url.Insert( 0, L"http://mixi.jp/" );
				data.SetBrowseUri( url );

				// photo
				CString image_url;
				util::GetBetweenSubString(target, L"\"photo\":\"", L"\"", image_url);
				data.AddImage(image_url);

				// nickname
				CString nickname;
				util::GetBetweenSubString(target, L"\"nickname\":\"", L"\"", nickname);
				data.SetName(nickname);

				// date(lastlogin_level)
				CString lastlogin_level;
				util::GetBetweenSubString(target, L"\"lastlogin_level\":\"", L"\"", lastlogin_level);
				switch (_wtoi(lastlogin_level)) {
				case 2:
					data.SetDate(L"1日以内");
					break;
				case 3:
					data.SetDate(L"1時間以内");
					break;
				case 1:
				default:
					data.SetDate(L"");
					break;
				}

				// 追加
				data.SetAccessType(ACCESS_PROFILE);

				out_.push_back(data);
			}

			MZ3LOGGER_INFO(L"JSON 解析終了");
			rval = true;
		} else {
			MZ3LOGGER_INFO(L"HTML 解析開始");

			// post_key の取得を行う
			for (int i=0; i<lastLine; i++) {
				const CString& line = html_.GetAt(i);

				if (util::LineHasStringsNoCase(line, L"<input", L"type", L"hidden", L"name", L"post_key")) {
					// <input type="hidden" value="536bdcfd534cc1125c3b0b2c0eda942b" name="post_key" />
					CString post_key;
					if (util::GetBetweenSubString(line, L"value=\"", L"\"", post_key)>=0) {
						// dummy アイテムに設定する
						CMixiData data;
						data.SetTextValue(L"post_key", post_key);
						out_.push_back(data);
						rval = true;
					}
				}
			}

			MZ3LOGGER_INFO(L"HTML 解析終了");
		}


		MZ3LOGGER_DEBUG( L"ListFriendParser.parse() finished." );
		return rval;
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
	/// <td align=right>1件〜50件を表示&nbsp;&nbsp;<a href=show_intro.pl?page=2&id=xxx>次を表示</a></td>
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
				ParserUtil::ParseDate( strDate, data );
				data.SetMyMixi( bMyMixi );

				out_.push_back( data );

			}
		}

		MZ3LOGGER_DEBUG( L"ShowLogParser.parse() finished." );
		return true;
	}
};



/**
 * [list] 足あとAPI 用パーサ
 * 【足あと】
 * http://mixi.jp/atom/tracks/r=2/member_id=
 */
class TrackParser : public MixiListParser
{
public:
	static bool parse( CMixiDataList& out_, const CHtmlArray& html_ )
	{
		MZ3LOGGER_DEBUG( L"TrackParser.parse() start." );

		// html_ の文字列化
		std::vector<TCHAR> text;
		html_.TranslateToVectorBuffer( text );

		// XML 解析
		xml2stl::Container root;
		if (!xml2stl::SimpleXmlParser::loadFromText( root, text )) {
			MZ3LOGGER_ERROR( L"XML 解析失敗" );
			return false;
		}

		// entry に対する処理
		try {
			const xml2stl::Node& feed = root.getNode( L"feed" );
			size_t nChildren = feed.getChildrenCount();
			for (size_t i=0; i<nChildren; i++) {
				const xml2stl::Node& node = feed.getNodeByIndex(i);
				if (node.getName() != L"entry") {
					continue;
				}
				try {
					const xml2stl::Node& entry = node;
					// オブジェクト生成
					CMixiData data;
					data.SetAccessType( ACCESS_PROFILE );

					// URL : entry/link/@href
					CString url = entry.getNode( L"link" ).getProperty( L"href" ).c_str();
					data.SetURL( url );
					data.SetBrowseUri( url );

					// name : entry/author/name
					const xml2stl::Node& author = entry.getNode( L"author" );
					data.SetName( author.getNode( L"name" ).getTextAll().c_str() );

					// 関係 : entry/author/tracks:relation
					const std::wstring& relation = author.getNode( L"tracks:relation" ).getTextAll();
					if (relation==L"friend") {
						data.SetMyMixi( true );
					} else {
						data.SetMyMixi( false );
					}

					// Image : entry/author/tracks:image
					data.AddImage( author.getNode( L"tracks:image" ).getTextAll().c_str() );

					// updated : entry/updated
					ParserUtil::ParseDate( entry.getNode( L"updated" ).getTextAll().c_str(), data );
	
					// 完成したので追加する
					out_.push_back( data );
				} catch (xml2stl::NodeNotFoundException& e) {
					MZ3LOGGER_ERROR( util::FormatString( L"some node or property not found... : %s", e.getMessage().c_str()) );
					break;
				}
			}
		} catch (xml2stl::NodeNotFoundException& e) {
			MZ3LOGGER_ERROR( util::FormatString( L"feed not found... : %s", e.getMessage().c_str()) );
		}

		MZ3LOGGER_DEBUG( L"TrackParser.parse() finished." );
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
	static bool parse( CMixiDataList& out_, const CHtmlArray& html_ );
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


/**
 * [list] みんなのエコー, ほか
 *
 * - http://mixi.jp/recent_echo.pl
 * - http://mixi.jp/res_echo.pl
 * - http://mixi.jp/list_echo.pl?id={owner_id}
 */
class RecentEchoParser : public MixiListParser
{
public:
	static bool parse( CMixiDataList& out_, const CHtmlArray& html_ )
	{
		MZ3LOGGER_DEBUG( L"RecentEchoParser.parse() start." );

		// html_ の文字列化
		std::vector<TCHAR> text;
		html_.TranslateToVectorBuffer( text );

		// XML 解析
		xml2stl::Container root;
		if (!xml2stl::SimpleXmlParser::loadFromText( root, text )) {
			MZ3LOGGER_ERROR( L"XML 解析失敗" );
			return false;
		}

		try {
			const xml2stl::Node& div_echo = root.getNode(L"html")
				                                .getNode(L"body")
											    .getNode(L"div", L"id=bodyArea")
											    .getNode(L"div", L"id=bodyMainArea")
											    .getNode(L"div", L"id=echo");

			// post_key の取得
			// input[name=post_key]
			CString post_key = div_echo.findNode(L"action=add_echo.pl")
									   .findNode(L"name=post_key").getProperty(L"value").c_str();
			MZ3LOGGER_INFO( util::FormatString(L"post_key : [%s]", post_key) );

			// post_key は全ての要素に設定する
			
			// tbody に対する処理
			const xml2stl::Node& tbody = div_echo.findNode(L"class=echoArchives")
				                                 .getNode(L"div", L"class=archiveList")
											     .getNode(L"table");
			size_t nChildren = tbody.getChildrenCount();
			for (size_t i=0; i<nChildren; i++) {
				const xml2stl::Node& tr = tbody.getNodeByIndex(i);
				if (tr.getName() != L"tr") {
					continue;
				}
				try {
					// オブジェクト生成
					CMixiData data;
					data.SetAccessType( ACCESS_MIXI_ECHO_USER );

					const xml2stl::Node& td_comment = tr.getNode(L"td", L"class=comment");
					// text : tr/td[@comment]
					CString strBody = td_comment.getTextAll().c_str();
					// strBody の <span> タグ以降は日付等のため削除
					util::GetBeforeSubString(strBody, L"<span>", strBody);
					strBody.Replace(L"</a>", L"</a>&nbsp;");
					mixi::ParserUtil::StripAllTags( strBody );
					mixi::ParserUtil::UnEscapeHtmlElement( strBody );
					data.AddBody( strBody );

					// 時間：tr/td[@comment]/span
					CString strDate = td_comment.getNode(L"span").getTextAll().c_str();
					// aタグは除去
					mixi::ParserUtil::StripAllTags( strDate );
					while( strDate.Replace( L"\n", L"" ) );
					data.SetDate(strDate);

					// 画像URL : tr/td[@thumb]/a/img/@src
					CString imageUrl = tr.getNode(L"td", L"class=thumb").getNode(L"a").getNode(L"img").getProperty(L"src").c_str();
					data.AddImage( imageUrl );

					// name : tr/td[@nickname]/a
					const xml2stl::Node& author = tr.getNode(L"td", L"class=nickname").getNode(L"a");
					CString name = author.getTextAll().c_str();
					mixi::ParserUtil::UnEscapeHtmlElement( name );
					while( name.Replace( L"\r\n", L"" ) );
					data.SetName( name );

					// プロフィール用URL
					// とりあえず
					// http://mixi.jp/list_echo.pl?id=xxxxx
					// を取得し、
					// http://mixi.jp/show_friend.pl?id=xxxxx
					// に変換する。
					CString url = L"http://mixi.jp/show_friend.pl?id=";
					url += util::GetParamFromURL(author.getProperty(L"href").c_str(), L"id");
					data.SetURL( url );

					// post_key は全ての要素に設定する
					data.SetTextValue(L"post_key", post_key);

					// 返信用データ
					// .../td[@comment]/div[#echo_member_id_*] : メンバーID => author_id に設定
					// .../td[@comment]/div[#echo_post_time_*] : 投稿時刻   => 記事ID 値に設定
					for (xml2stl::NodeRef nodeRef=td_comment.getChildrenNodeRef(); !nodeRef.isEnd(); nodeRef.next()) {
						const xml2stl::Node& item = nodeRef.getCurrentNode();
						try {
							if (item.getProperty(L"id").substr(0, 15)==L"echo_member_id_") {
								// echo_member_id_*
								data.SetAuthorID(_wtoi(item.getTextAll().c_str()));
//								MZ3_TRACE(L" echo_member_id : %s\n", item.getTextAll().c_str());
								continue;
							}
							if (item.getProperty(L"id").substr(0, 15)==L"echo_post_time_") {
								// echo_post_time_*
								data.SetTextValue(L"echo_post_time", item.getTextAll().c_str());
//								MZ3_TRACE(L" echo_post_time : %s\n", item.getTextAll().c_str());
								continue;
							}
						} catch (xml2stl::NodeNotFoundException&) {
							// id プロパティがないタグ(spanなど)もあるので続行
						}
					}

					// 完成したので追加する
					out_.push_back( data );
				} catch (xml2stl::NodeNotFoundException& e) {
					MZ3LOGGER_ERROR( util::FormatString( L"some node or property not found... : %s", e.getMessage().c_str()) );
					break;
				}
			}
		} catch (xml2stl::NodeNotFoundException& e) {
			MZ3LOGGER_ERROR( e.getMessage().c_str() );
			return false;
		}

		MZ3LOGGER_DEBUG( L"RecentEchoParser.parse() finished." );
		return true;
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

		mixi.SetAuthor( MZ3_APP_NAME );
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
 * [content] 汎用URL 用パーサ
 * 【デバッグ用】
 */
class PlainTextParser : public MixiContentParser
{
public:
	static bool parse( CMixiData& mixi, const CHtmlArray& html_ )
	{
		MZ3LOGGER_DEBUG( L"PlainTextParser.parse() start." );

		mixi.ClearAllList();
		INT_PTR count = html_.GetCount();

		int iLine = 0;
		mixi.SetAuthor( MZ3_APP_NAME );

		CString msg;
		msg.Format( L"URL [%s] has %d line(s).\r\n\r\n", mixi.GetURL(), count );
		mixi.AddBody( msg );

		for( int iLine=0; iLine<count; iLine++ ) {
			CString target = html_.GetAt(iLine);

			target.Replace( L"\r", L"" );
			target.Replace( L"\n", L"" );

			mixi.AddBody( target );
			mixi.AddBody( L"\r\n" );
		}

		MZ3LOGGER_DEBUG( L"PlainTextParser.parse() finished." );
		return true;
	}

};

}//namespace mixi


