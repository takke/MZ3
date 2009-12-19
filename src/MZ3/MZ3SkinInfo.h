/*
 * Copyright (c) MZ3 Project Team All rights reserved.
 * Code licensed under the BSD License:
 * http://www.mz3.jp/license.txt
 */
#pragma once

/**
 * スキン情報
 *
 * skin.ini に1対1で対応するモデル
 */
class CMZ3SkinInfo
{
public:
	CString strSkinName;		///< スキン名=スキンフォルダ

	CString strSkinTitle;						///< スキンタイトル
	CString strMainBodyCtrlImageFileName;		///< メイン画面のボディコントロール用背景画像ファイル名
	CString strMainCategoryCtrlImageFileName;	///< メイン画面のカテゴリコントロール用背景画像ファイル名
	CString strReportListCtrlImageFileName;		///< レポート画面のリストコントロール用背景画像ファイル名

	std::map<std::string, COLORREF> colorMap;	///< 各種カラーのマップ(カラー名 => カラー)

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

	bool	 bMainCategoryListBorder;			///< メイン画面、カテゴリコントロールの枠線有無
	bool	 bMainBodyListBorder;				///< メイン画面、ボディコントロールの枠線有無
	bool	 bMainStatusBorder;					///< メイン画面、ステータスバーの枠線有無
	bool	 bMainEditBorder;					///< メイン画面、エディットコントロールの枠線有無

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

