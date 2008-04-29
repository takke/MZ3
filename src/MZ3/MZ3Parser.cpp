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
	case ACCESS_LIST_FAVORITE:				return mixi::ListBookmarkParser::parse( body, html );
	case ACCESS_LIST_FRIEND:				return mixi::ListFriendParser::parse( body, html );
	case ACCESS_LIST_COMMUNITY:				return mixi::ListCommunityParser::parse( body, html );
	case ACCESS_LIST_INTRO:					return mixi::ShowIntroParser::parse( body, html );
	case ACCESS_LIST_BBS:					return mixi::ListBbsParser::parse( body, html );
	case ACCESS_LIST_NEW_BBS_COMMENT:		return mixi::ListNewBbsCommentParser::parse( body, html );
	case ACCESS_LIST_CALENDAR:				return mixi::ShowCalendarParser::parse( body, html );
	case ACCESS_TWITTER_FRIENDS_TIMELINE:	return twitter::TwitterFriendsTimelineXmlParser::parse( body, html );
	case ACCESS_TWITTER_FAVORITES:			return twitter::TwitterFriendsTimelineXmlParser::parse( body, html );	// 暫定
	case ACCESS_TWITTER_DIRECT_MESSAGES:	return twitter::TwitterDirectMessagesXmlParser::parse( body, html );
	default:
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
	}
}

}//namespace mixi
