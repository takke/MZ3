/*
 * Copyright (c) MZ3 Project Team All rights reserved.
 * Code licensed under the BSD License:
 * http://www.mz3.jp/license.txt
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

		// ���C����ʁA�X�e�[�^�X�o�[�̔w�i�F
		clrMainStatusBG						= loadColorFromInifile( inifile, clrMainStatusBG, 
																	"MainStatusBG", SECTION_NAME);

		// ���C����ʁA�X�e�[�^�X�o�[�̕����F
		clrMainStatusText					= loadColorFromInifile( inifile, clrMainStatusText, 
																	"MainStatusText", SECTION_NAME);

		// ���C����ʁA�G�f�B�b�g�R���g���[���̔w�i�F
		clrMainEditBG						= loadColorFromInifile( inifile, clrMainEditBG, 
																	"MainEditBG", SECTION_NAME);

		// ���C����ʁA�G�f�B�b�g�R���g���[���̕����F
		clrMainEditText						= loadColorFromInifile( inifile, clrMainEditText, 
																	"MainEditText", SECTION_NAME);

		// ���C����ʁA�J�e�S���R���g���[���̘g���L��
		bMainCategoryListBorder = loadBoolVar(inifile, bMainCategoryListBorder, "MainCategoryListBorder", SECTION_NAME);

		// ���C����ʁA�{�f�B�R���g���[���̘g���L��
		bMainBodyListBorder		= loadBoolVar(inifile, bMainBodyListBorder, "MainBodyListBorder", SECTION_NAME);

		// ���C����ʁA�X�e�[�^�X�o�[�̘g���L��
		bMainStatusBorder		= loadBoolVar(inifile, bMainStatusBorder, "MainStatusBorder", SECTION_NAME);

		// ���C����ʁA�G�f�B�b�g�R���g���[���̘g���L��
		bMainEditBorder			= loadBoolVar(inifile, bMainEditBorder, "MainEditBorder", SECTION_NAME);
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

	clrMainStatusBG						= RGB(0xEE, 0xEE, 0xEE);	// ���C����ʁA�X�e�[�^�X�o�[�̔w�i�F
	clrMainStatusText					= RGB(0x00, 0x00, 0x00);	// ���C����ʁA�X�e�[�^�X�o�[�̕����F
	clrMainEditBG						= RGB(0xFF, 0xFF, 0xFF);	// ���C����ʁA�G�f�B�b�g�R���g���[���̔w�i�F
	clrMainEditText						= RGB(0x00, 0x00, 0x00);	// ���C����ʁA�G�f�B�b�g�R���g���[���̕����F

	bMainCategoryListBorder = true;			// ���C����ʁA�J�e�S���R���g���[���̘g���L��
	bMainBodyListBorder		= true;			// ���C����ʁA�{�f�B�R���g���[���̘g���L��
	bMainStatusBorder		= true;			// ���C����ʁA�X�e�[�^�X�o�[�̘g���L��
	bMainEditBorder			= false;		// ���C����ʁA�G�f�B�b�g�R���g���[���̘g���L��

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

bool CMZ3SkinInfo::loadBoolVar(inifile::IniFile& inifile, bool defaultVar, LPCSTR key, LPCSTR section )
{
	std::string s = inifile.GetValue(key, section);
	if (!s.empty()) {
		// �l�����݂���̂ł��̒l���g��
		int n = atoi(s.c_str());
		if (n) {
			return true;
		} else {
			return false;
		}
	} else {
		// �l�����݂��Ȃ��̂Ńf�t�H���g�l��Ԃ�
		return defaultVar;
	}
}

