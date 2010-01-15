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

/// MZ3�pHTML�p�[�T
namespace parser {

/**
 * MZ3 Script : �p�[�T�Ăяo��
 */
static bool CallMZ3ScriptParser(const char* szServiceType, const char* szParserFuncName, 
								CMixiData* parent, CMixiDataList* body, CHtmlArray& html )
{
	// �����Ƃ��� parent, body, html ��n��

	// �X�^�b�N�̃T�C�Y���o���Ă���
	lua_State* L = theApp.m_luaState;
	int top = lua_gettop(L);

	// Lua�֐���("mixi.bbs_parser")��ς�
	lua_getglobal(L, szServiceType);		// szServiceType �e�[�u�����X�^�b�N�ɐς�
	//my_lua_printstack(L);
	lua_pushstring(L, szParserFuncName);	// �Ώەϐ�(�֐���)���e�[�u���ɐς�
	//my_lua_printstack(L);
	lua_gettable(L, -2);					// �X�^�b�N��2�Ԗڂ̗v�f("mixi"�e�[�u��)����A
											// �e�[�u���g�b�v�̕�����("bbs_parser")�Ŏ�����郁���o��
											// �X�^�b�N�ɐς�
	//my_lua_printstack(L);

	// ������ς�
	lua_pushlightuserdata(L, parent);
	lua_pushlightuserdata(L, body);
	lua_pushlightuserdata(L, &html);

	// �֐����s
	int n_arg = 3;
	int n_ret = 1;
	int status = lua_pcall(L, n_arg, n_ret, 0);

	int result = 0;
	if (status != 0) {
		// TODO �G���[����
		theApp.MyLuaErrorReport(status);
		return false;
	} else {
		// �Ԃ�l�擾
		result = lua_toboolean(L, -1);
	}
	lua_settop(L, top);
	return result != 0;
}

/// ���X�g�nHTML�̉��
bool MyDoParseMixiListHtml( ACCESS_TYPE aType, CMixiData& parent, CMixiDataList& body, CHtmlArray& html )
{
	// MZ3 Script �p�p�[�T������Η��p����
	const char* szSerializeKey = theApp.m_accessTypeInfo.getSerializeKey(aType);
	bool bRunLuaParser = false;
	if (theApp.m_luaParsers.count(szSerializeKey)!=0) {
		CStringA strParserName = theApp.m_luaParsers[szSerializeKey].c_str();

		// �p�[�T�����e�[�u���Ɗ֐����ɕ�������
		int idx = strParserName.Find('.');
		if (idx>0) {
			CStringA strTable = strParserName.Left(idx);
			CStringA strFuncName = strParserName.Mid(idx+1);
//			printf("parser : [%s], [%s]\n", strTable, strFuncName);
			CallMZ3ScriptParser(strTable, strFuncName, &parent, &body, html);

			// ProMode �p�̏�����ɏI������
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
	msg.Format( L"�T�|�[�g�O�̃A�N�Z�X��ʂł�(%d:%s)", aType, theApp.m_accessTypeInfo.getShortText(aType) );
	MZ3LOGGER_ERROR(msg);
	MessageBox(NULL, msg, NULL, MB_OK);
	return false;
}

/// View�nHTML�̉��
bool MyDoParseMixiHtml( ACCESS_TYPE aType, CMixiData& mixi, CHtmlArray& html )
{
	// MZ3 Script �p�p�[�T������Η��p����
	const char* szSerializeKey = theApp.m_accessTypeInfo.getSerializeKey(aType);
	if (theApp.m_luaParsers.count(szSerializeKey)!=0) {
		CStringA strParserName = theApp.m_luaParsers[szSerializeKey].c_str();

		// �p�[�T�����e�[�u���Ɗ֐����ɕ�������
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
	msg.Format( L"�T�|�[�g�O�̃A�N�Z�X��ʂł�(%d:%s)", aType, theApp.m_accessTypeInfo.getShortText(aType) );
	MZ3LOGGER_ERROR(msg);
	MessageBox(NULL, msg, NULL, MB_OK);
	return false;
}

//
// Twitter �p�p�[�T
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
		// �����N���o
		static MyRegex reg4;
		if( !util::CompileRegex( reg4, L"(https?://[-_.!~*'()a-zA-Z0-9;/?:@&=+$,%#]+)" ) ) {
			MZ3LOGGER_FATAL( FAILED_TO_COMPILE_REGEX_MSG );
			return;
		}

		CString target = s;
		for( int i=0; i<MZ3_INFINITE_LOOP_MAX_COUNT; i++ ) {	// MZ3_INFINITE_LOOP_MAX_COUNT �͖������[�v�h�~

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
				// �������B
				// �c��̕�����������ďI���B
				break;
			}

			data.m_linkList.push_back( CMixiData::Link(url, url) );

			// �^�[�Q�b�g���X�V�B
			target = target.Mid( offsetend );
		}
	}
}

bool TwitterFriendsTimelineXmlParser::parse( CMixiData& parent, CMixiDataList& body_, const CHtmlArray& html_ )
{
	MZ3LOGGER_DEBUG( L"TwitterFriendsTimelineXmlParser.parse() start." );

	INT_PTR count = html_.GetCount();

	// �S�������Ȃ�

	// �d���h�~�p�� id �ꗗ�𐶐�
	stdext::hash_set<INT64> id_set;
	int n = body_.size();
	for (int i=0; i<n; i++) {
		INT64 id = body_[i].GetID();
		id_set.insert(id);

		// �S�Ă� new �t���O������
		body_[i].SetIntValue(L"new_flag", 0);
	}

	util::StopWatch sw;
	sw.start();

	// �ꎞ���X�g
	CMixiDataList new_list;

	for (int i=0; i<count; i++) {
		const CString& line = html_.GetAt(i);
		if (util::LineHasStringsNoCase(line, L"<status>")) {
			// </status> �܂Ŏ擾����
			// �A���A����ID�������skip����
			i++;

			// data ����
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
						MZ3LOGGER_DEBUG(util::FormatString(L"id[%d]�͊��ɑ��݂���̂�skip����", id));
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
						// �����s�Ή�
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
						
						// URL �𒊏o���A�����N�ɂ���
						MyExtractLinks(s, data);

					} else if (tagName == L"source" && MyGetMatchString(line, L">", L"<", s)) {
						// source : status/source
						mixi::ParserUtil::ReplaceEntityReferenceToCharacter(s);
						data.SetTextValue(L"source", s);

					} else if (tagName == L"in_reply_to_status_id" && MyGetMatchString(line, L">", L"<", s)) {
						// in_reply_to_status_id : status/in_reply_to_status_id
						data.SetIntValue(L"in_reply_to_status_id", _wtol(s));
					} else if (tagName == L"retweeted_status") {
						// </retweeted_status> �܂œǂݔ�΂�(RT�Ή�)
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

						// title �ɓ����̂͋���̍�E�E�E
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
					// �ꎞ���X�g�ɒǉ�
					data.SetTextValue(L"user_tag", user_tag);
					new_list.push_back(data);
					break;
				}

				i_in_status++;
				i++;
			}
		}
	}

	// ���������f�[�^���o�͂ɔ��f
