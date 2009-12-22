/*
 * Copyright (c) MZ3 Project Team All rights reserved.
 * Code licensed under the BSD License:
 * http://www.mz3.jp/license.txt
 */
/// MZ3View.cpp : CMZ3View �N���X�̎���

#include "stdafx.h"
#include "version.h"
#include "MZ3.h"
#include "MZ3Doc.h"

// UI�֘A
#include "MZ3View.h"
#include "ReportView.h"
#include "DownloadView.h"
#include "DetailView.h"
#include "MainFrm.h"
#include "WriteView.h"
#include "CommonEditDlg.h"
#include "MouseGestureManager.h"
#include "OpenUrlDlg.h"
#include "ChooseAccessTypeDlg.h"

// ���[�e�B���e�B�֘A
#include "HtmlArray.h"
#include "MixiData.h"
#include "CategoryItem.h"
#include "util.h"
#include "util_mz3.h"
#include "util_gui.h"
#include "url_encoder.h"
#include "MixiParser.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define SPLITTER_HEIGHT			10

/// �A�N�Z�X��ʂƕ\����ʂ���A�{�f�B�[���X�g�̃w�b�_�[������i�P�J�����ځj���擾����
LPCTSTR MyGetBodyHeaderColName1( ACCESS_TYPE accessType )
{
	return theApp.m_accessTypeInfo.getBodyHeaderCol1Name(accessType);
}

/// �A�N�Z�X��ʂƕ\����ʂ���A�{�f�B�[���X�g�̃w�b�_�[������i�Q�J�����ځj���擾����
LPCTSTR MyGetBodyHeaderColName2( ACCESS_TYPE accessType, AccessTypeInfo::BODY_INDICATE_TYPE bodyIndicateType )
{
	AccessTypeInfo::BODY_INDICATE_TYPE type2 = theApp.m_accessTypeInfo.getBodyHeaderCol2Type(accessType);
	AccessTypeInfo::BODY_INDICATE_TYPE type3 = theApp.m_accessTypeInfo.getBodyHeaderCol3Type(accessType);

	if (bodyIndicateType==type2) {
		return theApp.m_accessTypeInfo.getBodyHeaderCol2Name(accessType);
	} else if (bodyIndicateType==type3) {
		return theApp.m_accessTypeInfo.getBodyHeaderCol3Name(accessType);
	} else {
		// ����`
		return L"";
	}
}


// CMZ3View

IMPLEMENT_DYNCREATE(CMZ3View, CFormView)

BEGIN_MESSAGE_MAP(CMZ3View, CFormView)
	ON_WM_SIZE()
	ON_WM_SETTINGCHANGE()
	ON_WM_TIMER()
	ON_NOTIFY(NM_CLICK, IDC_HEADER_LIST, &CMZ3View::OnNMClickCategoryList)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_HEADER_LIST, &CMZ3View::OnLvnItemchangedCategoryList)
	ON_NOTIFY(NM_DBLCLK, IDC_HEADER_LIST, &CMZ3View::OnNMDblclkCategoryList)
	ON_NOTIFY(NM_SETFOCUS, IDC_HEADER_LIST, &CMZ3View::OnNMSetfocusHeaderList)
	ON_NOTIFY(NM_RCLICK, IDC_HEADER_LIST, &CMZ3View::OnNMRclickHeaderList)

	ON_NOTIFY(NM_DBLCLK, IDC_BODY_LIST, &CMZ3View::OnNMDblclkBodyList)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_BODY_LIST, &CMZ3View::OnLvnItemchangedBodyList)
	ON_NOTIFY(NM_SETFOCUS, IDC_BODY_LIST, &CMZ3View::OnNMSetfocusBodyList)
	ON_NOTIFY(HDN_ITEMCLICK, 0, &CMZ3View::OnHdnItemclickBodyList)
	ON_NOTIFY(NM_RCLICK, IDC_BODY_LIST, &CMZ3View::OnNMRclickBodyList)
	ON_NOTIFY(TCN_SELCHANGE, IDC_GROUP_TAB, &CMZ3View::OnTcnSelchangeGroupTab)
	ON_NOTIFY(HDN_ENDTRACK, 0, &CMZ3View::OnHdnEndtrackHeaderList)
	ON_MESSAGE(WM_MZ3_GET_END, OnGetEnd)
	ON_MESSAGE(WM_MZ3_GET_END_BINARY, OnGetEndBinary)
    ON_MESSAGE(WM_MZ3_GET_ERROR, OnGetError)
    ON_MESSAGE(WM_MZ3_GET_ABORT, OnGetAbort)
    ON_MESSAGE(WM_MZ3_ABORT, OnAbort)
    ON_MESSAGE(WM_MZ3_ACCESS_INFORMATION, OnAccessInformation)
	ON_MESSAGE(WM_MZ3_ACCESS_LOADED, OnAccessLoaded)
    ON_MESSAGE(WM_MZ3_CHANGE_VIEW, OnChangeView)
	ON_MESSAGE(WM_MZ3_HIDE_VIEW, OnHideView)
	ON_MESSAGE(WM_MZ3_POST_END, OnPostEnd)
	ON_COMMAND(ID_WRITE_DIARY, &CMZ3View::OnWriteDiary)
    ON_COMMAND(ID_WRITE_BUTTON, OnWriteButton)
	ON_COMMAND(ID_OPEN_BROWSER, &CMZ3View::OnOpenBrowser)
	ON_COMMAND(ID_SHOW_DEBUG_INFO, &CMZ3View::OnShowDebugInfo)
	ON_COMMAND(ID_GET_ALL, &CMZ3View::OnGetAll)
	ON_COMMAND(ID_GET_LAST10, &CMZ3View::OnGetLast10)
	ON_COMMAND(ID_VIEW_LOG, &CMZ3View::OnViewLog)
	ON_COMMAND(ID_OPEN_BROWSER_USER, &CMZ3View::OnOpenBrowserUser)
	ON_COMMAND(ID_OPEN_INTRO, &CMZ3View::OnOpenIntro)
	ON_COMMAND(ID_OPEN_SELFINTRO, &CMZ3View::OnOpenSelfintro)
	ON_COMMAND(IDM_SET_NO_READ, &CMZ3View::OnSetNoRead)
	ON_COMMAND(IDM_VIEW_BBS_LIST, &CMZ3View::OnViewBbsList)
	ON_COMMAND(IDM_VIEW_BBS_LIST_LOG, &CMZ3View::OnViewBbsListLog)
	ON_COMMAND(IDM_CRUISE, &CMZ3View::OnCruise)
	ON_COMMAND(IDM_CHECK_CRUISE, &CMZ3View::OnCheckCruise)
	ON_COMMAND(ID_SEND_NEW_MESSAGE, &CMZ3View::OnSendNewMessage)
	ON_COMMAND(IDM_LAYOUT_CATEGORY_MAKE_NARROW, &CMZ3View::OnLayoutCategoryMakeNarrow)
	ON_COMMAND(IDM_LAYOUT_CATEGORY_MAKE_WIDE, &CMZ3View::OnLayoutCategoryMakeWide)
	ON_EN_SETFOCUS(IDC_INFO_EDIT, &CMZ3View::OnEnSetfocusInfoEdit)
    ON_UPDATE_COMMAND_UI(ID_WRITE_BUTTON, OnUpdateWriteButton)
	ON_NOTIFY(NM_CLICK, IDC_GROUP_TAB, &CMZ3View::OnNMClickGroupTab)
	ON_NOTIFY(NM_RCLICK, IDC_GROUP_TAB, &CMZ3View::OnNMRclickGroupTab)
	ON_COMMAND(ID_ACCELERATOR_FONT_MAGNIFY, &CMZ3View::OnAcceleratorFontMagnify)
	ON_COMMAND(ID_ACCELERATOR_FONT_SHRINK, &CMZ3View::OnAcceleratorFontShrink)
	ON_COMMAND(ID_ACCELERATOR_CONTEXT_MENU, &CMZ3View::OnAcceleratorContextMenu)
	ON_COMMAND(ID_ACCELERATOR_NEXT_TAB, &CMZ3View::OnAcceleratorNextTab)
	ON_COMMAND(ID_ACCELERATOR_PREV_TAB, &CMZ3View::OnAcceleratorPrevTab)
	ON_COMMAND(IDM_SET_READ, &CMZ3View::OnSetRead)
	ON_COMMAND(ID_ACCELERATOR_RELOAD, &CMZ3View::OnAcceleratorReload)
	ON_BN_CLICKED(IDC_UPDATE_BUTTON, &CMZ3View::OnBnClickedUpdateButton)
	ON_COMMAND_RANGE(ID_REPORT_URL_BASE+1, ID_REPORT_URL_BASE+50, OnLoadUrl)
	ON_WM_PAINT()
	ON_COMMAND(ID_TABMENU_DELETE, &CMZ3View::OnTabmenuDelete)
	ON_COMMAND_RANGE(ID_APPEND_MENU_BEGIN, ID_APPEND_MENU_END, &CMZ3View::OnAppendCategoryMenu)
	ON_COMMAND(ID_REMOVE_CATEGORY_ITEM, &CMZ3View::OnRemoveCategoryItem)
	ON_COMMAND(ID_EDIT_CATEGORY_ITEM, &CMZ3View::OnEditCategoryItem)
	ON_COMMAND(ID_TABMENU_EDIT, &CMZ3View::OnTabmenuEdit)
	ON_COMMAND(ID_TABMENU_ADD, &CMZ3View::OnTabmenuAdd)
	ON_COMMAND(ID_MENU_RSS_READ, &CMZ3View::OnMenuRssRead)
	ON_COMMAND(IDM_CATEGORY_OPEN, &CMZ3View::OnCategoryOpen)
	ON_COMMAND(ID_ADD_RSS_FEED_MENU, &CMZ3View::OnAddRssFeedMenu)

	ON_COMMAND(ID_ACCELERATOR_TOGGLE_INTEGRATED_MODE, &CMZ3View::OnAcceleratorToggleIntegratedMode)
	ON_COMMAND_RANGE(ID_REPORT_COPY_URL_BASE+1, ID_REPORT_COPY_URL_BASE+50, OnCopyClipboardUrl)
	ON_COMMAND_RANGE(ID_LUA_MENU_BASE, ID_LUA_MENU_BASE+1000, OnLuaMenu)
	ON_COMMAND(ID_TABMENU_MOVE_TO_RIGHT, &CMZ3View::OnTabmenuMoveToRight)
	ON_COMMAND(ID_TABMENU_MOVE_TO_LEFT, &CMZ3View::OnTabmenuMoveToLeft)
	ON_COMMAND(ID_CATEGORYMENU_MOVE_UP, &CMZ3View::OnCategorymenuMoveUp)
	ON_COMMAND(ID_CATEGORYMENU_MOVE_DOWN, &CMZ3View::OnCategorymenuMoveDown)
	ON_WM_CTLCOLOR()
	ON_COMMAND(IDM_LAYOUT_MAGNIFY_BODY_LIST, &CMZ3View::OnLayoutMagnifyBodyList)
	ON_COMMAND(IDM_LAYOUT_MAGNIFY_CATEGORY_LIST, &CMZ3View::OnLayoutMagnifyCategoryList)
	ON_COMMAND(IDM_LAYOUT_MAGNIFY_DEFAULT, &CMZ3View::OnLayoutMagnifyDefault)
END_MESSAGE_MAP()

// CMZ3View �R���X�g���N�V����/�f�X�g���N�V����

/**
 * �R���X�g���N�^
 */
CMZ3View::CMZ3View()
	: CFormView(CMZ3View::IDD)
	, m_dwLastReturn( 0 )
	, m_nKeydownRepeatCount( 0 )
	, m_checkNewComment( false )
	, m_viewStyle(VIEW_STYLE_DEFAULT)
	, m_rectIcon(0,0,0,0)
	, m_bReloadingGroupTabByThread(false)
	, m_bRetryReloadGroupTabByThread(false)
	, m_pCategorySubMenuList(NULL)
	, m_bImeCompositioning(false)
#ifndef WINCE
	, m_bDragging(false)
#endif
	, m_preCategory(0)
	, m_selGroup(NULL)
	, m_hotList(NULL)
	, m_bModifyingBodyList(false)
	, m_abort(FALSE)
#ifdef WINCE
	, m_magnifyMode(MAGNIFY_MODE_CATEGORY)
#else
	, m_magnifyMode(MAGNIFY_MODE_DEFAULT)
#endif
{
}

/**
 * �f�X�g���N�^
 */
CMZ3View::~CMZ3View()
{
	// �T�u���j���[�̉��
	if (m_pCategorySubMenuList != NULL) {
		delete[] m_pCategorySubMenuList;
	}
}

void CMZ3View::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_GROUP_TAB, m_groupTab);
	DDX_Control(pDX, IDC_HEADER_LIST, m_categoryList);
	DDX_Control(pDX, IDC_BODY_LIST, m_bodyList);
	DDX_Control(pDX, IDC_INFO_EDIT, m_infoEdit);
	DDX_Control(pDX, IDC_PROGRESS_BAR, mc_progressBar);
	DDX_Control(pDX, IDC_STATUS_EDIT, m_statusEdit);
}

BOOL CMZ3View::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: ���̈ʒu�� CREATESTRUCT cs ���C������ Window �N���X�܂��̓X�^�C����
	//  �C�����Ă��������B

	return CFormView::PreCreateWindow(cs);
}

/**
 * ����������
 */
void CMZ3View::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();

	RECT rect;
	SystemParametersInfo(SPI_GETWORKAREA, 0, &rect, 0);
	TRACE(_T("Rect = (%d, %d) - (%d, %d)\n"), rect.left, rect.top, rect.right, rect.bottom);
	OnSize(SIZE_RESTORED, rect.right - rect.left, rect.bottom - (rect.top*2));

	// �C���^�[�l�b�g������J��
//	theApp.m_inet.Open();

	theApp.m_access = false;

	// �v���O���X�o�[������
	mc_progressBar.SetRange( 0, 1000 );

	// �t�H���g�̍쐬
	theApp.MakeNewFont( GetFont(), theApp.m_optionMng.GetFontHeight(), theApp.m_optionMng.GetFontFace() );

	// --------------------------------------------------
	// �O���[�v�^�u�̐ݒ�
	// --------------------------------------------------
	{
		// �t�H���g�ύX
		m_groupTab.SetFont( &theApp.m_font );
	}

	// --------------------------------------------------
	// �J�e�S�����X�g�̐ݒ�
	// --------------------------------------------------
	{
		// �t�H���g�ύX
		m_categoryList.SetFont( &theApp.m_font );

		// �X�^�C���ύX
		DWORD dwStyle = m_categoryList.GetStyle();
		dwStyle &= ~LVS_TYPEMASK;
		// �f�t�H���g�����|�[�g�\���ŃI�[�i�[�Œ�`��ɂ���
		dwStyle |= LVS_REPORT | LVS_OWNERDRAWFIXED | LVS_SINGLESEL;

		// �X�^�C���̍X�V
		m_categoryList.ModifyStyle(0, dwStyle);

		// ��s�I�����[�h�̐ݒ�
		ListView_SetExtendedListViewStyle((HWND)m_categoryList.m_hWnd, LVS_EX_FULLROWSELECT);

		// �J�����쐬
		// ����������������ɍĐݒ肷��̂ŉ��̕����w�肵�Ă����B
		switch( theApp.GetDisplayMode() ) {
		case SR_VGA:
			m_categoryList.InsertColumn(0, _T(""), LVCFMT_LEFT, 125*2, -1);
			m_categoryList.InsertColumn(1, _T(""), LVCFMT_LEFT, 100*2, -1);
			break;
		case SR_QVGA:
		default:
			m_categoryList.InsertColumn(0, _T(""), LVCFMT_LEFT, 125, -1);
			m_categoryList.InsertColumn(1, _T(""), LVCFMT_LEFT, 100, -1);
			break;
		}
		m_categoryList.m_bUsePanScrollAnimation = theApp.m_optionMng.m_bUseRan2PanScrollAnimation;
		m_categoryList.m_bUseHorizontalDragMove = theApp.m_optionMng.m_bUseRan2HorizontalDragMove;
	}

	// --------------------------------------------------
	// �{�f�B���X�g�̐ݒ�
	// --------------------------------------------------
	{
		// �t�H���g�ύX
		m_bodyList.SetFont( &theApp.m_font );

		// �O���b�h���C���\��
		m_bodyList.SetExtendedStyle(m_bodyList.GetExtendedStyle() | LVS_EX_GRIDLINES);

		// ��s�I�����[�h�̐ݒ�
		ListView_SetExtendedListViewStyle((HWND)m_bodyList.m_hWnd, LVS_EX_FULLROWSELECT);

		// �X�^�C���ύX
		DWORD dwStyle = m_bodyList.GetStyle();
		dwStyle &= ~LVS_TYPEMASK;
		// �f�t�H���g�����|�[�g�\���ŃI�[�i�[�Œ�`��ɂ���
		dwStyle |= LVS_REPORT | LVS_OWNERDRAWFIXED | LVS_SINGLESEL;

		// �X�^�C���̍X�V
		m_bodyList.ModifyStyle(0, dwStyle);

		// �A�C�R�����X�g�̍쐬
		m_iconImageListSmall.Create(16, 16, ILC_COLOR24 | ILC_MASK, 0, 9);
		m_iconImageListSmall.Add( AfxGetApp()->LoadIcon(IDI_TOPIC_ICON) );
		m_iconImageListSmall.Add( AfxGetApp()->LoadIcon(IDI_EVENT_ICON) );
		m_iconImageListSmall.Add( AfxGetApp()->LoadIcon(IDI_ENQUETE_ICON) );
		m_iconImageListSmall.Add( AfxGetApp()->LoadIcon(IDI_EVENT_JOIN_ICON) );
		m_iconImageListSmall.Add( AfxGetApp()->LoadIcon(IDI_BIRTHDAY_ICON) );
		m_iconImageListSmall.Add( AfxGetApp()->LoadIcon(IDI_SCHEDULE_ICON) );
		m_iconImageListSmall.Add( AfxGetApp()->LoadIcon(IDI_MAIL_NEW_ICON) );
		m_iconImageListSmall.Add( AfxGetApp()->LoadIcon(IDI_MAIL_OPENED_ICON) );
		m_iconImageListSmall.Add( AfxGetApp()->LoadIcon(IDI_RSS_ICON) );
		m_bodyList.SetImageList(&m_iconImageListSmall, LVSIL_SMALL);

		m_iconImageListLarge.Create(32, 32, ILC_COLOR24 | ILC_MASK, 0, 9);
		m_iconImageListLarge.Add( AfxGetApp()->LoadIcon(IDI_TOPIC_ICON) );
		m_iconImageListLarge.Add( AfxGetApp()->LoadIcon(IDI_EVENT_ICON) );
		m_iconImageListLarge.Add( AfxGetApp()->LoadIcon(IDI_ENQUETE_ICON) );
		m_iconImageListLarge.Add( AfxGetApp()->LoadIcon(IDI_EVENT_JOIN_ICON) );
		m_iconImageListLarge.Add( AfxGetApp()->LoadIcon(IDI_BIRTHDAY_ICON) );
		m_iconImageListLarge.Add( AfxGetApp()->LoadIcon(IDI_SCHEDULE_ICON) );
		m_iconImageListLarge.Add( AfxGetApp()->LoadIcon(IDI_MAIL_NEW_ICON) );
		m_iconImageListLarge.Add( AfxGetApp()->LoadIcon(IDI_MAIL_OPENED_ICON) );
		m_iconImageListLarge.Add( AfxGetApp()->LoadIcon(IDI_RSS_ICON) );

		// �J�����쐬
		// ����������������ɍĐݒ肷��̂ŉ��̕����w�肵�Ă����B
		switch( theApp.GetDisplayMode() ) {
		case SR_VGA:
			m_bodyList.InsertColumn(0, _T(""), LVCFMT_LEFT, 120*2, -1);
			m_bodyList.InsertColumn(1, _T(""), LVCFMT_LEFT, 105*2, -1);
			m_bodyList.InsertColumn(2, _T(""), LVCFMT_LEFT, 0, -1);
			break;
		case SR_QVGA:
		default:
			m_bodyList.InsertColumn(0, _T(""), LVCFMT_LEFT, 120, -1);
			m_bodyList.InsertColumn(1, _T(""), LVCFMT_LEFT, 105, -1);
			m_bodyList.InsertColumn(2, _T(""), LVCFMT_LEFT, 0, -1);
			break;
		}
	}

	// ���X�g�J�������̕ύX
	ResetColumnWidth();

	// �C���t�H���[�V�����G�f�B�b�g�̐ݒ�
	{
		// �t�H���g�ύX
		m_infoEdit.SetFont( &theApp.m_font );
	}

	// �{�^������
	theApp.EnableCommandBarButton( ID_BACK_BUTTON, FALSE);
	theApp.EnableCommandBarButton( ID_FORWARD_BUTTON, FALSE);
	theApp.EnableCommandBarButton( ID_STOP_BUTTON, FALSE);
	theApp.EnableCommandBarButton( ID_IMAGE_BUTTON, FALSE);
	theApp.EnableCommandBarButton( ID_OPEN_BROWSER, FALSE);

	m_categoryList.SetFocus();

	// �������X���b�h�J�n
	AfxBeginThread( Initialize_Thread, this );

	// �L���b�V���폜�X���b�h�J�n
	AfxBeginThread( CacheCleanup_Thread, this );
}

/**
 * �{�f�B���X�g�̃A�C�R���̃C���f�b�N�X���擾����
 */
inline int MyGetBodyListDefaultIconIndex( const CMixiData& mixi )
{
	// MZ3 API : �t�b�N�֐��Ăяo��
	util::MyLuaDataList rvals;
	rvals.push_back(util::MyLuaData(0));
	CStringA serializeKey = CStringA(theApp.m_accessTypeInfo.getSerializeKey(mixi.GetAccessType()));
	if (util::CallMZ3ScriptHookFunctions2("get_body_list_default_icon_index", &rvals, 
			util::MyLuaData(serializeKey), 
			util::MyLuaData((void*)&mixi)))
	{
		return rvals[0].m_number;
	}
	return -1;
}

/**
 * �������p�X���b�h
 */
unsigned int CMZ3View::Initialize_Thread( LPVOID This )
{
	CMZ3View* pView = (CMZ3View*)This;

	::Sleep( 10L );

	pView->DoInitialize();

	return 0;
}

/**
 * �L���b�V���폜�p�X���b�h
 */
unsigned int CMZ3View::CacheCleanup_Thread( LPVOID This )
{
	CMZ3View* pView = (CMZ3View*)This;

	::Sleep( 300L );

	// �Â��L���b�V���t�@�C���̍폜
	theApp.DeleteOldCacheFiles();

	return 0;
}

/**
 * �x�����������\�b�h�i�������p�X���b�h����N�������j
 */
bool CMZ3View::DoInitialize()
{
	util::MySetInformationText( m_hWnd, L"��ʂ��쐬���Ă��܂�..." );

	// �����f�[�^�ݒ�
	InsertInitialData();

	m_hotList = &m_categoryList;

	// ���O���[�h���Ɉړ��ł��Ȃ��悤�ɁA�A�N�Z�X�t���O�𗧂ĂĂ���
	theApp.m_access = true;

	// ���O�̃��[�h
	MyLoadCategoryLogfile( *m_selGroup->getSelectedCategory() );

	// �{�f�B���X�g�ɐݒ�
	SetBodyList( m_selGroup->getSelectedCategory()->GetBodyList() );

	// �A�N�Z�X�t���O�����낷
	theApp.m_access = false;

	// �u�b�N�}�[�N�̃��[�h
	try{
		theApp.m_bookmarkMng.Load( theApp.m_root.GetBookmarkList() );
	}catch(...){
	}

	// �V�����b�Z�[�W�m�F
	if (theApp.m_optionMng.IsBootCheckMnC() != false) {
		// �V�����b�Z�[�W�m�F
		DoNewCommentCheck();
	}

	// �X�^�C���ύX
	VIEW_STYLE newStyle = MyGetViewStyleForSelectedCategory();
	if (newStyle!=m_viewStyle) {
		m_viewStyle = newStyle;
		MySetLayout(0,0);
		InvalidateRect( m_rectIcon, FALSE );
	}

	// Twitter�X�^�C���ł���΃J�e�S���ɉ����đ��M�^�C�v��������
	if (m_viewStyle==VIEW_STYLE_TWITTER) {
		MyResetTwitterStylePostMode();

		// �R���g���[����Ԃ̕ύX
		MyUpdateControlStatus();
	}

	return true;
}

/**
 * �����f�[�^�ݒ�
 */
void CMZ3View::InsertInitialData()
{
	// �O���[�v���X�g�f�[�^�\���𔽉f����B
	for( size_t i=0; i<theApp.m_root.groups.size(); i++ ) {
		CGroupItem& group = theApp.m_root.groups[i];

		// �O���[�v�^�u�ɒǉ�����
		m_groupTab.InsertItem( i, group.name );
	}

	// �����I�����ڐݒ�
	int tabIndex = 0;
	if (0 <= theApp.m_optionMng.m_lastTopPageTabIndex && theApp.m_optionMng.m_lastTopPageTabIndex < m_groupTab.GetItemCount()) {
		// �O��I�����̃^�u�𕜋A�B
		tabIndex = theApp.m_optionMng.m_lastTopPageTabIndex;
	}
	m_groupTab.SetCurSel( tabIndex );

	// �I�𒆂̃O���[�v���ڂ̐ݒ�
	m_selGroup = &theApp.m_root.groups[tabIndex];

	// �J�e�S���̑I����Ԃ𕜋A
	int nCategory = m_selGroup->categories.size();
	int lastIdx = theApp.m_optionMng.m_lastTopPageCategoryIndex;
	if (0 <= lastIdx && lastIdx < nCategory) {
		m_selGroup->focusedCategory = m_selGroup->selectedCategory = lastIdx;

		util::MySetListCtrlItemFocusedAndSelected( m_categoryList, lastIdx, true );
	}

	// �J�e�S���[���X�g������������
	MyUpdateCategoryListByGroupItem();
}

// CMZ3View �f�f

#ifdef _DEBUG
void CMZ3View::AssertValid() const
{
	CFormView::AssertValid();
}

CMZ3Doc* CMZ3View::GetDocument() const // �f�o�b�O�ȊO�̃o�[�W�����̓C�����C���ł��B
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CMZ3Doc)));
	return (CMZ3Doc*)m_pDocument;
}
#endif //_DEBUG


// CMZ3View ���b�Z�[�W �n���h��

/**
 * �T�C�Y�ύX���̏���
 */
void CMZ3View::OnSize(UINT nType, int cx, int cy)
{
	CFormView::OnSize(nType, cx, cy);
//	MZ3LOGGER_DEBUG( L"OnSize( " + util::int2str(nType) + L", " + util::int2str(cx) + L", " + util::int2str(cy) + L" )" );

	MySetLayout( cx, cy );
}

void CMZ3View::MySetLayout(int cx, int cy)
{
//	MZ3LOGGER_DEBUG(util::FormatString(L"MySetLayout(%d,%d)", cx, cy));
/*	// �O��̒l��ۑ����A(0,0) �̏ꍇ�͂��̒l�𗘗p����
	static int s_cx = 0;
	static int s_cy = 0;
	if (cx==0 && cy==0) {
		cx = s_cx;
		cy = s_cy;
	} else {
		s_cx = cx;
		s_cy = cy;
	}
*/	{
		CRect rect;

//		GetWindowRect( &rect );
//		MZ3LOGGER_DEBUG( util::FormatString( L" wrect-cx,cy : %d, %d", rect.Width(), rect.Height() ) );

		GetClientRect( &rect );
//		MZ3LOGGER_DEBUG( util::FormatString( L" crect-cx,cy : %d, %d", rect.Width(), rect.Height() ) );

		cx = rect.Width();
		cy = rect.Height();
	}

	int fontHeight = theApp.m_optionMng.GetFontHeightByPixel(theApp.GetDPI());
	if( fontHeight == 0 ) {
		fontHeight = 12;
	}

	// ��ʉ����̏��̈�
	int hInfoBase = theApp.GetInfoRegionHeight(fontHeight);
	int hInfo     = hInfoBase;

	// �O���[�v�^�u
	int hGroup    = theApp.GetTabHeight(fontHeight);

	// ���e�̈�
	int hPost     = 0;
	CWnd* pStatusEdit   = GetDlgItem( IDC_STATUS_EDIT );
	CWnd* pUpdateButton = GetDlgItem( IDC_UPDATE_BUTTON );
	CRect rectUpdateButton;

	switch (m_viewStyle) {
	case VIEW_STYLE_DEFAULT:
		if (m_infoEdit.m_hWnd!=NULL) {
			m_infoEdit.ModifyStyle( ES_MULTILINE, 0 );
		}
		if (pStatusEdit!=NULL) {
			pStatusEdit->ShowWindow(SW_HIDE);
		}
		if (pUpdateButton!=NULL) {
			pUpdateButton->ShowWindow(SW_HIDE);
		}
		break;

	case VIEW_STYLE_IMAGE:
		if (m_infoEdit.m_hWnd!=NULL) {
			m_infoEdit.ModifyStyle( 0, ES_MULTILINE );
		}
		if (pStatusEdit!=NULL) {
			pStatusEdit->ShowWindow(SW_HIDE);
		}
		if (pUpdateButton!=NULL) {
			pUpdateButton->ShowWindow(SW_HIDE);
		}
#ifdef WINCE
		hInfo = (int)(hInfoBase * (1+0.8*(theApp.m_optionMng.m_nTwitterStatusLineCount-1)));
#else
		hInfo = (int)(hInfoBase * (1+0.5*(theApp.m_optionMng.m_nTwitterStatusLineCount-1)));
#endif
		break;

	case VIEW_STYLE_TWITTER:
		if (m_infoEdit.m_hWnd!=NULL) {
			m_infoEdit.ModifyStyle( 0, ES_MULTILINE );
		}
		if (pStatusEdit!=NULL) {
			pStatusEdit->ShowWindow(SW_SHOW);
		}
		if (pUpdateButton!=NULL) {
			pUpdateButton->ShowWindow(SW_SHOW);
			pUpdateButton->GetClientRect(&rectUpdateButton);
		}
		hPost = rectUpdateButton.Height();
#ifdef WINCE
		hInfo = (int)(hInfoBase * (1+0.8*(theApp.m_optionMng.m_nTwitterStatusLineCount-1)));
#else
		hInfo = (int)(hInfoBase * (1+0.5*(theApp.m_optionMng.m_nTwitterStatusLineCount-1)));
#endif
		break;
	}

	// �{�[�_�[�Ȃ��X�L���̏ꍇ�� hInfo ��N�s�N�Z�����炷
	if (!theApp.m_skininfo.bMainStatusBorder) {
		hInfo -= 3;
	}
	if (!theApp.m_skininfo.bMainCategoryListBorder) {
		hGroup -= 3;
	}

	// �J�e�S���A�{�f�B���X�g�̗̈�� % �Ŏw��
	// �i�A���A�J�e�S�����X�g�̓O���[�v�^�u���A�{�f�B���X�g�͏��̈���܂ށj
	const int h1 = theApp.m_optionMng.m_nMainViewCategoryListHeightRatio;
	const int h2 = theApp.m_optionMng.m_nMainViewBodyListHeightRatio;

	// cy = hGroup + hCategory + hBody + hInfo + hPost

	int hCategory = 0;
	int hBody     = 0;

	switch (m_magnifyMode) {
	case MAGNIFY_MODE_DEFAULT:
		hCategory = (cy * h1 / (h1+h2)) - (hGroup -1);
		hBody     = (cy * h2 / (h1+h2)) - (hInfo + hPost -1);
		break;

	case MAGNIFY_MODE_CATEGORY:
		{
			int hItem0 = 0;
			if (m_bodyList.GetItemCount() > 0) {
				CRect rectItem0;
				m_bodyList.GetItemRect(0, &rectItem0, LVIR_BOUNDS);
				hItem0 = rectItem0.Height();
			}
			hCategory = cy - (hInfo + hPost +hGroup -1) - hItem0;
			hBody     = hItem0;
		}
		break;

	case MAGNIFY_MODE_BODY:
	default:
		{
			// �^�u��\��
			hGroup = 0;

			int hItem0 = 0;
			if (m_categoryList.GetItemCount() > 0) {
				CRect rectItem0;
				m_categoryList.GetItemRect(0, &rectItem0, LVIR_BOUNDS);
				hItem0 = rectItem0.Height();
			}
			hCategory = hItem0 +3;
			hBody     = cy - (hInfo + hPost +hGroup -1) - (hItem0+3);
		}
		break;
	}

//	MZ3LOGGER_INFO(util::FormatString(L"cx[%d] cy[%d] group[%d] category[%d] body[%d]", 
//		cx, cy,
//		hGroup, hCategory, hBody));

	int y = 0;
	if (hGroup != 0) {
		m_groupTab.ShowWindow(SW_SHOW);
		util::MoveDlgItemWindow( this, IDC_GROUP_TAB,   0, y, cx, hGroup    );
	} else {
		m_groupTab.ShowWindow(SW_HIDE);
	}
	y += hGroup;

	util::MoveDlgItemWindow( this, IDC_HEADER_LIST, 0, y, cx, hCategory+1 );
	y += hCategory;

	util::MoveDlgItemWindow( this, IDC_BODY_LIST,   0, y, cx, hBody     );
	y += hBody;

	switch (m_viewStyle) {
	case VIEW_STYLE_DEFAULT:
		m_rectIcon.SetRect( 0, 0, 0, 0 );
		util::MoveDlgItemWindow( this, IDC_INFO_EDIT, 0, y, cx, hInfo     );
		y += hInfo;
		break;
	case VIEW_STYLE_IMAGE:
	case VIEW_STYLE_TWITTER:
		{
			// �A�C�R���̕���16, 32, 48, 64px�Ƃ���
			int iconWidth = 16;
			if (hInfo>=64) {
				iconWidth = 64;
			} else if (hInfo>=48) {
				iconWidth = 48;
			} else if (hInfo>=32) {
				iconWidth = 32;
			} else {
				iconWidth = 16;
			}
			// ������ hInfo �Œ�
			m_rectIcon.SetRect( 0, y, iconWidth, y+hInfo );
			util::MoveDlgItemWindow( this, IDC_INFO_EDIT, iconWidth, y, cx-iconWidth, hInfo     );
			y += hInfo;
		}
		break;
	}

	if (pUpdateButton!=NULL) {
		int w = rectUpdateButton.Width();
		if (hPost>0) {
			util::MoveDlgItemWindow( this, IDC_STATUS_EDIT,   0,    y, cx - w, hPost );
			util::MoveDlgItemWindow( this, IDC_UPDATE_BUTTON, cx-w, y, w,      hPost );
		}
	}

	// �����J�������[�h
	if (theApp.m_optionMng.m_bBodyListIntegratedColumnMode) {
		// �J�����w�b�_�͕s�v
		util::ModifyStyleDlgItemWindow(this, IDC_BODY_LIST, NULL, LVS_NOCOLUMNHEADER);

	} else {
		// ���x���ʂ��E�����[�h�̏ꍇ�̓X�^�C����ύX�����؂�
		if( theApp.m_optionMng.m_killPaneLabel ) {
			// �J�����w�b�_����
			util::ModifyStyleDlgItemWindow(this, IDC_BODY_LIST, NULL, LVS_NOCOLUMNHEADER);
		} else {
			// �J�����w�b�_�ݒ�
			util::ModifyStyleDlgItemWindow(this, IDC_BODY_LIST, LVS_NOCOLUMNHEADER, NULL);
		}
	}

	// �摜�ĕ`��
	MoveMiniImageDlg();

	// �v���O���X�o�[�͕ʓr�z�u
	// �T�C�Y�� hInfoBase �� 2/3 �Ƃ���
	int hProgress = hInfoBase * 2 / 3;
	y = cy -hInfo -hPost -hProgress;
	util::MoveDlgItemWindow( this, IDC_PROGRESS_BAR, 0, y, cx, hProgress );

	// ���X�g�J�������̕ύX
	ResetColumnWidth();
}

