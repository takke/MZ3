/*
 * Copyright (c) MZ3 Project Team All rights reserved.
 * Code licensed under the BSD License:
 * http://www.mz3.jp/license.txt
 */
#pragma once

#include "GroupItem.h"

/// MZ3 のグループビューと1対1で対応するデータ構造
class Mz3GroupData {
public:
	std::vector<CGroupItem>	groups;				///< カテゴリリストの項目群
	std::string				services;			///< サポートするサービス種別のスペース区切り文字列

public:
	/// コンストラクタ
	Mz3GroupData() {
	}

	struct InitializeType
	{
		std::string strSelectedServices;

		InitializeType()
		{
		}
	};

public:
	/// groups から指定されたリスト種別の項目を探し、その項目を返す。
	/// 見つからなければ NULL を返す。
	CCategoryItem* GetCategoryItem( ACCESS_TYPE accessType )
	{
		for( size_t ig=0; ig<groups.size(); ig++ ) {
			CCategoryItemList& categories = groups[ig].categories;
			for( size_t i=0; i<categories.size(); i++ ) {
				CCategoryItem& item = categories[i];
				if( item.m_mixi.GetAccessType() == accessType ) {
					return &item;
				}
			}
		}
		return NULL;
	}

	/// groups からブックマーク項目を探し、見つかればその項目を返す。
	/// 見つからなければ例外を投げる
	CMixiDataList&	GetBookmarkList() {
		CCategoryItem* pCategoryItem = GetCategoryItem( ACCESS_LIST_BOOKMARK );
		if( pCategoryItem == NULL ) {
			throw L"Not Found the Requested Bookmark...";
		}else{
			return pCategoryItem->GetBodyList();
		}
	}

	bool initForTopPage(AccessTypeInfo& accessTypeInfo, const InitializeType initType);

	static bool appendCategoryByIniData( AccessTypeInfo& accessTypeInfo, 
								  CGroupItem& group, 
								  std::string category_name, 
								  ACCESS_TYPE category_type, 
								  const char* category_url=NULL, 
								  bool bCruise=false );
};

/// Mz3GroupData とグループ定義ファイルとのデータ交換用データ構造
class Mz3GroupDataInifileHelper {

private:
	/// カテゴリ種別文字列 → カテゴリ種別 マップ
	CMap<CString,LPCTSTR,ACCESS_TYPE,ACCESS_TYPE> category_string2type;

public:
	/**
	 * コンストラクタ。
	 *
	 * map の初期化を行う。
	 */
	Mz3GroupDataInifileHelper(AccessTypeInfo& accessTypeInfo) {
		InitMap(accessTypeInfo);
	}

private:
	void InitMap(AccessTypeInfo& accessTypeInfo);

public:

	/**
	 * カテゴリ種別文字列 → カテゴリ種別 変換
	 */
	ACCESS_TYPE CategoryString2Type( LPCTSTR category_string ) {
		// 要素がなければ ACCESS_INVALID を返す。
		ACCESS_TYPE value;
		if( category_string2type.Lookup( category_string, value ) == FALSE ) {
			return ACCESS_INVALID;
		}

		// マップから種別変換
		return value;
	}
};


/// Mz3GroupData をグループ定義ファイルに出力するクラス
class Mz3GroupDataWriter {
public:
	static bool save( AccessTypeInfo& accessTypeInfo, const Mz3GroupData& target, const CString& inifilename );
};

/// Mz3GroupData をグループ定義ファイルから構築するクラス
class Mz3GroupDataReader {
public:
	static bool load( AccessTypeInfo& accessTypeInfo, Mz3GroupData& target, const CString& inifilename );
};

