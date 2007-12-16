/*
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
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


