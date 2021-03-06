/*
 * Copyright (c) MZ3 Project Team All rights reserved.
 * Code licensed under the BSD License:
 * http://www.mz3.jp/license.txt
 */
// MainFrm.h : CMainFrame クラスのインターフェイス
//
#pragma once

class CMainFrame : public CFrameWnd
{
protected: // シリアル化からのみ作成します。
	CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)

public:

// 操作
public:

// オーバーライド
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

// 実装
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
#endif

public:  // コントロール バー用メンバ
#ifdef WINCE
	// WindowsMobile の場合はコマンドバー
//	CCommandBar m_wndCommandBar;
#else
	// Windows の場合はツールバー
	CToolBar    m_wndToolBar;
#endif

#ifdef WINCE
	HWND		m_hwndMenuBar;
	HMENU		m_hMenu;
#endif

	// 生成された、メッセージ割り当て関数
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);

	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnBackButton();
	afx_msg void OnForwardButton();
	afx_msg void OnAutoReloadButton();

	BOOL m_bBackPageEnabled;
	BOOL m_bForwardPageEnabled;

	inline afx_msg void OnStopButton() {
		::SendMessage(GetActiveView()->m_hWnd, WM_MZ3_ABORT, NULL, NULL);
	};
	afx_msg void OnSettingLogin();
	void OnUpdateBackButton(CCmdUI*);
	void OnUpdateForwardButton(CCmdUI*);
	void OnUpdateStopButton(CCmdUI*);
	void OnUpdateImageButton(CCmdUI*);
	void OnUpdateWriteButton(CCmdUI*);
	void OnUpdateBrowserButton(CCmdUI*);
	void OnUpdateAutoReloadButton(CCmdUI*);

	afx_msg void OnSettingGeneral();
	afx_msg void OnMenuClose();
	afx_msg void OnUpdateGetpageAll(CCmdUI *pCmdUI);
	afx_msg void OnUpdateSkinMenuItem(CCmdUI *pCmdUI);
	afx_msg void OnGetpageAll();
	afx_msg void OnGetpageLatest10();
	afx_msg void OnUpdateGetpageLatest10(CCmdUI *pCmdUI);
	bool ChangeAllViewFont(int fontHeight=-1);
	afx_msg void OnChangeFontNormal();
	afx_msg void OnUpdateMenuBack(CCmdUI *pCmdUI);
	afx_msg void OnUpdateMenuNext(CCmdUI *pCmdUI);
	afx_msg void OnMenuBack();
	afx_msg void OnMenuNext();
	afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
	afx_msg void OnCheckNew();
	afx_msg void OnHelpMenu();
	afx_msg void OnHistoryMenu();
	afx_msg void OnStartCruise();
	afx_msg void OnStartCruiseUnreadOnly();
//	afx_msg void OnOpenMixiMobileByBrowser();
	afx_msg void OnErrorlogMenu();

	afx_msg void OnChangeSkin();

	void GetSkinFolderNameList(std::vector<std::wstring>& skinfileList);
	void OnSkinMenuItem(UINT nID);
	void OnSelectCategory(UINT nID);
	afx_msg void OnMenuAction();
	afx_msg void OnDestroy();
	afx_msg void OnEnableIntervalCheck();
	afx_msg void OnUpdateEnableIntervalCheck(CCmdUI *pCmdUI);
	afx_msg void OnCheckUpdate();
	afx_msg void OnUpdateCheckUpdate(CCmdUI *pCmdUI);
protected:
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
public:
	void MySetTitle(void);
	afx_msg void OnMove(int x, int y);
	afx_msg void OnDownloadManagerView();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnMenuOpenUrl();
	afx_msg void OnMenuOpenLocalFile();
	afx_msg void OnMenuFontMagnify();
	afx_msg void OnMenuFontShrink();
	afx_msg void OnMenuReloadLuaScripts();
	afx_msg void OnMenuReloadCategoryListLog();
	afx_msg void OnUpdateMenuStop(CCmdUI *pCmdUI);
	afx_msg void OnMenuStop();
//	afx_msg void OnWinIniChange(LPCTSTR lpszSection);
	afx_msg void OnMenuOpenMenu();
#ifndef WINCE
	NOTIFYICONDATA m_notifyIconData;
	afx_msg LONG OnTrayIcon ( WPARAM wParam, LPARAM lParam );
#endif
};


