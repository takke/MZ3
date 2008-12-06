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

/// MZ3�pXML/HTML�p�[�T
namespace parser {

/// ���X�g�nHTML�̉��
bool MyDoParseMixiListHtml( ACCESS_TYPE aType, CMixiData& parent, CMixiDataList& body, CHtmlArray& html );
void MyDoParseMixiHtml( ACCESS_TYPE aType, CMixiData& mixi, CHtmlArray& html );

/// �e��p�[�T�̊�{�N���X
class MZ3ParserBase 
{
public:
	static bool ExtractLinks( CMixiData& data_ );
};

/// �e��R���e���c�p�p�[�T�̊�{�N���X
class MZ3ContentParser : MZ3ParserBase
{
};

/**
 * [content] RSS �p�[�T
 */
class RssFeedParser : public MZ3ContentParser
{
public:
	static bool parse( CMixiDataList& out_, const CHtmlArray& html_ ) {
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
	static bool parse( CMixiData& mixi, const CHtmlArray& html_ )
	{
		MZ3LOGGER_DEBUG( L"ErrorlogParser.parse() start." );

		mixi.ClearAllList();
		INT_PTR count = html_.GetCount();

		int iLine = 0;

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
		for( int iLine=0; iLine<count; iLine++ ) {
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

};


/**
 * [content] �ėpURL �p�p�[�T
 * �y�f�o�b�O�p�z
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
