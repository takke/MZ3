/*
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */
#pragma once

#include "MyRegex.h"
#include "HtmlArray.h"
#include "MixiParserUtil.h"
#include "xml2stl.h"

/// MZ3用XML/HTMLパーサ
namespace parser {

/// リスト系HTMLの解析
bool MyDoParseMixiListHtml( ACCESS_TYPE aType, CMixiData& parent, CMixiDataList& body, CHtmlArray& html );
void MyDoParseMixiHtml( ACCESS_TYPE aType, CMixiData& mixi, CHtmlArray& html );

/// 各種パーサの基本クラス
class MZ3ParserBase 
{
public:
	static bool ExtractLinks( CMixiData& data_ );
};

/// 各種コンテンツ用パーサの基本クラス
class MZ3ContentParser : MZ3ParserBase
{
};

/**
 * [content] RSS パーサ
 */
class RssFeedParser : public MZ3ContentParser
{
public:
	static bool parse( CMixiDataList& out_, const CHtmlArray& html_ ) {
		// html_ の文字列化
		std::vector<TCHAR> text;
		html_.TranslateToVectorBuffer( text );

		return RssFeedParser::parse( out_, text );
	}
	static bool parse( CMixiDataList& out_, const std::vector<TCHAR>& text_, CString* pStrTitle=NULL );
	static void setDescriptionTitle( CMixiData& data, CString description, CString title );
};

/**
 * [content] RSS AutoDiscovery パーサ
 */
class RssAutoDiscoveryParser : public MZ3ContentParser
{
public:
	static bool parse( CMixiDataList& out_, const std::vector<TCHAR>& text_ );
	static bool parseLinkRecursive( CMixiDataList& out_, const xml2stl::Node& node );
};

//■■■MZ3独自■■■
/**
 * [content] Readme.txt 用パーサ
 * 【MZ3ヘルプ用】
 */
class HelpParser : public MZ3ContentParser
{
public:
	static bool parse( CMixiData& mixi, const CHtmlArray& html_ );
};

/**
 * [content] mz3log.txt 用パーサ
 * 【MZ3ヘルプ用】
 */
class ErrorlogParser : public MZ3ContentParser
{
public:
	static bool parse( CMixiData& mixi, const CHtmlArray& html_ )
	{
		MZ3LOGGER_DEBUG( L"ErrorlogParser.parse() start." );

		mixi.ClearAllList();
		INT_PTR count = html_.GetCount();

		int iLine = 0;

		int status = 0;		// 0 : start, 1 : 最初の項目, 2 : 2番目以降の項目解析中
		CMixiData child;

		mixi.SetAuthor( MZ3_APP_NAME );
		mixi.AddBody( L"the top element.\r\n" );

		CString msg;
		msg.Format( L"mz3log.txt has %d line(s).", count );
		mixi.AddBody( msg );

		// 【解析対象概要】
		// ---
		// [2007/07/13△12:30:03]△DEBUG△本文△[.\InetAccess.cpp:555]
		// ---
		// または
		// ---
		// [2007/07/13△12:06:33]△DEBUG△本文
		// ずっと本文
		// ずっと本文△[.\MZ3View.cpp:947]
		// ---
		// という形式。
		for( int iLine=0; iLine<count; iLine++ ) {
			CString target = html_.GetAt(iLine);
			target.Replace(_T("\n"), _T("\r\n"));	// 改行コード変換

			child.ClearBody();
			child.SetCommentIndex( iLine+1 );

			// "[～]" を日付に。
			CString date;
			int index = 0;
			index = util::GetBetweenSubString( target, L"[", L"]", date );
			if( index == -1 ) {
				child.AddBody( L"★ '[～]' が見つからないのでスキップ [" + target + L"]" );
				mixi.AddChild( child );
				continue;
			}
			child.SetDate( date );

			// "]" 以降を切り出し
			target = target.Mid( index );

			// "△～△" をエラーレベル（名前）に。
			CString level;
			index = util::GetBetweenSubString( target, L" ", L" ", level );
			if( index == -1 ) {
				// "△～△" が見つからないのでスキップ
				child.AddBody( L"★ '△～△' が見つからないのでスキップ [" + target + L"]" );
				mixi.AddChild( child );
				continue;
			}

			// 2つ目の"△"以降を切り出し
			target = target.Mid( index );

			// 名前に「レベル△本文」を設定
			level += L" ";
			level += target;
			level = level.Left( 30 );
			child.SetAuthor( level );

			// 末尾が "]" なら本文に追加して終了。
			// 末尾が "]" 以外なら次の行以降を見つかるまで本文として解析。
			child.AddBody( L"\r\n" );
			child.AddBody( target );

			if( target.Right( 3 ) == L"]\r\n" ) {
				// 終了
			}else{
				// 末尾に "]" が現れるまで解析して終了。
				iLine ++;
				for( ;iLine<count; iLine++ ) {
					target = html_.GetAt( iLine );
					target.Replace(_T("\n"), _T("\r\n"));	// 改行コード変換
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

};


/**
 * [content] 汎用URL 用パーサ
 * 【デバッグ用】
 */
class PlainTextParser : public MZ3ContentParser
{
public:
	static bool parse( CMixiData& mixi, const CHtmlArray& html_ )
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

};

}//namespace mz3parser
