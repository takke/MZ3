// MZ3GroupFileEditorDlg.h : ヘッダー ファイル
//

#pragma once
#include "afxcmn.h"


// CMZ3GroupFileEditorDlg ダイアログ
class CMZ3GroupFileEditorDlg : public CDialog
{
// コンストラクション
public:
	CMZ3GroupFileEditorDlg(CWnd* pParent = NULL);	// 標準コンストラクタ

// ダイアログ データ
	enum { IDD = IDD_MZ3GROUPFILEEDITOR_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV サポート

	HACCEL m_hAccelTable;

// 実装
protected:
	HICON m_hIcon;

	// 生成された、メッセージ割り当て関数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnExitApp();
	afx_msg void OnAbout();
	afx_msg void OnOpenFile();
	CListCtrl mc_listCategory;
	CTabCtrl  mc_tab;
	afx_msg void OnTcnSelchangeTab1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMRclickTab1(NMHDR *pNMHDR, LRESULT *pResult);
	bool MyReloadCategory(void);
	bool MyUpdateControlsState(void);
	afx_msg void OnLvnItemchangedCategoryList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTcnFocusChangeTab1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMSetfocusCategoryList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMKillfocusCategoryList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnAcceleratorMoveDown();
	afx_msg void OnAcceleratorMoveLeft();
	afx_msg void OnAcceleratorMoveRight();
	afx_msg void OnAcceleratorMoveUp();
	int MyGetSelectedCategory(void);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	bool MyMoveCategoryItem(int idx, int idxNew);
	afx_msg void OnAcceleratorTabLeft();
	afx_msg void OnAcceleratorTabRight();
	bool MyMoveCategoryItemToOtherTab(int idxTab, int idxTabNew);
	afx_msg void OnAcceleratorSave();
	afx_msg void OnFileSave();
	afx_msg void OnUpdateFileSave(CCmdUI *pCmdUI);
	afx_msg LRESULT OnKickIdle(WPARAM wParam, LPARAM lParam);

protected:
	virtual void OnOK();
public:
	afx_msg void OnAddTab();
	afx_msg void OnDeleteTab();
	afx_msg void OnTabMoveLeft();
	afx_msg void OnTabMoveRight();
	bool MyReloadTabs(void);
	afx_msg void OnTabRename();
	afx_msg void OnNMRclickCategoryList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnCategorySetting();
	afx_msg void OnCategoryAdd();
	afx_msg void OnCategoryDelete();
	afx_msg void OnCategoryMoveUp();
	afx_msg void OnCategoryMoveDown();
	afx_msg void OnCategoryMoveLeft();
	afx_msg void OnCategoryMoveRight();
	afx_msg void OnNMDblclkCategoryList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMClickTab1(NMHDR *pNMHDR, LRESULT *pResult);
};
