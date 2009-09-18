/*
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */
// MZ3View.h : CMZ3View クラスのインターフェイス
//

#pragma once
#include "afxcmn.h"
#include "afxwin.h"
#include "MixiData.h"
#include "CategoryItem.h"
#include "CategoryListCtrl.h"
#include "BodyListCtrl.h"
#include "GroupTabCtrl.h"
#include "PostData.h"
#include "InetAccess.h"
#include "XcrawlCanceler.h"

#define TIMERID_INTERVAL_CHECK	101
class CMZ3Doc;

/**
 * MZ3 のメインビュー
 */
class CMZ3View : public CFormView
{
public:
	//--- UI 要素
	CGroupTabCtrl		m_groupTab;		///< グループタブ。ItemData は theApp.m_root.groups のインデックス
	CCategoryListCtrl	m_categoryList;	///< カテゴリリスト。ItemData は theApp.m_root.groups[].categories のインデックス
	CBodyListCtrl		m_bodyList;		///< ボディリスト。
	CEdit				m_infoEdit;		///< 情報表示用エディットボックス
	CProgressCtrl		mc_progressBar;	///< プログレスバー
	CEdit				m_statusEdit;	///< 入力領域。

	XcrawlCanceler		m_xcrawl;				///< Xcrawl 制御
	int					m_nKeydownRepeatCount;	///< WM_KEYDOWN の回数

	bool				m_bReloadingGroupTabByThread;	///< スレッドによるタブ切り替え中か？
	bool				m_bRetryReloadGroupTabByThread;	///<

	/// 表示スタイル
	enum VIEW_STYLE {
		VIEW_STYLE_DEFAULT = 0,					///< 標準スタイル
		VIEW_STYLE_IMAGE   = 1,					///< 標準スタイル+ImageIcon
		VIEW_STYLE_TWITTER = 2,					///< 標準スタイル+ImageIcon+StatusEdit (Twitter)
	};
	VIEW_STYLE			m_viewStyle;			///< 表示スタイル

	ACCESS_TYPE			m_twitterPostAccessType;///< Twitter 風送信アクセス種別


	CRect				m_rectIcon;				///< アイコン表示領域のRECT

	BOOL				m_abort;				///< 中断フラグ

private:
	bool				m_bModifyingBodyList;	///< SetBodyList で Body リスト生成中？
	bool				m_checkNewComment;		///< 新着コメント＆メッセージチェック中？

	/**
	 * 巡回用データ
	 *
	 * 巡回処理手順：
	 * <ul>
	 * <li>巡回開始
	 * <li>巡回対象カテゴリ探索
	 *  <ul>
	 *  <li>グループ／カテゴリを順に探索し、巡回予約フラグが1のものを探す。
	 *  <li>巡回予約フラグが1のものがなければ終了する。
	 *  </ul>
	 * <li>巡回する。
	 * <li>巡回対象カテゴリを+1する。
	 * <li>巡回対象カテゴリ探索にもどる
	 * </ul>
	 */
	class CruiseInfo {
	public:
		/// 巡回状態
		enum CRUISE_STATE {
			CRUISE_STATE_STOP,	///< 巡回停止中（初期状態）
			CRUISE_STATE_LIST,	///< リスト取得中
			CRUISE_STATE_BODY,	///< ボディ要素巡回中
		};
		/// 巡回状態
		CRUISE_STATE state;

		int targetBodyItem;			///< 現在の巡回対象ボディアイテムのインデックス

		// 予約巡回関連
		int targetGroupIndex;		///< 現在の巡回対象グループアイテムのインデックス
		int targetCategoryIndex;	///< 現在の巡回対象カテゴリアイテムのインデックス
		bool autoCruise;			///< 予約巡回モード？
		bool unreadOnly;			///< 未読のみ巡回モード

		CruiseInfo() : state(CRUISE_STATE_STOP), targetBodyItem(0), targetGroupIndex(0), targetCategoryIndex(0), 
			autoCruise(false), unreadOnly(false) {}

		/// 巡回中かどうかを返す
		bool enable() {
			return state != CRUISE_STATE_STOP;
		}

	private:
		void init() {
			targetGroupIndex = 0;
			targetCategoryIndex = 0;
			targetBodyItem = 0;
		}

	public:

		bool start() {
			state = CRUISE_STATE_LIST;
			init();
			return true;
		}

		bool backToListCruise() {
			state = CRUISE_STATE_LIST;
			return true;
		}

		bool startBodyCruise() {
			state = CRUISE_STATE_BODY;
			targetBodyItem = 0;
			return true;
		}

		bool finish() {
			state = CRUISE_STATE_STOP;
			init();
			return true;
		}

		bool stop() {
			state = CRUISE_STATE_STOP;
			init();
			return true;
		}

		bool isFetchListMode() { return state == CRUISE_STATE_LIST; } ///< リスト取得モード？
		bool isFetchBodyMode() { return state == CRUISE_STATE_BODY; } ///< ボディ取得モード？
	};
	CruiseInfo		m_cruise;				///< 巡回情報

public:
	CGroupItem*		m_selGroup;				///< 現在選択されているグループタブ項目

private:
	int				m_preCategory;			///< 前回選択していたカテゴリリスト項目のインデックス
	CListCtrl*		m_hotList;				///< 現在選択中のリストコントロール（他ビューからの復帰時に利用）

