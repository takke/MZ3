/*
 * Copyright (c) MZ3 Project Team All rights reserved.
 * Code licensed under the BSD License:
 * http://www.mz3.jp/license.txt
 */
// ReportView.cpp : �����t�@�C��
//

#include "stdafx.h"
#include "MZ3.h"
#include "ReportView.h"
#include "DownloadView.h"
#include "MainFrm.h"
#include "IniFile.h"
#include "WriteView.h"
#include "HtmlArray.h"
#include "util.h"
#include "util_gui.h"
#include "QuoteDlg.h"
#include "MixiParser.h"
#include "ViewFilter.h"
#include "CommonSelectDlg.h"
#include "Ran2View.h"

#define MASK_COLOR RGB(255,0,255);

// CReportView

IMPLEMENT_DYNCREATE(CReportView, CFormView)

/**
 * �R���X�g���N�^
 */
CReportView::CReportView()
	: CFormView(CReportView::IDD)
	, m_nKeydownRepeatCount(0)
	, m_currentData(NULL)
{
	m_imageState = FALSE;
	m_detailView = NULL;
}

/**
 * �f�X�g���N�^
 */
CReportView::~CReportView()
{
	delete m_detailView;
}

void CReportView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_REPORT_LIST, m_list);
	DDX_Control(pDX, IDC_TITLE_EDIT, m_titleEdit);
	DDX_Control(pDX, IDC_PROGRESS_BAR, mc_progressBar);
	DDX_Control(pDX, IDC_INFO_EDIT, m_infoEdit);
	DDX_Control(pDX, IDC_VSCROLLBAR, m_vScrollbar);
}

BEGIN_MESSAGE_MAP(CReportView, CFormView)
	ON_WM_SIZE()
	ON_WM_CTLCOLOR()
	ON_WM_VSCROLL()
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_REPORT_LIST, &CReportView::OnLvnItemchangedReportList)
	ON_NOTIFY(LVN_KEYDOWN, IDC_REPORT_LIST, &CReportView::OnLvnKeydownReportList)

	ON_MESSAGE(WM_MZ3_ACCESS_LOADED, OnAccessLoaded)
	ON_COMMAND(ID_ADD_BOOKMARK, &CReportView::OnAddBookmark)
	ON_COMMAND(ID_DEL_BOOKMARK, &CReportView::OnDelBookmark)
    ON_COMMAND_RANGE(ID_REPORT_IMAGE+1, ID_REPORT_IMAGE+50, OnLoadImage)
    ON_COMMAND_RANGE(ID_REPORT_MOVIE+1, ID_REPORT_MOVIE+50, OnLoadMovie)
	ON_COMMAND_RANGE(ID_REPORT_PAGE_LINK_BASE+1, ID_REPORT_PAGE_LINK_BASE+50, OnLoadPageLink)
	ON_COMMAND_RANGE(ID_REPORT_URL_BASE+1, ID_REPORT_URL_BASE+50, OnLoadUrl)
    ON_COMMAND(ID_IMAGE_BUTTON, OnImageButton)
	ON_COMMAND(IDM_RELOAD_PAGE, OnReloadPage)
    ON_MESSAGE(WM_MZ3_GET_END, OnGetEnd)
    ON_MESSAGE(WM_MZ3_GET_END_BINARY, OnGetEndBinary)
    ON_MESSAGE(WM_MZ3_GET_ERROR, OnGetError)
    ON_MESSAGE(WM_MZ3_GET_ABORT, OnGetAbort)
    ON_MESSAGE(WM_MZ3_ABORT, OnAbort)
    ON_MESSAGE(WM_MZ3_ACCESS_INFORMATION, OnAccessInformation)
    ON_MESSAGE(WM_MZ3_CHANGE_VIEW, OnChangeView)
    ON_MESSAGE(WM_MZ3_RELOAD, OnReload)
	ON_COMMAND(ID_WRITE_COMMENT, &CReportView::OnWriteComment)
	ON_COMMAND(ID_WRITE_MESSAGE, &CReportView::OnWriteComment)
    ON_COMMAND(ID_WRITE_BUTTON, OnWriteButton)
	ON_COMMAND(ID_EDIT_COPY, &CReportView::OnEditCopy)
    ON_MESSAGE(WM_MZ3_FIT, OnFit)
    ON_COMMAND(ID_OPEN_BROWSER, OnOpenBrowser)

	ON_COMMAND(ID_SHOW_DEBUG_INFO, &CReportView::OnShowDebugInfo)
	ON_COMMAND(ID_OPEN_BROWSER_USER, &CReportView::OnOpenBrowserUser)
	ON_UPDATE_COMMAND_UI(ID_WRITE_COMMENT, &CReportView::OnUpdateWriteComment)
	ON_COMMAND(ID_MENU_BACK, &CReportView::OnMenuBack)
	ON_COMMAND(ID_BACK_MENU, &CReportView::OnBackMenu)
	ON_COMMAND(ID_NEXT_MENU, &CReportView::OnNextMenu)
	ON_NOTIFY(HDN_ENDTRACK, 0, &CReportView::OnHdnEndtrackReportList)
	ON_COMMAND(IDM_LAYOUT_REPORTLIST_MAKE_NARROW, &CReportView::OnLayoutReportlistMakeNarrow)
	ON_COMMAND(IDM_LAYOUT_REPORTLIST_MAKE_WIDE, &CReportView::OnLayoutReportlistMakeWide)
	ON_NOTIFY(NM_RCLICK, IDC_REPORT_LIST, &CReportView::OnNMRclickReportList)
	ON_COMMAND(ID_OPEN_PROFILE, &CReportView::OnOpenProfile)
	ON_COMMAND(ID_OPEN_PROFILE_LOG, &CReportView::OnOpenProfileLog)
	ON_COMMAND(ID_SEND_MESSAGE, &CReportView::OnSendMessage)
	ON_WM_DESTROY()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_RBUTTONUP()
	ON_WM_MOUSEWHEEL()

	ON_MESSAGE(WM_MZ3_MOVE_DOWN_LIST, OnMoveDownList)
	ON_MESSAGE(WM_MZ3_MOVE_UP_LIST, OnMoveUpList)
	ON_MESSAGE(WM_MZ3_GET_LIST_ITEM_COUNT, OnGetListItemCount)

	ON_COMMAND_RANGE(ID_REPORT_COPY_URL_BASE+1, ID_REPORT_COPY_URL_BASE+50, OnCopyClipboardUrl)
	ON_COMMAND_RANGE(ID_REPORT_COPY_IMAGE+1, ID_REPORT_COPY_IMAGE+50, OnCopyClipboardImage)
	ON_COMMAND_RANGE(ID_REPORT_COPY_MOVIE+1, ID_REPORT_COPY_MOVIE+50, OnCopyClipboardMovie)
	ON_COMMAND(ID_MENU_NEXT_DIARY, &CReportView::OnMenuNextDiary)
	ON_COMMAND(ID_MENU_PREV_DIARY, &CReportView::OnMenuPrevDiary)
	ON_UPDATE_COMMAND_UI(ID_MENU_NEXT_DIARY, &CReportView::OnUpdateMenuNextDiary)
	ON_UPDATE_COMMAND_UI(ID_MENU_PREV_DIARY, &CReportView::OnUpdateMenuPrevDiary)
	ON_COMMAND(IDM_LOAD_FULL_DIARY, &CReportView::OnLoadFullDiary)
	ON_UPDATE_COMMAND_UI(IDM_LOAD_FULL_DIARY, &CReportView::OnUpdateLoadFullDiary)
	ON_COMMAND_RANGE(ID_LUA_MENU_BASE, ID_LUA_MENU_BASE+1000, OnLuaMenu)
END_MESSAGE_MAP()


// CReportView �f�f

#ifdef _DEBUG
void CReportView::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CReportView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


// CReportView ���b�Z�[�W �n���h��

/**
 * ����������
 */
void CReportView::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();

	// �v���O���X�o�[������
	mc_progressBar.SetRange( 0, 1000 );

	//--- �^�C�g���̕ύX
	{
		// �t�H���g�ύX
		m_titleEdit.SetFont( &theApp.m_font );
	}

	//--- ���X�g�̕ύX
	{
		// �t�H���g�ύX
		m_list.SetFont( &theApp.m_font );

		// �O���b�h���C���\��
		m_list.SetExtendedStyle(m_list.GetExtendedStyle() | LVS_EX_GRIDLINES);

		// ��s�I�����[�h�̐ݒ�
		ListView_SetExtendedListViewStyle((HWND)m_list.m_hWnd, LVS_EX_FULLROWSELECT);

		DWORD dwStyle = m_list.GetStyle();
		dwStyle &= ~LVS_TYPEMASK;
		// �f�t�H���g�����|�[�g�\���ŃI�[�i�[�Œ�`��ɂ���
		dwStyle |= LVS_REPORT | LVS_OWNERDRAWFIXED | LVS_SINGLESEL;

		// �X�^�C���̍X�V
		m_list.ModifyStyle(0, dwStyle);

		// �A�C�R�����X�g�̍쐬
		m_pimgList.Create(16, 16, ILC_COLOR4 | ILC_MASK, 2, 0);
		m_pimgList.Add( AfxGetApp()->LoadIcon(IDI_NO_PHOTO_ICON) );
		m_pimgList.Add( AfxGetApp()->LoadIcon(IDI_PHOTO_ICON) );
		m_list.SetImageList(&m_pimgList, LVSIL_SMALL);

		// �J�����̒ǉ�
		// ����������������ɍĐݒ肷��̂ŉ��̕����w�肵�Ă����B
		m_list.InsertColumn(0, _T(""), LVCFMT_LEFT, 20, -1);
		m_list.InsertColumn(1, _T("���O"), LVCFMT_LEFT, 20, -1);
		m_list.InsertColumn(2, _T("����"), LVCFMT_LEFT, 20, -1);

		// �I�v�V�����̐ݒ�
		m_list.m_bUsePanScrollAnimation = theApp.m_optionMng.m_bUseRan2PanScrollAnimation;
		m_list.m_bUseHorizontalDragMove = theApp.m_optionMng.m_bUseRan2HorizontalDragMove;
	}

	//--- �ʒm�̈�̕ύX
	{
		// �t�H���g�ύX
		m_infoEdit.SetFont( &theApp.m_font );
	}

	m_nochange = FALSE;

	// �X�N���[���ʂ̏����l�ݒ�
	m_scrollLine = theApp.m_optionMng.m_reportScrollLine;

	const int DETAIL_VIEWID = 1000;	// �b��Ȃ̂łĂ��Ɓ[
	if( m_detailView != NULL ){
		delete m_detailView;
	}

	// ��ʃT�C�Y�ō\�z����i���x���I�E�E�E�j
	CRect viewRect;
	SystemParametersInfo(SPI_GETWORKAREA, 0, &viewRect, 0);
	m_detailView = new Ran2View();
//	TRACE(TEXT("sy=%d,viewWidth=%d,viewHeight=%d\r\n"),sy,viewWidth,viewHeight);

	MZ3LOGGER_INFO(L"�����r���[�������J�n");
	m_detailView->Create(TEXT("RAN2WND"),TEXT(""),CS_GLOBALCLASS,viewRect,(CWnd*)this,DETAIL_VIEWID);
	MZ3LOGGER_INFO(L"�����r���[����������(1/2)");

	// ���b��
	int fontHeight = theApp.m_optionMng.GetFontHeight();
	if( fontHeight == 0 ) {
		fontHeight = 9;
	}
	m_detailView->ChangeViewFont( fontHeight, theApp.m_optionMng.GetFontFace() );
	m_detailView->ShowWindow(SW_SHOW);
	MZ3LOGGER_INFO(L"�����r���[����������(2/2)");
}

/**
 * �T�C�Y�ύX���̏���
 */
void CReportView::OnSize(UINT nType, int cx, int cy)
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

	CFormView::OnSize(nType, cx, cy);

	int fontHeight = theApp.m_optionMng.GetFontHeightByPixel(theApp.GetDPI());
	if( fontHeight == 0 ) {
		fontHeight = 12;
	}

	int hTitle  = theApp.GetInfoRegionHeight(fontHeight);	// �^�C�g���̈�̓t�H���g�T�C�Y�ˑ�

	const int h1 = theApp.m_optionMng.m_nReportViewListHeightRatio;
	const int h2 = theApp.m_optionMng.m_nReportViewBodyHeightRatio;
	int hList   = (cy * h1 / (h1+h2))-hTitle;	// (�S�̂�N%-�^�C�g���̈�) �����X�g�̈�Ƃ���
	int hReport = (cy * h2 / (h1+h2));			// �S�̂�N%�����|�[�g�̈�Ƃ���

	// ���̈�͕K�v�ɉ����ĕ\������邽�߁A��L�̔䗦�Ƃ͊֌W�Ȃ��T�C�Y��ݒ肷��
	int hInfo   = theApp.GetInfoRegionHeight(fontHeight);	// ���̈���t�H���g�T�C�Y�ˑ�

	// �X�N���[���o�[�̕�
	int barWidth = ::GetSystemMetrics(SM_CXVSCROLL);

	// �e�R���g���[���̈ړ�
	util::MoveDlgItemWindow( this, IDC_TITLE_EDIT,  0, 0,            cx, hTitle  );
	util::MoveDlgItemWindow( this, IDC_REPORT_LIST, 0, hTitle,       cx, hList   );

	// RAN2 �̈ړ�
	if (m_detailView && ::IsWindow(m_detailView->GetSafeHwnd())) {
		int wRan2 = cx - barWidth;
		m_detailView->MoveWindow( 0, hTitle+hList, wRan2, hReport );
		ShowCommentData( m_currentData );
	}

	util::MoveDlgItemWindow( this, IDC_INFO_EDIT,   0, cy - hInfo,   cx, hInfo   );

	// �X�N���[���o�[����
