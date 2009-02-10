/*
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */
// MainFrm.cpp : CMainFrame �N���X�̎���
//

#include "stdafx.h"
#include "MZ3.h"
#include "MainFrm.h"
#include "OptionSheet.h"
#include "ReportView.h"
#include "WriteView.h"
#include "DownloadView.h"
#include "MZ3View.h"
#include "Ran2View.h"
#include "UserDlg.h"

#include "util.h"
#include "util_gui.h"
#include "url_encoder.h"
#include "version.h"

// MZ4 �̏���N�����̃E�B���h�E�T�C�Y
#define MZ4_WINDOW_DEFAULT_SIZE_X	400
#define MZ4_WINDOW_DEFAULT_SIZE_Y	600

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	ON_WM_CREATE()
    ON_COMMAND(ID_BACK_BUTTON, OnBackButton)
    ON_COMMAND(ID_FORWARD_BUTTON, OnForwardButton)
    ON_COMMAND(ID_AUTO_RELOAD_BUTTON, OnAutoReloadButton)
    ON_COMMAND(ID_STOP_BUTTON, OnStopButton)
	ON_COMMAND(ID_SETTING_LOGIN, &CMainFrame::OnSettingLogin)
	ON_COMMAND(ID_SETTING_GENERAL, &CMainFrame::OnSettingGeneral)
	ON_COMMAND(ID_MENU_CLOSE, &CMainFrame::OnMenuClose)
	ON_COMMAND(IDM_GETPAGE_ALL, &CMainFrame::OnGetpageAll)
	ON_COMMAND(IDM_GETPAGE_LATEST10, &CMainFrame::OnGetpageLatest10)
	ON_COMMAND(ID_CHANGE_FONT_BIG, &CMainFrame::OnChangeFontBig)
	ON_COMMAND(ID_CHANGE_FONT_MEDIUM, &CMainFrame::OnChangeFontMedium)
	ON_COMMAND(ID_CHANGE_FONT_SMALL, &CMainFrame::OnChangeFontSmall)
	ON_COMMAND(ID_MENU_BACK, &CMainFrame::OnMenuBack)
	ON_COMMAND(ID_MENU_NEXT, &CMainFrame::OnMenuNext)
	ON_COMMAND(IDM_CHECK_NEW, &CMainFrame::OnCheckNew)
	ON_COMMAND(ID_HELP_MENU, &CMainFrame::OnHelpMenu)
	ON_COMMAND(ID_HISTORY_MENU, &CMainFrame::OnHistoryMenu)
	ON_COMMAND(IDM_START_CRUISE, &CMainFrame::OnStartCruise)
	ON_COMMAND(IDM_START_CRUISE_UNREAD_ONLY, &CMainFrame::OnStartCruiseUnreadOnly)
//	ON_COMMAND(IDM_OPEN_MIXI_MOBILE_BY_BROWSER, &CMainFrame::OnOpenMixiMobileByBrowser)
	ON_COMMAND(ID_ERRORLOG_MENU, &CMainFrame::OnErrorlogMenu)
	ON_COMMAND(ID_CHANGE_SKIN, &CMainFrame::OnChangeSkin)
	ON_COMMAND(ID_MENU_ACTION, &CMainFrame::OnMenuAction)
	ON_COMMAND(ID_ENABLE_INTERVAL_CHECK, &CMainFrame::OnEnableIntervalCheck)
	ON_COMMAND(ID_MENU_CHECK_UPDATE, &CMainFrame::OnCheckUpdate)
	ON_COMMAND_RANGE(ID_SKIN_BASE, ID_SKIN_BASE+99, &CMainFrame::OnSkinMenuItem)
    ON_UPDATE_COMMAND_UI(ID_BACK_BUTTON, OnUpdateBackButton)
    ON_UPDATE_COMMAND_UI(ID_FORWARD_BUTTON, OnUpdateForwardButton)
    ON_UPDATE_COMMAND_UI(ID_STOP_BUTTON, OnUpdateStopButton)
    ON_UPDATE_COMMAND_UI(ID_IMAGE_BUTTON, OnUpdateImageButton)
    ON_UPDATE_COMMAND_UI(ID_WRITE_BUTTON, OnUpdateWriteButton)
    ON_UPDATE_COMMAND_UI(ID_OPEN_BROWSER, OnUpdateBrowserButton)
	ON_UPDATE_COMMAND_UI(ID_AUTO_RELOAD_BUTTON, OnUpdateAutoReloadButton)
	ON_UPDATE_COMMAND_UI(IDM_GETPAGE_ALL, &CMainFrame::OnUpdateGetpageAll)
	ON_UPDATE_COMMAND_UI(IDM_GETPAGE_LATEST10, &CMainFrame::OnUpdateGetpageLatest10)
	ON_UPDATE_COMMAND_UI(ID_MENU_BACK, &CMainFrame::OnUpdateMenuBack)
	ON_UPDATE_COMMAND_UI(ID_MENU_NEXT, &CMainFrame::OnUpdateMenuNext)
	ON_UPDATE_COMMAND_UI(ID_ENABLE_INTERVAL_CHECK, &CMainFrame::OnUpdateEnableIntervalCheck)
	ON_UPDATE_COMMAND_UI_RANGE(ID_SKIN_BASE, ID_SKIN_BASE+99, &CMainFrame::OnUpdateSkinMenuItem)
	ON_UPDATE_COMMAND_UI(ID_MENU_CHECK_UPDATE, &CMainFrame::OnUpdateCheckUpdate)
	ON_WM_ACTIVATE()
	ON_WM_DESTROY()
	ON_WM_MOVE()
	ON_COMMAND(ID_DOWNLOAD_MANAGER_VIEW, &CMainFrame::OnDownloadManagerView)
	ON_COMMAND(ID_MENU_OPEN_URL, &CMainFrame::OnMenuOpenUrl)
	ON_COMMAND(ID_MENU_OPEN_LOCAL_FILE, &CMainFrame::OnMenuOpenLocalFile)
	ON_COMMAND(ID_MENU_FONT_MAGNIFY, &CMainFrame::OnMenuFontMagnify)
	ON_COMMAND(ID_MENU_FONT_SHRINK, &CMainFrame::OnMenuFontShrink)
	ON_COMMAND(ID_MENU_RELOAD_LUA_SCRIPTS, &CMainFrame::OnMenuReloadLuaScripts)
	ON_COMMAND(ID_MENU_RELOAD_CATEGORY_LIST_LOG, &CMainFrame::OnMenuReloadCategoryListLog)
