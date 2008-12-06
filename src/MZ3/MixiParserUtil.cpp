/*
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */
#include "stdafx.h"
#include "MixiParserUtil.h"

/// mixi 用HTMLパーサ
namespace mixi {

	/**
	 * 時刻変換(日付・時刻っぽい文字列を抽出し、mixi にSetDateする)
	 *
	 * @param line [in]  日時文字列を含む文字列。
	 *                   例："2006年11月19日 17:12"
	 *                       "<span class="date">2007年07月05日 21:55</span></dt>"
	 *                       "<td>10月08日</td></tr>"
	 *                       "1219927284" (エポック時間)
	 * @param mixi [out] 解析結果を SetDate で保存する。
	 */
	bool ParserUtil::ParseDate(LPCTSTR line, CMixiData& mixi)
	{
		// 汎用 形式
		{
			// 正規表現のコンパイル
			static MyRegex reg;
			if( !util::CompileRegex( reg, L"([0-9]{2,4})?年?([0-9]{1,2}?)月([0-9]{1,2})日[^0-9]*([0-9]{1,2})?:?時?([0-9]{2})?" ) ) {
				return false;
			}
			// 検索
			if( reg.exec(line) && reg.results.size() == 6 ) {
				// 抽出
				int year   = _wtoi( reg.results[1].str.c_str() );
				int month  = _wtoi( reg.results[2].str.c_str() );
				int day    = _wtoi( reg.results[3].str.c_str() );
				int hour   = _wtoi( reg.results[4].str.c_str() );
				int minute = _wtoi( reg.results[5].str.c_str() );
				//mixi.SetDate(year, month, day, hour, minute);

				if (year<1900) {
					// CTime は year<1900 をサポートしていないので、文字列として登録する。
					CString s;
					s.Format(_T("%02d/%02d %02d:%02d"), month, day, hour, minute);
					mixi.SetDate( s );
				} else {
					mixi.SetDate( CTime(year, month, day, hour, minute, 0) );
				}
				return true;
			}
		}

		CTime t;
		if (ParseDate(line, t)) {
			// 解析成功
			mixi.SetDate(t);
			return true;
		}

		CString msg = L"文字列内に日付・時刻が見つかりません : [";
		msg += line;
		msg += L"]";
		MZ3LOGGER_DEBUG( msg );
		return false;
	}


