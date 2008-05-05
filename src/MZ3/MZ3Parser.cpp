/*
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */
#pragma once

#include "stdafx.h"
#include "MZ3.h"
#include "MZ3Parser.h"
#include "MixiParser.h"
#include "TwitterParser.h"


/// MZ3用HTMLパーサ
namespace mz3parser {

/// リスト系HTMLの解析
bool MyDoParseMixiListHtml( ACCESS_TYPE aType, CMixiDataList& body, CHtmlArray& html )
{
	// リストの初期化
	switch (aType) {
	case ACCESS_TWITTER_FRIENDS_TIMELINE:
		// 初期化しない。
		break;
	default:
		body.clear();
		break;
	}

	switch (aType) {
	case ACCESS_LIST_DIARY:					return mixi::ListNewFriendDiaryParser::parse( body, html );
	case ACCESS_LIST_NEW_COMMENT:			return mixi::NewCommentParser::parse( body, html );
	case ACCESS_LIST_COMMENT:				return mixi::ListCommentParser::parse( body, html );
	case ACCESS_LIST_NEW_BBS:				return mixi::NewBbsParser::parse( body, html );
	case ACCESS_LIST_MYDIARY:				return mixi::ListDiaryParser::parse( body, html );
//	case ACCESS_LIST_FOOTSTEP:				return mixi::ShowLogParser::parse( body, html );
	case ACCESS_LIST_FOOTSTEP:				return mixi::TrackParser::parse( body, html );
	case ACCESS_LIST_MESSAGE_IN:			return mixi::ListMessageParser::parse( body, html );
	case ACCESS_LIST_MESSAGE_OUT:			return mixi::ListMessageParser::parse( body, html );
	case ACCESS_LIST_NEWS:					return mixi::ListNewsCategoryParser::parse( body, html );
	case ACCESS_LIST_FAVORITE_USER:			return mixi::ListBookmarkParser::parse( body, html );
	case ACCESS_LIST_FAVORITE_COMMUNITY:	return mixi::ListBookmarkParser::parse( body, html );
	case ACCESS_LIST_FRIEND:				return mixi::ListFriendParser::parse( body, html );
	case ACCESS_LIST_COMMUNITY:				return mixi::ListCommunityParser::parse( body, html );
	case ACCESS_LIST_INTRO:					return mixi::ShowIntroParser::parse( body, html );
	case ACCESS_LIST_BBS:					return mixi::ListBbsParser::parse( body, html );
	case ACCESS_LIST_NEW_BBS_COMMENT:		return mixi::ListNewBbsCommentParser::parse( body, html );
	case ACCESS_LIST_CALENDAR:				return mixi::ShowCalendarParser::parse( body, html );
	case ACCESS_TWITTER_FRIENDS_TIMELINE:	return twitter::TwitterFriendsTimelineXmlParser::parse( body, html );
	case ACCESS_TWITTER_FAVORITES:			return twitter::TwitterFriendsTimelineXmlParser::parse( body, html );	// 暫定
	case ACCESS_TWITTER_DIRECT_MESSAGES:	return twitter::TwitterDirectMessagesXmlParser::parse( body, html );
	case ACCESS_RSS_READER_FEED:			return mz3parser::RssParser::parse( body, html );
	default:
		{
			CString msg;
			msg.Format( L"サポート外のアクセス種別です(%d:%s)", aType, theApp.m_accessTypeInfo.getShortText(aType) );
			MZ3LOGGER_ERROR(msg);
			MessageBox(NULL, msg, NULL, MB_OK);
		}
		return false;
	}
}

/// View系HTMLの解析
void MyDoParseMixiHtml( ACCESS_TYPE aType, CMixiData& mixi, CHtmlArray& html )
{
	switch (aType) {
	case ACCESS_DIARY:			mixi::ViewDiaryParser::parse( mixi, html );			break;
	case ACCESS_BBS:			mixi::ViewBbsParser::parse( mixi, html );			break;
	case ACCESS_ENQUETE:		mixi::ViewEnqueteParser::parse( mixi, html );		break;
	case ACCESS_EVENT:			mixi::ViewEventParser::parse( mixi, html );			break;
	case ACCESS_EVENT_MEMBER:	mixi::ListEventMemberParser::parse( mixi, html );	break;
	case ACCESS_PROFILE:		mixi::ShowFriendParser::parse( mixi, html );		break;
	case ACCESS_MYDIARY:		mixi::ViewDiaryParser::parse( mixi, html );			break;
	case ACCESS_MESSAGE:		mixi::ViewMessageParser::parse( mixi, html );		break;
	case ACCESS_NEWS:			mixi::ViewNewsParser::parse( mixi, html );			break;
	case ACCESS_HELP:			mixi::HelpParser::parse( mixi, html );				break;
	case ACCESS_ERRORLOG:		mixi::ErrorlogParser::parse( mixi, html );			break;
	case ACCESS_PLAIN:			mixi::PlainTextParser::parse( mixi, html );			break;
	default:
		{
			CString msg;
			msg.Format( L"サポート外のアクセス種別です(%d:%s)", aType, theApp.m_accessTypeInfo.getShortText(aType) );
			MZ3LOGGER_ERROR(msg);
			MessageBox(NULL, msg, NULL, MB_OK);
		}
		break;
	}
}

bool RssParser::parse( CMixiDataList& out_, const CHtmlArray& html_ )
{
	MZ3LOGGER_DEBUG( L"RssParser.parse() start." );

	// html_ の文字列化
	std::vector<TCHAR> text;
	html_.TranslateToVectorBuffer( text );

	// XML 解析
	xml2stl::Container root;
	if (!xml2stl::SimpleXmlParser::loadFromText( root, text )) {
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
		root.getNode( L"rdf:RDF" ).getNode(L"channel");
		root.getNode( L"rdf:RDF" ).getNode(L"item");

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

	switch (rss_type) {
	case RSS_TYPE_1_0:
//		parseRss1(out_, root);
		// rdf:RDF/channel に対する処理
		try {
			const xml2stl::Node& channel = root.getNode( L"rdf:RDF" ).getNode(L"channel");

//			CString s = channel.getNode(L"title").getTextAll().c_str();
//			mixi::ParserUtil::ReplaceEntityReferenceToCharacter( s );
//			out_.SetTitle( s );
//			out_.SetAuthor( s );

		} catch (xml2stl::NodeNotFoundException& e) {
			MZ3LOGGER_ERROR( util::FormatString( L"node not found... : %s", e.getMessage().c_str()) );
		}

		// rdf:RDF/item に対する処理
		try {
			const xml2stl::Node& rdf = root.getNode(L"rdf:RDF");
			
			for (int i=0; i<rdf.getChildrenCount(); i++) {
				const xml2stl::Node& item = rdf.getNode(i);
				if (item.getName() != L"item") {
					continue;
				}

				CMixiData data;
				// title
				CString s = item.getNode(L"title").getTextAll().c_str();
				mixi::ParserUtil::ReplaceEntityReferenceToCharacter( s );
				data.SetTitle( s );
				data.SetAuthor( s );

				// description
				s = item.getNode(L"description").getTextAll().c_str();
				mixi::ParserUtil::ReplaceEntityReferenceToCharacter( s );
				data.AddBody(L"\r\n");
				data.AddBody(s);

				// rdf:about
				s = item.getProperty(L"rdf:about").c_str();
				data.m_linkList.push_back(CMixiData::Link(s, data.GetTitle().Left(20)));

				data.SetAccessType(ACCESS_RSS_READER_ITEM);

				out_.push_back(data);
			}

		} catch (xml2stl::NodeNotFoundException& e) {
			MZ3LOGGER_ERROR( util::FormatString( L"node not found... : %s", e.getMessage().c_str()) );
		}
		break;

	case RSS_TYPE_2_0:
//		parseRss2(out_, root);
		// rdf:RDF/channel に対する処理
		try {
			const xml2stl::Node& rss = root.getNode( L"rss", xml2stl::Property(L"version", L"2.0") );
			const xml2stl::Node& channel = rss.getNode(L"channel");

			CString s;
//			CString s = channel.getNode(L"title").getTextAll().c_str();
//			mixi::ParserUtil::ReplaceEntityReferenceToCharacter( s );
//			out_.SetTitle( s );
//			out_.SetAuthor( s );

//			try {
//				s = channel.getNode(L"description").getTextAll().c_str();
//				mixi::ParserUtil::ReplaceEntityReferenceToCharacter( s );
//				out_.AddBody(L"\r\n");
//				out_.AddBody( s );
//			} catch (xml2stl::NodeNotFoundException& e) {
//			}

			for (int i=0; i<channel.getChildrenCount(); i++) {
				const xml2stl::Node& item = channel.getNode(i);
				if (item.getName() != L"item") {
					continue;
				}

				CMixiData data;
				// title
				CString s = item.getNode(L"title").getTextAll().c_str();
				mixi::ParserUtil::ReplaceEntityReferenceToCharacter( s );
				data.SetTitle( s );
				data.SetAuthor( s );

				// description
				s = item.getNode(L"description").getTextAll().c_str();
				mixi::ParserUtil::ReplaceEntityReferenceToCharacter( s );
				data.AddBody(L"\r\n");
				data.AddBody(s);

				// link
				s = item.getNode(L"link").getTextAll().c_str();
				data.m_linkList.push_back(CMixiData::Link(s, data.GetTitle().Left(20)));

				// pubDate
				try {
					s = item.getNode(L"pubDate").getTextAll().c_str();
					mixi::ParserUtil::ParseDate(s, data);
				} catch (xml2stl::NodeNotFoundException&) {
				}

				// dc:date
				try {
					s = item.getNode(L"dc:date").getTextAll().c_str();
					mixi::ParserUtil::ParseDate(s, data);
				} catch (xml2stl::NodeNotFoundException&) {
				}

				data.SetAccessType(ACCESS_RSS_READER_ITEM);
				out_.push_back(data);
			}

		} catch (xml2stl::NodeNotFoundException& e) {
			MZ3LOGGER_ERROR( util::FormatString( L"node not found... : %s", e.getMessage().c_str()) );
		}

		break;

	default:
		MZ3LOGGER_ERROR(L"未サポートのRSSです");
		break;
	}

	MZ3LOGGER_DEBUG( L"RssParser.parse() finished." );
	return true;
}

}//namespace mixi
