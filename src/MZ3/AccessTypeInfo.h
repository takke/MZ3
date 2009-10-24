/*
 * Copyright (c) MZ3 Project Team All rights reserved.
 * Code licensed under the BSD License:
 * http://www.mz3.jp/license.txt
 */
#pragma once

#include "constants.h"
#include <map>
#include <vector>
#include <string>

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
	LPCTSTR info_type_to_text(INFO_TYPE e) {
		switch (e) {
		case INFO_TYPE_INVALID:		return L"未定義";
		case INFO_TYPE_GROUP:		return L"グループ系";
		case INFO_TYPE_CATEGORY:	return L"カテゴリ系";
		case INFO_TYPE_BODY:		return L"ボディ項目系";
		case INFO_TYPE_POST:		return L"POST系";
		case INFO_TYPE_OTHER:
		default:					return L"その他";
		}
	}
	INFO_TYPE text_to_info_type(const CString& s) {
		if (s==L"未定義")		return INFO_TYPE_INVALID;
		if (s==L"グループ系")	return INFO_TYPE_GROUP;
		if (s==L"カテゴリ系")	return INFO_TYPE_CATEGORY;
		if (s==L"ボディ項目系") return INFO_TYPE_BODY;		
		if (s==L"POST系")		return INFO_TYPE_POST;
		return INFO_TYPE_OTHER;
	}

	/// リクエスト種別
	enum REQUEST_METHOD {
		REQUEST_METHOD_INVALID = -1,	///< 未定義
		REQUEST_METHOD_GET = 0,			///< GET メソッド
		REQUEST_METHOD_POST = 1,		///< POST メソッド
	};
	LPCTSTR request_method_to_text(REQUEST_METHOD e) {
		switch (e) {
		case REQUEST_METHOD_GET:		return L"GET";
		case REQUEST_METHOD_POST:		return L"POST";
		case REQUEST_METHOD_INVALID:
		default:						return L"未定義";
		}
	}
	REQUEST_METHOD text_to_request_method(const CString& s) {
		if (s==L"GET")	return REQUEST_METHOD_GET;
		if (s==L"POST")	return REQUEST_METHOD_POST;
		return REQUEST_METHOD_INVALID;
	}

	/// エンコーディング
	enum ENCODING {
		ENCODING_SJIS = 0,
		ENCODING_EUC  = 1,
		ENCODING_UTF8 = 2,
		ENCODING_NOCONVERSION = 3,
	};
	LPCTSTR encoding_to_text(ENCODING e) {
		switch (e) {
		case ENCODING_SJIS:			return L"SJIS";
		case ENCODING_EUC:			return L"EUC";
		case ENCODING_UTF8:			return L"UTF8";
		case ENCODING_NOCONVERSION:
		default:					return L"NOCONVERSION";
		}
	}
	ENCODING text_to_encoding(const CString& s) {
		if (s==L"SJIS")	return ENCODING_SJIS;
		if (s==L"EUC")	return ENCODING_EUC;
		if (s==L"UTF8")	return ENCODING_UTF8;
		return ENCODING_NOCONVERSION;
	}

	/// ボディに CMixiData 内のどの項目を表示するかの識別子
	enum BODY_INDICATE_TYPE
	{
		BODY_INDICATE_TYPE_DATE,		///< 日付を表示する
		BODY_INDICATE_TYPE_TITLE,		///< タイトルを表示する
		BODY_INDICATE_TYPE_NAME,		///< 名前を表示する
		BODY_INDICATE_TYPE_BODY,		///< 本文を表示する
		BODY_INDICATE_TYPE_NONE,		///< 何も表示しない
	};
	LPCTSTR body_indicate_type_to_text(BODY_INDICATE_TYPE e) {
		switch (e) {
		case BODY_INDICATE_TYPE_DATE:	return L"日付";
		case BODY_INDICATE_TYPE_TITLE:	return L"タイトル";
		case BODY_INDICATE_TYPE_NAME:	return L"名前";
		case BODY_INDICATE_TYPE_BODY:	return L"本文";
		case BODY_INDICATE_TYPE_NONE:
		default:						return L"";
		}
	}

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
		REQUEST_METHOD		requestMethod;		///< リクエスト種別

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
		BodyHeaderColumn	bodyHeaderCol2;		///< ボディリストのヘッダー2のカラム
		BodyHeaderColumn	bodyHeaderCol3;		///< ボディリストのヘッダー3のカラム
		std::wstring		bodyIntegratedLinePattern1;		///< 統合カラムモード、1行目のパターン(%n でカラム要素を指定する)
		std::wstring		bodyIntegratedLinePattern2;		///< 統合カラムモード、2行目のパターン(%n でカラム要素を指定する)

