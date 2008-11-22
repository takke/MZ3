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

/// MZ3�pHTML�p�[�T
namespace mz3parser {

/// ���X�g�nHTML�̉��
bool MyDoParseMixiListHtml( ACCESS_TYPE aType, CMixiData& parent, CMixiDataList& body, CHtmlArray& html );
void MyDoParseMixiHtml( ACCESS_TYPE aType, CMixiData& mixi, CHtmlArray& html );

}//namespace mz3parser


//--- �ȉ��͖{���͎����̂� cpp �ɋL�q�������B�B�B

/// mixi �pHTML�p�[�T
namespace mixi {

/// �e��p�[�T�̊�{�N���X
class MZ3ParserBase 
{
public:
	static bool ExtractLinks( CMixiData& data_ );

	/**
	 * ���O�A�E�g���������`�F�b�N����
	 */
	static bool IsLogout( LPCTSTR szHtmlFilename )
	{
		// �ő�� N �s�ڂ܂Ń`�F�b�N����
		const int CHECK_LINE_NUM_MAX = 1000;

		FILE* fp = _wfopen(szHtmlFilename, _T("r"));
		if( fp == NULL ) {
			// �擾���s
			return false;
		}

		TCHAR buf[4096];
		for( int i=0; i<CHECK_LINE_NUM_MAX && fgetws(buf, 4096, fp) != NULL; i++ ) {
			// <form action="/login.pl" method="post">
			// ������΃��O�A�E�g��ԂƔ��肷��B
			if (util::LineHasStringsNoCase( buf, L"<form", L"action=", L"login.pl" )) {
				// ���O�A�E�g���
				fclose( fp );
				return true;
			}

			// API �Ή��i�������j
			if (i==0 && util::LineHasStringsNoCase( buf, L"WSSE�ɂ��F�؂��K�v�ł�" )) {
				fclose( fp );
				return true;
			}
		}
		fclose(fp);

		// �����ɂ̓f�[�^���Ȃ������̂Ń��O�A�E�g�Ƃ͔��f���Ȃ�
		return false;
	}
};

/// list �n�y�[�W�ɑ΂���p�[�T�̊�{�N���X
class MixiListParser : public MZ3ParserBase
{
protected:
	/**
	 * ���K�\�� reg �� HTML �\�[�X str ����͂��A�u���ցv�u�O�ցv�̃����N���擾���A�ݒ肷��B
	 *
	 * <ul>
	 * <li>���K�\�� reg �̎d�l
	 *  <ul>��͐������� 3 �v�f�ł��邱�Ɓi"()"���Q�܂ނ��Ɓj�B
	 *  <li>��͌���[1] ��URL�ł��邱�ƁB
	 *  <li>��͌���[2] �̓����N������ł��邱�ƁB�u���v�܂��́u�O�v���܂ނ��ƁB
	 *  </ul>
	 * <li>��͐������̓����N������ɉ����āAnextLink �܂��� backLink �𐶐�����B
	 * <li>�^�C�g���Ɩ��O�� "<< %s >>" ��ݒ肷��B%s �͉�͌���[2]�𖄂ߍ��ށB
	 * <li>�A�N�Z�X��ʂ� accessType ��ݒ肷��B
	 * </ul>
	 *
	 * @return �������� true�A���s���� false
	 */
	static bool parseNextBackLinkBase( CMixiData& nextLink, CMixiData& backLink, CString str, 
									   MyRegex& reg, LPCTSTR strUrlHead, ACCESS_TYPE accessType )
	{
		bool bFound = false;
		for(;;) {
			// �T��
			if( reg.exec(str) == false || reg.results.size() != 3 ) {
				// �������B
				break;
			}else{
				// �����B
				CString s( reg.results[2].str.c_str() );
				if( s.Find( L"�O" ) != -1 ) {
					// �u�O�v
					s.Format( L"%s%s", strUrlHead, reg.results[1].str.c_str() );
					backLink.SetURL( s );
					s.Format( L"<< %s >>", reg.results[2].str.c_str() );
					backLink.SetTitle( s );
					backLink.SetName( s );
					backLink.SetAccessType( accessType );
					bFound = true;
				}else if( s.Find( L"��" ) != -1 ) {
					// �u���v
					s.Format( L"%s%s", strUrlHead, reg.results[1].str.c_str() );
					nextLink.SetURL( s );
					s.Format( L"<< %s >>", reg.results[2].str.c_str() );
					nextLink.SetTitle( s );
					nextLink.SetName( s );
					nextLink.SetAccessType( accessType );
					bFound = true;
				}
				str = str.Mid( reg.results[0].end );
			}
		}
		return bFound;
	}

};

/// contents �n�y�[�W�ɑ΂���p�[�T�̊�{�N���X
class MixiContentParser : public MZ3ParserBase
{
public:
	/**
	 * �L�����e�pURL���̎擾
	 */
	static void parsePostURL(int startIndex, CMixiData& data_, const CHtmlArray& html_)
	{
		const int lastLine = html_.GetCount();

		for (int i=startIndex; i<lastLine; i++) {
			const CString& line = html_.GetAt(i);

			// <form name="bbs_comment_form" action="add_bbs_comment.pl?id=xxx&comm_id=yyy" enctype="multipart/form-data" method="post">
			if (line.Find(L"<form") != -1 &&
				line.Find(L"method=") != -1 &&
				line.Find(L"action=") != -1 &&
				line.Find(L"post") != -1) 
			{
				// delete_xxx.pl �̏ꍇ�͖���
				if (line.Find(L"delete_") != -1) {
					continue;
				}

				// Content-Type/enctype ����
				if (line.Find(_T("multipart")) != -1) {
					data_.SetContentType(CONTENT_TYPE_MULTIPART);
				} else {
					data_.SetContentType(CONTENT_TYPE_FORM_URLENCODED);
				}

				// action/URL �擾
				CString action;
				if( util::GetBetweenSubString( line, L"action=\"", L"\"", action ) < 0 ) {
					continue;
				}
				data_.SetPostAddress( action );

				switch(data_.GetAccessType()) {
				case ACCESS_DIARY:
				case ACCESS_MYDIARY:
				case ACCESS_NEIGHBORDIARY:
					break;
				default:
					// �ȍ~�̉�͕͂s�v�B
					return;
				}
//				continue;
			}
			
			// <input type="hidden" name="owner_id" value="yyy" />
			if (line.Find(L"<input") != -1 &&
				line.Find(L"hidden") != -1 &&
				line.Find(L"owner_id") != -1)
			{
				CString ownerId;
				if( util::GetBetweenSubString( line, L"value=\"", L"\"", ownerId ) < 0 ) {
					continue;
				}
				MZ3LOGGER_DEBUG( _T("owner_id = ") + ownerId );
				data_.SetOwnerID( _wtoi(ownerId) );
			}

		}
	}

