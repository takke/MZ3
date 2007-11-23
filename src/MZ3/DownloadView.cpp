// DownloadView.cpp : �����t�@�C��
//

#include "stdafx.h"
#include "MZ3.h"
#include "MZ3View.h"
#include "MainFrm.h"
#include "DownloadView.h"
#include "util.h"
#include "util_gui.h"


// CDownloadView

IMPLEMENT_DYNCREATE(CDownloadView, CFormView)

CDownloadView::CDownloadView()
	: CFormView(CDownloadView::IDD)
	, m_access(FALSE)
	, m_targetItemIndex( -1 )
{
}

CDownloadView::~CDownloadView()
{
}

void CDownloadView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PROGRESS_BAR, mc_progressBar);
	DDX_Control(pDX, IDC_ITEM_LIST, m_list);
	DDX_Control(pDX, IDC_INFO_EDIT, m_infoEdit);
	DDX_Control(pDX, IDC_CONTINUE_CHECK, mc_checkContinue);
	DDX_Control(pDX, IDC_TITLE_EDIT, m_titleEdit);
}

BEGIN_MESSAGE_MAP(CDownloadView, CFormView)
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_CONTINUE_CHECK, &CDownloadView::OnBnClickedContinueCheck)
    ON_MESSAGE(WM_MZ3_FIT, OnFit)
	ON_BN_CLICKED(IDC_START_STOP_BUTTON, &CDownloadView::OnBnClickedStartStopButton)
	ON_BN_CLICKED(IDC_EXIT_BUTTON, &CDownloadView::OnBnClickedExitButton)
    ON_MESSAGE(WM_MZ3_ACCESS_INFORMATION, OnAccessInformation)
//	ON_MESSAGE(WM_MZ3_GET_END, OnGetEnd)
	ON_MESSAGE(WM_MZ3_GET_END_BINARY, OnGetEndBinary)
	ON_MESSAGE(WM_MZ3_GET_ERROR, OnGetError)
	ON_MESSAGE(WM_MZ3_GET_ABORT, OnGetAbort)
	ON_MESSAGE(WM_MZ3_ABORT, OnAbort)
	ON_MESSAGE(WM_MZ3_ACCESS_LOADED, OnAccessLoaded)
END_MESSAGE_MAP()


// CDownloadView �f�f

#ifdef _DEBUG
void CDownloadView::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CDownloadView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


// CDownloadView ���b�Z�[�W �n���h��

void CDownloadView::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();

	// �v���O���X�o�[������
	mc_progressBar.SetRange( 0, 1000 );

	//--- �^�C�g���̕ύX
	{
		// �t�H���g�ύX
		m_titleEdit.SetFont( &theApp.m_font );

		// �^�C�g���ύX
		m_titleEdit.SetWindowText( L"�_�E�����[�h�}�l�[�W��" );
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
		dwStyle |= LVS_REPORT;

		// �X�^�C���̍X�V
		m_list.ModifyStyle(0, dwStyle);

		// �A�C�R�����X�g�̍쐬
		m_imageList.Create(16, 16, ILC_COLOR4, 2, 0);
		m_imageList.Add( AfxGetApp()->LoadIcon(IDI_DOWNLOADED_ICON) );
		m_list.SetImageList(&m_imageList, LVSIL_SMALL);

		// �J�����̒ǉ�
		// ����������������ɍĐݒ肷��̂ŉ��̕����w�肵�Ă����B
		m_list.InsertColumn(0, _T("�t�@�C����"), LVCFMT_LEFT, 20, -1);
		m_list.InsertColumn(1, _T("URL"), LVCFMT_LEFT, 20, -1);
		m_list.InsertColumn(2, _T("���l"), LVCFMT_LEFT, 20, -1);
	}

	//--- �`�F�b�N
	{
		mc_checkContinue.SetCheck( BST_CHECKED );
	}

	//--- �ʒm�̈�̕ύX
	{
		// �t�H���g�ύX
		m_infoEdit.SetFont( &theApp.m_font );
	}
}

/**
 * �T�C�Y�ύX���̏���
 */
