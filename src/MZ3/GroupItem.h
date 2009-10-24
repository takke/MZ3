/*
 * Copyright (c) MZ3 Project Team All rights reserved.
 * Code licensed under the BSD License:
 * http://www.mz3.jp/license.txt
 */
#pragma once

// CGroupItem コマンド ターゲット

#include "MixiData.h"
#include "CategoryItem.h"

/**
 * メイン画面のグループリストに表示するための項目
 */
class CGroupItem 
{
public:
	CMixiData			mixi;				///< mixi データ
	CCategoryItemList	categories;			///< このグループ項目に関連づけられたカテゴリリスト群
	CString				name;				///< リストに表示する名称
	int					selectedCategory;	///< categories 内における選択項目のインデックス
	int					focusedCategory;	///< categories 内におけるフォーカス項目のインデックス
	bool				bSaveToGroupFile;	///< グループ定義ファイルに保存するか（永続化するか）

public:
	/// コンストラクタ
	CGroupItem()
		: selectedCategory(0)
		, focusedCategory(0)
		, bSaveToGroupFile(true)
	{}

	/// 初期化
	void init( LPCTSTR group_name, LPCTSTR url, ACCESS_TYPE accessType )
	{
		categories.clear();

		name = group_name;
		mixi.SetURL( url );
		mixi.SetAccessType( accessType );
	}

	/// フォーカスカテゴリの取得
	CCategoryItem* getFocusedCategory() {
		if( focusedCategory < 0 || focusedCategory >= (int)categories.size() ) {
			return NULL;
		}
		return &categories[ focusedCategory ];
	}

	/// 選択カテゴリの取得
	CCategoryItem* getSelectedCategory() {
		if( selectedCategory < 0 || selectedCategory >= (int)categories.size() ) {
			return NULL;
		}
		return &categories[ selectedCategory ];
	}

	virtual ~CGroupItem()					{}
};
