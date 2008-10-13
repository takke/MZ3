/*
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */
// MixiData.cpp : 実装ファイル
//

#include "stdafx.h"
#include "MixiData.h"


// CMixiData

int g_nMZ3DataInstances=0;

// CMixiData メンバ関数

/**
 * タイトルデータの設定
 *
 * @param title [in] タイトル
 * @return なし
 */
void MZ3Data::SetTitle(CString title)
{
	// 変換処理
	while(title.Replace(_T("&gt;"), _T(">")));
	while(title.Replace(_T("&lt;"), _T("<")));

	m_StringMap[L"title"] = title;
}

/**
 * 名前データの設定
 *
 * @param name [in] 名前
 * @return なし
 */
void MZ3Data::SetName(CString name)
{
	// 変換処理
	while(name.Replace(_T("&gt;"), _T(">")));
	while(name.Replace(_T("&lt;"), _T("<")));

	m_StringMap[L"name"] = name;
}

/**
 * 投稿者データの設定
 *
 * @param author [in] 名前
 * @return なし
 */
void MZ3Data::SetAuthor(CString author)
{
	// 変換処理
	while(author.Replace(_T("&gt;"), _T(">")));
	while(author.Replace(_T("&lt;"), _T("<")));

	m_StringMap[L"author"] = author;
}

/**
 * m_dateText があればそれを返す。
 * なければ m_dateRaw を変換して返す。
 */
CString MZ3Data::GetDate() const {
	if (!m_dateText.IsEmpty()) {
		return m_dateText;
	} else {
		// 初期値であればから文字列を返す
		if (CTime() == m_dateRaw) {
			return L"";
		} else {
			CString s;
			s.Format(_T("%04d/%02d/%02d %02d:%02d"), 
				m_dateRaw.GetYear(), 
				m_dateRaw.GetMonth(), 
				m_dateRaw.GetDay(), 
				m_dateRaw.GetHour(), 
				m_dateRaw.GetMinute());
			return s;
		}
	}
}
