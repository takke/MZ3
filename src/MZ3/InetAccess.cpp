// InetAccess.cpp : �����t�@�C��
//

#include "stdafx.h"
#include "MZ3.h"
#include "InetAccess.h"
#include "HtmlArray.h"

#include "MyRegex.h"
#include "wininet.h"
#include "kfm.h"
#include "util.h"

#pragma comment(lib, "wininet.lib")
#pragma comment( lib, "cellcore.lib" )

#include <initguid.h>
#include <connmgr.h>
#include <connmgr_proxy.h>
#define ARRAYSIZE( a ) (sizeof( a )/sizeof(a[0]) )

#define SZ_REG_CONNECTION_ROOT TEXT("System\\State\\Connections")
#define SZ_REG_CONNECTION_DBVOL TEXT("Count") 

// CInetAccess

/**
 * ��ʉ����̏��̈�ɒʐM�ʂɊւ��郁�b�Z�[�W��\������
 *
 * @param dwLoaded      �ǂݍ��ݍς݃f�[�^��
 * @param dwContentLen  ���f�[�^�ʁB���̂Ƃ��͎Q�l�l�Ƃ݂Ȃ��B0 �̂Ƃ��͑��f�[�^�ʕs���Ƃ݂Ȃ��B
 * @param dwElapsedMsec �o�ߎ���[msec]
 */
inline void MySetTrafficInfo( HWND hWnd, LONG dwLoaded, LONG dwContentLen, DWORD dwElapsedMsec )
{
	// ���b�Z�[�W����
	CString msg;
	DWORD dwLength = 0;
	if( dwContentLen > 0 ) {
		// �i�����v�Z
		dwLength = dwContentLen;

		double rate = dwLoaded/(double)dwLength;
		rate = min(rate,1.0);	// [0,1] �ɐ��K��
		msg.Format( _T("(%.0f%%) %d.%d/%d.%dKB"), 
			rate*100.0, 
			dwLoaded / 1024,
			dwLoaded % 1024 / 103,
			dwLength / 1024,
			dwLength % 1024 / 103 );

	}else if( dwContentLen < 0 ) {
		// �O���M���̒l�i�Q�l�l�j�Ői�����Z�o
		dwLength = -dwContentLen;

		double rate = dwLoaded/(double)dwLength;
		rate = min(rate,1.0);	// [0,1] �ɐ��K��
		msg.Format( _T("(%.0f%%) %d.%d/%d.%d+KB"), 
			rate*100.0, 
			dwLoaded / 1024,
			dwLoaded % 1024 / 103,
			dwLength / 1024,
			dwLength % 1024 / 103 );
	}else{
		// ��M�T�C�Y�A���Q�l�l���Ȃ����߁A��M�ς݃T�C�Y�̂ݕ\��
		msg.Format( _T("%d.%dKB"), dwLoaded / 1024, dwLoaded % 1024 / 103 );

		dwLength = 0;
	}

	// �����ɗ\�z�������Ԃ�\������
	// - dwLength �� 0 �̏ꍇ�̓T�C�Y�s���̂��ߕ\�����Ȃ��B
	// - dwLoaded �� 0 �̏ꍇ�� 0 ���Z����̂��ߕ\�����Ȃ��B
	// - �o�ߎ��Ԃ� N [msec] �����̏ꍇ�́A��񂪐M���ł��Ȃ����ߕ\�����Ȃ��B
	if( dwLength != 0 && dwLoaded != 0 && dwElapsedMsec > 200 ) {
		// bps = loaded / sec
		// rest = len - loaded
		// sec = msec / 1000
		// �c�莞��[sec] = rest / bps
		//               = rest / (loaded/sec)
		//               = rest * sec / loaded
		DWORD rest     = dwLength - dwLoaded;
		if( rest < 0 ) {
			// ���ɗ\�z�]���ʂ𒴂��Ă���B
			// �c�� 0 �Ƃ݂Ȃ��B
			rest = 0;
		}
		// �S���I�� 0 [sec] =�I���Ǝv���邽�߁A�u�����_�ȉ��؂�グ�v���Ă����B
		DWORD restSec  = (DWORD)ceil( rest * (double)dwElapsedMsec / 1000 / dwLoaded );

		// �ő�� 3600 sec �܂łƂ���
		if( restSec > 3600 ) restSec = 3600;
		if( restSec < 0    ) restSec = 0;
		msg.AppendFormat( L" [%dsec]", restSec );
	}

	// ���b�Z�[�W�ݒ�
	util::MySetInformationText( hWnd, msg );

	// �v���O���X�o�[�ݒ���s��
	::SendMessage( hWnd, WM_MZ3_ACCESS_LOADED, (WPARAM)dwLoaded, (LPARAM)dwLength );
}

/**
 * �f�X�g���N�^
 */
CInetAccess::~CInetAccess()
{
	// m_hRequest, m_hConnection �����B
	CloseInternetHandles();
}


// CInetAccess �����o�֐�

