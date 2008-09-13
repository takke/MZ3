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


/// MZ3�pHTML�p�[�T
namespace mz3parser {

/// ���X�g�nHTML�̉��
bool MyDoParseMixiListHtml( ACCESS_TYPE aType, CMixiData& parent, CMixiDataList& body, CHtmlArray& html )
{
	// ���X�g�̏�����
	switch (aType) {
	case ACCESS_TWITTER_FRIENDS_TIMELINE:
	case ACCESS_WASSR_FRIENDS_TIMELINE:
		// ���������Ȃ��B
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
	case ACCESS_MIXI_RECENT_ECHO:			return mixi::RecentEchoParser::parse( body, html );
	case ACCESS_TWITTER_FRIENDS_TIMELINE:	return twitter::TwitterFriendsTimelineXmlParser::parse( parent, body, html );
	case ACCESS_TWITTER_FAVORITES:			return twitter::TwitterFriendsTimelineXmlParser::parse( parent, body, html );	// �b��
	case ACCESS_TWITTER_DIRECT_MESSAGES:	return twitter::TwitterDirectMessagesXmlParser::parse( body, html );
	case ACCESS_WASSR_FRIENDS_TIMELINE:		return twitter::WassrFriendsTimelineXmlParser::parse( body, html );
	case ACCESS_RSS_READER_FEED:			return mz3parser::RssFeedParser::parse( body, html );
	default:
		{
			CString msg;
			msg.Format( L"�T�|�[�g�O�̃A�N�Z�X��ʂł�(%d:%s)", aType, theApp.m_accessTypeInfo.getShortText(aType) );
			MZ3LOGGER_ERROR(msg);
			MessageBox(NULL, msg, NULL, MB_OK);
		}
		return false;
	}
}

/// View�nHTML�̉��
void MyDoParseMixiHtml( ACCESS_TYPE aType, CMixiData& mixi, CHtmlArray& html )
{
	switch (aType) {
	case ACCESS_DIARY:			mixi::ViewDiaryParser::parse( mixi, html );			break;
	case ACCESS_BBS:			mixi::ViewBbsParser::parse( mixi, html );			break;
	case ACCESS_ENQUETE:		mixi::ViewEnqueteParser::parse( mixi, html );		break;
	case ACCESS_EVENT_JOIN:
	case ACCESS_EVENT:			mixi::ViewEventParser::parse( mixi, html );			break;
	case ACCESS_EVENT_MEMBER:	mixi::ListEventMemberParser::parse( mixi, html );	break;
	case ACCESS_BIRTHDAY:
	case ACCESS_PROFILE:		mixi::ShowFriendParser::parse( mixi, html );		break;
	case ACCESS_MYDIARY:		mixi::ViewDiaryParser::parse( mixi, html );			break;
	case ACCESS_MESSAGE:		mixi::ViewMessageParser::parse( mixi, html );		break;
	case ACCESS_NEWS:			mixi::ViewNewsParser::parse( mixi, html );			break;
	case ACCESS_HELP:			mixi::HelpParser::parse( mixi, html );				break;
	case ACCESS_ERRORLOG:		mixi::ErrorlogParser::parse( mixi, html );			break;
	case ACCESS_SCHEDULE:
	case ACCESS_PLAIN:			mixi::PlainTextParser::parse( mixi, html );			break;
	default:
		{
			CString msg;
			msg.Format( L"�T�|�[�g�O�̃A�N�Z�X��ʂł�(%d:%s)", aType, theApp.m_accessTypeInfo.getShortText(aType) );
			MZ3LOGGER_ERROR(msg);
			MessageBox(NULL, msg, NULL, MB_OK);
		}
		break;
	}
}

bool RssFeedParser::parse( CMixiDataList& out_, const std::vector<TCHAR>& text_, CString* pStrTitle )
{
	MZ3LOGGER_DEBUG( L"RssFeedParser.parse() start." );

	// XML ���
	xml2stl::Container root;
	if (!xml2stl::SimpleXmlParser::loadFromText( root, text_ )) {
		MZ3LOGGER_ERROR( L"XML ��͎��s" );
		return false;
	}

	// RSS �o�[�W��������
	enum RSS_TYPE {
		RSS_TYPE_INVALID,
		RSS_TYPE_1_0,
		RSS_TYPE_2_0,
	};
	RSS_TYPE rss_type = RSS_TYPE_INVALID;

	// RSS 1.0 ����
	try {
		const xml2stl::Node& rdf = root.getNode( L"rdf:RDF" );
		rdf.getNode(L"channel");
		rdf.getNode(L"item");

		// OK.
		rss_type = RSS_TYPE_1_0;
	} catch (xml2stl::NodeNotFoundException&) {
	}

	// RSS 2.0 ����
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
		// rdf:RDF/channel �ɑ΂��鏈��

		// rdf:RDF/item �ɑ΂��鏈��
		try {
			const xml2stl::Node& rdf = root.getNode(L"rdf:RDF");
			
			for (xml2stl::NodeRef nodeRef=rdf.getChildrenNodeRef(); !nodeRef.isEnd(); nodeRef.next()) {
				const xml2stl::Node& item = nodeRef.getCurrentNode();
				if (item.getName() != L"item") {
					continue;
				}

				CMixiData data;

				// description, title �̎擾
				CString title = item.getNode(L"title").getTextAll().c_str();
				CString description = item.getNode(L"description").getTextAll().c_str();

				// ���`���Ċi�[����
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

			// �^�C�g���擾
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
		// rdf:RDF/channel �ɑ΂��鏈��
		try {
			const xml2stl::Node& rss = root.getNode( L"rss", xml2stl::Property(L"version", L"2.0") );
			const xml2stl::Node& channel = rss.getNode(L"channel");

			for (xml2stl::NodeRef nodeRef=channel.getChildrenNodeRef(); !nodeRef.isEnd(); nodeRef.next()) {
				const xml2stl::Node& item = nodeRef.getCurrentNode();
				if (item.getName() != L"item") {
					continue;
				}

				CMixiData data;

				// description, title �̎擾
				CString description = item.getNode(L"description").getTextAll().c_str();
				CString title = item.getNode(L"title").getTextAll().c_str();

				// ���`���Ċi�[����
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

			// �^�C�g���擾
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
		MZ3LOGGER_ERROR(L"���T�|�[�g��RSS�ł�");
		rval = false;
		break;
	}

	MZ3LOGGER_DEBUG( L"RssFeedParser.parse() finished." );
	return rval;
}

void RssFeedParser::setDescriptionTitle( CMixiData& data, CString description, CString title )
{
	// description �̐��`
	mixi::ParserUtil::ReplaceEntityReferenceToCharacter( description );
	description = description.Trim();

	// description �� img �^�O����͂��A�o�^����
/*	static MyRegex s_reg;
	if (util::CompileRegex(s_reg, L"<img[^>]*src=\"([^\"]+)\"[^>]*>")) {
		if (s_reg.exec(description) && s_reg.results.size() == 2 ) {
			// <img>�^�O
			MZ3_TRACE(L"RssFeedParser::setDescriptionTitle(), img-url[%s], description[%s]\n", 
				s_reg.results[1].str.c_str(), 
				description);
			data.AddImage(s_reg.results[1].str.c_str());
		}
	}
*/	
	// body �Ƃ��� description �� no-tag �ł�ݒ肷��
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

	// XML ���
	xml2stl::Container root;
	if (!xml2stl::SimpleXmlParser::loadFromText( root, text_ )) {
		MZ3LOGGER_ERROR( L"XML ��͎��s" );
		return false;
	}

	bool rval = true;

	// html/head/link[rel=alternate,type=application/rss+xml] ���擾����
	// XHTML �Ƃ͌���Ȃ��̂ŁA�S�Ă� link �^�O��ΏۂƂ���
	try {
		// html/... �� HTML/... �Ŏ��s����
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
		const xml2stl::Node& item = node.getNode(i);

		try {
			if (item.getName() == L"link" &&
				item.getProperty(L"rel")==L"alternate" &&
				item.getProperty(L"type")==L"application/rss+xml")
			{
				// �ǉ�����
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


}//namespace mixi
