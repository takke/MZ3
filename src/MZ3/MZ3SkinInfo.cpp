#include "stdafx.h"

#include "MZ3.h"
#include "util.h"

/**
 * スキンフォルダ/スキン名/skin.ini ファイルから各種情報を取得する。
 * ファイルがない場合は初期値を設定する
 */
bool CMZ3SkinInfo::loadSkinInifile( LPCTSTR szSkinName )
{
	// スキン名の設定
	strSkinName = szSkinName;

	// デフォルト値の設定
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
		// ファイルがないのでデフォルト値のまま終了
		MZ3LOGGER_INFO( L"スキン設定ファイルがないため、初期値を利用します" );
		return true;
	}

	inifile::IniFile inifile;

	if(! inifile.Load( strSkinIniFilePath ) ) {
		MZ3LOGGER_ERROR( L"スキン設定ファイル [" + strSkinIniFilePath + L"] が読み込めませんでした" );
		return false;
	}

	const static char* SECTION_NAME = "skin";
	if (inifile.SectionExists(SECTION_NAME) != FALSE) {
		// skin セクションがあるのでロードする

		// タイトル
		std::string s = inifile.GetValue("title", SECTION_NAME);
		if (!s.empty()) {
			strSkinTitle = s.c_str();
		}

		// メインビューのボディコントロール用背景画像ファイル名
		s = inifile.GetValue("main_body_imagefile", SECTION_NAME);
		if (!s.empty()) {
			strMainBodyCtrlImageFileName = s.c_str();
		}

		// メインビューのカテゴリコントロール用背景画像ファイル名
		s = inifile.GetValue("main_category_imagefile", SECTION_NAME);
		if (!s.empty()) {
			strMainCategoryCtrlImageFileName = s.c_str();
		}

		// レポートビューのリストコントロール用背景画像ファイル名
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
