#pragma once

// CCategoryItem コマンド ターゲット

#include "MixiData.h"

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

	/// ボディに CMixiData 内のどの項目を表示するかの識別子
	enum BODY_INDICATE_TYPE
	{
		BODY_INDICATE_TYPE_DATE,			///< 日付を表示する
		BODY_INDICATE_TYPE_TITLE,			///< タイトルを表示する
		BODY_INDICATE_TYPE_NAME,			///< 名前を表示する
		BODY_INDICATE_TYPE_BODY,			///< 本文を表示する
		BODY_INDICATE_TYPE_NONE,			///< 何も表示しない
	};
	BODY_INDICATE_TYPE m_firstBodyColType;	///< １つ目のカラムに表示する項目の識別子
	BODY_INDICATE_TYPE m_secondBodyColType;	///< ２つ目のカラムに表示する項目の識別子

	bool			m_bCruise;				///< 巡回対象かどうかを表すフラグ

private:
	CString			m_accessTime;			///< （カテゴリリストとしての）アクセス時刻
	int				m_idxItemOnList;		///< リストコントロール内のインデックス

public:
	/// コンストラクタ
	CCategoryItem() 
		: selectedBody(0)
	{}

	/// 初期化
	void init( LPCTSTR name,
			   LPCTSTR url, ACCESS_TYPE accessType, int index, 
			   BODY_INDICATE_TYPE firstColType, BODY_INDICATE_TYPE secondColType )
	{
		m_name = name;

		m_mixi.SetURL( url );
		m_mixi.SetAccessType( accessType );

		m_idxItemOnList = index;
		m_firstBodyColType = firstColType;
		m_secondBodyColType = secondColType;
	}
	virtual ~CCategoryItem()				{}

	/// 現在選択中のボディアイテムを取得する
	CMixiData& GetSelectedBody()			{ return m_body[ selectedBody ]; }

	CMixiDataList& GetBodyList()			{ return m_body; };

	void	SetAccessTime(LPCTSTR value)	{ m_accessTime = value; };
	LPCTSTR GetAccessTime()					{ return m_accessTime; };

	void	SetIndexOnList(int value)		{ m_idxItemOnList = value; };
	int		GetIndexOnList()				{ return m_idxItemOnList; };

};
