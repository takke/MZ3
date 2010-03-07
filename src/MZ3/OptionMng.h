/*
 * Copyright (c) MZ3 Project Team All rights reserved.
 * Code licensed under the BSD License:
 * http://www.mz3.jp/license.txt
 */
#pragma once

#include "constants.h"

/// �I�v�V�����f�[�^
namespace option {

/**
 * �I�v�V�����f�[�^�Ǘ��N���X
 */
class Option
{
public:
	/// �N�����̈ڍs�����̊����t���O
	enum STARTUP_TRANSITION_DONE_TYPE
	{
		STARTUP_TRANSITION_DONE_TYPE_NONE = 0,					///< ������
		STARTUP_TRANSITION_DONE_TYPE_TWITTER_MODE_ADDED = 1,	///< Twitter���[�h�ǉ����̃��b�Z�[�W�\����
		STARTUP_TRANSITION_DONE_TYPE_FONT_SIZE_SCALED = 2,		///< �t�H���g�T�C�Y�̒�����
	};
	STARTUP_TRANSITION_DONE_TYPE	m_StartupTransitionDoneType;	///< �N�����̃��b�Z�[�W�̕\���ς݃t���O

private:
	bool			m_bDebugMode;			///< �f�o�b�O���[�h

public:
	bool			m_bUseDevVerCheck;		///< �o�[�W�����`�F�b�N�ɊJ���ł��܂߂�
	bool			m_bShowBalloonOnNewTL;	///< [MZ4] �o���[���\���L��

private:
	CString			m_proxyServer;			///< �v���L�V�̃T�[�o
	int				m_proxyPort;			///< �v���L�V�̃|�[�g
	CString			m_proxyUser;			///< �v���L�V�̃��[�U
	CString			m_proxyPassword;		///< �v���L�V�̃p�X���[�h

#ifdef BT_MZ3
	GETPAGE_TYPE	m_GetPageType;			///< �y�[�W�擾���
#endif

	//--- BOOT
	bool			m_bBootCheckMnC;		///< �N�����̃��b�Z�[�W�m�F

	CString			m_quoteMark;			///< ���p����
	BOOL			m_bUseBgImage;			///< �w�i�ɉ摜��\�����邩�ǂ���
	int				m_totalRecvBytes;		///< ���f�[�^��M��

public:
	int				m_nIntervalCheckSec;	///< ����擾�Ԋu[sec]
	bool			m_bEnableIntervalCheck;	///< ����擾�@�\���L�����H

#ifndef WINCE
	CString			m_strWindowPos;			///< �E�B���h�E�ʒu�E�T�C�Y
#endif

	bool			m_bConfirmOpenURL;		///< URL���J���Ƃ��Ɋm�F����

	int				m_fontHeight;			///< �t�H���g�i���j�̃T�C�Y�i0 �̏ꍇ�͐ݒ肵�Ȃ��j
	CString			m_fontFace;				///< �t�H���g���i"" �̏ꍇ�͐ݒ肵�Ȃ��j
	bool			m_bUseClearTypeFont;	///< Clear Type Font

	int				m_longReturnRangeMSec;	///< ���������莞�ԁi�~���b�j

	CString			m_logFolder;			///< ���O�o�͐�t�H���_�B"" �̏ꍇ�̓f�t�H���g�o�͐�B
	bool			m_bSaveLog;				///< ���O��ۑ����邩�ǂ����B

	CString			m_strSkinname;			///< ���݂̃X�L�����i=�t�H���_���j

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

	bool			m_killPaneLabel;					///< �y�C���̃��x����\�����Ȃ��H

	int				m_lastTopPageTabIndex;				///< �O��I�����̃^�u�̃C���f�b�N�X
	int				m_lastTopPageCategoryIndex;			///< �O��I�����̃J�e�S���̃C���f�b�N�X

	bool			m_bAutoDownloadEmojiImageFiles;		///< �N�����ɊG�����t�@�C���������擾����

	//--- ���|�[�g���
	enum REPORT_SCROLL_TYPE {
		REPORT_SCROLL_TYPE_LINE = 0,	///< �s�P��
		REPORT_SCROLL_TYPE_PAGE = 1,	///< �y�[�W�P��
	};
	REPORT_SCROLL_TYPE	m_reportScrollType;				///< ���|�[�g��ʂ̃X�N���[���^�C�v
	int				m_reportScrollLine;					///< ���|�[�g��ʂ̃X�N���[���s��
	
	bool			m_bUseRan2PanScrollAnimation;		///< �����r���[�̃p���X�N���[���A�j���[�V����
	bool			m_bUseRan2HorizontalDragMove;		///< ���h���b�O�ł̍��ڈړ�
	bool			m_bUseRan2DoubleClickMove;			///< �_�u���N���b�N�ł̍��ڈړ�

	bool			m_bListScrollByPixelOnMZ3;			///< MZ3(WM)�Ńs�N�Z���P�ʃX�N���[�������s���邩�H
														///< T-01A(�����炭WM6.5��)�ł̓X�N���[�����������̂�
														///< �f�t�H���g�I�t�Ƃ���

	//--- ���C�����
	bool			m_bShowMainViewIcon;				///< �g�s�b�N���ɃA�C�R����\������H
	bool			m_bShowMainViewMiniImage;			///< ���[�U��R�~���j�e�B�̉摜��\������H
	bool			m_bAutoLoadMiniImage;				///< �摜�̎����擾

	bool			m_bBodyListIntegratedColumnMode;	///< �{�f�B���X�g�̃J�����̓������[�h
	int				m_nBodyListIntegratedColumnModeLine;///< �{�f�B���X�g�̃J�����̓������[�h
	bool			m_bOneClickCategoryFetchMode;		///< ��y�C���̃��X�g�N���b�N���Ɏ擾����
	bool			m_bMagnifyModeCategoryAtStart;		///< �N�����ɏ�y�C�����ő剻����

	enum BODYLIST_ICONSIZE {
		BODYLIST_ICONSIZE_AUTO = 0,		///< ����
		BODYLIST_ICONSIZE_64 = 64,
		BODYLIST_ICONSIZE_48 = 48,
		BODYLIST_ICONSIZE_32 = 32,
		BODYLIST_ICONSIZE_16 = 16,
	};
	BODYLIST_ICONSIZE m_bodyListIconSize;			///< �{�f�B���X�g�̃A�C�R���T�C�Y

	//--- �ʐM
	int				m_recvBufSize;					///< ��M�o�b�t�@�T�C�Y
	bool			m_bUseAutoConnection;			///< �����ڑ����g���H
	CString			m_strUserAgent;					///< User-Agent

	bool			m_bShowNetProgressBar;			///< �ʐM���̃v���O���X�o�[�\��

	bool			m_bUseProxy;					///< �v���L�V���g���H
	bool			m_bUseGlobalProxy;				///< �O���[�o���v���L�V���g���H
													///< �i�����ڑ�ON�̏ꍇ�̂ݗL���j
public:
	//--- Twitter
	bool			m_bAddSourceTextOnTwitterPost;	///< ������ *MZ3* �}�[�N������
	CString			m_strTwitterPostFotterText;		///< �����ɕt����}�[�N
	int				m_nTwitterStatusLineCount;		///< Twitter���[�h���̃X�e�[�^�X�s��
	int				m_nTwitterGetPageCount;			///< Twitter���[�h���̎擾�y�[�W��
	bool			m_bTwitterReloadTLAfterPost;	///< ���e��Ƀ^�C�����C�����擾����
	bool			m_bTwitterCursorRestoreAfterTLFetch;	///< TL�擾���ɃJ�[�\���ʒu���ێ�����
	int				m_nTwitterRTStyle;				///< ReTweet �`��
													///< 0:"RT @takke: ����"
													///< 1:"����RT"
													///< 2:"QT @takke: ����"
													///< 3:"����RT/RT"

public:
	Option()
		: m_bUseAutoConnection( true )
#ifdef BT_MZ3
		, m_GetPageType( GETPAGE_LATEST20 )
#endif
		, m_bUseProxy( false )
		, m_bUseGlobalProxy( true )
		, m_bBootCheckMnC( false )
		, m_bUseBgImage( TRUE )
		, m_fontHeight( 8 )					// �����l�� Load() ���Őݒ肳���
#ifdef WINCE
		, m_recvBufSize( 2048 )
#else
		, m_recvBufSize( 8192 )
#endif
		, m_bSaveLog( true )
		, m_totalRecvBytes( 0 )
		, m_bUseXcrawlExtension( false )
		, m_bUseRunConfirmDlg( true )
		, m_quoteMark( L"> " )
		, m_nMainViewCategoryListHeightRatio( 20 )	// �J�e�S�����X�g�� 20%
		, m_nMainViewBodyListHeightRatio( 80 )		// �{�f�B���X�g�� 80%
		, m_nReportViewListHeightRatio( 30 )		// ���X�g�� 30%
		, m_nReportViewBodyHeightRatio( 70 )		// �{�f�B�� 70%
		, m_nReportViewListCol1Ratio( 7 )			// �J�����P�� 7/N
		, m_nReportViewListCol2Ratio( 17 )			// �J�����Q�� 17/N
		, m_nReportViewListCol3Ratio( 21 )			// �J�����R�� 21/N
		, m_nMainViewCategoryListCol1Ratio( 25 )	// �J�e�S�����X�g�A�J�����P�� 25/N
		, m_nMainViewCategoryListCol2Ratio( 20 )	// �J�e�S�����X�g�A�J�����Q�� 20/N
		, m_nMainViewBodyListCol1Ratio( 24 )		// �{�f�B���X�g�A�J�����P�� 24/N
		, m_nMainViewBodyListCol2Ratio( 21 )		// �{�f�B���X�g�A�J�����Q�� 21/N
		, m_reportScrollType( REPORT_SCROLL_TYPE_LINE )
		, m_reportScrollLine( 7 )
		, m_bConfirmOpenURL( true )
/*
#ifdef WINCE
		, m_bConvertUrlForMixiMobile( true )
#else
		, m_bConvertUrlForMixiMobile( false )
#endif
*/
		, m_bDebugMode( false )
		, m_longReturnRangeMSec( 300 )
		, m_strUserAgent( L"Mozilla/4.0 (compatible; MSIE 6.0; Windows NT 5.1)" )
		, m_strSkinname( L"aluminium" )
		, m_nIntervalCheckSec( 60 )
		, m_bEnableIntervalCheck( false )
		, m_lastTopPageTabIndex( 0 )
		, m_lastTopPageCategoryIndex( 0 )
		, m_bShowMainViewIcon( true )
#ifdef WINCE
		, m_bShowMainViewMiniImage( true )
		, m_bUseRan2DoubleClickMove( false )
#else
		, m_bShowMainViewMiniImage( true )
		, m_bUseRan2DoubleClickMove( true )
#endif
		, m_bAutoLoadMiniImage( true )
		, m_bListScrollByPixelOnMZ3( false )
		, m_bBodyListIntegratedColumnMode( true )
		, m_nBodyListIntegratedColumnModeLine( 3 )
		, m_bOneClickCategoryFetchMode( true )
#ifdef BT_TKTW
		, m_bMagnifyModeCategoryAtStart( true )
#else
		, m_bMagnifyModeCategoryAtStart( false )
#endif
		, m_bAddSourceTextOnTwitterPost( true )
		, m_StartupTransitionDoneType( STARTUP_TRANSITION_DONE_TYPE_FONT_SIZE_SCALED )
		, m_nTwitterStatusLineCount(3)
		, m_bUseRan2PanScrollAnimation( true )
		, m_bUseRan2HorizontalDragMove( true )
		, m_bAutoDownloadEmojiImageFiles( true )
#ifdef WINCE
		, m_nTwitterGetPageCount(1)
#else
		, m_nTwitterGetPageCount(3)
#endif
#ifdef BT_TKTW
		, m_strTwitterPostFotterText(L" *TkTw*")
#else
		, m_strTwitterPostFotterText(L" *" MZ3_APP_NAME L"*")
#endif
		, m_bTwitterReloadTLAfterPost(false)
		, m_bTwitterCursorRestoreAfterTLFetch(false)
		, m_nTwitterRTStyle(0)
		, m_bUseDevVerCheck(false)
		, m_bShowNetProgressBar(false)
		, m_bShowBalloonOnNewTL(true)
		, m_bodyListIconSize(BODYLIST_ICONSIZE_AUTO)
		, m_bUseClearTypeFont(true)
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

private:
	void Sync(bool bSave);

public:
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

#ifdef BT_MZ3
	GETPAGE_TYPE GetPageType() { return m_GetPageType; }
	void SetPageType(GETPAGE_TYPE value) { m_GetPageType = value; }
#endif

