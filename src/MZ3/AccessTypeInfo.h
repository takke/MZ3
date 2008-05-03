/*
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */
#pragma once

#include "constants.h"
#include <map>

/**
 * アクセス種別に対するMZ3/4の振る舞いを管理するクラス
 */
class AccessTypeInfo
{
public:
	/// リクエスト種別
	enum REQUEST_METHOD {
		REQUEST_METHOD_INVALID = -1,	///< 未定義
		REQUEST_METHOD_GET = 0,			///< GET メソッド
		REQUEST_METHOD_POST = 1,		///< POST メソッド
	};

	/// 各アクセス種別の振る舞いを定義するデータ構造
	class Data
	{
	public:
		std::string		serviceType;	///< 対象とするサービスの種別（"mixi", "Twitter"など）
		std::wstring	shortText;		///< 説明文字列
		REQUEST_METHOD	requestType;	///< リクエスト種別
		bool			bCruiseTarget;	///< 巡回対象とするか？

		Data(const char* a_serviceType, const wchar_t* a_shortText, REQUEST_METHOD a_requestType, bool a_bCruiseTarget)
			: serviceType(a_serviceType)
			, shortText(a_shortText)
			, requestType(a_requestType)
			, bCruiseTarget(a_bCruiseTarget)
		{}
		Data()
			: serviceType("")
			, shortText(L"<unknown>")
			, requestType(REQUEST_METHOD_INVALID)
			, bCruiseTarget(false)
		{}
	};

	typedef std::map<ACCESS_TYPE, Data> MYMAP;
	MYMAP m_map;

public:
	bool init();

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

	/// 巡回対象とするか？（巡回予約可能か？）
	const bool isCruiseTarget( ACCESS_TYPE t ) {
		MYMAP::iterator it = m_map.find(t);
		if (it==m_map.end()) {
			return false;
		}
		return it->second.bCruiseTarget;
	}
};
