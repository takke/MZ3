/*
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */
#pragma once
#include "MixiData.h"

/// オプションデータ
namespace option {

/**
 * ブックマークデータ管理クラス
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