END_MESSAGE_MAP()


// CMainFrame �R���X�g���N�V����/�f�X�g���N�V����

// -----------------------------------------------------------------------------
// �R���X�g���N�^
// -----------------------------------------------------------------------------
CMainFrame::CMainFrame()
	: m_bBackPageEnabled(FALSE)
	, m_bForwardPageEnabled(FALSE)
#ifdef WINCE
	, m_hMenu(NULL)
#endif
{

}

// -----------------------------------------------------------------------------
// �f�X�g���N�^
// -----------------------------------------------------------------------------
CMainFrame::~CMainFrame()
{
}

/**
 * CREATE �C�x���g
 */



int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

#ifdef WINCE
/*
	if( theApp.m_bPocketPC ) {
		// dpi�l�ɂ���ăc�[���o�[�̉摜��ύX
		int id_toolbar = 0;

		// RealVGA�����Ă�����(dpi�l��96�ȉ���������)�R�}���h�o�[�ɏ������摜���g�p�B������傫��Dpi�͑傫���́B
		if (theApp.GetDPI() <= 96) {
			id_toolbar = IDR_TOOLBAR_QVGA;
		} else {
			id_toolbar = IDR_TOOLBAR;
		}

		if (!m_wndCommandBar.Create(this) ||
			!m_wndCommandBar.InsertMenuBar(IDR_MAINFRAME) ||
			!m_wndCommandBar.AddAdornments(CMDBAR_OK) ||
			!m_wndCommandBar.LoadToolBar(id_toolbar)) {
				MZ3LOGGER_FATAL(L"CommandBar �̍쐬�Ɏ��s���܂���\n");
				return -1;      // �쐬�ł��܂���ł����B
		}

		m_wndCommandBar.SetBarStyle(m_wndCommandBar.GetBarStyle() | CBRS_SIZE_FIXED);
	}
*/

	// ���j���[�o�[���쐬����
	{
		m_hMenu = LoadMenu(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDR_MAINFRAME));

		SHMENUBARINFO mbi;

		memset(&mbi, 0, sizeof(SHMENUBARINFO));
		mbi.cbSize = sizeof(SHMENUBARINFO);
		mbi.hwndParent = m_hWnd;
		mbi.dwFlags = SHCMBF_HMENU;
		mbi.nToolBarId = (UINT)m_hMenu;
		mbi.hInstRes = AfxGetInstanceHandle();

		if (SHCreateMenuBar(&mbi)==FALSE) {
			MZ3LOGGER_FATAL(L"Couldn't create menu bar");
		}
		m_hwndMenuBar = mbi.hwndMB;
	}
#endif

#ifndef WINCE
	// �c�[���o�[�̐���
	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT | TBSTYLE_TOOLTIPS, 
		                       WS_CHILD | WS_VISIBLE | 
							   CBRS_TOP | 
							   CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC))
	{
		MZ3LOGGER_FATAL(L"�c�[���o�[ �̍쐬�Ɏ��s���܂���");
		return -1;      // fail to create
	}

    //�c�[���o�[�����[�h
//	int idToolbar = IDR_TOOLBAR;
//	int nToolbarWidth  = 32;
//	int nToolbarHeight = 32;
	int idToolbar = IDR_TOOLBAR_WIN32;
	int nToolbarWidth  = 16;
	int nToolbarHeight = 16;

	m_wndToolBar.LoadToolBar(idToolbar);

	// 256 �F�r�b�g�}�b�v�����[�h
	HBITMAP hbm = (HBITMAP)::LoadImage(
		AfxGetInstanceHandle(),
		MAKEINTRESOURCE(idToolbar),
		IMAGE_BITMAP,
		0, 0,
		LR_CREATEDIBSECTION | LR_LOADMAP3DCOLORS);

	CBitmap bm;
	bm.Attach(hbm);

	// 256 �F�r�b�g�}�b�v�����蓖��
	int nBtnCnt = 8;	//�{�^���̐�
	static CImageList m_ImageList;
	m_ImageList.Create( nToolbarWidth, nToolbarHeight, ILC_COLOR32 | ILC_MASK, nBtnCnt, 1 );
	m_ImageList.Add(&bm, (CBitmap*)NULL);
	m_wndToolBar.GetToolBarCtrl().SetImageList(&m_ImageList);


	//  �X�^�C����ݒ肷��
	m_wndToolBar.SetBarStyle(m_wndToolBar.GetBarStyle() | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC);
	m_wndToolBar.EnableDocking( CBRS_ALIGN_ANY );		//  �h�b�L���O���[�h��ݒ�
#endif

#ifdef WINCE
	CRect rc;
	GetWindowRect( &rc );

	WINDOWPOS wp;
	wp.hwnd = m_hWnd;
	wp.cx = rc.Width();
	wp.cy = rc.Height();
	wp.flags = SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOOWNERZORDER | SWP_NOZORDER;
	SendMessage( WM_WINDOWPOSCHANGED, 0, (LPARAM)&wp );
#endif

	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if (!CFrameWnd::PreCreateWindow(cs))
		return FALSE;

	// �h�L�������g�̖��̂�\�����Ȃ�
	cs.style &= ~FWS_ADDTOTITLE;

