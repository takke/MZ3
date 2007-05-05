/// MZ3View.cpp : CMZ3View �N���X�̎���

#include "stdafx.h"
#include "MZ3.h"

#include "MZ3Doc.h"
#include "MZ3View.h"

#include "MixiData.h"
#include "CategoryItem.h"
#include "HtmlArray.h"
#include "ReportView.h"
#include "MainFrm.h"
#include "WriteView.h"
#include "util.h"
#include "MixiParser.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

/**
 * ���X�g�R���g���[���̃t�H�[�J�X��ԂƑI����Ԃ�ݒ肷��B
 */
inline void MySetListCtrlItemFocusedAndSelected( CListCtrl& listCtrl, int idx, bool bFocusedAndSelected )
{
	UINT nState = 0;
	if( bFocusedAndSelected ) {
		nState |= LVIS_FOCUSED | LVIS_SELECTED;
	}

	listCtrl.SetItemState( idx, nState, LVIS_FOCUSED | LVIS_SELECTED );
}


inline CString MyGetItemByBodyColType( CMixiData* data, CCategoryItem::BODY_INDICATE_TYPE bodyColType )
{
	switch( bodyColType ) {
	case CCategoryItem::BODY_INDICATE_TYPE_DATE:
		return data->GetDate();
	case CCategoryItem::BODY_INDICATE_TYPE_NAME:
		return data->GetName();
	case CCategoryItem::BODY_INDICATE_TYPE_TITLE:
		return data->GetTitle();
	case CCategoryItem::BODY_INDICATE_TYPE_BODY:
		{
			// �{����1�s�ɕϊ����Ċ��蓖�āB
			CString body;
			for( u_int i=0; i<data->GetBodySize(); i++ ) {
				CString line = data->GetBody(i);
				while( line.Replace( L"\r\n", L"" ) );
				body.Append( line );
			}
			return body.Left( 30 );
		}
	default:
		return L"";
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
		return L"";
	case ACCESS_LIST_INTRO:
		return L"�Љ";
	case ACCESS_LIST_NEW_BBS:
		switch( bodyIndicateType ) {
		case CCategoryItem::BODY_INDICATE_TYPE_NAME:
			return _T("�R�~���j�e�B>>");
		default:
			return _T("����>>");
		}
		break;
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
	default:
		return L"";
	}
}


// CMZ3View

IMPLEMENT_DYNCREATE(CMZ3View, CFormView)

BEGIN_MESSAGE_MAP(CMZ3View, CFormView)
	ON_WM_SIZE()
	ON_NOTIFY(NM_CLICK, IDC_HEADER_LIST, &CMZ3View::OnNMClickCategoryList)
	ON_NOTIFY(NM_DBLCLK, IDC_HEADER_LIST, &CMZ3View::OnNMDblclkCategoryList)

	ON_NOTIFY(LVN_ITEMCHANGED, IDC_HEADER_LIST, &CMZ3View::OnLvnItemchangedCategoryList)
    
	ON_MESSAGE(WM_MZ3_GET_END, OnGetEnd)
    ON_MESSAGE(WM_MZ3_GET_ERROR, OnGetError)
    ON_MESSAGE(WM_MZ3_GET_ABORT, OnGetAbort)
    ON_MESSAGE(WM_MZ3_ABORT, OnAbort)
    ON_MESSAGE(WM_MZ3_ACCESS_INFORMATION, OnAccessInformation)
	ON_MESSAGE(WM_MZ3_ACCESS_LOADED, OnAccessLoaded)
	ON_EN_SETFOCUS(IDC_INFO_EDIT, &CMZ3View::OnEnSetfocusInfoEdit)
	ON_NOTIFY(NM_DBLCLK, IDC_BODY_LIST, &CMZ3View::OnNMDblclkBodyList)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_BODY_LIST, &CMZ3View::OnLvnItemchangedBodyList)
    ON_MESSAGE(WM_MZ3_CHANGE_VIEW, OnChangeView)
	ON_COMMAND(ID_WRITE_DIARY, &CMZ3View::OnWriteDiary)
    ON_UPDATE_COMMAND_UI(ID_WRITE_BUTTON, OnUpdateWriteButton)
    ON_COMMAND(ID_WRITE_BUTTON, OnWriteButton)
	ON_NOTIFY(NM_SETFOCUS, IDC_BODY_LIST, &CMZ3View::OnNMSetfocusBodyList)
	ON_COMMAND(ID_OPEN_BROWSER, &CMZ3View::OnOpenBrowser)
	ON_NOTIFY(NM_SETFOCUS, IDC_HEADER_LIST, &CMZ3View::OnNMSetfocusHeaderList)
	ON_COMMAND(ID_SHOW_DEBUG_INFO, &CMZ3View::OnShowDebugInfo)
	ON_COMMAND(ID_GET_ALL, &CMZ3View::OnGetAll)
	ON_COMMAND(ID_GET_LAST10, &CMZ3View::OnGetLast10)
	ON_COMMAND(ID_VIEW_LOG, &CMZ3View::OnViewLog)
	ON_NOTIFY(HDN_ITEMCLICK, 0, &CMZ3View::OnHdnItemclickBodyList)
	ON_NOTIFY(TCN_SELCHANGE, IDC_GROUP_TAB, &CMZ3View::OnTcnSelchangeGroupTab)
	ON_COMMAND(ID_OPEN_BROWSER_USER, &CMZ3View::OnOpenBrowserUser)
	ON_COMMAND(ID_OPEN_INTRO, &CMZ3View::OnOpenIntro)
	ON_COMMAND(ID_OPEN_SELFINTRO, &CMZ3View::OnOpenSelfintro)
	ON_COMMAND(IDM_SET_NO_READ, &CMZ3View::OnSetNoRead)
	ON_COMMAND(IDM_VIEW_BBS_LIST, &CMZ3View::OnViewBbsList)
	ON_COMMAND(IDM_VIEW_BBS_LIST_LOG, &CMZ3View::OnViewBbsListLog)
	ON_COMMAND(IDM_CRUISE, &CMZ3View::OnCruise)
	ON_COMMAND(IDM_CHECK_CRUISE, &CMZ3View::OnCheckCruise)
	ON_COMMAND(ID_SEND_NEW_MESSAGE, &CMZ3View::OnSendNewMessage)
	ON_NOTIFY(HDN_ENDTRACK, 0, &CMZ3View::OnHdnEndtrackHeaderList)
