/*
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */
/// MZ3View.cpp : CMZ3View �N���X�̎���

#include "stdafx.h"
#include "version.h"
#include "MZ3.h"

#include "MZ3Doc.h"
#include "MZ3View.h"

#include "MixiData.h"
#include "CategoryItem.h"
#include "HtmlArray.h"
#include "ReportView.h"
#include "DownloadView.h"
#include "MainFrm.h"
#include "WriteView.h"
#include "util.h"
#include "util_gui.h"
#include "MixiParser.h"
#include "ChooseAccessTypeDlg.h"
#include "OpenUrlDlg.h"
#include "CommonEditDlg.h"
#include "MiniImageDialog.h"
#include "url_encoder.h"
#include "twitter_util.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define TIMERID_INTERVAL_CHECK	101

inline CString MyGetItemByBodyColType( CMixiData* data, CCategoryItem::BODY_INDICATE_TYPE bodyColType, bool bLimitForList=true )
{
	CString item;

	switch( bodyColType ) {
	case CCategoryItem::BODY_INDICATE_TYPE_DATE:
		item = data->GetDate();
		break;
	case CCategoryItem::BODY_INDICATE_TYPE_NAME:
		item = data->GetName();
		break;
	case CCategoryItem::BODY_INDICATE_TYPE_TITLE:
		item = data->GetTitle();
		break;
	case CCategoryItem::BODY_INDICATE_TYPE_BODY:
		// �{����1�s�ɕϊ����Ċ��蓖�āB
		for( u_int i=0; i<data->GetBodySize(); i++ ) {
			CString line = data->GetBody(i);
			while( line.Replace( L"\r\n", L"" ) );
			item.Append( line );
		}
		break;
	default:
		return L"";
	}

	// ����ݒ�
	if (bLimitForList) {
#ifdef WINCE
		// WindowsMobile �̏ꍇ�́A30�������炢�Ő؂�Ȃ��Ɨ�����̂Ő�������B
		return item.Left( 30 );
#else
		// Windows �̏ꍇ�́A�Ƃ肠����100�����Ő؂��Ă����B
		return item.Left( 100 );
#endif
	} else {
		return item;
	}
}

