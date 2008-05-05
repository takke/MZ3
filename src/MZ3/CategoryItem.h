/*
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */
#pragma once

// CCategoryItem �R�}���h �^�[�Q�b�g

#include "MixiData.h"
#include "AccessTypeInfo.h"

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

	AccessTypeInfo::BODY_INDICATE_TYPE m_firstBodyColType;	///< �P�ڂ̃J�����ɕ\�����鍀�ڂ̎��ʎq
	AccessTypeInfo::BODY_INDICATE_TYPE m_secondBodyColType;	///< �Q�ڂ̃J�����ɕ\�����鍀�ڂ̎��ʎq

	bool			m_bCruise;				///< ����Ώۂ��ǂ�����\���t���O

	bool			m_bFromLog;				///< ���O���擾�����l���ǂ�����\���t���O

	bool			bSaveToGroupFile;		///< �O���[�v��`�t�@�C���ɕۑ����邩�i�i�������邩�j

private:
	SYSTEMTIME		m_accessTime;			///< �i�J�e�S�����X�g�Ƃ��Ắj�A�N�Z�X����
	int				m_idxItemOnList;		///< ���X�g�R���g���[�����̃C���f�b�N�X

public:
	/// �R���X�g���N�^
	CCategoryItem() 
		: selectedBody(0)
		, m_bFromLog(0)
		, bSaveToGroupFile(true)
	{
		memset( &m_accessTime, 0, sizeof(SYSTEMTIME) );
	}

	enum SAVE_TO_GROUPFILE {
		SAVE_TO_GROUPFILE_YES = 1,
		SAVE_TO_GROUPFILE_NO = 2,
	};

	/// ������
	void init( LPCTSTR name,
			   LPCTSTR url, ACCESS_TYPE accessType, int index, 
			   AccessTypeInfo::BODY_INDICATE_TYPE firstColType, AccessTypeInfo::BODY_INDICATE_TYPE secondColType,
			   SAVE_TO_GROUPFILE saveToGroupFile=SAVE_TO_GROUPFILE_YES )
	{
		m_name = name;

		m_mixi.SetURL( url );
		m_mixi.SetAccessType( accessType );

		m_idxItemOnList = index;
		m_firstBodyColType = firstColType;
		m_secondBodyColType = secondColType;

		bSaveToGroupFile = saveToGroupFile == SAVE_TO_GROUPFILE_YES ? true : false;
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

	void	SetAccessTime(const SYSTEMTIME& value)	{ m_accessTime = value; };
	const SYSTEMTIME& GetAccessTime()				{ return m_accessTime; };
	CString GetAccessTimeString() {
		SYSTEMTIME zero;
		memset( &zero, 0, sizeof(SYSTEMTIME) );
		if (memcmp(&m_accessTime,&zero, sizeof(SYSTEMTIME))==0) {
			return L"";
		}
		if (m_bFromLog) {
			return CTime(m_accessTime).Format( L"%m/%d %H:%M:%S log" );
		} else {
			return CTime(m_accessTime).Format( L"%m/%d %H:%M:%S" );
		}
	}

	void	SetIndexOnList(int value)		{ m_idxItemOnList = value; };
	int		GetIndexOnList()				{ return m_idxItemOnList; };

};