/**
 * �J�e�S�����X�g�N���b�N���̏���
 */
void CMZ3View::OnNMClickCategoryList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW lpnmlv = (LPNMLISTVIEW)pNMHDR;

	if (theApp.m_access) {
		// �A�N�Z�X���͒��~
		return;
	}

	m_hotList = &m_categoryList;
	if (lpnmlv->iItem<0) {
		return;
	}

	// �J�e�S�����X�g���́u���ݑI������Ă��鍀�ځv���X�V
	m_categoryList.Update( m_selGroup->selectedCategory );
	int idx = (int)m_categoryList.GetItemData(lpnmlv->iItem);
	if( idx < 0 || idx >= (int)m_selGroup->categories.size() ) {
		idx = 0;
	}
	m_selGroup->selectedCategory = idx;

//	m_categoryList.Update( m_selGroup->getSelectedCategory()->selectedBody );

	// �J�e�S�����X�g�\���ł���΃{�f�B���X�g�\���ɕύX
	if (m_magnifyMode != MAGNIFY_MODE_DEFAULT) {
		MySetMagnifyModeTo(MAGNIFY_MODE_BODY);
	}

	// �Ď擾
	OnMySelchangedCategoryList();

	if (theApp.m_optionMng.m_bOneClickCategoryFetchMode) {
		// ��y�C���̃��X�g�N���b�N�Ŏ擾����

		// �Ď擾
		RetrieveCategoryItem();
	}

	*pResult = 0;
}

/**
 * �J�e�S�����X�g�_�u���N���b�N���̏���
 */
void CMZ3View::OnNMDblclkCategoryList(NMHDR *pNMHDR, LRESULT *pResult)
{
	if (theApp.m_access) {
		// �A�N�Z�X���͍ăA�N�Z�X�s��
		return;
	}

	LPNMLISTVIEW lpnmlv = (LPNMLISTVIEW)pNMHDR;
	m_hotList = &m_categoryList;

	if (lpnmlv->iItem<0) {
		return;
	}

	// �J�����g�f�[�^���擾
	int idx = (int)m_categoryList.GetItemData(lpnmlv->iItem);
	m_selGroup->selectedCategory = idx;
	m_selGroup->focusedCategory  = idx;
	if (m_preCategory != lpnmlv->iItem) {
		m_categoryList.SetActiveItem(lpnmlv->iItem);
		m_categoryList.Update(m_preCategory);
		m_categoryList.Update(lpnmlv->iItem);
	}

	// �A�N�Z�X�J�n
	if (!RetrieveCategoryItem()) {
		return;
	}

	*pResult = 0;
}

/**
 * �J�e�S�����X�g�I���A�C�e���ύX���̏���
 */
void CMZ3View::OnLvnItemchangedCategoryList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);

	if (pNMLV->uNewState != 3) {
		return;
	}

	int idx = (int)m_categoryList.GetItemData( pNMLV->iItem );
	m_selGroup->focusedCategory = idx;

	if (m_selGroup->getFocusedCategory()->m_mixi.GetAccessType() == ACCESS_LIST_MYDIARY) {
		// ���L�{�^�����A�N�e�B�u�ɂ���
		theApp.EnableCommandBarButton( ID_WRITE_BUTTON, TRUE);
	} else {
		theApp.EnableCommandBarButton( ID_WRITE_BUTTON, FALSE);
	}

	// ��1�J�����ɕ\�����Ă�����e��\������
	MySetInfoEditFromBodySelectedData();

	*pResult = 0;
}

/**
 * �A�N�Z�X�I���ʒm��M(binary)
 */
LRESULT CMZ3View::OnGetEndBinary(WPARAM wParam, LPARAM lParam)
{
	MZ3LOGGER_DEBUG(_T("OnGetEndBinary start"));

	if (m_abort) {
		::SendMessage(m_hWnd, WM_MZ3_GET_ABORT, NULL, lParam);
		return TRUE;
	}

	if (lParam == NULL) {
		// �f�[�^��NULL�̏ꍇ
		LPCTSTR msg = L"�����G���[���������܂���(�߂�l��NULL)";
		MZ3LOGGER_ERROR( msg );
		util::MySetInformationText( m_hWnd, msg );
		return TRUE;
	}

	CMixiData* data = (CMixiData*)lParam;
	ACCESS_TYPE aType = data->GetAccessType();

	switch (aType) {
	case ACCESS_IMAGE:
		{
			// �R�s�[
			CString path = util::MakeImageLogfilePathFromUrlMD5( theApp.m_inet.GetURL() );
			CopyFile( theApp.m_filepath.temphtml, path, FALSE/*bFailIfExists, �㏑��*/ );

			// ���݂��Ȃ��t�@�C�����X�g�ɓo�^����Ă���΍폜����
			if (theApp.m_notFoundFileList.count((LPCTSTR)path)>0) {
				theApp.m_notFoundFileList.erase((LPCTSTR)path);
			}

			// �ĕ`��
			MyRedrawBodyImages();

			// �u�A�C�R���̍쐬�����v��N�b��ɖ߂�
			::SetTimer(theApp.m_pMainView->m_hWnd, TIMERID_RESTORE_STATUSBAR, 1000L, NULL);
		}
		break;

	case ACCESS_DOWNLOAD:
		{
			// �p�X����
			CString strFilepath;
			{
				CString url = theApp.m_inet.GetURL();

				strFilepath.Format(_T("%s\\%s"), 
					theApp.m_filepath.downloadFolder, 
					util::ExtractFilenameFromUrl( url, L"_mz3_noname.dat" ) );

				// �R�s�[
				CopyFile( theApp.m_filepath.temphtml, strFilepath, FALSE/*bFailIfExists, �㏑��*/ );
			}

			// �_�E�����[�h�̏ꍇ�́A���s���m�F����B
			if (theApp.m_optionMng.m_bUseRunConfirmDlg) {
				CString msg;
				msg.Format( 
					L"�_�E�����[�h���������܂����I\n\n"
					L"�t�@�C�� %s ���J���܂����H", strFilepath );

				if (MessageBox( msg, 0, MB_YESNO | MB_ICONQUESTION | MB_DEFBUTTON2 ) == IDYES) {
					util::OpenByShellExecute(strFilepath);
				}
			} else {
				util::OpenByShellExecute(strFilepath);
			}
		}
		break;
	}

	// �ʐM�����i�t���O��������j
	theApp.m_access = false;

	// �J�e�S���ɉ�����Twitter���M���[�h��������
	MyResetTwitterStylePostMode();

	// �R���g���[����Ԃ̕ύX
	MyUpdateControlStatus();

	MZ3LOGGER_DEBUG(_T("OnGetEndBinary end"));

	return TRUE;
}

/**
 * �A�N�Z�X�I���ʒm��M
 */
LRESULT CMZ3View::OnGetEnd(WPARAM wParam, LPARAM lParam)
{
	TRACE(_T("InetAccess End\n"));

	util::MySetInformationText( m_hWnd, _T("��͒�") );

//	util::StopWatch sw;

	if (m_abort) {
		::SendMessage(m_hWnd, WM_MZ3_GET_ABORT, NULL, lParam);
		return TRUE;
	}

	// �ʐM�����i�t���O��������j
	theApp.m_access = false;

	if (lParam == NULL) {
		// �f�[�^��NULL�̏ꍇ
		LPCTSTR msg = L"�����G���[���������܂���(LPARAM=NULL)";
		MZ3LOGGER_ERROR( msg );
		util::MySetInformationText( m_hWnd, msg );
		return TRUE;
	}

	CMixiData* data = (CMixiData*)lParam;
	ACCESS_TYPE aType = data->GetAccessType();

	if (theApp.m_accessTypeInfo.getInfoType(aType)==AccessTypeInfo::INFO_TYPE_POST) {
		// POST �ɑ΂��鏈���� OnPostEnd �ɈϏ�����
		return OnPostEnd(wParam, lParam);
	}

	// ���O�C���y�[�W�ȊO�ł���΁A�ŏ��Ƀ��O�A�E�g�`�F�b�N���s���Ă���
	if (aType != ACCESS_LOGIN) {
		// HTML �̎擾

		// ���O�A�E�g�`�F�b�N
		if (theApp.IsMixiLogout(aType)) {
			// ���O�A�E�g��ԂɂȂ��Ă���
			MZ3LOGGER_INFO(_T("�ēx���O�C�����Ă���f�[�^���擾���܂��B"));
			util::MySetInformationText( m_hWnd, L"�ēx���O�C�����Ă���f�[�^���擾���܂�" );

			// mixi �f�[�^��ۑ��i�Ҕ��j
			theApp.m_mixiBeforeRelogin = *data;

			// ���O�C�����s
			theApp.m_access = true;
			m_abort = FALSE;

			// �R���g���[����Ԃ̕ύX
			MyUpdateControlStatus();

			theApp.StartMixiLoginAccess(m_hWnd, data);

			return TRUE;
		}
	}

	// ���O�A�E�g���Ă��Ȃ������̂Ńt�@�C���R�s�[
	if( theApp.m_optionMng.m_bSaveLog ) {
		// �ۑ��t�@�C���p�X�̐���
		CString strLogfilePath = util::MakeLogfilePath( *data );
		if(! strLogfilePath.IsEmpty() ) {
			// �ۑ��t�@�C���ɃR�s�[
			CopyFile( theApp.m_filepath.temphtml, strLogfilePath, FALSE/*bFailIfExists, �㏑��*/ );
		}
	}

	switch (aType) {
	case ACCESS_LOGIN:
		// --------------------------------------------------
		// ���O�C������
		// --------------------------------------------------
		{
			// HTML �̎擾
			CHtmlArray html;
			html.Load( theApp.m_filepath.temphtml );

			if( mixi::HomeParser::IsLoginSucceeded(html) ) {
				// ���O�C������
				if (wcslen(theApp.m_loginMng.GetMixiOwnerID()) == 0) {
					// �I�[�i�[�h�c���擾����
					data->SetAccessType(ACCESS_MAIN); // �A�N�Z�X��ʂ�ݒ�
					AccessProc(data, _T("http://mixi.jp/check.pl?n=%2Fhome.pl"));
				}
				else {
					// �f�[�^��Ҕ��f�[�^�ɖ߂�
					*data = theApp.m_mixiBeforeRelogin;
					AccessProc(data, util::CreateMixiUrl(data->GetURL()));
				}
				return TRUE;
			} else {
				// ���O�C�����s
				LPCTSTR emsg = _T("���O�C���o���܂���ł���");
				::MessageBox(m_hWnd, emsg, MZ3_APP_NAME, MB_ICONSTOP | MB_OK);
				MZ3LOGGER_ERROR( emsg );

				// �f�[�^��Ҕ��f�[�^�ɖ߂�
				*data = theApp.m_mixiBeforeRelogin;

				// �R���g���[����Ԃ̕ύX
				MyUpdateControlStatus();

				util::MySetInformationText( m_hWnd, emsg );
				return TRUE;
			}
			break;
		}

	case ACCESS_MAIN:
		// --------------------------------------------------
		// ���C�����
		// --------------------------------------------------
		{
			// HTML �̎擾
			CHtmlArray html;
			html.Load( theApp.m_filepath.temphtml );

			// �p�[�X
			theApp.DoParseMixiHomeHtml(data, &html);

			if (m_checkNewComment) {
				// �R�����g�E���b�Z�[�W���`�F�b�N���[�h

				// �V�����b�Z�[�W�A�V���R�����g�̒ʒm
				CString msg;

				int n = 0;
				
				n = data->GetIntValue(L"new_message_count", 0);
				if (n > 0) {
					msg.AppendFormat(_T(" �V�����b�Z�[�W:%d�� "), n);
				}

				n = data->GetIntValue(L"new_comment_count", 0);
				if (n > 0) {
					msg.AppendFormat(_T(" �V���R�����g:%d�� "), n);
				}

				n = data->GetIntValue(L"new_apply_count", 0);
				if (n > 0) {
					msg.AppendFormat(_T(" ���F�҂�:%d�l"), n);
				}

				if (msg.IsEmpty()) {
					msg = _T("�V�����b�Z�[�W�A�R�����g�͂���܂���");
				}

				util::MySetInformationText( m_hWnd, msg );

				m_checkNewComment = false;
			} else {
				// �V�����b�Z�[�W�ȊO�Ȃ̂ŁA���O�C���̂��߂̎擾�������B

				// �f�[�^��Ҕ��f�[�^�ɖ߂�
				*data = theApp.m_mixiBeforeRelogin;

				AccessProc(data, util::CreateMixiUrl(data->GetURL()));

				return TRUE;
			}
		}
		break;

	case ACCESS_RSS_READER_AUTO_DISCOVERY:
		// RSS AutoDiscovery �����݂�
		DoAccessEndProcForRssAutoDiscovery();
		break;

	case ACCESS_SOFTWARE_UPDATE_CHECK:
		// �o�[�W�����`�F�b�N
		if (!DoAccessEndProcForSoftwareUpdateCheck()) {
			MessageBox(L"�o�[�W�����`�F�b�N�Ɏ��s���܂����B���΂炭���Ă���ēx�������������B");
		}
		util::MySetInformationText( m_hWnd, L"����" );
		break;

	default:
		switch (theApp.m_accessTypeInfo.getInfoType(aType)) {
		case AccessTypeInfo::INFO_TYPE_CATEGORY:
			// --------------------------------------------------
			// �J�e�S�����ڂ̎擾
			// --------------------------------------------------
			if (DoAccessEndProcForBody(aType)) {
				return TRUE;
			}
			break;

		case AccessTypeInfo::INFO_TYPE_BODY:
			// --------------------------------------------------
			// �{�f�B���ڂ̎擾
			// --------------------------------------------------

			// ���񃂁[�h�Ȃ�i��͂����Ɂj���̏���ΏۂɈړ�����
			if( m_cruise.enable() ) {
				// ���̏���Ώۂ�����Ύ擾����
				// �Ȃ���ΏI������
				m_cruise.targetBodyItem++;

				// �I������
				util::MySetListCtrlItemFocusedAndSelected( m_bodyList, m_selGroup->getSelectedCategory()->selectedBody,  false );

				if( DoNextBodyItemCruise() ) {
					// �ʐM�p���̂��߂����� return ����
					return TRUE;
				}
			}else{
				// ���񃂁[�h�łȂ��̂ŁA��͂��ă��|�[�g��ʂ��J��
				static CMixiData mixi;
				mixi = *data;
				MyParseMixiHtml( theApp.m_filepath.temphtml, mixi );
				MyShowReportView( mixi );
			}
			break;
		default:
			util::MySetInformationText( m_hWnd, L"�����i�A�N�Z�X��ʕs���j" );
			break;
		}
	}

	// �J�e�S���ɉ�����Twitter���M���[�h��������
	MyResetTwitterStylePostMode();

	// �R���g���[����Ԃ̕ύX
	MyUpdateControlStatus();

	// �t�H�[�J�X�̕ύX
	MyUpdateFocus();

	return TRUE;
}

/**
* �A�N�Z�X�G���[�ʒm��M
*/
LRESULT CMZ3View::OnGetError(WPARAM wParam, LPARAM lParam)
{
	CMixiData* pMixi = (CMixiData*)lParam;

	ACCESS_TYPE aType = pMixi->GetAccessType();
	if (aType == ACCESS_LOGIN) {
		// ���O�C���̏ꍇ�͑Ҕ������Ă��邩�猳�ɖ߂�
		*pMixi = theApp.m_mixiBeforeRelogin;
	}

	// �ʐM�G���[�����������ꍇ�̏���
	LPCTSTR smsg = L"�G���[���������܂���";
	util::MySetInformationText( m_hWnd, smsg );

	CString msg;
	msg.Format( 
		L"%s\n\n"
		L"�����F%s", smsg, theApp.m_inet.GetErrorMessage() );
//	::MessageBox(m_hWnd, msg, MZ3_APP_NAME, MB_ICONSTOP | MB_OK);
	MZ3LOGGER_ERROR( msg );

	theApp.m_access = false;

	// �J�e�S���ɉ�����Twitter���M���[�h��������
	MyResetTwitterStylePostMode();

	// �R���g���[����Ԃ̕ύX
	MyUpdateControlStatus();

	// �t�H�[�J�X�̕ύX
	MyUpdateFocus();

	return TRUE;
}

/**
 * �A�N�Z�X���f�ʒm��M
 * 
 * ���[�U�w���ɂ�钆�f
 * 
 */
LRESULT CMZ3View::OnGetAbort(WPARAM wParam, LPARAM lParam)
{
	m_abort = FALSE;
	m_cruise.stop();

	return TRUE;
}

/**
 * ���f�{�^���������̏���
 */
LRESULT CMZ3View::OnAbort(WPARAM wParam, LPARAM lParam)
{
	if( theApp.m_inet.IsConnecting() ) {
		// �ʐM���Ȃ� Abort ���Ăяo��
		theApp.m_inet.Abort();
	}

	// ���f�𑗐M
	m_abort = TRUE;
	m_cruise.stop();

	theApp.m_access = false;

	// �J�e�S���ɉ�����Twitter���M���[�h��������
	MyResetTwitterStylePostMode();

	// �R���g���[����Ԃ̕ύX
	MyUpdateControlStatus();

	LPCTSTR msg = _T("���f���܂���");
	util::MySetInformationText( m_hWnd, msg );
//	::MessageBox(m_hWnd, msg, MZ3_APP_NAME, MB_ICONSTOP | MB_OK);

	// �t�H�[�J�X�̕ύX
	MyUpdateFocus();

	return TRUE;
}

/**
 * �A�N�Z�X���ʒm
 */
LRESULT CMZ3View::OnAccessInformation(WPARAM wParam, LPARAM lParam)
{
	m_infoEdit.SetWindowText( *(CString*)lParam );
	return TRUE;
}

/**
 * ��M�T�C�Y�ʒm
 */
LRESULT CMZ3View::OnAccessLoaded(WPARAM dwLoaded, LPARAM dwLength)
{
	if( dwLength == 0 ) {
		// ���T�C�Y���s���Ȃ̂ŁA���� N KB �Ƃ݂Ȃ�
		dwLength = 40 * 1024;
	}

	// ��M�T�C�Y�Ƒ��T�C�Y������Ȃ�A��M�����Ƃ݂Ȃ��A�v���O���X�o�[���\���ɂ���B
	if( dwLoaded == dwLength ) {
		mc_progressBar.ShowWindow( SW_HIDE );
		return TRUE;
	}

	// �v���O���X�o�[��\��
	mc_progressBar.ShowWindow( SW_SHOW );
	::SetWindowPos( mc_progressBar.m_hWnd, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOREPOSITION );

	// [0,1000] �Ŏ�M�T�C�Y�ʒm
	int pos = (int)(dwLoaded * 1000.0 / dwLength);
	mc_progressBar.SetPos( pos );

	return TRUE;
}

/**
 * �{�f�B�̃C���[�W���X�g���쐬
 *
 * �{�f�B���X�g�͍쐬�ς݂ł��邱�ƁB
 */
void CMZ3View::SetBodyImageList( CMixiDataList& body )
{
	util::MySetInformationText( m_hWnd, L"�A�C�R���쐬��..." );

	util::StopWatch sw_generate_icon, sw_set_icon;

	bool bUseDefaultIcon = false;
	bool bUseExtendedIcon = false;

	// �A�C�R���̕\���E��\���̐���
	// ���j�F(1) �I�v�V�����l�ɂ���\���ɂȂ��Ă���΃A�C�R����\���B
	//       (2) �S�A�C�e���𑖍����A�A�C�R�����K�v�ȍ��ڂ�����΃A�C�R���\���A�Ȃ���Δ�\���B
	INT_PTR count = body.size();
	if (theApp.m_optionMng.m_bShowMainViewIcon) {
		for (int i=0; i<count; i++) {
			int iconIndex = MyGetBodyListDefaultIconIndex(body[i]);
			if (iconIndex >= 0) {
				// �A�C�R������Ȃ̂ŕ\��
				bUseDefaultIcon = true;
				break;
			}
		}
	}

	// ���[�U��R�~���j�e�B�̉摜���A�C�R�������ĕ\������
	DWORD dwLoadAndResizeMS = 0;
	int   nLoadAndResize = 0;
	DWORD dwMakeImageLogfilePathMS = 0;
	DWORD dwAppendDLMMS = 0;

	sw_generate_icon.start();
	if (theApp.m_optionMng.m_bShowMainViewMiniImage && !bUseDefaultIcon) {
		// �f�t�H���g�A�C�R�����Ȃ������̂ŁA���[�U�E�R�~���j�e�B�A�C�R�������쐬����
		for (int i=0; i<count; i++) {
			// �^�u�؂�ւ����s����΃L�����Z��
			if (m_bReloadingGroupTabByThread && m_bRetryReloadGroupTabByThread) {
				return;
			}
			const CMixiData& mixi = body[i];

			util::StopWatch sw;
			sw.start();
			CString miniImagePath = util::MakeImageLogfilePath( body[i] );
			dwMakeImageLogfilePathMS += sw.getElapsedMilliSecUntilNow();

			if (theApp.m_imageCache.GetImageIndex(miniImagePath) >= 0) {
				// ���[�h�ς݂Ȃ̂Ń��[�h�s�v
				bUseExtendedIcon = true;
			} else {
				util::StopWatch sw_load_and_resize;
				sw_load_and_resize.start();

				// �����[�h�Ȃ̂Ń��[�h
				bool bLoaded = false;

				// WM ����I/O���x���̂Łu���݂��Ȃ��t�@�C�����X�g�v���Q�Ƃ���
				bool bExist = false;
				if (theApp.m_notFoundFileList.count((LPCTSTR)miniImagePath)==0) {
					if (util::ExistFile(miniImagePath)) {
						bExist = true;
					} else {
						theApp.m_notFoundFileList.insert((LPCTSTR)miniImagePath);
					}
				}

				if (bExist) {
					CMZ3BackgroundImage image(L"");
					image.load( miniImagePath );
					if (image.isEnableImage()) {
						// ���T�C�Y���ĉ摜�L���b�V���ɒǉ�����B
//						CSize size = image.getBitmapSize();
//						MZ3LOGGER_DEBUG(util::FormatString(L"AddImageToImageCache from %dx%d", size.cx, size.cy));
						theApp.AddImageToImageCache(this, image, miniImagePath);

						bUseExtendedIcon = true;

						bLoaded = true;
					} else {
						// TODO ����p�����邩������Ȃ��̂ŉ��L�̃R�[�h�͓���m�F��ɗL�������邱��

						// ���[�h���s�́u�t�@�C�������݂��Ȃ��v�Ƃ݂Ȃ��A�Ȍ�`�F�b�N���Ȃ�
						//theApp.m_notFoundFileList.insert((LPCTSTR)miniImagePath);
					}
				}
				dwLoadAndResizeMS += sw_load_and_resize.getElapsedMilliSecUntilNow();
				nLoadAndResize ++;

				if (!bLoaded) {
					// ���[�h�G���[ => �_�E�����[�h�}�l�[�W���ɓo�^����
					if (mixi.GetImageCount()>0 && !miniImagePath.IsEmpty()) {

						util::StopWatch sw;
						sw.start();

						CString url = mixi.GetImage(0);
						DownloadItem item( url, L"�G����", miniImagePath, true );
						theApp.m_pDownloadView->AppendDownloadItem( item );

						dwAppendDLMMS += sw.getElapsedMilliSecUntilNow();
					}
				}
			}
		}
	}
	sw_generate_icon.stop();

	// �s�̍��������ꂽ��Ԃŕ`�悳���̂�������邽�ߕ`���~
	m_bodyList.m_bStopDraw = true;

	// �A�C�R���\���E��\���ݒ�
	CBodyListCtrl::ICON_MODE iconMode = CBodyListCtrl::ICON_MODE_NONE;
	if (bUseDefaultIcon) {
		// �f�t�H���g�A�C�R��
		// 32px �̃f�t�H���g�A�C�R�����ł����牺�L�̃R�����g���O�����ƁB
//		if (theApp.m_optionMng.m_bBodyListIntegratedColumnMode) {
			// �����J�������[�h
//			CRect rect;
//			m_bodyList.GetItemRect(0, rect, LVIR_BOUNDS);
//			if (rect.Height()>=32) {
//				m_bodyList.SetImageList(&m_iconImageListLarge, LVSIL_SMALL);
//				iconMode = CBodyListCtrl::ICON_MODE_32;
//			} else {
//				m_bodyList.SetImageList(&m_iconImageListSmall, LVSIL_SMALL);
//				iconMode = CBodyListCtrl::ICON_MODE_16;
//			}
//		} else {
			m_bodyList.SetImageList(&m_iconImageListSmall, LVSIL_SMALL);
			iconMode = CBodyListCtrl::ICON_MODE_16;
//		}
	} else if (bUseExtendedIcon) {
		if (theApp.m_optionMng.m_bBodyListIntegratedColumnMode) {
			// �����J�������[�h
			MEASUREITEMSTRUCT measureItemStruct;
			m_bodyList.MeasureItem(&measureItemStruct);
			MZ3_TRACE(L"�A�C�e���̍��� : %d\n", measureItemStruct.itemHeight);
#ifdef WINCE
			bool bWM = true;
#else
			bool bWM = false;
#endif
			int hItem = measureItemStruct.itemHeight;
			// WM      �ł� 64 48 32 16
			// Windows �ł� 32 16
			// �ŕ\������
			if (bWM && hItem>=64) {
				m_bodyList.SetImageList(&theApp.m_imageCache.GetImageList64(), LVSIL_SMALL);
				iconMode = CBodyListCtrl::ICON_MODE_64;
			} else if (bWM && hItem>=48) {
				m_bodyList.SetImageList(&theApp.m_imageCache.GetImageList48(), LVSIL_SMALL);
				iconMode = CBodyListCtrl::ICON_MODE_48;
			} else if (hItem>=32) {
				m_bodyList.SetImageList(&theApp.m_imageCache.GetImageList32(), LVSIL_SMALL);
				iconMode = CBodyListCtrl::ICON_MODE_32;
			} else {
				m_bodyList.SetImageList(&theApp.m_imageCache.GetImageList16(), LVSIL_SMALL);
				iconMode = CBodyListCtrl::ICON_MODE_16;
			}
		} else {
			m_bodyList.SetImageList(&theApp.m_imageCache.GetImageList16(), LVSIL_SMALL);
			iconMode = CBodyListCtrl::ICON_MODE_16;
		}
	}

	// �A�C�R���`�惂�[�h�̐ݒ�
	m_bodyList.MySetIconMode( iconMode );

	// �A�C�R���̐ݒ�
	sw_set_icon.start();
	int itemCount = m_bodyList.GetItemCount();
	for (int i=0; i<itemCount; i++) {
		const CMixiData& mixi = body[i];

		// �A�C�R���̃C���f�b�N�X����ʂɂ��ݒ肷��
		int iconIndex = -1;
		if (bUseDefaultIcon ) {
			iconIndex = MyGetBodyListDefaultIconIndex(mixi);
		}
		if (bUseExtendedIcon) {
			// �t�@�C���p�X����C���f�b�N�X����������
			CString miniImagePath = util::MakeImageLogfilePath( body[i] );

			// �C���f�b�N�X�T��
			iconIndex = theApp.m_imageCache.GetImageIndex( miniImagePath );
		}

		// �A�C�R���̃C���f�b�N�X��ݒ�
		util::MySetListCtrlItemImageIndex( m_bodyList, i, 0, iconIndex );
	}
	sw_set_icon.stop();

	m_bodyList.m_bStopDraw = false;

	// �A�C�R����ݒ肷�邱�ƂŃ{�f�B���X�g�̃A�C�e���̍������V�X�e���W���ɖ߂��Ă���ꍇ������B
	// �����������邽�߁A�ēx�t�H���g��ݒ肷�邱�ƂŁA
	// OnSetFont() => MeasureItem() ���Ăяo�����悤�ɂ��A
	// �{�f�B���X�g�̃A�C�e���̍������Đݒ肷��B
	m_bodyList.SetFont( &theApp.m_font );

	// �X�^�C���ύX
	VIEW_STYLE newStyle = MyGetViewStyleForSelectedCategory();
	if (newStyle!=m_viewStyle) {
		m_viewStyle = newStyle;
		MySetLayout(0,0);
		InvalidateRect( m_rectIcon, FALSE );
	}

	util::MySetInformationText( m_hWnd, L"�A�C�R���̍쐬����" );
	MZ3LOGGER_DEBUG(
		util::FormatString(
			L"�A�C�R����������, generate[%dms](load/resize[%dms][%d], path resolve[%dms], dlm append[%dms]), "
			L"set[%dms]", 
			sw_generate_icon.getElapsedMilliSecUntilStoped(),
			dwLoadAndResizeMS,
			nLoadAndResize,
			dwMakeImageLogfilePathMS,
			dwAppendDLMMS,
			sw_set_icon.getElapsedMilliSecUntilStoped()));
}

/**
 * �{�f�B�Ƀf�[�^��ݒ�
 */
void CMZ3View::SetBodyList( CMixiDataList& body )
{
	m_bModifyingBodyList = true;

	// ���X�g�̃A�C�e�����폜
	m_bodyList.m_bStopDraw = true;
	m_bodyList.SetRedraw(FALSE);
	m_bodyList.DeleteAllItems();

	// �w�b�_�̕�����ύX
	CCategoryItem* pCategory = m_selGroup->getSelectedCategory();
	if (pCategory != NULL) {
		LPCTSTR szHeaderTitle1 = MyGetBodyHeaderColName1( pCategory->m_mixi.GetAccessType() );
		LPCTSTR szHeaderTitle2 = MyGetBodyHeaderColName2( pCategory->m_mixi.GetAccessType(), pCategory->m_bodyColType2 );
		LPCTSTR szHeaderTitle3 = MyGetBodyHeaderColName2( pCategory->m_mixi.GetAccessType(), pCategory->m_bodyColType3 );
		m_bodyList.SetHeader( szHeaderTitle1, szHeaderTitle2, szHeaderTitle3 );
	}

	// �A�C�e���̒ǉ�
	m_bodyList.MySetIconMode( CBodyListCtrl::ICON_MODE_NONE );	// �܂��A�C�R���̓I�t�ɂ��Đ���
	util::StopWatch sw;
	sw.start();
	int count = body.size();
	for (int i=0; i<count; i++) {
		// �^�u�؂�ւ����s����΃L�����Z��
		if (m_bReloadingGroupTabByThread && m_bRetryReloadGroupTabByThread) {
			m_bModifyingBodyList = false;
			return;
		}

		// ������͕\�����Ɏ擾����
		m_bodyList.InsertItem( i, L"", -1 );
	}
	MZ3LOGGER_DEBUG(
		util::FormatString(L"�{�f�B���X�g�ݒ芮��, elapsed[%dms], count[%d]", 
		sw.getElapsedMilliSecUntilNow(), m_bodyList.GetItemCount()));

	// �����J�������[�h�p�̃t�H�[�}�b�^��ݒ肷��
	if (pCategory!=NULL) {
		m_bodyList.m_strIntegratedLinePattern1 = 
			theApp.m_accessTypeInfo.getBodyIntegratedLinePattern1(pCategory->m_mixi.GetAccessType());
		m_bodyList.m_strIntegratedLinePattern2 = 
			theApp.m_accessTypeInfo.getBodyIntegratedLinePattern2(pCategory->m_mixi.GetAccessType());
	}

	m_bModifyingBodyList = false;
	util::MySetListCtrlItemFocusedAndSelected( m_bodyList, 0, true );

	// �A�C�R���pImageList�̐ݒ�
	SetBodyImageList( body );

	m_bodyList.SetRedraw(TRUE);
	m_bodyList.m_bStopDraw = false;

	// �o�b�N�o�b�t�@�o�R�ōĕ`��
	m_bodyList.DrawDetail();
	m_bodyList.UpdateWindow();

	if (m_bodyList.GetItemCount()==0) {
		// �A�C�e����0��
		util::MySetInformationText( m_hWnd, L"����" );
	} else {
		// ��1�J�����ɕ\�����Ă�����e��\������
		MySetInfoEditFromBodySelectedData();
	}
}

