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

	struct InitializeType
	{
		bool bUseMixi;		///< mixi
		bool bUseTwitter;	///< Twitter

		InitializeType(bool a_bUseMixi=true, bool a_bUseTwitter=true)
			: bUseMixi(a_bUseMixi)
			, bUseTwitter(a_bUseTwitter)
		{
		}
	};

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

	bool initForTopPage(AccessTypeInfo& accessTypeInfo, const InitializeType initType);

	bool appendCategoryByIniData( AccessTypeInfo& accessTypeInfo, 
								  CGroupItem& group, 
								  const std::string& category_name, 
								  ACCESS_TYPE category_type, 
								  const char* category_url=NULL, 
								  bool bCruise=false );
};

/// Mz3GroupData �ƃO���[�v��`�t�@�C���Ƃ̃f�[�^�����p�f�[�^�\��
class Mz3GroupDataInifileHelper {

private:
	/// �O���[�v��ʕ����� �� �O���[�v��� �}�b�v
	CMap<CStringA,LPCSTR,ACCESS_TYPE,ACCESS_TYPE> group_string2type;

	/// �J�e�S����ʕ����� �� �J�e�S����� �}�b�v
	CMap<CStringA,LPCSTR,ACCESS_TYPE,ACCESS_TYPE> category_string2type;

public:
	/**
	 * �R���X�g���N�^�B
	 *
	 * map �̏��������s���B
	 */
	Mz3GroupDataInifileHelper(AccessTypeInfo& accessTypeInfo) {
		InitMap(accessTypeInfo);
	}

private:
	void InitMap(AccessTypeInfo& accessTypeInfo);

public:

	/**
	 * �O���[�v��ʕ����� �� �O���[�v��� �ϊ�
	 */
	ACCESS_TYPE GroupString2Type( LPCSTR group_string ) {
		// �v�f���Ȃ���� ACCESS_INVALID ��Ԃ��B
		ACCESS_TYPE value;
		if( group_string2type.Lookup( group_string, value ) == FALSE ) {
			return ACCESS_INVALID;
		}

		// �}�b�v�����ʕϊ�
		return value;
	}

	/**
	 * �J�e�S����ʕ����� �� �J�e�S����� �ϊ�
	 */
	ACCESS_TYPE CategoryString2Type( LPCSTR category_string ) {
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
	static bool save( AccessTypeInfo& accessTypeInfo, const Mz3GroupData& target, const CString& inifilename );
};

/// Mz3GroupData ���O���[�v��`�t�@�C������\�z����N���X
class Mz3GroupDataReader {
public:
	static bool load( AccessTypeInfo& accessTypeInfo, Mz3GroupData& target, const CString& inifilename );
};

