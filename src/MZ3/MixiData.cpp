/*
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */
// MixiData.cpp : 実装ファイル
//

#include "stdafx.h"
#include "MZ3.h"
#include "MixiData.h"


// CMixiData



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
	if (year==0) {
		SetDate(month, day, hour, minute);
	} else {
		m_date.Format(_T("%04d/%02d/%02d %02d:%02d"), year, month, day, hour, minute);
	}
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