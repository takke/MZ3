/*
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */
#pragma once

#include "MyRegex.h"
#include "HtmlArray.h"
#include "util_base.h"
#include "util_mixi.h"
#include "util_mz3.h"

/// mixi �pHTML�p�[�T
namespace mixi {

/// �p�[�T�p���[�e�B���e�B�Q
class ParserUtil {
public:
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
		data->SetAuthorID( _wtoi(id) );

		// ���O���o
		CString name;
		if( util::GetBetweenSubString( target, L">", L"<", name ) == -1 ) {
			CString msg;
			msg.Format( L"������ '>', '<' ���܂݂܂��� str[%s]", str );
			MZ3LOGGER_ERROR(msg);
			return false;
		}
		// �f�R�[�h
		mixi::ParserUtil::ReplaceEntityReferenceToCharacter( name );
		data->SetAuthor( name );

		return true;
	}

	/**
	 * URL �ɍŌ�ɃA�N�Z�X�����Ƃ��́A���e�������擾����
	 */
	static int GetLastIndexFromIniFile(LPCTSTR uri, const CMixiData& mixi)
	{
		// URI�𕪉�
		CString strLogId = util::GetLogIdString(mixi);
		std::string idKey = util::my_wcstombs( (LPCTSTR)strLogId );

		// �L�[�擾
		int lastIndex = -1;

		if (theApp.m_readlog.RecordExists(idKey.c_str(), "Log") ) {
			// ���R�[�h���݂�΂�����g��
			lastIndex = atoi( theApp.m_readlog.GetValue(idKey.c_str(), "Log").c_str() );
		}
		else {
			lastIndex = -1;
		}

		return lastIndex;
	}

	/**
	 * URL �ɍŌ�ɃA�N�Z�X�����Ƃ��́A���e�������擾����
	 */
	static int GetLastIndexFromIniFile(const CMixiData& mixi)
	{
		return GetLastIndexFromIniFile( mixi.GetURL(), mixi );
	}

	/**
	 * �����ϊ�(���t�E�������ۂ�������𒊏o���Amixi ��SetDate����)
	 *
	 * @param line [in]  ������������܂ޕ�����B
	 *                   ��F"2006�N11��19�� 17:12"
	 *                       "<span class="date">2007�N07��05�� 21:55</span></dt>"
	 *                       "<td>10��08��</td></tr>"
	 * @param mixi [out] ��͌��ʂ� SetDate �ŕۑ�����B
	 */
	static bool ParseDate(LPCTSTR line, CMixiData& mixi)
	{
		// �ėp �`��
		{
			// ���K�\���̃R���p�C��
			static MyRegex reg;
			if( !util::CompileRegex( reg, L"([0-9]{2,4})?�N?([0-9]{1,2}?)��([0-9]{1,2})��[^0-9]*([0-9]{1,2})?:?��?([0-9]{2})?" ) ) {
				return false;
			}
			// ����
			if( reg.exec(line) && reg.results.size() == 6 ) {
				// ���o
				int year   = _wtoi( reg.results[1].str.c_str() );
				int month  = _wtoi( reg.results[2].str.c_str() );
				int day    = _wtoi( reg.results[3].str.c_str() );
				int hour   = _wtoi( reg.results[4].str.c_str() );
				int minute = _wtoi( reg.results[5].str.c_str() );
				//mixi.SetDate(year, month, day, hour, minute);

				if (year<1900) {
					// CTime �� year<1900 ���T�|�[�g���Ă��Ȃ��̂ŁA������Ƃ��ēo�^����B
					CString s;
					s.Format(_T("%02d/%02d %02d:%02d"), month, day, hour, minute);
					mixi.SetDate( s );
				} else {
					mixi.SetDate( CTime(year, month, day, hour, minute, 0) );
				}
				return true;
			}
		}

		CTime t;
		if (ParseDate(line, t)) {
			// ��͐���
			mixi.SetDate(t);
			return true;
		}

		CString msg = L"��������ɓ��t�E������������܂��� : [";
		msg += line;
		msg += L"]";
		MZ3LOGGER_DEBUG( msg );
		return false;
	}


