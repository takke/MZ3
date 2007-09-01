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
		MZ3LOGGER_INFO( L"�X�L���ݒ�t�@�C�����Ȃ����߁A�����l�𗘗p���܂�" );
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
	}

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
