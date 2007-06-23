#pragma once

#include "wininet.h"
#include "constants.h"
#include "PostData.h"

// CInetAccess �R�}���h �^�[�Q�b�g

class CInetAccess
{
public:
	/**
	 * ��M�t�@�C�����
	 *
	 * ���̎�ʂɂ���āA��M��̏������ω�����B
	 */
	enum FILE_TYPE {
		FILE_HTML,		///< HTML �t�@�C���B��M��� SJIS �ւ̊����R�[�h�ϊ������{����B
		FILE_BINARY		///< �o�C�i���t�@�C���B��M��̊����R�[�h�ϊ������{���Ȃ��B
	};

	/**
	 * �ʐM�����̎��
	 */
	enum EXEC_SENDRECV_TYPE {
		EXEC_SENDRECV_TYPE_GET,		///< GET ���\�b�h
		EXEC_SENDRECV_TYPE_POST,	///< POST ���\�b�h
	};
private:
	TCHAR				m_lpszHostName[256];
	TCHAR				m_lpszUserName[256];
	TCHAR				m_lpszPassword[256];
	URL_COMPONENTS		m_sComponents;
	CString				m_strPath;				///< URL �̃p�X�����i/�ȍ~�A/���܂ށj

	CString				m_uri;					///< URI
	CString				m_ref;					///< ���t�@���[

	HWND				m_hwnd;					///< �I���ʒm�𑗂�E�B���h�E�̃n���h��



	void*				m_object;				///< �I�����b�Z�[�W�� LPARAM �ɓn���|�C���^

	BOOL				m_abort;				///< ���f�t���O

	CPostData*			m_postData;				///< ���M�f�[�^
	CWinThread*			m_pThreadMain;			///< �X���b�h�̃|�C���^

	HINTERNET			m_hConnection;
	HINTERNET			m_hRequest;
	HINTERNET			m_hInternet;			///< �Z�b�V����(InternetOpen�Ŏ擾)

	bool				m_bAccessing;			///< �ʐM���t���O



	FILE_TYPE			m_fileType;				///< ��M�t�@�C�����

	CString				m_strErrorMsg;			///< �G���[���̃��b�Z�[�W�B
												///< WM_MZ3_GET_ERROR ���ɂ̂ݗL���B

	int					m_nRedirect;			///< ���_�C���N�g��

public:
	/**
	 * �R���X�g���N�^
	 */
	CInetAccess()
		: m_hInternet(NULL)
	{
		Initialize( NULL, NULL );
	}

	virtual ~CInetAccess();

	bool Open();
	void CloseInternetHandles();

	LPCTSTR	GetURL()			{ return m_uri; }

	BOOL DoGet( LPCTSTR uri, LPCTSTR ref, FILE_TYPE type );
	BOOL DoPost( LPCTSTR uri, LPCTSTR ref, FILE_TYPE type, CPostData* postData );

	void Initialize( HWND hwnd, void* object );

	bool	IsConnecting()		{ return m_bAccessing; }		///< �ʐM���H
	LPCTSTR GetErrorMessage()	{ return m_strErrorMsg; }		///< �G���[���b�Z�[�W�̎擾
	int		GetRedirectCount()	{ return m_nRedirect; }			///< ���_�C���N�g�񐔂̎擾

	/// ���f
	inline void Abort() 
	{ 
		m_abort = TRUE;
		::Sleep(100);

		CloseInternetHandles();

		// Abort ���� Internet �n���h��������
		if( m_hInternet != NULL ) {
			InternetCloseHandle( m_hInternet );
			m_hInternet = NULL;
		}
	};

private:
	static unsigned int __stdcall ExecGet_Thread(LPVOID);
	static unsigned int __stdcall ExecPost_Thread(LPVOID);

	void ParseURI();

	int ExecSendRecv( EXEC_SENDRECV_TYPE execType );

	HRESULT WINAPI SP_EstablishInetConnProc( CString& proxy );
};