/// �A�N�Z�X��ʂƕ\����ʂ���A�{�f�B�[���X�g�̃w�b�_�[������i�Q�J�����ځj���擾����
LPCTSTR MyGetBodyHeaderColName2( const CMixiData& mixi, CCategoryItem::BODY_INDICATE_TYPE bodyIndicateType )
{
	ACCESS_TYPE accessType = mixi.GetAccessType();

	switch (accessType) {
	case ACCESS_LIST_DIARY:
	case ACCESS_LIST_NEW_COMMENT:
	case ACCESS_LIST_COMMENT:
		switch( bodyIndicateType ) {
		case CCategoryItem::BODY_INDICATE_TYPE_NAME:
			return _T("���O>>");
		default:
			return _T("����>>");
		}
		break;
	case ACCESS_LIST_NEWS:
		switch( bodyIndicateType ) {
		case CCategoryItem::BODY_INDICATE_TYPE_NAME:
			return _T("�z����>>");
		default:
			return _T("�z�M����>>");
		}
		break;
	case ACCESS_LIST_FAVORITE:
		switch( bodyIndicateType ) {
		case CCategoryItem::BODY_INDICATE_TYPE_DATE:
			// ���C�ɓ���̎�ʁi���[�U�[or�R�~���j�e�B�j�ɉ����ĕύX
			if( mixi.GetURL().Find( L"kind=community" ) != -1 ) {
				return _T("�l��>>");
			}else{
				return _T("�ŏI���O�C��>>");
			}
		default:
			// ���C�ɓ���̎�ʁi���[�U�[or�R�~���j�e�B�j�ɉ����ĕύX
			if( mixi.GetURL().Find( L"kind=community" ) != -1 ) {
				return _T("����>>");
			}else{
				return _T("���ȏЉ�>>");
			}
		}
		break;
	case ACCESS_LIST_FRIEND:
		return _T("���O�C������");
	case ACCESS_LIST_COMMUNITY:
		return _T("�l��");
	case ACCESS_LIST_INTRO:
		return L"�Љ";
	case ACCESS_LIST_NEW_BBS:
	case ACCESS_LIST_NEW_BBS_COMMENT:
		switch( bodyIndicateType ) {
		case CCategoryItem::BODY_INDICATE_TYPE_NAME:
			return _T("�R�~���j�e�B>>");
		default:
			return _T("����>>");
		}
		break;
	case ACCESS_LIST_CALENDAR:
	case ACCESS_LIST_BBS:
		return L"���t";
	case ACCESS_LIST_BOOKMARK:
		return _T("�R�~���j�e�B");
	case ACCESS_LIST_MYDIARY:
		return _T("����");
	case ACCESS_LIST_MESSAGE_IN:
		switch( bodyIndicateType ) {
		case CCategoryItem::BODY_INDICATE_TYPE_NAME:
			return _T("���o�l>>");
		default:
			return _T("���t>>");
		}
	case ACCESS_LIST_MESSAGE_OUT:
		switch( bodyIndicateType ) {
		case CCategoryItem::BODY_INDICATE_TYPE_NAME:
			return _T("����>>");
		default:
			return _T("���t>>");
		}

	case ACCESS_LIST_FOOTSTEP:
		return _T("����");

	case ACCESS_TWITTER_FRIENDS_TIMELINE:
		switch( bodyIndicateType ) {
		case CCategoryItem::BODY_INDICATE_TYPE_NAME:
			return _T("���O>>");
		default:
			return _T("���t>>");
		}

	default:
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
	ON_WM_MOUSEWHEEL()
	ON_COMMAND(IDM_SET_READ, &CMZ3View::OnSetRead)
	ON_COMMAND(ID_ACCELERATOR_RELOAD, &CMZ3View::OnAcceleratorReload)
	ON_COMMAND(ID_MENU_TWITTER_READ, &CMZ3View::OnMenuTwitterRead)
	ON_COMMAND(ID_MENU_TWITTER_REPLY, &CMZ3View::OnMenuTwitterReply)
	ON_COMMAND(ID_MENU_TWITTER_UPDATE, &CMZ3View::OnMenuTwitterUpdate)
	ON_COMMAND(ID_MENU_TWITTER_HOME, &CMZ3View::OnMenuTwitterHome)
	ON_COMMAND(ID_MENU_TWITTER_FAVORITES, &CMZ3View::OnMenuTwitterFavorites)
	ON_COMMAND(ID_MENU_TWITTER_SITE, &CMZ3View::OnMenuTwitterSite)
	ON_BN_CLICKED(IDC_UPDATE_BUTTON, &CMZ3View::OnBnClickedUpdateButton)
	ON_COMMAND_RANGE(ID_REPORT_URL_BASE+1, ID_REPORT_URL_BASE+50, OnLoadUrl)
	ON_WM_PAINT()
	ON_COMMAND(ID_MENU_TWITTER_FRIEND_TIMELINE, &CMZ3View::OnMenuTwitterFriendTimeline)
	ON_COMMAND(ID_MENU_TWITTER_FRIEND_TIMELINE_WITH_OTHERS, &CMZ3View::OnMenuTwitterFriendTimelineWithOthers)
	ON_COMMAND(ID_TABMENU_DELETE, &CMZ3View::OnTabmenuDelete)
	ON_COMMAND_RANGE(ID_APPEND_MENU_BEGIN, ID_APPEND_MENU_END, &CMZ3View::OnAppendCategoryMenu)
	ON_COMMAND(ID_REMOVE_CATEGORY_ITEM, &CMZ3View::OnRemoveCategoryItem)
	ON_COMMAND(ID_EDIT_CATEGORY_ITEM, &CMZ3View::OnEditCategoryItem)
	ON_COMMAND(ID_TABMENU_EDIT, &CMZ3View::OnTabmenuEdit)
	ON_COMMAND(ID_TABMENU_ADD, &CMZ3View::OnTabmenuAdd)
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
{
	m_preCategory = 0;
	m_selGroup = NULL;
	m_pMiniImageDlg = NULL;

	m_hotList = NULL;

	m_nochange = FALSE;
	m_abort = FALSE;
}

/**
 * �f�X�g���N�^
 */
CMZ3View::~CMZ3View()
{
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

	m_access = FALSE;

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
		dwStyle |= LVS_REPORT | LVS_OWNERDRAWFIXED;

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
		dwStyle |= LVS_REPORT | LVS_OWNERDRAWFIXED;

		// �X�^�C���̍X�V
		m_bodyList.ModifyStyle(0, dwStyle);

		// �A�C�R�����X�g�̍쐬
		m_iconImageList.Create(16, 16, ILC_COLOR24 | ILC_MASK, 0, 4);
		m_iconImageList.Add( AfxGetApp()->LoadIcon(IDI_TOPIC_ICON) );
		m_iconImageList.Add( AfxGetApp()->LoadIcon(IDI_EVENT_ICON) );
		m_iconImageList.Add( AfxGetApp()->LoadIcon(IDI_ENQUETE_ICON) );
		m_bodyList.SetImageList(&m_iconImageList, LVSIL_SMALL);

		// �J�����쐬
		// ����������������ɍĐݒ肷��̂ŉ��̕����w�肵�Ă����B
		switch( theApp.GetDisplayMode() ) {
		case SR_VGA:
			m_bodyList.InsertColumn(0, _T("�^�C�g��"), LVCFMT_LEFT, 120*2, -1);
			m_bodyList.InsertColumn(1, _T("���O"), LVCFMT_LEFT, 105*2, -1);
			break;
		case SR_QVGA:
		default:
			m_bodyList.InsertColumn(0, _T("�^�C�g��"), LVCFMT_LEFT, 120, -1);
			m_bodyList.InsertColumn(1, _T("���O"), LVCFMT_LEFT, 105, -1);
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

	// �q���
#ifndef WINCE
	m_pMiniImageDlg = new CMiniImageDialog( this );
	m_pMiniImageDlg->ShowWindow( SW_HIDE );

	// ����������
	::SetWindowLong( m_pMiniImageDlg->m_hWnd, GWL_EXSTYLE, GetWindowLong(m_pMiniImageDlg->m_hWnd, GWL_EXSTYLE) | WS_EX_LAYERED);
	
	typedef BOOL (WINAPI *PROCSETLAYEREDWINDOW)(HWND, COLORREF, BYTE, DWORD);
	PROCSETLAYEREDWINDOW pProcSetLayeredWindowAttributes;
	pProcSetLayeredWindowAttributes = 
		(PROCSETLAYEREDWINDOW)GetProcAddress(GetModuleHandleA("USER32.DLL"), "SetLayeredWindowAttributes");

	int n = 100-10;
	if (pProcSetLayeredWindowAttributes!=NULL) {
		pProcSetLayeredWindowAttributes(m_pMiniImageDlg->m_hWnd, 0, 255*n/100, LWA_ALPHA);
	}
#endif

	// �������X���b�h�J�n
	AfxBeginThread( Initialize_Thread, this );

	// �C���^�[�o���^�C�}�[����
	UINT result = SetTimer( TIMERID_INTERVAL_CHECK, 1000, NULL );
//	DWORD e = ::GetLastError();
}

/**
 * �{�f�B���X�g�̃A�C�R���̃C���f�b�N�X���擾����
 */
inline int MyGetBodyListDefaultIconIndex( const CMixiData& mixi )
{
	int iconIndex = -1;
	switch (mixi.GetAccessType()) {
	case ACCESS_BBS:		iconIndex = 0;	break;
	case ACCESS_EVENT:		iconIndex = 1;	break;
	case ACCESS_ENQUETE:	iconIndex = 2;	break;
	default:				iconIndex = -1;	break;	// �A�C�R���Ȃ�
	}
	return iconIndex;
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
 * �x�����������\�b�h�i�������p�X���b�h����N�������j
 */
bool CMZ3View::DoInitialize()
{
	// �����f�[�^�ݒ�
	InsertInitialData();

	m_hotList = &m_categoryList;

	// ���O���[�h���Ɉړ��ł��Ȃ��悤�ɁA�A�N�Z�X�t���O�𗧂ĂĂ���
	m_access = TRUE;

	// ���O�̃��[�h
	MyLoadCategoryLogfile( *m_selGroup->getSelectedCategory() );

	// �{�f�B���X�g�ɐݒ�
	SetBodyList( m_selGroup->getSelectedCategory()->GetBodyList() );

	// �A�N�Z�X�t���O�����낷
	m_access = FALSE;

	// �u�b�N�}�[�N�̃��[�h
	try{
		theApp.m_bookmarkMng.Load( theApp.m_root.GetBookmarkList() );
	}catch(...){
	}

	// �V�����b�Z�[�W�m�F
	if (theApp.m_optionMng.IsBootCheckMnC() != FALSE) {
		// �V�����b�Z�[�W�m�F
		DoNewCommentCheck();
	}

	// �X�^�C���ύX
	VIEW_STYLE newStyle = MyGetViewStyleForSelectedCategory();
	if (newStyle!=m_viewStyle) {
		m_viewStyle = newStyle;
		MySetLayout(0,0);
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
	if (0 <= theApp.m_optionMng.m_lastTopPageCategoryIndex && theApp.m_optionMng.m_lastTopPageCategoryIndex < nCategory) {
		m_selGroup->focusedCategory = m_selGroup->selectedCategory = theApp.m_optionMng.m_lastTopPageCategoryIndex;
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
	MZ3LOGGER_DEBUG( L"OnSize( " + util::int2str(nType) + L", " + util::int2str(cx) + L", " + util::int2str(cy) + L" )" );

	MySetLayout( cx, cy );
}

void CMZ3View::MySetLayout(int cx, int cy)
{
	// �O��̒l��ۑ����A(0,0) �̏ꍇ�͂��̒l�𗘗p����
	static int s_cx = 0;
	static int s_cy = 0;
	if (cx==0 && cy==0) {
		cx = s_cx;
		cy = s_cy;
	} else {
		s_cx = cx;
		s_cy = cy;
	}
/*	{
		CRect rect;
		CString msg;

		GetWindowRect( &rect );
		msg.Format( L" wrect-cx,cy : %d, %d", rect.Width(), rect.Height() );
		MZ3LOGGER_DEBUG( msg );

		GetWindowRect( &rect );
		msg.Format( L" crect-cx,cy : %d, %d", rect.Width(), rect.Height() );
		MZ3LOGGER_DEBUG( msg );
	}
*/
	int fontHeight = theApp.m_optionMng.m_fontHeight;
	if( fontHeight == 0 ) {
		fontHeight = 24;
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
		hInfo = (int)(hInfoBase * 1.8);
#else
		hInfo = (int)(hInfoBase * 1.5);
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
		hInfo = (int)(hInfoBase * 1.8);
#else
		hInfo = (int)(hInfoBase * 1.5);
#endif
		break;
	}

	// �J�e�S���A�{�f�B���X�g�̗̈�� % �Ŏw��
	// �i�A���A�J�e�S�����X�g�̓O���[�v�^�u���A�{�f�B���X�g�͏��̈���܂ށj
	const int h1 = theApp.m_optionMng.m_nMainViewCategoryListHeightRatio;
	const int h2 = theApp.m_optionMng.m_nMainViewBodyListHeightRatio;

	int hCategory = (cy * h1 / (h1+h2)) - (hGroup -1);
	int hBody     = (cy * h2 / (h1+h2)) - (hInfo + hPost -1);

	int y = 0;
	util::MoveDlgItemWindow( this, IDC_GROUP_TAB,   0, y, cx, hGroup    );
	y += hGroup;

	util::MoveDlgItemWindow( this, IDC_HEADER_LIST, 0, y, cx, hCategory );
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
		m_rectIcon.SetRect( 0, y, hInfo, y+hInfo );
		util::MoveDlgItemWindow( this, IDC_INFO_EDIT, hInfo, y, cx-hInfo, hInfo     );
		y += hInfo;
		break;
	}

	if (pUpdateButton!=NULL) {
		int w = rectUpdateButton.Width();
		if (hPost>0) {
			util::MoveDlgItemWindow( this, IDC_STATUS_EDIT,   0,    y, cx - w, hPost );
			util::MoveDlgItemWindow( this, IDC_UPDATE_BUTTON, cx-w, y, w,      hPost );
		}
	}

	// ���x���ʂ��E�����[�h�̏ꍇ�̓X�^�C����ύX�����؂�
	if( theApp.m_optionMng.m_killPaneLabel ) {
		util::ModifyStyleDlgItemWindow(this, IDC_BODY_LIST, NULL, LVS_NOCOLUMNHEADER);
	}
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

	if (m_access) {
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

	OnMySelchangedCategoryList();

	*pResult = 0;
}

/**
 * �J�e�S�����X�g�_�u���N���b�N���̏���
 */
void CMZ3View::OnNMDblclkCategoryList(NMHDR *pNMHDR, LRESULT *pResult)
{
	if (m_access) {
		// �A�N�Z�X���͍ăA�N�Z�X�s��
		return;
	}

	LPNMLISTVIEW lpnmlv = (LPNMLISTVIEW)pNMHDR;
	m_hotList = &m_categoryList;

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
	}
	else {
		theApp.EnableCommandBarButton( ID_WRITE_BUTTON, FALSE);
	}
	m_infoEdit.SetWindowText(_T(""));

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
		// �f�[�^���m�t�k�k�̏ꍇ
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
			CString path = util::MakeImageLogfilePathFromUrl( theApp.m_inet.GetURL() );
			CopyFile( theApp.m_filepath.temphtml, path, FALSE/*bFailIfExists, �㏑��*/ );

			// �`��
			if (m_pMiniImageDlg!=NULL) {
				m_pMiniImageDlg->DrawImageFile( path );
			}

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
		break;
	}

	// �ʐM�����i�t���O��������j
	m_access = FALSE;

	// �v���O���X�o�[���\��
	mc_progressBar.ShowWindow( SW_HIDE );

	theApp.EnableCommandBarButton( ID_STOP_BUTTON, FALSE);

	MZ3LOGGER_DEBUG(_T("OnGetEndBinary end"));

	return TRUE;
}

/**
 * �A�N�Z�X�I���ʒm��M
 */
LRESULT CMZ3View::OnGetEnd(WPARAM wParam, LPARAM lParam)
{
	TRACE(_T("InetAccess End\n"));

	util::MySetInformationText( m_hWnd, _T("HTML��͒�") );

//	util::StopWatch sw;

	if (m_abort) {
		::SendMessage(m_hWnd, WM_MZ3_GET_ABORT, NULL, lParam);
		return TRUE;
	}

	if (lParam == NULL) {
		// �f�[�^���m�t�k�k�̏ꍇ
		LPCTSTR msg = L"�����G���[���������܂���(�߂�l��NULL)";
		MZ3LOGGER_ERROR( msg );
		util::MySetInformationText( m_hWnd, msg );
		return TRUE;
	}

	CMixiData* data = (CMixiData*)lParam;
	ACCESS_TYPE aType = data->GetAccessType();

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
			data->SetURL( theApp.MakeLoginUrl() );

			// ���O�C�����s
			data->SetAccessType( ACCESS_LOGIN );
			AccessProc( data, data->GetURL() );

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
				if (wcslen(theApp.m_loginMng.GetOwnerID()) == 0) {
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

				m_access = FALSE;

				// �v���O���X�o�[���\��
				mc_progressBar.ShowWindow( SW_HIDE );
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

			mixi::HomeParser::parse( html );

			if (m_checkNewComment) {
				// �R�����g�E���b�Z�[�W���`�F�b�N���[�h

				// �V�����b�Z�[�W�A�V���R�����g�̒ʒm
				CString msg;
				if( theApp.m_newMessageCount > 0 ) {
					if( theApp.m_newCommentCount > 0 ) {
						msg.Format(_T("�V�����b�Z�[�W %d���A�V���R�����g %d ��"), 
							theApp.m_newMessageCount, theApp.m_newCommentCount );
					}else{
						msg.Format(_T("�V�����b�Z�[�W %d��"), 
							theApp.m_newMessageCount);
					}
				}
				else {
					if( theApp.m_newCommentCount > 0 ) {
						msg.Format(_T("�V���R�����g %d��"), 
							theApp.m_newCommentCount);
					}else{
						msg = _T("�V�����b�Z�[�W�A�R�����g�͂���܂���");
					}
				}
				util::MySetInformationText( m_hWnd, msg );

				m_checkNewComment = false;

				theApp.EnableCommandBarButton( ID_STOP_BUTTON, FALSE);

				break;
			} else {
				// �V�����b�Z�[�W�ȊO�Ȃ̂ŁA���O�C���̂��߂̎擾�������B

				// �f�[�^��Ҕ��f�[�^�ɖ߂�
				*data = theApp.m_mixiBeforeRelogin;

				AccessProc(data, util::CreateMixiUrl(data->GetURL()));

				return TRUE;
			}
		}

	case ACCESS_LIST_DIARY:
	case ACCESS_LIST_NEW_COMMENT:
	case ACCESS_LIST_COMMENT:
	case ACCESS_LIST_NEWS:
	case ACCESS_LIST_FAVORITE:
	case ACCESS_LIST_FRIEND:
	case ACCESS_LIST_COMMUNITY:
	case ACCESS_LIST_NEW_BBS:
	case ACCESS_LIST_NEW_BBS_COMMENT:
	case ACCESS_LIST_MYDIARY:
	case ACCESS_LIST_FOOTSTEP:
	case ACCESS_LIST_MESSAGE_IN:
	case ACCESS_LIST_MESSAGE_OUT:
	case ACCESS_LIST_INTRO:
	case ACCESS_LIST_BBS:
	case ACCESS_LIST_CALENDAR:
	case ACCESS_TWITTER_FRIENDS_TIMELINE:
		// --------------------------------------------------
		// �J�e�S�����ڂ̎擾
		// --------------------------------------------------
		{
			// �X�e�[�^�X�R�[�h�`�F�b�N
			LPCTSTR szStatusErrorMessage = NULL;	// ��NULL�̏ꍇ�̓G���[����
			switch (aType) {
			case ACCESS_TWITTER_FRIENDS_TIMELINE:
				szStatusErrorMessage = twitter::CheckHttpResponseStatus( theApp.m_inet.m_dwHttpStatus );
				break;
			}
			if (szStatusErrorMessage!=NULL) {
				CString msg = util::FormatString(L"�T�[�o�G���[(%d)�F%s", theApp.m_inet.m_dwHttpStatus, szStatusErrorMessage);
				util::MySetInformationText( m_hWnd, msg );
				MZ3LOGGER_ERROR( msg );
				// �ȍ~�̏������s��Ȃ��B
				break;
			}

			util::MySetInformationText( m_hWnd, _T("HTML��͒� : 1/3") );

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
				break;
			}

			// HTML �̎擾
			CHtmlArray html;
			html.Load( theApp.m_filepath.temphtml );

			theApp.EnableCommandBarButton( ID_STOP_BUTTON, FALSE);

			// �ۑ��� body �̎擾�B
			CMixiDataList& body = m_selGroup->getSelectedCategory()->GetBodyList();

			// HTML ���
			util::MySetInformationText( m_hWnd,  _T("HTML��͒� : 2/3") );
			mixi::MyDoParseMixiListHtml( aType, body, html );

			// �{�f�B�ꗗ�̐ݒ�
			util::MySetInformationText( m_hWnd,  _T("HTML��͒� : 3/3") );

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
					return TRUE;
				}
			}

			util::MySetInformationText( m_hWnd, L"����" );
		}
		break;

	case ACCESS_DIARY:
	case ACCESS_BBS:
	case ACCESS_ENQUETE:
	case ACCESS_EVENT:
	case ACCESS_MYDIARY:
	case ACCESS_MESSAGE:
	case ACCESS_NEWS:
	case ACCESS_PLAIN:
	case ACCESS_PROFILE:
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

	// �ʐM�����i�t���O��������j
	m_access = FALSE;

	// �v���O���X�o�[���\��
	mc_progressBar.ShowWindow( SW_HIDE );

	theApp.EnableCommandBarButton( ID_STOP_BUTTON, FALSE);

	return TRUE;
}

/**
* �A�N�Z�X�G���[�ʒm��M
*/
LRESULT CMZ3View::OnGetError(WPARAM wParam, LPARAM lParam)
{
	// ���~�{�^�����g�p�s�ɂ���
	theApp.EnableCommandBarButton( ID_STOP_BUTTON, FALSE);

	CMixiData* pMixi = (CMixiData*)lParam;

	if (pMixi->GetAccessType() == ACCESS_LOGIN) {
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

	m_access = FALSE;

	// �v���O���X�o�[���\��
	mc_progressBar.ShowWindow( SW_HIDE );

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

	theApp.EnableCommandBarButton( ID_STOP_BUTTON, FALSE);

	m_access = FALSE;

	// �v���O���X�o�[���\��
	mc_progressBar.ShowWindow( SW_HIDE );

	LPCTSTR msg = _T("���f���܂���");
	util::MySetInformationText( m_hWnd, msg );
//	::MessageBox(m_hWnd, msg, MZ3_APP_NAME, MB_ICONSTOP | MB_OK);

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
	if (theApp.m_optionMng.m_bShowMainViewMiniImage && !bUseDefaultIcon) {
		// �f�t�H���g�A�C�R�����Ȃ������̂ŁA���[�U�E�R�~���j�e�B�A�C�R�������쐬����
		for (int i=0; i<count; i++) {
			const CMixiData& mixi = body[i];
			// icon
			CMZ3BackgroundImage image(L"");
			CString miniImagePath = util::MakeImageLogfilePath( mixi );

			// ���[�h�ς݂�����
			bool bLoaded = theApp.m_imageCache.GetImageIndex(miniImagePath) >= 0 ? true : false;
			if (bLoaded) {
				// ���[�h�ς݂Ȃ̂Ń��[�h�s�v
				bUseExtendedIcon = true;
				continue;
			}

			// �����[�h�Ȃ̂Ń��[�h
			image.load( miniImagePath );
			if (image.isEnableImage()) {
				// 16x16 �Ƀ��T�C�Y����B
				CMZ3BackgroundImage resizedImage(L"");
				util::MakeResizedImage( this, resizedImage, image );

				// �r�b�g�}�b�v�̒ǉ�
				CBitmap bm;
				bm.Attach( resizedImage.getHandle() );

				theApp.m_imageCache.Add( &bm, (CBitmap*)NULL, miniImagePath );

				bUseExtendedIcon = true;
			} else {
				// ���[�h�G���[
				// �_�E�����[�h�}�l�[�W���ɓo�^����
				if (mixi.GetImageCount()>0 && !miniImagePath.IsEmpty()) {
					CString url = mixi.GetImage(0);
					DownloadItem item( url, L"�G����", miniImagePath, true );
					theApp.m_pDownloadView->AppendDownloadItem( item );
				}
			}
		}
	}

	// �A�C�R���\���E��\���ݒ�
	m_bodyList.MyEnableIcon( bUseDefaultIcon || bUseExtendedIcon );
	if (bUseDefaultIcon) {
		m_bodyList.SetImageList(&m_iconImageList, LVSIL_SMALL);
	} else if (bUseExtendedIcon) {
		m_bodyList.SetImageList(&theApp.m_imageCache.GetImageList(), LVSIL_SMALL);
	}

	// �A�C�R���̐ݒ�
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
			CString miniImagePath = util::MakeImageLogfilePath( mixi );

			// �C���f�b�N�X�T��
			iconIndex = theApp.m_imageCache.GetImageIndex( miniImagePath );
		}

		// �A�C�R���̃C���f�b�N�X��ݒ�
		util::MySetListCtrlItemImageIndex( m_bodyList, i, 0, iconIndex );
	}

	// �X�^�C���ύX
	VIEW_STYLE newStyle = MyGetViewStyleForSelectedCategory();
	if (newStyle!=m_viewStyle) {
		m_viewStyle = newStyle;
		MySetLayout(0,0);
	}

	util::MySetInformationText( m_hWnd, L"�A�C�R���̍쐬����" );
}