void CDownloadView::OnSize(UINT nType, int cx, int cy)
{
	CFormView::OnSize(nType, cx, cy);

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

	int fontHeight = theApp.m_optionMng.GetFontHeight();
	if( fontHeight == 0 ) {
		fontHeight = 24;
	}

	int hTitle    = theApp.GetInfoRegionHeight(fontHeight);	// �^�C�g���̓t�H���g�T�C�Y�ˑ�
	int hButton   = theApp.GetInfoRegionHeight(fontHeight);	// �{�^���T�C�Y�̓t�H���g�T�C�Y�ˑ�
	int hInfo     = theApp.GetInfoRegionHeight(fontHeight);	// ���̈���t�H���g�T�C�Y�ˑ�
	int hProgress = hInfo * 2 / 3;							// �T�C�Y�� hInfo �� 2/3 �Ƃ���
	int hList     = cy-hTitle-hButton-hInfo-hProgress;		// (�S��-���̗̈�) �����X�g�̈�Ƃ���

	int y = 0;
	util::MoveDlgItemWindow( this, IDC_TITLE_EDIT,        0, y, cx, hTitle  );
	y += hTitle;
	util::MoveDlgItemWindow( this, IDC_ITEM_LIST,         0, y, cx, hList   );
	y += hList;

	int x = 0;
	int w = 50;
	util::MoveDlgItemWindow( this, IDC_START_STOP_BUTTON, x, y, w,  hButton );
	x += w;
	util::MoveDlgItemWindow( this, IDC_EXIT_BUTTON,       x, y, w,  hButton );
	x += w +10;
	w = cx - x;
	util::MoveDlgItemWindow( this, IDC_CONTINUE_CHECK,    x, y, w,  hButton );
	y += hButton;

	util::MoveDlgItemWindow( this, IDC_INFO_EDIT,         0, y, cx, hInfo   );
	y += hInfo;
	util::MoveDlgItemWindow( this, IDC_PROGRESS_BAR,      0, y, cx, hProgress );
	y += hProgress;

	// �I�𒆂̍s���\�������悤�ɂ���
	if( m_list.m_hWnd != NULL ) {
		int idx = util::MyGetListCtrlSelectedItemIndex(m_list);
		m_list.EnsureVisible( idx, FALSE );
	}

	// ���X�g�J�������̕ύX
	ResetColumnWidth();
}

/**
 * �J�����T�C�Y�i���j���Đݒ肷��B
 */
void CDownloadView::ResetColumnWidth(void)
{
	if( m_list.m_hWnd == NULL )
		return;

	// ���̒�`
	int w = GetListWidth();

	const int W_COL1 = 30;
	const int W_COL2 = 70;
	const int W_COL3 = 0;

	// �w���v�ȊO
	m_list.SetColumnWidth(0, w * W_COL1/(W_COL1+W_COL2+W_COL3) );
	m_list.SetColumnWidth(1, w * W_COL2/(W_COL1+W_COL2+W_COL3) );
	m_list.SetColumnWidth(2, w * W_COL3/(W_COL1+W_COL2+W_COL3) );
}

int CDownloadView::GetListWidth(void)
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

void CDownloadView::MyUpdateControls(void)
{
	// �J�n�E��~�{�^���̐���
	if (m_access) {
		GetDlgItem( IDC_START_STOP_BUTTON )->SetWindowText( L"��~" );
		GetDlgItem( IDC_START_STOP_BUTTON )->EnableWindow( TRUE );
	} else {
		GetDlgItem( IDC_START_STOP_BUTTON )->SetWindowText( L"�J�n" );

		// ��A�N�Z�X���Ȃ̂ŁA���_�E�����[�h�t�@�C��������ΊJ�n�\�B
		bool bHasNoFinishedItem = false;
		for (int i=0; i<m_items.size(); i++) {
			if (!m_items[i].bFinished) {
				bHasNoFinishedItem = true;
				break;
			}
		}
		GetDlgItem( IDC_START_STOP_BUTTON )->EnableWindow( bHasNoFinishedItem ? TRUE : FALSE );
	}

	// �߂�E��~�A�C�R��
	theApp.EnableCommandBarButton( ID_BACK_BUTTON, m_access ? FALSE : TRUE );
	theApp.EnableCommandBarButton( ID_STOP_BUTTON, m_access ? TRUE : FALSE );

	// �I���{�^���̓A�N�Z�X���͖���
	GetDlgItem( IDC_EXIT_BUTTON )->EnableWindow( m_access ? FALSE : TRUE );
}

