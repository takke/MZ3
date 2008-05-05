/*
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */
#pragma once

// CCategoryItem コマンド ターゲット

#include "MixiData.h"
#include "AccessTypeInfo.h"

class CCategoryItem;
typedef std::vector<CCategoryItem> CCategoryItemList;

/**
 * メイン画面のカテゴリリストに表示するための項目
 */
class CCategoryItem 
{
public:
	CMixiData		m_mixi;					///< mixi データ
	CMixiDataList	m_body;					///< このカテゴリリストに関連づけられたボディリストのデータ

	int				selectedBody;			///< ボディリストの選択項目のインデックス

	CString			m_name;					///< リストに表示する名称

	AccessTypeInfo::BODY_INDICATE_TYPE m_firstBodyColType;	///< １つ目のカラムに表示する項目の識別子
	AccessTypeInfo::BODY_INDICATE_TYPE m_secondBodyColType;	///< ２つ目のカラムに表示する項目の識別子

	bool			m_bCruise;				///< 巡回対象かどうかを表すフラグ

	bool			m_bFromLog;				///< ログを取得した値かどうかを表すフラグ

	bool			bSaveToGroupFile;		///< グループ定義ファイルに保存するか（永続化するか）

private:
	SYSTEMTIME		m_accessTime;			///< （カテゴリリストとしての）アクセス時刻
	int				m_idxItemOnList;		///< リストコントロール内のインデックス

public:
	/// コンストラクタ
	CCategoryItem() 
		: selectedBody(0)
		, m_bFromLog(0)
		, bSaveToGroupFile(true)
	{
		memset( &m_accessTime, 0, sizeof(SYSTEMTIME) );
	}

	enum SAVE_TO_GROUPFILE {
		SAVE_TO_GROUPFILE_YES = 1,
		SAVE_TO_GROUPFILE_NO = 2,
	};

	/// 初期化
	void init( LPCTSTR name,
			   LPCTSTR url, ACCESS_TYPE accessType, int index, 
			   AccessTypeInfo::BODY_INDICATE_TYPE firstColType, AccessTypeInfo::BODY_INDICATE_TYPE secondColType,
			   SAVE_TO_GROUPFILE saveToGroupFile=SAVE_TO_GROUPFILE_YES )
	{
		m_name = name;

		m_mixi.SetURL( url );
		m_mixi.SetAccessType( accessType );

		m_idxItemOnList = index;
		m_firstBodyColType = firstColType;
		m_secondBodyColType = secondColType;

		bSaveToGroupFile = saveToGroupFile == SAVE_TO_GROUPFILE_YES ? true : false;
	}
	virtual ~CCategoryItem()				{}

	/// 現在選択中のボディアイテムを取得する
	CMixiData& GetSelectedBody() {
		if (selectedBody < 0 || selectedBody >= (int)m_body.size()) {
			// ダミーを返す。本来はNULLを返すなり、Exception投げるなりすべき。
			static CMixiData s_dummy;
			CMixiData dummy;
			s_dummy = dummy;	// 初期化
			return s_dummy;
		}
		return m_body[ selectedBody ];
	}

	CMixiDataList& GetBodyList()			{ return m_body; };

	void	SetAccessTime(const SYSTEMTIME& value)	{ m_accessTime = value; };
	const SYSTEMTIME& GetAccessTime()				{ return m_accessTime; };
	CString GetAccessTimeString() {
		SYSTEMTIME zero;
		memset( &zero, 0, sizeof(SYSTEMTIME) );
		if (memcmp(&m_accessTime,&zero, sizeof(SYSTEMTIME))==0) {
			return L"";
		}
		if (m_bFromLog) {
			return CTime(m_accessTime).Format( L"%m/%d %H:%M:%S log" );
		} else {
			return CTime(m_accessTime).Format( L"%m/%d %H:%M:%S" );
		}
	}

	void	SetIndexOnList(int value)		{ m_idxItemOnList = value; };
	int		GetIndexOnList()				{ return m_idxItemOnList; };

};