/**
 * �{�f�B�Ƀf�[�^��ݒ�
 */
void CMZ3View::SetBodyList( CMixiDataList& body )
{
	m_nochange = TRUE;

	// ���X�g�̃A�C�e�����폜
	m_bodyList.m_bStopDraw = true;
	m_bodyList.SetRedraw(FALSE);
	m_bodyList.DeleteAllItems();

	// �w�b�_�̕�����ύX
	CCategoryItem* pCategory = m_selGroup->getSelectedCategory();
	if (pCategory != NULL) {
		LPCTSTR szHeaderTitle2 = MyGetBodyHeaderColName2( pCategory->m_mixi, pCategory->m_secondBodyColType );
		switch (pCategory->m_mixi.GetAccessType()) {
		case ACCESS_LIST_DIARY:
		case ACCESS_LIST_NEW_COMMENT:
		case ACCESS_LIST_COMMENT:
		case ACCESS_LIST_CALENDAR:
			m_bodyList.SetHeader( _T("�^�C�g��"), szHeaderTitle2 );
			break;
		case ACCESS_LIST_NEWS:
			m_bodyList.SetHeader( _T("���o��"), szHeaderTitle2 );
			break;
		case ACCESS_LIST_FAVORITE:
			m_bodyList.SetHeader( _T("���O"), szHeaderTitle2 );
			break;
		case ACCESS_LIST_FRIEND:
			m_bodyList.SetHeader( _T("���O"), szHeaderTitle2 );
			break;
		case ACCESS_LIST_COMMUNITY:
			m_bodyList.SetHeader( _T("�R�~���j�e�B"), szHeaderTitle2 );
			break;
		case ACCESS_LIST_MESSAGE_IN:
		case ACCESS_LIST_MESSAGE_OUT:
			m_bodyList.SetHeader( _T("����"), szHeaderTitle2 );
			break;
		case ACCESS_LIST_MYDIARY:
			m_bodyList.SetHeader( _T("�^�C�g��"), szHeaderTitle2 );
			break;
		case ACCESS_LIST_NEW_BBS:
		case ACCESS_LIST_NEW_BBS_COMMENT:
		case ACCESS_LIST_BBS:
		case ACCESS_LIST_BOOKMARK:
			m_bodyList.SetHeader( _T("�g�s�b�N"), szHeaderTitle2 );
			break;
		case ACCESS_LIST_FOOTSTEP:
			m_bodyList.SetHeader( _T("���O"), szHeaderTitle2 );
			break;
		case ACCESS_LIST_INTRO:
			m_bodyList.SetHeader( _T("���O"), szHeaderTitle2 );
			break;
		case ACCESS_TWITTER_FRIENDS_TIMELINE:
			m_bodyList.SetHeader( _T("����"), szHeaderTitle2 );
			break;
		}
	}

	// �A�C�e���̒ǉ�
	m_bodyList.MyEnableIcon( false );	// �܂��A�C�R���̓I�t�ɂ��Đ���
	INT_PTR count = body.size();
	for (int i=0; i<count; i++) {
		CMixiData* data = &body[i];

		// �P�J������
		// �ǂ̍��ڂ�^���邩�́A�J�e�S�����ڃf�[�^���̎�ʂŌ��߂�
		int index = m_bodyList.InsertItem( i, MyGetItemByBodyColType(data,pCategory->m_firstBodyColType), -1 );

		// �Q�J������
		m_bodyList.SetItemText( index, 1, MyGetItemByBodyColType(data,pCategory->m_secondBodyColType) );

		// �{�f�B�̍��ڂ� ItemData �� index �����蓖�Ă�B
		m_bodyList.SetItemData( index, index );
	}

	m_nochange = FALSE;
	util::MySetListCtrlItemFocusedAndSelected( m_bodyList, 0, true );

	m_bodyList.SetRedraw(TRUE);
	m_bodyList.m_bStopDraw = false;
	m_bodyList.Invalidate( FALSE );

	// �A�C�R���pImageList�̐ݒ�
	SetBodyImageList( body );

	// �A�C�e����0���̏ꍇ�́Amini�摜��ʂ��\���ɂ���
	if (m_bodyList.GetItemCount()==0) {
		MoveMiniImageDlg();
	}

	util::MySetInformationText( m_hWnd, L"����" );
}

