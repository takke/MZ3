// MixiData.cpp : �����t�@�C��
//

#include "stdafx.h"
#include "MZ3.h"
#include "MixiData.h"


// CMixiData

/**
 * �R���X�g���N�^
 */
CMixiData::CMixiData()
	: m_accessType(ACCESS_INVALID) // �����l
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
 * �f�X�g���N�^
 */
CMixiData::~CMixiData()
{
	ClearChildren();
	m_imageArray.clear();
	m_MovieArray.clear();
}


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