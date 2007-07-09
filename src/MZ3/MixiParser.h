#pragma once

#include "MyRegex.h"
#include "XmlParser.h"
#include "HtmlArray.h"

/// mixi �pHTML�p�[�T
namespace mixi {

/// mixi �� URL �𕪉����郆�[�e�B���e�B
class MixiUrlParser {
public:

	/**
	 * URL ����I�[�i�[ID���擾����B
	 *
	 * view_diary.pl?id=xxx&owner_id=xxx
	 */
	static CString GetOwnerID( CString url )
	{
		CString id;
		util::GetAfterSubString( url, _T("owner_id="), id );

		// '&' ���܂܂�Ă���΁A���̌����폜����
		if( id.Find( '&' ) != -1 ) {
			util::GetBeforeSubString( id, L"&", id );
		}

		return id;
	}

	/**
	 * URL ����ID���擾����B
	 *
	 * view_community.pl?id=1231285
	 */
	static int GetID( CString url )
	{
		CString id;
		util::GetAfterSubString( url, _T("id="), id );
		return _wtoi(id);
	}

	/**
	 * �R�����g���̎擾�B
	 *
	 * http://mixi.jp/view_bbs.pl?id=xxx&comment_count=yyy&comm_id=zzz
	 */
	static int GetCommentCount(LPCTSTR uri)
	{
		// "comment_count="��"&"�Ɉ͂܂ꂽ������𐔒l�ϊ��������́B
		CString strCommentCount;
		if( util::GetBetweenSubString( uri, L"comment_count=", L"&", strCommentCount ) == -1 ) {
			// not found.
			return -1;
		}

		return _wtoi(strCommentCount);
	}

	/**
	 * ���O�̎擾�B
	 *
	 * show_friend.pl?id=xxx">�Ȃ܂�</a>
	 *
	 * @param str A �^�O�̕���������
	 */
	static bool GetAuthor(LPCTSTR str, CMixiData* data)
	{
		TRACE( L"GetAuthor, param[%s]\n", str );

		// show_friend.pl �ȍ~�ɐ��`�B
		CString target;
		if( util::GetAfterSubString( str, L"show_friend.pl", target ) == -1 ) {
			// not found.
			CString msg;
			msg.Format( L"������ show_friend.pl ���܂݂܂��� str[%s]", str );
			MZ3LOGGER_ERROR(msg);
			return false;
		}

		// ID ���o
		CString id;
		if( util::GetBetweenSubString( target, L"id=", L"\">", id ) == -1 ) {
			// not found.
			CString msg;
			msg.Format( L"������ 'id=' ���܂݂܂��� str[%s]", str );
			MZ3LOGGER_ERROR(msg);
			return false;
		}
		data->SetAuthorID( id );

		// ���O���o
		CString name;
		if( util::GetBetweenSubString( target, L">", L"<", name ) == -1 ) {
			CString msg;
			msg.Format( L"������ '>', '<' ���܂݂܂��� str[%s]", str );
			MZ3LOGGER_ERROR(msg);
			return false;
		}
		data->SetAuthor( name );

		return true;
	}

};

/// �p�[�T�p���[�e�B���e�B�Q
class ParserUtil {
public:
	/**
	 * URL �ɍŌ�ɃA�N�Z�X�����Ƃ��́A���e�������擾����
	 */
	static void GetLastIndexFromIniFile(LPCTSTR uri, CMixiData* data)
	{
		// URI�𕪉�
		CString buf = uri;

		switch (data->GetAccessType()) {
		case ACCESS_DIARY:
		case ACCESS_MYDIARY:
			buf = buf.Mid(buf.Find(_T("id="))+ wcslen(_T("id=")));
			buf = buf.Left(buf.Find(_T("&")));
			buf.Format(_T("d%s"), buf);
			break;
		case ACCESS_BBS:
			buf.Format(_T("b%d"), data->GetID());
			break;
		case ACCESS_EVENT:
			buf.Format(_T("v%d"), data->GetID());
			break;
		case ACCESS_ENQUETE:
			buf.Format(_T("e%d"), data->GetID());
			break;
		}

		std::string idKey = util::my_wcstombs( (LPCTSTR)buf );

		// �L�[�擾
		int lastIndex = -1;

		if (theApp.m_logfile.RecordExists(idKey.c_str(), "Log") ) {
			// ���R�[�h���݂�΂�����g��
			lastIndex = atoi( theApp.m_logfile.GetValue(idKey.c_str(), "Log").c_str() );
		}
		else {
			lastIndex = -1;
		}

		data->SetLastIndex(lastIndex);
	}

	/**
	 * �����ϊ�
	 *
	 * @param strDate [in]  ���t������B��F"12��08��"
	 * @param strTime [in]  ����������B��F"23:31"
	 * @param data    [out] ��͌��ʂ� SetDate �ŕۑ�����B
	 */
	static void ChangeDate( CString strDate, CString strTime, CMixiData* data )
	{
		int month  = _wtoi( strDate.Left(strDate.Find(_T("��")) ) );

		strDate = strDate.Mid(strDate.Find(_T("��")) + wcslen(_T("��")));
		int day    = _wtoi( strDate.Left(strDate.Find(_T("��"))) );

		int hour   = _wtoi( strTime.Left( strTime.Find(_T(":")) ) );
		int minute = _wtoi( strTime.Mid( strTime.Find(_T(":")) + wcslen(_T(":")) ) );

		// �S�� 0 �Ȃ�ݒ肵�Ȃ�
		if( month!=0 || day!=0 || hour!=0 || minute!=0 ) {
			data->SetDate(month, day, hour, minute);
		}
	}

	/**
	 * �����ϊ�
	 *
	 * @param buf  [in]  ����������B��F"2006�N11��19�� 17:12"
	 * @param data [out] ��͌��ʂ� SetDate �ŕۑ�����B
	 */
	static void ChangeDate(CString buf, CMixiData* data)
	{
		int year = _wtoi(buf.Left(buf.Find(_T("�N"))));

		buf = buf.Mid(buf.Find(_T("�N")) + wcslen(_T("�N")));
		int month = _wtoi(buf.Left(buf.Find(_T("��"))));

		buf = buf.Mid(buf.Find(_T("��")) + wcslen(_T("��")));
		int day = _wtoi(buf.Left(buf.Find(_T("��"))));

		buf = buf.Mid(buf.Find(_T("��")) + wcslen(_T("��")));
		buf.Trim();
		int hour = _wtoi(buf.Left(buf.Find(_T(":"))));

		buf = buf.Mid(buf.Find(_T(":")) + wcslen(_T(":")));
		int minute = _wtoi(buf);

		// �S�� 0 �Ȃ�ݒ肵�Ȃ�
		if( year!=0 || month!=0 || day!=0 || hour!=0 || minute!=0 ) {
			if( year==0 ) {
				data->SetDate(month, day, hour, minute);
			}else{
				data->SetDate(year, month, day, hour, minute);
			}
		}
	}

	/**
	 * str ���烊���N���𒊏o�A���`���AHTML �̐��`�����āAdata_ �� AddBody ����
	 */
	static void AddBodyWithExtract( CMixiData& data_, CString str )
	{
		ExtractReplaceLink( str, &data_ );
		UnEscapeHtmlElement( str );
		data_.AddBody( str );
	}

	/**
	 * �����񒆂Ɋ܂܂��S�Ẵ^�O����������
	 */
	static bool StripAllTags( CString& str )
	{
		CString result;
		
		// '<' �� '>' �ň͂܂ꂽ�������S�č폜����

		bool bInTag = false;
		int len = str.GetLength();
		for( int i=0; i<len; i++ ) {
			if( bInTag ) {
				if( str[i] == '>' ) {
					bInTag = false;
				}
			}else{
				if( str[i] == '<' ) {
					bInTag = true;
				}else{
					result.AppendChar( str[i] );
				}
			}
		}

		str = result;

		// �^�O�������ɏI���΃G���[�Ƃ���
		if( bInTag ) 
			return false;

		return true;
	}

	/**
	 * HTML �v�f�̒u���B���̎Q�ƁB
	 */
	static void UnEscapeHtmlElement(CString& line)
	{
		while( line.Replace(_T("\n"), _T("")) ) ;

		// table �^�O�̏���
		{
			static MyRegex reg;
			if( !reg.isCompiled() ) 
				if(! reg.compile( L"<table[^>]*>" ) ) 
					return;
			if( line.Find( L"<table" ) != -1 ) 
				reg.replaceAll( line, L"" );
		}
		while( line.Replace(_T("</table>"), _T("")) );

		// tr �^�O�̏���
		{
			static MyRegex reg;
			if( !reg.isCompiled() ) 
				if(! reg.compile( L"<tr[^>]*>" ) ) 
					return;
			if( line.Find( L"<tr" ) != -1 ) 
				reg.replaceAll( line, L"" );
		}
		while( line.Replace(_T("</tr>"), _T("")) );

		// td �^�O�̏���
		{
			static MyRegex reg;
			if( !reg.isCompiled() ) 
				if(! reg.compile( L"<td[^>]*>" ) ) 
					return;
			if( line.Find( L"<td" ) != -1 ) 
				reg.replaceAll( line, L"" );
		}
		while( line.Replace(_T("</td>"), _T("")) );

		// div �^�O�̏���
		{
			static MyRegex reg;
			if( !reg.isCompiled() ) 
				if(! reg.compile( L"<div[^>]*>" ) ) 
					return;
			if( line.Find( L"<div" ) != -1 ) 
				reg.replaceAll( line, L"" );
		}
		while( line.Replace(_T("</div>"), _T("")) );

		// br �^�O�̒u��
		{
			static MyRegex reg;
			if( !reg.isCompiled() ) 
				if(! reg.compile( L"<br[^>]*>" ) ) 
					return;
			if( line.Find( L"<br" ) != -1 ) 
				reg.replaceAll( line, L"\r\n" );
		}

		// "<p>" �� �폜
		while( line.Replace(_T("<p>"), _T("")) ) ;

		// </p> �� ���s
		while( line.Replace(_T("</p>"), _T("\r\n\r\n")) ) ;

		while( line.Replace(_T("&quot;"), _T("\"")) ) ;
		while( line.Replace(_T("&gt;"), _T(">")) ) ;
		while( line.Replace(_T("&lt;"), _T("<")) ) ;
		while( line.Replace(_T("&nbsp;"), _T(" ")) ) ;

		// &#xxxx; �̎��̎Q�Ƃ̕�����
		// ��j&#3642; �� char(3642)
		ReplaceEntityReferenceToCharacter( line );
	}

	/**
	 * &#xxxx; �̎��̎Q�Ƃ̕�����
	 * ��j&#3642; �� char(3642)
	 */
	static void ReplaceEntityReferenceToCharacter( CString& str )
	{
		// ���K�\���̃R���p�C���i���̂݁j
		static MyRegex reg;
		if( !reg.isCompiled() ) {
			if(! reg.compile( L"&#([0-9]{4,5});" ) ) {
				MZ3LOGGER_FATAL( FAILED_TO_COMPILE_REGEX_MSG );
				return;
			}
		}

		CString target = str;
		str = L"";
		for( int i=0; i<100; i++ ) {	// 100 �͖������[�v�h�~
			std::vector<MyRegex::Result>* pResults = NULL;
			if( reg.exec(target) == false || reg.results.size() != 2 )
			{
				// �������B
				// �c��̕�����������ďI���B
				str += target;
				break;
			}
			// �����B
			// �}�b�`������̍������o��
			str += target.Left( reg.results[0].start );

			// ���̉�
			{
				int d = _wtoi( reg.results[1].str.c_str() );
				str.AppendFormat( L"%c", d );
			}

			// �^�[�Q�b�g���X�V�B
			target = target.Mid( reg.results[0].end );
		}
	}

	/**
	 * �����N�ϊ��B
	 */
	static void ExtractReplaceLink(CString& str, CMixiData* data)
	{
		// ������̏������A
		// ������ ExtractXxx �����s���Ă��������A
		// ExtractXxx �̓R�X�g�������̂ŁA���O�� Find �ōŒ���̒T�����s���Ă���

#define LINE_HAS_DIARY_IMAGE(str) util::LineHasStringsNoCase( str, L"<a", L"MM_openBrWindow('", L"'", L"</a>" )

		// �摜�̒��o
		// �A���o���摜�A���L�E�g�s�b�N�E�A���P�[�g�摜�̒��o

		if( str.Find( L"show_album_picture.pl" ) != -1 ) {
			// �A���o���摜�̒��o
			ExtractAlbumImage( str, *data );
		}else if( LINE_HAS_DIARY_IMAGE(str) ) {
			// ���L�E�g�s�b�N�E�A���P�[�g�摜�̒��o
			ExtractDiaryImage( str, *data );
		}

		// �G�����摜�����N�̕ϊ�
		// <img src="http://img.mixi.jp/img/emoji/85.gif" alt="�i��" width="16" height="16" class="emoji" border="0">
#define LINE_HAS_EMOJI_LINK(str)	util::LineHasStringsNoCase( str, L"<img", L"alt=", L"class=\"emoji\"", L">" )
		if( LINE_HAS_EMOJI_LINK(str) ) {
			ReplaceEmojiImageToText( str );
		}

		// ���̑��̉摜�����N�̕ϊ�
		if( util::LineHasStringsNoCase( str, L"<img", L"src=" ) ) {
			ExtractGeneralImageLink( str, *data );
		}

		// �����N�̒��o
		if (str.Find( L"href" ) != -1) {
			ExtractURI( str, *data );
		}

		// 2ch �`���̃����N���o
		if( str.Find( L"ttp://" ) != -1 ) {
			Extract2chURL( str, *data );
		}
	}

private:

	/**
	 * �G�����摜�����N�̕ϊ��B
	 *
	 * �Ƃ肠���� alt ������ɒu������B
	 */
	static void ReplaceEmojiImageToText( CString& line )
	{
		// <img src="http://img.mixi.jp/img/emoji/85.gif" alt="�i��" width="16" height="16" class="emoji" border="0">
		// �̂悤�ȃ����N��
		// "((�i��))" �ɕϊ�����

		// ���K�\���̃R���p�C���i���̂݁j
		static MyRegex reg;
		if( !reg.isCompiled() ) {
			LPCTSTR szPattern = L"<img src=\"http://img.mixi.jp/img/emoji/([^\"]+).gif\" alt=\"([^\"]+)\" [^c]+ class=\"emoji\"[^>]*>";
			if(! reg.compile( szPattern ) ) {
				MZ3LOGGER_FATAL( FAILED_TO_COMPILE_REGEX_MSG );
				return;
			}
		}

		// ((�i��)) �ɕϊ�����
		reg.replaceAll( line, L"(({2}))" );

		// [m:xx] �`���ɒu������ꍇ�͉��L�� replaceAll �Ƃ���B
//		if( reg.replaceAll( line, L"[m:{1}]" ) ) {
	}