//	util::MoveDlgItemWindow(this, IDC_VSCROLLBAR, cx-barWidth, cy - hInfo, cx, hInfo);
	if( m_vScrollbar ){
		int barSX = cx - barWidth;
		util::MoveDlgItemWindow(this, IDC_VSCROLLBAR, cx-barWidth, hTitle+hList, barWidth, hReport);
		m_vScrollbar.SetWindowPos( m_detailView, 0, 0, 0, 0, SWP_SHOWWINDOW | SWP_NOMOVE | SWP_NOSIZE);

		// �X�N���[���o�[���s�v�Ȏ��͉B��
		m_vScrollbar.ShowWindow(SW_HIDE);
		int viewLineCount = m_detailView->GetViewLineMax();
		int allLineCount  = m_detailView->GetAllLineCount();
		SCROLLINFO si;
		si.cbSize = sizeof(SCROLLINFO);
		si.fMask = SIF_POS | SIF_RANGE | SIF_PAGE;
		si.nMin  = 0;
		si.nMax  = allLineCount-1;
		si.nPage = viewLineCount;
		si.nPos  = 0;
		m_vScrollbar.SetScrollInfo(&si, TRUE);
		if( allLineCount-viewLineCount > 0 ) {
			m_vScrollbar.ShowWindow(SW_SHOW);
		}
	}

	// ���x���ʂ��E�����[�h�̏ꍇ�̓X�^�C����ύX�����؂�
	if( theApp.m_optionMng.m_killPaneLabel ){
		util::ModifyStyleDlgItemWindow(this,IDC_REPORT_LIST,NULL,LVS_NOCOLUMNHEADER);
	}

	// �X�N���[���^�C�v���u�y�[�W�P�ʁv�Ȃ�Čv�Z
	if( theApp.m_optionMng.m_reportScrollType == option::Option::REPORT_SCROLL_TYPE_PAGE ) {
		if (m_detailView != NULL) {
			int charHeightOffset = m_detailView->GetCharHeightOffset();
			if ((fontHeight+charHeightOffset)>0) {
				m_scrollLine = (hReport / (fontHeight+charHeightOffset)) - 2;
			}
		}
		TRACE(_T("Scrol Line = %d\n"), m_scrollLine);
	}else{
		m_scrollLine = theApp.m_optionMng.m_reportScrollLine;
	}

	// �I�𒆂̍s���\�������悤�ɂ���
	if( m_list.m_hWnd != NULL ) {
		int idx = m_list.GetSelectedItem();
		m_list.EnsureVisible( idx, FALSE );
	}

	// �v���O���X�o�[�͕ʓr�z�u
	// �T�C�Y�� hInfo �� 2/3 �Ƃ���
	int hProgress = hInfo * 2 / 3;
	int y = cy - hInfo - hProgress;
	util::MoveDlgItemWindow( this, IDC_PROGRESS_BAR, 0, y, cx, hProgress );

	// ���X�g�J�������̕ύX
	ResetColumnWidth( m_data );
}

HBRUSH CReportView::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{

	HBRUSH hbr = CFormView::OnCtlColor(pDC, pWnd, nCtlColor);

	if (pWnd->m_hWnd == GetDlgItem(IDC_INFO_EDIT)->m_hWnd) {
		return hbr;
	}

	switch (nCtlColor) {
	case CTLCOLOR_STATIC:
		pDC->SetBkMode(TRANSPARENT);
		return (HBRUSH)GetStockObject(WHITE_BRUSH);
		break;

	default:
		pDC->SetBkMode(TRANSPARENT);
		break;
	}

	return (HBRUSH)GetStockObject(WHITE_BRUSH);
}

/**
 * �f�[�^�ݒ�
 */
void CReportView::SetData(const CMixiData& data)
{
	// ����������
	m_data = data;
	m_nochange = TRUE;
	m_lastIndex = 0;

	m_list.DeleteAllItems();
	m_list.SetRedraw(FALSE);

	// �J�����T�C�Y��ύX����
	ResetColumnWidth( m_data );

	// �ǂ��܂Ńf�[�^���擾��������ݒ肷��
	TRACE(_T("Address = %s\n"), m_data.GetURL());

	// ID ��ݒ肵�Ă���
	if (m_data.GetID() == -1) {
		// �����łh�c�����
		m_data.SetID( mixi::MixiUrlParser::GetID(m_data.GetURL()) );
	}

	// ���ǈʒu�̕ύX
	m_lastIndex = mixi::ParserUtil::GetLastIndexFromIniFile(m_data);
	if (m_lastIndex == -1) {
		m_lastIndex = 0;
	} else {
		m_lastIndex ++;
	}

	INT_PTR count = m_data.GetChildrenSize();
	if (count > 0 && m_lastIndex > 0) {
		CMixiData& subItem = m_data.GetChild( count-1 );
		if (m_lastIndex > subItem.GetCommentIndex()) {
			m_lastIndex = subItem.GetCommentIndex();
		}
	}

	// �^�C�g���̐ݒ�
	CString title = m_data.GetTitle();
	switch (m_data.GetAccessType()) {
#ifdef BT_MZ3
	case ACCESS_MYDIARY:
	case ACCESS_MESSAGE:
	case ACCESS_PROFILE:
	case ACCESS_BIRTHDAY:
		break;
#endif
	default:
		if( !m_data.GetName().IsEmpty() ) {
			title.AppendFormat( _T("(%s)"), (LPCTSTR)m_data.GetName() );
		}
		break;
	}
	m_titleEdit.SetWindowText( title );


	// ----------------------------------------
	// �R�����g�̒ǉ�
	// ----------------------------------------
	TRACE(_T("�R�����g�� = [%d]\n"), count);

	int focusItem = 0;
	for (int i=0; i<count; i++) {
		CMixiData& subItem = m_data.GetChild(i);

		// �摜�̗L���ŃA�C�R���̃C���f�b�N�X��ύX����
		int imageIndex = (subItem.GetImageCount() == 0) ? 0 : 1;
		CString strIndex;
		if (subItem.GetCommentIndex()>=0) {
			// ���w��̏ꍇ�͋󔒂Ƃ���B
			strIndex = util::int2str(subItem.GetCommentIndex());
		}
		int idx = m_list.InsertItem(i+1, strIndex, imageIndex);

		if (subItem.GetCommentIndex() == m_lastIndex) {
			// ���̎��̍��ڂ�I�����ڂƂ���
			focusItem = idx + 1;
		}

		// Author ��
		m_list.SetItem(idx, 1, LVIF_TEXT | LVIF_IMAGE, subItem.GetAuthor(), 0, 0, 0, 0);
		// Date ��
		m_list.SetItem(idx, 2, LVIF_TEXT, subItem.GetDate(), 0, 0, 0, 0);
		// ItemData �� CMixiData* ��^����
		m_list.SetItemData(idx, (DWORD_PTR)&subItem);
	}

	// �e�����X�g�ɕ\��
	{
		// �摜�̗L���ŃA�C�R���̃C���f�b�N�X��ύX����
		int imageIndex = (m_data.GetImageCount() == 0) ? 0 : 1;
		int idx = m_list.InsertItem(0, _T("-"), imageIndex);
		// Author ��
		m_list.SetItem(idx, 1, LVIF_TEXT, m_data.GetAuthor(), 0, 0, 0, 0);
		// Date �� : �擪���ڂɂ͏����ł���\�����Ă��Ȃ��B�K�v�ł���΁A���L���R�����g�A�E�g���邱�ƁB
//		m_list.SetItem(idx, 2, LVIF_TEXT, m_data.GetDate(), 0, 0, 0, 0);
		if( !m_data.GetOpeningRange().IsEmpty() ) {
			m_list.SetItem(idx, 2, LVIF_TEXT, m_data.GetOpeningRange() , 0, 0, 0, 0);
		}

		// ItemData �� CMixiData* ��^����
		m_list.SetItemData(0, (DWORD_PTR)&m_data);
	}

	m_nochange = FALSE;

	if (count == 0) {
		// �R�����g���Ȃ����߁A�e�v�f�����ǈʒu�Ƃ���B
		m_lastIndex = 0;
	} else {
		// �I����Ԃ̐��K��
		CMixiData& firstCommentItem = m_data.GetChild(0);
		if (m_lastIndex < firstCommentItem.GetCommentIndex() && m_lastIndex > 0) {
			// �擪���ڂ̃C���f�b�N�X�������ǈʒu���������ꍇ�A
			// �ȑO�Ɍ��Ă������Ǎ��ڂ��u����Ă���v�Ɣ��f���A
			// ���ǈʒu��擪���ڂ̃C���f�b�N�X��^����B
			m_lastIndex = firstCommentItem.GetCommentIndex();
			focusItem = 1;
		}
	}

	// �v���t�B�[���\���̏ꍇ�͏�ɐ擪���ڂ�I��
#ifdef BT_MZ3
	if (m_data.GetAccessType() == ACCESS_PROFILE ||
		m_data.GetAccessType() == ACCESS_BIRTHDAY ||
		m_data.GetAccessType() == ACCESS_NEIGHBORDIARY ) {
		focusItem = 0;
	}
#endif

	//--- UI �֘A
	m_list.SetRedraw(TRUE);
	m_list.SetFocus();
	m_list.SetItemState( focusItem, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED );
	m_list.EnsureVisible( focusItem, FALSE );

	MyUpdateControlStatus();
}

/**
 * �e�L���f�[�^�\��
 */
void CReportView::ShowParentData(CMixiData* data)
{
	return ShowCommentData(data);
}

/**
 * �R�����g�f�[�^�\��
 */
void CReportView::ShowCommentData(CMixiData* data)
{
	if (data==NULL) {
		return;
	}

	CStringArray* bodyStrArray = new CStringArray();

	// ���̂�ύX����1�s�ڂ�`��
	bodyStrArray->Add(L"<blue>");
	bodyStrArray->Add(data->GetAuthor() + L"�@" + data->GetDate() );
	if( !data->GetOpeningRange().IsEmpty() ) {
		bodyStrArray->Add( L"�@�y" + data->GetOpeningRange() + L"�z" );
	}
	bodyStrArray->Add(L"</blue>");
	bodyStrArray->Add(L"<br>");

	// 2�s�ڂ�`��
	if( !data->GetPrevDiary().IsEmpty() || !data->GetNextDiary().IsEmpty() ){
		if( !data->GetPrevDiary().IsEmpty() ){
			//CString PrevLink = data->GetPrevDiary();
			//ViewFilter::ReplaceHTMLTagToRan2Tags( PrevLink, *bodyStrArray, theApp.m_emoji, this );
			bodyStrArray->Add(L"<prevdiary>");
			bodyStrArray->Add(L"<<�O�̓��L��");
			bodyStrArray->Add(L"</prevdiary>");
		}
		bodyStrArray->Add(L"�@");
		if( !data->GetNextDiary().IsEmpty() ){
			//CString NextLink = data->GetNextDiary();
			//ViewFilter::ReplaceHTMLTagToRan2Tags( NextLink, *bodyStrArray, theApp.m_emoji, this );
			bodyStrArray->Add(L"<nextdiary>");
			bodyStrArray->Add(L"���̓��L��>>");
			bodyStrArray->Add(L"</nextdiary>");
		}
		bodyStrArray->Add(L"<br>");
	}

//	TRACE( L"��---xdump start---\r\n" );
//	for (int i=0; i<data->GetBodySize(); i++) {
//		TRACE( L"{%d}%s|\r\n", CString(data->GetBody(i)).GetLength(), data->GetBody(i) );
//	}
//	TRACE( L"��---xdump end---\r\n" );

	// �擪�ɉ��s����������ƁA�F�X�t�H�[�}�b�g�����������̂ŒP��̕�����ɘA������
	CString str = data->GetBody();

	// ���s�������Ēǉ�����
	for (;;) {
		int idxCrlf = str.Find( L"\r\n" );

		CString target;
		if (idxCrlf == -1) {
			target = str;
			if (target.IsEmpty()) {
				break;
			}
		} else {
			target.SetString( str, idxCrlf );
		}

		if( target.GetLength() == 0 ) {
			if (idxCrlf != -1) {
				LPCTSTR brLine = TEXT("<br>");
				bodyStrArray->Add(brLine);
			}
		} else {
			// �G�����p�t�B���^
			//ViewFilter::ReplaceEmojiCodeToRan2ImageTags( target, *bodyStrArray, theApp.m_emoji, this );
			ViewFilter::ReplaceHTMLTagToRan2Tags( target, *bodyStrArray, theApp.m_emoji, this );
			LPCTSTR brLine = TEXT("<br>");
			bodyStrArray->Add(brLine);
		}

		if (idxCrlf == -1) {
			break;
		}
		str.Delete(0, idxCrlf+2);
	}

	// �Ō�ɂP�s����Č��₷������
	bodyStrArray->Add(_T("<br>"));

//	TRACE( L"��---dump start---\r\n" );
//	for (int i=0; i<bodyStrArray->GetCount(); i++) {
//		TRACE( L"{%d}%s|\r\n", bodyStrArray->GetAt(i).GetLength(), bodyStrArray->GetAt(i) );
//	}
//	TRACE( L"��---dump end---\r\n" );

	// blockquote�̑O�ɉ��s������
	ViewFilter::InsertBRTagToBeforeblockquoteTag( bodyStrArray );

	// �`��J�n
	m_scrollBarHeight = m_detailView->LoadDetail(bodyStrArray, &theApp.m_imageCache.GetImageList16());
	TRACE(TEXT("LoadDetail��%d�s���p�[�X���܂���\r\n"), m_scrollBarHeight);
	m_detailView->ResetDragOffset();
	// OnSize ���ɂ��̃��[�g��2��ʂ邽�߁A�����`����s����MZ3�̏ꍇ�Ɉ�u������ʂ������B
	// �����������邽�߁A�����`����s�킸�AInvalidate �ɂ�� WM_PAINT �ʒm�ł܂Ƃ߂ĕ`�悷��B
	m_detailView->DrawDetail(0, false);
	m_detailView->Invalidate(FALSE);
	bodyStrArray->RemoveAll();
	delete bodyStrArray;

	// �����r���[�I�v�V�����ݒ�
	m_detailView->m_bUsePanScrollAnimation = theApp.m_optionMng.m_bUseRan2PanScrollAnimation;
	m_detailView->m_bUseHorizontalDragMove = theApp.m_optionMng.m_bUseRan2HorizontalDragMove;
	m_detailView->m_bUseDoubleClickMove = theApp.m_optionMng.m_bUseRan2DoubleClickMove;

	// �X�N���[���o�[���s�v�Ȏ��͉B��
	m_vScrollbar.ShowWindow(SW_HIDE);
	int viewLineCount = m_detailView->GetViewLineMax();
	int allLineCount  = m_detailView->GetAllLineCount();
	SCROLLINFO si;
	si.cbSize = sizeof(SCROLLINFO);
	si.fMask = SIF_POS | SIF_RANGE | SIF_PAGE;
	si.nMin  = 0;
	si.nMax  = allLineCount-1;
	si.nPage = viewLineCount;
	si.nPos  = 0;
	m_vScrollbar.SetScrollInfo(&si, TRUE);
	if( allLineCount-viewLineCount > 0 ) {
		m_vScrollbar.ShowWindow(SW_SHOW);
	}
}

