/*
 * Copyright (c) MZ3 Project Team All rights reserved.
 * Code licensed under the BSD License:
 * http://www.mz3.jp/license.txt
 */
#pragma once

// リビジョン番号：
// "get_mz3_trunk_revision.vbs" で mz3.jp から取得し、生成されたファイルを include する。
#include "mz3_revision_in.h"

/// MZ3 用ユーティリティ
namespace util
{

/**
 * リビジョン番号を取得する
 *
 * 例："[94]"
 */
inline CString GetSourceRevision()
{
	CString rev = MZ3_SVN_REVISION;	// r998
	rev.Replace( L"r", L" [" );
	rev += L"]";
	return rev;
}

}
