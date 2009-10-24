/*
 * Copyright (c) MZ3 Project Team All rights reserved.
 * Code licensed under the BSD License:
 * http://www.mz3.jp/license.txt
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
};

}