void CReportView::OnLvnItemchangedReportList(NMHDR *pNMHDR, LRESULT *pResult)
{
	if (m_nochange != FALSE) {
		return;
	}

	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);

	if (pNMLV->uNewState != 3) {
		return;
	}

	CMixiData* data = (CMixiData*)m_list.GetItemData(pNMLV->iItem);
	m_currentData = data;
	if (m_lastIndex < data->GetCommentIndex()) {
		// ���ǈʒu�����������߁A���V�������ڂ������Ɣ��f���A
		// ���ǈʒu���X�V����B
		m_lastIndex = data->GetCommentIndex();
	}
	if ((pNMLV->iItem+1)==m_list.GetItemCount()) {
		// �ŏI���ڂ����Ă���ꍇ�A
		// �i�R�����g�폜���̉\�������邽�߁j
		// URL ���� comment_count ������΁A���̒l�����ǈʒu�Ƃ���B
		CString comment_count = util::GetParamFromURL( m_data.GetURL(), L"comment_count" );
		if (!comment_count.IsEmpty() && _wtoi(comment_count)>0) {
			m_lastIndex = _wtoi(comment_count);
		}
	}
		

	m_list.SetRedraw(FALSE);
	if (pNMLV->iItem == 0) {
		ShowParentData(data);
	}
	else {
		ShowCommentData(data);
	}

	m_imageState = (BOOL)(data->GetImageCount() > 0);
	theApp.EnableCommandBarButton( ID_IMAGE_BUTTON, ((!theApp.m_access) & m_imageState));
	theApp.EnableCommandBarButton( ID_OPEN_BROWSER, !theApp.m_access);

	m_list.SetRedraw(TRUE);

	*pResult = 0;
}

void CReportView::OnLvnKeydownReportList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLVKEYDOWN pLVKeyDow = reinterpret_cast<LPNMLVKEYDOWN>(pNMHDR);

	if (pLVKeyDow->wVKey == VK_RETURN) {
		// ���|�[�g���j���[�̕\��
		MyPopupReportMenu();
	}
	*pResult = 0;
}

/// ��ԏ�̍��ڂɈړ�
BOOL CReportView::CommandMoveToFirstList()
{
	util::MySetListCtrlItemFocusedAndSelected( m_list, m_list.GetItemCount()-1, false );
	util::MySetListCtrlItemFocusedAndSelected( m_list, 0, true );
	m_list.EnsureVisible( 0, FALSE );

	return TRUE;
}

/// ��ԉ��̍��ڂɈړ�
BOOL CReportView::CommandMoveToLastList()
{
	util::MySetListCtrlItemFocusedAndSelected( m_list, 0, false );
	util::MySetListCtrlItemFocusedAndSelected( m_list, m_list.GetItemCount()-1, true );
	m_list.EnsureVisible( m_list.GetItemCount()-1, FALSE );

	return TRUE;
}

BOOL CReportView::CommandMoveUpList()
{
	// �A�C�e������1�ȉ��ł���ΕύX���Ȃ�
	if (m_list.GetItemCount()<=1) {
		return TRUE;
	}

	if (m_list.GetItemState(0, LVIS_FOCUSED) != FALSE) {
		// ��ԏ�̍��ڑI�𒆂Ȃ̂ŁA��ԉ��Ɉړ�
		CommandMoveToLastList();
	} else {
		// ��ԏ�ł͂Ȃ��̂ŁA��Ɉړ�
		int idx = m_list.GetSelectedItem();
		if (idx<0) {
			// �I���Ȃ��Ȃ̂ōŏ��̍��ڂ�I��
			idx = 0;
			util::MySetListCtrlItemFocusedAndSelected( m_list, idx, true );
			// ��\���Ȃ�X�N���[��
			if( !util::IsVisibleOnListBox( m_list, idx ) ) {
				m_list.EnsureVisible( idx, FALSE );
			}
		} else {
			util::MySetListCtrlItemFocusedAndSelected( m_list,   idx, false );
			util::MySetListCtrlItemFocusedAndSelected( m_list, --idx, true );

			// �ړ��悪��\���Ȃ������ɃX�N���[��
			if( !util::IsVisibleOnListBox( m_list, idx ) ) {
				if( ( idx < m_list.GetTopIndex() ) &&
					( idx >= m_list.GetTopIndex() - m_list.GetCountPerPage() ) ) {
					// �ړ��悪��ʂ���A1��ʈȓ��ɂ��鎞��1��ʃX�N���[��
					m_list.Scroll( CSize(0, -m_list.GetCountPerPage() * theApp.m_optionMng.GetFontHeight()) );
				} else {
					// �ړ��悪��ʂ�艺���A���1��ʈȏ㗣��Ă��鎞��EnsureVisible()
					m_list.EnsureVisible( idx , TRUE );
				}
			}
		}
	}

	if (theApp.m_optionMng.m_bUseRan2PanScrollAnimation) {
		m_detailView->StartPanDraw(Ran2View::PAN_SCROLL_DIRECTION_RIGHT);
	}

	return TRUE;
}

BOOL CReportView::CommandMoveDownList()
{
	// �A�C�e������1�ȉ��ł���ΕύX���Ȃ�
	if (m_list.GetItemCount()<=1) {
		return TRUE;
	}

	if (m_list.GetItemState(m_list.GetItemCount()-1, LVIS_FOCUSED) != FALSE) {
		// ��ԉ��̍��ڑI�𒆂Ȃ̂ŁA��ԏ�Ɉړ�
		CommandMoveToFirstList();
	} else {
		// ��ԉ��ł͂Ȃ��̂ŁA���Ɉړ�
		int idx = m_list.GetSelectedItem();
		if (idx<0) {
			// �I���Ȃ��Ȃ̂ōŏ��̍��ڂ�I��
			idx = 0;
			util::MySetListCtrlItemFocusedAndSelected( m_list, idx, true );
			// ��\���Ȃ�X�N���[��
			if( !util::IsVisibleOnListBox( m_list, idx ) ) {
				m_list.EnsureVisible( idx, FALSE );
			}
		} else {
			util::MySetListCtrlItemFocusedAndSelected( m_list,   idx, false );
			util::MySetListCtrlItemFocusedAndSelected( m_list, ++idx, true );

			// �ړ��悪��\���Ȃ牺�����ɃX�N���[��
			if( !util::IsVisibleOnListBox( m_list, idx ) ) {
				if( ( idx >= m_list.GetTopIndex() + m_list.GetCountPerPage() ) &&
					( idx < m_list.GetTopIndex() + m_list.GetCountPerPage() * 2 ) ) {
					// �ړ��悪��ʂ�艺�A1��ʈȓ��ɂ��鎞��1��ʃX�N���[��
					m_list.Scroll( CSize(0, m_list.GetCountPerPage() * theApp.m_optionMng.GetFontHeight()) );
				} else {
					// �ړ��悪��ʂ��ォ�A����1��ʈȏ㗣��Ă��鎞��EnsureVisible()
					m_list.EnsureVisible( idx , TRUE );
				}
			}
		}
	}

	if (theApp.m_optionMng.m_bUseRan2PanScrollAnimation) {
		m_detailView->StartPanDraw(Ran2View::PAN_SCROLL_DIRECTION_LEFT);
	}

	return TRUE;
}

BOOL CReportView::CommandScrollUpEdit()
{
	int pos = m_vScrollbar.GetScrollPos();
	if( pos <= 0 ) {
		return FALSE;
	}
	pos -= m_scrollLine;
	if (pos<0) {
		// �����l�␳
		pos = 0;
	}

	m_vScrollbar.SetScrollPos(pos);
	m_detailView->ResetDragOffset();
	m_detailView->DrawDetail(pos);

	return TRUE;
}

BOOL CReportView::CommandScrollDownEdit()
{
	int pos = m_vScrollbar.GetScrollPos();
	if( pos >= m_scrollBarHeight ){
		return FALSE;
	}
	pos += m_scrollLine;
	if (pos > m_scrollBarHeight) {
		// ����l�␳
		pos = m_scrollBarHeight;
	}

	m_vScrollbar.SetScrollPos(pos);
	m_detailView->ResetDragOffset();
	m_detailView->DrawDetail(pos);
	return TRUE;
}


BOOL CReportView::OnKeyUp(MSG* pMsg)
{
	switch (pMsg->wParam) {
#ifndef WINCE
	case VK_F1:
		// �w���v�\��
		util::OpenByShellExecute( MZ4_MANUAL_URL );
		break;
#endif

#ifndef WINCE
	case VK_APPS:
		// ���|�[�g���j���[�̕\��
		MyPopupReportMenu();
		return TRUE;
#endif

	case VK_BACK:				// �N���A�{�^��
	case VK_ESCAPE:
		// ���X�g�̏ꍇ�͑O��ʂɖ߂�
		if (pMsg->hwnd == m_list.m_hWnd) {
			if (theApp.m_access) {
				// �A�N�Z�X���͒��f����
				::SendMessage(m_hWnd, WM_MZ3_ABORT, NULL, NULL);
			} else {
				OnMenuBack();
			}
			return TRUE;
		}

		// TODO: �G�f�B�b�g�R���g���[���Ƀt�H�[�J�X������ꍇ��
		//       ���X�g�Ƀt�H�[�J�X���ړ����������A�����ł͂ł��Ȃ��B
		//       �G�f�B�b�g�R���g���[���̃T�u�N���X�����K�v���H
		break;

/*	case VK_RETURN:
		if (pMsg->hwnd == m_list.m_hWnd) {
			// ���|�[�g���j���[�̕\��
			MyPopupReportMenu();
			return TRUE;
		}
		break;
*/	}

	// Xcrawl Canceler
	if( theApp.m_optionMng.m_bUseXcrawlExtension ) {
		if( m_xcrawl.procKeyup( pMsg->wParam ) ) {
			// �L�����Z�����ꂽ�̂ŏ㉺�L�[�𖳌��ɂ���B
	//		util::MySetInformationText( GetSafeHwnd(), L"Xcrawl canceled..." );
			return TRUE;
		}

		if (pMsg->hwnd == m_list.m_hWnd) {
			// ���X�g�ł̃L�[UP�C�x���g
			switch(pMsg->wParam) {
			case VK_UP:
				if( m_xcrawl.isXcrawlEnabled() ) {
					// Xcrawl �ł̓X�N���[��
					return CommandScrollUpEdit();
				}else{
					if( m_nKeydownRepeatCount >= 2 ) {
						// �L�[�������ɂ��A���ړ����Ȃ�A�L�[UP�ňړ����Ȃ��B
						return TRUE;
					}

					// �X�N���[���ʒu����i�X�N���[���]�n������΃X�N���[������j
					if (CommandScrollUpEdit()) {
						return TRUE;
					}

					if( CommandMoveUpList() ) {
						return TRUE;
					}
				}
				break;

			case VK_DOWN:
				if( m_xcrawl.isXcrawlEnabled() ) {
					// Xcrawl �ł̓X�N���[��
					return CommandScrollDownEdit();
				}else{
					if( m_nKeydownRepeatCount >= 2 ) {
						// �L�[�������ɂ��A���ړ����Ȃ�A�L�[UP�ňړ����Ȃ��B
						return TRUE;
					}

					// �X�N���[���ʒu����i�X�N���[���]�n������΃X�N���[������j
					if (CommandScrollDownEdit()) {
						return TRUE;
					}

					if( CommandMoveDownList() ) {
						return TRUE;
					}
				}
				break;
			}
		}
	}

	return FALSE;
}

BOOL CReportView::OnKeyDown(MSG* pMsg)
{
	switch (MapVirtualKey(pMsg->wParam, 2)) {
	case 48:
		TRACE(_T("0 Down\n"));
		return TRUE;
	}

	// VK_UP, VK_DOWN �̏���
	if( theApp.m_optionMng.m_bUseXcrawlExtension ) {
		// Xcrawl Canceler
		if( m_xcrawl.procKeydown(pMsg->wParam) ) {
			return TRUE;
		}

		if (pMsg->hwnd == m_list.m_hWnd) {
			// ���X�g�ł̃L�[�����C�x���g
			switch(pMsg->wParam) {
			case VK_UP:
				// VK_KEYDOWN �ł͖����B
				// VK_KEYUP �ŏ�������B
				// ����́A�A�h�G�X�� Xcrawl �Ή��̂��߁B

				// �������A�Q��ڈȍ~�̃L�[�����ł���΁A�������Ƃ݂Ȃ��A�ړ�����
				if( !m_xcrawl.isXcrawlEnabled() && m_nKeydownRepeatCount >= 2 ) {
					// �X�N���[���ʒu����i�X�N���[���]�n������΃X�N���[������j
					if (CommandScrollUpEdit()) {
						return TRUE;
					}
//					MZ3LOGGER_ERROR( L"repeat" );
					return CommandMoveUpList();
				}

				return TRUE;

			case VK_DOWN:
				// VK_KEYDOWN �ł͖����B
				// VK_KEYUP �ŏ�������B
				// ����́A�A�h�G�X�� Xcrawl �Ή��̂��߁B

				// �������A�Q��ڈȍ~�̃L�[�����ł���΁A�������Ƃ݂Ȃ��A�ړ�����
				if( !m_xcrawl.isXcrawlEnabled() && m_nKeydownRepeatCount >= 2 ) {
					// �X�N���[���ʒu����i�X�N���[���]�n������΃X�N���[������j
					if (CommandScrollDownEdit()) {
						return TRUE;
					}
//					MZ3LOGGER_ERROR( L"repeat" );
					return CommandMoveDownList();
				}

				return TRUE;
			}
		}
	} else {
		// Xcrawl �I�v�V�����������̏���
		if (pMsg->hwnd == m_list.m_hWnd) {
			// ���X�g�ł̃L�[�����C�x���g
			switch(pMsg->wParam) {
			case VK_UP:
				// �X�N���[���ʒu����i�X�N���[���]�n������΃X�N���[������j
				if (CommandScrollUpEdit()) {
					return TRUE;
				}

				// ���ڕύX
				if (m_list.GetItemState(0, LVIS_FOCUSED) != FALSE &&
					// �A�C�e������1�ȉ��ł���ΕύX���Ȃ�
					m_list.GetItemCount()>1)
				{
					// ��ԏ�̍��ڑI�𒆂Ȃ̂ŁA��ԉ��Ɉړ�
					CommandMoveToLastList();

					if (theApp.m_optionMng.m_bUseRan2PanScrollAnimation) {
						m_detailView->StartPanDraw(Ran2View::PAN_SCROLL_DIRECTION_RIGHT);
					}

					return true;
				} else {
#ifdef WINCE
					// �f�t�H���g����
					return FALSE;
#else
					return CommandMoveUpList();
#endif
				}

			case VK_DOWN:
				// �X�N���[���ʒu����i�X�N���[���]�n������΃X�N���[������j
				if (CommandScrollDownEdit()) {
					return TRUE;
				}

				// ���ڕύX
				if (m_list.GetItemState(m_list.GetItemCount()-1, LVIS_FOCUSED) != FALSE &&
					// �A�C�e������1�ȉ��ł���ΕύX���Ȃ�
					m_list.GetItemCount()>1)
				{
					// ��ԉ��̍��ڑI�𒆂Ȃ̂ŁA��ԏ�Ɉړ�
					CommandMoveToFirstList();

					if (theApp.m_optionMng.m_bUseRan2PanScrollAnimation) {
						m_detailView->StartPanDraw(Ran2View::PAN_SCROLL_DIRECTION_LEFT);
					}

					return TRUE;
				} else {
#ifdef WINCE
					// �f�t�H���g����
					return FALSE;
#else
					return CommandMoveDownList();
#endif
				}
				break;
			}
		}
	}

	if (pMsg->hwnd == m_list.m_hWnd) {
		// ���X�g�ł̃L�[�����C�x���g
		switch(pMsg->wParam) {
		case VK_RIGHT:
			// ���̃R�����g�Ɉړ�
			return CommandMoveDownList();

		case VK_LEFT:
			// �O�̃R�����g�Ɉړ�
			return CommandMoveUpList();

		case VK_SPACE:
			if (GetAsyncKeyState( VK_CONTROL ) & 0x8000) {
				// Ctrl+Space
				OnAcceleratorNextComment();
			} else {
				// Space
				OnAcceleratorScrollOrNextComment();
			}
			return TRUE;

		}
	}

	return FALSE;
}