/**
 * ���s�̃t�H�[�J�X�ݒ�
 */
void CMZ3View::OnEnSetfocusInfoEdit()
{
	// ���̍s�ɂ̓t�H�[�J�X���ڂ��Ȃ�
	// �J�e�S�����X�g�Ƀt�H�[�J�X���ڂ�
	m_categoryList.SetFocus();
}

/**
 * �{�f�B���X�g�N���b�N
 */
void CMZ3View::OnNMDblclkBodyList(NMHDR *pNMHDR, LRESULT *pResult)
{
//	MZ3LOGGER_DEBUG( L"OnNMDblclkBodyList start" );
	*pResult = 0;

	if (m_access) {
		// �A�N�Z�X���͍ăA�N�Z�X�s��
		return;
	}

	LPNMLISTVIEW lpnmlv = (LPNMLISTVIEW)pNMHDR;

	// �J�����g�f�[�^���擾
	m_hotList = &m_bodyList;
	m_selGroup->getSelectedCategory()->selectedBody = lpnmlv->iItem;

	CMixiData& data = m_selGroup->getSelectedCategory()->GetSelectedBody();

	TRACE(_T("http://mixi.jp/%s\n"), data.GetURL());

	if (data.GetAccessType() == ACCESS_LIST_FOOTSTEP) {
		return;
	}

	// �R�~���j�e�B�̏ꍇ�́A�g�s�b�N�ꗗ��\������B
	// �i�b��Ή��j
	switch (data.GetAccessType()) {
	case ACCESS_COMMUNITY:
		OnViewBbsList();
		return;

	case ACCESS_TWITTER_USER:
		// �_�u���N���b�N�̏ꍇ�͑S���\��
		OnMenuTwitterRead();
		return;
	}

	AccessProc(&data, util::CreateMixiUrl(data.GetURL()));
}

/**
 * �A�C�e���ύX
 */
void CMZ3View::OnLvnItemchangedBodyList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);

	if (m_nochange != FALSE) {
		return;
	}

	// �I����ύX������A���̃A�C�e�����A�N�e�B�u�ɂ���
	if (pNMLV->uNewState != 3) {
		return;
	}
	CCategoryItem* pCategory = m_selGroup->getSelectedCategory();
	pCategory->selectedBody = pNMLV->iItem;

	// ��1�J�����ɕ\�����Ă�����e��\������B
	m_infoEdit.SetWindowText( 
		MyGetItemByBodyColType(&GetSelectedBodyItem(), pCategory->m_firstBodyColType, false) );

	// �摜�ʒu�ύX
	MoveMiniImageDlg();

	// Twitter �ł���Γ���I�[�i�[ID�̍��ڂ��ĕ\��
	if (pCategory->m_mixi.GetAccessType()==ACCESS_TWITTER_FRIENDS_TIMELINE) {
		// �S���ĕ`�悷��B
		// �w�i�̍ĕ`�����߂�Ώ����̓}�V�ɂȂ邩���B
		m_bodyList.Invalidate(FALSE);
	}

	// �A�C�R���ĕ`��
	InvalidateRect( m_rectIcon, FALSE );

	*pResult = 0;
}

