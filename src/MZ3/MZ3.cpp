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
#include "version.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMZ3App

BEGIN_MESSAGE_MAP(CMZ3App, CWinApp)
	ON_COMMAND(ID_APP_ABOUT, &CMZ3App::OnAppAbout)
END_MESSAGE_MAP()


#define SZ_REG_ROOT		TEXT("System\\GWE\\Display")
#define SZ_REG_DBVOL	TEXT("LogicalPixelsY")

// -----------------------------------------------------------------------------
// �R���X�g���N�^
// -----------------------------------------------------------------------------
CMZ3App::CMZ3App()
	: CWinApp()
	, m_dpi(96)
	, m_bgImageReportListCtrl(L"report.jpg")
	, m_bgImageMainBodyCtrl(L"body.jpg")
	, m_bgImageMainCategoryCtrl(L"header.jpg")
	// �v���b�g�t�H�[���p�̃t���O
	, m_bPocketPC(FALSE)
	, m_bSmartphone(FALSE)
	, m_bWinMoFiveOh(FALSE)
	, m_bWinMo2003(FALSE)
	, m_bWinMo2003_SE(FALSE)
{
}

// -----------------------------------------------------------------------------
// �f�X�g���N�^
// -----------------------------------------------------------------------------
CMZ3App::~CMZ3App()
{
#ifndef WINCE
	return;
#endif
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
	// �v���b�g�t�H�[������
	InitPlatformFlags();

	// �𑜓x/DPI����
	InitResolutionFlags();

	// CAPEDIT ����� SIPPREF �̂悤�� Windows Mobile ���L�̃R���g���[��������������ɂ́A�A�v���P�[�V������
	// ���������� SHInitExtraControls ����x�Ăяo���K�v������܂��B
#ifdef	WINCE
	SHInitExtraControls();
#endif

#ifndef	WINCE
	// Win32 �p�Ƀ��P�[����ݒ�
	setlocale( LC_ALL, "Japanese" ); 
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

	MZ3LOGGER_INFO( MZ3_APP_NAME L" �N���J�n " + util::GetSourceRevision() );

	// �I�v�V�����ǂݍ���
	m_optionMng.Load();

	// �f�o�b�O���[�h�Ȃ烍�O���x���ύX
	if( m_optionMng.IsDebugMode() ) {
		m_logger.setLogLevel( CSimpleLogger::CATEGORY_DEBUG );
	}

	// �I�v�V�����ɂ���ăt�@�C���p�X�Q���Đ���
	m_filepath.init_logpath();

	// ���O�C�����̓ǂݍ���
	m_loginMng.Read();

	// ���O�pINI�t�@�C���̓ǂݍ���
	MZ3LOGGER_DEBUG( L"ini �t�@�C���ǂݍ��݊J�n" );
	m_logfile.Load( m_filepath.logfile );
	MZ3LOGGER_DEBUG( L"ini �t�@�C���ǂݍ��݊���" );

	// �X�L���֘A�̏�����
	LoadSkinSetting();

	// IE �R���|�[�l���g�̏�����
#ifdef WINCE
	if (m_optionMng.m_bRenderByIE) {	// TODO ���̏�����IE�����Ή��O�̂��߂̉��ݒ�B�����Ή���͏�Ɏ��s���邱�ƁB
		InitHTMLControl( AfxGetInstanceHandle() );
	}
#endif

	// �g�b�v�y�[�W�p�̏�����
	MZ3LOGGER_INFO( L"�^�u�ݒ�t�@�C���ǂݍ��݊J�n" );
	bool bExistGroupFile = util::ExistFile( theApp.m_filepath.groupfile );
	if( Mz3GroupDataReader::load( m_root, theApp.m_filepath.groupfile ) ) {
		MZ3LOGGER_INFO( L"�^�u�ݒ�t�@�C���ǂݍ��ݐ���" );
	}else{
		MZ3LOGGER_INFO( L"�^�u�ݒ�t�@�C���ǂݍ��ݎ��s" );
		// ���[�h���s
		// �t�@�C�������݂���̂Ƀ��[�h���s�Ȃ珉�����m�F�B
		// �t�@�C�������݂��Ȃ��Ȃ珉����ԂȂ̂Ŋm�F�Ȃ��ŏ������B
		if( bExistGroupFile ) {
			if( MessageBox( NULL, 
					L"�O���[�v��`�t�@�C���̓ǂݍ��݂Ɏ��s���܂����B\n"
					L"�f�t�H���g�̃O���[�v��`�ŋN�����܂����H\n\n"
					L"�u�������v�F" MZ3_APP_NAME L"���I�����܂��B",
					MZ3_APP_NAME, MB_YESNO ) != IDYES ) 
			{
				MZ3LOGGER_INFO( L"���[�U�ɂ��L�����Z���̂��ߏI��" );
				return FALSE;
			}
		}

		// ������
		MZ3LOGGER_INFO( L"�^�u������" );
		m_root.initForTopPage();
	}

	CSingleDocTemplate* pDocTemplate;
	pDocTemplate = new CSingleDocTemplate(
#ifdef WINCE
		IDR_MAINFRAME,
#else
		IDR_MAINFRAME_WIN32,
#endif
		RUNTIME_CLASS(CMZ3Doc),
		RUNTIME_CLASS(CMainFrame),       // ���C�� SDI �t���[�� �E�B���h�E
		RUNTIME_CLASS(CMZ3View));
	if (!pDocTemplate)
		return FALSE;
	AddDocTemplate(pDocTemplate);

	// DDE�Afile open �ȂǕW���̃V�F�� �R�}���h�̃R�}���h���C������͂��܂��B
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);

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
	((CView*)m_pReportView)->Create(NULL, MZ3_APP_NAME _T(" ReportView"), WS_CHILD, rect,
		m_pMainWnd, viewID, &newContext);
	m_pReportView->OnInitialUpdate();

	// �������݃r���[�̏�����
	((CView*)m_pWriteView)->Create(NULL, MZ3_APP_NAME _T(" WriteView"), WS_CHILD, rect,
		m_pMainWnd, viewID, &newContext);
	m_pWriteView->OnInitialUpdate();

	// ���C�� �E�B���h�E�����������ꂽ�̂ŁA�\���ƍX�V���s��
	m_pMainWnd->ShowWindow( SW_SHOW );
	m_pMainWnd->UpdateWindow();

