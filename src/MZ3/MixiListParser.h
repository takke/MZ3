/*
 * Copyright (c) MZ3 Project Team All rights reserved.
 * Code licensed under the BSD License:
 * http://www.mz3.jp/license.txt
 */
#pragma once

#include "MixiParser.h"
#include "xml2stl.h"

#ifdef BT_MZ3

/// mixi�p�p�[�T
namespace mixi {

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


}//namespace mixi

#endif	// BT_MZ3
