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

	COLORREF clrReportListText;					///< レポート画面、リストの文字色
	COLORREF clrMainCategoryListActiveText;		///< メイン画面、カテゴリコントロールのアクティブ項目の文字色
	COLORREF clrMainCategoryListInactiveText;	///< メイン画面、カテゴリコントロールの非アクティブ項目の文字色
	COLORREF clrMainBodyListDefaultText;		///< メイン画面、ボディコントロールのデフォルト文字色
	COLORREF clrMainBodyListNonreadText;		///< メイン画面、ボディコントロールの未読文字色
	COLORREF clrMainBodyListNewItemText;		///< メイン画面、ボディコントロールの新着記事の文字色
	COLORREF clrMainBodyListFootprintMyMixiText;///< メイン画面、ボディコントロールのあしあとのマイミクの文字色
	COLORREF clrMainBodyListExternalBlogText;	///< メイン画面、ボディコントロールの外部ブログの文字色
	COLORREF clrMainBodyListDayBreakLine;		///< メイン画面、ボディコントロールの日付区切り線
	COLORREF clrMainBodyListEmphasis2;			///< メイン画面、ボディコントロールの強調２(デフォルト：赤)
	COLORREF clrMainBodyListEmphasis3;			///< メイン画面、ボディコントロールの強調３(デフォルト：緑)

	COLORREF clrMainStatusBG;					///< メイン画面、ステータスバーの背景色
	COLORREF clrMainStatusText;					///< メイン画面、ステータスバーの文字色
	COLORREF clrMainEditBG;						///< メイン画面、エディットコントロールの背景色
	COLORREF clrMainEditText;					///< メイン画面、エディットコントロールの文字色

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
	COLORREF loadColorFromInifile( inifile::IniFile& inifile, COLORREF defaultColor, LPCSTR key, LPCSTR section );
	bool loadBoolVar(inifile::IniFile& inifile, bool defaultVar, LPCSTR key, LPCSTR section );
};