#ifndef WINCE
	cs.cx = MZ4_WINDOW_DEFAULT_SIZE_X;
	cs.cy = MZ4_WINDOW_DEFAULT_SIZE_Y;

	// �O��̃T�C�Y�𕜋A����
	if (theApp.m_optionMng.m_strWindowPos.GetLength() > 16) {
		LPCTSTR strrc = theApp.m_optionMng.m_strWindowPos;
		CRect rc(_wtoi(strrc), _wtoi(strrc+5), _wtoi(strrc+10), _wtoi(strrc+15));
		// �^�X�N�o�[�̈ʒu�ɂ�钲�����s��
		CRect desktoprct;
		SystemParametersInfo(SPI_GETWORKAREA,0,&desktoprct,0);
		rc.OffsetRect( desktoprct.left , desktoprct.top );

		cs.cx = rc.Width();
		cs.cy = rc.Height();

		cs.x  = rc.left;
		cs.y  = rc.top;
	}

	// �^�C�g���ύX
	MySetTitle();
#else
#endif

	return TRUE;
}

// CMainFrame �f�f

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}
#endif //_DEBUG

// CMainFrame ���b�Z�[�W �n���h��


/**
 * �߂�{�^��
 */
void CMainFrame::OnBackButton()
{
#ifndef WINCE
	if (!m_bBackPageEnabled) {
		// �{�^�����񊈐��Ȃ珈�����Ȃ��i���ڌĂ΂��ꍇ������̂Łj
		return;
	}
#endif

	CView* pActiveView = GetActiveView();

	if (pActiveView == theApp.m_pReportView) {
		// ���|�[�g�r���[ �� ���C���r���[

		// �I������
		((CReportView*)pActiveView)->EndProc();

		m_bForwardPageEnabled = TRUE;
		m_bBackPageEnabled = FALSE;

		// �߂�
		::SendMessage( theApp.m_pMainView->m_hWnd, WM_MZ3_CHANGE_VIEW, NULL, NULL );
		return;
	}

	if (pActiveView == theApp.m_pWriteView) {
		// Write �r���[�i���L, �V�K���b�Z�[�W�j �� ���C���r���[
		// Write �r���[�i���L�ȊO�j             �� ���|�[�g�r���[

		if( theApp.m_pWriteView->IsWriteFromMainView() ) {
			// Write �r���[�i���L, �V�K���b�Z�[�W�j �� ���C���r���[

			// ���������̃f�[�^�����邩�ǂ����̔���
			if (!theApp.m_pWriteView->IsWriteCompleted()) {
				// �����M�f�[�^�L��
				int ret = ::MessageBox(m_hWnd, _T("�����e�̃f�[�^������܂�\n�j������܂�����낵���ł����H"),
					MZ3_APP_NAME, MB_ICONQUESTION | MB_OKCANCEL);
				if (ret == IDCANCEL) {
					// �����𒆎~
					return;
				}
				theApp.m_pWriteView->SetWriteCompleted(true);
			}

			m_bForwardPageEnabled = FALSE;
			m_bBackPageEnabled = FALSE;

			theApp.ChangeView(theApp.m_pMainView);
		} else {
			// Write �r���[�i���L�ȊO�j �� ���|�[�g�r���[
			m_bForwardPageEnabled = TRUE;
			m_bBackPageEnabled = TRUE;

			// ReportView�ɖ߂�
			theApp.ChangeView(theApp.m_pReportView);
		}
		return;
	}

	if (pActiveView == theApp.m_pDownloadView) {
		// �_�E�����[�h�r���[ �� ���C���r���[
		m_bForwardPageEnabled = FALSE;
		m_bBackPageEnabled = FALSE;

		// �߂�
		theApp.ChangeView(theApp.m_pMainView);

		// ���C����ʂ̃����[�h(�A�C�R�������[�h�̉\�������邽��)
		theApp.m_pMainView->OnSelchangedGroupTab();

		return;
	}


	//--- ���C���r���[
	// �߂��͂Ȃ����A�Ƃ肠�����ύX�C�x���g�𑗂��Ă���
	::SendMessage(theApp.m_pMainView->m_hWnd, WM_MZ3_CHANGE_VIEW, NULL, NULL);

}

/**
 * �i�ރ{�^��
 */
void CMainFrame::OnForwardButton()
{
#ifndef WINCE
	if (!m_bForwardPageEnabled) {
		// �{�^�����񊈐��Ȃ珈�����Ȃ��i���ڌĂ΂��ꍇ������̂Łj
		return;
	}
#endif

	CView* pActiveView = GetActiveView();

	if (pActiveView == theApp.m_pReportView) {
		// ���|�[�g�r���[ �� �������݃r���[
		// �A���A�����M�̏ꍇ�̂݁B
		if (!theApp.m_pWriteView->IsWriteCompleted()) {
			m_bForwardPageEnabled = FALSE;
			m_bBackPageEnabled = TRUE;

			theApp.ChangeView( theApp.m_pWriteView );
		}
		return;
	}

	if (pActiveView == theApp.m_pWriteView) {
		// �������݃r���[ �� �i�J�ڐ�Ȃ��j
		return;
	}

	if (pActiveView == theApp.m_pMainView ) {
		// ���C���r���[ �� ���|�[�g�r���[

		// �������݃r���[�ɍs����Ȃ�ANEXT �{�^����L���ɁB
		// ���M�����t���O(IsWriteCompleted())��OFF�Ȃ�A�u�������݃r���[�ɍs����v�Ɣ��f����
		m_bForwardPageEnabled = theApp.m_pWriteView->IsWriteCompleted() ? FALSE : TRUE;
		m_bBackPageEnabled = TRUE;

		theApp.ChangeView(theApp.m_pReportView);
	}
}

/**
 * ����擾�{�^��
 */
