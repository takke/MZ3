/*
 * Copyright (c) MZ3 Project Team All rights reserved.
 * Code licensed under the BSD License:
 * http://www.mz3.jp/license.txt
 */
#pragma once

// CHtmlArray コマンド ターゲット

#include "MixiData.h"
#include "PostData.h"

/**
 * HTML データ管理（＆mixiデータ解析）クラス
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