/**
 * ���s�̃t�H�[�J�X�ݒ�
 */
void CMZ3View::OnEnSetfocusInfoEdit()
{
	/*
	// ���̍s�ɂ̓t�H�[�J�X���ڂ��Ȃ�
	// �J�e�S�����X�g�Ƀt�H�[�J�X���ڂ�
	m_categoryList.SetFocus();
	*/

	// 3�X�e�[�g

	// �W��:�J�e�S�����{�f�B���X�g�����̂܂܂̔䗦
	// �J�e�S�����X�g�\��:�J�e�S�����X�g���قڑS��ʕ\���A�{�f�B���X�g���ŉ�����1�s�\��
	// �{�f�B���X�g�\��  :�{�f�B���X�g���قڑS��ʕ\���A�J�e�S�����X�g���ŏ㕔��1�s�\��

	switch (m_magnifyMode) {
	case MAGNIFY_MODE_DEFAULT:
		m_magnifyMode = MAGNIFY_MODE_CATEGORY;
		break;

	case MAGNIFY_MODE_CATEGORY:
		m_magnifyMode = MAGNIFY_MODE_BODY;
		break;

	case MAGNIFY_MODE_BODY:
	default:
		m_magnifyMode = MAGNIFY_MODE_DEFAULT;
		break;
	}

	// ���C�A�E�g�ύX���f
	MySetLayout(0, 0);
	InvalidateRect( m_rectIcon, FALSE );

	// �ύX��̃��[�h�ɉ����ăt�H�[�J�X�ύX
	switch (m_magnifyMode) {
	case MAGNIFY_MODE_DEFAULT:
		m_bodyList.SetFocus();
		m_infoEdit.SetWindowText( L"�W�����[�h" );
		break;

	case MAGNIFY_MODE_CATEGORY:
		m_categoryList.SetFocus();
		m_infoEdit.SetWindowText( L"��y�C���ő�\��:�X�e�[�^�X�o�[�����Ŗ߂�܂�" );
		break;

	case MAGNIFY_MODE_BODY:
	default:
		m_infoEdit.SetWindowText( L"���y�C���ő�\��:�X�e�[�^�X�o�[�����Ŗ߂�܂�" );
		m_bodyList.SetFocus();
		break;
	}

	// N�b��ɃX�e�[�^�X�o�[��߂�
	::SetTimer(theApp.m_pMainView->m_hWnd, TIMERID_RESTORE_STATUSBAR, 2000L, NULL);
}

/**
 * �{�f�B���X�g�N���b�N
 */
void CMZ3View::OnNMDblclkBodyList(NMHDR *pNMHDR, LRESULT *pResult)
{
//	MZ3LOGGER_DEBUG( L"OnNMDblclkBodyList start" );
	*pResult = 0;

	if (theApp.m_access) {
		// �A�N�Z�X���͍ăA�N�Z�X�s��
		return;
	}

	LPNMLISTVIEW lpnmlv = (LPNMLISTVIEW)pNMHDR;

	// �J�����g�f�[�^���擾
	m_hotList = &m_bodyList;
	m_selGroup->getSelectedCategory()->selectedBody = lpnmlv->iItem;

	if( lpnmlv->iItem <0 ){
		return;
	}

	CMixiData& data = GetSelectedBodyItem();

	TRACE(_T("http://mixi.jp/%s\n"), data.GetURL());

	// MZ3 API : �t�b�N�֐��Ăяo��
	if (util::CallMZ3ScriptHookFunctions(
			  theApp.m_accessTypeInfo.getSerializeKey(data.GetAccessType()),
			  "dblclk_body_list", &data))
	{
		// �t�b�N�֐��o�^�ς݁��R�[�������̂��ߏI��
		return;
	}

	switch (data.GetAccessType()) {
	case ACCESS_LIST_FOOTSTEP:
		return;

	case ACCESS_COMMUNITY:
		// �R�~���j�e�B�̏ꍇ�́A�g�s�b�N�ꗗ��\������B
		// �i�b��Ή��j
		OnViewBbsList();
		return;

	case ACCESS_RSS_READER_ITEM:
		// �ڍו\��
		OnMenuRssRead();
		return;

	default:
		// ����ȗv�f�ȊO�Ȃ̂ŁA�ʐM�����J�n�B
		AccessProc(&data, util::CreateMixiUrl(data.GetURL()));
	}
}

/**
 * �A�C�e���ύX
 */
void CMZ3View::OnLvnItemchangedBodyList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);

	if (m_bModifyingBodyList) {
		// �A�C�e���ǉ����Ȃ̂ŃC�x���g�n���h�������Ȃ�
		return;
	}

	// �I����ύX������A���̃A�C�e�����A�N�e�B�u�ɂ���
	if (pNMLV->uNewState != 3) {
		return;
	}

	CCategoryItem* pCategory = m_selGroup->getSelectedCategory();
	pCategory->selectedBody = pNMLV->iItem;

	// ��1�J�����ɕ\�����Ă�����e��\������B
	MySetInfoEditFromBodySelectedData();

	// Twitter �ł���΍ĕ\��(����ID����p�҂̐F�ύX�𔺂�����)
	if (util::IsTwitterAccessType(pCategory->m_mixi.GetAccessType())) {
		static int s_lastSelected = 0;
		int selected = pCategory->selectedBody;

		if (!m_bodyList.m_bStopDraw) {
//			util::StopWatch sw_detect, sw_draw;
//			sw_detect.start();

			static std::set<int> redrawItems;
			redrawItems.clear();

			// selected, s_lastSelected �͍ĕ`��
//			if (selected < m_bodyList.GetItemCount()) {
//				redrawItems.insert( selected );
//			}
			if (s_lastSelected < m_bodyList.GetItemCount()) {
				redrawItems.insert( s_lastSelected );
			}

			CMixiData& sel_data      = pCategory->m_body[selected];
			CMixiData& last_sel_data = pCategory->m_body[s_lastSelected];

			// selected, s_lastSelected �Ɠ���ID�̍��ڂ͍ĕ`��
			// (�A���Aselected, s_lastSelected ��ID���قȂ�ꍇ)
			int idxStart = m_bodyList.GetTopIndex();
			int idxEnd   = idxStart + m_bodyList.GetCountPerPage();
			if (sel_data.GetOwnerID() == last_sel_data.GetOwnerID()) {
				// ����ID�̍��ڂ͕`���Ԃɕω����Ȃ��̂ōĕ`�悵�Ȃ�
			} else {
				// �ĕ`��ΏۂƂȂ郆�[�UID
				static std::set<int> redrawTargetUsers;
				redrawTargetUsers.clear();

				redrawTargetUsers.insert(sel_data.GetOwnerID());
				redrawTargetUsers.insert(last_sel_data.GetOwnerID());

				for (int idx=idxStart; idx<=idxEnd; idx++) {
					if (idx == selected || idx == s_lastSelected) {
						continue;
					}

					if (redrawItems.count(idx)==0 && 0 <= idx && idx < (int)pCategory->m_body.size()) {
						int id = pCategory->m_body[idx].GetOwnerID();
						if (redrawTargetUsers.count(id) > 0) {
							redrawItems.insert(idx);
						}
					}
				}
			}

			// selected, s_lastSelected �̌��y���[�U�͍ĕ`��
			static std::set<CString> redrawTargetUsers;
			redrawTargetUsers.clear();
			for (int i=0; i<2; i++) {
				MZ3Data* pSelectedData = (i==0) ? &sel_data : &last_sel_data;
				// �I�����ړ��̈��p���[�U���X�g���擾����B�Ȃ���΂����ō��B
				util::SetTwitterQuoteUsersWhenNotGenerated(pSelectedData);

				int n = pSelectedData->GetTextArraySize(L"quote_users");
				for (int i=0; i<n; i++) {
					redrawTargetUsers.insert(pSelectedData->GetTextArrayValue(L"quote_users", i));
				}
			}

			for (int idx=idxStart; idx<=idxEnd; idx++) {
				if (idx == selected || idx == s_lastSelected) {
					continue;
				}

				if (redrawItems.count(idx)==0 && 0 <= idx && idx < (int)pCategory->m_body.size()) {
					if (redrawTargetUsers.count(pCategory->m_body[idx].GetName()) > 0) {
						redrawItems.insert(idx);
					}
				}
			}
//			sw_detect.stop();

			
			// �ĕ`��
//			sw_draw.start();
			for (std::set<int>::iterator it=redrawItems.begin(); it!=redrawItems.end(); it++) {
				int idx = (*it);
				m_bodyList.DrawItemWithBackSurface(idx);
			}
//			sw_draw.stop();


			// �x���`�}�[�N���ʏo��
//			MZ3LOGGER_DEBUG(
//				util::FormatString(L"*** Twitter Detect/Draw n[%d] detect[%dms] draw[%dms]",
//					redrawItems.size(),
//					sw_detect.getElapsedMilliSecUntilStoped(),
//					sw_draw.getElapsedMilliSecUntilStoped()));
		}

		s_lastSelected = selected;
	} 

	// �摜�ʒu�ύX
//	util::StopWatch sw_draw1, sw_draw2;
//	sw_draw1.start();
	MoveMiniImageDlg();
//	sw_draw1.stop();

	// �A�C�R���ĕ`��
//	sw_draw2.start();
	InvalidateRect( m_rectIcon, FALSE );
//	sw_draw2.stop();

	// �x���`�}�[�N���ʏo��
//	MZ3LOGGER_DEBUG(
//		util::FormatString(L"*** IconRedraw draw[%dms][%dms]",
//			sw_draw1.getElapsedMilliSecUntilStoped(),
//			sw_draw2.getElapsedMilliSecUntilStoped()
//			));

	*pResult = 0;
}

BOOL CMZ3View::OnKeyUp(MSG* pMsg)
{
	// ���ʏ���
	switch (pMsg->wParam) {
#ifndef WINCE
	case VK_F1:
		// �w���v�\��
		util::OpenByShellExecute( MZ4_MANUAL_URL );
		break;
#endif

#ifndef WINCE
	case VK_APPS:
		OnAcceleratorContextMenu();
		return TRUE;
#endif

	case VK_BACK:
#ifndef WINCE
	case VK_ESCAPE:
#endif
		// ���f
		if (theApp.m_access) {
			::SendMessage(m_hWnd, WM_MZ3_ABORT, NULL, NULL);
		}
		break;

	case 'D':
		// Ctrl+Alt+D
		if ((GetAsyncKeyState(VK_CONTROL) & 0x8000) && (GetAsyncKeyState(VK_MENU) & 0x8000)) {
			CMainFrame* pMainFrame = (CMainFrame*)theApp.m_pMainWnd;
			pMainFrame->OnMenuOpenLocalFile();
		}
		break;

	case 'U':
		// Ctrl+Alt+U
		if ((GetAsyncKeyState(VK_CONTROL) & 0x8000) && (GetAsyncKeyState(VK_MENU) & 0x8000)) {
			CMainFrame* pMainFrame = (CMainFrame*)theApp.m_pMainWnd;
			pMainFrame->OnMenuOpenUrl();
		}
		break;

	default:
		break;
	}

	// Xcrawl Canceler
#ifdef WINCE
	if( theApp.m_optionMng.m_bUseXcrawlExtension ) {
		if( m_xcrawl.procKeyup( pMsg->wParam ) ) {
			// �L�����Z�����ꂽ�̂ŏ㉺�L�[�𖳌��ɂ���B
	//		util::MySetInformationText( GetSafeHwnd(), L"Xcrawl canceled..." );
			return TRUE;
		}
	}
#endif

	// �e�y�C�����̏���
	if (pMsg->hwnd == m_categoryList.m_hWnd) {
		if( OnKeyupCategoryList( pMsg->wParam ) ) {
			return TRUE;
		}
	}else if (pMsg->hwnd == m_bodyList.m_hWnd) {
		if( OnKeyupBodyList( pMsg->wParam ) ) {
			return TRUE;
		}
	}else if (pMsg->hwnd == m_statusEdit.m_hWnd) {
		switch (pMsg->wParam) {
		case VK_UP:
			if (m_bImeCompositioning) {
				// �������͒��̓f�t�H���g����
				return FALSE;
			} else {
				// �{�f�B���X�g�Ɉړ�
				CommandSetFocusBodyList();
				return TRUE;
			}
		}
	}

	return FALSE;
}

BOOL CMZ3View::OnKeyDown(MSG* pMsg)
{
	// Xcrawl Canceler
	if( theApp.m_optionMng.m_bUseXcrawlExtension ) {
		if( m_xcrawl.procKeydown(pMsg->wParam) ) {
			return TRUE;
		}
	}

	// �e�y�C�����̏���
	if (pMsg->hwnd == m_groupTab.m_hWnd) {
		if( OnKeydownGroupTab( pMsg->wParam ) ) {
			return TRUE;
		}
	}else if (pMsg->hwnd == m_categoryList.m_hWnd) {
		if( OnKeydownCategoryList( pMsg->wParam ) ) {
			return TRUE;
		}
	}else if (pMsg->hwnd == m_bodyList.m_hWnd) {
		if( OnKeydownBodyList( pMsg->wParam ) ) {
			return TRUE;
		}
	}else if (pMsg->hwnd == m_statusEdit.m_hWnd) {
		switch (pMsg->wParam) {
		case VK_UP:
			return TRUE;

		case 'V':
			if (GetAsyncKeyState(VK_CONTROL) & 0x8000) {
				m_statusEdit.Paste();
				return TRUE;
			}
			break;
		case 'C':
			if (GetAsyncKeyState(VK_CONTROL) & 0x8000) {
				m_statusEdit.Copy();
				return TRUE;
			}
			break;
		case 'X':
			if (GetAsyncKeyState(VK_CONTROL) & 0x8000) {
				m_statusEdit.Cut();
				return TRUE;
			}
			break;
		case 'Z':
			if (GetAsyncKeyState(VK_CONTROL) & 0x8000) {
				m_statusEdit.Undo();
				return TRUE;
			}
			break;
		case VK_RETURN:
			// Enter �����œ��e�E�Ď擾
			OnBnClickedUpdateButton();
			break;
		}
	}

	return FALSE;
}

BOOL CMZ3View::PreTranslateMessage(MSG* pMsg)
{
#ifdef DEBUG
/*	switch (pMsg->message) {
	case WM_MOUSEMOVE:		wprintf( L"PTM: WM_MOUSEMOVE\n" );		break;
	case WM_MOUSEWHEEL:		wprintf( L"PTM: WM_MOUSEWHEEL\n" );		break;
	case WM_RBUTTONDOWN:	wprintf( L"PTM: WM_RBUTTONDOWN\n" );	break;
	case WM_RBUTTONUP:		wprintf( L"PTM: WM_RBUTTONUP\n" );		break;
	}
*/
//	wprintf( L"pretranslatemessage : %d(0x%X)\n", pMsg->message, pMsg->message );
#endif

	if (theApp.m_optionMng.m_bEnableIntervalCheck) {
		// ���b�Z�[�W�ɉ����āA����擾�̃L�����Z���������s��
		switch (pMsg->message) {
		case WM_KEYUP:
		case WM_KEYDOWN:
		case WM_LBUTTONDOWN:
		case WM_LBUTTONUP:
		case WM_RBUTTONDOWN:
		case WM_RBUTTONUP:
		case WM_MOUSEWHEEL:
			ResetIntervalTimer();
			break;
		default:
			break;
		}
	}

	switch (pMsg->message) {
	case WM_KEYUP:
		{
			BOOL r = OnKeyUp( pMsg );

			// KEYDOWN ���s�[�g�񐔂�������
			m_nKeydownRepeatCount = 0;

			if (r) {
				return r;
			}
		}
		break;
	case WM_KEYDOWN:
		{
			// KEYDOWN ���s�[�g�񐔂��C���N�������g
			m_nKeydownRepeatCount ++;

			if (OnKeyDown( pMsg )) {
				return TRUE;
			}
		}
		break;

	case WM_IME_STARTCOMPOSITION:	// IME �ϊ��J�n
		m_bImeCompositioning = true;
		break;

	case WM_IME_ENDCOMPOSITION:		// IME �ϊ��I��
		m_bImeCompositioning = false;
		break;

	case WM_RBUTTONDOWN:
		OnRButtonDown( pMsg->wParam, CPoint(GET_X_LPARAM(pMsg->lParam), GET_Y_LPARAM(pMsg->lParam)) );
		break;

	case WM_RBUTTONUP:
		OnRButtonUp( pMsg->wParam, CPoint(GET_X_LPARAM(pMsg->lParam), GET_Y_LPARAM(pMsg->lParam)) );
		break;

	case WM_LBUTTONDOWN:
		OnLButtonDown( pMsg->wParam, CPoint(GET_X_LPARAM(pMsg->lParam), GET_Y_LPARAM(pMsg->lParam)) );
		break;

	case WM_LBUTTONUP:
		OnLButtonUp( pMsg->wParam, CPoint(GET_X_LPARAM(pMsg->lParam), GET_Y_LPARAM(pMsg->lParam)) );
		break;

	case WM_MOUSEWHEEL:
		OnMouseWheel( LOWORD(pMsg->wParam), HIWORD(pMsg->wParam), CPoint(GET_X_LPARAM(pMsg->lParam), GET_Y_LPARAM(pMsg->lParam)) );
		break;

	case WM_MOUSEMOVE:
		OnMouseMove( pMsg->wParam, CPoint(GET_X_LPARAM(pMsg->lParam), GET_Y_LPARAM(pMsg->lParam)) );
		break;
	}

	return CFormView::PreTranslateMessage(pMsg);
}

/**
 * ���r���[����̕��A����
 */
LRESULT CMZ3View::OnChangeView(WPARAM wParam, LPARAM lParam)
{
	m_hotList->SetFocus();
	util::MySetListCtrlItemFocusedAndSelected( m_bodyList, m_selGroup->getSelectedCategory()->selectedBody, true );
	theApp.ChangeView(theApp.m_pMainView);

	// �A�C�e���̐���
	theApp.EnableCommandBarButton( ID_BACK_BUTTON, FALSE);
	theApp.EnableCommandBarButton( ID_IMAGE_BUTTON, FALSE);

	// �������݃{�^��
	// �����̓��L�̏ꍇ�͗L���ɁB
	theApp.EnableCommandBarButton( ID_WRITE_BUTTON,
		(m_selGroup->getSelectedCategory()->m_mixi.GetAccessType() == ACCESS_LIST_MYDIARY));

	theApp.EnableCommandBarButton( ID_OPEN_BROWSER, FALSE );

	// mini�摜�E�B���h�E�̕��A
	MoveMiniImageDlg();

	return TRUE;
}

/**
 * ���L���e���j���[
 */
void CMZ3View::OnWriteDiary()
{
	if (wcslen(theApp.m_loginMng.GetMixiOwnerID()) == 0) {
		// �I�[�i�[�h�c���擾����
		// ���L�ꗗ����擾

		// �S�O���[�v�̑S�J�e�S���𑖍����A
		// �u���L�v�J�e�S�����������A
		// ���L�� CMixiData �� URL ����I�[�i�[ID���擾����B
		bool bFound = false;
		for( size_t i=0; i<m_selGroup->categories.size(); i++ ) {
			CCategoryItem& category = m_selGroup->categories[i];
			if( category.m_mixi.GetAccessType() == ACCESS_LIST_MYDIARY ) {
				// ���L�����B
				// ��1�v�f��T������B
				if( category.GetBodyList().size() > 0 ) {
					CMixiData& mixi = category.GetBodyList()[0];
					CString& url = mixi.GetURL();
					theApp.m_loginMng.SetMixiOwnerID(
						mixi::MixiUrlParser::GetOwnerID( url ) );

					theApp.m_loginMng.Write();

					bFound = true;
					break;
				}
			}
		}
	}

	theApp.m_pWriteView->StartWriteView( WRITEVIEW_TYPE_NEWDIARY, NULL );
}

/**
 * �O���[�v�^�u�̑I��ύX�C�x���g
 */
void CMZ3View::OnTcnSelchangeGroupTab(NMHDR *pNMHDR, LRESULT *pResult)
{
	OnSelchangedGroupTab();

	*pResult = 0;
}

/**
 * �R�}���h�F�O���[�v�^�u�̍����ڂւ̑I��ύX
 */
BOOL CMZ3View::CommandSelectGroupTabBeforeItem()
{
	if (theApp.m_access) return TRUE;	// �A�N�Z�X���͖���

	// ���X�N���[���A�j���[�V�������N������
	m_categoryList.StartPanScroll( CTouchListCtrl::PAN_SCROLL_DIRECTION_RIGHT );

	// �����ڂ֑I��ύX�B
	if( m_groupTab.GetCurSel() == 0 ) {
		// ���[�Ȃ̂ŉE�[�ցB
		m_groupTab.SetCurSel( m_groupTab.GetItemCount()-1 );
	}else{
		// �����ڂ֑I��ύX�B
		m_groupTab.SetCurSel( m_groupTab.GetCurSel()-1 );
	}

	// �x�����[�h�X���b�h�J�n
	AfxBeginThread( ReloadGroupTab_Thread, this );

	return TRUE;
}

/**
 * �R�}���h�F�O���[�v�^�u�̉E���ڂւ̑I��ύX
 */
BOOL CMZ3View::CommandSelectGroupTabNextItem()
{
	if (theApp.m_access) return TRUE;	// �A�N�Z�X���͖���

	// ���X�N���[���A�j���[�V�������N������
	m_categoryList.StartPanScroll( CTouchListCtrl::PAN_SCROLL_DIRECTION_LEFT );

	// �E���ڂ֑I��ύX�B
	if( m_groupTab.GetCurSel() == m_groupTab.GetItemCount()-1 ) {
		// �E�[�Ȃ̂ō��[�ցB
		m_groupTab.SetCurSel( 0 );
	}else{
		// �E���ڂ֑I��ύX�B
		m_groupTab.SetCurSel( m_groupTab.GetCurSel()+1 );
	}

	// �x�����[�h�X���b�h�J�n
	AfxBeginThread( ReloadGroupTab_Thread, this );

	return TRUE;
}

/**
 * �R�}���h�F�J�e�S�����X�g�ւ̃t�H�[�J�X�ړ�
 */
BOOL CMZ3View::CommandSetFocusCategoryList()
{
	m_categoryList.SetFocus();
	m_hotList = &m_categoryList;
//	m_infoEdit.SetWindowText( _T("") );

	m_categoryList.EnsureVisible( m_selGroup->focusedCategory, FALSE);

	// �{�f�B���X�g�ő�\���ł���΃J�e�S�����X�g�\���ɕύX
	if (m_magnifyMode != MAGNIFY_MODE_DEFAULT) {
		MySetMagnifyModeTo(MAGNIFY_MODE_CATEGORY);
	}

	return TRUE;
}

/**
 * �R�}���h�F�O���[�v�^�u�ւ̃t�H�[�J�X�ړ�
 */
BOOL CMZ3View::CommandSetFocusGroupTab()
{
	m_groupTab.SetFocus();
	m_infoEdit.SetWindowText( L"" );

	return TRUE;
}

/**
 * �R�}���h�F�{�f�B���X�g�ւ̃t�H�[�J�X�ړ�
 */
BOOL CMZ3View::CommandSetFocusBodyList()
{
	// �J�e�S�����X�g�\���ł���΃{�f�B���X�g�\���ɕύX
	if (m_magnifyMode == MAGNIFY_MODE_CATEGORY) {
		m_magnifyMode = MAGNIFY_MODE_BODY;

		// ���C�A�E�g�ύX���f
		MySetLayout(0, 0);
		InvalidateRect( m_rectIcon, FALSE );

		// �J�e�S���̑I�����ڂ݂̂�\��
		m_categoryList.EnsureVisible(m_selGroup->selectedCategory, FALSE);
	}

	if (m_bodyList.GetItemCount() != 0) {
		m_bodyList.SetFocus();
		m_hotList = &m_bodyList;

		// ��1�J�����ɕ\�����Ă�����e��\������
		MySetInfoEditFromBodySelectedData();

		// �I����Ԃ��X�V
		int idx = m_selGroup->getSelectedCategory()->selectedBody;
		util::MySetListCtrlItemFocusedAndSelected( m_bodyList, idx, true );
		m_bodyList.EnsureVisible( idx, FALSE);
	}

	return TRUE;
}

/**
 * �O���[�v�^�u�̃L�[�_�E���C�x���g
 */
BOOL CMZ3View::OnKeydownGroupTab( WORD vKey )
{
	switch( vKey ) {
	case VK_UP:
		// ��L�[�B
		if (theApp.m_access) return TRUE;	// �A�N�Z�X���͖���
		// ����
		return TRUE;
	case VK_DOWN:
		// ���L�[�B
		// �J�e�S���[���X�g�ւ̃t�H�[�J�X�ړ�
		return CommandSetFocusCategoryList();
	case VK_LEFT:
		// ���L�[�B
		if (theApp.m_access) return TRUE;	// �A�N�Z�X���͖���
		// �I��ύX
		return CommandSelectGroupTabBeforeItem();
	case VK_RIGHT:
		// �E�L�[�B
		if (theApp.m_access) return TRUE;	// �A�N�Z�X���͖���
		// �I��ύX
		return CommandSelectGroupTabNextItem();
	case VK_RETURN:
		// Enter�L�[�B
		// �J�e�S���[���X�g�ւ̃t�H�[�J�X�ړ�
		return CommandSetFocusCategoryList();
	}
	return FALSE;
}

/**
 * �J�e�S�����X�g�̃L�[�_�E���C�x���g
 */
BOOL CMZ3View::OnKeydownCategoryList( WORD vKey )
{
	// VK_UP, VK_DOWN
	if( theApp.m_optionMng.m_bUseXcrawlExtension ) {
		switch( vKey ) {
		case VK_UP:
			// VK_KEYDOWN �ł͖����B
			// VK_KEYUP �ŏ�������B
			// ����́A�A�h�G�X�� Xcrawl �Ή��̂��߁B

			// �������A�Q��ڈȍ~�̃L�[�����ł���΁A�������Ƃ݂Ȃ��A�ړ�����
			if( !m_xcrawl.isXcrawlEnabled() && m_nKeydownRepeatCount >= 2 ) {
				return CommandMoveUpCategoryList();
			}

			return TRUE;

		case VK_DOWN:
			// VK_KEYDOWN �ł͖����B
			// VK_KEYUP �ŏ�������B
			// ����́A�A�h�G�X�� Xcrawl �Ή��̂��߁B

			// �������A�Q��ڈȍ~�̃L�[�����ł���΁A�������Ƃ݂Ȃ��A�ړ�����
			if( !m_xcrawl.isXcrawlEnabled() && m_nKeydownRepeatCount >= 2 ) {
				return CommandMoveDownCategoryList();
			}

			return TRUE;
		}
	} else {
		switch( vKey ) {
		case VK_UP:
			if( m_categoryList.GetItemState(0, LVIS_FOCUSED) != FALSE ) {
				// ��ԏ�̍��ڂȂ疳��
				return TRUE;
			} else {
#ifdef WINCE
				// �f�t�H���g����
				return FALSE;
#else
				return CommandMoveUpCategoryList();
#endif
			}
			break;

		case VK_DOWN:
			if( m_categoryList.GetItemState(m_categoryList.GetItemCount()-1, LVIS_FOCUSED) != FALSE ) {
				// ��ԉ��̍��ڑI�𒆂Ȃ�A�{�f�B���X�g�̐擪�ցB
				if (m_bodyList.GetItemCount() != 0) {
					// �I����Ԃ�擪�ɁB�ȑO�̑I����Ԃ�Off�ɁB
					util::MySetListCtrlItemFocusedAndSelected( m_bodyList, m_selGroup->getSelectedCategory()->selectedBody, false );
					m_selGroup->getSelectedCategory()->selectedBody = 0;
				}
				return CommandSetFocusBodyList();
			} else {
#ifdef WINCE
				// �f�t�H���g����
				return FALSE;
#else
				return CommandMoveDownCategoryList();
#endif
			}
			break;
		}
	}

	switch( vKey ) {
	case VK_LEFT:
		if (theApp.m_access) return TRUE;	// �A�N�Z�X���͖���

		// �O���[�v�^�u�̑I��ύX
		return CommandSelectGroupTabBeforeItem();

	case VK_RIGHT:
		if (theApp.m_access) return TRUE;	// �A�N�Z�X���͖���

		// �O���[�v�^�u�̑I��ύX
		return CommandSelectGroupTabNextItem();

	case VK_RETURN:

		// �J�e�S�����X�g�\���ł���΃{�f�B���X�g�\���ɕύX
		if (m_magnifyMode != MAGNIFY_MODE_DEFAULT) {
			MySetMagnifyModeTo(MAGNIFY_MODE_BODY);
		}

		if (theApp.m_optionMng.m_bOneClickCategoryFetchMode) {
			// ��y�C���̃��X�g�N���b�N�Ŏ擾����
			if (m_selGroup->selectedCategory != m_selGroup->focusedCategory) {
				// ��I�����ڂȂ̂ŁA�擾�����ƃ{�f�B�̕ύX�B
				// ��擾�ŁA���O������Ȃ烍�O����擾�B

				// �A�N�Z�X���͑I��s��
				if (theApp.m_access) {
					return TRUE;
				}
				m_selGroup->selectedCategory = m_selGroup->focusedCategory;

				OnMySelchangedCategoryList();
			}

			// �Ď擾
			RetrieveCategoryItem();

		} else {
			// �I�����ڂ̃N���b�N�ōĎ擾�A��I�����ڂȂ獀�ڕύX�܂ŁB
			if (m_selGroup->selectedCategory == m_selGroup->focusedCategory) {
				RetrieveCategoryItem();
			} else {
				// ��I�����ڂȂ̂ŁA�擾�����ƃ{�f�B�̕ύX�B
				// ��擾�ŁA���O������Ȃ烍�O����擾�B

				// �A�N�Z�X���͑I��s��
				if (theApp.m_access) {
					return TRUE;
				}
				m_selGroup->selectedCategory = m_selGroup->focusedCategory;

				OnMySelchangedCategoryList();
			}
		}
		return TRUE;

	case VK_BACK:
#ifndef WINCE
	case VK_ESCAPE:
#endif
		if (theApp.m_access) {
			// �A�N�Z�X���͖���
			return TRUE;
		}
		// ��A�N�Z�X���́A�{�f�B���X�g�Ɉړ�����
		return CommandSetFocusBodyList();
	}
	return FALSE;
}

/**
 * �J�e�S�����X�g�̃L�[UP�C�x���g
 */
BOOL CMZ3View::OnKeyupCategoryList( WORD vKey )
{
	if( theApp.m_optionMng.m_bUseXcrawlExtension ) {
		switch( vKey ) {
		case VK_UP:
			// �L�[�������ɂ��A���ړ����Ȃ�A�L�[UP�ňړ����Ȃ��B
			if( !m_xcrawl.isXcrawlEnabled() && m_nKeydownRepeatCount >= 2 ) {
				return TRUE;
			}
			return CommandMoveUpCategoryList();

		case VK_DOWN:
			// �L�[�������ɂ��A���ړ����Ȃ�A�L�[UP�ňړ����Ȃ��B
			if( !m_xcrawl.isXcrawlEnabled() && m_nKeydownRepeatCount >= 2 ) {
				return TRUE;
			}
			return CommandMoveDownCategoryList();
		}
	}
	return FALSE;
}

/**
 * �{�f�B���X�g�̃L�[�_�E���C�x���g
 */
