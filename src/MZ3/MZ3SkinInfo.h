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
	CString strMainBodyCtrlImageFileName;		///< メインビューのボディコントロール用背景画像ファイル名
	CString strMainCategoryCtrlImageFileName;	///< メインビューのカテゴリコントロール用背景画像ファイル名
	CString strReportListCtrlImageFileName;		///< レポートビューのリストコントロール用背景画像ファイル名

	CMZ3SkinInfo()
	{
		strSkinName = L"default";

		setDefaultInfo();
	}

	bool loadSkinInifile( LPCTSTR szSkinName );

	/**
	 * 初期値の設定
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

