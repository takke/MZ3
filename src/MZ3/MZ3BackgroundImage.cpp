#include "stdafx.h"

#include "MZ3.h"
#include "util.h"

/**
 * スキンファイルの整合性チェックを行う
 *
 * 画像ファイルがなければ false を返す。
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
 * ファイルが未ロード、またはスキン名が変わったらロードする
 */
bool CMZ3BackgroundImage::load()
{
	// 画像パスの生成。
	// スキンフォルダ名、スキン名、画像ファイル名を連結して生成する。
	CString strImagePath = makeSkinFilePath( theApp.m_optionMng.m_strSkinname );

	if( m_hBitmap == NULL ) {
		// 未ロードなのでロードする
		m_hBitmap = loadBitmap( strImagePath );
		m_strImageFilepath = strImagePath;
	} else {
		// ロード済みなので、スキンファイル名が異なればロードする。
		if (m_strImageFilepath != strImagePath) {
			// 削除する
			deleteBitmap();

			// ロードする
			m_hBitmap = loadBitmap( strImagePath );
			m_strImageFilepath = strImagePath;
		}
	}

	return true;
}
