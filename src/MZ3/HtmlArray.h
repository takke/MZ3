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

	bool		GetPostConfirmData(CPostData*);
	BOOL		IsPostSucceeded(WRITEVIEW_TYPE type);
};


