/*
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */
#include "stdafx.h"
#include "MZ3.h"
#include "MyRegex.h"
#include "util_base.h"
#include "util_mixi.h"
#include "util_mz3.h"

/// MZ3 用ユーティリティ
namespace util
{

/**
 * ルールに従って data から文字列を抽出する
 *
 * @param data    データ
 * @param strRule ルール。例："urlparam:page"
 * @return strRule に従って data から抽出した文字列
 */
inline CString LogfileRuleToText( const CMixiData& data, const CString& strRule )
{
	LPCTSTR rule;
	
	// ルール："urlparam:パラメータ名"
	rule = L"urlparam:";
	if (wcsncmp(strRule, rule, wcslen(rule)) == 0) {
		// パラメータ名取得
		CString param = strRule.Mid(wcslen(rule));

		// URL から param パラメータを取得し、その文字列を返す。
		return GetParamFromURL( data.GetURL(), param );
	}
	
	// ルール："urlafter:TargetURL[:default_path]"
	rule = L"urlafter:";
	if (wcsncmp(strRule, rule, wcslen(rule)) == 0) {
		// TargetURL 取得
		CString targetURL = strRule.Mid(wcslen(rule));

		// default_path があれば取得
		CString defaultPath = L"";
		int defaultPathIdx = targetURL.Find(':');
		if (defaultPathIdx>0) {
			defaultPath = targetURL.Mid( defaultPathIdx+1 );	// ':' 以降
			targetURL = targetURL.Left( defaultPathIdx );		// ':' 以前
		}

		// URL から param パラメータを取得し、その文字列を返す。
		CString after;
		if (util::GetAfterSubString( data.GetURL(), targetURL, after )<0) {
			return defaultPath;
		} else {
			return after;
		}
	}

	// ルールにマッチしなかったので空文字列を返す。
	return L"";
}

/**
 * ルールに従ってログファイルパス文字列を生成する
 */
inline CString MakeLogfilePathByRule( CString strBasePath, const CMixiData& data, LPCTSTR strLogfilePathRule )
{
//	MZ3_TRACE(L"★MakeLogfilePathByRule(), base_path[%s], rule[%s], url[%s]\n", strBasePath, strLogfilePathRule, data.GetURL());

	CString rule = strLogfilePathRule;
	CString path = strBasePath;
	path += L"\\";

	// rule 解析
	// "{...}" があれば、ルールに従って置換する。
	for (;;) {
		int pos1 = rule.Find( '{' );
		if (pos1==-1) {
			path += rule;
			break;
		}

		int pos2 = rule.Find( '}', pos1+1 );
		if (pos2==-1) {
			// ex: "{x.html"
			path += rule;
			break;
		}

		// '{' より前の文字列を出力
		if (pos1>0) {
			path += rule.Left(pos1);
		}

		// 部分文字列の取得
		CString subrule = rule.Mid( pos1+1, pos2-pos1-1 );

		// ルール解析
		path += LogfileRuleToText( data, subrule );

		// '}' 以前の文字列を削除
		rule.Delete(0, pos2+1);
	}

	// path 内の '/' は '_' に置換する
	path.Replace( L"/", L"_" );
	// path 内の '?' は '_' に置換する
	path.Replace( L"?", L"_" );
	// path 内の '{', '}' は '_' に置換する
	path.Replace( L"{", L"_" );
	path.Replace( L"}", L"_" );

//	MZ3_TRACE( L"MakeLogfilePathByRule()\n" );
//	MZ3_TRACE( L" URL  : /%s/\n", data.GetURL() );
//	MZ3_TRACE( L" rule : /%s/\n", strLogfilePathRule );
//	MZ3_TRACE( L" path : /%s/\n", path );

	// ディレクトリがなければ生成する
	{
		int start = strBasePath.GetLength()+1;	// strBasePath は常に存在すると仮定する
		for (;;) {
			int idx = path.Find( '\\', start );
			if (idx==-1) {
				break;
			}

			CString dirpath = path.Left(idx);
//			MZ3_TRACE( L" CreateDirectory : /%s/\n", dirpath );
			CreateDirectory( dirpath, NULL/*always null*/ );

			start = idx+1;
		}
	}

	return path;
}

/**
 * CMixiData に対応するログファイルのパスを生成する
 */
CString MakeLogfilePath( const CMixiData& data )
{
	// アクセス種別に応じてパスを生成する
	CString strCacheFilePattern = theApp.m_accessTypeInfo.getCacheFilePattern( data.GetAccessType() );
	if (strCacheFilePattern.IsEmpty()) {
		// 未指定なのでキャッシュ保存しない。
		return L"";
	} else {
		// ルール解析した結果を返す。
		return MakeLogfilePathByRule( theApp.m_filepath.logFolder, data, strCacheFilePattern );
	}
}

CString MakeImageLogfilePathFromUrl( const CString& url )
{
	CString filename = ExtractFilenameFromUrl( url, L"" );
	if (!filename.IsEmpty()) {
		return theApp.m_filepath.imageFolder + L"\\" + filename;
	}
	return L"";
}

CString MakeImageLogfilePathFromUrlMD5( const CString& url )
{
	// http://takke.jp/hoge/fuga.png => (md5) => xxxx...xxx
	if (url.IsEmpty()) {
		return L"";
	} else {
		unsigned char ansi_string[1024];
		memset( &ansi_string[0], 0x00, sizeof(char) * 1024 );
		unsigned int len_in_mbs = wcstombs( (char*)&ansi_string[0], url, 1023 );

		MD5 md5(ansi_string, len_in_mbs);
		char* pMD5hexdigest = md5.hex_digest();
		if ( pMD5hexdigest ) {
			MZ3_TRACE(L"MakeImageLogfilePathFromUrlMD5(), url[%s], filename[%s]\n", 
				(LPCTSTR)url, (LPCTSTR)CString(CStringA(pMD5hexdigest)));
			CString LogfilePath = theApp.m_filepath.imageFolder + L"\\" + CString(CStringA(pMD5hexdigest));
			delete pMD5hexdigest;
			return LogfilePath;
		} else {
			return L"";
		}
	}
}

}