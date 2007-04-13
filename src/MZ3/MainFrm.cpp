// MainFrm.cpp : CMainFrame �N���X�̎���
//

#include "stdafx.h"
#include "MZ3.h"

#include "MainFrm.h"
#include "OptionSheet.h"
#include "OptionTabUser.h"
#include "OptionTabProxy.h"
#include "OptionTabGetPage.h"
#include "ReportView.h"
#include "WriteView.h"
#include "MZ3View.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

const DWORD dwAdornmentFlags = 0; // [�I��] �{�^��

// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	ON_WM_CREATE()
    ON_COMMAND(ID_BACK_BUTTON, OnBackButton)
    ON_COMMAND(ID_FORWARD_BUTTON, OnForwardButton)
    ON_COMMAND(ID_STOP_BUTTON, OnStopButton)
    ON_UPDATE_COMMAND_UI(ID_BACK_BUTTON, OnUpdateBackButton)
    ON_UPDATE_COMMAND_UI(ID_FORWARD_BUTTON, OnUpdateForwardButton)
    ON_UPDATE_COMMAND_UI(ID_STOP_BUTTON, OnUpdateStopButton)
    ON_UPDATE_COMMAND_UI(ID_IMAGE_BUTTON, OnUpdateImageButton)
    ON_UPDATE_COMMAND_UI(ID_WRITE_BUTTON, OnUpdateWriteButton)
    ON_UPDATE_COMMAND_UI(ID_OPEN_BROWSER, OnUpdateBrowserButton)
	ON_COMMAND(ID_SETTING_LOGIN, &CMainFrame::OnSettingLogin)
	ON_COMMAND(ID_SETTING_GENERAL, &CMainFrame::OnSettingGeneral)
	ON_COMMAND(ID_MENU_CLOSE, &CMainFrame::OnMenuClose)
	ON_UPDATE_COMMAND_UI(IDM_GETPAGE_ALL, &CMainFrame::OnUpdateGetpageAll)
	ON_COMMAND(IDM_GETPAGE_ALL, &CMainFrame::OnGetpageAll)
	ON_COMMAND(IDM_GETPAGE_LATEST10, &CMainFrame::OnGetpageLatest10)
	ON_UPDATE_COMMAND_UI(IDM_GETPAGE_LATEST10, &CMainFrame::OnUpdateGetpageLatest10)
	ON_COMMAND(ID_CHANGE_FONT_BIG, &CMainFrame::OnChangeFontBig)
	ON_COMMAND(ID_CHANGE_FONT_MEDIUM, &CMainFrame::OnChangeFontMedium)
	ON_COMMAND(ID_CHANGE_FONT_SMALL, &CMainFrame::OnChangeFontSmall)
	ON_UPDATE_COMMAND_UI(ID_MENU_BACK, &CMainFrame::OnUpdateMenuBack)
	ON_UPDATE_COMMAND_UI(ID_MENU_NEXT, &CMainFrame::OnUpdateMenuNext)
	ON_COMMAND(ID_MENU_BACK, &CMainFrame::OnMenuBack)
	ON_COMMAND(ID_MENU_NEXT, &CMainFrame::OnMenuNext)
	ON_WM_ACTIVATE()
	ON_COMMAND(IDM_CHECK_NEW, &CMainFrame::OnCheckNew)
	ON_COMMAND(ID_HELP_MENU, &CMainFrame::OnHelpMenu)
	ON_COMMAND(ID_HISTORY_MENU, &CMainFrame::OnHistoryMenu)
	ON_COMMAND(IDM_START_CRUISE, &CMainFrame::OnStartCruise)
END_MESSAGE_MAP()


// CMainFrame �R���X�g���N�V����/�f�X�g���N�V����

