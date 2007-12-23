/*
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */
#pragma once
/**
 * MZ3�ˑ����[�e�B���e�B�֐��Q
 * 
 * ��� theApp �Ɉˑ�������́B
 */

#include "MyRegex.h"
#include "util_base.h"
#include "util_mixi.h"

/// MZ3 �p���[�e�B���e�B
namespace util
{

/**
 * ��ʉ����̏��̈�Ƀ��b�Z�[�W��ݒ肷��
 */
inline void MySetInformationText( HWND hWnd, LPCTSTR szMessage )
{
	CString text;
	if( theApp.m_inet.IsConnecting() ) {
		// �ʐM���Ȃ̂ŁA�A�N�Z�X��ʂ𓪂ɕt����
		text = AccessType2Message(theApp.m_accessType);

		// ���_�C���N�g�񐔂��P��ȏ�Ȃ�A������\�����Ă���
		int nRedirect = theApp.m_inet.GetRedirectCount();
		if( nRedirect > 0 ) {
			CString s;
			s.Format( L"[%d]", nRedirect );
			text += s;
		}

		text += L"�F";
		text += szMessage;
	}else{
		text = szMessage;
	}
	::SendMessage( hWnd, WM_MZ3_ACCESS_INFORMATION, NULL, (LPARAM)&text );
}

inline CString ExtractFilenameFromUrl( const CString& url, const CString& strDefault )
{
	int idx = url.ReverseFind( '/' );
	if (idx >= 0) {
		return url.Mid( idx+1 );
	}
	return strDefault;
}

inline CString MakeImageLogfilePathFromUrl( const CString& url )
{
	CString filename = ExtractFilenameFromUrl( url, L"" );
	if (!filename.IsEmpty()) {
		return theApp.m_filepath.imageFolder + L"\\" + filename;
	}
	return L"";
}

/**
 * CMixiData �ɑΉ�����摜�t�@�C���̃p�X�𐶐�����
 */
inline CString MakeImageLogfilePath( const CMixiData& data )
{
	// �A�N�Z�X��ʂɉ����ăp�X�𐶐�����
	switch( data.GetAccessType() ) {
	case ACCESS_PROFILE:
	case ACCESS_COMMUNITY:
	case ACCESS_TWITTER_USER:
		if (data.GetImageCount()>0) {
			CString path = MakeImageLogfilePathFromUrl( data.GetImage(0) );
			if (!path.IsEmpty()) {
				return path;
			}
		}
		break;
	default:
		break;
	}
	return L"";
}

/**
 * mixi �p�A�N�Z�X��ʂ��ǂ�����Ԃ�
 */
inline bool IsMixiAccessType( ACCESS_TYPE type )
{
	switch (type) {
	case ACCESS_TWITTER_FRIENDS_TIMELINE:
		return false;
	default:
		return true;
	}
}

/**
 * CMixiData �ɑΉ����郍�O�t�@�C���̃p�X�𐶐�����
 */
inline CString MakeLogfilePath( const CMixiData& data )
{
	// �A�N�Z�X��ʂɉ����ăp�X�𐶐�����
	switch( data.GetAccessType() ) {

	// ���O�C���y�[�W
	case ACCESS_LOGIN:				return theApp.m_filepath.logFolder + L"\\login.html";

	// �g�b�v�y�[�W
	case ACCESS_MAIN:				return theApp.m_filepath.logFolder + L"\\home.html";
	
	// ���X�g�n
	case ACCESS_LIST_DIARY:			return theApp.m_filepath.logFolder + L"\\new_friend_diary.html";
	case ACCESS_LIST_NEW_COMMENT:	return theApp.m_filepath.logFolder + L"\\new_comment.html";
	case ACCESS_LIST_COMMENT:		return theApp.m_filepath.logFolder + L"\\list_comment.html";
	case ACCESS_LIST_NEW_BBS:
		{
			// �y�[�W�ԍ����ɕ���
			CString page = L"1";
			util::GetAfterSubString( data.GetURL(), L"page=", page );

			CString path;
			path.Format( L"%s\\new_bbs_%s.html", theApp.m_filepath.logFolder, page );
			return path;
		}

	case ACCESS_LIST_MESSAGE_IN:	return theApp.m_filepath.logFolder + L"\\list_message_in.html";
	case ACCESS_LIST_MESSAGE_OUT:	return theApp.m_filepath.logFolder + L"\\list_message_out.html";
	case ACCESS_LIST_FOOTSTEP:		return theApp.m_filepath.logFolder + L"\\tracks.xml";
	case ACCESS_LIST_MYDIARY:		return theApp.m_filepath.logFolder + L"\\list_diary.html";
	case ACCESS_LIST_CALENDAR:		return theApp.m_filepath.logFolder + L"\\show_calendar.html";  //icchu�ǉ�
	case ACCESS_LIST_NEWS:
		{
			// �J�e�S���A�y�[�W�ԍ����ɕ���
			// url : http://news.mixi.jp/list_news_category.pl?id=pickup&type=bn
			// url : http://news.mixi.jp/list_news_category.pl?id=1&type=bn&sort=1
			// url : http://news.mixi.jp/list_news_category.pl?page=2&id=pickup&type=bn
			// url : http://news.mixi.jp/list_news_category.pl?page=2&sort=1&id=1&type=bn

			CString url = data.GetURL();
			url += L"&";

			// �J�e�S���A�y�[�W�ԍ����o
			CString category;
			util::GetBetweenSubString( url, L"id=", L"&", category );
			CString page;
			util::GetBetweenSubString( url, L"page=", L"&", page );

			CString path;
			path.Format( L"%s\\list_news_category_%s_%s.html", theApp.m_filepath.logFolder, category, page );
			return path;
		}

	case ACCESS_LIST_BBS:
		{
			// url : list_bbs.pl?id=xxx
			CString id;
			util::GetAfterSubString( data.GetURL(), L"id=", id );

			CString path;
			path.Format( L"%s\\list_bbs_%s.html", theApp.m_filepath.logFolder, id );
			return path;
		}

	case ACCESS_LIST_FAVORITE:
		{
			// url(1) : http://mixi.jp/list_bookmark.pl
			// url(2) : http://mixi.jp/list_bookmark.pl?page=2&kind=friend
			// url(3) : http://mixi.jp/list_bookmark.pl?kind=community
			// url(4) : http://mixi.jp/list_bookmark.pl?page=2&kind=community

			// �y�[�W����
			CString page;
			util::GetAfterSubString( data.GetURL(), L"?", page );

			page.Replace( L"&", L"_" );
			page.Replace( L"=", L"_" );
			if( !page.IsEmpty() ) {
				page.Insert( 0, '_' );
			}

			CString path;
			path.Format( L"%s\\list_bookmark%s.html", theApp.m_filepath.logFolder, page );
			return path;
		}

	case ACCESS_LIST_FRIEND:
		{
			// url(1) : http://mixi.jp/list_friend.pl
			// url(2) : http://mixi.jp/list_friend.pl?page=2

			// �y�[�W����
			CString page;
			util::GetAfterSubString( data.GetURL(), L"page=", page );

			CString path;
			path.Format( L"%s\\list_friend%s.html", theApp.m_filepath.logFolder, page );
			return path;
		}
	case ACCESS_LIST_COMMUNITY:
		{
			// url(1) : http://mixi.jp/list_community.pl?id=xxx
			// url(2) : http://mixi.jp/list_community.pl?id=xxx&page=2

			// �y�[�W����
			CString page;
			util::GetAfterSubString( data.GetURL(), L"page=", page );

			CString path;
			path.Format( L"%s\\list_community%s.html", theApp.m_filepath.logFolder, page );
			return path;
		}
	case ACCESS_LIST_NEW_BBS_COMMENT:
		{
			// url(1) : http://mixi.jp/new_bbs_comment.pl?id=xxx
			// url(2) : http://mixi.jp/new_bbs_comment.pl?id=xxx&page=2

			// �y�[�W����
			CString page;
			util::GetAfterSubString( data.GetURL(), L"page=", page );

			CString path;
			path.Format( L"%s\\new_bbs_comment%s.html", theApp.m_filepath.logFolder, page );
			return path;
		}
	case ACCESS_LIST_INTRO:
		{
			// url(1) : http://mixi.jp/show_intro.pl
			// url(2) : http://mixi.jp/show_intro.pl?page=2

			// �y�[�W����
			CString page;
			util::GetAfterSubString( data.GetURL(), L"page=", page );

			CString path;
			path.Format( L"%s\\show_intro%s.html", theApp.m_filepath.logFolder, page );
			return path;
		}

	// �A�C�e���n
	case ACCESS_MYDIARY:
	case ACCESS_DIARY:
		{
			CString owner_id, id;
			// url(1) : http://mixi.jp/view_diary.pl?id=xxx&owner_id=yyy
			// url(2) : http://mixi.jp/view_diary.pl?id=xxx&owner_id=yyy&comment_count=8
			CString url = data.GetURL() + L"&";	// url(1) �̏ꍇ�ɑΏ����邽�߁A���� & ��t���Ă���
			util::GetBetweenSubString( url, L"?id=", L"&", id );
			util::GetBetweenSubString( url, L"owner_id=", L"&", owner_id );

			CString path;
			path.Format( L"%s\\%s", theApp.m_filepath.diaryFolder, owner_id );
			CreateDirectory( path, NULL/*always null*/ );
			path.AppendFormat( L"\\%s.html", id );
			return path;
		}

	case ACCESS_BBS:
		{
			CString comm_id, id;
			// url(1) : http://mixi.jp/view_bbs.pl?id=9360823&comm_id=1198460
			// url(2) : http://mixi.jp/view_bbs.pl?page=4&comm_id=1198460&id=9360823
			CString url;	// ? �ȍ~�� URL
			util::GetAfterSubString( data.GetURL(), L"?", url );
			// ���� & ��t���Ă���
			url += L"&";
			// ���� & ��t���Ă���
			url.Insert( 0, L"&" );
			// url(1) : &id=9360823&comm_id=1198460&
			// url(2) : &page=4&comm_id=1198460&id=9360823&
			util::GetBetweenSubString( url, L"&id=", L"&", id );
			util::GetBetweenSubString( url, L"comm_id=", L"&", comm_id );

			CString path;
			path.Format( L"%s\\%s", theApp.m_filepath.bbsFolder, comm_id );
			CreateDirectory( path, NULL/*always null*/ );
			path.AppendFormat( L"\\%s.html", id );
			return path;
		}

	case ACCESS_ENQUETE:
		{
			CString comm_id, id;
			// url(1) : http://mixi.jp/view_enquete.pl?id=13632077&comment_count=1&comm_id=1198460
			CString url;	// ? �ȍ~�� URL
			util::GetAfterSubString( data.GetURL(), L"?", url );
			// ���� & ��t���Ă���
			url += L"&";
			// ���� & ��t���Ă���
			url.Insert( 0, L"&" );
			// url(1) : &id=13632077&comment_count=1&comm_id=1198460&
			util::GetBetweenSubString( url, L"&id=", L"&", id );
			util::GetBetweenSubString( url, L"comm_id=", L"&", comm_id );

			CString path;
			path.Format( L"%s\\%s", theApp.m_filepath.enqueteFolder, comm_id );
			CreateDirectory( path, NULL/*always null*/ );
			path.AppendFormat( L"\\%s.html", id );
			return path;
		}

	case ACCESS_EVENT:
		{
			CString comm_id, id;
			// url(1) : http://mixi.jp/view_event.pl?id=xxx&comment_count=20&comm_id=xxx
			CString url;	// ? �ȍ~�� URL
			util::GetAfterSubString( data.GetURL(), L"?", url );
			// ���� & ��t���Ă���
			url += L"&";
			// ���� & ��t���Ă���
			url.Insert( 0, L"&" );
			// url(1) : &id=xxx&comment_count=20&comm_id=xxx&
			util::GetBetweenSubString( url, L"&id=", L"&", id );
			util::GetBetweenSubString( url, L"comm_id=", L"&", comm_id );

			CString path;
			path.Format( L"%s\\%s", theApp.m_filepath.eventFolder, comm_id );
			CreateDirectory( path, NULL/*always null*/ );
			path.AppendFormat( L"\\%s.html", id );
			return path;
		}

	case ACCESS_PROFILE:
		{
			// url(1) : http://mixi.jp/show_friend.pl?id={user_id}
			int id = mixi::MixiUrlParser::GetID( data.GetURL() );
			return util::FormatString( L"%s\\%d.html", theApp.m_filepath.profileFolder, id );
		}

	case ACCESS_MESSAGE:
		{
			CString comm_id, id;
			// url(1) : http://mixi.jp/view_message.pl?id=xxx&box=inbox
			// url(2) : http://mixi.jp/view_message.pl?id=xxx&box=outbox
			CString url;	// ? �ȍ~�� URL
			util::GetAfterSubString( data.GetURL(), L"?", url );
			// ���� & ��t���Ă���
			url += L"&";
			// ���� & ��t���Ă���
			url.Insert( 0, L"&" );
			// url(1) : &id=xxx&box=inbox&
			// url(2) : &id=xxx&box=outbox&
			util::GetBetweenSubString( url, L"&id=", L"&", id );

			// url �� inbox ������� inbox �f�B���N�g���ɁB
			CString path;
			if( url.Find( L"box=inbox" ) != -1 ) {
				path.Format( L"%s\\inbox", theApp.m_filepath.messageFolder );
			}else{
				path.Format( L"%s\\outbox", theApp.m_filepath.messageFolder );
			}
			CreateDirectory( path, NULL/*always null*/ );
			path.AppendFormat( L"\\%s.html", id );
			return path;
		}

	case ACCESS_NEWS:
		{
			CString media_id, id;
			// url(1) : http://news.mixi.jp/view_news.pl?id=132704&media_id=4
			CString url;	// ? �ȍ~�� URL
			util::GetAfterSubString( data.GetURL(), L"?", url );
			// ���� & ��t���Ă���
			url += L"&";
			// ���� & ��t���Ă���
			url.Insert( 0, L"&" );
			// url(1) : &id=132704&media_id=4&
			util::GetBetweenSubString( url, L"&id=", L"&", id );
			util::GetBetweenSubString( url, L"&media_id=", L"&", media_id );

			CString path;
			path.Format( L"%s\\%s", theApp.m_filepath.newsFolder, media_id );
			CreateDirectory( path, NULL/*always null*/ );
			path.AppendFormat( L"\\%s.html", id );
			return path;
		}

	//--- Twitter �n
	case ACCESS_TWITTER_FRIENDS_TIMELINE:
		{
			// http://twitter.com/statuses/friends_timeline.xml
			// http://twitter.com/statuses/friends_timeline/takke.xml
			// http://twitter.com/statuses/replies.xml
			// http://twitter.com/statuses/user_timeline.xml
			// http://twitter.com/statuses/user_timeline/takke.xml
			// => twitter_user_timeline_takke.xml

			CString filename;
			CString after;
			if (util::GetAfterSubString( data.GetURL(), L"statuses/", after )<0) {
				// default
				filename = L"twitter_friends_timeline.xml";
			} else {
				after.Replace( L"/", L"_" );
				filename = L"twitter_" + after;
			}

			return theApp.m_filepath.logFolder + L"\\" + filename;
		}

	default:
		// ��L�ȊO�Ȃ�A�t�@�C�����Ȃ�
		return L"";
	}
}

/**
 * �w�肳�ꂽURL�� mixi �� URL �ł���΁A
 * mixi ���o�C���̎������O�C���pURL�ɕϊ�����
 */
inline CString ConvertToMixiMobileAutoLoginUrl( LPCTSTR url )
{
	CString path;
	if( util::GetAfterSubString( url, L"//mixi.jp/", path ) >= 0 ) {
		// //mixi.jp/ ���܂܂�Ă���̂� mixi �� URL �Ƃ݂Ȃ��B
		// mixi ���o�C���pURL�ɕϊ�����URL��Ԃ��B
		return theApp.MakeLoginUrlForMixiMobile( path );
	}

	// mixi �pURL�ł͂Ȃ����߁A�ϊ������ɕԂ��B
	return url;
}

/**
 * �w�肳�ꂽURL�� mixi �� URL �ł���΁A
 * mixi �̎������O�C���pURL�ɕϊ�����
 */
inline CString ConvertToMixiAutoLoginUrl( LPCTSTR url )
{
	CString path;
	if( util::GetAfterSubString( url, L"//mixi.jp/", path ) >= 0 ) {
		// //mixi.jp/ ���܂܂�Ă���̂� mixi �� URL �Ƃ݂Ȃ��B
		// mixi �̎������O�C���pURL�ɕϊ�����URL��Ԃ��B
		return theApp.MakeLoginUrl( path );
	}

	// mixi �pURL�ł͂Ȃ����߁A�ϊ������ɕԂ��B
	return url;
}

/**
 * �w�肳�ꂽURL���u���E�U�ŊJ���i�m�F��ʕt���j
 */
inline void OpenBrowserForUrl( LPCTSTR url )
{
	// �m�F���
	CString msg;
	msg.Format( 
		L"���L��URL���u���E�U�ŊJ���܂��B\n\n"
		L"%s\n\n"
		L"��낵���ł����H", url );
	if( MessageBox( theApp.m_pMainWnd->m_hWnd, msg, MZ3_APP_NAME, MB_YESNO | MB_DEFBUTTON2 | MB_ICONQUESTION ) != IDYES )
	{
		// YES �{�^���ȊO�Ȃ̂ŏI��
		return;
	}

	// �������O�C���ϊ�
	CString requestUrl = url;

	// WindowsMobile �ł̂ݎ������O�C��URL�ϊ����s��
#ifdef WINCE
	if( theApp.m_optionMng.m_bConvertUrlForMixiMobile ) {
		// mixi ���o�C���p�������O�C��URL�ϊ�
		requestUrl = ConvertToMixiMobileAutoLoginUrl( url );
	} else {
		// mixi �p�������O�C��URL�ϊ�
		requestUrl = ConvertToMixiAutoLoginUrl( url );
	}
#endif

	// �u���E�U�ŊJ��
	util::OpenUrlByBrowser( requestUrl );
}

/**
 * �w�肳�ꂽ���[�U�̃v���t�B�[���y�[�W���u���E�U�ŊJ���i�m�F��ʕt���j
 */
inline void OpenBrowserForUser( LPCTSTR url, LPCTSTR szUserName )
{
	// �m�F���
	CString msg;
	msg.Format( 
		L"%s ����̃v���t�B�[���y�[�W���u���E�U�ŊJ���܂��B\n\n"
		L"%s\n\n"
		L"��낵���ł����H", szUserName, url );
	if( MessageBox( theApp.m_pMainWnd->m_hWnd, msg, MZ3_APP_NAME, MB_YESNO | MB_DEFBUTTON2 | MB_ICONQUESTION ) != IDYES )
	{
		// YES �{�^���ȊO�Ȃ̂ŏI��
		return;
	}

	// �������O�C���ϊ�
	CString requestUrl = url;

	// WindowsMobile �ł̂ݎ������O�C��URL�ϊ����s��
#ifdef WINCE
	if( theApp.m_optionMng.m_bConvertUrlForMixiMobile ) {
		// mixi ���o�C���p�������O�C��URL�ϊ�
		requestUrl = ConvertToMixiMobileAutoLoginUrl( url );
	} else {
		// mixi �p�������O�C��URL�ϊ�
		requestUrl = ConvertToMixiAutoLoginUrl( url );
	}
#endif

	// �u���E�U�ŊJ��
	util::OpenUrlByBrowser( requestUrl );
}

/**
 * ���R���p�C���ł���΃R���p�C������B
 *
 * �R���p�C�����s���̓G���[���O���o�͂���
 */
inline bool CompileRegex( MyRegex& reg, LPCTSTR szPattern )
{
	if( reg.isCompiled() ) {
		return true;
	} else {
		if(! reg.compile( szPattern ) ) {
			CString msg = FAILED_TO_COMPILE_REGEX_MSG;
			msg += L", pattern[";
			msg += szPattern;
			msg += L"]";
			MZ3LOGGER_FATAL( msg );
			return false;
		}
		return true;
	}
}

}
