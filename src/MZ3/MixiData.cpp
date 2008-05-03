/*
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */
// MixiData.cpp : �����t�@�C��
//

#include "stdafx.h"
#include "MixiData.h"


// CMixiData



// CMixiData �����o�֐�


/**
 * �^�C�g���f�[�^�̐ݒ�
 *
 * @param title [in] �^�C�g��
 * @return �Ȃ�
 */
void CMixiData::SetTitle(CString title)
{
	// �ϊ�����
	while(title.Replace(_T("&gt;"), _T(">")));
	while(title.Replace(_T("&lt;"), _T("<")));

	m_title = title;
}

/**
 * ���O�f�[�^�̐ݒ�
 *
 * @param name [in] ���O
 * @return �Ȃ�
 */
void CMixiData::SetName(CString name)
{
	// �ϊ�����
	while(name.Replace(_T("&gt;"), _T(">")));
	while(name.Replace(_T("&lt;"), _T("<")));

	m_name = name;
}

/**
 * ���e�҃f�[�^�̐ݒ�
 *
 * @param author [in] ���O
 * @return �Ȃ�
 */
void CMixiData::SetAuthor(CString author)
{
	// �ϊ�����
	while(author.Replace(_T("&gt;"), _T(">")));
	while(author.Replace(_T("&lt;"), _T("<")));

	m_author = author;
}

/**
 * m_dateText ������΂����Ԃ��B
 * �Ȃ���� m_dateRaw ��ϊ����ĕԂ��B
 */
CString CMixiData::GetDate() {
	if (!m_dateText.IsEmpty()) {
		return m_dateText;
	} else {
		// �����l�ł���΂��當�����Ԃ�
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

void CMixiData::ClearImage()
{
	m_imageArray.clear();
}

void CMixiData::ClearMovie()
{
	m_MovieArray.clear();
}