// -----------------------------------------------------------------------------
// �R���X�g���N�^
// -----------------------------------------------------------------------------
CMainFrame::CMainFrame()
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

	int id_toolbar = 0;
	switch( theApp.GetDisplayMode() ) {
	case SR_VGA:
		id_toolbar = IDR_TOOLBAR;
		break;
	case SR_QVGA:
	default:
		id_toolbar = IDR_TOOLBAR_QVGA;
		break;
	}
	if (!m_wndCommandBar.Create(this) ||
		!m_wndCommandBar.InsertMenuBar(IDR_MAINFRAME) ||
		!m_wndCommandBar.AddAdornments(dwAdornmentFlags) ||
		!m_wndCommandBar.LoadToolBar(id_toolbar)) {
			TRACE0("CommandBar �̍쐬�Ɏ��s���܂���\n");
			return -1;      // �쐬�ł��܂���ł����B
	}

	m_wndCommandBar.SetBarStyle(m_wndCommandBar.GetBarStyle() | CBRS_SIZE_FIXED);

	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if (!CFrameWnd::PreCreateWindow(cs))
		return FALSE;

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
	CView* pActiveView = GetActiveView();

	if (pActiveView == theApp.m_pReportView) {
		// ���|�[�g�r���[ �� ���C���r���[

		// �I������
		((CReportView*)pActiveView)->EndProc();

		theApp.EnableCommandBarButton( ID_FORWARD_BUTTON, TRUE );
		theApp.EnableCommandBarButton( ID_BACK_BUTTON, FALSE );

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
			if (theApp.m_pWriteView->IsSendEnd() == FALSE) {
				// �����M�f�[�^�L��
				int ret = ::MessageBox(m_hWnd, _T("�����e�̃f�[�^������܂�\n�j������܂�����낵���ł����H"),
					_T("MZ3"), MB_ICONQUESTION | MB_OKCANCEL);
				if (ret == IDCANCEL) {
					// �����𒆎~
					return;
				}
				theApp.m_pWriteView->SetSendEnd(TRUE);
			}

			theApp.EnableCommandBarButton( ID_FORWARD_BUTTON, FALSE );
			theApp.EnableCommandBarButton( ID_BACK_BUTTON, FALSE );

			theApp.ChangeView(theApp.m_pMainView);
		} else {
			// Write �r���[�i���L�ȊO�j �� ���|�[�g�r���[
			theApp.EnableCommandBarButton( ID_FORWARD_BUTTON, TRUE );
			theApp.EnableCommandBarButton( ID_BACK_BUTTON, TRUE );

			// ReportView�ɖ߂�
			theApp.ChangeView(theApp.m_pReportView);
		}
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
	CView* pActiveView = GetActiveView();

	if (pActiveView == theApp.m_pReportView) {
		// ���|�[�g�r���[ �� �������݃r���[
		// �A���A�����M�̏ꍇ�̂݁B
		if( theApp.m_pWriteView->m_sendEnd == FALSE ) {
			theApp.EnableCommandBarButton( ID_FORWARD_BUTTON, FALSE );
			theApp.EnableCommandBarButton( ID_BACK_BUTTON, TRUE );

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
		// ���M�����t���O(m_sendEnd)��ON�Ȃ�A�u�������݃r���[�ɍs����v�Ɣ��f����
		theApp.EnableCommandBarButton( ID_FORWARD_BUTTON, (theApp.m_pWriteView->m_sendEnd == FALSE) ? TRUE : FALSE );
		theApp.EnableCommandBarButton( ID_BACK_BUTTON, TRUE );

		theApp.ChangeView(theApp.m_pReportView);
	}
}

// -----------------------------------------------------------------------------
// ���O�C���ݒ�{�^��
// -----------------------------------------------------------------------------
void CMainFrame::OnSettingLogin()
{
	COptionSheet cPropSht( _T("�I�v�V����"));
	cPropSht.SetPage();
	cPropSht.SetActivePage(0);
	if (cPropSht.DoModal() == IDOK) {
		theApp.m_optionMng.Save();

		// �t�H���g�̃��Z�b�g
		ChangeAllViewFont( theApp.m_optionMng.m_fontHeight );
	}
}

// -----------------------------------------------------------------------------
// �X�g�b�v�{�^���̐���
// -----------------------------------------------------------------------------
void CMainFrame::OnUpdateStopButton(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(m_wndCommandBar.GetToolBarCtrl().IsButtonEnabled(ID_STOP_BUTTON));  
}

// -----------------------------------------------------------------------------
// �߂�{�^���̐���
// -----------------------------------------------------------------------------
void CMainFrame::OnUpdateBackButton(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(m_wndCommandBar.GetToolBarCtrl().IsButtonEnabled(ID_BACK_BUTTON));  
}

// -----------------------------------------------------------------------------
// �i�ރ{�^���̐���
// -----------------------------------------------------------------------------
void CMainFrame::OnUpdateForwardButton(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(m_wndCommandBar.GetToolBarCtrl().IsButtonEnabled(ID_FORWARD_BUTTON));  
}

// -----------------------------------------------------------------------------
// �摜�{�^���̐���
// -----------------------------------------------------------------------------
void CMainFrame::OnUpdateImageButton(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(m_wndCommandBar.GetToolBarCtrl().IsButtonEnabled(ID_IMAGE_BUTTON));  
}

// -----------------------------------------------------------------------------
// �������݃{�^���̐���
// -----------------------------------------------------------------------------
void CMainFrame::OnUpdateWriteButton(CCmdUI* pCmdUI)
{
	pCmdUI->Enable( m_wndCommandBar.GetToolBarCtrl().IsButtonEnabled(ID_WRITE_BUTTON) );
}

