/*
 * Copyright (c) MZ3 Project Team All rights reserved.
 * Code licensed under the BSD License:
 * http://www.mz3.jp/license.txt
 */
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
		setColorFromInifile( inifile, "ReportListText", "ReportListTextColor", SECTION_NAME );
		
		// メイン画面、カテゴリコントロールのアクティブ項目の文字色
		setColorFromInifile( inifile, "MainCategoryListActiveText", "MainCategoryListActiveTextColor", SECTION_NAME );
		
		// メイン画面、カテゴリコントロールの非アクティブ項目の文字色
		setColorFromInifile( inifile, "MainCategoryListInactiveText", "MainCategoryListInactiveTextColor", SECTION_NAME );
		
		// メイン画面、ボディコントロールのデフォルト文字色
		setColorFromInifile( inifile, "MainBodyListDefaultText", "MainBodyListDefaultTextColor", SECTION_NAME );
		
		// メイン画面、ボディコントロールの未読文字色
		setColorFromInifile( inifile, "MainBodyListNonreadText", "MainBodyListNonreadTextColor", SECTION_NAME );
		
		// メイン画面、ボディコントロールの新着記事の文字色
		setColorFromInifile( inifile, "MainBodyListNewItemText", "MainBodyListNewItemTextColor", SECTION_NAME );
		
		// メイン画面、ボディコントロールのあしあとのマイミクの文字色
		setColorFromInifile( inifile, "MainBodyListFootprintMyMixiText", "MainBodyListFootprintMyMixiTextColor", SECTION_NAME );
		
		// メイン画面、ボディコントロールの外部ブログの文字色
		setColorFromInifile( inifile, "MainBodyListExternalBlogText", "MainBodyListExternalBlogTextColor", SECTION_NAME );
		
		// メイン画面、ボディコントロールの日付区切り線
		setColorFromInifile( inifile, "MainBodyListDayBreakLine", "MainBodyListDayBreakLine", SECTION_NAME );
		
		// メイン画面、ボディコントロールの強調２
		setColorFromInifile( inifile, "MainBodyListEmphasis2", "MainBodyListEmphasis2", SECTION_NAME );
		
		// メイン画面、ボディコントロールの強調３
		setColorFromInifile( inifile, "MainBodyListEmphasis3", "MainBodyListEmphasis3", SECTION_NAME );
		
		// メイン画面、ボディコントロールの強調４
		setColorFromInifile( inifile, "MainBodyListEmphasis4", "MainBodyListEmphasis4", SECTION_NAME );
		
		// メイン画面、ステータスバーの背景色
		setColorFromInifile( inifile, "MainStatusBG", "MainStatusBG", SECTION_NAME);
		
		// メイン画面、ステータスバーの文字色
		setColorFromInifile( inifile, "MainStatusText", "MainStatusText", SECTION_NAME);
		
		// メイン画面、エディットコントロールの背景色
		setColorFromInifile( inifile, "MainEditBG", "MainEditBG", SECTION_NAME);
		
		// メイン画面、エディットコントロールの文字色
		setColorFromInifile( inifile, "MainEditText", "MainEditText", SECTION_NAME);

		// メイン画面、カテゴリコントロールの枠線有無
		bMainCategoryListBorder = loadBoolVar(inifile, bMainCategoryListBorder, "MainCategoryListBorder", SECTION_NAME);

		// メイン画面、ボディコントロールの枠線有無
		bMainBodyListBorder		= loadBoolVar(inifile, bMainBodyListBorder, "MainBodyListBorder", SECTION_NAME);

		// メイン画面、ステータスバーの枠線有無
		bMainStatusBorder		= loadBoolVar(inifile, bMainStatusBorder, "MainStatusBorder", SECTION_NAME);

		// メイン画面、エディットコントロールの枠線有無
		bMainEditBorder			= loadBoolVar(inifile, bMainEditBorder, "MainEditBorder", SECTION_NAME);
	}

	return true;
}


