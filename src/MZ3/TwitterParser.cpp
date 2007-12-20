/*
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */
#include "stdafx.h"
#include "MZ3.h"
#include "MixiParser.h"

/// Twitter 用パーサ
namespace twitter {

/**
 * [list] タイムライン用パーサ
 * 【タイムライン】
 * http://twitter.com/statuses/friends_timeline.xml
 */
bool TwitterFriendsTimelineAtomParser::parse( CMixiDataList& out_, const CHtmlArray& html_ )
{
	MZ3LOGGER_DEBUG( L"TwitterFriendsTimelineAtomParser.parse() start." );

	// html_ の文字列化
	std::vector<TCHAR> text;
	html_.TranslateToVectorBuffer( text );

	// XML 解析
	xml2stl::Container root;
	if (!xml2stl::SimpleXmlParser::loadFromText( root, text )) {
		MZ3LOGGER_ERROR( L"XML 解析失敗" );
		return false;
	}

	// status に対する処理
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

				// オブジェクト生成
				CMixiData data;
				data.SetAccessType( ACCESS_TWITTER_USER );

				// text : status/text
				CString strBody = status.getNode(L"text").getTextAll().c_str();
				mixi::ParserUtil::ReplaceEntityReferenceToCharacter( strBody );
				data.AddBody( strBody );

				// name : status/user/screen_name
				const xml2stl::Node& user = status.getNode( L"user" );
				data.SetName( user.getNode(L"screen_name").getTextAll().c_str() );

				// author : status/user/name
				CString strAuthor = user.getNode(L"name").getTextAll().c_str();
				mixi::ParserUtil::ReplaceEntityReferenceToCharacter( strAuthor );
				data.SetAuthor( strAuthor );

				// description : status/user/description
				// title に入れるのは苦肉の策・・・
				CString strTitle = user.getNode(L"description").getTextAll().c_str();
				mixi::ParserUtil::ReplaceEntityReferenceToCharacter( strTitle );
				data.SetTitle( strTitle );

				// id : status/id
				data.SetID( _wtoi( status.getNode(L"id").getTextAll().c_str() ) );

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

				// 完成したので追加する
				out_.push_back( data );
			} catch (xml2stl::NodeNotFoundException& e) {
				MZ3LOGGER_ERROR( util::FormatString( L"some node or property not found... : %s", e.getMessage().c_str()) );
				break;
			}
		}
	} catch (xml2stl::NodeNotFoundException& e) {
		MZ3LOGGER_ERROR( util::FormatString( L"statuses not found... : %s", e.getMessage().c_str()) );
	}

	MZ3LOGGER_DEBUG( L"TwitterFriendsTimelineAtomParser.parse() finished." );
	return true;
}

}//namespace twitter