	static bool ParseDate(LPCTSTR line, CTime& t_result)
	{
		// T1. RSS �`�� (YYYY-MM-DDT00:00:00Z)
		{
			// ���K�\���̃R���p�C��
			static MyRegex reg;
			if( !util::CompileRegex( reg, L"([0-9]{4})-([0-9]{2})-([0-9]{2})T([0-9]{2}):([0-9]{2}):([0-9]{2})Z" ) ) {
				return false;
			}
			// ����
			if( reg.exec(line) && reg.results.size() == 7 ) {
				// ���o
				int year   = _wtoi( reg.results[1].str.c_str() );
				int month  = _wtoi( reg.results[2].str.c_str() );
				int day    = _wtoi( reg.results[3].str.c_str() );
				int hour   = _wtoi( reg.results[4].str.c_str() );
				int minute = _wtoi( reg.results[5].str.c_str() );

				CTime t(year, month, day, hour, minute, 0);
				t += CTimeSpan(0, 9, 0, 0);

				t_result = t;
				//mixi.SetDate(t.GetYear(), t.GetMonth(), t.GetDay(), t.GetHour(), t.GetMinute());
				return true;
			}
		}

		// T2. RSS �`�� (YYYY-MM-DDT00:00:00+09:00)
		{
			// ���K�\���̃R���p�C��
			static MyRegex reg;
			if( !util::CompileRegex( reg, L"([0-9]{4})-([0-9]{2})-([0-9]{2})T([0-9]{2}):([0-9]{2}):([0-9]{2})\\+([0-9]{2}):([0-9]{2})" ) ) {
				return false;
			}
			// ����
			if( reg.exec(line) && reg.results.size() == 9 ) {
				// ���o
				int year   = _wtoi( reg.results[1].str.c_str() );
				int month  = _wtoi( reg.results[2].str.c_str() );
				int day    = _wtoi( reg.results[3].str.c_str() );
				int hour   = _wtoi( reg.results[4].str.c_str() );
				int minute = _wtoi( reg.results[5].str.c_str() );

				CTime t(year, month, day, hour, minute, 0);
//				t += CTimeSpan(0, 9, 0, 0);

				t_result = t;
				return true;
			}
		}

		// T3. RSS �`�� (Sun Dec 16 09:00:00 +0000 2007)
		{
			// ���K�\���̃R���p�C��
			static MyRegex reg;
			if( !util::CompileRegex( reg, L"([a-zA-Z]{3}) ([a-zA-Z]{3}) ([0-9]{2}) ([0-9]{2}):([0-9]{2}):([0-9]{2}) \\+([0-9]{4}) ([0-9]{4})" ) ) {
				return false;
			}
			// ����
			if( reg.exec(line) && reg.results.size() == 9 ) {
				// ���o

				// �N
				int year   = _wtoi( reg.results[8].str.c_str() );

				// ��
				int month  = ThreeCharMonthToInteger( reg.results[2].str.c_str() );
				int day    = _wtoi( reg.results[3].str.c_str() );
				int hour   = _wtoi( reg.results[4].str.c_str() );
				int minute = _wtoi( reg.results[5].str.c_str() );
				int sec    = _wtoi( reg.results[6].str.c_str() );

				CTime t(year, month, day, hour, minute, sec);
				t += CTimeSpan(0, 9, 0, 0);
				t_result = t;
				return true;
			}
		}

		// T4. RSS 2.0 �`�� (Mon, 16 Dec 2007 09:00:00 +0900)
		// T4. RSS 2.0 �`�� (Mon, 16 Dec 2007 09:00:00 GMT)
		{
			// ���K�\���̃R���p�C��
			static MyRegex reg;
			if( !util::CompileRegex( reg, L"([a-zA-Z]{3}), ([0-9]{2}) ([a-zA-Z]{3}) ([0-9]{4}) ([0-9]{2}):([0-9]{2}):([0-9]{2}) (.*)" ) ) {
				return false;
			}
			// ����
			if( reg.exec(line) && reg.results.size() == 9 ) {
				// ���o

				// �N
				int year   = _wtoi( reg.results[4].str.c_str() );

				// ��
				int month  = ThreeCharMonthToInteger( reg.results[3].str.c_str() );
				int day    = _wtoi( reg.results[2].str.c_str() );
				int hour   = _wtoi( reg.results[5].str.c_str() );
				int minute = _wtoi( reg.results[6].str.c_str() );
				int sec    = _wtoi( reg.results[7].str.c_str() );
				CTime t(year, month, day, hour, minute, sec);

				// �ȈՎ����ϊ�
				const std::wstring& time_diff = reg.results[8].str;

				if (time_diff==L"GMT") {
					t += CTimeSpan(0, 9, 0, 0);
				}

				t_result = t;
				return true;
			}
		}
		return false;
	}

