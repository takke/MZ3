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


}//namespace mixi

#endif	// BT_MZ3
