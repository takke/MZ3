/*
 * Copyright (c) MZ3 Project Team All rights reserved.
 * Code licensed under the BSD License:
 * http://www.mz3.jp/license.txt
 */
#pragma once

#include "MZ3Parser.h"
#include "xml2stl.h"

#ifdef BT_MZ3

/// mixi�p�p�[�T
namespace mixi {

/// �e��mixi�p�[�T�̊�{�N���X
class MixiParserBase : public parser::MZ3ParserBase
{
public:
	/**
	 * ���O�A�E�g���������`�F�b�N����
	 */
	static bool IsLogout( LPCTSTR szHtmlFilename );
};


/// list �n�y�[�W�ɑ΂���p�[�T�̊�{�N���X
class MixiListParser : public MixiParserBase
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
class MixiContentParser : public MixiParserBase, public parser::MZ3ContentParser
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
 * �摜�_�E�����[�hCGI �p�p�[�T
 *
 * show_diary_picture.pl
 * show_bbs_comment_picture.pl
 */
class ShowPictureParser : public MixiParserBase
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
};


//���������L������
/**
 * [list] list_comment.pl �p�p�[�T
 * �y�ŋ߂̃R�����g�ꗗ�z
 * http://mixi.jp/list_comment.pl
 */
class ListCommentParser : public MixiListParser
{
public:
	static bool parse( CMixiDataList& out_, const CHtmlArray& html_ )
	{
		MZ3LOGGER_DEBUG( L"ListCommentParser.parse() start." );

		INT_PTR count = html_.GetCount();

		/**
		 * ���j�F
		 * - <dt>2007�N10��02��&nbsp;22:22</dt>
		 *   �̂悤�ȓ��t������΂������獀�ڊJ�n�Ƃ݂Ȃ�
		 *   <dd><a href="view_diary.pl?��������܂ōs���΂�(�ʏ���t�̎��̍s)����{���J�n�B
		 *   �܂�URL��ID�𔲂��o���A
		 * - �I���^�O</dd>��������܂ŁA�s���������āA�Ō�ɖ{���𔲂����B
		 * - �ȍ~�ɁA"/listCommentArea"������΁A�������I������
		 *   
		 */
		int iLine = 180;		// �Ƃ肠�����ǂݔ�΂�
		for( ; iLine<count; iLine++ ) {
			const CString& str = html_.GetAt(iLine);
			//�ŏ��̓����𔭌�������s�������[�v�J�n
			if(util::LineHasStringsNoCase( str, L"<dt>", L"</dt>" ) )  {
				// mixi �f�[�^�̍쐬
				{
					CMixiData data;
					data.SetAccessType(ACCESS_DIARY);

					//�����̒��o
					//<dt>2007�N10��02��&nbsp;22:22</dt>
					ParserUtil::ParseDate(str, data);
					
					// ���o��
					//<dd><a href="view_diary.pl?
					{
						//<dd><a href="view_diary.pl?�s��������܂�
						for( iLine; iLine<count; iLine++ ) {
							bool findFlag = false;
							const CString& line = html_.GetAt(iLine);
							//�R�����g�J�n�s�𔭌�������
							if( util::LineHasStringsNoCase( line, L"<dd>", L"view_diary") )
							{
								// �t�q�h
								CString url;
								util::GetBetweenSubString( line, L"href=\"", L"\">", url );
								data.SetURL( url );
								
								// �h�c��ݒ�
								data.SetID( MixiUrlParser::GetID(url) );

								// �{���v���r���[
								CString target = L"";
								for( iLine; iLine<count; iLine++ ) {
									const CString& line2 = html_.GetAt(iLine);
									//</dd>�����������玟�̃R�����g��
									if( util::LineHasStringsNoCase( line2, L"</dd>") )
									{
										target += line2;
										//�{���𒊏o
										if( util::LineHasStringsNoCase( target, L"\">", L"</a>") )
										{
											CString title;
											util::GetBetweenSubString( target, L"\">", L"</a>", title );
											//���s���폜
											title.Replace(_T("\n"), _T(""));
											mixi::ParserUtil::ReplaceEntityReferenceToCharacter( title );
											//�^�C�g�����Z�b�g
											data.SetTitle(title);
											//���[�v�𔲂���t���O���Z�b�g
											findFlag = true;
										}
										//���O�𒊏o
										CString author;
										util::GetBetweenSubString( target, L"&nbsp;(", L")", author );
										mixi::ParserUtil::ReplaceEntityReferenceToCharacter( author );
										data.SetName( author );
										data.SetAuthor( author );
										break;
									} else {
										//</dd>�������܂Ō���
										target += line2;
									}
								}
							}
							//�t���O���������烋�[�v�𔲂���
							if(findFlag == true) 
							{
								break;
							}
						}
					}
					out_.push_back(data);
				}
			}

			if( str.Find(_T("/listCommentArea")) != -1 ) {
				// �I���^�O����
				break;
			}
		}

		MZ3LOGGER_DEBUG( L"ListCommentParser.parse() finished." );
		return true;
	}
};


//�������R�~���j�e�B������


/**
 * [list] list_community.pl �p�p�[�T�B
 * �y�R�~���j�e�B�ꗗ�z
 * http://mixi.jp/list_community.pl
 */
class ListCommunityParser : public MixiListParser
{
public:
	static bool parse( CMixiDataList& out_, const CHtmlArray& html_ )
	{
		MZ3LOGGER_DEBUG( L"ListCommunityParser.parse() start." );

		INT_PTR count = html_.GetCount();

		// �u���v�A�u�O�v�̃����N
		CMixiData backLink;
		CMixiData nextLink;

		/**
		 * ���j�F
		 * �� <ul>��5��������ł���B
		 *   ��s�Ƀ����N���摜���A�����s�ɖ��O������ł���B
		 *   �]���āA�u5�����̃����N���o�v�u5�����̖��O���o�v�Ƃ����菇�ōs���B
		 *
		 * ��iconList��������΁A�������獀�ڊJ�n�Ƃ݂Ȃ��B/messageArea�������܂ňȉ������s����B
		 *   (1) view_community.pl��������܂ł̊e�s���p�[�X���A����̌`���Ɉ�v���Ă���΁AURL�Ɖ摜�A�R�~�������擾����B
		 *   (2) ���o�����f�[�^�� out_ �ɒǉ�����B
		 */
		// ���ڊJ�n��T��
		bool bInItems = false;	// ���ڊJ�n�H
		int iLine = 160;		// �Ƃ肠�����ǂݔ�΂�
		for( ; iLine<count; iLine++ ) {
			const CString& line = html_.GetAt(iLine);

			if( !bInItems ) {
				// �u����\���v�A�u�O��\���v�̃����N�𒊏o����
				if( parseNextBackLink( nextLink, backLink, line ) ) {
					// ���o�ł�����I�������B
				}

				// ���ڊJ�n�H
				if( util::LineHasStringsNoCase( line, L"iconList") ) {
					bInItems = true;
				}
			}

			if( bInItems ) {
				// 5�����擾���ǉ�
				if(! parseTwoTR( out_, html_, iLine ) ) {
					// ��̓G���[
					break;
				}

				// pageNavigation ��������ΏI��
				const CString& line = html_.GetAt(iLine);
				if( util::LineHasStringsNoCase( line, L"pageNavigation" ) ) {
					break;
				}
			}
		}

		// �O�A���̃����N������΁A�ǉ�����B
		if( !backLink.GetTitle().IsEmpty() ) {
			out_.insert( out_.begin(), backLink );
		}
		if( !nextLink.GetTitle().IsEmpty() ) {
			out_.push_back( nextLink );
		}

		MZ3LOGGER_DEBUG( L"ListCommunityParser.parse() finished." );
		return true;
	}

private:
	/**
	 * ���e�𒊏o����
	 *
	 * (1) /messageArea �������܂ł̊e�s���p�[�X���A��URL�A���O�𐶐�����B
	 * (2) mixi_list �ɒǉ�����B
	 */
	static bool parseTwoTR( CMixiDataList& mixi_list, const CHtmlArray& html, int& iLine )
	{
		const int lastLine = html.GetCount();

		bool bBreak = false;
		for( ; iLine < lastLine && bBreak == false; iLine++ ) {
			const CString& line = html.GetAt( iLine );

			// "/messageArea"��������ΏI��
			if( util::LineHasStringsNoCase( line, L"/messageArea" ) ) {
				return false;
			}

			// view_community.pl �Ŏn�܂�Ȃ�A���o����
			if( util::LineHasStringsNoCase( line, L"view_community.pl" ) ) {
				/* �s�̌`���F
				line1: <div class="iconListImage"><a href="view_community.pl?id=2640122" style="background: url(http://img-c3.mixi.jp/photo/comm/1/22/2640122_234s.jpg); text-indent: -9999px;" class="iconTitle" title="xxxxxx">xxxxxx�̎ʐ^</a></div><span>xxxxxx(41)</span>
				line2: <div id="2640122" class="memo_pop"></div><p><a href="show_community_memo.pl?id=2640122" onClick="openMemo(event,'community',2640122);return false;"><img src="http://img.mixi.jp/img/basic/icon/memo001.gif" width="12" height="14" /></a></p>
				*/
				CMixiData mixi;

				// <a �ȍ~�݂̂ɂ���
				CString target;
				if( util::GetAfterSubString( line, L"<a", target ) < 0 ) {
					// <a ���Ȃ������̂Ŏ��̍s��͂ցB
					continue;
				}

				// URL ���o
				CString url;
				if( util::GetBetweenSubString( target, L"href=\"", L"\"", url ) < 0 ) {
					continue;
				}
				mixi.SetURL( url );

				// URL �\�z���ݒ�
				url.Insert( 0, L"http://mixi.jp/" );
				mixi.SetBrowseUri( url );

				// Image ���o
				CString image_url;
				if( util::GetBetweenSubString( target, L"url(", L"); text-indent", image_url ) < 0 ) {
					continue;
				}
				mixi.AddImage( image_url );

				// <span>��</span>�̊Ԃ̃R�~�����𒊏o
				CString name;
				if( util::GetBetweenSubString( target, L"<span>", L"</span>", name ) < 0 ) {
					continue;
				}
				// �R�~�����Ɛl���ɕ���
				CString userCount;
				int idxStart = name.ReverseFind( '(' );
				int idxEnd   = idxStart>0 ? (name.Find(')', idxStart+1)) : -1;
				if (idxStart > 0 && idxEnd > 0 && idxEnd-idxStart-1>0) {
					userCount = name.Mid(idxStart+1, idxEnd-idxStart-1) + L"�l";
					name = name.Left(idxStart);
				}
				mixi::ParserUtil::ReplaceEntityReferenceToCharacter( name );
				mixi.SetName( name );
				mixi.SetDate( userCount );	// ���ɓ��t�Ƃ��ēo�^����
				mixi.SetAccessType( ACCESS_COMMUNITY );

				// mixi_list �ɒǉ�����B
				mixi_list.push_back( mixi );

			}
		}
		if( iLine >= lastLine ) {
			return false;
		}
		return true;
	}

	/// �u����\���v�A�u�O��\���v�̃����N�𒊏o����
	/// <td align=right>1���`50����\��&nbsp;&nbsp;<a href=list_community.pl?page=2&id=xxx>����\��</a></td>
	static bool parseNextBackLink( CMixiData& nextLink, CMixiData& backLink, CString str )
	{
		// ���K�\���̃R���p�C���i���̂݁j
		static MyRegex reg;
		if( !util::CompileRegex( reg, L"<a href=[\"]?list_community.pl([?]page=[^\"^>]+)[\"]?>([^<]+)</a>" ) ) {
			MZ3LOGGER_FATAL( FAILED_TO_COMPILE_REGEX_MSG );
			return false;
		}

		return parseNextBackLinkBase( nextLink, backLink, str,
					reg, L"list_community.pl", ACCESS_LIST_COMMUNITY );
	}
};


/**
 * [list] list_comment.pl �p�p�[�T
 * �y�R�~���j�e�B�[�̃g�s�b�N�ꗗ�z
 * http://mixi.jp/list_bbs.pl
 */
class ListBbsParser : public MixiListParser
{
public:
	static bool parse( CMixiDataList& out_, const CHtmlArray& html_ )
	{
		MZ3LOGGER_DEBUG( L"ListBbsParser.parse() start." );

		INT_PTR count = html_.GetCount();

		// �u���v�A�u�O�v�̃����N
		CMixiData backLink;
		CMixiData nextLink;

		/**
		 * ���j�F
		 * - ���L�̃��W�b�N�ō��ڊJ�n�s���擾����B
		 *   - ��������200�s�ǂݔ�΂��B
		 *   - "<dt class="bbsTitle clearfix">" �̍s�������܂œǂݔ�΂��B
		 *   - ���̍s�ȍ~�����ځB
		 *
		 * - ���L�̍s���p�[�X���A���ڂ𐶐�����B
		 *   <span class="titleSpan"><a href="view_bbs.pl?id=23469005&comm_id=1198460" class="title">�y�񍐁z10/1���j���[�A���œ����Ȃ��I</a>
		 *   </span>�����̍s�͊Ǘ��҂̏ꍇ�ҏW�����N�Ȃǂ�����
		 *   <span class="date">2007�N09��30�� 16:40</span>
		 * - "/#bodyMainArea" �����ꂽ�疳�����ŏI���Ƃ���B
		 *   "<ul><li>�`����\��"�����ꂽ��i���ցA�O�ւ��܂ށj�i�r�Q�[�V�����s�B
		 */

		// ���ڊJ�n�s��T��
		int iLine = 200;		// �Ƃ肠�����ǂݔ�΂�
		bool bInItems = false;
		for( ; iLine<count; iLine++ ) {
			const CString& line = html_.GetAt(iLine);
			//<div class="pageTitle communityTitle002">
			if( util::LineHasStringsNoCase( line, L"<dt", L"bbsTitle" ) ) 
			{
				// ���ڊJ�n�s�����B�{���J�n
				bInItems = true;
				break;
			}
		}

		if( !bInItems ) {
			return false;
		}

		// ���ڂ̎擾
		for( ; iLine<count; iLine++ ) {
			const CString& line = html_.GetAt(iLine);

			// /#bodyMainArea�����������͂��I������B
			if( util::LineHasStringsNoCase( line, L"/#bodyMainArea") ) 
			{
				// ���o�̐��ۂɂ�����炸�I������B
				break;
			}

			// <ul><li>�����ꂽ��i�r�Q�[�V�����s�B
			// �u���v�A�u�O�v�̃����N���܂ށB
			if( util::LineHasStringsNoCase( line, L"<ul><li>", L"����\��" ) ) 
			{
				// �u����\���v�A�u�O��\���v�̃����N�𒊏o����
				parseNextBackLink( nextLink, backLink, line );
			}

			// ����
			//   <span class="titleSpan"><a href="view_bbs.pl?id=23469005&comm_id=1198460" class="title">�y�񍐁z10/1���j���[�A���œ����Ȃ��I</a>
			//   <span class="date">2007�N09��30�� 16:40</span>
			if( util::LineHasStringsNoCase( line, L"<span", L"titleSpan" ) ) {
				// ���
				CMixiData mixi;

				// �^�C�g�����o
				CString title;
				util::GetBetweenSubString( line, L"class=\"title\">", L"</a>", title );
				mixi::ParserUtil::ReplaceEntityReferenceToCharacter( title );
				mixi.SetTitle(title);

				// URL ���o
				CString url;
				util::GetBetweenSubString( line, L"href=\"", L"\" class=", url );
				mixi.SetURL( url );

				// URL �ɉ����ăA�N�Z�X��ʂ�ݒ�
				mixi.SetAccessType( util::EstimateAccessTypeByUrl(url) );

				// ID�̒��o�A�ݒ�
				mixi.SetID( MixiUrlParser::GetID(url) );

				// ���t���
				{
					//<span class="date">2007�N09��22�� 12:55</span>
					//4�s�ȓ���<span class="date">�s��������܂�
					for( iLine++; iLine<iLine+4; iLine++ ) {
						const CString& line = html_.GetAt(iLine);
						if( util::LineHasStringsNoCase( line, L"<span", L"date") ) {
							ParserUtil::ParseDate(line, mixi);
							break;
						}
					}
				}

				// �R�����g�����
				{
					// ���L�̍s���擾���ĉ�͂���B
					// <em><a href="view_bbs.pl?id=14823636&comm_id=4043">37</a></em>
					int commentCount = -1;
					for( iLine++; iLine<count; iLine++ ) {
						const CString& line = html_.GetAt(iLine);

						if( util::LineHasStringsNoCase( line, L"<em>", url) )
						{
							// �����B�R�����g�����
							CString cc;
							util::GetBetweenSubString( line, L"\">", L"</a></em", cc );
							commentCount = _wtoi(cc);
							break;
						}
					}

					mixi.SetCommentCount(commentCount);

					// �^�C�g���̖����ɕt������
					CString title = mixi.GetTitle();
					title.AppendFormat( L"(%d)", commentCount );
					mixi.SetTitle( title );
				}

				// �R�~���j�e�B��:�Ƃ肠�������ݒ�
				mixi.SetName(L"");
				out_.push_back( mixi );
			}
		}

		// �O�A���̃����N������΁A�ǉ�����B
		if( !backLink.GetTitle().IsEmpty() ) {
			out_.insert( out_.begin(), backLink );
		}
		if( !nextLink.GetTitle().IsEmpty() ) {
			out_.push_back( nextLink );
		}

		MZ3LOGGER_DEBUG( L"ListBbsParser.parse() finished." );
		return true;
	}

private:
	/// �u����\���v�A�u�O��\���v�̃����N�𒊏o����
	/// <td ALIGN=right BGCOLOR=#EED6B5>1���`20����\��&nbsp;&nbsp;<a href=list_bbs.pl?page=2&id=xxx>����\��</a></td></tr>
	static bool parseNextBackLink( CMixiData& nextLink, CMixiData& backLink, CString str )
	{
		// ���K�\���̃R���p�C���i���̂݁j
		static MyRegex reg;
		if( !util::CompileRegex( reg, L"<a href=[\"]?list_bbs.pl([?]page=[^\"^>]+)[\"]?>([^<]+)</a>" ) ) {
			MZ3LOGGER_FATAL( FAILED_TO_COMPILE_REGEX_MSG );
			return false;
		}

		return parseNextBackLinkBase( nextLink, backLink, str, reg, L"list_bbs.pl", ACCESS_LIST_BBS );
	}
};


/**
 * [content] view_bbs.pl �p�p�[�T
 * �y�R�~���j�e�B�[�g�s�b�N�ڍׁz
 * http://mixi.jp/view_bbs.pl
 */
class ViewBbsParser : public MixiContentParser
{
public:
	static bool parse( CMixiData& mixi, const CHtmlArray& html_ ) 
	{
		MZ3LOGGER_DEBUG( L"ViewBbsParser.parse() start." );

		mixi.ClearAllList();
		mixi.ClearChildren();

		// ���O��������
		mixi.SetName(L"");

		INT_PTR lastLine = html_.GetCount();

		if (html_.GetAt(2).Find(_T("302 Moved")) != -1) {
			// ���݂͌���Ȃ�
			TRACE(_T("Moved\n"));
			return false;
		}

		bool bFoundMainText = false;	///< �{����͐����H

		// �{���擾
		int i=100;
		for (; i<lastLine; i++) {
			const CString& line = html_.GetAt(i);

			if (bFoundMainText) {
				// �{���擾�ς݂Ȃ̂ŏI���B
				break;
			}

			// ���e�������擾����
			// <span class="date">2007�N07��14�� 22:22</span></dt>
			if( util::LineHasStringsNoCase( line, L"<span", L"class", L"date" ) ) {
				ParserUtil::ParseDate(line, mixi);
			}

			// �^�C�g�����擾����
			// <span class="titleSpan"><span class="title">xxxxx</span>...
			if( util::LineHasStringsNoCase( line, L"<span", L"class", L"titleSpan", L"title" ) ) {
				CString title;
				util::GetBetweenSubString( line, L"titleSpan\">", L"</span>", title );
				// �^�O�̏���
				ParserUtil::StripAllTags( title );
				mixi::ParserUtil::ReplaceEntityReferenceToCharacter( title );
				mixi.SetTitle( title );
				continue;
			}


			if (util::LineHasStringsNoCase( line, L"<dd", L"class", L"bbsContent" ) ) {
				// �{���J�n�B�g�s�b�N�{���ˁB
				bFoundMainText = true;

				// �Ƃ肠�������s
				mixi.AddBody(_T("\r\n"));

				// <dd>
				// ����
				// </dd>
				// �܂ł��擾���A��͂���B

				// <dd> ��T���B
				for( ; i<lastLine; i++ ) {
					const CString& line = html_.GetAt( i );

					// "show_friend.pl" �܂�g�s�쐬�҂̃v���t�B�[�������N������΁A
					// �g�s�쐬�҂̃��[�U�����擾����B
					if( util::LineHasStringsNoCase( line, L"show_friend.pl" ) ) {

						ParserUtil::GetAuthor(line, &mixi);

						// ���̍s�ɂ͂Ȃ��̂Ŏ��̍s�ցB
						continue;
					}

					if (util::LineHasStringsNoCase( line, L"<dd>" )) {
						// <dd>�ȍ~���擾���A����� ������ŏI���B
						CString after;
						util::GetAfterSubString( line, L"<dd>", after );
						ParserUtil::AddBodyWithExtract( mixi, after );
						++i;
						break;
					}
				}

				// </dd> �������܂ŁA��͂���B
				for( ; i<lastLine; i++ ) {
					const CString& line = html_.GetAt( i );

					if (util::LineHasStringsNoCase( line, L"</dd>" )) {
						// �I���B
						break;
					}

					// ����pscript�^�O������������script�^�O�I���܂ŉ�́B
					if(! ParserUtil::ExtractVideoLinkFromScriptTag( mixi, i, html_ ) ) {
						// script�^�O�������Ȃ̂ŁA��́�����

						// ��\���̃^�O���폜
						if (isBbsHiddenLine(line)) {
							// ���̍s�������I�ɃX�L�b�v
							continue;
						}
						ParserUtil::AddBodyWithExtract( mixi, line );
					}
				}
			}
		}

		if (bFoundMainText) {
			// �{���擾�ς݂Ȃ̂ŁA�R�����g�擾�������s���B
			for (; i<lastLine; i++) {
				const CString& line = html_.GetAt(i);

				// <dl class="commentList01"> �𔭌�������A�R�����g�擾�������s���B
				if (util::LineHasStringsNoCase( line, L"<dl", L"class", L"commentList01" )) {
					mixi.ClearChildren();

					int index = ++i;
					while( index < lastLine ) {
						index = parseBBSComment(index, lastLine, &mixi, html_);
						if( index == -1 ) {
							// �G���[�Ȃ̂ŏI��
							break;
						}
					}
					if (index == -1 || index >= lastLine) {
						break;
					}
				}
			}
		}

		// �R�~���j�e�B�����o
		parseCommunityName( mixi, html_ );

		// �y�[�W�ړ������N�̒��o
		parsePageLink( mixi, html_ );

		// �u�ŐV�̃g�s�b�N�v�̒��o
		parseRecentTopics( mixi, html_ );

		// �������ݐ�URL�̎擾
		parsePostURL( 200, mixi, html_ );

		MZ3LOGGER_DEBUG( L"ViewBbsParser.parse() finished." );
		return true;
	}

private:

	static bool isBbsHiddenLine(const CString& line)
	{
		// <p class="reportLink01" style="display: none;"><!-- ... --></p>
		if (util::LineHasStringsNoCase(line, L"<p class=\"reportLink01\"", L"display: none;", L"</p")) {
			return true;
		}
		return false;
	}

	/**
	 * �a�a�r�R�����g�擾 �g�s�b�N �R�����g�ꗗ
	 */
	static int parseBBSComment(int sIndex, int eIndex, CMixiData* data, const CHtmlArray& html_ ) 
	{
		INT_PTR lastLine = html_.GetCount(); //�s��

		int retIndex = eIndex;
		CMixiData cmtData;
		bool findFlag = false;

/*
<dt class="commentDate clearfix"><span class="senderId">&nbsp;&nbsp;�ԍ�
</span>
<span class="date">2007�N07��28�� 21:09</span></dt>
<dd>
<dl class="commentContent01">
<dt><a href="show_friend.pl?id=XXX">�Ȃ܂�</a></dt>
<dd>
...
</dd>
*/
		int i=0;
		for( i=sIndex; i<eIndex; i++ ) {
			const CString& line = html_.GetAt(i);
			// �ԍ��擾
			// <dt class="commentDate clearfix"><span class="senderId">&nbsp;&nbsp;�ԍ�
			if (util::LineHasStringsNoCase( line, L"<dt", L"commentDate", L"senderId" )) {
				CString number;
				util::GetAfterSubString( line, L"senderId\">", number );
				// &nbsp; ������
				while(number.Replace(L"&nbsp;",L"")) {}

				cmtData.SetCommentIndex(_wtoi(number));
			}

			// �ԍ��擾�Q�i�ҏW�\�ȃR�~���j
			// <span class="senderId"><input id="commentCheck01" name="comment_id" type="checkbox" value="291541807" /><label for="commentCheck01">&nbsp;12</label></span>
			if (util::LineHasStringsNoCase( line, L"<span", L"senderId", L"checkbox" )) {
				CString number;
				util::GetAfterSubString( line, L"<label", number );
				util::GetBetweenSubString( number, L">", L"</label", number );
				// &nbsp; ������
				while(number.Replace(L"&nbsp;",L"")) {}
				cmtData.SetCommentIndex(_wtoi(number));
			}

			// ���t
			// <span class="date">2007�N07��28�� 21:09</span></dt>
			if (util::LineHasStringsNoCase( line, L"<span", L"class", L"date" ) ) {
				ParserUtil::ParseDate(line, cmtData);
			}

			// <div class="pageNavigation01"> �𔭌�������A�R�����g�I���Ȃ̂Ŕ�����
			if (util::LineHasStringsNoCase( line, L"<div", L"class", L"pageNavigation01" )) {
				return -1;
			}

			// bbs_comment_form�𔭌�������A�R�����g�I���Ȃ̂Ŕ����� 
			if (util::LineHasStringsNoCase( line, L"<form", L"bbs_comment_form", L"action" )) { 
				return -1; 
			}

			// <!-- ADD_COMMENT: start -->�𔭌�������A�R�����g�I���Ȃ̂Ŕ�����
			if (util::LineHasStringsNoCase( line, L"<!", L"ADD_COMMENT", L"start" )) {
				return -1;
			}

			// <dl class="commentContent01"> �𔭌������烋�[�v������
			if (util::LineHasStringsNoCase( line, L"<dl", L"class", L"commentContent01" )) {
				findFlag = true;
				i++;
				break;
			}
		}

		for( ; i<eIndex; i++ ) {
			const CString& line = html_.GetAt(i);
			// �Ȃ܂�
			// <dt><a href="show_friend.pl?id=XXX">�Ȃ܂�</a></dt>
			if (util::LineHasStringsNoCase( line, L"<dt", L"show_friend.pl" )) {

				ParserUtil::GetAuthor( line, &cmtData );
			}

			// <dd> ���������烋�[�v������
			if (util::LineHasStringsNoCase( line, L"<dd>" )) {
				i++;

				// ���s�ǉ�
				cmtData.AddBody(_T("\r\n"));
				break;
			}
		}

		// </dd> �܂ŉ�́��ǉ�
		for( ; i<eIndex; i++ ) {
			const CString& line = html_.GetAt(i);

			// </dd> ���������烋�[�v������
			if (util::LineHasStringsNoCase( line, L"</dd>" )) {
				i++;
				break;
			}

			// ����pscript�^�O������������script�^�O�I���܂ŉ�́B
			if(! ParserUtil::ExtractVideoLinkFromScriptTag( cmtData, i, html_ ) ) {
				// script�^�O�������Ȃ̂ŁA��́�����

				// ��\���̃^�O���폜
				if (isBbsHiddenLine(line)) {
					// ���̍s�������I�ɃX�L�b�v
					continue;
				}

				ParserUtil::AddBodyWithExtract( cmtData, line );
			}
		}
		retIndex = i;

		if( findFlag ) {
			data->AddChild(cmtData);
		}
		return retIndex;
	}
};


/**
 * [content] view_enquete.pl �p�p�[�T
 * �y�A���P�[�g�ڍׁz
 * http://mixi.jp/view_enquete.pl
 */
class ViewEnqueteParser : public MixiContentParser
{
public:
	static bool parse( CMixiData& data_, const CHtmlArray& html_ )
	{
		MZ3LOGGER_DEBUG( L"ViewEnqueteParser.parse() start." );

		data_.ClearAllList();
		data_.ClearChildren();

		// ���O��������
		data_.SetName(L"");

		INT_PTR lastLine = html_.GetCount();

		bool bInEnquete = false;
		int iLine=100;
		for( ; iLine<lastLine; iLine++ ) {
			const CString& line = html_.GetAt(iLine);

			// ���^�C�g��
			//<dt class="bbsTitle clearfix"><span class="titleSpan">XXXXXXXX</span>
			if( util::LineHasStringsNoCase( line, L"<dt", L"bbsTitle" ) )
			{
				CString title;
				util::GetBetweenSubString( line, L"titleSpan\">", L"</span>", title );
				// �^�O�̏���
				ParserUtil::StripAllTags( title );
				mixi::ParserUtil::ReplaceEntityReferenceToCharacter( title );
				data_.SetTitle(title);
				continue;
			}
			
			// ������
			//<span class="date">2007�N10��06�� 17:20</span>	</dt>
			if( util::LineHasStringsNoCase( line, L"<span", L"date" ) )
			{
				ParserUtil::ParseDate(line, data_);
				continue;
			}

			// �����҉��
			if( util::LineHasStringsNoCase( line, L"<dt>", L"show_friend" ) )
			{
				ParserUtil::GetAuthor( line, &data_ );
				continue;
			}

			// ��������e���
			if( util::LineHasStringsNoCase( line, L"<dd>" ) )	// (C4819����̃_�~�[�R�����g�ł�)
			{
				//���e��͊֐��ցA<dd class="enqueteBlock">������������߂�
				if( !parseBody( data_, html_, iLine ) )
					return false;
				bInEnquete = true;
				continue;
			}

			// ���W�v���ʉ�́B
			// <td BGCOLOR=#FFD8B0 ALIGN=center><font COLOR=#996600>�W�v����</font></td>
			if( util::LineHasStringsNoCase( line, L"<h3>", L"�W�v����" ) )
			{
				if( !parseEnqueteResult( data_, html_, iLine ) )
					return false;
				continue;
			}

			// ���A���P�[�g���e�I��
			if( bInEnquete && 
				(util::LineHasStringsNoCase( line, L"<!-- COMMENT: start -->" ) ||
				 util::LineHasStringsNoCase( line, L"<div", L"id", L"enqueteComment") ||
				 // ���M�{�^��
				 //<li><input type="submit" value="���M����" class="formBt01" /></li>
				 util::LineHasStringsNoCase( line, L"<input", L"type=", L"submit", L"���M����" )
				 ) )
			{
				bInEnquete = false;
				break;
			}

		}

		// �R�����g���
		while( iLine<lastLine ) {
			iLine = parseEnqueteComment( iLine, &data_, html_ );
			if( iLine == -1 ) {
				break;
			}
		}

		// �R�~���j�e�B�����o
		parseCommunityName( data_, html_ );

		// �y�[�W�ړ������N�̒��o
		parsePageLink( data_, html_ );

		// �u�ŐV�̃g�s�b�N�v�̒��o
		parseRecentTopics( data_, html_ );

		MZ3LOGGER_DEBUG( L"ViewEnqueteParser.parse() finished." );
		return true;
	}

private:

	/**
	 * �ݖ���e���擾����B
	 *
	 * iLine �� "�ݖ���e" �����݂���s�B
	 *
	 * (1) "<dd class=\"enqueteBlock\">" �������܂Ŗ����B���̒��ォ��A�ݖ���e�{���B��͂��āAAddBody ����B
	 * (2) "</dd>" �����ꂽ��I���B
	 */
	static bool parseBody( CMixiData& mixi, const CHtmlArray& html, int& iLine )
	{
		++iLine;

		mixi.AddBody(_T("\r\n"));
		const int lastLine = html.GetCount();

		for( ; iLine<lastLine; iLine++ ) {
			const CString& line = html.GetAt(iLine);

			// <dd class=\"enqueteBlock\"> ������΁A���̌���{���Ƃ���B
			CString target;
			if( util::GetAfterSubString( line, L"<dd class=\"enqueteBlock\">", target ) >= 0 ) {
				// <dd class=\"enqueteBlock\">�����B
			}else{
				// <dd class=\"enqueteBlock\">�������B
				target = line;
			}

			// </dd>������΁A���̑O��ǉ����A�I���B
			// �Ȃ���΁A���̍s��ǉ����A���̍s�ցB
			if( util::GetBeforeSubString( target, L"</dd>", target ) < 0 ) {
				// </dd> ��������Ȃ������B
				ParserUtil::AddBodyWithExtract( mixi, target );
			}else{
				// </dd>�����������̂ŏI���B
				ParserUtil::AddBodyWithExtract( mixi, target );
				break;
			}

			// reply_enquete.pl �p�F</ul> ������ΏI���B
			if( target.Find(L"</ul>") != -1 ) {
				break;
			}
		}
		if( iLine >= lastLine ) {
			return false;
		}
		return true;
	}

	/**
	 * �W�v���ʂ��擾����B
	 *
	 * iLine �� "�W�v����" �����݂���s�B
	 *
	 * �� "</table>" �����ꂽ��I���B
	 *
	 * �� ���L�̌`���ŁA����I�����ɑ΂���v�f������B
	 * <!-- oneMeter -->
	 * <dl class="enqueteList">
	 * <dt>�񓚑I����<br />
	 * <img src="http://img.mixi.jp/img/bar.gif" width="28" height="16" alt="" />
	 * </dt>
	 * <dd><span>9</span>(3%)</dd>
	 * </dl>
	 * <!-- oneMeter -->
	 * ��́A���`���āAAddBody ����B
	 *
	 * �� ���L�̌`���ŁA���v������̂ŁA��́A���`���āAAddBody����B
	 * <dl class="enqueteTotal"><dt class="enqueteTotalNumber">���v<span>244</span></dt></dl>
	 */
	static bool parseEnqueteResult( CMixiData& mixi, const CHtmlArray& html, int& iLine )
	{
		++iLine;

		mixi.AddBody( L"\r\n" );
		mixi.AddBody( L"\r\n" );
		mixi.AddBody( L"���W�v����\r\n" );

		const int lastLine = html.GetCount();
		for( ; iLine<lastLine; iLine++ ) {
			const CString& line = html.GetAt(iLine);

			// <dd class="formButtons01">������΁A��͏I���B
			if( util::LineHasStringsNoCase( line, L"<dd class=\"formButtons" ) ) {
				break;
			}

			// �I�����A���[���A�S��������͂��āAAddBody ����B
			if( util::LineHasStringsNoCase( line, L"<dl class=\"enqueteList\">" ) ) {
				// ���E�l�`�F�b�N
				if( iLine+5 >= lastLine ) {
					break;
				}
				// ���̍s�ɖ{��������B
				const CString& line1 = html.GetAt(++iLine);
				CString item;
				util::GetBetweenSubString( line1, L"<dt>", L"<br />", item );
				
				// +3 �s�ڂɉ��L�̌`���ŁA���[���A�S����������B
				// <dd><span>125</span>(51%)</dd>
				iLine += 3;
				const CString& line2 = html.GetAt( iLine );
				CString target;
				util::GetBetweenSubString( line2, L"<span>", L"</span>", target );

				CString target2;
				util::GetBetweenSubString( line2, L"</span>", L"</dd>", target2 );
				
				// xx (yy%)
				CString num_rate;
				num_rate = target + target2;

				CString str;
				str.Format( L"  ��%s\r\n", item );
				mixi.AddBody( str );
				str.Format( L"      %s\r\n", num_rate );
				mixi.AddBody( str );
			}

			// ���v����͂���B
			if( util::LineHasStringsNoCase( line, L"enqueteTotal" ) )
			{
				// ���̍s�ɍ��v������B
				const CString& line1 = html.GetAt( iLine );

				CString total;
				util::GetBetweenSubString( line1, L"<span>", L"</span>", total );

				CString str;
				str.Format( L"  �����v\r\n" );
				mixi.AddBody( str );
				str.Format( L"      %s\r\n", total );
				mixi.AddBody( str );
			}
		}

		if( iLine >= lastLine ) {
			return false;
		}
		return true;
	}

	/**
	 * �A���P�[�g�R�����g�擾
	 */
	static int parseEnqueteComment(int sIndex, CMixiData* data, const CHtmlArray& html_ )
	{
		int count = html_.GetCount();
		int retIndex = count;

		CMixiData cmtData;
		BOOL findFlag = FALSE;

		for (int i=sIndex; i<count; i++) {
			const CString& str = html_.GetAt(i);

			if (findFlag == FALSE) {

				if (str.Find(_T("<!-- ADD_COMMENT: start -->")) != -1 ||
				    str.Find(_T("<!-- COMMENT: end -->")) != -1 ||
				    str.Find(_T("add_enquete_comment.pl")) != -1) 
				{
					// �R�����g�S�̂̏I���^�O����
					parsePostURL( i, *data, html_ );
					retIndex = -1;
					break;

				} else {

					const CString& line = html_.GetAt(i);
					// �R�����g�ԍ����擾
					//<label for="commentCheck01">37</label>
					if( util::LineHasStringsNoCase( line, L"\">", L"</label>")) {
						CString number;
						util::GetAfterSubString( line, L"<label", number );
						util::GetBetweenSubString( number, L">", L"</label", number );
						// &nbsp; ������
						while(number.Replace(L"&nbsp;",L"")) {}
						cmtData.SetCommentIndex(_wtoi(number));
					}
					
					// ���t���擾
					//<span class="date">2007�N10��07�� 11:25</span></dt>
					if( util::LineHasStringsNoCase( line, L"date\">", L"</span>")) {
						ParserUtil::ParseDate(line, cmtData);
					}
					
					// ���O���擾
					//<dt><a href="show_friend.pl?id=xxxxxx">�Ȃ܂�</a></dt>
					if( util::LineHasStringsNoCase( line, L"<dt>", L"</dt>")) {
						CString Author;
						util::GetBetweenSubString( line, L"<dt>", L"</dt>", Author );
						ParserUtil::GetAuthor( Author, &cmtData );
						//�R�����g�J�n�t���O��ON
						findFlag = TRUE;
					}

				}
			}
			else {

				if( util::LineHasStringsNoCase( str, L"<dd>") ) {
					// �R�����g�R�����g�{���擾
					const CString& str = html_.GetAt(i);

					// ----------------------------------------
					// �A���P�[�g�̃p�^�[��
					// ----------------------------------------
					cmtData.AddBody(_T("\r\n"));

					CString buf;
					util::GetAfterSubString( str, L">", buf );

					if( util::GetBeforeSubString( buf, L"</dd>", buf ) > 0 ) {

						// �I���^�O���������ꍇ
						ParserUtil::AddBodyWithExtract( cmtData, buf );
						retIndex = i + 5;
						break;
					}

					// ����ȊO�̏ꍇ
					ParserUtil::AddBodyWithExtract( cmtData, buf );

					while( i<count ) {
						i++;
						const CString& line  = html_.GetAt(i);
						int index = line.Find(_T("</dd>"));
						if (index != -1) {
							// �I���^�O����
							buf = line.Left(index);
							ParserUtil::AddBodyWithExtract( cmtData, buf );
							retIndex = i + 1;
							break;
						}

						ParserUtil::AddBodyWithExtract( cmtData, line );
					}
					break;
				}
			}

		}

		if( findFlag ) {
			data->AddChild(cmtData);
		}
		return retIndex;
	}

};


/**
 * [content] view_event.pl �p�p�[�T
 * �y�C�x���g�ڍׁz
 * http://mixi.jp/view_event.pl
 */
class ViewEventParser : public MixiContentParser
{
public:
	static bool parse( CMixiData& data_, const CHtmlArray& html_ )
	{
		MZ3LOGGER_DEBUG( L"ViewEventParser.parse() start." );

		data_.ClearAllList();
		data_.ClearChildren();
		
		// ���O��������
		data_.SetName(L"");

		INT_PTR lastLine = html_.GetCount();

		int iLine=100;
		for( ; iLine<lastLine; iLine++ ) {
			const CString& line = html_.GetAt(iLine);

			// ���^�C�g��(pattern1)
			//<dt class="bbsTitle clearfix"><span class="titleSpan"><span class="title">xxxxx</span></span>
			if( util::LineHasStringsNoCase( line, L"<span", L"titleSpan" ) )
			{
				CString title;
				util::GetBetweenSubString( line, L"class=\"title\">", L"</span>", title );
				mixi::ParserUtil::ReplaceEntityReferenceToCharacter( title );
				data_.SetTitle(title);
				continue;
			}

			// ������
			//<dd>2007�N09��27��(�n��ɂ���Ă͒x���ꍇ����)</dd>
			if( util::LineHasStringsNoCase( line, L"<dt>�J�Ó���</dt>" ) )
			{
				const CString& line2 = html_.GetAt(iLine+1);
				CString date;
				util::GetBetweenSubString( line2, L"<dd>", L"</dd>", date );
				//ParserUtil::ParseDate(date, cmtData);
				//data_.SetDate(date);
				CString buf = _T("�J�Ó��� ") + date;
				data_.AddBody(_T("\r\n"));
				data_.AddBody(buf);
				data_.AddBody(_T("\r\n"));
				
				continue;
			}

			// ���ꏊ
			//<dd>�s���{������(�S���̏��X�Ȃ�)</dd>
			if( util::LineHasStringsNoCase( line, L"<dt>�J�Ïꏊ</dt>" ) )
			{
				const CString& line2 = html_.GetAt(iLine+1);
				CString area;
				util::GetBetweenSubString( line2, L"<dd>", L"</dd>", area );
				CString buf = _T("�J�Ïꏊ ") + area;
				data_.AddBody(_T("\r\n"));
				data_.AddBody(buf);
				data_.AddBody(_T("\r\n"));
				
				continue;
			}

			// �����҉��
			if( util::LineHasStringsNoCase( line, L"<dt>", L"show_friend" ) )
			{
				const CString& line2 = html_.GetAt(iLine );
				CString Author;
				util::GetBetweenSubString( line2, L"<dt>", L"</dt>", Author );
				ParserUtil::GetAuthor( Author, &data_ );

				// �����e���
				//���e��͊֐��ցA</dd>������������߂�
				if( !parseBody( data_, html_, iLine ) )
					return false;
				continue;
			}

			// ����W����
			//<dd>�w��Ȃ�</dd>
			if( util::LineHasStringsNoCase( line, L"<dt>��W����</dt>" ) )
			{
				const CString& line2 = html_.GetAt(iLine+1);
				CString limit;
				util::GetBetweenSubString( line2, L"<dd>", L"</dd>", limit );
				CString buf = _T("��W���� ") + limit;
				data_.AddBody(_T("\r\n"));
				data_.AddBody(buf);
				data_.AddBody(_T("\r\n"));
				
				continue;
			}

			// ���Q����
			//<dd>NNN��</dd>
			if( util::LineHasStringsNoCase( line, L"<dt>�Q����</dt>" ) )
			{
				const CString& line2 = html_.GetAt(iLine+1);
				CString people;
				util::GetBetweenSubString( line2, L"<dd>", L"</dd>", people );
				CString buf = _T("�Q���� ") + people;
				data_.AddBody(_T("\r\n"));
				data_.AddBody(buf);
				data_.AddBody(_T("\r\n"));
				
				continue;
			}

			// ���C�x���g���e�I��
			if( util::LineHasStringsNoCase( line, L"<!-- COMMENT: start -->" ) ||
				util::LineHasStringsNoCase( line, L"<div", L"id", L"eventComment") ) {
				break;
			}

		}

		// �R�����g���
		while( iLine<lastLine ) {
			iLine = parseEventComment( iLine, &data_, html_ );
			if( iLine == -1 ) {
				break;
			}
		}

		// �C�x���g�Q�������o�[�ւ̃����N
		{
			CString url = util::FormatString(
							L"http://mixi.jp/list_event_member.pl?id=%s&comm_id=%s",
							(LPCTSTR)util::GetParamFromURL(data_.GetURL(), L"id"),
							(LPCTSTR)util::GetParamFromURL(data_.GetURL(), L"comm_id"));
			data_.m_linkPage.push_back( CMixiData::Link(url, L"���Q���҈ꗗ") );
		}

		// �R�~���j�e�B�����o
		parseCommunityName( data_, html_ );

		// �y�[�W�ړ������N�̒��o
		parsePageLink( data_, html_ );

		// �u�ŐV�̃g�s�b�N�v�̒��o
		parseRecentTopics( data_, html_ );

		MZ3LOGGER_DEBUG( L"ViewEventParser.parse() finished." );
		return true;
	}

private:

	/**
	 * �ݖ���e���擾����B
	 *
	 * iLine �� "�ݖ���e" �����݂���s�B
	 *
	 * (1) <dd>�������܂Ŗ����B���̒��ォ��A�ݖ���e�{���B��͂��āAAddBody ����B
	 * (2) "</dd>" �����ꂽ��I���B
	 */
	static bool parseBody( CMixiData& mixi, const CHtmlArray& html, int& iLine )
	{
		++iLine;

		mixi.AddBody(_T("\r\n"));
		const int lastLine = html.GetCount();

		for( ; iLine<lastLine; iLine++ ) {
			const CString& line = html.GetAt(iLine);

			// <dd>������΁A���̌���{���Ƃ���B
			CString target;
			if( util::GetAfterSubString( line, L"<dd>", target ) >= 0 ) {
				// <dd>�����B
			}else{
				// <dd>�������B
				target = line;
			}

			// </dd>������΁A���̑O��ǉ����A�I���B
			// �Ȃ���΁A���̍s��ǉ����A���̍s�ցB
			if( util::GetBeforeSubString( target, L"</dd>", target ) < 0 ) {
				// </dd> ��������Ȃ������B
				ParserUtil::AddBodyWithExtract( mixi, target );
			}else{
				// </dd>�����������̂ŏI���B
				ParserUtil::AddBodyWithExtract( mixi, target );
				break;
			}
		}
		if( iLine >= lastLine ) {
			return false;
		}
		return true;
	}

	/**
	 * �C�x���g�R�����g�擾
	 */
	static int parseEventComment(int sIndex, CMixiData* data, const CHtmlArray& html_ )
	{
		int count = html_.GetCount();
		int retIndex = count;

		CMixiData cmtData;
		BOOL findFlag = FALSE;

		for (int i=sIndex; i<count; i++) {
			const CString& str = html_.GetAt(i);

			if (findFlag == FALSE) {

				if (str.Find(_T("<!-- ADD_COMMENT: start -->")) != -1 ||
				    str.Find(_T("<!-- COMMENT: end -->")) != -1 ||
				    str.Find(_T("add_event_comment.pl")) != -1)
				{
					// �R�����g�S�̂̏I���^�O����
					parsePostURL( i, *data, html_ );
					retIndex = -1;
					break;
				
				} else {

					const CString& line = html_.GetAt(i);
					//�R�����g�ԍ����擾
					// <dt class="commentDate clearfix"><span class="senderId">183
					// or
					// <dt class="commentDate clearfix"><span class="senderId"><input type="checkbox" name="comment_id" id="commentCheck290675880" value="290675880" /><label for="commentCheck290675880">44</label></span>
					if( util::LineHasStringsNoCase( line, L"commentDate", L"senderId")) {
						CString number;
						if (util::GetAfterSubString( line, L"<label", number ) >= 0) {
							// �Ǘ��R�~����
							util::GetBetweenSubString( number, L">", L"</label", number );
						} else {
							util::GetAfterSubString( line, L"<span", number );
							util::GetAfterSubString( number, L">", number );
						}
						// &nbsp; ������
						while(number.Replace(L"&nbsp;",L"")) {}
						cmtData.SetCommentIndex( _wtoi(number) );
					}
					
					//���t���擾
					//<span class="date">2007�N10��07�� 11:25</span></dt>
					if( util::LineHasStringsNoCase( line, L"date\">", L"</span>")) {
						ParserUtil::ParseDate(line, cmtData);
					}
					
					//���O���擾
					//<dt><a href="show_friend.pl?id=xxxxxx">�Ȃ܂�</a></dt>
					if( util::LineHasStringsNoCase( line, L"show_friend", L"</dt>")) {
						CString Author;
						util::GetBetweenSubString( line, L"<dt>", L"</dt>", Author );
						ParserUtil::GetAuthor( Author, &cmtData );
						//�R�����g�J�n�t���O��ON
						findFlag = TRUE;
					}

				}
			}
			else {

				if( util::LineHasStringsNoCase( str, L"<dd>") ) {
					// �R�����g�R�����g�{���擾
					const CString& str = html_.GetAt(i);

					// ----------------------------------------
					// �C�x���g�̃p�^�[��
					// ----------------------------------------
					cmtData.AddBody(_T("\r\n"));

					CString buf;
					util::GetAfterSubString( str, L">", buf );

					if( util::GetBeforeSubString( buf, L"</dd>", buf ) > 0 ) {

						// �I���^�O���������ꍇ
						ParserUtil::AddBodyWithExtract( cmtData, buf );
						retIndex = i + 1;
						break;
					}

					// ����ȊO�̏ꍇ
					ParserUtil::AddBodyWithExtract( cmtData, buf );

					while( i<count ) {
						i++;
						const CString& line  = html_.GetAt(i);
						int index = line.Find(_T("</dd>"));
						if (index != -1) {
							// �I���^�O����
							buf = line.Left(index);
							ParserUtil::AddBodyWithExtract( cmtData, buf );
							retIndex = i + 1;
							break;
						}

						ParserUtil::AddBodyWithExtract( cmtData, line );
					}
					break;
				}
			}

		}

		if( findFlag ) {
			data->AddChild(cmtData);
		}
		return retIndex;
	}

};


//�������j���[�X������
/**
 * [list] list_news_category.pl �p�p�[�T�B
 * �y�j���[�X�̃J�e�S���z
 * http://news.mixi.jp/list_news_category.pl?id=pickup&type=bn
 */
class ListNewsCategoryParser : public MixiListParser
{
public:
	static bool parse( CMixiDataList& out_, const CHtmlArray& html_ )
	{
		MZ3LOGGER_DEBUG( L"ListNewsCategoryParser.parse() start." );

		INT_PTR count = html_.GetCount();

		// �u���v�A�u�O�v�̃����N
		CMixiData backLink;
		CMixiData nextLink;

		/**
		 * ���j�F
		 * - <table class=..
		 *   ��������΂������獀�ڊJ�n�Ƃ݂Ȃ�
		 * - ��������+18�s�ڈȍ~��
		 *   <td ...><A HREF="view_news.pl?id=XXXXX&media_id=X"class="new_link">title</A>
		 *   �Ƃ����`���ŁuURL�v�Ɓu�^�C�g���v�����݂���B
		 * - ����1�s���
		 *   <td ...><A HREF="list_news_media.pl?id=2">�񋟉��</A></td>
		 *   �Ƃ����`���Łu�񋟉�Ёv�����݂���B
		 * - ���̍s��
		 *   <td WIDTH="1%" nowrap CLASS="f08">11��30�� 20:36</td></tr>
		 *   �Ƃ����`���Łu�z�M�����v�����݂���B
		 * - ���ڂ��������Ĉȍ~�ɁA
		 *   </table>
		 *   ������΁A�������I������
		 */

		// ���ڊJ�n��T��
		bool bInItems = false;	// �u���ڂ̃s�b�N�A�b�v�v�J�n�H
		int iLine = 200;		// �Ƃ肠�����ǂݔ�΂�
		for( ; iLine<count; iLine++ ) {
			const CString& str = html_.GetAt(iLine);

			// �u���v�A�u�O�v�̃����N
			// ���ڔ�����ɂ̂ݑ��݂���
			if( bInItems ) {
				// �u����\���v�A�u�O��\���v�̃����N�𒊏o����
				if( parseNextBackLink( nextLink, backLink, str ) ) {
					continue;
				}
			}

			if( !bInItems ) {
				if( str.Find( L"<table class=" ) == -1 ) {
					// �J�n�t���O������
					continue;
				}
				bInItems = true;
			}
			if( str.Find( L"<a" ) == -1 || str.Find( L"view_news.pl" ) == -1 ) {
				// ���ږ�����
				continue;
			}
			// ���ڔ����B
			if( str.Find(_T("</table>")) != -1 ) {
				// �I���^�O����
				break;
			}

			//--- URL �� �^�C�g���̒��o
			std::wstring url, title;
			{
				// ���K�\���̃R���p�C���i���̂݁j
				static MyRegex reg;
				if( !util::CompileRegex( reg, L"\"(view_news\\.pl\\?id=[0-9]+\\&media_id=[0-9]+).+>(.+)</a" ) ) {
					MZ3LOGGER_FATAL( FAILED_TO_COMPILE_REGEX_MSG );
					return false;
				}
				// �T��
				if( reg.exec(str) == false || reg.results.size() != 3 ) {
					// �������B
					continue;
				}
				// �����B
				url   = reg.results[1].str;	// URL
				title = reg.results[2].str;	// title
			}

			//--- �񋟉�Ђ̒��o
			// +1 �` +4 �s�ڂ̕ӂ�ɂ���͂��B
			std::wstring author;
			for( int iInc=1; iInc<=4; iInc++ ) {
				const CString& line2 = html_.GetAt( ++iLine );

				// ���K�\���̃R���p�C���i���̂݁j
				static MyRegex reg;
				if( !util::CompileRegex( reg, L"list_news_media\\.pl.+>([^<]+)</a" ) ) {
					MZ3LOGGER_FATAL( FAILED_TO_COMPILE_REGEX_MSG );
					return false;
				}
				// �T��
				if( reg.exec(line2) == false || reg.results.size() != 2 ) {
					// �������B
					continue;
				}
				// �����B
				author = reg.results[1].str;	// �񋟉��
				break;
			}
			if( author.empty() ) {
				// �������̂��ߏI��
				continue;
			}

			//--- �z�M�����̒��o
			iLine += 1;
			// <td WIDTH="1%" nowrap CLASS="f08">10��14�� 16:47</td></tr>
			const CString& date = html_.GetAt(iLine);

			// mixi �f�[�^�̍쐬
			{
				CMixiData data;
				data.SetAccessType( ACCESS_NEWS );

				// ���t
				ParserUtil::ParseDate( date, data );
				TRACE(_T("%s\n"), data.GetDate());

				// ���o��
				CString wktitle = title.c_str();
				mixi::ParserUtil::ReplaceEntityReferenceToCharacter( wktitle );
				data.SetTitle( wktitle );
				TRACE(_T("%s\n"), data.GetTitle());

				// URL ����
				CString url2 = L"http://news.mixi.jp/";
				url2 += url.c_str();
				data.SetURL( url2 );
				TRACE(_T("%s\n"), data.GetURL());

				// ���O
				CString name = author.c_str();
				mixi::ParserUtil::ReplaceEntityReferenceToCharacter( name );
				data.SetName( name );
				data.SetAuthor( name );
				TRACE(_T("%s\n"), data.GetName());

				out_.push_back(data);
			}
		}

		// �O�A���̃����N������΁A�ǉ�����B
		if( !backLink.GetTitle().IsEmpty() ) {
			out_.insert( out_.begin(), backLink );
		}
		if( !nextLink.GetTitle().IsEmpty() ) {
			out_.push_back( nextLink );
		}

		MZ3LOGGER_DEBUG( L"ListNewsCategoryParser.parse() finished." );
		return true;
	}

private:
	/// �u����\���v�A�u�O��\���v�̃����N�𒊏o����
	static bool parseNextBackLink( CMixiData& nextLink, CMixiData& backLink, CString str )
	{
		// ���K�\���̃R���p�C���i���̂݁j
		static MyRegex reg;
		if( !util::CompileRegex( reg, L"<a href=[\"]?list_news_category.pl([?]page=[^\"^>]+)[\"]?>([^<]+)</a>" ) ) {
			MZ3LOGGER_FATAL( FAILED_TO_COMPILE_REGEX_MSG );
			return false;
		}

		return parseNextBackLinkBase( nextLink, backLink, str, reg, 
					L"http://news.mixi.jp/list_news_category.pl", ACCESS_LIST_NEWS );
	}

};


/**
 * [content] view_news.pl �p�p�[�T
 * �y�j���[�X�L���z
 * http://mixi.jp/view_news.pl
 */
class ViewNewsParser : public MixiContentParser
{
public:
	static bool parse( CMixiData& data_, const CHtmlArray& html_ )
	{
		MZ3LOGGER_DEBUG( L"ViewNewsParser.parse() start." );

		data_.ClearAllList();
		data_.ClearChildren();
		data_.SetName(L"");

		INT_PTR count = html_.GetCount();

		// �^�C�g���� title �^�O���璊�o����
		for (int iLine=0; iLine<20 && iLine<count; iLine++) {
			const CString& line = html_.GetAt(iLine);
	
			//<title>[mixi] �����Ƃ�i�z�M�����́j</title>
			if (util::LineHasStringsNoCase( line, L"<title>[mixi]", L"</title>" ) ) {
				CString title;
				util::GetBetweenSubString( line, L"<title>[mixi]", L"</title>", title );
				title.Trim();
				mixi::ParserUtil::ReplaceEntityReferenceToCharacter( title );
				data_.SetTitle(title);
				break;
			}
		}

		/**
		 * ���j�F
		 * - <div class=\"article\">
		 *   ��������΂������獀�ڊJ�n�Ƃ݂Ȃ��B
		 * - �ȍ~�A
		 *   </td></tr>
		 *   ��������܂ŁA�p�[�X�𑱂���B
		 */
		// ���ڊJ�n��T��
		bool bInItems = false;
		int iLine = 200;		// �Ƃ肠�����ǂݔ�΂�
		for( ; iLine<count; iLine++ ) {
			const CString& line = html_.GetAt(iLine);

			if( !bInItems ) {
				if( !util::LineHasStringsNoCase( line, L"<div class=\"article\">" ) ) {
					// �J�n�t���O������
					continue;
				}
				bInItems = true;
				
				// �Ƃ肠�������s�o��
				data_.AddBody(_T("\r\n"));

				// ���̍s�͖�������B
				continue;
			}
			
			// ���ڔ����B
			// �s���� <div class=\"bottomContents clearfix\">������ΏI���B
			LPCTSTR endTag = _T("<div class=\"bottomContents clearfix\">");
			if( wcsncmp( line, endTag, wcslen(endTag) ) == 0 ) {
				// �I���^�O����
				break;
			}

			// ���L������Ζ����Ƃ݂Ȃ�
			/*
			<a name="post-check"></a>
<p class="checkButton">
<a check_key="53be64390c2bf14912cc8ded09c28d346a7a28b0" check_button="button-5.gif">�`�F�b�N</a>
<script data-prefix-uri="http://mixi.jp/" 
        src="http://static.mixi.jp/js/share.js" 
        type="text/javascript"></script>
</p>
			*/
			endTag = _T("<a name=\"post-check\"></a>");
			if( wcsncmp( line, endTag, wcslen(endTag) ) == 0 ) {
				// �I���^�O����
				break;
			}

			ParserUtil::AddBodyWithExtract( data_, line );
		}

		int id = _wtoi(util::GetParamFromURL(data_.GetURL(), L"id"));
		data_.m_linkPage.push_back(
			MZ3Data::Link(util::FormatString(L"http://news.mixi.jp/list_quote_diary.pl?id=%d", id), 
						  L"�֘A���L"));

		MZ3LOGGER_DEBUG( L"ViewNewsParser.parse() finished." );
		return true;
	}

};



//���������̑�������

/**
 * [list] show_intro.pl �p�p�[�T�B
 * �y�Љ�z
 * http://mixi.jp/show_intro.pl
 */
class ShowIntroParser : public MixiListParser
{
public:
	static bool parse( CMixiDataList& out_, const CHtmlArray& html_ )
	{
		MZ3LOGGER_DEBUG( L"ShowIntroParser.parse() start." );

		INT_PTR lastLine = html_.GetCount();

		// �u���v�A�u�O�v�̃����N
		CMixiData backLink;
		CMixiData nextLink;

		// ���ڊJ�n��T��
		bool bInItems = false;	// ���ڊJ�n�H
		int iLine = 100;		// �Ƃ肠�����ǂݔ�΂�
		for( ; iLine<lastLine; iLine++ ) {
			const CString& line = html_.GetAt(iLine);

			// ���ڊJ�n�H
			if( !bInItems ) {
				//<ul class="introListContents">
				if( util::LineHasStringsNoCase( line, L"<ul", L"class=", L"introListContents" ) ) {
					bInItems = true;
				}
			} else {

				//<div class="pageNavigation01"> �ŏI��
				if( util::LineHasStringsNoCase( line, L"<div", L"class=", L"pageNavigation01" ) ) {
					break;
				}

				// show_friend.pl ������΍��ڂ��ۂ�
				if( util::LineHasStringsNoCase( line, L"<a", L"href=", L"show_friend.pl" ) ) {
					CMixiData mixi;
					if( parseOneIntro( mixi, html_, iLine ) ) {
						out_.push_back( mixi );
					}
				}
			}
		}

		// �I���^�O�����������̂ŁA���̌�̍s���玟�A�O�̃����N�𒊏o
		for( ; iLine<lastLine; iLine++ ) {
			const CString& line = html_.GetAt(iLine);

			// �u����\���v�A�u�O��\���v�̃����N�𒊏o����
			if( parseNextBackLink( nextLink, backLink, line ) ) {
				// ���o�ł�����I���^�O
				break;
			}
		}

		// �O�A���̃����N������΁A�ǉ�����B
		if( !backLink.GetTitle().IsEmpty() ) {
			out_.insert( out_.begin(), backLink );
		}
		if( !nextLink.GetTitle().IsEmpty() ) {
			out_.push_back( nextLink );
		}

		MZ3LOGGER_DEBUG( L"ShowIntroParser.parse() finished." );
		return true;
	}

private:
	/**
	 * �P�̏Љ�𒊏o����B
	 * iLine �� show_friend.pl �����ꂽ�s�B
	 */
	static bool parseOneIntro( CMixiData& mixi, const CHtmlArray& html, int& iLine )
	{
		const int lastLine = html.GetCount();

		if (iLine+2>lastLine) {
			return false;
		}

		// �摜���o
		const CString& line = html.GetAt( iLine );
		CString image_url;
		if (util::GetBetweenSubString( line, L"src=\"", L"\"", image_url ) >= 0 ) {
			mixi.AddImage( image_url );
		}

		const CString& line2 = html.GetAt( ++iLine );

		// URL ���o
		CString target;
		if( util::GetAfterSubString( line2, L"<a ", target ) < 0 ) {
			return false;
		}
		// target: href="show_friend.pl?id=xxx">�Ȃ܂�</a></dt>
		CString url;
		if( util::GetBetweenSubString( target, L"href=\"", L"\"", url ) < 0 ) {
			return false;
		}
		mixi.SetURL( url );

		// URL �\�z���ݒ�
		url.Insert( 0, L"http://mixi.jp/" );
		mixi.SetBrowseUri( url );

		// ���O���o
		CString name;
		if( util::GetBetweenSubString( target, L">", L"<", name ) < 0 ) {
			return false;
		}
		mixi::ParserUtil::ReplaceEntityReferenceToCharacter( name );
		mixi.SetName( name );

		mixi.SetAccessType( ACCESS_PROFILE );

		// </div> ��������܂ŁA�Љ��T��
		for( ++iLine; iLine<lastLine; iLine++ ) {
			const CString& line = html.GetAt( iLine );

			// <ul> or </div> ������ΏI���B
			if( util::LineHasStringsNoCase( line, L"</div>" ) ||
				util::LineHasStringsNoCase( line, L"<ul>" ) ) {
				break;
			}else{
				CString tagStripedLine = line;
				ParserUtil::StripAllTags( tagStripedLine );
				if (!tagStripedLine.IsEmpty()) {
					ParserUtil::AddBodyWithExtract( mixi, tagStripedLine );
					mixi.AddBody( L"\r\n" );
				}
			}
		}

		if( iLine >= lastLine ) {
			return false;
		}

		return true;
	}

	/// �u����\���v�A�u�O��\���v�̃����N�𒊏o����
	/// <td align=right>1���`50����\��&nbsp;&nbsp;<a href=show_intro.pl?page=2&id=xxx>����\��</a></td>
	static bool parseNextBackLink( CMixiData& nextLink, CMixiData& backLink, CString str )
	{
		// ���K�\���̃R���p�C���i���̂݁j
		static MyRegex reg;
		if( !util::CompileRegex( reg, L"<a href=[\"]?show_intro.pl([?]page=[^\"^>]+)[\"]?>([^<]+)</a>" ) ) {
			MZ3LOGGER_FATAL( FAILED_TO_COMPILE_REGEX_MSG );
			return false;
		}

		return parseNextBackLinkBase( nextLink, backLink, str, reg, L"show_intro.pl", ACCESS_LIST_INTRO );
	}
};


/**
 * [list] show_calendar.pl �p�p�[�T�B
 * �y�J�����_�[�z
 * http://mixi.jp/show_calendar.pl
 */
class ShowCalendarParser : public MixiListParser
{
public:
	static bool parse( CMixiDataList& out_, const CHtmlArray& html_ )
	{
		MZ3LOGGER_DEBUG( L"ShowCalendarParser.parse() start." );

		INT_PTR count = html_.GetCount();

		BOOL findFlag = FALSE;
		BOOL findFlag2 = FALSE;
		CString YearMonth;
		CString strDate;

		// �u���v�A�u�O�v�̃����N
		CMixiData backLink;
		CMixiData nextLink;
			
		for (int i=0; i<count; i++) {

			const CString& str = html_.GetAt(i);

			//<div class="calendarNavi bottom">�����ꂽ���͏I��
			if( util::LineHasStringsNoCase( str, L"<div class=\"calendarNavi bottom\">" ) ) {
				break;
			}
			
			//�N���𒊏o
			if( util::LineHasStringsNoCase( str, L"<title>[mixi]", L"�̃J�����_�[</title>" ) ) {
				util::GetBetweenSubString( str, L"<title>[mixi]", L"�̃J�����_�[</title>", YearMonth );
				findFlag = TRUE;
			}
			
			//�J�n�t���O
			if( util::LineHasStringsNoCase( str, L"<div class=\"calendarTable\">" ) ) {
				findFlag2 = TRUE;
			}

			// ���E�O�̌������N�̒��o
			if (findFlag && !findFlag2) {
				parseNextBackLink(nextLink, backLink, str);
			}

			if (findFlag != FALSE && findFlag2 != FALSE ) {

				CString target = str;
				CString title;
				CString url;
				BOOL findFlag3 = FALSE;
				CMixiData data;
					
				//<a href="view_event.pl?id=nnnnn&comm_id=xxxxx">�C�x���g�^�C�g��</a> �C�x���g
				if( util::LineHasStringsNoCase( target, L"view_event.pl" )) 
				{
					CString subtarget;
					util::GetAfterSubString( target , L"<a href=\"" , subtarget );
					util::GetBetweenSubString( subtarget, L"\">", L"</a>", title );
					util::GetBetweenSubString( target, L"<a href=\"", L"\">", url );
					if( util::LineHasStringsNoCase( target, L"<li class=\"join\">" )) {
						data.SetAccessType( ACCESS_EVENT_JOIN );
					} else {
						data.SetAccessType( ACCESS_EVENT );
					}
					findFlag3 = TRUE;
				}
				//<a href="show_friend.pl?id=xxxx">XX����</a>�@�a����
				if( util::LineHasStringsNoCase( target, L"show_friend.pl" ) ) {
				
					//show_friend.pl�ȍ~�����o��
					CString after;
					util::GetAfterSubString( target, L"show_friend.pl", after );
					util::GetBetweenSubString( after, L">", L"</a>", title );
					
					title = L"�y�a�����z" + title;
					//��΂���̃v���t�B�[��URL�𒊏o
					util::GetBetweenSubString( target, L"<a href=\"", L"\">", url );
					data.SetURL( url );
					data.SetAccessType( ACCESS_BIRTHDAY );
					findFlag3 = TRUE;
				}
				//<a href="javascript:void(0);" onClick="MM_openBrWindow('view_schedule.pl?id=nnnnn','','width=760,height=640,toolbar=no,scrollbars=yes,left=10,top=10')">�X�P�W���[��</a>�@�����X�P�W���[��
				if( util::LineHasStringsNoCase( target, L"view_schedule.pl" ) ) {
					util::GetBetweenSubString( target, L"')\">", L"</a>", title );

					title = L"�y�\��z" + title;
					//��΂���̃X�P�W���[���ڍׂ��܂��������̂��ߕۗ�
					util::GetBetweenSubString( target, L"MM_openBrWindow('", L"'", url );

					data.SetAccessType( ACCESS_SCHEDULE );
					findFlag3 = TRUE;
				}

				mixi::ParserUtil::ReplaceEntityReferenceToCharacter( title );
				if (findFlag3 != FALSE) {
					// �I�u�W�F�N�g����

					data.SetTitle( title );
					data.SetURL( url );
					strDate.Trim();
					data.SetDate( strDate );
					out_.push_back( data );
				}
				
				//<td xxx>�^�O�ɓ��t������̂ł��̍s�̍Ō��<td>�^�O����؂�o��
				CString td = target;
				bool bFindTd = false ;
				while( util::GetAfterSubString( td, L"<td", td ) != -1 ) {
					bFindTd = true;
				}
				if( bFindTd ){
					CString date = L"";
					// ">"��"<"�̊Ԃ���t�Ƃ��Ē��o����
					if( util::GetBetweenSubString( td, L">", L"<", date ) == -1 ){
						// "<"��������Ȃ���΍s�̍Ō�܂ł𒊏o����
						util::GetBetweenSubString( td, L">", L"\n" ,date );
					}
					strDate = YearMonth + date + L"��";
				}
			}
		}

		// �O�A���̃����N������΁A�ǉ�����B
		if( !backLink.GetTitle().IsEmpty() ) {
			out_.insert( out_.begin(), backLink );
		}
		if( !nextLink.GetTitle().IsEmpty() ) {
			out_.push_back( nextLink );
		}

		MZ3LOGGER_DEBUG( L"ShowCalendarParser.parse() finished." );
		return true;
	}

private:
	/// �u����\���v�A�u�O��\���v�̃����N�𒊏o����
	static bool parseNextBackLink( CMixiData& nextLink, CMixiData& backLink, CString str )
	{
		// ���K�\���̃R���p�C���i���̂݁j
/*
	<a href="show_calendar.pl?year=2007&month=9&pref_id=13">&lt;&lt;&nbsp;�O�̌�</a>
	<a href="show_calendar.pl?year=2007&month=10&pref_id=13">����</a>
	<a href="show_calendar.pl?year=2007&month=11&pref_id=13">���̌�&nbsp;&gt;&gt;</a>&nbsp;
*/
		static MyRegex reg;
		if( !util::CompileRegex( reg, L"<a href=\"show_calendar.pl([?].+?)\">.*?(�O�̌�|����|���̌�).*?</a>" ) ) {
			MZ3LOGGER_FATAL( FAILED_TO_COMPILE_REGEX_MSG );
			return false;
		}

		return parseNextBackLinkBase( nextLink, backLink, str,
			reg, L"show_calendar.pl", ACCESS_LIST_CALENDAR );
	}
};

}//namespace mixi

#endif	// BT_MZ3
