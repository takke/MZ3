/*
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */
#include "stdafx.h"
#include "MZ3.h"
#include "TwitterParser.h"
#include <set>
#include "xml2stl.h"

/// Twitter �p�p�[�T
namespace twitter {

/**
 * [list] �^�C�����C���p�p�[�T
 *
 * �y�^�C�����C���z
 * http://twitter.com/statuses/friends_timeline.xml
 */
bool TwitterFriendsTimelineXmlParser::parse( CMixiDataList& out_, const CHtmlArray& html_ )
{
	MZ3LOGGER_DEBUG( L"TwitterFriendsTimelineXmlParser.parse() start." );

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
		size_t nChildren = statuses.getChildrenCount();
		for (size_t i=0; i<nChildren; i++) {
			const xml2stl::Node& node = statuses.getNode(i);
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
				sourceData.AddBody( source );
				data.AddChild( sourceData );

				// name : status/user/screen_name
				const xml2stl::Node& user = status.getNode( L"user" );
				data.SetName( user.getNode(L"screen_name").getTextAll().c_str() );

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
				data.AddImage( strImage );

				// updated : status/created_at
				mixi::ParserUtil::ParseDate( status.getNode( L"created_at" ).getTextAll().c_str(), data );

				// URL �𒊏o���A�����N�ɂ���
				TwitterFriendsTimelineXmlParser::ExtractLinks( data );

				// id ���~���ɂȂ�悤�ɒǉ�����B
				{
					size_t j=0;
					for (; j<out_.size(); j++) {
						if (id > out_[j].GetID()) {
							break;
						}
					}
					out_.insert( out_.begin()+j, data );

					// for performance tuning
/*					if (out_.size()==1) {
						for (int k=0; k<100; k++) {
							out_.insert( out_.begin()+j, data );
						}
					}
*/				}
			} catch (xml2stl::NodeNotFoundException& e) {
				MZ3LOGGER_ERROR( util::FormatString( L"some node or property not found... : %s", e.getMessage().c_str()) );
				break;
			}
		}
	} catch (xml2stl::NodeNotFoundException& e) {
		MZ3LOGGER_ERROR( util::FormatString( L"statuses not found... : %s", e.getMessage().c_str()) );
	}

	// �ő匏�������i�����̗]���ȃf�[�^���폜����j
	const size_t LIST_MAX_SIZE = 1000;
	if (out_.size()>LIST_MAX_SIZE) {
		out_.erase( out_.begin()+LIST_MAX_SIZE, out_.end() );
	}

	MZ3LOGGER_DEBUG( L"TwitterFriendsTimelineXmlParser.parse() finished." );
	return true;
}

bool TwitterFriendsTimelineXmlParser::ExtractLinks(CMixiData &data_)
{
	// ���K�\���̃R���p�C���i���̂݁j
	static MyRegex reg;
	if( !util::CompileRegex( reg, L"(h?ttps?://[-_.!~*'()a-zA-Z0-9;/?:@&=+$,%#]+)" ) ) {
		MZ3LOGGER_FATAL( FAILED_TO_COMPILE_REGEX_MSG );
		return false;
	}

	CString target;
	for (u_int i=0; i<data_.GetBodySize(); i++) {
		target.Append( data_.GetBody(i) );
	}

	for( int i=0; i<100; i++ ) {	// 100 �͖������[�v�h�~
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

}//namespace twitter