END_MESSAGE_MAP()

// CMZ3View �R���X�g���N�V����/�f�X�g���N�V����

/**
 * �R���X�g���N�^
 */
CMZ3View::CMZ3View()
	: CFormView(CMZ3View::IDD)
{
	m_preCategory = 0;

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
	theApp.m_inet.Open();

	m_login = FALSE;
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
			m_categoryList.InsertColumn(0, _T(""), LVCFMT_LEFT, 250, -1);
			m_categoryList.InsertColumn(1, _T(""), LVCFMT_LEFT, 200, -1);
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

		// �J�����쐬
		// ����������������ɍĐݒ肷��̂ŉ��̕����w�肵�Ă����B
		if( theApp.GetDisplayMode() == SR_VGA ){
			m_bodyList.InsertColumn(0, _T("�^�C�g��"), LVCFMT_LEFT, 240, -1);
			m_bodyList.InsertColumn(1, _T("���O"), LVCFMT_LEFT, 210, -1);
		}else{
			m_bodyList.InsertColumn(0, _T("�^�C�g��"), LVCFMT_LEFT, 120, -1);
			m_bodyList.InsertColumn(1, _T("���O"), LVCFMT_LEFT, 105, -1);
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
	m_groupTab.SetCurSel( 0 );

	// �I�𒆂̃O���[�v���ڂ̐ݒ�
	m_selGroup = &theApp.m_root.groups[0];

	// �J�e�S���[���X�g������������
	MyUpdateCategoryListByGroupItem();

//	m_categoryList.SetFocus();
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

	int fontHeight = theApp.m_optionMng.m_fontHeight;
	if( fontHeight == 0 ) {
		fontHeight = 24;
	}

	// ��ʉ����̏��̈�
	int hInfo     = fontHeight +12;
	if( theApp.GetDisplayMode() == SR_QVGA ) {
		hInfo     = fontHeight - 4;
	}

	// �O���[�v�^�u
	int hGroup    = fontHeight +12;
	if( theApp.GetDisplayMode() == SR_QVGA ) {
		hGroup -= 4;
	}

	// �J�e�S���A�{�f�B���X�g�̗̈�� % �Ŏw��
	// �i�A���A�J�e�S�����X�g�̓O���[�v�^�u���A�{�f�B���X�g�͏��̈���܂ށj
	const int h1 = theApp.m_optionMng.m_nMainViewCategoryListHeightRatio;
	const int h2 = theApp.m_optionMng.m_nMainViewBodyListHeightRatio;

	int hCategory = (cy * h1 / (h1+h2)) - (hGroup - 1);
	int hBody     = (cy * h2 / (h1+h2)) - (hInfo - 1);

	int y = 0;
	GetDlgItem(IDC_GROUP_TAB)  ->MoveWindow( 0, y, cx, hGroup    );
	y += hGroup;
	GetDlgItem(IDC_HEADER_LIST)->MoveWindow( 0, y, cx, hCategory );
	y += hCategory;
	GetDlgItem(IDC_BODY_LIST)  ->MoveWindow( 0, y, cx, hBody     );
	y += hBody;
	GetDlgItem(IDC_INFO_EDIT)  ->MoveWindow( 0, y, cx, hInfo     );

	// �v���O���X�o�[�͕ʓr�z�u
	// �T�C�Y�� hInfo �� 2/3 �Ƃ���
	int hProgress = hInfo * 2 / 3;
	y = cy - hInfo - hProgress;
	GetDlgItem(IDC_PROGRESS_BAR)->MoveWindow( 0, y, cx, hProgress );

	// ���X�g�J�������̕ύX
	ResetColumnWidth();
}

/**
 * �J�e�S�����X�g�N���b�N���̏���
 */
void CMZ3View::OnNMClickCategoryList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW lpnmlv = (LPNMLISTVIEW)pNMHDR;

	m_hotList = &m_categoryList;

	// �J�e�S�����X�g���́u���ݑI������Ă��鍀�ځv���X�V
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

	CCategoryItem* pCategory = m_selGroup->getSelectedCategory();
	TRACE(_T("http://mixi.jp/%s\n"), pCategory->m_mixi.GetURL());

	if (pCategory->m_mixi.GetAccessType() == ACCESS_LIST_BOOKMARK) {
		theApp.m_root.GetBookmarkList().clear();
		theApp.m_bookmarkMng.Load( theApp.m_root.GetBookmarkList() );
		SetBodyList( pCategory->GetBodyList() );
		return;
	}else{
		// �C���^�[�l�b�g�ɃA�N�Z�X
		AccessProc( &pCategory->m_mixi, util::CreateMixiUrl(pCategory->m_mixi.GetURL()));
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
 * �A�N�Z�X�I���ʒm��M
 */
LRESULT CMZ3View::OnGetEnd(WPARAM wParam, LPARAM lParam)
{
	TRACE(_T("InetAccess End\n"));

	util::MySetInformationText( m_hWnd, _T("HTML��͒�") );

//	util::StopWatch sw;

	if (m_abort) {
		::SendMessage(m_hWnd, WM_MZ3_GET_ABORT, NULL, lParam);
		return LRESULT();
	}

	if (lParam == NULL) {
		// �f�[�^���m�t�k�k�̏ꍇ
		TRACE(_T("�A�N�Z�X�߂�f�[�^�m�t�k�k\n"));
		util::MySetInformationText( m_hWnd, L"�����G���[���������܂���(�߂�l��NULL)" );
		return LRESULT();
	}

	CMixiData* data = (CMixiData*)lParam;
	ACCESS_TYPE aType = data->GetAccessType();

	// ���O�C���y�[�W�ȊO�ł���΁A�ŏ��Ƀ��O�A�E�g�`�F�b�N���s���Ă���
	if (aType != ACCESS_LOGIN ) {
		// HTML �̎擾

		// ���O�A�E�g�`�F�b�N
		if (mixi::LoginPageParser::isLogout(theApp.m_filepath.temphtml) ) {

			// ���O�A�E�g��ԂɂȂ��Ă���
//			MessageBox(_T("���O�A�E�g���Ă܂�\n"));
			TRACE(_T("�ēx���O�C�����Ă���f�[�^���擾���܂��B\n"));

			util::MySetInformationText( m_hWnd, L"�ēx���O�C�����Ă���f�[�^���擾���܂�" );

			// mixi �f�[�^��ۑ��i�Ҕ��j
			theApp.m_mixiBeforeRelogin = *data;
			data->SetURL( theApp.MakeLoginUrl() );

			// ���O�C�����s
			data->SetAccessType( ACCESS_LOGIN );
			AccessProc( data, data->GetURL() );

			return LRESULT();
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
				m_login = TRUE;
				if (wcslen(theApp.m_loginMng.GetOwnerID()) == 0) {
					// �I�[�i�[�h�c���擾����
					data->SetAccessType(ACCESS_MAIN); // �A�N�Z�X��ʂ�ݒ�
					AccessProc(data, _T("http://mixi.jp/home.pl"));
				}
				else {
					// �f�[�^��Ҕ��f�[�^�ɖ߂�
					*data = theApp.m_mixiBeforeRelogin;
					AccessProc(data, util::CreateMixiUrl(data->GetURL()));
				}
				return LRESULT();
			} else {
				// ���O�C�����s
				::MessageBox(m_hWnd, _T("���O�C���o���܂���ł���"), _T("MZ3"), MB_ICONSTOP | MB_OK);

				// �f�[�^��Ҕ��f�[�^�ɖ߂�
				*data = theApp.m_mixiBeforeRelogin;

				m_access = FALSE;
				// �v���O���X�o�[���\��
				mc_progressBar.ShowWindow( SW_HIDE );
				util::MySetInformationText( m_hWnd, _T("���O�C���o���܂���ł���") );
				return LRESULT();
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

				m_checkNewComment = FALSE;

				theApp.EnableCommandBarButton( ID_STOP_BUTTON, FALSE);

				break;
			}else{
				// �V�����b�Z�[�W�ȊO�Ȃ̂ŁA���O�C���̂��߂̎擾�������B

				// �f�[�^��Ҕ��f�[�^�ɖ߂�
				*data = theApp.m_mixiBeforeRelogin;

				AccessProc(data, util::CreateMixiUrl(data->GetURL()));

				return LRESULT();
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
	case ACCESS_LIST_MYDIARY:
	case ACCESS_LIST_FOOTSTEP:
	case ACCESS_LIST_MESSAGE_IN:
	case ACCESS_LIST_MESSAGE_OUT:
	case ACCESS_LIST_INTRO:
	case ACCESS_LIST_BBS:
		// --------------------------------------------------
		// �J�e�S�����ڂ̎擾
		// --------------------------------------------------
		{
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
			body.clear();

			// HTML ���
			util::MySetInformationText( m_hWnd,  _T("HTML��͒� : 2/3") );
			mixi::MyDoParseMixiHtml( aType, body, html );

			// �{�f�B�ꗗ�̐ݒ�
			util::MySetInformationText( m_hWnd,  _T("HTML��͒� : 3/3") );

			// �擾����������̍쐬
			CString timeStr;
			{
				SYSTEMTIME localTime;
				GetLocalTime(&localTime);
				timeStr.Format( _T("%02d/%02d %02d:%02d:%02d"),
					localTime.wMonth,
					localTime.wDay,
					localTime.wHour,
					localTime.wMinute,
					localTime.wSecond);
			}
			m_selGroup->getSelectedCategory()->SetAccessTime( timeStr );
			m_categoryList.SetItemText( m_selGroup->selectedCategory, 1, m_selGroup->getSelectedCategory()->GetAccessTime() );
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

				// �擾����
				CMixiData& mixi = GetSelectedBodyItem();
				AccessProc( &mixi, util::CreateMixiUrl(mixi.GetURL()) );

				// �ʐM�p���̂��߂����� return ����
				return TRUE;
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
		// --------------------------------------------------
		// �{�f�B���ڂ̎擾
		// --------------------------------------------------

		// ���񃂁[�h�Ȃ�i��͂����Ɂj���̏���ΏۂɈړ�����
		if( m_cruise.enable() ) {
			// ���̏���Ώۂ�����Ύ擾����
			// �Ȃ���ΏI������
			m_cruise.targetBodyItem++;
			CMixiDataList& bodyList = m_selGroup->getSelectedCategory()->GetBodyList();
			if( m_cruise.targetBodyItem >= (int)bodyList.size() ) {
				// ����I��
				if( m_cruise.autoCruise ) {
					// �\�񏄉�Ȃ̂Ŏ��ɐi��
					m_cruise.targetCategoryIndex++;
					CruiseToNextCategory();
				}else{
					// �ꎞ����Ȃ̂ł����ŏI���B
					m_cruise.finish();
					MessageBox( L"���񊮗�" );
				}
			}else{
				// ���̏���Ώۂ��擾����

				// �i�{�f�B���X�g��́j���̗v�f�Ɉړ�����
				int idxSel  = m_selGroup->getSelectedCategory()->selectedBody;
				int idxNext = idxSel +1;
				MySetListCtrlItemFocusedAndSelected( m_bodyList, idxSel,  false );
				MySetListCtrlItemFocusedAndSelected( m_bodyList, idxNext, true );
				m_bodyList.EnsureVisible( idxNext, FALSE );

				// �擾����
				CMixiData& mixi = GetSelectedBodyItem();
				AccessProc( &mixi, util::CreateMixiUrl(mixi.GetURL()) );

				// �ʐM�p���̂��߂����� return ����
				return TRUE;
			}
		}else{
			// ���񃂁[�h�łȂ��̂ŁA��͂��ă��|�[�g��ʂ��J��
			MyParseMixiHtml( theApp.m_filepath.temphtml, *data );
			MyShowReportView( *data );
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

	return LRESULT();
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
	::MessageBox(m_hWnd, msg, _T("MZ3"), MB_ICONSTOP | MB_OK);

	m_access = FALSE;

	// �v���O���X�o�[���\��
	mc_progressBar.ShowWindow( SW_HIDE );

	return LRESULT();
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

	return LRESULT();
}

/**
 * ���f����
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
//	::MessageBox(m_hWnd, msg, _T("MZ3"), MB_ICONSTOP | MB_OK);

	return LRESULT();
}

/**
 * �A�N�Z�X���ʒm
 */
LRESULT CMZ3View::OnAccessInformation(WPARAM wParam, LPARAM lParam)
{
	m_infoEdit.SetWindowText( *(CString*)lParam );
	return LRESULT();
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
 * �{�f�B�Ƀf�[�^��ݒ�
 */
void CMZ3View::SetBodyList( CMixiDataList& body )
{
	m_nochange = TRUE;

	try{
		if (&body == &theApp.m_root.GetBookmarkList() ) {
			// �u�b�N�}�[�N�͐F�Â����Ȃ�
			m_bodyList.SetUseColor( FALSE );
		} else {
			m_bodyList.SetUseColor( TRUE );
		}
	}catch( ... ) {
	}

	// ���X�g�̃A�C�e�����폜
	m_bodyList.m_bStopDraw = true;
	m_bodyList.SetRedraw(FALSE);
	m_bodyList.DeleteAllItems();

	// �w�b�_�̕�����ύX
	CCategoryItem* pCategory = m_selGroup->getSelectedCategory();
	LPCTSTR szHeaderTitle2 = MyGetBodyHeaderColName2( pCategory->m_mixi, pCategory->m_secondBodyColType );
	switch (pCategory->m_mixi.GetAccessType()) {
	case ACCESS_LIST_DIARY:
	case ACCESS_LIST_NEW_COMMENT:
	case ACCESS_LIST_COMMENT:
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
	}

	// �A�C�e���̒ǉ�
	INT_PTR count = body.size();
	for (int i=0; i<count; i++) {
		CMixiData* data = &body[i];

		// �P�J������
		// �ǂ̍��ڂ�^���邩�́A�J�e�S�����ڃf�[�^���̎�ʂŌ��߂�
		int index = m_bodyList.InsertItem( i, MyGetItemByBodyColType(data,pCategory->m_firstBodyColType) );
		// �Q�J������
		m_bodyList.SetItemText( index, 1, MyGetItemByBodyColType(data,pCategory->m_secondBodyColType) );

		// �{�f�B�̍��ڂ� ItemData �ɂ� CMixiData �����蓖�Ă�B
		m_bodyList.SetItemData( index, (DWORD_PTR)data );
	}

	m_nochange = FALSE;
	MySetListCtrlItemFocusedAndSelected( m_bodyList, 0, true );

	m_bodyList.SetRedraw(TRUE);
	m_bodyList.m_bStopDraw = false;
	m_bodyList.Invalidate( FALSE );
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
	*pResult = 0;

	if (m_access) {
		// �A�N�Z�X���͍ăA�N�Z�X�s��
		return;
	}

	LPNMLISTVIEW lpnmlv = (LPNMLISTVIEW)pNMHDR;

	// �J�����g�f�[�^���擾
	m_hotList = &m_bodyList;
	m_selGroup->getSelectedCategory()->selectedBody = lpnmlv->iItem;

	CMixiData* data = (CMixiData*)m_bodyList.GetItemData(lpnmlv->iItem);
	TRACE(_T("http://mixi.jp/%s\n"), data->GetURL());

	if (data->GetAccessType() == ACCESS_LIST_FOOTSTEP) {
		return;
	}

	AccessProc(data, util::CreateMixiUrl(data->GetURL()));
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
	m_selGroup->getSelectedCategory()->selectedBody = pNMLV->iItem;

	// ��1�J�����ɕ\�����Ă�����e��\������B
//	m_infoEdit.SetWindowText( GetSelectedBodyItem().GetTitle() );
	m_infoEdit.SetWindowText( 
		MyGetItemByBodyColType(&GetSelectedBodyItem(), m_selGroup->getSelectedCategory()->m_firstBodyColType) );

	*pResult = 0;
}

BOOL CMZ3View::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYUP) {
		switch (pMsg->wParam) {
		case VK_F1:
			if( theApp.m_optionMng.m_bUseLeftSoftKey ) {
				// ���C�����j���[�̃|�b�v�A�b�v
				RECT rect;
				SystemParametersInfo(SPI_GETWORKAREA, 0, &rect, 0);

				CMenu menu;
				CMainFrame* pMainFrame = (CMainFrame*)theApp.m_pMainWnd;
				menu.Attach( pMainFrame->m_wndCommandBar.GetMenu() );
				menu.GetSubMenu(0)->TrackPopupMenu(TPM_CENTERALIGN | TPM_VCENTERALIGN,
					rect.left,
					rect.bottom,
					pMainFrame );
				menu.Detach();
				return TRUE;
			}
			break;
		case VK_F2:
			if( GetFocus() == &m_bodyList ) {
				// �{�f�B���X�g�ł̉E�N���b�N���j���[
				PopupBodyMenu();
			}else{
				// �J�e�S�����X�g�ł̉E�N���b�N
				RECT rect;
				SystemParametersInfo(SPI_GETWORKAREA, 0, &rect, 0);

				POINT pt;
				pt.x = (rect.right-rect.left) / 2;
				pt.y = (rect.bottom-rect.top) / 2;
				CMenu menu;
				menu.LoadMenu(IDR_CATEGORY_MENU);
				CMenu* pSubMenu = menu.GetSubMenu(0);

				// ����ΏۈȊO�̃J�e�S���ł���Ώ��񃁃j���[�𖳌�������
				switch( m_selGroup->getFocusedCategory()->m_mixi.GetAccessType() ) {
				case ACCESS_LIST_NEW_BBS:
				case ACCESS_LIST_NEWS:
				case ACCESS_LIST_MESSAGE_IN:
				case ACCESS_LIST_MESSAGE_OUT:
				case ACCESS_LIST_DIARY:
				case ACCESS_LIST_MYDIARY:
				case ACCESS_LIST_BBS:
					// ����ΏۂȂ̂ŏ��񃁃j���[�𖳌������Ȃ�
					break;
				default:
					// ���񃁃j���[�𖳌�������
					pSubMenu->EnableMenuItem( IDM_CRUISE, MF_GRAYED | MF_BYCOMMAND );
					pSubMenu->EnableMenuItem( IDM_CHECK_CRUISE, MF_GRAYED | MF_BYCOMMAND );
					break;
				}

				// ����\��ς݂ł���΃`�F�b�N��t����B
				if( m_selGroup->getFocusedCategory()->m_bCruise ) {
					pSubMenu->CheckMenuItem( IDM_CHECK_CRUISE, MF_CHECKED );
				}else{
					pSubMenu->CheckMenuItem( IDM_CHECK_CRUISE, MF_UNCHECKED );
				}

				// ���j���[�\��
				pSubMenu->TrackPopupMenu(TPM_CENTERALIGN | TPM_VCENTERALIGN, pt.x, pt.y, this);
			}
			return TRUE;
		case VK_BACK:
			// ���f
			if (m_access) {
				::SendMessage(m_hWnd, WM_MZ3_ABORT, NULL, NULL);
			}
			break;

		default:
			break;
		}
	}
	else if (pMsg->message == WM_KEYDOWN) {
		// ���ʏ���
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
	MySetListCtrlItemFocusedAndSelected( *m_hotList, m_selGroup->getSelectedCategory()->selectedBody, true );
	theApp.ChangeView(theApp.m_pMainView);

	// �A�C�e���̐���
	theApp.EnableCommandBarButton( ID_BACK_BUTTON, FALSE);
	theApp.EnableCommandBarButton( ID_IMAGE_BUTTON, FALSE);

	// �������݃{�^��
	// �����̓��L�̏ꍇ�͗L���ɁB
	theApp.EnableCommandBarButton( ID_WRITE_BUTTON,
		(m_selGroup->getSelectedCategory()->m_mixi.GetAccessType() == ACCESS_LIST_MYDIARY));

	theApp.EnableCommandBarButton( ID_OPEN_BROWSER, FALSE );

	return LRESULT();
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
			if( category.m_mixi.GetAccessType() == ACCESS_LIST_DIARY ) {
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
		m_infoEdit.SetWindowText( GetSelectedBodyItem().GetTitle() );

		// �I����Ԃ��X�V
		int idx = m_selGroup->getSelectedCategory()->selectedBody;
		MySetListCtrlItemFocusedAndSelected( m_bodyList, idx, true );
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
	switch( vKey ) {
	case VK_UP:
		if( m_access ) return TRUE;	// �A�N�Z�X���͖���
		if( m_categoryList.GetItemState(0, LVIS_FOCUSED) != FALSE ) {
			// ��ԏ�̍��ڂȂ疳��
			return TRUE;

			// ��ԏ�̍��ڑI�𒆂Ȃ�A�O���[�v�^�u�ցB
//			return CommandSetFocusGroupTab();

/*			// ��ԏ�̍��ڑI�𒆂Ȃ̂ŁA��ԉ��Ɉړ�
			MySetListCtrlItemFocusedAndSelected( m_categoryList, 0, false );
			MySetListCtrlItemFocusedAndSelected( m_categoryList, m_categoryList.GetItemCount()-1, true );
			m_categoryList.EnsureVisible( m_categoryList.GetItemCount()-1, FALSE );
			return TRUE;
*/		}
		break;
	case VK_DOWN:
		if( m_access ) return TRUE;	// �A�N�Z�X���͖���
		if( m_categoryList.GetItemState(m_categoryList.GetItemCount()-1, LVIS_FOCUSED) != FALSE ) {
			// ��ԉ��̍��ڑI�𒆂Ȃ�A�{�f�B���X�g�̐擪�ցB
			if (m_bodyList.GetItemCount() != 0) {
				// �I����Ԃ�擪�ɁB�ȑO�̑I����Ԃ�Off�ɁB
				MySetListCtrlItemFocusedAndSelected( m_bodyList, m_selGroup->getSelectedCategory()->selectedBody, false );
				m_selGroup->getSelectedCategory()->selectedBody = 0;
			}
			return CommandSetFocusBodyList();
/*			// ��ԉ��̍��ڑI�𒆂Ȃ̂ŁA��ԏ�Ɉړ�
			MySetListCtrlItemFocusedAndSelected( m_categoryList, m_categoryList.GetItemCount()-1, false );
			MySetListCtrlItemFocusedAndSelected( m_categoryList, 0, true );
			m_categoryList.EnsureVisible( 0, FALSE );
			return TRUE;
*/		}
		break;
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
			// �A�N�Z�X���͍ăA�N�Z�X�s��
			if (m_access) {
				return TRUE;
			}
			if (m_selGroup->getFocusedCategory()->m_mixi.GetAccessType() == ACCESS_LIST_BOOKMARK) {
				SetBodyList( m_selGroup->getFocusedCategory()->GetBodyList() );
				return TRUE;
			}
			// �C���^�[�l�b�g�ɃA�N�Z�X
			m_hotList = &m_bodyList;
			AccessProc( 
				&m_selGroup->getFocusedCategory()->m_mixi, 
				util::CreateMixiUrl(m_selGroup->getFocusedCategory()->m_mixi.GetURL()));
		}
		else {
			// ��I�����ڂȂ̂ŁA�擾�����ƃ{�f�B�̕ύX�B
			// ��擾�ŁA���O������Ȃ烍�O����擾�B
			m_selGroup->selectedCategory = m_selGroup->focusedCategory;

			OnMySelchangedCategoryList();
		}
		return TRUE;
	case VK_BACK:
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
 * �{�f�B���X�g�̃L�[�_�E���C�x���g
 */
BOOL CMZ3View::OnKeydownBodyList( WORD vKey )
{
	// �{�f�B�[���X�g�ł̃L�[����
	switch( vKey ) {
	case VK_UP:
		if( m_access ) return TRUE;	// �A�N�Z�X���͖���
		if (m_bodyList.GetItemState(0, LVIS_FOCUSED) != FALSE) {
			// ��ԏ�B
			// �J�e�S���Ɉړ�

			// �I����Ԃ𖖔��ɁB�ȑO�̑I����Ԃ�Off�ɁB
			MySetListCtrlItemFocusedAndSelected( m_categoryList, m_selGroup->focusedCategory, false );
			m_selGroup->focusedCategory = m_categoryList.GetItemCount()-1;
			MySetListCtrlItemFocusedAndSelected( m_categoryList, m_selGroup->focusedCategory, true );
			
			return CommandSetFocusCategoryList();
		}
		break;
	case VK_DOWN:
		if( m_access ) return TRUE;	// �A�N�Z�X���͖���
		if (m_bodyList.GetItemState(m_bodyList.GetItemCount()-1, LVIS_FOCUSED) != FALSE) {
			// ��ԉ��Ȃ疳���B
			return TRUE;
		}
		break;
	case VK_LEFT:
		// ���{�^���B
		// �V���[�g�J�b�g�ړ��B
		{
			int idxSel = m_selGroup->getSelectedCategory()->selectedBody;
			int nItem = m_bodyList.GetItemCount();

			MySetListCtrlItemFocusedAndSelected( m_bodyList, idxSel, false );
			if( idxSel == 0 ) {
				// ��ԏ� �� ��ԉ�
				MySetListCtrlItemFocusedAndSelected( m_bodyList, nItem-1, true );
				m_bodyList.EnsureVisible( nItem-1, FALSE);
				return TRUE;
			}else if( idxSel == nItem-1 ) {
				// ��ԉ�     �� ��ԏ�
				MySetListCtrlItemFocusedAndSelected( m_bodyList, 0, true );
				m_bodyList.EnsureVisible( 0, FALSE );
				return TRUE;
			}else if( idxSel < nItem/2 ) {
				// �������� �� ��ԏ�
				MySetListCtrlItemFocusedAndSelected( m_bodyList, 0, true );
				m_bodyList.EnsureVisible( 0, FALSE );
				return TRUE;
			}else{
				// ������艺 �� ��ԉ�
				MySetListCtrlItemFocusedAndSelected( m_bodyList, nItem-1, true );
				m_bodyList.EnsureVisible( nItem-1, FALSE);
				return TRUE;
			}
		}
		// �J�e�S���Ɉړ�
//		return CommandSetFocusCategoryList();
	case VK_RIGHT:
		// �E�{�^���ŁA�Q�ڂ̍��ڂ�ω�������
		MyChangeBodyHeader();
		return TRUE;

	case VK_RETURN:
		// �A�N�Z�X���͍ăA�N�Z�X�s��
		if( m_access ) return TRUE;

		switch( GetSelectedBodyItem().GetAccessType() ) {
		case ACCESS_COMMUNITY:
			// ���j���[�\��
			PopupBodyMenu();
			break;
		case ACCESS_PROFILE:
			// �v���t�B�[���Ȃ�A�J�e�S�����ڂɉ����ď�����ύX����B�i�b��j
			switch( m_selGroup->getSelectedCategory()->m_mixi.GetAccessType() ) {
			case ACCESS_LIST_INTRO:			// �Љ
			case ACCESS_LIST_FAVORITE:		// ���C�ɓ���
			case ACCESS_LIST_FOOTSTEP:		// ������
			case ACCESS_LIST_FRIEND:		// �}�C�~�N�ꗗ
				// ��������j���[�őI��
				{
					RECT rect;
					SystemParametersInfo(SPI_GETWORKAREA, 0, &rect, 0);

					POINT pt;
					pt.x = (rect.right-rect.left) / 2;
					pt.y = (rect.bottom-rect.top) / 2;
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

					// ���j���[���J��
					pSubMenu->TrackPopupMenu(TPM_CENTERALIGN | TPM_VCENTERALIGN,
						pt.x,
						pt.y,
						this);
				}

				break;
			}
			break;
		default:
			// ����ȗv�f�ȊO�Ȃ̂ŁA�ʐM�����J�n�B
			AccessProc( &GetSelectedBodyItem(), util::CreateMixiUrl(GetSelectedBodyItem().GetURL()));
			break;
		}
		return TRUE;
	case VK_BACK:
		if( m_access ) {
			// �A�N�Z�X���͖���
			return TRUE;
		}
		// ��A�N�Z�X���́A�J�e�S�����X�g�Ɉړ�����
		return CommandSetFocusCategoryList();
	}
	return FALSE;
}

/**
 * �������݃{�^���̐���
 */
void CMZ3View::OnUpdateWriteButton(CCmdUI* pCmdUI)
{
	CMainFrame* pFrame;
	pFrame = (CMainFrame*)(theApp.GetMainWnd());
	pCmdUI->Enable(pFrame->m_wndCommandBar.GetToolBarCtrl().IsButtonEnabled(ID_WRITE_BUTTON));  
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
	theApp.EnableCommandBarButton( ID_OPEN_BROWSER, 
		(GetSelectedBodyItem().GetAccessType() == ACCESS_LIST_FOOTSTEP) );

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
void CMZ3View::AccessProc(CMixiData* data, LPCTSTR a_url)
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
		case GETPAGE_LATEST10:
			// �ŐV�P�O���擾
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

	data->SetBrowseUri(uri);

	// ���~�{�^�����g�p�ɂ���
	theApp.EnableCommandBarButton( ID_STOP_BUTTON, TRUE);

	// �A�N�Z�X�J�n
	m_access = TRUE;
	m_abort = FALSE;

	theApp.m_inet.Initialize( m_hWnd, data );
	theApp.m_inet.DoGet(uri, referer, CInetAccess::FILE_HTML );
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

/// �E�\�t�g�L�[���j���[�b�ŐV��10����ǂ�
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

	// 10���ɐݒ肵�A�A�N�Z�X�J�n
	theApp.m_optionMng.SetPageType( GETPAGE_LATEST10 );
	AccessProc( &GetSelectedBodyItem(), util::CreateMixiUrl(GetSelectedBodyItem().GetURL()));
}

/**
 * �J�e�S�����X�g�̑I�����ڂ̕ύX���̏���
 */
void CMZ3View::OnMySelchangedCategoryList(void)
{
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

	// �I�����ڂ��u���擾�v�Ȃ�A�Ƃ肠�����t�@�C������擾����
	CCategoryItem* pCategory = m_selGroup->getSelectedCategory();
	if( wcscmp( pCategory->GetAccessTime(), L"" ) == 0 ) {
		MyLoadCategoryLogfile( *pCategory );
	}

	// �{�f�B���X�g�ɐݒ�
	SetBodyList( pCategory->GetBodyList() );
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
	CString time = status.m_mtime.Format( L"%m/%d %H:%M:%S log" );

	{
		CMixiDataList& body = category.GetBodyList();
		body.clear();

		CString msgHead;
		msgHead.Format( L"%s : ", util::AccessType2Message(category.m_mixi.GetAccessType()) );

		// HTML �̎擾
		util::MySetInformationText( m_hWnd, msgHead + _T("HTML��͒� : 1/3") );
		CHtmlArray html;
		html.Load( logfile );

		// HTML ���
		util::MySetInformationText( m_hWnd, msgHead + _T("HTML��͒� : 2/3") );
		mixi::MyDoParseMixiHtml( category.m_mixi.GetAccessType(), body, html );

		// �{�f�B�ꗗ�̐ݒ�
		util::MySetInformationText( m_hWnd, msgHead + _T("HTML��͒� : 3/3") );

		// �擾����������̐ݒ�
		category.SetAccessTime( time );
		m_categoryList.SetItemText( category.GetIndexOnList(), 1, category.GetAccessTime() );
	}

	return true;
}

/**
 * �u���O���J���v���j���[
 */
void CMZ3View::OnViewLog()
{
	CMixiData& mixi = GetSelectedBodyItem();
	CString strLogfilePath = util::MakeLogfilePath( mixi );

	MyParseMixiHtml( strLogfilePath, mixi );
	MyShowReportView( mixi );
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
	theApp.m_pReportView->SetData( &mixi );

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
	switch (pCategory->m_mixi.GetAccessType()) {
	case ACCESS_LIST_DIARY:
	case ACCESS_LIST_NEW_COMMENT:
	case ACCESS_LIST_COMMENT:
	case ACCESS_LIST_NEWS:
	case ACCESS_LIST_NEW_BBS:
	case ACCESS_LIST_MESSAGE_IN:
	case ACCESS_LIST_MESSAGE_OUT:
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
	case ACCESS_LIST_FRIEND:
	case ACCESS_LIST_COMMUNITY:
	case ACCESS_LIST_FOOTSTEP:
	case ACCESS_LIST_BOOKMARK:
	case ACCESS_LIST_MYDIARY:
	case ACCESS_LIST_INTRO:
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
			MySetListCtrlItemFocusedAndSelected( m_categoryList, i, i==m_selGroup->focusedCategory );
		}

		// �擾����������̐ݒ�
		m_categoryList.SetItemText( i, 1, category.GetAccessTime() );

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
		MySetListCtrlItemFocusedAndSelected( m_categoryList, m_selGroup->focusedCategory, true );
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
	m_categoryList.Invalidate( FALSE );
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

	// ��������� lastIndex ��������
	mixi.SetLastIndex( -1 );

	// ���OINI�t�@�C���̍��ڂ�������
	// �i�{���͍��ڂ��ƍ폜���ׂ��j
	theApp.m_logfile.SetValue( (LPCSTR)util::int2str_a(mixi.GetID()), "-1", "Log" );

	// ���O�t�@�C���폜
	DeleteFile( util::MakeLogfilePath(mixi) );

	// �r���[���X�V
	m_bodyList.Update( m_selGroup->getSelectedCategory()->selectedBody );
}

/// �{�f�B���X�g�ł̉E�N���b�N���j���[
bool CMZ3View::PopupBodyMenu(void)
{
	// �E�N���b�N���j���[�\���ʒu
	RECT rect;
	SystemParametersInfo(SPI_GETWORKAREA, 0, &rect, 0);

	POINT pt;
	pt.x = (rect.right-rect.left) / 2;
	pt.y = (rect.bottom-rect.top) / 2;

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
				// 10��or�S���̐؂�ւ����j���[���o��
				if( theApp.m_optionMng.GetPageType() == GETPAGE_ALL ) {
					// �u�S���v�Ƀ`�F�b�N
					pSubMenu->CheckMenuItem( ID_GET_ALL, MF_CHECKED | MF_BYCOMMAND );
				}
				if( theApp.m_optionMng.GetPageType() == GETPAGE_LATEST10 ) {
					// �u�ŐV��10���v�Ƀ`�F�b�N
					pSubMenu->CheckMenuItem( ID_GET_LAST10, MF_CHECKED | MF_BYCOMMAND );
				}
				break;
			default:
				// ����ȊO�ł́A�u�S���v�Ɓu�ŐV��10���v���폜�B
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
			pSubMenu->TrackPopupMenu( TPM_CENTERALIGN | TPM_VCENTERALIGN, pt.x, pt.y, this );
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
			pSubMenu->TrackPopupMenu( TPM_CENTERALIGN | TPM_VCENTERALIGN, pt.x, pt.y, this );
		}
		break;
	}
	return true;
}

/// �g�s�b�N�ꗗ�̉{������
bool CMZ3View::PrepareViewBbsList(void)
{
	CMixiData& bodyItem = GetSelectedBodyItem();

	// �O���[�v�ɃR�~���j�e�B��p���ڂ�ǉ�����B���ɂ���Ύ擾����B
	CCategoryItem* pCategoryItem = NULL;
	for( u_int i=0; i<m_selGroup->categories.size(); i++ ) {
		CCategoryItem& category = m_selGroup->categories[i];
		if( category.m_mixi.GetAccessType() == ACCESS_LIST_BBS ) {
			// �Y�����ڔ����B
			pCategoryItem = &category;
			break;
		}
	}
	// �������Ȃ�ǉ�
	if( pCategoryItem == NULL ) {
		CCategoryItem category;
		m_selGroup->categories.push_back( category );
		pCategoryItem = &m_selGroup->categories[ m_selGroup->categories.size()-1 ];
	}

	// ������
	
	// URL �̓{�f�B�̃A�C�e������id�������p���B
	CString url;
	url.Format( L"list_bbs.pl?id=%d", mixi::MixiUrlParser::GetID(bodyItem.GetURL()) );
	// ���O�͕�����悤�ɂ��Ă���
	CString name;
	name.Format( L"��%s", bodyItem.GetName() );
	pCategoryItem->init( name, url, ACCESS_LIST_BBS, m_selGroup->categories.size(),
		CCategoryItem::BODY_INDICATE_TYPE_TITLE,
		CCategoryItem::BODY_INDICATE_TYPE_DATE );

	// �^�u�̏�����
	MyUpdateCategoryListByGroupItem();

	// �J�e�S���̑I�����ڂ��ĕ\���B
	{
		int idxLast = m_selGroup->focusedCategory;
		int idxNew  = m_categoryList.GetItemCount()-1;

		MySetListCtrlItemFocusedAndSelected( m_categoryList, idxLast, false );
		MySetListCtrlItemFocusedAndSelected( m_categoryList, idxNew, true );
		m_selGroup->focusedCategory  = idxNew;
		m_selGroup->selectedCategory = idxNew;
		m_categoryList.SetActiveItem( idxNew );
		m_categoryList.Update( idxLast );
		m_categoryList.Update( idxNew );
	}

	// �t�H�[�J�X���J�e�S�����X�g�ɁB
	m_categoryList.SetFocus();

	// �{�f�B���X�g�͏������Ă����B
	m_bodyList.DeleteAllItems();
	m_bodyList.SetRedraw(TRUE);
	m_bodyList.Invalidate( FALSE );

	return true;
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
	mixi.SetTitle(L"MZ3 �w���v");
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
	mixi.SetTitle(L"MZ3 ���ŗ���");
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
}

bool CMZ3View::CruiseToNextCategory(void)
{
	// ���̏���J�e�S����T������
	if( !MoveToNextCruiseCategory() ) {
		// ���񊮗�
		m_cruise.finish();
		util::MySetInformationText( m_hWnd, L"���񊮗�" );

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
	MySetListCtrlItemFocusedAndSelected( m_categoryList, m_selGroup->focusedCategory, false );
	m_selGroup->selectedCategory = m_selGroup->focusedCategory = m_cruise.targetCategoryIndex;
	MySetListCtrlItemFocusedAndSelected( m_categoryList, m_selGroup->focusedCategory, true );
	CommandSetFocusCategoryList();
	OnMySelchangedCategoryList();

	// �t�H�[�J�X����y�C���ɁB


	// ����ΏۃJ�e�S���̉���
	CCategoryItem& targetCategory = theApp.m_root.groups[ m_cruise.targetGroupIndex ].categories[ m_cruise.targetCategoryIndex ];
	CMixiData& mixi = targetCategory.m_mixi;

//	CString msg;
//	msg.Format( L"%s, %d, %d", targetCategory.m_name, m_cruise.targetGroupIndex, m_cruise.targetCategoryIndex );
//	MessageBox( msg );

	AccessProc( &mixi, util::CreateMixiUrl(mixi.GetURL()));

	return true;
}

/**
 * ����J�n
 */
void CMZ3View::StartCruise(void)
{
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
//		CString msg;
//		msg.Format( L"%d of %d\n", m_cruise.targetGroupIndex, groups.size() );
//		MessageBox( msg );

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
	switch( theApp.GetDisplayMode() ) {
	case SR_VGA:
		w -= 35;
		break;
	case SR_QVGA:
	default:
		w -= 35/2;
		break;
	}
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
