// MZ3View.h : CMZ3View �N���X�̃C���^�[�t�F�C�X
//

#pragma once
#include "afxcmn.h"
#include "afxwin.h"
#include "MixiData.h"
#include "CategoryItem.h"
#include "CategoryListCtrl.h"
#include "BodyListCtrl.h"
#include "PostData.h"
#include "InetAccess.h"
#include "XcrawlCanceler.h"

class CMZ3Doc;
class CMiniImageDialog;

/**
 * MZ3 �̃��C���r���[
 */
class CMZ3View : public CFormView
{
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

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �T�|�[�g
	virtual void OnInitialUpdate(); // �\�z��ɏ��߂ČĂяo����܂��B

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
	afx_msg LRESULT OnGetEndBinary(WPARAM, LPARAM);
	afx_msg LRESULT OnGetError(WPARAM, LPARAM);
	afx_msg LRESULT OnGetAbort(WPARAM, LPARAM);
	afx_msg LRESULT OnAccessInformation(WPARAM, LPARAM);
	afx_msg LRESULT OnAccessLoaded(WPARAM, LPARAM);
	afx_msg LRESULT OnChangeView(WPARAM, LPARAM);
	afx_msg LRESULT OnAbort(WPARAM, LPARAM);

private:
	void InsertInitialData();

	void AccessProc(CMixiData* data, LPCTSTR url_, CInetAccess::ENCODING encoding=CInetAccess::ENCODING_EUC);

	void SetBodyList( CMixiDataList& body );
	void SetBodyImageList( CMixiDataList& body );

	void OnUpdateWriteButton(CCmdUI*);
	afx_msg void OnWriteButton();

public:
	//--- UI �v�f
	CTabCtrl			m_groupTab;		///< �O���[�v�^�u�BItemData �� theApp.m_root.groups �̃C���f�b�N�X
	CCategoryListCtrl	m_categoryList;	///< �J�e�S�����X�g�BItemData �� theApp.m_root.groups[].categories �̃C���f�b�N�X
	CBodyListCtrl		m_bodyList;		///< �{�f�B���X�g�B
	CEdit				m_infoEdit;		///< ���\���p�G�f�B�b�g�{�b�N�X

	XcrawlCanceler		m_xcrawl;		///< Xcrawl ����
	int					m_nKeydownRepeatCount;	///< WM_KEYDOWN �̉�

	CMiniImageDialog*	m_pMiniImageDlg;	///< �摜���

private:
	BOOL			m_access;			///< �A�N�Z�X���t���O

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
	CruiseInfo		m_cruise;			///< ������

	BOOL			m_nochange;
	BOOL			m_abort;			///< ���f�t���O
	bool			m_checkNewComment;	///< �V���R�����g�����b�Z�[�W�`�F�b�N���H

public:
	CGroupItem*		m_selGroup;			///< ���ݑI������Ă���O���[�v�^�u����

private:
	int				m_preCategory;		///< �O��I�����Ă����J�e�S�����X�g���ڂ̃C���f�b�N�X
	CListCtrl*		m_hotList;			///< ���ݑI�𒆂̃��X�g�R���g���[���i���r���[����̕��A���ɗ��p�j
	CPostData*		m_postData;

	CImageList		m_iconImageList;			///< �A�C�R���p�摜���X�g
	CImageList		m_iconExtendedImageList;	///< �A�C�R���p�摜���X�g�i���I�����j
	CStringArray	m_extendeImageListMap;		///< m_iconExtendedImageList �̃C���f�b�N�X��
												///< �ǂ̃t�@�C�����i�[����Ă��邩�������^���}�b�v

	/// RETURN �L�[��������������
	DWORD			m_dwLastReturn;

	afx_msg void OnEnSetfocusInfoEdit();
	afx_msg void OnNMDblclkBodyList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLvnItemchangedBodyList(NMHDR *pNMHDR, LRESULT *pResult);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnWriteDiary();

	BOOL OnKeydownGroupTab( WORD vKey );
	BOOL OnKeydownCategoryList( WORD vKey );
	BOOL OnKeydownBodyList( WORD vKey );
	BOOL OnKeyupBodyList( WORD vKey );
	BOOL OnKeyupCategoryList( WORD vKey );

	BOOL OnKeyDown(MSG* pMsg);
	BOOL OnKeyUp(MSG* pMsg);

	afx_msg void OnNMSetfocusBodyList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnOpenBrowser();
	afx_msg void OnNMSetfocusHeaderList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnShowDebugInfo();
public:
	bool DoNewCommentCheck(void);
private:
	afx_msg void OnGetAll();
	afx_msg void OnGetLast10();
	void OnMySelchangedCategoryList(void);
	bool MyLoadCategoryLogfile(CCategoryItem& category);
private:
	static unsigned int Initialize_Thread( LPVOID This );
	static unsigned int ReloadGroupTab_Thread( LPVOID This );
	static unsigned int LongReturnKey_Thread( LPVOID This );

	bool DoInitialize();
	afx_msg void OnViewLog();
	void MyParseMixiHtml(LPCTSTR szHtmlfile, CMixiData& mixi);
	void MyShowReportView(CMixiData& mixi);
public:
	afx_msg void OnHdnItemclickBodyList(NMHDR *pNMHDR, LRESULT *pResult);
	bool MyChangeBodyHeader(void);
	void MyUpdateCategoryListByGroupItem(void);
	afx_msg void OnTcnSelchangeGroupTab(NMHDR *pNMHDR, LRESULT *pResult);
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
	CProgressCtrl mc_progressBar;
	afx_msg void OnOpenBrowserUser();
	afx_msg void OnOpenIntro();
	afx_msg void OnOpenSelfintro();
	afx_msg void OnSetNoRead();
	afx_msg void OnViewBbsList();
	bool PopupBodyMenu(void);
	afx_msg void OnViewBbsListLog();
	bool PrepareViewBbsList(void);
	afx_msg void OnCruise();
	void MyShowHelp(void);
	void MyShowHistory(void);
	void MyShowErrorlog(void);
	afx_msg void OnCheckCruise();
	void StartCruise( bool unreadOnly );
	bool CruiseToNextCategory(void);
	bool MoveToNextCruiseCategory(void);
	afx_msg void OnSendNewMessage();
	void ResetColumnWidth();
	afx_msg void OnHdnEndtrackHeaderList(NMHDR *pNMHDR, LRESULT *pResult);
	int GetListWidth(void);
	bool DoNextBodyItemCruise();
	afx_msg void OnSettingChange(UINT uFlags, LPCTSTR lpszSection);
	afx_msg void OnLayoutCategoryMakeNarrow();
	afx_msg void OnLayoutCategoryMakeWide();
	afx_msg void OnNMRclickHeaderList(NMHDR *pNMHDR, LRESULT *pResult);
	void PopupCategoryMenu(void);
	afx_msg void OnNMRclickBodyList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTimer(UINT_PTR nIDEvent);

	bool RetrieveCategoryItem(void);
public:
	DWORD m_dwIntervalTimerStartMsec;			///< ����擾�p�^�C�}�[�̊J�n����
	void ResetIntervalTimer(void);
protected:
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
public:
	afx_msg void OnNMClickGroupTab(NMHDR *pNMHDR, LRESULT *pResult);
	void MoveMiniImageDlg(int idxBody=-1, int pointx=-1, int pointy=-1);
	afx_msg LRESULT OnHideView(WPARAM wParam, LPARAM lParam);
	bool MyLoadMiniImage(const CMixiData& mixi);
	afx_msg void OnAcceleratorFontMagnify();
	afx_msg void OnAcceleratorFontShrink();
	afx_msg void OnAcceleratorContextMenu();
	afx_msg void OnAcceleratorNextTab();
	afx_msg void OnAcceleratorPrevTab();
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnSetRead();
	afx_msg void OnAcceleratorReload();
};

#ifndef _DEBUG  // MZ3View.cpp �̃f�o�b�O �o�[�W����
inline CMZ3Doc* CMZ3View::GetDocument() const
   { return reinterpret_cast<CMZ3Doc*>(m_pDocument); }
#endif

