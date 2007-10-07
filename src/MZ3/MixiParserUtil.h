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

		if (theApp.m_optionMng.m_bRenderByIE) {
			return;
		}

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

		// ����t�@�C���̒��o
		if(str.Find( L".flv" ) != -1) {
			ExtractGeneralVideoLink( str, *data );
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
		if (theApp.m_optionMng.m_bRenderByIE) {
			return;
		}

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
			if( util::GetBeforeSubString( buf, tag_AlbumBegin, left ) == -1 ) {
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
			MZ3LOGGER_DEBUG( L"�A���o���摜URL [" + url_image + L"]" );

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
/*
<td><a href="javascript:void(0);" onClick="MM_openBrWindow('show_diary_picture.pl?
owner_id=zzz&id=zzz&number=zz','pict','width=680,height=660,toolbar=no,scrollbars=
yes,left=5,top=5');"><img src="http://ic77.mixi.jp/p/zzz/zzz/diary/zz/zz/zzs.jpg"
alt="" /></a></td>
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

			// class="emoji" ���܂܂�Ă���΁A�G�����Ɣ��f���A��������B
			if( util::LineHasStringsNoCase( results[0].str.c_str(), L"class=\"emoji\"" ) ) {
				line.Append( results[0].str.c_str() );
			} else {
				CString text = L"<<�摜>>";
				// url ��ǉ�
				LPCTSTR url = results[1].str.c_str();
				data_.m_linkList.push_back( CMixiData::Link(url, text) );

				// �u��
				line.Append( text );

				// �Ƃ肠�������s
				line += _T("<br>");
			}

			// �^�[�Q�b�g���X�V�B
			target.Delete( 0, results[0].end );
		}

	}

	/**
	 * ����ϊ��B
	 *
	 * str ���瓮�惊���N�𒊏o���A���̃����N�� data �ɒǉ�����B
	 * �܂��Astr ����Y�����铮�惊���N���폜����B
	 */
	static void ExtractGeneralVideoLink(CString& line, CMixiData& data_)
	{

		// ���K�\���̃R���p�C���i���̂݁j
		static MyRegex reg;
		if( !reg.isCompiled() ) {
			LPCTSTR szPattern = L".*video *: '([^']+)',.*";
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

			CString text = L"<<����>>";

			// url ��ǉ�
			LPCTSTR url = results[1].str.c_str();
			
			//�����ǉ�
			data_.AddMovie( url );

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

}