/**
 * �C���^�[�l�b�g�̐ڑ����J��
 * 
 * @note ���O�C������O�ɂ��Ȃ炸�s������
 */
bool CInetAccess::Open()
{
	// �v���L�V������
	CString proxy = L"";

	if( theApp.m_optionMng.IsUseAutoConnection() ) {
		// �C���^�[�l�b�g�����ڑ��B
		// �ڑ�������������_�C�A���A�b�v�������s���B
		// �܂��A�O���[�o���v���L�V�̐ڑ�����擾����B
		SP_EstablishInetConnProc( proxy );
	}

	if (theApp.m_optionMng.IsUseProxy() ) {
		// �v���L�V������𐶐�����

		if( theApp.m_optionMng.IsUseGlobalProxy() ) {
			// �u�[���̐ݒ���g�p����v�I�v�V�������L���B
			// SP_EstablishInetConnProc �ɂĎ擾�ς݂ł��邽�߁A�����ł͉������Ȃ��B
		}else{
			// �u�蓮�ݒ�v�I�v�V�������L���B
			// ���[�U�w��̃T�[�o�ƃ|�[�g�ԍ�����v���L�V������𐶐�����
			proxy.Format(_T("%s:%d"),
				theApp.m_optionMng.GetProxyServer(),
				theApp.m_optionMng.GetProxyPort());
		}
	}

	try {
		// ���ɊJ����Ă���Ε���
		CloseInternetHandles();

		// �ڑ����J��
		m_hInternet = InternetOpen(_T("MZ3"), INTERNET_OPEN_TYPE_PROXY, proxy, NULL, 0);
	} catch (CException &) {
		m_hInternet = NULL;

		// �G���[����
		m_strErrorMsg = L"�G���[����";
		MessageBox( NULL, m_strErrorMsg, 0, MB_OK );
		MZ3LOGGER_ERROR( m_strErrorMsg );
		return false;
	}
	if( m_hInternet == NULL ) {
		m_strErrorMsg.Format( L"�G���[���� (%d)", GetLastError() );
		MessageBox( NULL, m_strErrorMsg, 0, MB_OK );
		MZ3LOGGER_ERROR( m_strErrorMsg );
		return false;
	}

/*
	util::MySetInformationText( m_hwnd, _T("������������") );
*/
	return true;
}

/**
 * �ʐM�̏���������
 */
void CInetAccess::Initialize( HWND hwnd, void* object )
{
	m_bAccessing	= false;
	m_hConnection	= NULL;
	m_hRequest		= NULL;

	m_hwnd			= hwnd;
	m_object		= object;
	m_nRedirect		= 0;
}

/**
 * �C���^�[�l�b�g�A�N�Z�X
 * 
 * �w���URI�ɃA�N�Z�X���āA�t�@�C�����擾���ASJIS�ɕϊ����ăt�@�C���ɕۑ�����B
 * �t�@�C���͑S�� theApp.m_filepath.temphtml �ɕۑ�����B
 * 
 * @param uri	[in] URI
 * @param ref	[in] ���t�@��
 *
 * @return ��� TRUE ��Ԃ��B
 */
BOOL CInetAccess::DoGet( LPCTSTR uri, LPCTSTR ref, FILE_TYPE type )
{
	// ���f�t���O��������
	m_abort		= FALSE;

	m_uri		= uri;
	m_ref		= ref;
	m_fileType	= type;
	m_nRedirect = 0;

	// �X���b�h�J�n
	m_pThreadMain = AfxBeginThread(ExecGet_Thread, this);

	return TRUE;
}

/**
 * �X���b�h���p
 */
unsigned int CInetAccess::ExecGet_Thread(LPVOID This)
{
	// GET ����
	CInetAccess* inet = (CInetAccess*)This;
	inet->m_bAccessing = true;
	int msg = inet->ExecSendRecv( EXEC_SENDRECV_TYPE_GET );
	::PostMessage( inet->m_hwnd, msg, NULL, (LPARAM)inet->m_object );
	inet->m_bAccessing = false;

	return 0;
}
/**
 * �f�[�^���|�X�g����
 */
BOOL CInetAccess::DoPost( LPCTSTR uri, LPCTSTR ref, FILE_TYPE type, CPostData* postData )
{
	// ���f�t���O��������
	m_abort		= FALSE;

	m_uri		= uri;
	m_ref		= ref;
	m_fileType	= type;
	m_postData	= postData;
	m_nRedirect = 0;

	// �X���b�h�J�n
	m_pThreadMain = AfxBeginThread(ExecPost_Thread, this);

	return TRUE;
}

/**
 * �X���b�h���p
 */
unsigned int CInetAccess::ExecPost_Thread(LPVOID This)
{
	// POST ����
	CInetAccess* inet = (CInetAccess*)This;
	inet->m_bAccessing = true;
	int msg = inet->ExecSendRecv( EXEC_SENDRECV_TYPE_POST );
	::PostMessage( inet->m_hwnd, msg, NULL, (LPARAM)inet->m_object );
	inet->m_bAccessing = false;

	return 0;
}

