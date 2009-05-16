/*
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */
#pragma once

// CPostData �R�}���h �^�[�Q�b�g

/**
 * POST ���M�f�[�^
 */
class CPostData
{
public:
	/// POST �̃{�f�B���̌^
	typedef std::vector<char> post_array;

private:

	post_array	 m_postBody;	///< POST�̃{�f�B��

	CString		 m_comment;		///< hidden �^�O�Ƃ��ď������ݎ��ɑ��郆�[�U���̓��b�Z�[�W�̖{��

	CString		 m_postkey;		///< �m�F��ʂ� post_type �l�imixi �̔F�؃L�[�H�j
	CString		 m_packed;		///< �m�F��ʂ� packed �l�imixi �̔F�؃L�[�H�j

	CString		 m_contentType;	///< Content-Type
	int			 m_procType;	///< �����^�C�v�iMZ3_POST_* �萔�j�B
								///< �������� CInternetAccess �X���b�h���瑗�M����郁�b�Z�[�W��ʂɂȂ�B
	CString		 m_confirmUri;	///< �m�FURI

	CString		 m_additionalHeaders;	///< �ǉ��p�����[�^

public:
	CPostData() { m_postBody.reserve(10 * 1024); }
	virtual ~CPostData() {}

	bool AppendAdditionalHeader( LPCTSTR szHeader ) {
		if (!m_additionalHeaders.IsEmpty()) {
			m_additionalHeaders.Append( L"\r\n" );
		}
		m_additionalHeaders.Append( szHeader );
		return true;
	}

	const CString& GetAdditionalHeaders() {
		return m_additionalHeaders;
	}

	/// POST �̃{�f�B���̎擾�i�Q�Ƃɂ��ݒ���\�j
	post_array& GetPostBody() { return m_postBody; }

	/// POST �̃{�f�B��������������
	void ClearPostBody() {
		m_postBody.clear();
		m_additionalHeaders = L"";
	}

	/// POST �̃{�f�B���ɕ������ǉ�����B
	/// ������ CRLF ��ǉ�����B
	/// ������� EUC-JP URL Encoded String �ł��邱�ƁB
	void AppendPostBodyWithCRLF( LPCTSTR strEucUrlEncodedString ) {
		AppendPostBody( strEucUrlEncodedString );
		AppendPostBody( "\r\n" );
	}

	/// POST �̃{�f�B���� ANSI �������ǉ�����B
	/// ������ CRLF ��ǉ�����B
	void AppendPostBodyWithCRLF( const char* str ) {
		AppendPostBody( str );
		AppendPostBody( "\r\n" );
	}

	/// POST �̃{�f�B���ɕ������ǉ�����B
	/// ������� EUC-JP URL Encoded String �ł��邱�ƁB
	void AppendPostBody( LPCTSTR strEucUrlEncodedString ) {

		if( strEucUrlEncodedString != NULL && wcslen(strEucUrlEncodedString) > 0 ) {
			static post_array buf;
			buf.resize( 10001 );
			memset( &buf[0], 0x00, sizeof(char) * 10001);

			// ANSI ������ɕϊ�
			wcstombs( &buf[0], strEucUrlEncodedString, 10000);

			// ANSI �������ǉ�
			AppendPostBody( &buf[0] );
		}
	}

	/// POST �̃{�f�B���� ANSI �������ǉ�����B
	void AppendPostBody( const char* str ) {

		AppendPostBody( str, strlen(str) );
	}

	/// POST �̃{�f�B���� char �z���ǉ�����B
	void AppendPostBody( const char* str, int len ) {

		if( len>0 ) {
			// ���̃T�C�Y
			size_t n = m_postBody.size();

			// �����ɒǉ��ł��邾���̃o�b�t�@���m��
			m_postBody.resize( n+len );

			// �����ɃR�s�[
			memcpy( &m_postBody[0] +n, str, len );
		}
	}

	/// post_key �l�̐ݒ�i�m�F��ʂ̉�͎��ɐݒ肳���j
	void SetPostKey(LPCTSTR str) { m_postkey = str; }

	/// post_key �l�̎擾�i�������݃{�^����������POST���b�Z�[�W�쐬���ɗ��p�����j
	LPCTSTR GetPostKey() { return m_postkey; }

	/// Content-Type �̐ݒ�
	void SetContentType(CONTENT_TYPE value) {
		m_contentType = value;
	}

	/// Content-Type �̎擾
	CONTENT_TYPE GetContentType() { return m_contentType; }

	void SetSuccessMessage(int value) { m_procType = value; }
	int GetSuccessMessage() { return m_procType; }

	void SetComment(LPCTSTR str) { m_comment = str; }
	LPCTSTR GetComment() { return m_comment; }

	void SetConfirmUri(LPCTSTR str) { m_confirmUri = str; }
	LPCTSTR GetConfirmUri() { return m_confirmUri; }

	void SetPacked( LPCTSTR str ) { m_packed = str; }
	LPCTSTR GetPacked() { return m_packed; }
};