BOOL CMZ3View::OnKeydownBodyList( WORD vKey )
{
	// �{�f�B�[���X�g�ł̃L�[����

	// VK_UP, VK_DOWN
	if( theApp.m_optionMng.m_bUseXcrawlExtension ) {
		// Xcrawl �g�����[�h
		switch( vKey ) {
		case VK_UP:
			// VK_KEYDOWN �ł͖����B
			// VK_KEYUP �ŏ�������B
			// ����́A�A�h�G�X�� Xcrawl �Ή��̂��߁B

			// �������A�Q��ڈȍ~�̃L�[�����ł���΁A�������Ƃ݂Ȃ��A�ړ�����
			if( !m_xcrawl.isXcrawlEnabled() && m_nKeydownRepeatCount >= 2 ) {
				return CommandMoveUpBodyList();
			}
			return TRUE;

		case VK_DOWN:
			// VK_KEYDOWN �ł͖����B
			// VK_KEYUP �ŏ�������B
			// ����́A�A�h�G�X�� Xcrawl �Ή��̂��߁B

			// �������A�Q��ڈȍ~�̃L�[�����ł���΁A�������Ƃ݂Ȃ��A�ړ�����
			if( !m_xcrawl.isXcrawlEnabled() && m_nKeydownRepeatCount >= 2 ) {
				return CommandMoveDownBodyList();
			}
			return TRUE;
		}
	} else {
		// Xcrawl �g�����[�h�ȊO
		switch( vKey ) {
		case VK_UP:
			if (m_bodyList.GetItemState(0, LVIS_FOCUSED) != FALSE) {
				// ��ԏ�B
				// �J�e�S���Ɉړ�

//				if (theApp.m_access) return TRUE;	// �A�N�Z�X���͖���

				// �I����Ԃ𖖔��ɁB�ȑO�̑I����Ԃ�Off�ɁB
				util::MySetListCtrlItemFocusedAndSelected( m_categoryList, m_selGroup->focusedCategory, false );
				m_selGroup->focusedCategory = m_categoryList.GetItemCount()-1;
				util::MySetListCtrlItemFocusedAndSelected( m_categoryList, m_selGroup->focusedCategory, true );
				
				return CommandSetFocusCategoryList();
			}else{
				if (m_bImeCompositioning) {
					// �������͒��̓f�t�H���g����
					return FALSE;
				} else {
					return CommandMoveUpBodyList();
				}
			}
			break;

		case VK_DOWN:
			if (m_bodyList.GetItemState(m_bodyList.GetItemCount()-1, LVIS_FOCUSED) != FALSE) {
				// ��ԉ��Ȃ̂Ŗ����B
//				if (theApp.m_access) return TRUE;	// �A�N�Z�X���͖���

				// Twitter ���[�h�Ȃ�Ԃ₭���[�h�ցB
				if (m_viewStyle==VIEW_STYLE_TWITTER) {
					// ���[�h����
					CCategoryItem* pCategory = m_selGroup->getSelectedCategory();
					if (pCategory!=NULL) {
						std::string strServiceType = theApp.m_accessTypeInfo.getServiceType(pCategory->m_mixi.GetAccessType());
						if (strServiceType == "Twitter") {

							// Lua �֐��Ăяo���Ŏ���
							theApp.MyLuaExecute(L"twitter.on_twitter_update()");

							return TRUE;
						} else if (strServiceType == "Wassr") {

							// Lua �֐��Ăяo���Ŏ���
							theApp.MyLuaExecute(L"wassr.on_wassr_update()");

							return TRUE;
						} else if (strServiceType == "gooHome") {
							
							// Lua �֐��Ăяo���Ŏ���
							theApp.MyLuaExecute(L"goohome.on_goohome_update()");

							return TRUE;
						} else if (pCategory->m_mixi.GetAccessType()==ACCESS_MIXI_RECENT_ECHO) {

							// Lua �֐��Ăяo���Ŏ���
							theApp.MyLuaExecute(L"mixi.on_mixi_echo_update()");

							return TRUE;
						}
					}
				}
				// �f�t�H���g����
				return FALSE;

			}else{
				if (m_bImeCompositioning) {
					// �������͒��̓f�t�H���g����
					return FALSE;
				} else {
					return CommandMoveDownBodyList();
				}
			}
			break;
		}
	}

	switch( vKey ) {
	case VK_RETURN:

		if( m_dwLastReturn != 0 ) {
			if( GetTickCount() < m_dwLastReturn + theApp.m_optionMng.m_longReturnRangeMSec ) {
				// ���������莞�ԓ��ɗ����ꂽ�̂ŁA�ʏ퉟���Ƃ݂Ȃ��B

				// �L�[�����������N���A���Ă����B
				m_dwLastReturn = 0;
			}else{
				// ���������莞�Ԉȏ�o���Ă���̂ŁA���O���J��

				util::MySetInformationText( m_hWnd, L"���O���J���Ă��܂�..." );

				// ���O���J��
				OnViewLog();

				// �L�[�����������N���A���Ă����B
				m_dwLastReturn = 0;

				// �����ł͏I���B
				return TRUE;
			}
		}

		// �A�N�Z�X���͍ăA�N�Z�X�s��
		if (theApp.m_access) return TRUE;	// �A�N�Z�X���͖���

		// MZ3 API : �t�b�N�֐��Ăяo��
		if (util::CallMZ3ScriptHookFunctions(
				theApp.m_accessTypeInfo.getSerializeKey(GetSelectedBodyItem().GetAccessType()),
				"enter_body_list", &GetSelectedBodyItem()))
		{
			// �t�b�N�֐��o�^�ς݁��R�[�������̂��ߏI��
			return TRUE;
		}

		switch( GetSelectedBodyItem().GetAccessType() ) {
		case ACCESS_COMMUNITY:
			// ���j���[�\��
			PopupBodyMenu();
			break;

		case ACCESS_RSS_READER_ITEM:
			// �ڍו\��
			OnMenuRssRead();
			break;

		default:
			// ����ȗv�f�ȊO�Ȃ̂ŁA�ʐM�����J�n�B
			AccessProc( &GetSelectedBodyItem(), util::CreateMixiUrl(GetSelectedBodyItem().GetURL()));
			break;
		}
		return TRUE;

	case VK_PROCESSKEY:	// 0xE5
	case VK_F23:		// 0x86
		// W-ZERO3 �� RETURN �L�[�������ɔ��ł���L�[�B
		if( m_dwLastReturn == 0 ) {
			// m_dwLastReturn ���X�V���Ă����B
			m_dwLastReturn = GetTickCount();

			// ����������p�X���b�h���J�n
			AfxBeginThread( LongReturnKey_Thread, this );
		}
		break;

	case VK_LEFT:
		// ���{�^���B
		// �V���[�g�J�b�g�ړ��B
		{
			CCategoryItem* pCategory = m_selGroup->getSelectedCategory();
			if (pCategory != NULL) {
				int idxSel = pCategory->selectedBody;
				int nItem = m_bodyList.GetItemCount();

				util::MySetListCtrlItemFocusedAndSelected( m_bodyList, idxSel, false );
				if( idxSel == 0 ) {
					// ��ԏ� �� ��ԉ�
					util::MySetListCtrlItemFocusedAndSelected( m_bodyList, nItem-1, true );
					m_bodyList.EnsureVisible( nItem-1, FALSE);
					return TRUE;
				}else if( idxSel == nItem-1 ) {
					// ��ԉ�     �� ��ԏ�
					util::MySetListCtrlItemFocusedAndSelected( m_bodyList, 0, true );
					m_bodyList.EnsureVisible( 0, FALSE );
					return TRUE;
				}else if( idxSel < nItem/2 ) {
					// �������� �� ��ԏ�
					util::MySetListCtrlItemFocusedAndSelected( m_bodyList, 0, true );
					m_bodyList.EnsureVisible( 0, FALSE );
					return TRUE;
				}else{
					// ������艺 �� ��ԉ�
					util::MySetListCtrlItemFocusedAndSelected( m_bodyList, nItem-1, true );
					m_bodyList.EnsureVisible( nItem-1, FALSE);
					return TRUE;
				}
			}
		}
		// �J�e�S���Ɉړ�
//		return CommandSetFocusCategoryList();
	case VK_RIGHT:
		// �E�{�^���ŁA�Q�ڂ̍��ڂ�ω�������
		MyChangeBodyHeader();
		return TRUE;

	case VK_BACK:
#ifndef WINCE
	case VK_ESCAPE:
#endif
		if (theApp.m_access) {
			// �A�N�Z�X���͖���
			return TRUE;
		}
		// ��A�N�Z�X���́A�J�e�S�����X�g�Ɉړ�����
		return CommandSetFocusCategoryList();

		// ��A�N�Z�X���́A�J�e�S�����X�g�܂��͓��͗̈�Ɉړ�����
/*		switch (m_viewStyle) {
		case VIEW_STYLE_TWITTER:
			GetDlgItem( IDC_STATUS_EDIT )->SetFocus();
			return TRUE;
		case VIEW_STYLE_DEFAULT:
		default:
			return CommandSetFocusCategoryList();
		}
*/
		return CommandSetFocusCategoryList();

	default:
//		if( MZ3LOGGER_IS_DEBUG_ENABLED() ) {
//			CString msg;
//			msg.Format( L"WM_KEYDOWN, 0x%X", vKey );
//			MZ3LOGGER_DEBUG( msg );
//		}
		break;
	}
	return FALSE;
}

/**
 * �{�f�B���X�g�̃L�[UP�C�x���g
 */
BOOL CMZ3View::OnKeyupBodyList( WORD vKey )
{
	if( theApp.m_optionMng.m_bUseXcrawlExtension ) {
		switch( vKey ) {
		case VK_UP:
			// �L�[�������ɂ��A���ړ����Ȃ�A�L�[UP�ňړ����Ȃ��B
			if( !m_xcrawl.isXcrawlEnabled() && m_nKeydownRepeatCount >= 2 ) {
				return TRUE;
			}

			return CommandMoveUpBodyList();

		case VK_DOWN:
			// �L�[�������ɂ��A���ړ����Ȃ�A�L�[UP�ňړ����Ȃ��B
			if( !m_xcrawl.isXcrawlEnabled() && m_nKeydownRepeatCount >= 2 ) {
				return TRUE;
			}

			return CommandMoveDownBodyList();
		}
	}

	return FALSE;
}

BOOL CMZ3View::CommandMoveUpCategoryList()
{
//	if( theApp.m_access ) return TRUE;	// �A�N�Z�X���͖���

	if( m_categoryList.GetItemState(0, LVIS_FOCUSED) != FALSE ) {
		// ��ԏ�̍��ڂȂ疳��
		return TRUE;
	} else {
		// ��ԏ�ł͂Ȃ��̂ŁA��Ɉړ�
		util::MySetListCtrlItemFocusedAndSelected( m_categoryList, m_selGroup->focusedCategory, false );
		m_selGroup->focusedCategory --;
		util::MySetListCtrlItemFocusedAndSelected( m_categoryList, m_selGroup->focusedCategory, true );

		// �ړ��悪��\���Ȃ������ɃX�N���[��
		if( !util::IsVisibleOnListBox( m_categoryList, m_selGroup->focusedCategory ) ) {
			if( ( m_selGroup->focusedCategory < m_categoryList.GetTopIndex() ) &&
				( m_selGroup->focusedCategory >= m_categoryList.GetTopIndex() - m_categoryList.GetCountPerPage() ) ) {
				// �ړ��悪��ʂ���A1��ʈȓ��ɂ��鎞��1��ʃX�N���[��
				m_categoryList.Scroll( CSize(0, -m_categoryList.GetCountPerPage() * theApp.m_optionMng.GetFontHeight()) );
			} else {
				// �ړ��悪��ʂ�艺���A���1��ʈȏ㗣��Ă��鎞��EnsureVisible()
				m_categoryList.EnsureVisible( m_selGroup->focusedCategory , TRUE );
			}

			// �ĕ`��
			if (theApp.m_optionMng.IsUseBgImage()) {
				m_categoryList.RedrawItems(0, m_categoryList.GetItemCount());
				m_categoryList.UpdateWindow();
			}
		}
		return TRUE;
	}
}

BOOL CMZ3View::CommandMoveDownCategoryList()
{
//	if( theApp.m_access ) return TRUE;	// �A�N�Z�X���͖���

	if( m_categoryList.GetItemState(m_categoryList.GetItemCount()-1, LVIS_FOCUSED) != FALSE ) {
		// ��ԉ��̍��ڑI�𒆂Ȃ�A�{�f�B���X�g�̐擪�ցB
		if (m_bodyList.GetItemCount() != 0) {
			// �I����Ԃ�擪�ɁB�ȑO�̑I����Ԃ�Off�ɁB
			util::MySetListCtrlItemFocusedAndSelected( m_bodyList, m_selGroup->getSelectedCategory()->selectedBody, false );
			m_selGroup->getSelectedCategory()->selectedBody = 0;
		}
		return CommandSetFocusBodyList();
	} else {
		// ��ԉ��ł͂Ȃ��̂ŁA���Ɉړ�
		util::MySetListCtrlItemFocusedAndSelected( m_categoryList, m_selGroup->focusedCategory, false );
		m_selGroup->focusedCategory ++;
		util::MySetListCtrlItemFocusedAndSelected( m_categoryList, m_selGroup->focusedCategory, true );

		// �ړ��悪��\���Ȃ牺�����ɃX�N���[��
		if( !util::IsVisibleOnListBox( m_categoryList, m_selGroup->focusedCategory ) ) {
			if( ( m_selGroup->focusedCategory >= m_categoryList.GetTopIndex() + m_categoryList.GetCountPerPage() ) &&
				( m_selGroup->focusedCategory >= m_categoryList.GetTopIndex() + m_categoryList.GetCountPerPage() * 2) ) {
				// �ړ��悪��ʂ�艺�A1��ʈȓ��ɂ��鎞��1��ʃX�N���[��
				m_categoryList.Scroll( CSize(0, m_categoryList.GetCountPerPage() * theApp.m_optionMng.GetFontHeight()) );
			} else {
				// �ړ��悪��ʂ��ォ�A����1��ʈȏ㗣��Ă��鎞��EnsureVisible()
				m_categoryList.EnsureVisible( m_selGroup->focusedCategory , TRUE );
			}

			// �ĕ`��
			if (theApp.m_optionMng.IsUseBgImage()) {
				m_categoryList.RedrawItems(0, m_categoryList.GetItemCount());
				m_categoryList.UpdateWindow();
			}
		}
		return TRUE;
	}
}

BOOL CMZ3View::CommandMoveUpBodyList()
{
	if (m_bodyList.GetItemState(0, LVIS_FOCUSED) != FALSE) {
		// ��ԏ�B
		// �J�e�S���Ɉړ�

//		if( theApp.m_access ) return TRUE;	// �A�N�Z�X���͋֎~

		// �I����Ԃ𖖔��ɁB�ȑO�̑I����Ԃ�Off�ɁB
		util::MySetListCtrlItemFocusedAndSelected( m_categoryList, m_selGroup->focusedCategory, false );
		m_selGroup->focusedCategory = m_categoryList.GetItemCount()-1;
		util::MySetListCtrlItemFocusedAndSelected( m_categoryList, m_selGroup->focusedCategory, true );
		
		return CommandSetFocusCategoryList();
	}else{
		// ��ԏ�ł͂Ȃ��B
		// ��Ɉړ�
		CCategoryItem* pCategory = m_selGroup->getSelectedCategory();
		if (pCategory!=NULL) {
			util::MySetListCtrlItemFocusedAndSelected( m_bodyList, pCategory->selectedBody, false );
			pCategory->selectedBody --;
			// �I�����ڂ�������Ή��[�ֈړ�
			pCategory->selectedBody = ( pCategory->selectedBody < 0 ? m_bodyList.GetItemCount() - 1 : pCategory->selectedBody );
			util::MySetListCtrlItemFocusedAndSelected( m_bodyList, pCategory->selectedBody, true );

			// �ړ��悪��\���Ȃ������ɃX�N���[��
			if( !util::IsVisibleOnListBox( m_bodyList, pCategory->selectedBody ) ) {
				int topIdx = m_bodyList.GetTopIndex();
				int countPerPage = m_bodyList.GetCountPerPage();
				int selectedBody = pCategory->selectedBody;
				if (topIdx - countPerPage <= selectedBody && selectedBody < topIdx) {
					// �ړ��悪��ʂ���ŁA�㑤��1��ʓ��ɂ��鎞��1��ʃX�N���[��
					//            +-[TopIndex - CountPerPage]
					//            +
					// [selected]-+
					//            --[TopIndex]
					MEASUREITEMSTRUCT measureItemStruct;
					m_bodyList.MeasureItem(&measureItemStruct);
					int y = -countPerPage * measureItemStruct.itemHeight;
					m_bodyList.Scroll( CSize(0, y) );
				} else {
					// �ړ��悪��ʂ�艺���A���1��ʈȏ㗣��Ă��鎞��EnsureVisible()
					m_bodyList.EnsureVisible( selectedBody, TRUE );
				}

				// �ĕ`��
				if (theApp.m_optionMng.IsUseBgImage()) {
					// �ĕ`��
					int topIdx = m_bodyList.GetTopIndex();
					m_bodyList.RedrawItems(topIdx, topIdx + m_bodyList.GetCountPerPage());
					m_bodyList.UpdateWindow();
					MoveMiniImageDlg();
				}
			}
		}
		return TRUE;
	}
}

BOOL CMZ3View::CommandMoveDownBodyList()
{
	if (m_bodyList.GetItemState(m_bodyList.GetItemCount()-1, LVIS_FOCUSED) != FALSE) {
		// ��ԉ��Ȃ̂Ŗ����B
		if( theApp.m_access ) return TRUE;	// �A�N�Z�X���͋֎~

		return TRUE;
	}else{
		// ��ԉ��ł͂Ȃ��B
		// ���Ɉړ�
		CCategoryItem* pCategory = m_selGroup->getSelectedCategory();
		if (pCategory!=NULL) {
			util::MySetListCtrlItemFocusedAndSelected( m_bodyList, pCategory->selectedBody, false );
			pCategory->selectedBody ++;
			util::MySetListCtrlItemFocusedAndSelected( m_bodyList, pCategory->selectedBody, true );

			// �ړ��悪��\���Ȃ牺�����ɃX�N���[��
			if( !util::IsVisibleOnListBox( m_bodyList, pCategory->selectedBody ) ) {
				int topIdx = m_bodyList.GetTopIndex();
				int countPerPage = m_bodyList.GetCountPerPage();
				int selectedBody = pCategory->selectedBody;
				if (topIdx + countPerPage <= selectedBody && selectedBody < topIdx + countPerPage * 2) {
					// �ړ��悪��ʂ�艺�ŁA������1��ʓ��ɂ��鎞��1��ʃX�N���[��
					//            +-[TopIndex + CountPerPage]
					//            +
					// [selected]-+
					//            --[TopIndex + CountPerPage*2]
					MEASUREITEMSTRUCT measureItemStruct;
					m_bodyList.MeasureItem(&measureItemStruct);
					int y = countPerPage * measureItemStruct.itemHeight;
					m_bodyList.Scroll( CSize(0, y) );
				} else {
					// �ړ��悪��ʂ��ォ�A����1��ʈȏ㗣��Ă��鎞��EnsureVisible()
					m_bodyList.EnsureVisible( selectedBody, TRUE );
				}

				// �ĕ`��
				if (theApp.m_optionMng.IsUseBgImage()) {
					// �ĕ`��
					int topIdx = m_bodyList.GetTopIndex();
					m_bodyList.RedrawItems(topIdx, topIdx + m_bodyList.GetCountPerPage());
					m_bodyList.UpdateWindow();
					MoveMiniImageDlg();
				}
			}
		}
		return TRUE;
	}
}


/**
 * ����������p�X���b�h
 */
unsigned int CMZ3View::LongReturnKey_Thread( LPVOID This )
{
	CMZ3View* pView = (CMZ3View*)This;

	// ���������莞�ԕ��ASleep ����B
	Sleep( theApp.m_optionMng.m_longReturnRangeMSec );

	if( pView->m_dwLastReturn != 0 ) {
		// �܂� VK_RETURN �����Ă��Ȃ��̂ŁA���O���J��
		// �̂̓{�^�������[�X���ŁA�����ł̓��b�Z�[�W�̂ݕύX���Ă����B
		util::MySetInformationText( pView->GetSafeHwnd(), L"���O���J���܂�..." );

		// W-ZERO3 �̏ꍇ�́AVK_PROCESSKEY ��������̌o�ߎ��ԂŒ���������
	}else{
		// ���� VK_RETURN �����Ă���̂ŁA�������Ȃ��B
	}

	return 0;
}

/**
 * �������݃{�^���̐���
 */
void CMZ3View::OnUpdateWriteButton(CCmdUI* pCmdUI)
{
/*	if( theApp.m_bPocketPC ) {
#ifdef WINCE
		CMainFrame* pFrame;
		pFrame = (CMainFrame*)(theApp.GetMainWnd());
		pCmdUI->Enable(pFrame->m_wndCommandBar.GetToolBarCtrl().IsButtonEnabled(ID_WRITE_BUTTON));  
#endif
	}
*/
}

/**
 * �������݃{�^��
 */
void CMZ3View::OnWriteButton()
{
	OnWriteDiary();
}

/**
 * �{�f�B���X�g�̃t�H�[�J�X�擾�C�x���g
 *
 * �{�^������ȂǁB
 */
void CMZ3View::OnNMSetfocusBodyList(NMHDR *pNMHDR, LRESULT *pResult)
{
	// �u���E�U�{�^��
	// �����Ƃ̏ꍇ�͗L���ɁB
	if (m_selGroup!=NULL && m_selGroup->getSelectedCategory()!= NULL) {
		switch (m_selGroup->getSelectedCategory()->m_mixi.GetAccessType()) {
			case ACCESS_LIST_FOOTSTEP:
			case ACCESS_LIST_FRIEND:
				theApp.EnableCommandBarButton( ID_OPEN_BROWSER, TRUE );
				break;

			default:
				theApp.EnableCommandBarButton( ID_OPEN_BROWSER, FALSE );
				break;
		}
	} else {
		theApp.EnableCommandBarButton( ID_OPEN_BROWSER, FALSE );
	}

	*pResult = 0;
}

/**
 * �J�e�S�����X�g�̃t�H�[�J�X�擾�C�x���g
 */
void CMZ3View::OnNMSetfocusHeaderList(NMHDR *pNMHDR, LRESULT *pResult)
{
	// �u���E�U�{�^��
	// ��ɖ���
	theApp.EnableCommandBarButton( ID_OPEN_BROWSER, FALSE );

	*pResult = 0;
}

/**
 * �u���E�U�ŊJ��
 */
void CMZ3View::OnOpenBrowser()
{
	LPCTSTR url = GetSelectedBodyItem().GetBrowseUri();
	if( wcslen(url) == 0 ) {
		// url ���󕶎��Ȃ̂ŏI���B
		return;
	}

	switch( GetSelectedBodyItem().GetAccessType() ) {
	case ACCESS_PROFILE:			// �v���t�B�[��
	case ACCESS_BIRTHDAY:			// �a�����v���t�B�[��
		{
			CString name = GetSelectedBodyItem().GetName();
			switch( m_selGroup->getSelectedCategory()->m_mixi.GetAccessType() ) {
			case ACCESS_LIST_FRIEND:		// �}�C�~�N�ꗗ
				// ���O���uXXX ����(N)�v�ƂȂ��Ă���̂ŁA
				// �uXXX�v�ɐ��`�B
				int idx = name.ReverseFind( '(' );	// �܂� '(' �ȍ~���폜
				if( idx > 0 ) {
					name = name.Left( idx );
					// �����́u����v���폜
					if( name.Right(2) == L"����" && name.GetLength() > 2 ) {
						name = name.Left( name.GetLength()-2 );
					}
				}
			}
			util::OpenUrlByBrowserWithConfirmForUser( url, name );
		}
		break;
	default:
		util::OpenUrlByBrowserWithConfirm( url );
		break;
	}
}

/**
 * �f�o�b�O���\��
 *
 * ���݂̑I�����ڂɊւ������\������
 */
void CMZ3View::OnShowDebugInfo()
{
	CWnd* wnd = GetFocus();
	if( wnd->m_hWnd == m_categoryList.m_hWnd ) {
		// �J�e�S���̍��ڂɊւ������\������
		theApp.ShowMixiData( &m_selGroup->getFocusedCategory()->m_mixi );
	}else if( wnd->m_hWnd == m_groupTab.m_hWnd ) {
		// �O���[�v�̍��ڂɊւ������\������
		theApp.ShowMixiData( &m_selGroup->mixi );
	}else{
		// �{�f�B�̍��ڂɊւ������\������
		theApp.ShowMixiData( &GetSelectedBodyItem() );
	}
}

/**
 * �V�����b�Z�[�W�̃`�F�b�N
 */
bool CMZ3View::DoNewCommentCheck(void)
{
	if( theApp.m_access ) {
		// �A�N�Z�X���͋֎~
		return false;
	}

	static CMixiData mixi;
	mixi.SetAccessType( ACCESS_MAIN );

	m_checkNewComment = true;

	AccessProc( &mixi, _T("http://mixi.jp/home.pl") );

	return true;
}

/**
 * �l�b�g�A�N�Z�X
 */
void CMZ3View::AccessProc(CMixiData* data, LPCTSTR a_url, CInetAccess::ENCODING encoding)
{
	if( data != NULL ) {
		theApp.m_mixi4recv = *data;
	}

	// �A�N�Z�X��ʂ�ݒ�
	theApp.m_accessType = data->GetAccessType();

	// URL ���`
	CString uri = a_url;
	switch (data->GetAccessType()) {
	case ACCESS_BBS:
	case ACCESS_ENQUETE:
		// �R�~���j�e�B�A�A���P�[�g�Ȃ�A�擾������ύX
		switch( theApp.m_optionMng.GetPageType() ) {
		case GETPAGE_LATEST20:
			// �ŐV�Q�O���擾
			uri = a_url;
			break;

		case GETPAGE_ALL:
			// �S���擾
			{
				CString buf = a_url;
				if (buf.Find(_T("comment_count")) != -1) {
					int pos = 0;
					CString tmp = buf.Tokenize(_T("&"), pos);
					uri = tmp;
					tmp = buf.Tokenize(_T("&"), pos); // �����͎̂Ă�
					tmp = buf.Tokenize(_T("&"), pos);
					uri += _T("&");
					uri += tmp;
					uri += _T("&page=all");
				}
				else {
					uri = a_url;
					uri += _T("&page=all");
				}
			}
			break;
		}
		break;

	}

	// �yAPI �p�z
	// URL ����ID�u��
	uri.Replace( L"{owner_id}", theApp.m_loginMng.GetMixiOwnerID() );
	uri.Replace( L"{wassr:id}", theApp.m_loginMng.GetWassrId() );
	uri.Replace( L"{twitter:id}", theApp.m_loginMng.GetTwitterId() );

	data->SetBrowseUri(uri);

	// ���t�@��
	CString referer;
	switch (data->GetAccessType()) {
	case ACCESS_ENQUETE:
		// �A���P�[�g
		referer = _T("http://mixi.jp/") + data->GetURL();
		referer.Replace(_T("view_enquete"), _T("reply_enquete"));
		break;

	case ACCESS_LIST_FRIEND:
		// �}�C�~�N�ꗗ
		referer = L"http://mixi.jp/list_friend.pl";
		break;
	}

	// encoding �w��
	encoding = theApp.GetInetAccessEncodingByAccessType(data->GetAccessType());

	// MZ3 API : BASIC�F�ؐݒ�
	CString strUser = NULL;
	CString strPassword = NULL;
	util::MyLuaDataList rvals;
	rvals.push_back(util::MyLuaData(0));	// is_cancel
	rvals.push_back(util::MyLuaData(""));	// id
	rvals.push_back(util::MyLuaData(""));	// password
	if (util::CallMZ3ScriptHookFunctions2("set_basic_auth_account", &rvals, 
			util::MyLuaData(theApp.m_accessTypeInfo.getSerializeKey(data->GetAccessType()))))
	{
		int is_cancel = rvals[0].m_number;
		if (is_cancel) {
			return;
		}
		strUser     = rvals[1].m_strText;
		strPassword = rvals[2].m_strText;
	}

	// �A�N�Z�X�J�n
	theApp.m_access = true;
	m_abort = FALSE;

	// �R���g���[����Ԃ̕ύX
	MyUpdateControlStatus();

	// GET/POST ����
	bool bPost = false;	// �f�t�H���g��GET
	switch (theApp.m_accessTypeInfo.getRequestMethod(data->GetAccessType())) {
	case AccessTypeInfo::REQUEST_METHOD_POST:
		bPost = true;
		break;
	case AccessTypeInfo::REQUEST_METHOD_GET:
	case AccessTypeInfo::REQUEST_METHOD_INVALID:
	default:
		break;
	}

	static CPostData post;
	if (bPost) {
		post.SetSuccessMessage( WM_MZ3_POST_END );
		post.AppendAdditionalHeader(L"");
	}

	// [MZ3-API] GET/POST ���O�̃t�b�N����(�̗\��)
	switch (data->GetAccessType()) {
	case ACCESS_LIST_FRIEND:
		// �}�C�~�N�ꗗ
		// �}�C�~�N�ꗗ�̓u���E�U�ł͉��L�̎菇�Ŏ擾�����B
		//  1. list_friend.pl �� post_key ���擾���A
		//  2. ���̃L�[���܂߂� ajax_friend_setting.pl ���� JSON �`�����擾����
		// �]���āAMZ3 �ł͉��L�̎菇�Ŏ擾����B
		//  1. data �� post_key ���Ȃ���� list_friend.pl ����擾����B
		//  2. ��������� ajax_friend_setting.pl ����擾����B
		CString post_key;

		CMixiDataList& body = m_selGroup->getSelectedCategory()->m_body;
		if (body.size()>0) {
			post_key = body[0].GetTextValue(L"post_key");
		}

//		MZ3_TRACE(L"��post_key[%s]\n", post_key);
		if (post_key.IsEmpty()) {
			// list_friend.pl ����擾����
			uri = L"http://mixi.jp/list_friend.pl";
			bPost = false;
		} else {
			post.AppendAdditionalHeader(L"X-Requested-With: XMLHttpRequest");
			post.SetContentType(CONTENT_TYPE_FORM_URLENCODED);
			post.AppendPostBody(L"post_key=");
			post.AppendPostBody(post_key);
			post.AppendPostBody(L"&_=");
		}
		break;
	}

	// UserAgent�ݒ�
	// Wassr �̓u���E�U��UA����API���@���Ȃ����ۂ̉��
	CString strUserAgent = L"";	// "" �ŏ����l
	if (theApp.m_accessTypeInfo.getServiceType(data->GetAccessType())=="Wassr") {
		strUserAgent = MZ3_APP_NAME;
	}

	// �A�b�v�f�[�g����UA�ݒ�
	switch (data->GetAccessType()) {
	case ACCESS_SOFTWARE_UPDATE_CHECK:
		strUserAgent.Format(L"%s(%s)", MZ3_APP_NAME, MZ3_VERSION_TEXT_SHORT);
		break;
	}

	// GET/POST �J�n
	theApp.m_inet.Initialize( m_hWnd, data, encoding );
	if (bPost) {
		theApp.m_inet.DoPost(uri, referer, CInetAccess::FILE_HTML, &post, strUser, strPassword, strUserAgent );
	} else {
		theApp.m_inet.DoGet(uri, referer, CInetAccess::FILE_HTML, strUser, strPassword, strUserAgent );
	}
}

/// �E�\�t�g�L�[���j���[�b�S���ǂ�
void CMZ3View::OnGetAll()
{
	// �`�F�b�N
	//if( m_hotList != &m_bodyList ) {
	//	return;
	//}
	switch( GetSelectedBodyItem().GetAccessType() ) {
	case ACCESS_BBS:
	case ACCESS_ENQUETE:
	case ACCESS_DIARY:
	case ACCESS_MYDIARY:
	case ACCESS_NEIGHBORDIARY:
		// ok.
		break;
	default:
		// NG.
		return;
	}

	// �S���ɐݒ肵�A�A�N�Z�X�J�n
	theApp.m_optionMng.SetPageType( GETPAGE_ALL );
	AccessProc( &GetSelectedBodyItem(), util::CreateMixiUrl(GetSelectedBodyItem().GetURL()));
}

/// �E�\�t�g�L�[���j���[�b�ŐV��20����ǂ�
void CMZ3View::OnGetLast10()
{
	// �`�F�b�N
	//if( m_hotList != &m_bodyList ) {
	//	return;
	//}
	switch( GetSelectedBodyItem().GetAccessType() ) {
	case ACCESS_BBS:
	case ACCESS_ENQUETE:
	case ACCESS_DIARY:
	case ACCESS_MYDIARY:
	case ACCESS_NEIGHBORDIARY:
		// ok.
		break;
	default:
		// NG.
		return;
	}

	// 20���ɐݒ肵�A�A�N�Z�X�J�n
	theApp.m_optionMng.SetPageType( GETPAGE_LATEST20 );
	AccessProc( &GetSelectedBodyItem(), util::CreateMixiUrl(GetSelectedBodyItem().GetURL()));
}

/**
 * �J�e�S�����X�g�̑I�����ڂ̕ύX���̏���
 */
void CMZ3View::OnMySelchangedCategoryList(void)
{
	if (m_selGroup==NULL) {
		return;
	}

	// �X�^�C���ύX
	VIEW_STYLE newStyle = MyGetViewStyleForSelectedCategory();
	if (newStyle!=m_viewStyle) {
		m_viewStyle = newStyle;
		MySetLayout(0,0);
	}

	// Twitter�X�^�C���ł���΃J�e�S���ɉ����đ��M�^�C�v��������
	if (m_viewStyle==VIEW_STYLE_TWITTER) {
		// �J�e�S���ɉ�����Twitter���M���[�h��������
		MyResetTwitterStylePostMode();

		// �R���g���[����Ԃ̕ύX
		MyUpdateControlStatus();
	}

	// �I����ԁi�ԁj�̕ύX
	m_categoryList.SetActiveItem( m_selGroup->selectedCategory );

	// �I�����ڂ̍ĕ`��
	m_categoryList.Update( m_selGroup->selectedCategory );

	// �t�H�[�J�X���ڂ̍ĕ`��
	if( m_selGroup->focusedCategory != m_selGroup->selectedCategory ) {
		m_categoryList.Update( m_selGroup->focusedCategory );
	}

	// �O��I�����ڂ̍ĕ`��
	if (m_preCategory != m_selGroup->selectedCategory &&
		m_preCategory != m_selGroup->focusedCategory ) 
	{
		m_categoryList.Update( m_preCategory );
		m_preCategory = m_selGroup->selectedCategory;
	}

	CCategoryItem* pCategory = m_selGroup->getSelectedCategory();
	if (pCategory==NULL) {
		CMixiDataList body;
		SetBodyList( body );
	} else {
		// �I�����ڂ��u���擾�v�Ȃ�A�Ƃ肠�����t�@�C������擾����
		if( wcscmp( pCategory->GetAccessTimeString(), L"" ) == 0 ) {
			MyLoadCategoryLogfile( *pCategory );
		}

		// �{�f�B���X�g�ɐݒ�
		SetBodyList( pCategory->GetBodyList() );
	}

	// ��1�J�����ɕ\�����Ă�����e��\������
	MySetInfoEditFromBodySelectedData();
}

/**
 * �J�e�S�����ڂ����O�t�@�C������ǂݍ���
 */
bool CMZ3View::MyLoadCategoryLogfile( CCategoryItem& category )
{
	CString logfile = util::MakeLogfilePath( category.m_mixi );
	if( logfile.IsEmpty() ) {
		return false;
	}

	// �t�@�C���̑��݊m�F
	CFileStatus status;
	if( !CFile::GetStatus(logfile, status) ) {
		return false;
	}

	// �X�V�����̎擾
//	CString time = status.m_mtime.Format( L"%m/%d %H:%M:%S log" );

	{
		CMixiDataList& body = category.GetBodyList();

		CString msgHead;
		msgHead.Format( L"%s : ", theApp.m_accessTypeInfo.getShortText(category.m_mixi.GetAccessType()));

		// HTML �̎擾
		util::MySetInformationText( m_hWnd, msgHead + _T("��͒� : 1/3") );
		CHtmlArray html;
		html.Load( logfile );

		// HTML ���
		util::MySetInformationText( m_hWnd, msgHead + _T("��͒� : 2/3") );
		parser::MyDoParseMixiListHtml( category.m_mixi.GetAccessType(), category.m_mixi, body, html );

		// �擾�����f�[�^�� from_log_flag ��ݒ肷��
		// �i�uTwitter�̐V�������J�E���g�Ɋ܂߂�v���̂��߁j
		for (size_t i=0; i<body.size(); i++) {
			body[i].SetIntValue(L"from_log_flag", 1);
		}

		// �{�f�B�ꗗ�̐ݒ�
		util::MySetInformationText( m_hWnd, msgHead + _T("��͒� : 3/3") );

		// �擾����������̐ݒ�
		SYSTEMTIME st;
		status.m_mtime.GetAsSystemTime( st );
		category.SetAccessTime( st );
		category.m_bFromLog = true;
		m_categoryList.SetItemText( category.GetIndexOnList(), 1, category.GetAccessTimeString() );

		util::MySetInformationText( m_hWnd, msgHead + _T("����") );
	}

	return true;
}