//		std::wstring		refererUrlPattern;	///< リファラURLのパターン。

		Data(INFO_TYPE a_infoType, const char* a_serviceType, const wchar_t* a_shortText, REQUEST_METHOD a_requestMethod)
			: infoType(a_infoType)
			, serviceType(a_serviceType)
			, shortText(a_shortText)
			, requestMethod(a_requestMethod)
			, requestEncoding(ENCODING_EUC)
			, cacheFilePattern(L"")
			, serializeKey("")
			, bCruiseTarget(false)
			, defaultCategoryURL(L"")
			, bodyIntegratedLinePattern1(L"%1")
			, bodyIntegratedLinePattern2(L"%2 | %3")
//			, refererUrlPattern(L"")
		{}
		Data()
			: infoType(INFO_TYPE_INVALID)
			, serviceType("")
			, shortText(L"<unknown>")
			, requestMethod(REQUEST_METHOD_INVALID)
			, requestEncoding(ENCODING_EUC)
			, cacheFilePattern(L"")
			, serializeKey("")
			, bCruiseTarget(false)
			, defaultCategoryURL(L"")
			, bodyIntegratedLinePattern1(L"%1")
			, bodyIntegratedLinePattern2(L"%2 | %3")
//			, refererUrlPattern(L"")
		{}
	};

	// アクセス種別 → データマップ
	typedef std::map<ACCESS_TYPE, Data> ACCESS_TYPE_TO_DATA_MAP;
	ACCESS_TYPE_TO_DATA_MAP m_map;

	// シリアライズキー → アクセス種別マップ
	typedef std::map<std::string, ACCESS_TYPE> SERIALIZE_KEY_TO_ACCESS_TYPE_MAP;
	SERIALIZE_KEY_TO_ACCESS_TYPE_MAP m_serializeKeyToAccessKeyMap;

