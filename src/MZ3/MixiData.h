#pragma once

// CMixiData �R�}���h �^�[�Q�b�g

#include "constants.h"
#include <vector>

class CMixiData;
typedef std::vector<CMixiData> CMixiDataList;

/**
 * mixi �̃f�[�^�Ǘ��N���X
 */
class CMixiData
{
protected:
	ACCESS_TYPE		m_accessType;			///< �A�N�Z�X��ʁi�Ƃ����������̃I�u�W�F�N�g�̎�ʁj
	CString			m_date;					///< ���t����
	CString			m_name;					///< ���O
	CString			m_author;				///< ���e�Җ�
	int				m_authorId;				///< ���e�҂�ID�i�I�[�i�[ID�ɐݒ肳���ꍇ������j
	CString			m_title;				///< �^�C�g��
	CString			m_url;					///< URL
	CString			m_postAddress;			///< �o�n�r�s�p�̃A�h���X
	int				m_id;					///< �L��ID
	int				m_commId;				///< �R�����gID
	int				m_commentIndex;			///< �R�����g�ԍ�
	int				m_commentCount;			///< �R�����g�̐�
	CONTENT_TYPE	m_contentType;			///< Content-Type

	int				m_ownerId;				///< �I�[�i�[ID�i���e��ID�ɐݒ肳���ꍇ������j

	std::vector<CString>	m_bodyArray;	///< �{���i�R�����g���̖{���j
	std::vector<CString>	m_imageArray;	///< �摜�̃��X�g
	std::vector<CString>	m_MovieArray;	///< ����̃��X�g

	CMixiDataList	m_children;				///< �q�v�f�i�R�����g�Ȃǁj
											///< �R�~���j�e�B�y�[�W�v�f�̏ꍇ�́A
											///< �v�f0�� list_bbs.pl �̃f�[�^�����B

	BOOL			m_otherDiary;			///< �O���u���O
	CString			m_browseUri;
	bool			m_myMixi;				///< �}�C�~�N�t���O�i�����Ƃ���̃}�C�~�N���o���̂ݑΉ��j

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
	CMixiData()
		: m_accessType(ACCESS_INVALID) // �����l
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
	 * �f�X�g���N�^
	 */
	virtual ~CMixiData()
	{
		ClearChildren();
		m_imageArray.clear();
		m_MovieArray.clear();
	}

public:

	/// �S���X�g�̏�����
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
		// ���l�ϊ�
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

	//--- �q�v�f����
	void	AddChild(const CMixiData& child)
									{ m_children.push_back(child); }	///< �q�v�f�̒ǉ�
	CMixiData& GetChild( int idx )	{ return m_children[idx]; }			///< �q�v�f�̎擾
	size_t	GetChildrenSize()		{ return m_children.size(); }		///< �q�v�f���̎擾
	CMixiDataList& GetChildren()	{ return m_children; }				///< �q�v�f���X�g�̎擾
	void	ClearChildren()			{ m_children.clear(); }				///< �q�v�f�̍폜
};


