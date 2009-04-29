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

typedef std::wstring				MZ3String;			///< ������^
typedef std::vector<std::wstring>	MZ3StringArray;		///< ������z��^

extern int g_nMZ3DataInstances;							///< MZ3Data �I�u�W�F�N�g��

/// �ėp������z��R���e�i
class MZ3StringArrayMap
{
private:
	std::map<MZ3String, MZ3StringArray> m_theMap;	///< ������z���map

	/// m_StringArrayMap ���當����z��I�u�W�F�N�g���擾����
	MZ3StringArray* GetStringArray(LPCTSTR key)
	{
		return &m_theMap[key];
	}

	/// m_StringArrayMap ���當����z��I�u�W�F�N�g���擾����B
	const MZ3StringArray* GetStringArrayConst(LPCTSTR key) const
	{
		if( m_theMap.size() == 0 ){
			return NULL;
		}
		const std::map<MZ3String, MZ3StringArray>::const_iterator& v = m_theMap.find(key);
		if (v==m_theMap.end()) {
			return NULL;
		}
		return &v->second;
	}

public:
	/// �N���A����B
	void Clear(LPCTSTR key)
	{
		MZ3StringArray* pArray = GetStringArray(key);
		if (pArray!=NULL) {
			pArray->clear();
		}
	}

	/// �������ǉ�����B
	void AppendString(LPCTSTR key, LPCTSTR text)
	{
		MZ3StringArray* pArray = GetStringArray(key);
		if (pArray!=NULL) {
			pArray->push_back(text);
		}
	}

	/// ��������擾����B
	LPCTSTR GetString(LPCTSTR key, int idx) const
	{
		const MZ3StringArray* pArray = GetStringArrayConst(key);
		if (pArray==NULL) {
			return L"";
		}
		return (*pArray)[idx].c_str();
	}

	/// �v�f�����擾����Bkey �ɊY������z�񂪂Ȃ��ꍇ�́A0 ��Ԃ��B
	size_t GetSize(LPCTSTR key) const
	{
		const MZ3StringArray* pArray = GetStringArrayConst(key);
		if (pArray==NULL) {
			return 0;
		}
		return pArray->size();
	}
};

typedef std::map<MZ3String, MZ3String>	MZ3StringMap;
typedef std::map<MZ3String, int>		MZ3IntegerMap;

/**
 * MZ3 �̃f�[�^�Ǘ��N���X
 */
class MZ3Data
{
protected:
	ACCESS_TYPE			m_accessType;		///< �I�u�W�F�N�g���
	CString				m_dateText;			///< ���t�����i������j�FGetDate() �ŗD�悵�ĕԋp�����
	CTime				m_dateRaw;			///< ���t�����i���^���j

	CONTENT_TYPE		m_contentType;		///< Content-Type

	MZ3DataList			m_children;			///< �q�v�f�i�R�����g�Ȃǁj
											///< �R�~���j�e�B�y�[�W�v�f�̏ꍇ�́A
											///< �v�f0�� list_bbs.pl �̃f�[�^�����B

	MZ3StringMap		m_StringMap;		///< �ėp������R���e�i
	MZ3IntegerMap		m_IntegerMap;		///< �ėp���l�R���e�i
	MZ3StringArrayMap	m_StringArrayMap;	///< �ėp������z��R���e�i

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
		, m_contentType(CONTENT_TYPE_INVALID)
	{
		g_nMZ3DataInstances ++;
	}

