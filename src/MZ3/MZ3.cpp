// MZ3.cpp : �A�v���P�[�V�����̃N���X������`���܂��B
//

#include "stdafx.h"
#include "MZ3.h"
#include "MainFrm.h"

#include "MZ3Doc.h"
#include "MZ3View.h"
#include "ReportView.h"
#include "WriteView.h"
#include "AboutDlg.h"
#include "util.h"
#include "DebugDlg.h"
#include "url_encoder.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMZ3App

BEGIN_MESSAGE_MAP(CMZ3App, CWinApp)
	ON_COMMAND(ID_APP_ABOUT, &CMZ3App::OnAppAbout)
END_MESSAGE_MAP()



// -----------------------------------------------------------------------------
// �R���X�g���N�^
// -----------------------------------------------------------------------------
CMZ3App::CMZ3App()
	: CWinApp()
{
}

// -----------------------------------------------------------------------------
// �f�X�g���N�^
// -----------------------------------------------------------------------------
CMZ3App::~CMZ3App()
{
	if( m_pReportView != NULL ) {
		delete m_pReportView;
		m_pReportView = NULL;
	}
//	if( m_pWriteView != NULL ) {
//		delete m_pWriteView;
//		m_pWriteView = NULL;
//	}
}

// �B��� CMZ3App �I�u�W�F�N�g�ł��B
CMZ3App theApp;

// CMZ3App ������

BOOL CMZ3App::InitInstance()
{
	// �𑜓x����
	{
		int horizontal = DRA::GetScreenCaps(HORZRES);
		int vertical   = DRA::GetScreenCaps(VERTRES);

		if( (horizontal == 240 && vertical == 320) || (horizontal == 320 && vertical == 240) ) {
			currentDisplayMode = SR_QVGA;
		}else if( (horizontal == 480 && vertical == 640) || (horizontal == 640 && vertical == 480) ) {
			currentDisplayMode = SR_VGA;
		}else if( (horizontal == 480 && vertical == 800) || (horizontal == 800 && vertical == 480) ) {
			//EM�ONE�Ή�
			currentDisplayMode = SR_VGA;
		}else if( (horizontal == 240 && vertical == 240) ) {
			currentDisplayMode = SR_SQUARE240;
		}else{
			// �f�t�H���g�l
			currentDisplayMode = SR_QVGA;
		}
	}

	// CAPEDIT ����� SIPPREF �̂悤�� Windows Mobile ���L�̃R���g���[��������������ɂ́A�A�v���P�[�V������
	// ���������� SHInitExtraControls ����x�Ăяo���K�v������܂��B
#ifdef	WINCE
	SHInitExtraControls();
#endif

	// �t�@�C���p�X�Q��������
	m_filepath.init();

	// MZ3 ���K�[�̏�����
	m_logger.init( m_filepath.mz3logfile );

	// ���O���x���̐ݒ�
#ifdef	MZ3_DEBUG
	// Release_MZ3 �Ȃ̂� debug �L��
	m_logger.setLogLevel( CSimpleLogger::CATEGORY_DEBUG );
#else
#ifdef	_DEBUG
	// Debug �ȊO�Ȃ̂� debug �L��
	m_logger.setLogLevel( CSimpleLogger::CATEGORY_DEBUG );
#else
	// �����[�X�Ȃ̂� info �܂ŗL��
	m_logger.setLogLevel( CSimpleLogger::CATEGORY_INFO );
#endif
#endif

	// �I�v�V�����ǂݍ���
	m_optionMng.Load();

	// �I�v�V�����ɂ���ăt�@�C���p�X�Q���Đ���
	m_filepath.init_logpath();

	// ���O�C�����̓ǂݍ���
	m_loginMng.Read();

	// ���O�pINI�t�@�C���̓ǂݍ���
	MZ3LOGGER_DEBUG( L"ini �t�@�C���ǂݍ��݊J�n" );
	m_logfile.Load( m_filepath.logfile );
	MZ3LOGGER_DEBUG( L"ini �t�@�C���ǂݍ��݊���" );

	CSingleDocTemplate* pDocTemplate;
	pDocTemplate = new CSingleDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(CMZ3Doc),
		RUNTIME_CLASS(CMainFrame),       // ���C�� SDI �t���[�� �E�B���h�E
		RUNTIME_CLASS(CMZ3View));
	if (!pDocTemplate)
		return FALSE;
	AddDocTemplate(pDocTemplate);

	// DDE�Afile open �ȂǕW���̃V�F�� �R�}���h�̃R�}���h���C������͂��܂��B
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);

	// �g�b�v�y�[�W�p�̏�����
	bool bExistGroupFile = util::ExistFile( theApp.m_filepath.groupfile );
	if( Mz3GroupDataReader::load( m_root, theApp.m_filepath.groupfile ) == false ) {
		// ���[�h���s
		// �t�@�C�������݂���̂Ƀ��[�h���s�Ȃ珉�����m�F�B
		// �t�@�C�������݂��Ȃ��Ȃ珉����ԂȂ̂Ŋm�F�Ȃ��ŏ������B
		if( bExistGroupFile ) {
			if( MessageBox( NULL, 
					L"�O���[�v��`�t�@�C���̓ǂݍ��݂Ɏ��s���܂����B\n"
					L"�f�t�H���g�̃O���[�v��`�ŋN�����܂����H\n\n"
					L"�u�������v�FMZ3���I�����܂��B",
					L"MZ3", MB_YESNO ) != IDYES ) 
			{
				MZ3LOGGER_INFO( L"���[�U�ɂ��L�����Z���̂��ߏI��" );
				return FALSE;
			}
		}
		// ������
		m_root.initForTopPage();
	}

	// �R�}���h ���C���Ŏw�肳�ꂽ�f�B�X�p�b�` �R�}���h�ł��B�A�v���P�[�V������
	// /RegServer�A/Register�A/Unregserver �܂��� /Unregister �ŋN�����ꂽ�ꍇ�AFALSE ��Ԃ��܂��B
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;

	m_nCmdShow = SW_SHOWMAXIMIZED;

	CDocument* pCurrentDoc = ((CFrameWnd*)m_pMainWnd)->GetActiveDocument();

	CCreateContext newContext;
	newContext.m_pNewViewClass = NULL;
	newContext.m_pNewDocTemplate = NULL;
	newContext.m_pLastView = NULL;
	newContext.m_pCurrentFrame = NULL;
	newContext.m_pCurrentDoc = pCurrentDoc;

	m_pMainView		= (CMZ3View*)((CFrameWnd*)m_pMainWnd)->GetActiveView();
	m_pReportView	= new CReportView;
	m_pWriteView	= new CWriteView;

	// �r���[�̏�����
	INT viewID = AFX_IDW_PANE_FIRST + 1;
	RECT rect;
	SystemParametersInfo(SPI_GETWORKAREA, 0, &rect, 0);

	// ���|�[�g�r���[�̏�����
	((CView*)m_pReportView)->Create(NULL, _T("MZ3 ReportView"), WS_CHILD, rect,
		m_pMainWnd, viewID, &newContext);
	m_pReportView->OnInitialUpdate();

	// �������݃r���[�̏�����
	((CView*)m_pWriteView)->Create(NULL, _T("MZ3 WriteView"), WS_CHILD, rect,
		m_pMainWnd, viewID, &newContext);
	m_pWriteView->OnInitialUpdate();

	// ���C�� �E�B���h�E�����������ꂽ�̂ŁA�\���ƍX�V���s��
	m_pMainWnd->ShowWindow( SW_SHOW );
	m_pMainWnd->UpdateWindow();

	// ����N�����i���[�UID�A�p�X���[�h���ݒ莞�j��
	// ���[�U�ݒ��ʂ�\������B
	if( wcslen(m_loginMng.GetEmail()) == 0 ||
		wcslen(m_loginMng.GetPassword()) == 0 )
	{
		CString msg;
		msg += L"MZ3 ���_�E�����[�h���Ă����������肪�Ƃ��������܂��I\n\n";
		msg += L"���̉�ʂŏ����ݒ�i���O�C���ݒ�j���s���Ă��������B";
		MessageBox( NULL, msg, L"MZ3.i", MB_OK );

		// ���[�U�ݒ��ʂ�\������
		((CMainFrame*)m_pMainWnd)->OnSettingLogin();
	}

	return TRUE;
}

