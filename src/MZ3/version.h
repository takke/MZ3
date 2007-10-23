#pragma once

/// バージョン文字列
#define MZ3_VERSION_TEXT			L"Version 0.8.2.0 Beta30" 

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