void CMainFrame::OnAutoReloadButton()
{
	// ���j���[�̃n���h���ɈϏ�
	OnEnableIntervalCheck();
}

// -----------------------------------------------------------------------------
// ���O�C���ݒ�{�^��
// -----------------------------------------------------------------------------
void CMainFrame::OnSettingLogin()
{
	CUserDlg dlg;
	dlg.DoModal();
}

// -----------------------------------------------------------------------------
// �X�g�b�v�{�^���̐���
// -----------------------------------------------------------------------------
void CMainFrame::OnUpdateStopButton(CCmdUI* pCmdUI)
{
#ifdef WINCE
//	if( theApp.m_bPocketPC ) {
//		pCmdUI->Enable(m_wndCommandBar.GetToolBarCtrl().IsButtonEnabled(ID_STOP_BUTTON));
//	}
#else
	pCmdUI->Enable(m_wndToolBar.GetToolBarCtrl().IsButtonEnabled(ID_STOP_BUTTON));
#endif
}

// -----------------------------------------------------------------------------
// �߂�{�^���̐���
// -----------------------------------------------------------------------------
void CMainFrame::OnUpdateBackButton(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(m_bBackPageEnabled); 
}

// -----------------------------------------------------------------------------
// �i�ރ{�^���̐���
// -----------------------------------------------------------------------------
void CMainFrame::OnUpdateForwardButton(CCmdUI* pCmdUI)
{
#ifndef WINCE
	pCmdUI->Enable(m_bForwardPageEnabled);
#endif
}

// -----------------------------------------------------------------------------
// �摜�{�^���̐���
// -----------------------------------------------------------------------------
void CMainFrame::OnUpdateImageButton(CCmdUI* pCmdUI)
{
#ifdef WINCE
//	if( theApp.m_bPocketPC ) {
//		pCmdUI->Enable(m_wndCommandBar.GetToolBarCtrl().IsButtonEnabled(ID_IMAGE_BUTTON));  
//	}
#else
	pCmdUI->Enable(m_wndToolBar.GetToolBarCtrl().IsButtonEnabled(ID_IMAGE_BUTTON));
#endif
}

// -----------------------------------------------------------------------------
// �������݃{�^���̐���
// -----------------------------------------------------------------------------
void CMainFrame::OnUpdateWriteButton(CCmdUI* pCmdUI)
{
#ifdef WINCE
//	if( theApp.m_bPocketPC ) {
//		pCmdUI->Enable( m_wndCommandBar.GetToolBarCtrl().IsButtonEnabled(ID_WRITE_BUTTON) );
//	}
#else
	pCmdUI->Enable(m_wndToolBar.GetToolBarCtrl().IsButtonEnabled(ID_WRITE_BUTTON));
#endif
}

// -----------------------------------------------------------------------------
// �u���E�U�{�^���̐���
// -----------------------------------------------------------------------------
void CMainFrame::OnUpdateBrowserButton(CCmdUI* pCmdUI)
{
#ifdef WINCE
//	if( theApp.m_bPocketPC ) {
//		pCmdUI->Enable(m_wndCommandBar.GetToolBarCtrl().IsButtonEnabled(ID_OPEN_BROWSER));  
//	}
#else
	pCmdUI->Enable(m_wndToolBar.GetToolBarCtrl().IsButtonEnabled(ID_OPEN_BROWSER));
#endif
}

// -----------------------------------------------------------------------------
// ����擾�{�^���̐���
// -----------------------------------------------------------------------------
void CMainFrame::OnUpdateAutoReloadButton(CCmdUI* pCmdUI)
{
#ifdef WINCE
	// WinCE �͒���擾�{�^���Ȃ�
#else
	pCmdUI->Enable();
	pCmdUI->SetCheck( theApp.m_optionMng.m_bEnableIntervalCheck ? TRUE : FALSE );
#endif
}

// -----------------------------------------------------------------------------
// �ݒ�
// -----------------------------------------------------------------------------
void CMainFrame::OnSettingGeneral()
{
	if( theApp.m_bSmartphone ) {
		// Smartphone/Standard Edition �ł� COptionSheet �������p�ł��Ȃ����߁A�x�����b�Z�[�W��\������B
		MessageBox( 
			L"Smartphone/Standard Edition �ł̓I�v�V������ʂ�\���ł��܂���B\n"
			L"���萔�ł����Amz3.ini �𒼐ڕҏW���Ă�������" );
	} else {
#ifndef SMARTPHONE2003_UI_MODEL
		static int s_iLastActivePage = 1;

		COptionSheet cPropSht( _T("�I�v�V����"));
		
		// �y�[�W����
		cPropSht.SetPage();

		// �A�N�e�B�u�y�[�W�̐ݒ�
		if (s_iLastActivePage < 0 || s_iLastActivePage >= cPropSht.GetPageCount()) {
			s_iLastActivePage = 0;	// ������
		}
		cPropSht.SetActivePage(s_iLastActivePage);
		if (cPropSht.DoModal() == IDOK) {

			// ini�t�@�C���̕ۑ�
			theApp.m_optionMng.Save();

			// �t�H���g�̃��Z�b�g
			ChangeAllViewFont();

			// ���C����ʂ̃����[�h
			theApp.m_pMainView->OnSelchangedGroupTab();
		}

		// �A�N�e�B�u�y�[�W�̃C���f�b�N�X��ۑ����Ă���
		s_iLastActivePage = cPropSht.m_iLastPage;
#endif
	}
}

/// �u����v���j���[�C�x���g�n���h��
void CMainFrame::OnMenuClose()
{
	int iRet;
	iRet = ::MessageBox(m_hWnd, MZ3_APP_NAME _T("���I�����܂����H"), MZ3_APP_NAME, MB_ICONQUESTION | MB_OKCANCEL);
	if (iRet == IDOK) {
		theApp.m_pReportView->SaveIndex();
		ShowWindow(SW_HIDE);
		AfxGetMainWnd()->PostMessage(WM_CLOSE);
	}
}