BOOL CReportView::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->hwnd == m_list.m_hWnd) {
		switch (pMsg->message) {
		case WM_KEYUP:
			{
				BOOL r = OnKeyUp(pMsg);

				// KEYDOWN ���s�[�g�񐔂�������
				m_nKeydownRepeatCount = 0;

				if( r ) {
					return TRUE;
				}
			}
			break;

		case WM_KEYDOWN:
			{
				// KEYDOWN ���s�[�g�񐔂��C���N�������g
				m_nKeydownRepeatCount ++;

				if( OnKeyDown(pMsg) ) {
					return TRUE;
				}
			}
			break;

		case WM_MOUSEWHEEL:
			// �����r���[��̃z�C�[�����t�H�[�J�X���Ⴄ�̂ł�����ɗ���B
			// �}�E�X�̍��W�Ŕ��肷��B
			{
				CPoint pt;
				GetCursorPos( &pt );

				CRect rect;
				m_detailView->GetWindowRect( &rect );
				if (rect.PtInRect( pt )) {
					OnMouseWheel( LOWORD(pMsg->wParam), HIWORD(pMsg->wParam), CPoint(LOWORD(pMsg->lParam), HIWORD(pMsg->lParam)) );
					return TRUE;
				}
			}
			break;
		}
	}

	return CFormView::PreTranslateMessage(pMsg);
}

/**
 * �I������
 */
void CReportView::EndProc()
{
	SaveIndex();
}

/**
 * �C���f�b�N�X�ۑ�
 *
 * m_lastIndex ��ۑ�
 */
void CReportView::SaveIndex()
{
	theApp.m_readlog.SetValue(util::GetLogIdString( m_data ), (LPCTSTR)util::int2str(m_lastIndex), "Log");
}

/**
 * �u�b�N�}�[�N�ɒǉ�
 */
void CReportView::OnAddBookmark()
{
#ifdef BT_MZ3
	if (m_data.GetAccessType() != ACCESS_BBS &&
		m_data.GetAccessType() != ACCESS_EVENT &&
		m_data.GetAccessType() != ACCESS_EVENT_JOIN &&
		m_data.GetAccessType() != ACCESS_ENQUETE) {
		::MessageBox(m_hWnd, _T("�R�~���j�e�B�ȊO��\n�o�^�o���܂���"), MZ3_APP_NAME, NULL);
		return;
	}

	if( theApp.m_bookmarkMng.Add( &m_data, theApp.m_root.GetBookmarkList() ) != FALSE ) {
		::MessageBox(m_hWnd, _T("�o�^���܂���"), MZ3_APP_NAME, NULL);
	}
	else {
		::MessageBox(m_hWnd, _T("���ɓo�^����Ă��܂�"), MZ3_APP_NAME, NULL);
	}
#else
	// TODO �b��
	::MessageBox(m_hWnd, _T("�R�~���j�e�B�ȊO��\n�o�^�o���܂���"), MZ3_APP_NAME, NULL);
#endif
}

/**
 * �u�b�N�}�[�N����폜
 */
void CReportView::OnDelBookmark()
{
	if (theApp.m_bookmarkMng.Delete(&m_data,theApp.m_root.GetBookmarkList()) != FALSE) {
		::MessageBox(m_hWnd, _T("�폜���܂���"), MZ3_APP_NAME, NULL);
	}
	else {
		::MessageBox(m_hWnd, _T("�o�^����Ă��܂���"), MZ3_APP_NAME, NULL);
	}
}

/**
 * �摜�c�k
 */
void CReportView::OnLoadImage(UINT nID)
{
	if (m_currentData==NULL) {
		return;
	}

	CString url = m_currentData->GetImage(nID - ID_REPORT_IMAGE-1);
	MZ3LOGGER_DEBUG( L"�摜�_�E�����[�h�J�n url[" + url + L"]" );

	theApp.m_access = true;
	m_abort = FALSE;

	theApp.m_inet.Initialize( m_hWnd, NULL );

	// �C���[�WURL��CGI����擾
	theApp.m_accessType = ACCESS_IMAGE;
	theApp.m_inet.DoGet(url, _T(""), CInetAccess::FILE_HTML );

	MyUpdateControlStatus();
}

/**
 * ����c�k
 */
void CReportView::OnLoadMovie(UINT nID)
{
	if (m_currentData==NULL) {
		return;
	}

#ifdef BT_MZ3
	CString url = m_currentData->GetMovie(nID - ID_REPORT_MOVIE-1);
	MZ3LOGGER_DEBUG( L"����_�E�����[�h�J�n url[" + url + L"]" );

	theApp.m_access = true;
	m_abort = FALSE;

	theApp.m_inet.Initialize( m_hWnd, NULL );

	// ����URL��CGI����擾
	theApp.m_accessType = ACCESS_MOVIE;
	theApp.m_inet.DoGet(url, _T(""), CInetAccess::FILE_BINARY );

	MyUpdateControlStatus();
#endif
}


/**
 * �y�[�W�ύX
 */
void CReportView::OnLoadPageLink(UINT nID)
{
	int idx = nID - ID_REPORT_PAGE_LINK_BASE-1;
	if( 0 <= idx && idx <(int)m_data.m_linkPage.size() ) {
		// ok.
	}else{
		return;
	}

	MyLoadMixiViewPage( m_data.m_linkPage[idx] );
}

/**
 * mixi �������N�y�[�W���J��
 */
bool CReportView::MyLoadMixiViewPage( const CMixiData::Link link )
{
	if (theApp.m_access) {
		return false;
	}

	ACCESS_TYPE estimatedAccessType = util::EstimateAccessTypeByUrl( link.url );
	if (theApp.m_accessTypeInfo.getInfoType(estimatedAccessType)==AccessTypeInfo::INFO_TYPE_BODY) {
		// ���ǈʒu��ۑ�
		SaveIndex();

		theApp.m_access = true;
		m_abort = FALSE;

		MyUpdateControlStatus();

		// m_data �̏�������
		{
			// ������
			CMixiData mixi;

			// �f�[�^�\�z
			mixi.SetURL( link.url );
			mixi.SetTitle( link.text );

			// ���O�͈����p��
			mixi.SetName( m_data.GetName() );

			// �A�N�Z�X��ʂ�ݒ�
			mixi.SetAccessType( estimatedAccessType );
			
			m_data = mixi;
			m_data.SetBrowseUri(util::CreateMixiUrl(link.url));
		}
		theApp.m_inet.Initialize( m_hWnd, NULL, theApp.GetInetAccessEncodingByAccessType(m_data.GetAccessType()) );

		theApp.m_accessType = m_data.GetAccessType();
		theApp.m_inet.DoGet( util::CreateMixiUrl(link.url), _T(""), CInetAccess::FILE_HTML );
		return true;
	} else {

		m_infoEdit.ShowWindow(SW_SHOW);
		util::MySetInformationText( m_hWnd, L"���T�|�[�g��URL�ł��F" + link.url );
		return false;
	}

	return true;
}

/**
 * �ēǍ�
 */
void CReportView::OnReloadPage()
{
	// �A�N�Z�X��ʂ��u�w���v�v�Ȃ牽�����Ȃ�
	switch( m_data.GetAccessType() ) {
	case ACCESS_HELP:
		return;
	}

	// ���ǈʒu��ۑ�
	SaveIndex();

	theApp.m_access = true;
	m_abort = FALSE;

	theApp.m_inet.Initialize( m_hWnd, NULL, theApp.GetInetAccessEncodingByAccessType(m_data.GetAccessType()) );

	theApp.m_accessType = m_data.GetAccessType();
	theApp.m_inet.DoGet( util::CreateMixiUrl(m_data.GetURL()), _T(""), CInetAccess::FILE_HTML );

	MyUpdateControlStatus();
}

/**
 * URL
 */
void CReportView::OnLoadUrl(UINT nID)
{
	if (m_currentData==NULL) {
		return;
	}

	UINT idx = nID - (ID_REPORT_URL_BASE+1);
	if( m_currentData->m_linkList.size() == 0 || idx > m_currentData->m_linkList.size() ) {
		return;
	}

	// mixi �������N�ł���΂����Ń��[�h����B
	if (MyLoadMixiViewPage( m_currentData->m_linkList[idx] )) {
		return;
	} else {
		// mixi�������N�łȂ���΃G���[���\������Ă���̂ŉB��
		m_infoEdit.ShowWindow(SW_HIDE);
	}

	LPCTSTR url  = m_currentData->m_linkList[idx].url;
	LPCTSTR text = m_currentData->m_linkList[idx].text;

	// mixi �������N�łȂ��������̂ŁA�u���E�U�ŊJ�����̏������s��

	// �m�F���
	CString msg;
	msg.Format( 
		L"���L��URL���J���܂��B\n"
		L"�ǂ̕��@�ŊJ���܂����H\n\n"
		L"%s", url );

	CCommonSelectDlg dlg;
	dlg.SetTitle( L"�I�[�v�����@�̑I��" );
	dlg.SetMessage( msg );
	dlg.SetButtonText( CCommonSelectDlg::BUTTONCODE_SELECT1, L"�u���E�U�ŊJ��" );
#ifndef WINCE
	dlg.SetButtonText( CCommonSelectDlg::BUTTONCODE_SELECT2, MZ3_APP_NAME L"�Ń_�E�����[�h" );
#else
	if( _tcsstr( url , L"maps.google.co.jp" ) == NULL ){
		dlg.SetButtonText( CCommonSelectDlg::BUTTONCODE_SELECT2, MZ3_APP_NAME L"�Ń_�E�����[�h" );
	} else {
		dlg.SetButtonText( CCommonSelectDlg::BUTTONCODE_SELECT2, L"Google Maps�ŊJ��" );
	}
#endif
	dlg.SetButtonText( CCommonSelectDlg::BUTTONCODE_CANCEL,  L"�L�����Z��" );
	if( dlg.DoModal() != IDOK ) {
		return;
	}

	int r = dlg.m_pressedButtonCode;
	switch( r ) {
	case CCommonSelectDlg::BUTTONCODE_SELECT1:
		// �u���E�U�ŊJ��
		{
			// �������O�C���ϊ�
			CString requestUrl = url;

			// WindowsMobile �ł̂ݎ������O�C��URL�ϊ����s��
/*
#ifdef WINCE
			if( theApp.m_optionMng.m_bConvertUrlForMixiMobile ) {
				// mixi ���o�C���p�������O�C��URL�ϊ�
				requestUrl = util::ConvertToMixiMobileAutoLoginUrl( url );
			} else {
				// mixi �p�������O�C��URL�ϊ�
				requestUrl = util::ConvertToMixiAutoLoginUrl( url );
			}
#endif
*/
			// �u���E�U�ŊJ��
			util::OpenUrlByBrowser( requestUrl );
		}
		break;
	case CCommonSelectDlg::BUTTONCODE_SELECT2:
#ifdef WINCE
		if( _tcsstr( url , L"maps.google.co.jp" ) != NULL && _tcsstr( url , L"ll=" ) != NULL ) {
			// Google Maps�ŊJ��
			static MyRegex reg;
			if( util::CompileRegex( reg, L"[&|\\?]ll=(\\-?[0-9\\.]*,\\-?[0-9\\.]*)" ) ) {
				if( reg.exec(url) && reg.results.size() == 2 ) {
					CString commandline = L"/Program Files/GoogleMaps/GoogleMaps.exe";
					CString commandparam =  L"-URL \"?action=locn";
					if( _tcsstr( url , L"t=h" ) ){
						commandparam += "&view=satv";
					} else {
						commandparam += "&view=mapv";
					}
					commandparam += "&a=@latlon:";
					commandparam += reg.results[1].str.c_str();
					commandparam += L"\"";

					// Google Maps���N��
					util::OpenUrlByBrowser(commandline, commandparam);
					break;
				}
			}
		}
#endif
		// MZ3�Ń_�E�����[�h
		{
			theApp.m_access = true;
			m_abort = FALSE;

			// �_�E�����[�h�t�@�C���p�X
			theApp.m_inet.Initialize( m_hWnd, NULL );
			theApp.m_accessType = ACCESS_DOWNLOAD;

			MyUpdateControlStatus();

			theApp.m_inet.DoGet(url, _T(""), CInetAccess::FILE_BINARY );
		}
		break;
	default:
		break;
	}
}

