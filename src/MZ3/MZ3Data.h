/*
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */
#pragma once

// MZ3Data

#include "constants.h"
#include <vector>
#include <string>
#include <map>

class MZ3Data;
typedef std::vector<MZ3Data> MZ3DataList;

/**
 * MZ3 のデータ管理クラス
 */
class MZ3Data
{
protected:
	ACCESS_TYPE		m_accessType;			///< オブジェクト種別
	CString			m_dateText;				///< 日付時刻（文字列）：GetDate() で優先して返却される
	CTime			m_dateRaw;				///< 日付時刻（メタ情報）

	int				m_authorId;				///< 投稿者のID（オーナーIDに設定される場合もある）
	int				m_id;					///< 記事ID
	int				m_commentIndex;			///< コメント番号
	int				m_commentCount;			///< コメントの数
	CONTENT_TYPE	m_contentType;			///< Content-Type

	int				m_ownerId;				///< オーナーID（投稿者IDに設定される場合もある）

	std::vector<CString>	m_bodyArray;	///< 本文（コメント等の本文）
	std::vector<CString>	m_imageArray;	///< 画像のリスト
	std::vector<CString>	m_MovieArray;	///< 動画のリスト

	MZ3DataList		m_children;				///< 子要素（コメントなど）
											///< コミュニティページ要素の場合は、
											///< 要素0に list_bbs.pl のデータを持つ。

	bool			m_myMixi;				///< マイミクフラグ（足あとからのマイミク抽出時のみ対応）

	std::map<std::wstring, std::wstring> m_StringMap;	///< 汎用文字列コンテナ
	std::map<std::wstring, int>			 m_IntegerMap;	///< 汎用数値コンテナ

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
		, m_authorId(-1)
		, m_id(-1)
		, m_commentIndex(-1)
		, m_commentCount(0)
		, m_contentType(CONTENT_TYPE_INVALID)
		, m_ownerId(-1)
		, m_myMixi(false)
	{
	}

	/**
	 * デストラクタ
	 */
	virtual ~MZ3Data()
	{
		ClearChildren();
		m_imageArray.clear();
		m_MovieArray.clear();
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

	void SetDate(const CTime& t)		{ m_dateRaw = t; }
	void SetDate(CString date)			{ m_dateText = date; }
	CString GetDate();
	const CTime& GetDateRaw() const		{ return m_dateRaw;	}

	void SetAuthorID(int authorId)		{ m_authorId = authorId; }
	int GetAuthorID() const				{ return m_authorId; }

	void SetAccessType(ACCESS_TYPE type) { m_accessType = type; }
	ACCESS_TYPE GetAccessType()	const	{ return m_accessType; }

	void SetCommentIndex(int value)		{ m_commentIndex = value; }
	int GetCommentIndex() const			{ return m_commentIndex; }

	void SetID(int value)				{ m_id = value; }
	int GetID()	const					{ return m_id; }

	void SetContentType(CONTENT_TYPE value)	{ m_contentType = value; }
	CONTENT_TYPE GetContentType() const		{ return m_contentType; }

	void SetCommentCount(int value)	    { m_commentCount = value; }
	int GetCommentCount() const			{ return m_commentCount; }

	void SetOwnerID(int value)			{ m_ownerId = value; }
	int GetOwnerID() const				{ return m_ownerId; }

	void	AddImage(LPCTSTR str)		{ m_imageArray.push_back(str); }
	void	AddMovie(LPCTSTR str)		{ m_MovieArray.push_back(str); }
	int		GetImageCount()	const		{ return (int)m_imageArray.size(); }
	int		GetMovieCount()	const		{ return (int)m_MovieArray.size(); }
	LPCTSTR GetImage(int index) const	{ return m_imageArray[index]; }
	LPCTSTR GetMovie(int index)	const	{ return m_MovieArray[index]; }
	void	ClearImage()				{ m_imageArray.clear(); }
	void	ClearMovie()				{ m_MovieArray.clear(); }

	void	SetMyMixi(bool bMyMixi)		{ m_myMixi = bMyMixi; }
	bool	IsMyMixi()					{ return m_myMixi; }

	void	ClearBody()					{ m_bodyArray.clear(); }
	void	AddBody(CString str)		{ m_bodyArray.push_back(str); }
	LPCTSTR GetBody(int idx) const		{ return m_bodyArray[idx]; }
	size_t	GetBodySize() const			{ return m_bodyArray.size(); }

	//--- 汎用文字列コンテナのアクセッサ

	/// 汎用文字列コンテナから key 値を取得する。失敗時は L"" を返す。
	LPCTSTR FindStringMap(LPCTSTR key) const
	{
		const std::map<std::wstring, std::wstring>::const_iterator& v = m_StringMap.find(key);
		if (v==m_StringMap.end()) {
			return L"";
		}
		return v->second.c_str();
	}

	// name : 名前
	void SetName(CString name);
	CString GetName() const					{ return FindStringMap(L"name"); }

	// author : 投稿者名
	void SetAuthor(CString author);
	CString GetAuthor() const				{ return FindStringMap(L"author"); }

	// url
	void SetURL(CString url)				{ m_StringMap[L"url"] = url; }
	CString GetURL() const					{ return FindStringMap(L"url"); }

	// title : タイトル
	void SetTitle(CString title);
	CString GetTitle()						{ return FindStringMap(L"title"); }

	// browse_uri
	void	SetBrowseUri(LPCTSTR str)		{ m_StringMap[L"browse_uri"] = str; }
	LPCTSTR GetBrowseUri()					{ return FindStringMap(L"browse_uri"); }

	// post_address : POST用のアドレス
	void SetPostAddress(LPCTSTR str)		{ m_StringMap[L"post_address"] = str; }
	LPCTSTR GetPostAddress()				{ return FindStringMap(L"post_address"); }

	// opening_range : 公開範囲
	void	SetOpeningRange(LPCTSTR range)	{ m_StringMap[L"opening_range"] = range; }
	CString GetOpeningRange()				{ return FindStringMap(L"opening_range"); }

	//--- 子要素操作
	void	AddChild(const MZ3Data& child)	{ m_children.push_back(child); }	///< 子要素の追加
	MZ3Data& GetChild( int idx )			{ return m_children[idx]; }			///< 子要素の取得
	size_t	GetChildrenSize()				{ return m_children.size(); }		///< 子要素数の取得
	MZ3DataList& GetChildren()				{ return m_children; }				///< 子要素リストの取得
	void	ClearChildren()					{ m_children.clear(); }				///< 子要素の削除
};