/**
 * 初期値の設定
 */
bool CMZ3SkinInfo::setDefaultInfo()
{
	strSkinTitle = strSkinName;										// スキンタイトル
	strMainBodyCtrlImageFileName		= L"body.jpg";				// メイン画面のボディコントロール用背景画像ファイル名
	strMainCategoryCtrlImageFileName	= L"header.jpg";			// メイン画面のカテゴリコントロール用背景画像ファイル名
	strReportListCtrlImageFileName		= L"report.jpg";			// レポート画面のリストコントロール用背景画像ファイル名

	colorMap.clear();
	setColor("ReportListText", 					RGB(0x00, 0x00, 0x00));	// レポート画面、リストの文字色
	setColor("MainCategoryListActiveText",		RGB(0xFF, 0x00, 0x00));	// メイン画面、カテゴリコントロールのアクティブ項目の文字色
	setColor("MainCategoryListInactiveText",	RGB(0x00, 0x00, 0x00));	// メイン画面、カテゴリコントロールの非アクティブ項目の文字色
	setColor("MainBodyListDefaultText",			RGB(0x00, 0x00, 0x00));	// メイン画面、ボディコントロールのデフォルト文字色
	setColor("MainBodyListNonreadText",			RGB(0x00, 0x00, 0xFF));	// メイン画面、ボディコントロールの未読文字色
	setColor("MainBodyListNewItemText",			RGB(0xFF, 0x00, 0x00));	// メイン画面、ボディコントロールの新着記事の文字色
	setColor("MainBodyListFootprintMyMixiText",	RGB(0x00, 0x00, 0xFF));	// メイン画面、ボディコントロールのあしあとのマイミクの文字色
	setColor("MainBodyListExternalBlogText",	RGB(0x80, 0x80, 0x80));	// メイン画面、ボディコントロールの外部ブログの文字色
	setColor("MainBodyListDayBreakLine",		RGB(0xC0, 0xC0, 0xFF));	// メイン画面、ボディコントロールの日付区切り線
	setColor("MainBodyListEmphasis2",			RGB(0xFF, 0x00, 0x00));	// メイン画面、ボディコントロールの強調２(デフォルト：赤)
	setColor("MainBodyListEmphasis3",			RGB(0x00, 0xA0, 0x00));	// メイン画面、ボディコントロールの強調３(デフォルト：緑)
	setColor("MainBodyListEmphasis4",			RGB(0xCC, 0x00, 0xCC));	// メイン画面、ボディコントロールの強調４(デフォルト：ピンク)

	setColor("MainStatusBG",					RGB(0xEE, 0xEE, 0xEE));	// メイン画面、ステータスバーの背景色
	setColor("MainStatusText",					RGB(0x00, 0x00, 0x00));	// メイン画面、ステータスバーの文字色
	setColor("MainEditBG",						RGB(0xFF, 0xFF, 0xFF));	// メイン画面、エディットコントロールの背景色
	setColor("MainEditText",					RGB(0x00, 0x00, 0x00));	// メイン画面、エディットコントロールの文字色

	bMainCategoryListBorder = true;			// メイン画面、カテゴリコントロールの枠線有無
	bMainBodyListBorder		= true;			// メイン画面、ボディコントロールの枠線有無
	bMainStatusBorder		= true;			// メイン画面、ステータスバーの枠線有無
	bMainEditBorder			= false;		// メイン画面、エディットコントロールの枠線有無

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

bool CMZ3SkinInfo::loadBoolVar(inifile::IniFile& inifile, bool defaultVar, LPCSTR key, LPCSTR section )
{
	std::string s = inifile.GetValue(key, section);
	if (!s.empty()) {
		// 値が存在するのでその値を使う
		int n = atoi(s.c_str());
		if (n) {
			return true;
		} else {
			return false;
		}
	} else {
		// 値が存在しないのでデフォルト値を返す
		return defaultVar;
	}
}