/**
 * �A�N�Z�X�I���ʒm(HTML)
 */
LRESULT CReportView::OnGetEnd(WPARAM wParam, LPARAM lParam)
{
	TRACE(_T("InetAccess End\n"));
	util::MySetInformationText( m_hWnd, _T("��͒�") );

	if (m_abort) {
		::SendMessage(m_hWnd, WM_MZ3_GET_ABORT, NULL, lParam);
		return TRUE;
	}

	theApp.m_access = false;

	// ���O�C���y�[�W�ȊO�ł���΁A�ŏ��Ƀ��O�A�E�g�`�F�b�N���s���Ă���
	if (theApp.m_accessType != ACCESS_LOGIN && theApp.IsMixiLogout(theApp.m_accessType)) {
		// ���O�A�E�g��ԂɂȂ��Ă���
		MZ3LOGGER_INFO(_T("�ēx���O�C�����Ă���f�[�^���擾���܂��B"));
		util::MySetInformationText( m_hWnd, L"�ēx���O�C�����Ă���f�[�^���擾���܂�" );

		// mixi �f�[�^��ۑ��i�Ҕ��j
		theApp.m_mixiBeforeRelogin = m_data;
		theApp.m_mixi4recv.SetAccessType(ACCESS_LOGIN);

		// ���O�C�����s
		theApp.m_accessType = ACCESS_LOGIN;
		theApp.StartMixiLoginAccess(m_hWnd, &theApp.m_mixi4recv);

		return TRUE;
	}

	bool bRetry = false;
	switch( theApp.m_accessType ) {
#ifdef BT_MZ3
	case ACCESS_IMAGE:
	case ACCESS_MOVIE:
		{
			CHtmlArray html;
			html.Load( theApp.m_filepath.temphtml );

			//�C���[�W��URL���擾
			CString url;
			switch( theApp.m_accessType ) {
			case ACCESS_IMAGE:		url = mixi::ShowPictureParser::GetImageURL( html );	break;
			case ACCESS_MOVIE:		url = theApp.m_inet.GetURL();							break;
			default:
				break;
			}

			util::MySetInformationText( m_hWnd, _T("����") );

			// �C���[�W�̃t�@�C�����𐶐�
			CString strFilepath;
			CString strFilename;

			// url ����t�@�C�����𒊏o����B
			// url : http://ic32.mixi.jp/p/ZZZ/ZZZ/album/ZZ/ZZ/XXXXX.jpg
			// url : http://ic46.mixi.jp/p/ZZZ/ZZZ/diary/ZZ/ZZ/XXXXX.jpg
			// url : http://ic32.mixi.jp/p/ZZZ/ZZZ/album/ZZ/ZZ/XXXXX.flv
			// �Ƃ肠���� / �ȍ~���g��
			int idx = url.ReverseFind( '/' );
			if( idx == -1 ) {
				// ������Ȃ������̂Ńt�@�C�����s���G���[
				CString msg;
				msg.Format( 
					L"�t�@�C�������s���̂��ߑ��s�ł��܂���\n"
					L" url : [%s]", url );
				MZ3LOGGER_ERROR( msg );
				MessageBox( msg );
				return 0;
			}else{ 
				strFilename = url.Mid(idx+1);
				switch( theApp.m_accessType ) {
				case ACCESS_IMAGE:
					strFilepath.Format(_T("%s\\%s"), 
						theApp.m_filepath.imageFolder, 
						strFilename );
					break;
				case ACCESS_MOVIE:
					strFilepath.Format(_T("%s\\%s"), 
						theApp.m_filepath.downloadFolder, 
						strFilename );
					break;
				}
			}

			// ���Ƀ_�E�����[�h�ς݂Ȃ�ă_�E�����[�h���邩�m�F�B
			if( util::ExistFile( strFilepath ) ) {
				CString msg;
				msg.Format( 
					L"�����̃t�@�C�����_�E�����[�h�ς݂ł��B\n"
					L"�t�@�C�����F%s\n\n"
					L"�ă_�E�����[�h���܂����H"
					, strFilename );

				CCommonSelectDlg dlg;
				dlg.SetTitle( L"�I�[�v�����@�̑I��" );
				dlg.SetMessage( msg );
				dlg.SetButtonText( CCommonSelectDlg::BUTTONCODE_SELECT1, L"�ă_�E�����[�h����" );
				dlg.SetButtonText( CCommonSelectDlg::BUTTONCODE_SELECT2, L"�_�E�����[�h�ς݃t�@�C�����J��" );
				dlg.SetButtonText( CCommonSelectDlg::BUTTONCODE_CANCEL,  L"����ς��߂�" );
				if( dlg.DoModal() != IDOK ) {
					break;
				}

				int r = dlg.m_pressedButtonCode;
				switch( r ) {
				case CCommonSelectDlg::BUTTONCODE_SELECT1:
					// �ă_�E�����[�h
					// �_�E�����[�h���s�B
					bRetry = true;
					break;
				case CCommonSelectDlg::BUTTONCODE_SELECT2:
					// �_�E�����[�h�ς݃t�@�C�����J��
					util::OpenByShellExecute( strFilepath );
					break;
				default:
					break;
				}
			}else{
				bRetry = true;
			}

			if( bRetry ) {
				// �C���[�W�E������ă_�E�����[�h
				theApp.m_inet.DoGet(url, _T(""), CInetAccess::FILE_BINARY );
			}
		}
		break;

	case ACCESS_LOGIN:
		// ���O�C���������ǂ����̊m�F
		{
			CHtmlArray html;
			html.Load( theApp.m_filepath.temphtml );
			if( mixi::HomeParser::IsLoginSucceeded(html) ) {
				// ���O�C������
				if (wcslen(theApp.m_loginMng.GetMixiOwnerID()) != 0) {
					MZ3LOGGER_DEBUG( L"OwnerID �擾�ς�" );
				} else {
					MZ3LOGGER_INFO( L"OwnerID�����擾�Ȃ̂ŁA���O�C�����A�擾���� (2)" );

					((CMixiData*)lParam)->SetAccessType(ACCESS_MAIN);
					theApp.m_accessType = ACCESS_MAIN;
					theApp.m_inet.DoGet(L"http://mixi.jp/check.pl?n=%2Fhome.pl", L"", CInetAccess::FILE_HTML );
					return TRUE;
				}
			} else {
				// ���O�C�����s
				LPCTSTR msg = L"���O�C���Ɏ��s���܂���";
				util::MySetInformationText( m_hWnd, msg );
				::MessageBox(m_hWnd, msg, MZ3_APP_NAME, MB_ICONERROR);

				::SendMessage(m_hWnd, WM_MZ3_POST_ABORT, NULL, lParam);
				return TRUE;
			}
		}
		break;

	case ACCESS_MAIN:
		{
			CHtmlArray html;
			html.Load( theApp.m_filepath.temphtml );

			// �p�[�X
			MZ3Data data;
			theApp.DoParseMixiHomeHtml(&data, &html);

			if (wcslen(theApp.m_loginMng.GetMixiOwnerID()) == 0) {
				LPCTSTR msg = L"���O�C���Ɏ��s���܂���(2)";
				util::MySetInformationText( m_hWnd, msg );

				MZ3LOGGER_ERROR( msg );

				::SendMessage(m_hWnd, WM_MZ3_POST_ABORT, NULL, lParam);
				return TRUE;
			}

			// �f�[�^��Ҕ��f�[�^�ɖ߂�
			m_data = theApp.m_mixiBeforeRelogin;
			theApp.m_accessType = m_data.GetAccessType();
			theApp.m_inet.DoGet(util::CreateMixiUrl(m_data.GetURL()), L"", CInetAccess::FILE_HTML );
		}
		break;
#endif

	default:
		if( theApp.m_accessType == m_data.GetAccessType() ) {
			// �����[�h or �y�[�W�ύX

			CHtmlArray html;
			html.Load( theApp.m_filepath.temphtml );

			// HTML ���
			parser::MyDoParseMixiHtml( m_data.GetAccessType(), m_data, html );
			util::MySetInformationText( m_hWnd, _T("wait...") );

			theApp.m_pReportView->SetData( m_data );
			util::MySetInformationText( m_hWnd, L"����" );

			// ���|�[�g�r���[�ɑJ��
			theApp.EnableCommandBarButton( ID_BACK_BUTTON, TRUE );
#ifdef BT_MZ3
			theApp.EnableCommandBarButton( ID_FORWARD_BUTTON, theApp.m_pWriteView->IsWriteCompleted() ? FALSE : TRUE);
#else
			theApp.EnableCommandBarButton( ID_FORWARD_BUTTON, FALSE );
#endif
			theApp.ChangeView( theApp.m_pReportView );

			// ���O�t�@�C���ɕۑ�
			if( theApp.m_optionMng.m_bSaveLog ) {
				// �ۑ��t�@�C���p�X�̐���
				CString strLogfilePath = util::MakeLogfilePath( m_data );
				if( !strLogfilePath.IsEmpty() ) {
					// �ۑ��t�@�C���ɃR�s�[
					CopyFile( theApp.m_filepath.temphtml, strLogfilePath, FALSE/*bFailIfExists, �㏑��*/ );
				}
			}
		}else{
			util::MySetInformationText( m_hWnd, _T("��ʕs��") );
		}
		break;
	}

	if( bRetry == false ) {
		MyUpdateControlStatus();
	}

	return TRUE;
}

/**
 * �A�N�Z�X�I���ʒm��M(Binary)
 */
LRESULT CReportView::OnGetEndBinary(WPARAM wParam, LPARAM lParam)
{
	if (m_abort) {
		::SendMessage(m_hWnd, WM_MZ3_GET_ABORT, NULL, NULL);
		return TRUE;
	}

	theApp.m_access = false;

	MyUpdateControlStatus();

	// MZ3 API : �C�x���g�n���h���֐��Ăяo��
	util::MyLuaDataList rvals;
	rvals.push_back(util::MyLuaData(0));
	CStringA serializeKey = CStringA(theApp.m_accessTypeInfo.getSerializeKey(theApp.m_accessType));
	if (util::CallMZ3ScriptHookFunctions2("get_end_binary_report_view", &rvals, 
			util::MyLuaData(serializeKey),
			util::MyLuaData(theApp.m_inet.m_dwHttpStatus),
			util::MyLuaData(CStringA(theApp.m_inet.GetURL())),
			util::MyLuaData(CStringA(theApp.m_filepath.temphtml))
			))
			
	{
		// �C�x���g�n���h������
		return TRUE;
	}


	// �ۑ��t�@�C���ɃR�s�[
	// �p�X����
	CString strFilepath;
	{
		CString url = theApp.m_inet.GetURL();

		switch( theApp.m_accessType ) {
#ifdef BT_MZ3
		case ACCESS_IMAGE:
			strFilepath.Format(_T("%s\\%s"), 
				theApp.m_filepath.imageFolder, 
				util::ExtractFilenameFromUrl( url, L"_mz3_noname.png" ) );
			break;

		case ACCESS_MOVIE:
			strFilepath.Format(_T("%s\\%s"), 
				theApp.m_filepath.downloadFolder, 
				util::ExtractFilenameFromUrl( url, L"_mz3_noname.flv" ) );
			break;
#endif

		default:
			strFilepath.Format(_T("%s\\%s"), 
				theApp.m_filepath.downloadFolder, 
				util::ExtractFilenameFromUrl( url, L"_mz3_noname.dat" ) );
			break;
		}

		// �R�s�[
		CopyFile( theApp.m_filepath.temphtml, strFilepath, FALSE/*bFailIfExists, �㏑��*/ );
	}

	// �_�E�����[�h�̏ꍇ�́A���s���m�F����B
	if( theApp.m_optionMng.m_bUseRunConfirmDlg ) {
		CString msg;
		msg.Format( 
			L"�_�E�����[�h���������܂����I\n\n"
			L"�t�@�C�� %s ���J���܂����H", strFilepath );

		if( MessageBox( msg, 0, MB_YESNO | MB_ICONQUESTION | MB_DEFBUTTON2 ) != IDYES ) {
			return TRUE;
		}
	}

	// �t�@�C�����J��
	util::OpenByShellExecute(strFilepath);

	return TRUE;
}

/**
 * �A�N�Z�X�G���[�ʒm��M
 */
LRESULT CReportView::OnGetError(WPARAM wParam, LPARAM lParam)
{
	LPCTSTR smsg = L"�G���[���������܂���";
	util::MySetInformationText( m_hWnd, smsg );

	CString msg;
	msg.Format( 
		L"%s\n\n"
		L"�����F%s", smsg, theApp.m_inet.GetErrorMessage() );
//	::MessageBox(m_hWnd, msg, MZ3_APP_NAME, MB_ICONSTOP | MB_OK);
	MZ3LOGGER_ERROR( msg );

	theApp.m_access = false;

	MyUpdateControlStatus();

	return TRUE;
}

/**
 * �A�N�Z�X���f�ʒm��M
 * 
 * ���[�U�w���ɂ�钆�f
 * 
 */
LRESULT CReportView::OnGetAbort(WPARAM wParam, LPARAM lParam)
{
	// ���[�U����̃A�N�Z�X���f�w����������ƁA�����ɒʒm���A���Ă���
	// �{�^�������ɖ߂��ă��b�Z�[�W�������ď����I��
	util::MySetInformationText( m_hWnd, _T("���f���܂���") );

	theApp.m_access = false;

	MyUpdateControlStatus();

	return TRUE;
}

/**
 * ���f�{�^���������̏���
 */
LRESULT CReportView::OnAbort(WPARAM wParam, LPARAM lParam)
{
	// �ʐM���łȂ��Ȃ炷���ɏI������
	if( !theApp.m_inet.IsConnecting() ) {
		return TRUE;
	}
	theApp.m_inet.Abort();
	m_abort = TRUE;

	LPCTSTR msg = _T("���f���܂���");
	util::MySetInformationText( m_hWnd, msg );
//	::MessageBox(m_hWnd, msg, MZ3_APP_NAME, MB_ICONSTOP | MB_OK);

	theApp.m_access = false;

	MyUpdateControlStatus();

	return TRUE;
}

/**
 * �A�N�Z�X���ʒm
 */
LRESULT CReportView::OnAccessInformation(WPARAM wParam, LPARAM lParam)
{
	m_infoEdit.SetWindowText(*(CString*)lParam);
	return TRUE;
}

