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
	CString strMainBodyCtrlImageFileName;		///< ���C���r���[�̃{�f�B�R���g���[���p�w�i�摜�t�@�C����
	CString strMainCategoryCtrlImageFileName;	///< ���C���r���[�̃J�e�S���R���g���[���p�w�i�摜�t�@�C����
	CString strReportListCtrlImageFileName;		///< ���|�[�g�r���[�̃��X�g�R���g���[���p�w�i�摜�t�@�C����

	CMZ3SkinInfo()
	{
		strSkinName = L"default";

		setDefaultInfo();
	}

	bool loadSkinInifile( LPCTSTR szSkinName );

	/**
	 * �����l�̐ݒ�
	 */
	bool setDefaultInfo()
	{
		strSkinTitle = strSkinName;
		strMainBodyCtrlImageFileName		= L"body.jpg";
		strMainCategoryCtrlImageFileName	= L"header.jpg";
		strReportListCtrlImageFileName		= L"report.jpg";

		return true;
	}

	static CString loadSkinTitle( LPCTSTR szSkinName );
};