public:
	bool init();

	/// シリアライズキーからアクセス種別を取得する
	ACCESS_TYPE getAccessTypeBySerializeKey(const std::string& key) {
		SERIALIZE_KEY_TO_ACCESS_TYPE_MAP::iterator it = m_serializeKeyToAccessKeyMap.find(key);
		if (it==m_serializeKeyToAccessKeyMap.end()) {
			return ACCESS_INVALID;
		}
		return it->second;
	}

	/// カテゴリ系種別一覧の生成
	std::vector<ACCESS_TYPE> getCategoryTypeList() {
		std::vector<ACCESS_TYPE> types;
		for (ACCESS_TYPE_TO_DATA_MAP::iterator it=m_map.begin(); it!=m_map.end(); it++) {
			if (it->second.infoType==INFO_TYPE_CATEGORY) {
				types.push_back(it->first);
			}
		}
		return types;
	}

	/// データ種別
	INFO_TYPE getInfoType( ACCESS_TYPE t ) {
		ACCESS_TYPE_TO_DATA_MAP::iterator it = m_map.find(t);
		if (it==m_map.end()) {
			return INFO_TYPE_INVALID;
		}
		return it->second.infoType;
	}

	/// サービス種別の取得
	std::string getServiceType( ACCESS_TYPE t ) {
		ACCESS_TYPE_TO_DATA_MAP::iterator it = m_map.find(t);
		if (it==m_map.end()) {
			return "";
		}
		return it->second.serviceType.c_str();
	}

	/// リクエスト種別の取得
	REQUEST_METHOD getRequestMethod( ACCESS_TYPE t ) {
		ACCESS_TYPE_TO_DATA_MAP::iterator it = m_map.find(t);
		if (it==m_map.end()) {
			return REQUEST_METHOD_INVALID;
		}
		return it->second.requestMethod;
	}

	/// 説明文字列の取得
	const wchar_t* getShortText( ACCESS_TYPE t ) {
		ACCESS_TYPE_TO_DATA_MAP::iterator it = m_map.find(t);
		if (it==m_map.end()) {
			return L"<unknown>";
		}
		return it->second.shortText.c_str();
	}

	/// シリアライズキーの取得
	const char* getSerializeKey( ACCESS_TYPE t ) {
		ACCESS_TYPE_TO_DATA_MAP::iterator it = m_map.find(t);
		if (it==m_map.end()) {
			return "";
		}
		return it->second.serializeKey.c_str();
	}

	/// 巡回対象とするか？（巡回予約可能か？）
	const bool isCruiseTarget( ACCESS_TYPE t ) {
		ACCESS_TYPE_TO_DATA_MAP::iterator it = m_map.find(t);
		if (it==m_map.end()) {
			return false;
		}
		return it->second.bCruiseTarget;
	}

	/// ボディリストのヘッダー1のカラム名
	const wchar_t* getBodyHeaderCol1Name( ACCESS_TYPE t ) {
		ACCESS_TYPE_TO_DATA_MAP::iterator it = m_map.find(t);
		if (it==m_map.end()) {
			return L"";
		}
		return it->second.bodyHeaderCol1.title.c_str();
	}

	/// ボディリストのヘッダー2のカラム名
	const wchar_t* getBodyHeaderCol2Name( ACCESS_TYPE t ) {
		ACCESS_TYPE_TO_DATA_MAP::iterator it = m_map.find(t);
		if (it==m_map.end()) {
			return L"";
		}
		return it->second.bodyHeaderCol2.title.c_str();
	}

	/// ボディリストのヘッダー3のカラム名
	const wchar_t* getBodyHeaderCol3Name( ACCESS_TYPE t ) {
		ACCESS_TYPE_TO_DATA_MAP::iterator it = m_map.find(t);
		if (it==m_map.end()) {
			return L"";
		}
		return it->second.bodyHeaderCol3.title.c_str();
	}

	/// ボディリストのヘッダー1のカラム種別
	BODY_INDICATE_TYPE getBodyHeaderCol1Type( ACCESS_TYPE t ) {
		ACCESS_TYPE_TO_DATA_MAP::iterator it = m_map.find(t);
		if (it==m_map.end()) {
			return BODY_INDICATE_TYPE_NONE;
		}
		return it->second.bodyHeaderCol1.type;
	}

	/// ボディリストのヘッダー2のカラム種別
	BODY_INDICATE_TYPE getBodyHeaderCol2Type( ACCESS_TYPE t ) {
		ACCESS_TYPE_TO_DATA_MAP::iterator it = m_map.find(t);
		if (it==m_map.end()) {
			return BODY_INDICATE_TYPE_NONE;
		}
		return it->second.bodyHeaderCol2.type;
	}

	/// ボディリストのヘッダー3のカラム種別
	BODY_INDICATE_TYPE getBodyHeaderCol3Type( ACCESS_TYPE t ) {
		ACCESS_TYPE_TO_DATA_MAP::iterator it = m_map.find(t);
		if (it==m_map.end()) {
			return BODY_INDICATE_TYPE_NONE;
		}
		return it->second.bodyHeaderCol3.type;
	}

	/// 統合カラムモード、1行目のパターン
	const wchar_t* getBodyIntegratedLinePattern1( ACCESS_TYPE t ) {
		ACCESS_TYPE_TO_DATA_MAP::iterator it = m_map.find(t);
		if (it==m_map.end()) {
			return L"";
		}
		return it->second.bodyIntegratedLinePattern1.c_str();
	}

	/// 統合カラムモード、2行目のパターン
	const wchar_t* getBodyIntegratedLinePattern2( ACCESS_TYPE t ) {
		ACCESS_TYPE_TO_DATA_MAP::iterator it = m_map.find(t);
		if (it==m_map.end()) {
			return L"";
		}
		return it->second.bodyIntegratedLinePattern2.c_str();
	}

	/// デフォルトURL
	const wchar_t* getDefaultCategoryURL( ACCESS_TYPE t ) {
		ACCESS_TYPE_TO_DATA_MAP::iterator it = m_map.find(t);
		if (it==m_map.end()) {
			return L"";
		}
		return it->second.defaultCategoryURL.c_str();
	}

	/// エンコーディング
	ENCODING getRequestEncoding( ACCESS_TYPE t ) {
		ACCESS_TYPE_TO_DATA_MAP::iterator it = m_map.find(t);
		if (it==m_map.end()) {
			return ENCODING_EUC;
		}
		return it->second.requestEncoding;
	}

	/// キャッシュファイル名のパターン
	const wchar_t* getCacheFilePattern( ACCESS_TYPE t ) {
		ACCESS_TYPE_TO_DATA_MAP::iterator it = m_map.find(t);
		if (it==m_map.end()) {
			return L"";
		}
		return it->second.cacheFilePattern.c_str();
	}

	/// refererの取得
/*	const wchar_t* getRefererUrlPattern( ACCESS_TYPE t ) {
		ACCESS_TYPE_TO_DATA_MAP::iterator it = m_map.find(t);
		if (it==m_map.end()) {
			return NULL;
		}
		return it->second.refererUrl.c_str();
	}
*/
};