/// �u�S���擾�v���j���[�����C�x���g
void CMainFrame::OnGetpageAll()
{
	theApp.m_optionMng.SetPageType( GETPAGE_ALL );
}

/// �u�S���擾�v���j���[�̃`�F�b�N��Ԑ���
void CMainFrame::OnUpdateGetpageAll(CCmdUI *pCmdUI)
{
	if( theApp.m_optionMng.GetPageType() == GETPAGE_ALL ) {
		pCmdUI->SetCheck( TRUE );
	}else{
		pCmdUI->SetCheck( FALSE );
	}
}

/// �u�ŐV�P�O���擾�v���j���[�����C�x���g
void CMainFrame::OnGetpageLatest10()
{
	theApp.m_optionMng.SetPageType( GETPAGE_LATEST20 );
}

/// �u�ŐV�P�O���擾�v���j���[�̃`�F�b�N��Ԑ���
void CMainFrame::OnUpdateGetpageLatest10(CCmdUI *pCmdUI)
{
	if( theApp.m_optionMng.GetPageType() == GETPAGE_LATEST20 ) {
		pCmdUI->SetCheck( TRUE );
	}else{
		pCmdUI->SetCheck( FALSE );
	}
}

/// �u�����T�C�Y�b��v���j���[
void CMainFrame::OnChangeFontBig()
{
	theApp.m_optionMng.m_fontHeight = theApp.m_optionMng.m_fontHeightBig;
	ChangeAllViewFont();
}

/// �u�����T�C�Y�b���v���j���[
void CMainFrame::OnChangeFontMedium()
{
	theApp.m_optionMng.m_fontHeight = theApp.m_optionMng.m_fontHeightMedium;
	ChangeAllViewFont();
}

/// �u�����T�C�Y�b���v���j���[
void CMainFrame::OnChangeFontSmall()
{
	theApp.m_optionMng.m_fontHeight = theApp.m_optionMng.m_fontHeightSmall;
	ChangeAllViewFont();
}

inline void MySetListViewFont( CListCtrl* pListCtrl )
{
	// ��������A�I�[�i�[�Œ�`�����������
	DWORD dwStyle = pListCtrl->GetStyle();
	pListCtrl->ModifyStyle(LVS_OWNERDRAWFIXED, 0);

	// �t�H���g�ύX
	pListCtrl->SetFont( &theApp.m_font );

	// �I�[�i�[�Œ�`��ĊJ
	pListCtrl->ModifyStyle( 0, LVS_OWNERDRAWFIXED );
}

/**
 * �S�Ẵr���[�̃t�H���g��ύX����
 */
bool CMainFrame::ChangeAllViewFont(int fontHeight)
{
	if (fontHeight < 0) {
		// �f�t�H���g�l�␳
		fontHeight = theApp.m_optionMng.m_fontHeight;
	}

	// �t�H���g�̍쐬
	theApp.MakeNewFont( theApp.m_pMainView->GetFont(), fontHeight, theApp.m_optionMng.GetFontFace() );

	//--- ���C���r���[
	{
		CMZ3View* pView = (CMZ3View*)theApp.m_pMainView;

		// �O���[�v�^�u
		pView->m_groupTab.SetFont( &theApp.m_font );

		// �J�e�S�����X�g
		MySetListViewFont( &pView->m_categoryList );

		// �{�f�B���X�g
		MySetListViewFont( &pView->m_bodyList );

		// �C���t�H���[�V�����G�f�B�b�g
		pView->m_infoEdit.SetFont( &theApp.m_font );

		// �r���[���e�̍Đݒ�
		pView->ResetViewContent();
	}

	//--- ���|�[�g�r���[
	{
		CReportView* pView = theApp.m_pReportView;

		// �^�C�g��
		pView->m_titleEdit.SetFont( &theApp.m_font );

		// ���X�g
		MySetListViewFont( &pView->m_list );

		// RAN2�G�f�B�b�g
		pView->m_detailView->ChangeViewFont( fontHeight, theApp.m_optionMng.GetFontFace() );
		pView->m_detailView->DrawDetail(0);

		// �ʒm�̈�
		pView->m_infoEdit.SetFont( &theApp.m_font );
	}

	//--- �������݃r���[
	{
		CWriteView* pView = theApp.m_pWriteView;

		// �^�C�g��
		pView->m_titleEdit.SetFont( &theApp.m_font );

		// �G�f�B�b�g
		pView->m_bodyEdit.SetFont( &theApp.m_font );

		// �{�^��
		pView->m_sendButton.SetFont( &theApp.m_font );
		pView->m_cancelButton.SetFont( &theApp.m_font );

		// �ʒm�̈�
		pView->m_infoEdit.SetFont( &theApp.m_font );

		// ���J�͈̓R���{�{�b�N�X
		pView->m_viewlimitCombo.SetFont( &theApp.m_font );
	}

	// �T�C�Y�ύX
	{
#ifdef WINCE
		CRect rect;
		SystemParametersInfo(SPI_GETWORKAREA, 0, &rect, 0);
		int w = rect.Width();
		int h = rect.Height() - rect.top;

		// Smartphone/Standard Edition �̏ꍇ�̓c�[���o�[�̕�����������
		if( theApp.m_bSmartphone ) {
			h += MZ3_TOOLBAR_HEIGHT;
		}

		SetWindowPos( NULL, 0, 0, w, h, SWP_NOMOVE | SWP_NOZORDER );
#else
		// Win32 �̏ꍇ�͌��݂̑傫�����p������
		// (0,0) �𑗂邱�ƂŁA�O��̃T�C�Y�l���ė��p����B
		int w = 0;
		int h = 0;
#endif

		// �eView�ɒʒm�𑗂�
		theApp.m_pMainView->OnSize( 0, w, h );
		theApp.m_pReportView->OnSize( 0, w, h );
		theApp.m_pWriteView->OnSize( 0, w, h );
	}
	return false;
}

