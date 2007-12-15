/*
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */
#pragma once

#include "util_base.h"

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
	static CString GetOwnerID( const CString& url )
	{
		CString id = util::GetParamFromURL( url, L"owner_id" );
		return id;
	}

	/**
	 * URL ����ID���擾����B
	 *
	 * view_community.pl?id=1231285 => 1231285
	 * view_diary.pl?id=xxxx&owner_id=yyyy => xxxx
	 */
	static int GetID( const CString& url )
	{
		CString id = util::GetParamFromURL( url, L"id" );
		return _wtoi(id);
	}

	/**
	 * �R�����g���̎擾�B
	 *
	 * http://mixi.jp/view_bbs.pl?id=xxx&comment_count=yyy&comm_id=zzz
	 */
	static int GetCommentCount(LPCTSTR url)
	{
		// "comment_count="��"&"�Ɉ͂܂ꂽ������𐔒l�ϊ��������́B
		CString strCommentCount = util::GetParamFromURL( url, L"comment_count" );
		if (strCommentCount.IsEmpty()) {
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
		data->SetAuthorID( _wtoi(id) );

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

}