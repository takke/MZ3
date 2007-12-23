/*
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */
#pragma once
#include "stdafx.h"

namespace twitter {

inline LPCTSTR CheckHttpResponseStatus( DWORD dwStatus )
{
	LPCTSTR szStatusErrorMessage = NULL;

	switch (dwStatus) {
	case 200:	// OK: 成功
	case 304:	// Not Modified: 新しい情報はない
		break;
	case 400:	// Bad Request:
		szStatusErrorMessage = L"API の実行回数制限に引っ掛かった、などの理由でリクエストを却下した";
		break;
	case 401:	// Not Authorized:
		szStatusErrorMessage = L"認証失敗";
		break;
	case 403:	// Forbidden:
		szStatusErrorMessage = L"権限がないAPI を実行しようとした";
		break;
	case 404:	// Not Found:
		szStatusErrorMessage = L"存在しない API を実行しようとした、存在しないユーザを引数で指定して API を実行しようとした";
		break;
	case 500:	// Internal Server Error:
		szStatusErrorMessage = L"Twitter 側で何らかの問題が発生しています";
		break;
	case 502:	// Bad Gateway:
		szStatusErrorMessage = L"Twitter のサーバが止まっています（メンテ中かもしれません）";
		break;
	case 503:	// Service Unavailable:
		szStatusErrorMessage = L"Twitter のサーバの負荷が高すぎて、リクエストを裁き切れない状態になっています";
		break;
	}
	return szStatusErrorMessage;
}

}//namespace twitter
