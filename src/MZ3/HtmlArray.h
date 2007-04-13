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

	bool		GetPostConfirmData(CPostData*);
	BOOL		IsPostSucceeded(WRITEVIEW_TYPE type);
};


