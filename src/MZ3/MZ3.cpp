/*
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */
// MZ3.cpp : �A�v���P�[�V�����̃N���X������`���܂��B
//

#include "stdafx.h"
#include "MZ3.h"
#include "MainFrm.h"
#include "MZ3Doc.h"
#include "MZ3View.h"
#include "ReportView.h"
#include "WriteView.h"
#include "Ran2View.h"
#include "DownloadView.h"
#include "AboutDlg.h"
#include "util.h"
#include "util_gui.h"
#include "DebugDlg.h"
#include "url_encoder.h"
#include "mz3_revision.h"
#include "MZ3Parser.h"
#include "MouseGestureManager.h"
#include "MZ3FileCacheManager.h"
#include "ChooseClientTypeDlg.h"

/* lua support exp.
extern "C" {
#include "lua/src/lua.h"
#include "lua/src/lualib.h"
#include "lua/src/lauxlib.h"
}
*/

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMZ3App

BEGIN_MESSAGE_MAP(CMZ3App, CWinApp)
	ON_COMMAND(ID_APP_ABOUT, &CMZ3App::OnAppAbout)
END_MESSAGE_MAP()

/* lua support exp.
// the Lua object
lua_State* g_luaState = NULL;

int lua_mz3_trace(lua_State *lua)
{
	CString s( lua_tostring(lua, -1) );

	MZ3_TRACE(s);

	return 0;
}
*/

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
	, m_pMouseGestureManager(NULL)
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
	if (m_pMouseGestureManager != NULL) {
		delete m_pMouseGestureManager;
		m_pMouseGestureManager = NULL;
	}
}

// �B��� CMZ3App �I�u�W�F�N�g�ł��B
CMZ3App theApp;

// CMZ3App ������

BOOL CMZ3App::InitInstance()
{
#ifndef WINCE 
	::GdiplusStartup(&gdiToken, &gdiSI, NULL);
#endif
	// �v���b�g�t�H�[������
	InitPlatformFlags();

	// �𑜓x/DPI����
	InitResolutionFlags();

	// �A�N�Z�X��ʖ��̐U�镑����`��������
	m_accessTypeInfo.init();

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
	MZ3_TRACE(L" sizeof MZ3Data : %d bytes\n", sizeof(MZ3Data));

/* lua support exp.
	// Lua �̏�����
	g_luaState = lua_open();

	lua_register(g_luaState, "mz3_trace", lua_mz3_trace);
	luaL_dostring(g_luaState, "mz3_trace('xxx\\n');");

	lua_close(g_luaState);
*/
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
	m_readlog.Load( m_filepath.logfile );
	MZ3LOGGER_DEBUG( L"ini �t�@�C���ǂݍ��݊���" );

	// �X�L���֘A�̏�����
	LoadSkinSetting();

	/*
	// MSXML ���p�̂��߂̃R�[�h�A��������������c���Ă�����
	{
		CMixiData rss_data;
		rss_data.SetAccessType(ACCESS_RSS_READER_FEED);
		rss_data.SetURL(L"http://b.hatena.ne.jp/hotentry?mode=rss");
#ifdef WINCE
		CString rss_data_path = L"\\hatena.rss.utf8";
#else
		CString rss_data_path = L"M:\\hatena.rss.utf8";
#endif
		MZ3_TRACE( L"rss_file_path : /%s/\n", rss_data_path );

		util::StopWatch sw;
		{
			CoInitializeEx(NULL, COINIT_MULTITHREADED);

			CComPtr<IXMLDOMDocument> iXMLDoc;
			CComPtr<IXMLDOMNodeList> iXMLList;

			VARIANT_BOOL bSuccess=false;

			if(S_OK == iXMLDoc.CoCreateInstance( __uuidof(DOMDocument) ) ){

				sw.start();

				for (int loop=0; loop<10; loop++) {
					// �����Ǎ�
					iXMLDoc->put_async(VARIANT_FALSE);

					// load XML
					CComVariant         varFile = rss_data_path;
					iXMLDoc->load(varFile, &bSuccess);
					if (bSuccess) {
						iXMLDoc->selectNodes(_bstr_t("rdf:RDF/item/title"), &iXMLList);

						if (iXMLList != NULL) {

							_bstr_t _bstGet;
							for (int i=0;; i++) {
								CComPtr<IXMLDOMElement> iXMLInfo;

								iXMLList->get_item(i, (IXMLDOMNode**)&iXMLInfo);

								if (NULL != iXMLInfo) {
									iXMLInfo->get_text( &_bstGet.GetBSTR() );

									MZ3_TRACE( L" %d : /%s/\n", i, _bstGet.GetBSTR() );
								} else {
									break;
								}
							}

						}
					} else {
						MessageBox( NULL, L"load failed", L"", MB_OK );
					}
				}

				CString msg;
				msg.Format( L"elapsed time : %.2f [sec]\n", sw.stop() / 1000.0 );
				MessageBox(NULL, msg, L"", MB_OK);

			} else {
				MessageBox( NULL, L"object generate failed", L"", MB_OK );
			}
		}

		{
			CString rss_data_path = util::MakeLogfilePath(rss_data);

			sw.start();

			CHtmlArray html_;
			html_.Load( rss_data_path );

			// html_ �̕�����
			std::vector<TCHAR> text;
			html_.TranslateToVectorBuffer( text );

			for (int loop=0; loop<10; loop++) {

				// XML ���
				xml2stl::Container root;
				if (!xml2stl::SimpleXmlParser::loadFromText( root, text )) {
					MZ3LOGGER_ERROR( L"XML ��͎��s" );
				} else {
					// rdf:RDF/item �ɑ΂��鏈��
					try {
						const xml2stl::Node& rdf = root.getNode(L"rdf:RDF");
						
						int counter = 0;
						for (unsigned int i=0; i<rdf.getChildrenCount(); i++) {
							const xml2stl::Node& item = rdf.getNode(i);
							if (item.getName() != L"item") {
								continue;
							}

							// description, title �̎擾
							CString title = item.getNode(L"title").getTextAll().c_str();

							MZ3_TRACE( L" %d : /%s/\n", counter, title );
							counter++;
						}

					} catch (xml2stl::NodeNotFoundException& e) {
						MZ3LOGGER_ERROR( util::FormatString( L"node not found... : %s", e.getMessage().c_str()) );
					}
				}
			}

			CString msg;
			msg.Format( L"elapsed time : %.2f [sec]\n", sw.stop() / 1000.0 );
			MessageBox(NULL, msg, L"", MB_OK);
		}
	}
	*/

	// �ڍ׉�ʂ̃N���X�o�^
#ifdef USE_RAN2
	Ran2View::RegisterWndClass(AfxGetInstanceHandle());
#endif

	// �摜�L���b�V���̏�����
	m_imageCache.Create();

	// �g�b�v�y�[�W�p�̏�����
	MZ3LOGGER_INFO( L"�^�u�ݒ�t�@�C���ǂݍ��݊J�n" );
	bool bExistGroupFile = util::ExistFile( m_filepath.groupfile );
	if( Mz3GroupDataReader::load( m_accessTypeInfo, m_root, m_filepath.groupfile ) ) {
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
		// �N���C�A���g�I����ʂ�\��
		CChooseClientTypeDlg dlg;
		if (dlg.DoModal()==IDOK) {
			// �ݒ肳�ꂽ���e�ŏ�����
			m_root.initForTopPage(m_accessTypeInfo, dlg.m_initType);
		}

		theApp.SaveGroupData();
	}

	// �ڍs����
	switch (m_optionMng.m_StartupTransitionDoneType) {
	case option::Option::STARTUP_TRANSITION_DONE_TYPE_NONE:
		{
			// �N���C�A���g�I����ʂ�\��
			CChooseClientTypeDlg dlg;
			if (dlg.DoModal()==IDOK) {
				// �ݒ肳�ꂽ���e�ŏ�����
				m_root.initForTopPage(m_accessTypeInfo, dlg.m_initType);
			}

			// ����ȍ~�͕\�����Ȃ�
			m_optionMng.m_StartupTransitionDoneType = option::Option::STARTUP_TRANSITION_DONE_TYPE_FONT_SIZE_SCALED;
			theApp.SaveGroupData();
		}
		break;

	case option::Option::STARTUP_TRANSITION_DONE_TYPE_TWITTER_MODE_ADDED:
		// ���s�ς�
		{
			// �t�H���g�T�C�Y�̈ڍs����
			MZ3LOGGER_DEBUG(util::FormatString(L"�t�H���g�T�C�Y�̈ڍs�����Fm_fontHeight : %d", m_optionMng.m_fontHeight));

			// px ���Z�l�� pt ���Z�l�ɕϊ�����
			int iDPI = theApp.GetDPI();
			m_optionMng.m_fontHeight       = ::MulDiv(m_optionMng.m_fontHeight       , 72, iDPI);
			m_optionMng.m_fontHeightBig    = ::MulDiv(m_optionMng.m_fontHeightBig    , 72, iDPI);
			m_optionMng.m_fontHeightMedium = ::MulDiv(m_optionMng.m_fontHeightMedium , 72, iDPI);
			m_optionMng.m_fontHeightSmall  = ::MulDiv(m_optionMng.m_fontHeightSmall  , 72, iDPI);

			MZ3LOGGER_DEBUG(util::FormatString(L"�ڍs��Fm_fontHeight : %d", m_optionMng.m_fontHeight));

			// ����ȍ~�͏������Ȃ�
			m_optionMng.m_StartupTransitionDoneType = option::Option::STARTUP_TRANSITION_DONE_TYPE_FONT_SIZE_SCALED;
			theApp.SaveGroupData();
		}
		break;

	case option::Option::STARTUP_TRANSITION_DONE_TYPE_FONT_SIZE_SCALED:
	default:
		// �ŐV�̂��߈ڍs�����Ȃ�
		break;
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

	m_pMouseGestureManager = new MouseGestureManager;

	m_pMainView		= (CMZ3View*)((CFrameWnd*)m_pMainWnd)->GetActiveView();
	m_pReportView	= new CReportView;
	m_pWriteView	= new CWriteView;
	m_pDownloadView	= new CDownloadView;

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

	// �_�E�����[�h�r���[�̏�����
	((CView*)m_pDownloadView)->Create(NULL, MZ3_APP_NAME _T(" DownloadView"), WS_CHILD, rect,
		m_pMainWnd, viewID, &newContext);
	m_pDownloadView->OnInitialUpdate();

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
	if ((wcslen(m_loginMng.GetEmail()) == 0 && wcslen(m_loginMng.GetPassword()) == 0) &&
		(wcslen(m_loginMng.GetTwitterId()) == 0 && wcslen(m_loginMng.GetTwitterPassword()) == 0)
		)
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

//	if( m_bSmartphone ) {
#ifdef WINCE
	// ���L�̗��R���炱���ōĕ`��
	// - Smartphone/Standard ���Ə����`�悳��Ȃ����ۂ�
	// - �����J�������[�h�̃T�C�Y�������s���Ȃ�
	((CMainFrame*)m_pMainWnd)->ChangeAllViewFont();
#endif

	// �G������`�t�@�C���̃��[�h
	{
		inifile::IniFile emojifile;
		if (emojifile.Load( m_filepath.emojifile )) {
			LPCSTR szSection = "mixi";
			int start = atoi(emojifile.GetValue( "start", szSection ).c_str());
			int end   = atoi(emojifile.GetValue( "end", szSection ).c_str());
			for (int i=start; i<=end; i++) {
				std::string s = emojifile.GetValue( (LPCSTR)util::int2str_a(i), szSection );
				if (!s.empty()) {
					std::vector<std::string> values;
					if (!util::split_by_comma( values, s ) || values.size() < 3) {
						// ���[�h�G���[
						CString msg;
						msg.Format( L"load emojifile error, id at %d", i );
						MZ3LOGGER_ERROR( msg );
					}
					m_emoji.push_back( 
						EmojiMap(util::my_mbstowcs(values[0]).c_str(), 
								 util::my_mbstowcs(values[1]).c_str(), 
								 util::my_mbstowcs(values[2]).c_str()) );
				}
			}
		}
	}
	// ���_�E�����[�h�t�@�C��������΃_�E�����[�h�}�l�[�W���ɓo�^�A�N��
	if (theApp.m_optionMng.m_bAutoDownloadEmojiImageFiles) {
		bool bHasNoDownloadedEmojiFile = false;
		for (size_t i=0; i<m_emoji.size(); i++) {
			EmojiMap& emoji = m_emoji[i];
			CString path = util::MakeImageLogfilePathFromUrl( emoji.url );
			if (!util::ExistFile(path)) {
				DownloadItem item( emoji.url, emoji.text, path, true );
				m_pDownloadView->AppendDownloadItem( item );
				bHasNoDownloadedEmojiFile = true;
			}
		}
		if (bHasNoDownloadedEmojiFile) {
			CString msg;
			msg += "�G�����t�@�C�����_�E�����[�h���܂����H";
			if (MessageBox( m_pMainView->GetSafeHwnd(), msg, MZ3_APP_NAME, MB_YESNO ) == IDYES) {
				ChangeView( m_pDownloadView );
			} else {
				msg = L"����N�����ɊG�����t�@�C�����_�E�����[�h���܂����H";
				if (MessageBox( m_pMainView->GetSafeHwnd(), msg, MZ3_APP_NAME, MB_YESNO ) != IDYES) {
					theApp.m_optionMng.m_bAutoDownloadEmojiImageFiles = false;
					theApp.m_optionMng.Save();
				}
			}
		}
	}

	//--- �e�X�g�p�R�[�h
#ifdef DEBUG
	// 
	// encode_euc TEST
	//
	if (false) {
		CString src_text = L"";
		CString valid_text = L"";
		CString url_encoded_text = L"";

		// �A���t�@�x�b�gTEST
		url_encoded_text = URLEncoder::encode_euc(L"a");
		ASSERT(url_encoded_text==L"a");

		// 2�o�C�g����TEST
		url_encoded_text = URLEncoder::encode_euc(L"��");
		ASSERT(url_encoded_text==L"%A4%A2");

		// �L��TEST
		url_encoded_text = URLEncoder::encode_euc(L"!\"#$%&'()-=^~\\|@[]{}+*};:],./<>?_");
		ASSERT(url_encoded_text==L"%21%22%23%24%25%26%27%28%29-%3D%5E%7E%5C%7C%40%5B%5D%7B%7D%2B%2A%7D%3B%3A%5D%2C.%2F%3C%3E%3F_");

		// 200000����TEST
		src_text = L"";
		valid_text = L"";
		for (int i=0; i<20000; i++) {
			src_text += L"��������������������";
			valid_text += L"%A4%A2%A4%A4%A4%A6%A4%A8%A4%AA%A4%AB%A4%AD%A4%AF%A4%B1%A4%B3";
		}
		url_encoded_text = URLEncoder::encode_euc(src_text);
		ASSERT(url_encoded_text==valid_text);
	}

	// 
	// encode_utf8 TEST
	//
	if (0) {
		CString src_text = L"";
		CStringA valid_text = "";
		CStringA url_encoded_text = "";

		// �A���t�@�x�b�gTEST
		url_encoded_text = URLEncoder::encode_utf8(L"a");
		ASSERT(url_encoded_text=="a");

		// 2�o�C�g����TEST
		url_encoded_text = URLEncoder::encode_utf8(L"��");
		ASSERT(url_encoded_text=="%E3%81%82");

		// �L��TEST
		url_encoded_text = URLEncoder::encode_utf8(L"!\"#$%&'()-=^~\\|@[]{}+*};:],./<>?_");
		ASSERT(url_encoded_text=="%21%22%23%24%25%26%27%28%29-%3D%5E%7E%5C%7C%40%5B%5D%7B%7D%2B%2A%7D%3B%3A%5D%2C.%2F%3C%3E%3F_");

		// 200000����TEST
		src_text = L"";
		valid_text = L"";
		for (int i=0; i<20000; i++) {
			src_text += L"��������������������";
			valid_text += "%E3%81%82%E3%81%84%E3%81%86%E3%81%88%E3%81%8A%E3%81%8B%E3%81%8D%E3%81%8F%E3%81%91%E3%81%93";
		}
		url_encoded_text = URLEncoder::encode_utf8(src_text);
		ASSERT(url_encoded_text==valid_text);
	}

#endif

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
#ifndef WINCE 
	::GdiplusShutdown(gdiToken);
#endif
//	CString msg;
//	msg.Format( L"���O�t�@�C����\n%s\n�ɕۑ����܂�", logfile );
//	MessageBox( NULL, msg, 0, MB_OK );

	// �ڍ׉�ʂ̃N���X�o�^����
#ifdef USE_RAN2
	Ran2View::UnregisterWndClass(AfxGetInstanceHandle());
#endif

	MZ3LOGGER_DEBUG( MZ3_APP_NAME L" �I�������J�n" );

	// ���O�t�@�C���̕ۑ�
	if( m_readlog.Save( m_filepath.logfile ) == false ) {
		MessageBox( NULL, L"���O�t�@�C���ۑ����s", m_filepath.logfile, MB_OK );
	}

	// �I�v�V�����̕ۑ�
	m_optionMng.Save();

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
	// ���݂̃r���[�̃E�B���h�EID�̕ύX
	CView* pActiveView = ((CFrameWnd*)m_pMainWnd)->GetActiveView();
	UINT tempId = ::GetWindowLong( pActiveView->m_hWnd, GWL_ID );
	::SetWindowLong( pActiveView->m_hWnd, GWL_ID, ::GetWindowLong(pNewView->m_hWnd, GWL_ID) );
	::SetWindowLong( pNewView->m_hWnd,    GWL_ID, tempId );

	//--- ���݂̃r���[
	// ��\����
	pActiveView->ShowWindow( SW_HIDE );

	// MZ3 ��\�������ʒm
	::SendMessage( pActiveView->m_hWnd, WM_MZ3_HIDE_VIEW, NULL, NULL );

	//--- �؂�ւ���r���[
	pNewView->ShowWindow( SW_SHOW );
	((CFrameWnd*)m_pMainWnd)->SetActiveView( pNewView );
	((CFrameWnd*)m_pMainWnd)->RecalcLayout( TRUE );
	m_pMainWnd->UpdateWindow();

	// MZ3 �؂�ւ������ʒm
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
/*
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
*/
/**
 * ���[�U�ݒ�iE-Mail�A�h���X�A�p�X���[�h�j��p���āA�������O�C��URL�𐶐�����
 */
/*
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
*/

/**
 * mixi �̃��O�C���p�ʐM���J�n����
 */
void CMZ3App::StartMixiLoginAccess(HWND hwnd, CMixiData* data)
{
	//--- �������烍�O�C���Ή�
	static CPostData post_data;

	CString url = L"http://mixi.jp/login.pl";
	LPCTSTR nextUrl = L"/home.pl";

	post_data.ClearPostBody();
	post_data.AppendPostBody(L"email=");
	post_data.AppendPostBody(URLEncoder::encode_euc( theApp.m_loginMng.GetEmail() ));
	post_data.AppendPostBody(L"&");
	post_data.AppendPostBody(L"password=");
	post_data.AppendPostBody(URLEncoder::encode_euc( theApp.m_loginMng.GetPassword() ));
	post_data.AppendPostBody(L"&");
	post_data.AppendPostBody(L"next_url=");
	post_data.AppendPostBody(URLEncoder::encode_euc( nextUrl ));

	// Content-Type: application/x-www-form-urlencoded
	post_data.SetContentType( CONTENT_TYPE_FORM_URLENCODED );

	// �������̃��b�Z�[�W��ݒ肷��
	post_data.SetSuccessMessage( WM_MZ3_GET_END );

	data->SetURL( url );
	data->SetAccessType( ACCESS_LOGIN );
	theApp.m_accessType = ACCESS_LOGIN;

	// �ʐM�J�n
	CString refUrl = L"";
	theApp.m_inet.Initialize( hwnd, data );
	theApp.m_inet.DoPost(
		url, 
		refUrl, 
		CInetAccess::FILE_HTML, 
		&post_data);
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
#ifdef WINCE
		int iDPI = ::GetDeviceCaps(NULL, LOGPIXELSY);
		BYTE fontQuality = NONANTIALIASED_QUALITY;
		if (iDPI==192) {
			// VGA(��RealVGA) �Ȃ�ClearType�w��
			fontQuality = CLEARTYPE_QUALITY;
		}
#else
		// DPI �l�̎擾
		int iDPI = theApp.GetDPI();

		BYTE fontQuality = DEFAULT_QUALITY;
#endif
		int newHeight = - theApp.pt2px(fontHeight);	// �����Ƃ��邱�Ƃ� pixel �l�w��Ƃ���

		MZ3LOGGER_DEBUG( 
			util::FormatString(L"CMZ3App::MakeNewFont(), dpi[%d], fontHeight[%d], newHeight[%d]",
				iDPI, fontHeight, newHeight) );

		theApp.m_font.Detach();
		theApp.m_font.CreateFont( 
			newHeight,					// nHeight
			0,							// nWidth
			0,							// nEscapement
			0,							// nOrientation
			FW_MEDIUM,					// nWeight
			FALSE,						// bItalic
			FALSE,						// bUnderline
			0,							// cStrikeOut
			DEFAULT_CHARSET,			// nCharSet
			OUT_DEFAULT_PRECIS,			// nOutPrecision
			CLIP_DEFAULT_PRECIS,		// nClipPrecision
			fontQuality,				// nQuality
			DEFAULT_PITCH | FF_SWISS,	// nPitchAndFamily
			fontFace);					// lpszFacename
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
	// MZ3 �{�̂̃��O�t�@�C��
	mz3logfile.Format( L"%s\\mz3log.txt", theApp.GetAppDirPath() );

	// HTML �p�ꎞ�t�@�C��(SJIS)
//	TCHAR temppath[256];
//	::GetTempPath(256, temppath);
//	temphtml.Format( _T("%s\\mz3s.dat"), temppath );
	temphtml.Format( _T("%s\\mz3s.dat"), theApp.GetAppDirPath() );

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

	// �G������`�t�@�C�� �̃p�X
	emojifile.Format(_T("%s\\%s"), theApp.GetAppDirPath(), _T("emoji.ini"));

	// �X�L���t�H���_�̃p�X
	skinFolder.Format( L"%s\\skin", theApp.GetAppDirPath() );
}

/// ���O�֘A�t�@�C���p�X�̏�����
void CMZ3App::FilePath::init_logpath()
{
	//--- �t�H���_
	deleteTargetFolders.RemoveAll();

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
	// �N���[���A�b�v�Ώۃt�H���_�ɒǉ�
	deleteTargetFolders.Add(downloadFolder);

	// �_�E�����[�h�ς݉摜�t�@�C���p�t�H���_
	imageFolder.Format(_T("%s\\image"), logFolder );
	CreateDirectory( imageFolder, NULL/*always null*/ );

	// ���L�̃��O�p�t�H���_�̃p�X
	deleteTargetFolders.Add(logFolder + L"\\diary");

	// �R�~���j�e�B�������݂̃��O�p�t�H���_�̃p�X
	deleteTargetFolders.Add(logFolder + L"\\bbs");

	// �A���P�[�g�̃��O�p�t�H���_�̃p�X
	deleteTargetFolders.Add(logFolder + L"\\enquete");

	// �C�x���g�̃��O�p�t�H���_�̃p�X
	deleteTargetFolders.Add(logFolder + L"\\event");

	// �v���t�B�[���̃��O�p�t�H���_�̃p�X
	deleteTargetFolders.Add(logFolder + L"\\profile");

	// ���b�Z�[�W�̃��O�p�t�H���_�̃p�X
	deleteTargetFolders.Add(logFolder + L"\\message");

	// �j���[�X�̃��O�p�t�H���_�̃p�X
	deleteTargetFolders.Add(logFolder + L"\\news");

	// Twitter�̃��O�p�t�H���_�̃p�X
	deleteTargetFolders.Add(logFolder + L"\\twitter");
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

	case SR_QVGA:
	default:
		// VGA �ȊO
		if (m_bSmartphone) {
			// Smartphone/Standard Edition ��
			// X02HT �ł̌��،��ʂ��󂯂ĉ��L�̒l��ݒ�B
			return fontHeight +6;
		} else {
			return fontHeight +6;
//			return fontHeight -4;
		}
	}
#else
	return fontHeight +6*2;
#endif
}

/**
 * �^�u�̍������t�H���g�̍������x�[�X�Ɍv�Z����
 */
int CMZ3App::GetTabHeight( int fontHeight )
{
#ifdef WINCE
	switch( theApp.GetDisplayMode() ) {
	case SR_VGA:
		if( theApp.GetDPI() > 96 ) {
			// VGA ����RealVGA��
			return fontHeight + 6*2;
		}else{
			// VGA ����RealVGA��
			return fontHeight + 8;
		}
		break;

	case SR_QVGA:
	default:
		// VGA �ȊO
		if( theApp.m_bSmartphone ) {
			// Smartphone/Standard Edition ��
			return fontHeight +8;
		} else {
			return fontHeight +8;
//			return fontHeight -2;
		}
	}
#else
	// for win32
	return fontHeight + 6*2;
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

/**
 * �O���[�v��`�t�@�C���̕ۑ�
 */
bool CMZ3App::SaveGroupData(void)
{
	return Mz3GroupDataWriter::save( m_accessTypeInfo, m_root, m_filepath.groupfile );
}

/**
 * ���O�A�E�g��Ԃ��ǂ����𔻒肷��B�f�[�^�擾����ɌĂяo�����ƁB
 */
bool CMZ3App::IsMixiLogout( ACCESS_TYPE aType )
{
	if (util::IsMixiAccessType(aType)) {
		if (mixi::MZ3ParserBase::IsLogout(theApp.m_filepath.temphtml) ) {
			return true;
		} else if (aType != ACCESS_MAIN && wcslen(theApp.m_loginMng.GetOwnerID())==0) {
			// �I�[�i�[ID���擾�̏ꍇ�����O�A�E�g�Ƃ݂Ȃ��B
			return true;
		}
	}
	return false;
}

/**
 * �Â��L���b�V���t�@�C���̍폜
 */
bool CMZ3App::DeleteOldCacheFiles(void)
{
	MZ3LOGGER_INFO( L"�Â��L���b�V���t�@�C���̍폜�J�n" );
	int nDeleted = 0;	// �폜�ς݃t�@�C����
	int nTargetFileLastWriteDaysBack = 30*2;	// 2�����ȏ�O�̃t�@�C�����폜�ΏۂƂ���
	MZ3FileCacheManager cacheManager;
	cacheManager.DeleteFiles( m_filepath.deleteTargetFolders, &nDeleted, nTargetFileLastWriteDaysBack );
	MZ3LOGGER_INFO( util::FormatString(L"�Â��L���b�V���t�@�C�����폜���܂����F%d��", nDeleted) );

	return true;
}

/**
 * pt �P�ʂ̒l�����݂� DPI �ɂ��킹�� px �P�ʂɕϊ�����
 */
int CMZ3App::pt2px(int pt)
{
	return ::MulDiv(pt, m_dpi, 72);
}

/**
 * image �����T�C�Y���A�摜�L���b�V���ɒǉ�����B
 */
int CMZ3App::AddImageToImageCache(CWnd* pWnd, CMZ3BackgroundImage& image, const CString& strImagePath)
{
	// 16x16, 32x32, 48x48 �Ƀ��T�C�Y����B
	CMZ3BackgroundImage image16(L""), image32(L""), image48(L"");
	util::MakeResizedImage( pWnd, image16, image, 16, 16 );
	util::MakeResizedImage( pWnd, image32, image, 32, 32 );
	util::MakeResizedImage( pWnd, image48, image, 48, 48 );

	// �r�b�g�}�b�v�̒ǉ�
	CBitmap bm16, bm32, bm48;
	bm16.Attach( image16.getHandle() );
	bm32.Attach( image32.getHandle() );
	bm48.Attach( image48.getHandle() );

	return theApp.m_imageCache.Add( &bm16, &bm32, &bm48, (CBitmap*)NULL, strImagePath );
}