/// ��ʁb�O�̉�ʃ��j���[�̐���
void CMainFrame::OnUpdateMenuBack(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(m_bBackPageEnabled);
}

/// ��ʁb���̉�ʃ��j���[�̐���
void CMainFrame::OnUpdateMenuNext(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(m_bForwardPageEnabled);
}

/// ����擾���j���[
void CMainFrame::OnEnableIntervalCheck()
{
	if (theApp.m_optionMng.m_bEnableIntervalCheck == false) {
		// �L���ɂȂ����̂ŁA���b�Z�[�W��\������
		if ((GetAsyncKeyState(VK_CONTROL) & 0x8000) ||
			(GetAsyncKeyState(VK_SHIFT) & 0x8000))
		{
			// Ctrl �� Shift �������ɂ̓_�C�A���O���o���Ȃ�
		} else {
			CString msg;
			msg.Format( 
				L"����擾�@�\�́A�J�e�S���̍��ڂ����I�Ɏ擾����@�\�ł��B\n"
				L"\n"
				L"�E���C����ʂ��J���Ă���ꍇ�̂ݗL���ł��B\n"
				L"�E�擾�Ԋu�̓I�v�V������ʂŐݒ�ł��܂��B\n"
				L"�@�i���݂̎擾�Ԋu�́y%d�b�z�ł��j"
				, theApp.m_optionMng.m_nIntervalCheckSec );
			MessageBox( msg );
		}

		// �ݒ肷��
		theApp.m_pMainView->ResetIntervalTimer();
	}

	// �I�v�V�����̃g�O��
	theApp.m_optionMng.m_bEnableIntervalCheck = !theApp.m_optionMng.m_bEnableIntervalCheck;

	// �^�C�g���ύX
	MySetTitle();
}

/// ����擾���j���[�̐���
void CMainFrame::OnUpdateEnableIntervalCheck(CCmdUI *pCmdUI)
{
	pCmdUI->Enable();
	pCmdUI->SetCheck( theApp.m_optionMng.m_bEnableIntervalCheck ? TRUE : FALSE );
}

/// �o�[�W�����`�F�b�N
void CMainFrame::OnCheckUpdate()
{
	// ���C����ʂł���Η��p�\
	CView* pActiveView = GetActiveView();
	if (pActiveView == theApp.m_pMainView) {
		theApp.m_pMainView->DoCheckSoftwareUpdate();
	}
}

/// �o�[�W�����`�F�b�N���j���[�̐���
void CMainFrame::OnUpdateCheckUpdate(CCmdUI *pCmdUI)
{
	// ���C����ʂł���Η��p�\
	CView* pActiveView = GetActiveView();
	if (pActiveView == theApp.m_pMainView) {
		pCmdUI->Enable();
	} else {
		pCmdUI->Enable(FALSE);
	}
}

/// ��ʁb�O�̉�ʃ��j���[�̃C�x���g
void CMainFrame::OnMenuBack()
{
	OnBackButton();
}

/// ��ʁb���̉�ʃ��j���[�̃C�x���g
void CMainFrame::OnMenuNext()
{
	OnForwardButton();
}

void CMainFrame::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized)
{
	CFrameWnd::OnActivate(nState, pWndOther, bMinimized);

	// TODO: �����Ƀ��b�Z�[�W �n���h�� �R�[�h��ǉ����܂��B
}

void CMainFrame::OnCheckNew()
{
	theApp.m_pMainView->DoNewCommentCheck();
}

/**
 * �w���v�bMZ3 �w���v
 */
void CMainFrame::OnHelpMenu()
{
#ifdef WINCE
	// Readme.txt ����͂��ĕ\��
	theApp.m_pMainView->MyShowHelp();
#else
	// Win32 �̏ꍇ�̓I�����C���}�j���A���\��
	util::OpenByShellExecute( MZ4_MANUAL_URL);
#endif
}

/**
 * �w���v�b���ŗ���
 */
void CMainFrame::OnHistoryMenu()
{
	theApp.m_pMainView->MyShowHistory();
}

/**
 * �G���[���O���J��
 */
void CMainFrame::OnErrorlogMenu()
{
	theApp.m_pMainView->MyShowErrorlog();
}

/**
 * �X�L���ύX�B
 *
 * ����ɃX�L���ꗗ�̃��j���[��\������
 */
void CMainFrame::OnChangeSkin()
{
	CMenu menu;
	menu.LoadMenu(IDR_SKIN_MENU);
	CMenu* pcThisMenu = menu.GetSubMenu(0);

	// �X�L���t�H���_�̈ꗗ�𐶐�����
	std::vector<std::wstring> skinfileList;
	GetSkinFolderNameList(skinfileList);

	// ���X�g��
	for( int i=0; i<(int)skinfileList.size(); i++ ) {
		UINT flag = MF_STRING | MF_ENABLED;

		LPCTSTR filename = skinfileList[i].c_str();

		// ���݂̃X�L���ł���΃`�F�b�N
		if (filename == theApp.m_optionMng.m_strSkinname) {
			flag |= MF_CHECKED;
		}

		// �X�L���^�C�g�����擾
		CString title = CMZ3SkinInfo::loadSkinTitle( filename );

		// �X�L�������ǉ�����
		if (title != filename) {
			title.AppendFormat( L" (%s)", filename );
		}

		pcThisMenu->AppendMenu( flag, ID_SKIN_BASE+i, title );
	}

	// �_�~�[���폜
	pcThisMenu->DeleteMenu( ID_SKIN_DUMMY, MF_BYCOMMAND );

	POINT pt    = util::GetPopupPos();
	int   flags = util::GetPopupFlags();
	menu.GetSubMenu(0)->TrackPopupMenu(flags, pt.x, pt.y, this);
}

