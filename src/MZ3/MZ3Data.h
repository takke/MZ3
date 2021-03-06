/*
 * Copyright (c) MZ3 Project Team All rights reserved.
 * Code licensed under the BSD License:
 * http://www.mz3.jp/license.txt
 */
#pragma once

// MZ3Data

#include "constants.h"
#include <vector>
#include <string>
//#include <map>
#include <hash_map>

// hash_map 関連で多数発生する warning を抑制「C4503: 装飾された名前の長さが限界を超えました。名前は切り捨てられます。」
#pragma warning(disable:4503)


class MZ3Data;
typedef std::vector<MZ3Data> MZ3DataList;

typedef std::wstring				MZ3String;			///< 文字列型
typedef std::vector<std::wstring>	MZ3StringArray;		///< 文字列配列型
typedef stdext::hash_map<MZ3String, MZ3StringArray> MZ3StringsMap;	///< 文字列配列のmap

extern int g_nMZ3DataInstances;							///< MZ3Data オブジェクト数

/// 汎用文字列配列コンテナ
class MZ3StringArrayMap
{
private:
	MZ3StringsMap					m_theMap;	///< 文字列配列のmap

	/// m_StringArrayMap から文字列配列オブジェクトを取得する
	MZ3StringArray* GetStringArray(LPCTSTR key)
	{
		return &m_theMap[key];
	}

	/// m_StringArrayMap から文字列配列オブジェクトを取得する。
	const MZ3StringArray* GetStringArrayConst(LPCTSTR key) const
	{
		if( m_theMap.size() == 0 ){
			return NULL;
		}
		const MZ3StringsMap::const_iterator& v = m_theMap.find(key);
		if (v==m_theMap.end()) {
			return NULL;
		}
		return &v->second;
	}

public:
	/// クリアする。
	void Clear(LPCTSTR key)
	{
		MZ3StringArray* pArray = GetStringArray(key);
		if (pArray!=NULL) {
			pArray->clear();
		}
	}

	/// 文字列を追加する。
	void AppendString(LPCTSTR key, LPCTSTR text)
	{
		MZ3StringArray* pArray = GetStringArray(key);
		if (pArray!=NULL) {
			pArray->push_back(text);
		}
	}

	/// 文字列を取得する。
	LPCTSTR GetString(LPCTSTR key, int idx) const
	{
		const MZ3StringArray* pArray = GetStringArrayConst(key);
		if (pArray==NULL) {
			return L"";
		}
		return (*pArray)[idx].c_str();
	}

	/// 要素数を取得する。key に該当する配列がない場合は、0 を返す。
	size_t GetSize(LPCTSTR key) const
	{
		const MZ3StringArray* pArray = GetStringArrayConst(key);
		if (pArray==NULL) {
			return 0;
		}
		return pArray->size();
	}
};

//typedef std::map<MZ3String, MZ3String>	MZ3StringMap;
//typedef std::map<MZ3String, INT32>		MZ3Int32Map;
//typedef std::map<MZ3String, INT64>		MZ3Int64Map;
typedef stdext::hash_map<MZ3String, MZ3String>	MZ3StringMap;
typedef stdext::hash_map<MZ3String, INT32>		MZ3Int32Map;
typedef stdext::hash_map<MZ3String, INT64>		MZ3Int64Map;

/**
 * MZ3 のデータ管理クラス
 */
class MZ3Data
{
protected:
	ACCESS_TYPE			m_accessType;		///< オブジェクト種別
	CString				m_dateText;			///< 日付時刻（文字列）：GetDate() で優先して返却される
	CTime				m_dateRaw;			///< 日付時刻（メタ情報）

	MZ3DataList			m_children;			///< 子要素（コメントなど）
											///< コミュニティページ要素の場合は、
											///< 要素0に list_bbs.pl のデータを持つ。

	MZ3StringMap		m_StringMap;		///< 汎用文字列コンテナ
	MZ3Int32Map			m_Int32Map;			///< 汎用32bit数値コンテナ
	MZ3Int64Map			m_Int64Map;			///< 汎用64bit数値コンテナ
	MZ3StringArrayMap	m_StringArrayMap;	///< 汎用文字列配列コンテナ

public:

	/// リンクデータ
	class Link {
	public:
		CString url;		///< URL
		CString text;		///< リンクの文字列

		/// コンストラクタ
		Link( LPCTSTR url_, LPCTSTR text_ ) : url(url_), text(text_) {}
	};
	std::vector<Link> m_linkList;		///< ページに含まれるリンクの一覧
	std::vector<Link> m_linkPage;		///< ページ移動リンクの一覧。
										///< 「全てを表示」「1」「2」...

public:
	/**
	 * コンストラクタ
	 */
	MZ3Data()
		: m_accessType(ACCESS_INVALID) // 初期値
	{
		g_nMZ3DataInstances ++;
	}

	/**
	 * デストラクタ
	 */
	virtual ~MZ3Data()
	{
		ClearChildren();
//		g_nMZ3DataInstances --;
	}

public:

	//--- アクセッサ

	/// 全リストの初期化
	void ClearAllList() {
		ClearBody();
		ClearImage();
		ClearMovie();
		m_linkList.clear();
		m_linkPage.clear();
	}

	// 日付時刻のアクセッサ
	void		 SetDate(const CTime& t)	{ m_dateRaw = t; }
	void		 SetDate(CString date)		{ m_dateText = date; }
	CString		 GetDate() const;
	const CTime& GetDateRaw() const			{ return m_dateRaw;	}

	// アクセス種別のアクセッサ
	void SetAccessType(ACCESS_TYPE type)	{ m_accessType = type; }
	ACCESS_TYPE GetAccessType()	const		{ return m_accessType; }

public:
	//--- 汎用64bit数値コンテナのアクセッサ

	/// 汎用64bit数値コンテナへのキー指定による数値設定
	void	SetInt64Value(LPCTSTR key, INT64 value) {
		m_Int64Map[key] = value;
	}
	/// 汎用64bit数値コンテナからのキー指定による数値取得
	/// 取得失敗時は default_value を返す
	INT64	GetInt64Value(LPCTSTR key, INT64 default_value=-1) {
		return FindInt64Map(key, default_value);
	}

	//--- 汎用数値コンテナのアクセッサ

	/// 汎用数値コンテナへのキー指定による数値設定
	void	SetIntValue(LPCTSTR key, int value) {
		m_Int32Map[key] = value;
	}
	/// 汎用数値コンテナからのキー指定による数値取得
	/// 取得失敗時は default_value を返す
	int	GetIntValue(LPCTSTR key, int default_value=-1) {
		return FindInt32Map(key, default_value);
	}

	// author_id : 投稿者のID（オーナーIDに設定される場合もある）
//	void	SetAuthorID(int authorId)	{ m_Int32Map[L"author_id"] = authorId; }
	int		GetAuthorID() const			{ return FindInt32Map(L"author_id", -1); }

	// id : 記事ID
	void	SetID(int value)			{ m_Int32Map[L"id"] = value; }
	INT64	GetID()	const				{
		INT64 id = FindInt64Map(L"id", -1);
		if (id!=-1) {
			return id;
		} else {
			return FindInt32Map(L"id", -1);
		}
	}

	// comment_index : コメント番号
	void	SetCommentIndex(int value)	{ m_Int32Map[L"comment_index"] = value; }
	int		GetCommentIndex() const		{ return FindInt32Map(L"comment_index", -1); }

	// comment_count : コメントの数
	void	SetCommentCount(int value)	{ m_Int32Map[L"comment_count"] = value; }
	int		GetCommentCount() const		{ return FindInt32Map(L"comment_count", 0); }

	// owner_id : オーナーID（投稿者IDに設定される場合もある）
	void	SetOwnerID(int value)		{ m_Int32Map[L"owner_id"] = value; }
	int		GetOwnerID() const			{ return FindInt32Map(L"owner_id", -1); }

	// my_mixi : マイミクフラグ（足あとからのマイミク抽出時のみ対応）
//	void	SetMyMixi(bool bMyMixi)		{ m_Int32Map[L"my_mixi"] = bMyMixi ? 1 : 0; }
	bool	IsMyMixi() const			{ return FindInt32Map(L"my_mixi", 0) ? true : false; }


	//--- 汎用文字列配列コンテナのアクセッサ

	void	ClearTextArray(LPCTSTR key) {
		m_StringArrayMap.Clear(key);
	}
	void	AddTextArray(LPCTSTR key, LPCTSTR value) {
		m_StringArrayMap.AppendString(key, value);
	}
	LPCTSTR	GetTextArrayValue(LPCTSTR key, int idx) {
		return m_StringArrayMap.GetString(key, idx);
	}
	size_t	GetTextArraySize(LPCTSTR key) {
		return m_StringArrayMap.GetSize(key);
	}


	// body list
	void	ClearBody()					{ m_StringArrayMap.Clear(L"body"); }
	void	AddBody(LPCTSTR str)		{ m_StringArrayMap.AppendString(L"body", str); }
	LPCTSTR GetBodyItem(int idx) const	{ return m_StringArrayMap.GetString(L"body", idx); }
	size_t	GetBodySize() const			{ return m_StringArrayMap.GetSize(L"body"); }
	CString GetBody() const {
		CString s;
		unsigned int n=this->GetBodySize();
		for (unsigned int i=0; i<n; i++) {
			s.Append( this->GetBodyItem(i) );
		}
		return s;
	}


	// image list
	void	ClearImage()				{ m_StringArrayMap.Clear(L"image"); }
	void	AddImage(LPCTSTR str)		{ m_StringArrayMap.AppendString(L"image", str); }
	LPCTSTR GetImage(int idx) const		{ return m_StringArrayMap.GetString(L"image", idx); }
	int		GetImageCount()	const		{ return m_StringArrayMap.GetSize(L"image"); }

