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

/// MZ3�pHTML�p�[�T
namespace parser {

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
	case ACCESS_LIST_DIARY:						return mixi::ListNewFriendDiaryParser::parse( body, html );
	case ACCESS_LIST_NEW_COMMENT:				return mixi::NewCommentParser::parse( body, html );
	case ACCESS_LIST_COMMENT:					return mixi::ListCommentParser::parse( body, html );
	case ACCESS_LIST_NEW_BBS:
		{
			// �����Ƃ��� parent, body, html ��n��

			// �X�^�b�N�̃T�C�Y���o���Ă���
			lua_State* L = theApp.m_luaState;
			int top = lua_gettop(L);

			// Lua�֐���("mixi.bbs_parser")��ς�
			lua_getglobal(L, "mixi");				// "mixi" �e�[�u�����X�^�b�N�ɐς�
			//my_lua_printstack(L);
			lua_pushstring(L, "bbs_parser");		// �Ώەϐ�(�֐���)���e�[�u���ɐς�
			//my_lua_printstack(L);
			lua_gettable(L, -2);					// �X�^�b�N��2�Ԗڂ̗v�f("mixi"�e�[�u��)����A
													// �e�[�u���g�b�v�̕�����("bbs_parser")�Ŏ�����郁���o��
													// �X�^�b�N�ɐς�
//			lua_getglobal(L, "mixi.bbs_parser");
			//my_lua_printstack(L);

			// ������ς�
			lua_pushlightuserdata(L, &parent);
			lua_pushlightuserdata(L, &body);
			lua_pushlightuserdata(L, &html);

			// �֐����s
			int n_arg = 3;
			int n_ret = 1;
			int status = lua_pcall(L, n_arg, n_ret, 0);

			if (status != 0) {
				// TODO �G���[����
				theApp.MyLuaErrorReport(status);
			} else {
				// �Ԃ�l�擾
				bool result = lua_toboolean(L, -1);
				lua_settop(L, top);
			}

			//theApp.MyLuaExecute(L"mixi.bbs_parser();");
		}
		return mixi::NewBbsParser::parse( body, html );
	case ACCESS_LIST_MYDIARY:					return mixi::ListDiaryParser::parse( body, html );
//	case ACCESS_LIST_FOOTSTEP:					return mixi::ShowLogParser::parse( body, html );
	case ACCESS_LIST_FOOTSTEP:					return mixi::TrackParser::parse( body, html );
	case ACCESS_LIST_MESSAGE_IN:				return mixi::ListMessageParser::parse( body, html );
	case ACCESS_LIST_MESSAGE_OUT:				return mixi::ListMessageParser::parse( body, html );
	case ACCESS_LIST_NEWS:						return mixi::ListNewsCategoryParser::parse( body, html );
	case ACCESS_LIST_FAVORITE_USER:				return mixi::ListBookmarkParser::parse( body, html );
	case ACCESS_LIST_FAVORITE_COMMUNITY:		return mixi::ListBookmarkParser::parse( body, html );
	case ACCESS_LIST_FRIEND:					return mixi::ListFriendParser::parse( body, html );
	case ACCESS_LIST_COMMUNITY:					return mixi::ListCommunityParser::parse( body, html );
	case ACCESS_LIST_INTRO:						return mixi::ShowIntroParser::parse( body, html );
	case ACCESS_LIST_BBS:						return mixi::ListBbsParser::parse( body, html );
	case ACCESS_LIST_NEW_BBS_COMMENT:			return mixi::ListNewBbsCommentParser::parse( body, html );
	case ACCESS_LIST_CALENDAR:					return mixi::ShowCalendarParser::parse( body, html );
	case ACCESS_MIXI_RECENT_ECHO:				return mixi::RecentEchoParser::parse( body, html );
	case ACCESS_TWITTER_FRIENDS_TIMELINE:		return parser::TwitterFriendsTimelineXmlParser::parse( parent, body, html );
	case ACCESS_TWITTER_FAVORITES:				return parser::TwitterFriendsTimelineXmlParser::parse( parent, body, html );	// �b��
	case ACCESS_TWITTER_DIRECT_MESSAGES:		return parser::TwitterDirectMessagesXmlParser::parse( body, html );
	case ACCESS_WASSR_FRIENDS_TIMELINE:			return parser::WassrFriendsTimelineXmlParser::parse( body, html );
	case ACCESS_RSS_READER_FEED:				return parser::RssFeedParser::parse( body, html );
	case ACCESS_GOOHOME_QUOTE_QUOTES_FRIENDS:	return parser::GoohomeQuoteQuotesFriendsParser::parse( body, html );
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
	case ACCESS_NEIGHBORDIARY:	mixi::ViewDiaryParser::parse( mixi, html );			break;
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
	case ACCESS_HELP:			parser::HelpParser::parse( mixi, html );				break;
	case ACCESS_ERRORLOG:		parser::ErrorlogParser::parse( mixi, html );			break;
	case ACCESS_SCHEDULE:
	case ACCESS_PLAIN:			parser::PlainTextParser::parse( mixi, html );			break;
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
	if (!xml2stl::SimpleXmlParser::loadFromText( root, text_, false )) {
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
		const xml2stl::Node& item = node.getNodeByIndex(i);

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

bool MZ3ParserBase::ExtractLinks(CMixiData &data_)
{
	// ���K�\���̃R���p�C���i���̂݁j
	static MyRegex reg;
	if( !util::CompileRegex( reg, L"(h?ttps?://[-_.!~*'()a-zA-Z0-9;/?:@&=+$,%#]+)" ) ) {
		return false;
	}

	CString target = data_.GetBody();

	for( int i=0; i<MZ3_INFINITE_LOOP_MAX_COUNT; i++ ) {	// MZ3_INFINITE_LOOP_MAX_COUNT �͖������[�v�h�~
		if( reg.exec(target) == false || reg.results.size() != 2 ) {
			// �������B�I���B
			break;
		}

		// �����B

		// URL
		std::wstring& url = reg.results[1].str;

		if (!url.empty() && url[0] != 'h') {
			url.insert( url.begin(), 'h' );
		}

		// �f�[�^�ɒǉ�
		data_.m_linkList.push_back( CMixiData::Link(url.c_str(), url.c_str()) );

		// �^�[�Q�b�g���X�V�B
		target = target.Mid( reg.results[0].end );
	}

	return true;
}

bool HelpParser::parse( CMixiData& mixi, const CHtmlArray& html_ )
{
	MZ3LOGGER_DEBUG( L"HelpParser.parse() start." );

	mixi.ClearAllList();

	INT_PTR count = html_.GetCount();

	int iLine = 0;

	int status = 0;		// 0 : start, 1 : �ŏ��̍���, 2 : 2�Ԗڈȍ~�̍��ډ�͒�
	CMixiData child;

	for( ; iLine<count; iLine++ ) {
		const CString& line = html_.GetAt(iLine);

		CString head2 = line.Left(2);

		// - "--" �Ŏn�܂�s�͖�������B
		if( head2 == "--" ) {
			continue;
		}

		// - "* " �Ŏn�܂�s������΁A�X�e�[�^�X�ɉ����ď������s���B
		if( head2 == L"* " ) {
			// 3 �����ڈȍ~�� Author �Ƃ��ēo�^����B
			// ����ɂ��A�͖ڎ��Ƃ��ă��|�[�g��ʂɕ\�������B
			CString chapter = line.Mid(2);
			chapter.Replace( L"\n", L"" );
			chapter.Replace( L"\t", L"    " );
			switch( status ) {
			case 0:
				// �ŏ��̍��ڂ����������̂ŁA�g�b�v�v�f�Ƃ��Ēǉ�����B
				mixi.SetAuthor( chapter );
				status = 1;
				break;
			case 1:
				// 2�Ԗڈȍ~�̍��ڂ����������̂ŁA�ŏ��̎q�v�f�Ƃ��Ēǉ�����B
				child.SetAuthor( chapter );
				status = 2;
				break;

			case 2:
				// 3�Ԗڈȍ~�̍��ڂ����������̂ŁA�ǉ����A�q�v�f������������B
				child.SetCommentIndex( mixi.GetChildrenSize()+1 );
				mixi.AddChild( child );

				child.ClearBody();
				child.SetAuthor( chapter );
				break;
			}
			continue;
		}

		// - "** " �Ŏn�܂�s������΁A�ߍ��ڂƂ��ĉ�́A�ǉ�����B
		if( line.Left(3) == L"** " ) {
			// 4 �����ڈȍ~�� Author �Ƃ��ēo�^����B
			// ����ɂ��A�ߖڎ��Ƃ��ă��|�[�g��ʂɕ\�������B
			CString section = line.Mid(3);
			section.Replace( L"\n", L"" );
			section.Replace( L"\t", L"    " );

			// �ߖ��̂̐擪�Ɏ��ʎq��ǉ�����B
			section.Insert( 0, L" " );

			switch( status ) {
			case 0:
				// �ŏ��̍��ڂƂ��Ă̐ߍ��ڂ͂��肦�Ȃ��̂Ŗ�������B
				break;
			case 1:
				// 1�Ԗڂ̉�͒��Ɍ��������̂ŁA�ŏ��̎q�v�f�Ƃ��Ēǉ�����B
				child.SetAuthor( section );
				status = 2;
				break;

			case 2:
				// 2�Ԗڈȍ~�̉�͒��Ɍ��������̂ŁA�ǉ����A�q�v�f������������B
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
				// �v�f�p��
				CString str = line;
				str.Replace( L"\n", L"" );
				str.Replace( L"\t", L"    " );

				// �v�f�ǉ�
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

bool ErrorlogParser::parse( CMixiData& mixi, const CHtmlArray& html_ )
{
	MZ3LOGGER_DEBUG( L"ErrorlogParser.parse() start." );

	mixi.ClearAllList();
	INT_PTR count = html_.GetCount();

	int iLine = 0;

	int status = 0;		// 0 : start, 1 : �ŏ��̍���, 2 : 2�Ԗڈȍ~�̍��ډ�͒�
	CMixiData child;

	mixi.SetAuthor( MZ3_APP_NAME );
	mixi.AddBody( L"the top element.\r\n" );

	CString msg;
	msg.Format( L"mz3log.txt has %d line(s).", count );
	mixi.AddBody( msg );

	// �y��͑ΏۊT�v�z
	// ---
	// [2007/07/13��12:30:03]��DEBUG���{����[.\InetAccess.cpp:555]
	// ---
	// �܂���
	// ---
	// [2007/07/13��12:06:33]��DEBUG���{��
	// �����Ɩ{��
	// �����Ɩ{����[.\MZ3View.cpp:947]
	// ---
	// �Ƃ����`���B
	for( int iLine=0; iLine<count; iLine++ ) {
		CString target = html_.GetAt(iLine);
		target.Replace(_T("\n"), _T("\r\n"));	// ���s�R�[�h�ϊ�

		child.ClearBody();
		child.SetCommentIndex( iLine+1 );

		// "[�`]" ����t�ɁB
		CString date;
		int index = 0;
		index = util::GetBetweenSubString( target, L"[", L"]", date );
		if( index == -1 ) {
			child.AddBody( L"�� '[�`]' ��������Ȃ��̂ŃX�L�b�v [" + target + L"]" );
			mixi.AddChild( child );
			continue;
		}
		child.SetDate( date );

		// "]" �ȍ~��؂�o��
		target = target.Mid( index );

		// "���`��" ���G���[���x���i���O�j�ɁB
		CString level;
		index = util::GetBetweenSubString( target, L" ", L" ", level );
		if( index == -1 ) {
			// "���`��" ��������Ȃ��̂ŃX�L�b�v
			child.AddBody( L"�� '���`��' ��������Ȃ��̂ŃX�L�b�v [" + target + L"]" );
			mixi.AddChild( child );
			continue;
		}

		// 2�ڂ�"��"�ȍ~��؂�o��
		target = target.Mid( index );

		// ���O�Ɂu���x�����{���v��ݒ�
		level += L" ";
		level += target;
		level = level.Left( 30 );
		child.SetAuthor( level );

		// ������ "]" �Ȃ�{���ɒǉ����ďI���B
		// ������ "]" �ȊO�Ȃ玟�̍s�ȍ~��������܂Ŗ{���Ƃ��ĉ�́B
		child.AddBody( L"\r\n" );
		child.AddBody( target );

		if( target.Right( 3 ) == L"]\r\n" ) {
			// �I��
		}else{
			// ������ "]" �������܂ŉ�͂��ďI���B
			iLine ++;
			for( ;iLine<count; iLine++ ) {
				target = html_.GetAt( iLine );
				target.Replace(_T("\n"), _T("\r\n"));	// ���s�R�[�h�ϊ�
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
