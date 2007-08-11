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
	bool			m_bDebugMode;			///< �f�o�b�O���[�h
	bool			m_bUseAutoConnection;	///< �����ڑ����g���H
	bool			m_bUseProxy;			///< �v���L�V���g���H
	bool			m_bUseGlobalProxy;		///< �O���[�o���v���L�V���g���H
											///< �i�����ڑ�ON�̏ꍇ�̂ݗL���j
public:
	CString			m_strUserAgent;			///< User-Agent

private:
	CString			m_proxyServer;			///< �v���L�V�̃T�[�o
	int				m_proxyPort;			///< �v���L�V�̃|�[�g
	CString			m_proxyUser;			///< �v���L�V�̃��[�U
	CString			m_proxyPassword;		///< �v���L�V�̃p�X���[�h

	GETPAGE_TYPE	m_GetPageType;			///< �y�[�W�擾���

	BOOL			m_bBootCheckMnC;		///< �N�����̃��b�Z�[�W�m�F
	CString			m_quoteMark;			///< ���p����

	BOOL			m_bUseBgImage;			///< �w�i�ɉ摜��\�����邩�ǂ���
	int				m_totalRecvBytes;		///< ���f�[�^��M��

public:

	bool			m_bConvertUrlForMixiMobile;	///< mixi ���o�C����URL�ɕϊ����邩�ǂ���

	int				m_fontHeight;			///< �t�H���g�i���j�̃T�C�Y�i0 �̏ꍇ�͐ݒ肵�Ȃ��j
	CString			m_fontFace;				///< �t�H���g���i"" �̏ꍇ�͐ݒ肵�Ȃ��j

	int				m_fontHeightBig;		///< �t�H���g�i��j�̃T�C�Y�i0 �̏ꍇ�͐ݒ肵�Ȃ��j
	int				m_fontHeightMedium;		///< �t�H���g�i���j�̃T�C�Y�i0 �̏ꍇ�͐ݒ肵�Ȃ��j
	int				m_fontHeightSmall;		///< �t�H���g�i���j�̃T�C�Y�i0 �̏ꍇ�͐ݒ肵�Ȃ��j

	int				m_longReturnRangeMSec;	///< ���������莞�ԁi�~���b�j

	CString			m_logFolder;			///< ���O�o�͐�t�H���_�B"" �̏ꍇ�̓f�t�H���g�o�͐�B
	bool			m_bSaveLog;				///< ���O��ۑ����邩�ǂ����B

	bool			m_bUseLeftSoftKey;		///< ���\�t�g�L�[�ɂ�郁�j���[�\���𗘗p����H
	bool			m_bUseXcrawlExtension;	///< Xcrawl �̌듮��h�~�@�\
	bool			m_bUseRunConfirmDlg;	///< �_�E�����[�h��̎��s�m�F��ʂ�\������H

	int				m_nMainViewCategoryListHeightRatio;	///< ���C����ʂ̃J�e�S�����X�g�̍����i�䗦�j[1,1000]
	int				m_nMainViewBodyListHeightRatio;		///< ���C����ʂ̃{�f�B���X�g�̍����i�䗦�j[1,1000]
	int				m_nReportViewListHeightRatio;		///< ���|�[�g��ʂ̃��X�g�̍����i�䗦�j[1,1000]
	int				m_nReportViewBodyHeightRatio;		///< ���|�[�g��ʂ̃{�f�B�̍����i�䗦�j[1,1000]

	int				m_nMainViewCategoryListCol1Ratio;	///< ���C����ʂ̃J�e�S�����X�g�̃J�����P�̕��i�䗦�j[1,1000]
	int				m_nMainViewCategoryListCol2Ratio;	///< ���C����ʂ̃J�e�S�����X�g�̃J�����Q�̕��i�䗦�j[1,1000]
	int				m_nMainViewBodyListCol1Ratio;		///< ���C����ʂ̃{�f�B���X�g�̃J�����P�̕��i�䗦�j[1,1000]
	int				m_nMainViewBodyListCol2Ratio;		///< ���C����ʂ̃{�f�B���X�g�̃J�����Q�̕��i�䗦�j[1,1000]

	int				m_nReportViewListCol1Ratio;			///< ���|�[�g��ʂ̃��X�g�̃J�����P�̕��i�䗦�j[1,1000]
	int				m_nReportViewListCol2Ratio;			///< ���|�[�g��ʂ̃��X�g�̃J�����Q�̕��i�䗦�j[1,1000]
	int				m_nReportViewListCol3Ratio;			///< ���|�[�g��ʂ̃��X�g�̃J�����R�̕��i�䗦�j[1,1000]

	enum REPORT_SCROLL_TYPE {
		REPORT_SCROLL_TYPE_LINE = 0,	///< �s�P��
		REPORT_SCROLL_TYPE_PAGE = 1,	///< �y�[�W�P��
	};
	REPORT_SCROLL_TYPE	m_reportScrollType;	///< ���|�[�g��ʂ̃X�N���[���^�C�v
	int				m_reportScrollLine;	///< ���|�[�g��ʂ̃X�N���[���s��

private:
	int				m_recvBufSize;		///< ��M�o�b�t�@�T�C�Y

public:
	Option()
		: m_GetPageType( GETPAGE_ALL )
		, m_bUseAutoConnection( true )
		, m_bUseProxy( false )
		, m_bUseGlobalProxy( true )
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
		, m_bUseXcrawlExtension( true )
		, m_bUseRunConfirmDlg( true )
		, m_quoteMark( L"> " )
		, m_nMainViewCategoryListHeightRatio( 30 )	// �J�e�S�����X�g�� 30%
		, m_nMainViewBodyListHeightRatio( 70 )		// �{�f�B���X�g�� 70%
		, m_nReportViewListHeightRatio( 40 )		// ���X�g�� 40%
		, m_nReportViewBodyHeightRatio( 60 )		// �{�f�B�� 60%
		, m_nReportViewListCol1Ratio( 7 )			// �J�����P�� 7/N
		, m_nReportViewListCol2Ratio( 17 )			// �J�����Q�� 17/N
		, m_nReportViewListCol3Ratio( 21 )			// �J�����R�� 21/N
		, m_nMainViewCategoryListCol1Ratio( 25 )	// �J�e�S�����X�g�A�J�����P�� 25/N
		, m_nMainViewCategoryListCol2Ratio( 20 )	// �J�e�S�����X�g�A�J�����Q�� 20/N
		, m_nMainViewBodyListCol1Ratio( 24 )		// �{�f�B���X�g�A�J�����P�� 24/N
		, m_nMainViewBodyListCol2Ratio( 21 )		// �{�f�B���X�g�A�J�����Q�� 21/N
		, m_reportScrollType( REPORT_SCROLL_TYPE_PAGE )
		, m_reportScrollLine( 7 )
		, m_bConvertUrlForMixiMobile( true )
		, m_bDebugMode( false )
		, m_longReturnRangeMSec( 300 )
		, m_strUserAgent( L"Mozilla/4.0 (compatible; MSIE 6.0; Windows NT 5.1)" )
	{
	}

	virtual ~Option()
	{
	}

	/// ���K��
	static int normalizeRange( int value, int minv, int maxv ) 
	{
		// �����l����
		value = max( value, minv );

		// ����l����
		value = min( value, maxv );

		return value;
	}

	void Load();
	void Save();

	void SetUseProxy(bool value) { m_bUseProxy = value; }
	bool IsUseProxy() { return m_bUseProxy; }
	
	void SetDebugMode(bool value) { m_bDebugMode = value; }
	bool IsDebugMode() { return m_bDebugMode; }
	
	void SetUseGlobalProxy(bool value) { m_bUseGlobalProxy = value; }
	bool IsUseGlobalProxy() { return m_bUseGlobalProxy; }
	
	void SetUseAutoConnection(bool value) { m_bUseAutoConnection = value; }
	bool IsUseAutoConnection() { return m_bUseAutoConnection; }
	
	void SetProxyServer(LPCTSTR value) { m_proxyServer = value; }
	LPCTSTR GetProxyServer() { return m_proxyServer; }
	
	void SetProxyPort(int value) { m_proxyPort = value; }
	int GetProxyPort() { return m_proxyPort; }

	void SetProxyUser(LPCTSTR value) { m_proxyUser = value; }
	LPCTSTR GetProxyUser() { return m_proxyUser; }

	void SetProxyPassword(LPCTSTR value) { m_proxyPassword = value; }
	LPCTSTR GetProxyPassword() { return m_proxyPassword; }

	GETPAGE_TYPE GetPageType() { return m_GetPageType; }
	void SetPageType(GETPAGE_TYPE value) { m_GetPageType = value; }

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
		const int RECVBUFSIZE_MAX = 32768;

		return normalizeRange( bufSize, RECVBUFSIZE_MIN, RECVBUFSIZE_MAX );
	}

	int GetRecvBufSize() {
		// ���K�������l��Ԃ�
		return NormalizeRecvBufSize( m_recvBufSize );
	}

	void SetRecvBufSize( int recvBufSize ) {
		m_recvBufSize = NormalizeRecvBufSize( recvBufSize );
	}

	/// �t�H���g�T�C�Y�̐��K��
	static int normalizeFontSize( int n ) {
		return normalizeRange( n, 8, 50 );
	}

	/// ���������莞�Ԃ̐��K��
	static int normalizeLongReturnRangeMSec( int msec ) {
		return normalizeRange( msec, 100, 1000 );
	}

};

}