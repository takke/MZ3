#pragma once

#include "constants.h"

/// �I�v�V�����f�[�^
namespace option {

/**
 * �I�v�V�����f�[�^�Ǘ��N���X
 */
class Option
{
private:
	BOOL			m_proxyUse;			///< �v���L�V���g���H
	CString			m_proxyServer;		///< �v���L�V�̃T�[�o
	int				m_proxyPort;		///< �v���L�V�̃|�[�g
	CString			m_proxyUser;		///< �v���L�V�̃��[�U
	CString			m_proxyPassword;	///< �v���L�V�̃p�X���[�h

	GETPAGE_TYPE	m_GetPageType;		///< �y�[�W�擾���

	BOOL			m_bBootCheckMnC;	///< �N�����̃��b�Z�[�W�m�F
	CString			m_quoteMark;		///< ���p����

	BOOL			m_bUseBgImage;		///< �w�i�ɉ摜��\�����邩�ǂ���
	int				m_totalRecvBytes;	///< ���f�[�^��M��

public:
	int				m_fontHeight;		///< �t�H���g�i���j�̃T�C�Y�i0 �̏ꍇ�͐ݒ肵�Ȃ��j
	CString			m_fontFace;			///< �t�H���g���i"" �̏ꍇ�͐ݒ肵�Ȃ��j

	int				m_fontHeightBig;	///< �t�H���g�i��j�̃T�C�Y�i0 �̏ꍇ�͐ݒ肵�Ȃ��j
	int				m_fontHeightMedium;	///< �t�H���g�i���j�̃T�C�Y�i0 �̏ꍇ�͐ݒ肵�Ȃ��j
	int				m_fontHeightSmall;	///< �t�H���g�i���j�̃T�C�Y�i0 �̏ꍇ�͐ݒ肵�Ȃ��j

	CString			m_logFolder;		///< ���O�o�͐�t�H���_�B"" �̏ꍇ�̓f�t�H���g�o�͐�B
	bool			m_bSaveLog;			///< ���O��ۑ����邩�ǂ����B

	bool			m_bUseLeftSoftKey;	///< ���\�t�g�L�[�ɂ�郁�j���[�\���𗘗p����H

	int				m_nMainViewCategoryListHeightRatio;	///< ���C����ʂ̃J�e�S�����X�g�̍����i�䗦�j[1,100]
	int				m_nMainViewBodyListHeightRatio;		///< ���C����ʂ̃{�f�B���X�g�̍����i�䗦�j[1,100]
	int				m_nReportViewListHeightRatio;		///< ���|�[�g��ʂ̃��X�g�̍����i�䗦�j[1,100]
	int				m_nReportViewBodyHeightRatio;		///< ���|�[�g��ʂ̃{�f�B�̍����i�䗦�j[1,100]

private:
	int				m_recvBufSize;		///< ��M�o�b�t�@�T�C�Y

public:
	Option()
		: m_GetPageType( GETPAGE_ALL )
		, m_bBootCheckMnC( FALSE )
		, m_bUseBgImage( TRUE )
		, m_fontHeight( 24 )
		, m_fontHeightBig( 28 )
		, m_fontHeightMedium( 24 )
		, m_fontHeightSmall( 18 )
		, m_recvBufSize( 512 )
		, m_bSaveLog( true )
		, m_totalRecvBytes( 0 )
		, m_bUseLeftSoftKey( true )
		, m_quoteMark( L"> " )
		, m_nMainViewCategoryListHeightRatio( 30 )	// �J�e�S�����X�g�� 30%
		, m_nMainViewBodyListHeightRatio( 70 )		// �{�f�B���X�g�� 70%
		, m_nReportViewListHeightRatio( 40 )		// ���X�g�� 40%
		, m_nReportViewBodyHeightRatio( 60 )		// �{�f�B�� 60%
	{
	}

	virtual ~Option()
	{
	}

	void Load();
	void Save();

	void SetProxyUse(BOOL value) { m_proxyUse = value; };
	BOOL IsProxyUse() { return m_proxyUse; };
	
	void SetProxyServer(LPCTSTR value) { m_proxyServer = value; };
	LPCTSTR GetProxyServer() { return m_proxyServer; };
	
	void SetProxyPort(int value) { m_proxyPort = value; };
	int GetProxyPort() { return m_proxyPort; };

	void SetProxyUser(LPCTSTR value) { m_proxyUser = value; };
	LPCTSTR GetProxyUser() { return m_proxyUser; };

	void SetProxyPassword(LPCTSTR value) { m_proxyPassword = value; };
	LPCTSTR GetProxyPassword() { return m_proxyPassword; };

	GETPAGE_TYPE GetPageType() { return m_GetPageType; };
	void SetPageType(GETPAGE_TYPE value) { m_GetPageType = value; };

	BOOL IsBootCheckMnC()			{ return m_bBootCheckMnC; }			///< �N�����̃`�F�b�N
	void SetBootCheckMnC(BOOL flag) { m_bBootCheckMnC = flag; }			///< �N�����̃`�F�b�N�̐ݒ�

	LPCTSTR GetQuoteMark()				{ return m_quoteMark; }			///< ���p�����̎擾
	void SetQuoteMark( LPCTSTR mark )	{ m_quoteMark = mark; }			///< ���p�����̐ݒ�

	BOOL IsUseBgImage()					{ return m_bUseBgImage; }		///< �w�i�摜���g���H
	void SetUseBgImage(BOOL flag)		{ m_bUseBgImage = flag; }		///< �w�i�摜���g�����ǂ����̐ݒ�

	int  GetFontHeight()				{ return m_fontHeight; }		///< �t�H���g�T�C�Y�̎擾
	void SetFontHeight( int fontHeight ){ m_fontHeight = fontHeight; }	///< �t�H���g�T�C�Y�̐ݒ�

	LPCTSTR GetFontFace()				{ return m_fontFace; }			///< �t�H���g���̎擾
	void SetFontFace( LPCTSTR fontFace ){ m_fontFace = fontFace; }		///< �t�H���g���̐ݒ�

	LPCTSTR GetLogFolder()					{ return m_logFolder; }		///< ���O�t�H���_�̎擾
	void SetLogFolder( LPCTSTR logFolder )	{ m_logFolder = logFolder; }///< ���O�t�H���_�̐ݒ�

	int	 GetTotalRecvBytes()			{ return m_totalRecvBytes; }	///< ���f�[�^��M�ʂ̎擾
	void AddTotalRecvBytes( int bytes )	{ m_totalRecvBytes += bytes; }	///< ���f�[�^��M�ʂ̒ǉ�
	void ResetTotalRecvBytes()			{ m_totalRecvBytes = 0; }		///< ���f�[�^��M�ʂ̃��Z�b�g

	/// �w�肳�ꂽ�l����M�o�b�t�@�T�C�Y�Ƃ��ėL���Ȓl�ɕϊ�����
	int NormalizeRecvBufSize( int bufSize ) {

		const int RECVBUFSIZE_MIN = 128;
		if( bufSize < RECVBUFSIZE_MIN ) {
			return RECVBUFSIZE_MIN;
		}
		const int RECVBUFSIZE_MAX = 32768;
		if( bufSize > RECVBUFSIZE_MAX ) {
			return RECVBUFSIZE_MAX;
		}

		return bufSize;
	}

	int GetRecvBufSize() {
		// ���K�������l��Ԃ�
		return NormalizeRecvBufSize( m_recvBufSize );
	}

	void SetRecvBufSize( int recvBufSize ) {
		m_recvBufSize = NormalizeRecvBufSize( recvBufSize );
	}

};

}