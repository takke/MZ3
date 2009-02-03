/*
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */
// MZ3.h : MZ3 �A�v���P�[�V�����̃��C�� �w�b�_�[ �t�@�C��
//
#pragma once

#ifndef __AFXWIN_H__
#error "PCH �̂��̃t�@�C�����C���N���[�h����O�ɁA'stdafx.h' ���C���N���[�h���܂�"
#endif

#include "resourceppc.h"
#include "OptionMng.h"
#include "BookmarkMng.h"
#include "LoginMng.h"
#include "IniFile.h"
#include "InetAccess.h"
#include "GroupItem.h"
#include "Mz3GroupData.h"
#include "CategoryItem.h"
#include "SimpleLogger.h"
#include "MZ3BackgroundImage.h"
#include "MZ3SkinInfo.h"
#include "EmojiMap.h"
#include "ImageCacheManager.h"
#include "AccessTypeInfo.h"

#ifndef WINCE
	#include "gdiplus.h"
	using namespace Gdiplus;
#endif

// �R���\�[���p�f�o�b�O���O�o�͗p�}�N��
#ifdef CONSOLE_DEBUG
# define MZ3_TRACE wprintf
#else
# define MZ3_TRACE __noop
#endif

// ���O�o�͗p�}�N��
#define MZ3LOGGER_FATAL(msg)			SIMPLELOGGER_FATAL(theApp.m_logger,msg)
#define MZ3LOGGER_ERROR(msg)			SIMPLELOGGER_ERROR(theApp.m_logger,msg)
#define MZ3LOGGER_INFO(msg)				SIMPLELOGGER_INFO (theApp.m_logger,msg)
#define MZ3LOGGER_DEBUG(msg)			SIMPLELOGGER_DEBUG(theApp.m_logger,msg)
#define MZ3LOGGER_TRACE(msg)			SIMPLELOGGER_TRACE(theApp.m_logger,msg)
#define MZ3LOGGER_IS_DEBUG_ENABLED()	theApp.m_logger.isDebugEnabled()

#define TOOLBAR_HEIGHT 24

/// ���s��WM�f�o�C�X�ŗ��p����Ă���𑜓x�̗񋓁B(Phone edition�͏���)
enum ScreenResolution { 
	SR_QVGA,			///< 320x240
	SR_SQUARE240,		///< 240x240
	SR_VGA,				///< 640x480
};

class CMZ3View;
class CReportView;
class CWriteView;
class CDownloadView;
class CMZ3BackgroundImage;
class MouseGestureManager;

// CMZ3App:
// ���̃N���X�̎����ɂ��ẮAMZ3.cpp ���Q�Ƃ��Ă��������B
//

class CMZ3App : public CWinApp
{
public:
	AccessTypeInfo	m_accessTypeInfo;	///< �A�N�Z�X��ʖ���MZ3/4�̐U�镑�����`������

	lua_State*		m_luaState;			///< the Lua object


	/// MZ3 �ŗ��p���Ă���t�@�C���̃p�X
	class FilePath {
	public:
		CString		mz3logfile;		///< MZ3 �̃��O�t�@�C��
		CString		temphtml;		///< HTML �p�ꎞ�t�@�C��(SJIS)�̃p�X
		CString		logfile;		///< ���O�t�@�C���iINI�t�@�C���j�̃p�X
		CString		inifile;		///< �I�v�V�����pINI�t�@�C���̃p�X
		CString		groupfile;		///< �O���[�v��`�t�@�C���iINI�t�@�C���j�̃p�X
		CString		helpfile;		///< Readme.txt �̃p�X
		CString		historyfile;	///< ChangeLog.txt �̃p�X
		CString		tempdraftfile;	///< �������ݎ��s���̉������t�@�C��
		CString		emojifile;		///< �G������`�t�@�C��

		CString		skinFolder;		///< �X�L���p�t�H���_�̃p�X

		// �t�H���_
		CString		downloadFolder;	///< �_�E�����[�h�ς݃t�@�C���p�t�H���_�̃p�X
		CString		logFolder;		///< ���O�p���ʃt�H���_�̃p�X

		CString		imageFolder;	///< �摜�̃_�E�����[�h�ς݃t�@�C���p�t�H���_�̃p�X

		CStringArray deleteTargetFolders;	///< ����N���[���A�b�v�Ώۃt�H���_���X�g

		void init();
		void init_logpath();
	};

public:
	CMZ3App();
	~CMZ3App();

	// �I�[�o�[���C�h
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

	// ����
public:
	afx_msg void OnAppAbout();

	DECLARE_MESSAGE_MAP()

public:
	DWORD				m_dpi;					///< DPI
	ScreenResolution	m_currentDisplayMode;	///< �𑜓x