	// movie list
	void	ClearMovie()				{ m_StringArrayMap.Clear(L"movie"); }
	void	AddMovie(LPCTSTR str)		{ m_StringArrayMap.AppendString(L"movie", str); }
	LPCTSTR GetMovie(int idx) const		{ return m_StringArrayMap.GetString(L"movie", idx); }
	int		GetMovieCount()	const		{ return m_StringArrayMap.GetSize(L"movie"); }


	//--- 汎用文字列コンテナのアクセッサ

	/// 汎用文字列コンテナへのキー指定による文字列設定
	void	SetTextValue(LPCTSTR key, LPCTSTR value) {
		m_StringMap[key] = value;
	}
	/// 汎用文字列コンテナからのキー指定による文字列取得
	CString	GetTextValue(LPCTSTR key) const {
		return FindStringMap(key);
	}

	// name : 名前
	void	SetName(CString name);
	CString GetName() const					{ return FindStringMap(L"name"); }

	// author : 投稿者名
	void	SetAuthor(CString author);
	CString GetAuthor() const				{ return FindStringMap(L"author"); }

	// url
	void	SetURL(CString url)				{ m_StringMap[L"url"] = url; }
	CString GetURL() const					{ return FindStringMap(L"url"); }

	// title : タイトル
	void	SetTitle(CString title);
	CString GetTitle() const				{ return FindStringMap(L"title"); }

	// browse_uri
	void	SetBrowseUri(LPCTSTR str)		{ m_StringMap[L"browse_uri"] = str; }
	LPCTSTR GetBrowseUri()					{ return FindStringMap(L"browse_uri"); }

	// post_address : POST用のアドレス
//	void	SetPostAddress(LPCTSTR str)		{ m_StringMap[L"post_address"] = str; }
	LPCTSTR GetPostAddress()				{ return FindStringMap(L"post_address"); }

	// opening_range : 公開範囲
//	void	SetOpeningRange(LPCTSTR range)	{ m_StringMap[L"opening_range"] = range; }
	CString GetOpeningRange()				{ return FindStringMap(L"opening_range"); }

	// next_diary : 次の日記
//	void	SetNextDiary(LPCTSTR url)		{ m_StringMap[L"next_diary"] = url; }
	CString GetNextDiary()					{ return FindStringMap(L"next_diary"); }

	// prev_diary : 前の日記
//	void	SetPrevDiary(LPCTSTR url)		{ m_StringMap[L"prev_diary"] = url; }
	CString GetPrevDiary()					{ return FindStringMap(L"prev_diary"); }

	// full_diary : 全てを表示
//	void	SetFullDiary(LPCTSTR url)		{ m_StringMap[L"full_diary"] = url; }
	CString GetFullDiary()					{ return FindStringMap(L"full_diary"); }

	// Content-Typeのアクセッサ
	void SetContentType(CONTENT_TYPE value)	{ SetTextValue(L"content_type", value); }
	CString GetContentType() const			{ return FindStringMap(L"content_type"); }

	//--- 子要素操作
	void	ClearChildren()					{ m_children.clear(); }				///< 子要素の削除
	void	AddChild(const MZ3Data& child)	{ m_children.push_back(child); }	///< 子要素の追加
	size_t	GetChildrenSize()				{ return m_children.size(); }		///< 子要素数の取得
	MZ3Data&	 GetChild( int idx )		{ return m_children[idx]; }			///< 子要素の取得
	MZ3DataList& GetChildren()				{ return m_children; }				///< 子要素リストの取得

	//--- debug 情報
	int		GetInstanceCount()				{ return g_nMZ3DataInstances; }		///< MZ3Data インスタンス数

private:
	/// 汎用数値コンテナから key 値を取得する。失敗時は default_value を返す。
	int FindInt32Map(LPCTSTR key, INT32 default_value) const
	{
		if( m_Int32Map.size() == 0 ){
			return default_value;
		}
		const MZ3Int32Map::const_iterator& v = m_Int32Map.find(key);
		if (v==m_Int32Map.end()) {
			return default_value;
		}
		return v->second;
	}

	/// 汎用64bit数値コンテナから key 値を取得する。失敗時は default_value を返す。
	INT64 FindInt64Map(LPCTSTR key, INT64 default_value) const
	{
		if( m_Int64Map.size() == 0 ){
			return default_value;
		}
		const MZ3Int64Map::const_iterator& v = m_Int64Map.find(key);
		if (v==m_Int64Map.end()) {
			return default_value;
		}
		return v->second;
	}

	/// 汎用文字列コンテナから key 値を取得する。失敗時は L"" を返す。
	LPCTSTR FindStringMap(LPCTSTR key) const
	{
		if( m_StringMap.size() == 0 ){
			return L"";
		}
		const MZ3StringMap::const_iterator& v = m_StringMap.find(key);
		if (v==m_StringMap.end()) {
			return L"";
		}
		return v->second.c_str();
	}
};