	/**
	 * �A���o���摜�ϊ�
	 *
	 * str ����A���o���摜�����N�𒊏o���A���̃����N�� data �ɒǉ�(AddImage)����B
	 * �܂��Astr ����Y������摜�����N���폜����B
	 */
	static void ExtractAlbumImage(CString& str, CMixiData& data_)
	{
/* �z�蕶����i���s�Ȃ��j
<br>
<a href="javascript:void(0)" onClick="MM_openBrWindow('show_album_picture.pl?
album_id=ZZZ&number=ZZZ&owner_id=ZZZ&key=ZZZ
','','width=650,height=650,toolbar=no,scrollbars=yes,left=10,top=10')">
<img alt=�ʐ^ src=http://ic30.mixi.jp/p/ZZZ/ZZZ/album/ZZZ/ZZZ/ZZZ.jpg border=0></a>
<a href="javascript:void(0)" onClick="MM_openBrWindow('show_album_picture.pl?
album_id=ZZZ&number=ZZZ&owner_id=ZZZ&key=ZZZ
','','width=650,height=650,toolbar=no,scrollbars=yes,left=10,top=10')">
<img alt=�ʐ^ src=http://ic3.mixi.jp/p/ZZZ/ZZZ/album/ZZZ/ZZZ/ZZZ.jpg border=0></a>
*/

		CString ret = _T("");

		LPCTSTR tag_MMBegin		= _T("MM_openBrWindow('");
		LPCTSTR tag_MMEnd		= _T("'");
		CString tag_AlbumBegin	= L"<a href=\"javascript:void(0)\"";
		CString tag_AlbumEnd	= L"</a>";
		CString url_mixi		= _T("http://mixi.jp/");

		// �܂��A�A���o���֘A�^�O�J�n�O�܂ł��o�̓o�b�t�@�ɒǉ�
		int pos = str.Find(tag_AlbumBegin);
		if( pos >= 0 ) {
			// �A���o���֘A�^�O�J�n�O�܂ł��o�̓o�b�t�@�ɒǉ��B
			ret += str.Left( pos );
		}else{
			// ������Ȃ�������orz
			// �A���o���Ȃ����B
			// �ϊ������ɏI���B
			return;
		}

		CString buf = str.Mid( pos );
		for( int i=0; i<100; i++ ) {	// 100 �͖������[�v�h�~

			// �^�O�J�n�O�܂ł�ǉ�
			CString left;
			if( util::GetBeforeSubString( buf, tag_MMBegin, left ) == -1 ) {
				// not found
				ret += buf;
				break;
			}
			ret += left;

			CString url_right;
			if( util::GetBetweenSubString( buf, tag_MMBegin, tag_MMEnd, url_right ) == -1 ) {
				// not found
				ret += buf;
				break;
			}

			TRACE(_T("%s\n"), url_right);
			CString url_image = url_mixi + url_right;
			data_.AddImage(url_image);

#define REPLACE_ALBUMURL_STRING
#ifdef  REPLACE_ALBUMURL_STRING
			ret.AppendFormat( L"<<�摜%02d(album)>>", data_.GetImageCount() );
#else
			ret += url_image;
#endif
			ret += _T("<br>");

			// ���̃T�[�`�̂��߂�buf��ύX����
			int index = buf.Find(tag_AlbumEnd);
			buf = buf.Mid(index + wcslen(tag_AlbumEnd));
		}

		// �X�V���ďI���B
		str = ret;
	}

	/**
	 * ���L�A�g�s�b�N���̉摜�ϊ��B
	 *
	 * str ����摜�����N�𒊏o���A���̃����N�� data �ɒǉ�(AddImage)����B
	 * �܂��Astr ����Y������摜�����N���폜����B
	 */
	static void ExtractDiaryImage(CString& line, CMixiData& data_)
	{
/* �z�蕶����i���s�Ȃ��BZ�͉p�����j
<td><table><tr><td width="130" height="140" align="center" valign="middle">
<a href="javascript:void(0)" onClick="MM_openBrWindow('show_diary_picture.pl?
owner_id=ZZZZZ&id=ZZZZZ&number=ZZZZZ','pict','width=680,height=660,toolbar=
no,scrollbars=yes,left=5,top=5')"><img src="http://ic29.mixi.jp/p/ZZZZZZZ/
ZZZ/diary/ZZ/ZZ/ZZs.jpg" border="0"></a></td>
*/
		LPCTSTR url_mixi		= _T("http://mixi.jp/");

		// ���o�͕��������͑ΏۂƂ���B
		CString target = line;

		// �Ƃ肠�������o�͕��������ɂ���B
		line = L"";

		for( int i=0; i<100; i++ ) {	// 100 �͖������[�v�h�~
			if( LINE_HAS_DIARY_IMAGE(target) ) {

				// �������Ƃ肠�����o��
				CString left;
				if( util::GetBeforeSubString( target, L"<a", left ) == -1 ) {
					// not found
					// LineHasStringsNoCase �Ń`�F�b�N���Ă���̂ł����ɗ���������G���[�B
					MZ3LOGGER_FATAL( L"�摜�����N��͂ŃG���[���������܂����B line[" + target + L"]" );
					break;
				}
				line += left;

				CString url_right;
				if( util::GetBetweenSubString( target, L"MM_openBrWindow('", L"'", url_right ) == -1 ) {
					// not found
					// LineHasStringsNoCase �Ń`�F�b�N���Ă���̂ł����ɗ���������G���[�B
					MZ3LOGGER_FATAL( L"�摜�����N��͂ŃG���[���������܂����B line[" + target + L"]" );
					break;
				}

				TRACE(_T("%s\n"), url_right);
				CString url_image = url_mixi + url_right;
				data_.AddImage( url_image );

				MZ3LOGGER_DEBUG( L"�摜URL [" + url_image + L"]" );

				// �摜�����N��u������
				line.AppendFormat( L"<<�摜%02d>><br>", data_.GetImageCount() );

				// ���̃T�[�`�̂��߂� str ���X�V����
				if( util::GetAfterSubString( target, L"</a>", target ) < 0 ) {
					// �I���^�O���Ȃ������B�^�O�d�l�ύX�H
					MZ3LOGGER_ERROR( L"�摜�̏I���^�O��������܂���B line[" + target + L"]" );
					break;
				}else{
					// �X�VOK
				}
			}else{
				// not found
				// ������Ȃ������̂ŁA��͑Ώە��������o�͕�����ɒǉ�����
				line += target;
				break;
			}
		}
	}

	/**
	 * �摜�ϊ��B
	 *
	 * str ����摜�����N�𒊏o���A���̃����N�� data �ɒǉ�(AddImage)����B
	 * �܂��Astr ����Y������摜�����N���폜����B
	 */
	static void ExtractGeneralImageLink(CString& line, CMixiData& data_)
	{
		// ���K�\���̃R���p�C���i���̂݁j
		static MyRegex reg;
		if( !reg.isCompiled() ) {
			LPCTSTR szPattern = L"<img[^>]*src=\"([^\"]+)\" [^>]*>";
			if(! reg.compile( szPattern ) ) {
				MZ3LOGGER_FATAL( FAILED_TO_COMPILE_REGEX_MSG );
				return;
			}
		}

		CString target = line;
		line = L"";
		for( int i=0; i<100; i++ ) {	// 100 �͖������[�v�h�~
			if( !reg.exec(target) || reg.results.size() != 2 ) {
				// �������B
				// �c��̕�����������ďI���B
				line += target;
				break;
			}

			// �����B
			std::vector<MyRegex::Result>& results = reg.results;

			// �}�b�`������S�̂̍������o��
			line.Append( target, results[0].start );

			CString text = L"<<�摜>>";

			// url ��ǉ�
			LPCTSTR url = results[1].str.c_str();
			data_.m_linkList.push_back( CMixiData::Link(url, text) );

			// �u��
			line.Append( text );

			// �Ƃ肠�������s
			line += _T("<br>");

			// �^�[�Q�b�g���X�V�B
			target.Delete( 0, results[0].end );
		}

	}

public:
	/**
	 * �S�Ẵ����N������i<a...>XXX</a>�j���A������̂݁iXXX�j�ɕύX����
	 * �܂��Ahref="url" �� list_ �ɒǉ�����B
	 */
	static void ExtractURI( CString& str, std::vector<CMixiData::Link>& list_ )
	{
		TRACE( L"ExtractURI:str[%s]\n", (LPCTSTR)str );

		// <wbr/> �^�O�̏���
		while( str.Replace(_T("<wbr/>"), _T("")) );

		// ���K�\���̃R���p�C���i���̂݁j
		static MyRegex reg;
		if( !reg.isCompiled() ) {
			if(! reg.compile( L"<a href='([^']+)'[^>]*>([^<]*)</a>" ) ) {
				MZ3LOGGER_FATAL( FAILED_TO_COMPILE_REGEX_MSG );
				return;
			}
		}
		static MyRegex reg2;
		if( !reg2.isCompiled() ) {
			if(! reg2.compile( L"<a href=\"([^\"]+)\"[^>]*>([^<]*)</a>" ) ) {
				MZ3LOGGER_FATAL( FAILED_TO_COMPILE_REGEX_MSG );
				return;
			}
		}
		static MyRegex reg3;
		if( !reg3.isCompiled() ) {
			if(! reg3.compile( L"<a href=([^>]+)>([^<]*)</a>" ) ) {
				MZ3LOGGER_FATAL( FAILED_TO_COMPILE_REGEX_MSG );
				return;
			}
		}

		CString target = str;
		str = L"";
		for( int i=0; i<100; i++ ) {	// 100 �͖������[�v�h�~
			std::vector<MyRegex::Result>* pResults = NULL;
			if( reg.exec(target) && reg.results.size() == 3 ) {
				pResults = &reg.results;
			}
			if( pResults == NULL && reg2.exec(target) && reg2.results.size() == 3 ) {
				pResults = &reg2.results;
			}
			if( pResults == NULL && reg3.exec(target) && reg3.results.size() == 3 ) {
				pResults = &reg3.results;
			}
			if( pResults == NULL ) {
				// �������B
				// �c��̕�����������ďI���B
				str += target;
				break;
			}
			// �����B
			std::vector<MyRegex::Result>& results = *pResults;


			// �}�b�`������S�̂̍������o��
			str += target.Left( results[0].start );

			// URL
			const std::wstring& url = results[1].str;
			TRACE( L"regex-match-URL  : %s\n", url.c_str() );

			// ������
			const std::wstring& text = results[2].str;
			TRACE( L"regex-match-TEXT : %s\n", text.c_str() );
			str += text.c_str();

			// �f�[�^�ɒǉ�
			list_.push_back( CMixiData::Link(url.c_str(),text.c_str()) );

			// �^�[�Q�b�g���X�V�B
			target = target.Mid( results[0].end );
		}
	}

private:
	/**
	 * �S�Ẵ����N������i<a...>XXX</a>�j���A������̂݁iXXX�j�ɕύX����
	 * �܂��Ahref="url" �� data_ �̃����N���X�g�ɒǉ�����B
	 */
	static void ExtractURI( CString& str, CMixiData& data_ ) {
		ExtractURI( str, data_.m_linkList );
	}