/**
 * �X�L���i�[�p�t�H���_���炷�ׂẴX�L���p�t�H���_�����擾����
 */
void CMainFrame::GetSkinFolderNameList(std::vector<std::wstring>& skinfileList)
{
    int    nResult = TRUE;
    HANDLE hFile  = INVALID_HANDLE_VALUE;
    TCHAR  szPath[ MAX_PATH ];
    WIN32_FIND_DATA data;

    //  �t�@�C���T��
	_stprintf( szPath, _T("%s\\*"), (LPCTSTR)theApp.m_filepath.skinFolder);
    hFile = FindFirstFile(szPath, &data);
    if( hFile != INVALID_HANDLE_VALUE )
    {
        do
        {
			if (data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
				// �f�B���N�g���������B�ǉ�����B

				// . ����n�܂�f�B���N�g���͏��O
				if (data.cFileName[0] != '.') {
					skinfileList.push_back( data.cFileName );
				}
			}
        }
        while( FindNextFile( hFile, &data) && nResult);

        FindClose( hFile );
    }

}

void CMainFrame::OnUpdateSkinMenuItem(CCmdUI *pCmdUI)
{
	pCmdUI->Enable();
}

/**
 * �X�L���؂�ւ�
 */
void CMainFrame::OnSkinMenuItem(UINT nID)
{
	CWnd* pWnd = GetActiveView();

	// �X�L���̗L���E�����`�F�b�N
	if (!theApp.m_optionMng.IsUseBgImage()) {
		return;
	}

	int n = nID - ID_SKIN_BASE;

	// �X�L���t�H���_�̈ꗗ�𐶐�����
	std::vector<std::wstring> skinfileList;
	GetSkinFolderNameList(skinfileList);

	// ���̓`�F�b�N
	if (n < 0 || n >= (int)skinfileList.size()) {
		return;
	}

	// �X�L�����̉���
	LPCTSTR szSkinName = skinfileList[n].c_str();
//	MessageBox( szSkinName );

	// ���݂̃X�L�����̕ۑ�
	CString strOriginalSkinName = theApp.m_optionMng.m_strSkinname;

	// �X�L���؂�ւ�
	theApp.m_optionMng.m_strSkinname = szSkinName;

	// �X�L�����̍X�V
	theApp.LoadSkinSetting();

	// �X�L���t�@�C���`�F�b�N
	if (!theApp.m_bgImageMainBodyCtrl.isValidSkinfile(szSkinName) ||
		!theApp.m_bgImageMainCategoryCtrl.isValidSkinfile(szSkinName) ||
		!theApp.m_bgImageReportListCtrl.isValidSkinfile(szSkinName))
	{
		// �X�L���t�@�C����������Ȃ����ߏI��

		// �X�L������߂�
		theApp.m_optionMng.m_strSkinname = strOriginalSkinName;
		
		// �X�L�����̍X�V
		theApp.LoadSkinSetting();

		if (pWnd) {
			util::MySetInformationText( pWnd->GetSafeHwnd(), L"�X�L���摜�t�@�C����������܂���" );
		}
		return;
	}

	// �w�i�摜�������[�h����
	theApp.m_bgImageMainBodyCtrl.load();
	theApp.m_bgImageMainCategoryCtrl.load();
	theApp.m_bgImageReportListCtrl.load();

	// �����[�h
	ChangeAllViewFont();

	// ���b�Z�[�W
	CString msg;
	if (theApp.m_skininfo.strSkinName != szSkinName) {
		msg.Format( L"�X�L���� [%s (%s)] �ɕύX���܂���", theApp.m_skininfo.strSkinTitle, szSkinName );
	} else {
		msg.Format( L"�X�L���� [%s] �ɕύX���܂���", theApp.m_skininfo.strSkinTitle );
	}
	if (pWnd) {
		util::MySetInformationText( pWnd->GetSafeHwnd(), msg );
	}
}

/**
 * ���̑��b�S�Ă�����
 */
void CMainFrame::OnStartCruise()
{
	theApp.m_pMainView->StartCruise( false );
}

/**
 * ���̑��b���ǂ�����
 */
void CMainFrame::OnStartCruiseUnreadOnly()
{
	theApp.m_pMainView->StartCruise( true );
}

/**
 * ���̑��bmixi���o�C�����J��
 */
/*
void CMainFrame::OnOpenMixiMobileByBrowser()
{
	CString url = theApp.MakeLoginUrlForMixiMobile( L"home.pl" );
	util::OpenUrlByBrowser( url );
}
*/

/**
 * �E�\�t�g�L�[�����C�x���g�F�|�b�v�A�b�v
 */
void CMainFrame::OnMenuAction()
{
	CView* pActiveView = GetActiveView();

	if (pActiveView == theApp.m_pMainView) {
		// ���C���r���[
		theApp.m_pMainView->OnAcceleratorContextMenu();
		return;
	}

	if (pActiveView == theApp.m_pReportView) {
		// ���|�[�g�r���[
		theApp.m_pReportView->MyPopupReportMenu();
		return;
	}

	if (pActiveView == theApp.m_pWriteView) {
		// Write �r���[
		theApp.m_pWriteView->PopupWriteBodyMenu();
		return;
	}

	if (pActiveView == theApp.m_pDownloadView) {
		// �_�E�����[�h�r���[
		// �����Ȃ�
		return;
	}

}

