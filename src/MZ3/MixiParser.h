/*
 * Copyright (c) MZ3 Project Team All rights reserved.
 * Code licensed under the BSD License:
 * http://www.mz3.jp/license.txt
 */
#pragma once

#include "MZ3Parser.h"

#ifdef BT_MZ3

/// mixi�p�p�[�T
namespace mixi {

/// �e��mixi�p�[�T�̊�{�N���X
class MixiParserBase : public parser::MZ3ParserBase
{
public:
	/**
	 * ���O�A�E�g���������`�F�b�N����
	 */
	static bool IsLogout( LPCTSTR szHtmlFilename );
};

//���������ʁ�����

/**
 * �摜�_�E�����[�hCGI �p�p�[�T
 *
 * show_diary_picture.pl
 * show_bbs_comment_picture.pl
 */
class ShowPictureParser : public MixiParserBase
{
public:

	/**
	 * �摜URL�擾
	 */
	static CString GetImageURL( const CHtmlArray& html )
	{
		INT_PTR count = html.GetCount();

		/* 
		 * ��͑Ώە�����F
		 * <img SRC="http://ic76.mixi.jp/p/xxx/xxx/diary/xx/x/xxx.jpg" BORDER=0>
		 */
		CString uri;
		for (int i=0; i<count; i++) {
			// �摜�ւ̃����N�𒊏o
			const CString& line = html.GetAt(i);
			if( util::LineHasStringsNoCase( line, L"<img src=\"", L"\"" ) ) {
				// " ���� " �܂ł��擾����B
				if( util::GetBetweenSubString( line, L"\"", L"\"", uri ) > 0 ) {
					MZ3LOGGER_DEBUG( L"�摜�ւ̃����N���oOK, url[" + uri + L"]" );
					break;
				}
			}
		}
		return uri;
	}
};


/**
 * [content] home.pl ���O�C����̃��C����ʗp�p�[�T
 * �y���C���g�b�v��ʁz
 * http://mixi.jp/home.pl
 */
class HomeParser
{
public:
	/**
	 * ���O�C������
	 *
	 * ���O�C�������������ǂ����𔻒�
	 *
	 * @return ���O�C���������͎���URL�A���s���͋�̕������Ԃ�
	 */
	static bool IsLoginSucceeded( const CHtmlArray& html )
	{
		INT_PTR count = html.GetCount();

		for (int i=0; i<count; i++) {
			const CString& line = html.GetAt(i);

			if (util::LineHasStringsNoCase( line, L"refresh", L"check.pl" )) {
				// <html><head><meta http-equiv="refresh" content="0;url=/check.pl?n=%2Fhome.pl"></head></html>
				return true;
			}
			// <title>[mixi]</title>
			if (util::LineHasStringsNoCase(line, L"<title>[mixi]</title>")) {
				return true;
			}
		}

		return false;
	}
};



}//namespace mixi

#endif	// BT_MZ3