	/**
	 * �y�[�W�ύX�����N�̒��o�B
	 */
	static bool parsePageLink( CMixiData& data, const CHtmlArray& html, int startIndex=180 )
	{
		// startIndex �ȍ~�ɉ��L�𔭌�������A��͊J�n�B
/*
<div class="pageNavigation01">
<div class="pageList01">
<div>[<ul><li><a href="view_bbs.pl?page=1&comm_id=xxx&id=yyy">1</a></li><li><a href="view_bbs.pl?page=2&comm_id=xxx&id=23332197">2</a></li><li><a href="view_bbs.pl?page=3&comm_id=xxx&id=23332197">3</a></li><li><a href="view_bbs.pl?page=4&comm_id=xxx&id=23332197">4</a></li><li><a href="view_bbs.pl?page=5&comm_id=xxx&id=23332197">5</a></li><li><a href="view_bbs.pl?page=6&comm_id=xxx&id=23332197">6</a></li></ul>]</div>
</div>
<div class="pageList02">
<ul>
<li><a href="view_bbs.pl?id=23332197&comm_id=xxx&page=all">�S�Ă�\��</a></li>
<li>�ŐV��20����\��</li>
</ul>
</div>
</div>
*/
		const int count = html.GetCount();
		int i=startIndex;

		// pageList01
		for( ; i<count; i++ ) {
			const CString& line = html.GetAt(i);
			if( util::LineHasStringsNoCase( line, L"<div", L"class", L"pageList01" ) ) {
				break;
			}
		}
		if( i >= count ) {
			return false;
		}

		// </div> ��������܂Ńp�^�[���}�b�`���J��Ԃ��B
		for( i=i+1; i<count; i++ ) {
			const CString& line = html.GetAt(i);

			CString str = line;
			ParserUtil::ExtractURI( str, data.m_linkPage );

			if( util::LineHasStringsNoCase( line, L"</div>" ) ) {
				break;
			}
		}

		// pageList02
		for( ; i<count; i++ ) {
			const CString& line = html.GetAt(i);
			if( util::LineHasStringsNoCase( line, L"<div", L"class", L"pageList02" ) ) {
				break;
			}
		}
		if( i >= count ) {
			return false;
		}

		// </div> ��������܂Ńp�^�[���}�b�`���J��Ԃ��B
		for( i=i+1; i<count; i++ ) {
			const CString& line = html.GetAt(i);

			CString str = line;
			ParserUtil::ExtractURI( str, data.m_linkPage );

			if( util::LineHasStringsNoCase( line, L"</div>" ) ) {
				break;
			}
		}


		return !data.m_linkPage.empty();
	}