void CMainFrame::OnDestroy()
{
	CFrameWnd::OnDestroy();

	// �I�����̃^�u�E�J�e�S���I����Ԃ�ۑ�
	CMZ3View* pView = (CMZ3View*)theApp.m_pMainView;
	theApp.m_optionMng.m_lastTopPageTabIndex      = pView->m_groupTab.GetCurSel();
	theApp.m_optionMng.m_lastTopPageCategoryIndex = pView->m_selGroup->selectedCategory;

#ifndef WINCE
	// �I�����̈ʒu�E�T�C�Y��ۑ�
	WINDOWPLACEMENT    wp;
    if (GetWindowPlacement(&wp)) {
		CString    cb;
		cb.Format( L"%04d %04d %04d %04d", 
			wp.rcNormalPosition.left, wp.rcNormalPosition.top, wp.rcNormalPosition.right, wp.rcNormalPosition.bottom);
		theApp.m_optionMng.m_strWindowPos = cb;
	}
#endif

#ifdef WINCE
	// ���j���[�̔j��
	if (m_hMenu!=NULL) {
		DestroyMenu(m_hMenu);
	}
#endif
}

BOOL CMainFrame::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	LPNMHDR nmhdr = (LPNMHDR)lParam;

#ifndef WINCE
	switch (nmhdr->code) {
	case TTN_NEEDTEXT:
		{
			static LPTOOLTIPTEXT lptip;
			lptip = (LPTOOLTIPTEXT)lParam;
			switch (lptip->hdr.idFrom) {
			case ID_BACK_BUTTON:	lptip->lpszText = L"�߂�";				break;
			case ID_FORWARD_BUTTON:	lptip->lpszText = L"�i��";				break;
			case ID_STOP_BUTTON:	lptip->lpszText = L"��~";				break;
			case ID_WRITE_BUTTON:
				// View �ɂ���ĈႤ
				if (GetActiveView() == theApp.m_pMainView) {
					lptip->lpszText = L"���L������";
				} else {
					lptip->lpszText = L"�R�����g������";
				}
				break;
			case ID_IMAGE_BUTTON:		lptip->lpszText = L"�摜���J��";		break;
			case ID_OPEN_BROWSER:		lptip->lpszText = L"�u���E�U�ŊJ��";	break;
			case ID_AUTO_RELOAD_BUTTON:	lptip->lpszText = L"����擾";			break;
			case ID_APP_ABOUT:			lptip->lpszText = L"�o�[�W�������";	break;
			}
		}
		break;
	default:
		break;
	}
#endif

	return CFrameWnd::OnNotify(wParam, lParam, pResult);
}

/// �^�C�g���ύX
void CMainFrame::MySetTitle(void)
{
#ifdef WINCE
	// �^�C�g���ύX
	CString title = MZ3_APP_NAME;

	// ����擾���
	if( theApp.m_optionMng.m_bEnableIntervalCheck ) {
		title += L".i";
	}

	SetTitle(title);
	if (m_hWnd) {
		SetWindowText(title);
	}
#else
	// �^�C�g���ύX
	CString title = MZ3_APP_NAME L" " MZ3_VERSION_TEXT;
	title.Replace( L"Version ", L"v" );

	// ����擾���
	if( theApp.m_optionMng.m_bEnableIntervalCheck ) {
		title += L" �y����擾�z";
	}

	SetTitle(title);
	if (m_hWnd) {
		SetWindowText(title);
	}
#endif
}

void CMainFrame::OnMove(int x, int y)
{
	CFrameWnd::OnMove(x, y);

	if (theApp.m_pMainView) {
		theApp.m_pMainView->MoveMiniImageDlg();
	}
}

void CMainFrame::OnDownloadManagerView()
{
	m_bForwardPageEnabled = FALSE;
	m_bBackPageEnabled = TRUE;

	theApp.ChangeView(theApp.m_pDownloadView);
}

BOOL CMainFrame::PreTranslateMessage(MSG* pMsg)
{
#ifndef WINCE
	switch (pMsg->message) {
	case WM_XBUTTONUP:
		{
			// IntelliMouse�̃T�C�h�L�[�{�^��
			switch (GET_XBUTTON_WPARAM(pMsg->wParam)) {
			case XBUTTON1:
				// first X button
				OnBackButton();
				return TRUE;
			case XBUTTON2:
				// second X button
				OnForwardButton();
				return TRUE;
			}
		}
		break;
	}
#endif

	return CFrameWnd::PreTranslateMessage(pMsg);
}

/**
 * �C�ӂ�URL���J��
 */
void CMainFrame::OnMenuOpenUrl()
{
	// ���C���r���[�\��
	theApp.ChangeView(theApp.m_pMainView);

	// �Ϗ�
	theApp.m_pMainView->MyOpenUrl();
}

/**
 * �C�ӂ̃��[�J���t�@�C�����J��
 */
void CMainFrame::OnMenuOpenLocalFile()
{
	// ���C���r���[�\��
	theApp.ChangeView(theApp.m_pMainView);

	// �Ϗ�
	theApp.m_pMainView->MyOpenLocalFile();
}

/// �u�����T�C�Y�b�傫������v���j���[
void CMainFrame::OnMenuFontMagnify()
{
	theApp.m_optionMng.m_fontHeight = option::Option::normalizeFontSize( theApp.m_optionMng.m_fontHeight+1 );
	ChangeAllViewFont();
}

/// �u�����T�C�Y�b����������v���j���[
void CMainFrame::OnMenuFontShrink()
{
	theApp.m_optionMng.m_fontHeight = option::Option::normalizeFontSize( theApp.m_optionMng.m_fontHeight-1 );
	ChangeAllViewFont();
}

/// �u�X�N���v�g�̃����[�h�v���j���[
void CMainFrame::OnMenuReloadLuaScripts()
{
	// �N���[�Y
	theApp.MyLuaClose();

	// ������
	theApp.MyLuaInit();
}

/// �u�J�e�S�����O�̃����[�h�v���j���[
void CMainFrame::OnMenuReloadCategoryListLog()
{
	theApp.m_pMainView->ReloadCategoryListLog();
}