// -----------------------------------------------------------------------------
// �u���E�U�{�^���̐���
// -----------------------------------------------------------------------------
void CMainFrame::OnUpdateBrowserButton(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(m_wndCommandBar.GetToolBarCtrl().IsButtonEnabled(ID_OPEN_BROWSER));  
}

// -----------------------------------------------------------------------------
// �ݒ�
// -----------------------------------------------------------------------------
void CMainFrame::OnSettingGeneral()
{
	COptionSheet cPropSht( _T("�I�v�V����"));
	cPropSht.SetPage();
	cPropSht.SetActivePage(1);
	if (cPropSht.DoModal() == IDOK) {
		theApp.m_optionMng.Save();

		// �t�H���g�̃��Z�b�g
		ChangeAllViewFont( theApp.m_optionMng.m_fontHeight );
	}
}

/// �u����v���j���[�C�x���g�n���h��
void CMainFrame::OnMenuClose()
{
	int iRet;
	iRet = ::MessageBox(m_hWnd, _T("�l�y�R���I�����܂����H"), _T("�l�y�R"), MB_ICONQUESTION | MB_OKCANCEL);
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
	theApp.m_optionMng.SetPageType( GETPAGE_LATEST10 );
}

/// �u�ŐV�P�O���擾�v���j���[�̃`�F�b�N��Ԑ���
void CMainFrame::OnUpdateGetpageLatest10(CCmdUI *pCmdUI)
{
	if( theApp.m_optionMng.GetPageType() == GETPAGE_LATEST10 ) {
		pCmdUI->SetCheck( TRUE );
	}else{
		pCmdUI->SetCheck( FALSE );
	}
}

/// �u�����T�C�Y�b��v���j���[
void CMainFrame::OnChangeFontBig()
{
	theApp.m_optionMng.m_fontHeight = theApp.m_optionMng.m_fontHeightBig;
	ChangeAllViewFont( theApp.m_optionMng.m_fontHeight );
}

/// �u�����T�C�Y�b���v���j���[
void CMainFrame::OnChangeFontMedium()
{
	theApp.m_optionMng.m_fontHeight = theApp.m_optionMng.m_fontHeightMedium;
	ChangeAllViewFont( theApp.m_optionMng.m_fontHeight );
}

/// �u�����T�C�Y�b���v���j���[
void CMainFrame::OnChangeFontSmall()
{
	theApp.m_optionMng.m_fontHeight = theApp.m_optionMng.m_fontHeightSmall;
	ChangeAllViewFont( theApp.m_optionMng.m_fontHeight );
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
	// �t�H���g�̍쐬
	theApp.MakeNewFont( theApp.m_pMainView->GetFont(), fontHeight, theApp.m_optionMng.GetFontFace() );

	//--- ���C���r���[
	{
		CMZ3View* pView = (CMZ3View*)theApp.m_pMainView;

		// �O���[�v�^�u
//		MySetListViewFont( &pView->m_groupList );
		pView->m_groupTab.SetFont( &theApp.m_font );

		// �J�e�S�����X�g
		MySetListViewFont( &pView->m_categoryList );

		// �{�f�B���X�g
		MySetListViewFont( &pView->m_bodyList );

		// �C���t�H���[�V�����G�f�B�b�g
		pView->m_infoEdit.SetFont( &theApp.m_font );
	}

	//--- ���|�[�g�r���[
	{
		CReportView* pView = theApp.m_pReportView;

		// �^�C�g��
		pView->m_titleEdit.SetFont( &theApp.m_font );

		// ���X�g
		MySetListViewFont( &pView->m_list );

		// �G�f�B�b�g
		pView->m_edit.SetFont( &theApp.m_font );

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
	}

	// �T�C�Y�ύX
	{
		CRect rect;
		SystemParametersInfo(SPI_GETWORKAREA, 0, &rect, 0);
		int w = rect.Width();
		int h = rect.Height() - rect.top;
		SetWindowPos( NULL, 0, 0, w, h,
			SWP_NOMOVE | SWP_NOZORDER );

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
	pCmdUI->Enable( m_wndCommandBar.GetToolBarCtrl().IsButtonEnabled(ID_BACK_BUTTON) );
}

/// ��ʁb���̉�ʃ��j���[�̐���
void CMainFrame::OnUpdateMenuNext(CCmdUI *pCmdUI)
{
	pCmdUI->Enable( m_wndCommandBar.GetToolBarCtrl().IsButtonEnabled(ID_FORWARD_BUTTON) );
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
	theApp.m_pMainView->MyShowHelp();
}

/**
 * �w���v�b���ŗ���
 */
void CMainFrame::OnHistoryMenu()
{
	theApp.m_pMainView->MyShowHistory();
}

/**
 * ���̑��b����
 */
void CMainFrame::OnStartCruise()
{
	theApp.m_pMainView->StartCruise();
}