#ifdef WINCE
	if( m_bSmartphone ) {
		// �S��ʕ\��
		::SHFullScreen(m_pMainWnd->m_hWnd, SHFS_HIDETASKBAR | SHFS_HIDESIPBUTTON );
		::ShowWindow(((CMainFrame*)m_pMainWnd)->m_hCommandBar, SW_HIDE);

		RECT rc;

		//get window size
		SystemParametersInfo(SPI_GETWORKAREA, 0, &rc, FALSE);

		m_pMainWnd->MoveWindow( 
			rc.left, 
			rc.top, 
			rc.right, 
			rc.bottom + MZ3_TOOLBAR_HEIGHT, 
			TRUE);
	}
#endif

	// ����N�����i���[�UID�A�p�X���[�h���ݒ莞�j��
	// ���[�U�ݒ��ʂ�\������B
	if( wcslen(m_loginMng.GetEmail()) == 0 ||
		wcslen(m_loginMng.GetPassword()) == 0 )
	{
		CString msg;
		msg += MZ3_APP_NAME;
		msg += L" ���_�E�����[�h���Ă����������肪�Ƃ��������܂��I\n\n";
		msg += L"���̉�ʂŏ����ݒ�i���O�C���ݒ�j���s���Ă��������B";
		MessageBox( NULL, msg, MZ3_APP_NAME, MB_OK );

		// ���[�U�ݒ��ʂ�\������
		((CMainFrame*)m_pMainWnd)->OnSettingLogin();
	}

	// �t�H�[�J�X�����C���r���[�ɕύX����
	m_pMainView->SetFocus();

	if( m_bSmartphone ) {
		// Smartphone/Standard ���Ə����`�悳��Ȃ����ۂ��̂ŁA�����ōĕ`�悵�Ă���
		((CMainFrame*)m_pMainWnd)->ChangeAllViewFont();
	}

	MZ3LOGGER_INFO( MZ3_APP_NAME L" ����������" );

	return TRUE;
}

