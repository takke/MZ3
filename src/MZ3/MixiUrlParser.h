/*
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */
#pragma once

#include "util_base.h"

/// mixi 用HTMLパーサ
namespace mixi {

/// mixi の URL を分解するユーティリティ
class MixiUrlParser {
public:

	/**
	 * URL からオーナーIDを取得する。
	 *
	 * view_diary.pl?id=xxx&owner_id=xxx
	 */
	static CString GetOwnerID( const CString& url )
	{
		CString id = util::GetParamFromURL( url, L"owner_id" );
		return id;
	}

	/**
	 * URL からIDを取得する。
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
	 * コメント数の取得。
	 *
	 * http://mixi.jp/view_bbs.pl?id=xxx&comment_count=yyy&comm_id=zzz
	 */
	static int GetCommentCount(LPCTSTR url)
	{
		// "comment_count="と"&"に囲まれた文字列を数値変換したもの。
		CString strCommentCount = util::GetParamFromURL( url, L"comment_count" );
		if (strCommentCount.IsEmpty()) {
			// not found.
			return -1;
		}

		return _wtoi(strCommentCount);
	}

	/**
	 * 名前の取得。
	 *
	 * show_friend.pl?id=xxx">なまえ</a>
	 *
	 * @param str A タグの部分文字列
	 */
	static bool GetAuthor(LPCTSTR str, CMixiData* data)
	{
		TRACE( L"GetAuthor, param[%s]\n", str );

		// show_friend.pl 以降に整形。
		CString target;
		if( util::GetAfterSubString( str, L"show_friend.pl", target ) == -1 ) {
			// not found.
			CString msg;
			msg.Format( L"引数が show_friend.pl を含みません str[%s]", str );
			MZ3LOGGER_ERROR(msg);
			return false;
		}

		// ID 抽出
		CString id;
		if( util::GetBetweenSubString( target, L"id=", L"\">", id ) == -1 ) {
			// not found.
			CString msg;
			msg.Format( L"引数が 'id=' を含みません str[%s]", str );
			MZ3LOGGER_ERROR(msg);
			return false;
		}
		data->SetAuthorID( _wtoi(id) );

		// 名前抽出
		CString name;
		if( util::GetBetweenSubString( target, L">", L"<", name ) == -1 ) {
			CString msg;
			msg.Format( L"引数が '>', '<' を含みません str[%s]", str );
			MZ3LOGGER_ERROR(msg);
			return false;
		}
		data->SetAuthor( name );

		return true;
	}

};

}