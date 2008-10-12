/*
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */
#include "stdafx.h"

#include "MZ3.h"
#include "util.h"

/**
 * �X�L���t�H���_/�X�L����/skin.ini �t�@�C������e������擾����B
 * �t�@�C�����Ȃ��ꍇ�͏����l��ݒ肷��
 */
bool CMZ3SkinInfo::loadSkinInifile( LPCTSTR szSkinName )
{
	// �X�L�����̐ݒ�
	strSkinName = szSkinName;

	// �f�t�H���g�l�̐ݒ�
	setDefaultInfo();


	CString strSkinIniFilePath;

	LPCTSTR szSkinIniFilenames[] = { L"skin.ini", L"skin1.ini", L"skin2.ini", L"skin3.ini", NULL };
	int i=0;
	for (; szSkinIniFilenames[i] != NULL; i++) {
		strSkinIniFilePath.Format( L"%s\\%s\\%s", theApp.m_filepath.skinFolder, strSkinName, szSkinIniFilenames[i] );
		if (util::ExistFile(strSkinIniFilePath)) {
			break;
		}
	}

	if (szSkinIniFilenames[i] == NULL) {
		// �t�@�C�����Ȃ��̂Ńf�t�H���g�l�̂܂܏I��
		CString msg;
		msg.Format( L"�X�L���ݒ�t�@�C�����Ȃ����߁A�����l�𗘗p���܂��Bskin-name[%s]", szSkinName );
		MZ3LOGGER_INFO( msg );
		return true;
	}

	inifile::IniFile inifile;

	if(! inifile.Load( strSkinIniFilePath ) ) {
		MZ3LOGGER_ERROR( L"�X�L���ݒ�t�@�C�� [" + strSkinIniFilePath + L"] ���ǂݍ��߂܂���ł���" );
		return false;
	}

	const static char* SECTION_NAME = "skin";
	if (inifile.SectionExists(SECTION_NAME) != FALSE) {
		// skin �Z�N�V����������̂Ń��[�h����

		// �^�C�g��
		std::string s = inifile.GetValue("title", SECTION_NAME);
		if (!s.empty()) {
			strSkinTitle = s.c_str();
		}

		// ���C���r���[�̃{�f�B�R���g���[���p�w�i�摜�t�@�C����
		s = inifile.GetValue("main_body_imagefile", SECTION_NAME);
		if (!s.empty()) {
			strMainBodyCtrlImageFileName = s.c_str();
		}

		// ���C���r���[�̃J�e�S���R���g���[���p�w�i�摜�t�@�C����
		s = inifile.GetValue("main_category_imagefile", SECTION_NAME);
		if (!s.empty()) {
			strMainCategoryCtrlImageFileName = s.c_str();
		}

		// ���|�[�g�r���[�̃��X�g�R���g���[���p�w�i�摜�t�@�C����
		s = inifile.GetValue("report_list_imagefile", SECTION_NAME);
		if (!s.empty()) {
			strReportListCtrlImageFileName = s.c_str();
		}

		// ���|�[�g��ʁA���X�g�̕����F
		clrReportListText					= loadColorFromInifile( inifile, clrReportListText, 
																	"ReportListTextColor", SECTION_NAME );
		// ���C����ʁA�J�e�S���R���g���[���̃A�N�e�B�u���ڂ̕����F
		clrMainCategoryListActiveText		= loadColorFromInifile( inifile, clrMainCategoryListActiveText, 
																	"MainCategoryListActiveTextColor", SECTION_NAME );
		// ���C����ʁA�J�e�S���R���g���[���̔�A�N�e�B�u���ڂ̕����F
		clrMainCategoryListInactiveText		= loadColorFromInifile( inifile, clrMainCategoryListInactiveText, 
																	"MainCategoryListInactiveTextColor", SECTION_NAME );
		// ���C����ʁA�{�f�B�R���g���[���̃f�t�H���g�����F
		clrMainBodyListDefaultText			= loadColorFromInifile( inifile, clrMainBodyListDefaultText, 
																	"MainBodyListDefaultTextColor", SECTION_NAME );
		// ���C����ʁA�{�f�B�R���g���[���̖��Ǖ����F
		clrMainBodyListNonreadText			= loadColorFromInifile( inifile, clrMainBodyListNonreadText, 
																	"MainBodyListNonreadTextColor", SECTION_NAME );
		// ���C����ʁA�{�f�B�R���g���[���̐V���L���̕����F
		clrMainBodyListNewItemText			= loadColorFromInifile( inifile, clrMainBodyListNewItemText, 
																	"MainBodyListNewItemTextColor", SECTION_NAME );
		// ���C����ʁA�{�f�B�R���g���[���̂������Ƃ̃}�C�~�N�̕����F
		clrMainBodyListFootprintMyMixiText	= loadColorFromInifile( inifile, clrMainBodyListFootprintMyMixiText, 
																	"MainBodyListFootprintMyMixiTextColor", SECTION_NAME );
		// ���C����ʁA�{�f�B�R���g���[���̊O���u���O�̕����F
		clrMainBodyListExternalBlogText		= loadColorFromInifile( inifile, clrMainBodyListExternalBlogText, 
																	"MainBodyListExternalBlogTextColor", SECTION_NAME );

		// ���C����ʁA�{�f�B�R���g���[���̓��t��؂��
		clrMainBodyListDayBreakLine			= loadColorFromInifile( inifile, clrMainBodyListDayBreakLine, 
																	"MainBodyListDayBreakLine", SECTION_NAME );

		// ���C����ʁA�{�f�B�R���g���[���̋����Q
		clrMainBodyListEmphasis2			= loadColorFromInifile( inifile, clrMainBodyListEmphasis2, 
																	"MainBodyListEmphasis2", SECTION_NAME );

		// ���C����ʁA�{�f�B�R���g���[���̋����Q
		clrMainBodyListEmphasis3			= loadColorFromInifile( inifile, clrMainBodyListEmphasis3, 
																	"MainBodyListEmphasis3", SECTION_NAME );
	}

	return true;
}