/// �v���b�g�t�H�[���p�̃t���O�̐ݒ�
void CMZ3App::InitPlatformFlags()
{
#ifdef WINCE
	// �v���b�g�t�H�[�����̖₢���킹
	TCHAR atchPlat[64];
	SystemParametersInfo(SPI_GETPLATFORMTYPE, 64, (PVOID)atchPlat, 0);

	if (_tcsncmp(atchPlat, L"PocketPC", 64) == 0)
	{
		m_bPocketPC = TRUE;
	}

	if (_tcsncmp(atchPlat, L"SmartPhone", 64) == 0)
	{
		m_bSmartphone = TRUE;
	}

	OSVERSIONINFO osvi;
	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	GetVersionEx(&osvi);
	if ((osvi.dwMajorVersion == 5) && (osvi.dwMinorVersion == 01)) 
		m_bWinMoFiveOh = TRUE;
	if ((osvi.dwMajorVersion == 4) && (osvi.dwMinorVersion == 20)) 
		m_bWinMo2003 = TRUE;
	if ((osvi.dwMajorVersion == 4) && (osvi.dwMinorVersion == 21)) 
		m_bWinMo2003_SE = TRUE;
#endif
}

/**
 * �𑜓x/DPI�̔���
 */
void CMZ3App::InitResolutionFlags()
{
	// �𑜓x�̔���
#ifdef WINCE
	int horizontal = DRA::GetScreenCaps(HORZRES);
	int vertical   = DRA::GetScreenCaps(VERTRES);

	if( (horizontal == 240 && vertical == 320) || (horizontal == 320 && vertical == 240) ) {
		m_currentDisplayMode = SR_QVGA;
	}else if( (horizontal == 480 && vertical == 640) || (horizontal == 640 && vertical == 480) ) {
		m_currentDisplayMode = SR_VGA;
	}else if( (horizontal == 480 && vertical == 800) || (horizontal == 800 && vertical == 480) ) {
		// EM�ONE�Ή�
		m_currentDisplayMode = SR_VGA;
	}else if( (horizontal == 240 && vertical == 240) ) {
		m_currentDisplayMode = SR_SQUARE240;
	}else{
		// �f�t�H���g�l
		m_currentDisplayMode = SR_QVGA;
	}
#endif

	// DPI �l�̎擾
	{
		HKEY  hKey = NULL;
		DWORD dwDpi = m_dpi;	// "Dpi"�̃f�[�^���󂯎��
		DWORD dwType;			// �l�̎�ނ��󂯎��
		DWORD dwSize;			// �f�[�^�̃T�C�Y���󂯎��

		// ���W�X�g�����I�[�v��
		
		// �G���[�Ȃ���Βl���擾
		if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, SZ_REG_ROOT, 0, KEY_READ, &hKey) == 0) 
		{ 
			RegQueryValueEx(hKey, SZ_REG_DBVOL, NULL, &dwType, NULL, &dwSize);
			RegQueryValueEx(hKey, SZ_REG_DBVOL, NULL, &dwType, (LPBYTE)&dwDpi, &dwSize);

			// �G���[���Ȃ���Ύ擾�����l��ۑ��B
			m_dpi = dwDpi;

			//���W�X�g���̃N���[�Y
			RegCloseKey(hKey);
		}
	}
}


