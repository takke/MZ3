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
		BODY_INDICATE_TYPE_DATE,		///< 日付を表示する
		BODY_INDICATE_TYPE_TITLE,		///< タイトルを表示する
		BODY_INDICATE_TYPE_NAME,		///< 名前を表示する
		BODY_INDICATE_TYPE_BODY,		///< 本文を表示する
		BODY_INDICATE_TYPE_NONE,		///< 何も表示しない
	};

	enum ENCODING {
		ENCODING_SJIS = 0,
		ENCODING_EUC  = 1,
		ENCODING_UTF8 = 2,
	};

	/// ボディのカラム用データ
	class BodyHeaderColumn {
	public:
		BODY_INDICATE_TYPE type;		///< どの項目を表示するか
		std::wstring	   title;		///< カラムに表示する文字列

		/// コンストラクタ
		BodyHeaderColumn() : type(BODY_INDICATE_TYPE_NONE), title(L"") {}
		/// コンストラクタ
		BodyHeaderColumn(BODY_INDICATE_TYPE a_type, const wchar_t* a_title) : type(a_type), title(a_title) {}
	};

	/// 各アクセス種別の振る舞いを定義するデータ構造
	class Data
	{
	public:
		INFO_TYPE			infoType;			///< データ自体の種別（何に利用されるアクセス種別か？）
		std::string			serviceType;		///< 対象とするサービスの種別（"mixi", "Twitter"など）
		std::wstring		shortText;			///< 説明文字列
		REQUEST_METHOD		requestType;		///< リクエスト種別

		ENCODING			requestEncoding;	///< 取得時のエンコーディングタイプ

		std::wstring		cacheFilePattern;	///< キャッシュファイル名のパターン。
												///< MakeLogfilePath により、下記のパターンが使用可能。
												///< "{urlparam:パラメータ名}"
												///< "{urlafter:TargetURL[:default_path]}"

		//--- グループ系、カテゴリ系のみが持つ項目
		std::string			serializeKey;		///< Mz3GroupData を ini ファイルにシリアライズする際のキー

		//--- カテゴリ系のみが持つ項目
		bool				bCruiseTarget;		///< 巡回対象とするか？
		std::wstring		defaultCategoryURL;	///< カテゴリのURL
		BodyHeaderColumn	bodyHeaderCol1;		///< ボディリストのヘッダー1のカラム
		BodyHeaderColumn	bodyHeaderCol2A;	///< ボディリストのヘッダー2のカラムA
		BodyHeaderColumn	bodyHeaderCol2B;	///< ボディリストのヘッダー2のカラムB

		Data(INFO_TYPE a_infoType, const char* a_serviceType, const wchar_t* a_shortText, REQUEST_METHOD a_requestType)
			: infoType(a_infoType)
			, serviceType(a_serviceType)
			, shortText(a_shortText)
			, requestType(a_requestType)
			, requestEncoding(ENCODING_EUC)
			, cacheFilePattern(L"")
			, serializeKey("")
			, bCruiseTarget(false)
			, defaultCategoryURL(L"")
		{}
		Data()
			: infoType(INFO_TYPE_INVALID)
			, serviceType("")
			, shortText(L"<unknown>")
			, requestType(REQUEST_METHOD_INVALID)
			, requestEncoding(ENCODING_EUC)
			, cacheFilePattern(L"")
			, serializeKey("")
			, bCruiseTarget(false)
			, defaultCategoryURL(L"")
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
		return it->second.bodyHeaderCol1.title.c_str();
	}

	/// ボディリストのヘッダー2のカラム名A
	const wchar_t* getBodyHeaderCol2NameA( ACCESS_TYPE t ) {
		MYMAP::iterator it = m_map.find(t);
		if (it==m_map.end()) {
			return L"";
		}
		return it->second.bodyHeaderCol2A.title.c_str();
	}

	/// ボディリストのヘッダー2のカラム名B
	const wchar_t* getBodyHeaderCol2NameB( ACCESS_TYPE t ) {
		MYMAP::iterator it = m_map.find(t);
		if (it==m_map.end()) {
			return L"";
		}
		return it->second.bodyHeaderCol2B.title.c_str();
	}

	/// ボディリストのヘッダー1のカラム種別
	BODY_INDICATE_TYPE getBodyHeaderCol1Type( ACCESS_TYPE t ) {
		MYMAP::iterator it = m_map.find(t);
		if (it==m_map.end()) {
			return BODY_INDICATE_TYPE_NONE;
		}
		return it->second.bodyHeaderCol1.type;
	}

	/// ボディリストのヘッダー2のカラム種別A
	BODY_INDICATE_TYPE getBodyHeaderCol2TypeA( ACCESS_TYPE t ) {
		MYMAP::iterator it = m_map.find(t);
		if (it==m_map.end()) {
			return BODY_INDICATE_TYPE_NONE;
		}
		return it->second.bodyHeaderCol2A.type;
	}

	/// ボディリストのヘッダー2のカラム種別B
	BODY_INDICATE_TYPE getBodyHeaderCol2TypeB( ACCESS_TYPE t ) {
		MYMAP::iterator it = m_map.find(t);
		if (it==m_map.end()) {
			return BODY_INDICATE_TYPE_NONE;
		}
		return it->second.bodyHeaderCol2B.type;
	}

	/// デフォルトURL
	const wchar_t* getDefaultCategoryURL( ACCESS_TYPE t ) {
		MYMAP::iterator it = m_map.find(t);
		if (it==m_map.end()) {
			return L"";
		}
		return it->second.defaultCategoryURL.c_str();
	}

	/// エンコーディング
	ENCODING getRequestEncoding( ACCESS_TYPE t ) {
		MYMAP::iterator it = m_map.find(t);
		if (it==m_map.end()) {
			return ENCODING_EUC;
		}
		return it->second.requestEncoding;
	}

	/// キャッシュファイル名のパターン
	const wchar_t* getCacheFilePattern( ACCESS_TYPE t ) {
		MYMAP::iterator it = m_map.find(t);
		if (it==m_map.end()) {
			return L"";
		}
		return it->second.cacheFilePattern.c_str();
	}

};