int CMZ3App::ExitInstance()
{
//	CString msg;
//	msg.Format( L"���O�t�@�C����\n%s\n�ɕۑ����܂�", logfile );
//	MessageBox( NULL, msg, 0, MB_OK );

	MZ3LOGGER_DEBUG( L"MZ3 �I�������J�n" );

	// ���O�t�@�C���̕ۑ�
	if( m_logfile.Save( m_filepath.logfile ) == false ) {
		MessageBox( NULL, L"���O�t�@�C���ۑ����s", m_filepath.logfile, MB_OK );
	}

	// �I�v�V�����̕ۑ�
	m_optionMng.Save();

	// �O���[�v��`�t�@�C���̕ۑ�
	Mz3GroupDataWriter::save( m_root, m_filepath.groupfile );

	MZ3LOGGER_DEBUG( L"MZ3 �I����������" );

	MZ3LOGGER_INFO( L"MZ3 �I��" );

	return CWinApp::ExitInstance();
}

/// �o�[�W�������
void CMZ3App::OnAppAbout()
{
	CAboutDlg dlg;
	dlg.DoModal();
}

/**
 * �r���[�؂�ւ�
 */
void CMZ3App::ChangeView( CView* pNewView )
{
	// ���݂̃r���[
	CView* pActiveView = ((CFrameWnd*)m_pMainWnd)->GetActiveView();
	UINT temp = ::GetWindowLong( pActiveView->m_hWnd, GWL_ID );
	::SetWindowLong( pActiveView->m_hWnd, GWL_ID, ::GetWindowLong(pNewView->m_hWnd, GWL_ID) );
	::SetWindowLong( pNewView->m_hWnd, GWL_ID, temp );

	pActiveView->ShowWindow( SW_HIDE );
	pNewView->ShowWindow( SW_SHOW );
	((CFrameWnd*)m_pMainWnd)->SetActiveView( pNewView );
	((CFrameWnd*)m_pMainWnd)->RecalcLayout( TRUE );
	m_pMainWnd->UpdateWindow();

	::SendMessage( pNewView->m_hWnd, WM_MZ3_FIT, NULL, NULL );
}

/**
 * �A�v���P�[�V�������s�f�B���N�g���p�X�̎擾
 */
CString CMZ3App::GetAppDirPath()
{
	CString appPath;

	{
		TCHAR path[256];

		memset(path, 0x00, sizeof(TCHAR) * 256);
		GetModuleFileName(NULL, path, 256);

		appPath = path;
	}

	// �Ō��\���擾���Ă������獶��Ԃ�
	return appPath.Left(appPath.ReverseFind('\\'));
}

/**
 * ���[�U�ݒ�iE-Mail�A�h���X�A�p�X���[�h�j��p���āA���O�C���y�[�W�̎��s���URL�𐶐�����
 */
CString CMZ3App::GetLoginUri()
{
	// �A�h���X�A�p�X���[�h�� EUC �G���R�[�h����
	CString mail = URLEncoder::encode_euc( m_loginMng.GetEmail() );
	CString pass = URLEncoder::encode_euc( m_loginMng.GetPassword() );

	CString uri;
	uri.Format(_T("http://mixi.jp/login.pl?email=%s&password=%s&next_url=/home.pl"),
		mail,
		pass);

//	MessageBox( NULL, uri, L"MZ3", MB_OK );
	return uri;
}

/// �R�}���h�o�[�̃{�^���̗L���E��������
BOOL CMZ3App::EnableCommandBarButton( int nID, BOOL bEnable )
{
	return ((CMainFrame*)m_pMainWnd)->m_wndCommandBar.GetToolBarCtrl().EnableButton( nID, bEnable);
}

/**
 * ���ʃt�H���g�쐬
 *
 * pBaseFont ���x�[�X�ɁA������ fontHeight, �t�H���g���� fontFace �ɕύX�����t�H���g���쐬���A
 * m_font �Ƃ���B
 */
bool CMZ3App::MakeNewFont( CFont* pBaseFont, int fontHeight, LPCTSTR fontFace )
{
	CFont *pFont = pBaseFont;
	if (pFont)
	{
		LOGFONT lf;
		pFont->GetLogFont( &lf );

		if( fontHeight != 0 ) {
			lf.lfHeight = fontHeight;
		}
		if( wcslen(fontFace) > 0 ) {
			wcscpy_s( lf.lfFaceName, 31, fontFace );
		}

		theApp.m_font.Detach();
		theApp.m_font.CreateFontIndirect( &lf );
	}
	return true;
}

/**
 * CMixiData ��\������i�f�o�b�O�p�j
 */
void CMZ3App::ShowMixiData( CMixiData* data )
{
	CString msg;

	if( data==NULL ) {
		return;
	}

	CDebugDlg dlg;
	dlg.m_data = data;
	dlg.DoModal();
}