	CImageList		m_iconImageListSmall;		///< アイコン用画像リスト(16x16)
	CImageList		m_iconImageListLarge;		///< アイコン用画像リスト(32x32)

	/// RETURN キーを押下した時刻
	DWORD			m_dwLastReturn;

	DWORD			m_dwIntervalTimerStartMsec;	///< 定期取得用タイマーの開始時刻
	CMenu*			m_pCategorySubMenuList;		///< カテゴリ用メニュー
	bool			m_bImeCompositioning;		///< IME変換中フラグ

#ifndef WINCE
	bool	m_bDragging;			///< マウスドラッグ中
#endif
	CPoint	m_ptDragStart;			///< ドラッグ開始位置
	int		m_nOriginalH1;			///< ドラッグ開始時のH1
	int		m_nOriginalH2;			///< ドラッグ開始時のH2

public: // シリアル化からのみ作成します。
	CMZ3View();
	DECLARE_DYNCREATE(CMZ3View)

public:
	enum{ IDD = IDD_MZ3_FORM };

// 属性
public:
	CMZ3Doc* GetDocument() const;

// 操作
public:

// オーバーライド
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL PreTranslateMessage(MSG* pMsg);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート
	virtual void OnInitialUpdate(); // 構築後に初めて呼び出されます。
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);

// 実装
public:
	virtual ~CMZ3View();
#ifdef _DEBUG
	virtual void AssertValid() const;
#endif

protected:

// 生成された、メッセージ割り当て関数
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnNMClickCategoryList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMDblclkCategoryList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLvnItemchangedCategoryList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg LRESULT OnGetEnd(WPARAM, LPARAM);
	afx_msg LRESULT OnPostEnd(WPARAM, LPARAM);
	afx_msg LRESULT OnGetEndBinary(WPARAM, LPARAM);
	afx_msg LRESULT OnGetError(WPARAM, LPARAM);
	afx_msg LRESULT OnGetAbort(WPARAM, LPARAM);
	afx_msg LRESULT OnAccessInformation(WPARAM, LPARAM);
	afx_msg LRESULT OnAccessLoaded(WPARAM, LPARAM);
	afx_msg LRESULT OnChangeView(WPARAM, LPARAM);
	afx_msg LRESULT OnAbort(WPARAM, LPARAM);
	afx_msg void OnWriteButton();
	afx_msg void OnEnSetfocusInfoEdit();
	afx_msg void OnNMDblclkBodyList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLvnItemchangedBodyList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnWriteDiary();
	afx_msg void OnNMSetfocusBodyList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnOpenBrowser();
	afx_msg void OnNMSetfocusHeaderList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnShowDebugInfo();
	afx_msg void OnGetAll();
	afx_msg void OnGetLast10();
	afx_msg void OnHdnItemclickBodyList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTcnSelchangeGroupTab(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnOpenBrowserUser();
	afx_msg void OnOpenIntro();
	afx_msg void OnOpenSelfintro();
	afx_msg void OnSetNoRead();
	afx_msg void OnViewBbsList();
	afx_msg void OnViewBbsListLog();
	afx_msg void OnCruise();
	afx_msg void OnCheckCruise();
	afx_msg void OnSendNewMessage();
	afx_msg void OnHdnEndtrackHeaderList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnSettingChange(UINT uFlags, LPCTSTR lpszSection);
	afx_msg void OnLayoutCategoryMakeNarrow();
	afx_msg void OnLayoutCategoryMakeWide();
	afx_msg void OnNMRclickHeaderList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMRclickBodyList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnNMClickGroupTab(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg LRESULT OnHideView(WPARAM wParam, LPARAM lParam);
	afx_msg void OnBnClickedUpdateButton();
	afx_msg void OnAcceleratorFontMagnify();
	afx_msg void OnAcceleratorFontShrink();
	afx_msg void OnAcceleratorContextMenu();
	afx_msg void OnAcceleratorNextTab();
	afx_msg void OnAcceleratorPrevTab();
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnSetRead();
	afx_msg void OnAcceleratorReload();
	afx_msg void OnLoadUrl(UINT);
	afx_msg void OnPaint();
	afx_msg void OnNMRclickGroupTab(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTabmenuDelete();
	afx_msg void OnTcnKeydownGroupTab(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnAppendCategoryMenu(UINT nID);
	afx_msg void OnRemoveCategoryItem();
	afx_msg void OnEditCategoryItem();
	afx_msg void OnTabmenuEdit();
	afx_msg void OnTabmenuAdd();
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMenuRssRead();
	afx_msg void OnViewLog();
	afx_msg void OnCategoryOpen();
	afx_msg void OnCopyClipboardUrl(UINT);
	afx_msg void OnLuaMenu(UINT);

	BOOL OnKeydownGroupTab( WORD vKey );
	BOOL OnKeydownCategoryList( WORD vKey );
	BOOL OnKeydownBodyList( WORD vKey );
	BOOL OnKeyupBodyList( WORD vKey );
	BOOL OnKeyupCategoryList( WORD vKey );
	BOOL OnKeyDown(MSG* pMsg);
	BOOL OnKeyUp(MSG* pMsg);

	bool MyLoadCategoryLogfile(CCategoryItem& category);

private:
	void InsertInitialData();

	void AccessProc(CMixiData* data, LPCTSTR url_, CInetAccess::ENCODING encoding=CInetAccess::ENCODING_EUC);

	void SetBodyList( CMixiDataList& body );
	void SetBodyImageList( CMixiDataList& body );

	void OnUpdateWriteButton(CCmdUI*);
	void OnMySelchangedCategoryList(void);

	static unsigned int Initialize_Thread( LPVOID This );
	static unsigned int CacheCleanup_Thread( LPVOID This );
	static unsigned int ReloadGroupTab_Thread( LPVOID This );
	static unsigned int LongReturnKey_Thread( LPVOID This );

	bool DoInitialize();
	void MyParseMixiHtml(LPCTSTR szHtmlfile, CMixiData& mixi);
	void MyShowReportView(CMixiData& mixi);

public:
	/**
	 * 現在選択中のボディアイテムを取得する
	 *
	 * 単純なアクセッサ。
	 */
	CMixiData& GetSelectedBodyItem(void)
	{
		CCategoryItem* pSelectedCategory = m_selGroup->getSelectedCategory();
		if (pSelectedCategory==NULL) {
			// ダミーを返す。本来はNULLを返すなり、Exception投げるなりすべき。
			static CMixiData s_dummy;
			CMixiData dummy;
			s_dummy = dummy;	// 初期化
			return s_dummy;
		}
		return pSelectedCategory->GetSelectedBody();
	}

	bool DoNewCommentCheck(void);
	bool DoCheckSoftwareUpdate(void);
	bool MyChangeBodyHeader(void);
	void MyUpdateCategoryListByGroupItem(void);
	void OnSelchangedGroupTab(void);

	BOOL CommandSetFocusCategoryList();
	BOOL CommandSetFocusGroupTab();
	BOOL CommandSetFocusBodyList();
	BOOL CommandSelectGroupTabNextItem();
	BOOL CommandSelectGroupTabBeforeItem();

	BOOL CommandMoveUpCategoryList();
	BOOL CommandMoveDownCategoryList();
	BOOL CommandMoveUpBodyList();
	BOOL CommandMoveDownBodyList();

	bool PopupBodyMenu(POINT pt_=CPoint(0,0), int flags_=0);
	void PopupCategoryMenu(POINT pt_=CPoint(0,0), int flags_=0);
	bool PopupTabMenu(POINT pt_=CPoint(0,0), int flags_=0);
	bool PrepareViewBbsList(void);
	void MyShowHelp(void);
	void MyShowHistory(void);
	void MyShowErrorlog(void);

	void StartCruise( bool unreadOnly );
	bool CruiseToNextCategory(void);

	bool MoveToNextCruiseCategory(void);
	void ResetColumnWidth();
	int  GetListWidth(void);
	bool DoNextBodyItemCruise();
	bool RetrieveCategoryItem(void);

public:
	void ResetIntervalTimer(void);
	bool AppendCategoryList(const CCategoryItem& categoryItem);
	bool DoAccessEndProcForBody(ACCESS_TYPE aType);

	void MoveMiniImageDlg(int idxBody=-1, int pointx=-1, int pointy=-1);
	bool MyLoadMiniImage(CMixiData& mixi);
	VIEW_STYLE MyGetViewStyleForSelectedCategory(void);
	void MySetLayout(int cx, int cy);
	void MyUpdateControlStatus(void);
	void MyUpdateFocus(void);
	void MyOpenLocalFile(void);
	void MyOpenUrl(void);
	afx_msg void OnAddRssFeedMenu();

	void ResetViewContent(void);
	afx_msg void OnAcceleratorToggleIntegratedMode();
	void MyResetTwitterStylePostMode();
	bool DoAccessEndProcForRssAutoDiscovery(void);
private:
	bool DoAccessEndProcForSoftwareUpdateCheck(void);
public:
	void ReloadCategoryListLog();

	void ReflectSplitterLineMove();
	afx_msg void OnTabmenuMoveToRight();
	afx_msg void OnTabmenuMoveToLeft();
	void MoveTabItem(int oldTabIndex, int newTabIndex);
	void MyRedrawBodyImages(void);
	afx_msg void OnCategorymenuMoveUp();
	afx_msg void OnCategorymenuMoveDown();
	void MoveCategoryItem(int oldCategoryIndex, int newCategoryIndex);
};

#ifndef _DEBUG  // MZ3View.cpp のデバッグ バージョン
inline CMZ3Doc* CMZ3View::GetDocument() const
   { return reinterpret_cast<CMZ3Doc*>(m_pDocument); }
#endif