#ifdef WINCE
	TwitterParserBase::MergeNewList(body_, new_list, 100);
#else
	TwitterParserBase::MergeNewList(body_, new_list, 1000);
#endif

	// �V�������� parent(�J�e�S���� m_mixi) �ɐݒ肷��
	parent.SetIntValue(L"new_count", new_list.size());
	
	sw.stop();

	MZ3LOGGER_DEBUG(
		util::FormatString(L"TwitterFriendsTimelineXmlParser.parse() end; elapsed : %d [msec]",
			sw.getElapsedMilliSecUntilStoped()));
	return true;
}

#ifdef BT_MZ3
//
// RSS �p�p�[�T
//

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
#endif	// BT_MZ3


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

//
// help �p�p�[�T
//

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

//
// �G���[���O �p�p�[�T
//

bool ErrorlogParser::parse( CMixiData& mixi, const CHtmlArray& html_ )
{
	MZ3LOGGER_DEBUG( L"ErrorlogParser.parse() start." );

	mixi.ClearAllList();
	INT_PTR count = html_.GetCount();

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

	// ������N�s�̂ݑΏۂƂ���
	int iLine = 0;
	const int LINE_LIMIT = 200;
	if (count > LINE_LIMIT) {
		iLine = count-LINE_LIMIT;
	}
	for(; iLine<count; iLine++ ) {
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

//
// �e�L�X�g�p�p�[�T
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
