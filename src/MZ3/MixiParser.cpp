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

/// mixi �pHTML�p�[�T
namespace mixi {

/**
 * ���O�A�E�g���������`�F�b�N����
 */
#ifdef BT_MZ3
bool MixiParserBase::IsLogout( LPCTSTR szHtmlFilename )
{
	// �ő�� N �s�ڂ܂Ń`�F�b�N����
	const int CHECK_LINE_NUM_MAX = 1000;

	FILE* fp = _wfopen(szHtmlFilename, _T("r"));
	if( fp == NULL ) {
		// �擾���s
		return false;
	}

	TCHAR buf[4096];
	for( int i=0; i<CHECK_LINE_NUM_MAX && fgetws(buf, 4096, fp) != NULL; i++ ) {
		// <form action="/login.pl" method="post">
		// ������΃��O�A�E�g��ԂƔ��肷��B
		if (util::LineHasStringsNoCase( buf, L"<form", L"action=", L"login.pl" )) {
			// ���O�A�E�g���
			fclose( fp );
			return true;
		}

		// API �Ή��i�������j
		if (i==0 && util::LineHasStringsNoCase( buf, L"WSSE�ɂ��F�؂��K�v�ł�" )) {
			fclose( fp );
			return true;
		}
	}
	fclose(fp);

	// �����ɂ̓f�[�^���Ȃ������̂Ń��O�A�E�g�Ƃ͔��f���Ȃ�
	return false;
}
#endif


};