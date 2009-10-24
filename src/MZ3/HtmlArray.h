/*
 * Copyright (c) MZ3 Project Team All rights reserved.
 * Code licensed under the BSD License:
 * http://www.mz3.jp/license.txt
 */
#pragma once

// CHtmlArray �R�}���h �^�[�Q�b�g

#include "MixiData.h"
#include "PostData.h"

/**
 * HTML �f�[�^�Ǘ��i��mixi�f�[�^��́j�N���X
 */
class CHtmlArray : public CStringArray
{
public:
	CHtmlArray();
	virtual ~CHtmlArray();

public:
	void		Load( LPCTSTR szHtmlFilename );

	void		TranslateToVectorBuffer( std::vector<TCHAR>& text ) const;

	bool		GetPostConfirmData(CPostData*);
	BOOL		IsPostSucceeded(WRITEVIEW_TYPE type);
};