/**
 * �u���O���J���v���j���[
 */
void CMZ3View::OnViewLog()
{
	CMixiData& mixi = GetSelectedBodyItem();

	// ���|�[�g��ʂŊJ����^�C�v�̂݃T�|�[�g����
	switch (mixi.GetAccessType()) {	
	case ACCESS_DIARY:
	case ACCESS_NEIGHBORDIARY:
	case ACCESS_BBS:
	case ACCESS_ENQUETE:
	case ACCESS_EVENT:
	case ACCESS_EVENT_JOIN:
	case ACCESS_PROFILE:
	case ACCESS_BIRTHDAY:
	case ACCESS_MYDIARY:
	case ACCESS_MESSAGE:
	case ACCESS_NEWS:
		// �T�|�[�g���Ă���
		break;
	default:
		// ���T�|�[�g�Ȃ̂ŏI������
		{
			CString msg = L"���̌`���̃��O�̓T�|�[�g���Ă��܂��� : ";
			msg += theApp.m_accessTypeInfo.getShortText(mixi.GetAccessType());
			MZ3LOGGER_INFO( msg );

			util::MySetInformationText( m_hWnd, msg );
		}
		return;
	}

	CString strLogfilePath = util::MakeLogfilePath( mixi );

	// �t�@�C�����݊m�F
	if(! util::ExistFile( strLogfilePath ) ) {
		// FILE NOT FOUND.
		CString msg = L"���O�t�@�C��������܂��� : " + strLogfilePath;
		MZ3LOGGER_INFO( msg );

		util::MySetInformationText( m_hWnd, msg );
		
		return;
	}

	// ���
	static CMixiData s_mixi;
	s_mixi = mixi;
	MyParseMixiHtml( strLogfilePath, s_mixi );

	// URL �ݒ�
	s_mixi.SetBrowseUri( util::CreateMixiUrl(s_mixi.GetURL()) );

	// �\��
	MyShowReportView( s_mixi );
}

/**
 * szHtmlfile ����͂��Amixi ���\�z����
 */
void CMZ3View::MyParseMixiHtml(LPCTSTR szHtmlfile, CMixiData& mixi)
{
	// HTML �̎擾
	CHtmlArray html;
	html.Load( szHtmlfile );

	// HTML ���
	parser::MyDoParseMixiHtml( mixi.GetAccessType(), mixi, html );
	util::MySetInformationText( m_hWnd, L"����" );
}

/**
 * ���|�[�g�r���[���J���A�f�[�^���Q�Ƃ���
 */
void CMZ3View::MyShowReportView(CMixiData& mixi)
{
	// ���|�[�g�r���[�ɑJ��
	util::MySetInformationText( m_hWnd, L"wait..." );
	theApp.m_pReportView->SetData( mixi );

	theApp.EnableCommandBarButton( ID_BACK_BUTTON, TRUE );
	theApp.EnableCommandBarButton( ID_FORWARD_BUTTON, theApp.m_pWriteView->IsWriteCompleted() ? FALSE : TRUE );
	theApp.ChangeView( theApp.m_pReportView );
}

/**
 * �ڍ׃r���[���J���A�f�[�^���Q�Ƃ���
 */
void CMZ3View::MyShowDetailView(CMixiData& data)
{
	// ���A��t�H�[�J�X�����y�C���ɂ��Ă���
	m_hotList = &m_bodyList;

	// �ڍ׃r���[�ɑJ��
	theApp.m_pDetailView->m_data = data;

	theApp.EnableCommandBarButton( ID_BACK_BUTTON, TRUE );
	theApp.EnableCommandBarButton( ID_FORWARD_BUTTON, FALSE );
	theApp.ChangeView( theApp.m_pDetailView );
}

void CMZ3View::OnHdnItemclickBodyList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMHEADER phdr = reinterpret_cast<LPNMHEADER>(pNMHDR);

	if( phdr->iItem == 1 ) {
		// �\�����ڂ̕ύX�i�g�O���j
		MyChangeBodyHeader();
	}

	*pResult = 0;
}

bool CMZ3View::MyChangeBodyHeader(void)
{
	CCategoryItem* pCategory = m_selGroup->getSelectedCategory();
	if (pCategory == NULL) {
		return false;
	}

	/**
	 * �����J�������[�h �� 1�s���[�h(A)
	 * 1�s���[�h(A) �� [B���[�h����] 1�s���[�h(B)
	 *              �� [B���[�h�Ȃ�] �����J�������[�h
	 * 1�s���[�h(B) �� �����J�������[�h
	 */

	ACCESS_TYPE categoryAccessType = pCategory->m_mixi.GetAccessType();
	AccessTypeInfo::BODY_INDICATE_TYPE colType2 = theApp.m_accessTypeInfo.getBodyHeaderCol2Type(categoryAccessType);
	AccessTypeInfo::BODY_INDICATE_TYPE colType3 = theApp.m_accessTypeInfo.getBodyHeaderCol3Type(categoryAccessType);

	// �����J�������[�h �� 1�s���[�h(�^�C�vA)
	if (theApp.m_optionMng.m_bBodyListIntegratedColumnMode) {
		// �^�C�vA�ɕύX
		pCategory->m_bodyColType2 = colType2;
		pCategory->m_bodyColType3 = colType3;

		OnAcceleratorToggleIntegratedMode();
		return true;
	}

	// ��Ԃɉ����āA�u�\�����ځv�̕ύX���s��
	// ������g�O������B
	if (colType3==AccessTypeInfo::BODY_INDICATE_TYPE_NONE) {
		// �ύX��^�C�v���Ȃ�
		// �� �����J�������[�h
		OnAcceleratorToggleIntegratedMode();
		return true;
	}

	// �g�O������
	if (pCategory->m_bodyColType2==colType2) {
		// 2 to 3
		pCategory->m_bodyColType2 = colType3;
		pCategory->m_bodyColType3 = colType2;
	} else {
		// 3 to 2
		// ? to 2
//		pCategory->m_bodyColType2 = colType2;
//		pCategory->m_bodyColType3 = colType3;

		// 3 to �����J�������[�h
		// ? to �����J�������[�h
		OnAcceleratorToggleIntegratedMode();
		return true;
	}

	// �w�b�_�[������̕ύX�i��2, 3�J�����̂݁j
	m_bodyList.SetHeader( NULL, 
		MyGetBodyHeaderColName2(categoryAccessType, pCategory->m_bodyColType2), 
		MyGetBodyHeaderColName2(categoryAccessType, pCategory->m_bodyColType3));

	if (theApp.m_optionMng.m_bBodyListIntegratedColumnMode) {
		// �����J�������[�h�ł͔w�i�ȊO�ĕ`��
		Invalidate(FALSE);
	} else {
		// �ĕ`��
		// �o�b�N�o�b�t�@�o�R�ōĕ`��
		m_bodyList.DrawDetail();
		m_bodyList.UpdateWindow();
	}

	return true;
}

/**
 * m_selGroup �̃O���[�v�ŁA�J�e�S�����X�g������������B
 */
void CMZ3View::MyUpdateCategoryListByGroupItem(void)
{
	m_categoryList.SetRedraw( FALSE );
	m_categoryList.m_bStopDraw = true;	// �\����~

	CCategoryItemList& list = m_selGroup->categories;
	size_t size = list.size();

	for( size_t i=0; i<size; i++ ) {
		CCategoryItem& category = list[i];

		// �J�e�S���[���X�g�ɒǉ�����
		// �J�e�S���[���X�g�̌��T�C�Y��� i ��
		// �������傫����Βǉ��B
		// ��������΍X�V�B
		if( i>=(size_t)m_categoryList.GetItemCount() ) {
			m_categoryList.InsertItem( category.GetIndexOnList(), category.m_name, 0 );
		}else{
			m_categoryList.SetItemText( i, 0, category.m_name );
			util::MySetListCtrlItemFocusedAndSelected( m_categoryList, i, i==m_selGroup->focusedCategory );
		}

		// �擾����������̐ݒ�
		m_categoryList.SetItemText( i, 1, category.GetAccessTimeString() );

		// ItemData �ɃC���f�b�N�X��^����
		m_categoryList.SetItemData( i, (DWORD_PTR)i );
	}
	// ���ӂꂽ�����폜
	int n = m_categoryList.GetItemCount();
	if( (size_t)n > size ) {
		for( int i=size; i<n; i++ ) {
			// ���ӂꂽ���̍ŏ��̃C���f�b�N�X(=size)��n-size��w�肵�āA�폜����
			m_categoryList.DeleteItem( size );
		}
	}

	// �t�H�[�J�X�A�I����Ԃ̕��A
	if( size > 0 ) {
		util::MySetListCtrlItemFocusedAndSelected( m_categoryList, m_selGroup->focusedCategory, true );
		m_categoryList.EnsureVisible( m_selGroup->focusedCategory, FALSE );

		// �I����ԁi�ԁj�̕��A
		m_categoryList.SetActiveItem( m_selGroup->selectedCategory );
	}

	m_categoryList.SetRedraw( TRUE );
	m_categoryList.m_bStopDraw = false;	// �\���ĊJ
	m_categoryList.Invalidate( FALSE );
}

/**
 * �O���[�v�^�u�̑I��ύX���̋��ʏ���
 */
void CMZ3View::OnSelchangedGroupTab(void)
{
	// �I�𒆂̃O���[�v���ڂ̐ݒ�
	m_selGroup = &theApp.m_root.groups[ m_groupTab.GetCurSel() ];

	// �J�e�S���[���X�g������������
	MyUpdateCategoryListByGroupItem();
	// �I�v�V�����̐ݒ�
	m_categoryList.m_bUsePanScrollAnimation = theApp.m_optionMng.m_bUseRan2PanScrollAnimation;
	m_categoryList.m_bUseHorizontalDragMove = theApp.m_optionMng.m_bUseRan2HorizontalDragMove;

	// �\�����X�V
	if( m_categoryList.WaitForPanScroll( 0L ) ){
		// ���X�N���[�����I����Ă�����
		// �w�i�摜�݂̂��\������Ă����ԂȂ̂ōŐV��ԂɍX�V����
#ifdef WINCE
		m_categoryList.Invalidate( FALSE );
#else
		m_categoryList.Invalidate( TRUE );
#endif
		m_categoryList.Update( 0 );
	}

	// �I��ύX���̏��������s����i���O�̓ǂݍ��݁j
	m_categoryList.m_bStopDraw = true;
	OnMySelchangedCategoryList();
	m_categoryList.m_bStopDraw = false;

	// ���X�N���[���̏I����҂�
	m_categoryList.WaitForPanScroll( 1000L );

	// �w�i�摜�݂̂��\������Ă����ԂȂ̂ōŐV��ԂɍX�V����
#ifdef WINCE
	m_categoryList.Invalidate( FALSE );
#else
	m_categoryList.Invalidate( TRUE );
#endif
	m_categoryList.Update( 0 );

	// �A�C�R���ĕ`��
	InvalidateRect( m_rectIcon, FALSE );
}

/**
 * �O���[�v�^�u�x���ǂݍ��ݗp�X���b�h
 */
unsigned int CMZ3View::ReloadGroupTab_Thread( LPVOID This )
{
	CMZ3View* pView = (CMZ3View*)This;

	if( pView->m_bReloadingGroupTabByThread ) {
		pView->m_bRetryReloadGroupTabByThread = true;
		return 0;
	}

	pView->m_bReloadingGroupTabByThread = true;
	::Sleep( 10L );

	// �����O��őI���O���[�v���ω����Ă���΁A�ēǍ����s��
	int selectedGroup = pView->m_groupTab.GetCurSel();
	for(;;) {
		// �I��ύX���̏��������s����
		pView->m_bRetryReloadGroupTabByThread = false;
		pView->OnSelchangedGroupTab();

		if( selectedGroup == pView->m_groupTab.GetCurSel() ) {
			// �����O�ƑI���O���[�v�������Ȃ̂ŏ��������Ƃ���B
			break;
		}else{
			// �����O�ƑI���O���[�v���Ⴄ�̂ŁA�ēǍ����s���B
			selectedGroup = pView->m_groupTab.GetCurSel();
		}
	}

	pView->m_bReloadingGroupTabByThread = false;
	pView->m_bRetryReloadGroupTabByThread = false;

	return 0;
}

/// �u�v���t�B�[���y�[�W���u���E�U�ŊJ���v���j���[�C�x���g
void CMZ3View::OnOpenBrowserUser()
{
	OnOpenBrowser();
}

/// �Љ���ȈՕ\�����j���[�C�x���g
void CMZ3View::OnOpenIntro()
{
	// �{���i�Љ�j��\��
	// ���������Ȃ̂� OnOpenSelfintro �ɈϏ��B
	OnOpenSelfintro();
}

/// ���ȏЉ���ȈՕ\�����j���[�C�x���g
void CMZ3View::OnOpenSelfintro()
{
	// �{���i���ȏЉ�j��\��
	MessageBox( GetSelectedBodyItem().GetBody() );
}

/// ���ǂɂ���
void CMZ3View::OnSetNoRead()
{
	CMixiData& mixi = GetSelectedBodyItem();

	// ���OINI�t�@�C���̍��ڂ�������
	CString logId = util::GetLogIdString( mixi );
	theApp.m_readlog.DeleteRecord( util::my_wcstombs((LPCTSTR)logId), "Log" );

	// ���O�t�@�C���폜
	DeleteFile( util::MakeLogfilePath(mixi) );

	// �r���[���X�V
	m_bodyList.Update( m_selGroup->getSelectedCategory()->selectedBody );
}


/// ���ǂɂ���
void CMZ3View::OnSetRead()
{
	CMixiData& mixi = GetSelectedBodyItem();

	// ���OINI�t�@�C���̍��ڂ�ύX
	CString logId = util::GetLogIdString( mixi );
	theApp.m_readlog.SetValue(util::my_wcstombs((LPCTSTR)logId), (const char*)util::int2str_a(mixi.GetCommentCount()), "Log");

	// �r���[���X�V
	m_bodyList.Update( m_selGroup->getSelectedCategory()->selectedBody );
}

/// �{�f�B���X�g�ł̉E�N���b�N���j���[
bool CMZ3View::PopupBodyMenu(POINT pt_, int flags_)
{
	POINT pt    = pt_;
	int   flags = flags_;

	if (pt.x==0 && pt.y==0) {
		pt = util::GetPopupPosForSoftKeyMenu2();
		flags = util::GetPopupFlagsForSoftKeyMenu2();
	}

	CMixiData& bodyItem = GetSelectedBodyItem();

	// MZ3 API : �t�b�N�֐��Ăяo��
	util::MyLuaDataList rvals;
	rvals.push_back(util::MyLuaData(0));
	CStringA serializeKey = CStringA(theApp.m_accessTypeInfo.getSerializeKey(bodyItem.GetAccessType()));
	if (util::CallMZ3ScriptHookFunctions2("popup_body_menu", &rvals, 
			util::MyLuaData(serializeKey), 
			util::MyLuaData(&bodyItem), 
			util::MyLuaData(this)))
	{
		int rval = rvals[0].m_number;
		return rval!=0 ? true : false;
	}

	switch( bodyItem.GetAccessType() ) {
	case ACCESS_DIARY:
	case ACCESS_NEIGHBORDIARY:
	case ACCESS_BBS:
	case ACCESS_ENQUETE:
	case ACCESS_EVENT:
	case ACCESS_EVENT_JOIN:
	case ACCESS_MYDIARY:
	case ACCESS_MESSAGE:
	case ACCESS_NEWS:
		{
			CMenu menu;
			menu.LoadMenu( IDR_BODY_MENU );
			CMenu* pSubMenu = menu.GetSubMenu(0);	// ��ʃ��j���[��idx=0

			switch( bodyItem.GetAccessType() ) {
			case ACCESS_ENQUETE:
			case ACCESS_BBS:
			case ACCESS_DIARY:
			case ACCESS_MYDIARY:
			case ACCESS_NEIGHBORDIARY:
				// BBS, �A���P�[�g
				// 20��or�S���̐؂�ւ����j���[���o��
				if( theApp.m_optionMng.GetPageType() == GETPAGE_ALL ) {
					// �u�S���v�Ƀ`�F�b�N
					pSubMenu->CheckMenuItem( ID_GET_ALL, MF_CHECKED | MF_BYCOMMAND );
				}
				if( theApp.m_optionMng.GetPageType() == GETPAGE_LATEST20 ) {
					// �u�ŐV��20���v�Ƀ`�F�b�N
					pSubMenu->CheckMenuItem( ID_GET_LAST10, MF_CHECKED | MF_BYCOMMAND );
				}
				break;
			default:
				// ����ȊO�ł́A�u�S���v�Ɓu�ŐV��20���v���폜�B
				pSubMenu->EnableMenuItem( ID_GET_ALL, MF_GRAYED | MF_BYCOMMAND );
				pSubMenu->EnableMenuItem( ID_GET_LAST10, MF_GRAYED | MF_BYCOMMAND );
			}

			// ���O�̗L���`�F�b�N
			if( util::ExistFile( util::MakeLogfilePath(bodyItem) ) ) {
				pSubMenu->EnableMenuItem( ID_VIEW_LOG, MF_ENABLED | MF_BYCOMMAND );
			}else{
				pSubMenu->EnableMenuItem( ID_VIEW_LOG, MF_GRAYED | MF_BYCOMMAND );
			}

			// ���j���[���J��
			pSubMenu->TrackPopupMenu( flags, pt.x, pt.y, this );
		}
		break;

	case ACCESS_COMMUNITY:
		{
			CMenu menu;
			menu.LoadMenu( IDR_BODY_MENU );
			CMenu* pSubMenu = menu.GetSubMenu(1);	// �R�~���j�e�B�p���j���[��idx=1

			// ���O�̗L���`�F�b�N
			// �R�~���j�e�B�̓g�s�b�N�ꗗ��URL��p����
			CMixiData mixi = bodyItem;
			CString url;
			url.Format( L"list_bbs.pl?id=%d", mixi::MixiUrlParser::GetID(bodyItem.GetURL()) );
			mixi.SetURL(url);
			mixi.SetAccessType( ACCESS_LIST_BBS );
			if( util::ExistFile( util::MakeLogfilePath(mixi) ) ) {
				pSubMenu->EnableMenuItem( IDM_VIEW_BBS_LIST_LOG, MF_ENABLED | MF_BYCOMMAND );
			}else{
				pSubMenu->EnableMenuItem( IDM_VIEW_BBS_LIST_LOG, MF_GRAYED | MF_BYCOMMAND );
			}

			// ���j���[���J��
			pSubMenu->TrackPopupMenu( flags, pt.x, pt.y, this );
		}
		break;

	case ACCESS_PROFILE:
	case ACCESS_BIRTHDAY:
		// �v���t�B�[���Ȃ�A�J�e�S�����ڂɉ����ď�����ύX����B�i�b��j
		switch( m_selGroup->getSelectedCategory()->m_mixi.GetAccessType() ) {
		case ACCESS_LIST_INTRO:				// �Љ
		case ACCESS_LIST_FAVORITE_USER:		// ���C�ɓ���
		case ACCESS_LIST_FOOTSTEP:			// ������
		case ACCESS_LIST_FRIEND:			// �}�C�~�N�ꗗ
		case ACCESS_LIST_CALENDAR:			// �J�����_�[
			// ��������j���[�őI��
			{
				CMenu menu;
				menu.LoadMenu( IDR_PROFILE_ITEM_MENU );
				CMenu* pSubMenu = menu.GetSubMenu(0);

				ACCESS_TYPE categoryType = m_selGroup->getSelectedCategory()->m_mixi.GetAccessType();

				// ���C�ɓ���ȊO�ł́u���ȏЉ�v���폜�B
				if( categoryType != ACCESS_LIST_FAVORITE_USER ) {
					pSubMenu->DeleteMenu( ID_OPEN_SELFINTRO, MF_BYCOMMAND );
				}

				// �Љ�ȊO�ł́u�Љ�v���폜
				if( categoryType != ACCESS_LIST_INTRO ) {
					pSubMenu->DeleteMenu( ID_OPEN_INTRO, MF_BYCOMMAND );
				}

				// ���O�̗L���`�F�b�N
				if( util::ExistFile( util::MakeLogfilePath(bodyItem) ) ) {
					pSubMenu->EnableMenuItem( ID_VIEW_LOG, MF_ENABLED | MF_BYCOMMAND );
				}else{
					pSubMenu->EnableMenuItem( ID_VIEW_LOG, MF_GRAYED | MF_BYCOMMAND );
				}

				// ���j���[���J��
				pSubMenu->TrackPopupMenu( flags, pt.x, pt.y, this );
			}

			break;
		}
		break;

	case ACCESS_RSS_READER_ITEM:
		{
			CMenu menu;
			menu.LoadMenu( IDR_RSS_MENU );
			CMenu editmenu;
			editmenu.CreatePopupMenu();

			CMenu* pSubMenu = menu.GetSubMenu(0);	// ���j���[��idx=0

			// �����N
			int n = (int)bodyItem.m_linkList.size();
			if( n > 0 ) {
				pSubMenu->AppendMenu(MF_SEPARATOR, ID_REPORT_URL_BASE, _T("-"));
				for( int i=0; i<n; i++ ) {
					// �ǉ�
					CString s;
					s.Format( L"link : %s", bodyItem.m_linkList[i].text );
					pSubMenu->AppendMenu( MF_STRING, ID_REPORT_URL_BASE+(i+1), s);
					// URL���R�s�[���j���[�̐���
					editmenu.AppendMenu( MF_STRING , ID_REPORT_COPY_URL_BASE+(i+1), s);
				}
			}
			// �����N�̉��ɁuURL���R�s�[�v���j���[��ǉ�����
			pSubMenu->AppendMenu( MF_SEPARATOR , ID_REPORT_URL_BASE, _T("-"));
			pSubMenu->AppendMenu( MF_POPUP , (UINT_PTR)editmenu.GetSafeHmenu() , L"URL���R�s�[" ); 

			// ���j���[���J��
			pSubMenu->TrackPopupMenu( flags, pt.x, pt.y, this );
		}
		break;

	}
	return true;
}

/// �g�s�b�N�ꗗ�̉{������
bool CMZ3View::PrepareViewBbsList(void)
{
	CMixiData& bodyItem = GetSelectedBodyItem();
	if (bodyItem.GetAccessType() == ACCESS_INVALID) {
		return false;
	}

	// URL �̓{�f�B�̃A�C�e������id�������p���B
	CString url;
	url.Format( L"list_bbs.pl?id=%d", mixi::MixiUrlParser::GetID(bodyItem.GetURL()) );
	// ���O�͕�����悤�ɂ��Ă���
	CString name;
	name.Format( L"��%s", bodyItem.GetName() );
	CCategoryItem categoryItem;
	categoryItem.init( name, url, ACCESS_LIST_BBS, m_selGroup->categories.size(),
		theApp.m_accessTypeInfo.getBodyHeaderCol1Type(ACCESS_LIST_BBS),
		theApp.m_accessTypeInfo.getBodyHeaderCol2Type(ACCESS_LIST_BBS),
		theApp.m_accessTypeInfo.getBodyHeaderCol3Type(ACCESS_LIST_BBS),
		CCategoryItem::SAVE_TO_GROUPFILE_NO );

	return AppendCategoryList(categoryItem);
}

/// �R�~���j�e�B�̉E�\�t�g�L�[���j���[�b�g�s�b�N�ꗗ
void CMZ3View::OnViewBbsList()
{
	if( theApp.m_access ) {
		// �A�N�Z�X���͋֎~
		return;
	}

	// �g�s�b�N�ꗗ�̉{������
	PrepareViewBbsList();

	// �擾�J�n
	CCategoryItem* pCategoryItem = m_selGroup->getSelectedCategory();
	AccessProc( &pCategoryItem->m_mixi, util::CreateMixiUrl(pCategoryItem->m_mixi.GetURL()));
}

/// �R�~���j�e�B�̉E�\�t�g�L�[���j���[�b�g�s�b�N�ꗗ�i���O�j
void CMZ3View::OnViewBbsListLog()
{
	if( theApp.m_access ) {
		// �A�N�Z�X���͋֎~
		return;
	}

	// �g�s�b�N�ꗗ�̉{������
	PrepareViewBbsList();

	// ���O����擾����
	CCategoryItem* pCategory = m_selGroup->getSelectedCategory();
	MyLoadCategoryLogfile( *pCategory );

	// �{�f�B���X�g�ɐݒ�
	SetBodyList( pCategory->GetBodyList() );

	// �t�H�[�J�X���{�f�B���X�g�ɁB
	m_bodyList.SetFocus();
}

/**
 * ����
 */
void CMZ3View::OnCruise()
{
	CCategoryItem* pCategory = m_selGroup->getFocusedCategory();
	CMixiData& mixi = pCategory->m_mixi;

	CString msg;
	msg.Format( 
		L"������J�n���܂��B��낵���ł����H\n"
		L"\n"
		L"����ΏہF%s", pCategory->m_name );
	if( MessageBox( msg, 0, MB_YESNO ) != IDYES ) {
		return;
	}

	// ����J�n
	m_cruise.start();
	m_cruise.autoCruise = false;
	m_cruise.unreadOnly = false;

	// �u�I�𒆂̃J�e�S���v���i�t�H�[�J�X�J�e�S���Łj�ύX���Ă����B
	m_selGroup->selectedCategory = m_selGroup->focusedCategory;
	OnMySelchangedCategoryList();

	AccessProc( &mixi, util::CreateMixiUrl(mixi.GetURL()));
}

/**
 * Readme.txt ����͂��A���|�[�g��ʂŕ\������B
 */
void CMZ3View::MyShowHelp(void)
{
	// *** Readme.txt ����͂��� ***

	// HTML �̎擾
	CHtmlArray html;
	html.Load( theApp.m_filepath.helpfile );

	// HTML ���
	static CMixiData mixi;
	CMixiData dummy;
	mixi = dummy;
	mixi.SetAccessType( ACCESS_HELP );
	mixi.SetTitle(MZ3_APP_NAME L" �w���v");
	parser::MyDoParseMixiHtml( mixi.GetAccessType(), mixi, html );
	util::MySetInformationText( m_hWnd, L"����" );

	// *** ��͌��ʂ�\������ ***
	theApp.m_pMainView->MyShowReportView( mixi );
}

/**
 * ChangeLog.txt ����͂��A���|�[�g��ʂŕ\������B
 */
void CMZ3View::MyShowHistory(void)
{
	// *** ChangeLog.txt ����͂��� ***

	// HTML �̎擾
	CHtmlArray html;
	html.Load( theApp.m_filepath.historyfile );

	// HTML ���
	static CMixiData mixi;
	CMixiData dummy;
	mixi = dummy;
	mixi.SetAccessType( ACCESS_HELP );
	mixi.SetTitle(MZ3_APP_NAME L" ���ŗ���");
	parser::MyDoParseMixiHtml( mixi.GetAccessType(), mixi, html );
	util::MySetInformationText( m_hWnd, L"����" );

	// *** ��͌��ʂ�\������ ***
	theApp.m_pMainView->MyShowReportView( mixi );
}

/**
 * mz3log.txt ����͂��A���|�[�g��ʂŕ\������B
 */
void CMZ3View::MyShowErrorlog(void)
{
	// ���O�����
	theApp.m_logger.finish();

	// HTML �̎擾
	CHtmlArray html;
	html.Load( theApp.m_filepath.mz3logfile );

	// ���O���J��
	theApp.m_logger.init( theApp.m_filepath.mz3logfile );

	// HTML ���
	static CMixiData mixi;
	CMixiData dummy;
	mixi = dummy;
	mixi.SetAccessType( ACCESS_ERRORLOG );
	mixi.SetTitle(MZ3_APP_NAME L" �G���[���O");
	parser::MyDoParseMixiHtml( mixi.GetAccessType(), mixi, html );
	util::MySetInformationText( m_hWnd, L"����" );

	// *** ��͌��ʂ�\������ ***
	theApp.m_pMainView->MyShowReportView( mixi );
}

/**
 * ����\��
 */
void CMZ3View::OnCheckCruise()
{
	CCategoryItem* pCategory = m_selGroup->getFocusedCategory();
	CMixiData& mixi = pCategory->m_mixi;

	CString msg;
	if( pCategory->m_bCruise ) {
		// ����ΏۂɂȂ��Ă���̂ŏ���\��̉����B
		msg.Format( 
			L"���̍��ڂ�����Ώۂ���������܂��B��낵���ł����H\n"
			L"\n"
			L"���ږ��F%s"
			, pCategory->m_name );
	}else{
		// ����Ώۂł͂Ȃ��̂ŏ���\��B
		msg.Format( 
			L"���̍��ڂ�����Ώۂɂ��܂��B��낵���ł����H\n"
			L"\n"
			L"���ږ��F%s\n"
			L"\n"
			L"������̓��j���[�b���̑��b���񂩂�J�n�ł��܂�"
			, pCategory->m_name );
	}
	if( MessageBox( msg, 0, MB_YESNO ) != IDYES ) {
		return;
	}

	// YES �Ȃ̂ŗ\��ݒ�/���������s����
	pCategory->m_bCruise = !pCategory->m_bCruise;

	// �O���[�v��`�t�@�C���̕ۑ�
	theApp.SaveGroupData();
}

bool CMZ3View::CruiseToNextCategory(void)
{
	// ���̏���J�e�S����T������
	if( !MoveToNextCruiseCategory() ) {
		// ���񊮗�
		m_cruise.finish();
		util::MySetInformationText( m_hWnd, L"���񊮗��i�\�񏄉�j" );

		// �J�e�S�����X�g�Ƀt�H�[�J�X���ړ�����
		CommandSetFocusCategoryList();
		return false;
	}

	// ���X�g���񃂁[�h
	m_cruise.backToListCruise();

	// ���񂷂�
	if( m_groupTab.GetCurSel() != m_cruise.targetGroupIndex ) {
		// �O���[�v���ω������̂ŁA�u�I�𒆂̃O���[�v�v��ύX����B
		m_selGroup = &theApp.m_root.groups[ m_cruise.targetGroupIndex ];
		m_groupTab.SetCurSel( m_cruise.targetGroupIndex );
		OnSelchangedGroupTab();
	}

	// �u�I�𒆂̃J�e�S���v��ύX����B
	util::MySetListCtrlItemFocusedAndSelected( m_categoryList, m_selGroup->focusedCategory, false );
	m_selGroup->selectedCategory = m_selGroup->focusedCategory = m_cruise.targetCategoryIndex;
	util::MySetListCtrlItemFocusedAndSelected( m_categoryList, m_selGroup->focusedCategory, true );
	CommandSetFocusCategoryList();
	OnMySelchangedCategoryList();

	// �t�H�[�J�X����y�C���ɁB

	// ����ΏۃJ�e�S���̉���
	CCategoryItem& targetCategory = theApp.m_root.groups[ m_cruise.targetGroupIndex ].categories[ m_cruise.targetCategoryIndex ];
	CMixiData& mixi = targetCategory.m_mixi;

	AccessProc( &mixi, util::CreateMixiUrl(mixi.GetURL()) );

	return true;
}

/**
 * ����J�n
 */
void CMZ3View::StartCruise( bool unreadOnly )
{
	m_cruise.unreadOnly = unreadOnly;

	if( MessageBox( L"������J�n���܂��B��낵���ł����H", NULL, MB_YESNO ) != IDYES ) {
		return;
	}

	// ����ΏۃJ�e�S���T��
	// targetGroupIndex, targetCategoryIndex ���珄��Ώۍ��ڂ�T������
	m_cruise.start();
	m_cruise.autoCruise = true;

	CruiseToNextCategory();
}

/**
 * ���̏��񍀖ڂ�T������
 *
 * @return true�F���񍀖ڔ���<br>
 *         false�F���񍀖ڂȂ�
 */
bool CMZ3View::MoveToNextCruiseCategory(void)
{
	std::vector<CGroupItem>& groups = theApp.m_root.groups;
	for( ; m_cruise.targetGroupIndex<(int)groups.size(); m_cruise.targetGroupIndex++ ) {

		CGroupItem& group = groups[m_cruise.targetGroupIndex];
		CCategoryItemList& categories = group.categories;
		for( ; m_cruise.targetCategoryIndex<(int)categories.size(); m_cruise.targetCategoryIndex++ ) {
			if( categories[ m_cruise.targetCategoryIndex ].m_bCruise ) {
				// ����
				return true;
			}
		}
		m_cruise.targetCategoryIndex = 0;
	}
	MessageBox( L"����I��" );
	return false;
}

/**
 * ���b�Z�[�W�𑗂�
 */
void CMZ3View::OnSendNewMessage()
{
	static CMixiData mixi;
	mixi = GetSelectedBodyItem();

	theApp.m_pWriteView->StartWriteView( WRITEVIEW_TYPE_NEWMESSAGE, &mixi );
}

int CMZ3View::GetListWidth(void)
{
	CRect rect;
	GetWindowRect( &rect );
	int w = rect.Width();

	// �s�N�Z�����̔������i�X�N���[������菭������������j
#ifdef WINCE
	switch( theApp.GetDisplayMode() ) {
	case SR_VGA:
		w -= 35;
		break;
	case SR_QVGA:
	default:
		w -= 35/2;
		break;
	}
#else
	w -= 35;
#endif
	return w;
}

/**
 * �J�����T�C�Y�i���j���Đݒ肷��B
 */
