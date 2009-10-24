/*
 * Copyright (c) MZ3 Project Team All rights reserved.
 * Code licensed under the BSD License:
 * http://www.mz3.jp/license.txt
 */
#pragma once
/**
 * バージョン情報画面
 */

#ifndef __AFXWIN_H__
#error "PCH のこのファイルをインクルードする前に、'stdafx.h' をインクルードします"
#endif

#include "resourceppc.h"


/**
 * MZ3 のバージョン情報
 */
class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// ダイアログ データ
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート

// 実装
protected:
	afx_msg void OnSize(UINT /*nType*/, int /*cx*/, int /*cy*/);
	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnStnClickedStatic1();
	afx_msg void OnBnClickedResetTotalRecvBytesButton();
	void SetTotalBytes(void);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	void MyReplaceText(void);
};