	/**
	 * 3�����́u���v������𐔒l�ɕϊ�����B
	 *
	 * "Jan" �̏ꍇ�͐��l 1 ��Ԃ��B
	 * ���������� 1 ��Ԃ��B
	 */
	static int ThreeCharMonthToInteger( const CString& strMonth )
	{
		int month = 1;
		if (strMonth == L"Jan")			month = 1;
		else if (strMonth == L"Feb")	month = 2;
		else if (strMonth == L"Mar")	month = 3;
		else if (strMonth == L"Apr")	month = 4;
		else if (strMonth == L"May")	month = 5;
		else if (strMonth == L"Jun")	month = 6;
		else if (strMonth == L"Jul")	month = 7;
		else if (strMonth == L"Aug")	month = 8;
		else if (strMonth == L"Sep")	month = 9;
		else if (strMonth == L"Oct")	month = 10;
		else if (strMonth == L"Nov")	month = 11;
		else if (strMonth == L"Dec")	month = 12;
		else {
			// ??
		}
		return month;
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
		// ���s�R�[�h�̏���
		while( line.Replace(_T("\n"), _T("")) ) ;

		// table �^�O�̏���
		{
			static MyRegex reg;
			if( !util::CompileRegex( reg, L"<table[^>]*>" ) ) {
				return;
			}
			if( line.Find( L"<table" ) != -1 ) 
				reg.replaceAll( line, L"" );
		}
		while( line.Replace(_T("</table>"), _T("")) );

		// tr �^�O�̏���
		{
			static MyRegex reg;
			if( !util::CompileRegex( reg, L"<tr[^>]*>" ) ) {
				return;
			}
			if( line.Find( L"<tr" ) != -1 ) 
				reg.replaceAll( line, L"" );
		}
		while( line.Replace(_T("</tr>"), _T("")) );

		// td �^�O�̏���
		{
			static MyRegex reg;
			if( !util::CompileRegex( reg, L"<td[^>]*>" ) ) {
				return;
			}
			if( line.Find( L"<td" ) != -1 ) 
				reg.replaceAll( line, L"" );
		}
		while( line.Replace(_T("</td>"), _T("")) );

		// div �^�O�̏���
		{
			static MyRegex reg;
			if( !util::CompileRegex( reg, L"<div[^>]*>" ) ) {
				return;
			}
			if( line.Find( L"<div" ) != -1 ) 
				reg.replaceAll( line, L"" );
		}
		while( line.Replace(_T("</div>"), _T("")) );

		// span �^�O�̏���
		{
			static MyRegex reg;
			if( !util::CompileRegex( reg, L"<span[^>]*>" ) ) {
				return;
			}
			if( line.Find( L"<span" ) != -1 ) 
				reg.replaceAll( line, L"" );
		}
		while( line.Replace(_T("</span>"), _T("")) );

		// br �^�O�̒u��
		{
			static MyRegex reg;
			if( !util::CompileRegex( reg, L"<br[^>]*?>" ) ) {
				return;
			}
			if( line.Find( L"<br" ) != -1 ) 
				reg.replaceAll( line, L"\r\n" );
		}

		// �R�����g�̒u��
		{
			static MyRegex reg;
			if( !util::CompileRegex( reg, L"<!--.*?-->" ) ) {
				return;
			}
			if( line.Find( L"<!--" ) != -1 ) 
				reg.replaceAll( line, L"" );
		}

		// "<p>" �� �폜
		while( line.Replace(_T("<p>"), _T("")) ) ;

		// </p> �� ���s
		while( line.Replace(_T("</p>"), _T("\r\n\r\n")) ) ;

		// &quot; �Ȃǂ̒�`�ςݎ��̎Q�Ƃ̕�����
		ReplaceDefinedEntityReferenceToCharacter( line );

		// &#xxxx; �̎��̎Q�Ƃ̕�����
		// ��j&#3642; �� char(3642)
		ReplaceNumberEntityReferenceToCharacter( line );
	}

	static void ReplaceEntityReferenceToCharacter( CString& str )
	{
		ReplaceDefinedEntityReferenceToCharacter( str );
		ReplaceNumberEntityReferenceToCharacter( str );
	}

	/**
	 * &quot; �Ȃǂ̒�`�ςݎ��̎Q�Ƃ̕�����
	 */
	static void ReplaceDefinedEntityReferenceToCharacter( CString& str )
	{
		while( str.Replace(_T("&quot;"), _T("\"")) ) ;
		while( str.Replace(_T("&gt;"),   _T(">")) ) ;
		while( str.Replace(_T("&lt;"),   _T("<")) ) ;
		while( str.Replace(_T("&nbsp;"), _T(" ")) ) ;
		while( str.Replace(_T("&amp;"),  _T("&")) ) ;
	}

