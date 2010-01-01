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

/// MZ3�pXML/HTML�p�[�T
namespace parser {

/// ���X�g�nHTML�̉��
bool MyDoParseMixiListHtml( ACCESS_TYPE aType, CMixiData& parent, CMixiDataList& body, CHtmlArray& html );
bool MyDoParseMixiHtml( ACCESS_TYPE aType, CMixiData& mixi, CHtmlArray& html );

/// �e��p�[�T�̊�{�N���X
class MZ3ParserBase 
{
public:
	static bool ExtractLinks( CMixiData& data_ );
};

/// �e�탊�X�g�p�p�[�T�̊�{�N���X
class MZ3ListParser : MZ3ParserBase
{
//	virtual bool parse( CMixiDataList& out_, const CHtmlArray& html_ ) = 0;
};

/// �e��R���e���c�p�p�[�T�̊�{�N���X
class MZ3ContentParser : MZ3ParserBase
{
//	virtual bool parse( CMixiData& out_, const CHtmlArray& html_ ) = 0;
};

#ifdef BT_MZ3
/**
 * [list] RSS �p�[�T
 */
class RssFeedParser : public MZ3ListParser
{
public:
	static bool parse( CMixiDataList& out_, const CHtmlArray& html_ )
	{
		// html_ �̕�����
		std::vector<TCHAR> text;
		html_.TranslateToVectorBuffer( text );

		return RssFeedParser::parse( out_, text );
	}
	static bool parse( CMixiDataList& out_, const std::vector<TCHAR>& text_, CString* pStrTitle=NULL );
	static void setDescriptionTitle( CMixiData& data, CString description, CString title );
};

/**
 * [content] RSS AutoDiscovery �p�[�T
 */
class RssAutoDiscoveryParser : public MZ3ContentParser
{
public:
	static bool parse( CMixiDataList& out_, const std::vector<TCHAR>& text_ );
	static bool parseLinkRecursive( CMixiDataList& out_, const xml2stl::Node& node );
};
#endif


//������MZ3�Ǝ�������

/**
 * [content] Readme.txt �p�p�[�T
 * �yMZ3�w���v�p�z
 */
class HelpParser : public MZ3ContentParser
{
public:
	static bool parse( CMixiData& mixi, const CHtmlArray& html_ );
};

/**
 * [content] mz3log.txt �p�p�[�T
 * �yMZ3�w���v�p�z
 */
class ErrorlogParser : public MZ3ContentParser
{
public:
	static bool parse( CMixiData& mixi, const CHtmlArray& html_ );
};

/**
 * [content] �ėpURL �p�p�[�T
 * �y�f�o�b�O�p�z
 */
class PlainTextParser : public MZ3ContentParser
{
public:
	static bool parse( CMixiData& mixi, const CHtmlArray& html_ );
};

}//namespace mz3parser