/**
 * �\���C�x���g
 */
LRESULT CDownloadView::OnFit(WPARAM wParam, LPARAM lParam)
{
	// TODO �_�E�����[�h�ς݂̍��ڂ��폜

	// ���ڂ̍X�V
	m_list.DeleteAllItems();
	for (size_t i=0; i<m_items.size(); i++) {
		DownloadItem& item = m_items[i];

		CString filename;
		int idxFind = item.localpath.ReverseFind( '\\' );
		if (idxFind>=0) {
			filename = item.localpath.Mid(idxFind+1);
		}

		int idx = m_list.InsertItem( i, filename, item.bFinished ? 0 : -1 );
		m_list.SetItem( idx, 1, LVIF_TEXT, item.url, 0, 0, 0, 0 );
	}
	util::MySetListCtrlItemFocusedAndSelected( m_list, 0, true );

	// �R���g���[����Ԃ̍X�V
	MyUpdateControls();

	if (!m_access) {
		// ��������
		util::MySetInformationText( m_hWnd, L"��������" );
	}

	return TRUE;
}

void CDownloadView::OnBnClickedContinueCheck()
{
	// �_�E�����[�h�������Ƀ`�F�b�N��Ԃ��Q�Ƃ��邽�ߓ��ɏ����͕s�v�B
}

/**
 * �J�n�E��~�{�^��
 */
void CDownloadView::OnBnClickedStartStopButton()
{
	if (m_access) {
		// ��~����
		::SendMessage(m_hWnd, WM_MZ3_ABORT, NULL, NULL);
	} else {
		// �J�n����

		// �^�[�Q�b�g�I��
		int idx = util::MyGetListCtrlSelectedItemIndex( m_list );
		if (idx<0) {
			return;
		}
		m_targetItemIndex = idx;

		// �_�E�����[�h�J�n
		DoDownloadSelectedItem();
	}
}

/**
 * �I���{�^��
 */
void CDownloadView::OnBnClickedExitButton()
{
	// �ʐM���͖���
	if (m_access) {
		return;
	}

	// �߂�
	CMainFrame* pMainFrame = (CMainFrame*)theApp.m_pMainWnd;
	pMainFrame->OnBackButton();
}

/**
 * �A�N�Z�X���ʒm
 */
LRESULT CDownloadView::OnAccessInformation(WPARAM wParam, LPARAM lParam)
{
	m_infoEdit.SetWindowText(*(CString*)lParam);
	return TRUE;
}

/**
 * �A�N�Z�X�I���ʒm��M(Binary)
 */
LRESULT CDownloadView::OnGetEndBinary(WPARAM wParam, LPARAM lParam)
{
	if (m_abortRequested) {
		// WM_MZ3_GET_ABORT ���b�Z�[�W�����Ȃ��ꍇ������̂ł����ŏ�ԕ��A
		util::MySetInformationText( m_hWnd, _T("���f���܂���") );

		m_access = FALSE;
		m_abortRequested = FALSE;
		MyUpdateControls();
		return FALSE;
	}

	// �ۑ��t�@�C���ɃR�s�[
	CopyFile( theApp.m_filepath.temphtml, m_items[m_targetItemIndex].localpath, FALSE/*bFailIfExists, �㏑��*/ );

	// �t���O�ύX
	m_items[ m_targetItemIndex ].bFinished = true;
	// �t���O�ύX�ɑ΂���C���[�W�ύX
	util::MySetListCtrlItemImageIndex( m_list, m_targetItemIndex, 0, 0 );
	// �X�V
	m_list.Update( m_targetItemIndex );

	// �u�A���v�`�F�b�N�ς݂ŁA���̃A�C�e��������Όp���B
	if (IsDlgButtonChecked(IDC_CONTINUE_CHECK) == BST_CHECKED) {
		// ���̖��_�E�����[�h�A�C�e����T��
		for (int i=m_targetItemIndex+1; i<m_items.size(); i++) {
			if (!m_items[i].bFinished) {
				// �����B�I����Ԃ�ύX���A�ă_�E�����[�h�J�n�B
				util::MySetListCtrlItemFocusedAndSelected( m_list, m_targetItemIndex, false );	// ��I��
				m_targetItemIndex = i;
				util::MySetListCtrlItemFocusedAndSelected( m_list, m_targetItemIndex, true );	// �I��

				// �_�E�����[�h�J�n
				if (DoDownloadSelectedItem()) {
					return TRUE;
				}
			}
		}
	}

	// �_�E�����[�h����
	m_access = FALSE;
	MyUpdateControls();

	util::MySetInformationText( m_hWnd, L"�_�E�����[�h���������܂���" );

	return TRUE;
}

