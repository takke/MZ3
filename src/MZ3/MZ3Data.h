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
 * MZ3 �̃f�[�^�Ǘ��N���X
 */
class MZ3Data
{
protected:
	ACCESS_TYPE		m_accessType;			///< �I�u�W�F�N�g���
	CString			m_dateText;				///< ���t�����i������j�FGetDate() �ŗD�悵�ĕԋp�����
	CTime			m_dateRaw;				///< ���t�����i���^���j

	int				m_authorId;				///< ���e�҂�ID�i�I�[�i�[ID�ɐݒ肳���ꍇ������j
	int				m_id;					///< �L��ID
	int				m_commentIndex;			///< �R�����g�ԍ�
	int				m_commentCount;			///< �R�����g�̐�
	CONTENT_TYPE	m_contentType;			///< Content-Type

	int				m_ownerId;				///< �I�[�i�[ID�i���e��ID�ɐݒ肳���ꍇ������j

	std::vector<CString>	m_bodyArray;	///< �{���i�R�����g���̖{���j
	std::vector<CString>	m_imageArray;	///< �摜�̃��X�g
	std::vector<CString>	m_MovieArray;	///< ����̃��X�g

	MZ3DataList		m_children;				///< �q�v�f�i�R�����g�Ȃǁj
											///< �R�~���j�e�B�y�[�W�v�f�̏ꍇ�́A
											///< �v�f0�� list_bbs.pl �̃f�[�^�����B

	bool			m_myMixi;				///< �}�C�~�N�t���O�i�����Ƃ���̃}�C�~�N���o���̂ݑΉ��j

	std::map<std::wstring, std::wstring> m_StringMap;	///< �ėp������R���e�i
	std::map<std::wstring, int>			 m_IntegerMap;	///< �ėp���l�R���e�i

public:

	/// �����N�f�[�^
	class Link {
	public:
		CString url;		///< URL
		CString text;		///< �����N�̕�����

		/// �R���X�g���N�^
		Link( LPCTSTR url_, LPCTSTR text_ ) : url(url_), text(text_) {}
	};
	std::vector<Link> m_linkList;		///< �y�[�W�Ɋ܂܂�郊���N�̈ꗗ
	std::vector<Link> m_linkPage;		///< �y�[�W�ړ������N�̈ꗗ�B
										///< �u�S�Ă�\���v�u1�v�u2�v...

public:
	/**
	 * �R���X�g���N�^
	 */
	MZ3Data()
		: m_accessType(ACCESS_INVALID) // �����l
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
	 * �f�X�g���N�^
	 */
	virtual ~MZ3Data()
	{
		ClearChildren();
		m_imageArray.clear();
		m_MovieArray.clear();
	}

public:

	//--- �A�N�Z�b�T

	/// �S���X�g�̏�����
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

	//--- �ėp������R���e�i�̃A�N�Z�b�T

	/// �ėp������R���e�i���� key �l���擾����B���s���� L"" ��Ԃ��B
	LPCTSTR FindStringMap(LPCTSTR key) const
	{
		const std::map<std::wstring, std::wstring>::const_iterator& v = m_StringMap.find(key);
		if (v==m_StringMap.end()) {
			return L"";
		}
		return v->second.c_str();
	}

	// name : ���O
	void SetName(CString name);
	CString GetName() const					{ return FindStringMap(L"name"); }

	// author : ���e�Җ�
	void SetAuthor(CString author);
	CString GetAuthor() const				{ return FindStringMap(L"author"); }

	// url
	void SetURL(CString url)				{ m_StringMap[L"url"] = url; }
	CString GetURL() const					{ return FindStringMap(L"url"); }

	// title : �^�C�g��
	void SetTitle(CString title);
	CString GetTitle()						{ return FindStringMap(L"title"); }

	// browse_uri
	void	SetBrowseUri(LPCTSTR str)		{ m_StringMap[L"browse_uri"] = str; }
	LPCTSTR GetBrowseUri()					{ return FindStringMap(L"browse_uri"); }

	// post_address : POST�p�̃A�h���X
	void SetPostAddress(LPCTSTR str)		{ m_StringMap[L"post_address"] = str; }
	LPCTSTR GetPostAddress()				{ return FindStringMap(L"post_address"); }

	// opening_range : ���J�͈�
	void	SetOpeningRange(LPCTSTR range)	{ m_StringMap[L"opening_range"] = range; }
	CString GetOpeningRange()				{ return FindStringMap(L"opening_range"); }

	//--- �q�v�f����
	void	AddChild(const MZ3Data& child)	{ m_children.push_back(child); }	///< �q�v�f�̒ǉ�
	MZ3Data& GetChild( int idx )			{ return m_children[idx]; }			///< �q�v�f�̎擾
	size_t	GetChildrenSize()				{ return m_children.size(); }		///< �q�v�f���̎擾
	MZ3DataList& GetChildren()				{ return m_children; }				///< �q�v�f���X�g�̎擾
	void	ClearChildren()					{ m_children.clear(); }				///< �q�v�f�̍폜
};


