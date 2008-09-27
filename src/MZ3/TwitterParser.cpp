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

/// Twitter 用パーサ
namespace twitter {

/**
 * [list] タイムライン用パーサ
 *
 * 【タイムライン】
 * http://twitter.com/statuses/friends_timeline.xml
 */
bool TwitterFriendsTimelineXmlParser::parse( CMixiData& parent, CMixiDataList& out_, const CHtmlArray& html_ )
{
	MZ3LOGGER_DEBUG( L"TwitterFriendsTimelineXmlParser.parse() start." );

	util::StopWatch sw, sw_serialize, sw_parse, sw_generate;
	sw.start();

	// 新規に追加するデータ群
	CMixiDataList new_list;

	// html_ の文字列化
	sw_serialize.start();
	std::vector<TCHAR> text;
	html_.TranslateToVectorBuffer( text );
	sw_serialize.stop();

	// XML 解析
	sw_parse.start();
	xml2stl::Container root;
	if (!xml2stl::SimpleXmlParser::loadFromText( root, text )) {
		MZ3LOGGER_ERROR( L"XML 解析失敗" );
		return false;
	}
	sw_parse.stop();

	// 重複防止用の id 一覧を生成。
	std::set<int> id_set;
	for (size_t i=0; i<out_.size(); i++) {
		id_set.insert( out_[i].GetID() );
	}

	// status に対する処理
	sw_generate.start();
	try {
		const xml2stl::Node& statuses = root.getNode( L"statuses" );
		for (xml2stl::NodeRef statusesRef = statuses.getChildrenNodeRef();
			!statusesRef.isEnd();
			statusesRef.next())
		{
			const xml2stl::Node& node = statusesRef.getCurrentNode();
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
				while( strBody.Replace(_T("&amp;"),  _T("&")) ) ;
				mixi::ParserUtil::ReplaceEntityReferenceToCharacter( strBody );
				data.AddBody( strBody );

				// source : status/source
				// 子要素として追加
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
				// title に入れるのは苦肉の策・・・
				CString strTitle = user.getNode(L"description").getTextAll().c_str();
				mixi::ParserUtil::ReplaceEntityReferenceToCharacter( strTitle );
				data.SetTitle( strTitle );

				// id : status/id
				int id = _wtoi( status.getNode(L"id").getTextAll().c_str() );
				data.SetID( id );

				// 同一IDがあれば追加しない。
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
				// ファイル名のみをURLエンコード
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

				// URL を抽出し、リンクにする
				MZ3ParserBase::ExtractLinks( data );

				// 一時リストに追加
				new_list.push_back(data);

			} catch (xml2stl::NodeNotFoundException& e) {
				MZ3LOGGER_ERROR( util::FormatString( L"some node or property not found... : %s", e.getMessage().c_str()) );
				break;
			}
		}
	} catch (xml2stl::NodeNotFoundException& e) {
		MZ3LOGGER_ERROR( util::FormatString( L"statuses not found... : %s", e.getMessage().c_str()) );
	}
	sw_generate.stop();

	// 生成したデータを出力に反映
	TwitterParserBase::MergeNewList(out_, new_list);

	// 新着件数を parent(カテゴリの m_mixi) に設定する
	parent.SetIntValue(L"new_count", new_list.size());

	MZ3LOGGER_DEBUG( 
		util::FormatString(
			L"TwitterFriendsTimelineXmlParser.parse() finished. elapsed:[%dms]"
			L", serialize[%dms], parse[%dms], generate[%dms]", 
			sw.getElapsedMilliSecUntilNow(),
			sw_serialize.getElapsedMilliSecUntilStoped(), 
			sw_parse.getElapsedMilliSecUntilStoped(), 
			sw_generate.getElapsedMilliSecUntilStoped()
			) );
	return true;
}

/**
 * [list] DM用パーサ
 *
 * 【メッセージ】
 * http://twitter.com/direct_messages.xml
 */