	/**
	 * �f�X�g���N�^
	 */
	virtual ~MZ3Data()
	{
		ClearChildren();
//		g_nMZ3DataInstances --;
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

	// ���t�����̃A�N�Z�b�T
	void		 SetDate(const CTime& t)	{ m_dateRaw = t; }
	void		 SetDate(CString date)		{ m_dateText = date; }
	CString		 GetDate() const;
	const CTime& GetDateRaw() const			{ return m_dateRaw;	}

	// �A�N�Z�X��ʂ̃A�N�Z�b�T
	void SetAccessType(ACCESS_TYPE type)	{ m_accessType = type; }
	ACCESS_TYPE GetAccessType()	const		{ return m_accessType; }

	// Content-Type�̃A�N�Z�b�T
	void SetContentType(CONTENT_TYPE value)	{ m_contentType = value; }
	CONTENT_TYPE GetContentType() const		{ return m_contentType; }

public:
	//--- �ėp���l�R���e�i�̃A�N�Z�b�T

	/// �ėp���l�R���e�i�ւ̃L�[�w��ɂ�鐔�l�ݒ�
	void	SetIntValue(LPCTSTR key, int value) {
		m_IntegerMap[key] = value;
	}
	/// �ėp���l�R���e�i����̃L�[�w��ɂ�鐔�l�擾
	/// �擾���s���� default_value ��Ԃ�
	int	GetIntValue(LPCTSTR key, int default_value=-1) {
		return FindIntegerMap(key, default_value);
	}

	// author_id : ���e�҂�ID�i�I�[�i�[ID�ɐݒ肳���ꍇ������j
	void	SetAuthorID(int authorId)	{ m_IntegerMap[L"author_id"] = authorId; }
	int		GetAuthorID() const			{ return FindIntegerMap(L"author_id", -1); }

	// id : �L��ID
	void	SetID(int value)			{ m_IntegerMap[L"id"] = value; }
	int		GetID()	const				{ return FindIntegerMap(L"id", -1); }

	// comment_index : �R�����g�ԍ�
	void	SetCommentIndex(int value)	{ m_IntegerMap[L"comment_index"] = value; }
	int		GetCommentIndex() const		{ return FindIntegerMap(L"comment_index", -1); }

	// comment_count : �R�����g�̐�
	void	SetCommentCount(int value)	{ m_IntegerMap[L"comment_count"] = value; }
	int		GetCommentCount() const		{ return FindIntegerMap(L"comment_count", 0); }

	// owner_id : �I�[�i�[ID�i���e��ID�ɐݒ肳���ꍇ������j
	void	SetOwnerID(int value)		{ m_IntegerMap[L"owner_id"] = value; }
	int		GetOwnerID() const			{ return FindIntegerMap(L"owner_id", -1); }

	// my_mixi : �}�C�~�N�t���O�i�����Ƃ���̃}�C�~�N���o���̂ݑΉ��j
	void	SetMyMixi(bool bMyMixi)		{ m_IntegerMap[L"my_mixi"] = bMyMixi ? 1 : 0; }
	bool	IsMyMixi() const			{ return FindIntegerMap(L"my_mixi", 0) ? true : false; }


	//--- �ėp������z��R���e�i�̃A�N�Z�b�T

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


	//--- �ėp������R���e�i�̃A�N�Z�b�T

	/// �ėp������R���e�i�ւ̃L�[�w��ɂ�镶����ݒ�
	void	SetTextValue(LPCTSTR key, LPCTSTR value) {
		m_StringMap[key] = value;
	}
	/// �ėp������R���e�i����̃L�[�w��ɂ�镶����擾
	CString	GetTextValue(LPCTSTR key) const {
		return FindStringMap(key);
	}

	// name : ���O
	void	SetName(CString name);
	CString GetName() const					{ return FindStringMap(L"name"); }

	// author : ���e�Җ�
	void	SetAuthor(CString author);
	CString GetAuthor() const				{ return FindStringMap(L"author"); }

	// url
	void	SetURL(CString url)				{ m_StringMap[L"url"] = url; }
	CString GetURL() const					{ return FindStringMap(L"url"); }

	// title : �^�C�g��
	void	SetTitle(CString title);
	CString GetTitle() const				{ return FindStringMap(L"title"); }

	// browse_uri
	void	SetBrowseUri(LPCTSTR str)		{ m_StringMap[L"browse_uri"] = str; }
	LPCTSTR GetBrowseUri()					{ return FindStringMap(L"browse_uri"); }

	// post_address : POST�p�̃A�h���X
	void	SetPostAddress(LPCTSTR str)		{ m_StringMap[L"post_address"] = str; }
	LPCTSTR GetPostAddress()				{ return FindStringMap(L"post_address"); }

	// opening_range : ���J�͈�
	void	SetOpeningRange(LPCTSTR range)	{ m_StringMap[L"opening_range"] = range; }
	CString GetOpeningRange()				{ return FindStringMap(L"opening_range"); }

	// next_diary : ���̓��L
	void	SetNextDiary(LPCTSTR url)		{ m_StringMap[L"next_diary"] = url; }
	CString GetNextDiary()				{ return FindStringMap(L"next_diary"); }

	// prev_diary : �O�̓��L
	void	SetPrevDiary(LPCTSTR url)		{ m_StringMap[L"prev_diary"] = url; }
	CString GetPrevDiary()				{ return FindStringMap(L"prev_diary"); }

	// full_diary : �S�Ă�\��
	void	SetFullDiary(LPCTSTR url)		{ m_StringMap[L"full_diary"] = url; }
	CString GetFullDiary()				{ return FindStringMap(L"full_diary"); }

	//--- �q�v�f����
	void	ClearChildren()					{ m_children.clear(); }				///< �q�v�f�̍폜
	void	AddChild(const MZ3Data& child)	{ m_children.push_back(child); }	///< �q�v�f�̒ǉ�
	size_t	GetChildrenSize()				{ return m_children.size(); }		///< �q�v�f���̎擾
	MZ3Data&	 GetChild( int idx )		{ return m_children[idx]; }			///< �q�v�f�̎擾
	MZ3DataList& GetChildren()				{ return m_children; }				///< �q�v�f���X�g�̎擾

	//--- debug ���
	int		GetInstanceCount()				{ return g_nMZ3DataInstances; }		///< MZ3Data �C���X�^���X��

private:
	/// �ėp���l�R���e�i���� key �l���擾����B���s���� default_value ��Ԃ��B
	int FindIntegerMap(LPCTSTR key, int default_value) const
	{
		if( m_IntegerMap.size() == 0 ){
			return default_value;
		}
		const MZ3IntegerMap::const_iterator& v = m_IntegerMap.find(key);
		if (v==m_IntegerMap.end()) {
			return default_value;
		}
		return v->second;
	}

	/// �ėp������R���e�i���� key �l���擾����B���s���� L"" ��Ԃ��B
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


