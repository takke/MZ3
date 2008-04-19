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

/// Twitter 用パーサ
namespace twitter {

bool TwitterParserBase::ExtractLinks(CMixiData &data_)
{
	// 正規表現のコンパイル（一回のみ）
	static MyRegex reg;
	if( !util::CompileRegex( reg, L"(h?ttps?://[-_.!~*'()a-zA-Z0-9;/?:@&=+$,%#]+)" ) ) {
		MZ3LOGGER_FATAL( FAILED_TO_COMPILE_REGEX_MSG );
		return false;
	}

	CString target;
	for (u_int i=0; i<data_.GetBodySize(); i++) {
		target.Append( data_.GetBody(i) );
	}

	for( int i=0; i<100; i++ ) {	// 100 は無限ループ防止
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

/**
 * [list] タイムライン用パーサ
 *
 * 【タイムライン】
 * http://twitter.com/statuses/friends_timeline.xml
 */
bool TwitterFriendsTimelineXmlParser::parse( CMixiDataList& out_, const CHtmlArray& html_ )
{
	MZ3LOGGER_DEBUG( L"TwitterFriendsTimelineXmlParser.parse() start." );

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
				data.AddImage( strImage );

				// updated : status/created_at
				mixi::ParserUtil::ParseDate( status.getNode( L"created_at" ).getTextAll().c_str(), data );

				// URL を抽出し、リンクにする
				TwitterParserBase::ExtractLinks( data );

				// id が降順になるように追加する。
				TwitterParserBase::InsertWithOrder(out_, data);

			} catch (xml2stl::NodeNotFoundException& e) {
				MZ3LOGGER_ERROR( util::FormatString( L"some node or property not found... : %s", e.getMessage().c_str()) );
				break;
			}
		}
	} catch (xml2stl::NodeNotFoundException& e) {
		MZ3LOGGER_ERROR( util::FormatString( L"statuses not found... : %s", e.getMessage().c_str()) );
	}

	// 最大件数調整（末尾の余分なデータを削除する）
	TwitterParserBase::EraseExtraItems(out_);

	MZ3LOGGER_DEBUG( L"TwitterFriendsTimelineXmlParser.parse() finished." );
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
		const xml2stl::Node& direct_messages = root.getNode( L"direct-messages" );
		size_t nChildren = direct_messages.getChildrenCount();
		for (size_t i=0; i<nChildren; i++) {
			const xml2stl::Node& node = direct_messages.getNode(i);
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
				TwitterParserBase::ExtractLinks( data );

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

				// id が降順になるように追加する。
				TwitterParserBase::InsertWithOrder(out_, data);

			} catch (xml2stl::NodeNotFoundException& e) {
				MZ3LOGGER_ERROR( util::FormatString( L"some node or property not found... : %s", e.getMessage().c_str()) );
				break;
			}
		}
	} catch (xml2stl::NodeNotFoundException& e) {
		MZ3LOGGER_ERROR( util::FormatString( L"direct-messages not found... : %s", e.getMessage().c_str()) );
	}

	// 最大件数調整（末尾の余分なデータを削除する）
	TwitterParserBase::EraseExtraItems(out_);

	MZ3LOGGER_DEBUG( L"TwitterDirectMessagesXmlParser.parse() finished." );
	return true;
}

}//namespace twitter