void CMZ3View::ResetColumnWidth()
{
	// �v�f��ʂ��u�w���v�v�Ȃ������\�����Ȃ��B

	// ���̒�`
	int w = GetListWidth();

	// �J�e�S�����X�g�� 25:20 �̔䗦�ŕ�������
	if( m_categoryList.m_hWnd != NULL ) {
		const int W_COL1 = theApp.m_optionMng.m_nMainViewCategoryListCol1Ratio;
		const int W_COL2 = theApp.m_optionMng.m_nMainViewCategoryListCol2Ratio;
		m_categoryList.SetColumnWidth(0, w * W_COL1/(W_COL1+W_COL2) );
		m_categoryList.SetColumnWidth(1, w * W_COL2/(W_COL1+W_COL2) );
	}

	// �{�f�B���X�g�� 24:21 �̔䗦�ŕ�������
	if( m_bodyList.m_hWnd != NULL ) {
		const int W_COL1 = theApp.m_optionMng.m_nMainViewBodyListCol1Ratio;
		const int W_COL2 = theApp.m_optionMng.m_nMainViewBodyListCol2Ratio;
		m_bodyList.SetColumnWidth(0, w * W_COL1/(W_COL1+W_COL2) );
		m_bodyList.SetColumnWidth(1, w * W_COL2/(W_COL1+W_COL2) );
	}
}

/**
 * �w�b�_�̃h���b�O�I��
 *
 * �J�������̍č\�z���s���B
 */
void CMZ3View::OnHdnEndtrackHeaderList(NMHDR *pNMHDR, LRESULT *pResult)
{
	MZ3LOGGER_DEBUG( L"OnHdnEndtrackHeaderList" );

	LPNMHEADER phdr = reinterpret_cast<LPNMHEADER>(pNMHDR);

	// �J�������̔��f
	m_bodyList.SetColumnWidth( phdr->iItem, phdr->pitem->cxy );

	CRect rect;
	CHeaderCtrl* pHeader = NULL;

	// �{�f�B���X�g�w�b�_�̎擾
	if( (pHeader = m_bodyList.GetHeaderCtrl()) == NULL ) {
		MZ3LOGGER_ERROR( L"�{�f�B���X�g�̃w�b�_���擾�ł��Ȃ��̂ŏI��" );
		return;
	}

	// �J�����P
	if(! pHeader->GetItemRect( 0, rect ) ) {
		MZ3LOGGER_ERROR( L"�{�f�B���X�g�̃w�b�_�A��1�J�����̕����擾�ł��Ȃ��̂ŏI��" );
		return;
	}
	theApp.m_optionMng.m_nMainViewBodyListCol1Ratio = rect.Width();

	// �J�����Q
	// �ŏI�J�����Ȃ̂ŁA���X�g��-���̃J�����T�C�Y�Ƃ���B
	theApp.m_optionMng.m_nMainViewBodyListCol2Ratio
		= GetListWidth() - theApp.m_optionMng.m_nMainViewBodyListCol1Ratio;

	// �J�e�S�����X�g

	// �J�e�S�����X�g�̕��̓h���b�O�ŕύX�ł��Ȃ��̂ŁA�ۑ����Ȃ��B
/*	if( (pHeader = m_categoryList.GetHeaderCtrl()) == NULL ) {
		MZ3LOGGER_ERROR( L"�J�e�S�����X�g�̃w�b�_ �� NULL �Ȃ̂ŏI��" );
		return;
	}

	// �J�����P
	if(! pHeader->GetItemRect( 0, rect ) ) return;
	theApp.m_optionMng.m_nMainViewCategoryListCol1Ratio = rect.Width();

	// �J�����Q
	// �ŏI�J�����Ȃ̂ŁA���X�g��-���̃J�����T�C�Y�Ƃ���B
	theApp.m_optionMng.m_nMainViewCategoryListCol2Ratio
		= GetListWidth() - theApp.m_optionMng.m_nMainViewCategoryListCol1Ratio;
*/

	*pResult = 0;
}

/**
 *
 * @return ����p���̏ꍇ�� true�A����I���̏ꍇ�� false ��Ԃ�
 */
bool CMZ3View::DoNextBodyItemCruise()
{
	CMixiDataList& bodyList = m_selGroup->getSelectedCategory()->GetBodyList();
	if( m_cruise.targetBodyItem >= (int)bodyList.size() ) {
		// ����I��
		if( m_cruise.autoCruise ) {
			// �\�񏄉�Ȃ̂Ŏ��ɐi��
			m_cruise.targetCategoryIndex++;
			bool rval = CruiseToNextCategory();
			if( rval ) {
				// �ʐM�I��

				// ���ڂ�I��/�\����Ԃɂ���
				int idx = m_selGroup->getSelectedCategory()->selectedBody;
				util::MySetListCtrlItemFocusedAndSelected( m_bodyList, idx, true );
				m_bodyList.EnsureVisible( idx, FALSE );
			}
			return rval;
		}else{
			// �ꎞ����Ȃ̂ł����ŏI���B
			m_cruise.finish();
			MessageBox( L"�N�C�b�N���񊮗�" );

			// �ʐM�I��
			return false;
		}
	}else{
		// ���̏���Ώۂ��擾����

		// ���̃{�f�B�v�f�Ɉړ�����

		m_selGroup->getSelectedCategory()->selectedBody = m_cruise.targetBodyItem;

		CMixiData& mixi = GetSelectedBodyItem();

		// ���Ǐ��񃂁[�h�Ȃ�A���Ǘv�f���X�L�b�v����
		if( m_cruise.unreadOnly ) {

			// ���Ǐ��񃂁[�h�Ȃ̂ŁA���̖��Ǘv�f��T������B
			// �S�Ċ��ǂȂ玟�̃J�e�S���ցB
			bool unread = false;	// ���ǃt���O
			switch( mixi.GetAccessType() ) {
			case ACCESS_BBS:
			case ACCESS_EVENT:
			case ACCESS_EVENT_JOIN:
			case ACCESS_ENQUETE:
				// �R�~���j�e�B�A�C�x���g�A�A���P�[�g�Ȃ̂ŁA
				// �Y���g�s�b�N�̃R�����g��S�Ċ��ǂȂ���ǂƔ��肷��B
				{
					int lastIndex = mixi::ParserUtil::GetLastIndexFromIniFile(mixi);
					if (lastIndex == -1) {
						// �S���̖���
						unread = true;
					} else if (lastIndex >= mixi.GetCommentCount()) {
						// �X�V�Ȃ�
						unread = false;
					} else {
						// ���ǂ���
						unread = true;
					}
				}
				break;
			default:
				// �R�~���j�e�B�ȊO�Ȃ̂ŁA���O�̗L���Ŋ��ǁE���ǂ𔻒肷��
				if(! util::ExistFile(util::MakeLogfilePath(mixi)) ) {
					unread = true;
				}
				break;
			}
			if(! unread ) {
				// ���ǂȂ̂Ŏ��̃{�f�B�v�f�ցB
				m_cruise.targetBodyItem ++;

				// �ċA�Ăяo���ɂ��A���̏��񍀖ڂ�T���B
				return DoNextBodyItemCruise();
			}
		}

		// ���ڂ�I��/�\����Ԃɂ���
		int idxNext = m_cruise.targetBodyItem;
		util::MySetListCtrlItemFocusedAndSelected( m_bodyList, idxNext, true );
		m_bodyList.EnsureVisible( idxNext, FALSE );

		// �擾����
		AccessProc( &mixi, util::CreateMixiUrl(mixi.GetURL()) );

		// �ʐM�p��
		return true;
	}
}

void CMZ3View::OnSettingChange(UINT uFlags, LPCTSTR lpszSection)
{
	CFormView::OnSettingChange(uFlags, lpszSection);

/*	MZ3LOGGER_DEBUG( L"OnSettingChange( " + util::int2str(uFlags) + L", " + lpszSection + L" )" );

	if( uFlags & SETTINGCHANGE_RESET ) {
	}
*/
}

static const int N_HC_MIN = 10;		///< �J�e�S�����X�g�̍ŏ��l [%]
static const int N_HC_MAX = 90;		///< �J�e�S�����X�g�̍ő�l [%]
static const int N_HB_MIN = 10;		///< �{�f�B���X�g�̍ŏ��l [%]
static const int N_HB_MAX = 90;		///< �{�f�B���X�g�̍ő�l [%]

/**
 * �J�e�S�����X�g����������
 */
void CMZ3View::OnLayoutCategoryMakeNarrow()
{
	int& hc = theApp.m_optionMng.m_nMainViewCategoryListHeightRatio;
	int& hb = theApp.m_optionMng.m_nMainViewBodyListHeightRatio;

	// �I�v�V�����l�� % �ɕ␳
	int sum = hc + hb;
	if (sum>0) {
		hc = (int)(hc * 100.0 / sum);
		hb = (int)(hb * 100.0 / sum);
	}

	// ���߂�
	const int STEP = 5;
	hc -= STEP;
	hb += STEP;

	if (sum<=0 || hc < N_HC_MIN || hb > N_HB_MAX) {
		// �ŏ��l�ɐݒ�
		hc = N_HC_MIN;
		hb = N_HB_MAX;
	}

	// �ĕ`��
	CMainFrame* pMainFrame = (CMainFrame*)theApp.m_pMainWnd;
	pMainFrame->ChangeAllViewFont();
}

/**
 * �J�e�S�����X�g���L������
 */
void CMZ3View::OnLayoutCategoryMakeWide()
{
	int& hc = theApp.m_optionMng.m_nMainViewCategoryListHeightRatio;
	int& hb = theApp.m_optionMng.m_nMainViewBodyListHeightRatio;

	// �I�v�V�����l�� % �ɕ␳
	int sum = hc + hb;
	if (sum>0) {
		hc = (int)(hc * 100.0 / sum);
		hb = (int)(hb * 100.0 / sum);
	}

	// �L������
	const int STEP = 5;
	hc += STEP;
	hb -= STEP;

	if (sum<=0 || hc > N_HC_MAX || hb < N_HB_MIN) {
		// �ŏ��l�ɐݒ�
		hc = N_HC_MAX;
		hb = N_HB_MIN;
	}

	// �ĕ`��
	CMainFrame* pMainFrame = (CMainFrame*)theApp.m_pMainWnd;
	pMainFrame->ChangeAllViewFont();
}

/**
 * �J�e�S�����X�g�̉E�N���b�N�C�x���g
 */
void CMZ3View::OnNMRclickHeaderList(NMHDR *pNMHDR, LRESULT *pResult)
{
	if (theApp.m_pMouseGestureManager->IsProcessed()) {
		// �}�E�X�W�F�X�`���������s��ꂽ���߃L�����Z��
		return;
	}

	// �J�e�S�����X�g�ł̉E�N���b�N
	PopupCategoryMenu();

	*pResult = 0;
}

/**
 * �J�e�S�����X�g�̃|�b�v�A�b�v���j���[
 */
void CMZ3View::PopupCategoryMenu(POINT pt_, int flags_)
{
	POINT pt    = pt_;
	int   flags = flags_;

	if (pt.x==0 && pt.y==0) {
		pt = util::GetPopupPosForSoftKeyMenu2();
		flags = util::GetPopupFlagsForSoftKeyMenu2();
	}

	CMenu menu;
	menu.LoadMenu(IDR_CATEGORY_MENU);
	CMenu* pSubMenu = menu.GetSubMenu(0);

	// ����ΏۈȊO�̃J�e�S���ł���Ώ��񃁃j���[�𖳌�������
	CCategoryItem* pCategory = m_selGroup->getFocusedCategory();
	if (pCategory != NULL) {
		if (theApp.m_accessTypeInfo.isCruiseTarget(pCategory->m_mixi.GetAccessType())) {
			// ����ΏۂȂ̂ŏ��񃁃j���[�𖳌������Ȃ�
		} else {
			// ���񃁃j���[�𖳌�������
			pSubMenu->EnableMenuItem( IDM_CRUISE, MF_GRAYED | MF_BYCOMMAND );
			pSubMenu->EnableMenuItem( IDM_CHECK_CRUISE, MF_GRAYED | MF_BYCOMMAND );
		}

		// ����\��ς݂ł���΃`�F�b�N��t����B
		if( pCategory->m_bCruise ) {
			pSubMenu->CheckMenuItem( IDM_CHECK_CRUISE, MF_CHECKED );
		}else{
			pSubMenu->CheckMenuItem( IDM_CHECK_CRUISE, MF_UNCHECKED );
		}
	} else {
		// ���񃁃j���[�𖳌�������
		pSubMenu->EnableMenuItem( IDM_CRUISE, MF_GRAYED | MF_BYCOMMAND );
		pSubMenu->EnableMenuItem( IDM_CHECK_CRUISE, MF_GRAYED | MF_BYCOMMAND );
	}

	// ��ʃ��C�A�E�g
	const int CATEGORY_MENU_LAYOUT_SUB_MENU_IDX = 6;
	if (pSubMenu->GetMenuItemCount()<=CATEGORY_MENU_LAYOUT_SUB_MENU_IDX) {
		MZ3LOGGER_FATAL(L"�J�e�S�����j���[�̍��ڐ����s���ł�");
	} else {
		CMenu* pLayoutMenu = pSubMenu->GetSubMenu(CATEGORY_MENU_LAYOUT_SUB_MENU_IDX);
		if (pLayoutMenu) {
			// �J�e�S�����X�g�\��
			pLayoutMenu->CheckMenuItem(IDM_LAYOUT_MAGNIFY_CATEGORY_LIST,
				(m_magnifyMode==MAGNIFY_MODE_CATEGORY ? MF_CHECKED : MF_UNCHECKED) | MF_BYCOMMAND);

			// �{�f�B���X�g�\��
			pLayoutMenu->CheckMenuItem(IDM_LAYOUT_MAGNIFY_BODY_LIST,
				(m_magnifyMode==MAGNIFY_MODE_BODY ? MF_CHECKED : MF_UNCHECKED) | MF_BYCOMMAND);

			// �㉺�y�C�������ɖ߂�
			pLayoutMenu->EnableMenuItem(IDM_LAYOUT_MAGNIFY_DEFAULT,
				(m_magnifyMode==MAGNIFY_MODE_DEFAULT ? MF_GRAYED : MF_ENABLED) | MF_BYCOMMAND);
		}
	}

	// ���ڂ�ǉ�
	const int CATEGORY_MENU_APPEND_SUB_MENU_IDX = 7;
	if (pSubMenu->GetMenuItemCount()<=CATEGORY_MENU_APPEND_SUB_MENU_IDX) {
		MZ3LOGGER_FATAL(L"�J�e�S�����j���[�̍��ڐ����s���ł�");
	} else {
		CMenu* pAppendMenu = pSubMenu->GetSubMenu(CATEGORY_MENU_APPEND_SUB_MENU_IDX);
		if (pAppendMenu) {
			// �_�~�[���폜
			pAppendMenu->RemoveMenu( ID_APPEND_MENU_BEGIN, MF_BYCOMMAND );

			// �t���e���v���[�g����
			Mz3GroupData template_data;
			template_data.initForTopPage(theApp.m_accessTypeInfo, Mz3GroupData::InitializeType());

			// �T�u���j���[�̏�����
			if (m_pCategorySubMenuList != NULL) {
				delete[] m_pCategorySubMenuList;
			}
			m_pCategorySubMenuList = new CMenu[template_data.groups.size()];

			int menuId = ID_APPEND_MENU_BEGIN;
			for (unsigned int groupIdx=0; groupIdx<template_data.groups.size(); groupIdx++) {
				m_pCategorySubMenuList[groupIdx].CreatePopupMenu();

				CGroupItem& group = template_data.groups[groupIdx];

				// m_pCategorySubMenuList[groupIdx] �ɃJ�e�S������ǉ�
				for (unsigned int ic=0; ic<group.categories.size(); ic++) {
					m_pCategorySubMenuList[groupIdx].AppendMenuW( MF_STRING, menuId, group.categories[ic].m_name );
					menuId ++;
				}

				pAppendMenu->AppendMenuW( MF_POPUP, (UINT)m_pCategorySubMenuList[groupIdx].m_hMenu, group.name );
				m_pCategorySubMenuList[groupIdx].Detach();
			}
		}
	}

	// �u���ڂ��폜�v�̗L���E����
	if (pCategory != NULL) {
		pSubMenu->CheckMenuItem( ID_REMOVE_CATEGORY_ITEM, MF_BYCOMMAND | (pCategory->bSaveToGroupFile ? MF_UNCHECKED : MF_CHECKED) );
	} else {
		pSubMenu->RemoveMenu( ID_REMOVE_CATEGORY_ITEM, MF_BYCOMMAND );
	}

	// �u���ڂ�ύX�v
	if (pCategory == NULL) {
		pSubMenu->RemoveMenu( ID_EDIT_CATEGORY_ITEM, MF_BYCOMMAND );
	}

	// ���j���[�\��
	pSubMenu->TrackPopupMenu(flags, pt.x, pt.y, this);
}

/**
 * �{�f�B���X�g�̉E�N���b�N�C�x���g
 */
void CMZ3View::OnNMRclickBodyList(NMHDR *pNMHDR, LRESULT *pResult)
{
	if (theApp.m_pMouseGestureManager->IsProcessed()) {
		// �}�E�X�W�F�X�`���������s��ꂽ���߃L�����Z��
		return;
	}

	PopupBodyMenu();
	*pResult = 0;
}

void CMZ3View::OnTimer(UINT_PTR nIDEvent)
{
	if (nIDEvent == TIMERID_INTERVAL_CHECK) {
		// ����擾�@�\
		if (theApp.m_optionMng.m_bEnableIntervalCheck) {
			// �t�H�[�J�X�`�F�b�N
			// ���݂̃A�v���� MZ3�F
			//  �t�H�[�J�X���J�e�S�����X�g�ɂ���ꍇ
			// ���݂̃A�v���� MZ3 �ȊO�F
			//  View �� MZ3View �ł���ꍇ
			// �Ƃ����`�F�b�N�����������ǁA�Ƃ肠����View�̃`�F�b�N�̂݁B
			CMainFrame* pMainFrame = (CMainFrame*)theApp.m_pMainWnd;
			if (pMainFrame->GetActiveView() != theApp.m_pMainView) {
				// ���݂�View��MZ3View�ȊO�Ȃ̂Œ���擾���s��Ȃ��B
				
				// �^�C�}�[���X�V�i�����N�b�o�܂ő҂j
				ResetIntervalTimer();
				return;
			}

			// �^�C�}�[�J�n���� N �b�o�߂������H
			int nElapsedSec = (GetTickCount() - m_dwIntervalTimerStartMsec)/1000;
			if( nElapsedSec >= theApp.m_optionMng.m_nIntervalCheckSec ) {
				util::MySetInformationText( m_hWnd, _T("������擾���J�n���܂�") );

				// �o�߁B�擾�J�n�B
				RetrieveCategoryItem();

				// �^�C�}�[���X�V
				ResetIntervalTimer();
			} else {
				// �J�E���g�_�E��
				int restSec = theApp.m_optionMng.m_nIntervalCheckSec - nElapsedSec;
				switch( restSec ) {
				case 1:
					util::MySetInformationText( m_hWnd, _T("������擾1�b�O") );
					break;
				case 2:
					util::MySetInformationText( m_hWnd, _T("������擾2�b�O") );
					break;
				case 3:
					util::MySetInformationText( m_hWnd, _T("������擾3�b�O") );
					break;
				}
			}
		}
		return;
	}

	if (nIDEvent == TIMERID_RESTORE_STATUSBAR) {
		KillTimer(nIDEvent);

		// �X�e�[�^�X�o�[�̕��A
		MySetInfoEditFromBodySelectedData();

		return;
	}

	CFormView::OnTimer(nIDEvent);
}

bool CMZ3View::RetrieveCategoryItem(void)
{
	// �A�N�Z�X���͍ăA�N�Z�X�s��
	if (theApp.m_access) {
		return false;
	}
	if (m_selGroup==NULL) {
		return false;
	}
	CCategoryItem* pCategory = m_selGroup->getSelectedCategory();
	if (pCategory==NULL) {
		return false;
	}

	// MZ3 API : �n���h���Ăяo��
	util::MyLuaDataList rvals;
	rvals.push_back(util::MyLuaData(0));
	if (util::CallMZ3ScriptHookFunctions2("retrieve_category_item", &rvals, 
			util::MyLuaData(theApp.m_accessTypeInfo.getSerializeKey(pCategory->m_mixi.GetAccessType()))))
	{
		// Lua ���ŏ�������
		switch (rvals[0].m_number) {
		case 2:
			// ��������
			return true;

		case 1:
			// �u�b�N�}�[�N���l�Ƀ��[�J���X�g���[�W�o�R�Ŏ擾
			SetBodyList( pCategory->GetBodyList() );
			return true;

		case 0:
		default:
			// �C���^�[�l�b�g�ɃA�N�Z�X(���s)
			break;
		}
	}

	// �C���^�[�l�b�g�ɃA�N�Z�X
	m_hotList = &m_bodyList;
	AccessProc( &pCategory->m_mixi, util::CreateMixiUrl(pCategory->m_mixi.GetURL()));

	return true;
}

/**
 * �^�C�}�[���X�V�i�����N�b�o�܂ő҂j
 */
void CMZ3View::ResetIntervalTimer(void)
{
	m_dwIntervalTimerStartMsec = GetTickCount();
}

BOOL CMZ3View::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	if (theApp.m_optionMng.m_bEnableIntervalCheck) {
		// ���b�Z�[�W�ɉ����āA����擾�̃L�����Z���������s��
		LPNMHDR pnmhdr = (LPNMHDR)lParam;
		int id = wParam;
		switch( id ) {
		case IDC_HEADER_LIST:
		case IDC_BODY_LIST:
			switch( pnmhdr->code ) {
			case NM_RCLICK:
			case NM_CLICK:
			case NM_DBLCLK:
//			case LVN_ITEMCHANGED:
				ResetIntervalTimer();
				break;
			}
			break;
		case IDC_GROUP_TAB:
			switch(pnmhdr->code ) {
			case TCN_SELCHANGE:
				ResetIntervalTimer();
				break;
			}
			break;
		}
	}

	return CFormView::OnNotify(wParam, lParam, pResult);
}

/**
 * �^�u�̃N���b�N
 */
void CMZ3View::OnNMClickGroupTab(NMHDR *pNMHDR, LRESULT *pResult)
{
	// ���z�I�Ƀ_�u���N���b�N�𔻒肷��B
	static int s_idxLast = -1;
	static DWORD s_dwLastClickedTickCount = GetTickCount();

	int idx = m_groupTab.GetCurSel();
	if (s_idxLast != idx) {
		s_idxLast = idx;
		s_dwLastClickedTickCount = GetTickCount();
	} else {

		// �������l���V�X�e������擾���A�_�u���N���b�N����
		if (GetTickCount() - s_dwLastClickedTickCount < GetDoubleClickTime()) {
			s_idxLast = -1;

			// �_�u���N���b�N���̏��������s�F
			// �I���J�e�S���̃����[�h
			RetrieveCategoryItem();
		} else {
			s_dwLastClickedTickCount = GetTickCount();
		}
	}

	*pResult = 0;
}

/**
 * �{�f�B���X�g�I�����ڂ̉摜���Ȃ���Ύ擾����
 */
void CMZ3View::MoveMiniImageDlg(int idxBody/*=-1*/, int pointx/*=-1*/, int pointy/*=-1*/)
{
	if (!theApp.m_optionMng.m_bShowMainViewMiniImage) {
		return;
	}

	if (m_selGroup!=NULL) {
		CCategoryItem* pCategory = m_selGroup->getSelectedCategory();
		if (pCategory!=NULL) {
			// �Ώ�(idxBody�p�����[�^)�����w��ł���΁u�I�����ځv��ΏۂƂ���B
			int target = idxBody;
			if (idxBody<0 || idxBody>=(int)pCategory->m_body.size()) {
				target = pCategory->selectedBody;
			}

			// mini�摜�������[�h�ł���Ύ擾����
			if (!pCategory->m_body.empty() && 0 <= target && target < (int)pCategory->m_body.size() ) {
				CMixiData& data = pCategory->m_body[ target ];
				MyLoadMiniImage( data );
			}
		}
	}
}

LRESULT CMZ3View::OnHideView(WPARAM wParam, LPARAM lParam)
{
	return TRUE;
}

bool CMZ3View::MyLoadMiniImage(CMixiData& mixi)
{
	if (!theApp.m_optionMng.m_bShowMainViewMiniImage) {
		return false;
	}

	if (!theApp.m_optionMng.m_bAutoLoadMiniImage) {
		return false;
	}

	CString miniImagePath = util::MakeImageLogfilePath( mixi );
	if (!miniImagePath.IsEmpty()) {
		if (!util::ExistFile(miniImagePath)) {
			if (!theApp.m_access) {
				// �A�N�Z�X���͋֎~
				// �擾
				static CMixiData s_data;
				CMixiData dummy;
				s_data = dummy;
				s_data.SetAccessType( ACCESS_IMAGE );

				CString url = mixi.GetImage(0);

				// �A�N�Z�X��ʂ�ݒ�
				theApp.m_accessType = s_data.GetAccessType();

				// �A�N�Z�X�J�n
				theApp.m_access = true;
				m_abort = FALSE;

				// �R���g���[����Ԃ̕ύX
				MyUpdateControlStatus();

				theApp.m_inet.Initialize( m_hWnd, &s_data );
				theApp.m_inet.DoGet(url, L"", CInetAccess::FILE_BINARY );
			}
		}
	}

	return true;
}

/**
 * �t�H���g�g��
 */
void CMZ3View::OnAcceleratorFontMagnify()
{
	CMainFrame* pMainFrame = (CMainFrame*)theApp.m_pMainWnd;
	pMainFrame->OnMenuFontMagnify();
}

/**
 * �t�H���g�k��
 */
void CMZ3View::OnAcceleratorFontShrink()
{
	CMainFrame* pMainFrame = (CMainFrame*)theApp.m_pMainWnd;
	pMainFrame->OnMenuFontShrink();
}

/**
 * �R���e�L�X�g���j���[�\��
 */
void CMZ3View::OnAcceleratorContextMenu()
{
	if( GetFocus() == &m_bodyList ) {
		// �{�f�B���X�g�ł̉E�N���b�N���j���[
		PopupBodyMenu();
	}else{
		// �J�e�S�����X�g�ł̉E�N���b�N
		PopupCategoryMenu();
	}
}

/**
 * ���̃^�u
 */
void CMZ3View::OnAcceleratorNextTab()
{
	CommandSelectGroupTabNextItem();
}

/**
 * �O�̃^�u
 */
void CMZ3View::OnAcceleratorPrevTab()
{
	CommandSelectGroupTabBeforeItem();
}

/**
 * �z�C�[��
 */
BOOL CMZ3View::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	if (theApp.m_pMouseGestureManager->IsGestureMode()) {
		// Ctrl+�z�C�[���Ŋg��E�k��
		// �������t�ɂȂ�����L�����Z��
		int& s_delta = theApp.m_pMouseGestureManager->m_delta;
		if ((s_delta>0 && zDelta<0) || (s_delta<0 && zDelta>0)) {
			s_delta = 0;
		}
		s_delta += zDelta;

		if (s_delta>=WHEEL_DELTA) {
			// �O�̃^�u
			CommandSelectGroupTabBeforeItem();

			// �E�N���b�N�����L�����Z���̂��߁A�����ς݃t���O��ݒ肷��
			theApp.m_pMouseGestureManager->SetProcessed();
			s_delta -= WHEEL_DELTA;
		} else if (s_delta<=-WHEEL_DELTA) {
			// ���̃^�u
			CommandSelectGroupTabNextItem();

			// �E�N���b�N�����L�����Z���̂��߁A�����ς݃t���O��ݒ肷��
			theApp.m_pMouseGestureManager->SetProcessed();
			s_delta += WHEEL_DELTA;
		}
		return TRUE;
	}

	if (nFlags & MK_CONTROL) {
		// Ctrl+�z�C�[���Ŋg��E�k��
		// ���𑜓x�z�C�[���Ή��̂��߁Adelta �l��ݐς���B
		static int s_delta = 0;
		// �������t�ɂȂ�����L�����Z��
		if ((s_delta>0 && zDelta<0) || (s_delta<0 && zDelta>0)) {
			s_delta = 0;
		}
		s_delta += zDelta;

		if (s_delta>=WHEEL_DELTA) {
			OnAcceleratorFontMagnify();
			s_delta -= WHEEL_DELTA;
		} else if (s_delta<=-WHEEL_DELTA) {
			OnAcceleratorFontShrink();
			s_delta += WHEEL_DELTA;
		}
		return TRUE;
	}

	return CFormView::OnMouseWheel(nFlags, zDelta, pt);
}

/**
 * �����[�h (F5)
 */
void CMZ3View::OnAcceleratorReload()
{
	if (theApp.m_access) {
		// �A�N�Z�X���͍ăA�N�Z�X�s��
		return;
	}

	// �A�N�Z�X�J�n
	if (!RetrieveCategoryItem()) {
		return;
	}
}

/**
 * �J�e�S���̑I����Ԃɉ������r���[�X�^�C���̔���
 */
CMZ3View::VIEW_STYLE CMZ3View::MyGetViewStyleForSelectedCategory(void)
{
	if (m_selGroup!=NULL) {
		CCategoryItem* pCategory = m_selGroup->getSelectedCategory();
		if (pCategory!=NULL) {

			// MZ3 API : �n���h���Ăяo��
			util::MyLuaDataList rvals;
			rvals.push_back(util::MyLuaData(VIEW_STYLE_DEFAULT));
			if (util::CallMZ3ScriptHookFunctions2("get_view_style", &rvals, 
					util::MyLuaData(theApp.m_accessTypeInfo.getSerializeKey(pCategory->m_mixi.GetAccessType()))))
			{
				// Lua ���ŏ��������B�I���B
				return (VIEW_STYLE)rvals[0].m_number;
			}

			if (pCategory->m_mixi.GetAccessType()==ACCESS_MIXI_RECENT_ECHO) {
				// �G�R�[�� Twitter �X�^�C��
				return VIEW_STYLE_TWITTER;
			}

			if (m_bodyList.IsEnableIcon()) {
				// �摜������΁u�C���[�W�t���v�Ƃ���
				CImageList* pImageList = m_bodyList.GetImageList(LVSIL_SMALL);
				if (pImageList != NULL &&
					pImageList->m_hImageList == theApp.m_imageCache.GetImageList16().m_hImageList)
				{
					return VIEW_STYLE_IMAGE;
				}
			}
		}
	}
	return VIEW_STYLE_DEFAULT;
}

/**
 * Twitter, �X�V
 */
void CMZ3View::OnBnClickedUpdateButton()
{
	if (theApp.m_access) {
		return;
	}

	// �J�e�S���I����Ԋm�F
	CCategoryItem* pCategory = m_selGroup->getSelectedCategory();
	if (pCategory==NULL) {
		MZ3LOGGER_ERROR( L"�J�e�S�������I��" );
		return;
	}

	// MZ3 API : �X�V�{�^�������C�x���g
	{
		util::MyLuaDataList rvals;
		if (util::CallMZ3ScriptHookFunctions2("click_update_button", &rvals, 
				util::MyLuaData(theApp.m_accessTypeInfo.getSerializeKey(m_twitterPostAccessType))))
		{
			// Lua ���ŏ��������B�I���B
			return;
		}
	}

	//----------------------------------------------------------------
	// TODO �ȉ��̃R�[�h�� Twitter �ɕ���� Lua�X�N���v�g�����邱�� 
	//----------------------------------------------------------------

	// ���͕�������擾
	CString strStatus;
	GetDlgItemText( IDC_STATUS_EDIT, strStatus );

	// �A�N�Z�X��ʂ�ݒ�
	theApp.m_accessType = m_twitterPostAccessType;

	// �����͎��̏���
	if (strStatus.IsEmpty()) {
		switch (m_twitterPostAccessType) {
		case ACCESS_GOOHOME_QUOTE_UPDATE:
		default:
			// �����͂Ȃ̂ōŐV�擾
			RetrieveCategoryItem();
			return;
		}
	}

	// ���M��m�F
	switch (m_twitterPostAccessType) {
	case ACCESS_GOOHOME_QUOTE_UPDATE:
		{
			CString msg;
			msg.Format( 
				L"goo�z�[���ЂƂ��ƂŔ������܂��B\r\n"
				L"----\r\n"
				L"%s\r\n"
				L"----\r\n"
				L"��낵���ł����H", 
				strStatus );
			if (IDYES != MessageBox(msg, 0, MB_YESNO)) {
				return;
			}
		}
		break;

	default:
		break;
	}

	// �t�H�[�J�X����͗̈�Ɉړ�
	GetDlgItem( IDC_STATUS_EDIT )->SetFocus();

	// ���͗̈�𖳌���
	GetDlgItem( IDC_STATUS_EDIT )->EnableWindow( FALSE );

	static CPostData post;
	post.ClearPostBody();

	// POST �p�����[�^��ݒ�
	switch (m_twitterPostAccessType) {

	case ACCESS_GOOHOME_QUOTE_UPDATE:
		{
			// text=***&privacy=***
			// privacy 1:����J
			//         2:���J
			//         4:�F�B�܂Ō��J
			//         8:�F�B�̗F�B�܂Ō��J
			post.AppendPostBody( "text=" );
			post.AppendPostBody( URLEncoder::encode_utf8(strStatus) );
			post.AppendPostBody( "&privacy=2" );
		}
		break;

	}
	post.SetContentType( CONTENT_TYPE_FORM_URLENCODED );
	post.SetSuccessMessage( WM_MZ3_POST_END );

	// POST��URL�ݒ�
	CString url;
	switch (m_twitterPostAccessType) {
	case ACCESS_GOOHOME_QUOTE_UPDATE:
		url = L"http://home.goo.ne.jp/api/quote/quotes/post/json";
		break;
	}

	// MZ3 API : BASIC�F�ؐݒ�
	CString strUser = NULL;
	CString strPassword = NULL;
	util::MyLuaDataList rvals;
	rvals.push_back(util::MyLuaData(0));	// is_cancel
	rvals.push_back(util::MyLuaData(""));	// id
	rvals.push_back(util::MyLuaData(""));	// password
	if (util::CallMZ3ScriptHookFunctions2("set_basic_auth_account", &rvals, 
			util::MyLuaData(theApp.m_accessTypeInfo.getSerializeKey(theApp.m_accessType))))
	{
		int is_cancel = rvals[0].m_number;
		if (is_cancel) {
			return;
		}
		strUser     = rvals[1].m_strText;
		strPassword = rvals[2].m_strText;
	}

	// �A�N�Z�X�J�n
	theApp.m_access = true;
	m_abort = FALSE;

	// �R���g���[����Ԃ̕ύX
	MyUpdateControlStatus();

	static MZ3Data s_data;
	MZ3Data data;
	s_data = data;
	s_data.SetAccessType(m_twitterPostAccessType);

	theApp.m_inet.Initialize( m_hWnd, &s_data, theApp.GetInetAccessEncodingByAccessType(m_twitterPostAccessType) );
	theApp.m_inet.DoPost(
		url, 
		L"", 
		CInetAccess::FILE_HTML, 
		&post, strUser, strPassword );
}

