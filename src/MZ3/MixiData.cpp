// MixiData.cpp : 実装ファイル
//

#include "stdafx.h"
#include "MZ3.h"
#include "MixiData.h"


// CMixiData

/**
 * コンストラクタ
 */
CMixiData::CMixiData()
	: m_accessType(ACCESS_INVALID) // 初期値
	, m_authorId(-1)
	, m_id(-1)
	, m_commId(-1)
	, m_commentIndex(-1)
	, m_commentCount(0)
	, m_lastIndex(-1)
	, m_contentType(CONTENT_TYPE_INVALID)
	, m_ownerId(-1)
	, m_otherDiary(FALSE)
	, m_myMixi(false)
{
}

/**
 * デストラクタ
 */
CMixiData::~CMixiData()
{
	ClearChildren();
	m_imageArray.clear();
	m_MovieArray.clear();
}


// CMixiData メンバ関数


/**
 * タイトルデータの設定
 *
 * @param title [in] タイトル
 * @return なし
 */
void CMixiData::SetTitle(CString title)
{
	// 変換処理
	while(title.Replace(_T("&gt;"), _T(">")));
	while(title.Replace(_T("&lt;"), _T("<")));

	m_title = title;
}

/**
 * 名前データの設定
 *
 * @param name [in] 名前
 * @return なし
 */
void CMixiData::SetName(CString name)
{
	// 変換処理
	while(name.Replace(_T("&gt;"), _T(">")));
	while(name.Replace(_T("&lt;"), _T("<")));

	m_name = name;
}

/**
 * 投稿者データの設定
 *
 * @param author [in] 名前
 * @return なし
 */
void CMixiData::SetAuthor(CString author)
{
	// 変換処理
	while(author.Replace(_T("&gt;"), _T(">")));
	while(author.Replace(_T("&lt;"), _T("<")));

	m_author = author;
}

void CMixiData::SetDate(int year, int month, int day, int hour, int minute)
{
	m_date.Format(_T("%04d/%02d/%02d %02d:%02d"), year, month, day, hour, minute);
//	m_date.Format(_T("%02d/%02d %02d:%02d"), month, day, hour, minute);
}

void CMixiData::SetDate(int month, int day, int hour, int minute)
{
	m_date.Format(_T("%02d/%02d %02d:%02d"), month, day, hour, minute);
}

void CMixiData::ClearImage()
{
	m_imageArray.clear();
}

void CMixiData::ClearMovie()
{
	m_MovieArray.clear();
}