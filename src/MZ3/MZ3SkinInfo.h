/*
 * Copyright (c) MZ3 Project Team All rights reserved.
 * Code licensed under the BSD License:
 * http://www.mz3.jp/license.txt
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
	COLORREF clrMainBodyListEmphasis2;			///< ���C����ʁA�{�f�B�R���g���[���̋����Q(�f�t�H���g�F��)
	COLORREF clrMainBodyListEmphasis3;			///< ���C����ʁA�{�f�B�R���g���[���̋����R(�f�t�H���g�F��)
	COLORREF clrMainBodyListEmphasis4;			///< ���C����ʁA�{�f�B�R���g���[���̋����S(�f�t�H���g�F�s���N)

	COLORREF clrMainStatusBG;					///< ���C����ʁA�X�e�[�^�X�o�[�̔w�i�F
	COLORREF clrMainStatusText;					///< ���C����ʁA�X�e�[�^�X�o�[�̕����F
	COLORREF clrMainEditBG;						///< ���C����ʁA�G�f�B�b�g�R���g���[���̔w�i�F
	COLORREF clrMainEditText;					///< ���C����ʁA�G�f�B�b�g�R���g���[���̕����F

	bool	 bMainCategoryListBorder;			///< ���C����ʁA�J�e�S���R���g���[���̘g���L��
	bool	 bMainBodyListBorder;				///< ���C����ʁA�{�f�B�R���g���[���̘g���L��
	bool	 bMainStatusBorder;					///< ���C����ʁA�X�e�[�^�X�o�[�̘g���L��
	bool	 bMainEditBorder;					///< ���C����ʁA�G�f�B�b�g�R���g���[���̘g���L��

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
	bool loadBoolVar(inifile::IniFile& inifile, bool defaultVar, LPCSTR key, LPCSTR section );
};

