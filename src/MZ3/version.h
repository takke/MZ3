/*
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */
#pragma once

/// バージョン文字列
#define MZ3_VERSION_TEXT_SHORT		L"0.9.2.0 Beta7"
#define MZ3_VERSION_TEXT			L"Version " MZ3_VERSION_TEXT_SHORT

/// リビジョン番号
#define MZ3_SVN_REVISION			L"$Rev$"

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
	CString rev = MZ3_SVN_REVISION;	// $Rev$
	rev.Replace( L"$Rev: ", L" [" );
	rev.Replace( L" $", L"]" );
	return rev;
}

}