	/// �v���b�g�t�H�[���p�̃t���O
	BOOL				m_bPocketPC;
	BOOL				m_bSmartphone;
	BOOL				m_bWinMoFiveOh;
	BOOL				m_bWinMo2003;
	BOOL				m_bWinMo2003_SE;

public:
	//--- ���K�[
	CSimpleLogger		m_logger;				///< ���O�o�̓I�u�W�F�N�g

	//--- UI
	CFont				m_font;					///< ���ʃt�H���g
	CMZ3View*			m_pMainView;			///< ���C���r���[
	CReportView*		m_pReportView;			///< ���|�[�g�r���[
	CWriteView*			m_pWriteView;			///< �������݃r���[
	CDownloadView*		m_pDownloadView;		///< �_�E�����[�h�r���[

	MouseGestureManager* m_pMouseGestureManager;///< �}�E�X�W�F�X�`���Ǘ�

	CMZ3SkinInfo		m_skininfo;				///< �X�L�����

	CMZ3BackgroundImage	m_bgImageMainCategoryCtrl;	///< ���C���r���[�A�J�e�S���R���g���[���̔w�i�p�r�b�g�}�b�v
	CMZ3BackgroundImage	m_bgImageMainBodyCtrl;		///< ���C���r���[�A�{�f�B�R���g���[���̔w�i�p�r�b�g�}�b�v
	CMZ3BackgroundImage	m_bgImageReportListCtrl;	///< ���|�[�g�r���[�A���X�g�R���g���[���̔w�i�p�r�b�g�}�b�v

	int					m_newMessageCount;		///< �V�����b�Z�[�W��
	int					m_newCommentCount;		///< �V���R�����g��
 	int					m_newApplyCount;		///< ���F�҂���

	FilePath			m_filepath;				///< MZ3 �ŗ��p���Ă���t�@�C���p�X�Q

	option::Option		m_optionMng;			///< �I�v�V�����f�[�^
	option::Login		m_loginMng;				///< ���O�C���f�[�^
	option::Bookmark	m_bookmarkMng;			///< �u�b�N�}�[�N�f�[�^

	inifile::IniFile	m_readlog;				///< ���ǊǗ����O�p INI �t�@�C��

	//--- mixi �f�[�^
	Mz3GroupData		m_root;					///< mixi �p�f�[�^�̃��[�g�v�f

	//--- �G�����}�b�v
	EmojiMapList		m_emoji;				///< mixi �G�����}�b�v

	//--- �摜
	ImageCacheManager	m_imageCache;			///< �O���[�o���摜�L���b�V���i16x16�j

	//--- �ʐM�n
	CMixiData			m_mixiBeforeRelogin;	///< �ă��O�C���O�� mixi �I�u�W�F�N�g
	ACCESS_TYPE			m_accessType;			///< ���݂̃A�N�Z�X���

	CMixiData			m_mixi4recv;			///< ��M���ɗ��p���� mixi �f�[�^
	CInetAccess			m_inet;					///< �l�b�g�ڑ��I�u�W�F�N�g

#ifndef WINCE
	// GDI+�������g�[�N��
	GdiplusStartupInput gdiSI;
	ULONG_PTR			gdiToken;
#endif

public:
	void ChangeView(CView*);
	CString GetAppDirPath();

//	CString MakeLoginUrl( LPCTSTR nextUrl=L"/home.pl" );
//	CString MakeLoginUrlForMixiMobile( LPCTSTR nextUrl );
	void StartMixiLoginAccess(HWND hwnd, CMixiData* data);

	BOOL EnableCommandBarButton( int nID, BOOL bEnable );
public:
	bool MakeNewFont( CFont* pBaseFont, int fontHeight, LPCTSTR fontFace );
	void ShowMixiData( CMixiData* data );

	ScreenResolution GetDisplayMode() { return m_currentDisplayMode; }
	DWORD GetDPI() { return m_dpi; }
	int GetInfoRegionHeight( int fontHeight );
	int GetTabHeight( int fontHeight );

	bool LoadSkinSetting();
	void InitPlatformFlags();
	void InitResolutionFlags();
	bool SaveGroupData(void);

	bool IsMixiLogout( ACCESS_TYPE aType );
	bool DeleteOldCacheFiles(void);
	int pt2px(int pt);
	int AddImageToImageCache(CWnd* pWnd, CMZ3BackgroundImage& srcImage, const CString& strImagePath);
	CString MakeMZ3RegularVersion(CString strVersion);
	bool MyLuaInit(void);
	bool MyLuaClose(void);
	bool MyLuaExecute(LPCTSTR szLuaStatement);
	int MyLuaErrorReport(int status);
};

extern CMZ3App theApp;
