// ReportView.cpp : �����t�@�C��
//

#include "stdafx.h"
#include "MZ3.h"
#include "ReportView.h"
#include "MainFrm.h"
#include "IniFile.h"
#include "WriteView.h"
#include "HtmlArray.h"
#include "util.h"
#include "QuoteDlg.h"
#include "MixiParser.h"

#define MASK_COLOR RGB(255,0,255);

// CReportView

IMPLEMENT_DYNCREATE(CReportView, CFormView)

/**
 * �R���X�g���N�^
 */
CReportView::CReportView()
	: CFormView(CReportView::IDD)
{
	m_data = NULL;
	m_whiteBr = CreateSolidBrush(RGB(255, 255, 255));
	m_imageState = FALSE;
}

/**
 * �f�X�g���N�^
 */
CReportView::~CReportView()
{
	m_pimgList->DeleteImageList();
	delete m_pimgList;
	m_pimgList = NULL;
}

void CReportView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_REPORT_LIST, m_list);
	DDX_Control(pDX, IDC_REPORT_EDIT, m_edit);
	DDX_Control(pDX, IDC_TITLE_EDIT, m_titleEdit);
	DDX_Control(pDX, IDC_PROGRESS_BAR, mc_progressBar);
	DDX_Control(pDX, IDC_INFO_EDIT, m_infoEdit);
}

BEGIN_MESSAGE_MAP(CReportView, CFormView)
	ON_WM_SIZE()
	ON_WM_CTLCOLOR()
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_REPORT_LIST, &CReportView::OnLvnItemchangedReportList)
	ON_NOTIFY(LVN_KEYDOWN, IDC_REPORT_LIST, &CReportView::OnLvnKeydownReportList)

	ON_MESSAGE(WM_MZ3_ACCESS_LOADED, OnAccessLoaded)
	ON_COMMAND(ID_ADD_BOOKMARK, &CReportView::OnAddBookmark)
	ON_COMMAND(ID_DEL_BOOKMARK, &CReportView::OnDelBookmark)
    ON_COMMAND_RANGE(ID_REPORT_IMAGE+1, ID_REPORT_IMAGE+50, OnLoadImage)
	ON_COMMAND_RANGE(ID_REPORT_PAGE_LINK_BASE+1, ID_REPORT_PAGE_LINK_BASE+50, OnLoadPageLink)
	ON_COMMAND_RANGE(ID_REPORT_URL_BASE+1, ID_REPORT_URL_BASE+50, OnLoadUrl)
    ON_COMMAND(ID_IMAGE_BUTTON, OnImageButton)
	ON_COMMAND(IDM_RELOAD_PAGE, OnReloadPage)
    ON_MESSAGE(WM_MZ3_GET_END, OnGetEnd)
    ON_MESSAGE(WM_MZ3_GET_END_BINARY, OnGetImageEnd)
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
	ON_COMMAND(ID_SELECT_ALL, &CReportView::OnSelectAll)
    ON_MESSAGE(WM_MZ3_FIT, OnFit)
    ON_COMMAND(ID_OPEN_BROWSER, OnOpenBrowser)

	ON_COMMAND(ID_SHOW_DEBUG_INFO, &CReportView::OnShowDebugInfo)
	ON_COMMAND(ID_OPEN_BROWSER_USER, &CReportView::OnOpenBrowserUser)
	ON_UPDATE_COMMAND_UI(ID_WRITE_COMMENT, &CReportView::OnUpdateWriteComment)
	ON_COMMAND(ID_MENU_BACK, &CReportView::OnMenuBack)
	ON_COMMAND(ID_BACK_MENU, &CReportView::OnBackMenu)
	ON_COMMAND(ID_NEXT_MENU, &CReportView::OnNextMenu)
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
		dwStyle |= LVS_REPORT | LVS_OWNERDRAWFIXED;

		// �X�^�C���̍X�V
		m_list.ModifyStyle(0, dwStyle);

		m_pimgList = new CImageList();
		m_pimgList->Create(16, 16, ILC_COLOR4, 2, 0);
		m_pimgList->Add( AfxGetApp()->LoadIcon(IDI_NO_PHOTO_ICON) );
		m_pimgList->Add( AfxGetApp()->LoadIcon(IDI_PHOTO_ICON) );
		m_list.SetImageList(m_pimgList, LVSIL_SMALL);

		// �J�����̒ǉ�
		// ����������������ɍĐݒ肷��̂ŉ��̕����w�肵�Ă����B
		m_list.InsertColumn(0, _T(""), LVCFMT_LEFT, 20, -1);
		m_list.InsertColumn(1, _T("���O"), LVCFMT_LEFT, 20, -1);
		m_list.InsertColumn(2, _T("����"), LVCFMT_LEFT, 20, -1);
	}

	//--- �G�f�B�b�g�̕ύX
	{
		// �t�H���g�ύX
		m_edit.SetFont( &theApp.m_font );
	}

	//--- �ʒm�̈�̕ύX
	{
		// �t�H���g�ύX
		m_infoEdit.SetFont( &theApp.m_font );
	}

	m_nochange = FALSE;
	m_scrollLine = 7;
}

/**
 * �T�C�Y�ύX���̏���
 */
