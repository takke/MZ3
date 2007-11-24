// MZ3GroupFileEditor.h : PROJECT_NAME アプリケーションのメイン ヘッダー ファイルです。
//

#pragma once

#ifndef __AFXWIN_H__
	#error "PCH に対してこのファイルをインクルードする前に 'stdafx.h' をインクルードしてください"
#endif

#include "resource.h"		// メイン シンボル
#include "../MZ3/Mz3GroupData.h"

// NOP
#define MZ3LOGGER_ERROR(msg) ;

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

// 実装

	DECLARE_MESSAGE_MAP()
};

extern CMZ3GroupFileEditorApp theApp;