/*
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */
#pragma once

#include "constants.h"
#include <map>
#include <vector>

/**
 * アクセス種別に対するMZ3/4の振る舞いを管理するクラス
 */
class AccessTypeInfo
{
public:
	/// データ自体の種別
	enum INFO_TYPE {
		INFO_TYPE_INVALID = -1,			///< 未定義
		INFO_TYPE_GROUP,				///< グループ系
		INFO_TYPE_CATEGORY,				///< カテゴリ系
		INFO_TYPE_BODY,					///< ボディ項目系
		INFO_TYPE_POST,					///< POST系
		INFO_TYPE_OTHER,				///< その他
	};

	/// リクエスト種別
	enum REQUEST_METHOD {
		REQUEST_METHOD_INVALID = -1,	///< 未定義
		REQUEST_METHOD_GET = 0,			///< GET メソッド
		REQUEST_METHOD_POST = 1,		///< POST メソッド
	};

	/// ボディに CMixiData 内のどの項目を表示するかの識別子
	enum BODY_INDICATE_TYPE
	{
		BODY_INDICATE_TYPE_DATE,			///< 日付を表示する
		BODY_INDICATE_TYPE_TITLE,			///< タイトルを表示する
		BODY_INDICATE_TYPE_NAME,			///< 名前を表示する
		BODY_INDICATE_TYPE_BODY,			///< 本文を表示する
		BODY_INDICATE_TYPE_NONE,			///< 何も表示しない
	};

	/// 各アクセス種別の振る舞いを定義するデータ構造
	class Data
	{
	public:
		INFO_TYPE			infoType;			///< データ自体の種別（何に利用されるアクセス種別か？）
		std::string			serviceType;		///< 対象とするサービスの種別（"mixi", "Twitter"など）
		std::wstring		shortText;			///< 説明文字列
		REQUEST_METHOD		requestType;		///< リクエスト種別

		//--- グループ系、カテゴリ系のみが持つ項目
		std::string			serializeKey;		///< Mz3GroupData を ini ファイルにシリアライズする際のキー

		//--- カテゴリ系のみが持つ項目
		bool				bCruiseTarget;		///< 巡回対象とするか？
		std::wstring		defaultCategoryURL;	///< カテゴリのURL
		std::wstring		bodyHeaderCol1Name;	///< ボディリストのヘッダー1のカラム名
		std::wstring		bodyHeaderCol2NameA;///< ボディリストのヘッダー2のカラム名A
		std::wstring		bodyHeaderCol2NameB;///< ボディリストのヘッダー2のカラム名B
		BODY_INDICATE_TYPE	bodyHeaderCol1Type;	///< ボディリストのヘッダー1のカラム種別
		BODY_INDICATE_TYPE	bodyHeaderCol2TypeA;///< ボディリストのヘッダー2のカラム種別A（Bとトグル）
		BODY_INDICATE_TYPE	bodyHeaderCol2TypeB;///< ボディリストのヘッダー2のカラム種別B（Aとトグル）

		Data(INFO_TYPE a_infoType, const char* a_serviceType, const wchar_t* a_shortText, REQUEST_METHOD a_requestType)
			: infoType(a_infoType)
			, serviceType(a_serviceType)
			, shortText(a_shortText)
			, requestType(a_requestType)
			, serializeKey("")
			, bCruiseTarget(false)
			, defaultCategoryURL(L"")
			, bodyHeaderCol1Name(L"")
			, bodyHeaderCol2NameA(L"")
			, bodyHeaderCol2NameB(L"")
			, bodyHeaderCol1Type(BODY_INDICATE_TYPE_NONE)
			, bodyHeaderCol2TypeA(BODY_INDICATE_TYPE_NONE)
			, bodyHeaderCol2TypeB(BODY_INDICATE_TYPE_NONE)
		{}
		Data()
			: infoType(INFO_TYPE_INVALID)
			, serviceType("")
			, shortText(L"<unknown>")
			, requestType(REQUEST_METHOD_INVALID)
			, serializeKey("")
			, bCruiseTarget(false)
			, defaultCategoryURL(L"")
			, bodyHeaderCol1Name(L"")
			, bodyHeaderCol2NameA(L"")
			, bodyHeaderCol2NameB(L"")
			, bodyHeaderCol1Type(BODY_INDICATE_TYPE_NONE)
			, bodyHeaderCol2TypeA(BODY_INDICATE_TYPE_NONE)
			, bodyHeaderCol2TypeB(BODY_INDICATE_TYPE_NONE)
		{}
	};

