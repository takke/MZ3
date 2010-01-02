/*
 * Copyright (c) MZ3 Project Team All rights reserved.
 * Code licensed under the BSD License:
 * http://www.mz3.jp/license.txt
 */
#pragma once

#include "stdafx.h"
#include "MZ3.h"
#include "MZ3Parser.h"
#include "MixiParser.h"

/// mixi 用HTMLパーサ
namespace mixi {

/**
 * ログアウトしたかをチェックする
 */
#ifdef BT_MZ3
bool MixiParserBase::IsLogout( LPCTSTR szHtmlFilename )
{
	// 最大で N 行目までチェックする
	const int CHECK_LINE_NUM_MAX = 1000;

	FILE* fp = _wfopen(szHtmlFilename, _T("r"));
	if( fp == NULL ) {
		// 取得失敗
		return false;
	}

	TCHAR buf[4096];
	for( int i=0; i<CHECK_LINE_NUM_MAX && fgetws(buf, 4096, fp) != NULL; i++ ) {
		// <form action="/login.pl" method="post">
		// があればログアウト状態と判定する。
		if (util::LineHasStringsNoCase( buf, L"<form", L"action=", L"login.pl" )) {
			// ログアウト状態
			fclose( fp );
			return true;
		}

		// API 対応（仮実装）
		if (i==0 && util::LineHasStringsNoCase( buf, L"WSSEによる認証が必要です" )) {
			fclose( fp );
			return true;
		}
	}
	fclose(fp);

	// ここにはデータがなかったのでログアウトとは判断しない
	return false;
}
#endif


};