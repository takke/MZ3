#pragma once

#include "MyRegex.h"
#include "HtmlArray.h"
#include "MixiParserUtil.h"
#include "xml2stl.h"

/// mixi �pHTML�p�[�T
namespace mixi {

/// mixi �p�p�[�T�̊�{�N���X
class MixiParserBase 
{
public:
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
class MixiContentParser : public MixiParserBase
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
					break;
				default:
					// �ȍ~�̉�͕͂s�v�B
					return;
				}
				continue;
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
};

//���������ʁ�����
/**
 * home.pl ���O�C����ʗp�p�[�T
 * �y���O�C����ʁz
 * http://mixi.jp/home.pl
 */
class LoginPageParser : public MixiParserBase
{
public:
	/**
	 * ���O�A�E�g���������`�F�b�N����
	 */
	static bool isLogout( LPCTSTR szHtmlFilename )
	{
		// �ő�� N �s�ڂ܂Ń`�F�b�N����
		const int CHECK_LINE_NUM_MAX = 300;

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
	 * </ul>
	 */
	static bool parse( const CHtmlArray& html_ )
	{
		MZ3LOGGER_DEBUG( L"HomeParser.parse() start." );

		INT_PTR count = html_.GetCount();

		int index = 0;

		// �V�����b�Z�[�W���̎擾
		int messageNum = GetNewMessageCount( html_, 100, count, index);
		if (messageNum != 0) {
			theApp.m_newMessageCount = messageNum;

			// �o�C�u�����Ⴄ
			// NLED_SETTINGS_INFO led;
			//led.LedNum = ::NLedSetDevice(0, 
		}

		// �V���R�����g���̎擾
		int commentNum = GetNewCommentCount( html_, 100, count, index);
		if (commentNum != 0) {
			theApp.m_newCommentCount = commentNum;
		}

		if (wcslen(theApp.m_loginMng.GetOwnerID()) == 0) {
			// OwnerID �����擾�Ȃ̂ŉ�͂���
			MZ3LOGGER_DEBUG( L"OwnerID �����擾�Ȃ̂ŉ�͂��܂�" );

			for (int i=0; i<count; i++) {
				const CString& line = html_.GetAt(i);
				if( util::LineHasStringsNoCase( line, L"<a", L"href=", L"list_community.pl" ) ) {

					// list_community.pl �ȍ~�𒊏o
					CString after;
					util::GetAfterSubString( line, L"list_community.pl", after );

					CString id;
					if( util::GetBetweenSubString( after, L"id=", L"\"", id ) == -1 ) {
						MZ3LOGGER_ERROR( L"list_community.pl �̈����� id �w�肪����܂���B line[" + line + L"], after[" + after + L"]" );
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
			else if (line.Find(_T("<!-- ���m�点���b�Z�[�W �����܂� -->")) != -1) {
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
			else if (line.Find(_T("<!-- ���m�点���b�Z�[�W �����܂� -->")) != -1) {
				retIndex = i;
				break;
			}
		}

		return commentNum;
	}

};


//���������L������
/**
 * [list] list_diary.pl �p�p�[�T
 * �y�ŋ߂̓��L�ꗗ(�����̓��L�ꗗ)�z
 * http://mixi.jp/list_diary.pl
 */
class ListDiaryParser : public MixiListParser
{
public:
	static bool parse( CMixiDataList& out_, const CHtmlArray& html_ )
	{
		MZ3LOGGER_DEBUG( L"ListDiaryParser.parse() start." );

		INT_PTR count = html_.GetCount();

		BOOL findFlag = FALSE;
		CMixiData backLink;
		CMixiData nextLink;

		CMixiData data;
		data.SetAccessType( ACCESS_MYDIARY );

		for (int i=170; i<count; i++) {
			const CString& str = html_.GetAt(i);

			if( findFlag ) {
				if( str.Find(_T("/bodyMainAreaMain")) != -1 ) {
					// �I��
					break;
				}

				// �u�O��\���v�u����\���v�̒��o
				LPCTSTR key = L"����\��";
				if( str.Find( key ) != -1 ) {
					// �����N���ۂ��̂Ő��K�\���}�b�`���O�Œ��o
					if( parseNextBackLink( nextLink, backLink, str ) ) {
						continue;
					}
				}
			}

			// ���O�̎擾
			// "�̓��L</h2>" ������s�B
			if (str.Find(_T("�̓��L")) != -1) {
				// "<h2>" �� "�̓��L</h2>" �ň͂܂ꂽ�����𒊏o����
				CString name;
				util::GetBetweenSubString( str, L"<h2>", L"�̓��L</h2>", name );

				// ���O
				data.SetName( name );
				data.SetAuthor( name );
			}

			const CString& key = _T("<dt><input");
			if (str.Find(key) != -1) {
				findFlag = TRUE;

				// �^�C�g��
				// ��͑ΏہF
				//<dt><input name="diary_id" type="checkbox" value="xxxxx"  /><a href="view_diary.pl?id=xxxx&owner_id=xxxx">�^�C�g��</a><span><a href="edit_diary.pl?id=xxxx">�ҏW����</a></span></dt>

				CString buf;
				util::GetBetweenSubString( str, key, L"</dt>", buf );

				CString title;
				util::GetBetweenSubString( buf, L"\">", L"</a><span><a", title );
				data.SetTitle( title );

				// ���t
				//<dd>2007�N06��18��12:10</dd>
				const CString& str = html_.GetAt(i+1);	
				ParserUtil::ParseDate(str, data);

				for (int j=i; j<count; j++) {
					const CString& str = html_.GetAt(j);

					LPCTSTR key = _T("<a href=\"view_diary.pl?id");
					if (str.Find(key) != -1) {
						CString uri;
						util::GetBetweenSubString( str, _T("<a href=\""), L"\">", uri );
						data.SetURL( uri );

						// �h�c��ݒ�
						data.SetID( MixiUrlParser::GetID(uri) );

						i = j;
						break;
					}
				}

				out_.push_back( data );
			}
		}
		// �O�A���̃����N������΁A�ǉ�����B
		if( !backLink.GetTitle().IsEmpty() ) {
			out_.insert( out_.begin(), backLink );
		}
		if( !nextLink.GetTitle().IsEmpty() ) {
			out_.push_back( nextLink );
		}

		MZ3LOGGER_DEBUG( L"ListDiaryParser.parse() finished." );
		return true;
	}

private:
	/// �u����\���v�A�u�O��\���v�̃����N�𒊏o����
	static bool parseNextBackLink( CMixiData& nextLink, CMixiData& backLink, CString str )
	{
		// ���K�\���̃R���p�C���i���̂݁j
		static MyRegex reg;
		if( !util::CompileRegex( reg, L"<a href=list_diary.pl([?]page=[^>]+)>([^<]+)</a>" ) ) {
			MZ3LOGGER_FATAL( FAILED_TO_COMPILE_REGEX_MSG );
			return false;
		}

		return parseNextBackLinkBase( nextLink, backLink, str, reg, L"list_diary.pl", ACCESS_LIST_MYDIARY );
	}

};

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
											//�^�C�g�����Z�b�g
											data.SetTitle(title);
											//���[�v�𔲂���t���O���Z�b�g
											findFlag = true;
										}
										//���O�𒊏o
										CString author;
										util::GetBetweenSubString( target, L"&nbsp;(", L")", author );
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

/**
 * [list] new_friend_diary.pl �p�p�[�T
 * �y�}�C�~�N�ŐV���L�ꗗ�z
 * http://mixi.jp/new_friend_diary.pl
 */
class ListNewFriendDiaryParser : public MixiListParser
{
public:
	static bool parse( CMixiDataList& out_, const CHtmlArray& html_ )
	{
		MZ3LOGGER_DEBUG( L"ListNewFriendDiaryParser.parse() start." );

		// �u���v�A�u�O�v�̃����N
		CMixiData backLink;
		CMixiData nextLink;

		INT_PTR count = html_.GetCount();

		// ���L�J�n�t���O�̒T��
		int iLine = 100;
		for (; iLine<count; iLine++) {
			const CString& line = html_.GetAt(iLine);

			if (util::LineHasStringsNoCase( line, _T("newFriendDiaryArea"))) {
				// ���L�J�n
				break;
			}
		}

		// �e���L���ڂ̎擾
		// ���L���J�n���Ă��Ȃ���΍ŏI�s�Ȃ̂Ń��[�v���Ȃ��B
		bool bDataFound   = false;
		for (; iLine<count; iLine++) {
			const CString& line = html_.GetAt(iLine);

			// �u����\���v�A�u�O��\���v�̃����N�𒊏o����
			if( parseNextBackLink( nextLink, backLink, line ) ) {
				continue;
			}

			if (util::LineHasStringsNoCase( line, L"<dt>", L"</dt>" ) ) {

				bDataFound = true;

				CMixiData data;
				data.SetAccessType(ACCESS_DIARY);

				//--- �����̒��o
				//<dt>2007�N10��02��&nbsp;22:22</dt>
				ParserUtil::ParseDate(line, data);

				//--- ���o���̒��o
				//<dd><a href="view_diary.pl?id=xxx&owner_id=xxx">�^�C�g��</a> (�Ȃ܂�)<div style="visibility: hidden;" class="diary_pop" id="xxx"></div>
				// or
				//<dd><a href="view_diary.pl?url=xxx&owner_id=xxx">�^�C�g��</a> (�Ȃ܂�)
				iLine++;
				const CString& line2 = html_.GetAt(iLine);

				CString after;
				util::GetAfterSubString( line2, L"<a", after );
				CString title;
				util::GetBetweenSubString( after, L">", L"<", title );
				data.SetTitle(title);

				// �t�q�h
				if (util::LineHasStringsNoCase( line2, L"list_diary.pl" ) ) {
					iLine += 5;
					continue;
				}
				CString url;
				util::GetBetweenSubString( after, L"\"", L"\"", url );
				data.SetURL(url);

				// �h�c��ݒ�
				data.SetID( MixiUrlParser::GetID(url) );

				// ���O
				CString name;
				if (util::GetBetweenSubString( line2, L"</a> (", L")<div", name) < 0) {
					// ���s�����̂� ")" �܂ł𒊏o
					util::GetBetweenSubString( line2, L"</a> (", L")", name);
				}

				data.SetName(name);
				data.SetAuthor(name);

				out_.push_back( data );
				iLine += 5;
			}
			else if (bDataFound && line.Find(_T("pageNavigation01")) != -1) {
				// �I���^�O����
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

		MZ3LOGGER_DEBUG( L"ListNewFriendDiaryParser.parse() finished." );
		return true;
	}

private:
	/// �u����\���v�A�u�O��\���v�̃����N�𒊏o����
	static bool parseNextBackLink( CMixiData& nextLink, CMixiData& backLink, CString str )
	{
		// ���K�\���̃R���p�C���i���̂݁j
		static MyRegex reg;
		if( !util::CompileRegex( reg, L"<a href=new_friend_diary.pl([?]page=[^>]+)>([^<]+)</a>" ) ) {
			MZ3LOGGER_FATAL( FAILED_TO_COMPILE_REGEX_MSG );
			return false;
		}

		if( parseNextBackLinkBase( nextLink, backLink, str, reg, L"new_friend_diary.pl", ACCESS_LIST_DIARY ) ) {
			// Name �v�f�͕s�v�Ȃ̂ō폜�i�ڍ׃��X�g�̉E���ɕ\������Ă��܂��̂�������邽�߂̎b�菈�u�j
			nextLink.SetName( L"" );
			backLink.SetName( L"" );
			return true;
		}
		return false;
	}

};

/**
 * [list] new_comment.pl �p�p�[�T
 * �y���L�R�����g�L�������ꗗ�z
 * http://mixi.jp/new_comment.pl
 */
class NewCommentParser : public MixiListParser
{
public:
	static bool parse( CMixiDataList& out_, const CHtmlArray& html_ )
	{
		MZ3LOGGER_DEBUG( L"NewCommentParser.parse() start." );

		INT_PTR count = html_.GetCount();

		BOOL bEntryStarted = FALSE;
		BOOL bItemFound = FALSE;

		//int index;
		for (int i=160; i<count; i++) {

			const CString& line = html_.GetAt(i);

			if (bEntryStarted == FALSE) {
				if (util::LineHasStringsNoCase( line, L"entryList01") ) {
					// �J�n�t���O�����B
					// �Ƃ肠����N�s��������B
					i += 3;
					bEntryStarted = TRUE;
				}
				continue;
			}
			else {

				if (util::LineHasStringsNoCase( line, L"<dt>") ) {
					// ���ڔ���
					bItemFound = TRUE;

					CMixiData data;
					data.SetAccessType(ACCESS_DIARY);

					// ���t
					// <dt>2007�N10��01��&nbsp;01:11</dt>
					ParserUtil::ParseDate(line, data);
					MZ3LOGGER_DEBUG( L"date : " + data.GetDate() );

					// ���o���^�C�g��
					//<dd><a href="view_diary.pl?id=xxx&owner_id=yyy&comment_count=2">�^�C�g��(2)</a>&nbsp;(�Ȃ܂�)</dd>
					i += 1;
					const CString& line2 = html_.GetAt(i);

					CString title;
					util::GetBetweenSubString( line2, L"\">", L"</a>", title );
					data.SetTitle(title);
					MZ3LOGGER_DEBUG( L"title : " + data.GetTitle() );

					// URL
					CString url;
					util::GetBetweenSubString( line2, L"href=\"", L"\"", url );
					data.SetURL(url);
					MZ3LOGGER_DEBUG( L"URL : " + data.GetURL() );

					// �h�c��ݒ�
					data.SetID( MixiUrlParser::GetID(url) );

					// ���O
					CString author;
					if( util::GetBetweenSubString( line2, L"&nbsp;(", L")</dd>", author ) < 0 ) {
						// not found.
						continue;
					}

					data.SetName( author );
					data.SetAuthor( author );
					TRACE(_T("%s\n"), data.GetName());

					out_.push_back(data);
				}
				else if (line.Find(_T("/newCommentArea")) != -1 && bItemFound) {
					// �I���^�O����
					break;
				}

			}
		}

		MZ3LOGGER_DEBUG( L"NewCommentParser.parse() finished." );
		return true;
	}
};

/**
 * [content] view_diary.pl �p�p�[�T
 * �y���L�ڍׁz
 * http://mixi.jp/view_diary.pl
 */
class ViewDiaryParser : public MixiContentParser
{
public:
	static bool parse( CMixiData& data_, const CHtmlArray& html_ )
	{
		MZ3LOGGER_DEBUG( L"ViewDiaryParser.parse() start." );

		data_.ClearAllList();

		INT_PTR lastLine = html_.GetCount();

		// �����̓��L���ǂ����𔻒f����i���O�̎擾�̂��߁j
		bool bMyDiary = true;
		for( int i=170; i<min(100,lastLine); i++ ) {
			const CString& line = html_.GetAt( i );
			if( util::LineHasStringsNoCase( line, L"diaryTitleFriend" ) ) {
				// ��L������Ƃ������Ƃ́A�����̓��L�ł͂Ȃ��B
				bMyDiary = false;
				break;
			}
		}

		// �ŏ���10�s�ڂ܂ł��������A<title> �^�O����͂��邱�ƂŁA
		// �^�C�g�����擾����
		for( int i=3; i<min(10,lastLine); i++ ) {
			const CString& line = html_.GetAt( i );
			CString title;
			if( util::GetBetweenSubString( line, L"<title>[mixi] ", L"</title>", title ) >= 0 ) {
				// �����B
				// �����̓��L�Ȃ�@�@�u<title>[mixi] �^�C�g��</title>�v
				// �����ȊO�̓��L�Ȃ�u<title>[mixi] ���O | �^�C�g��</title>�v
				// �Ƃ����`���Ȃ̂ŁA�^�C�g���������𒊏o
				int idx = title.Find( L" | " );
				if( idx >= 0 ) {
					// �^�C�g�����������o
					title = title.Mid( idx + wcslen(L" | ") );
				}else{
					// ������Ȃ���Ύ����̓��L�Ɣ��f���A���̂܂܃^�C�g���ɁB
				}
				data_.SetTitle( title );
			}
		}

		// �u�ŋ߂̓��L�v�̎擾
		bool bStartRecentDiary = false;
		for( int iLine=300; iLine<lastLine; iLine++ ) {
			// <h3>�ŋ߂̓��L</h3>
			// ������΁u�ŋ߂̓��L�v�J�n�Ƃ݂Ȃ��B
			const CString& line = html_.GetAt( iLine );
			if( util::LineHasStringsNoCase( line, L"<h3>", L"�ŋ߂̓��L" ) ) {
				bStartRecentDiary = true;
				continue;
			}

			if( bStartRecentDiary ) {
				// </ul>
				// ������΁u�ŋ߂̓��L�v�I���Ƃ݂Ȃ��B
				if( util::LineHasStringsNoCase( line, L"</ul>" ) ) {
					break;
				}

				// ���
				// ���L�̌`���Łu�ŋ߂̓��L�v�����N�����݂���
				// <li><a href="view_diary.pl?id=xxx&owner_id=xxx">�����Ƃ�</a></li>
				if( util::LineHasStringsNoCase( line, L"view_diary.pl" ) ) {
					CString url;
					int idx = util::GetBetweenSubString( line, L"<a href=\"", L"\"", url );
					if( idx >= 0 ) {
						CString buf = line.Mid( idx );
						// buf:
						// >�����Ƃ�</a></li>

						// �^�C�g�����o
						CString title;
						if( util::GetBetweenSubString( buf, L">", L"<", title ) > 0 ) {
							CMixiData::Link link( url, title );
							data_.m_linkPage.push_back( link );
						}
					}
				}
			}
		}

		// ���L�{�����R�����g���
		bool bStartDiary = false;	// ���L�{���J�n�t���O
		bool bEndDiary   = false;	// ���L�{���I���t���O
		for (int i=180; i<lastLine; i++) {
			const CString& line = html_.GetAt(i);

			if (bStartDiary == false) {

				//�����̒��o	
				if( util::LineHasStringsNoCase( line, L"<dt>", L"<span>", L"edit_diary.pl" ) ) {
					const CString& line2 = html_.GetAt( i+1 );
					//<dd>2007�N10��02�� 22:22</dd>
					CString date;
					util::GetBetweenSubString( line2, L"<dd>", L"</dd>", date );
					data_.SetDate( date );
				}
			
				// ���L�J�n�t���O�𔭌�����܂ŉ�
				if (util::LineHasStringsNoCase( line, L"<div id=\"diary_body\">" ) ) {
					// ���L�J�n�t���O�����i���L�{�������j
					bStartDiary = true;

					// �Ƃ肠�������s�o��
					data_.AddBody(_T("\r\n"));

					// ���L�̒���
					{
						// �t���O�̂T�O�s���炢�O�ɁA���L�̒��҂�����͂��B
						//<div class="diaryTitle clearfix">
						//<h2>�������̓��L</h2>
						for( int iBack=-50; iBack<0; iBack++ ) {
							const CString& line = html_.GetAt( i+iBack );
							if (util::LineHasStringsNoCase(line, L"<h2>", L"</h2>")) {
								// �����̓��L�Ȃ�uXXX�̓��L�v�A�����ȊO�Ȃ�uXXX����̓��L�v�̂͂��B
								// ���̋K���Œ��҂���́B
								CString author;
								if( bMyDiary ) {
									util::GetBetweenSubString( line, L">", L"�̓��L<", author );
								}else{
									util::GetBetweenSubString( line, L">", L"����̓��L<", author );
								}
								// ���Ґݒ�
								data_.SetAuthor( author );
								data_.SetName( author );
								break;
							}
						}
					}

					// ���J���x��
					{
						// �t���O�̂T�O�s���炢�O�ɁA�u�F�l�܂Ō��J�v�Ƃ�������񂪂���͂��B
						// <img src="http://img.mixi.jp/img/diary_icon1.gif" alt="�F�l�܂Ō��J" height="20" hspace="5" width="22"></td>
					}

					// ���L�̓Y�t�摜�擾
					parseImageLink( data_, html_, i );

					// "<div" �̑O������
					CString str = line.Mid( line.Find(L"<div") );

					// ���݂̍s����́A�ǉ��B
					CString before;
					if( util::GetBeforeSubString( str, L"</div>", before ) > 0 ) {
						// ���̂P�s�ŏI���
						ParserUtil::AddBodyWithExtract( data_, before );
						bEndDiary = true;
					}else{
						// ����pscript�^�O������������script�^�O�I���܂ŉ�́B
						if(! ParserUtil::ExtractVideoLinkFromScriptTag( data_, i, html_ ) ) {
							// script�^�O�������Ȃ̂ŁA��́�����
							ParserUtil::AddBodyWithExtract( data_, line );
						}
					}
				}
				else if (line.Find(_T("����͊O���u���O���g���Ă��܂��B")) != -1) {
					// �O���u���O���
					parseExternalBlog( data_, html_, i );
					break;
				}
			}
			else {
				// ���L�J�n�t���O�����ς݁B

				// �I���^�O�܂Ńf�[�^�擾
				if (line.Find(_T("/viewDiaryBox")) != -1 ) {
					bEndDiary = true;
				}

				// �I���^�O�܂Ńf�[�^�擾
				if (line.Find(_T("</div>")) != -1 ) {
					bEndDiary = true;
				}

				if( bEndDiary == false ) {
					// �{���I���^�O������
					// ���L�{�����

					// ����pscript�^�O������������script�^�O�I���܂ŉ�́B
					if(! ParserUtil::ExtractVideoLinkFromScriptTag( data_, i, html_ ) ) {
						// script�^�O�������Ȃ̂ŁA��́�����
						ParserUtil::AddBodyWithExtract( data_, line );
					}

				} else {
					// �I���^�O����
					if (line.Find(_T("</div>")) != -1) {
						ParserUtil::AddBodyWithExtract( data_, line );
					}

					// �R�����g�擾
					i += 10;
					data_.ClearChildren();

					int cmtNum = 0;		// �R�����g�ԍ�
					int index = i;
					while( index < lastLine ) {
						cmtNum++;
						index  = parseDiaryComment(index, lastLine, data_, html_, cmtNum);
						if (index == -1) {
							break;
						}
					}
					if (index == -1 || index >= lastLine) {
						break;
					}
				}
			}
		}

		MZ3LOGGER_DEBUG( L"ViewDiaryParser.parse() finished." );
		return true;
	}

private:
	/// �O���u���O���
	static bool parseExternalBlog( CMixiData& mixi_, const CHtmlArray& html_, int i )
	{
		// �O���u���O�t���O�𗧂Ă�
		mixi_.SetOtherDiary(TRUE);

		// �Ƃ肠�������s
		mixi_.AddBody(_T("\r\n"));

		// �{�����
		int lastLine = html_.GetCount();
		for (; i<lastLine; i++ ) {
			const CString& line = html_.GetAt(i);

			if (util::LineHasStringsNoCase(line, L"</div>")) {
				break;
			}

			// �{���ǉ�
			ParserUtil::AddBodyWithExtract(mixi_, line);
		}

		return true;
	}

	/// ���L�̓Y�t�摜�擾
	static bool parseImageLink( CMixiData& data_, const CHtmlArray& html_, int iLine_ )
	{
		// �t���O��N�s�O�ɉ摜�����N�����邩���B
		int n_images_begin = -25;
		int n_images_end   = -1;
		bool bImageFound = false;
		for( int iBack=n_images_begin; iBack<=n_images_end; iBack++ ) {
			if( iLine_+iBack >= html_.GetCount() ) {
				break;
			}
			CString line = html_.GetAt( iLine_ +iBack );
			if (line.Find(_T("MM_openBrWindow('")) != -1) {
				// �摜�����B
				// �ǉ��B

				// �擪�̃^�u���폜���邽�߂� "<" �ȑO���폜
				int index = line.Find( L"<" );
				if( index > 0 ) {
					line = line.Mid( index );
				}

				ParserUtil::AddBodyWithExtract( data_, line );

				bImageFound = true;
			}
		}
		if( bImageFound ) {
			// �摜������΁A���̌��ɉ��s��ǉ����Ă����B
			data_.AddBody(L"\r\n");
		}
		return bImageFound;
	}

	/**
	 * �R�����g�擾
	 *
	 * @param sIndex [in] �J�n�C���f�b�N�X
	 * @param eIndex [in] �I���C���f�b�N�X
	 * @param data   [in/out] CMixiData* �f�[�^
	 * @param cmtNum [in] �R�����g�ԍ�
	 */
	static int parseDiaryComment(int sIndex, int eIndex, CMixiData& data_, const CHtmlArray& html_, int cmtNum)
	{
		CString name;
		CString date;
		CString comment;

		CString str;

		BOOL findFlag = FALSE;

		int retIndex = eIndex;

		CMixiData cmtData;		// �R�����g�f�[�^

		int index;
		CString buf;

		for (int i=sIndex; i<eIndex; i++) {
			str = html_.GetAt(i);

			if (findFlag == FALSE) {
				if( util::LineHasStringsNoCase( str, L"add_comment.pl" ) ||	// �R�����g�Ȃ�
					util::LineHasStringsNoCase( str, L"<!--/comment-->" ) ) // �R�����g�S�̂̏I���^�O����
				{
					parsePostURL( i, data_, html_ );
					return -1;
				}

				if ((index = str.Find(_T("show_friend.pl"))) != -1) {
					// �R�����g�w�b�_�擾

					// �h�c
					cmtData.SetCommentIndex(cmtNum);

					// ���O
					buf = str.Mid(index);
					MixiUrlParser::GetAuthor(buf, &cmtData);

					// ����
					for (int j=i+3; j>0; j--) {
						str = html_.GetAt(j);
						if (str.Find(_T("��&nbsp;")) != -1) {
							ParserUtil::ParseDate(str, cmtData);
							break;
						}
					}

					findFlag = TRUE;

				}
			}
			else {

				if (str.Find(_T("<dd>")) != -1) {
					// �R�����g�{���擾
					i++;
					str = html_.GetAt(i);

					if (str.Find(_T("<dd>")) != 0) {
						cmtData.AddBody(_T("\r\n"));
					}

					ParserUtil::AddBodyWithExtract( cmtData, str );

					i++;
					for( ; i<eIndex; i++ ) {
						str = html_.GetAt(i);
						if (str.Find(_T("</dd>")) != -1) {
							// �I���^�O���o�Ă����̂ł����ŏI���
							retIndex = i+5;
							break;
						}

						ParserUtil::AddBodyWithExtract( cmtData, str );
					}
					break;
				}
			}

		}

		if( findFlag ) {
			data_.AddChild(cmtData);
		}
		return retIndex;
	}

};


//�������R�~���j�e�B������
/**
 * [list] new_bbs.pl �p�p�[�T
 * �y�R�~���j�e�B�ŐV�������݈ꗗ�z
 * http://mixi.jp/new_bbs.pl
 */
class NewBbsParser : public MixiListParser
{
public:
	static bool parse( CMixiDataList& out_, const CHtmlArray& html_ )
	{
		MZ3LOGGER_DEBUG( L"NewBbsParser.parse() start." );

		INT_PTR count = html_.GetCount();

		// �u���v�A�u�O�v�̃����N
		CMixiData backLink;
		CMixiData nextLink;

		BOOL dataFind = FALSE;

		for (int i=140; i<count; i++) {
			const CString& str = html_.GetAt(i);

			// �u���v�A�u�O�v�̃����N
			// ���ڔ����O�ɂ̂ݑ��݂���
			if( !dataFind ) {
				// �u����\���v�A�u�O��\���v�̃����N�𒊏o����
				if( parseNextBackLink( nextLink, backLink, str ) ) {
					continue;
				}
			}

			// ���ڒT��
			// <dt class="iconTopic">2007�N10��01��&nbsp;22:14</dt>
			if( util::LineHasStringsNoCase( str, L"<dt", L"class", L"iconTopic" ) ||
				util::LineHasStringsNoCase( str, L"<dt", L"class", L"iconEvent" ) ||
				util::LineHasStringsNoCase( str, L"<dt", L"class", L"iconEnquete" ) )
			{
				dataFind = TRUE;

				CMixiData data;

				// ���t
				ParserUtil::ParseDate(str, data);

				// ���o��
				i += 1;
				CString str2 = html_.GetAt(i);
				// <dd><a href="view_bbs.pl?id=20728968&comment_count=3&comm_id=1198460">�y�`���b�g�z�W���Ixxx</a> (MZ3 -Mixi for ZERO3-)</dd>
				CString after;
				if (!util::GetAfterSubString( str2, L"href=", after )) {
					MZ3LOGGER_ERROR(L"�擾�ł��܂���:" + str );
					return false;
				}
				CString title;
				util::GetBetweenSubString( after, L">", L"<", title );
				
				//�A���P�[�g�A�C�x���g�̏ꍇ�̓^�C�g���̑O�Ƀ}�[�N��t����
				if(util::LineHasStringsNoCase( str, L"<dt", L"class", L"iconEvent" )){
					data.SetTitle(L"�y���z" + title);
				}else if(util::LineHasStringsNoCase( str, L"<dt", L"class", L"iconEnquete" )){
					data.SetTitle(L"�y���z" + title);
				}else{
					data.SetTitle(title);
				}

				// �t�q�h
				CString href;
				util::GetBetweenSubString( str2, L"\"", L"\"", href );

				// &�ŕ�������
				while (href.Replace(_T("&amp;"), _T("&")));

				data.SetURL(href);
				data.SetCommentCount(
					MixiUrlParser::GetCommentCount( href ) );

				// URL �ɉ����ăA�N�Z�X��ʂ�ݒ�
				data.SetAccessType( util::EstimateAccessTypeByUrl(href) );

				// �h�c��ݒ�
				data.SetID( MixiUrlParser::GetID(href) );

				// �R�~���j�e�B��
				CString communityName;
				util::GetBetweenSubString( str2, L"</a>", L"</dd>", communityName );

				// ���`�F�ŏ��ƍŌ�̊��ʂ���菜��
				communityName.Trim();
				util::GetBetweenSubString( communityName, L"(", L")", communityName );
				data.SetName(communityName);
				out_.push_back( data );
			}
			else if ( dataFind ) {
				if( str.Find(_T("</ul>")) != -1 ) {
					// �I���^�O����
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

		MZ3LOGGER_DEBUG( L"NewBbsParser.parse() finished." );
		return true;
	}

private:
	/// �u����\���v�A�u�O��\���v�̃����N�𒊏o����
	static bool parseNextBackLink( CMixiData& nextLink, CMixiData& backLink, CString str )
	{
		// ���K�\���̃R���p�C���i���̂݁j
		static MyRegex reg;
		if( !util::CompileRegex( reg, L"<a href=new_bbs.pl([?]page=[^>]+)>([^<]+)</a>" ) ) {
			MZ3LOGGER_FATAL( FAILED_TO_COMPILE_REGEX_MSG );
			return false;
		}

		if( parseNextBackLinkBase( nextLink, backLink, str, reg, L"new_bbs.pl", ACCESS_LIST_NEW_BBS ) ) {
			// Name �v�f�͕s�v�Ȃ̂ō폜�i�ڍ׃��X�g�̉E���ɕ\������Ă��܂��̂�������邽�߂̎b�菈�u�j
			nextLink.SetName(L"");
			backLink.SetName(L"");
			return true;
		}
		return false;
	}

};

/**
 * [list] new_bbs_comment.pl �p�p�[�T
 * �y�R�~���j�e�B�R�����g�L�������z
 * http://mixi.jp/new_bbs_comment.pl
 */
class ListNewBbsCommentParser : public MixiListParser
{
public:
	static bool parse( CMixiDataList& out_, const CHtmlArray& html_ )
	{
		MZ3LOGGER_DEBUG( L"ListNewBbsCommentParser.parse() start." );

		// NewBbsParser �ɈϏ�
		bool rval = NewBbsParser::parse( out_, html_ );

		MZ3LOGGER_DEBUG( L"ListNewBbsCommentParser.parse() finished." );
		return rval;
	}
};

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
		if( !util::CompileRegex( reg, L"<a href=list_community.pl([?][^>]+)>([^<]+)</a>" ) ) {
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
		if( !util::CompileRegex( reg, L"<a href=list_bbs.pl([?][^>]+)>([^<]+)</a>" ) ) {
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

						MixiUrlParser::GetAuthor(line, &mixi);

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

				MixiUrlParser::GetAuthor( line, &cmtData );
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
				MixiUrlParser::GetAuthor( line, &data_ );
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
						MixiUrlParser::GetAuthor( Author, &cmtData );
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
				MixiUrlParser::GetAuthor( Author, &data_ );

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
						MixiUrlParser::GetAuthor( Author, &cmtData );
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


/**
 * [content] show_friend.pl �p�p�[�T
 * �y�v���t�B�[���z
 * http://mixi.jp/show_friend.pl
 */
class ShowFriendParser : public MixiContentParser
{
public:
	static bool parse( CMixiData& mixi, const CHtmlArray& html_ )
	{
		MZ3LOGGER_DEBUG( L"ShowFriendParser.parse() start." );

		mixi.ClearAllList();
		mixi.ClearChildren();
		mixi.SetName( L"" );
		mixi.SetDate( L"" );

		// html_ �̕�����
		std::vector<TCHAR> text;
		text.reserve( 10*1024 );	// �o�b�t�@�\��
		INT_PTR count = html_.GetCount();
		for (int i=0; i<count; i++) {
			const CString& line = html_.GetAt(i);
			size_t size = text.size();
			int new_size = wcslen(line);
			if (new_size>0) {
				text.resize( size+new_size );
				wcsncpy( &text[size], (LPCTSTR)line, new_size );
			}
		}

		// XML ���
		xml2stl::Container root;
		if (!xml2stl::SimpleXmlParser::loadFromText( root, text )) {
			MZ3LOGGER_ERROR( L"XML ��͎��s" );
			return false;
		}

		// ���O
		// /html/body/div[2]/div/div/div/div/h3
		try {
			const xml2stl::Node& h3 = root.getNode( L"html" )
										  .getNode( L"body" )
										  .getNode( L"div", 1 )
										  .getNode( L"div" )
										  .getNode( L"div" )
										  .getNode( L"div" )
										  .getNode( L"div" )
										  .getNode( L"h3" );
			CString name = h3.getTextAll().c_str();
			mixi.SetName( name );
			mixi.SetTitle( name );
			mixi.SetAuthor( name );
		} catch (xml2stl::NodeNotFoundException& e) {
			MZ3LOGGER_ERROR( util::FormatString( L"h3 not found... : %s", e.getMessage().c_str()) );
		}

		// ���[�U�摜
		// /html/body/div[2]/div/div/div/div/img
		try {
			const xml2stl::Node& img = root.getNode( L"html" )
										   .getNode( L"body" )
										   .getNode( L"div", 1 )
										   .getNode( L"div" )
										   .getNode( L"div" )
										   .getNode( L"div" )
										   .getNode( L"div" )
										   .getNode( L"img" );
			LPCTSTR url = img.getProperty( L"src" ).c_str();
			mixi.m_linkList.push_back( CMixiData::Link( url, L"���[�U�摜" ) );

			MZ3LOGGER_DEBUG( util::FormatString( L"user image : [%s]", url ) );
		} catch (xml2stl::NodeNotFoundException& e) {
			MZ3LOGGER_ERROR( util::FormatString( L"img not found... : %s", e.getMessage().c_str()) );
		}

		// �v���t�B�[����S�Ď擾���A�{���ɐݒ肷��B
		// /html/body/div[2]/div/div#bodyContents/div#profile/ul
		try {
			const xml2stl::Node& ul = root.getNode( L"html" )
										  .getNode( L"body" )
										  .getNode( L"div", xml2stl::Property(L"id", L"bodyArea") )
										  .getNode( L"div", xml2stl::Property(L"id", L"bodyMainArea") )
										  .getNode( L"div", xml2stl::Property(L"id", L"bodyContents") )
										  .getNode( L"div", xml2stl::Property(L"id", L"profile") )
										  .getNode( L"ul" );

			// �Ƃ肠�������s
			mixi.AddBody(_T("\r\n"));

			// �eli��ǉ����Ă���
			int nChildren = ul.getChildrenCount();
			for (int i=0; i<nChildren; i++) {
				const xml2stl::Node& li = ul.getNode(i);
				if (li.getName()!=L"li")
					continue;

				const xml2stl::Node& dl = li.getNode( L"dl" );

				// ���ڂ̖���
				CString target = util::FormatString( L"�� %s", dl.getNode(L"dt").getTextAll().c_str() );
				ParserUtil::AddBodyWithExtract( mixi, target );
				mixi.AddBody(_T("\r\n"));

				// ���ڂ̓��e
				const xml2stl::Node& dd = dl.getNode( L"dd" );
				target = dd.getTextAll().c_str();
				ParserUtil::AddBodyWithExtract( mixi, target );
				mixi.AddBody(_T("\r\n"));
				mixi.AddBody(_T("\r\n"));
			}
		} catch (xml2stl::NodeNotFoundException& e) {
			MZ3LOGGER_ERROR( util::FormatString( L"(�v���t�B�[���{��) not found... : %s", e.getMessage().c_str()) );
		}

		int nChildItemNumber = 1;

		// �ŐV�̓��L�擾
		// /html/body/div[2]/div/div[2]/div[3]/div/div[2]/dl
		try {
			const xml2stl::Node& dl = root.getNode( L"html" )
										  .getNode( L"body" )
										  .getNode( L"div", xml2stl::Property(L"id", L"bodyArea") )
										  .getNode( L"div" )
										  .getNode( L"div", xml2stl::Property(L"id", L"bodyContents") )
										  .getNode( L"div", xml2stl::Property(L"id", L"mymixiUpdate") )
										  .getNode( L"div", xml2stl::Property(L"id", L"newFriendDiary") )
										  .getNode( L"div", xml2stl::Property(L"class", L"contents") )
										  .getNode( L"dl" );

			// dt/span, dd/a �����݂ɏo������B
			int n = dl.getChildrenCount();

			CMixiData diaryItem;
			CMixiData::Link link( L"", L"" );

			// �Ƃ肠�������s
			diaryItem.AddBody(_T("\r\n"));

			for (int i=0; i<n; i++) {
				const xml2stl::Node& node = dl.getNode(i);
				if (node.getName() == L"dt") {
					CString date = node.getNode(L"span").getTextAll().c_str();
					
					// �����N���ݒ�
					link.text += date;

					// �{���ݒ�
					diaryItem.AddBody( util::FormatString(L"�� %s", date ) );
				}
				if (node.getName() == L"dd") {
					const xml2stl::Node& a = node.getNode(L"a");

					// �����N���ݒ�
					link.text += L" : ";
					link.text += a.getTextAll().c_str();

					// URL, ID��ݒ�
					link.url = a.getProperty( L"href" ).c_str();
					diaryItem.m_linkList.push_back(link);

					// �{���ɒǉ�
					ParserUtil::AddBodyWithExtract( diaryItem, util::FormatString(L" : %s", a.getTextAll().c_str()) );
					diaryItem.AddBody( L"\r\n" );
					ParserUtil::AddBodyWithExtract( diaryItem, util::FormatString(L" (%s)", link.url) );
					diaryItem.AddBody( L"\r\n" );
					diaryItem.AddBody( L"\r\n" );

					// ������
					link = CMixiData::Link( L"", L"" );
				}
			}
			// �o�^
			diaryItem.SetCommentIndex( nChildItemNumber++ );
			diaryItem.SetAuthor( L"�ŐV�̓��L" );
			mixi.AddChild( diaryItem );

		} catch (xml2stl::NodeNotFoundException& e) {
			MZ3LOGGER_ERROR( util::FormatString( L"(�ŐV�̓��L) not found... : %s", e.getMessage().c_str()) );
		}

		// �Љ�̎擾
		// /html/body/div[2]/div/div[2]/div[4]/div[2]
		try {
			const xml2stl::Node& div = root.getNode( L"html" )
										   .getNode( L"body" )
										   .getNode( L"div", xml2stl::Property(L"id", L"bodyArea") )
										   .getNode( L"div" )
										   .getNode( L"div", xml2stl::Property(L"id", L"bodyContents") )
										   .getNode( L"div", xml2stl::Property(L"id", L"intro") )
										   .getNode( L"div", xml2stl::Property(L"class", L"contents") );

			// dl �������B
			int n = div.getChildrenCount();
			for (int i=0; i<n; i++) {
				const xml2stl::Node& dl = div.getNode(i);
				if (dl.getName() != L"dl") {
					continue;
				}

				CMixiData introItem;
				introItem.AddBody( L"\r\n" );
				
				// dt/a[2] : ���O
				// dt/a[1]/img/@src : �摜
				const xml2stl::Node& dt = dl.getNode(L"dt");
				CString name = dt.getNode( L"a", 1 ).getTextAll().c_str();
				CString url  = dt.getNode( L"a", 1 ).getProperty(L"href").c_str();

				// dd/p class=relation  : �֌W
				// dd/p class=userInput : �Љ
				CString intro, relation;
				const xml2stl::Node& dd = dl.getNode(L"dd");
				for (int j=0; j<dd.getChildrenCount(); j++) {
					const xml2stl::Node& dd_sub = dd.getNode(j);
					if (dd_sub.isNode() && dd_sub.getName() == L"p") {
						CString className = dd_sub.getProperty(L"class").c_str();
						if (className==L"relation") {
							relation = dd_sub.getTextAll().c_str();
						} else if (className==L"userInput") {
							intro    = dd_sub.getTextAll().c_str();
						}
					}
				}

				if (!relation.IsEmpty()) {
					ParserUtil::AddBodyWithExtract( introItem, relation );
					introItem.AddBody( L"\r\n" );
				}
				if (!intro.IsEmpty()) {
					ParserUtil::AddBodyWithExtract( introItem, intro );

					// �o�^
					introItem.SetCommentIndex( nChildItemNumber++ );
					introItem.SetAuthor( util::FormatString( L"�Љ(%s)", name ) );
					introItem.SetAuthorID( mixi::MixiUrlParser::GetID(url) );
					introItem.SetURL( url );
					introItem.SetAccessType( ACCESS_PROFILE );
					mixi.AddChild( introItem );
				}
			}
		} catch (xml2stl::NodeNotFoundException& e) {
			MZ3LOGGER_ERROR( util::FormatString( L"(�Љ) not found... : %s", e.getMessage().c_str()) );
		}

		MZ3LOGGER_DEBUG( L"ShowFriendParser.parse() finished." );
		return true;
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
		 * - <td WIDTH=35 background=http://img.mixi.jp/img/bg_w.gif>...
		 *   ��������΂������獀�ڊJ�n�Ƃ݂Ȃ�
		 * - ��������+18�s�ڈȍ~��
		 *   <td ...><A HREF="view_news.pl?id=XXXXX&media_id=X"class="new_link">title</A>
		 *   �Ƃ����`���ŁuURL�v�Ɓu�^�C�g���v�����݂���B
		 * - ����4�s���
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
				if( str.Find( L"<img" ) == -1 || str.Find( L"bg_w.gif" ) == -1 ) {
					// �J�n�t���O������
					continue;
				}
				bInItems = true;
			}
			if( str.Find( L"<A" ) == -1 || str.Find( L"view_news.pl" ) == -1 ) {
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
				if( !util::CompileRegex( reg, L"\"(view_news.pl\\?id=[0-9]+\\&media_id=[0-9]+).+>(.+)</" ) ) {
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
				if( !util::CompileRegex( reg, L"list_news_media.pl.+>([^<]+)</" ) ) {
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
				data.SetTitle( title.c_str() );
				TRACE(_T("%s\n"), data.GetTitle());

				// URL ����
				CString url2 = L"http://news.mixi.jp/";
				url2 += url.c_str();
				data.SetURL( url2 );
				TRACE(_T("%s\n"), data.GetURL());

				// ���O
				data.SetName( author.c_str() );
				data.SetAuthor( author.c_str() );
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
		if( !util::CompileRegex( reg, L"<a href=list_news_category.pl([?][^>]+)>([^<]+)</a>" ) ) {
			MZ3LOGGER_FATAL( FAILED_TO_COMPILE_REGEX_MSG );
			return false;
		}

		return parseNextBackLinkBase( nextLink, backLink, str, reg, 
					L"http://news.mixi.jp/list_news_category.pl", ACCESS_LIST_NEWS );
	}

};

/**
 * [content] view_news.pl �p�p�[�T
 * �y�j���[�X�ڍׁz
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
				data_.SetTitle(title);
				break;
			}
		}

		/**
		 * ���j�F
		 * - CLASS="h130"
		 *   ��������΁A���̍s��
		 *   <td CLASS="h130">title</td>
		 *   �Ƃ����`���Łu�^�C�g���v�����݂���B
		 * - <td CLASS="h150">
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
				if( !util::LineHasStringsNoCase( line, L"<td CLASS=\"h150\">" ) ) {
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
			// �s���� </td></tr> ������ΏI���B
			LPCTSTR endTag = _T("</td></tr>");
			if( wcsncmp( line, endTag, wcslen(endTag) ) == 0 ) {
				// �I���^�O����
				break;
			}

			ParserUtil::AddBodyWithExtract( data_, line );
		}

		MZ3LOGGER_DEBUG( L"ViewNewsParser.parse() finished." );
		return true;
	}

};




//���������b�Z�[�W������
/**
 * [list] list_message.pl �p�p�[�T
 * �y���b�Z�[�W�ꗗ�z
 * http://mixi.jp/list_message.pl
 */
class ListMessageParser : public MixiListParser
{
public:
	static bool parse( CMixiDataList& out_, const CHtmlArray& html_ )
	{
		MZ3LOGGER_DEBUG( L"ListMessageParser.parse() start." );

		// �u���v�A�u�O�v�̃����N
		CMixiData backLink;
		CMixiData nextLink;

		INT_PTR count = html_.GetCount();

		bool bInMessages = false;

		for (int i=100; i<count; i++) {
			const CString& line = html_.GetAt(i);

			// <td><a HREF="view_message.pl?id=xxx&box=inbox">����</a></td>

			if (util::LineHasStringsNoCase( line, _T("view_message.pl")) ) {
				bInMessages = true;

				CMixiData data;
				data.SetAccessType(ACCESS_MESSAGE);

				// �����N���擾
				CString buf;
				if( util::GetAfterSubString( line, L"<a", buf ) < 0 ) {
					continue;
				}
				// buf: HREF="view_message.pl?id=xxx&box=inbox">����</a></td>

				CString link;
				if( util::GetBetweenSubString( buf, L"HREF=\"", L"\"", link ) < 0 ) {
					continue;
				}
				data.SetURL(link);

				// ������
				CString title;
				if( util::GetBetweenSubString( buf, L">", L"<", title ) < 0 ) {
					continue;
				}
				data.SetTitle(title);

				// ���O
				// �O�̍s����擾����
				const CString& line0 = html_.GetAt( i-1 );
				// line0: <td>�Ȃ܂�</td>
				if( util::GetBetweenSubString( line0, L"<td>", L"</td>", buf ) < 0 ) {
					continue;
				}
				data.SetName(buf);
				data.SetAuthor(buf);

				// ���̍s����A���t���擾
				const CString& line2 = html_.GetAt( i+1 );
				// line2: <td>04��08��</td></tr>
				if( util::GetBetweenSubString( line2, L"<td>", L"</td>", buf ) < 0 ) {
					continue;
				}
				data.SetDate( buf );

				out_.push_back( data );
			}

			if( bInMessages && util::LineHasStringsNoCase( line, L"</table>" ) ) {
				// �����I��
				break;
			}
		}

		MZ3LOGGER_DEBUG( L"ListMessageParser.parse() finished." );
		return true;
	}
};




/**
 * [content] view_message.pl �p�p�[�T
 * �y���b�Z�[�W�ڍׁz
 * http://mixi.jp/view_message.pl
 */
class ViewMessageParser : public MixiContentParser
{
public:
	static bool parse( CMixiData& data_, const CHtmlArray& html_ )
	{
		MZ3LOGGER_DEBUG( L"ViewMessageParser.parse() start." );

		data_.ClearAllList();

		INT_PTR count = html_.GetCount();


		for (int i=0; i<count; i++) {
			CString str = html_.GetAt(i);

			// ���t���o
			// <font COLOR=#996600>���@�t</font>&nbsp;:&nbsp;2007�N10��08�� 21��52��&nbsp;&nbsp;
			if( util::LineHasStringsNoCase( str, L"<font", L" COLOR=#996600", L">", L"���@�t" ) ) {
				CString buf = str;
				buf.Replace(_T("��"), _T(":"));
				ParserUtil::ParseDate(buf, data_);
				continue;
			}

			// ���o�lID���o
			if( util::LineHasStringsNoCase( str, L"<font", L" COLOR=#996600", L">", L"���o�l", L"show_friend.pl?id=" ) ) {
				CString buf;
				util::GetBetweenSubString( str, L"show_friend.pl?id=", L"\"", buf );
				data_.SetOwnerID(_wtoi(buf));
				continue;
			}

			// �{�����o
			if( util::LineHasStringsNoCase( str, L"<td", L"CLASS=", L"h120" ) ) {

				data_.AddBody(_T("\r\n"));
				ParserUtil::AddBodyWithExtract( data_, str );

				// �����s�� </td></tr> �����݂���ΏI���B
				if( util::LineHasStringsNoCase( str, L"</td></tr>" ) ) {
					break;
				}

				for (int j=i+1; j<count; j++) {
					str = html_.GetAt(j);
					ParserUtil::AddBodyWithExtract( data_, str );

					if( util::LineHasStringsNoCase( str, L"</td></tr>" ) ) {
						// �I���t���O����
						break;
					}
				}
				break;

			}
		}

		MZ3LOGGER_DEBUG( L"ViewMessageParser.parse() finished." );
		return true;
	}
};


//���������̑�������
/**
 * [list] list_friend.pl �p�p�[�T�B
 * �y�}�C�~�N�ꗗ�z
 * http://mixi.jp/list_friend.pl
 */
class ListFriendParser : public MixiListParser
{
public:
	static bool parse( CMixiDataList& out_, const CHtmlArray& html_ )
	{
		MZ3LOGGER_DEBUG( L"ListFriendParser.parse() start." );

		INT_PTR lastLine = html_.GetCount();

		// �u���v�A�u�O�v�̃����N
		CMixiData backLink;
		CMixiData nextLink;

		/**
		 * ���j�F
		 * �ӂ��� ul, li, div �v�f�ŕ���ł���̂łӂ��Ƀp�[�X���邾���B
		 */
		bool bInItems = false;	// ���ڊJ�n�H
		int iLine = 100;		// �Ƃ肠�����ǂݔ�΂�
		for( ; iLine+1<lastLine; iLine++ ) {
			const CString& line = html_.GetAt(iLine);

			if( !bInItems ) {
				// ���ڊJ�n�H
				// <div class="iconList03">
				if( util::LineHasStringsNoCase( line, L"<div", L"class=", L"iconList03" ) ) {
					bInItems = true;
				}
			}

			if( bInItems ) {
				// <div class="pageNavigation01"> ��������ΏI��
				if( util::LineHasStringsNoCase( line, L"<div", L"class=", L"pageNavigation01" ) ) {
					break;
				}

				// �I���^�O��������Ȃ������̂ŁA��́{�ǉ�
				//<li><div class="iconState0X" id="xxxx">
				//<div class="iconListImage"><a href="show_friend.pl?id=xxx" style="background: url(http://member.img.mixi.jp/photo/member/xx/xx/xxs.jpg); text-indent: -9999px;" class="iconTitle" title="�Ȃ܂�����">�Ȃ܂�����̎ʐ^</a></div><span>�Ȃ܂�����(11)</span>
				//<div id="xxx" class="memo_pop"></div><p><a href="show_friend_memo.pl?id=xxx" onClick="openMemo(event,'friend',xxx);return false;"><img src="http://img.mixi.jp/img/basic/icon/memo001.gif" width="12" height="14" /></a></p>
				//</div></li>
				if( util::LineHasStringsNoCase( line, L"<li", L"<div", L"class=", L"iconState0" ) ) {
					// iconState01 : 1���ȏ�
					// iconState02 : 1���ȓ�
					// iconState03 : 1���Ԉȓ�
					CMixiData mixi;
					if( util::LineHasStringsNoCase( line, L"iconState03" ) ) {
						mixi.SetDate( L"1���Ԉȓ�" );
					}else if( util::LineHasStringsNoCase( line, L"iconState02" ) ) {
						mixi.SetDate( L"1���ȓ�" );
					}else{
						mixi.SetDate( L"-" );
					}

					// ���̍s���t�F�b�`
					const CString& line2 = html_.GetAt( ++iLine );

					// <a �ȍ~�݂̂ɂ���
					CString target;
					if( util::GetAfterSubString( line2, L"<a", target ) < 0 ) {
						// <a ���Ȃ������̂Ŏ��̍s��͂ցB
						continue;
					}
					// target:  href="show_friend.pl?id=xxx" style="background: url(http://member.img.mixi.jp/photo/member/xx/xx/xxs.jpg); text-indent: -9999px;" class="iconTitle" title="�Ȃ܂�����">�Ȃ܂�����̎ʐ^</a></div><span>�Ȃ܂�����(11)</span>
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
					if( util::GetBetweenSubString( target, L"url(", L")", image_url ) < 0 ) {
						continue;
					}
					mixi.AddImage( image_url );

					// ���O���o
					// <span>�Ȃ܂�����(11)</span>
					CString name;
					if( util::GetBetweenSubString( target, L"<span>", L"</span>", name ) < 0 ) {
						continue;
					}
					mixi.SetName( name );

					mixi.SetAccessType( ACCESS_PROFILE );

					// �ǉ�����B
					out_.push_back( mixi );
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

		MZ3LOGGER_DEBUG( L"ListFriendParser.parse() finished." );
		return true;
	}

private:
	/// �u����\���v�A�u�O��\���v�̃����N�𒊏o����
	/// <td align="right" bgcolor="#EED6B5">1���`50����\��&nbsp;&nbsp;<a href=list_friend.pl?page=2&id=xxx>����\��</a></td></tr>
	static bool parseNextBackLink( CMixiData& nextLink, CMixiData& backLink, CString str )
	{
		// ���K�\���̃R���p�C���i���̂݁j
		static MyRegex reg;
		if( !util::CompileRegex( reg, L"<a href=list_friend.pl([?][^>]+)>([^<]+)</a>" ) ) {
			MZ3LOGGER_FATAL( FAILED_TO_COMPILE_REGEX_MSG );
			return false;
		}

		return parseNextBackLinkBase( nextLink, backLink, str, reg, L"list_friend.pl", ACCESS_LIST_FRIEND );
	}
};


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
		if( !util::CompileRegex( reg, L"<a href=show_intro.pl([?][^>]+)>([^<]+)</a>" ) ) {
			MZ3LOGGER_FATAL( FAILED_TO_COMPILE_REGEX_MSG );
			return false;
		}

		return parseNextBackLinkBase( nextLink, backLink, str, reg, L"show_intro.pl", ACCESS_LIST_INTRO );
	}
};





/**
 * [list] show_log.pl �p�p�[�T
 * �y�����Ɓz
 * http://mixi.jp/show_log.pl
 */
class ShowLogParser : public MixiListParser
{
public:
	static bool parse( CMixiDataList& out_, const CHtmlArray& html_ )
	{
		MZ3LOGGER_DEBUG( L"ShowLogParser.parse() start." );

		INT_PTR count = html_.GetCount();

		BOOL findFlag = FALSE;

		for (int i=0; i<count; i++) {

			CString str = html_.GetAt(i);

			if (findFlag != FALSE && str.Find(_T("</td></tr>")) != -1) {
				// �I��
				break;
			}

			if (str.Find(_T("show_friend.pl")) != -1) {
				findFlag = TRUE;

/* ��͑Ώە�����
<li style="padding: 0; margin: 0; list-style: none;">
2006�N11��21�� 18:10 <a href="show_friend.pl?id=ZZZZZ">XXXXX</a></li>
*/

				CString target = str;

				// URL �𒊏o
				// <a href="�`"> �ŉ��
				CString url;
				int pos = util::GetBetweenSubString( target, L"<a href=\"", L"\">", url );
				if( pos < 0 ) 
					continue;

				// ���t�𒊏o
				// ">�`<a �ŉ��
				target = str.Left( pos );
				CString strDate;
				pos = util::GetBetweenSubString( target, L"\">", L"<a", strDate );
				if( pos < 0 ) 
					continue;

				strDate.Trim();

				// ���O
				// ">�`</a �ŉ��
				// ��͑Ώۂ́A���t�̏I���ȍ~�B
				target = str.Mid( pos );
				CString name;
				pos = util::GetBetweenSubString( target, L">", L"</a>", name );

				// "show_friend_memo.pl" �� "http://img.mixi.jp/img/memo_s.gif" ������΃}�C�~�N�Ƃ݂Ȃ�
				bool bMyMixi = false;
				if( util::LineHasStringsNoCase( target, L"show_friend_memo.pl", L"http://img.mixi.jp/img/memo_s.gif" ) ) {
					// �}�C�~�N�Ƃ݂Ȃ�
					bMyMixi = true;
				}

				// �I�u�W�F�N�g����
				CMixiData data;
				data.SetAccessType( ACCESS_PROFILE );
				data.SetName( name );
				data.SetURL( url );
				data.SetBrowseUri( L"http://mixi.jp/" + url );
				ParserUtil::ParseDate( strDate, data );
				data.SetMyMixi( bMyMixi );

				out_.push_back( data );

			}
		}

		MZ3LOGGER_DEBUG( L"ShowLogParser.parse() finished." );
		return true;
	}
};



/**
 * [list] ������API �p�p�[�T
 * �y�����Ɓz
 * http://mixi.jp/atom/tracks/r=2/member_id=
 */
class TrackParser : public MixiListParser
{
public:
	static bool parse( CMixiDataList& out_, const CHtmlArray& html_ )
	{
		MZ3LOGGER_DEBUG( L"TrackParser.parse() start." );

		// html_ �̕�����
		std::vector<TCHAR> text;
		text.reserve( 10*1024 );	// �o�b�t�@�\��
		INT_PTR count = html_.GetCount();
		for (int i=0; i<count; i++) {
			const CString& line = html_.GetAt(i);
			size_t size = text.size();
			int new_size = wcslen(line);
			if (new_size>0) {
				text.resize( size+new_size );
				wcsncpy( &text[size], (LPCTSTR)line, new_size );
			}
		}

		// XML ���
		xml2stl::Container root;
		if (!xml2stl::SimpleXmlParser::loadFromText( root, text )) {
			MZ3LOGGER_ERROR( L"XML ��͎��s" );
			return false;
		}

		// entry �ɑ΂��鏈��
		try {
			const xml2stl::Node& feed = root.getNode( L"feed" );
			size_t nChildren = feed.getChildrenCount();
			for (size_t i=0; i<nChildren; i++) {
				const xml2stl::Node& node = feed.getNode(i);
				if (node.getName() != L"entry") {
					continue;
				}
				try {
					const xml2stl::Node& entry = node;
					// �I�u�W�F�N�g����
					CMixiData data;
					data.SetAccessType( ACCESS_PROFILE );

					// URL : entry/link/@href
					CString url = entry.getNode( L"link" ).getProperty( L"href" ).c_str();
					data.SetURL( url );
					data.SetBrowseUri( url );

					// name : entry/author/name
					const xml2stl::Node& author = entry.getNode( L"author" );
					data.SetName( author.getNode( L"name" ).getTextAll().c_str() );

					// �֌W : entry/author/tracks:relation
					const std::wstring& relation = author.getNode( L"tracks:relation" ).getTextAll();
					if (relation==L"friend") {
						data.SetMyMixi( true );
					} else {
						data.SetMyMixi( false );
					}

					// Image : entry/author/tracks:image
					data.AddImage( author.getNode( L"tracks:image" ).getTextAll().c_str() );

					// updated : entry/updated
					ParserUtil::ParseDate( entry.getNode( L"updated" ).getTextAll().c_str(), data );
	
					// ���������̂Œǉ�����
					out_.push_back( data );
				} catch (xml2stl::NodeNotFoundException& e) {
					MZ3LOGGER_ERROR( util::FormatString( L"some node or property not found... : %s", e.getMessage().c_str()) );
					break;
				}
			}
		} catch (xml2stl::NodeNotFoundException& e) {
			MZ3LOGGER_ERROR( util::FormatString( L"feed not found... : %s", e.getMessage().c_str()) );
		}

		MZ3LOGGER_DEBUG( L"TrackParser.parse() finished." );
		return true;
	}
};






/**
 * [list] list_bookmark.pl �p�p�[�T�B
 * �y���C�ɓ���z
 * http://mixi.jp/list_bookmark.pl
 */
class ListBookmarkParser : public MixiListParser
{
public:
	static bool parse( CMixiDataList& out_, const CHtmlArray& html_ )
	{
		MZ3LOGGER_DEBUG( L"ListBookmarkParser.parse() start." );

		INT_PTR count = html_.GetCount();

		// �u���v�A�u�O�v�̃����N
		CMixiData backLink;
		CMixiData nextLink;

		/**
		 * ���j�F
		 * - <td ... background=http://img.mixi.jp/img/bg_line.gif>
		 *   ��������΂������獀�ڊJ�n�Ƃ݂Ȃ�
		 * - 1�l���̍��ڂ𒊏o����B�i�ڍׂ� parseOneUser �Q�Ɓj
		 * - �ȍ~�A��L���J��Ԃ��B
		 * - �ŏ��̍��ڂ����������ȍ~�ɁA
		 *   <td ALIGN=right BGCOLOR=#EED6B5>1���`30����\��&nbsp;&nbsp;<a href=list_bookmark.pl?page=2>����\��</a></td></tr>
		 *   �Ƃ����`���ŁA�u����\���v�u�O��\���v�����݂���B
		 */

		// ���ڊJ�n��T��
		bool bInItems = false;	// ���ڊJ�n�H
		int iLine = 150;		// �Ƃ肠�����ǂݔ�΂�
		for( ; iLine<count; iLine++ ) {
			const CString& str = html_.GetAt(iLine);

			// �u���v�A�u�O�v�̃����N
			// ���ڔ�����ɂ̂ݑ��݂���
			if( bInItems ) {
				// �u����\���v�A�u�O��\���v�̃����N�𒊏o����
				if( parseNextBackLink( nextLink, backLink, str ) ) {
					// ���o�ł�����I���^�O�Ƃ݂Ȃ��B
					break;
				}
			}

			if( str.Find( L"<td" ) != -1 && str.Find( L"bg_line.gif" ) != -1 ) {
				// ���ڃt���O����
				CMixiData mixi;
				if( parseOneItem( mixi, html_, iLine ) ) {
					out_.push_back( mixi );
				}
				bInItems = true;
			}
		}

		// �O�A���̃����N������΁A�ǉ�����B
		if( !backLink.GetTitle().IsEmpty() ) {
			out_.insert( out_.begin(), backLink );
		}
		if( !nextLink.GetTitle().IsEmpty() ) {
			out_.push_back( nextLink );
		}

		MZ3LOGGER_DEBUG( L"ListBookmarkParser.parse() finished." );
		return true;
	}

private:
	/**
	 * 1�l���̃��[�U�̓��e�𒊏o����
	 *
	 * - ���̍s��
	 *   <a href="show_friend.pl?id=xxxxx">
	 *   �Ƃ����`���ŁuURL�v�����݂���B
	 * - ��������+3�s�ڂ�
	 *   <td ><a ...>�Ȃ܂�</a></td>
	 *   �Ƃ����`���Łu���O�v�����݂���B
	 * - ���̍s�ȍ~��
	 *   <td COLSPAN=2 BGCOLOR=#FFFFFF>...
	 *   �Ƃ����`���Łu���ȏЉ�v�����݂���B
	 * - ���̍s�ȍ~��
	 *   ...</td></tr>
	 *   ������΁u���ȏЉ�v�I���B
	 * - ���̍s�ȍ~��
	 *   <td BGCOLOR=#FFFFFF WIDTH=140>3���ȏ�</td>
	 *   �Ƃ����`���Łu�ŏI���O�C���v�����݂���B
	 */
	static bool parseOneItem( CMixiData& mixi, const CHtmlArray& html, int& iLine )
	{
		const int lastLine = html.GetCount();

		// URL ���o
		// bg_line.gif �̂���s���� N �s�ȓ��ɔ�������Ȃ���΃G���[�Ƃ��ďI������B
		CString url;
		int endLine = iLine+3;
		iLine ++;
		for( ; iLine<endLine; iLine ++ ) {
			const CString line = html.GetAt( iLine );

			if( util::GetBetweenSubString( line, L"<a href=\"", L"\">", url ) >= 0 ) {
				// ����
				mixi.SetURL( url );
				// URL �\�z���ݒ�
				url.Insert( 0, L"http://mixi.jp/" );
				mixi.SetBrowseUri( url );

				// ����ɉ摜������Β��o
				CString image_url;
				if (util::GetBetweenSubString( line, L"src=\"", L"\"", image_url ) >= 0 ) {
					mixi.AddImage( image_url );
				}

				break;
			}
		}
		if( iLine >= endLine ) {
			MZ3LOGGER_ERROR( L"<a href=\"...\"> ������܂���ł����B iLine[" + util::int2str(iLine) + L"]" );
			return false;
		}

		// ���O���o
		{
			iLine += 2;
			for( ; iLine<lastLine; iLine++ ) {
				const CString line = html.GetAt( iLine );

				CString buf;
				// <td colspan="2" bgcolor="#FFFFFF"><a href="show_friend.pl?id=xxx">�Ȃ܂�</a></td>
				// �܂���
				// <a href="view_community.pl?id=xxx">�R�~���j�e�B��</a>
				// ���疼�̂𒊏o����
				if( util::GetBetweenSubString( line, L"<a href=\"", L"</a>", buf ) > 0 ) {
					// buf : view_community.pl?id=xxx">�Ȃ܂�
					// buf : show_friend.pl?id=xxx">�R�~���j�e�B��
					// ���O���o
					CString name;
					if( util::GetAfterSubString( buf, L">", name ) > 0 ) {
						// ���O�ݒ�
						mixi.SetName( name );
						break;
					}
				}
			}
			if( iLine >= lastLine ) {
				MZ3LOGGER_ERROR( L"<a href=\"...\"> ������܂���ł����B iLine[" + util::int2str(iLine) + L"]" );
				return false;
			}
		}

		// ���ȏЉ�o
		{
			iLine += 1;
			bool bInIntroduce = false;
			for( ; iLine<lastLine; iLine++ ) {
				const CString line = html.GetAt( iLine );

				if( !bInIntroduce ) {
					// �J�n�^�O��T��
					CString after;
					if( util::GetAfterSubString( 
							line,
							L"<td colspan=\"2\" bgcolor=\"#FFFFFF\"", after ) < 0 )
					{
						continue;
					}
					// �����B
					bInIntroduce = true;
					continue;
				}

				// ���ȏЉ�I���H
				CString left;
				if( util::GetBeforeSubString( line, L"</td>", left ) >= 0 ) {
					// ���ȏЉ�I���B
					// ���̍������擾���A�{���ɒǉ����A���[�v�I��
					ParserUtil::AddBodyWithExtract( mixi, left );
					break;
				}else{
					// ���ȏЉ�p���B
					// �{���ɂ��̂܂ܒǉ��B
					ParserUtil::AddBodyWithExtract( mixi, line );
				}
			}
		}

		// �ŏI���O�C�����o
		{
			iLine += 1;
			for( ; iLine+1<lastLine; iLine++ ) {
				const CString line = html.GetAt( iLine );
				
				if( util::LineHasStringsNoCase( line, L"<td", L"bgcolor=\"#FFFFFF\"", L"width=\"250\"", L"align=\"left\">" ) ) {
					// ���̍s���ŏI���O�C������������
					CString date = html.GetAt( iLine+1 );

					// �ŏI���O�C�������B
					date.Replace(_T("\n"), _T(""));
					mixi.SetDate( date );
					break;
				}
			}
		}

		// URL �ɉ����ăA�N�Z�X��ʂ�ݒ肷��
		mixi.SetAccessType( util::EstimateAccessTypeByUrl( url ) );

		return true;
	}

	/// �u����\���v�A�u�O��\���v�̃����N�𒊏o����
	static bool parseNextBackLink( CMixiData& nextLink, CMixiData& backLink, CString str )
	{
		// ���K�\���̃R���p�C���i���̂݁j
		static MyRegex reg;
		if( !util::CompileRegex( reg, L"<a href=list_bookmark.pl([?][^>]+)>([^<]+)</a>" ) ) {
			MZ3LOGGER_FATAL( FAILED_TO_COMPILE_REGEX_MSG );
			return false;
		}

		return parseNextBackLinkBase( nextLink, backLink, str,
			reg, L"list_bookmark.pl", ACCESS_LIST_FAVORITE );
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

			//</form>�����ꂽ���͏I��
			if (str.Find(_T("</form>")) != -1) {
				break;
			}
			
			//�N���𒊏o
			if( util::LineHasStringsNoCase( str, L"<title>[mixi]", L"�̃J�����_�[</title>" ) ) {
				util::GetBetweenSubString( str, L"<title>[mixi]", L"�̃J�����_�[</title>", YearMonth );
				findFlag = TRUE;
			}
			
			//�J�n�t���O
			if( util::LineHasStringsNoCase( str, L"calendarTable01" ) ) {
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
					
				//<img src="http://img.mixi.jp/img/calendaricon2/i_iv2.gif" width="16" height="16" align="middle" /><a href="view_event.pl?id=xxxx&comm_id=xxxx">XXXXXXXXXXXXXXX</a></td><td height="65" bgcolor="#FFFFFF" align="left" valign="top"><font style="color: #996600;">9</font></td><td height="65" bgcolor="#FFFFFF" align="left" valign="top"><font style="color: #996600;">10</font>

				//i_iv1.gif �C�x���g�E���������Ă�
				//i_iv2.gif �C�x���g�E���l�����Ă�
				if( util::LineHasStringsNoCase( target, L"i_iv1.gif" ) ||
				    util::LineHasStringsNoCase( target, L"i_iv2.gif" ) ) 
				{
					util::GetBetweenSubString( target, L"\">", L"</a>", title );
					util::GetBetweenSubString( target, L"<a href=\"", L"\">", url );
					
					data.SetAccessType( ACCESS_EVENT );
					findFlag3 = TRUE;
				}
				//i_bd.gif�@�a����
				if( util::LineHasStringsNoCase( target, L"i_bd.gif" ) ) {
				
					//show_friend.pl�ȍ~�����o��
					CString after;
					util::GetAfterSubString( target, L"show_friend.pl", after );
					util::GetBetweenSubString( after, L">", L"</a>", title );
					
					title = L"�y�a�����z" + title;
					//��΂���̃v���t�B�[��URL�𒊏o
					util::GetBetweenSubString( target, L"<a href=", L">", url );
					data.SetURL( url );
					data.SetAccessType( ACCESS_PROFILE );
					findFlag3 = TRUE;
				}
				//i_sc-.gif�@�����X�P�W���[��
				if( util::LineHasStringsNoCase( target, L"i_sc-.gif" ) ) {
					util::GetBetweenSubString( target, L"align=\"absmiddle\" />", L"</a>", title );

					title = L"�y�X�P�W���[���z" + title;
					//��΂���̃X�P�W���[���ڍׂ��܂��������̂��ߕۗ�
					//util::GetBetweenSubString( target, L"<a href=\"", L"\">", url );

					data.SetAccessType( ACCESS_PROFILE );
					findFlag3 = TRUE;
				}

				if (findFlag3 != FALSE) {
					// �I�u�W�F�N�g����

					data.SetTitle( title );
					data.SetURL( url );
					strDate.Trim();
					data.SetDate( strDate );
					out_.push_back( data );
				}
				
				////�Ō�́u<font style=�v�ȍ~�̕�����؂�o��
				CString after = target;
				while( after.Find(_T("<font style=")) != -1 ) {
					util::GetAfterSubString( after, L"<font style=", after );
				}

				//�����̏ꍇ�@font-weight: bold;">21</font>
				if( util::LineHasStringsNoCase( after, L"font-weight: bold;\">" ) ) {
					CString date;
					util::GetBetweenSubString( after, L"font-weight: bold;\">", L"</font>", date );
					strDate = YearMonth + date + L"��";
				}
				
				//<font style="color: #996600;"> ������Ε��� 
				if( util::LineHasStringsNoCase( after, L"color: #996600;\">" ) ) {
					CString date;
					util::GetBetweenSubString( after, L"color: #996600;\">", L"</font>", date );
					strDate = YearMonth + date + L"��";
				}
				//<font style="color: #0000ff;">������Γy�j��
				if( util::LineHasStringsNoCase( after, L"color: #0000ff;\">" ) ) {
					CString date;
					util::GetBetweenSubString( after, L"color: #0000ff;\">", L"</font>", date );
					strDate = YearMonth + date + L"��";
				}
				//<font style="color: #ff0000;">������΋x��
				if( util::LineHasStringsNoCase( after, L"color: #ff0000;\">" ) ) {
					CString date;
					util::GetBetweenSubString( after, L"color: #ff0000;\">", L"</font>", date );
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
		if( !util::CompileRegex( reg, L"<a href=\"show_calendar.pl([?].+?)\">.*?(�O�̌�|���̌�).*?</a>" ) ) {
			MZ3LOGGER_FATAL( FAILED_TO_COMPILE_REGEX_MSG );
			return false;
		}

		return parseNextBackLinkBase( nextLink, backLink, str,
			reg, L"show_calendar.pl", ACCESS_LIST_CALENDAR );
	}
};

//������MZ3�Ǝ�������
/**
 * [content] Readme.txt �p�p�[�T
 * �yMZ3�w���v�p�z
 */
class HelpParser : public MixiContentParser
{
public:
	static bool parse( CMixiData& mixi, const CHtmlArray& html_ )
	{
		MZ3LOGGER_DEBUG( L"HelpParser.parse() start." );

		mixi.ClearAllList();

		INT_PTR count = html_.GetCount();

		int iLine = 0;

		int status = 0;		// 0 : start, 1 : �ŏ��̍���, 2 : 2�Ԗڈȍ~�̍��ډ�͒�
		CMixiData child;

		for( ; iLine<count; iLine++ ) {
			const CString& line = html_.GetAt(iLine);

			CString head2 = line.Left(2);

			// - "--" �Ŏn�܂�s�͖�������B
			if( head2 == "--" ) {
				continue;
			}

			// - "* " �Ŏn�܂�s������΁A�X�e�[�^�X�ɉ����ď������s���B
			if( head2 == L"* " ) {
				// 3 �����ڈȍ~�� Author �Ƃ��ēo�^����B
				// ����ɂ��A�͖ڎ��Ƃ��ă��|�[�g��ʂɕ\�������B
				CString chapter = line.Mid(2);
				chapter.Replace( L"\n", L"" );
				chapter.Replace( L"\t", L"    " );
				switch( status ) {
				case 0:
					// �ŏ��̍��ڂ����������̂ŁA�g�b�v�v�f�Ƃ��Ēǉ�����B
					mixi.SetAuthor( chapter );
					status = 1;
					break;
				case 1:
					// 2�Ԗڈȍ~�̍��ڂ����������̂ŁA�ŏ��̎q�v�f�Ƃ��Ēǉ�����B
					child.SetAuthor( chapter );
					status = 2;
					break;

				case 2:
					// 3�Ԗڈȍ~�̍��ڂ����������̂ŁA�ǉ����A�q�v�f������������B
					child.SetCommentIndex( mixi.GetChildrenSize()+1 );
					mixi.AddChild( child );

					child.ClearBody();
					child.SetAuthor( chapter );
					break;
				}
				continue;
			}

			// - "** " �Ŏn�܂�s������΁A�ߍ��ڂƂ��ĉ�́A�ǉ�����B
			if( line.Left(3) == L"** " ) {
				// 4 �����ڈȍ~�� Author �Ƃ��ēo�^����B
				// ����ɂ��A�ߖڎ��Ƃ��ă��|�[�g��ʂɕ\�������B
				CString section = line.Mid(3);
				section.Replace( L"\n", L"" );
				section.Replace( L"\t", L"    " );

				// �ߖ��̂̐擪�Ɏ��ʎq��ǉ�����B
				section.Insert( 0, L" " );

				switch( status ) {
				case 0:
					// �ŏ��̍��ڂƂ��Ă̐ߍ��ڂ͂��肦�Ȃ��̂Ŗ�������B
					break;
				case 1:
					// 1�Ԗڂ̉�͒��Ɍ��������̂ŁA�ŏ��̎q�v�f�Ƃ��Ēǉ�����B
					child.SetAuthor( section );
					status = 2;
					break;

				case 2:
					// 2�Ԗڈȍ~�̉�͒��Ɍ��������̂ŁA�ǉ����A�q�v�f������������B
					child.SetCommentIndex( mixi.GetChildrenSize()+1 );
					mixi.AddChild( child );

					child.ClearBody();
					child.SetAuthor( section );
					break;
				}
				continue;
			}

			switch( status ) {
			case 1:
			case 2:
				{
					// �v�f�p��
					CString str = line;
					str.Replace( L"\n", L"" );
					str.Replace( L"\t", L"    " );

					// �v�f�ǉ�
					if( status == 1 ) {
						mixi.AddBody( L"\r\n" + str );
					}else{
						child.AddBody( L"\r\n" + str );
					}
				}
				break;
			}
		}
		if( status == 2 ) {
			if( child.GetBodySize()>0 ) {
				child.SetCommentIndex( mixi.GetChildrenSize()+1 );
				mixi.AddChild( child );
			}
		}

		MZ3LOGGER_DEBUG( L"HelpParser.parse() finished." );
		return true;
	}

};

/**
 * [content] mz3log.txt �p�p�[�T
 * �yMZ3�w���v�p�z
 */
class ErrorlogParser : public MixiContentParser
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
class PlainTextParser : public MixiContentParser
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

/// ���X�g�nHTML�̉��
inline void MyDoParseMixiHtml( ACCESS_TYPE aType, CMixiDataList& body, CHtmlArray& html )
{
	switch (aType) {
	case ACCESS_LIST_DIARY:				mixi::ListNewFriendDiaryParser::parse( body, html );	break;
	case ACCESS_LIST_NEW_COMMENT:		mixi::NewCommentParser::parse( body, html );			break;
	case ACCESS_LIST_COMMENT:			mixi::ListCommentParser::parse( body, html );			break;
	case ACCESS_LIST_NEW_BBS:			mixi::NewBbsParser::parse( body, html );				break;
	case ACCESS_LIST_MYDIARY:			mixi::ListDiaryParser::parse( body, html );				break;
//	case ACCESS_LIST_FOOTSTEP:			mixi::ShowLogParser::parse( body, html );				break;
	case ACCESS_LIST_FOOTSTEP:			mixi::TrackParser::parse( body, html );					break;
	case ACCESS_LIST_MESSAGE_IN:		mixi::ListMessageParser::parse( body, html );			break;
	case ACCESS_LIST_MESSAGE_OUT:		mixi::ListMessageParser::parse( body, html );			break;
	case ACCESS_LIST_NEWS:				mixi::ListNewsCategoryParser::parse( body, html );		break;
	case ACCESS_LIST_FAVORITE:			mixi::ListBookmarkParser::parse( body, html );			break;
	case ACCESS_LIST_FRIEND:			mixi::ListFriendParser::parse( body, html );			break;
	case ACCESS_LIST_COMMUNITY:			mixi::ListCommunityParser::parse( body, html );			break;
	case ACCESS_LIST_INTRO:				mixi::ShowIntroParser::parse( body, html );				break;
	case ACCESS_LIST_BBS:				mixi::ListBbsParser::parse( body, html );				break;
	case ACCESS_LIST_NEW_BBS_COMMENT:	mixi::ListNewBbsCommentParser::parse( body, html );		break;
	case ACCESS_LIST_CALENDAR:			mixi::ShowCalendarParser::parse( body, html );			break;
	}
}

/// View�nHTML�̉��
inline void MyDoParseMixiHtml( ACCESS_TYPE aType, CMixiData& mixi, CHtmlArray& html )
{
	switch (aType) {
	case ACCESS_DIARY:		mixi::ViewDiaryParser::parse( mixi, html );		break;
	case ACCESS_BBS:		mixi::ViewBbsParser::parse( mixi, html );		break;
	case ACCESS_ENQUETE:	mixi::ViewEnqueteParser::parse( mixi, html );	break;
	case ACCESS_EVENT:		mixi::ViewEventParser::parse( mixi, html );		break;
	case ACCESS_PROFILE:	mixi::ShowFriendParser::parse( mixi, html );	break;
	case ACCESS_MYDIARY:	mixi::ViewDiaryParser::parse( mixi, html );		break;
	case ACCESS_MESSAGE:	mixi::ViewMessageParser::parse( mixi, html );	break;
	case ACCESS_NEWS:		mixi::ViewNewsParser::parse( mixi, html );		break;
	case ACCESS_HELP:		mixi::HelpParser::parse( mixi, html );			break;
	case ACCESS_ERRORLOG:	mixi::ErrorlogParser::parse( mixi, html );		break;
	case ACCESS_PLAIN:		mixi::PlainTextParser::parse( mixi, html );		break;
	}
}

}//namespace mixi