	bool IsBootCheckMnC()			{ return m_bBootCheckMnC; }			///< �N�����̃`�F�b�N
	void SetBootCheckMnC(bool flag) { m_bBootCheckMnC = flag; }			///< �N�����̃`�F�b�N�̐ݒ�

	LPCTSTR GetQuoteMark()				{ return m_quoteMark; }			///< ���p�����̎擾
	void SetQuoteMark( LPCTSTR mark )	{ m_quoteMark = mark; }			///< ���p�����̐ݒ�

	BOOL IsUseBgImage()					{ return m_bUseBgImage; }		///< �w�i�摜���g���H
	void SetUseBgImage(BOOL flag)		{ m_bUseBgImage = flag; }		///< �w�i�摜���g�����ǂ����̐ݒ�

	int  GetFontHeight()				{ return m_fontHeight; }		///< �t�H���g�T�C�Y�̎擾
	
	/// �t�H���g�T�C�Y(pt���Z)��pixel���Z�l(local DPI���Z)�ŕԋp����
	int  GetFontHeightByPixel(int iDPI)	{
		return ::MulDiv(m_fontHeight, iDPI, 72);
	}
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
		return normalizeRange( n, 5, 50 );
	}

	/// ���������莞�Ԃ̐��K��
	static int normalizeLongReturnRangeMSec( int msec ) {
		return normalizeRange( msec, 100, 1000 );
	}

	/// ����擾�Ԋu�̐��K��
	static int normalizeIntervalCheckSec( int sec ) {
		return normalizeRange( sec, 10, 600 );
	}

	/// Twitter���[�h���̃X�e�[�^�X�s��
	static int normalizeTwitterStatusLineCount( int n ) {
		return normalizeRange( n, 2, 3 );
	}

	/// Twitter���[�h���̍ő�擾�y�[�W��
	static int normalizeTwitterGetPageCount( int n ) {
		return normalizeRange( n, 1, 5 );
	}
};

}