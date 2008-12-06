/*
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */
#pragma once
#include "MyRegex.h"

/// goo用ユーティリティ
namespace gooutil {

	/// ひとこと投稿アドレスからAPI KEYを取得する
	inline CString GetAPIKeyFromQuoteMailAddress(LPCTSTR address)
	{
		static MyRegex reg;
		if (!reg.isCompiled()) {
			if (!reg.compile( L"^quote-([0-9a-zA-Z\\-_]{12})@home\\.goo\\.ne\\.jp$" ) ) {
				MZ3LOGGER_DEBUG( FAILED_TO_COMPILE_REGEX_MSG );
			}
		}
		if (!reg.isCompiled()) {
			return L"";
		}
		if (!reg.exec(address) || reg.results.size()!=2) {
			return L"";
		}
		return reg.results[1].str.c_str();
	}

	/// "quote-XXXXXXXXXXXX@home.goo.ne.jp" の形式であることを確認する
	inline bool IsValidQuoteMailAddress(LPCTSTR address)
	{
		CString strAPIKey = GetAPIKeyFromQuoteMailAddress(address);
		return !strAPIKey.IsEmpty();
	}
}