	typedef std::map<ACCESS_TYPE, Data> MYMAP;
	MYMAP m_map;

public:
	bool init();

	/// カテゴリ系種別一覧の生成
	std::vector<ACCESS_TYPE> getCategoryTypeList() {
		std::vector<ACCESS_TYPE> types;
		for (MYMAP::iterator it=m_map.begin(); it!=m_map.end(); it++) {
			if (it->second.infoType==INFO_TYPE_CATEGORY) {
				types.push_back(it->first);
			}
		}
		return types;
	}

	/// データ種別
	INFO_TYPE getInfoType( ACCESS_TYPE t ) {
		MYMAP::iterator it = m_map.find(t);
		if (it==m_map.end()) {
			return INFO_TYPE_INVALID;
		}
		return it->second.infoType;
	}

	/// サービス種別の取得
	const char* getServiceType( ACCESS_TYPE t ) {
		MYMAP::iterator it = m_map.find(t);
		if (it==m_map.end()) {
			return "";
		}
		return it->second.serviceType.c_str();
	}

	/// リクエスト種別の取得
	REQUEST_METHOD getRequestMethod( ACCESS_TYPE t ) {
		MYMAP::iterator it = m_map.find(t);
		if (it==m_map.end()) {
			return REQUEST_METHOD_INVALID;
		}
		return it->second.requestType;
	}

	/// 説明文字列の取得
	const wchar_t* getShortText( ACCESS_TYPE t ) {
		MYMAP::iterator it = m_map.find(t);
		if (it==m_map.end()) {
			return L"<unknown>";
		}
		return it->second.shortText.c_str();
	}

	/// シリアライズキーの取得
	const char* getSerializeKey( ACCESS_TYPE t ) {
		MYMAP::iterator it = m_map.find(t);
		if (it==m_map.end()) {
			return "";
		}
		return it->second.serializeKey.c_str();
	}

	/// 巡回対象とするか？（巡回予約可能か？）
	const bool isCruiseTarget( ACCESS_TYPE t ) {
		MYMAP::iterator it = m_map.find(t);
		if (it==m_map.end()) {
			return false;
		}
		return it->second.bCruiseTarget;
	}

	/// ボディリストのヘッダー1のカラム名
	const wchar_t* getBodyHeaderCol1Name( ACCESS_TYPE t ) {
		MYMAP::iterator it = m_map.find(t);
		if (it==m_map.end()) {
			return L"";
		}
		return it->second.bodyHeaderCol1Name.c_str();
	}

	/// ボディリストのヘッダー2のカラム名A
	const wchar_t* getBodyHeaderCol2NameA( ACCESS_TYPE t ) {
		MYMAP::iterator it = m_map.find(t);
		if (it==m_map.end()) {
			return L"";
		}
		return it->second.bodyHeaderCol2NameA.c_str();
	}

	/// ボディリストのヘッダー2のカラム名B
	const wchar_t* getBodyHeaderCol2NameB( ACCESS_TYPE t ) {
		MYMAP::iterator it = m_map.find(t);
		if (it==m_map.end()) {
			return L"";
		}
		return it->second.bodyHeaderCol2NameB.c_str();
	}

	/// ボディリストのヘッダー1のカラム種別
	BODY_INDICATE_TYPE getBodyHeaderCol1Type( ACCESS_TYPE t ) {
		MYMAP::iterator it = m_map.find(t);
		if (it==m_map.end()) {
			return BODY_INDICATE_TYPE_NONE;
		}
		return it->second.bodyHeaderCol1Type;
	}

	/// ボディリストのヘッダー2のカラム種別A
	BODY_INDICATE_TYPE getBodyHeaderCol2TypeA( ACCESS_TYPE t ) {
		MYMAP::iterator it = m_map.find(t);
		if (it==m_map.end()) {
			return BODY_INDICATE_TYPE_NONE;
		}
		return it->second.bodyHeaderCol2TypeA;
	}

	/// ボディリストのヘッダー2のカラム種別B
	BODY_INDICATE_TYPE getBodyHeaderCol2TypeB( ACCESS_TYPE t ) {
		MYMAP::iterator it = m_map.find(t);
		if (it==m_map.end()) {
			return BODY_INDICATE_TYPE_NONE;
		}
		return it->second.bodyHeaderCol2TypeB;
	}

	/// デフォルトURL
	const wchar_t* getDefaultCategoryURL( ACCESS_TYPE t ) {
		MYMAP::iterator it = m_map.find(t);
		if (it==m_map.end()) {
			return L"";
		}
		return it->second.defaultCategoryURL.c_str();
	}

};