int CMZ3App::ExitInstance()
{
//	CString msg;
//	msg.Format( L"���O�t�@�C����\n%s\n�ɕۑ����܂�", logfile );
//	MessageBox( NULL, msg, 0, MB_OK );

	MZ3LOGGER_DEBUG( MZ3_APP_NAME L" �I�������J�n" );

	// ���O�t�@�C���̕ۑ�
	if( m_logfile.Save( m_filepath.logfile ) == false ) {
		MessageBox( NULL, L"���O�t�@�C���ۑ����s", m_filepath.logfile, MB_OK );
	}

	// �I�v�V�����̕ۑ�
	m_optionMng.Save();

	// �O���[�v��`�t�@�C���̕ۑ�
	Mz3GroupDataWriter::save( m_root, m_filepath.groupfile );

	MZ3LOGGER_DEBUG( MZ3_APP_NAME L" �I����������" );

	MZ3LOGGER_INFO( MZ3_APP_NAME L" �I��" );

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
CString CMZ3App::MakeLoginUrl( LPCTSTR nextUrl )
{
	// �A�h���X�A�p�X���[�h�Anext_url �� EUC �G���R�[�h����
	CString mail = URLEncoder::encode_euc( theApp.m_loginMng.GetEmail() );
	CString pass = URLEncoder::encode_euc( theApp.m_loginMng.GetPassword() );
	CString encodedNextUrl = URLEncoder::encode_euc( nextUrl );

	CString uri;
	uri.Format(_T("http://mixi.jp/login.pl?email=%s&password=%s&next_url=%s"),
		mail,
		pass,
		encodedNextUrl
		);

//	MessageBox( NULL, uri, MZ3_APP_NAME, MB_OK );
	return uri;
}

/**
 * ���[�U�ݒ�iE-Mail�A�h���X�A�p�X���[�h�j��p���āA�������O�C��URL�𐶐�����
 */
CString CMZ3App::MakeLoginUrlForMixiMobile( LPCTSTR nextUrl )
{
	// �A�h���X�A�p�X���[�h�Anext_url �� EUC �G���R�[�h����
	CString mail = URLEncoder::encode_euc( theApp.m_loginMng.GetEmail() );
	CString pass = URLEncoder::encode_euc( theApp.m_loginMng.GetPassword() );
	CString encodedNextUrl = URLEncoder::encode_euc( nextUrl );

	CString url;
	url.Format( 
		L"http://m.mixi.jp/login.pl?submit=main&type=full&mode=password&key=&next_url=%s&email=%s&password=%s",
		encodedNextUrl,
		mail,
		pass);

	return url;
}

/// �R�}���h�o�[�̃{�^���̗L���E��������
BOOL CMZ3App::EnableCommandBarButton( int nID, BOOL bEnable )
{
#ifdef WINCE
	if( theApp.m_bPocketPC ) {
		return ((CMainFrame*)m_pMainWnd)->m_wndCommandBar.GetToolBarCtrl().EnableButton( nID, bEnable);
	} else {
		return TRUE;
	}
#else
	return ((CMainFrame*)m_pMainWnd)->m_wndToolBar.GetToolBarCtrl().EnableButton( nID, bEnable);
#endif
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

	// temp_draftfile.txt �̃p�X
	tempdraftfile.Format(_T("%s\\%s"), theApp.GetAppDirPath(), _T("temp_draftfile.txt"));

	// �X�L���t�H���_�̃p�X
	skinFolder.Format( L"%s\\skin", theApp.GetAppDirPath() );
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

/**
 * ���̈�Ȃǂ̍������t�H���g�̍������x�[�X�Ɍv�Z����
 */
int CMZ3App::GetInfoRegionHeight( int fontHeight )
{
#ifdef WINCE
	switch( theApp.GetDisplayMode() ) {
	case SR_VGA:
		if( theApp.GetDPI() > 96 ) {
			// VGA ����RealVGA��
			return fontHeight +6*2;
		}else{
			// VGA ����RealVGA��
			return fontHeight +6;
		}
		break;
	default:
		// VGA �ȊO
		if (m_bSmartphone) {
			return fontHeight +6;
		} else {
			return fontHeight -4;
		}
	}
#else
	return fontHeight +6*2;
#endif
}

bool CMZ3App::LoadSkinSetting()
{
	// �ݒ�t�@�C���̃��[�h
	m_skininfo.loadSkinInifile( theApp.m_optionMng.m_strSkinname );

	// �e�摜�t�@�C�����̏�����
	theApp.m_bgImageMainBodyCtrl.setImageFileName( m_skininfo.strMainBodyCtrlImageFileName );
	theApp.m_bgImageMainCategoryCtrl.setImageFileName( m_skininfo.strMainCategoryCtrlImageFileName );
	theApp.m_bgImageReportListCtrl.setImageFileName( m_skininfo.strReportListCtrlImageFileName );

	return true;
}
