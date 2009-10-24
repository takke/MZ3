/*
 * Copyright (c) MZ3 Project Team All rights reserved.
 * Code licensed under the BSD License:
 * http://www.mz3.jp/license.txt
 */
#pragma once

// CGroupItem �R�}���h �^�[�Q�b�g

#include "MixiData.h"
#include "CategoryItem.h"

/**
 * ���C����ʂ̃O���[�v���X�g�ɕ\�����邽�߂̍���
 */
class CGroupItem 
{
public:
	CMixiData			mixi;				///< mixi �f�[�^
	CCategoryItemList	categories;			///< ���̃O���[�v���ڂɊ֘A�Â���ꂽ�J�e�S�����X�g�Q
	CString				name;				///< ���X�g�ɕ\�����閼��
	int					selectedCategory;	///< categories ���ɂ�����I�����ڂ̃C���f�b�N�X
	int					focusedCategory;	///< categories ���ɂ�����t�H�[�J�X���ڂ̃C���f�b�N�X
	bool				bSaveToGroupFile;	///< �O���[�v��`�t�@�C���ɕۑ����邩�i�i�������邩�j

public:
	/// �R���X�g���N�^
	CGroupItem()
		: selectedCategory(0)
		, focusedCategory(0)
		, bSaveToGroupFile(true)
	{}

	/// ������
	void init( LPCTSTR group_name, LPCTSTR url, ACCESS_TYPE accessType )
	{
		categories.clear();

		name = group_name;
		mixi.SetURL( url );
		mixi.SetAccessType( accessType );
	}

	/// �t�H�[�J�X�J�e�S���̎擾
	CCategoryItem* getFocusedCategory() {
		if( focusedCategory < 0 || focusedCategory >= (int)categories.size() ) {
			return NULL;
		}
		return &categories[ focusedCategory ];
	}

	/// �I���J�e�S���̎擾
	CCategoryItem* getSelectedCategory() {
		if( selectedCategory < 0 || selectedCategory >= (int)categories.size() ) {
			return NULL;
		}
		return &categories[ selectedCategory ];
	}

	virtual ~CGroupItem()					{}
};
