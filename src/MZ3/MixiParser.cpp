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

/// mixi 用HTMLパーサ
namespace mixi {

bool ListBookmarkParser::parse( CMixiDataList& out_, const CHtmlArray& html_ )
{
	MZ3LOGGER_DEBUG( L"ListBookmarkParser.parse() start." );

	// html_ の文字列化
	std::vector<TCHAR> text;
	html_.TranslateToVectorBuffer( text );

	// XML 解析
	xml2stl::Container root;
	if (!xml2stl::SimpleXmlParser::loadFromText( root, text )) {
		MZ3LOGGER_ERROR( L"XML 解析失敗" );
		return false;
	}

	try {
		const xml2stl::Node& div_bookmarkList = root.getNode( L"html" )
													.getNode( L"body" )
													.getNode( L"div", L"id=bodyArea" )
													.getNode( L"div", L"id=bodyMainArea" )
													.getNode( L"div", L"id=bodyMainAreaMain" )
													.getNode( L"div", L"class=bookmarkList" );

		// 一覧
		const xml2stl::Node& ul = div_bookmarkList.getNode( L"ul" );
		for (size_t i=0; i<ul.getChildrenCount(); i++) {
			const xml2stl::Node& li = ul.getNode(i);
			if (li.getName()!=L"li") {
				continue;
			}

			CMixiData mixi;

			// image
			const xml2stl::Node& div_listIcon = li.getNode( L"div", L"class=listIcon" );
			xml2stl::XML2STL_STRING image_url = div_listIcon.getNode(L"a").getNode(L"img").getProperty(L"src");
			mixi.AddImage(image_url.c_str());

			// the item info
			const xml2stl::Node& div_listContents = li.getNode( L"div", L"class=listContents" );
			const xml2stl::Node& a = div_listContents.getNode(L"div", L"class=heading").getNode(L"a");
			
			// URL 構築＆設定
			CString url = a.getProperty(L"href").c_str();
			mixi.SetURL( url );
			url.Insert( 0, L"http://mixi.jp/" );
			mixi.SetBrowseUri( url );

			// name
			mixi.SetName( a.getTextAll().c_str() );

			// 最終ログイン抽出
			CString span = div_listContents.getNode(L"div", L"class=heading").getNode(L"span").getTextAll().c_str();
			CString date;
			if (util::GetBetweenSubString(span, L"(", L")", date)>=0) {
				mixi.SetDate(date);
			}

			// URL に応じてアクセス種別を設定する
			mixi.SetAccessType( util::EstimateAccessTypeByUrl( url ) );

			// 自己紹介
			CString description = div_listContents.getNode(L"p", L"class=description").getTextAll().c_str();
			ParserUtil::AddBodyWithExtract( mixi, description );


			out_.push_back( mixi );
		}

		// 「次」、「前」のリンク
		{
			CMixiData backLink;
			CMixiData nextLink;

			const xml2stl::Node& div_pageNavigation01 = div_bookmarkList.getNode( L"div", L"class=pageNavigation01 top" );
			const xml2stl::Node& ul = div_pageNavigation01.getNode(L"div", L"class=pageList02").getNode(L"ul");
			for (size_t i=0; i<ul.getChildrenCount(); i++) {
				const xml2stl::Node& li = ul.getNode(i);
				if (li.getName()!=L"li") {
					continue;
				}

				try {
					const xml2stl::Node& a = li.getNode(L"a");
					CString title = a.getTextAll().c_str();
					wprintf( L"**** title : %s\n", title );
					if (title.Find(L"前")>=0) {
						backLink.SetTitle(title);
						backLink.SetName(title);
						backLink.SetURL(a.getProperty(L"href").c_str());
						backLink.SetAccessType( util::EstimateAccessTypeByUrl(backLink.GetURL()) );
					}
					if (title.Find(L"次")>=0) {
						nextLink.SetTitle(title);
						nextLink.SetName(title);
						nextLink.SetURL(a.getProperty(L"href").c_str());
						nextLink.SetAccessType( util::EstimateAccessTypeByUrl(nextLink.GetURL()) );
					}
				} catch (...) {
				}
			}

			// 前、次のリンクがあれば、追加する。
			if( !backLink.GetTitle().IsEmpty() ) {
				out_.insert( out_.begin(), backLink );
			}
			if( !nextLink.GetTitle().IsEmpty() ) {
				out_.push_back( nextLink );
			}
		}

	} catch (xml2stl::NodeNotFoundException& e) {
		MZ3LOGGER_ERROR( e.getMessage().c_str() );
	}

	MZ3LOGGER_DEBUG( L"ListBookmarkParser.parse() finished." );
	return true;
}

};