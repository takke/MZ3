/*
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */
#pragma once

/**
 * �X�L�����
 *
 * skin.ini ��1��1�őΉ����郂�f��
 */
class CMZ3SkinInfo
{
public:
	CString strSkinName;		///< �X�L����=�X�L���t�H���_

	CString strSkinTitle;						///< �X�L���^�C�g��
	CString strMainBodyCtrlImageFileName;		///< ���C����ʂ̃{�f�B�R���g���[���p�w�i�摜�t�@�C����
	CString strMainCategoryCtrlImageFileName;	///< ���C����ʂ̃J�e�S���R���g���[���p�w�i�摜�t�@�C����
	CString strReportListCtrlImageFileName;		///< ���|�[�g��ʂ̃��X�g�R���g���[���p�w�i�摜�t�@�C����

	COLORREF clrReportListText;					///< ���|�[�g��ʁA���X�g�̕����F
	COLORREF clrMainCategoryListActiveText;		///< ���C����ʁA�J�e�S���R���g���[���̃A�N�e�B�u���ڂ̕����F
	COLORREF clrMainCategoryListInactiveText;	///< ���C����ʁA�J�e�S���R���g���[���̔�A�N�e�B�u���ڂ̕����F
	COLORREF clrMainBodyListDefaultText;		///< ���C����ʁA�{�f�B�R���g���[���̃f�t�H���g�����F
	COLORREF clrMainBodyListNonreadText;		///< ���C����ʁA�{�f�B�R���g���[���̖��Ǖ����F
	COLORREF clrMainBodyListNewItemText;		///< ���C����ʁA�{�f�B�R���g���[���̐V���L���̕����F
	COLORREF clrMainBodyListFootprintMyMixiText;///< ���C����ʁA�{�f�B�R���g���[���̂������Ƃ̃}�C�~�N�̕����F
	COLORREF clrMainBodyListExternalBlogText;	///< ���C����ʁA�{�f�B�R���g���[���̊O���u���O�̕����F
	COLORREF clrMainBodyListDayBreakLine;		///< ���C����ʁA�{�f�B�R���g���[���̓��t��؂��

	CMZ3SkinInfo()
	{
		strSkinName = L"default";

		setDefaultInfo();
	}

	bool loadSkinInifile( LPCTSTR szSkinName );
	bool setDefaultInfo();

	static CString loadSkinTitle( LPCTSTR szSkinName );

private:
	COLORREF loadColorFromInifile( inifile::IniFile& inifile, COLORREF defaultColor, LPCSTR key, LPCSTR section );
};