/**
 * out_buf �̖����� pData �� dwSize �����ǉ�����
 */
inline bool my_append_buf( kfm::kf_buf_type& out_buf, char* pData, DWORD dwSize )
{
	if( dwSize==0 ) {
		return true;
	}

	// �g���O�̃T�C�Y
	size_t base_size = out_buf.size();

	// �g��
	out_buf.resize( base_size+dwSize );

	// �i�[
	memcpy( &out_buf[base_size], pData, dwSize );

	return true;
}

/**
 * �����ڑ��B
 *
 * �ڑ�������Ă��Ȃ�������_�C�A���A�b�v�������s���B
 * �܂��A�R�l�N�V�����}�l�[�W������v���L�V�����擾����B
 */
HRESULT WINAPI CInetAccess::SP_EstablishInetConnProc( CString& proxy )
{
	CONNMGR_CONNECTIONINFO ci = {0};
	PROXY_CONFIG pcProxy = {0};
	DWORD dwStatus = 0;
	DWORD dwIndex = 0;
	HRESULT hr = S_OK;
	HANDLE hConnection = NULL;
	DWORD dwTimeOut = 120000;	//�^�C���A�E�g�l

	// �R�l�N�V�����}�l�[�W���̏�����
	ci.cbSize     = sizeof(CONNMGR_CONNECTIONINFO);
	ci.dwParams   = CONNMGR_PARAM_GUIDDESTNET;
	ci.dwFlags    = CONNMGR_FLAG_PROXY_HTTP;
	ci.dwPriority = CONNMGR_PRIORITY_USERINTERACTIVE;

	// URL�`�F�b�N
	hr = ConnMgrMapURL(L"http://mixi.jp", &(ci.guidDestNet), &dwIndex);

	// �ڑ�����
	hr = ConnMgrEstablishConnectionSync(&ci, &hConnection, dwTimeOut, &dwStatus);

	// �R�l�N�V�����}�l�[�W������v���N�V�����擾����
	hr = ConnMgrProviderMessage( hConnection, &IID_ConnPrv_IProxyExtension, NULL, 0, 0, (PBYTE)&pcProxy, sizeof(pcProxy)); 
	if (S_OK == hr)
	{
		// �擾OK�B
		// proxy �Ƀv���L�V����ݒ肷��
		proxy = pcProxy.szProxyServer;
	}
	else if (E_NOINTERFACE == hr)
	{
		// �v���N�V��񂪂Ȃ��ꍇ�̓_�C���N�g�ڑ�
		proxy = L"";
		hr = S_OK;
	}

	if (hConnection)
	{
		ConnMgrReleaseConnection(hConnection, TRUE);
	}

	return hr;
}

/**
 * �l�b�g���[�N�̐ڑ���Ԃ��m�F����B
 *
 * @return �ڑ��Ftrue�A�ؒf�Ffalse
 */
bool CInetAccess::IsNetworkConnected()
{
	// �ؒf����Ă���΃N���[�Y��A�ăI�[�v��
	HKEY  hKey = NULL;
	DWORD dwConnect=0;		// Connection�̃X�e�[�^�X
	DWORD dwType;			// �l�̎�ނ��󂯎��
	DWORD dwSize;			// �f�[�^�̃T�C�Y���󂯎��

	// ���W�X�g�����I�[�v��
	if (RegOpenKeyEx(HKEY_LOCAL_MACHINE,SZ_REG_CONNECTION_ROOT,0,KEY_READ,&hKey) == 0) 
	{ 
		RegQueryValueEx(hKey,SZ_REG_CONNECTION_DBVOL,NULL,&dwType,NULL,&dwSize);
		RegQueryValueEx(hKey,SZ_REG_CONNECTION_DBVOL,NULL,&dwType,(LPBYTE)&dwConnect,&dwSize);

		// ���W�X�g���̃N���[�Y
		RegCloseKey(hKey);

		// dwConnect>0 �ł���ΐڑ����
		return (dwConnect > 0) ? true : false;
	}else{
		// ���W�X�g���G���g�����Ȃ���ΐؒf��ԂƔ��f����
		return false;
	}
}

/**
 * ����M�������s���B
 *
 * GET ���\�b�h�܂��� POST ���\�b�h�ŒʐM���s���B
 *
 * @return �E�B���h�E���b�Z�[�W�BWM_MZ3_GET_ERROR ���ɂ́A�G���[���b�Z�[�W���ݒ肳���B
 */
