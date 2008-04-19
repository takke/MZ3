/*
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */
#pragma once

#include "GroupItem.h"

/// MZ3 �̃O���[�v�r���[��1��1�őΉ�����f�[�^�\��
class Mz3GroupData {
public:
	std::vector<CGroupItem>	groups;				///< �J�e�S�����X�g�̍��ڌQ

public:
	/// �R���X�g���N�^
	Mz3GroupData() {
	}

public:
	/// groups ����w�肳�ꂽ���X�g��ʂ̍��ڂ�T���A���̍��ڂ�Ԃ��B
	/// ������Ȃ���� NULL ��Ԃ��B
	CCategoryItem* GetCategoryItem( ACCESS_TYPE accessType )
	{
		for( size_t ig=0; ig<groups.size(); ig++ ) {
			CCategoryItemList& categories = groups[ig].categories;
			for( size_t i=0; i<categories.size(); i++ ) {
				CCategoryItem& item = categories[i];
				if( item.m_mixi.GetAccessType() == accessType ) {
					return &item;
				}
			}
		}
		return NULL;
	}

	/// groups ����u�b�N�}�[�N���ڂ�T���A������΂��̍��ڂ�Ԃ��B
	/// ������Ȃ���Η�O�𓊂���
	CMixiDataList&	GetBookmarkList() {
		CCategoryItem* pCategoryItem = GetCategoryItem( ACCESS_LIST_BOOKMARK );
		if( pCategoryItem == NULL ) {
			throw L"Not Found the Requested Bookmark...";
		}else{
			return pCategoryItem->GetBodyList();
		}
	}

	bool initForTopPage();

	bool appendCategoryByIniData( CGroupItem& group, const std::string& category_name, ACCESS_TYPE category_type, const char* category_url=NULL, bool bCruise=false );
};

/// Mz3GroupData �ƃO���[�v��`�t�@�C���Ƃ̃f�[�^�����p�f�[�^�\��
class Mz3GroupDataInifileHelper {

private:
	/// �O���[�v��ʕ����� �� �O���[�v��� �}�b�v
	CMap<CString,LPCTSTR,ACCESS_TYPE,ACCESS_TYPE> group_string2type;

	/// �O���[�v��� �� �O���[�v��ʕ����� �}�b�v
	CMap<ACCESS_TYPE,ACCESS_TYPE,CString,LPCTSTR> group_type2string;

	/// �J�e�S����ʕ����� �� �J�e�S����� �}�b�v
	CMap<CString,LPCTSTR,ACCESS_TYPE,ACCESS_TYPE> category_string2type;