/**
 * �A�N�Z�X�G���[�ʒm��M
 */
LRESULT CDownloadView::OnGetError(WPARAM wParam, LPARAM lParam)
{
	if (m_abortRequested) {
		// WM_MZ3_GET_ABORT ���b�Z�[�W�����Ȃ��ꍇ������̂ł����ŏ�ԕ��A
		util::MySetInformationText( m_hWnd, _T("���f���܂���") );

		m_access = FALSE;
		m_abortRequested = FALSE;
		MyUpdateControls();
		return TRUE;
	}

	LPCTSTR smsg = L"�G���[���������܂���";
	util::MySetInformationText( m_hWnd, smsg );

	CString msg;
	msg.Format( 
		L"%s\n\n"
		L"�����F%s", smsg, theApp.m_inet.GetErrorMessage() );
	MZ3LOGGER_ERROR( msg );

	m_access = FALSE;
	MyUpdateControls();

	return TRUE;
}

/**
 * �A�N�Z�X���f�ʒm��M
 * 
 * ���[�U�w���ɂ�钆�f
 */
LRESULT CDownloadView::OnGetAbort(WPARAM wParam, LPARAM lParam)
{
	// ���[�U����̃A�N�Z�X���f�w����������ƁA�����ɒʒm���A���Ă���
	// �{�^�������ɖ߂��ă��b�Z�[�W�������ď����I��
	util::MySetInformationText( m_hWnd, _T("���f���܂���") );

	m_access = FALSE;
	m_abortRequested = FALSE;
	MyUpdateControls();

	return TRUE;
}

/**
 * ���f�{�^���������̏���
 */
LRESULT CDownloadView::OnAbort(WPARAM wParam, LPARAM lParam)
{
	// �ʐM���łȂ��Ȃ炷���ɏI������
	if( !theApp.m_inet.IsConnecting() ) {
		return TRUE;
	}
	theApp.m_inet.Abort();

	LPCTSTR msg = _T("���f���܂���");
	util::MySetInformationText( m_hWnd, msg );
	MZ3LOGGER_DEBUG( msg );

	m_access = FALSE;
	m_abortRequested = TRUE;
	MyUpdateControls();

	return TRUE;
}

/**
 * ��M�T�C�Y�ʒm
 */
LRESULT CDownloadView::OnAccessLoaded(WPARAM dwLoaded, LPARAM dwLength)
{
	if( dwLength == 0 ) {
		// ���T�C�Y���s���Ȃ̂ŁA���� N KB �Ƃ݂Ȃ�
		dwLength = 40 * 1024;
	}

	// ��M�T�C�Y�Ƒ��T�C�Y������Ȃ�A��M�����Ƃ݂Ȃ�
	if( dwLoaded == dwLength ) {
	}

	// [0,1000] �Ŏ�M�T�C�Y�ʒm
	int pos = (int)(dwLoaded * 1000.0 / dwLength);
	mc_progressBar.SetPos( pos );

	return TRUE;
}

/**
 * �_�E�����[�h�J�n����
 */
bool CDownloadView::DoDownloadSelectedItem(void)
{
	if (m_targetItemIndex<0 || m_targetItemIndex >= m_items.size()) {
		MZ3LOGGER_FATAL( L"�C���f�b�N�X�s��" );
		return false;
	}

	if( m_list.m_hWnd != NULL ) {
		m_list.EnsureVisible( m_targetItemIndex, FALSE );
	}

	CString url = m_items[m_targetItemIndex].url;
	MZ3LOGGER_DEBUG( L"�_�E�����[�h�J�n url[" + url + L"]" );

	m_access = TRUE;
	MyUpdateControls();
	mc_progressBar.SetPos( 0 );

	// �擾�J�n
	theApp.m_inet.Initialize( m_hWnd, NULL );
	theApp.m_accessType = ACCESS_DOWNLOAD;
	theApp.m_inet.DoGet( url, _T(""), CInetAccess::FILE_BINARY );

	return true;
}