int CInetAccess::ExecSendRecv( EXEC_SENDRECV_TYPE execType )
{
	// URL ����
	ParseURI();

	// ----------------------------------------
	// �ڑ��J�n
	// ----------------------------------------
	if (m_hInternet == NULL) {
		// �ڑ��������s��
		if( !Open() ) {
			m_strErrorMsg = L"�ڑ��G���[(InternetOpen,1)";
			MZ3LOGGER_ERROR( m_strErrorMsg );
			return WM_MZ3_GET_ERROR;
		}
	}else{
		// �����ڑ���ON�̏ꍇ�A�ڑ���Ԃ��`�F�b�N����B
		// ���炩�̗��R�Őؒf����Ă���ꍇ�A�Đڑ����s���B
		if( theApp.m_optionMng.IsUseAutoConnection() ) {
			// �ؒf����Ă���΃N���[�Y��A�ăI�[�v��
			if( !IsNetworkConnected() ) {
				// �N���[�Y
				CloseInternetHandles();
				// �I�[�v��
				if( !Open() ) {
					m_strErrorMsg = L"�ڑ��G���[(InternetOpen,2)";
					MZ3LOGGER_ERROR( m_strErrorMsg );
					return WM_MZ3_GET_ERROR;
				}
			}
		}
	}

	try {
		// �ڑ�
		m_hConnection = ::InternetConnect(
			m_hInternet,				// �Z�b�V����
			m_lpszHostName,				// �z�X�g��
			m_sComponents.nPort,		// �|�[�g�ԍ�
			NULL,						// ���[�U
			NULL,						// �p�X���[�h
			INTERNET_SERVICE_HTTP,
			0,							// ����
			NULL);						// �R���e�L�X�g�Ȃ�

		// --------------------------------------------------
		// ���f�m�F
		// --------------------------------------------------
		if (m_abort != FALSE) {
			// m_hRequest, m_hConnection �����B
			CloseInternetHandles();
			return WM_MZ3_GET_ABORT;
		}

	} catch (CException &) {
		CloseInternetHandles();
		if (m_abort == FALSE) {
			m_strErrorMsg = L"��O����(InternetConnect)";
			MZ3LOGGER_ERROR( m_strErrorMsg );
			return WM_MZ3_GET_ERROR;
		}else{
			return WM_MZ3_GET_ABORT;
		}
	}

	if( m_hConnection==NULL ) {
		// �ڑ��m�f
		// m_hRequest, m_hConnection �����B
		CloseInternetHandles();
		m_strErrorMsg = L"�R�l�N�V�����Ȃ�";
		MZ3LOGGER_ERROR( m_strErrorMsg );
		return WM_MZ3_GET_ERROR;
	}

	util::MySetInformationText( m_hwnd, _T("�T�C�g�ɐڑ����܂���") );

	// �^�C���A�E�g�l��30�b�ɐݒ�
	int timeout = 30000;
	::InternetSetOption(m_hConnection, INTERNET_OPTION_RECEIVE_TIMEOUT, (LPVOID)timeout, sizeof(timeout));

	// �v���N�V�ݒ肪ON�̏ꍇ�AID�ƃp�X���[�h���Z�b�g
	if (theApp.m_optionMng.IsUseProxy() ) {

		LPTSTR proxy = _T("INTERNET_OPTION_PROXY");
		LPTSTR user  = const_cast<LPTSTR>(theApp.m_optionMng.GetProxyUser());
		LPTSTR pass  = const_cast<LPTSTR>(theApp.m_optionMng.GetProxyPassword());

		::InternetSetOption(m_hConnection, INTERNET_OPTION_PROXY, proxy, sizeof(proxy));
		::InternetSetOption(m_hConnection, INTERNET_OPTION_PROXY_USERNAME, user, sizeof(user));
		::InternetSetOption(m_hConnection, INTERNET_OPTION_PROXY_PASSWORD, pass, sizeof(pass));
	}

	// ���N�G�X�g���M
	try {
		DWORD dwFlags = INTERNET_FLAG_RELOAD |
						INTERNET_FLAG_NO_UI |
						INTERNET_FLAG_KEEP_CONNECTION |
						INTERNET_FLAG_NO_AUTO_REDIRECT;

		if (m_sComponents.nScheme == INTERNET_SCHEME_HTTPS) {
			// HTTPS �Őڑ����邽�߁A
			// �t���O�� INTERNET_FLAG_SECURE ��ǉ��B
			dwFlags = dwFlags | INTERNET_FLAG_SECURE;
		}

		// ���\�b�h����HTTP�o�[�W�������A���\�b�h�ɉ����ĕύX����
		LPCTSTR szMethodName = L"";
		LPCTSTR szHttpVersion = NULL;
		if( execType == EXEC_SENDRECV_TYPE_GET ) {
			szMethodName  = L"GET";
			szHttpVersion = NULL;		// �f�t�H���g��HTTP�o�[�W����
		}else{
			szMethodName  = L"POST";
			szHttpVersion = L"HTTP/1.1";
		}

		// ���N�G�X�g���M
		m_hRequest = HttpOpenRequest( m_hConnection, 
									  szMethodName,	// ���\�b�h
									  m_strPath,
									  szHttpVersion,// HTTP �o�[�W����
									  m_ref,		// �����Ȃ�
									  NULL,
									  dwFlags,		// �t���O
									  NULL);		// �R���e�L�X�g�Ȃ�
	}
	catch (CException &) {
		// m_hRequest, m_hConnection �����B
		CloseInternetHandles();
		if (m_abort == FALSE) {
			m_strErrorMsg = L"��O����(HttpOpenRequest)";
			MZ3LOGGER_ERROR( m_strErrorMsg );
			return WM_MZ3_GET_ERROR;
		}
		else {
			return WM_MZ3_GET_ABORT;
		}
	}
	if( m_hRequest == NULL ) {
		// ���N�G�X�g���s�i�I�[�v���j
		// m_hRequest, m_hConnection �����B
		CloseInternetHandles();
		m_strErrorMsg = L"���N�G�X�g���s�i�I�[�v���j";
		MZ3LOGGER_ERROR( m_strErrorMsg );
		return WM_MZ3_GET_ERROR;
	}


	// --------------------------------------------------
	// ���f�m�F
	// --------------------------------------------------
	if (m_abort != FALSE) {
		// m_hRequest, m_hConnection �����B
		CloseInternetHandles();
		return WM_MZ3_GET_ABORT;
	}

	//--- ���\�b�h�ɉ����āA�u�f�[�^���M�vor�u���N�G�X�g���M�v��؂�ւ���
	if( execType == EXEC_SENDRECV_TYPE_GET ) {
		//--- GET ���\�b�h�Ȃ̂Łu���N�G�X�g���M�v�����s
		util::MySetInformationText( m_hwnd, _T("���N�G�X�g���M��") );

		// logging
		if( theApp.m_logger.isDebugEnabled() ) {
			CString msg;
			msg.Format( L"url : %s", m_strPath );

			// �p�X���[�h������
			static MyRegex reg;
			if( !reg.isCompiled() ) {
				if(! reg.compile( L"password=[^&]+" ) ) {
					MZ3LOGGER_DEBUG( FAILED_TO_COMPILE_REGEX_MSG );
				}
			}
			if( reg.isCompiled() ) {
				reg.replaceAll( msg, L"password=xxxxxxxx" );
			}
			MZ3LOGGER_DEBUG( msg );

			msg.Format( L"ref : %s", m_ref );
			MZ3LOGGER_DEBUG( msg );
		}

		try {

			BOOL bRet = HttpSendRequest(m_hRequest,
				NULL,    // �ǉ��w�b�_�Ȃ�
				0,       // �w�b�_��
				NULL,    // �{�f�B�Ȃ�
				0);      // �{�f�B��

			if( bRet == FALSE ) {
				// ���N�G�X�g���M���s
				// m_hRequest, m_hConnection �����B
				CloseInternetHandles();
				m_strErrorMsg = L"���N�G�X�g���M���s";
				MZ3LOGGER_ERROR( m_strErrorMsg );
				return WM_MZ3_GET_ERROR;
			}
		}
		catch (CException &) {
			// m_hRequest, m_hConnection �����B
			CloseInternetHandles();
			if (m_abort == FALSE) {
				m_strErrorMsg = L"��O����(HttpSendRequest)";
				MZ3LOGGER_ERROR( m_strErrorMsg );
				return WM_MZ3_GET_ERROR;
			}
			else {
				return WM_MZ3_GET_ABORT;
			}
		}
		//--- ���N�G�X�g���M����
	}else{
		//--- POST ���\�b�h�Ȃ̂Łu�f�[�^���M�v�����s
		util::MySetInformationText( m_hwnd, _T("�f�[�^���M��") );

		// �f�[�^���M
		// POST�f�[�^�̐ݒ�
		CPostData::post_array& buf = m_postData->GetPostBody();

		// logging
		if( theApp.m_logger.isDebugEnabled() ) {
			CString msg;
			msg.Format( L"url : %s", m_strPath );
			MZ3LOGGER_DEBUG( msg );
			msg.Format( L"ref : %s", m_ref );
			MZ3LOGGER_DEBUG( msg );
			msg.Format( L"post-body : %s", CStringW(&buf[0], buf.size()) );
			MZ3LOGGER_DEBUG( msg );
		}

		try {
			BOOL bRet = FALSE;
			switch (m_postData->GetContentType() ) {
			case CONTENT_TYPE_MULTIPART:
				{
					// multiform
					TCHAR pszContentType_Multi[] = 
						_T("Content-Type: multipart/form-data; boundary=---------------------------7d62ee108071e\r\n");
					bRet = ::HttpSendRequest(m_hRequest,
						pszContentType_Multi,
						lstrlen(pszContentType_Multi),
						&buf[0],
						buf.size() );
					if( bRet == FALSE ) {
						// �w�b�_���M���s
						// m_hRequest, m_hConnection �����B
						CloseInternetHandles();
						m_strErrorMsg = L"�w�b�_���M���s";
						MZ3LOGGER_ERROR( m_strErrorMsg );
						return WM_MZ3_GET_ERROR;
					}
				}
				break;

			case CONTENT_TYPE_FORM_URLENCODED:
				{
					// urlencode
					TCHAR pszContentType_UrlEncoded[] = 
						_T("Content-Type: application/x-www-form-urlencoded\r\n");
					bRet = ::HttpSendRequest(m_hRequest,
						pszContentType_UrlEncoded,
						lstrlen(pszContentType_UrlEncoded),
						&buf[0],
						buf.size() );
					if( bRet == FALSE ) {
						// POST���b�Z�[�W���M���s
						// m_hRequest, m_hConnection �����B
						CloseInternetHandles();
						m_strErrorMsg = L"POST���b�Z�[�W���M���ɃG���[���������܂���";
						MZ3LOGGER_ERROR( m_strErrorMsg );
						return WM_MZ3_GET_ERROR;
					}
				}
				break;

			default:
				{
					// ���T�|�[�g��Content-Type�Ȃ̂ŃG���[�Ƃ���B
					m_strErrorMsg.Format( 
						L"�A�v���P�[�V�����G���[\r\n"
						L"���T�|�[�g��Content-Type [%d]", m_postData->GetContentType() );
					MZ3LOGGER_ERROR( m_strErrorMsg );
					return WM_MZ3_GET_ERROR;
				}
				break;
			}
		}
		catch (CException &) {
			// m_hRequest, m_hConnection �����B
			CloseInternetHandles();
			if (m_abort == FALSE) {
				m_strErrorMsg = L"��O����(HttpSendRequest)";
				MZ3LOGGER_ERROR( m_strErrorMsg );
				return WM_MZ3_GET_ERROR;
			}
			else {
				return WM_MZ3_GET_ABORT;
			}
		}
		//--- �f�[�^���M����
	}

	// --------------------------------------------------
	// ���f�m�F
	// --------------------------------------------------
	if (m_abort != FALSE) {
		// m_hRequest, m_hConnection �����B
		CloseInternetHandles();
		return WM_MZ3_GET_ABORT;
	}

	// ��M�o�b�t�@�i�S�́j
	kfm::kf_buf_type recv_buffer;
	recv_buffer.reserve( 1024*100 );	// �o�b�t�@�T�C�Y��傫�����Ă���

	try {
		DWORD dwContentLen  = 0;		// �R���e���g���i0 �Ȃ��M�T�C�Y�s���j
		DWORD dwContentLenSample = 0;	// �R���e���g���̎Q�l�l�i�O���M���̒l�j

		// �R���e���g�����m�F����
		{
			util::MySetInformationText( m_hwnd, _T("��M�T�C�Y�m�F��") );
			DWORD dwContentLenSize = sizeof(dwContentLen);
			if( HttpQueryInfo(m_hRequest, 
				HTTP_QUERY_CONTENT_LENGTH | HTTP_QUERY_FLAG_NUMBER, 
				(LPVOID)&dwContentLen,
				&dwContentLenSize,
				0) ) 
			{
				// �R���e���g���ɂ���Ċ����������� (%) ���v�Z�ł���B
			}else{
				// �R���e���g�����Ȃ��B
				// �O��̓��y�[�W�̎�M�T�C�Y������΁A�Q�l�l�Ƃ��Ċ����i%�j���v�Z����B
				CFileStatus status;
				if( CFile::GetStatus(util::MakeLogfilePath(theApp.m_mixi4recv),status) &&
					status.m_size > 0 ) 
				{
					// �Q�l�l��ݒ�
					dwContentLenSample = (DWORD)status.m_size;
					dwContentLen = 0;
				}else{
					// ������������ (%) ���v�Z�ł��Ȃ��B
					// ��������܂Ŏ�M�B
					dwContentLen = 0;
				}
			}
		}

		// �X�e�[�^�X�R�[�h�̎擾
		{
			util::MySetInformationText( m_hwnd, _T("�X�e�[�^�X�R�[�h�m�F��") );
			TCHAR szStatusCode[16] = L"";
			DWORD len = sizeof(szStatusCode);
			if( HttpQueryInfo(m_hRequest, HTTP_QUERY_STATUS_CODE, &szStatusCode, &len, 0 ) ) {
				// �X�e�[�^�X�R�[�h�擾�����B
				// �X�e�[�^�X�R�[�h�� 30x �Ȃ烊�_�C���N�g�B
				if( lstrcmp( szStatusCode, L"301" ) == 0 ||
					lstrcmp( szStatusCode, L"302" ) == 0 ) 
				{
					// ���_�C���N�g����擾����
					TCHAR szLocation[1024] = L"";
					DWORD len = sizeof(szLocation);
					if( HttpQueryInfo(m_hRequest, HTTP_QUERY_LOCATION, &szLocation, &len, 0 ) ) {
						// ���_�C���N�g�悪�擾�ł����̂ŁA���_�C���N�g���s�B
						// �Ƃ肠�����ċA�ŁB
						m_uri = szLocation;
						m_nRedirect ++;

						if( theApp.m_logger.isDebugEnabled() ) {
							CString msg;
							msg.Format( L"���_�C���N�g�F[%s]", m_uri );
							MZ3LOGGER_DEBUG( msg );
						}
						return ExecSendRecv( execType );
					}
				}
			}
		}

		DWORD dwOneRecvSize = theApp.m_optionMng.GetRecvBufSize();	// ��x�Ɏ�M����T�C�Y
		{
			// �o���f�[�V����
			if( dwOneRecvSize < 128 ) {
				dwOneRecvSize = 128;
			}
			if( dwOneRecvSize > 1024*100 ) {
				dwOneRecvSize = 1024*100;
			}
		}
		// ��M����
		util::StopWatch sw;
		sw.start();
		for(;;) {
			// ���N�G�X�g�T�C�Y�̌v�Z
			// ��{�I�ɂ� dwOneRecvSize ��p����B
			// ���T�C�Y���������Ă��郊�N�G�X�g�i�摜�ȂǁACGI�ȊO�j�ŁA
			// �c��T�C�Y�� dwOneRecvSize ��菬�����ꍇ�́A�c��T�C�Y���w�肷��B
			DWORD dwRequestSize = dwOneRecvSize;
			if( dwContentLen > 0 && dwContentLen-recv_buffer.size() < dwRequestSize ) {
				dwRequestSize = dwContentLen - recv_buffer.size();
			}

			// ��M
			static std::vector<char> pData( 1024*100+1 );
			DWORD dwBytesRead = 0;
			if( InternetReadFile(m_hRequest, &pData[0], dwRequestSize, &dwBytesRead) == FALSE ) {
				break;
			}
			if( dwBytesRead <= 0 ) {
				break;
			}

			// �T�C�Y�񍐕t��
			CString msg;
			DWORD dwElapsedMsec = sw.getElapsedMilliSecUntilNow();
			if( dwContentLen > 0 ) {
				// �i�����v�Z
				MySetTrafficInfo( m_hwnd, recv_buffer.size(), dwContentLen, dwElapsedMsec );
			}else if( dwContentLenSample > 0 ) {
				// �O���M���̒l�i�Q�l�l�j�Ői�����Z�o
				MySetTrafficInfo( m_hwnd, recv_buffer.size(), -(LONG)dwContentLenSample, dwElapsedMsec );
			}else{
				// ��M�T�C�Y�A���Q�l�l���Ȃ����߁A��M�ς݃T�C�Y�̂ݕ\��
				MySetTrafficInfo( m_hwnd, recv_buffer.size(), 0, dwElapsedMsec );
			}

			pData[dwBytesRead] = 0;

			// recv_buffer �ɒǉ�
			my_append_buf( recv_buffer, &pData[0], dwBytesRead );

			// ���f�[�^��M�ʂ̉��Z
			theApp.m_optionMng.AddTotalRecvBytes( dwBytesRead );

			// ���f�m�F
			if (m_abort != FALSE) {
				// m_hRequest, m_hConnection �����B
				CloseInternetHandles();
				return WM_MZ3_GET_ABORT;
			}
		}
		// ��M����
		MySetTrafficInfo( m_hwnd, recv_buffer.size(), recv_buffer.size(), sw.getElapsedMilliSecUntilNow() );
	}
	catch (CException &) {
		// m_hRequest, m_hConnection �����B
		CloseInternetHandles();
		if (m_abort == FALSE) {
			m_strErrorMsg = L"��O����(��M������)";
			MZ3LOGGER_ERROR( m_strErrorMsg );
			return WM_MZ3_GET_ERROR;
		}
		else {
			return WM_MZ3_GET_ABORT;
		}
	}

	// ��M�T�C�Y�O�Ȃ玸�s�A���f
	if( recv_buffer.empty() )
	{
		m_strErrorMsg = L"��M�T�C�Y�� 0 byte";
		MZ3LOGGER_ERROR( m_strErrorMsg );
		return WM_MZ3_GET_ERROR;
	}

	// HTML �Ȃ當���R�[�h�ϊ������{����
	if (m_fileType == FILE_HTML) {

		util::MySetInformationText( m_hwnd, _T("�f�[�^�ϊ���") );

		if (m_abort != FALSE) {
			// ���f
			// m_hRequest, m_hConnection �����B
			CloseInternetHandles();
			return WM_MZ3_GET_ABORT;
		}

//		util::StopWatch sw;
//		sw.start();

		// �����R�[�h�ϊ�(SJIS�ɕϊ�)
		kfm::kf_buf_type out_buf;
		{
			out_buf.reserve( recv_buffer.size() );
			kfm::kfm k( recv_buffer, out_buf );
			k.tosjis();
		}

//		sw.stop();
//		CString msg;
//		msg.Format( L"%d [msec]", sw.getElapsedMilliSec() );
//		MessageBox( m_hwnd, msg, L"", MB_OK );

//		sw.start();
		// �t�@�C���o��
		{
			FILE* fp_out = _wfopen( theApp.m_filepath.temphtml, _T("wb"));
			if( fp_out == NULL ) {
				m_strErrorMsg = L"�t�@�C���̏o�͂Ɏ��s���܂���";
				MZ3LOGGER_ERROR( m_strErrorMsg );
				return WM_MZ3_GET_ERROR;
			}
			
			fwrite( &out_buf[0], out_buf.size(), 1, fp_out );
			fclose(fp_out);
		}
//		sw.stop();
//		msg.Format( L"%d [msec]", sw.getElapsedMilliSec() );
//		MessageBox( m_hwnd, msg, L"", MB_OK );

		if (m_abort != FALSE) {
			// ���f
			// m_hRequest, m_hConnection �����B
			CloseInternetHandles();
			return WM_MZ3_GET_ABORT;
		}
	}else{
		// HTML �ł͂Ȃ��̂ŁA�o�b�t�@���t�@�C���ɏo�͂���
		FILE* fp_out = _wfopen( theApp.m_filepath.temphtml, _T("wb"));
		if( fp_out == NULL ) {
			m_strErrorMsg = L"�t�@�C���̏o�͂Ɏ��s���܂���";
			MZ3LOGGER_ERROR( m_strErrorMsg );
			return WM_MZ3_GET_ERROR;
		}

		fwrite( &recv_buffer[0], recv_buffer.size(), 1, fp_out );
		fclose(fp_out);
	}

	util::MySetInformationText( m_hwnd, _T("�A�N�Z�X�I��") );

	// �I���ʒm
	if( execType == EXEC_SENDRECV_TYPE_GET ) {
		switch( m_fileType ) {
		case FILE_BINARY:
			return WM_MZ3_GET_END_BINARY;
		case FILE_HTML:
		default:
			return WM_MZ3_GET_END;
		}
	}else{
		return m_postData->GetSuccessMessage();
	}
}

