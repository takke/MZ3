/*
 * Copyright (c) MZ3 Project Team All rights reserved.
 * Code licensed under the BSD License:
 * http://www.mz3.jp/license.txt
 */
// MZ3GroupFileEditor.h : PROJECT_NAME アプリケーションのメイン ヘッダー ファイルです。
//

#pragma once

#ifndef __AFXWIN_H__
	#error "PCH に対してこのファイルをインクルードする前に 'stdafx.h' をインクルードしてください"
#endif

#include "resource.h"		// メイン シンボル

// NOP
#define MZ3LOGGER_ERROR(msg) ;

#include "../MZ3/Mz3GroupData.h"
#include "../MZ3/AccessTypeInfo.h"

// CMZ3GroupFileEditorApp:
// このクラスの実装については、MZ3GroupFileEditor.cpp を参照してください。
//

class CMZ3GroupFileEditorApp : public CWinApp
{
public:
	CMZ3GroupFileEditorApp();

// オーバーライド
public:
	virtual BOOL InitInstance();

public:
	Mz3GroupData m_group_info;
	CString		 m_strGroupInifilePath;
	AccessTypeInfo	m_accessTypeInfo;	///< アクセス種別毎のMZ3/4の振る舞いを定義する情報

// 実装

	DECLARE_MESSAGE_MAP()
};

extern CMZ3GroupFileEditorApp theApp;