	bool ParserUtil::ParseDate(LPCTSTR line, CTime& t_result)
	{
		// T1. RSS 形式 (YYYY-MM-DDT00:00:00Z)
		{
			// 正規表現のコンパイル
			static MyRegex reg;
			if( !util::CompileRegex( reg, L"([0-9]{4})-([0-9]{2})-([0-9]{2})T([0-9]{2}):([0-9]{2}):([0-9]{2})Z" ) ) {
				return false;
			}
			// 検索
			if( reg.exec(line) && reg.results.size() == 7 ) {
				// 抽出
				int year   = _wtoi( reg.results[1].str.c_str() );
				int month  = _wtoi( reg.results[2].str.c_str() );
				int day    = _wtoi( reg.results[3].str.c_str() );
				int hour   = _wtoi( reg.results[4].str.c_str() );
				int minute = _wtoi( reg.results[5].str.c_str() );

				CTime t(year, month, day, hour, minute, 0);
				t += CTimeSpan(0, 9, 0, 0);

				t_result = t;
				//mixi.SetDate(t.GetYear(), t.GetMonth(), t.GetDay(), t.GetHour(), t.GetMinute());
				return true;
			}
		}

		// T2. RSS 形式 (YYYY-MM-DDT00:00:00+09:00)
		{
			// 正規表現のコンパイル
			static MyRegex reg;
			if( !util::CompileRegex( reg, L"([0-9]{4})-([0-9]{2})-([0-9]{2})T([0-9]{2}):([0-9]{2}):([0-9]{2})\\+([0-9]{2}):([0-9]{2})" ) ) {
				return false;
			}
			// 検索
			if( reg.exec(line) && reg.results.size() == 9 ) {
				// 抽出
				int year   = _wtoi( reg.results[1].str.c_str() );
				int month  = _wtoi( reg.results[2].str.c_str() );
				int day    = _wtoi( reg.results[3].str.c_str() );
				int hour   = _wtoi( reg.results[4].str.c_str() );
				int minute = _wtoi( reg.results[5].str.c_str() );

				CTime t(year, month, day, hour, minute, 0);
//				t += CTimeSpan(0, 9, 0, 0);

				t_result = t;
				return true;
			}
		}

		// T3. RSS 形式 (Sun Dec 16 09:00:00 +0000 2007)
		// T3. RSS 形式 (Fri Dec 5 1:03:05 +0900 2008)
		{
			// 正規表現のコンパイル
			static MyRegex reg;
			if( !util::CompileRegex( reg, L"([a-zA-Z]{3}) ([a-zA-Z]{3}) ([0-9]{1,2}) ([0-9]{1,2}):([0-9]{2}):([0-9]{2}) \\+([0-9]{4}) ([0-9]{4})" ) ) {
				return false;
			}
			// 検索
			if( reg.exec(line) && reg.results.size() == 9 ) {
				// 抽出

				// 年
				int year   = _wtoi( reg.results[8].str.c_str() );

				// 月
				int month  = ThreeCharMonthToInteger( reg.results[2].str.c_str() );
				int day    = _wtoi( reg.results[3].str.c_str() );
				int hour   = _wtoi( reg.results[4].str.c_str() );
				int minute = _wtoi( reg.results[5].str.c_str() );
				int sec    = _wtoi( reg.results[6].str.c_str() );

				CTime t(year, month, day, hour, minute, sec);

				int diff   = _wtoi( reg.results[7].str.c_str() );
				if (diff==0) {
					t += CTimeSpan(0, 9, 0, 0);
				}
				t_result = t;
				return true;
			}
		}

		// T4. RSS 2.0 形式 (Mon, 16 Dec 2007 09:00:00 +0900)
		// T4. RSS 2.0 形式 (Mon, 16 Dec 2007 09:00:00 GMT)
		{
			// 正規表現のコンパイル
			static MyRegex reg;
			if( !util::CompileRegex( reg, L"([a-zA-Z]{3}), ([0-9]{2}) ([a-zA-Z]{3}) ([0-9]{4}) ([0-9]{2}):([0-9]{2}):([0-9]{2}) (.*)" ) ) {
				return false;
			}
			// 検索
			if( reg.exec(line) && reg.results.size() == 9 ) {
				// 抽出

				// 年
				int year   = _wtoi( reg.results[4].str.c_str() );

				// 月
				int month  = ThreeCharMonthToInteger( reg.results[3].str.c_str() );
				int day    = _wtoi( reg.results[2].str.c_str() );
				int hour   = _wtoi( reg.results[5].str.c_str() );
				int minute = _wtoi( reg.results[6].str.c_str() );
				int sec    = _wtoi( reg.results[7].str.c_str() );
				CTime t(year, month, day, hour, minute, sec);

				// 簡易時差変換
				const std::wstring& time_diff = reg.results[8].str;

				if (time_diff==L"GMT") {
					t += CTimeSpan(0, 9, 0, 0);
				}

				t_result = t;
				return true;
			}
		}

		// T5. epoch 形式 (1219927284)
		{
			// 正規表現のコンパイル
			static MyRegex reg;
			if( !util::CompileRegex( reg, L"^([0-9]*)$" ) ) {
				return false;
			}
			// 検索
			if( reg.exec(line) && reg.results.size() == 2 ) {
				// 抽出

				t_result = CTime(_wtoi(reg.results[1].str.c_str()));

				return true;
			}
		}
		return false;
	}

}
