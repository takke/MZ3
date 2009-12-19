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
		setColorFromInifile( inifile, "ReportListText", "ReportListTextColor", SECTION_NAME );
		
		// ���C����ʁA�J�e�S���R���g���[���̃A�N�e�B�u���ڂ̕����F
		setColorFromInifile( inifile, "MainCategoryListActiveText", "MainCategoryListActiveTextColor", SECTION_NAME );
		
		// ���C����ʁA�J�e�S���R���g���[���̔�A�N�e�B�u���ڂ̕����F
		setColorFromInifile( inifile, "MainCategoryListInactiveText", "MainCategoryListInactiveTextColor", SECTION_NAME );
		
		// ���C����ʁA�{�f�B�R���g���[���̃f�t�H���g�����F
		setColorFromInifile( inifile, "MainBodyListDefaultText", "MainBodyListDefaultTextColor", SECTION_NAME );
		
		// ���C����ʁA�{�f�B�R���g���[���̖��Ǖ����F
		setColorFromInifile( inifile, "MainBodyListNonreadText", "MainBodyListNonreadTextColor", SECTION_NAME );
		
		// ���C����ʁA�{�f�B�R���g���[���̐V���L���̕����F
		setColorFromInifile( inifile, "MainBodyListNewItemText", "MainBodyListNewItemTextColor", SECTION_NAME );
		
		// ���C����ʁA�{�f�B�R���g���[���̂������Ƃ̃}�C�~�N�̕����F
		setColorFromInifile( inifile, "MainBodyListFootprintMyMixiText", "MainBodyListFootprintMyMixiTextColor", SECTION_NAME );
		
		// ���C����ʁA�{�f�B�R���g���[���̊O���u���O�̕����F
		setColorFromInifile( inifile, "MainBodyListExternalBlogText", "MainBodyListExternalBlogTextColor", SECTION_NAME );
		
		// ���C����ʁA�{�f�B�R���g���[���̓��t��؂��
		setColorFromInifile( inifile, "MainBodyListDayBreakLine", "MainBodyListDayBreakLine", SECTION_NAME );
		
		// ���C����ʁA�{�f�B�R���g���[���̋����Q
		setColorFromInifile( inifile, "MainBodyListEmphasis2", "MainBodyListEmphasis2", SECTION_NAME );
		
		// ���C����ʁA�{�f�B�R���g���[���̋����R
		setColorFromInifile( inifile, "MainBodyListEmphasis3", "MainBodyListEmphasis3", SECTION_NAME );
		
		// ���C����ʁA�{�f�B�R���g���[���̋����S
		setColorFromInifile( inifile, "MainBodyListEmphasis4", "MainBodyListEmphasis4", SECTION_NAME );
		
		// ���C����ʁA�X�e�[�^�X�o�[�̔w�i�F
		setColorFromInifile( inifile, "MainStatusBG", "MainStatusBG", SECTION_NAME);
		
		// ���C����ʁA�X�e�[�^�X�o�[�̕����F
		setColorFromInifile( inifile, "MainStatusText", "MainStatusText", SECTION_NAME);
		
		// ���C����ʁA�G�f�B�b�g�R���g���[���̔w�i�F
		setColorFromInifile( inifile, "MainEditBG", "MainEditBG", SECTION_NAME);
		
		// ���C����ʁA�G�f�B�b�g�R���g���[���̕����F
		setColorFromInifile( inifile, "MainEditText", "MainEditText", SECTION_NAME);

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

	colorMap.clear();
	setColor("ReportListText", 					RGB(0x00, 0x00, 0x00));	// ���|�[�g��ʁA���X�g�̕����F
	setColor("MainCategoryListActiveText",		RGB(0xFF, 0x00, 0x00));	// ���C����ʁA�J�e�S���R���g���[���̃A�N�e�B�u���ڂ̕����F
	setColor("MainCategoryListInactiveText",	RGB(0x00, 0x00, 0x00));	// ���C����ʁA�J�e�S���R���g���[���̔�A�N�e�B�u���ڂ̕����F
	setColor("MainBodyListDefaultText",			RGB(0x00, 0x00, 0x00));	// ���C����ʁA�{�f�B�R���g���[���̃f�t�H���g�����F
	setColor("MainBodyListNonreadText",			RGB(0x00, 0x00, 0xFF));	// ���C����ʁA�{�f�B�R���g���[���̖��Ǖ����F
	setColor("MainBodyListNewItemText",			RGB(0xFF, 0x00, 0x00));	// ���C����ʁA�{�f�B�R���g���[���̐V���L���̕����F
	setColor("MainBodyListFootprintMyMixiText",	RGB(0x00, 0x00, 0xFF));	// ���C����ʁA�{�f�B�R���g���[���̂������Ƃ̃}�C�~�N�̕����F
	setColor("MainBodyListExternalBlogText",	RGB(0x80, 0x80, 0x80));	// ���C����ʁA�{�f�B�R���g���[���̊O���u���O�̕����F
	setColor("MainBodyListDayBreakLine",		RGB(0xC0, 0xC0, 0xFF));	// ���C����ʁA�{�f�B�R���g���[���̓��t��؂��
	setColor("MainBodyListEmphasis2",			RGB(0xFF, 0x00, 0x00));	// ���C����ʁA�{�f�B�R���g���[���̋����Q(�f�t�H���g�F��)
	setColor("MainBodyListEmphasis3",			RGB(0x00, 0xA0, 0x00));	// ���C����ʁA�{�f�B�R���g���[���̋����R(�f�t�H���g�F��)
	setColor("MainBodyListEmphasis4",			RGB(0xCC, 0x00, 0xCC));	// ���C����ʁA�{�f�B�R���g���[���̋����S(�f�t�H���g�F�s���N)

	setColor("MainStatusBG",					RGB(0xEE, 0xEE, 0xEE));	// ���C����ʁA�X�e�[�^�X�o�[�̔w�i�F
	setColor("MainStatusText",					RGB(0x00, 0x00, 0x00));	// ���C����ʁA�X�e�[�^�X�o�[�̕����F
	setColor("MainEditBG",						RGB(0xFF, 0xFF, 0xFF));	// ���C����ʁA�G�f�B�b�g�R���g���[���̔w�i�F
	setColor("MainEditText",					RGB(0x00, 0x00, 0x00));	// ���C����ʁA�G�f�B�b�g�R���g���[���̕����F

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

void CMZ3SkinInfo::setColorFromInifile( inifile::IniFile& inifile, LPCSTR szColorName, LPCSTR key, LPCSTR section )
{
	COLORREF clrDefault = getColor(szColorName);
	COLORREF clr = loadColorFromInifile(inifile, clrDefault, key, section);
	setColor(szColorName, clr);
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

