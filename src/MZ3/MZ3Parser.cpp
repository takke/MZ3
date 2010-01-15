/*
 * Copyright (c) MZ3 Project Team All rights reserved.
 * Code licensed under the BSD License:
 * http://www.mz3.jp/license.txt
 */
#pragma once

#include "stdafx.h"
#include "MZ3.h"
#include "MZ3Parser.h"
#include "MixiParser.h"
#include "TwitterParser.h"

static void my_lua_printstack( lua_State *L )
{
	int top = lua_gettop(L);
	for (int i=1; i<=top; i++) {
		switch (lua_type(L,i)) {
		case LUA_TSTRING:
			printf("%s:%s ", lua_typename(L, lua_type(L,i)), (const char*)lua_tostring(L,i));
			break;

		case LUA_TNUMBER:
		default:
			printf("%s:%d ", lua_typename(L, lua_type(L,i)), (int)lua_tonumber(L,i));
			break;
		}
	}
	printf("*\n");
}

/// MZ3用HTMLパーサ
namespace parser {

/**
 * MZ3 Script : パーサ呼び出し
 */
static bool CallMZ3ScriptParser(const char* szServiceType, const char* szParserFuncName, 
								CMixiData* parent, CMixiDataList* body, CHtmlArray& html )
{
	// 引数として parent, body, html を渡す

	// スタックのサイズを覚えておく
	lua_State* L = theApp.m_luaState;
	int top = lua_gettop(L);

	// Lua関数名("mixi.bbs_parser")を積む
	lua_getglobal(L, szServiceType);		// szServiceType テーブルをスタックに積む
	//my_lua_printstack(L);
	lua_pushstring(L, szParserFuncName);	// 対象変数(関数名)をテーブルに積む
	//my_lua_printstack(L);
	lua_gettable(L, -2);					// スタックの2番目の要素("mixi"テーブル)から、
											// テーブルトップの文字列("bbs_parser")で示されるメンバを
											// スタックに積む
	//my_lua_printstack(L);

	// 引数を積む
	lua_pushlightuserdata(L, parent);
	lua_pushlightuserdata(L, body);
	lua_pushlightuserdata(L, &html);

	// 関数実行
	int n_arg = 3;
	int n_ret = 1;
	int status = lua_pcall(L, n_arg, n_ret, 0);

	int result = 0;
	if (status != 0) {
		// TODO エラー処理
		theApp.MyLuaErrorReport(status);
		return false;
	} else {
		// 返り値取得
		result = lua_toboolean(L, -1);
	}
	lua_settop(L, top);
	return result != 0;
}

/// リスト系HTMLの解析
bool MyDoParseMixiListHtml( ACCESS_TYPE aType, CMixiData& parent, CMixiDataList& body, CHtmlArray& html )
{
	// MZ3 Script 用パーサがあれば利用する
	const char* szSerializeKey = theApp.m_accessTypeInfo.getSerializeKey(aType);
	bool bRunLuaParser = false;
	if (theApp.m_luaParsers.count(szSerializeKey)!=0) {
		CStringA strParserName = theApp.m_luaParsers[szSerializeKey].c_str();

		// パーサ名をテーブルと関数名に分離する
		int idx = strParserName.Find('.');
		if (idx>0) {
			CStringA strTable = strParserName.Left(idx);
			CStringA strFuncName = strParserName.Mid(idx+1);
//			printf("parser : [%s], [%s]\n", strTable, strFuncName);
			CallMZ3ScriptParser(strTable, strFuncName, &parent, &body, html);

			// ProMode 用の処理後に終了する
			bRunLuaParser = true;
		}
	}

	if (theApp.m_bProMode) {
		switch (aType) {
		case ACCESS_TWITTER_FRIENDS_TIMELINE:
		case ACCESS_TWITTER_FAVORITES:
			return parser::TwitterFriendsTimelineXmlParser::parse( parent, body, html );
		}
	}

	if (bRunLuaParser) {
		return true;
	}

#ifdef BT_MZ3
	switch (aType) {
//	case ACCESS_MAIN:							body.clear();	return mixi::HomeParser::parse( parent, html );
	case ACCESS_LIST_NEW_COMMENT:				body.clear();	return mixi::NewCommentParser::parse( body, html );
	case ACCESS_LIST_COMMENT:					body.clear();	return mixi::ListCommentParser::parse( body, html );
	case ACCESS_LIST_MYDIARY:					body.clear();	return mixi::ListDiaryParser::parse( body, html );
//	case ACCESS_LIST_FOOTSTEP:					body.clear();	return mixi::ShowLogParser::parse( body, html );
	case ACCESS_LIST_FOOTSTEP:					body.clear();	return mixi::TrackParser::parse( body, html );
	case ACCESS_LIST_NEWS:						body.clear();	return mixi::ListNewsCategoryParser::parse( body, html );
//	case ACCESS_LIST_FAVORITE_USER:				body.clear();	return mixi::ListBookmarkParser::parse( body, html );
//	case ACCESS_LIST_FAVORITE_COMMUNITY:		body.clear();	return mixi::ListBookmarkParser::parse( body, html );
	case ACCESS_LIST_FRIEND:					body.clear();	return mixi::ListFriendParser::parse( body, html );
	case ACCESS_LIST_COMMUNITY:					body.clear();	return mixi::ListCommunityParser::parse( body, html );
	case ACCESS_LIST_INTRO:						body.clear();	return mixi::ShowIntroParser::parse( body, html );
	case ACCESS_LIST_BBS:						body.clear();	return mixi::ListBbsParser::parse( body, html );
	case ACCESS_LIST_CALENDAR:					body.clear();	return mixi::ShowCalendarParser::parse( body, html );
	case ACCESS_RSS_READER_FEED:				body.clear();	return parser::RssFeedParser::parse( body, html );
	default:
		break;
	}
#endif

	CString msg;
	msg.Format( L"サポート外のアクセス種別です(%d:%s)", aType, theApp.m_accessTypeInfo.getShortText(aType) );
	MZ3LOGGER_ERROR(msg);
	MessageBox(NULL, msg, NULL, MB_OK);
	return false;
}

/// View系HTMLの解析
bool MyDoParseMixiHtml( ACCESS_TYPE aType, CMixiData& mixi, CHtmlArray& html )
{
	// MZ3 Script 用パーサがあれば利用する
	const char* szSerializeKey = theApp.m_accessTypeInfo.getSerializeKey(aType);
	if (theApp.m_luaParsers.count(szSerializeKey)!=0) {
		CStringA strParserName = theApp.m_luaParsers[szSerializeKey].c_str();

		// パーサ名をテーブルと関数名に分離する
		int idx = strParserName.Find('.');
		if (idx>0) {
			CStringA strTable = strParserName.Left(idx);
			CStringA strFuncName = strParserName.Mid(idx+1);
//			printf("parser : [%s], [%s]\n", strTable, strFuncName);
			return CallMZ3ScriptParser(strTable, strFuncName, &mixi, NULL, html);
		}
	}

	switch (aType) {
#ifdef BT_MZ3
//	case ACCESS_DIARY:
//	case ACCESS_NEIGHBORDIARY:
//	case ACCESS_MYDIARY:		return mixi::ViewDiaryParser::parse( mixi, html );
	case ACCESS_BBS:			return mixi::ViewBbsParser::parse( mixi, html );
	case ACCESS_ENQUETE:		return mixi::ViewEnqueteParser::parse( mixi, html );
	case ACCESS_EVENT_JOIN:
	case ACCESS_EVENT:			return mixi::ViewEventParser::parse( mixi, html );
	case ACCESS_EVENT_MEMBER:	return mixi::ListEventMemberParser::parse( mixi, html );
	case ACCESS_BIRTHDAY:
	case ACCESS_NEWS:			return mixi::ViewNewsParser::parse( mixi, html );
#endif
	case ACCESS_HELP:			return parser::HelpParser::parse( mixi, html );
	case ACCESS_ERRORLOG:		return parser::ErrorlogParser::parse( mixi, html );
#ifdef BT_MZ3
	case ACCESS_SCHEDULE:
#endif
	case ACCESS_PLAIN:			return parser::PlainTextParser::parse( mixi, html );
	default:
		break;
	}

	CString msg;
	msg.Format( L"サポート外のアクセス種別です(%d:%s)", aType, theApp.m_accessTypeInfo.getShortText(aType) );
	MZ3LOGGER_ERROR(msg);
	MessageBox(NULL, msg, NULL, MB_OK);
	return false;
}

//
// Twitter 用パーサ
//

inline bool MyGetMatchString(const CString& target, const CString& left, const CString& right, CString& buf)
{
	if (util::GetBetweenSubString(target, left, right, buf)<0) {
		return false;
	} else {
		return true;
	}
}

inline void MyExtractLinks(const CString& s, MZ3Data& data)
{
	if (s.Find( L"ttp://" ) != -1) {
		// リンク抽出
		static MyRegex reg4;
		if( !util::CompileRegex( reg4, L"(https?://[-_.!~*'()a-zA-Z0-9;/?:@&=+$,%#]+)" ) ) {
			MZ3LOGGER_FATAL( FAILED_TO_COMPILE_REGEX_MSG );
			return;
		}

		CString target = s;
		for( int i=0; i<MZ3_INFINITE_LOOP_MAX_COUNT; i++ ) {	// MZ3_INFINITE_LOOP_MAX_COUNT は無限ループ防止

			int offsetend = -1;
			CString url;

			// URL
			if (target.Find( L"ttp://" ) != -1) {
				if( reg4.exec(target) && reg4.results.size() == 2 ) {
					offsetend = reg4.results[0].end;
					url += reg4.results[1].str.c_str();
				}
			}
			if (offsetend < 0) {
				// 未発見。
				// 残りの文字列を代入して終了。
				break;
			}

			data.m_linkList.push_back( CMixiData::Link(url, url) );

			// ターゲットを更新。
			target = target.Mid( offsetend );
		}
	}
}

bool TwitterFriendsTimelineXmlParser::parse( CMixiData& parent, CMixiDataList& body_, const CHtmlArray& html_ )
{
	MZ3LOGGER_DEBUG( L"TwitterFriendsTimelineXmlParser.parse() start." );

	INT_PTR count = html_.GetCount();

	// 全消去しない

	// 重複防止用の id 一覧を生成
	stdext::hash_set<INT64> id_set;
	int n = body_.size();
	for (int i=0; i<n; i++) {
		INT64 id = body_[i].GetID();
		id_set.insert(id);

		// 全ての new フラグを解除
		body_[i].SetIntValue(L"new_flag", 0);
	}

	util::StopWatch sw;
	sw.start();

	// 一時リスト
	CMixiDataList new_list;

	for (int i=0; i<count; i++) {
		const CString& line = html_.GetAt(i);
		if (util::LineHasStringsNoCase(line, L"<status>")) {
			// </status> まで取得する
			// 但し、同一IDがあればskipする
			i++;

			// data 生成
			MZ3Data data;
			data.SetAccessType(ACCESS_TWITTER_USER);
			data.SetIntValue(L"new_flag", 1);

			int i_in_status = 0;
			bool bInUser = false;	// /status      : false
									// /status/user : true
			CString user_tag;

			while (i<count) {
				const CString& line = html_.GetAt(i);

				CString tagName;
				MyGetMatchString(line, L"<", L">", tagName);
				CString s;
				
				if (i_in_status<3 && tagName == L"id") {
					if (util::GetBetweenSubString(line, L">", L"<", s) < 0) {
						continue;
					}
					INT64 id = _wtoi64(s);
					if (id_set.count(id) > 0) {
						MZ3LOGGER_DEBUG(util::FormatString(L"id[%d]は既に存在するのでskipする", id));
						i += 30;
						while (i<count) {
							const CString& line = html_.GetAt(i);
							if (util::LineHasStringsNoCase(line, L"</status>")) {
								break;
							}
							i++;
						}
						break;				
					}

					data.SetInt64Value(L"id", id);
				}

				if (!bInUser) {
					// /status
					if (i_in_status > 7 && tagName==L"user") {
						bInUser = true;

					} else if (tagName == L"created_at" && MyGetMatchString(line, L">", L"<", s)) {
						// updated : status/created_at
						mixi::ParserUtil::ParseDate(s, data);

					} else if (tagName == L"text") {
						// 複数行対応
						if (!MyGetMatchString(line, L">", L"<", s)) {
							util::GetAfterSubString(line, L">", s);
							i++;

							while(i<count) {
								const CString& line = html_.GetAt(i);
								int idx = line.Find(L"<");
								if (idx >= 0) {
									s += line.Left(idx);
									break;
								} else {
									s += line;
								}
								i++;
							}
						}

						// text : status/text
						s.Replace(L"&amp;", L"&");
						mixi::ParserUtil::ReplaceEntityReferenceToCharacter(s);
						data.AddBody(s);
						
						// URL を抽出し、リンクにする
						MyExtractLinks(s, data);

					} else if (tagName == L"source" && MyGetMatchString(line, L">", L"<", s)) {
						// source : status/source
						mixi::ParserUtil::ReplaceEntityReferenceToCharacter(s);
						data.SetTextValue(L"source", s);

					} else if (tagName == L"in_reply_to_status_id" && MyGetMatchString(line, L">", L"<", s)) {
						// in_reply_to_status_id : status/in_reply_to_status_id
						data.SetIntValue(L"in_reply_to_status_id", _wtol(s));
					} else if (tagName == L"retweeted_status") {
						// </retweeted_status> まで読み飛ばす(RT対応)
						i++;
						while (i<count) {
							const CString& line = html_.GetAt(i);
							if (line.Find(L"</retweeted_status>") != -1) {
								break;
							}
							i++;
						}
					}

				} else {
					// /status/user

					user_tag += line;

					if (tagName == L"screen_name" && MyGetMatchString(line, L">", L"<", s)) {
						// name : status/user/screen_name
						s.Replace(L"&amp;", L"&");
						mixi::ParserUtil::ReplaceEntityReferenceToCharacter(s);
						data.SetTextValue(L"name", s);
					
					} else if (tagName == L"name" && MyGetMatchString(line, L">", L"<", s)) {
						// author : status/user/name
						s.Replace(L"&amp;", L"&");
						mixi::ParserUtil::ReplaceEntityReferenceToCharacter(s);
						data.SetTextValue(L"author", s);

					} else if (tagName == L"description" && MyGetMatchString(line, L">", L"<", s)) {
						// description : status/user/description

						// title に入れるのは苦肉の策・・・
						s.Replace(L"&amp;", L"&");
						mixi::ParserUtil::ReplaceEntityReferenceToCharacter(s);
						data.SetTextValue(L"title", s);

					} else if (tagName == L"id" && MyGetMatchString(line, L">", L"<", s)) {
						// owner-id : status/user/id
						data.SetIntValue(L"owner_id", _wtol(s));
						
					} else if (tagName == L"url" && MyGetMatchString(line, L">", L"<", s)) {
						// URL : status/user/url
						data.SetTextValue(L"url", s);
						data.SetTextValue(L"browse_uri", s);

					} else if (tagName == L"profile_image_url" && MyGetMatchString(line, L">", L"<", s)) {
						// Image : status/user/profile_image_url
						s.Replace(L"&amp;", L"&");
						mixi::ParserUtil::ReplaceEntityReferenceToCharacter(s);
						data.AddImage(s);
					}
				}

				if (i_in_status>35 && line.Find(L"</status>") >= 0) {
					// 一時リストに追加
					data.SetTextValue(L"user_tag", user_tag);
					new_list.push_back(data);
					break;
				}

				i_in_status++;
				i++;
			}
		}
	}

	// 生成したデータを出力に反映
#ifdef WINCE
	TwitterParserBase::MergeNewList(body_, new_list, 100);
#else
	TwitterParserBase::MergeNewList(body_, new_list, 1000);
#endif

	// 新着件数を parent(カテゴリの m_mixi) に設定する
	parent.SetIntValue(L"new_count", new_list.size());
	
	sw.stop();

	MZ3LOGGER_DEBUG(
		util::FormatString(L"TwitterFriendsTimelineXmlParser.parse() end; elapsed : %d [msec]",
			sw.getElapsedMilliSecUntilStoped()));
	return true;
}

#ifdef BT_MZ3
//
// RSS 用パーサ
//

bool RssFeedParser::parse( CMixiDataList& out_, const std::vector<TCHAR>& text_, CString* pStrTitle )
{
	MZ3LOGGER_DEBUG( L"RssFeedParser.parse() start." );

	// XML 解析
	xml2stl::Container root;
	if (!xml2stl::SimpleXmlParser::loadFromText( root, text_, false )) {
		MZ3LOGGER_ERROR( L"XML 解析失敗" );
		return false;
	}

	// RSS バージョン判別
	enum RSS_TYPE {
		RSS_TYPE_INVALID,
		RSS_TYPE_1_0,
		RSS_TYPE_2_0,
	};
	RSS_TYPE rss_type = RSS_TYPE_INVALID;

	// RSS 1.0 判定
	try {
		const xml2stl::Node& rdf = root.getNode( L"rdf:RDF" );
		rdf.getNode(L"channel");
		rdf.getNode(L"item");

		// OK.
		rss_type = RSS_TYPE_1_0;
	} catch (xml2stl::NodeNotFoundException&) {
	}

	// RSS 2.0 判定
	try {
		const xml2stl::Node& rss = root.getNode( L"rss", xml2stl::Property(L"version", L"2.0") );
		const xml2stl::Node& channel = rss.getNode(L"channel");
		channel.getNode(L"item");

		// OK.
		rss_type = RSS_TYPE_2_0;
	} catch (xml2stl::NodeNotFoundException&) {
	}

	bool rval = true;
	switch (rss_type) {
	case RSS_TYPE_1_0:
//		parseRss1(out_, root);
		// rdf:RDF/channel に対する処理

		// rdf:RDF/item に対する処理
		try {
			const xml2stl::Node& rdf = root.getNode(L"rdf:RDF");
			
			for (xml2stl::NodeRef nodeRef=rdf.getChildrenNodeRef(); !nodeRef.isEnd(); nodeRef.next()) {
				const xml2stl::Node& item = nodeRef.getCurrentNode();
				if (item.getName() != L"item") {
					continue;
				}

				CMixiData data;

				// description, title の取得
				CString title = item.getNode(L"title").getTextAll().c_str();
				CString description = item.getNode(L"description").getTextAll().c_str();

				// 整形して格納する
				RssFeedParser::setDescriptionTitle(data, description, title);

				// link = rdf:about
				CString s = item.getProperty(L"rdf:about").c_str();
				data.m_linkList.push_back(CMixiData::Link(s, data.GetTitle().Left(20)));

				// dc:date
				try {
					CString s = item.getNode(L"dc:date").getTextAll().c_str();
					mixi::ParserUtil::ParseDate(s, data);
				} catch (xml2stl::NodeNotFoundException&) {
				}

				data.SetAccessType(ACCESS_RSS_READER_ITEM);
				out_.push_back(data);
			}

			// タイトル取得
			if (pStrTitle!=NULL) {
				const xml2stl::Node& channel = rdf.getNode(L"channel");
				CString title = channel.getNode(L"title").getTextAll().c_str();
				(*pStrTitle) = title;
			}

		} catch (xml2stl::NodeNotFoundException& e) {
			MZ3LOGGER_ERROR( util::FormatString( L"node not found... : %s", e.getMessage().c_str()) );
			rval = false;
		}
		break;

	case RSS_TYPE_2_0:
//		parseRss2(out_, root);
		// rdf:RDF/channel に対する処理
		try {
			const xml2stl::Node& rss = root.getNode( L"rss", xml2stl::Property(L"version", L"2.0") );
			const xml2stl::Node& channel = rss.getNode(L"channel");

			for (xml2stl::NodeRef nodeRef=channel.getChildrenNodeRef(); !nodeRef.isEnd(); nodeRef.next()) {
				const xml2stl::Node& item = nodeRef.getCurrentNode();
				if (item.getName() != L"item") {
					continue;
				}

				CMixiData data;

				// description, title の取得
				CString description = item.getNode(L"description").getTextAll().c_str();
				CString title = item.getNode(L"title").getTextAll().c_str();

				// 整形して格納する
				RssFeedParser::setDescriptionTitle(data, description, title);

				// link
				CString link = item.getNode(L"link").getTextAll().c_str();
				data.m_linkList.push_back(CMixiData::Link(link, data.GetTitle().Left(20)));

				// pubDate
				try {
					CString s = item.getNode(L"pubDate").getTextAll().c_str();
					mixi::ParserUtil::ParseDate(s, data);
				} catch (xml2stl::NodeNotFoundException&) {
				}

				// dc:date
				try {
					CString s = item.getNode(L"dc:date").getTextAll().c_str();
					mixi::ParserUtil::ParseDate(s, data);
				} catch (xml2stl::NodeNotFoundException&) {
				}

				data.SetAccessType(ACCESS_RSS_READER_ITEM);
				out_.push_back(data);
			}

			// タイトル取得
			if (pStrTitle!=NULL) {
				CString title = channel.getNode(L"title").getTextAll().c_str();
				(*pStrTitle) = title;
			}

		} catch (xml2stl::NodeNotFoundException& e) {
			MZ3LOGGER_ERROR( util::FormatString( L"node not found... : %s", e.getMessage().c_str()) );
			rval = false;
		}

		break;

	default:
		MZ3LOGGER_ERROR(L"未サポートのRSSです");
		rval = false;
		break;
	}

	MZ3LOGGER_DEBUG( L"RssFeedParser.parse() finished." );
	return rval;
}

void RssFeedParser::setDescriptionTitle( CMixiData& data, CString description, CString title )
{
	// description の整形
	mixi::ParserUtil::ReplaceEntityReferenceToCharacter( description );
	description = description.Trim();

	// description の img タグを解析し、登録する
/*	static MyRegex s_reg;
	if (util::CompileRegex(s_reg, L"<img[^>]*src=\"([^\"]+)\"[^>]*>")) {
		if (s_reg.exec(description) && s_reg.results.size() == 2 ) {
			// <img>タグ
			MZ3_TRACE(L"RssFeedParser::setDescriptionTitle(), img-url[%s], description[%s]\n", 
				s_reg.results[1].str.c_str(), 
				description);
			data.AddImage(s_reg.results[1].str.c_str());
		}
	}
*/	
	// body として description の no-tag 版を設定する
	mixi::ParserUtil::StripAllTags(description);
	description = description.Trim();
	data.AddBody(description);

	// title = title
	mixi::ParserUtil::ReplaceEntityReferenceToCharacter( title );
	if (!title.IsEmpty()) {
		title += L"\r\n";
	}
//	title += description;
	title = title.Trim();
	data.SetTitle( title );
}

bool RssAutoDiscoveryParser::parse( CMixiDataList& out_, const std::vector<TCHAR>& text_ )
{
	MZ3LOGGER_DEBUG( L"RssAutoDiscoveryParser.parse() start." );

	// XML 解析
	xml2stl::Container root;
	if (!xml2stl::SimpleXmlParser::loadFromText( root, text_ )) {
		MZ3LOGGER_ERROR( L"XML 解析失敗" );
		return false;
	}

	bool rval = true;

	// html/head/link[rel=alternate,type=application/rss+xml] を取得する
	// XHTML とは限らないので、全ての link タグを対象とする
	try {
		// html/... と HTML/... で試行する
		try {
			parseLinkRecursive( out_, root.getNode(L"HTML") );
		} catch (xml2stl::NodeNotFoundException&) {
			parseLinkRecursive( out_, root.getNode(L"html") );
		}
	} catch (xml2stl::NodeNotFoundException& e) {
		MZ3LOGGER_ERROR( util::FormatString( L"node not found... : %s", e.getMessage().c_str()) );
		rval = false;
	}

	MZ3LOGGER_DEBUG( L"RssAutoDiscoveryParser.parse() finished." );
	return rval;
}

bool RssAutoDiscoveryParser::parseLinkRecursive( CMixiDataList& out_, const xml2stl::Node& node )
{
	for (unsigned int i=0; i<node.getChildrenCount(); i++) {
		const xml2stl::Node& item = node.getNodeByIndex(i);

		try {
			if (item.getName() == L"link" &&
				item.getProperty(L"rel")==L"alternate" &&
				item.getProperty(L"type")==L"application/rss+xml")
			{
				// 追加する
				CMixiData data;

				CString title = item.getProperty(L"title").c_str();
				CString href = item.getProperty(L"href").c_str();

				mixi::ParserUtil::ReplaceEntityReferenceToCharacter( title );

				data.SetTitle(title);
				data.SetURL(href);

				data.SetAccessType(ACCESS_RSS_READER_FEED);
				out_.push_back(data);
				continue;
			}

			if (item.getChildrenCount()>0) {
				parseLinkRecursive(out_, item);
			}
		} catch (xml2stl::NodeNotFoundException&) {
			continue;
		}
	}

	return true;
}
#endif	// BT_MZ3


bool MZ3ParserBase::ExtractLinks(CMixiData &data_)
{
	// 正規表現のコンパイル（一回のみ）
	static MyRegex reg;
	if( !util::CompileRegex( reg, L"(h?ttps?://[-_.!~*'()a-zA-Z0-9;/?:@&=+$,%#]+)" ) ) {
		return false;
	}

	CString target = data_.GetBody();

	for( int i=0; i<MZ3_INFINITE_LOOP_MAX_COUNT; i++ ) {	// MZ3_INFINITE_LOOP_MAX_COUNT は無限ループ防止
		if( reg.exec(target) == false || reg.results.size() != 2 ) {
			// 未発見。終了。
			break;
		}

		// 発見。

		// URL
		std::wstring& url = reg.results[1].str;

		if (!url.empty() && url[0] != 'h') {
			url.insert( url.begin(), 'h' );
		}

		// データに追加
		data_.m_linkList.push_back( CMixiData::Link(url.c_str(), url.c_str()) );

		// ターゲットを更新。
		target = target.Mid( reg.results[0].end );
	}

	return true;
}

//
// help 用パーサ
//

bool HelpParser::parse( CMixiData& mixi, const CHtmlArray& html_ )
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

//
// エラーログ 用パーサ
//

bool ErrorlogParser::parse( CMixiData& mixi, const CHtmlArray& html_ )
{
	MZ3LOGGER_DEBUG( L"ErrorlogParser.parse() start." );

	mixi.ClearAllList();
	INT_PTR count = html_.GetCount();

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

	// 末尾のN行のみ対象とする
	int iLine = 0;
	const int LINE_LIMIT = 200;
	if (count > LINE_LIMIT) {
		iLine = count-LINE_LIMIT;
	}
	for(; iLine<count; iLine++ ) {
		CString target = html_.GetAt(iLine);
		target.Replace(_T("\n"), _T("\r\n"));	// 改行コード変換

		child.ClearBody();
		child.SetCommentIndex( iLine+1 );

		// "[～]" を日付に。
		CString date;
		int index = 0;
		index = util::GetBetweenSubString( target, L"[", L"]", date );
		if( index == -1 ) {
			child.AddBody( L"★ '[～]' が見つからないのでスキップ [" + target + L"]" );
			mixi.AddChild( child );
			continue;
		}
		child.SetDate( date );

		// "]" 以降を切り出し
		target = target.Mid( index );

		// "△～△" をエラーレベル（名前）に。
		CString level;
		index = util::GetBetweenSubString( target, L" ", L" ", level );
		if( index == -1 ) {
			// "△～△" が見つからないのでスキップ
			child.AddBody( L"★ '△～△' が見つからないのでスキップ [" + target + L"]" );
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

//
// テキスト用パーサ
//

bool PlainTextParser::parse( CMixiData& mixi, const CHtmlArray& html_ )
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

}//namespace parser
