/*
 * Copyright (c) MZ3 Project Team All rights reserved.
 * Code licensed under the BSD License:
 * http://www.mz3.jp/license.txt
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
bool MyDoParseMixiHtml( ACCESS_TYPE aType, CMixiData& mixi, CHtmlArray& html );

/// 各種パーサの基本クラス
class MZ3ParserBase 
{
public:
	static bool ExtractLinks( CMixiData& data_ );
};

/// 各種リスト用パーサの基本クラス
class MZ3ListParser : MZ3ParserBase
{
//	virtual bool parse( CMixiDataList& out_, const CHtmlArray& html_ ) = 0;
};

/// 各種コンテンツ用パーサの基本クラス
class MZ3ContentParser : MZ3ParserBase
{
//	virtual bool parse( CMixiData& out_, const CHtmlArray& html_ ) = 0;
};

#ifdef BT_MZ3
/**
 * [list] RSS パーサ
 */
class RssFeedParser : public MZ3ListParser
{
public:
	static bool parse( CMixiDataList& out_, const CHtmlArray& html_ )
	{
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
#endif


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
	static bool parse( CMixiData& mixi, const CHtmlArray& html_ );
};

/**
 * [content] 汎用URL 用パーサ
 * 【デバッグ用】
 */
class PlainTextParser : public MZ3ContentParser
{
public:
	static bool parse( CMixiData& mixi, const CHtmlArray& html_ );
};

}//namespace mz3parser
