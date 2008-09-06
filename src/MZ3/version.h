/*
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */
#pragma once

/// バージョン文字列
#define MZ3_VERSION_TEXT_SHORT		L"0.9.3.0"
#define MZ3_VERSION_TEXT			L"Version " MZ3_VERSION_TEXT_SHORT

/// リビジョン番号
/// "get_mz3_trunk_revision.vbs" で mz3.jp から取得し、生成されたファイルを include する。
#include "mz3_revision.h"

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