	/**
	 * &#xxxx; �̎��̎Q�Ƃ̕�����
	 * ��j&#3642; �� char(3642)
	 */
	static void ReplaceNumberEntityReferenceToCharacter( CString& str )
	{
		// ���K�\���̃R���p�C���i���̂݁j
		static MyRegex reg;
		if( !util::CompileRegex( reg, L"&#([0-9]{3,5});" ) ) {
			return;
		}

		CString target = str;
		str = L"";
		for( int i=0; i<MZ3_INFINITE_LOOP_MAX_COUNT; i++ ) {	// MZ3_INFINITE_LOOP_MAX_COUNT �͖������[�v�h�~
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
			ReplaceEmojiImageToCode( str );
		}

		// ���̑��̉摜�����N�̕ϊ�
		//if( util::LineHasStringsNoCase( str, L"<img", L"src=" ) ) {
		//	ExtractGeneralImageLink( str, *data );
		//}

		// ����t�@�C���̒��o
		if (str.Find( L".flv" ) != -1) {
			ExtractGeneralVideoLink( str, *data );
		}

		// Youtube ����̒��o(���L��1�s�ő��݂���)
		//youtube_write('<div class="insert_video"><object width="450" height="373"><param name="movie" value="
		//http://www.youtube.com/v/xxx"></param><param name="wmode" value="transparent"></param>
		//<embed src="http://www.youtube.com/v/xxx" type="application/x-shockwave-flash" wmode="transparent" 
		//width="450" height="373"></embed></object></div>');
#define LINE_HAS_YOUTUBE_LINK(line)		util::LineHasStringsNoCase( line, L"<embed", L"src=", L"youtube.com" )
		// GoogleMap�̒��o(���L�������s�ő��݂���)
		//map.push({
		//  map_id : "map_1",
		//	size   : { width : 480, height : 480 },
		//  url    : { embed : "http://maps.google.co.jp/maps?spn=xxxx,xxxx&ie=UTF8&ll=xx,xx&output=embed&z=15&s=xx", link : "http://maps.google.co.jp/maps?source=embed&spn=xxx,xxx&ie=UTF8&ll=xxx,xxx&z=15" }
		//});
#define LINE_HAS_GOOGLEMAP_LINK(line)	util::LineHasStringsNoCase( line, L"url", L"embed", L"maps.google.co.jp" )
		// �j�R�j�R���惊���N�̒��o(���L��1�s�ő��݂���)
		//<script type="text/javascript" src="http://ext.nicovideo.jp/thumb_watch/smxxxxxxxx?w=450&h=357&n=1" charset="utf-8"></script>
#define LINE_HAS_NICOVIDEO_LINK(line)	util::LineHasStringsNoCase( line, L"<script", L"src=", L"ext.nicovideo.jp" )

		// �����N�̒��o
		if ( ( str.Find( L"href" ) != -1 ) ||
			 (str.Find( L"ttp://" ) != -1) ||
			 LINE_HAS_YOUTUBE_LINK(str) ||
			 LINE_HAS_GOOGLEMAP_LINK(str) ||
			 LINE_HAS_NICOVIDEO_LINK(str) ||
			 ( util::LineHasStringsNoCase( str, L"<img", L"src=" ) ) ) {
			ExtractURI( str, *data );
		}

		// 2ch �`���̃����N���o
		//if( str.Find( L"ttp://" ) != -1 ) {
		//	//Extract2chURL( str, *data );
		//}
	}

	/**
	 * ���惊���N�̒��o
	 *
	 * <script> �^�O�𒊏o���邽�߁A�����s�̃p�[�X���s���B
	 * iLine_ �p�����[�^���Q�Ƃł���_�ɒ��ӁI
	 */
	static bool ExtractVideoLinkFromScriptTag( CMixiData& mixi_, int& iLine_, const CHtmlArray& html_ )
	{
		const int lastLine = html_.GetCount();
		if( iLine_>=lastLine ) {
			return false;
		}

		// ����pScript�^�O������������X�N���v�g�p���[�v�J�n
		const CString& line = html_.GetAt( iLine_ );
		if( util::LineHasStringsNoCase( line, L"<script") ) {
			while( iLine_<lastLine ) {
				// ���̍s���t�F�b�`
				const CString& nextLine = html_.GetAt( ++iLine_ );
				// �g���q.flv�����������瓊��
				if( util::LineHasStringsNoCase( nextLine, L".flv" ) ) {
					ParserUtil::AddBodyWithExtract( mixi_, nextLine );
				}
				// Youtube�Ή�
				if( LINE_HAS_YOUTUBE_LINK(nextLine) ) {
					ParserUtil::AddBodyWithExtract( mixi_, nextLine );
				}
				// </script> ������ΏI��
				if( util::LineHasStringsNoCase( nextLine, L"</script>" ) ) {
					break;
				}
			}
			return true;
		} else {
			return false;
		}
	}

private:

	/**
	 * �G�����摜�����N�̕ϊ�
	 *
	 * [m:xxx] ������ɒu������B
	 */
	static void ReplaceEmojiImageToCode( CString& line )
	{
		// <img src="http://img.mixi.jp/img/emoji/85.gif" alt="�i��" width="16" height="16" class="emoji" border="0">
		// �̂悤�ȃ����N��
		// "((�i��))" �ɕϊ�����

		// ���K�\���̃R���p�C���i���̂݁j
		static MyRegex reg;
		if (!util::CompileRegex( reg, L"<img src=\"http://img.mixi.jp/img/emoji/([^\"]+).gif\" alt=\"([^\"]+)\" [^c]+ class=\"emoji\"[^>]*>" )) {
			return;
		}

		// ((�i��)) �ɕϊ�����
//		reg.replaceAll( line, L"(({2}))" );

		// [m:xx] �`���ɒu������
		reg.replaceAll( line, L"[m:{1}]" );
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
		for( int i=0; i<MZ3_INFINITE_LOOP_MAX_COUNT; i++ ) {	// MZ3_INFINITE_LOOP_MAX_COUNT �͖������[�v�h�~

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
			ret.AppendFormat( L"<_img><<�摜%02d(album)>></_img>", data_.GetImageCount() );
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
	 * ���L�A�g�s�b�N���̉摜�ϊ�
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

		for( int i=0; i<MZ3_INFINITE_LOOP_MAX_COUNT; i++ ) {	// MZ3_INFINITE_LOOP_MAX_COUNT �͖������[�v�h�~
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
				line.AppendFormat( L"<_img><<�摜%02d>></_img><br>", data_.GetImageCount() );

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
	 * ����ϊ�
	 *
	 * line ���瓮�惊���N�𒊏o���A���̃����N�� data �ɒǉ�����B
	 * �܂��Aline ����Y�����铮�惊���N���폜����B
	 */
	static void ExtractGeneralVideoLink(CString& line, CMixiData& data_)
	{

		// ���K�\���̃R���p�C���i���̂݁j
		static MyRegex reg;
		if( !util::CompileRegex( reg, L".*video *: '([^']+)',.*" ) ) {
			MZ3LOGGER_FATAL( FAILED_TO_COMPILE_REGEX_MSG );
			return;
		}

		static MyRegex reg2;
		if( !util::CompileRegex( reg2, L"player\\.push\\(\\{([^\\)]+)\\}\\);" ) ) {
			MZ3LOGGER_FATAL( FAILED_TO_COMPILE_REGEX_MSG );
			return;
		}

		static MyRegex reg3;
		if( !util::CompileRegex( reg3, L"Event\\.observe\\(.*\\)" ) ) {
			MZ3LOGGER_FATAL( FAILED_TO_COMPILE_REGEX_MSG );
			return;
		}

		CString target = line;
		line = L"";
		for( int i=0; i<MZ3_INFINITE_LOOP_MAX_COUNT; i++ ) {	// MZ3_INFINITE_LOOP_MAX_COUNT �͖������[�v�h�~
			// player.push({�]�X});��T��
			if( !reg2.exec(target) || reg2.results.size() != 2 ) {
				// �������B
				// ������̒��œ��惊���N��������
				if( reg.exec( target ) && reg.results.size() == 2 ) {
					// ����
					std::vector<MyRegex::Result>& results = reg.results;

					// �}�b�`������S�̂̍������o��
					line.Append( target, results[0].start );

					// ���惊���N�̕\��
					CString text = L"<<����>>";

					// url ��ǉ�
					LPCTSTR url = results[1].str.c_str();
					
					//�����ǉ�
					data_.AddMovie( url );

					// �u����
					line.Append( L"<_mov>" + text + L"</_mov>" );

					// �Ƃ肠�������s
					line += _T("<br>");

					// �^�[�Q�b�g���X�V�B
					target.Delete( 0, results[0].end );
				} else {
					// ������
					line += target;
					break;
				}

			} else {
				// �����B
				std::vector<MyRegex::Result>& results2 = reg2.results;

				// �}�b�`������S�̂̍������o��
				line.Append( target, results2[0].start );

				// �}�b�`������̒��œ��惊���N��������
				if( reg.exec( results2[1].str.c_str() ) && reg.results.size() == 2 ) {
					// ����
					std::vector<MyRegex::Result>& results = reg.results;

					// ���惊���N�̕\��
					CString text = L"<<����>>";

					// url ��ǉ�
					LPCTSTR url = results[1].str.c_str();
					
					//�����ǉ�
					data_.AddMovie( url );

					// �u����
					line.Append( L"<_mov>" + text + L"</_mov>" );

					// �Ƃ肠�������s
					line += _T("<br>");
				} else {
					// ������
					line += results2[1].str.c_str();
				}

				// �^�[�Q�b�g���X�V�B
				target.Delete( 0, results2[0].end );
			}

			// Event.observe(�]�X) ������Ώ���
			if( reg3.exec(target) && reg3.results.size() != 0 ) {
				// �����B
				std::vector<MyRegex::Result>& results3 = reg3.results;
				// �^�[�Q�b�g���X�V�B
				target.Delete( results3[0].start , results3[0].end );
			}


		}
	}

public:
	/**
	 * �S�Ẵ����N������i<a...>XXX</a>�j���A������̂݁iXXX�j�ɕύX����
	 * �܂��Ahref="url" �� list_ �ɒǉ�����B
	 * ����ɁA2ch �`����URL(ttp://...)�����o���A���K������ data �̃����N���X�g�ɒǉ�����B
	 * YouTube���惊���N�����o����
	 * GoogleMap�����N�����o����
	 * <img>�^�O����̉摜�����N�����o����
	 */
	static void ExtractURI( CString& str, std::vector<CMixiData::Link>& list_ )
	{
		TRACE( L"ExtractURI:str[%s]\n", (LPCTSTR)str );

		// <wbr/> �^�O�̏���
		while( str.Replace(_T("<wbr/>"), _T("")) );
		while( str.Replace(_T("<wbr />"), _T("")) );

		// ���K�\���̃R���p�C���i���̂݁j
		static MyRegex reg;
		if( !util::CompileRegex( reg, L"<a .*?href *?='([^']+)'[^>]*>(.*?)</a>" ) ) {
			MZ3LOGGER_FATAL( FAILED_TO_COMPILE_REGEX_MSG );
			return;
		}
		static MyRegex reg2;
		if( !util::CompileRegex( reg2, L"<a .*?href *?=\"([^\"]+)\"[^>]*>(.*?)</a>" ) ) {
			MZ3LOGGER_FATAL( FAILED_TO_COMPILE_REGEX_MSG );
			return;
		}
		static MyRegex reg3;
		if( !util::CompileRegex( reg3, L"<a .*?href *?=([^>]+)>(.*?)</a>" ) ) {
			MZ3LOGGER_FATAL( FAILED_TO_COMPILE_REGEX_MSG );
			return;
		}
		// 2ch�`�������N���o�p
		static MyRegex reg4;
		if( !util::CompileRegex( reg4, L"([^h]|^)(ttps?://[-_.!~*'()a-zA-Z0-9;/?:@&=+$,%#]+)" ) ) {
			MZ3LOGGER_FATAL( FAILED_TO_COMPILE_REGEX_MSG );
			return;
		}
		// YouTube���惊���N���o�p
		static MyRegex reg5;
		if( !util::CompileRegex( reg5, L"youtube_write.*src=\"(.*?)\".*?;" ) ) {
			MZ3LOGGER_FATAL( FAILED_TO_COMPILE_REGEX_MSG );
			return;
		}
		// GoogleMap�����N���o�p
		static MyRegex reg6;
		if( !util::CompileRegex( reg6, L".*url *: \\{ embed : \"([^\"]+)\", *link *: \"([^\"]+)\" *\\}" ) ) {
			MZ3LOGGER_FATAL( FAILED_TO_COMPILE_REGEX_MSG );
			return;
		}
		static MyRegex reg7;
		if( !util::CompileRegex( reg7, L"map\\.push\\(\\{([^\\)]+)\\}\\);" ) ) {
			MZ3LOGGER_FATAL( FAILED_TO_COMPILE_REGEX_MSG );
			return;
		}
		static MyRegex reg8;
		if( !util::CompileRegex( reg8, L"Event\\.observe\\(.*\\)" ) ) {
			MZ3LOGGER_FATAL( FAILED_TO_COMPILE_REGEX_MSG );
			return;
		}
		// �j�R�j�R���惊���N���o�p
		static MyRegex reg9;
		if( !util::CompileRegex( reg9, L"<script *type=\"text/javascript\" *src=\"http://ext\\.nicovideo\\.jp/thumb_watch/([a-z0-9]*)\\?.*?\".*?>" ) ) {
			MZ3LOGGER_FATAL( FAILED_TO_COMPILE_REGEX_MSG );
			return;
		}

		// <img>�^�O���o�p
		static MyRegex reg10;
		if( !util::CompileRegex( reg10, L"<img[^>]*src=\"([^\"]+)\"[^>]*>" ) ) {
			return;
		}

		// <span>�^�O���o�p
		static MyRegex regs;
		if( !util::CompileRegex( regs, L"<span[^>]*>" ) ) {
			MZ3LOGGER_FATAL( FAILED_TO_COMPILE_REGEX_MSG );
			return;
		}

		CString target = str;
		str = L"";
		for( int i=0; i<MZ3_INFINITE_LOOP_MAX_COUNT; i++ ) {	// MZ3_INFINITE_LOOP_MAX_COUNT �͖������[�v�h�~
			int offset = -1;
			u_int offsetend = 0;
			std::wstring url = L"";
			std::wstring text = L"";
			std::wstring url2 = L"";
			std::wstring text2 = L"";
			bool bCrLf = false;
			bool bEventObserve = false;

			CString regtarget = target;
			// <a href=xx>
			if( regtarget.Find( L"href" ) != -1 ) {
				if( reg.exec(regtarget) && reg.results.size() == 3 ) {
					offset = reg.results[0].start ;
					offsetend = reg.results[0].end;
					url = reg.results[1].str;
					text = reg.results[2].str;
					regtarget = regtarget.Left(offset);				// ���̃T�[�`�͈͂����肷��
				}
				if( reg2.exec(regtarget) && reg2.results.size() == 3 ) {
					//if( offset < 0 || ( reg2.results[0].start < offset ) ){
						offset = reg2.results[0].start ;
						offsetend = reg2.results[0].end;
						url = reg2.results[1].str;
						text = reg2.results[2].str;
						regtarget = regtarget.Left(offset);			// ���̃T�[�`�͈͂����肷��
					//}
				}
				if( reg3.exec(regtarget) && reg3.results.size() == 3 ) {
					//if( offset < 0 || ( reg3.results[0].start < offset ) ){
						offset = reg3.results[0].start ;
						offsetend = reg3.results[0].end;
						url = reg3.results[1].str;
						text = reg3.results[2].str;
						regtarget = regtarget.Left(offset);			// ���̃T�[�`�͈͂����肷��
					//}
				}
			}
			// 2ch URL
			if( regtarget.Find( L"ttp://" ) != -1 ) {
				if( reg4.exec(regtarget) && reg4.results.size() == 3 ) {
					// 2ch URL
					//if( offset < 0 || ( reg4.results[2].start < offset ) ){
						offset = reg4.results[2].start;
						offsetend = reg4.results[0].end;
						// 2ch URL �𐳋K��
						url = L"h";
						url += reg4.results[2].str;
						text = reg4.results[2].str;
						regtarget = regtarget.Left(offset);			// ���̃T�[�`�͈͂����肷��
					//}
				}
			}
			// YouTube���惊���N
			if( LINE_HAS_YOUTUBE_LINK(regtarget) ) {
				if( reg5.exec(regtarget) && reg5.results.size() == 2 ) {
					//if( offset < 0 || ( reg5.results[0].start < offset ) ){
						// YouTube���惊���N
						offset = reg5.results[0].start ;
						offsetend = reg5.results[0].end;
						url = reg5.results[1].str;
						text = L"<<Youtube����>>";
						bCrLf = true;					// YouTube�����N�̏ꍇ�͉��s����
						regtarget = regtarget.Left(offset);			// ���̃T�[�`�͈͂����肷��
					//}
				}
			}
			// GoogleMap�����N
			if( LINE_HAS_GOOGLEMAP_LINK(regtarget) ) {
				if( reg7.exec(regtarget) && reg7.results.size() == 2 ) {
					// �����B
					//if( offset < 0 || ( reg7.results[0].start < offset ) ){
						// �}�b�`������̒���GoogleMap�����N��������
						if( reg6.exec( reg7.results[1].str.c_str() ) && reg6.results.size() == 3 ) {
							// ����
							// GoogleMap�����N
							offset = reg7.results[0].start ;
							offsetend = reg7.results[0].end ;
#ifndef WINCE
							// Win32��link:����URL���擾
							url = reg6.results[2].str;
#else
							// WM��embed:����URL���擾
							url = reg6.results[1].str;
#endif
							text = L"<<Google�}�b�v>>";
							bCrLf = true;					// GoogleMap�����N�̏ꍇ�͉��s����
							bEventObserve = true;			// Event.Observe������Ώ����w��
							regtarget = regtarget.Left(offset);			// ���̃T�[�`�͈͂����肷��
						}
					//}
				}
			}
			// �j�R�j�R���惊���N
			if( LINE_HAS_NICOVIDEO_LINK(regtarget) ) {
				if( reg9.exec(regtarget) && reg9.results.size() == 2 ) {
					//if( offset < 0 || ( reg9.results[0].start < offset ) ){
						// �j�R�j�R���惊���N
						offset = reg9.results[0].start ;
						offsetend = reg9.results[0].end;
						url = L"http://www.nicovideo.jp/watch/" + reg9.results[1].str;
						text = L"<<�j�R�j�R����>>";
						bCrLf = true;					// �j�R�j�R���惊���N�̏ꍇ�͉��s����
						regtarget = regtarget.Left(offset);			// ���̃T�[�`�͈͂����肷��
					//}
				}
			}
			// <img>�摜�����N
			if( regtarget.Find( L"<img" ) != -1 ) {
				if( reg10.exec(regtarget) && reg10.results.size() == 2 ) {
					//if( offset < 0 || ( reg10.results[0].start < offset ) ){
						// class="emoji" ���܂܂�Ă���΁A�G�����Ɣ��f���A��������B
						if( util::LineHasStringsNoCase( reg10.results[0].str.c_str(), L"class=\"emoji\"" ) ) {
						} else {
							// <img>�摜�����N
							offset = reg10.results[0].start ;
							offsetend = reg10.results[0].end;
							url = reg10.results[1].str;
							CString csWork;
							csWork.Format( L"<<link%02d(�摜)>>" , list_.size() + 1 );
							text = csWork;
							bCrLf = true;					// <img>�摜�����N�̏ꍇ�͉��s����
							regtarget = regtarget.Left(offset);			// ���̃T�[�`�͈͂����肷��
						}
					//}
				}
			}
			// �����N�e�L�X�g�̒���<img>������Β��o����
			{
				regtarget = text.c_str();
				if( regtarget.Find( L"<img" ) != -1 ) {
					if( reg10.exec(regtarget) && reg10.results.size() == 2 ) {
						// class="emoji" ���܂܂�Ă���΁A�G�����Ɣ��f���A��������B
						if( util::LineHasStringsNoCase( reg10.results[0].str.c_str(), L"class=\"emoji\"" ) ) {
						} else {
							// <img>�摜�����N
							CString csWork;
							csWork.Format( L"<<link%02d>>" , list_.size() + 1 );
							text = csWork;
							csWork.Format( L"<<link%02d(link%02d�̉摜)>>" , list_.size() + 2 , list_.size() + 1 );
							url2 = reg10.results[1].str;
							text2 = csWork;
							bCrLf = true;					// <img>�摜�����N�̏ꍇ�͉��s����
						}
					}
				}
			}
			if( offset < 0 ) {
				// �������B
				// �c��̕�����������ďI���B
				str += target;
				break;
			}
			// �����B
			// �}�b�`������S�̂̍������o��
			str += target.Left( offset );

			// URL
			TRACE( L"regex-match-URL  : %s\n", url.c_str() );

			// ������
			TRACE( L"regex-match-TEXT : %s\n", text.c_str() );
			str += L"<_a>";
			str += text.c_str();
			str += L"</_a>";

			// �f�[�^�ɒǉ�
			// span,strong,br �^�O�̏���
			CString csText = text.c_str();
			if( csText.Find( L"<span" ) != -1 ) 
				regs.replaceAll( csText, L"" );
			while( csText.Replace(_T("</span>"), _T("")) );
			while( csText.Replace(_T("<strong>"), _T("")) );
			while( csText.Replace(_T("</strong>"), _T("")) );
			while( csText.Replace(_T("<br>"), _T("")) );
			while( csText.Replace(_T("<br />"), _T("")) );

			list_.push_back( CMixiData::Link(url.c_str(),csText) );

			// �����N���ɉ摜�����N������΃f�[�^�ɒǉ�
			if( url2 != L"" ){
				// ������
				str += L":<_a>";
				str += text2.c_str();
				str += L"</_a>";

				// �f�[�^�ɒǉ�
				list_.push_back( CMixiData::Link(url2.c_str(),text2.c_str() ) );
			}

			// ���s
			if( bCrLf ){
				str += L"<br>";
			}

			// �^�[�Q�b�g���X�V�B
			target = target.Mid( offsetend );

			if( bEventObserve ) {
				// Event.observe(�]�X) ������Ώ���
				if( reg8.exec(target) && reg8.results.size() != 0 ) {
					// �����B
					// �^�[�Q�b�g���X�V�B
					target.Delete( reg8.results[0].start , reg8.results[0].end );
				}
			}
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

};

}