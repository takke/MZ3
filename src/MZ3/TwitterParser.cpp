/*
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */
#include "stdafx.h"
#include "MZ3.h"
#include "TwitterParser.h"
#include "url_encoder.h"
#include <set>
#include "xml2stl.h"

/// Twitter �p�p�[�T
namespace twitter {

bool TwitterParserBase::ExtractLinks(CMixiData &data_)
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

/**
 * [list] �^�C�����C���p�p�[�T
 *
 * �y�^�C�����C���z
 * http://twitter.com/statuses/friends_timeline.xml
 */
bool TwitterFriendsTimelineXmlParser::parse( CMixiDataList& out_, const CHtmlArray& html_ )
{
	MZ3LOGGER_DEBUG( L"TwitterFriendsTimelineXmlParser.parse() start." );

	util::StopWatch sw;
	sw.start();

	// �V�K�ɒǉ�����f�[�^�Q
	CMixiDataList new_list;

	// html_ �̕�����
	std::vector<TCHAR> text;
	html_.TranslateToVectorBuffer( text );

	// XML ���
	xml2stl::Container root;
	if (!xml2stl::SimpleXmlParser::loadFromText( root, text )) {
		MZ3LOGGER_ERROR( L"XML ��͎��s" );
		return false;
	}

	// �d���h�~�p�� id �ꗗ�𐶐��B
	std::set<int> id_set;
	for (size_t i=0; i<out_.size(); i++) {
		id_set.insert( out_[i].GetID() );
	}

	// status �ɑ΂��鏈��
	try {
		const xml2stl::Node& statuses = root.getNode( L"statuses" );
		for (xml2stl::NodeRef statusesRef = statuses.getNodeRef();
			!statusesRef.isEnd();
			statusesRef.next())
		{
			const xml2stl::Node& node = statusesRef.getCurrentNode();
			if (node.getName() != L"status") {
				continue;
			}
			try {
				const xml2stl::Node& status = node;

				// �I�u�W�F�N�g����
				CMixiData data;
				data.SetAccessType( ACCESS_TWITTER_USER );

				// text : status/text
				CString strBody = status.getNode(L"text").getTextAll().c_str();
				while( strBody.Replace(_T("&amp;"),  _T("&")) ) ;
				mixi::ParserUtil::ReplaceEntityReferenceToCharacter( strBody );
				data.AddBody( strBody );

				// source : status/source
				// �q�v�f�Ƃ��Ēǉ�
				CString source = status.getNode(L"source").getTextAll().c_str();
				mixi::ParserUtil::ReplaceEntityReferenceToCharacter( source );
				CMixiData sourceData;
				sourceData.AddBody( L"source : " + source );
				data.AddChild( sourceData );

				// name : status/user/screen_name
				const xml2stl::Node& user = status.getNode( L"user" );
				data.SetName( user.getNode(L"screen_name").getTextAll().c_str() );
				data.SetAuthor( data.GetName() );

				// author : status/user/name
				CString strAuthor = user.getNode(L"name").getTextAll().c_str();
				mixi::ParserUtil::ReplaceEntityReferenceToCharacter( strAuthor );
				data.SetAuthor( strAuthor );

				// description : status/user/description
				// title �ɓ����̂͋���̍�E�E�E
				CString strTitle = user.getNode(L"description").getTextAll().c_str();
				mixi::ParserUtil::ReplaceEntityReferenceToCharacter( strTitle );
				data.SetTitle( strTitle );

				// id : status/id
				int id = _wtoi( status.getNode(L"id").getTextAll().c_str() );
				data.SetID( id );

				// ����ID������Βǉ����Ȃ��B
				if (id_set.count(id)>0) {
					continue;
				}

				// owner-id : status/user/id
				data.SetOwnerID( _wtoi( user.getNode(L"id").getTextAll().c_str() ) );

				// URL : status/user/url
				CString url = user.getNode( L"url" ).getTextAll().c_str();
				data.SetURL( url );
				data.SetBrowseUri( url );

				// Image : status/user/profile_image_url
				CString strImage = user.getNode( L"profile_image_url" ).getTextAll().c_str();
				mixi::ParserUtil::ReplaceEntityReferenceToCharacter( strImage );
				// �t�@�C�����݂̂�URL�G���R�[�h
				int idx = strImage.ReverseFind( '/' );
				if (idx >= 0) {
					CString strFileName = strImage.Mid( idx +1 );
					strFileName = URLEncoder::encode_utf8( strFileName );
					strImage = strImage.Left(idx + 1);
					strImage += strFileName;
				}

				data.AddImage( strImage );

				// updated : status/created_at
				mixi::ParserUtil::ParseDate( status.getNode( L"created_at" ).getTextAll().c_str(), data );

				// URL �𒊏o���A�����N�ɂ���
				TwitterParserBase::ExtractLinks( data );

				// �ꎞ���X�g�ɒǉ�
				new_list.push_back(data);

			} catch (xml2stl::NodeNotFoundException& e) {
				MZ3LOGGER_ERROR( util::FormatString( L"some node or property not found... : %s", e.getMessage().c_str()) );
				break;
			}
		}
	} catch (xml2stl::NodeNotFoundException& e) {
		MZ3LOGGER_ERROR( util::FormatString( L"statuses not found... : %s", e.getMessage().c_str()) );
	}


	// ���������f�[�^���o�͂ɔ��f
	TwitterParserBase::AppendNewList(out_, new_list);

	MZ3LOGGER_DEBUG( util::FormatString(L"TwitterFriendsTimelineXmlParser.parse() finished. elapsed:%d[msec]", sw.getElapsedMilliSecUntilNow()) );
	return true;
}

/**
 * [list] DM�p�p�[�T
 *
 * �y���b�Z�[�W�z
 * http://twitter.com/direct_messages.xml
 */
bool TwitterDirectMessagesXmlParser::parse( CMixiDataList& out_, const CHtmlArray& html_ )
{
	MZ3LOGGER_DEBUG( L"TwitterDirectMessagesXmlParser.parse() start." );

	// �V�K�ɒǉ�����f�[�^�Q
	CMixiDataList new_list;

	// html_ �̕�����
	std::vector<TCHAR> text;
	html_.TranslateToVectorBuffer( text );

	// XML ���
	xml2stl::Container root;
	if (!xml2stl::SimpleXmlParser::loadFromText( root, text )) {
		MZ3LOGGER_ERROR( L"XML ��͎��s" );
		return false;
	}

	// �d���h�~�p�� id �ꗗ�𐶐��B
	std::set<int> id_set;
	for (size_t i=0; i<out_.size(); i++) {
		id_set.insert( out_[i].GetID() );
	}

	// direct-messages �ɑ΂��鏈��
	try {
		const xml2stl::Node& direct_messages = root.getNode( L"direct-messages" );
		size_t nChildren = direct_messages.getChildrenCount();
		for (size_t i=0; i<nChildren; i++) {
			const xml2stl::Node& node = direct_messages.getNode(i);
			if (node.getName() != L"direct_message") {
				continue;
			}
			try {
				const xml2stl::Node& direct_message = node;

				// �I�u�W�F�N�g����
				CMixiData data;
				data.SetAccessType( ACCESS_TWITTER_USER );

				// text : direct_message/text
				CString strBody = direct_message.getNode(L"text").getTextAll().c_str();
				while( strBody.Replace(_T("&amp;"),  _T("&")) ) ;
				mixi::ParserUtil::ReplaceEntityReferenceToCharacter( strBody );
				data.AddBody( strBody );

				// id : direct_message/id
				int id = _wtoi( direct_message.getNode(L"id").getTextAll().c_str() );
				data.SetID( id );

				// ����ID������Βǉ����Ȃ��B
				if (id_set.count(id)>0) {
					continue;
				}

				// updated : direct_message/created_at
				mixi::ParserUtil::ParseDate( direct_message.getNode( L"created_at" ).getTextAll().c_str(), data );

				// URL �𒊏o���A�����N�ɂ���
				TwitterParserBase::ExtractLinks( data );

				// ���M�ҏ��FCMixiData �̃��^�f�[�^�Ƃ��ĕۑ�����
				{
					// name : direct_message/sender/screen_name
					const xml2stl::Node& sender = direct_message.getNode( L"sender" );
					data.SetName( sender.getNode(L"screen_name").getTextAll().c_str() );

					// author : direct_message/sender/name
					CString strAuthor = sender.getNode(L"name").getTextAll().c_str();
					mixi::ParserUtil::ReplaceEntityReferenceToCharacter( strAuthor );
					data.SetAuthor( strAuthor );

					// description : direct_message/sender/description
					// title �ɓ����̂͋���̍�E�E�E
					CString strTitle = sender.getNode(L"description").getTextAll().c_str();
					mixi::ParserUtil::ReplaceEntityReferenceToCharacter( strTitle );
					data.SetTitle( strTitle );

					// owner-id : direct_message/sender/id
					data.SetOwnerID( _wtoi( sender.getNode(L"id").getTextAll().c_str() ) );

					// URL : direct_message/sender/url
					CString url = sender.getNode( L"url" ).getTextAll().c_str();
					data.SetURL( url );
					data.SetBrowseUri( url );

					// Image : direct_message/sender/profile_image_url
					CString strImage = sender.getNode( L"profile_image_url" ).getTextAll().c_str();
					mixi::ParserUtil::ReplaceEntityReferenceToCharacter( strImage );
					data.AddImage( strImage );
				}

				// ��M�ҏ��F�q�v�f�Ƃ��ĕۑ�����i�ڍו\�����ɂ̂݉{���\�j
				{
					CMixiData recipientData;
					const xml2stl::Node& recipient = direct_message.getNode( L"recipient" );

					// direct_message/recipient/screen_name
					// direct_message/recipient/name
					CString strAuthor = recipient.getNode(L"name").getTextAll().c_str();
					mixi::ParserUtil::ReplaceEntityReferenceToCharacter( strAuthor );

					CString s;
					s.Format( L"��M�� : %s(%s)", 
						recipient.getNode(L"screen_name").getTextAll().c_str(),
						strAuthor
						);
					recipientData.AddBody( s );

					data.AddChild( recipientData );
				}

				// �ꎞ���X�g�ɒǉ�
				new_list.push_back(data);

			} catch (xml2stl::NodeNotFoundException& e) {
				MZ3LOGGER_ERROR( util::FormatString( L"some node or property not found... : %s", e.getMessage().c_str()) );
				break;
			}
		}
	} catch (xml2stl::NodeNotFoundException& e) {
		MZ3LOGGER_ERROR( util::FormatString( L"direct-messages not found... : %s", e.getMessage().c_str()) );
	}

	// ���������f�[�^���o�͂ɔ��f
	TwitterParserBase::AppendNewList(out_, new_list);

	MZ3LOGGER_DEBUG( L"TwitterDirectMessagesXmlParser.parse() finished." );
	return true;
}

/**
 * [list] �^�C�����C���p�p�[�T
 *
 * �y�^�C�����C���z
 * http://api.wassr.jp/statuses/friends_timeline.xml
 */
bool WassrFriendsTimelineXmlParser::parse( CMixiDataList& out_, const CHtmlArray& html_ )
{
	MZ3LOGGER_DEBUG( L"WassrFriendsTimelineXmlParser.parse() start." );

	util::StopWatch sw;
	sw.start();

	// �V�K�ɒǉ�����f�[�^�Q
	CMixiDataList new_list;

	// html_ �̕�����
	std::vector<TCHAR> text;
	html_.TranslateToVectorBuffer( text );

	// XML ���
	xml2stl::Container root;
	if (!xml2stl::SimpleXmlParser::loadFromText( root, text )) {
		MZ3LOGGER_ERROR( L"XML ��͎��s" );
		return false;
	}

	// �d���h�~�p�� id �ꗗ�𐶐��B
	std::set<int> id_set;
	for (size_t i=0; i<out_.size(); i++) {
		id_set.insert( out_[i].GetID() );
	}

	// status �ɑ΂��鏈��
	try {
		const xml2stl::Node& statuses = root.getNode( L"statuses" );
		for (xml2stl::NodeRef statusesRef = statuses.getNodeRef();
			!statusesRef.isEnd();
			statusesRef.next())
		{
			const xml2stl::Node& node = statusesRef.getCurrentNode();
			if (node.getName() != L"status") {
				continue;
			}
			try {
				const xml2stl::Node& status = node;

				// �I�u�W�F�N�g����
				CMixiData data;
				data.SetAccessType( ACCESS_WASSR_USER );

				// text : status/text
				CString strBody = status.getNode(L"text").getTextAll().c_str();
				while( strBody.Replace(_T("&amp;"),  _T("&")) ) ;
				mixi::ParserUtil::ReplaceEntityReferenceToCharacter( strBody );
				data.AddBody( strBody );

				// source : status/source
/*				try {
					// �q�v�f�Ƃ��Ēǉ�
					CString source = status.getNode(L"source").getTextAll().c_str();
					mixi::ParserUtil::ReplaceEntityReferenceToCharacter( source );
					CMixiData sourceData;
					sourceData.AddBody( L"source : " + source );
					data.AddChild( sourceData );
				} catch (xml2stl::NodeNotFoundException& e) {
					MZ3LOGGER_INFO( e.getMessage().c_str() );
				} 
*/
				// name : status/user/screen_name
				const xml2stl::Node& user = status.getNode( L"user" );
				CString strName = user.getNode(L"screen_name").getTextAll().c_str();
				mixi::ParserUtil::ReplaceEntityReferenceToCharacter( strName );
				data.SetName( strName );
				data.SetAuthor( data.GetName() );

				// author : status/user/name
/*				try {
					CString strAuthor = user.getNode(L"name").getTextAll().c_str();
					mixi::ParserUtil::ReplaceEntityReferenceToCharacter( strAuthor );
					data.SetAuthor( strAuthor );
				} catch (xml2stl::NodeNotFoundException& e) {
					MZ3LOGGER_INFO( e.getMessage().c_str() );
				}
*/
/*				// description : status/user/description
				try {
					// title �ɓ����̂͋���̍�E�E�E
					CString strTitle = user.getNode(L"description").getTextAll().c_str();
					mixi::ParserUtil::ReplaceEntityReferenceToCharacter( strTitle );
					data.SetTitle( strTitle );
				} catch (xml2stl::NodeNotFoundException& e) {
					MZ3LOGGER_INFO( e.getMessage().c_str() );
				}
*/
				// id : status/id
				int id = _wtoi( status.getNode(L"id").getTextAll().c_str() );
				data.SetID( id );

				// ����ID������Βǉ����Ȃ��B
				if (id_set.count(id)>0) {
					continue;
				}

				// owner-id : status/user_login_id
				data.SetOwnerID( _wtoi( status.getNode(L"user_login_id").getTextAll().c_str() ) );

				// URL : status/user/url
//				CString url = user.getNode( L"url" ).getTextAll().c_str();
//				data.SetURL( url );
//				data.SetBrowseUri( url );

				// Image : status/user/profile_image_url
				CString strImage = user.getNode( L"profile_image_url" ).getTextAll().c_str();
				mixi::ParserUtil::ReplaceEntityReferenceToCharacter( strImage );
				// �t�@�C�����݂̂�URL�G���R�[�h
				int idx = strImage.ReverseFind( '/' );
				if (idx >= 0) {
					CString strFileName = strImage.Mid( idx +1 );
					strFileName = URLEncoder::encode_utf8( strFileName );
					strImage = strImage.Left(idx + 1);
					strImage += strFileName;
				}

				data.AddImage( strImage );

				// updated : status/epoch
				mixi::ParserUtil::ParseDate( status.getNode( L"epoch" ).getTextAll().c_str(), data );

				// URL �𒊏o���A�����N�ɂ���
				TwitterParserBase::ExtractLinks( data );

				// �ꎞ���X�g�ɒǉ�
				new_list.push_back(data);

			} catch (xml2stl::NodeNotFoundException& e) {
				MZ3LOGGER_ERROR( util::FormatString( L"some node or property not found... : %s", e.getMessage().c_str()) );
				break;
			}
		}
	} catch (xml2stl::NodeNotFoundException& e) {
		MZ3LOGGER_ERROR( util::FormatString( L"statuses not found... : %s", e.getMessage().c_str()) );
	}

	// ���������f�[�^���o�͂ɔ��f
	TwitterParserBase::AppendNewList(out_, new_list);

	MZ3LOGGER_DEBUG( util::FormatString(L"WassrFriendsTimelineXmlParser.parse() finished. elapsed:%d[msec]", sw.getElapsedMilliSecUntilNow()) );
	return true;
}

}//namespace twitter