	/**
	 * �u�ŐV�̃g�s�b�N�v�̒��o�B
	 */
	static bool parseRecentTopics( CMixiData& data, const CHtmlArray& html, int startIndex=200 )
	{
		// startIndex �ȍ~�ɉ��L�𔭌�������A��͊J�n�B
		// <ul class="newTopicList01">

		const int count = html.GetCount();
		int i=startIndex;
		for( ; i<count; i++ ) {
			const CString& line = html.GetAt(i);
			if( util::LineHasStringsNoCase( line, L"<ul", L"class", L"newTopicList01" ) ) {
				break;
			}
		}
		if( i >= count ) {
			return false;
		}

		// �I���^�O�𔭌�����܂Ńp�^�[���}�b�`���J��Ԃ��B
		for( i=i+1; i<count; i++ ) {
			const CString& line = html.GetAt(i);
			if( util::LineHasStringsNoCase( line, L"<p", L"class", L"utilityLinks03" ) ) {
				break;
			}

			CString str = line;
			if( line.Find( L"view_enquete.pl" ) != -1 ||
				line.Find( L"view_event.pl" ) != -1 ||
				line.Find( L"view_bbs.pl" ) != -1 )
			{
				ParserUtil::ExtractURI( str, data.m_linkPage );
			}
		}

		return !data.m_linkPage.empty();
	}

	/**
	 * �R�~���j�e�B�����o
	 */
	static bool parseCommunityName( CMixiData& mixi, const CHtmlArray& html_ )
	{
		// <p class="utilityLinks03"><a href="view_community.pl?id=1198460">[MZ3 -Mixi for ZERO3-] �R�~���j�e�B�g�b�v��</a></p>
		const int lastLine = html_.GetCount();
		for (int i=0; i<lastLine; i++) {
			const CString& line = html_.GetAt(i);

			if (util::LineHasStringsNoCase( line, L"<p", L"utilityLinks03", L"view_community.pl" )) {
				CString name;
				if (util::GetBetweenSubString( line, L"<a", L"</a>", name ) >= 0) {
					if (util::GetBetweenSubString( name, L"[", L"]", name ) >= 0) {
						mixi::ParserUtil::ReplaceEntityReferenceToCharacter( name );
						mixi.SetName( name );
						return true;
					}
				}
			}
		}
		return false;
	}

};

//���������ʁ�����

/**
 * [content] home.pl ���O�C����̃��C����ʗp�p�[�T
 * �y���C���g�b�v��ʁz
 * http://mixi.jp/home.pl
 */
class HomeParser : public MixiContentParser
{
public:
	/**
	 * ���O�C������
	 *
	 * ���O�C�������������ǂ����𔻒�
	 *
	 * @return ���O�C���������͎���URL�A���s���͋�̕������Ԃ�
	 */
	static bool IsLoginSucceeded( const CHtmlArray& html )
	{
		INT_PTR count = html.GetCount();

		for (int i=0; i<count; i++) {
			const CString& line = html.GetAt(i);

			if (util::LineHasStringsNoCase( line, L"refresh", L"check.pl" )) {
				// <html><head><meta http-equiv="refresh" content="0;url=/check.pl?n=%2Fhome.pl"></head></html>
				return true;
			}
			// <title>[mixi]</title>
			if (util::LineHasStringsNoCase(line, L"<title>[mixi]</title>")) {
				return true;
			}
		}

		return false;
	}