bool TwitterDirectMessagesXmlParser::parse( CMixiDataList& out_, const CHtmlArray& html_ )
{
	MZ3LOGGER_DEBUG( L"TwitterDirectMessagesXmlParser.parse() start." );

	// 新規に追加するデータ群
	CMixiDataList new_list;

	// html_ の文字列化
	std::vector<TCHAR> text;
	html_.TranslateToVectorBuffer( text );

	// XML 解析
	xml2stl::Container root;
	if (!xml2stl::SimpleXmlParser::loadFromText( root, text )) {
		MZ3LOGGER_ERROR( L"XML 解析失敗" );
		return false;
	}

	// 重複防止用の id 一覧を生成。
	std::set<int> id_set;
	for (size_t i=0; i<out_.size(); i++) {
		id_set.insert( out_[i].GetID() );
	}

	// direct-messages に対する処理
	try {
		const xml2stl::Node& direct_messages = root.getNode( L"directmessages" );
		size_t nChildren = direct_messages.getChildrenCount();
		for (size_t i=0; i<nChildren; i++) {
			const xml2stl::Node& node = direct_messages.getNodeByIndex(i);
			if (node.getName() != L"direct_message") {
				continue;
			}
			try {
				const xml2stl::Node& direct_message = node;

				// オブジェクト生成
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

				// 同一IDがあれば追加しない。
				if (id_set.count(id)>0) {
					continue;
				}

				// updated : direct_message/created_at
				mixi::ParserUtil::ParseDate( direct_message.getNode( L"created_at" ).getTextAll().c_str(), data );

				// URL を抽出し、リンクにする
				MZ3ParserBase::ExtractLinks( data );

				// 送信者情報：CMixiData のメタデータとして保存する
				{
					// name : direct_message/sender/screen_name
					const xml2stl::Node& sender = direct_message.getNode( L"sender" );
					data.SetName( sender.getNode(L"screen_name").getTextAll().c_str() );

					// author : direct_message/sender/name
					CString strAuthor = sender.getNode(L"name").getTextAll().c_str();
					mixi::ParserUtil::ReplaceEntityReferenceToCharacter( strAuthor );
					data.SetAuthor( strAuthor );

					// description : direct_message/sender/description
					// title に入れるのは苦肉の策・・・
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

				// 受信者情報：子要素として保存する（詳細表示時にのみ閲覧可能）
				{
					CMixiData recipientData;
					const xml2stl::Node& recipient = direct_message.getNode( L"recipient" );

					// direct_message/recipient/screen_name
					// direct_message/recipient/name
					CString strAuthor = recipient.getNode(L"name").getTextAll().c_str();
					mixi::ParserUtil::ReplaceEntityReferenceToCharacter( strAuthor );

					CString s;
					s.Format( L"受信者 : %s(%s)", 
						recipient.getNode(L"screen_name").getTextAll().c_str(),
						strAuthor
						);
					recipientData.AddBody( s );

					data.AddChild( recipientData );
				}

				// 一時リストに追加
				new_list.push_back(data);

			} catch (xml2stl::NodeNotFoundException& e) {
				MZ3LOGGER_ERROR( util::FormatString( L"some node or property not found... : %s", e.getMessage().c_str()) );
				break;
			}
		}
	} catch (xml2stl::NodeNotFoundException& e) {
		MZ3LOGGER_ERROR( util::FormatString( L"direct-messages not found... : %s", e.getMessage().c_str()) );
	}

	// 生成したデータを出力に反映
	TwitterParserBase::MergeNewList(out_, new_list);

	MZ3LOGGER_DEBUG( L"TwitterDirectMessagesXmlParser.parse() finished." );
	return true;
}

/**
 * [list] タイムライン用パーサ
 *
 * 【タイムライン】
 * http://api.wassr.jp/statuses/friends_timeline.xml
 */
bool WassrFriendsTimelineXmlParser::parse( CMixiDataList& out_, const CHtmlArray& html_ )
{
	MZ3LOGGER_DEBUG( L"WassrFriendsTimelineXmlParser.parse() start." );

	util::StopWatch sw;
	sw.start();

	// 新規に追加するデータ群
	CMixiDataList new_list;

	// html_ の文字列化
	std::vector<TCHAR> text;
	html_.TranslateToVectorBuffer( text );

	// XML 解析
	xml2stl::Container root;
	if (!xml2stl::SimpleXmlParser::loadFromText( root, text )) {
		MZ3LOGGER_ERROR( L"XML 解析失敗" );
		return false;
	}

	// 重複防止用の id 一覧を生成。
	std::set<int> id_set;
	for (size_t i=0; i<out_.size(); i++) {
		id_set.insert( out_[i].GetID() );
	}

	// status に対する処理
	try {
		const xml2stl::Node& statuses = root.getNode( L"statuses" );
		for (xml2stl::NodeRef statusesRef = statuses.getChildrenNodeRef();
			!statusesRef.isEnd();
			statusesRef.next())
		{
			const xml2stl::Node& node = statusesRef.getCurrentNode();
			if (node.getName() != L"status") {
				continue;
			}
			try {
				const xml2stl::Node& status = node;

				// オブジェクト生成
				CMixiData data;
				data.SetAccessType( ACCESS_WASSR_USER );

				// text : status/text
				CString strBody = status.getNode(L"text").getTextAll().c_str();
				while( strBody.Replace(_T("&amp;"),  _T("&")) ) ;
				mixi::ParserUtil::ReplaceEntityReferenceToCharacter( strBody );
				data.AddBody( strBody );

				// source : status/source
/*				try {
					// 子要素として追加
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
					// title に入れるのは苦肉の策・・・
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

				// 同一IDがあれば追加しない。
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
				// ファイル名のみをURLエンコード
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

				// URL を抽出し、リンクにする
				MZ3ParserBase::ExtractLinks( data );

				// 一時リストに追加
				new_list.push_back(data);

			} catch (xml2stl::NodeNotFoundException& e) {
				MZ3LOGGER_ERROR( util::FormatString( L"some node or property not found... : %s", e.getMessage().c_str()) );
				break;
			}
		}
	} catch (xml2stl::NodeNotFoundException& e) {
		MZ3LOGGER_ERROR( util::FormatString( L"statuses not found... : %s", e.getMessage().c_str()) );
	}

	// 生成したデータを出力に反映
	TwitterParserBase::MergeNewList(out_, new_list);

	MZ3LOGGER_DEBUG( util::FormatString(L"WassrFriendsTimelineXmlParser.parse() finished. elapsed:%d[msec]", sw.getElapsedMilliSecUntilNow()) );
	return true;
}

}//namespace twitter
