/*
 * Copyright (c) MZ3 Project Team All rights reserved.
 * Code licensed under the BSD License:
 * http://www.mz3.jp/license.txt
 */
#pragma once
#include "MixiData.h"

/// �I�v�V�����f�[�^
namespace option {

/**
 * �u�b�N�}�[�N�f�[�^�Ǘ��N���X
 */
class Bookmark
{
private:
//	CMixiDataList m_bookmark;

public:
	Bookmark();
	virtual ~Bookmark();

	void Load( CMixiDataList& bookmark );
	void Save( CMixiDataList& bookmark );

	BOOL Add( CMixiData* data, CMixiDataList& bookmark );
	BOOL Delete( CMixiData* data, CMixiDataList& bookmark );

private:
	void Devide( LPCTSTR line, CMixiDataList& bookmark );
};

}