/**
 * �A�N�Z�X�I���ʒm��M (POST)
 */
LRESULT CMZ3View::OnPostEnd(WPARAM wParam, LPARAM lParam)
{
	if (m_abort) {
		::SendMessage(m_hWnd, WM_MZ3_GET_ABORT, NULL, lParam);
		return TRUE;
	}

	// �ʐM�����i�t���O��������j
	theApp.m_access = false;

	ACCESS_TYPE aType = theApp.m_accessType;
	switch (theApp.m_accessTypeInfo.getInfoType(aType)) {
	case AccessTypeInfo::INFO_TYPE_CATEGORY:
		// --------------------------------------------------
		// �J�e�S�����ڂ̎擾
		// --------------------------------------------------

		// ���O�ۑ�
		{
			CString strLogfilePath = util::MakeLogfilePath( *(CMixiData*)lParam );
			if(! strLogfilePath.IsEmpty() ) {
				// �ۑ��t�@�C���ɃR�s�[
				CopyFile( theApp.m_filepath.temphtml, strLogfilePath, FALSE/*bFailIfExists, �㏑��*/ );
			}
		}

		DoAccessEndProcForBody(aType);

		break;

	case AccessTypeInfo::INFO_TYPE_POST:
		// --------------------------------------------------
		// ���e����
		// --------------------------------------------------
		MZ3_TRACE(L"POST Status[%d]\n", theApp.m_inet.m_dwHttpStatus);

		{
			// MZ3 API : �C�x���g�n���h���֐��Ăяo��
			util::MyLuaDataList rvals;
			rvals.push_back(util::MyLuaData(0));
			CStringA serializeKey = CStringA(theApp.m_accessTypeInfo.getSerializeKey(aType));
			if (util::CallMZ3ScriptHookFunctions2("post_end", &rvals, 
					util::MyLuaData(serializeKey),
					util::MyLuaData(theApp.m_inet.m_dwHttpStatus),
					util::MyLuaData(CStringA(theApp.m_filepath.temphtml))))
			{
				// �C�x���g�n���h������
			} else {
				if (theApp.m_accessTypeInfo.getServiceType(aType) == "gooHome") {
					// HTTP�X�e�[�^�X�`�F�b�N���s���B
					if (theApp.m_inet.m_dwHttpStatus==200) {
						// OK

						// ���͒l������
						SetDlgItemText( IDC_STATUS_EDIT, L"" );
					} else {
						LPCTSTR szStatusErrorMessage = L"?";
						CString msg = util::FormatString(L"�T�[�o�G���[(%d)�F%s", theApp.m_inet.m_dwHttpStatus, szStatusErrorMessage);
						util::MySetInformationText( m_hWnd, msg );
						MZ3LOGGER_ERROR( msg );
					}
				} else {
					// Twitter �ȊO => ��ʕs��
				}

				// ��Lua�X�N���v�g���������܂�
			}
		}
		break;

	default:
		// �A�N�Z�X��ʕs��
		break;
	}

	// �J�e�S���ɉ�����Twitter���M���[�h��������
	MyResetTwitterStylePostMode();

	// �R���g���[����Ԃ̕ύX
	MyUpdateControlStatus();

	// �t�H�[�J�X�̕ύX
	MyUpdateFocus();

	// [MZ3-API] �ʐM�I��(UI����������)��̃t�b�N����
	switch (theApp.m_accessTypeInfo.getInfoType(aType)) {
	case AccessTypeInfo::INFO_TYPE_POST:
		switch (aType) {
		case ACCESS_TWITTER_UPDATE:
			// ���e��Ƀ^�C�����C�����擾����
			if (theApp.m_optionMng.m_bTwitterReloadTLAfterPost) {
				// http://twitter.com/statuses/friends_timeline.xml �ł���΍Ď擾����B
				// http://twitter.com/statuses/replies.xml ��
				// http://twitter.com/statuses/user_timeline/takke.xml �ł���΍Ď擾���Ȃ��B
				CCategoryItem* pCategory = m_selGroup->getSelectedCategory();
				if (pCategory!=NULL && wcsstr(pCategory->m_mixi.GetURL(), L"friends_timeline.xml")!=NULL) {
					RetrieveCategoryItem();
				}
			}
			break;
		}
	}

	return TRUE;
}

/**
 * URL
 */
void CMZ3View::OnLoadUrl(UINT nID)
{
	const CMixiData& data = GetSelectedBodyItem();

	UINT idx = nID - (ID_REPORT_URL_BASE+1);
	if( idx > data.m_linkList.size() ) {
		return;
	}

	LPCTSTR url  = data.m_linkList[idx].url;
	LPCTSTR text = data.m_linkList[idx].text;

	// �m�F���
	util::OpenUrlByBrowserWithConfirm( url );
}

void CMZ3View::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// �`�惁�b�Z�[�W�� CFormView::OnPaint() ���Ăяo���Ȃ��ł��������B

	switch (m_viewStyle) {
	case VIEW_STYLE_DEFAULT:
		break;
	case VIEW_STYLE_IMAGE:
	case VIEW_STYLE_TWITTER:
		// �A�C�R���`��
		if (m_rectIcon.Width()>0 && m_rectIcon.Height()>0) {

			bool bDrawFinished = false;

			CMixiData& data = GetSelectedBodyItem();
			CString path = util::MakeImageLogfilePath( data );
			if (!path.IsEmpty() ) {
				// ���̈�̍����ɕ`�悷��B
				const CRect& rectIcon = m_rectIcon;

				// �A�C�R��������΂�����ŕ`�悷��
				int iconIndex = theApp.m_imageCache.GetImageIndex( path );
				if (iconIndex >= 0) {

					CImageList* pImageList = NULL;
					int h = rectIcon.Height();
					int iconHeight = h;
					if (h>=64) {
						pImageList = &theApp.m_imageCache.GetImageList64();
						iconHeight = 64;
					} else if (h>=48) {
						pImageList = &theApp.m_imageCache.GetImageList48();
						iconHeight = 48;
					} else if (h>=32) {
						pImageList = &theApp.m_imageCache.GetImageList32();
						iconHeight = 32;
					} else {
						pImageList = &theApp.m_imageCache.GetImageList16();
						iconHeight = 16;
					}
					ImageList_DrawEx(
						pImageList->m_hImageList, iconIndex,
						dc.GetSafeHdc(),
						rectIcon.left, rectIcon.top, 0, 0,
						CLR_DEFAULT, CLR_DEFAULT,
						ILD_IMAGE);

					// �c�蕔����h��Ԃ�
					if (rectIcon.Height() > iconHeight) {
						CRect rect(m_rectIcon.left, m_rectIcon.top + iconHeight, m_rectIcon.right, m_rectIcon.bottom);
						dc.FillSolidRect(rect, theApp.m_skininfo.getColor("MainStatusBG"));
					}

					bDrawFinished = true;
				} else {
					// �A�C�R�����Ȃ��̂Ŏ��O�ŕ`�悷��
					CMZ3BackgroundImage image(L"");
					image.load( path );
					if (image.isEnableImage()) {
						// ���T�C�Y����

						// ���̃X�N�G�A�Ƃ���
						CMZ3BackgroundImage resizedImage(L"");
						util::MakeResizedImage( this, resizedImage, image, rectIcon.Width(), rectIcon.Width() );

						util::DrawBitmap( dc.GetSafeHdc(), resizedImage.getHandle(), 
							rectIcon.left, rectIcon.top, rectIcon.Width(), rectIcon.Width(), 0, 0 );

						// �c�蕔����h��Ԃ�
						if (rectIcon.Height() > rectIcon.Width()) {
							CRect rect(m_rectIcon.left, m_rectIcon.top + rectIcon.Width(), m_rectIcon.right, m_rectIcon.bottom);
							dc.FillSolidRect(rect, theApp.m_skininfo.getColor("MainStatusBG"));
						}

						bDrawFinished = true;
					}
				}
			}

			if (!bDrawFinished) {
				// �h��Ԃ�
				dc.FillSolidRect( m_rectIcon, theApp.m_skininfo.getColor("MainStatusBG"));
			}
		}
		break;
	}
}

bool CMZ3View::AppendCategoryList(const CCategoryItem& categoryItem)
{
	CString url = categoryItem.m_mixi.GetURL();

	// �O���[�v(�^�u)�Ƀ^�C�����C����p���ڂ�ǉ�����B���ɂ���Ύ擾����B
	CCategoryItem* pCategoryItem = NULL;
	for( u_int i=0; i<m_selGroup->categories.size(); i++ ) {
		CCategoryItem& category = m_selGroup->categories[i];
		if( category.m_mixi.GetAccessType() == ACCESS_TWITTER_FRIENDS_TIMELINE &&
			category.m_mixi.GetURL()==url)
		{
			// �Y�����ڔ����B
			pCategoryItem = &category;
			break;
		}
	}
	// �������Ȃ�ǉ�
	if( pCategoryItem == NULL ) {
		m_selGroup->categories.push_back( categoryItem );
		pCategoryItem = &m_selGroup->categories[ m_selGroup->categories.size()-1 ];
		pCategoryItem->SetIndexOnList( m_selGroup->categories.size()-1 );
	}

//	*pCategoryItem = categoryItem;

	// �^�u�̏�����
	MyUpdateCategoryListByGroupItem();

	// �J�e�S���̑I�����ڂ��ĕ\���B
	{
		int idxLast = m_selGroup->focusedCategory;
		int idxNew  = pCategoryItem->GetIndexOnList();

		util::MySetListCtrlItemFocusedAndSelected( m_categoryList, idxLast, false );
		util::MySetListCtrlItemFocusedAndSelected( m_categoryList, idxNew, true );
		m_selGroup->focusedCategory  = idxNew;
		m_selGroup->selectedCategory = idxNew;
		m_categoryList.SetActiveItem( idxNew );
		m_categoryList.Update( idxLast );
		m_categoryList.Update( idxNew );

		m_categoryList.EnsureVisible( idxNew, FALSE );
	}

	// �t�H�[�J�X���J�e�S�����X�g�ɁB
	m_categoryList.SetFocus();

	// �{�f�B���X�g�͏������Ă����B
	m_bodyList.DeleteAllItems();
	m_bodyList.SetRedraw(TRUE);
	// �o�b�N�o�b�t�@�o�R�ōĕ`��
	m_bodyList.DrawDetail();
	m_bodyList.UpdateWindow();

	// �O���[�v��`�t�@�C���̕ۑ�
	theApp.SaveGroupData();

	return true;
}

/**
 * �^�u�̉E�N���b�N���j���[
 */
void CMZ3View::OnNMRclickGroupTab(NMHDR *pNMHDR, LRESULT *pResult)
{
	PopupTabMenu();

	*pResult = 0;
}

/**
 * �^�u���j���[�b�폜
 */
void CMZ3View::OnTabmenuDelete()
{
	if (m_selGroup->bSaveToGroupFile) {
		m_selGroup->bSaveToGroupFile = false;

		MessageBox( util::FormatString( L"[%s] �^�u�͎���N�����ɍ폜����܂�", (LPCTSTR)m_selGroup->name ) );
	} else {
		m_selGroup->bSaveToGroupFile = true;
	}

	// �O���[�v��`�t�@�C���̕ۑ�
	theApp.SaveGroupData();
}

/**
 * �^�u���X�g�̃|�b�v�A�b�v���j���[
 */
bool CMZ3View::PopupTabMenu(POINT pt_, int flags_)
{
	POINT pt    = pt_;
	int   flags = flags_;

	if (pt.x==0 && pt.y==0) {
		pt = util::GetPopupPos();
		flags = util::GetPopupFlags();
	}

	CMenu menu;
	menu.LoadMenu(IDR_TAB_MENU);
	CMenu* pSubMenu = menu.GetSubMenu(0);

	// �폜�̗L���E����
	pSubMenu->CheckMenuItem( ID_TABMENU_DELETE, MF_BYCOMMAND | (m_selGroup->bSaveToGroupFile ? MF_UNCHECKED : MF_CHECKED) );

	// ���[�Ȃ�u���Ɉړ��v�𖳌���
	if (m_groupTab.GetCurSel()==0) {
		pSubMenu->EnableMenuItem(ID_TABMENU_MOVE_TO_LEFT, MF_GRAYED | MF_BYCOMMAND);
	}

	// �E�[�Ȃ�u�E�Ɉړ��v�𖳌���
	if (m_groupTab.GetCurSel()==m_groupTab.GetItemCount()-1) {
		pSubMenu->EnableMenuItem(ID_TABMENU_MOVE_TO_RIGHT, MF_GRAYED | MF_BYCOMMAND);
	}

	// ���j���[�\��
	pSubMenu->TrackPopupMenu(flags, pt.x, pt.y, this);

	return true;
}

/**
 * �J�e�S���̒ǉ�
 */
void CMZ3View::OnAppendCategoryMenu(UINT nID)
{
	int idx = nID - ID_APPEND_MENU_BEGIN;

	// �t���e���v���[�g����
	Mz3GroupData template_data;
	template_data.initForTopPage(theApp.m_accessTypeInfo, Mz3GroupData::InitializeType());

	int idxCounter = 0;
	for (unsigned int groupIdx=0; groupIdx<template_data.groups.size(); groupIdx++) {
		CGroupItem& group = template_data.groups[groupIdx];

		for (unsigned int ic=0; ic<group.categories.size(); ic++) {
			if (idxCounter==idx) {
				// ���̍��ڂ�ǉ�����
				CCategoryItem item = group.categories[ic];
				AppendCategoryList( item );

				// �J�e�S�����X�g���́u���ݑI������Ă��鍀�ځv���X�V
				m_hotList = &m_categoryList;
				m_selGroup->selectedCategory = m_selGroup->categories.size()-1;
				OnMySelchangedCategoryList();

				return;
			}

			idxCounter ++;
		}
	}
}

/**
 * �J�e�S�����j���[�b���ڂ��폜
 */
void CMZ3View::OnRemoveCategoryItem()
{
	CCategoryItem* pCategoryItem = m_selGroup->getFocusedCategory();

/*	if (pCategoryItem->bSaveToGroupFile) {
		pCategoryItem->bSaveToGroupFile = false;

		MessageBox( util::FormatString( L"[%s] �͎���N�����ɍ폜����܂�", (LPCTSTR)pCategoryItem->m_name ) );
	} else {
		pCategoryItem->bSaveToGroupFile = true;
	}
*/

	// 1���ȉ��Ȃ�폜�s��
	if (m_selGroup->categories.size()<=1) {
		MessageBox(L"���̃^�u�̃J�e�S����1���Ȃ̂ō폜�ł��܂���");
		return;
	}

	// �Y�����ڂ��폜����
	m_selGroup->categories.erase(m_selGroup->categories.begin() + m_selGroup->focusedCategory);

	// �������폜�����ꍇ�͑I����1�O�ɂ���
	// ��)
	// ���̃T�C�Y            :5
	// �폜��̃T�C�Y        :4
	// �I�����ڂ̃C���f�b�N�X:4
	if (m_selGroup->focusedCategory==m_selGroup->categories.size()) {
		// 1�O��\������
		m_selGroup->focusedCategory --;
		m_selGroup->selectedCategory = m_selGroup->focusedCategory;
		theApp.m_optionMng.m_lastTopPageCategoryIndex = m_selGroup->focusedCategory;
	}

	// �J�e�S���[���X�g������������
	MyUpdateCategoryListByGroupItem();

	// �C���f�b�N�X�ւ̔��f(�č\�z)
	for (size_t i=0; i<m_selGroup->categories.size(); i++) {
		m_selGroup->categories[i].SetIndexOnList(i);
	}

	// �J�e�S�����X�g���́u���ݑI������Ă��鍀�ځv���X�V
	OnMySelchangedCategoryList();

	// �O���[�v��`�t�@�C���̕ۑ�
	theApp.SaveGroupData();
}

/**
 * �J�e�S�����j���[�b�ύX
 */
void CMZ3View::OnEditCategoryItem()
{
	CCategoryItem* pCategoryItem = m_selGroup->getFocusedCategory();
	
	CCommonEditDlg dlg;
	dlg.SetTitle( L"�J�e�S���̃^�C�g���ύX" );
	dlg.SetMessage( L"�J�e�S���̃^�C�g��/URL����͂��Ă�������" );
	dlg.mc_strEdit  = pCategoryItem->m_name;
	dlg.mc_strEdit2 = pCategoryItem->m_mixi.GetURL();
	if (dlg.DoModal()==IDOK) {
		pCategoryItem->m_name = dlg.mc_strEdit;
		pCategoryItem->m_mixi.SetURL( dlg.mc_strEdit2 );
		m_categoryList.SetItemText( m_selGroup->focusedCategory, 0, pCategoryItem->m_name );

		// �O���[�v��`�t�@�C���̕ۑ�
		theApp.SaveGroupData();
	}
}

/**
 * �^�u���j���[�b�ύX
 */
void CMZ3View::OnTabmenuEdit()
{
	CGroupItem* pGroupItem = m_selGroup;
	
	CCommonEditDlg dlg;
	dlg.SetTitle( L"�^�u�̃^�C�g���ύX" );
	dlg.SetMessage( L"�^�u�̃^�C�g������͂��Ă�������" );
	dlg.mc_strEdit = pGroupItem->name;
	if (dlg.DoModal()==IDOK) {
		pGroupItem->name = dlg.mc_strEdit;

		TCITEM tcItem;
		TCHAR buffer[256] = {0};
		tcItem.pszText = buffer;
		tcItem.cchTextMax = 256;
		tcItem.mask = TCIF_TEXT;
		wcsncpy( tcItem.pszText, pGroupItem->name, 255 );
		m_groupTab.SetItem(m_groupTab.GetCurSel(), &tcItem);

		// �O���[�v��`�t�@�C���̕ۑ�
		theApp.SaveGroupData();
	}
}

/**
 * �^�u���j���[�b�ǉ�
 */
void CMZ3View::OnTabmenuAdd()
{
	CCommonEditDlg dlg;
	dlg.SetTitle( L"�^�u�̒ǉ�" );
	dlg.SetMessage( L"�^�u�̃^�C�g������͂��Ă�������" );
	dlg.mc_strEdit = L"�V�����^�u";
	if (dlg.DoModal()==IDOK) {
		// �f�[�^�\���ǉ�
		CGroupItem group;
		group.init( dlg.mc_strEdit, L"", ACCESS_GROUP_GENERAL );
		theApp.m_root.groups.push_back( group );

		// �O���[�v�^�u�ɒǉ�
		int tabIndex = m_groupTab.GetItemCount();
		m_groupTab.InsertItem( tabIndex, dlg.mc_strEdit);

		// �I��ύX
		m_groupTab.SetCurSel( tabIndex );

		// �I�𒆂̃O���[�v���ڂ̐ݒ�
		m_selGroup = &theApp.m_root.groups[tabIndex];

		// �J�e�S���[���X�g������������
		OnSelchangedGroupTab();

		// �O���[�v��`�t�@�C���̕ۑ�
		theApp.SaveGroupData();
	}
}

/**
 * ��Ԃɉ������R���g���[����Ԃ̕ύX
 */
void CMZ3View::MyUpdateControlStatus(void)
{
	// ���~�{�^��
	theApp.EnableCommandBarButton( ID_STOP_BUTTON, theApp.m_access ? TRUE : FALSE );

	// �^�u�E�J�e�S���̖�����
	m_groupTab.EnableWindow( theApp.m_access ? FALSE : TRUE );
//	m_categoryList.EnableWindow( theApp.m_access ? FALSE : TRUE );

	// �v���O���X�o�[
	if (theApp.m_access) {
		// �ʐM���͎�M���Ɏ����\��
	} else {
		mc_progressBar.ShowWindow( SW_HIDE );
	}

	// Twitter �̍X�V�{�^��
	CWnd* pUpdateButton = GetDlgItem( IDC_UPDATE_BUTTON );
	if (pUpdateButton!=NULL) {
		pUpdateButton->EnableWindow( theApp.m_access ? FALSE : TRUE );
	}

	// Twitter �̓��͗̈�
	CWnd* pStatusEdit = GetDlgItem( IDC_STATUS_EDIT );
	if (pStatusEdit!=NULL) {
		pStatusEdit->EnableWindow( theApp.m_access ? FALSE : TRUE );
	}

	// Twitter�X�^�C���Ȃ�{�^���̖��̂����[�h�ɂ��ύX����
	switch (m_viewStyle) {
	case VIEW_STYLE_TWITTER:
		if (pUpdateButton!=NULL) {
			// MZ3 API : �C�x���g�n���h���֐��Ăяo��
			util::MyLuaDataList rvals;
			rvals.push_back(util::MyLuaData(""));
			CStringA serializeKey = CStringA(theApp.m_accessTypeInfo.getSerializeKey(m_twitterPostAccessType));
			if (util::CallMZ3ScriptHookFunctions2("update_twitter_update_button", &rvals, 
					util::MyLuaData(serializeKey)))
			{
				// �C�x���g�n���h������
				pUpdateButton->SetWindowTextW( CString(rvals[0].m_strText) );
				break;
			}
		}
		break;
	}
}

/**
 * �E�{�^������
 */
void CMZ3View::OnRButtonDown(UINT nFlags, CPoint point)
{
//	MZ3_TRACE( L"CMZ3View::OnRButtonDown\n" );

	// �W�F�X�`���J�n
	theApp.m_pMouseGestureManager->StartGestureMode(point);

	// �}�E�X�L���v�`���J�n
//	SetCapture();

	CFormView::OnRButtonDown(nFlags, point);
}

/**
 * �E�{�^�������[�X
 */
void CMZ3View::OnRButtonUp(UINT nFlags, CPoint point)
{
	MZ3_TRACE( L"CMZ3View::OnRButtonUp\n" );

	// �}�E�X�L���v�`���I��
//	ReleaseCapture();

#ifndef WINCE
	// MZ4 Only

	// �F���ς݂̃W�F�X�`���Ɣ�r
	CString strCmdList = CStringW(theApp.m_pMouseGestureManager->GetCmdList().c_str());
	MZ3_TRACE(L" gesture : %s\n", (LPCTSTR)strCmdList);

	if (strCmdList==L"R") {
		// �E�F���̃^�u
		OnAcceleratorNextTab();
	} else if (strCmdList==L"L") {
		// ���F�O�̃^�u
		OnAcceleratorPrevTab();
	} else if (strCmdList==L"D") {
		// ���F�J�����ύX(�b��)
		MyChangeBodyHeader();
	} else if (strCmdList==L"U") {
		// ��F�����[�h
		OnAcceleratorReload();
	} else {
		// ���b�Z�[�W�N���A
		m_infoEdit.SetWindowText( L"" );
//		util::MyGetItemByBodyColType(&GetSelectedBodyItem(), m_selGroup->getSelectedCategory()->m_bodyColType1, false)
	}
#endif

	// �W�F�X�`���I��
	theApp.m_pMouseGestureManager->StopGestureMode();

	CFormView::OnRButtonUp(nFlags, point);
}

/**
 * ���{�^������
 */
void CMZ3View::OnLButtonDown(UINT nFlags, CPoint point)
{
#ifndef WINCE
	{
		CRect r;
		m_categoryList.GetWindowRect(&r);

		CPoint pt;
		GetCursorPos(&pt);

		int dy = pt.y-r.bottom;
		if (abs(dy)<SPLITTER_HEIGHT) {
			MZ3LOGGER_INFO(L"�h���b�O�J�n");

			// �J�[�\���ύX
			SetCursor(LoadCursor(NULL, IDC_SIZENS));

			// �h���b�O�J�n
			m_bDragging = true;
			m_ptDragStart = pt;
			m_nOriginalH1 = theApp.m_optionMng.m_nMainViewCategoryListHeightRatio;
			m_nOriginalH2 = theApp.m_optionMng.m_nMainViewBodyListHeightRatio;

			// �q�E�B���h�E�̃h���b�O�����~
			m_categoryList.m_bStopDragging = true;
			m_bodyList.m_bStopDragging = true;

			// �L���v�`���J�n
			SetCapture();

			return;
		}
	}
#endif

	CFormView::OnLButtonDown(nFlags, point);
}

void CMZ3View::ReflectSplitterLineMove()
{
	// �X�v���b�^�[�ύX
	int h1 = m_nOriginalH1;
	int h2 = m_nOriginalH2;

	// �I�v�V�����l�� % �ɕ␳
	int sum = h1 + h2;
	if (sum>0) {
		h1 = (int)(h1 * 100.0 / sum);
		h2 = (int)(h2 * 100.0 / sum);
		sum = h1 + h2;
	}

	CRect rw;
	GetWindowRect(&rw);
	int cy = rw.Height();

	CPoint pt;
	GetCursorPos(&pt);

	int dy = pt.y - m_ptDragStart.y +3;

	int fontHeight = theApp.m_optionMng.GetFontHeightByPixel(theApp.GetDPI());
	int hGroup    = theApp.GetTabHeight(fontHeight);
	int hCategory = (cy * h1 / sum) - (hGroup -1);
	int mh1 = (int)(sum/(double)cy * (hCategory + dy +hGroup -1) - h1);
	MZ3LOGGER_DEBUG(util::FormatString(L"mh1 : %d  dy : %d", mh1, dy));

	h1 += mh1;
	h2 -= mh1;

	// ���K��
	if (h1 < N_HC_MIN || h2 > N_HB_MAX) {
		// �ŏ��l�ɐݒ�
		h1 = N_HC_MIN;
		h2 = N_HB_MAX;
	}
	if (h1 > N_HC_MAX || h2 < N_HB_MIN) {
		// �ŏ��l�ɐݒ�
		h1 = N_HC_MAX;
		h2 = N_HB_MIN;
	}

	// �I�v�V�����ɔ��f
	theApp.m_optionMng.m_nMainViewCategoryListHeightRatio = h1;
	theApp.m_optionMng.m_nMainViewBodyListHeightRatio = h2;

	// �ĕ`��
	CMainFrame* pMainFrame = (CMainFrame*)theApp.m_pMainWnd;
	pMainFrame->ChangeAllViewFont();
}


/**
 * ���{�^�������[�X
 */
void CMZ3View::OnLButtonUp(UINT nFlags, CPoint point)
{
#ifndef WINCE
	if (m_bDragging) {
		// �ړ����f
		ReflectSplitterLineMove();

		// �h���b�O�I������
		MZ3LOGGER_INFO(L"�h���b�O�I��");

		// �L���v�`���I��
		ReleaseCapture();

		// �}�E�X�J�[�\�������ɖ߂�
		SetCursor(LoadCursor(NULL, IDC_ARROW));

		// �t���O�N���A
		m_bDragging = false;

		// �q�E�B���h�E�̃h���b�O����ĊJ
		m_categoryList.m_bStopDragging = false;
		m_bodyList.m_bStopDragging = false;
	}
#endif

	CFormView::OnLButtonUp(nFlags, point);
}

/**
 * �}�E�X�ړ�
 */
void CMZ3View::OnMouseMove(UINT nFlags, CPoint point)
{
//	MZ3_TRACE( L"CMZ3View::OnMouseMove\n" );

#ifndef WINCE
	// MZ4 Only

	// �X�v���b�^
	{
		CRect r;
		m_categoryList.GetWindowRect(&r);

		CPoint pt;
		GetCursorPos(&pt);

		// �J�e�S�����X�g�̉����t�߂ŗL��
		int dy = pt.y-r.bottom;
		if (m_bDragging || (dy < 0 && dy>-SPLITTER_HEIGHT)) {
			SetCursor(LoadCursor(NULL, IDC_SIZENS));
		}

		if (m_bDragging) {
			// ������Ƃ����f�B���C������
			static DWORD s_dwLastMove = GetTickCount();
			DWORD dwNow = GetTickCount();
			if (dwNow > s_dwLastMove +10) {
				s_dwLastMove = dwNow;

				// �X�v���b�^�[�ύX
				ReflectSplitterLineMove();

				// �ĕ`��
				m_categoryList.UpdateWindow();
			}
		}
	}

	// �W�F�X�`��
	if (theApp.m_pMouseGestureManager->IsGestureMode()) {

		// 臒l�ȏ�ړ����Ă���Ώ������s
		const CPoint ptLastCmd = theApp.m_pMouseGestureManager->m_posLastCmd;

		int dx = point.x - ptLastCmd.x;
		int dy = point.y - ptLastCmd.y;

//		MZ3_TRACE( L" dx[%3d], dy[%3d]\n", dx, dy );

		// 臒l
		const int GESTURE_LIMIT_X = 20;
		const int GESTURE_LIMIT_Y = 20;

		MouseGestureManager::CMD cmd = MouseGestureManager::CMD_NONE;
		MouseGestureManager::CMD lastCmd = theApp.m_pMouseGestureManager->GetLastCmd();

		if (dx >= GESTURE_LIMIT_X) {
			// �E�ړ� : 1�O�Ɠ����łȂ���ΔF��
			if (lastCmd != MouseGestureManager::CMD_RIGHT) {
				cmd = MouseGestureManager::CMD_RIGHT;
			}
		} else if (dx <= -GESTURE_LIMIT_X) {
			// ���ړ� : 1�O�Ɠ����łȂ���ΔF��
			if (lastCmd != MouseGestureManager::CMD_LEFT) {
				cmd = MouseGestureManager::CMD_LEFT;
			}
		}
		
		if (dy >= GESTURE_LIMIT_Y) {
			// ���ړ� : 1�O�Ɠ����łȂ���ΔF��
			if (lastCmd != MouseGestureManager::CMD_DOWN) {
				cmd = MouseGestureManager::CMD_DOWN;
			}
		} else if (dy <= -GESTURE_LIMIT_Y) {
			// ��ړ� : 1�O�Ɠ����łȂ���ΔF��
			if (lastCmd != MouseGestureManager::CMD_UP) {
				cmd = MouseGestureManager::CMD_UP;
			}
		}

		// 1�O�Ɠ����łȂ���Γo�^
		if (cmd != MouseGestureManager::CMD_NONE &&	lastCmd!=cmd) {

			theApp.m_pMouseGestureManager->m_cmdList.push_back(cmd);
			theApp.m_pMouseGestureManager->m_posLastCmd = point;

			// �X�e�[�^�X�o�[�ɕ\��
			CString strCmdList = CStringW(theApp.m_pMouseGestureManager->GetCmdList().c_str());
			strCmdList.Replace(L"U", L"��");
			strCmdList.Replace(L"D", L"��");
			strCmdList.Replace(L"L", L"��");
			strCmdList.Replace(L"R", L"��");
			util::MySetInformationText( m_hWnd, strCmdList );
		}
	}
#endif
}

/**
 * �{�f�B�n���ڂ̎擾��������
 *
 * @return bool �㑱�̏������s��Ȃ��ꍇ�� true ��Ԃ��B
 */