	/**
	 * ���C���y�[�W����̏��擾�B
	 *
	 * ���L�̕ϐ��ɏ����i�[����B
	 * <ul>
	 * <li>theApp.m_loginMng
	 * <li>theApp.m_newMessageCount
	 * <li>theApp.m_newCommentCount
	 * <li>theApp.m_newApplyCount
	 * </ul>
	 */
	static bool parse( const CHtmlArray& html_ )
	{
		MZ3LOGGER_DEBUG( L"HomeParser.parse() start." );

		INT_PTR count = html_.GetCount();

		int index = 0;

		// �V�����b�Z�[�W���̎擾
		int messageNum = GetNewMessageCount( html_, 350, count, index);
		if (messageNum != 0) {
			theApp.m_newMessageCount = messageNum;

			// �o�C�u�����Ⴄ
			// NLED_SETTINGS_INFO led;
			//led.LedNum = ::NLedSetDevice(0, 
		}

		// �V���R�����g���̎擾
		int commentNum = GetNewCommentCount( html_, 350, count, index);
		if (commentNum != 0) {
			theApp.m_newCommentCount = commentNum;
		}

		// ���F�҂����̎擾
		int applyNum = GetNewAcknowledgmentCount( html_, 350, count, index);
		if (applyNum != 0) {
			theApp.m_newApplyCount = applyNum;
		}

		if (wcslen(theApp.m_loginMng.GetOwnerID()) == 0) {
			// OwnerID �����擾�Ȃ̂ŉ�͂���
			MZ3LOGGER_DEBUG( L"OwnerID �����擾�Ȃ̂ŉ�͂��܂�" );

			for (int i=0; i<count; i++) {
				const CString& line = html_.GetAt(i);
				if( util::LineHasStringsNoCase( line, L"<a", L"href=", L"add_diary.pl?id=" ) ) {

					// add_diary.pl �ȍ~�𒊏o
					CString after;
					util::GetAfterSubString( line, L"add_diary.pl", after );

					CString id;
					if( util::GetBetweenSubString( after, L"id=", L"\"", id ) == -1 ) {
						MZ3LOGGER_ERROR( L"add_diary.pl �̈����� id �w�肪����܂���B line[" + line + L"], after[" + after + L"]" );
					}else{
						MZ3LOGGER_DEBUG( L"OwnerID = " + id );
						theApp.m_loginMng.SetOwnerID(id);
						theApp.m_loginMng.Write();
					}
					break;
				}
			}

			if (wcslen(theApp.m_loginMng.GetOwnerID()) == 0) {
				MZ3LOGGER_ERROR( L"OwnerID ���擾�ł��܂���ł���" );
			}
		}

		MZ3LOGGER_DEBUG( L"HomeParser.parse() finished." );
		return true;
	}

private:
	/**
	 * �V�����b�Z�[�W���̉�́A�擾
	 */
	static int GetNewMessageCount( const CHtmlArray& html, int sLine, int eLine, int& retIndex )
	{
		CString msg = _T("�V�����b�Z�[�W��");

		int messageNum = 0;

		for (int i=sLine; i<eLine; i++) {
			const CString& line = html.GetAt(i);

			int pos;
			if ((pos = line.Find(msg)) != -1) {
				// �V�����b�Z�[�W����
				// �s�v�������폜
				CString buf = line.Mid(pos + msg.GetLength());

				pos = buf.Find(_T("��"));
				// ����������폜
				buf = buf.Mid(0, pos);

				TRACE(_T("���b�Z�[�W���� = %s\n"), buf);

				messageNum = _wtoi(buf);
			}
			else if (line.Find(_T("<div id=\"bodySide\">")) != -1) {
				retIndex = i;
				break;
			}
		}

		return messageNum;
	}