/**
 * �C���[�W�{�^���������̏���
 */
void CReportView::OnImageButton()
{
	if (m_currentData==NULL) {
		return;
	}

	POINT pt    = util::GetPopupPos();
	int   flags = util::GetPopupFlags();

	CMenu menu;
	menu.LoadMenu(IDR_IMAGE_MENU);
	CMenu* pcThisMenu = menu.GetSubMenu(0);

	CString imageName;
	if (m_currentData != NULL) {
		if (m_currentData->GetImageCount() > 0) {
			// �Z�p���[�^��t���Ă���
			pcThisMenu->AppendMenu(MF_SEPARATOR, ID_REPORT_IMAGE, _T("-"));

			// �摜�����N�𐶐�����
			for (int i=0; i<m_currentData->GetImageCount(); i++) {
				imageName.Format(_T("�摜%02d"), i+1);
				pcThisMenu->AppendMenu(MF_STRING, ID_REPORT_IMAGE+i+1, imageName);
			}
		}
	}

	pcThisMenu->DeleteMenu(0, MF_BYPOSITION);
	menu.GetSubMenu(0)->TrackPopupMenu(flags, pt.x, pt.y, this);
}

/**
 * �������݊J�n�i�{�����̓r���[�̕\���j
 */
void CReportView::OnWriteComment()
{
#ifdef BT_MZ3
	// ���p���@�̊m�F
	quote::QuoteType quoteType = quote::QUOTETYPE_INVALID;
	int idx = m_list.GetSelectedItem();
	CMixiData* data = NULL;
	if( idx >= 0 ){
		data = (CMixiData*)m_list.GetItemData(idx);
	}

	{
		CQuoteDlg dlg;
		dlg.m_pMixi = data;
		dlg.DoModal();
		quoteType = dlg.m_quoteType;

		// �L�����Z���Ȃ�I��
		if( quoteType == quote::QUOTETYPE_INVALID ) {
			return;
		}
	}

	// �������ݎ�ʂ̎擾
	WRITEVIEW_TYPE writeViewType = ACCESS_INVALID;

	// MZ3 API : �t�b�N�֐��Ăяo��
	util::MyLuaDataList rvals;
	rvals.push_back(util::MyLuaData((int)ACCESS_INVALID));
	if (util::CallMZ3ScriptHookFunctions2("get_write_view_type_by_report_item_access_type", &rvals, 
			util::MyLuaData(&m_data))) {
		writeViewType = (WRITEVIEW_TYPE) rvals[0].m_number;
		MZ3LOGGER_DEBUG(util::FormatString(L"estimated write view type by lua : %d", writeViewType));
	} else {
		MZ3LOGGER_ERROR(L"Lua ���ŏ������Ȃ��̂Œ��~");
		MessageBox(L"�T�|�[�g����Ă��Ȃ�����ł�");
		return;
	}

	// �������݃r���[��\��
	CWriteView* pWriteView = theApp.m_pWriteView;
	pWriteView->StartWriteView( writeViewType, &m_data );

	// ���p����
	if( quoteType != quote::QUOTETYPE_INVALID && data != NULL ) {
		CString str = quote::MakeQuoteString( *data, quoteType );

		// �����r���[�^�O������
		ViewFilter::RemoveRan2ViewTag( str );

		((CEdit*)pWriteView->GetDlgItem(IDC_WRITE_BODY_EDIT))->SetWindowText(str);
	}
#endif
}

/**
 * �������݃{�^��
 */
void CReportView::OnWriteButton()
{
	OnWriteComment();
}

/**
 * �e�h�s
 */
LRESULT CReportView::OnFit(WPARAM wParam, LPARAM lParam)
{
	RECT rect;
	SystemParametersInfo(SPI_GETWORKAREA, 0, &rect, 0);
	if (theApp.m_Platforms.PocketPC) {
		OnSize(SIZE_RESTORED, rect.right - rect.left, rect.bottom - (rect.top*2));
	}

	theApp.EnableCommandBarButton( ID_WRITE_BUTTON, TRUE );
	theApp.EnableCommandBarButton( ID_OPEN_BROWSER, TRUE );

	return TRUE;
}

/**
 * ���r���[����̕��A����
 */
LRESULT CReportView::OnChangeView(WPARAM wParam, LPARAM lParam)
{
	theApp.ChangeView(theApp.m_pReportView);

	// Write �r���[���L���Ȃ�ON�ɁB
#ifdef BT_MZ3
	theApp.EnableCommandBarButton(ID_FORWARD_BUTTON, theApp.m_pWriteView->IsWriteCompleted() ? FALSE : TRUE);
#else
	theApp.EnableCommandBarButton(ID_FORWARD_BUTTON, FALSE);
#endif

	theApp.EnableCommandBarButton(ID_WRITE_BUTTON, TRUE);
	theApp.EnableCommandBarButton(ID_OPEN_BROWSER, TRUE);

	return TRUE;
}

/**
 * �ēǍ��i���e��Ȃǁj
 */
LRESULT CReportView::OnReload(WPARAM wParam, LPARAM lParam)
{
	theApp.m_access = true;
	m_abort = FALSE;

	theApp.m_inet.Initialize( m_hWnd, NULL, theApp.GetInetAccessEncodingByAccessType(m_data.GetAccessType()) );
	theApp.m_accessType = m_data.GetAccessType();
	theApp.m_inet.DoGet( util::CreateMixiUrl(m_data.GetURL()), _T(""), CInetAccess::FILE_HTML );

	MyUpdateControlStatus();

	return TRUE;
}

/**
 * �u���E�U�ŊJ��
 */
void CReportView::OnOpenBrowser()
{
	util::OpenUrlByBrowserWithConfirm( m_data.GetBrowseUri() );
}

/**
 * �u���E�U�Ń��[�U�̃v���t�B�[���y�[�W���J��
 */
void CReportView::OnOpenBrowserUser()
{
	// �I���A�C�e���̎擾
	int idx = m_list.GetSelectedItem();
	if( idx < 0 ) {
		return;
	}
	CMixiData* data = (CMixiData*)m_list.GetItemData(idx);

	// ���[�U����URL�̐���
	CString strUserName = data->GetAuthor();
	int nUserId = data->GetAuthorID();
	if( nUserId < 0 ) {
		nUserId = data->GetOwnerID();
	}
	CString url;
	url.Format( L"http://mixi.jp/show_friend.pl?id=%d", nUserId );

	// �J��
	util::OpenUrlByBrowserWithConfirmForUser( url, strUserName );
}

/**
 * �v���t�B�[���y�[�W���J��
 */
void CReportView::OnOpenProfile()
{
	// �I���A�C�e���̎擾
	int idx = m_list.GetSelectedItem();
	if( idx < 0 ) {
		return;
	}
	CMixiData* data = (CMixiData*)m_list.GetItemData(idx);

	// URL�̐���
	int nUserId = util::GetUserIdFromAuthorOrOwnerID(*data);
	if (nUserId<0) {
		return;
	}
	
	CString url;
	url.Format( L"http://mixi.jp/show_friend.pl?id=%d", nUserId );

	// �J��
	MyLoadMixiViewPage( CMixiData::Link( url, data->GetAuthor() ) );
}

/**
 * �v���t�B�[���y�[�W���J���i���O�j
 */
void CReportView::OnOpenProfileLog()
{
#ifdef BT_MZ3
	// �I���A�C�e���̎擾
	int idx = m_list.GetSelectedItem();
	if( idx < 0 ) {
		return;
	}
	CMixiData* data = (CMixiData*)m_list.GetItemData(idx);

	// URL�̐���
	int nUserId = util::GetUserIdFromAuthorOrOwnerID(*data);
	if (nUserId<0) {
		return;
	}
	
	CString url;
	url.Format( L"http://mixi.jp/show_friend.pl?id=%d", nUserId );

	// �J��
	static CMixiData s_mixi;
	CMixiData dummy;
	s_mixi = dummy;
	s_mixi.SetAccessType(ACCESS_PROFILE);
	s_mixi.SetURL(url);
	s_mixi.SetBrowseUri(url);

	CString strLogfilePath = util::MakeLogfilePath( s_mixi );

	// �t�@�C�����݊m�F
	if(! util::ExistFile( strLogfilePath ) ) {
		// FILE NOT FOUND.
		CString msg = L"���O�t�@�C��������܂��� : " + strLogfilePath;
		MZ3LOGGER_ERROR( msg );

		util::MySetInformationText( m_hWnd, msg );
		
		return;
	}

	// HTML �̎擾
	CHtmlArray html;
	html.Load( strLogfilePath );

	// HTML ���
	parser::MyDoParseMixiHtml( s_mixi.GetAccessType(), s_mixi, html );
	util::MySetInformationText( m_hWnd, L"����" );

	// URL �ݒ�
	s_mixi.SetBrowseUri( util::CreateMixiUrl(s_mixi.GetURL()) );

	// �\��
	SetData( s_mixi );
#endif
}


/**
 * ���b�Z�[�W���M
 */
void CReportView::OnSendMessage()
{
#ifdef BT_MZ3
	// �I���A�C�e���̎擾
	int idx = m_list.GetSelectedItem();
	if( idx < 0 ) {
		return;
	}
	CMixiData* data = (CMixiData*)m_list.GetItemData(idx);

	// URL�̐���
	int nUserId = util::GetUserIdFromAuthorOrOwnerID(*data);
	if (nUserId<0) {
		return;
	}
	CString url;
	url.Format( L"http://mixi.jp/show_friend.pl?id=%d", nUserId );

	// mixi �A�C�e���̐���
	static CMixiData s_mixi;
	CMixiData dummy;
	s_mixi = dummy;
	s_mixi.SetAccessType(ACCESS_PROFILE);
	s_mixi.SetURL(url);
	s_mixi.SetBrowseUri(url);

	// hack : OwnerID���Ȃ����AuthorID�𑗐M�惆�[�UID�Ƃ���
	s_mixi.SetOwnerID(data->GetOwnerID()>0 ? data->GetOwnerID() : data->GetAuthorID());

	// �������݉�ʐ���
	theApp.m_pWriteView->StartWriteView( WRITEVIEW_TYPE_NEWMESSAGE, &s_mixi );
#endif
}

/**
 * ���j���[����
 */
