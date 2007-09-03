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
		CString msg;
		msg.Format( L"スキン設定ファイルがないため、初期値を利用します。skin-name[%s]", szSkinName );
		MZ3LOGGER_INFO( msg );
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

		// レポート画面、リストの文字色
		clrReportListText					= loadColorFromInifile( inifile, clrReportListText, 
																	"ReportListTextColor", SECTION_NAME );
		// メイン画面、カテゴリコントロールのアクティブ項目の文字色
		clrMainCategoryListActiveText		= loadColorFromInifile( inifile, clrMainCategoryListActiveText, 
																	"MainCategoryListActiveTextColor", SECTION_NAME );
		// メイン画面、カテゴリコントロールの非アクティブ項目の文字色
		clrMainCategoryListInactiveText		= loadColorFromInifile( inifile, clrMainCategoryListInactiveText, 
																	"MainCategoryListInactiveTextColor", SECTION_NAME );
		// メイン画面、ボディコントロールのデフォルト文字色
		clrMainBodyListDefaultText			= loadColorFromInifile( inifile, clrMainBodyListDefaultText, 
																	"MainBodyListDefaultTextColor", SECTION_NAME );
		// メイン画面、ボディコントロールの未読文字色
		clrMainBodyListNonreadText			= loadColorFromInifile( inifile, clrMainBodyListNonreadText, 
																	"MainBodyListNonreadTextColor", SECTION_NAME );
		// メイン画面、ボディコントロールの新着記事の文字色
		clrMainBodyListNewItemText			= loadColorFromInifile( inifile, clrMainBodyListNewItemText, 
																	"MainBodyListNewItemTextColor", SECTION_NAME );
		// メイン画面、ボディコントロールのあしあとのマイミクの文字色
		clrMainBodyListFootprintMyMixiText	= loadColorFromInifile( inifile, clrMainBodyListFootprintMyMixiText, 
																	"MainBodyListFootprintMyMixiTextColor", SECTION_NAME );
		// メイン画面、ボディコントロールの外部ブログの文字色
		clrMainBodyListExternalBlogText		= loadColorFromInifile( inifile, clrMainBodyListExternalBlogText, 
																	"MainBodyListExternalBlogTextColor", SECTION_NAME );
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

COLORREF CMZ3SkinInfo::loadColorFromInifile( inifile::IniFile& inifile, COLORREF defaultColor, LPCSTR key, LPCSTR section )
{
	std::string s = inifile.GetValue(key, section);
	if (!s.empty()) {
		// 値が存在するのでその値を使う
		// #xxxxxx 形式をパースする
		int r, g, b;
		if (sscanf( s.c_str(), "%02x%02x%02x", &r, &g, &b )==3) {
			// パース成功
			return RGB(r,g,b);
		} else {
			// パース失敗
			// デフォルト値を返す
			CString msg;
			msg.Format( L"スキンの色定義が間違っています。key[%s], value[%s]", 
				util::my_mbstowcs(key).c_str(), 
				util::my_mbstowcs(s).c_str() );
			MZ3LOGGER_ERROR( msg );
			return defaultColor;
		}
	} else {
		// 値が存在しないのでデフォルト値を返す
		return defaultColor;
	}
}
