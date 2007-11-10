#pragma once

// CMixiData コマンド ターゲット

#include "constants.h"
#include <vector>

class CMixiData;
typedef std::vector<CMixiData> CMixiDataList;

/**
 * mixi のデータ管理クラス
 */
class CMixiData
{
protected:
	ACCESS_TYPE		m_accessType;			///< アクセス種別（というよりもこのオブジェクトの種別）
	CString			m_date;					///< 日付時刻
	CString			m_name;					///< 名前
	CString			m_author;				///< 投稿者名
	int				m_authorId;				///< 投稿者のID（オーナーIDに設定される場合もある）
	CString			m_title;				///< タイトル
	CString			m_url;					///< URL
	CString			m_postAddress;			///< ＰＯＳＴ用のアドレス
	int				m_id;					///< 記事ID
	int				m_commId;				///< コメントID
	int				m_commentIndex;			///< コメント番号
	int				m_commentCount;			///< コメントの数
	CONTENT_TYPE	m_contentType;			///< Content-Type

	int				m_ownerId;				///< オーナーID（投稿者IDに設定される場合もある）

	std::vector<CString>	m_bodyArray;	///< 本文（コメント等の本文）
	std::vector<CString>	m_imageArray;	///< 画像のリスト
	std::vector<CString>	m_MovieArray;	///< 動画のリスト

	CMixiDataList	m_children;				///< 子要素（コメントなど）
											///< コミュニティページ要素の場合は、
											///< 要素0に list_bbs.pl のデータを持つ。

	BOOL			m_otherDiary;			///< 外部ブログ
	CString			m_browseUri;
	bool			m_myMixi;				///< マイミクフラグ（足あとからのマイミク抽出時のみ対応）

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
	CMixiData()
		: m_accessType(ACCESS_INVALID) // 初期値
		, m_authorId(-1)
		, m_id(-1)
		, m_commId(-1)
		, m_commentIndex(-1)
		, m_commentCount(0)
		, m_contentType(CONTENT_TYPE_INVALID)
		, m_ownerId(-1)
		, m_otherDiary(FALSE)
		, m_myMixi(false)
	{
	}

	/**
	 * デストラクタ
	 */
	virtual ~CMixiData()
	{
		ClearChildren();
		m_imageArray.clear();
		m_MovieArray.clear();
	}

public:

	/// 全リストの初期化
	void ClearAllList() {
		ClearBody();
		ClearImage();
		ClearMovie();
		m_linkList.clear();
		m_linkPage.clear();
	}

	void SetDate(CString date)			{ m_date = date; }
	void SetDate(int, int, int, int, int);
	void SetDate(int, int, int, int);
	CString GetDate()					{ return m_date; }

	void SetName(CString name);
	CString GetName()					{ return m_name; }

	void SetAuthor(CString author);
	CString GetAuthor() const			{ return m_author; }

	void SetAuthorID(CString authorId) {
		// 数値変換
		m_authorId = _wtoi(authorId);
	}
	int GetAuthorID()					{ return m_authorId; }

	void SetTitle(CString title);
	CString GetTitle()					{ return m_title; }

	void SetURL(CString url)			{ m_url = url; }
	CString GetURL() const				{ return m_url; }

	void SetAccessType(ACCESS_TYPE type) { m_accessType = type; };
	ACCESS_TYPE GetAccessType()	const	{ return m_accessType; };

	void SetCommentIndex(int value)		{ m_commentIndex = value; };
	int GetCommentIndex() const			{ return m_commentIndex; };

	void SetID(int value)				{ m_id = value; };
	int GetID()	const					{ return m_id; };

	void SetCommentID(int value)		{ m_commId = value; };
	int GetCommentID()					{ return m_commId; };

	void SetPostAddress(LPCTSTR str)	{ m_postAddress = str; };
	LPCTSTR GetPostAddress()			{ return m_postAddress; };

	void SetContentType(CONTENT_TYPE value)	{ m_contentType = value; };
	CONTENT_TYPE GetContentType() const		{ return m_contentType; };

	void SetCommentCount(int value)	    { m_commentCount = value; };
	int GetCommentCount() const			{ return m_commentCount; };

	void SetOwnerID(int value)			{ m_ownerId = value; };
	int GetOwnerID() const				{ return m_ownerId; };

	void	AddImage(LPCTSTR str)		{ m_imageArray.push_back(str); }
	void	AddMovie(LPCTSTR str)		{ m_MovieArray.push_back(str); }
	int		GetImageCount()	const		{ return (int)m_imageArray.size(); }
	int		GetMovieCount()	const		{ return (int)m_MovieArray.size(); }
	LPCTSTR GetImage(int index) const	{ return m_imageArray[index]; }
	LPCTSTR GetMovie(int index)	const	{ return m_MovieArray[index]; }
	void	ClearImage();
	void	ClearMovie();

	void SetOtherDiary(BOOL flag)		{ m_otherDiary = flag; }
	BOOL IsOtherDiary()					{ return m_otherDiary; }

	void SetMyMixi(bool bMyMixi)		{ m_myMixi = bMyMixi; }
	BOOL IsMyMixi()						{ return m_myMixi; }

	void SetBrowseUri(LPCTSTR str)		{ m_browseUri = str; }
	LPCTSTR GetBrowseUri()				{ return m_browseUri; }

	void	ClearBody()					{ m_bodyArray.clear(); }
	void	AddBody(CString str)		{ m_bodyArray.push_back(str); }
	LPCTSTR GetBody(int idx) const		{ return m_bodyArray[idx]; }
	size_t	GetBodySize() const			{ return m_bodyArray.size(); }

	//--- 子要素操作
	void	AddChild(const CMixiData& child)
									{ m_children.push_back(child); }	///< 子要素の追加
	CMixiData& GetChild( int idx )	{ return m_children[idx]; }			///< 子要素の取得
	size_t	GetChildrenSize()		{ return m_children.size(); }		///< 子要素数の取得
	CMixiDataList& GetChildren()	{ return m_children; }				///< 子要素リストの取得
	void	ClearChildren()			{ m_children.clear(); }				///< 子要素の削除
};


