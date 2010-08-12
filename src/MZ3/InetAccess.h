/*
 * Copyright (c) MZ3 Project Team All rights reserved.
 * Code licensed under the BSD License:
 * http://www.mz3.jp/license.txt
 */
#pragma once

#include "wininet.h"
#include "constants.h"
#include "PostData.h"
#include "kfm.h"

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
	TCHAR				m_lpszScheme[256];
	TCHAR				m_lpszHostName[256];
	TCHAR				m_lpszUserName[256];
	TCHAR				m_lpszPassword[256];
	URL_COMPONENTS		m_sComponents;
	CString				m_strPath;				///< URL �̃p�X�����i/�ȍ~�A/���܂ށj

	CString				m_uri;					///< URI
	CString				m_ref;					///< ���t�@���[

	CString				m_strUserId;			///< �F�ؗp���[�UID
	CString				m_strPassword;			///< �F�ؗp�p�X���[�h

	HWND				m_hwnd;					///< �I���ʒm�𑗂�E�B���h�E�̃n���h��

	void*				m_object;				///< �I�����b�Z�[�W�� LPARAM �ɓn���|�C���^

	BOOL				m_abort;				///< ���f�t���O

	CPostData*			m_postData;				///< ���M�f�[�^
	CWinThread*			m_pThreadMain;			///< �X���b�h�̃|�C���^

	HINTERNET			m_hConnection;
	HINTERNET			m_hRequest;
public:
	HINTERNET			m_hInternet;			///< �Z�b�V����(InternetOpen�Ŏ擾)

private:
	bool				m_bAccessing;			///< �ʐM���t���O

	FILE_TYPE			m_fileType;				///< ��M�t�@�C�����

	CString				m_strErrorMsg;			///< �G���[���̃��b�Z�[�W�B
												///< WM_MZ3_GET_ERROR ���ɂ̂ݗL���B

	CString				m_strUserAgent;			///< ���N�G�X�g���ꂽ���[�U�G�[�W�F���g

	int					m_nRedirect;			///< ���_�C���N�g��
	bool				m_bIsBlocking;			///< �u���b�L���O���[�h���ǂ���

public:
	enum ENCODING {
		ENCODING_SJIS = 0,
		ENCODING_EUC  = 1,
		ENCODING_UTF8 = 2,
		ENCODING_NOCONVERSION = 3,
	};
	ENCODING			m_encodingFrom;			///< �ʐM�擾�f�[�^�̃G���R�[�f�B���O�i�f�t�H���g�� mixi HTML �Ȃ̂� EUC-JP�j
	kfm::kf_buf_type	out_buf;

	DWORD				m_dwHttpStatus;			///< HTTP�X�e�[�^�X

	/**
	 * �R���X�g���N�^
	 */
	CInetAccess()
		: m_hInternet(NULL)
		, m_encodingFrom(ENCODING_EUC)
		, m_dwHttpStatus(0)
		, m_bIsBlocking(false)
	{
		Initialize( NULL, NULL );
	}

	virtual ~CInetAccess();

	bool Open();
	void CloseInternetHandles();
	bool IsNetworkConnected();

	LPCTSTR	GetURL()			{ return m_uri; }

	BOOL DoPostBlocking( LPCTSTR uri, LPCTSTR ref, FILE_TYPE type, CPostData* postData );
	BOOL DoGetBlocking( LPCTSTR uri, LPCTSTR ref, FILE_TYPE type, CPostData* postDataForHeaders );
	BOOL DoPost( LPCTSTR uri, LPCTSTR ref, FILE_TYPE type, CPostData* postData, LPCTSTR szUserId=NULL, LPCTSTR szPassword=NULL, LPCTSTR strUserAgent=L"" );
	BOOL DoGet( LPCTSTR uri, LPCTSTR ref, FILE_TYPE type, CPostData* postDataForHeaders, LPCTSTR szUserId=NULL, LPCTSTR szPassword=NULL, LPCTSTR strUserAgent=L"" );

	void Initialize( HWND hwnd, void* object, ENCODING encoding=ENCODING_EUC );

	bool	IsConnecting()		{ return m_bAccessing; }		///< �ʐM���H
	LPCTSTR GetErrorMessage()	{ return m_strErrorMsg; }		///< �G���[���b�Z�[�W�̎擾
	int		GetRedirectCount()	{ return m_nRedirect; }			///< ���_�C���N�g�񐔂̎擾

	/// ���f
	inline void Abort() 
	{
		if( m_abort ) 
			return;

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
#ifdef WINCE
	static unsigned int __stdcall ExecGet_Thread(LPVOID);
	static unsigned int __stdcall ExecPost_Thread(LPVOID);
#else
	static unsigned int __cdecl ExecGet_Thread(LPVOID);
	static unsigned int __cdecl ExecPost_Thread(LPVOID);
#endif

	void ParseURI();

	int ExecSendRecv( EXEC_SENDRECV_TYPE execType );

	HRESULT WINAPI SP_EstablishInetConnProc( CString& proxy );
};