bool CMZ3View::DoAccessEndProcForBody(ACCESS_TYPE aType)
{
	MZ3_TRACE(L"DoAccessEndProcForBody(), HTTP Status[%d], [%s]\n", 
		theApp.m_inet.m_dwHttpStatus,
		(LPCTSTR)CString(theApp.m_accessTypeInfo.getServiceType(aType).c_str()));

	// MZ3 API : �C�x���g�n���h���֐��Ăяo��
	util::MyLuaDataList rvals;
	rvals.push_back(util::MyLuaData(0));
	CStringA serializeKey = CStringA(theApp.m_accessTypeInfo.getSerializeKey(aType));
	if (util::CallMZ3ScriptHookFunctions2("get_end", &rvals, 
			util::MyLuaData(serializeKey),
			util::MyLuaData(theApp.m_inet.m_dwHttpStatus)))
	{
		// �C�x���g�n���h������
		return false;
	}

	// �X�e�[�^�X�R�[�h�`�F�b�N
	LPCTSTR szStatusErrorMessage = NULL;	// ��NULL�̏ꍇ�̓G���[����
	if (theApp.m_accessTypeInfo.getServiceType(aType)=="Wassr") {
		// Wassr
		if (theApp.m_inet.m_dwHttpStatus==200) {
			// OK
		} else {
			szStatusErrorMessage = L"�s���ȃG���[";
		}
	}
	if (szStatusErrorMessage!=NULL) {
		CString msg = util::FormatString(L"�T�[�o�G���[(%d)�F%s", theApp.m_inet.m_dwHttpStatus, szStatusErrorMessage);
		util::MySetInformationText( m_hWnd, msg );
		MZ3LOGGER_ERROR( msg );
		// �ȍ~�̏������s��Ȃ��B
		return false;
	}

	util::MySetInformationText( m_hWnd, _T("��͒� : 1/3") );

	// ���񃂁[�h�i���X�g���[�h�j�̏ꍇ�́A���񃂁[�h���I������B
	if( m_cruise.enable() && !m_cruise.isFetchListMode() ) {
		// ���X�g���[�h�ȊO�Ȃ̂ŒʐM���I������
		// �Ⴆ�΃{�f�B���[�h�̍Ō�̗v�f���u���ցv�̂悤�ȏꍇ�ɂ͂����ɗ���B
		if( m_cruise.autoCruise ) {
			// �\�񏄉�Ȃ̂Ŏ��ɐi��
			m_cruise.targetCategoryIndex++;
			CruiseToNextCategory();
		}else{
			// �ꎞ����Ȃ̂ł����ŏI���B
			m_cruise.finish();
			MessageBox( L"���񊮗�" );
		}
		return false;
	}

	// HTML �̎擾
	CHtmlArray html;
	html.Load( theApp.m_filepath.temphtml );

	// �ۑ��� body �̎擾�B
	CMixiData& parent_data = m_selGroup->getSelectedCategory()->m_mixi;
	CMixiDataList& body = m_selGroup->getSelectedCategory()->GetBodyList();

	// HTML ���
	util::MySetInformationText( m_hWnd,  _T("��͒� : 2/3") );
	bool bParserResult = parser::MyDoParseMixiListHtml( aType, parent_data, body, html );

	// �{�f�B�ꗗ�̐ݒ�
	util::MySetInformationText( m_hWnd,  _T("��͒� : 3/3") );

	// �擾����������̍쐬
	SYSTEMTIME localTime;
	GetLocalTime(&localTime);
	m_selGroup->getSelectedCategory()->m_bFromLog = false;
	m_selGroup->getSelectedCategory()->SetAccessTime( localTime );
	CString timeStr = m_selGroup->getSelectedCategory()->GetAccessTimeString();
	m_categoryList.SetItemText( m_selGroup->selectedCategory, 1, timeStr );
	SetBodyList( body );		// �{�f�B�ꗗ�ɕ\��

	if( aType == ACCESS_LIST_BBS ) {
		// �R�~���j�e�B���X�g�̏ꍇ�͎����I�Ƀ{�f�B�ꗗ�Ƀt�H�[�J�X����
		m_bodyList.SetFocus();
	}

	if (bParserResult) {
		// [MZ3-API] : �p�[�X��̃t�b�N����
		util::MyLuaDataList rvals;
		rvals.push_back(util::MyLuaData(0));
		CStringA serializeKey = CStringA(theApp.m_accessTypeInfo.getSerializeKey(aType));
		if (util::CallMZ3ScriptHookFunctions2("after_get_end", &rvals, 
				util::MyLuaData(serializeKey)))
		{
			// �C�x���g�n���h������
		} else {
			switch (aType) {
			case ACCESS_LIST_FRIEND:
	//			MZ3_TRACE(L"��ACCESS_LIST_FRIEND\n");
				// �}�C�~�N�ꗗ
				// list_friend.pl �ł���΁Aajax_friend_setting.pl �ɕύX���čă��N�G�X�g
				if (wcsstr(theApp.m_inet.GetURL(), L"list_friend.pl")!=NULL) {
					// �ă��N�G�X�g
					CCategoryItem* pCategoryItem = m_selGroup->getSelectedCategory();
					AccessProc( &pCategoryItem->m_mixi, util::CreateMixiUrl(pCategoryItem->m_mixi.GetURL()));
					return false;
				} else {
					// �擾���������̂� post_key �����������Ă����B
					CMixiDataList& body = m_selGroup->getSelectedCategory()->m_body;
					if (body.size()>0) {
	//					MZ3_TRACE(L"  ��post_key, reset\n");
						body[0].SetTextValue(L"post_key", L"");
					}
				}
				break;

			case ACCESS_TWITTER_FRIENDS_TIMELINE:
				MZ3_TRACE(L"��ACCESS_TWITTER_FRIENDS_TIMELINE\n");
				// Twitter �^�C�����C���A�y�[�W�ύX(�����y�[�W�擾)����
				if (theApp.m_access) {
					// �摜�擾���̉\������B�L�����Z��
					break;
				} else {
					CCategoryItem* pCategoryItem = m_selGroup->getSelectedCategory();
					int page = pCategoryItem->m_mixi.GetIntValue(L"request_page", 1);
					MZ3_TRACE(L"�@��page=%d\n", page);

					// �V���������擾
					int new_count = parent_data.GetIntValue(L"new_count", 0);
					// ���O�t�@�C������擾���Ă����f�[�^�͐V���Ƃ݂Ȃ�(���O���O�̃f�[�^���擾���邽��)
					if (page==1) {
						size_t body_size = body.size();
						for (size_t i=0; i<body_size; i++) {
							if (body[i].GetIntValue(L"from_log_flag", 0)) {
								new_count ++;
								// ����̓J�E���g���Ȃ����߃t���O�������Ƃ�
								body[i].SetIntValue(L"from_log_flag", 0);
							}
						}
					}

					MZ3_TRACE(L"�@���V������=%d\n", new_count);

					// �ő�y�[�W�������ŁA���A�V��������臒l���������ꍇ
					if (page<theApp.m_optionMng.m_nTwitterGetPageCount && new_count >= 20/2) {
	//				if (page<50 && new_count >= 20/2) {
						// ���y�[�W���N�G�X�g
						page ++;
						pCategoryItem->m_mixi.SetIntValue(L"request_page", page);
						CString url = util::CreateMixiUrl(pCategoryItem->m_mixi.GetURL());
						url.AppendFormat(L"%spage=%d", (url.Find('?')<0 ? L"?" : L"&"), page);

						static MZ3Data s_data;
						s_data = pCategoryItem->m_mixi;
						s_data.SetURL(url);
						s_data.SetBrowseUri(url);
						theApp.m_inet.CloseInternetHandles();
						AccessProc(&s_data, url);
						return false;
					} else {
						// ���N�G�X�g�y�[�W�ϐ������������ďI��
						pCategoryItem->m_mixi.SetIntValue(L"request_page", 1);
					}
				}
				break;
			}
		}
	}

	// ���񃂁[�h�Ȃ�A�{�f�B�v�f�̎擾���J�n����
	if( m_cruise.enable() ) {
		// ���񃂁[�h

		// ���X�g���[�h�Ȃ̂Ń{�f�B���[�h�ɕύX���A
		// �{�f�B�̎擾���J�n����B
		m_cruise.startBodyCruise();

		// �{�f�B�̍ŏ��̗v�f���擾����

		// �{�f�B���X�g�Ƀt�H�[�J�X���ړ�����
		CommandSetFocusBodyList();

		// �I������
		util::MySetListCtrlItemFocusedAndSelected( m_bodyList, m_selGroup->getSelectedCategory()->selectedBody,  false );

		// ���̏��񍀖ڂցB
		if( DoNextBodyItemCruise() ) {
			// �ʐM�p���̂��߂����� return ����
			return true;
		}
	}

	return false;
}

/**
 * �r���[�̏�Ԃɉ����ăt�H�[�J�X��W���ʒu�ɖ߂�
 *
 * ��ɒʐM�������̕��A�����ɗp����
 */
void CMZ3View::MyUpdateFocus(void)
{
	CWnd* pFocus = GetFocus();

	switch (m_viewStyle) {
	case VIEW_STYLE_TWITTER:
		{
			// Twitter�֘A��POST�����������̂ŁA�t�H�[�J�X����͗̈�Ɉړ�����B
			// �A���A�t�H�[�J�X�����X�g�ɂ���ꍇ�͈ړ����Ȃ��B
			if (pFocus == NULL ||
				(pFocus->m_hWnd != m_categoryList.m_hWnd &&
				 pFocus->m_hWnd != m_bodyList.m_hWnd))
			{
				GetDlgItem( IDC_STATUS_EDIT )->SetFocus();
			}
		}
		break;

	default:
		break;
	}
}

/**
 * �C�ӂ�URL���J��
 */
void CMZ3View::MyOpenUrl(void)
{
	COpenUrlDlg dlg;
	if (dlg.DoModal() == IDOK) {

		CInetAccess::ENCODING encoding = (CInetAccess::ENCODING)dlg.m_encoding;

		// �A�N�Z�X��ʂ̑I��
		CChooseAccessTypeDlg dlg1;
		if (dlg1.DoModal() == IDOK) {
			static CMixiData s_mixi;
			s_mixi.SetAccessType( dlg1.m_selectedAccessType );

			// URL �ݒ�
			s_mixi.SetURL( dlg.mc_strUrl );
			s_mixi.SetBrowseUri( dlg.mc_strUrl );

			// �ʐM�J�n
			AccessProc( &s_mixi, s_mixi.GetURL(), encoding );
		}
	}
}

/**
 * �C�ӂ̃��[�J���t�@�C�����J��
 */
void CMZ3View::MyOpenLocalFile(void)
{
	WCHAR szFile[MAX_PATH] = L"\0";

	OPENFILENAME ofn;
	memset( &(ofn), 0, sizeof(ofn) );
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = m_hWnd;
	ofn.lpstrFile = szFile;
	ofn.nMaxFile = MAX_PATH; 
	ofn.lpstrTitle = L"HTMĻ�ق��J��...";
	ofn.lpstrFilter = L"HTMĻ�� (*.htm;*.html)\0*.htm;*.html\0\0";
	ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST;
	ofn.lpstrInitialDir = L"";

	if (GetOpenFileName(&ofn) == IDOK) {
		CString strLogfilePath = szFile;

		// �A�N�Z�X��ʂ̑I��
		CChooseAccessTypeDlg dlg;
		if (dlg.DoModal() == IDOK) {
			// ���
			static CMixiData s_mixi;
			s_mixi.SetAccessType( dlg.m_selectedAccessType );
			MyParseMixiHtml( strLogfilePath, s_mixi );

			// URL �ݒ�
			s_mixi.SetBrowseUri( util::CreateMixiUrl(s_mixi.GetURL()) );

			// �\��
			MyShowReportView( s_mixi );
		}
	}
}

/**
 * RSS | �S����ǂ�
 */
void CMZ3View::OnMenuRssRead()
{
	CMixiData& data = GetSelectedBodyItem();

	CString item;

	// �^�C�g��
	item = data.GetTitle();
	item.Append( L"\r\n" );

	// �{��
	CString body = data.GetBody();
	while( body.Replace( L"\r\n", L"" ) );
	item += body;
	item.Append( L"\r\n" );

	// ���t������Βǉ�
	if (!data.GetDate().IsEmpty()) {
		item.Append( L"----\r\n" );
		item.AppendFormat( L"%s\r\n", data.GetDate() );
	}

	// URL�ǉ�
	int n = (int)data.m_linkList.size();
	if( n > 0 ) {
		for( int i=0; i<n; i++ ) {
			// �ǉ�
			item.AppendFormat( L"URL : %s", data.m_linkList[i].url );
		}
	}

	MessageBox( item, data.GetName() );
}

/*
 * OnCategoryOpen()
 * WM_COMMAND IDM_CATEBORY_OPEN
 * �J�e�S���̃R���e�L�X�g���j���[�u�ŐV�̈ꗗ���擾����v
*/
void CMZ3View::OnCategoryOpen()
{
	if (theApp.m_access) {
		// �A�N�Z�X���͍ăA�N�Z�X�s��
		return;
	}

	int iItem = m_selGroup->selectedCategory;
	m_hotList = &m_categoryList;

	if (iItem<0) {
		return;
	}

	// �J�����g�f�[�^���擾
	int idx = (int)m_categoryList.GetItemData(iItem);
	m_selGroup->focusedCategory = idx;
	if (m_preCategory != iItem) {
		m_categoryList.SetActiveItem(iItem);
		m_categoryList.Update(m_preCategory);
		m_categoryList.Update(iItem);
	}

	// �A�N�Z�X�J�n
	if (!RetrieveCategoryItem()) {
		return;
	}
}

/**
 * RSS�t�B�[�h�̒ǉ�
 */
void CMZ3View::OnAddRssFeedMenu()
{
	/*
	 * 1. ���[�U�ɁuRSS�I��URL�v����͂�����B
	 * 2. 1. ��URL���擾����B
	 * 3. �擾����URL��RSS�t�B�[�h�ł���΂��̂܂ܒǉ����ďI���B�^�C�g���������t�^���邱�ƁB
	 * 4. HTML�y�[�W�ł���΁ARSS AutoDiscovery �����{����B
	 * 5. 4. ����������΃t�B�[�h��ǉ����ďI���Btitle �^�O����^�C�g���������t�^���邱�ƁB
	 * 6. 4. �����s����΃G���[�o�͂��ďI���B
	 */
	if (theApp.m_access) {
		// �A�N�Z�X���͍ăA�N�Z�X�s��
		return;
	}

	CCommonEditDlg dlg;
	dlg.SetTitle( L"RSS�t�B�[�h�̒ǉ�" );
	dlg.SetMessage( L"RSS��URL����͂��Ă�������" );
	dlg.mc_strEdit  = L"http://";
	if (dlg.DoModal()==IDOK) {
		CString url = dlg.mc_strEdit;

		static MZ3Data s_data;
		s_data.SetAccessType( ACCESS_RSS_READER_AUTO_DISCOVERY );

		// URL �ݒ�
		s_data.SetURL( url );
		s_data.SetBrowseUri( url );

		// �ʐM�J�n
		AccessProc( &s_data, s_data.GetURL(), CInetAccess::ENCODING_NOCONVERSION );
	}

}

/**
 * �\�����e�̍Đݒ�
 *
 * �t�H���g��t�H���g�T�C�Y�̕ύX���ɌĂяo�����B
 */
void CMZ3View::ResetViewContent(void)
{
	// �Ƃ肠�����J�e�S���ύX���Ɠ������������{�B
	// ����ɂ��A�C�R���T�C�Y�̍Đݒ肪�s����
	OnMySelchangedCategoryList();
}

/**
 * �����J�������[�h�̕ύX(�g�O��)
 */
void CMZ3View::OnAcceleratorToggleIntegratedMode()
{
	// �����J�������[�h�̕ύX(�g�O��)
	theApp.m_optionMng.m_bBodyListIntegratedColumnMode = !theApp.m_optionMng.m_bBodyListIntegratedColumnMode;

	// ���C�A�E�g���f
	MySetLayout(0, 0);

	// �{�f�B���X�g�I���ʒu�̕ۑ�
	int selectedBody = 0;
	CCategoryItem* pCategory = m_selGroup->getSelectedCategory();
	if (pCategory) {
		selectedBody = pCategory->selectedBody;
	}

	// �{�f�B���X�g�̍č\�z
	OnMySelchangedCategoryList();

	// �{�f�B���X�g�I���ʒu�̕��A
	if (pCategory) {
		pCategory->selectedBody = selectedBody;
		util::MySetListCtrlItemFocusedAndSelected( m_bodyList, pCategory->selectedBody, true );
		m_bodyList.EnsureVisible(pCategory->selectedBody, FALSE);
	}
}

/**
 * Twitter�X�^�C���̑��M���[�h���A�J�e�S���̃A�N�Z�X��ʂɉ����ď���������
 */
void CMZ3View::MyResetTwitterStylePostMode()
{
	MZ3_TRACE(L"MyResetTwitterStylePostMode()\n");

	// �J�e�S���̎�ʂ�D�悵�ď�����
	CCategoryItem* pCategory = m_selGroup->getSelectedCategory();
	if (pCategory) {
		// MZ3 API : �J�e�S����ʖ���Twitter���������݃��[�h�̏�����
		util::MyLuaDataList rvals;
		CStringA serializeKey = CStringA(theApp.m_accessTypeInfo.getSerializeKey(pCategory->m_mixi.GetAccessType()));
		if (util::CallMZ3ScriptHookFunctions2("reset_twitter_style_post_mode", &rvals, util::MyLuaData(serializeKey))) {
			return;
		}
	} else {
		// �f�t�H���g�� Twitter
		m_twitterPostAccessType = ACCESS_TWITTER_UPDATE;
	}
}

/**
 * URL���N���b�v�{�[�h�ɃR�s�[
 */
void CMZ3View::OnCopyClipboardUrl(UINT nID)
{
	const CMixiData& data = GetSelectedBodyItem();

	UINT idx = nID - (ID_REPORT_COPY_URL_BASE+1);
	if( idx > data.m_linkList.size() ) {
		return;
	}

	LPCTSTR url  = data.m_linkList[idx].url;

	// �N���b�v�{�[�h�ɃR�s�[
	util::SetClipboardDataTextW( url );

}

/**
 * MZ3 API �œo�^���ꂽ���j���[�̃C�x���g
 */
void CMZ3View::OnLuaMenu(UINT nID)
{
	UINT idx = nID - ID_LUA_MENU_BASE;
	if (idx >= theApp.m_luaMenus.size()) {
		MZ3LOGGER_ERROR(util::FormatString(L"�s���ȃ��j���[ID�ł� [%d]", nID));
		return;
	}

	// ID �̒ʒm
	lua_pushinteger(theApp.m_luaState, idx);
	lua_setglobal(theApp.m_luaState, "mz3_selected_menu_id");

	// Lua�֐����擾���Ăяo��
	const std::string& strFuncName = theApp.m_luaMenus[idx];
	util::CallMZ3ScriptHookFunction("", "select_menu", strFuncName.c_str(), &GetSelectedBodyItem());
}

/**
 * RSS AutoDiscovery URL �擾��̏���
 */
bool CMZ3View::DoAccessEndProcForRssAutoDiscovery(void)
{
	// �܂��� RSS�t�B�[�h�ł��邩�m�F����B

	// HTML �̎擾
	std::vector<unsigned char> text;
	util::LoadDownloadedFile(text, theApp.m_filepath.temphtml);

	// �����R�[�h�ϊ�
	// �Ƃ肠���� UTF-8 �Œ�Ƃ���
	/*
	std::vector<unsigned char> out_text;
	out_text.reserve( text.size() );
	kfm::kfm k( text, out_text );
	k.tosjis();
	*/
	std::vector<TCHAR> out_text;
	kfm::utf8_to_ucs2( text, out_text );


	CMixiDataList dummy_list;
	CString title;

	int nAppendedFeed = 0;
	if (parser::RssFeedParser::parse( dummy_list, out_text, &title )) {
		MZ3LOGGER_INFO( util::FormatString(L"RSS �������̂Œǉ������Burl[%s], title[%s]", 
			theApp.m_inet.GetURL(), title) );

		// ���ڂ̒ǉ�
		CCategoryItem categoryItem;
		categoryItem.init( 
			// ���O
			title,
			theApp.m_inet.GetURL(), 
			ACCESS_RSS_READER_FEED, 
			m_selGroup->categories.size()+1,
			theApp.m_accessTypeInfo.getBodyHeaderCol1Type(ACCESS_RSS_READER_FEED),
			theApp.m_accessTypeInfo.getBodyHeaderCol2Type(ACCESS_RSS_READER_FEED),
			theApp.m_accessTypeInfo.getBodyHeaderCol3Type(ACCESS_RSS_READER_FEED));
		AppendCategoryList(categoryItem);

		nAppendedFeed = 1;
	} else {
		MZ3LOGGER_INFO( L"RSS ����Ȃ��̂ŁARSS AutoDiscovery ���Ă݂��" );

		CMixiDataList items;
		if (parser::RssAutoDiscoveryParser::parse( items, out_text ) &&
			items.size()>0)
		{
			// items �̓o�^�m�F
			for (u_int i=0; i<items.size(); i++) {
				// ���ڂ̒ǉ�
				CCategoryItem categoryItem;
				categoryItem.init( 
					// ���O
					items[i].GetTitle(),
					items[i].GetURL(), 
					ACCESS_RSS_READER_FEED, 
					m_selGroup->categories.size()+1,
					theApp.m_accessTypeInfo.getBodyHeaderCol1Type(ACCESS_RSS_READER_FEED),
					theApp.m_accessTypeInfo.getBodyHeaderCol2Type(ACCESS_RSS_READER_FEED),
					theApp.m_accessTypeInfo.getBodyHeaderCol3Type(ACCESS_RSS_READER_FEED));
				AppendCategoryList(categoryItem);
			}

			nAppendedFeed = items.size();
		}
	}
	
	switch (nAppendedFeed) {
	case 0:
		MessageBox( L"RSS ��������܂���ł���" );
		break;
	case 1:
		MessageBox( L"RSS ��ǉ����܂���" );
		break;
	default:
		MessageBox( util::FormatString(L"%d �� RSS ��ǉ����܂���", nAppendedFeed) );
		break;
	}

	return true;
}

/**
 * �o�[�W�����`�F�b�N�J�n
 */
bool CMZ3View::DoCheckSoftwareUpdate(void)
{
	if (theApp.m_access) {
		// �A�N�Z�X���͍ăA�N�Z�X�s��
		return false;
	}

	static CMixiData s_data;
	s_data = CMixiData();
	s_data.SetAccessType(ACCESS_SOFTWARE_UPDATE_CHECK);
	AccessProc( &s_data, theApp.m_accessTypeInfo.getDefaultCategoryURL(ACCESS_SOFTWARE_UPDATE_CHECK));

	return true;
}

/**
 * �o�[�W�����`�F�b�N URL �擾��̏���
 */
bool CMZ3View::DoAccessEndProcForSoftwareUpdateCheck(void)
{
	// XML ���
	xml2stl::Container root;
	if (!xml2stl::SimpleXmlParser::loadFromFile(root, theApp.m_filepath.temphtml)) {
		MZ3LOGGER_ERROR( L"XML ��͎��s" );
		return false;
	}

	// �o�[�W�����AURL�A�^�C�g���̎擾
	const xml2stl::Node* pLatestVerNode = NULL;
	try {
#ifdef WINCE
		pLatestVerNode = &root.getNode(L"latest_version").getNode(L"mz3");
#else
		pLatestVerNode = &root.getNode(L"latest_version").getNode(L"mz4");
#endif
	} catch (xml2stl::NodeNotFoundException& e) {
		MZ3LOGGER_ERROR( util::FormatString( L"node not found... : %s", e.getMessage().c_str()) );
		return false;
	}

	MZ3LOGGER_DEBUG(
		util::FormatString(L"�o�[�W�����擾���ʁFcurrent[%s], latest_version[%s], url[%s], title[%s]",
			MZ3_VERSION_TEXT_SHORT,
			pLatestVerNode->getProperty(L"version").c_str(),
			pLatestVerNode->getProperty(L"url").c_str(),
			pLatestVerNode->getProperty(L"title").c_str()));

	const xml2stl::Node* pDevLatestVerNode = NULL;
	if (theApp.m_optionMng.m_bUseDevVerCheck) {
		try {
#ifdef WINCE
			pDevLatestVerNode = &root.getNode(L"latest_version").getNode(L"mz3_dev");
#else
			pDevLatestVerNode = &root.getNode(L"latest_version").getNode(L"mz4_dev");
#endif
		} catch (xml2stl::NodeNotFoundException& e) {
			MZ3LOGGER_ERROR( util::FormatString( L"node not found... : %s", e.getMessage().c_str()) );
			return false;
		}
		MZ3LOGGER_DEBUG(
			util::FormatString(L"�o�[�W�����擾����(dev)�Fcurrent[%s], latest_version[%s], url[%s], title[%s]",
				MZ3_VERSION_TEXT_SHORT,
				pDevLatestVerNode->getProperty(L"version").c_str(),
				pDevLatestVerNode->getProperty(L"url").c_str(),
				pDevLatestVerNode->getProperty(L"title").c_str()));
	}

	// �o�[�W�����ԍ��̐��K��
	// 0.9.3.7       => 0.9310700
	CString strCurrentVersionR = theApp.MakeMZ3RegularVersion(MZ3_VERSION_TEXT_SHORT);
	CString strLatestVersionR  = theApp.MakeMZ3RegularVersion(pLatestVerNode->getProperty(L"version").c_str());
	CString strDevVersionR     = L"0.0000000";
	if (theApp.m_optionMng.m_bUseDevVerCheck) {
		strDevVersionR = theApp.MakeMZ3RegularVersion(pDevLatestVerNode->getProperty(L"version").c_str());
	}

	MZ3LOGGER_DEBUG(util::FormatString(L"���K���o�[�W�����ԍ��Fcurrent[%s]", strCurrentVersionR));
	MZ3LOGGER_DEBUG(util::FormatString(L"���K���o�[�W�����ԍ��Flatest [%s]", strLatestVersionR));
	MZ3LOGGER_DEBUG(util::FormatString(L"���K���o�[�W�����ԍ��Fdev    [%s]", strDevVersionR));

	CString strNewURL;
	CString strNewTitle;
	if (strLatestVersionR > strCurrentVersionR) {
		// ����ł��ŐV
		strNewTitle = pLatestVerNode->getProperty(L"title").c_str();
		strNewURL   = pLatestVerNode->getProperty(L"url").c_str();
	} else if (strDevVersionR > strCurrentVersionR) {
		// �J���ł��ŐV
		strNewTitle = pDevLatestVerNode->getProperty(L"title").c_str();
		strNewURL   = pDevLatestVerNode->getProperty(L"url").c_str();
	} else {
		// �ŐV�o�[�W����
		MessageBox(L"�V�����o�[�W�����͂���܂���ł����B", NULL, MB_ICONINFORMATION);
		return true;
	}

	// �V�o�[�W��������
	CString msg;
	msg.Format(L"�V�����o�[�W����(%s)�����p�ł��܂��B\n�������_�E�����[�h���Ă���낵���ł����H", 
		strNewTitle);
	if (MessageBox(msg, NULL, MB_YESNO | MB_ICONQUESTION)==IDYES) {
		// MZ4�̓_�E�����[�h
		static CMixiData s_data;
		s_data = CMixiData();
		s_data.SetAccessType(ACCESS_DOWNLOAD);

		// �A�N�Z�X�J�n
		theApp.m_access = true;
		m_abort = FALSE;

		// �R���g���[����Ԃ̕ύX
		MyUpdateControlStatus();

		// �_�E�����[�h�t�@�C���p�X
		theApp.m_inet.Initialize( m_hWnd, &s_data );
		theApp.m_accessType = s_data.GetAccessType();
		theApp.m_inet.DoGet(strNewURL, _T(""), CInetAccess::FILE_BINARY);
	}

	return true;
}

/// �u�J�e�S�����O�̃����[�h�v���j���[
void CMZ3View::ReloadCategoryListLog()
{
	// ���O�̃��[�h
	MyLoadCategoryLogfile( *m_selGroup->getSelectedCategory() );

	// �{�f�B���X�g�ɐݒ�
	SetBodyList( m_selGroup->getSelectedCategory()->GetBodyList() );
}

/// �^�u�b�E�Ɉړ�
void CMZ3View::OnTabmenuMoveToRight()
{
	MoveTabItem(m_groupTab.GetCurSel(), m_groupTab.GetCurSel()+1);
}

/// �^�u�b���Ɉړ�
void CMZ3View::OnTabmenuMoveToLeft()
{
	MoveTabItem(m_groupTab.GetCurSel(), m_groupTab.GetCurSel()-1);
}

/// �^�u�̈ړ�
void CMZ3View::MoveTabItem(int oldTabIndex, int newTabIndex)
{
	if (newTabIndex < 0 || newTabIndex > m_groupTab.GetItemCount()-1) {
		return;
	}

	if (theApp.m_access) {
		return;
	}

	// ����(theApp.m_root�̕ύX)
	CString oldTabName = theApp.m_root.groups[oldTabIndex].name;
	CGroupItem oldGroupItem = theApp.m_root.groups[oldTabIndex];
	theApp.m_root.groups[oldTabIndex] = theApp.m_root.groups[newTabIndex];
	theApp.m_root.groups[newTabIndex] = oldGroupItem;

	// �R���g���[���ւ̔��f
	TCITEM tcItem;
	TCHAR buffer[256] = {0};
	tcItem.pszText = buffer;
	tcItem.cchTextMax = 256;
	tcItem.mask = TCIF_TEXT;
	wcsncpy( tcItem.pszText, theApp.m_root.groups[oldTabIndex].name, 255 );
	m_groupTab.SetItem(oldTabIndex, &tcItem);
	wcsncpy( tcItem.pszText, theApp.m_root.groups[newTabIndex].name, 255 );
	m_groupTab.SetItem(newTabIndex, &tcItem);

	// �I��ύX
	m_groupTab.SetCurSel( newTabIndex );

	// �J�e�S���[���X�g������������
	OnSelchangedGroupTab();

	// �O���[�v��`�t�@�C���̕ۑ�
	theApp.SaveGroupData();
}

void CMZ3View::MyRedrawBodyImages(void)
{
	// �A�C�R�������ւ�
	CCategoryItem* pCategory = m_selGroup->getSelectedCategory();
	if (pCategory!=NULL) {
		int idx = pCategory->selectedBody;

		// CImageList::Replace �������Ȃ��̂ŁA�����[�h���Ă��܂��B
		SetBodyImageList( pCategory->GetBodyList() );
	}

	// �A�C�R���ĕ`��
	InvalidateRect( m_rectIcon, FALSE );
}

/// �J�e�S���b��Ɉړ�
void CMZ3View::OnCategorymenuMoveUp()
{
	MoveCategoryItem(m_selGroup->focusedCategory, m_selGroup->focusedCategory-1);
}

/// �J�e�S���b���Ɉړ�
void CMZ3View::OnCategorymenuMoveDown()
{
	MoveCategoryItem(m_selGroup->focusedCategory, m_selGroup->focusedCategory+1);
}

/// �J�e�S���̈ړ�
void CMZ3View::MoveCategoryItem(int oldCategoryIndex, int newCategoryIndex)
{
	if (newCategoryIndex<0 || newCategoryIndex>=(int)m_selGroup->categories.size()) {
		return;
	}

	if (theApp.m_access) {
		return;
	}

	// ����
	CCategoryItem tmp = m_selGroup->categories[oldCategoryIndex];
	m_selGroup->categories[oldCategoryIndex] = m_selGroup->categories[newCategoryIndex];
	m_selGroup->categories[newCategoryIndex] = tmp;

	// index�̍Đݒ�
	m_selGroup->categories[oldCategoryIndex].SetIndexOnList(newCategoryIndex);
	m_selGroup->categories[newCategoryIndex].SetIndexOnList(oldCategoryIndex);

	// �I����ԕύX
	m_selGroup->selectedCategory = m_selGroup->focusedCategory = newCategoryIndex;
	m_preCategory = newCategoryIndex;

	// �R���g���[���ւ̔��f �� ���S�̂��߃^�u�P�ʂ̍č\�z�ŋ������f
	OnSelchangedGroupTab();

	// �O���[�v��`�t�@�C���̕ۑ�
	theApp.SaveGroupData();
}

HBRUSH CMZ3View::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CFormView::OnCtlColor(pDC, pWnd, nCtlColor);

	int idc = pWnd->GetDlgCtrlID();
	switch (idc) {
	case IDC_INFO_EDIT:
		pDC->SetBkMode(TRANSPARENT);
		pDC->SetTextColor(theApp.m_skininfo.getColor("MainStatusText"));
		return (HBRUSH)theApp.m_brushMainStatusBar;

	case IDC_STATUS_EDIT:
		pDC->SetBkColor(theApp.m_skininfo.getColor("MainEditBG"));
		pDC->SetTextColor(theApp.m_skininfo.getColor("MainEditText"));
		return (HBRUSH)theApp.m_brushMainEdit;

	default:
		break;
	}

	return hbr;
}

/**
 * ��1�J�����ɕ\�����Ă�����e���X�e�[�^�X�o�[�ɕ\������
 */
bool CMZ3View::MySetInfoEditFromBodySelectedData(void)
{
	if (m_selGroup == NULL) {
		return false;
	}

	CCategoryItem* pCategoryItem = m_selGroup->getSelectedCategory();
	if (pCategoryItem == NULL) {
		return false;
	}

	// ��1�J�����ɕ\�����Ă�����e��\������
	CString strInfo = util::MyGetItemByBodyColType(&GetSelectedBodyItem(), pCategoryItem->m_bodyColType1, false);

	// �G�����𕶎���ɕϊ�����
	if( LINE_HAS_EMOJI_LINK(strInfo) ) {
		mixi::ParserUtil::ReplaceEmojiImageToText( strInfo );
	}

	m_infoEdit.SetWindowText( strInfo );

	return true;
}

void CMZ3View::MySetMagnifyModeTo(MAGNIFY_MODE magnifyModeTo)
{
	switch (magnifyModeTo) {
	case MAGNIFY_MODE_BODY:
		// �{�f�B���X�g�\���ɕύX
		m_magnifyMode = MAGNIFY_MODE_BODY;

		// ���C�A�E�g�ύX���f
		MySetLayout(0, 0);
		InvalidateRect( m_rectIcon, FALSE );

		// �J�e�S���̑I�����ڂ݂̂�\��
		m_categoryList.EnsureVisible(m_selGroup->focusedCategory, FALSE);

		// �t�H�[�J�X�ړ�
		if (m_bodyList.GetItemCount() > 0) {
			m_bodyList.SetFocus();
		}
		break;

	case MAGNIFY_MODE_CATEGORY:
		// �J�e�S���ő�\���ɕύX
		m_magnifyMode = MAGNIFY_MODE_CATEGORY;

		// ���C�A�E�g�ύX���f
		MySetLayout(0, 0);
		InvalidateRect( m_rectIcon, FALSE );

		// �I�����ڂɃt�H�[�J�X�ړ�
		m_categoryList.EnsureVisible(m_selGroup->selectedCategory, FALSE);
		m_selGroup->focusedCategory = m_selGroup->selectedCategory;
		util::MySetListCtrlItemFocusedAndSelected( m_categoryList, m_selGroup->focusedCategory, true );
		break;

	default:
		m_magnifyMode = MAGNIFY_MODE_DEFAULT;

		// ���C�A�E�g�ύX���f
		MySetLayout(0, 0);
		InvalidateRect( m_rectIcon, FALSE );
		break;
	}
}

/// ���y�C���ő剻
void CMZ3View::OnLayoutMagnifyBodyList()
{
	MySetMagnifyModeTo(MAGNIFY_MODE_BODY);
}

/// ��y�C���ő剻
void CMZ3View::OnLayoutMagnifyCategoryList()
{
	MySetMagnifyModeTo(MAGNIFY_MODE_CATEGORY);
}

/// �㉺�y�C���̍ő剻�����ɖ߂�
void CMZ3View::OnLayoutMagnifyDefault()
{
	MySetMagnifyModeTo(MAGNIFY_MODE_DEFAULT);
}
