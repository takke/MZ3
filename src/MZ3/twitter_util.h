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
	case 200:	// OK: ����
	case 304:	// Not Modified: �V�������͂Ȃ�
		break;
	case 400:	// Bad Request:
		szStatusErrorMessage = L"API �̎��s�񐔐����Ɉ����|�������A�Ȃǂ̗��R�Ń��N�G�X�g���p������";
		break;
	case 401:	// Not Authorized:
		szStatusErrorMessage = L"�F�؎��s";
		break;
	case 403:	// Forbidden:
		szStatusErrorMessage = L"�������Ȃ�API �����s���悤�Ƃ���";
		break;
	case 404:	// Not Found:
		szStatusErrorMessage = L"���݂��Ȃ� API �����s���悤�Ƃ����A���݂��Ȃ����[�U�������Ŏw�肵�� API �����s���悤�Ƃ���";
		break;
	case 500:	// Internal Server Error:
		szStatusErrorMessage = L"Twitter ���ŉ��炩�̖�肪�������Ă��܂�";
		break;
	case 502:	// Bad Gateway:
		szStatusErrorMessage = L"Twitter �̃T�[�o���~�܂��Ă��܂��i�����e����������܂���j";
		break;
	case 503:	// Service Unavailable:
		szStatusErrorMessage = L"Twitter �̃T�[�o�̕��ׂ��������āA���N�G�X�g���ق��؂�Ȃ���ԂɂȂ��Ă��܂�";
		break;
	}
	return szStatusErrorMessage;
}

}//namespace twitter