void CReportView::MyPopupReportMenu(POINT pt_, int flags_)
{
	POINT pt    = pt_;
	int   flags = flags_;

	if (pt.x==0 && pt.y==0) {
		pt = util::GetPopupPosForSoftKeyMenu2();
		flags = util::GetPopupFlagsForSoftKeyMenu2();
	}

	// MZ3 API : �t�b�N�֐��Ăяo��
	util::MyLuaDataList rvals;
	rvals.push_back(util::MyLuaData(0));
	CStringA serializeKey = CStringA(theApp.m_accessTypeInfo.getSerializeKey(m_data.GetAccessType()));
	if (util::CallMZ3ScriptHookFunctions2("popup_report_menu", &rvals, 
			util::MyLuaData(serializeKey), 
			util::MyLuaData(&m_data), 
			util::MyLuaData(m_list.GetSelectedItem()),
			util::MyLuaData(this)))
	{
		return;
	}

	// TODO ���L�̃r���g�C���R�[�h��Lua�X�N���v�g�����邱��
	CMenu menu;
	menu.LoadMenu(IDR_REPORT_MENU);
	CMenu* pcThisMenu = menu.GetSubMenu(0);

	// �u�i�ށv�폜
	int idxPage = 7;		// �u�y�[�W�v���j���[�i���ւ��L���̏ꍇ��-1�ƂȂ�j
	int idxDiarySeparator = 5;
#ifdef BT_MZ3
	if( theApp.m_pWriteView->IsWriteCompleted() ) {
#else
	if( false ) {
#endif
		pcThisMenu->RemoveMenu(ID_NEXT_MENU, MF_BYCOMMAND);
		idxPage --;
		idxDiarySeparator--;
	}

	// �u�������݁v�Ɋւ��鏈��
	switch( m_data.GetAccessType() ) {
#ifdef BT_MZ3
	case ACCESS_MESSAGE:
		// ���b�Z�[�W�Ȃ�A���M������M�����ɂ���ď������قȂ�
		if( m_data.GetURL().Find( L"&box=outbox" ) != -1 ) {
			// ���M���Ȃ̂ŁA�������ݖ���
			pcThisMenu->RemoveMenu(ID_WRITE_COMMENT, MF_BYCOMMAND);
		}else{
			// ��M���Ȃ̂ŁA�u���b�Z�[�W�ԐM�v�ɕύX
			pcThisMenu->ModifyMenu( ID_WRITE_COMMENT,
				MF_BYCOMMAND,
				ID_WRITE_COMMENT,
				_T("���b�Z�[�W�ԐM"));
		}
		break;

	case ACCESS_DIARY:
	case ACCESS_NEIGHBORDIARY:
	case ACCESS_MYDIARY:
	case ACCESS_BBS:
	case ACCESS_ENQUETE:
	case ACCESS_EVENT:
	case ACCESS_EVENT_JOIN:
		// ���L�A�g�s���ł���ΗL��
		break;

	case ACCESS_NEWS:
#endif
	default:
		// �j���[�X���Ȃ珑�����ݖ���
		pcThisMenu->RemoveMenu(ID_WRITE_COMMENT, MF_BYCOMMAND);
		idxPage --;
		idxDiarySeparator --;
		break;

	}

	// �O���u���O�Ȃ�A�������ݖ���
//	if (m_data.IsOtherDiary() != FALSE) {
//		pcThisMenu->RemoveMenu(ID_WRITE_COMMENT, MF_BYCOMMAND);
//	}

	// �O�̓��L�A���̓��L�̏���
	switch( m_data.GetAccessType() ) {
#ifdef BT_MZ3
	case ACCESS_DIARY:
	case ACCESS_NEIGHBORDIARY:
	case ACCESS_MYDIARY:
		if( m_data.GetPrevDiary().IsEmpty() ){
			pcThisMenu->RemoveMenu(ID_MENU_PREV_DIARY, MF_BYCOMMAND);
			idxPage--;
			idxDiarySeparator--;
		}
		if( m_data.GetNextDiary().IsEmpty() ){
			pcThisMenu->RemoveMenu(ID_MENU_NEXT_DIARY, MF_BYCOMMAND);
			idxPage--;
			idxDiarySeparator--;
		}
		if( m_data.GetPrevDiary().IsEmpty() &&  m_data.GetNextDiary().IsEmpty() ){
			pcThisMenu->RemoveMenu(idxDiarySeparator, MF_BYPOSITION);
			idxPage--;
		}
		// �u�S�Ă�\���v���j���[�̏���
		{
			CString fulldiarylink = m_data.GetFullDiary();
			if( fulldiarylink.IsEmpty() ){
				pcThisMenu->RemoveMenu(IDM_LOAD_FULL_DIARY, MF_BYCOMMAND);
			} else {
				// �����N����������j���[�ɐݒ肷��
				std::vector<CMixiData::Link> list_;
				mixi::ParserUtil::ExtractURI( fulldiarylink , list_ );

				pcThisMenu->ModifyMenu( IDM_LOAD_FULL_DIARY ,  MF_BYCOMMAND | MF_STRING , IDM_LOAD_FULL_DIARY , list_[0].text );
			}
		}
		break;
#endif

	default:
		// �O�̓��L��
		pcThisMenu->RemoveMenu(ID_MENU_PREV_DIARY, MF_BYCOMMAND);
		idxPage--;
		idxDiarySeparator--;

		// ���̓��L��
		pcThisMenu->RemoveMenu(ID_MENU_NEXT_DIARY, MF_BYCOMMAND);
		idxPage--;
		idxDiarySeparator--;

		// ���̓��L�ցA�̌�̃Z�p���[�^
		pcThisMenu->RemoveMenu(idxDiarySeparator, MF_BYPOSITION);
		idxPage--;

		// �y�[�W�b�S�Ă�\��
		pcThisMenu->RemoveMenu(IDM_LOAD_FULL_DIARY, MF_BYCOMMAND);
	}

	// �u�y�[�W�v����сuURL���R�s�[�v�̒ǉ�

	// URL�R�s�[�T�u���j���[
	CMenu* pcEditSubItem = NULL;
	CMenu cEditCopyURLSubItem;

	cEditCopyURLSubItem.CreatePopupMenu();

	bool bSubLinkAppended = false;

	// �y�[�W�����N������Βǉ��B
	if( !m_data.m_linkPage.empty() ) {
		// �����N������̂Œǉ��B
		CMenu* pSubMenu = pcThisMenu->GetSubMenu( idxPage );
		if( pSubMenu != NULL ) {
			// �ǉ�
			for (int i=0; i<(int)m_data.m_linkPage.size(); i++) {
				const CMixiData::Link& link = m_data.m_linkPage[i];
				pSubMenu->AppendMenu(MF_STRING, ID_REPORT_PAGE_LINK_BASE+i+1, link.text);
			}
		}
	}

	// �摜
	if (m_currentData != NULL) {
		if (m_currentData->GetImageCount() > 0) {
			// �Z�p���[�^��ǉ�
			pcThisMenu->AppendMenu(MF_SEPARATOR, ID_REPORT_IMAGE, _T("-"));
			for (int i=0; i<m_currentData->GetImageCount(); i++) {
				CString imageName;
				imageName.Format(_T("�摜%02d"), i+1);
				pcThisMenu->AppendMenu(MF_STRING, ID_REPORT_IMAGE+i+1, imageName);
				// �T�u���j���[�ɉ摜�����N�R�s�[���j���[��ǉ�
				cEditCopyURLSubItem.AppendMenu(MF_STRING, ID_REPORT_COPY_IMAGE+i+1, imageName);
				bSubLinkAppended = true;
			}
		}
	}

	// ����
	if (m_currentData != NULL) {
		if (m_currentData->GetMovieCount() > 0) {
			pcThisMenu->AppendMenu(MF_SEPARATOR, ID_REPORT_MOVIE, _T("-"));
			if( bSubLinkAppended ){
				// �T�u���j���[�ɃZ�p���[�^��ǉ�
				cEditCopyURLSubItem.AppendMenu(MF_SEPARATOR, ID_REPORT_COPY_MOVIE, _T("-"));
			}
			for (int i=0; i<m_currentData->GetMovieCount(); i++) {
				CString MovieName;
				MovieName.Format(_T("����%02d"), i+1);
				pcThisMenu->AppendMenu(MF_STRING, ID_REPORT_MOVIE+i+1, MovieName);
				// �T�u���j���[�ɓ��惊���N�R�s�[���j���[��ǉ�
				cEditCopyURLSubItem.AppendMenu(MF_STRING, ID_REPORT_COPY_MOVIE+i+1, MovieName);
				bSubLinkAppended = true;
			}
		}
	}

	// �����N
	if( m_currentData != NULL ) {
		int n = (int)m_currentData->m_linkList.size();
		if( n > 0 ) {
			pcThisMenu->AppendMenu(MF_SEPARATOR, ID_REPORT_URL_BASE, _T("-"));
			if( bSubLinkAppended ){
				// �T�u���j���[�ɃZ�p���[�^��ǉ�
				cEditCopyURLSubItem.AppendMenu(MF_SEPARATOR, ID_REPORT_COPY_URL_BASE, _T("-"));
			}
			for( int i=0; i<n; i++ ) {
				// �ǉ�
				CString s;
				s.Format( L"link : %s", m_currentData->m_linkList[i].text );
				pcThisMenu->AppendMenu( MF_STRING, ID_REPORT_URL_BASE+(i+1), s);
				// �T�u���j���[�Ƀ����N�R�s�[���j���[��ǉ�
				cEditCopyURLSubItem.AppendMenu( MF_STRING, ID_REPORT_COPY_URL_BASE+(i+1), s);
				bSubLinkAppended = true;
			}
		}
	}

	// �����N�T�u���j���[�����j���[�ɒǉ�
	if( bSubLinkAppended ){
		// �u�ҏW�v-�u�R�s�[�v�T�u���j���[��T��
		for( UINT idxMenu = 0 ; idxMenu < pcThisMenu->GetMenuItemCount() ; idxMenu++ ){
			pcEditSubItem = pcThisMenu->GetSubMenu( idxMenu );
			if( pcEditSubItem ) {
				if( pcEditSubItem->GetMenuItemID( 0 ) == ID_EDIT_COPY ) {
					break;
				}
			}
			pcEditSubItem = NULL;
		}
		// �u�R�s�[�v�̉��ɁuURL���R�s�[�v�T�u���j���[��ǉ�����
		if( pcEditSubItem ) {
			pcEditSubItem->AppendMenu( MF_SEPARATOR, ID_REPORT_COPY_IMAGE, _T("-"));
			pcEditSubItem->AppendMenu( MF_POPUP , (UINT_PTR)cEditCopyURLSubItem.m_hMenu , _T("URL���R�s�["));
		}
	}

	// �u���E�U�ŊJ��(���̃y�[�W)�FURL���Ȃ���Ζ���
	if (m_currentData->GetURL().IsEmpty()) {
		pcThisMenu->RemoveMenu(ID_OPEN_BROWSER, MF_BYCOMMAND);
	}

	// �u���E�U�ŊJ��(���[�U�y�[�W), 
	// �v���t�B�[���y�[�W���J��, ���b�Z�[�W�𑗐M����FID���Ȃ���Ζ���
	int nUserId = util::GetUserIdFromAuthorOrOwnerID(*m_currentData);
	if (nUserId<=0) {
		pcThisMenu->RemoveMenu(ID_OPEN_BROWSER_USER, MF_BYCOMMAND);
		pcThisMenu->RemoveMenu(ID_OPEN_PROFILE, MF_BYCOMMAND);
		pcThisMenu->RemoveMenu(ID_OPEN_PROFILE_LOG, MF_BYCOMMAND);
		pcThisMenu->RemoveMenu(ID_SEND_MESSAGE, MF_BYCOMMAND);
	} else {
		// �v���t�B�[���y�[�W���J���i���O�j�̓��O���Ȃ���Ζ�����
#ifdef BT_MZ3
		CMixiData mixi;
		mixi.SetAccessType( ACCESS_PROFILE );
		mixi.SetURL( util::FormatString( L"http://mixi.jp/show_friend.pl?id=%d", nUserId ) );
		CString path = util::MakeLogfilePath( mixi );
		if (!util::ExistFile(path)) {
			pcThisMenu->EnableMenuItem( ID_OPEN_PROFILE_LOG, MF_GRAYED | MF_BYCOMMAND );
		}
#endif
	}

	// ���j���[�̃|�b�v�A�b�v
	menu.GetSubMenu(0)->TrackPopupMenu(flags, pt.x, pt.y, this);	  
}

/**
 * ���݂̍��ڂɊւ���f�o�b�O����\������
 */
void CReportView::OnShowDebugInfo()
{
	int idx = m_list.GetSelectedItem();
	if( idx < 0 ) {
		return;
	}

	CMixiData* data = (CMixiData*)m_list.GetItemData(idx);
	theApp.ShowMixiData( data );
}


void CReportView::OnUpdateWriteComment(CCmdUI *pCmdUI)
{
}

/// �O�̉�ʂɖ߂�{�^�������C�x���g
void CReportView::OnMenuBack()
{
	CMainFrame* pMainFrame = (CMainFrame*)theApp.m_pMainWnd;
	pMainFrame->OnBackButton();
}

/// �E�\�t�g�L�[���j���[�b�O��
void CReportView::OnBackMenu()
{
	CMainFrame* pMainFrame = (CMainFrame*)theApp.m_pMainWnd;
	pMainFrame->OnBackButton();
}

/// �E�\�t�g�L�[���j���[�b����
void CReportView::OnNextMenu()
{
	CMainFrame* pMainFrame = (CMainFrame*)theApp.m_pMainWnd;
	pMainFrame->OnForwardButton();
}

/**
 * ��M�T�C�Y�ʒm
 */
LRESULT CReportView::OnAccessLoaded(WPARAM dwLoaded, LPARAM dwLength)
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

int CReportView::GetListWidth(void)
{
	CRect rect;
	GetWindowRect( &rect );
	int w = rect.Width();

	// �s�N�Z�����̔������i�X�N���[������菭������������j
#ifdef WINCE
	switch( theApp.GetDisplayMode() ) {
	case SR_VGA:
		w -= 30;
		break;
	case SR_QVGA:
	default:
		w -= 30/2;
		break;
	}
#else
	w -= 30;
#endif
	return w;
}

/**
 * �\������v�f�ɉ����āA�J�����T�C�Y�i���j���Đݒ肷��B
 */
void CReportView::ResetColumnWidth(const CMixiData& mixi)
{
	if( m_list.m_hWnd == NULL )
		return;

	// �v�f��ʂ��u�w���v�v�Ȃ������\�����Ȃ��B

	// ���̒�`
	int w = GetListWidth();

	// �w���v�Ȃ�AW_COL1:(W_COL2+W_COL3):0 �̔䗦�ŕ�������
	// �w���v�ȊO�Ȃ�AW_COL1:W_COL2:W_COL3 �̔䗦�ŕ�������
	const int W_COL1 = theApp.m_optionMng.m_nReportViewListCol1Ratio;
	const int W_COL2 = theApp.m_optionMng.m_nReportViewListCol2Ratio;
	const int W_COL3 = theApp.m_optionMng.m_nReportViewListCol3Ratio;
	switch( mixi.GetAccessType() ) {
	case ACCESS_HELP:
		// �w���v
		m_list.SetColumnWidth(0, w * W_COL1/(W_COL1+W_COL2+W_COL3) );
		m_list.SetColumnWidth(1, w * (W_COL2+W_COL3)/(W_COL1+W_COL2+W_COL3) );
		m_list.SetColumnWidth(2, 0 );
		break;
	default:
		// �w���v�ȊO
		m_list.SetColumnWidth(0, w * W_COL1/(W_COL1+W_COL2+W_COL3) );
		m_list.SetColumnWidth(1, w * W_COL2/(W_COL1+W_COL2+W_COL3) );
		m_list.SetColumnWidth(2, w * W_COL3/(W_COL1+W_COL2+W_COL3) );
		break;
	}
}

/**
 * �w�b�_�̃h���b�O�I��
 *
 * �J�������̍č\�z���s���B
 */
void CReportView::OnHdnEndtrackReportList(NMHDR *pNMHDR, LRESULT *pResult)
{
	MZ3LOGGER_DEBUG( L"OnHdnEndtrackReportList" );

	LPNMHEADER phdr = reinterpret_cast<LPNMHEADER>(pNMHDR);

	// �J�������̔��f
	m_list.SetColumnWidth( phdr->iItem, phdr->pitem->cxy );

	CRect rect;
	CHeaderCtrl* pHeader = NULL;

	// ���X�g�̎擾
	if( (pHeader = m_list.GetHeaderCtrl()) == NULL ) {
		MZ3LOGGER_ERROR( L"���X�g�̃w�b�_���擾�ł��Ȃ��̂ŏI��" );
		return;
	}

	// �J�����P
	if(! pHeader->GetItemRect( 0, rect ) ) {
		MZ3LOGGER_ERROR( L"���X�g�̃w�b�_�A��1�J�����̕����擾�ł��Ȃ��̂ŏI��" );
		return;
	}
	theApp.m_optionMng.m_nReportViewListCol1Ratio = rect.Width();

	// �J�����Q
	if(! pHeader->GetItemRect( 1, rect ) ) {
		MZ3LOGGER_ERROR( L"���X�g�̃w�b�_�A��2�J�����̕����擾�ł��Ȃ��̂ŏI��" );
		return;
	}
	theApp.m_optionMng.m_nReportViewListCol2Ratio = rect.Width();

	// �J�����R
	// �ŏI�J�����Ȃ̂ŁA���X�g��-���̃J�����T�C�Y�Ƃ���B
	theApp.m_optionMng.m_nReportViewListCol3Ratio
		= GetListWidth() 
			- theApp.m_optionMng.m_nReportViewListCol1Ratio
			- theApp.m_optionMng.m_nReportViewListCol2Ratio;

	*pResult = 0;
}

LRESULT CReportView::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message) {
	case WM_NOTIFY:
		break;
	}

	return CFormView::WindowProc(message, wParam, lParam);
}

static const int N_HC_MIN = 10;		///< ���X�g�̍ŏ��l [%]
static const int N_HC_MAX = 90;		///< ���X�g�̍ő�l [%]
static const int N_HB_MIN = 10;		///< �G�f�B�b�g�̍ŏ��l [%]
static const int N_HB_MAX = 90;		///< �G�f�B�b�g�̍ő�l [%]

/**
 * ���X�g����������
 */