BOOL CMZ3View::OnKeyUp(MSG* pMsg)
{
	// ���ʏ���
	switch (pMsg->wParam) {
	case VK_F1:
#ifdef WINCE
		if( theApp.m_optionMng.m_bUseLeftSoftKey ) {
			// ���C�����j���[�̃|�b�v�A�b�v
			RECT rect;
			int flags = TPM_CENTERALIGN | TPM_VCENTERALIGN;
			SystemParametersInfo(SPI_GETWORKAREA, 0, &rect, 0);

			CMenu menu;
			CMainFrame* pMainFrame = (CMainFrame*)theApp.m_pMainWnd;
			if( theApp.m_bPocketPC ) {
				menu.Attach( pMainFrame->m_wndCommandBar.GetMenu() );
			}else{
				menu.LoadMenu(IDR_MAINFRAME);
			}
			menu.GetSubMenu(0)->TrackPopupMenu(flags,
				rect.left,
				rect.bottom,
				pMainFrame );
			menu.Detach();
			return TRUE;
		}
#else
		// �w���v�\��
		util::OpenByShellExecute( MZ3_CHM_HELPFILENAME );
#endif
		break;
#ifdef WINCE
	case VK_F2:
#endif
#ifndef WINCE
	case VK_APPS:
#endif
		OnAcceleratorContextMenu();
		return TRUE;
	case VK_BACK:
#ifndef WINCE
	case VK_ESCAPE:
#endif
		// ���f
		if (m_access) {
			::SendMessage(m_hWnd, WM_MZ3_ABORT, NULL, NULL);
		}
		break;

	case 'D':
		// Ctrl+Alt+D
		if ((GetAsyncKeyState(VK_CONTROL) & 0x8000) && (GetAsyncKeyState(VK_MENU) & 0x8000)) {
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
		break;

	case 'U':
		// Ctrl+Alt+U
		if ((GetAsyncKeyState(VK_CONTROL) & 0x8000) && (GetAsyncKeyState(VK_MENU) & 0x8000)) {
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
		}
	}

	return FALSE;
}

BOOL CMZ3View::PreTranslateMessage(MSG* pMsg)
{
	if (theApp.m_optionMng.m_bEnableIntervalCheck) {
		// ���b�Z�[�W�ɉ����āA����擾�̃L�����Z���������s��
		switch (pMsg->message) {
		case WM_KEYUP:
		case WM_KEYDOWN:
			ResetIntervalTimer();
			break;
		default:
			break;
		}
	}

	if (pMsg->message == WM_KEYUP) {
		BOOL r = OnKeyUp( pMsg );

		// KEYDOWN ���s�[�g�񐔂�������
		m_nKeydownRepeatCount = 0;

		if (r) {
			return r;
		}
	}
	else if (pMsg->message == WM_KEYDOWN) {
		// KEYDOWN ���s�[�g�񐔂��C���N�������g
		m_nKeydownRepeatCount ++;

		if (OnKeyDown( pMsg )) {
			return TRUE;
		}
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
	if (wcslen(theApp.m_loginMng.GetOwnerID()) == 0) {
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
					theApp.m_loginMng.SetOwnerID(
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
	m_infoEdit.SetWindowText( _T("") );

	m_categoryList.EnsureVisible( m_selGroup->focusedCategory, FALSE);

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
	if (m_bodyList.GetItemCount() != 0) {
		m_bodyList.SetFocus();
		m_hotList = &m_bodyList;

		// ��1�J�����ɕ\�����Ă�����e��\������B
		m_infoEdit.SetWindowText( 
			MyGetItemByBodyColType(&GetSelectedBodyItem(), m_selGroup->getSelectedCategory()->m_firstBodyColType, false) );

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
		if( m_access ) return TRUE;	// �A�N�Z�X���͖���
		// ����
		return TRUE;
	case VK_DOWN:
		// ���L�[�B
		// �J�e�S���[���X�g�ւ̃t�H�[�J�X�ړ�
		return CommandSetFocusCategoryList();
	case VK_LEFT:
		// ���L�[�B
		if( m_access ) return TRUE;	// �A�N�Z�X���͖���
		// �I��ύX
		return CommandSelectGroupTabBeforeItem();
	case VK_RIGHT:
		// �E�L�[�B
		if( m_access ) return TRUE;	// �A�N�Z�X���͖���
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
		if( m_access ) return TRUE;	// �A�N�Z�X���͖���

		// �O���[�v�^�u�̑I��ύX
		return CommandSelectGroupTabBeforeItem();

		// �O���[�v�^�u�Ɉړ�
//		return CommandSetFocusGroupTab();
		return TRUE;
	case VK_RIGHT:
		if( m_access ) return TRUE;	// �A�N�Z�X���͖���

		// �O���[�v�^�u�̑I��ύX
		return CommandSelectGroupTabNextItem();

		// �{�f�B���X�g�Ɉړ�
//		return CommandSetFocusBodyList();
	case VK_RETURN:
		if (m_selGroup->selectedCategory == m_selGroup->focusedCategory) {
			RetrieveCategoryItem();
		}
		else {
			// ��I�����ڂȂ̂ŁA�擾�����ƃ{�f�B�̕ύX�B
			// ��擾�ŁA���O������Ȃ烍�O����擾�B

			// �A�N�Z�X���͑I��s��
			if (m_access) {
				return TRUE;
			}
			m_selGroup->selectedCategory = m_selGroup->focusedCategory;

			OnMySelchangedCategoryList();
		}
		return TRUE;
	case VK_BACK:
#ifndef WINCE
	case VK_ESCAPE:
#endif
		if( m_access ) {
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
		switch( vKey ) {
		case VK_UP:
			if (m_bodyList.GetItemState(0, LVIS_FOCUSED) != FALSE) {
				// ��ԏ�B
				// �J�e�S���Ɉړ�

		//		if( m_access ) return TRUE;	// �A�N�Z�X���͋֎~

				// �I����Ԃ𖖔��ɁB�ȑO�̑I����Ԃ�Off�ɁB
				util::MySetListCtrlItemFocusedAndSelected( m_categoryList, m_selGroup->focusedCategory, false );
				m_selGroup->focusedCategory = m_categoryList.GetItemCount()-1;
				util::MySetListCtrlItemFocusedAndSelected( m_categoryList, m_selGroup->focusedCategory, true );
				
				return CommandSetFocusCategoryList();
			}else{
#ifdef WINCE
				// �f�t�H���g����
				return FALSE;
#else
				return CommandMoveUpBodyList();
#endif
			}
			break;

		case VK_DOWN:
			if (m_bodyList.GetItemState(m_bodyList.GetItemCount()-1, LVIS_FOCUSED) != FALSE) {
				// ��ԉ��Ȃ̂Ŗ����B
				if( m_access ) return TRUE;	// �A�N�Z�X���͋֎~
				return TRUE;
			}else{
#ifdef WINCE
				// �f�t�H���g����
				return FALSE;
#else
				return CommandMoveDownBodyList();
#endif
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
		if( m_access ) return TRUE;

		switch( GetSelectedBodyItem().GetAccessType() ) {
		case ACCESS_COMMUNITY:
		case ACCESS_TWITTER_USER:
			// ���j���[�\��
			PopupBodyMenu();
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
		if( m_access ) {
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
//	if( m_access ) return TRUE;	// �A�N�Z�X���͖���

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
			m_categoryList.Scroll( CSize(0, -m_categoryList.GetCountPerPage() * theApp.m_optionMng.GetFontHeight()) );

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
//	if( m_access ) return TRUE;	// �A�N�Z�X���͖���

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
			m_categoryList.Scroll( CSize(0, m_categoryList.GetCountPerPage() * theApp.m_optionMng.GetFontHeight()) );

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

//		if( m_access ) return TRUE;	// �A�N�Z�X���͋֎~

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
			util::MySetListCtrlItemFocusedAndSelected( m_bodyList, pCategory->selectedBody, true );

			// �ړ��悪��\���Ȃ������ɃX�N���[��
			if( !util::IsVisibleOnListBox( m_bodyList, pCategory->selectedBody ) ) {
				m_bodyList.Scroll( CSize(0, -m_bodyList.GetCountPerPage() * theApp.m_optionMng.GetFontHeight()) );

				// �ĕ`��
				if (theApp.m_optionMng.IsUseBgImage()) {
					m_bodyList.RedrawItems(0, m_bodyList.GetItemCount());
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
		if( m_access ) return TRUE;	// �A�N�Z�X���͋֎~
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
				m_bodyList.Scroll( CSize(0, m_bodyList.GetCountPerPage() * theApp.m_optionMng.GetFontHeight()) );

				// �ĕ`��
				if (theApp.m_optionMng.IsUseBgImage()) {
					m_bodyList.RedrawItems(0, m_bodyList.GetItemCount());
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
	if( theApp.m_bPocketPC ) {
#ifdef WINCE
		CMainFrame* pFrame;
		pFrame = (CMainFrame*)(theApp.GetMainWnd());
		pCmdUI->Enable(pFrame->m_wndCommandBar.GetToolBarCtrl().IsButtonEnabled(ID_WRITE_BUTTON));  
#endif
	}
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
			util::OpenBrowserForUser( url, name );
		}
		break;
	default:
		util::OpenBrowserForUrl( url );
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
	if( m_access ) {
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

	CString uri;

	// �A�N�Z�X��ʂ�ݒ�
	theApp.m_accessType = data->GetAccessType();

	switch (data->GetAccessType()) {
	case ACCESS_BBS:
	case ACCESS_ENQUETE:
		// �R�~���j�e�B�A�A���P�[�g�Ȃ�A�擾������ύX
		switch( theApp.m_optionMng.GetPageType() ) {
		case GETPAGE_LATEST20:
			// �ŐV�Q�O���擾
			{
				uri = a_url;
			}
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
	default:
		uri = a_url;
		break;
	}

	// ���t�@��
	CString referer;
	if (data->GetAccessType() == ACCESS_ENQUETE) {
		// �A���P�[�g�̏ꍇ�̓��t�@���[��ݒ�
		referer = _T("http://mixi.jp/") + data->GetURL();
		referer.Replace(_T("view_enquete"), _T("reply_enquete"));
	}

	// �yAPI �p�z
	// URL ����ID�u��
	uri.Replace( L"{owner_id}", theApp.m_loginMng.GetOwnerID() );

	data->SetBrowseUri(uri);

	// encoding �w��
	switch (data->GetAccessType()) {
	case ACCESS_LIST_FOOTSTEP:
		// mixi API => UTF-8
		encoding = CInetAccess::ENCODING_UTF8;
		break;

	case ACCESS_TWITTER_FRIENDS_TIMELINE:
		// Twitter API => UTF-8
		encoding = CInetAccess::ENCODING_UTF8;
		break;
	}

	// �F�؏��̐ݒ�
	LPCTSTR szUser = NULL;
	LPCTSTR szPassword = NULL;
	switch (data->GetAccessType()) {
	case ACCESS_TWITTER_FRIENDS_TIMELINE:
		// Twitter API => Basic �F��
		szUser     = theApp.m_loginMng.GetTwitterId();
		szPassword = theApp.m_loginMng.GetTwitterPassword();

		// ���w��̏ꍇ�̓G���[�o��
		if (wcslen(szUser)==0 || wcslen(szPassword)==0) {
			MessageBox( L"���O�C���ݒ��ʂŃ��[�UID�ƃp�X���[�h��ݒ肵�Ă�������" );
			return;
		}
		break;
	}

	// ���~�{�^�����g�p�ɂ���
	theApp.EnableCommandBarButton( ID_STOP_BUTTON, TRUE);

	// �A�N�Z�X�J�n
	m_access = TRUE;
	m_abort = FALSE;

	theApp.m_inet.Initialize( m_hWnd, data, encoding );
	theApp.m_inet.DoGet(uri, referer, CInetAccess::FILE_HTML, szUser, szPassword );
}

/// �E�\�t�g�L�[���j���[�b�S���ǂ�
void CMZ3View::OnGetAll()
{
	// �`�F�b�N
	if( m_hotList != &m_bodyList ) {
		return;
	}
	switch( GetSelectedBodyItem().GetAccessType() ) {
	case ACCESS_BBS:
	case ACCESS_ENQUETE:
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
	if( m_hotList != &m_bodyList ) {
		return;
	}
	switch( GetSelectedBodyItem().GetAccessType() ) {
	case ACCESS_BBS:
	case ACCESS_ENQUETE:
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
	// �X�^�C���ύX
	VIEW_STYLE newStyle = MyGetViewStyleForSelectedCategory();
	if (newStyle!=m_viewStyle) {
		m_viewStyle = newStyle;
		MySetLayout(0,0);
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
		msgHead.Format( L"%s : ", util::AccessType2Message(category.m_mixi.GetAccessType()) );

		// HTML �̎擾
		util::MySetInformationText( m_hWnd, msgHead + _T("HTML��͒� : 1/3") );
		CHtmlArray html;
		html.Load( logfile );

		// HTML ���
		util::MySetInformationText( m_hWnd, msgHead + _T("HTML��͒� : 2/3") );
		mixi::MyDoParseMixiListHtml( category.m_mixi.GetAccessType(), body, html );

		// �{�f�B�ꗗ�̐ݒ�
		util::MySetInformationText( m_hWnd, msgHead + _T("HTML��͒� : 3/3") );

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
	case ACCESS_BBS:
	case ACCESS_ENQUETE:
	case ACCESS_EVENT:
	case ACCESS_PROFILE:
	case ACCESS_MYDIARY:
	case ACCESS_MESSAGE:
	case ACCESS_NEWS:
		// �T�|�[�g���Ă���
		break;
	default:
		// ���T�|�[�g�Ȃ̂ŏI������
		{
			CString msg = L"���̌`���̃��O�̓T�|�[�g���Ă��܂��� : ";
			msg += util::AccessType2Message(mixi.GetAccessType());
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
	mixi::MyDoParseMixiHtml( mixi.GetAccessType(), mixi, html );
	util::MySetInformationText( m_hWnd, L"����" );
}

/**
 * ���|�[�g�r���[���J���Amixi �f�[�^���Q�Ƃ���
 */
void CMZ3View::MyShowReportView(CMixiData& mixi)
{
	// ���|�[�g�r���[�ɑJ��
	util::MySetInformationText( m_hWnd, L"wait..." );
	theApp.m_pReportView->SetData( mixi );

	theApp.EnableCommandBarButton( ID_BACK_BUTTON, TRUE );
	theApp.EnableCommandBarButton( ID_FORWARD_BUTTON, (theApp.m_pWriteView->m_sendEnd == FALSE) ? TRUE : FALSE );
	theApp.ChangeView( theApp.m_pReportView );
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
	// ��Ԃɉ����āA�u�\�����ځv�̕ύX���s��
	// ������g�O������B
	CCategoryItem* pCategory = m_selGroup->getSelectedCategory();
	if (pCategory == NULL) {
		return false;
	}
	switch (pCategory->m_mixi.GetAccessType()) {
	case ACCESS_LIST_DIARY:
	case ACCESS_LIST_NEW_COMMENT:
	case ACCESS_LIST_COMMENT:
	case ACCESS_LIST_NEWS:
	case ACCESS_LIST_NEW_BBS:
	case ACCESS_LIST_MESSAGE_IN:
	case ACCESS_LIST_MESSAGE_OUT:
	case ACCESS_LIST_NEW_BBS_COMMENT:
		// �u���O�v�Ɓu�����v
		if( pCategory->m_secondBodyColType == CCategoryItem::BODY_INDICATE_TYPE_NAME ) {
			pCategory->m_secondBodyColType = CCategoryItem::BODY_INDICATE_TYPE_DATE;
		}else{
			pCategory->m_secondBodyColType = CCategoryItem::BODY_INDICATE_TYPE_NAME;
		}
		break;
	case ACCESS_LIST_FAVORITE:
		// �u���t�v�Ɓu�{���v
		if( pCategory->m_secondBodyColType == CCategoryItem::BODY_INDICATE_TYPE_BODY ) {
			pCategory->m_secondBodyColType = CCategoryItem::BODY_INDICATE_TYPE_DATE;
		}else{
			pCategory->m_secondBodyColType = CCategoryItem::BODY_INDICATE_TYPE_BODY;
		}
		break;
	case ACCESS_TWITTER_FRIENDS_TIMELINE:
		// �u���t�v�Ɓu���O�v
		if( pCategory->m_secondBodyColType == CCategoryItem::BODY_INDICATE_TYPE_DATE ) {
			pCategory->m_secondBodyColType = CCategoryItem::BODY_INDICATE_TYPE_NAME;
		}else{
			pCategory->m_secondBodyColType = CCategoryItem::BODY_INDICATE_TYPE_DATE;
		}
		break;
	case ACCESS_LIST_FRIEND:
	case ACCESS_LIST_COMMUNITY:
	case ACCESS_LIST_FOOTSTEP:
	case ACCESS_LIST_BOOKMARK:
	case ACCESS_LIST_MYDIARY:
	case ACCESS_LIST_INTRO:
	case ACCESS_LIST_CALENDAR:
	default:
		// ����ȊO�ł͕ύX���Ȃ��̂ŏI���B
		return false;
	}

	// �w�b�_�[������̕ύX�i��Q�J�����̂݁j
	m_bodyList.SetHeader( NULL, 
		MyGetBodyHeaderColName2(pCategory->m_mixi, pCategory->m_secondBodyColType) );

	// �A�C�e���̍X�V
	INT_PTR count = pCategory->GetBodyList().size();
	for (int i=0; i<count; i++) {
		CMixiData& data = pCategory->GetBodyList()[i];
		// �Q�J������
		m_bodyList.SetItem( i, 1, LVIF_TEXT, 
			MyGetItemByBodyColType(&data,pCategory->m_secondBodyColType), 0, 0, 0, 0 );
	}

	// ��1�J�����ɕ\�����Ă�����e��\������B
	m_infoEdit.SetWindowText( 
		MyGetItemByBodyColType(&GetSelectedBodyItem(), m_selGroup->getSelectedCategory()->m_firstBodyColType, false) );

	// �A�C�R���ĕ`��
	InvalidateRect( m_rectIcon, FALSE );

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
	m_categoryList.Update( 0 );

	// �I��ύX���̏��������s����i���O�̓ǂݍ��݁j
	m_categoryList.m_bStopDraw = true;
	OnMySelchangedCategoryList();
	m_categoryList.m_bStopDraw = false;
#ifdef WINCE
	m_categoryList.Invalidate( FALSE );
#else
	m_categoryList.Invalidate( TRUE );
#endif

	// �A�C�R���ĕ`��
	InvalidateRect( m_rectIcon, FALSE );
}

/**
 * �O���[�v�^�u�x���ǂݍ��ݗp�X���b�h
 */
unsigned int CMZ3View::ReloadGroupTab_Thread( LPVOID This )
{
	CMZ3View* pView = (CMZ3View*)This;

	static bool s_reloading = false;
	if( s_reloading ) {
		return 0;
	}

	s_reloading = true;
	::Sleep( 10L );

	// �����O��őI���O���[�v���ω����Ă���΁A�ēǍ����s��
	int selectedGroup = pView->m_groupTab.GetCurSel();
	for(;;) {
		// �I��ύX���̏��������s����
		pView->OnSelchangedGroupTab();

		if( selectedGroup == pView->m_groupTab.GetCurSel() ) {
			// �����O�ƑI���O���[�v�������Ȃ̂ŏ��������Ƃ���B
			break;
		}else{
			// �����O�ƑI���O���[�v���Ⴄ�̂ŁA�ēǍ����s���B
			selectedGroup = pView->m_groupTab.GetCurSel();
		}
	}

	s_reloading = false;

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
	CString body = L"";
	CMixiData& mixi = GetSelectedBodyItem();
	for( u_int i=0; i<mixi.GetBodySize(); i++ ) {
		body.Append( mixi.GetBody(i) );
	}
	MessageBox( body );
}

/// ���ǂɂ���
void CMZ3View::OnSetNoRead()
{
	CMixiData& mixi = GetSelectedBodyItem();

	// ���OINI�t�@�C���̍��ڂ�������
	CString logId = util::GetLogIdString( mixi );
	theApp.m_logfile.DeleteRecord( util::my_wcstombs((LPCTSTR)logId), "Log" );

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
	theApp.m_logfile.SetValue(util::my_wcstombs((LPCTSTR)logId), (const char*)util::int2str_a(mixi.GetCommentCount()), "Log");

	// �r���[���X�V
	m_bodyList.Update( m_selGroup->getSelectedCategory()->selectedBody );
}

/// �{�f�B���X�g�ł̉E�N���b�N���j���[
bool CMZ3View::PopupBodyMenu(POINT pt_, int flags_)
{
	POINT pt    = pt_;
	int   flags = flags_;

	if (pt.x==0 && pt.y==0) {
		pt = util::GetPopupPos();
		flags = util::GetPopupFlags();
	}

	CMixiData& bodyItem = GetSelectedBodyItem();
	switch( bodyItem.GetAccessType() ) {
	case ACCESS_DIARY:
	case ACCESS_BBS:
	case ACCESS_ENQUETE:
	case ACCESS_EVENT:
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
		// �v���t�B�[���Ȃ�A�J�e�S�����ڂɉ����ď�����ύX����B�i�b��j
		switch( m_selGroup->getSelectedCategory()->m_mixi.GetAccessType() ) {
		case ACCESS_LIST_INTRO:			// �Љ
		case ACCESS_LIST_FAVORITE:		// ���C�ɓ���
		case ACCESS_LIST_FOOTSTEP:		// ������
		case ACCESS_LIST_FRIEND:		// �}�C�~�N�ꗗ
		case ACCESS_LIST_CALENDAR:		// �J�����_�[
			// ��������j���[�őI��
			{
				CMenu menu;
				menu.LoadMenu( IDR_PROFILE_ITEM_MENU );
				CMenu* pSubMenu = menu.GetSubMenu(0);

				ACCESS_TYPE categoryType = m_selGroup->getSelectedCategory()->m_mixi.GetAccessType();

				// ���C�ɓ���ȊO�ł́u���ȏЉ�v���폜�B
				if( categoryType != ACCESS_LIST_FAVORITE ) {
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

	case ACCESS_TWITTER_USER:
		{
			CMenu menu;
			menu.LoadMenu( IDR_TWITTER_MENU );
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
				}
			}

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
		CCategoryItem::BODY_INDICATE_TYPE_TITLE,
		CCategoryItem::BODY_INDICATE_TYPE_DATE,
		CCategoryItem::SAVE_TO_GROUPFILE_NO );

	return AppendCategoryList(categoryItem);
}

/// �R�~���j�e�B�̉E�\�t�g�L�[���j���[�b�g�s�b�N�ꗗ
void CMZ3View::OnViewBbsList()
{
	if( m_access ) {
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
	if( m_access ) {
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
	mixi::MyDoParseMixiHtml( mixi.GetAccessType(), mixi, html );
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
	mixi::MyDoParseMixiHtml( mixi.GetAccessType(), mixi, html );
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
	mixi::MyDoParseMixiHtml( mixi.GetAccessType(), mixi, html );
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
		pt = util::GetPopupPos();
		flags = util::GetPopupFlags();
	}

	CMenu menu;
	menu.LoadMenu(IDR_CATEGORY_MENU);
	CMenu* pSubMenu = menu.GetSubMenu(0);

	// ����ΏۈȊO�̃J�e�S���ł���Ώ��񃁃j���[�𖳌�������
	CCategoryItem* pCategory = m_selGroup->getFocusedCategory();
	if (pCategory != NULL) {
		switch( pCategory->m_mixi.GetAccessType() ) {
		case ACCESS_LIST_NEW_BBS:
		case ACCESS_LIST_NEWS:
		case ACCESS_LIST_MESSAGE_IN:
		case ACCESS_LIST_MESSAGE_OUT:
		case ACCESS_LIST_DIARY:
		case ACCESS_LIST_MYDIARY:
		case ACCESS_LIST_BBS:
	//	case ACCESS_TWITTER_FRIENDS_TIMELINE:
			// ����ΏۂȂ̂ŏ��񃁃j���[�𖳌������Ȃ�
			break;
		default:
			// ���񃁃j���[�𖳌�������
			pSubMenu->EnableMenuItem( IDM_CRUISE, MF_GRAYED | MF_BYCOMMAND );
			pSubMenu->EnableMenuItem( IDM_CHECK_CRUISE, MF_GRAYED | MF_BYCOMMAND );
			break;
		}

		// ����\��ς݂ł���΃`�F�b�N��t����B
		if( pCategory->m_bCruise ) {
			pSubMenu->CheckMenuItem( IDM_CHECK_CRUISE, MF_CHECKED );
		}else{
			pSubMenu->CheckMenuItem( IDM_CHECK_CRUISE, MF_UNCHECKED );
		}
	} else {
		pSubMenu->EnableMenuItem( IDM_CRUISE, MF_GRAYED | MF_BYCOMMAND );
		pSubMenu->EnableMenuItem( IDM_CHECK_CRUISE, MF_GRAYED | MF_BYCOMMAND );
	}

	// ���ڂ�ǉ�
	CMenu* pAppendMenu = pSubMenu->GetSubMenu(5);
	if (pAppendMenu) {
		// �_�~�[���폜
		pAppendMenu->RemoveMenu( ID_APPEND_MENU_BEGIN, MF_BYCOMMAND );

		Mz3GroupData template_data;
		template_data.initForTopPage();
		static CArray<CMenu, CMenu> subMenu;
		subMenu.RemoveAll();
		subMenu.SetSize( template_data.groups.size() );
		int menuId = ID_APPEND_MENU_BEGIN;
		for (unsigned int groupIdx=0; groupIdx<template_data.groups.size(); groupIdx++) {
			subMenu[groupIdx].CreatePopupMenu();

			CGroupItem& group = template_data.groups[groupIdx];

			// subMenu �ɃJ�e�S������ǉ�
			for (unsigned int ic=0; ic<group.categories.size(); ic++) {
				subMenu[groupIdx].AppendMenuW( MF_STRING, menuId, group.categories[ic].m_name );
				menuId ++;
			}

			pAppendMenu->AppendMenuW( MF_POPUP, (UINT)subMenu[groupIdx].m_hMenu, group.name );
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
/*
			if( GetFocus() != &m_categoryList ) {
				// �t�H�[�J�X���Ⴄ�̂ŁA�^�C�}�[���X�V�i�����N�b�o�܂ő҂j
				ResetIntervalTimer();
				return;
			}
*/
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

	CFormView::OnTimer(nIDEvent);
}

bool CMZ3View::RetrieveCategoryItem(void)
{
	// �A�N�Z�X���͍ăA�N�Z�X�s��
	if (m_access) {
		return false;
	}
	if (m_selGroup==NULL) {
		return false;
	}
	CCategoryItem* item = m_selGroup->getSelectedCategory();
	if (item==NULL) {
		return false;
	}
	if (item->m_mixi.GetAccessType() == ACCESS_LIST_BOOKMARK) {
		// �u�b�N�}�[�N�̓A�N�Z�X�Ȃ�
		SetBodyList( item->GetBodyList() );
	} else {
		// �C���^�[�l�b�g�ɃA�N�Z�X
		m_hotList = &m_bodyList;
		AccessProc( &item->m_mixi, util::CreateMixiUrl(item->m_mixi.GetURL()));
	}

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
 * mini�摜�E�B���h�E�̈ړ��i����я����j
 */
void CMZ3View::MoveMiniImageDlg(int idxBody/*=-1*/, int pointx/*=-1*/, int pointy/*=-1*/)
{
	if (!theApp.m_optionMng.m_bShowMainViewMiniImage) {
		return;
	}

	// mini�摜��ʐ���
	bool bDrawMiniImage = false;
	if (m_selGroup!=NULL) {
		CCategoryItem* pCategory = m_selGroup->getSelectedCategory();
		if (pCategory!=NULL) {
			// mini�摜�������[�h�ł���Ύ擾����
			if (idxBody<0 || idxBody>=(int)pCategory->m_body.size()) {
				idxBody = pCategory->selectedBody;
			}
			if (!pCategory->m_body.empty()) {
				const CMixiData& data = pCategory->m_body[ idxBody ];
				MyLoadMiniImage( data );

				// �v���t�B�[�� or �R�~���j�e�B�ŁA
				// ���摜������Ε\��

				CString path = util::MakeImageLogfilePath( data );
				if (!path.IsEmpty() ) {
					bDrawMiniImage = true;
				}
			}
		}
	}

	if (m_pMiniImageDlg != NULL) {
		if (!theApp.m_optionMng.m_bShowMainViewMiniImageDlg) {
			// �I�v�V������Off�Ȃ̂ŁA��ɔ�\��
			m_pMiniImageDlg->ShowWindow( SW_HIDE );
		} else {
			m_pMiniImageDlg->ShowWindow( bDrawMiniImage ? SW_SHOWNOACTIVATE : SW_HIDE );

			if (bDrawMiniImage) {
				CCategoryItem* pCategory = m_selGroup->getSelectedCategory();
				int idx = idxBody;
			
				CRect rectBodyList; 
				m_bodyList.GetWindowRect( &rectBodyList );

				CRect rect;
				m_bodyList.GetItemRect( idx, &rect, LVIR_BOUNDS );

				rect.OffsetRect( rectBodyList.left, rectBodyList.top );

				// �I�v�V�����Ŏw��
				const int w = theApp.m_optionMng.m_nMainViewMiniImageSize;
				const int h = theApp.m_optionMng.m_nMainViewMiniImageSize;

				// �Ƃ肠�����s�̒����ɕ`��B
				int delta = 5;
				int x = 0;
				if (pointx>=0) {
					x = rectBodyList.left +pointx +delta;
					if (x+w > rectBodyList.right) {
						// �{�f�B���X�g����͂ݏo���̂ō����ɕ`��B
						x = x -w -delta -delta;
					}
				} else {
					x = rect.left+32;
				}

				int y = 0;
				if (pointy>=0) {
					y = rectBodyList.top +pointy +delta;
					if (y+h > rectBodyList.bottom) {
						// �{�f�B���X�g����͂ݏo���̂ŏ㑤�ɕ`��B
						y = rectBodyList.top +pointy -h -delta;
					}
				} else {
					y = rect.bottom;
					if (y+h > rectBodyList.bottom) {
						// �{�f�B���X�g����͂ݏo���̂ŏ㑤�ɕ`��B
						y = rect.top -h;
					}
				}

				// ����ł��݂͂����ꍇ�i�X�N���[�����Ȃǁj�͔�\��
				if (y+h > rectBodyList.bottom || y<rectBodyList.top) {
					m_pMiniImageDlg->ShowWindow( SW_HIDE );
				} else {
					m_pMiniImageDlg->MoveWindow( x, y, w, h );
				}
			}
		}
	}
}

LRESULT CMZ3View::OnHideView(WPARAM wParam, LPARAM lParam)
{
	// �摜�E�B���h�E�̏���
	if (m_pMiniImageDlg != NULL) {
		m_pMiniImageDlg->ShowWindow( SW_HIDE );
	}

	return TRUE;
}

bool CMZ3View::MyLoadMiniImage(const CMixiData& mixi)
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
			if(! m_access ) {
				// �A�N�Z�X���͋֎~
				// �擾
				static CMixiData s_data;
				CMixiData dummy;
				s_data = dummy;
				s_data.SetAccessType( ACCESS_IMAGE );

				CString url = mixi.GetImage(0);

				// ���~�{�^�����g�p�ɂ���
				theApp.EnableCommandBarButton( ID_STOP_BUTTON, TRUE);

				// �A�N�Z�X��ʂ�ݒ�
				theApp.m_accessType = s_data.GetAccessType();

				// �A�N�Z�X�J�n
				m_access = TRUE;
				m_abort = FALSE;

				theApp.m_inet.Initialize( m_hWnd, &s_data );
				theApp.m_inet.DoGet(url, L"", CInetAccess::FILE_BINARY );
			}
		} else {
			// ���łɑ��݂���̂ŕ`��
			if (m_pMiniImageDlg!=NULL) {
				m_pMiniImageDlg->DrawImageFile( miniImagePath );
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
	theApp.m_optionMng.m_fontHeight = option::Option::normalizeFontSize( theApp.m_optionMng.m_fontHeight+1 );

	CMainFrame* pMainFrame = (CMainFrame*)theApp.m_pMainWnd;
	pMainFrame->ChangeAllViewFont();
}

/**
 * �t�H���g�k��
 */
void CMZ3View::OnAcceleratorFontShrink()
{
	theApp.m_optionMng.m_fontHeight = option::Option::normalizeFontSize( theApp.m_optionMng.m_fontHeight-1 );

	CMainFrame* pMainFrame = (CMainFrame*)theApp.m_pMainWnd;
	pMainFrame->ChangeAllViewFont();
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
	if (nFlags & MK_CONTROL) {
		// Ctrl+�z�C�[���Ŋg��E�k��
		// ���𑜓x�z�C�[���Ή��̂��߁Adelta �l��ݐς���B
		static int s_delta = 0;
		// �������t�ɂȂ�����L�����Z��
		if ((s_delta>0 && zDelta<0) || (s_delta<0 && zDelta>0)) {
			s_delta = 0;
		}
		s_delta += zDelta;

		if (s_delta>WHEEL_DELTA) {
			OnAcceleratorFontMagnify();
			s_delta -= WHEEL_DELTA;
		} else if (s_delta<-WHEEL_DELTA) {
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
	if (m_access) {
		// �A�N�Z�X���͍ăA�N�Z�X�s��
		return;
	}

	// �A�N�Z�X�J�n
	if (!RetrieveCategoryItem()) {
		return;
	}
}

/**
 * Twitter | �S����ǂ�
 */
void CMZ3View::OnMenuTwitterRead()
{
	CMixiData& data = GetSelectedBodyItem();

	// �{����1�s�ɕϊ����Ċ��蓖�āB
	CString item;
	for( u_int i=0; i<data.GetBodySize(); i++ ) {
		CString line = data.GetBody(i);
		while( line.Replace( L"\r\n", L"" ) );
		item.Append( line );
	}

	item.Append( L"\r\n" );
	item.Append( L"----\r\n" );
	item.AppendFormat( L"name : %s\r\n", data.GetAuthor() );
	item.AppendFormat( L"description : %s\r\n", data.GetTitle() );
	item.AppendFormat( L"%s\r\n", data.GetDate() );
	item.AppendFormat( L"id : %d\r\n", data.GetID() );
	item.AppendFormat( L"owner-id : %d\r\n", data.GetOwnerID() );

	if (data.GetChildrenSize()>=1) {
		CString source = data.GetChild(0).GetBody(0);
		mixi::ParserUtil::StripAllTags( source );
		item.AppendFormat( L"source : %s", source );
	}

	MessageBox( item, data.GetName() );
}

/**
 * Twitter | �����Ԃ�
 */
void CMZ3View::OnMenuTwitterReply()
{
	// ���͗̈�Ƀ��[�U�̃X�N���[������ǉ��B
	CString strStatus;
	GetDlgItemText( IDC_STATUS_EDIT, strStatus );

	CMixiData& data = GetSelectedBodyItem();
	strStatus.AppendFormat( L"@%s ", (LPCTSTR)data.GetName() );

	SetDlgItemText( IDC_STATUS_EDIT, strStatus );

	// �t�H�[�J�X�ړ��B
	GetDlgItem( IDC_STATUS_EDIT )->SetFocus();

	// End
	keybd_event( VK_END, 0, 0, 0 );
	keybd_event( VK_END, 0, KEYEVENTF_KEYUP, 0 );
}

/**
 * Twitter | �Ԃ₭
 */
void CMZ3View::OnMenuTwitterUpdate()
{
	// �t�H�[�J�X�ړ��B
	GetDlgItem( IDC_STATUS_EDIT )->SetFocus();
}

/**
 * Twitter | �z�[��
 */
void CMZ3View::OnMenuTwitterHome()
{
	CMixiData& data = GetSelectedBodyItem();
	util::OpenBrowserForUrl( util::FormatString(L"http://twitter.com/%s", data.GetName()) );
}

/**
 * Twitter | Favorites
 */
void CMZ3View::OnMenuTwitterFavorites()
{
	CMixiData& data = GetSelectedBodyItem();
	util::OpenBrowserForUrl( util::FormatString(L"http://twitter.com/%s/favorites", data.GetName()) );
}

/**
 * Twitter | �T�C�g
 */
void CMZ3View::OnMenuTwitterSite()
{
	CMixiData& data = GetSelectedBodyItem();
	util::OpenBrowserForUrl( data.GetURL() );
}

CMZ3View::VIEW_STYLE CMZ3View::MyGetViewStyleForSelectedCategory(void)
{
	if (m_selGroup!=NULL) {
		CCategoryItem* pCategory = m_selGroup->getSelectedCategory();
		if (pCategory!=NULL) {
			switch (pCategory->m_mixi.GetAccessType()) {
			case ACCESS_TWITTER_FRIENDS_TIMELINE:
				return VIEW_STYLE_TWITTER;
			default:
				if (m_bodyList.IsEnableIcon()) {
					CImageList* pImageList = m_bodyList.GetImageList(LVSIL_SMALL);
					if (pImageList != NULL &&
						pImageList->m_hImageList == theApp.m_imageCache.GetImageList().m_hImageList)
					{
						return VIEW_STYLE_IMAGE;
					}
				}
				break;
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
	if (m_access) {
		return;
	}

	CString strStatus;
	GetDlgItemText( IDC_STATUS_EDIT, strStatus );

	if (strStatus.IsEmpty()) {
		return;
	}

	static CPostData post;
	post.ClearPostBody();

	// �w�b�_�[��ݒ�
	post.AppendAdditionalHeader( util::FormatString( L"X-Twitter-Client: %s", MZ3_APP_NAME ) );
	post.AppendAdditionalHeader( util::FormatString( L"X-Twitter-Client-URL: %s", L"http://mz3.jp/" ) );
	post.AppendAdditionalHeader( util::FormatString( L"X-Twitter-Client-Version: %s", MZ3_VERSION_TEXT_SHORT ) );

	// POST �p�����[�^��ݒ�
	post.AppendPostBody( "status=" );
	post.AppendPostBody( URLEncoder::encode_utf8(strStatus) );
	if (theApp.m_optionMng.m_bAddSourceTextOnTwitterPost) {
		post.AppendPostBody( L" *" MZ3_APP_NAME L"*" );
	}
	post.AppendPostBody( "&source=" );
	post.AppendPostBody( MZ3_APP_NAME );

	post.SetContentType( CONTENT_TYPE_FORM_URLENCODED );

	post.SetSuccessMessage( WM_MZ3_POST_END );

	CString url = L"http://twitter.com/statuses/update.xml";

	// Twitter API => Basic �F��
	LPCTSTR szUser = NULL;
	LPCTSTR szPassword = NULL;
	szUser     = theApp.m_loginMng.GetTwitterId();
	szPassword = theApp.m_loginMng.GetTwitterPassword();

	// ���w��̏ꍇ�̓G���[�o��
	if (wcslen(szUser)==0 || wcslen(szPassword)==0) {
		MessageBox( L"���O�C���ݒ��ʂŃ��[�UID�ƃp�X���[�h��ݒ肵�Ă�������" );
		return;
	}

	// ���~�{�^�����g�p�ɂ���
	theApp.EnableCommandBarButton( ID_STOP_BUTTON, TRUE);

	// �A�N�Z�X��ʂ�ݒ�
	theApp.m_accessType = ACCESS_TWITTER_UPDATE;

	// �A�N�Z�X�J�n
	m_access = TRUE;
	m_abort = FALSE;

	theApp.m_inet.Initialize( m_hWnd, NULL );
	theApp.m_inet.DoPost(
		url, 
		L"", 
		CInetAccess::FILE_HTML, 
		&post, szUser, szPassword );

//	CPostData::post_array& buf = post.GetPostBody();
//	MessageBox( CStringW(&buf[0], buf.size()) );
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
	m_access = FALSE;

	// �����Twitter�̂ݑΉ��B
	// HTTP�X�e�[�^�X�`�F�b�N���s���B
	LPCTSTR szStatusErrorMessage = twitter::CheckHttpResponseStatus( theApp.m_inet.m_dwHttpStatus );
	if (szStatusErrorMessage!=NULL) {
		CString msg = util::FormatString(L"�T�[�o�G���[(%d)�F%s", theApp.m_inet.m_dwHttpStatus, szStatusErrorMessage);
		util::MySetInformationText( m_hWnd, msg );
		MZ3LOGGER_ERROR( msg );
	} else {
		util::MySetInformationText( m_hWnd, L"�X�e�[�^�X���M�I��" );

		// ���͒l������
		SetDlgItemText( IDC_STATUS_EDIT, L"" );
	}

	// �t�H�[�J�X����͗̈�Ɉړ�
	GetDlgItem( IDC_STATUS_EDIT )->SetFocus();

	// �v���O���X�o�[���\��
	mc_progressBar.ShowWindow( SW_HIDE );

	theApp.EnableCommandBarButton( ID_STOP_BUTTON, FALSE);

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
	util::OpenBrowserForUrl( url );
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

			const CMixiData& data = GetSelectedBodyItem();
			CString path = util::MakeImageLogfilePath( data );
			if (!path.IsEmpty() ) {
				// ���̈�̍����ɕ`�悷��B
				const CRect& rectIcon = m_rectIcon;

				CMZ3BackgroundImage image(L"");
				image.load( path );
				if (image.isEnableImage()) {
					// ���T�C�Y����B
					CMZ3BackgroundImage resizedImage(L"");
					util::MakeResizedImage( this, resizedImage, image, rectIcon.Width(), rectIcon.Height() );

					util::DrawBitmap( dc.GetSafeHdc(), resizedImage.getHandle(), 
						rectIcon.left, rectIcon.top, rectIcon.Width(), rectIcon.Height(), 0, 0 );

					bDrawFinished = true;
				}
			}

			if (!bDrawFinished) {
				// �h��Ԃ�
				dc.FillSolidRect( m_rectIcon, RGB(255,255,255) );
			}
		}
		break;
	}
}

void CMZ3View::OnMenuTwitterFriendTimeline()
{
	if( m_access ) {
		// �A�N�Z�X���͋֎~
		return;
	}

	// �^�C�����C�����ڂ̒ǉ�
	CMixiData& bodyItem = GetSelectedBodyItem();
	CCategoryItem categoryItem;
	categoryItem.init( 
		// ���O
		util::FormatString( L"+%s�̃^�C�����C��", bodyItem.GetName() ),
		util::FormatString( L"http://twitter.com/statuses/user_timeline/%s.xml", (LPCTSTR)bodyItem.GetName() ), 
		ACCESS_TWITTER_FRIENDS_TIMELINE, 
		m_selGroup->categories.size()+1,
		CCategoryItem::BODY_INDICATE_TYPE_BODY,
		CCategoryItem::BODY_INDICATE_TYPE_NAME,
		CCategoryItem::SAVE_TO_GROUPFILE_NO );
	AppendCategoryList(categoryItem);

	// �擾�J�n
	CCategoryItem* pCategoryItem = m_selGroup->getSelectedCategory();
	AccessProc( &pCategoryItem->m_mixi, util::CreateMixiUrl(pCategoryItem->m_mixi.GetURL()));
}

void CMZ3View::OnMenuTwitterFriendTimelineWithOthers()
{
	if( m_access ) {
		// �A�N�Z�X���͋֎~
		return;
	}

	// �^�C�����C�����ڂ̒ǉ�
	CMixiData& bodyItem = GetSelectedBodyItem();
	CCategoryItem categoryItem;
	categoryItem.init( 
		// ���O
		util::FormatString( L"+%s�̃^�C�����C��", bodyItem.GetName() ),
		util::FormatString( L"http://twitter.com/statuses/friends_timeline/%s.xml", (LPCTSTR)bodyItem.GetName() ), 
		ACCESS_TWITTER_FRIENDS_TIMELINE, 
		m_selGroup->categories.size()+1,
		CCategoryItem::BODY_INDICATE_TYPE_BODY,
		CCategoryItem::BODY_INDICATE_TYPE_NAME,
		CCategoryItem::SAVE_TO_GROUPFILE_NO );
	AppendCategoryList(categoryItem);

	// �擾�J�n
	CCategoryItem* pCategoryItem = m_selGroup->getSelectedCategory();
	AccessProc( &pCategoryItem->m_mixi, util::CreateMixiUrl(pCategoryItem->m_mixi.GetURL()));
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
	m_bodyList.Invalidate( FALSE );

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

	// ���j���[�\��
	pSubMenu->TrackPopupMenu(flags, pt.x, pt.y, this);

	return true;
}

void CMZ3View::OnAppendCategoryMenu(UINT nID)
{
	int idx = nID - ID_APPEND_MENU_BEGIN;

	Mz3GroupData template_data;
	template_data.initForTopPage();

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

				// �O���[�v��`�t�@�C���̕ۑ�
				theApp.SaveGroupData();
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

	if (pCategoryItem->bSaveToGroupFile) {
		pCategoryItem->bSaveToGroupFile = false;

		MessageBox( util::FormatString( L"[%s] �͎���N�����ɍ폜����܂�", (LPCTSTR)pCategoryItem->m_name ) );
	} else {
		pCategoryItem->bSaveToGroupFile = true;
	}

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
	dlg.SetMessage( L"�J�e�S���̃^�C�g������͂��Ă�������" );
	dlg.mc_strEdit = pCategoryItem->m_name;
	if (dlg.DoModal()==IDOK) {
		pCategoryItem->m_name = dlg.mc_strEdit;
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
		group.init( dlg.mc_strEdit, L"", ACCESS_GROUP_OTHERS );
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