void CReportView::OnSize(UINT nType, int cx, int cy)
{
	CFormView::OnSize(nType, cx, cy);

	int fontHeight = theApp.m_optionMng.GetFontHeight();
	if( fontHeight == 0 ) {
		fontHeight = 24;
	}

	int hTitle  = fontHeight+6*2;			// �^�C�g���̈�̓t�H���g�T�C�Y�ˑ�
	if( theApp.GetDisplayMode() == SR_QVGA ){
		hTitle  = fontHeight - 4;
	}

	const int h1 = theApp.m_optionMng.m_nReportViewListHeightRatio;
	const int h2 = theApp.m_optionMng.m_nReportViewBodyHeightRatio;
	int hList   = (cy * h1 / (h1+h2))-hTitle;	// (�S�̂�N%-�^�C�g���̈�) �����X�g�̈�Ƃ���
	int hReport = (cy * h2 / (h1+h2));			// �S�̂�N%�����|�[�g�̈�Ƃ���

	// ���̈�͕K�v�ɉ����ĕ\������邽�߁A��L�Ƃ͊֌W�Ȃ��B
	int hInfo   = fontHeight+6*2;			// ���̈���t�H���g�T�C�Y�ˑ�
	if( theApp.GetDisplayMode() == SR_QVGA ){
		hInfo   = fontHeight - 4;
	}

	GetDlgItem(IDC_TITLE_EDIT) ->MoveWindow( 0, 0,            cx, hTitle  );
	GetDlgItem(IDC_REPORT_LIST)->MoveWindow( 0, hTitle,       cx, hList   );
	GetDlgItem(IDC_REPORT_EDIT)->MoveWindow( 0, hTitle+hList, cx, hReport );
	GetDlgItem(IDC_INFO_EDIT)  ->MoveWindow( 0, cy - hInfo,   cx, hInfo   );

	m_scrollLine = (hReport / fontHeight) - 2;
	TRACE(_T("Scrol Line = %d\n"), m_scrollLine);

	// �I�𒆂̍s���\�������悤�ɂ���
	if( m_list.m_hWnd != NULL ) {
		int idx = m_list.GetSelectedItem();
		m_list.EnsureVisible( idx, FALSE );
	}

	// �v���O���X�o�[�͕ʓr�z�u
	// �T�C�Y�� hInfo �� 2/3 �Ƃ���
	int hProgress = hInfo * 2 / 3;
	int y = cy - hInfo - hProgress;
	GetDlgItem(IDC_PROGRESS_BAR)->MoveWindow( 0, y, cx, hProgress );

	// ���X�g�J�������̕ύX
	ResetColumnWidth( *m_data );
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
	  return (HBRUSH)m_whiteBr;
	  break;

  default:
	  pDC->SetBkMode(TRANSPARENT);
	  break;
	}

	return m_whiteBr;
}

/**
 * �f�[�^�ݒ�
 */