void CReportView::OnLayoutReportlistMakeNarrow()
{
	int& hc = theApp.m_optionMng.m_nReportViewListHeightRatio;
	int& hb = theApp.m_optionMng.m_nReportViewBodyHeightRatio;

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
 * ���X�g���L������
 */
void CReportView::OnLayoutReportlistMakeWide()
{
	int& hc = theApp.m_optionMng.m_nReportViewListHeightRatio;
	int& hb = theApp.m_optionMng.m_nReportViewBodyHeightRatio;

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
 * ���X�g�̉E�N���b�N�C�x���g
 */
void CReportView::OnNMRclickReportList(NMHDR *pNMHDR, LRESULT *pResult)
{
	MyPopupReportMenu();

	*pResult = 0;
}


void CReportView::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	CString logStr;
	int newPos = pScrollBar->GetScrollPos();
	int pageOffset = (m_detailView->GetViewLineMax()-1);

	// �X�N���[���o�[�̈ʒu���s�����z����Ȃ疳�����ŏ����𒆒f
	if( newPos > m_scrollBarHeight ){
		return;
	}

	switch( nSBCode ){

		case SB_THUMBTRACK:	// �m�u�ɂ��ړ��͋֎~
			if( (int)nPos < m_scrollBarHeight )
				newPos = nPos;
			break;

		case SB_PAGEUP:
			if( newPos - pageOffset >= 0 )
				newPos -= pageOffset;
			else
				newPos = 0;
			break;

		case SB_PAGEDOWN:
			if( newPos + pageOffset <= m_scrollBarHeight )
				newPos += pageOffset;
			else
				newPos = m_scrollBarHeight;
			break;

		case SB_LINEUP:
			if( newPos - 1 >= 0 )
				newPos -= 1;
			break;

		case SB_LINEDOWN:
			if( newPos + 1 <= m_scrollBarHeight )
				newPos += 1;
			break;
	}

	// �I���ʒm�ȊO�͈ʒu���Đݒ肷��B
	if( nSBCode != SB_ENDSCROLL ){ 
		// �b���LINEUP/DOWN,PAGEUP/DOWN�݂̂�������
		if( nSBCode == SB_LINEUP || nSBCode == SB_LINEDOWN ||
			nSBCode == SB_PAGEUP || nSBCode == SB_PAGEDOWN ||
			nSBCode == SB_THUMBTRACK ){
/*			logStr.Format(TEXT("�o�[�ʒu:%d/%d\r\n"),newPos,scrollBarHeight);
			OutputDebugString(logStr);
*/
			pScrollBar->SetScrollPos(newPos);
			m_detailView->ResetDragOffset();
			m_detailView->DrawDetail(newPos);
		}
	}else if( nSBCode == SB_ENDSCROLL ){
	}
}


void CReportView::OnDestroy()
{
	CFormView::OnDestroy();

	m_detailView->DestroyWindow();
}

void CReportView::OnLButtonUp(UINT nFlags, CPoint point)
{
	// �X�N���[���m��B
	// �X�N���[���o�[�̈ʒu��ύX�B
	m_vScrollbar.SetScrollPos( m_detailView->MyGetScrollPos() );

	CFormView::OnLButtonUp(nFlags, point);
}

void CReportView::OnMouseMove(UINT nFlags, CPoint point)
{
	// �X�N���[���o�[�̈ʒu��ύX�B
	m_vScrollbar.SetScrollPos( m_detailView->MyGetScrollPos() );

	CFormView::OnMouseMove(nFlags, point);
}

void CReportView::OnAcceleratorScrollOrNextComment()
{
	// �X�N���[���ʒu����i�X�N���[���]�n������΃X�N���[������j
	if (CommandScrollDownEdit()) {
		return;
	}

	// ���̍��ڂɈړ�
	CommandMoveDownList();
}

void CReportView::OnAcceleratorNextComment()
{
	// ���̍��ڂɈړ�
	CommandMoveDownList();
}

void CReportView::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	if (m_detailView == NULL) {
		return;
	}

	// �㉺N%�ȓ��ł̃_�u���N���b�N�ł���΍��ڕύX
	CRect rect;
	m_detailView->GetClientRect(&rect);
	int y = point.y - rect.top;
	double y_pos_in_percent = y / (double)rect.Height() * 100.0;

#define N_DOUBLE_CLICK_MOVE_ITEM_LIMIT	40.0
	if (y_pos_in_percent < N_DOUBLE_CLICK_MOVE_ITEM_LIMIT) {
		// �O�̍��ڂɈړ�
		CommandMoveUpList();
	} else if (y_pos_in_percent > 100.0-N_DOUBLE_CLICK_MOVE_ITEM_LIMIT) {
		// ���̍��ڂɈړ�
		CommandMoveDownList();
	}
}

void CReportView::OnRButtonUp(UINT nFlags, CPoint point)
{
	MyPopupReportMenu();

//	CFormView::OnRButtonUp(nFlags, point);
}

afx_msg void CReportView::OnEditCopy()
{
	if (m_currentData==NULL) {
		return;
	}

	CString str;

	str += m_currentData->GetAuthor();
	str += _T("�@");
	str += m_currentData->GetDate();
	str += _T("\r\n");

	str += m_currentData->GetBody();

	// �����r���[�^�O������
	ViewFilter::RemoveRan2ViewTag( str );

	util::SetClipboardDataTextW( str );
}

BOOL CReportView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	// ���𑜓x�z�C�[���Ή��̂��߁Adelta �l��ݐς���B
	static int s_delta = 0;
	// �������t�ɂȂ�����L�����Z��
	if ((s_delta>0 && zDelta<0) || (s_delta<0 && zDelta>0)) {
		s_delta = 0;
	}
	s_delta += zDelta;

	if (s_delta>WHEEL_DELTA) {
		CommandScrollUpEdit();
		s_delta -= WHEEL_DELTA;
	} else if (s_delta<-WHEEL_DELTA) {
		CommandScrollDownEdit();
		s_delta += WHEEL_DELTA;
	}
	return TRUE;

//	return CFormView::OnMouseWheel(nFlags, zDelta, pt);
}

/**
 * ���̃R�����g�ֈړ�
 */
LRESULT CReportView::OnMoveDownList(WPARAM dwLoaded, LPARAM dwLength)
{
	return CommandMoveDownList();
}

/**
 * �O�̃R�����g�ֈړ�
 */
LRESULT CReportView::OnMoveUpList(WPARAM dwLoaded, LPARAM dwLength)
{
	return CommandMoveUpList();
}

/**
 * �R�����g���X�g�̃A�C�e�������擾
 */
LRESULT CReportView::OnGetListItemCount(WPARAM dwLoaded, LPARAM dwLength)
{
	return (LRESULT)m_list.GetItemCount();
}
/**
 * �����N��URL���N���b�v�{�[�h�ɃR�s�[
 */
void CReportView::OnCopyClipboardUrl(UINT nID)
{
	if (m_currentData==NULL) {
		return;
	}

	UINT idx = nID - (ID_REPORT_COPY_URL_BASE+1);
	if( idx > m_currentData->m_linkList.size() ) {
		return;
	}

	CMixiData::Link link = m_currentData->m_linkList[idx];
	CString url = link.url;

#ifdef BT_MZ3
	ACCESS_TYPE estimatedAccessType = util::EstimateAccessTypeByUrl( link.url );
	switch (estimatedAccessType) {
	case ACCESS_MYDIARY:
	case ACCESS_DIARY:
	case ACCESS_NEIGHBORDIARY:
	case ACCESS_BBS:
	case ACCESS_ENQUETE:
	case ACCESS_EVENT:
	case ACCESS_EVENT_JOIN:
	case ACCESS_EVENT_MEMBER:
	case ACCESS_PROFILE:
	case ACCESS_BIRTHDAY:
	case ACCESS_MESSAGE:
	case ACCESS_NEWS:
		url = util::CreateMixiUrl(link.url);
		break;
	}
#endif

	util::SetClipboardDataTextW( url );
}

/**
 * �摜��URL���N���b�v�{�[�h�ɃR�s�[
 */
void CReportView::OnCopyClipboardImage(UINT nID)
{
	if (m_currentData==NULL) {
		return;
	}

	UINT idx = nID - (ID_REPORT_COPY_IMAGE+1);
	if( (int)idx > m_currentData->GetImageCount() ) {
		return;
	}

	util::SetClipboardDataTextW( m_currentData->GetImage(idx) );
}

/**
 * �����URL���N���b�v�{�[�h�ɃR�s�[
 */
void CReportView::OnCopyClipboardMovie(UINT nID)
{
	if (m_currentData==NULL) {
		return;
	}

	UINT idx = nID - (ID_REPORT_COPY_MOVIE+1);
	if( (int)idx > m_currentData->GetMovieCount() ) {
		return;
	}

	util::SetClipboardDataTextW( m_currentData->GetMovie(idx) );
}

/**
 * �u���̓��L��>>�v���j���[����
 */
void CReportView::OnMenuNextDiary()
{
	//MZ3_TRACE(L"CReportView::OnMenuNextDiary()\n");

	CString link = m_data.GetNextDiary();
	if( !link.IsEmpty() ){
		std::vector<CMixiData::Link> list_;
		mixi::ParserUtil::ExtractURI( link , list_ );

		if( list_.size() > 0 ){
			// mixi �������N�̂͂��Ȃ̂Ń��[�h����B
			if ( MyLoadMixiViewPage( list_[0] )) {
				// ���X�N���[���A�j���[�V�������N������
				m_list.StartPanScroll( CTouchListCtrl::PAN_SCROLL_DIRECTION_LEFT );
				return;
			} else {
				// mixi�������N�łȂ���΃G���[���\������Ă���̂ŉB��
				m_infoEdit.ShowWindow(SW_HIDE);
			}
		}

	}
}

/**
 * �u<<�O�̓��L�ցv���j���[����
 */
void CReportView::OnMenuPrevDiary()
{
	//MZ3_TRACE(L"CReportView::OnMenuPrevDiary()\n");

	CString link = m_data.GetPrevDiary();
	if( !link.IsEmpty() ){
		std::vector<CMixiData::Link> list_;
		mixi::ParserUtil::ExtractURI( link , list_ );

		if( list_.size() > 0 ){
			// mixi �������N�̂͂��Ȃ̂Ń��[�h����B
			if ( MyLoadMixiViewPage( list_[0] )) {
				// ���X�N���[���A�j���[�V�������N������
				m_list.StartPanScroll( CTouchListCtrl::PAN_SCROLL_DIRECTION_RIGHT );
				return;
			} else {
				// mixi�������N�łȂ���΃G���[���\������Ă���̂ŉB��
				m_infoEdit.ShowWindow(SW_HIDE);
			}
		}
	}
}

/**
 * �u���̓��L��>>�v���j���[����������
 */
void CReportView::OnUpdateMenuNextDiary(CCmdUI *pCmdUI)
{
	pCmdUI->Enable( false );
	if( !m_data.GetNextDiary().IsEmpty() ){
		pCmdUI->Enable( true );
	}
}

/**
 * �u<<�O�̓��L�ցv���j���[����������
 */
void CReportView::OnUpdateMenuPrevDiary(CCmdUI *pCmdUI)
{
	pCmdUI->Enable( false );
	if( !m_data.GetPrevDiary().IsEmpty() ){
		pCmdUI->Enable( true );
	}
}

/**
 * �u�S�Ă�\���v���j���[����
 */
void CReportView::OnLoadFullDiary()
{
	//MZ3_TRACE(L"CReportView::OnLoadFullDiary()\n");

	CString link = m_data.GetFullDiary();
	if( !link.IsEmpty() ){
		std::vector<CMixiData::Link> list_;
		mixi::ParserUtil::ExtractURI( link , list_ );

		if( list_.size() > 0 ){
			// mixi �������N�̂͂��Ȃ̂Ń��[�h����B
			if ( MyLoadMixiViewPage( list_[0] )) {
				return;
			} else {
				// mixi�������N�łȂ���΃G���[���\������Ă���̂ŉB��
				m_infoEdit.ShowWindow(SW_HIDE);
			}
		}
	}
}

/**
 * �u�S�Ă�\���v���j���[����������
 */
void CReportView::OnUpdateLoadFullDiary(CCmdUI *pCmdUI)
{
	pCmdUI->Enable( false );
	if( !m_data.GetFullDiary().IsEmpty() ){
		pCmdUI->Enable( true );
	}
}

void CReportView::MyUpdateControlStatus(void)
{
	if (theApp.m_access) {
		MZ3LOGGER_DEBUG(L"****** CReportView::MyUpdateControlStatus(1)");
		theApp.EnableCommandBarButton( ID_BACK_BUTTON, FALSE);
		theApp.EnableCommandBarButton( ID_STOP_BUTTON, TRUE);
		theApp.EnableCommandBarButton( ID_IMAGE_BUTTON, FALSE);
		theApp.EnableCommandBarButton( ID_WRITE_BUTTON, FALSE);
		theApp.EnableCommandBarButton( ID_OPEN_BROWSER, FALSE);

		// �v���O���X�o�[��\��
		mc_progressBar.ShowWindow( SW_SHOW );

		m_infoEdit.ShowWindow(SW_SHOW);

	} else {
		MZ3LOGGER_DEBUG(L"****** CReportView::MyUpdateControlStatus(0)");
		theApp.EnableCommandBarButton( ID_STOP_BUTTON, FALSE);
		theApp.EnableCommandBarButton( ID_BACK_BUTTON, TRUE);
		theApp.EnableCommandBarButton( ID_WRITE_BUTTON, TRUE);
		theApp.EnableCommandBarButton( ID_IMAGE_BUTTON, m_imageState);
		theApp.EnableCommandBarButton( ID_OPEN_BROWSER, TRUE);

		// �v���O���X�o�[���\��
		mc_progressBar.ShowWindow( SW_HIDE );

		m_infoEdit.ShowWindow(SW_HIDE);
	}
}

/**
 * MZ3 API �œo�^���ꂽ���j���[�̃C�x���g
 */
void CReportView::OnLuaMenu(UINT nID)
{
	UINT idx = nID - ID_LUA_MENU_BASE;
	if (idx >= theApp.m_luaMenus.size()) {
		MZ3LOGGER_ERROR(util::FormatString(L"�s���ȃ��j���[ID�ł� [%d]", nID));
		return;
	}

	// Lua�֐����擾���Ăяo��
	const std::string& strFuncName = theApp.m_luaMenus[idx];
	util::CallMZ3ScriptHookFunction("", "select_menu", strFuncName.c_str(), &m_data);
}