	/// �J�e�S����� �� �J�e�S����ʕ����� �}�b�v
	CMap<ACCESS_TYPE,ACCESS_TYPE,CString,LPCTSTR> category_type2string;

public:
	/**
	 * �R���X�g���N�^�B
	 *
	 * map �̏��������s���B
	 */
	Mz3GroupDataInifileHelper() {
		InitMap();
	}

private:
	/**
	 * ���L�̃}�b�v�𐶐�����B
	 *
	 * �O���[�v��ʕ����� ���� �O���[�v���
	 * �J�e�S����ʕ����� ���� �J�e�S�����
	 */
	void InitMap() {
		group_string2type.RemoveAll();
		group_string2type.InitHashTable( 10 );
		group_string2type[ L"MYDIARY"   ] = ACCESS_GROUP_MYDIARY;
		group_string2type[ L"COMMUNITY" ] = ACCESS_GROUP_COMMUNITY;
		group_string2type[ L"MESSAGE"   ] = ACCESS_GROUP_MESSAGE;
		group_string2type[ L"NEWS"      ] = ACCESS_GROUP_NEWS;
		group_string2type[ L"OTHERS"    ] = ACCESS_GROUP_OTHERS;
		group_string2type[ L"TWITTER"   ] = ACCESS_GROUP_TWITTER;

		group_type2string.RemoveAll();
		group_type2string.InitHashTable( 10 );
		group_type2string[ ACCESS_GROUP_MYDIARY   ] = L"MYDIARY";
		group_type2string[ ACCESS_GROUP_COMMUNITY ] = L"COMMUNITY";
		group_type2string[ ACCESS_GROUP_MESSAGE   ] = L"MESSAGE";
		group_type2string[ ACCESS_GROUP_NEWS      ] = L"NEWS";
		group_type2string[ ACCESS_GROUP_OTHERS    ] = L"OTHERS";
		group_type2string[ ACCESS_GROUP_TWITTER   ] = L"TWITTER";

		category_string2type.RemoveAll();
		category_string2type.InitHashTable( 20 );
		category_string2type[ L"MYDIARY"     ] = ACCESS_LIST_MYDIARY;
		category_string2type[ L"DIARY"       ] = ACCESS_LIST_DIARY;
		category_string2type[ L"NEW_COMMENT" ] = ACCESS_LIST_NEW_COMMENT;
		category_string2type[ L"BBS"         ] = ACCESS_LIST_NEW_BBS;
		category_string2type[ L"MESSAGE_IN"  ] = ACCESS_LIST_MESSAGE_IN;
		category_string2type[ L"MESSAGE_OUT" ] = ACCESS_LIST_MESSAGE_OUT;
		category_string2type[ L"FOOTSTEP"    ] = ACCESS_LIST_FOOTSTEP;
		category_string2type[ L"COMMENT"     ] = ACCESS_LIST_COMMENT;
		category_string2type[ L"NEWS"        ] = ACCESS_LIST_NEWS;
		category_string2type[ L"BOOKMARK"    ] = ACCESS_LIST_BOOKMARK;
		category_string2type[ L"FAVORITE"    ] = ACCESS_LIST_FAVORITE;
		category_string2type[ L"FRIEND"      ] = ACCESS_LIST_FRIEND;
		category_string2type[ L"INTRO"       ] = ACCESS_LIST_INTRO;
		category_string2type[ L"COMMUNITY"   ] = ACCESS_LIST_COMMUNITY;
		category_string2type[ L"NEW_BBS_COMMENT" ] = ACCESS_LIST_NEW_BBS_COMMENT;
		category_string2type[ L"CALENDAR"    ] = ACCESS_LIST_CALENDAR;  //icchu�ǉ�
		// Twitter
		category_string2type[ L"TWITTER_FRIENDS_TIMELINE" ] = ACCESS_TWITTER_FRIENDS_TIMELINE;
		category_string2type[ L"TWITTER_DIRECT_MESSAGES" ] = ACCESS_TWITTER_DIRECT_MESSAGES;

		category_type2string.RemoveAll();
		category_type2string.InitHashTable( 20 );
		category_type2string[ ACCESS_LIST_MYDIARY     ] = L"MYDIARY";
		category_type2string[ ACCESS_LIST_DIARY       ] = L"DIARY";
		category_type2string[ ACCESS_LIST_NEW_COMMENT ] = L"NEW_COMMENT";
		category_type2string[ ACCESS_LIST_NEW_BBS     ] = L"BBS";
		category_type2string[ ACCESS_LIST_MESSAGE_IN  ] = L"MESSAGE_IN";
		category_type2string[ ACCESS_LIST_MESSAGE_OUT ] = L"MESSAGE_OUT";
		category_type2string[ ACCESS_LIST_FOOTSTEP    ] = L"FOOTSTEP";
		category_type2string[ ACCESS_LIST_COMMENT     ] = L"COMMENT";
		category_type2string[ ACCESS_LIST_NEWS        ] = L"NEWS";
		category_type2string[ ACCESS_LIST_BOOKMARK    ] = L"BOOKMARK";
		category_type2string[ ACCESS_LIST_FAVORITE    ] = L"FAVORITE";
		category_type2string[ ACCESS_LIST_FRIEND      ] = L"FRIEND";
		category_type2string[ ACCESS_LIST_INTRO       ] = L"INTRO";
		category_type2string[ ACCESS_LIST_COMMUNITY   ] = L"COMMUNITY";
		category_type2string[ ACCESS_LIST_NEW_BBS_COMMENT ] = L"NEW_BBS_COMMENT";
		category_type2string[ ACCESS_LIST_CALENDAR    ] = L"CALENDAR";  //icchu�ǉ�
		// Twitter
		category_type2string[ ACCESS_TWITTER_FRIENDS_TIMELINE ] = L"TWITTER_FRIENDS_TIMELINE";
		category_type2string[ ACCESS_TWITTER_DIRECT_MESSAGES ] = L"TWITTER_DIRECT_MESSAGES";
	}

public:

	/**
	 * �O���[�v��ʈꗗ�̎擾
	 */
	std::vector<ACCESS_TYPE> GetGroupTypeList()
	{
		std::vector<ACCESS_TYPE> list;
		POSITION pos = group_type2string.GetStartPosition();

		ACCESS_TYPE key;
		CString     value;
		while (pos != NULL) {
			group_type2string.GetNextAssoc( pos, key, value );

			list.push_back( key );
		}

		return list;
	}

	/**
	 * �J�e�S����ʈꗗ�̎擾
	 */
	std::vector<ACCESS_TYPE> GetCategoryTypeList()
	{
		std::vector<ACCESS_TYPE> list;
		POSITION pos = category_type2string.GetStartPosition();

		ACCESS_TYPE key;
		CString     value;
		while (pos != NULL) {
			category_type2string.GetNextAssoc( pos, key, value );

			list.push_back( key );
		}

		return list;
	}

	/**
	 * �O���[�v��� �� �O���[�v��ʕ����� �ϊ�
	 */
	LPCTSTR GroupType2String( ACCESS_TYPE group_type ) {

		// �v�f���Ȃ���� NULL ��Ԃ��B
		CString value;
		if( group_type2string.Lookup( group_type, value ) == FALSE ) {
			return NULL;
		}

		// �}�b�v���當����ϊ�
		return value;
	}

	/**
	 * �O���[�v��ʕ����� �� �O���[�v��� �ϊ�
	 */
	ACCESS_TYPE GroupString2Type( LPCTSTR group_string ) {
		// �v�f���Ȃ���� ACCESS_INVALID ��Ԃ��B
		ACCESS_TYPE value;
		if( group_string2type.Lookup( group_string, value ) == FALSE ) {
			return ACCESS_INVALID;
		}

		// �}�b�v�����ʕϊ�
		return value;
	}

	/**
	 * �J�e�S����� �� �J�e�S����ʕ����� �ϊ�
	 */
	LPCTSTR CategoryType2String( ACCESS_TYPE category_type ) {

		// �v�f���Ȃ���� NULL ��Ԃ��B
		CString value;
		if( category_type2string.Lookup( category_type, value ) == FALSE ) {
			return NULL;
		}

		// �}�b�v���當����ϊ�
		return value;
	}

	/**
	 * �J�e�S����ʕ����� �� �J�e�S����� �ϊ�
	 */
	ACCESS_TYPE CategoryString2Type( LPCTSTR category_string ) {
		// �v�f���Ȃ���� ACCESS_INVALID ��Ԃ��B
		ACCESS_TYPE value;
		if( category_string2type.Lookup( category_string, value ) == FALSE ) {
			return ACCESS_INVALID;
		}

		// �}�b�v�����ʕϊ�
		return value;
	}
};


/// Mz3GroupData ���O���[�v��`�t�@�C���ɏo�͂���N���X
class Mz3GroupDataWriter {
public:
	static bool save( const Mz3GroupData& target, const CString& inifilename );
};

/// Mz3GroupData ���O���[�v��`�t�@�C������\�z����N���X
class Mz3GroupDataReader {
public:
	static bool load( Mz3GroupData& target, const CString& inifilename );
};