	/**
	 * 2ch �`����URL(ttp://...)�𒊏o���A���K������ data �̃����N���X�g�ɒǉ�����B
	 */
	static void Extract2chURL( const CString& str, CMixiData& data_ )
	{
//		TRACE( L"Extract2chURL:str[%s]\n", (LPCTSTR)str );

		// ���K�\���̃R���p�C���i���̂݁j
		static MyRegex reg;
		if( !reg.isCompiled() ) {
			LPCTSTR szPattern = L"[^h](ttps?://[-_.!~*'()a-zA-Z0-9;/?:@&=+$,%#]+)";
			if(! reg.compile( szPattern ) ) {
				MZ3LOGGER_FATAL( FAILED_TO_COMPILE_REGEX_MSG );
				return;
			}
		}

		CString target = str;
		for( int i=0; i<100; i++ ) {	// 100 �͖������[�v�h�~
			if( reg.exec(target) == false || 
				reg.results.size() != 2 )
			{
				// �������B�I���B
				break;
			}

			// �����B

			// 2ch URL
			const std::wstring& url_2ch = reg.results[1].str;
			TRACE( L"regex-match-2chURL : %s\n", url_2ch.c_str() );

			// 2ch URL �𐳋K��
			std::wstring url = L"h";
			url += url_2ch;

			// �f�[�^�ɒǉ�
			data_.m_linkList.push_back( CMixiData::Link(url.c_str(), url_2ch.c_str()) );

			// �^�[�Q�b�g���X�V�B
			target = target.Mid( reg.results[0].end );
		}
	}
};

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
	static void GetPostAddress(int sIndex, int eIndex, const CHtmlArray& html_, CMixiData& data_)
	{
		CString str;
		for (int i=sIndex; i<eIndex; i++) {
			str = html_.GetAt(i);

			// <form name="bbs_comment_form" action="add_bbs_comment.pl?id=xxx&comm_id=xxx"
			// method="post" enctype="multipart/form-data">
			if (str.Find(_T("<form")) != -1 &&
				str.Find(_T("method=")) != -1 &&
				str.Find(_T("action=")) != -1 &&
				str.Find(_T("post")) != -1) 
			{
				// Content-Type/enctype ����
				if (str.Find(_T("multipart")) != -1) {
					data_.SetContentType(CONTENT_TYPE_MULTIPART);
				} else {
					data_.SetContentType(CONTENT_TYPE_FORM_URLENCODED);
				}

				// action �擾
				CString action;
				if( util::GetBetweenSubString( str, L"action=\"", L"\"", action ) < 0 ) {
					continue;
				}
				data_.SetPostAddress( action );

				if (data_.GetAccessType() != ACCESS_DIARY &&
					data_.GetAccessType() != ACCESS_MYDIARY)
				{
					break;
				}
				continue;
			}
			else if (str.Find(_T("owner_id")) != -1) {

				CString ownerId;
				if( util::GetBetweenSubString( str, L"value=\"", L"\"", ownerId ) < 0 ) {
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
		// �������Ȃ�I���B
		// <table ... bgcolor="#eed6b5">
		const int count = html.GetCount();
		int iLine=startIndex;
		for( ; iLine<count; iLine++ ) {
			const CString& line = html.GetAt(iLine);
			if( util::LineHasStringsNoCase( line, L"<table", L"bgcolor=\"#eed6b5\">" ) )
			{
				break;
			}
		}
		if( iLine >= count ) {
			return false;
		}

		// </table> ��������܂Ńp�^�[���}�b�`���J��Ԃ��B
		for( iLine=iLine+1; iLine<count; iLine++ ) {
			const CString& line = html.GetAt(iLine);
			if( util::LineHasStringsNoCase( line, L"</table>" ) ) {
				break;
			}

			CString str = line;
			ParserUtil::ExtractURI( str, data.m_linkPage );
		}

		return !data.m_linkPage.empty();
	}

	/**
	 * �u�ŐV�̃g�s�b�N�v�̒��o�B
	 */
	static bool parseRecentTopics( CMixiData& data, const CHtmlArray& html, int startIndex=200 )
	{
		// startIndex �ȍ~�ɉ��L�𔭌�������A��͊J�n�B
		// �������Ȃ�I���B
		// <table ... bgcolor="#E8C79B">
		const int count = html.GetCount();
		int iLine=startIndex;
		for( ; iLine<count; iLine++ ) {
			const CString& line = html.GetAt(iLine);
			if( util::LineHasStringsNoCase( line, L"<table", L"bgcolor=\"#E8C79B\">" ) )
			{
				break;
			}
		}
		if( iLine >= count ) {
			return false;
		}

		// </table> ��������܂Ńp�^�[���}�b�`���J��Ԃ��B
		for( iLine=iLine+1; iLine<count; iLine++ ) {
			const CString& line = html.GetAt(iLine);
			if( util::LineHasStringsNoCase( line, L"</table>" ) ) {
				break;
			}

			CString str = line;
			ParserUtil::ExtractURI( str, data.m_linkPage );
		}

		return !data.m_linkPage.empty();
	}
};

/**
 * [list] list_news_category.pl �p�p�[�T�B
 * 
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
				if( !reg.isCompiled() ) {
					if(! reg.compile( L"\"(view_news.pl\\?id=[0-9]+\\&media_id=[0-9]+).+>(.+)</" ) ) {
						MZ3LOGGER_FATAL( FAILED_TO_COMPILE_REGEX_MSG );
						return false;
					}
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
				if( !reg.isCompiled() ) {
					if(! reg.compile( L"list_news_media.pl.+>([^<]+)</" ) ) {
						MZ3LOGGER_FATAL( FAILED_TO_COMPILE_REGEX_MSG );
						return false;
					}
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
			const CString& line3 = html_.GetAt(iLine);

			std::wstring date;
			{
				// ���K�\���̃R���p�C���i���̂݁j
				static MyRegex reg;
				if( !reg.isCompiled() ) {
					if(! reg.compile( L"<td[^>]+>([^<]+)</" ) ) {
						MZ3LOGGER_FATAL( FAILED_TO_COMPILE_REGEX_MSG );
						return false;
					}
				}
				// �T��
				if( reg.exec(line3) == false || reg.results.size() != 2 ) {
					// �������B
					continue;
				}
				// �����B
				date = reg.results[1].str;	// �z�M����
			}


			// mixi �f�[�^�̍쐬
			{
				CMixiData data;
				data.SetAccessType( ACCESS_NEWS );

				// ���t
				ParserUtil::ChangeDate( date.c_str(), &data );
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
		if( !reg.isCompiled() ) {
			if(! reg.compile( L"<a href=list_news_category.pl([?][^>]+)>([^<]+)</a>" ) ) {
				MZ3LOGGER_FATAL( FAILED_TO_COMPILE_REGEX_MSG );
				return false;
			}
		}

		return parseNextBackLinkBase( nextLink, backLink, str, reg, 
					L"http://news.mixi.jp/list_news_category.pl", ACCESS_LIST_NEWS );
	}

};

/**
 * [list] list_bookmark.pl �p�p�[�T�B
 * 
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
		if( !reg.isCompiled() ) {
			if(! reg.compile( L"<a href=list_bookmark.pl([?][^>]+)>([^<]+)</a>" ) ) {
				MZ3LOGGER_FATAL( FAILED_TO_COMPILE_REGEX_MSG );
				return false;
			}
		}

		return parseNextBackLinkBase( nextLink, backLink, str,
			reg, L"list_bookmark.pl", ACCESS_LIST_FAVORITE );
	}
};

/**
 * [list] list_friend.pl �p�p�[�T�B
 * 
 * http://mixi.jp/list_friend.pl
 */
class ListFriendParser : public MixiListParser
{
public:
	static bool parse( CMixiDataList& out_, const CHtmlArray& html_ )
	{
		MZ3LOGGER_DEBUG( L"ListFriendParser.parse() start." );

		INT_PTR count = html_.GetCount();

		// �u���v�A�u�O�v�̃����N
		CMixiData backLink;
		CMixiData nextLink;

		/**
		 * ���j�F
		 * �� table ��5�l������ł���B
		 *   ��s�Ƀ����N���摜���A�����s�ɖ��O������ł���B
		 *   �]���āA�u5�l���̃����N���o�v�u5�l���̖��O���o�v�u�˂����킹�v�u�f�[�^�ǉ��v�Ƃ����菇�ōs���B
		 *
		 * �� <table ... CELLPADDING=2 ...>
		 *    ��������΁A�������獀�ڊJ�n�Ƃ݂Ȃ��B</table> �������܂ňȉ������s����B
		 *   (1) </tr> ��������܂ł̊e�s���p�[�X���A����̌`���Ɉ�v���Ă���΁AURL �Ǝ������擾����B
		 *   (2) ���� </tr> ��������܂ł̊e�s���p�[�X���A����̌`���Ɉ�v���Ă���΁A���O�𒊏o����B
		 *   (3) (1), (2) �Œ��o�����f�[�^�� out_ �ɒǉ�����B
		 */

		// ���ڊJ�n��T��
		bool bInItems = false;	// ���ڊJ�n�H
		int iLine = 100;		// �Ƃ肠�����ǂݔ�΂�
		for( ; iLine<count; iLine++ ) {
			const CString& line = html_.GetAt(iLine);

			if( !bInItems ) {
				// ���ڊJ�n�H
				if( util::LineHasStringsNoCase( line, L"<table", L"cellpadding=\"2\"", L">" ) ) {
					bInItems = true;
				}
			}

			if( bInItems ) {
				// </table> ��������ΏI��
				if( util::LineHasStringsNoCase( line, L"</table>" ) ) {
					break;
				}

				// </table> ��������Ȃ������̂�5�l���擾���ǉ�
				if( parseTwoTR( out_, html_, iLine ) ) {
				}else{
					// ��̓G���[
					break;
				}
			}
		}

		// </table> �����������̂ŁA���̌�̍s���玟�A�O�̃����N�𒊏o
		for( ; iLine<count; iLine++ ) {
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
	/**
	 * 5�l���̃��[�U�̓��e�𒊏o����
	 *
	 * (1) </tr> �������܂ł̊e�s���p�[�X���A5�l����URL�A�����𐶐�����B
	 * (2) </tr> �������܂ł̊e�s���p�[�X���A5�l���̖��O�𐶐�����B
	 * (3) mixi_list �ɒǉ�����B
	 */
	static bool parseTwoTR( CMixiDataList& mixi_list, const CHtmlArray& html, int& iLine )
	{
		const int lastLine = html.GetCount();

		// �ꎞ�I�� CMixiData �̃��X�g
		CMixiDataList tmp_list;

		// 1�ڂ� </tr> �܂ł̉��
		bool bBreak = false;
		for( ; iLine < lastLine && bBreak == false; iLine++ ) {
			const CString& line = html.GetAt( iLine );
			if( util::LineHasStringsNoCase( line, L"</tr>" ) ) {
				// </tr> �����A���O���o�Ɉڂ�B
				bBreak = true;
			}
			if( util::LineHasStringsNoCase( line, L"</table>" ) ) {
				// </table> �����A�I��
				return false;
			}

			// <td �Ŏn�܂�Ȃ�A���o����
			if( line.Left( 3 ).CompareNoCase( L"<td" ) == 0 ) {
				/* �s�̌`���F
line1 : <td width="20%" height="100" background="http://img.mixi.jp/img/bg_orange2-.gif">
line2 : <a href="show_friend.pl?id=xxx"><img src="http://img.mixi.jp/photo/member/xxx.jpg" alt=""  border="0" /></a></td>
*/
				CMixiData mixi;

				// ��������
				{
					// "bg_orange1-.gif" ������΁A�u1���Ԉȓ��v
					// "bg_orange2-.gif" ������΁A�u1���ȓ��v
					// ��������Ȃ���΁A�u1���ȏ�v
					if( util::LineHasStringsNoCase( line, L"bg_orange1-.gif" ) ) {
						mixi.SetDate( L"1���Ԉȓ�" );
					}else if( util::LineHasStringsNoCase( line, L"bg_orange2-.gif" ) ) {
						mixi.SetDate( L"1���ȓ�" );
					}else{
						mixi.SetDate( L"-" );
					}
				}

				// line2 �̃t�F�b�`
				const CString& line2 = html.GetAt( ++iLine );
				if( line2.Find( L"</tr>" ) != -1 ) {
					// </tr> �����A���O���o�Ɉڂ�B
					bBreak = true;
				}
				if( util::LineHasStringsNoCase( line2, L"</table>" ) ) {
					// </table> �����A�I��
					return false;
				}

				// <a �ȍ~�݂̂ɂ���
				CString target;
				if( util::GetAfterSubString( line2, L"<a", target ) < 0 ) {
					// <a ���Ȃ������̂Ŏ��̍s��͂ցB
					continue;
				}
				// target:  href="show_friend.pl?id=xxx"><img ...
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
				if( util::GetAfterSubString( target, L"<img", target ) < 0 ) {
					continue;
				}
				// target:  src="http://img.mixi.jp/photo/member/xxx.jpg" alt=""  border="0" /></a></td>
				CString image_url;
				if( util::GetBetweenSubString( target, L"src=\"", L"\"", image_url ) < 0 ) {
					continue;
				}
				mixi.AddImage( image_url );

				// ��͐����Ȃ̂Œǉ�����B
				tmp_list.push_back( mixi );
			}
		}
		if( iLine >= lastLine ) {
			return false;
		}

		// 2�ڂ� </tr> �܂ł̉��
		int mixiIndex = 0;		// ���Ԗڂ̍��ڂ���͂��Ă��邩��\���C���f�b�N�X
		for( ; iLine < lastLine; iLine++ ) {
			const CString& line = html.GetAt( iLine );
			if( util::LineHasStringsNoCase( line, L"</tr>" ) ) {
				// </tr> �����A�I��
				break;
			}
			if( util::LineHasStringsNoCase( line, L"</table>" ) ) {
				// </table> �����A�I��
				return false;
			}

			if( mixiIndex >= (int)tmp_list.size() ) {
				// ���X�g���ȏ㌩�������̂ŁA�I���B
				break;
			}

			// <td ������Ȃ�A���o����
			if( util::LineHasStringsNoCase( line, L"<td" ) ) {
				CMixiData& mixi = tmp_list[mixiIndex];
				mixiIndex ++;

				// �s�̌`���F
				// <tr align="center" bgcolor="#FFF4E0"><td valign="top">xxx����(xx)
				// �܂���
				// </td><td valign="top">xxx����(xx)

				// ���O���o
				CString name = line;

				// �S�Ẵ^�O������
				ParserUtil::StripAllTags( name );

				// ������ \n ���폜
				name.Replace( L"\n", L"" );

				mixi.SetName( name );
				mixi.SetAccessType( ACCESS_PROFILE );

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
	/// <td align="right" bgcolor="#EED6B5">1���`50����\��&nbsp;&nbsp;<a href=list_friend.pl?page=2&id=xxx>����\��</a></td></tr>
	static bool parseNextBackLink( CMixiData& nextLink, CMixiData& backLink, CString str )
	{
		// ���K�\���̃R���p�C���i���̂݁j
		static MyRegex reg;
		if( !reg.isCompiled() ) {
			if(! reg.compile( L"<a href=list_friend.pl([?][^>]+)>([^<]+)</a>" ) ) {
				MZ3LOGGER_FATAL( FAILED_TO_COMPILE_REGEX_MSG );
				return false;
			}
		}

		return parseNextBackLinkBase( nextLink, backLink, str, reg, L"list_friend.pl", ACCESS_LIST_FRIEND );
	}
};

/**
 * [list] list_community.pl �p�p�[�T�B
 * 
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
		 * �� table ��5��������ł���B
		 *   ��s�Ƀ����N���摜���A�����s�ɖ��O������ł���B
		 *   �]���āA�u5�����̃����N���o�v�u5�����̖��O���o�v�u�˂����킹�v�u�f�[�^�ǉ��v�Ƃ����菇�ōs���B
		 *
		 * �� <table ... CELLPADDING=2 ...>
		 *    ��������΁A�������獀�ڊJ�n�Ƃ݂Ȃ��B</table> �������܂ňȉ������s����B
		 *   (1) </tr> ��������܂ł̊e�s���p�[�X���A����̌`���Ɉ�v���Ă���΁AURL �Ǝ������擾����B
		 *   (2) ���� </tr> ��������܂ł̊e�s���p�[�X���A����̌`���Ɉ�v���Ă���΁A���O�𒊏o����B
		 *   (3) (1), (2) �Œ��o�����f�[�^�� out_ �ɒǉ�����B
		 */

		// ���ڊJ�n��T��
		bool bInItems = false;	// ���ڊJ�n�H
		int iLine = 100;		// �Ƃ肠�����ǂݔ�΂�
		for( ; iLine<count; iLine++ ) {
			const CString& line = html_.GetAt(iLine);

			if( !bInItems ) {
				// ���ڊJ�n�H
				if( util::LineHasStringsNoCase( line, L"<table", L"cellpadding=\"2\"", L">" ) ) {
					bInItems = true;
				}
			}

			if( bInItems ) {
				// 5�����擾���ǉ�
				if(! parseTwoTR( out_, html_, iLine ) ) {
					// ��̓G���[
					break;
				}

				// </table> ��������ΏI��
				const CString& line = html_.GetAt(iLine);
				if( util::LineHasStringsNoCase( line, L"</table>" ) ) {
					break;
				}
			}
		}

		// </table> �����������̂ŁA���̌�̍s���玟�A�O�̃����N�𒊏o
		for( ; iLine<count; iLine++ ) {
			const CString& str = html_.GetAt(iLine);

			// �u����\���v�A�u�O��\���v�̃����N�𒊏o����
			if( parseNextBackLink( nextLink, backLink, str ) ) {
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

		MZ3LOGGER_DEBUG( L"ListCommunityParser.parse() finished." );
		return true;
	}

private:
	/**
	 * 5�����̃��[�U�̓��e�𒊏o����
	 *
	 * (1) </tr> �������܂ł̊e�s���p�[�X���A5������URL�A�����𐶐�����B
	 * (2) </tr> �������܂ł̊e�s���p�[�X���A5�����̖��O�𐶐�����B
	 * (3) mixi_list �ɒǉ�����B
	 */
	static bool parseTwoTR( CMixiDataList& mixi_list, const CHtmlArray& html, int& iLine )
	{
		const int lastLine = html.GetCount();

		// �ꎞ�I�� CMixiData �̃��X�g
		CMixiDataList tmp_list;

		// 1�ڂ� </tr> �܂ł̉��
		bool bBreak = false;
		for( ; iLine < lastLine && bBreak == false; iLine++ ) {
			const CString& line = html.GetAt( iLine );

			// </table> ��������ΏI��
			if( util::LineHasStringsNoCase( line, L"</table>" ) ) {
				return false;
			}

			if( util::LineHasStringsNoCase( line, L"</tr>" ) ) {
				// </tr> �����A���O���o�Ɉڂ�B
				bBreak = true;
			}

			// <td �Ŏn�܂�Ȃ�A���o����
			if( line.Left( 3 ).CompareNoCase( L"<td" ) == 0 ) {
				/* �s�̌`���F
line1: <td width="20%" height="100" background="http://img.mixi.jp/img/bg_line.gif">
line2: <a href="view_community.pl?id=1231285"><img src="http://img-c1.mixi.jp/photo/comm/12/85/1231285_120s.jpg" alt=""  border="0" /></a></td>
*/
				CMixiData mixi;

				// ���̍s���t�F�b�`
				const CString& line2 = html.GetAt( ++iLine );
				if( util::LineHasStringsNoCase( line2, L"</tr>" ) ) {
					// </tr> �����A���O���o�Ɉڂ�B
					bBreak = true;
				}

				// <a �ȍ~�݂̂ɂ���
				CString target;
				if( util::GetAfterSubString( line2, L"<a", target ) < 0 ) {
					// <a ���Ȃ������̂Ŏ��̍s��͂ցB
					continue;
				}
				// target:  href="view_community.pl?id=1231285"><img src="http://img-c1.mixi.jp/photo/comm/12/85/1231285_120s.jpg" alt=""  border="0" /></a></td>
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
				if( util::GetAfterSubString( target, L"<img", target ) < 0 ) {
					continue;
				}
				// target:  src="http://img-c1.mixi.jp/photo/comm/12/85/1231285_120s.jpg" alt=""  border="0" /></a></td>
				CString image_url;
				if( util::GetBetweenSubString( target, L"src=\"", L"\"", image_url ) < 0 ) {
					continue;
				}
				mixi.AddImage( image_url );

				// ��͐����Ȃ̂Œǉ�����B
				tmp_list.push_back( mixi );
			}
		}
		if( iLine >= lastLine ) {
			return false;
		}

		// 2�ڂ� </tr> �܂ł̉��
		bBreak = false;
		int mixiIndex = 0;		// ���Ԗڂ̍��ڂ���͂��Ă��邩��\���C���f�b�N�X
		for( ; iLine < lastLine && bBreak == false; iLine++ ) {
			const CString& line = html.GetAt( iLine );
			if( util::LineHasStringsNoCase( line, L"</tr>" ) ) {
				// </tr> �����A�I��
				bBreak = true;
			}

			if( mixiIndex >= (int)tmp_list.size() ) {
				// ���X�g���ȏ㌩�������̂ŁA�I���B
				break;
			}

			// </table> ��������ΏI��
			if( util::LineHasStringsNoCase( line, L"</table>" ) ) {
				return false;
			}

			// <td ������΁A���o����
			if( util::LineHasStringsNoCase( line, L"<td" ) ) {
				CMixiData& mixi = tmp_list[mixiIndex];
				mixiIndex ++;

				// �s�̌`���F
				// <tr align="center" bgcolor="#FFF4E0"><td valign="top">MZ3 Debuggers(50)
				// �܂���
				// </td><td valign="top">MZ3 -Mixi for ZERO3-(1504)

				// ���O���o

				// <td �ȍ~�𒊏o
				CString name;
				if( util::GetAfterSubString( line, L"<td ", name ) < 0 ) {
					continue;
				}

				// > �ȍ~�𒊏o
				if( util::GetBetweenSubString( name, L">", L"\n", name ) < 0 ) {
					continue;
				}
				mixi.SetName( name );
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
		if( !reg.isCompiled() ) {
			if(! reg.compile( L"<a href=list_community.pl([?][^>]+)>([^<]+)</a>" ) ) {
				MZ3LOGGER_FATAL( FAILED_TO_COMPILE_REGEX_MSG );
				return false;
			}
		}

		return parseNextBackLinkBase( nextLink, backLink, str,
					reg, L"list_community.pl", ACCESS_LIST_COMMUNITY );
	}
};

/**
 * [list] show_intro.pl �p�p�[�T�B
 * 
 * http://mixi.jp/show_intro.pl
 */
class ShowIntroParser : public MixiListParser
{
public:
	static bool parse( CMixiDataList& out_, const CHtmlArray& html_ )
	{
		MZ3LOGGER_DEBUG( L"ShowIntroParser.parse() start." );

		INT_PTR count = html_.GetCount();

		// �u���v�A�u�O�v�̃����N
		CMixiData backLink;
		CMixiData nextLink;

		/**
		 * ���j�F
		 * - bg_line.gif ��������΁A�������獀�ڊJ�n�Ƃ݂Ȃ��B
		 * - </tr> �������܂ł��p�[�X���A���ڂ𐶐�����B
		 * - </td></tr></table> �����ꂽ��I���Ƃ���B
		 */

		// ���ڊJ�n��T��
		bool bInItems = false;	// ���ڊJ�n�H
		int iLine = 100;		// �Ƃ肠�����ǂݔ�΂�
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

			// ���ځH
			if( str.Find( L"<td" ) != -1 && str.Find( L"bg_line.gif" ) != -1 ) {
				bInItems = true;

				// ���
				CMixiData mixi;
				if( parseOneIntro( mixi, html_, iLine ) ) {
					out_.push_back( mixi );
				}
			}

			if( bInItems ) {
				// </td></tr></table> ��������ΏI��
				if( str.Find( L"</td></tr></table>" ) != -1 ) {
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

		MZ3LOGGER_DEBUG( L"ShowIntroParser.parse() finished." );
		return true;
	}

private:
	/**
	 * �P�̏Љ�𒊏o����B
	 * iLine �� bg_line.gif �����ꂽ�s�B
	 *
	 * (1) iLine ����URL�A���O�𒊏o����B
	 * (2) <td WIDTH=480>�Ƃ����s��T���B���̎��̍s�ɉ��L�̌`���Łu�֌W�v�����݂���B
     *     �֌W�F���񂯂�<br>
	 * (3) ���L�̌`���ŏЉ������̂ŁAAddBody����B
	 *     <div style="width:286px; margin:0; padding:0;">�ق�Ԃ�</div>
	 * (4) </tr> ��������ΏI������B
	 */
	static bool parseOneIntro( CMixiData& mixi, const CHtmlArray& html, int& iLine )
	{
		const int lastLine = html.GetCount();

		const CString& line = html.GetAt( iLine );

		// ���݂̍s(1)�Ǝ��̍s(2)�ɁA
		// ���L�̌`���ŁuURL�v�u�摜URL�v�u���O�v�����݂���̂ŁA���o����B
		// (1)�F<td WIDTH=150 background=http://img.mixi.jp/img/bg_line.gif align="center">
		// (1)�F<a href="show_friend.pl?id=xxx"><img src="http://img.mixi.jp/photo/member/xxx.jpg" border=0><br>
		// (2)�F�Ȃ܂�</td></a>

		// URL ���o
		CString target;
		if( util::GetAfterSubString( line, L"<a ", target ) < 0 ) {
			return false;
		}
		// target: href="show_friend.pl?id=xxx"><img src="http://img.mixi.jp/photo/member/xxx.jpg" border=0><br>
		CString url;
		if( util::GetBetweenSubString( target, L"href=\"", L"\"", url ) < 0 ) {
			return false;
		}
		mixi.SetURL( url );

		// URL �\�z���ݒ�
		url.Insert( 0, L"http://mixi.jp/" );
		mixi.SetBrowseUri( url );

		// �摜URL���o
		if( util::GetAfterSubString( target, L"<img ", target ) < 0 ) {
			return false;
		}
		// target: src="http://img.mixi.jp/photo/member/xxx.jpg" border=0><br>
		CString image_url;
		if( util::GetBetweenSubString( target, L"src=\"", L"\"", image_url ) < 0 ) {
			return false;
		}
		mixi.AddImage( image_url );

		// ���̍s���疼�O���o
		const CString& line2 = html.GetAt( ++iLine );
		// </td> �̑O�܂ł𖼑O�Ƃ���B
		CString name;
		if( util::GetBeforeSubString( line2, L"</td>", name ) < 0 ) {
			return false;
		}
		mixi.SetName( name );

		mixi.SetAccessType( ACCESS_PROFILE );

		// �֌W�F�̒��o
		for( ++iLine; iLine+1<lastLine; iLine++ ) {
			const CString& line = html.GetAt( iLine );
			if( util::LineHasStringsNoCase( line, L"<td WIDTH=480>" ) ) {
				// �֌W�F���񂯂�<br>
				CString line1 = html.GetAt( ++iLine );

				if( line1.Find( L"�֌W�F" ) == -1 ) {
					break;
				}

				// ���p�X�y�[�X " " ��ǉ����Ă���
				line1.Replace( L"<br>", L" <br>" );

				ParserUtil::AddBodyWithExtract( mixi, line1 );
				break;
			}
		}


		// </tr> ��������܂ŁA�Љ��T��
		bool bInIntro = false;
		for( ++iLine; iLine< lastLine; iLine++ ) {
			const CString& line = html.GetAt( iLine );

			if( util::LineHasStringsNoCase( line, L"</tr>" ) ) {
				break;
			}
			if( !bInIntro ) {
				// <div style="width:286px; margin:0; padding:0;">�ق�Ԃ�</div>
				if( util::LineHasStringsNoCase( line, L"<div" ) ) {
					// ���̍s�ȍ~�ɏЉ������B
					bInIntro = true;

					// <div ... > ���폜
					CString result;
					if( util::GetAfterSubString( line, L">", result ) < 0 ) {
						continue;
					}
					// </div> ������΍폜���Љ�I��
					if( util::GetBeforeSubString( result, L"</div>", result ) >= 0 ) {
						// �Љ�ǉ��B
						mixi.AddBody( result );
						bInIntro = false;
					}else{
						// �Љ�ǉ��B
						mixi.AddBody( result );
					}
				}
			}else{
				// </div> ������΂��̑O�܂ł�ǉ����A�I���B
				// </div> ���Ȃ���΁A�ǉ����A�p���B
				if( util::LineHasStringsNoCase( line, L"</div>" ) ) {
					CString result;
					util::GetBeforeSubString( line, L"</div>", result );

					ParserUtil::AddBodyWithExtract( mixi, result );
					bInIntro = false;
				}else{
					ParserUtil::AddBodyWithExtract( mixi, line );
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
		if( !reg.isCompiled() ) {
			if(! reg.compile( L"<a href=show_intro.pl([?][^>]+)>([^<]+)</a>" ) ) {
				MZ3LOGGER_FATAL( FAILED_TO_COMPILE_REGEX_MSG );
				return false;
			}
		}

		return parseNextBackLinkBase( nextLink, backLink, str, reg, L"show_intro.pl", ACCESS_LIST_INTRO );
	}
};

/**
 * [list] list_bbs.pl �p�p�[�T�B
 * 
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
		 *   - "<table ... BGCOLOR="#EED6B5">" �̍s��T���B
		 *   - "</td></tr></table>" �������܂œǂݔ�΂��B
		 *   - ���̍s�ȍ~�����ځB
		 *
		 * - ���L�̍s���p�[�X���A���ڂ𐶐�����B
		 *   <td ALIGN=center ROWSPAN=3 NOWRAP bgcolor=#FFD8B0>0X��XX��<br>XX:XX</td>
		 *   <td bgcolor=#FFF4E0>&nbsp;<a href="view_bbs.pl?id=xxx&comm_id=xxx">���e</a></td></tr>
		 * - �ēx "<table ... BGCOLOR=#EED6B5>" �����ꂽ��I���Ƃ���B
		 *   ���̍s�́i���ցA�O�ւ��܂ށj�i�r�Q�[�V�����s�B
		 */

		// ���ڊJ�n�s��T��
		int iLine = 200;		// �Ƃ肠�����ǂݔ�΂�
		bool bInItems = false;
		for( ; iLine<count; iLine++ ) {
			const CString& line = html_.GetAt(iLine);

			if( util::LineHasStringsNoCase( line, L"<table", L"#EED6B5" ) ) 
			{
				// ���ڊJ�n�s�����B
				bInItems = true;

				// ����ɖ��ʂȍs���X�L�b�v���邽�߁Abreak ���Ȃ��B
			}

			if( bInItems ) {
				// ���ʂȍs�̃X�L�b�v
				if( util::LineHasStringsNoCase( line, L"</td></tr></table>" ) ) {
					// ���ʂȍs�̃X�L�b�v�����B
					iLine ++;
					break;
				}
			}
		}

		if( !bInItems ) {
			return false;
		}

		// ���ڂ̎擾
		for( ; iLine<count; iLine++ ) {
			const CString& line = html_.GetAt(iLine);

			// "<table ... BGCOLOR=#EED6B5>" �����ꂽ��i�r�Q�[�V�����s�B
			// �u���v�A�u�O�v�̃����N���܂ށB
			// ��͂��I������B
			if( util::LineHasStringsNoCase( line, L"<td", L"#EED6B5" ) ) 
			{
				// �u����\���v�A�u�O��\���v�̃����N�𒊏o����
				parseNextBackLink( nextLink, backLink, line );

				// ���o�̐��ۂɂ�����炸�I������B
				break;
			}

			// ���ځH
			//   <td align="center" rowspan="3" nowrap="nowrap" bgcolor="#FFD8B0">0X��XX��<br />XX:XX</td>
			//   <td bgcolor="#FFF4E0">&nbsp;<a href="view_bbs.pl?id=xxx&comm_id=xxx">�y�񍐁z���삵�܂���</a></td></tr>
			if( util::LineHasStringsNoCase( line, L"<td", L"#FFD8B0" ) ) {
				// ���
				CMixiData mixi;

				// ���t
				CString date, time;
				util::GetBetweenSubString( line, L">", L"<br />", date );
				util::GetBetweenSubString( line, L"<br />", L"<", time );
				ParserUtil::ChangeDate( date, time, &mixi );

				// ���̍s���擾���A���o���ƃ����N�𒊏o����
				iLine += 1;
				const CString& line2 = html_.GetAt(iLine);

				// ���o�����o
				CString target;
				{
					// �܂� "<a href=" �܂œǂݔ�΂�
					util::GetAfterSubString( line2, L"<a href=", target );
					// buf : "view_bbs.pl?id=xxx&comm_id=xxx">���e</a></td></tr>
					CString title;
					util::GetBetweenSubString( target, L">", L"<", title );
					mixi.SetTitle(title);
				}

				// URL ���o
				CString url;
				util::GetBetweenSubString( target, L"\"", L"\"", url );
				mixi.SetURL( url );

				// URL �ɉ����ăA�N�Z�X��ʂ�ݒ�
				mixi.SetAccessType( util::EstimateAccessTypeByUrl(url) );

				// ID�̒��o�A�ݒ�
				mixi.SetID( MixiUrlParser::GetID(url) );

				// �R�����g�����
				{
					// ���L�̍s���擾���ĉ�͂���B
					// <td ALIGN=right bgcolor="#FFFFFF"><a href=view_bbs.pl?id=xxx&comm_id=xxx>��������(19)</a>
					int commentCount = -1;
					for( iLine++; iLine<count; iLine++ ) {
						const CString& line = html_.GetAt(iLine);

						if( util::LineHasStringsNoCase( line, L"<td", url, L"��������(" ) )
						{
							// �����B�R�����g�����
							CString cc;
							util::GetBetweenSubString( line, L"��������(", L")", cc );
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

				// �O��̃C���f�b�N�X���擾
				ParserUtil::GetLastIndexFromIniFile(mixi.GetURL(), &mixi);

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
		if( !reg.isCompiled() ) {
			if(! reg.compile( L"<a href=list_bbs.pl([?][^>]+)>([^<]+)</a>" ) ) {
				MZ3LOGGER_FATAL( FAILED_TO_COMPILE_REGEX_MSG );
				return false;
			}
		}

		return parseNextBackLinkBase( nextLink, backLink, str, reg, L"list_bbs.pl", ACCESS_LIST_BBS );
	}
};

/**
 * [content] view_news.pl �p�p�[�T
 */
class ViewNewsParser : public MixiContentParser
{
public:
	static bool parse( CMixiData& data_, const CHtmlArray& html_ )
	{
		MZ3LOGGER_DEBUG( L"ViewNewsParser.parse() start." );

		data_.ClearAllList();

		INT_PTR count = html_.GetCount();

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

			CString str = line;

			ParserUtil::AddBodyWithExtract( data_, str );
		}

		MZ3LOGGER_DEBUG( L"ViewNewsParser.parse() finished." );
		return true;
	}

};

/**
 * [content] view_diary.pl �p�p�[�T
 */
class ViewDiaryParser : public MixiContentParser
{
public:
	static bool parse( CMixiData& data_, const CHtmlArray& html_ )
	{
		MZ3LOGGER_DEBUG( L"ViewDiaryParser.parse() start." );

		data_.ClearAllList();

		INT_PTR count = html_.GetCount();

		CString str;
		BOOL findFlag = FALSE;
		BOOL endFlag = FALSE;
		int index;
		CString buf;

		// �����̓��L���ǂ����𔻒f����i���O�̎擾�̂��߁j
		bool bMyDiary = true;
		for( int i=75; i<min(100,count); i++ ) {
			const CString& line = html_.GetAt( i );
			if( util::LineHasStringsNoCase( line, L"f_np_0_message.gif')\">'" ) ) {
				// ��L������Ƃ������Ƃ́A�����̓��L�ł͂Ȃ��B
				bMyDiary = false;
				break;
			}
		}

		// �ŏ���10�s�ڂ܂ł��������A<title> �^�O����͂��邱�ƂŁA
		// �^�C�g�����擾����
		for( int i=3; i<min(10,count); i++ ) {
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
		for( int iLine=100; iLine<count; iLine++ ) {
			// <table BORDER=0 CELLSPACING=0 CELLPADDING=0 BGCOLOR=#F8A448>
			// ������΁u�ŋ߂̓��L�v�J�n�Ƃ݂Ȃ��B
			const CString& line = html_.GetAt( iLine );
			if( util::LineHasStringsNoCase( line, L"<table", L"#F8A448" ) )
			{
				bStartRecentDiary = true;
				continue;
			}

			if( bStartRecentDiary ) {
				// </table>
				// ������΁u�ŋ߂̓��L�v�I���Ƃ݂Ȃ��B
				if( util::LineHasStringsNoCase( line, L"</table>" ) ) {
					break;
				}

				// ���
				// ���L�̌`���Łu�ŋ߂̓��L�v�����N�����݂���
				// <td BGCOLOR=#FFFFFF><a HREF="view_diary.pl?id=xxx&owner_id=xxx" CLASS=wide>
				// <img src=http://img.mixi.jp/img/pt_or.gif ALIGN=left BORDER=0 WIDTH=8 HEIGHT=16>
				// �^�C�g��</a></td>
				if( util::LineHasStringsNoCase( line, L"view_diary.pl" ) ) {
					CString url;
					int idx = util::GetBetweenSubString( line, L"<a HREF=\"", L"\"", url );
					if( idx > 0 ) {
						CString buf = line.Mid( idx );
						// buf:
						//  CLASS=wide>
						// <img src=http://img.mixi.jp/img/pt_or.gif ALIGN=left BORDER=0 WIDTH=8 HEIGHT=16>
						// �^�C�g��</a></td>

						// > ��ǂݔ�΂�
						CString after;
						if( util::GetAfterSubString( buf, L">", after ) > 0 ) {
							CString title;
							if( util::GetBetweenSubString( after, L">", L"</a>", title ) > 0 ) {
								CMixiData::Link link( url, title );
								data_.m_linkPage.push_back( link );
							}
						}
					}
				}
			}
		}

		// ���L�J�n�t���O

		for (int i=75; i<count; i++) {
			str = html_.GetAt(i);

			if (findFlag == FALSE) {
				// ���L�J�n�t���O�𔭌�����܂ŉ�

				if (util::LineHasStringsNoCase( str, L"<td", L"class=\"h12\"" ) ||
					util::LineHasStringsNoCase( str, L"<td", L"CLASS=h12" ) )
				{
					// ���L�J�n�t���O�����i���L�{�������j
					findFlag = TRUE;

					// �Ƃ肠�������s�o��
					data_.AddBody(_T("\r\n"));

					// ���L�̒���
					{
						// �t���O�̂T�O�s���炢�O�ɁA���L�̒��҂�����͂��B
						// <td WIDTH=490 background=http://img.mixi.jp/img/bg_w.gif><b><font COLOR=#605048>XXX�̓��L</font></b></td>
						// <td WIDTH=490 background=http://img.mixi.jp/img/bg_w.gif><b><font COLOR=#605048>XXX����̓��L</font></b></td>
						for( int iBack=-50; iBack<0; iBack++ ) {
							const CString& line = html_.GetAt( i+iBack );
							LPCTSTR pattern = L"bg_w.gif><b><font ";
							int idx = line.Find( pattern );
							if( idx >= 0 ) {
								// �����B
								CString target = line.Mid( idx+wcslen(pattern) );
								// �����̓��L�Ȃ�uXXX�̓��L�v�A�����ȊO�Ȃ�uXXX����̓��L�v�̂͂��B
								// ���̋K���Œ��҂���́B
								CString author;
								if( bMyDiary ) {
									util::GetBetweenSubString( target, L">", L"�̓��L<", author );
								}else{
									util::GetBetweenSubString( target, L">", L"����̓��L<", author );
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

					// "<td" �ȍ~�ɐ��`
					str = str.Mid( str.Find(L"<td") );

					// ���݂̍s����́A�ǉ��B
					if( util::GetBeforeSubString( str, L"</td>", buf ) > 0 ) {
						// ���̂P�s�ŏI���
						ParserUtil::AddBodyWithExtract( data_, buf );
						endFlag = TRUE;
					}else{
						ParserUtil::AddBodyWithExtract( data_, str );
					}
				}
				else if (str.Find(_T("����͊O���u���O���g���Ă��܂��B<br>")) != -1) {
					// �O���u���O���
					parseExternalBlog( data_, html_, i );
					break;
				}
			}
			else {
				// ���L�J�n�t���O�����ς݁B

				// �I���^�O�܂Ńf�[�^�擾
				if (str.Find(_T("</td>")) != -1 ) {
					endFlag = TRUE;
				}

				if( endFlag == FALSE ) {
					// �I���^�O������
					// ���L�{�����
					ParserUtil::AddBodyWithExtract( data_, str );
				}
				else {
					// �I���^�O����
					if (str.Find(_T("<br>")) != -1) {
						ParserUtil::AddBodyWithExtract( data_, str );
					}

					// �R�����g�擾
					i += 10;
					data_.ClearChildren();

					int cmtNum = 0;

					index = i;
					while( index < count ) {
						cmtNum++;
						index  = parseDiaryComment(index, count, data_, html_, cmtNum);
						if (index == -1) {
							break;
						}
					}
					if (index == -1 || index >= count) {
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
	static bool parseExternalBlog( CMixiData& data_, const CHtmlArray& html_, int i )
	{
		CString str;

		data_.AddBody(_T("\r\n"));

		str = html_.GetAt(i-1);
		ParserUtil::UnEscapeHtmlElement(str);
		data_.AddBody(str);

		str = html_.GetAt(i);
		ParserUtil::UnEscapeHtmlElement(str);
		data_.AddBody(str);

		i += 5;
		str = html_.GetAt(i);
		data_.AddBody(_T("\r\n"));
		CString buf = str.Mid(str.Find(_T("href=\"")) + wcslen(_T("href=\"")));
		buf = buf.Left(buf.Find(_T("\"")));

		// �O���u���O�t���O�𗧂āA�O���u���O��URL��ݒ肵�Ă����B
		data_.SetOtherDiary(TRUE);
		data_.SetBrowseUri(buf);

		data_.AddBody(buf);

		return true;
	}

	/// ���L�̓Y�t�摜�擾
	static bool parseImageLink( CMixiData& data_, const CHtmlArray& html_, int iLine_ )
	{
		// �t���O��N�s�O�ɉ摜�����N�����邩���B
		int n_images_begin = -25;
		int n_images_end   = -5;
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
	 * @param id
	 */
	static int parseDiaryComment(int sIndex, int eIndex, CMixiData& data_, const CHtmlArray& html_, int id)
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
					util::LineHasStringsNoCase( str, L"<!-- ///// �R�����g : end ///// -->" ) ) // �R�����g�S�̂̏I���^�O����
				{
					GetPostAddress(i, eIndex, html_, data_);
					return -1;
				}

				if ((index = str.Find(_T("show_friend.pl"))) != -1) {
					// �R�����g�w�b�_�擾

					// �h�c
					cmtData.SetCommentIndex(id);

					// ���O
					buf = str.Mid(index);
					MixiUrlParser::GetAuthor(buf, &cmtData);

					// ����
					for (int j=i; j>0; j--) {
						str = html_.GetAt(j);
						if (str.Find(_T("��<br>")) != -1) {

							ParserUtil::ChangeDate( 
								util::XmlParser::GetElement(str, 1) + _T(" ") + util::XmlParser::GetElement(str, 3),
								&cmtData);
							break;
						}
					}

					findFlag = TRUE;

				}
			}
			else {

				if (str.Find(_T("CLASS=h12")) != -1) {
					// �R�����g�{���擾
					i++;
					str = html_.GetAt(i);

					if (str.Find(_T("<br>")) != 0) {
						cmtData.AddBody(_T("\r\n"));
					}

					ParserUtil::AddBodyWithExtract( cmtData, str );

					i++;
					for( ; i<eIndex; i++ ) {
						str = html_.GetAt(i);
						if (str.Find(_T("</td></tr>")) != -1) {
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

/**
 * [list] new_bbs.pl �p�p�[�T
 * 
 * �R�~���j�e�B�ꗗ���擾
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
			if( str.Find(_T("/pen_r.gif")) != -1 && str.Find(_T("<img")) != -1 ) 
			{
				dataFind = TRUE;

				CMixiData data;

				// ���t
				ParserUtil::ChangeDate(util::XmlParser::GetElement(str, 3), &data);

				// ���o��
				i+=2;
				CString str = html_.GetAt(i);
				CString buf;
				{
					LPCTSTR key = _T("class=\"new_link\">");
					int index = str.Find(key);
					buf = str.Mid(index + wcslen(key));
				}
				str = buf;
				{
					int index = str.Find(_T("</a> ("));
					buf = str.Left(index);
				}
				data.SetTitle(buf);

				// �t�q�h
				str = html_.GetAt(i);
				buf = util::XmlParser::GetElement(str, 1);
				buf = util::XmlParser::GetAttribute(buf, _T("href="));
				// "����菜��
				buf = buf.Right(buf.GetLength() -1);
				buf = buf.Left(buf.Find(_T("\"")));


				// �t�q�h
				// &�ŕ�������
				while (buf.Replace(_T("&amp;"), _T("&")));

				data.SetURL(buf);
				data.SetCommentCount(
					MixiUrlParser::GetCommentCount( buf ) );

				// URL �ɉ����ăA�N�Z�X��ʂ�ݒ�
				data.SetAccessType( util::EstimateAccessTypeByUrl(buf) );

				// �h�c��ݒ�
				buf = buf.Mid(buf.Find(_T("id=")) + wcslen(_T("id=")));
				buf = buf.Left(buf.Find(_T("&")));
				data.SetID(_wtoi(buf));

				// �O��̃C���f�b�N�X���擾
				ParserUtil::GetLastIndexFromIniFile(data.GetURL(), &data);

				// �R�~���j�e�B��
				{
					LPCTSTR key = _T("</a> (");
					int index = str.Find(key);
					key = _T("</a>");
					buf = str.Mid(index + wcslen(key));
				}
				// ���`�F�ŏ��ƍŌ�̊��ʂ���菜��
				buf.Trim();
				buf.Replace(_T("\n"), _T(""));
				buf = buf.Mid( 1, buf.GetLength()-2 );
				data.SetName(buf);
				out_.push_back( data );
			}
			else if ( dataFind ) {
				if( str.Find(_T("</table>")) != -1 ) {
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
		if( !reg.isCompiled() ) {
			if(! reg.compile( L"<a href=new_bbs.pl([?]page=[^>]+)>([^<]+)</a>" ) ) {
				MZ3LOGGER_FATAL( FAILED_TO_COMPILE_REGEX_MSG );
				return false;
			}
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
 * [list] list_message.pl �p�p�[�T
 * 
 * ���b�Z�[�W�ꗗ���擾
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
 * [list] new_friend_diary.pl �p�p�[�T
 * 
 * ���L�ꗗ���擾
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

		CString str;
		CString buf;
		CString key;

		BOOL findFlag = FALSE;
		BOOL dataFind = FALSE;
		int index;

		for (int i=76; i<count; i++) {

			str = html_.GetAt(i);

			if (findFlag == FALSE) {
				if (str.Find(_T("bg_w.gif")) != -1) {
					findFlag = TRUE;
				}
				continue;
			}
			else {

				// �u����\���v�A�u�O��\���v�̃����N�𒊏o����
				if( parseNextBackLink( nextLink, backLink, str ) ) {
					continue;
				}

				if (util::LineHasStringsNoCase( str, L"<img", L"pen.gif" ) ) {

					dataFind = TRUE;

					CMixiData data;
					data.SetAccessType(ACCESS_DIARY);

					ParserUtil::ChangeDate(
						util::XmlParser::GetElement(str, 3) + _T("") + util::XmlParser::GetElement(str, 5), 
						&data);

					// ���o��
					i++;
					str = html_.GetAt(i);
					// ��߂�>�܂Ńf�[�^���̂Ă�
					index = str.Find(_T("><"));
					buf = str.Mid(index + 10);
					str = buf;
					index = str.Find('>');
					buf = str.Mid(index+1);
					str = buf;
					key = _T("</a> (");
					index = str.Find(key);
					buf = str.Left(index);
					data.SetTitle(buf);

					// �t�q�h
					str = html_.GetAt(i);
					if (util::LineHasStringsNoCase( str, L"list_diary.pl" ) ) {
						i += 5;
						continue;
					}
					buf = util::XmlParser::GetElement(str, 2);
					data.SetURL(util::XmlParser::GetAttribute(buf, _T("href=")));

					// �h�c��ݒ�
					buf = data.GetURL();
					buf = buf.Mid(buf.Find(_T("id=")) + wcslen(_T("id=")));
					buf = buf.Left(buf.Find(_T("&")));
					data.SetID(_wtoi(buf));
					ParserUtil::GetLastIndexFromIniFile(data.GetURL(), &data);

					// ���O
					key = _T("</a> (");
					index = str.Find(key);
					key = _T("</a>");
					buf = str.Mid(index + key.GetLength());
					// �ŏ��ƍŌ�̊��ʂ���菜��
					buf.Trim();
					buf.Replace(_T("\n"), _T(""));
					buf = buf.Left(buf.GetLength() -1);
					buf = buf.Right(buf.GetLength() -1);
					data.SetName(buf);
					data.SetAuthor(buf);

					out_.push_back( data );
					i += 5;
				}
				else if (str.Find(_T("</table>")) != -1 && dataFind != FALSE) {
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

		MZ3LOGGER_DEBUG( L"ListNewFriendDiaryParser.parse() finished." );
		return true;
	}

private:
	/// �u����\���v�A�u�O��\���v�̃����N�𒊏o����
	static bool parseNextBackLink( CMixiData& nextLink, CMixiData& backLink, CString str )
	{
		// ���K�\���̃R���p�C���i���̂݁j
		static MyRegex reg;
		if( !reg.isCompiled() ) {
			if(! reg.compile( L"<a href=new_friend_diary.pl([?]page=[^>]+)>([^<]+)</a>" ) ) {
				MZ3LOGGER_FATAL( FAILED_TO_COMPILE_REGEX_MSG );
				return false;
			}
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
 * 
 * ���L�R�����g�L���������擾
 */
class NewCommentParser : public MixiListParser
{
public:
	static bool parse( CMixiDataList& out_, const CHtmlArray& html_ )
	{
		MZ3LOGGER_DEBUG( L"NewCommentParser.parse() start." );

		INT_PTR count = html_.GetCount();

		CString str;
		CString buf;
		CString key;

		BOOL findFlag = FALSE;
		BOOL dataFind = FALSE;

		int index;
		for (int i=76; i<count; i++) {

			str = html_.GetAt(i);

			if (findFlag == FALSE) {
				if (util::LineHasStringsNoCase( str, L"img/bg_w.gif>") ) {
					// �J�n�t���O�����B
					// �Ƃ肠����N�s��������B
					i+=10;
					findFlag = TRUE;
				}
				continue;
			}
			else {

				if (util::LineHasStringsNoCase( str, L"<img", L"pen_y.gif" ) ) {
					// ���ڔ���

					dataFind = TRUE;

					CMixiData data;
					data.SetAccessType(ACCESS_DIARY);

					// ���t
					ParserUtil::ChangeDate( util::XmlParser::GetElement(str, 3), &data );
					TRACE(_T("%s\n"), data.GetDate());

					// ���o��
					i+=2;
					str = html_.GetAt(i);
					key = _T("class=\"new_link\">");
					index = str.Find(key);
					buf = str.Mid(index + key.GetLength());
					buf.Trim();
					buf.Replace(_T("\n"), _T(""));
					data.SetTitle(buf);
					TRACE(_T("%s\n"), data.GetTitle());

					// �t�q�h
					buf = util::XmlParser::GetElement(str, 1);
					buf = util::XmlParser::GetAttribute(buf, _T("href="));
					buf = buf.Left(buf.GetLength() -1);
					buf = buf.Right(buf.GetLength() -1);
					data.SetURL(buf);
					TRACE(_T("%s\n"), data.GetURL());

					// �h�c��ݒ�
					buf = data.GetURL();
					buf = buf.Mid(buf.Find(_T("id=")) + wcslen(_T("id=")));
					buf = buf.Left(buf.Find(_T("&")));
					data.SetID(_wtoi(buf));
					ParserUtil::GetLastIndexFromIniFile(data.GetURL(), &data);

					// ���O
					// +1�`+2 �s�ڂɂ���
					for( int iInc=1; iInc<=2; iInc++ ) {
						str = html_.GetAt( ++i );

						CString author;
						if( util::GetBetweenSubString( str, L"(", L")", author ) < 0 ) {
							// not found.
							continue;
						}

						data.SetName( author );
						data.SetAuthor( author );
						TRACE(_T("%s\n"), data.GetName());
						break;
					}

					out_.push_back(data);
				}
				else if (str.Find(_T("</table>")) != -1 && dataFind != FALSE) {
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
 * [content] view_bbs.pl �p�p�[�T
 * 
 * �a�a�r�f�[�^�擾
 */
class ViewBbsParser : public MixiContentParser
{
public:
	static bool parse( CMixiData& mixi, const CHtmlArray& html_ ) 
	{
		MZ3LOGGER_DEBUG( L"ViewBbsParser.parse() start." );

		mixi.ClearAllList();

		INT_PTR lastLine = html_.GetCount();

		if (html_.GetAt(2).Find(_T("302 Moved")) != -1) {
			// ���݂͌���Ȃ�
			TRACE(_T("Moved\n"));
			return false;
		}

		BOOL findFlag = FALSE;
		BOOL bFound2ndTableEndTag = FALSE;

		const CString& tagBR  = _T("<br>");

		for (int i=180; i<lastLine; i++) {
			CString str = html_.GetAt(i);

			if (findFlag == FALSE) {
				// �t���O�𔭌�����܂ŉ�
				// �t���O�Fclass="h120"

				// "show_friend.pl" �܂�g�s�쐬�҂̃v���t�B�[�������N������΁A
				// �g�s�쐬�҂̃��[�U���Ɠ��e�������擾����B
				if ( util::LineHasStringsNoCase( str, L"show_friend.pl" ) ) {

					parseTopicAuthorAndCreateDate( i, html_, mixi );

					// ���̍s�ɂ͂Ȃ��̂Ŏ��̍s�ցB
					continue;
				}

				if (util::LineHasStringsNoCase( str, L"class=\"h120\"" ) ) {
					// </table>��T��
					findFlag = TRUE;

					// �Ƃ肠�������s
					mixi.AddBody(_T("\r\n"));

					// ��͑Ώ�(str) �̃p�^�[���͉��L�̂Q�ʂ�B
					// ���摜�Ȃ��p�^�[��1�F�i1�s�j
					// <table width="500" border="0" cellspacing="0" cellpadding="5"><tr><td class="h120">
					// <table><tr></tr></table>���e
					// ���摜�Ȃ��p�^�[��2�F�i1�s�j
					// <table width="500" border="0" cellspacing="0" cellpadding="5"><tr><td class="h120">
					// <table><tr></tr></table>���e</td></tr></table>
					// ���摜����p�^�[��1�F�i2�s�j
					// line1: <table width="500" border="0" cellspacing="0" cellpadding="5"><tr><td class="h120"><table>
					//        <tr><td width="130" height="140" align="center" valign="middle">
					//        <a href="javascript:void(0)" onClick="MM_openBrWindow('show_bbs_picture.pl?id=xxx&number=xxx',
					//        'pict','width=680,height=660,toolbar=no,scrollbars=yes,left=5,top=5')">
					//        <img src="http://ic52.mixi.jp/p/xxx.jpg" border="0"></a></td>
					// line2: </tr></table>�ʐ^�t���g�s�b�N</td></tr></table>
					// ���摜����p�^�[��2�F�i2�s�j
					// line1: <table width="500" border="0" cellspacing="0" cellpadding="5"><tr><td class="h120"><table>
					//        <tr><td width="130" height="140" align="center" valign="middle">
					//        <a href="javascript:void(0)" onClick="MM_openBrWindow('show_bbs_picture.pl?id=xxx&number=xxx',
					//        'pict','width=680,height=660,toolbar=no,scrollbars=yes,left=5,top=5')">
					//        <img src="http://ic39.mixi.jp/p/xxx.jpg" border="0"></a></td>
					// line2: </tr></table>�{����������ƒ���
					// ���摜����p�^�[��3�F�i3�s�j
					// line1: <table width="500" border="0" cellspacing="0" cellpadding="5"><tr><td class="h120"><table>
					//        <tr><td width="130" height="140" align="center" valign="middle">
					//        <a href="javascript:void(0)" onClick="MM_openBrWindow('show_bbs_picture.pl?id=xxx&comm_id=xxx&number=xxx',
					//        'pict','width=680,height=660,toolbar=no,scrollbars=yes,left=5,top=5')">
					//        <img src="http://ic57.mixi.jp/p/xxx.jpg" border="0"></a></td>
					// line2: <td width="130" height="140" align="center" valign="middle">
					//        <a href="javascript:void(0)" onClick="MM_openBrWindow('show_bbs_picture.pl?id=xxx&comm_id=xxx&number=xxx',
					//        'pict','width=680,height=660,toolbar=no,scrollbars=yes,left=5,top=5')">
					//        <img src="http://ic43.mixi.jp/p/xxx.jpg" border="0"></a></td>
					// line3: </tr></table>�{���\�������H

					// </table> �������܂Ŋe�s���o�b�t�@�����O�B
					// ����́A�摜����������悤�ȏꍇ�ɁA</table> �����̍s�Ɋ܂܂�邽�߁B

					// ���݂̍s���o�b�t�@�����O
					CString line = str;

					// </table> ���Ȃ���� </table> ��������܂Ńo�b�t�@�����O
					if( !util::LineHasStringsNoCase( line, L"</table>" ) ) {

						while( i<lastLine ) {
							// ���̍s���t�F�b�`
							const CString& nextLine = html_.GetAt( ++i );

							// �o�b�t�@�����O
							line += nextLine;

							// </table> ������ΏI��
							if( util::LineHasStringsNoCase( nextLine, L"</table>" ) ) {
								break;
							}
						}
					}

					// �ŏ��� </table> �܂łɂ��āA
					// �摜�����N������΁A��͂��A�����B
					{
						CString strBeforeTableEndTag;
						util::GetBeforeSubString( line, L"</table>", strBeforeTableEndTag );
						if( LINE_HAS_DIARY_IMAGE(strBeforeTableEndTag) ) {
							ParserUtil::AddBodyWithExtract( mixi, strBeforeTableEndTag );
						}
					}

					// �ŏ��� </table> �ȍ~���擾�B
					CString strAfterTableEndTag;
					util::GetAfterSubString( line, L"</table>", strAfterTableEndTag );

					// �ŏ��� </table> �ȍ~����́�����
					ParserUtil::AddBodyWithExtract( mixi, strAfterTableEndTag );

					// ����� </table> ������(�p�^�[��2)�Ȃ�A�I���t���O�𗧂ĂĂ���
					if( util::LineHasStringsNoCase( strAfterTableEndTag, L"</table>" ) ) {
						bFound2ndTableEndTag = TRUE;
					}

				}else{
					// �t���O���Ȃ��̂œǂݔ�΂�
				}
			}
			else {
				// �t���O�����ς݁B
				// �ŏ��� </table> ��������s�܂ł͉�͍ς݁B
				// �u���Ƀg�s�b�N�̏I���t���O�i2�ڂ�</table>�^�O�j�������ς݁v
				// �܂���
				// �u</table> �𔭌��v����΃R�����g�擾�������s���B

				if( util::LineHasStringsNoCase(str,L"</table>") || bFound2ndTableEndTag ) {
					if (str.Find(tagBR) != -1) {
						str.Replace(_T("</td></tr>"), _T(""));
						str.Replace(_T("</tbody>"), _T(""));
						str.Replace(_T("</table>"), _T(""));
						ParserUtil::AddBodyWithExtract( mixi, str );
					}

					// �R�����g�̊J�n�܂ŒT��
					mixi.ClearChildren();

					int index = i;
					while( index < lastLine ) {
						index = parseBBSComment(index, lastLine, &mixi, html_);
						if( index == -1 ) {
							break;
						}
					}
					if (index == -1 || index >= lastLine) {
						break;
					}
				}else{
					// 2�ڂ�</table>�^�O�������Ȃ̂ŁA��́�����
					ParserUtil::AddBodyWithExtract( mixi, str );
				}
			}
		}

		// �y�[�W�ړ������N�̒��o
		parsePageLink( mixi, html_ );

		// �u�ŐV�̃g�s�b�N�v�̒��o
		parseRecentTopics( mixi, html_ );

		MZ3LOGGER_DEBUG( L"ViewBbsParser.parse() finished." );
		return true;
	}

private:

	/**
	 * i �s�ڂ��瓊�e���[�U�����擾���A������ɓ��e������T������
	 */
	static bool parseTopicAuthorAndCreateDate( int i, const CHtmlArray& html_, CMixiData& mixi )
	{
		// ���e���[�U��
		const CString& line = html_.GetAt( i );
		MixiUrlParser::GetAuthor(line, &mixi);

		// ���t���擾
		// i �s�ځ�2�s�ڂ܂ŒT������B
		// �ʏ��3�s���x�O�ɉ��L������B
		// <td rowspan="3" width="110" bgcolor="#ffd8b0" align="center" valign="top" nowrap>
		// 2007�N05��10��<br>19:57</td>
		for (int j=i; j>0; j--) {
			const CString& line = html_.GetAt(j);
			if( util::LineHasStringsNoCase( line, L"�N", L"��", L"��" ) ) {
				// TODO XmlParser �g��Ȃ������_��ł́H(takke)
				ParserUtil::ChangeDate(
					util::XmlParser::GetElement(line, 2) + _T(" ") + util::XmlParser::GetElement(line, 4),
					&mixi);
				return true;
			}
		}

		// �������̂��ߏI��
		return false;
	}

	/**
	 * �a�a�r�R�����g�擾 �g�s�b�N �R�����g�ꗗ
	 */
	static int parseBBSComment(int sIndex, int eIndex, CMixiData* data, const CHtmlArray& html_ ) 
	{
		CString str;

		int retIndex = eIndex;
		CMixiData cmtData;
		BOOL findFlag = FALSE;
		CString date;
		CString comment;
		int index;

		for (int i=sIndex; i<eIndex; i++) {
			str = html_.GetAt(i);

			if (findFlag == FALSE) {
				CString buf;

				if (str.Find(_T("<!-- ADD_COMMENT: start -->")) != -1) {
					// �R�����g�Ȃ�
					// ���e�A�h���X�̎擾
					GetPostAddress(i, eIndex, html_, *data);
					retIndex = -1;
					break;
				}
				else if (str.Find(_T("<!-- COMMENT: end -->")) != -1) {
					// �R�����g�S�̂̏I���^�O����
					GetPostAddress(i, eIndex, html_, *data);
					retIndex = -1;
					break;
				}
				//�R�����g�I�������ǉ�(2006/11/19 icchu�ǉ�)
				else if (str.Find(_T("add_bbs_comment.pl")) != -1) {
					// �R�����g�S�̂̏I���^�O����
					GetPostAddress(i, eIndex, html_, *data);
					retIndex = -1;
					break;
				}
				//2006/11/19 icchu�ǉ������܂�
				else if ((index = str.Find(_T("show_friend.pl"))) != -1) {

					str = html_.GetAt(i-1);
					util::GetAfterSubString( str, L"<b>", buf );
					buf = util::XmlParser::GetElement(buf, 1);
					while(buf.Replace(_T("&nbsp;"), _T("")));
					cmtData.SetCommentIndex(_wtoi(buf));

					str = html_.GetAt(i);
					buf = str.Mid(index);
					MixiUrlParser::GetAuthor(buf, &cmtData);

					if (html_.GetAt(i-4).Find(_T("checkbox")) != -1) {
						// �����Ǘ��R�~��
						// ����
						str = html_.GetAt(i-6);
						buf = util::XmlParser::GetElement(str, 1);
						date = buf;
						str = html_.GetAt(i-5);
						buf = util::XmlParser::GetElement(str, 1);
						date += _T(" ");
						date += buf;
					}
					else {
						if (html_.GetAt(i-5).Find(_T(":")) != -1) {
							// ����
							str = html_.GetAt(i-5);
							buf = util::XmlParser::GetElement(str, 1);
							date = buf;
							buf = util::XmlParser::GetElement(str, 3);
							date += _T(" ");
							date += buf;
						}
						else {
							// ����
							str = html_.GetAt(i-5);
							buf = util::XmlParser::GetElement(str, 1);
							date = buf;
							str = html_.GetAt(i-4);
							buf = util::XmlParser::GetElement(str, 1);
							date += _T(" ");
							date += buf;
						}
					}

					ParserUtil::ChangeDate(date, &cmtData);

					findFlag = TRUE;

				}
			}
			else {

				if( util::LineHasStringsNoCase(str, L"class=\"h120\"") ) {
					// �R�����g�{���擾
					str = html_.GetAt(i);

					cmtData.AddBody(_T("\r\n"));
					ParserUtil::AddBodyWithExtract( cmtData, str );

					i++;
					for( ; i+1<eIndex; i++ ) {
						const CString& line  = html_.GetAt(i);
						const CString& line2 = html_.GetAt(i+1);

						if (util::LineHasStringsNoCase( line, L"</tr>") && 
							util::LineHasStringsNoCase( line2,L"</table>") )
						{
							// ��ԍŌ�̉��s���폜
							ParserUtil::AddBodyWithExtract( cmtData, line );

							// �I���^�O���o�Ă����̂ł����ŏI���
							retIndex = i + 5;
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
 * [content] view_enquete.pl �p�p�[�T
 * 
 * �A���P�[�g�f�[�^�擾
 */
class ViewEnqueteParser : public MixiContentParser
{
public:
	static bool parse( CMixiData& data_, const CHtmlArray& html_ )
	{
		MZ3LOGGER_DEBUG( L"ViewEnqueteParser.parse() start." );

		data_.ClearAllList();
		data_.ClearChildren();

		INT_PTR lastLine = html_.GetCount();

		/*
		 * ��͕��j�F
		 * ���ݖ���e��́B
		 *   "BGCOLOR=#FFD8B0", "COLOR=#996600", "�ݖ���e" ������s����������A�ݖ���e��͂��s���B
		 *   �ڍׂ� parseBody �̃R�����g�Q�ƁB
		 *   ����ȍ~���u�A���P�[�g�J�n�v�Ƃ���B
		 * �����҉�́B
		 *   "BGCOLOR=#FFD8B0", "COLOR=#996600", "����" ������s����������A���҉�͂��s���B
		 *   ���̍s�ɁA���L�̌`���Ŋ��҂�����B
		 *   <td BGCOLOR=#FFFFFF><a href="show_friend.pl?id=xxx">���O</a></td></tr>
		 * ���W�v���ʉ�́B
		 *   ���L�̌`���̍s����W�v���ʊJ�n�B
		 *   <td BGCOLOR=#FFD8B0 ALIGN=center><font COLOR=#996600>�W�v����</font></td>
		 *   �ڍׂ� parseEnqueteResult �Q�ƁB
		 * ��</table> �݂̂̍s������΁A�u�A���P�[�g�I���v�Ƃ��A�R�����g��͂Ɉڂ�B
		 */
		bool bInEnquete = false;
		int iLine=180;
		for( ; iLine<lastLine; iLine++ ) {
			const CString& line = html_.GetAt(iLine);

			// ���ݖ���e���
			if( util::LineHasStringsNoCase( line, L"BGCOLOR=#FFD8B0", L"COLOR=#996600", L"�ݖ���e" ) )
			{
				if( !parseBody( data_, html_, iLine ) )
					return false;
				bInEnquete = true;
				continue;
			}

			// �����҉��
			if( util::LineHasStringsNoCase( line, L"BGCOLOR=#FFD8B0", L"COLOR=#996600", L"����" ) )
			{
				// ���̍s�ɂ���B
				const CString& line = html_.GetAt( ++iLine );
				MixiUrlParser::GetAuthor( line, &data_ );
				data_.SetDate(_T(""));
				continue;
			}

			// ���W�v���ʉ�́B
			// <td BGCOLOR=#FFD8B0 ALIGN=center><font COLOR=#996600>�W�v����</font></td>
			if( util::LineHasStringsNoCase( line, L"BGCOLOR=#FFD8B0", L"COLOR=#996600", L"�W�v����" ) )
			{
				if( !parseEnqueteResult( data_, html_, iLine ) )
					return false;
				continue;
			}

			// ���A���P�[�g�I���H
			if( bInEnquete && util::LineHasStringsNoCase( line, L"</table>" ) ) {
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
	 * (1) "</table>" �������܂Ŗ����B���̒��ォ��A�ݖ���e�{���B��͂��āAAddBody ����B
	 * (2) "</td></tr>" �����ꂽ��I���B
	 */
	static bool parseBody( CMixiData& mixi, const CHtmlArray& html, int& iLine )
	{
		++iLine;

		mixi.AddBody(_T("\r\n"));
		const int lastLine = html.GetCount();

		for( ; iLine<lastLine; iLine++ ) {
			const CString& line = html.GetAt(iLine);

			// </table> ������΁A���̌���{���Ƃ���B
			CString target;
			if( util::GetAfterSubString( line, L"</table>", target ) >= 0 ) {
				// </table> �����B
			}else{
				// </table> �������B
				target = line;
			}

			// </td></tr> ������΁A���̑O��ǉ����A�I���B
			// �Ȃ���΁A���̍s��ǉ����A���̍s�ցB
			if( util::GetBeforeSubString( target, L"</td></tr>", target ) < 0 ) {
				// </td></tr> ��������Ȃ������B
				ParserUtil::AddBodyWithExtract( mixi, target );
			}else{
				// </td></tr> �����������̂ŏI���B
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
	 * �W�v���ʂ��擾����B
	 *
	 * iLine �� "�W�v����" �����݂���s�B
	 *
	 * �� "</table>" �����ꂽ��I���B
	 *
	 * �� ���L�̌`���ŁA����I�����ɑ΂���v�f������B
<tr>
<td bgcolor="#fdf9f2">
�A���P�[�g�I�������e<br>
<img alt="" src="http://img.mixi.jp/img/bar.gif" width="355" height="16" hspace="1" vspace="1">
</td>
<td width="50" bgcolor="#fff4e0" align="right" valign="bottom"><font color="#996600">���[�� (�S����%)</font></td>
</tr>
     * ��́A���`���āAAddBody ����B
	 *
	 * �� ���L�̌`���ŁA���v������̂ŁA��́A���`���āAAddBody����B
<td height="26" bgcolor="#f7f0e6"><font color="#996600">�� �v</font>&nbsp;</td>
<td width="50" bgcolor="#ffd8b0"><b><font color="#996600">34</font></b></td>
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

			// </table> ������΁A�I���B
			if( util::LineHasStringsNoCase( line, L"</table>" ) ) {
				break;
			}

			// �I�����A���[���A�S��������͂��āAAddBody ����B
			if( util::LineHasStringsNoCase( line, L"<td bgcolor=\"#fdf9f2\">" ) ) {
				// ���E�l�`�F�b�N
				if( iLine+5 >= lastLine ) {
					break;
				}
				// ���̍s�ɖ{��������B
				const CString& line1 = html.GetAt(++iLine);
				CString item;
				util::GetBeforeSubString( line1, L"<br>", item );

				// +3 �s�ڂɉ��L�̌`���ŁA���[���A�S����������B
				// <td width="50" bgcolor="#fff4e0" align="right" valign="bottom"><font color="#996600">xx (yy%)</font></td>
				iLine += 3;
				const CString& line2 = html.GetAt( iLine );
				CString target;
				util::GetAfterSubString( line2, L"#996600\">", target );
				// xx (yy%)</font></td>
				CString num_rate;
				util::GetBeforeSubString( target, L"</font>", num_rate );

				CString str;
				str.Format( L"  ��%s\r\n", item );
				mixi.AddBody( str );
				str.Format( L"      %s\r\n", num_rate );
				mixi.AddBody( str );
			}

			// ���v����͂���B
			if( util::LineHasStringsNoCase( line, L"<td", L"#f7f0e6", L"#996600", L"�� �v" ) )
			{
				// ���̍s�ɍ��v������B
				const CString& line1 = html.GetAt( ++iLine );

				CString total;
				util::GetBetweenSubString( line1, L"#996600\">", L"</font>", total );

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
		CString str;

		int eIndex = html_.GetCount();
		int retIndex = eIndex;

		CMixiData cmtData;
		BOOL findFlag = FALSE;
		CString buf;
		CString comment;
		int index;

		for (int i=sIndex; i<eIndex; i++) {
			str = html_.GetAt(i);

			if (findFlag == FALSE) {

				if (str.Find(_T("<!-- ADD_COMMENT: start -->")) != -1) {
					// �R�����g�Ȃ�
					GetPostAddress(i, eIndex, html_, *data);
					retIndex = -1;
					break;
				}
				else if (str.Find(_T("<!-- COMMENT: end -->")) != -1) {
					// �R�����g�S�̂̏I���^�O����
					GetPostAddress(i, eIndex, html_, *data);
					retIndex = -1;
					break;
				}
				//�R�����g�I�������ǉ�(2006/11/19 icchu�ǉ�)
				else if (str.Find(_T("add_enquete_comment.pl")) != -1) {
					// �R�����g�S�̂̏I���^�O����
					GetPostAddress(i, eIndex, html_, *data);
					retIndex = -1;
					break;
				}
				//2006/11/19 icchu�ǉ������܂�
				else if ((index = str.Find(_T("show_friend.pl"))) != -1) {

					// 2�s�O�ɉ��L�̌`���ŃR�����g�ԍ������݂���B
					// &nbsp;<font color="#f8a448"><b>&nbsp;&nbsp;1</b>:</font>&nbsp;
					const CString& line1 = html_.GetAt( i-2 );

					CString strIndex;
					util::GetBetweenSubString( line1, L"<b>", L"</b>", strIndex );
					while( strIndex.Replace( L"&nbsp;", L"" ) );
					cmtData.SetCommentIndex( _wtoi(strIndex) );

					// ���̍s�ɁA�R�����g�҂����L�̌`���ő��݂���B
					// <a href="show_friend.pl?id=xxxx">�Ȃ܂�</a>
					const CString& line2 = html_.GetAt(i);
					MixiUrlParser::GetAuthor( line2, &cmtData );

					// 10�s�O�ɁA���L�̌`���Ŏ��������݂���B
					// 2006�N12��xx��<br>HH:MM<br>
					const CString& line3 = html_.GetAt(i-10);
					CString date = line3;
					while( date.Replace( L"<br>", L" " ) );

					ParserUtil::ChangeDate(date, &cmtData);

					findFlag = TRUE;
				}
			}
			else {

				if( util::LineHasStringsNoCase( str, L"<tr", L"<td", L"class=\"h120\"") ) {
					// �R�����g�R�����g�{���擾
					str = html_.GetAt(i);

					// ----------------------------------------
					// �A���P�[�g�̃p�^�[��
					// ----------------------------------------
					cmtData.AddBody(_T("\r\n"));

					// [    <tr><td class="h120" width="500">�O�P�Q�R�S�T�U�V�W�X�O]
					// ��
					// [<td class="h120" width="500">�O�P�Q�R�S�T�U�V�W�X�O]

					// [    <tr><td class="h120" width="500">�e�X�g����</td></tr></table>]
					// ��
					// [<td class="h120" width="500">�e�X�g����</td></tr></table>]
					util::GetAfterSubString( str, L">", buf );

					if( util::GetBeforeSubString( buf, L"</td></tr></table>", buf ) > 0 ) {
						// [<td class="h120" width="500">�e�X�g����</td></tr></table>]
						// ��
						// [<td class="h120" width="500">�e�X�g����]

						// �I���^�O���������ꍇ
						ParserUtil::AddBodyWithExtract( cmtData, buf );
						retIndex = i + 5;
						break;
					}

					// ����ȊO�̏ꍇ
					buf.Replace(_T("\n"), _T("\r\n"));
					ParserUtil::AddBodyWithExtract( cmtData, buf );

					while( i<eIndex ) {
						i++;

						const CString& line  = html_.GetAt(i);
						const CString& line2 = html_.GetAt(i+1);
						if (util::LineHasStringsNoCase(line, L"</td>") &&
							util::LineHasStringsNoCase(line2,L"</tr>") ) 
						{
							// ��ԍŌ�̉��s���폜
							buf = line;
							buf.Replace(_T("\n"), _T(""));
							ParserUtil::AddBodyWithExtract( cmtData, buf );

							// �I���^�O���o�Ă����̂ł����ŏI���
							retIndex = i + 5;
							break;
						}
						else if ((index = line.Find(_T("</td></tr>"))) != -1) {
							// �I���^�O����
							buf = line.Left(index);
							ParserUtil::AddBodyWithExtract( cmtData, buf );
							retIndex = i + 5;
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
 * 
 * �C�x���g�f�[�^�擾
 */
class ViewEventParser : public MixiContentParser
{
public:
	static bool parse( CMixiData& data_, const CHtmlArray& html_ )
	{
		MZ3LOGGER_DEBUG( L"ViewEventParser.parse() start." );

		data_.ClearAllList();

		INT_PTR count;
		count = html_.GetCount();

		CString str;
		CString buf;
		BOOL findFlag = FALSE;
		BOOL endFlag = FALSE;

		int index;

		for (int i=180; i<count; i++) {
			str = html_.GetAt(i);

			if (findFlag == FALSE) {

				if (str.Find(_T("����")) != -1) {

					// ���̍s�Ɋ��ҁiAuthor�j������͂��B
					// <td bgcolor=#FDF9F2>&nbsp;<a href="show_friend.pl?id=xxx">�Ȃ܂�</a>
					i++;
					str = html_.GetAt(i);

					index = str.Find(_T("show_friend.pl"));
					if( index < 0 ) {
						MZ3LOGGER_ERROR( L"show_friend.pl ��������܂��� [" + str + L"]" );
						break;
					}
					buf = str.Mid(index);
					MixiUrlParser::GetAuthor( buf, &data_ );

					// �J�Ó����̎擾
					i += 6;
					str = html_.GetAt(i);
					ParserUtil::UnEscapeHtmlElement(str);
					buf = _T("�J�Ó��� ") + str;
					data_.AddBody(_T("\r\n"));
					data_.AddBody(buf);
					data_.AddBody(_T("\r\n"));

					// �J�Ïꏊ�̎擾
					i += 6;
					str = html_.GetAt(i);
					ParserUtil::UnEscapeHtmlElement(str);
					buf = _T("�J�Ïꏊ ") + str;
					data_.AddBody(buf);
					data_.AddBody(_T("\r\n"));

					i++;
					continue;
				}

				// �t���O�𔭌�����܂ŉ�
				if (util::LineHasStringsNoCase( str, L"�ڍ�") ) {
					findFlag = TRUE;

					// </td></tr>��T��
					i++;
					str = html_.GetAt(i);

					data_.AddBody(_T("\r\n"));

					// [<td bgcolor=#ffffff><table BORDER=0 CELLSPACING=0 CELLPADDING=5><tr><td CLASS=h120 width="410">�e�X�g�p�ł�</td></tr></table></td>]

					if( util::GetBeforeSubString( str, L"</td></tr>", buf ) > 0 ) {
						// 1�s�Ȃ̂ŏI���B
						// [<td bgcolor=#ffffff><table BORDER=0 CELLSPACING=0 CELLPADDING=5><tr><td CLASS=h120 width="410">�e�X�g�p�ł�]
						ParserUtil::AddBodyWithExtract( data_, buf );

						// �����ŏI��
						endFlag = TRUE;
					}else{
						// ���I���Ȃ̂ōs�����̂܂ܓ����B
						ParserUtil::AddBodyWithExtract( data_, str );
					}
					continue;
				}
			}
			else {

				if (endFlag == FALSE) {
					// </td></tr> �������܂ŉ�́��ǉ��B
					if( util::GetBeforeSubString( str, L"</td></tr>", buf) >= 0 ) {
						// �ڍׁA�I��
						ParserUtil::AddBodyWithExtract( data_, buf );
						endFlag = TRUE;
					}else{
						// ���I���Ȃ̂ōs�����̂܂ܓ����B
						ParserUtil::AddBodyWithExtract( data_, str );
					}
				}
				else {
					data_.SetDate(_T(""));

					// �R�����g�̊J�n�܂ŒT��
					data_.ClearChildren();

					index = i;
					while( index<count ) {
						index = parseEventComment(index, count, &data_, html_);
						if (index == -1) {
							break;
						}
					}
					if (index == -1 || index >= count) {
						break;
					}
				}
			}
		}

		// �y�[�W�ړ������N�̒��o
		parsePageLink( data_, html_ );

		// �u�ŐV�̃g�s�b�N�v�̒��o
		parseRecentTopics( data_, html_ );

		MZ3LOGGER_DEBUG( L"ViewEventParser.parse() finished." );
		return true;
	}

	/**
	 * �C�x���g�R�����g�擾
	 */
	static int parseEventComment(int sIndex, int eIndex, CMixiData* data, const CHtmlArray& html_ )
	{
		CString str;

		int retIndex = eIndex;
		CMixiData cmtData;
		BOOL findFlag = FALSE;
		CString date;
		CString comment;
		int index;

		for (int i=sIndex; i<eIndex; i++) {
			str = html_.GetAt(i);

			if (findFlag == FALSE) {

				if( util::LineHasStringsNoCase( str, L"<!-- end : Loop -->" ) ||
					util::LineHasStringsNoCase( str, L"<!-- ADD_COMMENT: start -->" ) ||
					util::LineHasStringsNoCase( str, L"<!-- COMMENT: end -->" ) ||
					util::LineHasStringsNoCase( str, L"add_event_comment.pl" ) ) 
				{
					// �R�����g�S�̂̏I���^�O����
					GetPostAddress(i, eIndex, html_, *data);
					return -1;
				}
				
				if ((index = str.Find(_T("show_friend.pl"))) != -1) {

					CString buf = str.Mid(index);
					MixiUrlParser::GetAuthor(buf, &cmtData);

					// �R�����g�ԍ���O�̍s����擾����
					// [<font color="#f8a448"><b>&nbsp;25</b>&nbsp;:</font>]
					str = html_.GetAt(i-1);

					if( util::GetBetweenSubString( str, L"<b>", L"</b>", buf ) < 0 ) {
						MZ3LOGGER_ERROR( L"�R�����g�ԍ����擾�ł��܂���Bmixi �d�l�ύX�H" );
						return -1;
					}

					while(buf.Replace(_T("&nbsp;"), _T("")));
					cmtData.SetCommentIndex(_wtoi(buf));

					// ����
					if (html_.GetAt(i-4).Find(_T("checkbox")) != -1) {
						// �����Ǘ��R�~��
						date = util::XmlParser::GetElement(html_.GetAt(i-6), 1) + _T(" ")
							+ util::XmlParser::GetElement(html_.GetAt(i-5), 1);
						ParserUtil::ChangeDate(date, &cmtData);
					}
					else {
						date = util::XmlParser::GetElement(html_.GetAt(i-5), 1) + _T(" ")
							+ util::XmlParser::GetElement(html_.GetAt(i-4), 1);
						ParserUtil::ChangeDate(date, &cmtData);
					}

					findFlag = TRUE;
				}
			}
			else {

				// �R�����g�R�����g�{���擾

				if( util::LineHasStringsNoCase( str, L"<td", L"class=h120" ) ||
					util::LineHasStringsNoCase( str, L"<td", L"class=\"h120\"" ) )
				{
					cmtData.AddBody(_T("\r\n"));

					// [<tr><td class="h120" width="500">�Ă���</td></tr>]

					ParserUtil::AddBodyWithExtract( cmtData, str );

					CString line2 = html_.GetAt(i+1);
					if( util::LineHasStringsNoCase( str,   L"</td>" ) &&
						util::LineHasStringsNoCase( line2, L"</table>" ) )
					{
						// �I���^�O���������ꍇ
						retIndex = i + 5;
					}else{
						// �I���^�O�������̏ꍇ�F
						// �I���^�O�������܂ŉ�́��ǉ��B
						while( i<eIndex ) {
							i++;
							const CString& line = html_.GetAt(i);

							CString buf;
							if( util::GetBeforeSubString( line, L"</td>", buf ) >= 0 ||
								util::GetBeforeSubString( line, L"</table>", buf ) >= 0 ) 
							{
								// �I���^�O����
								ParserUtil::AddBodyWithExtract( cmtData, buf );
								retIndex = i + 5;
								break;
							}

							ParserUtil::AddBodyWithExtract( cmtData, line );
						}
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
 * [content] view_message.pl �p�p�[�T
 * 
 * ���b�Z�[�W�擾
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
			if( util::LineHasStringsNoCase( str, L"<font", L" COLOR=#996600", L">", L"���@�t" ) ) {
				CString buf = str.Mid(str.Find(_T("</font>")) + wcslen(_T("</font>")));
				while( buf.Replace(_T("&nbsp;"), _T(" ")) );
				buf.Replace(_T(":"), _T(""));
				buf.Replace(_T(" "), _T(""));
				buf.Replace(_T("��"), _T(":"));
				ParserUtil::ChangeDate(buf, &data_);
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

/**
 * [list] show_log.pl �p�p�[�T
 * 
 * �����Ƃ̎擾
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
				ParserUtil::ChangeDate( strDate, &data );
				data.SetMyMixi( bMyMixi );

				out_.push_back( data );

			}
		}

		MZ3LOGGER_DEBUG( L"ShowLogParser.parse() finished." );
		return true;
	}
};

/**
 * [list] list_diary.pl �p�p�[�T
 * 
 * �����̓��L�ꗗ�擾
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

		for (int i=200; i<count; i++) {
			const CString& str = html_.GetAt(i);

			if( findFlag ) {
				if( str.Find(_T("</td></tr></table>")) != -1 ) {
					// �I��
					break;
				}

				// �u�O��\���v�u����\���v�̒��o
				LPCTSTR key = L"=#EED6B5>";
				if( str.Find( key ) != -1 ) {
					// �����N���ۂ��̂Ő��K�\���}�b�`���O�Œ��o
					if( parseNextBackLink( nextLink, backLink, str ) ) {
						continue;
					}
				}
			}

			// ���O�̎擾
			// "bg_w.gif" ������A"<font COLOR=#605048>" ������A"�̓��L" ������s�B
			if (str.Find(_T("bg_w.gif")) != -1 && str.Find(_T("�̓��L")) != -1) {
				// "<font COLOR=#605048>" �� "�̓��L" �ň͂܂ꂽ�����𒊏o����
				CString name;
				util::GetBetweenSubString( str, L"<font COLOR=#605048>", L"�̓��L", name );

				// ���O
				data.SetName( name );
				data.SetAuthor( name );
			}

			const CString& key = _T("<td bgcolor=\"#FFF4E0\">&nbsp;");
			if (str.Find(key) != -1) {
				findFlag = TRUE;

				// �^�C�g��
				// ��͑ΏہF
				// <td bgcolor="#FFF4E0">&nbsp;<a href="view_diary.pl?id=xxx&owner_id=xxx">�^�C�g��</a></td>
				CString buf;
				util::GetBetweenSubString( str, key, L"</td>", buf );

				CString title;
				util::GetBetweenSubString( buf, L">", L"</", title );
				data.SetTitle( title );

				// ���t
				const CString& str = html_.GetAt(i-1);
        //2007/06/20 �������イ���L���e�s��Ή�
				ParserUtil::ChangeDate(util::XmlParser::GetElement(str, 5), util::XmlParser::GetElement(str, 7), &data);

				for (int j=i; j<count; j++) {
					const CString& str = html_.GetAt(j);

					LPCTSTR key = _T("<a href=\"view_diary.pl?id");
					if (str.Find(key) != -1) {
						CString uri;
						util::GetBetweenSubString( str, _T("<a href=\""), L"\">", uri );
						data.SetURL( uri );

						// �h�c��ݒ�
						CString id;
						util::GetBetweenSubString( uri, L"id=", L"&", id );
						data.SetID( _wtoi(id) );
						ParserUtil::GetLastIndexFromIniFile( data.GetURL(), &data );

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
		if( !reg.isCompiled() ) {
			if(! reg.compile( L"<a href=list_diary.pl([?]page=[^>]+)>([^<]+)</a>" ) ) {
				MZ3LOGGER_FATAL( FAILED_TO_COMPILE_REGEX_MSG );
				return false;
			}
		}

		return parseNextBackLinkBase( nextLink, backLink, str, reg, L"list_diary.pl", ACCESS_LIST_MYDIARY );
	}

};

/**
 * [list] list_comment.pl �p�p�[�T
 * 
 * �R�����g�ꗗ�̎擾
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
		 * - <img src=http://img.mixi.jp/img/pen_y.gif ALIGN=left WIDTH=14 HEIGHT=16>
		 *   ��������΂������獀�ڊJ�n�Ƃ݂Ȃ�
		 * - ���̍s��
		 *   <img ...>2006�N09��20�� 07:47</td>
		 *   �𐳋K�\���Ńp�[�X���A�����𒊏o����
		 * - ��L��2�s����
		 *   <a href="view_diary.pl?id=ZZZ&owner_id=ZZZ">���e</a> (Author)
		 *   �Ƃ����`���ō��ڂ����݂���B
		 * - ����𐳋K�\���Ńp�[�X���A�ǉ�����B
		 * - ���ڂ��������Ĉȍ~�ɁA
		 *   </table>
		 *   ������΁A�������I������
		 */

		// ���ڊJ�n��T��
		bool bInItems = false;	// ���ڂ������������B
		int iLine = 76;		// �Ƃ肠�����ǂݔ�΂�
		for( ; iLine<count; iLine++ ) {
			const CString& str = html_.GetAt(iLine);

			if( str.Find( L"<img" ) != -1 && str.Find( L"pen_y.gif" ) != -1 ) {
				// ���ڔ����B
				bInItems = true;

				//--- �����̒��o
				// ���K�\���̃R���p�C���i���̂݁j
				static MyRegex regDate;
				if( !regDate.isCompiled() ) {
					if(! regDate.compile( L"<img.*>(.*)</" ) ) {
						MZ3LOGGER_FATAL( FAILED_TO_COMPILE_REGEX_MSG );
						return false;
					}
				}
				// �T��
				if( regDate.exec(str) == false || regDate.results.size() != 2 ) {
					// �������B
					continue;
				}
				// �����B
				const std::wstring& date = regDate.results[1].str;	// ����������

				//--- ���e�̒��o
				// 2�s���ȍ~�ɂ���
				// �����s�ɂ킽���Ă���ꍇ������̂ŁA�Ƃ肠�����P�O�s�v���t�F�b�`�B
				// �A���A</a> ��������ΏI���B
				CString target = L"";
				for( int i=0; i<10 && iLine+2+i < count; i++ ) {
					CString s = html_.GetAt(iLine+2+i);
					target += s;
					if( s.Find( L"</a>" ) >= 0 ) {
						break;
					}
				}
				// ���s�͏���
				while( target.Replace( L"\n", L"" ) );

				// ���K�\���̃R���p�C���i���̂݁j
				static MyRegex regBody;
				if( !regBody.isCompiled() ) {
					if(! regBody.compile( 
							L"<a href=\"view_diary.pl\\?id=([0-9]+)&owner_id=([0-9]+)\">(.*)</a> \\((.*)\\)" ) )
					{
						MZ3LOGGER_FATAL( FAILED_TO_COMPILE_REGEX_MSG );
						return false;
					}
				}

				// �T��
				if( regBody.exec(target) == false || regBody.results.size() != 5 ) {
					// �������B
					continue;
				}

				// �����B
				const std::wstring& id        = regBody.results[1].str;	// id
				const std::wstring& ownder_id = regBody.results[2].str;	// ownder_id
				const std::wstring& body      = regBody.results[3].str;	// �����N������
				const std::wstring& author    = regBody.results[4].str;	// ���e��

				// URL �č\�z
				CString url;
				url.Format( L"view_diary.pl?id=%s&owner_id=%s", id.c_str(), ownder_id.c_str() );

				// mixi �f�[�^�̍쐬
				{
					CMixiData data;
					data.SetAccessType(ACCESS_DIARY);

					// ���t
					ParserUtil::ChangeDate( date.c_str(), &data );

					// ���o��
					data.SetTitle( body.c_str() );

					// �t�q�h
					data.SetURL( url );

					// �h�c��ݒ�
					data.SetID(_wtoi(id.c_str()));
					ParserUtil::GetLastIndexFromIniFile(data.GetURL(), &data);

					// ���O
					data.SetName( author.c_str() );
					data.SetAuthor( author.c_str() );

					out_.push_back(data);
				}

				// 5�s�ǂݔ�΂�
				iLine += 5;
				continue;
			}

			if( bInItems && str.Find(_T("</table>")) != -1 ) {
				// �I���^�O����
				break;
			}
		}

		MZ3LOGGER_DEBUG( L"ListCommentParser.parse() finished." );
		return true;
	}
};

/**
 * [content] Readme.txt �p�p�[�T
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
 * [content] home.pl ���O�C����̃��C����ʗp�p�[�T
 */
class HomeParser : public MixiContentParser
{
public:
	/**
	 * ���O�C������
	 *
	 * ���O�C�������������ǂ����𔻒�
	 *
	 * @return ���O�C����������TRUE�A���s����FALSE��Ԃ�
	 */
	static bool IsLoginSucceeded( const CHtmlArray& html )
	{
		INT_PTR count = html.GetCount();

		for (int i=0; i<count; i++) {
			const CString& line = html.GetAt(i);

			if (util::LineHasStringsNoCase( line, L"refresh", L"home.pl" )) {
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
	// 		NLED_SETTINGS_INFO led;
	//		led.LedNum = ::NLedSetDevice(0, 
		}

		// �V���R�����g���̎擾
		int commentNum = GetNewCommentCount( html_, 100, index, index);
		if (commentNum != 0) {
			theApp.m_newCommentCount = commentNum;
		}

		if (!wcscmp(theApp.m_loginMng.GetOwnerID(), _T(""))) {
			// OwnerID �����擾�Ȃ̂ŉ�͂���
			MZ3LOGGER_DEBUG( L"OwnerID �����擾�Ȃ̂ŉ�͂��܂�" );

			for (int i=index; i<count; i++) {
				const CString& line = html_.GetAt(i);
				if( util::LineHasStringsNoCase( line, L"<a", L"href=", L"list_community.pl" ) ) {
					CString buf;
					
					if( util::GetBetweenSubString( line, L"id=", L"\"", buf ) == -1 ) {
						MZ3LOGGER_ERROR( L"list_community.pl �̈����� id �w�肪����܂���B line[" + line + L"]" );
					}else{
						MZ3LOGGER_DEBUG( L"OwnerID = " + buf );
						theApp.m_loginMng.SetOwnerID(buf);
						theApp.m_loginMng.Write();
					}
					break;
				}
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

/**
 * home.pl ���O�C����ʗp�p�[�T
 */
class LoginPageParser : public MixiParserBase
{
public:
	/**
	 * ���O�A�E�g���������`�F�b�N����
	 */
	static bool isLogout( LPCTSTR szHtmlFilename )
	{
		// �ő�� 30 �s�ڂ܂Ń`�F�b�N����
		const int CHECK_LINE_NUM_MAX = 30;

		FILE* fp = _wfopen(szHtmlFilename, _T("r"));
		if( fp != NULL ) {
			TCHAR buf[4096];

			for( int i=0; i<CHECK_LINE_NUM_MAX && fgetws(buf, 4096, fp) != NULL; i++ ) {
				if( wcsstr( buf, L"regist.pl" ) != NULL ) {
					// ���O�A�E�g���
					fclose( fp );
					return true;
				}
			}
			fclose(fp);
		}

		// �����ɂ̓f�[�^���Ȃ������̂Ń��O�A�E�g�Ƃ͔��f���Ȃ�
		return false;
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
		 * <img SRC="http://ic39.mixi.jp/p/xxx/xxx/diary/xx/x/xxx.jpg" BORDER=0>
		 */
		CString uri;
		for (int i=0; i<count; i++) {
			// �摜�ւ̃����N�𒊏o
			if( util::GetBetweenSubString( html.GetAt(i), _T("<img SRC=\""), _T("\" BORDER=0>"), uri ) > 0 ) {
				break;
			}
		}
		return uri;
	}
};

/// ���X�g�nHTML�̉��
inline void MyDoParseMixiHtml( ACCESS_TYPE aType, CMixiDataList& body, CHtmlArray& html )
{
	switch (aType) {
	case ACCESS_LIST_DIARY:			mixi::ListNewFriendDiaryParser::parse( body, html );	break;
	case ACCESS_LIST_NEW_COMMENT:	mixi::NewCommentParser::parse( body, html );			break;
	case ACCESS_LIST_COMMENT:		mixi::ListCommentParser::parse( body, html );			break;
	case ACCESS_LIST_NEW_BBS:		mixi::NewBbsParser::parse( body, html );				break;
	case ACCESS_LIST_MYDIARY:		mixi::ListDiaryParser::parse( body, html );				break;
	case ACCESS_LIST_FOOTSTEP:		mixi::ShowLogParser::parse( body, html );				break;
	case ACCESS_LIST_MESSAGE_IN:	mixi::ListMessageParser::parse( body, html );			break;
	case ACCESS_LIST_MESSAGE_OUT:	mixi::ListMessageParser::parse( body, html );			break;
	case ACCESS_LIST_NEWS:			mixi::ListNewsCategoryParser::parse( body, html );		break;
	case ACCESS_LIST_FAVORITE:		mixi::ListBookmarkParser::parse( body, html );			break;
	case ACCESS_LIST_FRIEND:		mixi::ListFriendParser::parse( body, html );			break;
	case ACCESS_LIST_COMMUNITY:		mixi::ListCommunityParser::parse( body, html );			break;
	case ACCESS_LIST_INTRO:			mixi::ShowIntroParser::parse( body, html );				break;
	case ACCESS_LIST_BBS:			mixi::ListBbsParser::parse( body, html );				break;
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
	case ACCESS_MYDIARY:	mixi::ViewDiaryParser::parse( mixi, html );		break;
	case ACCESS_MESSAGE:	mixi::ViewMessageParser::parse( mixi, html );	break;
	case ACCESS_NEWS:		mixi::ViewNewsParser::parse( mixi, html );		break;
	case ACCESS_HELP:		mixi::HelpParser::parse( mixi, html );			break;
	}
}

}//namespace mixi