/**
 * m_hRequest, m_hConnection �����B
 */
void CInetAccess::CloseInternetHandles()
{
	// m_hRequest �� NULL �łȂ���΁A����
	if( m_hRequest != NULL ) {
		MZ3LOGGER_DEBUG( L"InternetCloseHandle( m_hRequest )" );
		InternetCloseHandle( m_hRequest );
		m_hRequest = NULL;
	}

	// m_hConnection �� NULL �łȂ���΁A����
	if( m_hConnection != NULL ) {
		MZ3LOGGER_DEBUG( L"InternetCloseHandle( m_hConnection )" );
		InternetCloseHandle( m_hConnection );
		m_hConnection = NULL;
	}
}

/**
 * �ڑ��̂��߂̑O����
 *
 * URI �̉�́A�������s��
 */
void CInetAccess::ParseURI()
{
	TCHAR lpszScheme[256];
	TCHAR lpszUrlPath[1024];
	TCHAR lpszExtraInfo[1024];

	ZeroMemory(&m_sComponents,sizeof(URL_COMPONENTS));
	m_sComponents.dwStructSize		= sizeof(URL_COMPONENTS);
	m_sComponents.lpszHostName		= m_lpszHostName;
	m_sComponents.dwHostNameLength	= sizeof(m_lpszHostName) / sizeof(TCHAR);
	m_sComponents.lpszScheme		= lpszScheme;
	m_sComponents.dwSchemeLength	= sizeof(lpszScheme) / sizeof(TCHAR);
	m_sComponents.lpszUserName		= m_lpszUserName;
	m_sComponents.dwUserNameLength	= sizeof(m_lpszUserName) / sizeof(TCHAR);
	m_sComponents.lpszPassword		= m_lpszPassword;
	m_sComponents.dwPasswordLength	= sizeof(m_lpszPassword) / sizeof(TCHAR);
	m_sComponents.lpszUrlPath		= lpszUrlPath;
	m_sComponents.dwUrlPathLength	= sizeof(lpszUrlPath) / sizeof(TCHAR);
	m_sComponents.lpszExtraInfo		= lpszExtraInfo;
	m_sComponents.dwExtraInfoLength	= sizeof(lpszExtraInfo) / sizeof(TCHAR);

	::InternetCrackUrl(m_uri, lstrlen(m_uri), ICU_ESCAPE, &m_sComponents);

	// �p�X�����𐶐�
	m_strPath = m_uri;
	int nFind = m_strPath.Find(m_lpszHostName);
	if (nFind >= 0) {
		// �z�X�g������������
		// '/' �ŕ������Ă݂�
		nFind = m_strPath.Find(_T("/"),nFind);
		if (nFind >= 0) {
			// '/' �����������̂ŁA'/' �ȍ~�𒊏o
			m_strPath = m_strPath.Right(m_strPath.GetLength() - nFind);
		}
	}
	if (nFind < 0) {
		// �z�X�g����������Ȃ�����
		m_strPath = lpszUrlPath;
		m_strPath += lpszExtraInfo;
	}
}
