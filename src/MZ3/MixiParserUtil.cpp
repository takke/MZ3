/*
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */
#include "stdafx.h"
#include "MixiParserUtil.h"

/// mixi �pHTML�p�[�T
namespace mixi {

	/**
	 * �����ϊ�(���t�E�������ۂ�������𒊏o���Amixi ��SetDate����)
	 *
	 * @param line [in]  ������������܂ޕ�����B
	 *                   ��F"2006�N11��19�� 17:12"
	 *                       "<span class="date">2007�N07��05�� 21:55</span></dt>"
	 *                       "<td>10��08��</td></tr>"
	 *                       "1219927284" (�G�|�b�N����)
	 * @param mixi [out] ��͌��ʂ� SetDate �ŕۑ�����B
	 */
	bool ParserUtil::ParseDate(LPCTSTR line, CMixiData& mixi)
	{
		// �ėp �`��
		{
			// ���K�\���̃R���p�C��
			static MyRegex reg;
			if( !util::CompileRegex( reg, L"([0-9]{2,4})?�N?([0-9]{1,2}?)��([0-9]{1,2})��[^0-9]*([0-9]{1,2})?:?��?([0-9]{2})?" ) ) {
				return false;
			}
			// ����
			if( reg.exec(line) && reg.results.size() == 6 ) {
				// ���o
				int year   = _wtoi( reg.results[1].str.c_str() );
				int month  = _wtoi( reg.results[2].str.c_str() );
				int day    = _wtoi( reg.results[3].str.c_str() );
				int hour   = _wtoi( reg.results[4].str.c_str() );
				int minute = _wtoi( reg.results[5].str.c_str() );
				//mixi.SetDate(year, month, day, hour, minute);

				if (year<1900) {
					// CTime �� year<1900 ���T�|�[�g���Ă��Ȃ��̂ŁA������Ƃ��ēo�^����B
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
			// ��͐���
			mixi.SetDate(t);
			return true;
		}

		CString msg = L"��������ɓ��t�E������������܂��� : [";
		msg += line;
		msg += L"]";
		MZ3LOGGER_DEBUG( msg );
		return false;
	}


	bool ParserUtil::ParseDate(LPCTSTR line, CTime& t_result)
	{
		// T1. RSS �`�� (YYYY-MM-DDT00:00:00Z)
		{
			// ���K�\���̃R���p�C��
			static MyRegex reg;
			if( !util::CompileRegex( reg, L"([0-9]{4})-([0-9]{2})-([0-9]{2})T([0-9]{2}):([0-9]{2}):([0-9]{2})Z" ) ) {
				return false;
			}
			// ����
			if( reg.exec(line) && reg.results.size() == 7 ) {
				// ���o
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

		// T2. RSS �`�� (YYYY-MM-DDT00:00:00+09:00)
		{
			// ���K�\���̃R���p�C��
			static MyRegex reg;
			if( !util::CompileRegex( reg, L"([0-9]{4})-([0-9]{2})-([0-9]{2})T([0-9]{2}):([0-9]{2}):([0-9]{2})\\+([0-9]{2}):([0-9]{2})" ) ) {
				return false;
			}
			// ����
			if( reg.exec(line) && reg.results.size() == 9 ) {
				// ���o
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

		// T3. RSS �`�� (Sun Dec 16 09:00:00 +0000 2007)
		// T3. RSS �`�� (Fri Dec 5 1:03:05 +0900 2008)
		{
			// ���K�\���̃R���p�C��
			static MyRegex reg;
			if( !util::CompileRegex( reg, L"([a-zA-Z]{3}) ([a-zA-Z]{3}) ([0-9]{1,2}) ([0-9]{1,2}):([0-9]{2}):([0-9]{2}) \\+([0-9]{4}) ([0-9]{4})" ) ) {
				return false;
			}
			// ����
			if( reg.exec(line) && reg.results.size() == 9 ) {
				// ���o

				// �N
				int year   = _wtoi( reg.results[8].str.c_str() );

				// ��
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

		// T4. RSS 2.0 �`�� (Mon, 16 Dec 2007 09:00:00 +0900)
		// T4. RSS 2.0 �`�� (Mon, 16 Dec 2007 09:00:00 GMT)
		{
			// ���K�\���̃R���p�C��
			static MyRegex reg;
			if( !util::CompileRegex( reg, L"([a-zA-Z]{3}), ([0-9]{2}) ([a-zA-Z]{3}) ([0-9]{4}) ([0-9]{2}):([0-9]{2}):([0-9]{2}) (.*)" ) ) {
				return false;
			}
			// ����
			if( reg.exec(line) && reg.results.size() == 9 ) {
				// ���o

				// �N
				int year   = _wtoi( reg.results[4].str.c_str() );

				// ��
				int month  = ThreeCharMonthToInteger( reg.results[3].str.c_str() );
				int day    = _wtoi( reg.results[2].str.c_str() );
				int hour   = _wtoi( reg.results[5].str.c_str() );
				int minute = _wtoi( reg.results[6].str.c_str() );
				int sec    = _wtoi( reg.results[7].str.c_str() );
				CTime t(year, month, day, hour, minute, sec);

				// �ȈՎ����ϊ�
				const std::wstring& time_diff = reg.results[8].str;

				if (time_diff==L"GMT") {
					t += CTimeSpan(0, 9, 0, 0);
				}

				t_result = t;
				return true;
			}
		}

		// T5. epoch �`�� (1219927284)
		{
			// ���K�\���̃R���p�C��
			static MyRegex reg;
			if( !util::CompileRegex( reg, L"^([0-9]*)$" ) ) {
				return false;
			}
			// ����
			if( reg.exec(line) && reg.results.size() == 2 ) {
				// ���o

				t_result = CTime(_wtoi(reg.results[1].str.c_str()));

				return true;
			}
		}
		return false;
	}

}