void CReportView::SetData(CMixiData* data)
{
	m_access = FALSE;
	m_nochange = TRUE;
	m_lastIndex = 0;

	m_data = data;

	m_list.DeleteAllItems();
	m_list.SetRedraw(FALSE);

	// �J�����T�C�Y��ύX����
	ResetColumnWidth( *data );

	// �ǂ��܂Ńf�[�^���擾��������ݒ肷��
	TRACE(_T("Address = %s\n"), m_data->GetURL());

	// URI�𕪉�
	CString tmp;

	if (m_data->GetID() == -1) {
		// �����łh�c�����
		tmp = m_data->GetURL();
		tmp = tmp.Mid(tmp.Find(_T("id=")) + wcslen(_T("id=")));
		tmp = tmp.Left(tmp.Find(_T("&")));
		data->SetID(_wtoi(tmp));
	}

	switch (m_data->GetAccessType()) {
	case ACCESS_DIARY:
	case ACCESS_MYDIARY:
		tmp = m_data->GetURL().Mid(m_data->GetURL().Find(_T("id="))+wcslen(_T("id=")));
		tmp = tmp.Left(tmp.Find(_T("&")));
		tmp.Format(_T("d%s"), tmp);
		break;
	case ACCESS_BBS:
		tmp.Format(_T("b%d"), m_data->GetID());
		break;
	case ACCESS_EVENT:
		tmp.Format(_T("v%d"), m_data->GetID());
		break;
	case ACCESS_ENQUETE:
		tmp.Format(_T("e%d"), m_data->GetID());
		break;

	}

	memset(m_idKey, 0x00, sizeof(char) * 24);
	wcstombs(m_idKey, tmp, 24);

	// ���ǈʒu�̕ύX
	m_lastIndex = m_data->GetLastIndex();
	if (m_lastIndex == -1) {
		m_lastIndex = 0;
	} else {
		m_lastIndex++;
	}


	int focusItem = 0;
	INT_PTR count = m_data->GetChildrenSize();

	if (count != 0 && m_lastIndex != 0) {
		CMixiData& cmtData = m_data->GetChild( count-1 );
		if (cmtData.GetCommentIndex() <= m_lastIndex && m_lastIndex != 0) {
			m_lastIndex = cmtData.GetCommentIndex();
		}
	}

	// �^�C�g���̐ݒ�
	int index;
	CString title = m_data->GetTitle();
	index = title.ReverseFind(_T('('));
	if (index != -1) {
		title = title.Left(index);
	}
	if (m_data->GetAccessType() != ACCESS_MYDIARY &&
		m_data->GetAccessType() != ACCESS_MESSAGE) 
	{
		if( !m_data->GetName().IsEmpty() ) {
			title.AppendFormat( _T("(%s)"), (LPCTSTR)m_data->GetName() );
		}
	}
	m_titleEdit.SetWindowText( title );


	int nItem;
	int imgNo = 0;

	// ----------------------------------------
	// �R�����g�̒ǉ�
	// ----------------------------------------
	TRACE(_T("�R�����g�� = [%d]\n"), count);

	for (int i=0; i<count; i++) {
		CMixiData& cmtData = m_data->GetChild(i);

		if (cmtData.GetImageCount() == 0) {
			imgNo = 0;
		}
		else {
			imgNo = 1;
		}
		nItem = m_list.InsertItem(i+1, util::int2str(cmtData.GetCommentIndex()), imgNo);

		if (cmtData.GetCommentIndex() == m_lastIndex) {
			focusItem = nItem + 1;
		}

		m_list.SetItem(nItem, 1, LVIF_TEXT | LVIF_IMAGE, cmtData.GetAuthor(), 0, 0, 0, 0);
		m_list.SetItem(nItem, 2, LVIF_TEXT, cmtData.GetDate(), 0, 0, 0, 0);
		m_list.SetItemData(nItem, (DWORD_PTR)&cmtData);
	}

	// �e�����X�g�ɕ\��
	if (m_data->GetImageCount() == 0) {
		imgNo = 0;
	}
	else {
		imgNo = 1;
	}
	nItem = m_list.InsertItem(0, _T("-"), imgNo);
	m_list.SetItem(nItem, 1, LVIF_TEXT, m_data->GetAuthor(), 0, 0, 0, 0);
	m_list.SetItemData(0, (DWORD_PTR)m_data);

	m_nochange = FALSE;

	if (count == 0) {
		m_lastIndex = 0;
	}
	else {
		// �擪�̃f�[�^���擾
		CMixiData& cmtData = m_data->GetChild(0);
		if (cmtData.GetCommentIndex() > m_lastIndex && m_lastIndex != 0) {
			m_lastIndex = cmtData.GetCommentIndex();
			focusItem = 1;
		}
	}

	//--- UI �֘A
	m_list.SetRedraw(TRUE);

	m_list.SetFocus();
	m_list.SetItemState( focusItem, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED );

	m_list.EnsureVisible( focusItem, FALSE );

	theApp.EnableCommandBarButton( ID_WRITE_BUTTON, !m_data->IsOtherDiary());
	theApp.EnableCommandBarButton( ID_OPEN_BROWSER, TRUE);

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
	CString str = _T("");

	str += data->GetAuthor();
	str += _T("�@");
	str += data->GetDate();
	str += _T("\r\n");

	for( size_t i=0; i<data->GetBodySize(); i++ ){
		str += data->GetBody(i);
	}

	str += _T("\r\n");			// �Ō�ɂP�s����Č��₷������
	m_edit.SetWindowText(str);
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

	CMixiData* data;
	data = (CMixiData*)m_list.GetItemData(pNMLV->iItem);
	m_currentData = data;
	if (data->GetCommentIndex() > m_lastIndex) {
		m_lastIndex = data->GetCommentIndex();
	}

	m_list.SetRedraw(FALSE);
	if (pNMLV->iItem == 0) {
		ShowParentData(data);
	}
	else {
		ShowCommentData(data);
	}

	m_imageState = (BOOL)(data->GetImageCount() > 0);
	theApp.EnableCommandBarButton( ID_IMAGE_BUTTON, ((!m_access) & m_imageState));
	theApp.EnableCommandBarButton( ID_OPEN_BROWSER, !m_access);

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

BOOL CReportView::PreTranslateMessage(MSG* pMsg)
{
	CMenu menu;
	RECT rect;

	if (pMsg->message == WM_KEYUP) {
		switch (pMsg->wParam) {
		case VK_F1:
			if( theApp.m_optionMng.m_bUseLeftSoftKey ) {
				// ���C�����j���[�̃|�b�v�A�b�v
				CMainFrame* pMainFrame = (CMainFrame*)theApp.m_pMainWnd;
				menu.Attach( pMainFrame->m_wndCommandBar.GetMenu() );

				SystemParametersInfo(SPI_GETWORKAREA, 0, &rect, 0);
				menu.GetSubMenu(0)->TrackPopupMenu(TPM_CENTERALIGN | TPM_VCENTERALIGN,
					rect.left,
					rect.bottom - TOOLBAR_HEIGHT,
					pMainFrame );
				menu.Detach();
				return TRUE;
			}
			break;
		case VK_F2:
			// ���|�[�g���j���[�̕\��
			MyPopupReportMenu();
			break;
		}
	}
	else if (pMsg->message == WM_KEYDOWN) {
		switch (MapVirtualKey(pMsg->wParam, 2)) {
		case VK_BACK:				// �N���A�{�^��
			if (m_access != FALSE) {
				// �A�N�Z�X���͒��f����
				::SendMessage(m_hWnd, WM_MZ3_ABORT, NULL, NULL);
			}
			else {
				OnMenuBack();
			}
			return TRUE;
		case 48:
			TRACE(_T("0 Down\n"));
			return TRUE;
		}

		if (pMsg->hwnd == m_list.m_hWnd) {
			// ���X�g�ł̃L�[�����C�x���g
			switch(pMsg->wParam) {
			case VK_UP:
				if (m_list.GetItemState(0, LVIS_FOCUSED) != FALSE) {
					// ��ԏ�̍��ڑI�𒆂Ȃ̂ŁA��ԉ��Ɉړ�
					m_list.SetItemState(0, 0, LVIS_FOCUSED | LVIS_SELECTED );
					m_list.SetItemState(m_list.GetItemCount()-1, 
						LVIS_FOCUSED | LVIS_SELECTED, LVIS_FOCUSED | LVIS_SELECTED );
					m_list.EnsureVisible(m_list.GetItemCount()-1,FALSE);
					return TRUE;
				}
				break;
			case VK_DOWN:
				if (m_list.GetItemState(m_list.GetItemCount()-1, LVIS_FOCUSED) != FALSE) {
					// ��ԉ��̍��ڑI�𒆂Ȃ̂ŁA��ԏ�Ɉړ�
					m_list.SetItemState(m_list.GetItemCount()-1, 0, LVIS_FOCUSED | LVIS_SELECTED );
					m_list.SetItemState(0, 
						LVIS_FOCUSED | LVIS_SELECTED, LVIS_FOCUSED | LVIS_SELECTED );
					m_list.EnsureVisible(0,FALSE);
					return TRUE;
				}
				break;
			case VK_RIGHT:
				m_edit.LineScroll(m_scrollLine);
				return TRUE;
			case VK_LEFT:
				m_edit.LineScroll(m_scrollLine * -1);
				return TRUE;
			}
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
 * m_lastIndex �� m_idKey �ɕۑ�
 */
void CReportView::SaveIndex()
{
	if (m_data != NULL) {
		theApp.m_logfile.SetValue(m_idKey, (const char*)util::int2str_a(m_lastIndex), "Log");

		m_data->SetLastIndex(m_lastIndex);
	}
}

/**
 * �u�b�N�}�[�N�ɒǉ�
 */
void CReportView::OnAddBookmark()
{

	if (m_data->GetAccessType() != ACCESS_BBS &&
		m_data->GetAccessType() != ACCESS_EVENT &&
		m_data->GetAccessType() != ACCESS_ENQUETE) {
			::MessageBox(m_hWnd, _T("�R�~���j�e�B�ȊO��\n�o�^�o���܂���"), _T("MZ3"), NULL);
			return;
	}

	if( theApp.m_bookmarkMng.Add( m_data, theApp.m_root.GetBookmarkList() ) != FALSE ) {
		::MessageBox(m_hWnd, _T("�o�^���܂���"), _T("MZ3"), NULL);
	}
	else {
		::MessageBox(m_hWnd, _T("���ɓo�^����Ă��܂�"), _T("MZ3"), NULL);
	}


}

/**
 * �u�b�N�}�[�N����폜
 */
void CReportView::OnDelBookmark()
{
	if (theApp.m_bookmarkMng.Delete(m_data,theApp.m_root.GetBookmarkList()) != FALSE) {
		::MessageBox(m_hWnd, _T("�폜���܂���"), _T("MZ3"), NULL);
	}
	else {
		::MessageBox(m_hWnd, _T("�o�^����Ă��܂���"), _T("MZ3"), NULL);
	}
}

/**
 * �摜�c�k
 */
void CReportView::OnLoadImage(UINT nID)
{
	theApp.EnableCommandBarButton( ID_BACK_BUTTON, FALSE);
	theApp.EnableCommandBarButton( ID_STOP_BUTTON, TRUE);
	theApp.EnableCommandBarButton( ID_IMAGE_BUTTON, FALSE);
	theApp.EnableCommandBarButton( ID_WRITE_BUTTON, FALSE);
	theApp.EnableCommandBarButton( ID_OPEN_BROWSER, FALSE);

	m_infoEdit.ShowWindow(SW_SHOW);

	TCHAR uri[256];
	memset(uri, 0x00, sizeof(TCHAR)*256);
	wcscpy(uri, m_currentData->GetImage(nID - ID_REPORT_IMAGE-1));

	m_access = TRUE;

	m_abort = FALSE;

	theApp.m_inet.Initialize( m_hWnd, NULL );

	//�C���[�WURL��CGI����擾
	theApp.m_accessType = ACCESS_IMAGE;
	theApp.m_inet.DoGet(uri, _T(""), CInetAccess::FILE_HTML );
}

/**
 * �y�[�W�ύX
 */
void CReportView::OnLoadPageLink(UINT nID)
{
	int idx = nID - ID_REPORT_PAGE_LINK_BASE-1;
	if( 0 <= idx && idx <(int)m_data->m_linkPage.size() ) {
		// ok.
	}else{
		return;
	}

	// ���ǈʒu��ۑ�
	SaveIndex();

	theApp.EnableCommandBarButton( ID_BACK_BUTTON, FALSE);
	theApp.EnableCommandBarButton( ID_STOP_BUTTON, TRUE);
	theApp.EnableCommandBarButton( ID_IMAGE_BUTTON, FALSE);
	theApp.EnableCommandBarButton( ID_WRITE_BUTTON, FALSE);
	theApp.EnableCommandBarButton( ID_OPEN_BROWSER, FALSE);

	m_infoEdit.ShowWindow(SW_SHOW);

	const CMixiData::Link& link = m_data->m_linkPage[idx];

	m_access = TRUE;
	m_abort = FALSE;

	// m_data �̏�������
	{
		// ���̂͂Ƃ肠���� static �ɂ��Ă����B
		// m_data ���g������������Ɓu���O���J���v���قȂ��Ă��܂����߁B
		static CMixiData s_mixi;
		// ������
		CMixiData mixi;	// �������p

		// �f�[�^�\�z
		mixi.SetURL( link.url );
		mixi.SetTitle( link.text );

		// ���O�͈����p��
		mixi.SetName( m_data->GetName() );

		// �A�N�Z�X��ʂ� URL ���琄��
		ACCESS_TYPE estimatedAccessType = util::EstimateAccessTypeByUrl( link.url );
		if( estimatedAccessType != ACCESS_INVALID ) {
			mixi.SetAccessType( estimatedAccessType );
		}

		s_mixi = mixi;
		m_data = &s_mixi;
	}

	theApp.m_inet.Initialize( m_hWnd, NULL );

	theApp.m_accessType = m_data->GetAccessType();
	theApp.m_inet.DoGet( util::CreateMixiUrl(link.url), _T(""), CInetAccess::FILE_HTML );
}

/**
 * �ēǍ�
 */
void CReportView::OnReloadPage()
{
	// �A�N�Z�X��ʂ��u�w���v�v�Ȃ牽�����Ȃ�
	if( m_data->GetAccessType() == ACCESS_HELP ) {
		return;
	}

	// ���ǈʒu��ۑ�
	SaveIndex();

	theApp.EnableCommandBarButton( ID_BACK_BUTTON, FALSE);
	theApp.EnableCommandBarButton( ID_STOP_BUTTON, TRUE);
	theApp.EnableCommandBarButton( ID_IMAGE_BUTTON, FALSE);
	theApp.EnableCommandBarButton( ID_WRITE_BUTTON, FALSE);
	theApp.EnableCommandBarButton( ID_OPEN_BROWSER, FALSE);

	m_infoEdit.ShowWindow(SW_SHOW);

	m_access = TRUE;
	m_abort = FALSE;

	theApp.m_inet.Initialize( m_hWnd, NULL );

	theApp.m_accessType = m_data->GetAccessType();
	theApp.m_inet.DoGet( util::CreateMixiUrl(m_data->GetURL()), _T(""), CInetAccess::FILE_HTML );
}

/**
 * URL
 */
void CReportView::OnLoadUrl(UINT nID)
{
	UINT idx = nID - (ID_REPORT_URL_BASE+1);
	if( idx > m_currentData->m_linkList.size() ) {
		return;
	}

	LPCTSTR url  = m_currentData->m_linkList[idx].url;
	LPCTSTR text = m_currentData->m_linkList[idx].text;

	// �m�F���
	CString msg;
	msg.Format( 
		L"���L��URL���J���܂��B\n"
		L"�u���E�U�ŊJ���܂����H\n\n"
		L"�y�͂��z�u���E�U�ŊJ��\n"
		L"�y�������zMZ3�Ń_�E�����[�h\n"
		L"�y�L�����Z���z����ς��߂�\n\n"
		L"%s", url );
	int r = MessageBox( msg, L"MZ3", MB_YESNOCANCEL | MB_DEFBUTTON3 | MB_ICONQUESTION );
	switch( r ) {
	case IDYES:
		// �u���E�U�ŊJ��
		util::OpenUrlByBrowser( url );
		break;
	case IDNO:
		// MZ3�Ń_�E�����[�h
		{
			m_access = TRUE;
			m_abort = FALSE;

			// �_�E�����[�h�t�@�C���p�X
			CString strUrl = url;

			m_infoEdit.ShowWindow(SW_SHOW);
			theApp.m_inet.Initialize( m_hWnd, NULL );
			theApp.m_accessType = ACCESS_DOWNLOAD;

			// �{�^����Ԃ̕ύX
			theApp.EnableCommandBarButton( ID_STOP_BUTTON, TRUE);
			theApp.EnableCommandBarButton( ID_BACK_BUTTON, FALSE);
			theApp.EnableCommandBarButton( ID_WRITE_BUTTON, FALSE);
			theApp.EnableCommandBarButton( ID_IMAGE_BUTTON, FALSE);
			theApp.EnableCommandBarButton( ID_OPEN_BROWSER, FALSE);

			theApp.m_inet.DoGet(url, _T(""), CInetAccess::FILE_BINARY );
		}
		break;
	default:
		break;
	}
}

/**
 * �A�N�Z�X�I���ʒm��M(HTML)
 */
LRESULT CReportView::OnGetEnd(WPARAM wParam, LPARAM lParam)
{

	TRACE(_T("InetAccess End\n"));
	util::MySetInformationText( m_hWnd, _T("HTML��͒�") );

	CHtmlArray html;
	html.Load( theApp.m_filepath.temphtml );

	if (m_abort != FALSE) {
		::SendMessage(m_hWnd, WM_MZ3_GET_ABORT, NULL, lParam);
		return LRESULT();
	}

	bool bRetry = false;
	switch( theApp.m_accessType ) {
	case ACCESS_IMAGE:
		{
			//�C���[�W��URL���擾
			CString url = mixi::ShowPictureParser::GetImageURL( html );

			util::MySetInformationText( m_hWnd, _T("����") );

			m_access = FALSE;

			// �C���[�W�̃t�@�C�����𐶐�
			CString strFilepath;
			CString strFilename;
			{
				// url ����t�@�C�����𒊏o����B
				// url : http://ic32.mixi.jp/p/ZZZ/ZZZ/album/ZZ/ZZ/XXXXX.jpg
				// url : http://ic46.mixi.jp/p/ZZZ/ZZZ/diary/ZZ/ZZ/XXXXX.jpg
				// �Ƃ肠���� / �ȍ~���g��
				strFilename = url.Mid(url.ReverseFind( '/' )+1);
				strFilepath.Format(_T("%s\\%s"), 
					theApp.m_filepath.imageFolder, 
					strFilename );
			}

			// ���Ƀ_�E�����[�h�ς݂Ȃ�ă_�E�����[�h���邩�m�F�B
			if( util::ExistFile( strFilepath ) ) {
				CString msg;
				msg.Format( 
					L"�����̃t�@�C�����_�E�����[�h�ς݂ł��B\n"
					L"�t�@�C�����F%s\n\n"
					L"�ă_�E�����[�h���܂����H\n\n"
					L"�y�͂��z�ă_�E�����[�h����\n"
					L"�y�������z�_�E�����[�h�ς݃t�@�C�����J��\n"
					L"�y�L�����Z���z����ς��߂�", strFilename );
				int r = MessageBox( msg, 0, MB_YESNOCANCEL | MB_ICONQUESTION | MB_DEFBUTTON2 );
				switch( r ) {
				case IDYES:
					// �ă_�E�����[�h
					// �_�E�����[�h���s�B
					bRetry = true;
					break;
				case IDNO:
					// �_�E�����[�h�ς݃t�@�C�����J��
					{
						// �t�@�C�����J��
						SHELLEXECUTEINFO sei;
						sei.cbSize       = sizeof(sei);
						sei.fMask        = SEE_MASK_NOCLOSEPROCESS;
						sei.hwnd         = 0;
						sei.lpVerb       = _T("open");
						sei.lpFile       = strFilepath;
						sei.lpParameters = NULL;
						sei.lpDirectory  = NULL;
						sei.nShow        = SW_NORMAL;
						ShellExecuteEx(&sei);
					}
					break;
				default:
					break;
				}
			}else{
				bRetry = true;
			}

			if( bRetry ) {
				// �C���[�W���_�E�����[�h
				theApp.m_accessType = ACCESS_IMAGE;
				theApp.m_inet.DoGet(url, _T(""), CInetAccess::FILE_BINARY );
			}
		}
		break;
	default:
		if( theApp.m_accessType == m_data->GetAccessType() ) {
			// �����[�h or �y�[�W�ύX

			// HTML ���
			mixi::MyDoParseMixiHtml( m_data->GetAccessType(), *m_data, html );
			util::MySetInformationText( m_hWnd, _T("wait...") );

			theApp.m_pReportView->SetData( m_data );
			util::MySetInformationText( m_hWnd, L"����" );

			// ���|�[�g�r���[�ɑJ��
			theApp.EnableCommandBarButton( ID_BACK_BUTTON, TRUE );
			theApp.EnableCommandBarButton( ID_FORWARD_BUTTON, (theApp.m_pWriteView->m_sendEnd == FALSE) ? TRUE : FALSE );
			theApp.ChangeView( theApp.m_pReportView );

			// ���O�t�@�C���ɕۑ�
			if( theApp.m_optionMng.m_bSaveLog ) {
				// �ۑ��t�@�C���p�X�̐���
				CString strLogfilePath = util::MakeLogfilePath( *m_data );
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
		theApp.EnableCommandBarButton( ID_STOP_BUTTON, FALSE);
		theApp.EnableCommandBarButton( ID_BACK_BUTTON, TRUE);
		theApp.EnableCommandBarButton( ID_WRITE_BUTTON, TRUE);
		theApp.EnableCommandBarButton( ID_IMAGE_BUTTON, m_imageState);
		theApp.EnableCommandBarButton( ID_OPEN_BROWSER, TRUE);

		// �v���O���X�o�[���\��
		mc_progressBar.ShowWindow( SW_HIDE );

		m_infoEdit.ShowWindow(SW_HIDE);
	}

	return LRESULT();
}
/**
 * �A�N�Z�X�I���ʒm��M(Image)
 */
LRESULT CReportView::OnGetImageEnd(WPARAM wParam, LPARAM lParam)
{
	if (m_abort) {
		::SendMessage(m_hWnd, WM_MZ3_GET_ABORT, NULL, NULL);
		return LRESULT();
	}

	m_access = FALSE;

	theApp.EnableCommandBarButton( ID_STOP_BUTTON, FALSE);
	theApp.EnableCommandBarButton( ID_BACK_BUTTON, TRUE);
	theApp.EnableCommandBarButton( ID_WRITE_BUTTON, TRUE);
	theApp.EnableCommandBarButton( ID_IMAGE_BUTTON, m_imageState);
	theApp.EnableCommandBarButton( ID_OPEN_BROWSER, TRUE);

	// �v���O���X�o�[���\��
	mc_progressBar.ShowWindow( SW_HIDE );

	m_infoEdit.ShowWindow(SW_HIDE);

	// �ۑ��t�@�C���ɃR�s�[
	// �p�X����
	CString strFilepath;
	{
		CString url = theApp.m_inet.GetURL();

		switch( theApp.m_accessType ) {
		case ACCESS_IMAGE:
			strFilepath.Format(_T("%s\\%s"), 
				theApp.m_filepath.imageFolder, 
				url.Mid( url.ReverseFind( '/' )+1 ) );
			break;
		default:
			strFilepath.Format(_T("%s\\%s"), 
				theApp.m_filepath.downloadFolder, 
				url.Mid( url.ReverseFind( '/' )+1 ) );
			break;
		}

		// �R�s�[
		CopyFile( theApp.m_filepath.temphtml, strFilepath, FALSE/*bFailIfExists, �㏑��*/ );
	}

	// �_�E�����[�h�̏ꍇ�́A���s���m�F����B
//	if( theApp.m_accessType == ACCESS_DOWNLOAD ) {
	{
		CString msg;
		msg.Format( 
			L"�_�E�����[�h���������܂����I\n\n"
			L"�t�@�C�� %s ���J���܂����H", strFilepath );

		if( MessageBox( msg, 0, MB_YESNO | MB_ICONQUESTION | MB_DEFBUTTON2 ) != IDYES ) {
			return LRESULT();
		}
	}

	// �t�@�C�����J��
	SHELLEXECUTEINFO sei;
	sei.cbSize       = sizeof(sei);
	sei.fMask        = SEE_MASK_NOCLOSEPROCESS;
	sei.hwnd         = 0;
	sei.lpVerb       = _T("open");
	sei.lpFile       = strFilepath;
	sei.lpParameters = NULL;
	sei.lpDirectory  = NULL;
	sei.nShow        = SW_NORMAL;
	ShellExecuteEx(&sei);

	return LRESULT();
}

/**
 * �A�N�Z�X�G���[�ʒm��M
 */
LRESULT CReportView::OnGetError(WPARAM wParam, LPARAM lParam)
{
	theApp.EnableCommandBarButton( ID_STOP_BUTTON, FALSE);
	theApp.EnableCommandBarButton( ID_BACK_BUTTON, TRUE);
	theApp.EnableCommandBarButton( ID_IMAGE_BUTTON, TRUE);
	theApp.EnableCommandBarButton( ID_WRITE_BUTTON, TRUE);

	LPCTSTR smsg = L"�G���[���������܂���";
	util::MySetInformationText( m_hWnd, smsg );

	CString msg;
	msg.Format( 
		L"%s\n\n"
		L"�����F%s", smsg, theApp.m_inet.GetErrorMessage() );
	::MessageBox(m_hWnd, msg, _T("MZ3"), MB_ICONSTOP | MB_OK);

	m_access = FALSE;
	m_infoEdit.ShowWindow(SW_HIDE);

	return LRESULT();
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

	// ���~�{�^�����g�p�s�ɂ���
	theApp.EnableCommandBarButton( ID_STOP_BUTTON, FALSE);
	theApp.EnableCommandBarButton( ID_BACK_BUTTON, TRUE);
	theApp.EnableCommandBarButton( ID_IMAGE_BUTTON, m_imageState);
	theApp.EnableCommandBarButton( ID_WRITE_BUTTON, TRUE);

	// �v���O���X�o�[���\��
	mc_progressBar.ShowWindow( SW_HIDE );

	m_infoEdit.ShowWindow(SW_HIDE);

	m_access = FALSE;

	return LRESULT();
}

/**
 * ���f����
 */
LRESULT CReportView::OnAbort(WPARAM wParam, LPARAM lParam)
{
	// �ʐM���łȂ��Ȃ炷���ɏI������
	if( !theApp.m_inet.IsConnecting() ) {
		return LRESULT();
	}
	theApp.m_inet.Abort();
	m_abort = TRUE;

	LPCTSTR msg = _T("���f���܂���");
	util::MySetInformationText( m_hWnd, msg );
//	::MessageBox(m_hWnd, msg, _T("MZ3"), MB_ICONSTOP | MB_OK);

	// ���~�{�^�����g�p�s�ɂ���
	theApp.EnableCommandBarButton( ID_STOP_BUTTON, FALSE);
	theApp.EnableCommandBarButton( ID_BACK_BUTTON, TRUE);
	theApp.EnableCommandBarButton( ID_IMAGE_BUTTON, m_imageState);
	theApp.EnableCommandBarButton( ID_WRITE_BUTTON, TRUE);
	theApp.EnableCommandBarButton( ID_OPEN_BROWSER, TRUE);

	m_infoEdit.ShowWindow(SW_HIDE);

	m_access = FALSE;
	return LRESULT();
}

/**
 * �A�N�Z�X���ʒm
 */
LRESULT CReportView::OnAccessInformation(WPARAM wParam, LPARAM lParam)
{
  m_infoEdit.SetWindowText(*(CString*)lParam);
  return LRESULT();
}

/**
 * �C���[�W�{�^���������̏���
 */
void CReportView::OnImageButton()
{
	POINT pt;
	RECT rect;
	CMenu menu;

	SystemParametersInfo(SPI_GETWORKAREA, 0, &rect, 0);

	pt.x = (rect.right-rect.left) / 2;
	pt.y = (rect.bottom-rect.top) / 2;
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
	menu.GetSubMenu(0)->TrackPopupMenu(TPM_CENTERALIGN | TPM_VCENTERALIGN, pt.x, pt.y, this);
}

/**
 * �������݊J�n�i�{�����̓r���[�̕\���j
 */
void CReportView::OnWriteComment()
{
	// ���p���@�̊m�F
	quote::QuoteType quoteType = quote::QUOTETYPE_INVALID;
	int idx = m_list.GetSelectedItem();
	CMixiData* data = (CMixiData*)m_list.GetItemData(idx);

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

	// �������݃r���[��\��
	CWriteView* pWriteView = ((CWriteView*)theApp.m_pWriteView);
	if (m_data->GetAccessType() == ACCESS_MESSAGE) {
		pWriteView->StartWriteView( WRITEVIEW_TYPE_REPLYMESSAGE, m_data );
	} else {
		pWriteView->StartWriteView( WRITEVIEW_TYPE_COMMENT, m_data );
	}

	// ���p����
	if( quoteType != quote::QUOTETYPE_INVALID && data != NULL ) {
		CString str = quote::MakeQuoteString( *data, quoteType );

		((CEdit*)pWriteView->GetDlgItem(IDC_WRITE_BODY_EDIT))->SetWindowText(str);
	}

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
	OnSize(SIZE_RESTORED, rect.right - rect.left, rect.bottom - (rect.top*2));
	return LRESULT();
}

/**
 * ���r���[����̕��A����
 */
LRESULT CReportView::OnChangeView(WPARAM wParam, LPARAM lParam)
{
	theApp.ChangeView(theApp.m_pReportView);

	// Write �r���[���L���Ȃ�ON�ɁB
	theApp.EnableCommandBarButton( ID_FORWARD_BUTTON, theApp.m_pWriteView->m_sendEnd==FALSE ? TRUE : FALSE );

	theApp.EnableCommandBarButton( ID_WRITE_BUTTON, TRUE );
	theApp.EnableCommandBarButton( ID_OPEN_BROWSER, TRUE );

	return LRESULT();
}

/**
 * �ēǍ��i���e��Ȃǁj
 */
LRESULT CReportView::OnReload(WPARAM wParam, LPARAM lParam)
{
	theApp.EnableCommandBarButton( ID_BACK_BUTTON, FALSE);
	theApp.EnableCommandBarButton( ID_STOP_BUTTON, TRUE);
	theApp.EnableCommandBarButton( ID_IMAGE_BUTTON, FALSE);
	theApp.EnableCommandBarButton( ID_WRITE_BUTTON, FALSE);
	theApp.EnableCommandBarButton( ID_OPEN_BROWSER, FALSE);

	m_infoEdit.ShowWindow(SW_SHOW);

	m_access = TRUE;
	m_abort = FALSE;

	theApp.m_inet.Initialize( m_hWnd, NULL );
	theApp.m_accessType = m_data->GetAccessType();
	theApp.m_inet.DoGet( util::CreateMixiUrl(m_data->GetURL()), _T(""), CInetAccess::FILE_HTML );

	return LRESULT();
}

/**
 * �u���E�U�ŊJ��
 */
void CReportView::OnOpenBrowser()
{
	util::OpenBrowserForUrl( m_data->GetBrowseUri() );
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
	util::OpenBrowserForUser( url, strUserName );
}

void CReportView::MyPopupReportMenu(void)
{
	CMenu menu;
	POINT pt;
	RECT rect;
	SystemParametersInfo(SPI_GETWORKAREA, 0, &rect, 0);

	pt.x = (rect.right - rect.left) / 2;
	pt.y = (rect.bottom - rect.top) / 2;

	menu.LoadMenu(IDR_REPORT_MENU);
	CMenu* pcThisMenu = menu.GetSubMenu(0);

	// �u�������݁v�Ɋւ��鏈��
	switch( m_data->GetAccessType() ) {
	case ACCESS_MESSAGE:
		// ���b�Z�[�W�Ȃ�A���M������M�����ɂ���ď������قȂ�
		if( m_data->GetURL().Find( L"&box=outbox" ) != -1 ) {
			// ���M���Ȃ̂ŁA�������ݖ���
			pcThisMenu->RemoveMenu( ID_WRITE_COMMENT, MF_BYCOMMAND);
		}else{
			// ��M���Ȃ̂ŁA�u���b�Z�[�W�ԐM�v�ɕύX
			pcThisMenu->ModifyMenu( ID_WRITE_COMMENT,
				MF_BYCOMMAND,
				ID_WRITE_COMMENT,
				_T("���b�Z�[�W�ԐM"));
		}
		break;
	case ACCESS_NEWS:
		// �j���[�X�Ȃ珑�����ݖ���
		pcThisMenu->RemoveMenu(ID_WRITE_COMMENT, MF_BYCOMMAND);
		break;
	}

	// �O���u���O�Ȃ�A�������ݖ���
	if (m_data->IsOtherDiary() != FALSE) {
		pcThisMenu->RemoveMenu(ID_WRITE_COMMENT, MF_BYCOMMAND);
	}

	// �u���ցv������
	int idxPage = 4;		// �u�y�[�W�v���j���[�i���ւ��L���̏ꍇ��-1�ƂȂ�j
	if( theApp.m_pWriteView->m_sendEnd ) {
		pcThisMenu->RemoveMenu(ID_NEXT_MENU, MF_BYCOMMAND);
		idxPage = idxPage-1;
	}

	// �u�y�[�W�v�̒ǉ�
	// �y�[�W�����N������Βǉ��B
	if( !m_data->m_linkPage.empty() ) {
		// �����N������̂Œǉ��B
		CMenu* pSubMenu = pcThisMenu->GetSubMenu( idxPage );
		if( pSubMenu != NULL ) {
			// �ǉ�
			for (int i=0; i<(int)m_data->m_linkPage.size(); i++) {
				const CMixiData::Link& link = m_data->m_linkPage[i];
				pSubMenu->AppendMenu(MF_STRING, ID_REPORT_PAGE_LINK_BASE+i+1, link.text);
			}
		}
	}

	// �摜
	if (m_currentData != NULL) {
		if (m_currentData->GetImageCount() > 0) {
			pcThisMenu->AppendMenu(MF_SEPARATOR, ID_REPORT_IMAGE, _T("-"));
			for (int i=0; i<m_currentData->GetImageCount(); i++) {
				CString imageName;
				imageName.Format(_T("�摜%02d"), i+1);
				pcThisMenu->AppendMenu(MF_STRING, ID_REPORT_IMAGE+i+1, imageName);
			}
		}
	}

	// �����N
	if( m_currentData != NULL ) {
		int n = (int)m_currentData->m_linkList.size();
		if( n > 0 ) {
			pcThisMenu->AppendMenu(MF_SEPARATOR, ID_REPORT_URL_BASE, _T("-"));
			for( int i=0; i<n; i++ ) {
				// �ǉ�
				CString s;
				s.Format( L"link : %s", m_currentData->m_linkList[i].text );
				pcThisMenu->AppendMenu( MF_STRING, ID_REPORT_URL_BASE+(i+1), s);
			}
		}
	}

	// ���j���[�̃|�b�v�A�b�v
	menu.GetSubMenu(0)->
		TrackPopupMenu(TPM_CENTERALIGN | TPM_VCENTERALIGN, pt.x, pt.y, this);	  
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

/**
 * �\������v�f�ɉ����āA�J�����T�C�Y�i���j���Đݒ肷��B
 */
void CReportView::ResetColumnWidth(const CMixiData& mixi)
{
	// �v�f��ʂ��u�w���v�v�Ȃ������\�����Ȃ��B

	// ���̒�`
	CRect rect;
	GetWindowRect( &rect );
	int w = rect.Width();
	// �s�N�Z�����̔������i�X�N���[������菭������������j
	switch( theApp.GetDisplayMode() ) {
	case SR_VGA:
		w -= 30;
		break;
	case SR_QVGA:
	default:
		w -= 30/2;
		break;
	}

	// �w���v�Ȃ�A7:(17+21):0 �̔䗦�ŕ�������
	// �w���v�ȊO�Ȃ�A7:17:21 �̔䗦�ŕ�������
	const int W_COL1 =  7;
	const int W_COL2 = 17;
	const int W_COL3 = 21;
	if( mixi.GetAccessType() == ACCESS_HELP ) {
		// �w���v
		m_list.SetColumnWidth(0, w * W_COL1/(W_COL1+W_COL2+W_COL3) );
		m_list.SetColumnWidth(1, w * (W_COL2+W_COL3)/(W_COL1+W_COL2+W_COL3) );
		m_list.SetColumnWidth(2, 0 );
	}else{
		// �w���v�ȊO
		m_list.SetColumnWidth(0, w * W_COL1/(W_COL1+W_COL2+W_COL3) );
		m_list.SetColumnWidth(1, w * W_COL2/(W_COL1+W_COL2+W_COL3) );
		m_list.SetColumnWidth(2, w * W_COL3/(W_COL1+W_COL2+W_COL3) );
	}
}
