/*
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */
// MZ3View.h : CMZ3View �N���X�̃C���^�[�t�F�C�X
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
 * MZ3 �̃��C���r���[
 */
class CMZ3View : public CFormView
{
public:
	//--- UI �v�f
	CGroupTabCtrl		m_groupTab;		///< �O���[�v�^�u�BItemData �� theApp.m_root.groups �̃C���f�b�N�X
	CCategoryListCtrl	m_categoryList;	///< �J�e�S�����X�g�BItemData �� theApp.m_root.groups[].categories �̃C���f�b�N�X
	CBodyListCtrl		m_bodyList;		///< �{�f�B���X�g�B
	CEdit				m_infoEdit;		///< ���\���p�G�f�B�b�g�{�b�N�X
	CProgressCtrl		mc_progressBar;	///< �v���O���X�o�[
	CEdit				m_statusEdit;	///< ���͗̈�B

	XcrawlCanceler		m_xcrawl;				///< Xcrawl ����
	int					m_nKeydownRepeatCount;	///< WM_KEYDOWN �̉�

	bool				m_bReloadingGroupTabByThread;	///< �X���b�h�ɂ��^�u�؂�ւ������H
	bool				m_bRetryReloadGroupTabByThread;	///<

	/// �\���X�^�C��
	enum VIEW_STYLE {
		VIEW_STYLE_DEFAULT = 0,					///< �W���X�^�C��
		VIEW_STYLE_IMAGE   = 1,					///< �W���X�^�C��+ImageIcon
		VIEW_STYLE_TWITTER = 2,					///< �W���X�^�C��+ImageIcon+StatusEdit (Twitter)
	};
	VIEW_STYLE			m_viewStyle;			///< �\���X�^�C��

	ACCESS_TYPE			m_twitterPostAccessType;///< Twitter �����M�A�N�Z�X���


	CRect				m_rectIcon;				///< �A�C�R���\���̈��RECT

	BOOL				m_abort;				///< ���f�t���O

private:
	bool				m_bModifyingBodyList;	///< SetBodyList �� Body ���X�g�������H
	bool				m_checkNewComment;		///< �V���R�����g�����b�Z�[�W�`�F�b�N���H

	/**
	 * ����p�f�[�^
	 *
	 * ���񏈗��菇�F
	 * <ul>
	 * <li>����J�n
	 * <li>����ΏۃJ�e�S���T��
	 *  <ul>
	 *  <li>�O���[�v�^�J�e�S�������ɒT�����A����\��t���O��1�̂��̂�T���B
	 *  <li>����\��t���O��1�̂��̂��Ȃ���ΏI������B
	 *  </ul>
	 * <li>���񂷂�B
	 * <li>����ΏۃJ�e�S����+1����B
	 * <li>����ΏۃJ�e�S���T���ɂ��ǂ�
	 * </ul>
	 */
	class CruiseInfo {
	public:
		/// ������
		enum CRUISE_STATE {
			CRUISE_STATE_STOP,	///< �����~���i������ԁj
			CRUISE_STATE_LIST,	///< ���X�g�擾��
			CRUISE_STATE_BODY,	///< �{�f�B�v�f����
		};
		/// ������
		CRUISE_STATE state;

		int targetBodyItem;			///< ���݂̏���Ώۃ{�f�B�A�C�e���̃C���f�b�N�X

		// �\�񏄉�֘A
		int targetGroupIndex;		///< ���݂̏���ΏۃO���[�v�A�C�e���̃C���f�b�N�X
		int targetCategoryIndex;	///< ���݂̏���ΏۃJ�e�S���A�C�e���̃C���f�b�N�X
		bool autoCruise;			///< �\�񏄉񃂁[�h�H
		bool unreadOnly;			///< ���ǂ̂ݏ��񃂁[�h

		CruiseInfo() : state(CRUISE_STATE_STOP), targetBodyItem(0), targetGroupIndex(0), targetCategoryIndex(0), 
			autoCruise(false), unreadOnly(false) {}

		/// ���񒆂��ǂ�����Ԃ�
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

		bool isFetchListMode() { return state == CRUISE_STATE_LIST; } ///< ���X�g�擾���[�h�H
		bool isFetchBodyMode() { return state == CRUISE_STATE_BODY; } ///< �{�f�B�擾���[�h�H
	};
	CruiseInfo		m_cruise;				///< ������

public:
	CGroupItem*		m_selGroup;				///< ���ݑI������Ă���O���[�v�^�u����

private:
	int				m_preCategory;			///< �O��I�����Ă����J�e�S�����X�g���ڂ̃C���f�b�N�X
	CListCtrl*		m_hotList;				///< ���ݑI�𒆂̃��X�g�R���g���[���i���r���[����̕��A���ɗ��p�j

	CImageList		m_iconImageListSmall;		///< �A�C�R���p�摜���X�g(16x16)
	CImageList		m_iconImageListLarge;		///< �A�C�R���p�摜���X�g(32x32)

	/// RETURN �L�[��������������
	DWORD			m_dwLastReturn;

	DWORD			m_dwIntervalTimerStartMsec;	///< ����擾�p�^�C�}�[�̊J�n����
	CMenu*			m_pCategorySubMenuList;		///< �J�e�S���p���j���[
	bool			m_bImeCompositioning;		///< IME�ϊ����t���O

#ifndef WINCE
	bool	m_bDragging;			///< �}�E�X�h���b�O��
#endif
	CPoint	m_ptDragStart;			///< �h���b�O�J�n�ʒu
	int		m_nOriginalH1;			///< �h���b�O�J�n����H1
	int		m_nOriginalH2;			///< �h���b�O�J�n����H2

public: // �V���A��������̂ݍ쐬���܂��B
	CMZ3View();
	DECLARE_DYNCREATE(CMZ3View)

public:
	enum{ IDD = IDD_MZ3_FORM };

// ����
public:
	CMZ3Doc* GetDocument() const;

// ����
public:

// �I�[�o�[���C�h
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL PreTranslateMessage(MSG* pMsg);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �T�|�[�g
	virtual void OnInitialUpdate(); // �\�z��ɏ��߂ČĂяo����܂��B
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);

// ����
public:
	virtual ~CMZ3View();
#ifdef _DEBUG
	virtual void AssertValid() const;
#endif

protected:

// �������ꂽ�A���b�Z�[�W���蓖�Ċ֐�
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
	 * ���ݑI�𒆂̃{�f�B�A�C�e�����擾����
	 *
	 * �P���ȃA�N�Z�b�T�B
	 */
	CMixiData& GetSelectedBodyItem(void)
	{
		CCategoryItem* pSelectedCategory = m_selGroup->getSelectedCategory();
		if (pSelectedCategory==NULL) {
			// �_�~�[��Ԃ��B�{����NULL��Ԃ��Ȃ�AException������Ȃ肷�ׂ��B
			static CMixiData s_dummy;
			CMixiData dummy;
			s_dummy = dummy;	// ������
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

#ifndef _DEBUG  // MZ3View.cpp �̃f�o�b�O �o�[�W����
inline CMZ3Doc* CMZ3View::GetDocument() const
   { return reinterpret_cast<CMZ3Doc*>(m_pDocument); }
#endif

