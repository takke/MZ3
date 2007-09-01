#include "stdafx.h"

#include "MZ3.h"
#include "util.h"

/**
 * �X�L���t�@�C���̐������`�F�b�N���s��
 *
 * �摜�t�@�C�����Ȃ���� false ��Ԃ��B
 */
bool CMZ3BackgroundImage::isValidSkinfile( LPCTSTR szSkinName )
{
	CString strImagePath = makeSkinFilePath(szSkinName);
	if (!util::ExistFile(strImagePath)) {
		return false;
	}

	return true;
}

CString CMZ3BackgroundImage::makeSkinFilePath( LPCTSTR szSkinName )
{
	CString strImagePath;
	strImagePath.Format( L"%s\\%s\\%s", (LPCTSTR)theApp.m_filepath.skinFolder, (LPCTSTR)szSkinName, m_strImageName );
	return strImagePath;
}

/**
 * �t�@�C���������[�h�A�܂��̓X�L�������ς�����烍�[�h����
 */
bool CMZ3BackgroundImage::load()
{
	// �摜�p�X�̐����B
	// �X�L���t�H���_���A�X�L�����A�摜�t�@�C������A�����Đ�������B
	CString strImagePath = makeSkinFilePath( theApp.m_optionMng.m_strSkinname );

	if( m_hBitmap == NULL ) {
		// �����[�h�Ȃ̂Ń��[�h����
		m_hBitmap = loadBitmap( strImagePath );
		m_strImageFilepath = strImagePath;
	} else {
		// ���[�h�ς݂Ȃ̂ŁA�X�L���t�@�C�������قȂ�΃��[�h����B
		if (m_strImageFilepath != strImagePath) {
			// �폜����
			deleteBitmap();

			// ���[�h����
			m_hBitmap = loadBitmap( strImagePath );
			m_strImageFilepath = strImagePath;
		}
	}

	return true;
}
