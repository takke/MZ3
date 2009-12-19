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

	std::map<std::string, COLORREF> colorMap;	///< �e��J���[�̃}�b�v(�J���[�� => �J���[)

	COLORREF getColor(const char* szColorName)
	{
		if (colorMap.count(szColorName)==0) {
			return RGB(0, 0, 0);
		}
		return colorMap[szColorName];
	}

	void setColor(const char* szColorName, COLORREF clr)
	{
		colorMap[szColorName] = clr;
	}

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
	void setColorFromInifile( inifile::IniFile& inifile, LPCSTR szColorName, LPCSTR key, LPCSTR section );
	COLORREF loadColorFromInifile( inifile::IniFile& inifile, COLORREF defaultColor, LPCSTR key, LPCSTR section );
	bool loadBoolVar(inifile::IniFile& inifile, bool defaultVar, LPCSTR key, LPCSTR section );
};