/// �t�@�C���p�X�̏������B
/// ���O�֘A�͏����B
void CMZ3App::FilePath::init()
{
	TCHAR temppath[256];
	::GetTempPath(256, temppath);

	// MZ3 �{�̂̃��O�t�@�C��
	mz3logfile.Format( L"%s\\mz3log.txt", theApp.GetAppDirPath() );

	// HTML �p�ꎞ�t�@�C��(SJIS)
	temphtml.Format( _T("%s\\mz3s.dat"), temppath );

	// �I�v�V�����pINI�t�@�C���̃p�X
	inifile.Format(_T("%s\\%s"), theApp.GetAppDirPath(), _T("mz3.ini"));

	// ���O�t�@�C���iINI�t�@�C���j�̃p�X
	logfile.Format(_T("%s\\%s"), theApp.GetAppDirPath(), _T("log.ini"));

	// �O���[�v��`�t�@�C���iINI�t�@�C���j�̃p�X
	groupfile.Format(_T("%s\\%s"), theApp.GetAppDirPath(), _T("toppage_group.ini"));

	// Readme.txt �̃p�X
	helpfile.Format(_T("%s\\%s"), theApp.GetAppDirPath(), _T("Readme.txt"));

	// ChangeLog.txt �̃p�X
	historyfile.Format(_T("%s\\%s"), theApp.GetAppDirPath(), _T("ChangeLog.txt"));

	// �O���[�v���X�g�w�i�摜�̃p�X
	groupBgImage.Format( L"%s\\skin\\group.jpg", theApp.GetAppDirPath() );

	// �J�e�S�����X�g�w�i�摜�̃p�X
	categoryBgImage.Format( L"%s\\skin\\header.jpg", theApp.GetAppDirPath() );

	// �{�f�B���X�g�w�i�摜�̃p�X
	bodyBgImage.Format( L"%s\\skin\\body.jpg", theApp.GetAppDirPath() );

	// ���|�[�g���X�g�w�i�摜�̃p�X
	reportBgImage.Format( L"%s\\skin\\report.jpg", theApp.GetAppDirPath() );
}

/// ���O�֘A�t�@�C���p�X�̏�����
void CMZ3App::FilePath::init_logpath()
{
	//--- �t�H���_

	// ���O�p�t�H���_
	// �I�v�V�����Ŏw�肳��Ă���΂�����g���B
	// �w�肳��Ă��Ȃ���΁A�f�t�H���g�o�͐�𐶐�����B
	logFolder = theApp.m_optionMng.GetLogFolder();
	if( logFolder.IsEmpty() ) {
		logFolder.Format(_T("%s\\log"), theApp.GetAppDirPath());
	}
	CreateDirectory( logFolder, NULL/*always null*/ );

	// �f�[�^�p�t�H���_�̍쐬
	downloadFolder.Format(_T("%s\\download"), logFolder);
	CreateDirectory( downloadFolder, NULL/*always null*/ );

	// �_�E�����[�h�ς݉摜�t�@�C���p�t�H���_
	imageFolder.Format(_T("%s\\image"), logFolder );
	CreateDirectory( imageFolder, NULL/*always null*/ );

	// ���L�̃��O�p�t�H���_�̃p�X
	diaryFolder.Format(_T("%s\\diary"), logFolder );
	CreateDirectory( diaryFolder, NULL/*always null*/ );

	// �R�~���j�e�B�������݂̃��O�p�t�H���_�̃p�X
	bbsFolder.Format(_T("%s\\bbs"), logFolder );
	CreateDirectory( bbsFolder, NULL/*always null*/ );

	// �A���P�[�g�̃��O�p�t�H���_�̃p�X
	enqueteFolder.Format(_T("%s\\enquete"), logFolder );
	CreateDirectory( enqueteFolder, NULL/*always null*/ );

	// �C�x���g�̃��O�p�t�H���_�̃p�X
	eventFolder.Format(_T("%s\\event"), logFolder );
	CreateDirectory( eventFolder, NULL/*always null*/ );

	// ���b�Z�[�W�̃��O�p�t�H���_�̃p�X
	messageFolder.Format(_T("%s\\message"), logFolder );
	CreateDirectory( messageFolder, NULL/*always null*/ );

	// �j���[�X�̃��O�p�t�H���_�̃p�X
	newsFolder.Format(_T("%s\\news"), logFolder );
	CreateDirectory( newsFolder, NULL/*always null*/ );

}