/**
 * �����l�̐ݒ�
 */
bool CMZ3SkinInfo::setDefaultInfo()
{
	strSkinTitle = strSkinName;										// �X�L���^�C�g��
	strMainBodyCtrlImageFileName		= L"body.jpg";				// ���C����ʂ̃{�f�B�R���g���[���p�w�i�摜�t�@�C����
	strMainCategoryCtrlImageFileName	= L"header.jpg";			// ���C����ʂ̃J�e�S���R���g���[���p�w�i�摜�t�@�C����
	strReportListCtrlImageFileName		= L"report.jpg";			// ���|�[�g��ʂ̃��X�g�R���g���[���p�w�i�摜�t�@�C����

	clrReportListText					= RGB(0x00, 0x00, 0x00);	// ���|�[�g��ʁA���X�g�̕����F
	clrMainCategoryListActiveText		= RGB(0xFF, 0x00, 0x00);	// ���C����ʁA�J�e�S���R���g���[���̃A�N�e�B�u���ڂ̕����F
	clrMainCategoryListInactiveText		= RGB(0x00, 0x00, 0x00);	// ���C����ʁA�J�e�S���R���g���[���̔�A�N�e�B�u���ڂ̕����F
	clrMainBodyListDefaultText			= RGB(0x00, 0x00, 0x00);	// ���C����ʁA�{�f�B�R���g���[���̃f�t�H���g�����F
	clrMainBodyListNonreadText			= RGB(0x00, 0x00, 0xFF);	// ���C����ʁA�{�f�B�R���g���[���̖��Ǖ����F
	clrMainBodyListNewItemText			= RGB(0xFF, 0x00, 0x00);	// ���C����ʁA�{�f�B�R���g���[���̐V���L���̕����F
	clrMainBodyListFootprintMyMixiText	= RGB(0x00, 0x00, 0xFF);	// ���C����ʁA�{�f�B�R���g���[���̂������Ƃ̃}�C�~�N�̕����F
	clrMainBodyListExternalBlogText		= RGB(0x80, 0x80, 0x80);	// ���C����ʁA�{�f�B�R���g���[���̊O���u���O�̕����F
	clrMainBodyListDayBreakLine			= RGB(0xC0, 0xC0, 0xFF);	// ���C����ʁA�{�f�B�R���g���[���̓��t��؂��
	clrMainBodyListEmphasis2			= RGB(0xFF, 0x00, 0x00);	// ���C����ʁA�{�f�B�R���g���[���̋����Q(�f�t�H���g�F��)
	clrMainBodyListEmphasis3			= RGB(0x00, 0xA0, 0x00);	// ���C����ʁA�{�f�B�R���g���[���̋����R(�f�t�H���g�F��)

	return true;
}


CString CMZ3SkinInfo::loadSkinTitle( LPCTSTR szSkinName )
{
	CMZ3SkinInfo skininfo;

	if (!skininfo.loadSkinInifile( szSkinName )) {
		return szSkinName;
	} else {
		return skininfo.strSkinTitle;
	}
}

COLORREF CMZ3SkinInfo::loadColorFromInifile( inifile::IniFile& inifile, COLORREF defaultColor, LPCSTR key, LPCSTR section )
{
	std::string s = inifile.GetValue(key, section);
	if (!s.empty()) {
		// �l�����݂���̂ł��̒l���g��
		// #xxxxxx �`�����p�[�X����
		int r, g, b;
		if (sscanf( s.c_str(), "%02x%02x%02x", &r, &g, &b )==3) {
			// �p�[�X����
			return RGB(r,g,b);
		} else {
			// �p�[�X���s
			// �f�t�H���g�l��Ԃ�
			CString msg;
			msg.Format( L"�X�L���̐F��`���Ԉ���Ă��܂��Bkey[%s], value[%s]", 
				util::my_mbstowcs(key).c_str(), 
				util::my_mbstowcs(s).c_str() );
			MZ3LOGGER_ERROR( msg );
			return defaultColor;
		}
	} else {
		// �l�����݂��Ȃ��̂Ńf�t�H���g�l��Ԃ�
		return defaultColor;
	}
}