	/**
	 * �V���R�����g���̉�́A�擾
	 *
	 * @todo �{���͂��̃��\�b�h���ŁA�R�����g�̃����N���擾���ׂ��B
	 *       ���L���E�R�����g���ɑ΂��Ăǂ�� HTML �ɂȂ�̂��悭������Ȃ��̂ŕۗ��B
	 */
	static int GetNewCommentCount( const CHtmlArray& html, int sLine, int eLine, int& retIndex)
	{
	/* �Ώە�����i���s�Ȃ��j
	<td><font COLOR=#605048> <font COLOR=#CC9933>�E</font> <font color=red><b>1���̓��L�ɑ΂��ĐV���R�����g������܂��I</b></font></td>
	*/
		CString msg = _T("�V���R�����g��");

		int commentNum = 0;

		for (int i=sLine; i<eLine; i++) {
			const CString& line = html.GetAt(i);

			int pos;
			if ((pos = line.Find(msg)) != -1) {
				// �V���R�����g����

				// msg �̌�둤���폜�B
				CString str = line.Left( pos );

				// "<b>" �� "��" �Ɉ͂܂ꂽ������������A�����Ƃ���B
				CString result;
				util::GetBetweenSubString( str, L"<b>", L"��", result );

				TRACE(_T("�R�����g�� = %s\n"), result);

				commentNum = _wtoi(result);
			}
			else if (line.Find(_T("<div id=\"bodySide\">")) != -1) {
				retIndex = i;
				break;
			}
		}

		return commentNum;
	}

	/**
	 * ���F�҂����̉�́A�擾
	 */
	static int GetNewAcknowledgmentCount( const CHtmlArray& html, int sLine, int eLine, int& retIndex )
	{
		CString msg = _T("���F�҂��̗F�l��");

		int applyNum = 0;

		for (int i=sLine; i<eLine; i++) {
			const CString& line = html.GetAt(i);

			int pos;
			if ((pos = line.Find(msg)) != -1) {
				// ���F�҂�����
				// �s�v�������폜
				CString buf = line.Mid(pos + msg.GetLength());

				pos = buf.Find(_T("��"));
				// ����������폜
				buf = buf.Mid(0, pos);

				TRACE(_T("���F�҂����� = %s\n"), buf);

				applyNum = _wtoi(buf);
			}
			else if (line.Find(_T("<div id=\"bodySide\">")) != -1) {
				retIndex = i;
				break;
			}
		}

		return applyNum;
	}
};


/**
 * �摜�_�E�����[�hCGI �p�p�[�T
 *
 * show_diary_picture.pl
 * show_bbs_comment_picture.pl
 */
class ShowPictureParser : public MZ3ParserBase
{
public:

	/**
	 * �摜URL�擾
	 */
	static CString GetImageURL( const CHtmlArray& html )
	{
		INT_PTR count = html.GetCount();

		/* 
		 * ��͑Ώە�����F
		 * <img SRC="http://ic76.mixi.jp/p/xxx/xxx/diary/xx/x/xxx.jpg" BORDER=0>
		 */
		CString uri;
		for (int i=0; i<count; i++) {
			// �摜�ւ̃����N�𒊏o
			const CString& line = html.GetAt(i);
			if( util::LineHasStringsNoCase( line, L"<img src=\"", L"\"" ) ) {
				// " ���� " �܂ł��擾����B
				if( util::GetBetweenSubString( line, L"\"", L"\"", uri ) > 0 ) {
					MZ3LOGGER_DEBUG( L"�摜�ւ̃����N���oOK, url[" + uri + L"]" );
					break;
				}
			}
		}
		return uri;
	}
};

}//namespace mixi

namespace mz3parser {

/**
 * [content] RSS �p�[�T
 */
class RssFeedParser : public mixi::MixiContentParser
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
class RssAutoDiscoveryParser : public mixi::MixiContentParser
{
public:
	static bool parse( CMixiDataList& out_, const std::vector<TCHAR>& text_ );
	static bool parseLinkRecursive( CMixiDataList& out_, const xml2stl::Node& node );
};

}//namespace mz3parser
