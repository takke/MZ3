#pragma once

// CCategoryItem �R�}���h �^�[�Q�b�g

#include "MixiData.h"

class CCategoryItem;
typedef std::vector<CCategoryItem> CCategoryItemList;

/**
 * ���C����ʂ̃J�e�S�����X�g�ɕ\�����邽�߂̍���
 */
class CCategoryItem 
{
public:
	CMixiData		m_mixi;					///< mixi �f�[�^
	CMixiDataList	m_body;					///< ���̃J�e�S�����X�g�Ɋ֘A�Â���ꂽ�{�f�B���X�g�̃f�[�^

	int				selectedBody;			///< �{�f�B���X�g�̑I�����ڂ̃C���f�b�N�X

	CString			m_name;					///< ���X�g�ɕ\�����閼��

	/// �{�f�B�� CMixiData ���̂ǂ̍��ڂ�\�����邩�̎��ʎq
	enum BODY_INDICATE_TYPE
	{
		BODY_INDICATE_TYPE_DATE,			///< ���t��\������
		BODY_INDICATE_TYPE_TITLE,			///< �^�C�g����\������
		BODY_INDICATE_TYPE_NAME,			///< ���O��\������
		BODY_INDICATE_TYPE_BODY,			///< �{����\������
		BODY_INDICATE_TYPE_NONE,			///< �����\�����Ȃ�
	};
	BODY_INDICATE_TYPE m_firstBodyColType;	///< �P�ڂ̃J�����ɕ\�����鍀�ڂ̎��ʎq
	BODY_INDICATE_TYPE m_secondBodyColType;	///< �Q�ڂ̃J�����ɕ\�����鍀�ڂ̎��ʎq

	bool			m_bCruise;				///< ����Ώۂ��ǂ�����\���t���O

private:
	CString			m_accessTime;			///< �i�J�e�S�����X�g�Ƃ��Ắj�A�N�Z�X����
	int				m_idxItemOnList;		///< ���X�g�R���g���[�����̃C���f�b�N�X

public:
	/// �R���X�g���N�^
	CCategoryItem() 
		: selectedBody(0)
	{}

	/// ������
	void init( LPCTSTR name,
			   LPCTSTR url, ACCESS_TYPE accessType, int index, 
			   BODY_INDICATE_TYPE firstColType, BODY_INDICATE_TYPE secondColType )
	{
		m_name = name;

		m_mixi.SetURL( url );
		m_mixi.SetAccessType( accessType );

		m_idxItemOnList = index;
		m_firstBodyColType = firstColType;
		m_secondBodyColType = secondColType;
	}
	virtual ~CCategoryItem()				{}

	/// ���ݑI�𒆂̃{�f�B�A�C�e�����擾����
	CMixiData& GetSelectedBody() {
		if (selectedBody < 0 || selectedBody >= (int)m_body.size()) {
			// �_�~�[��Ԃ��B�{����NULL��Ԃ��Ȃ�AException������Ȃ肷�ׂ��B
			static CMixiData s_dummy;
			CMixiData dummy;
			s_dummy = dummy;	// ������
			return s_dummy;
		}
		return m_body[ selectedBody ];
	}

	CMixiDataList& GetBodyList()			{ return m_body; };

	void	SetAccessTime(LPCTSTR value)	{ m_accessTime = value; };
	LPCTSTR GetAccessTime()					{ return m_accessTime; };

	void	SetIndexOnList(int value)		{ m_idxItemOnList = value; };
	int		GetIndexOnList()				{ return m_idxItemOnList; };

};
