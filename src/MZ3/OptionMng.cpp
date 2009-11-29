/*
 * Copyright (c) MZ3 Project Team All rights reserved.
 * Code licensed under the BSD License:
 * http://www.mz3.jp/license.txt
 */
// OptionMng.cpp : �����t�@�C��
//

#include "stdafx.h"
#include "MZ3.h"
#include "OptionMng.h"
#include "IniFile.h"
#include "util.h"

/// �I�v�V�����f�[�^
namespace option {

// Option �����o�֐�

/**
 * ini �t�@�C���̓���(bool)
 */
inline void SyncIniValue(inifile::IniFile& inifile, bool bSave, bool& targetValue, const char* key, const char* section, bool defaultValue)
{
	if (bSave) {
		inifile.SetValue(key, (LPCSTR)util::int2str_a(targetValue ? 1 : 0), section);
	} else {
		std::string s = inifile.GetValue(key, section).c_str();
		if (s.empty()) {
			// �����l
			targetValue = defaultValue;
		} else {
			targetValue = atoi( s.c_str() ) ? true : false;
		}
	}
}

/**
 * ini �t�@�C���̓���(CString)
 */
inline void SyncIniValue(inifile::IniFile& inifile, bool bSave, CString& targetValue, const char* key, const char* section, const CString defaultValue)
{
	if (bSave) {
		inifile.SetValue(util::my_mbstowcs(key).c_str(), targetValue, section);
	} else {
		std::string s = inifile.GetValue(key, section).c_str();
		if (s.empty()) {
			// �����l
			targetValue = defaultValue;
		} else {
			targetValue = util::my_mbstowcs( s.c_str() ).c_str();
		}
	}
}

/**
 * ini �t�@�C���̓���(int)
 */
inline void SyncIniValue(inifile::IniFile& inifile, bool bSave, int& targetValue, const char* key, const char* section, int defaultValue)
{
	if (bSave) {
		inifile.SetValue(key, (LPCSTR)util::int2str_a(targetValue), section);
	} else {
		std::string s = inifile.GetValue(key, section).c_str();
		if (s.empty()) {
			// �����l
			targetValue = defaultValue;
		} else {
			targetValue = atoi( s.c_str() );
		}
	}
}
inline void SyncIniValue(inifile::IniFile& inifile, bool bSave, bool& targetValue, const char* key, const char* section)
{
	SyncIniValue(inifile, bSave, targetValue, key, section, targetValue);
}
inline void SyncIniValue(inifile::IniFile& inifile, bool bSave, CString& targetValue, const char* key, const char* section)
{
	SyncIniValue(inifile, bSave, targetValue, key, section, targetValue);
}
inline void SyncIniValue(inifile::IniFile& inifile, bool bSave, int& targetValue, const char* key, const char* section)
{
	SyncIniValue(inifile, bSave, targetValue, key, section, targetValue);
}

/**
 * �ۑ�/�Ǎ�����
 */
void Option::Sync(bool bSave)
{
	inifile::IniFile inifile;

	// ProMode
	if (theApp.m_bProMode && !bSave) {
		// �X�L���̃f�t�H���g�l��ύX
#ifdef WINCE
		m_strSkinname = L"dark";
#endif
	}

	//--- ��������
	if (bSave) {
		// �ۑ����̏�������
	} else {
		// �Ǎ����̏�������
		const CString& fileName = theApp.m_filepath.inifile;

		CFileStatus rStatus;
		if (CFile::GetStatus(fileName, rStatus) == FALSE) {
			inifile::StaticMethod::Create( util::my_wcstombs((LPCTSTR)fileName).c_str() );
		}

		if(! inifile.Load( theApp.m_filepath.inifile ) ) {
	//		return;
		}
	}

	//--- General
	SyncIniValue(inifile, bSave, (int&)m_StartupTransitionDoneType, "StartupMessageDoneType", "General" );
	SyncIniValue(inifile, bSave, m_bConfirmOpenURL, "ConfirmOpenURL", "General" );
//	SyncIniValue(inifile, bSave, m_bConvertUrlForMixiMobile, "ConvertUrlForMixiMobile", "General" );
	SyncIniValue(inifile, bSave, m_bUseDevVerCheck, "UseDevVerCheck", "General");

	// �N�����ɊG�����t�@�C���������擾����
	SyncIniValue(inifile, bSave, m_bAutoDownloadEmojiImageFiles, "AutoDownloadEmojiImageFiles", "General" );

	//--- Proxy
	SyncIniValue(inifile, bSave, m_bUseProxy,		"Use",			  "Proxy");
	SyncIniValue(inifile, bSave, m_bUseGlobalProxy, "UseGlobalProxy", "Proxy");

	SyncIniValue(inifile, bSave, m_proxyServer,     "Server",		  "Proxy");
	SyncIniValue(inifile, bSave, m_proxyPort,       "Port",		      "Proxy");
	SyncIniValue(inifile, bSave, m_proxyUser,       "User",		      "Proxy");
	SyncIniValue(inifile, bSave, m_proxyPassword,   "Password",		  "Proxy");

	//--- Page
	SyncIniValue(inifile, bSave, (int&)m_GetPageType, "GetType", "Page");

	//--- Boot
	SyncIniValue(inifile, bSave, m_bBootCheckMnC, "CheckMnC", "Boot");

	//--- UI
	// �w�i�摜�̗L��
	SyncIniValue(inifile, bSave, m_bUseBgImage, "UseBgImage", "UI");

	// �t�H���g�T�C�Y
	if (!bSave) {
		// �����l�ݒ�
#ifdef WINCE
//		switch( theApp.GetDisplayMode() ) {
//		case SR_VGA:		m_fontHeight = 24; break;
//		case SR_QVGA:
//		default:			m_fontHeight = 12; break;
//		}
		m_fontHeight = 9;
#else
		m_fontHeight = 11;
#endif
	}
	SyncIniValue(inifile, bSave, m_fontHeight, "FontHeight", "UI");

	// �t�H���g�T�C�Y�i��j
	if (!bSave) {
		// �����l�ݒ�
#ifdef WINCE
//		switch( theApp.GetDisplayMode() ) {
//		case SR_VGA:		m_fontHeightBig = 28; break;
//		case SR_QVGA:
//		default:			m_fontHeightBig = 14; break;
//		}
		m_fontHeightBig = 11;
#else
		m_fontHeightBig = 13;
#endif
	}
	SyncIniValue(inifile, bSave, m_fontHeightBig, "FontHeight_Big", "UI");

	// �t�H���g�T�C�Y�i���j
	if (!bSave) {
		// �����l�ݒ�
		m_fontHeightMedium = m_fontHeight;
	}
	SyncIniValue(inifile, bSave, m_fontHeightMedium, "FontHeight_Medium", "UI");

	// �t�H���g�T�C�Y�i���j
	if (!bSave) {
		// �����l�ݒ�
#ifdef WINCE
//		switch( theApp.GetDisplayMode() ) {
//		case SR_VGA:		m_fontHeightSmall = 18; break;
//		case SR_QVGA:
//		default:			m_fontHeightSmall = 10; break;
//		}
		m_fontHeightSmall = 7;
#else
		m_fontHeightSmall = 9;
#endif
	}
	SyncIniValue(inifile, bSave, m_fontHeightSmall, "FontHeight_Small", "UI");

	// �t�H���g��
	SyncIniValue(inifile, bSave, m_fontFace, "FontFace", "UI");

	// Xcrawl �듮��h�~�@�\
	SyncIniValue(inifile, bSave, m_bUseXcrawlExtension, "UseXcrawlCanceler", "UI" );

	// �_�E�����[�h��̎��s�m�F��ʂ�\������H
	SyncIniValue(inifile, bSave, m_bUseRunConfirmDlg, "UseRunConfirmDlg", "UI" );

	// ���������莞��
	SyncIniValue(inifile, bSave, m_longReturnRangeMSec, "LongReturnRangeMSec", "UI" );

	// ���p����
	if (bSave) {
		// ���p����
		// �����̔��p�X�y�[�X��ۑ����邽�߁A
		// "/" �ň͂ތ`�ŕۑ�����
		inifile.SetValue( L"QuoteMark", L"/" + m_quoteMark + L"/", "UI" );
	} else {
		// ���p����
		std::string s = inifile.GetValue( "QuoteMark", "UI" );
		if( s.empty() ) {
			// �����l�����̂܂܎g��
		}else{
			m_quoteMark = s.c_str();
			// �擪�Ɩ����� "/" ���폜����
			if( m_quoteMark.Left(1) == L"/" ) {
				m_quoteMark.Delete( 0 );
			}
			if( m_quoteMark.Right(1) == L"/" ) {
				m_quoteMark.Delete( m_quoteMark.GetLength()-1 );
			}
		}
	}

	//--- UI �n
	// ���X�g�̍����i�䗦�j
	const int RATIO_MAX = 1000;
	SyncIniValue(inifile, bSave, m_nMainViewCategoryListHeightRatio, "MainViewCategoryListHeightRatio", "UI" );
	if (!bSave) {
		// normalize
		m_nMainViewCategoryListHeightRatio = normalizeRange(m_nMainViewCategoryListHeightRatio, 1, RATIO_MAX );
	}
	SyncIniValue(inifile, bSave, m_nMainViewBodyListHeightRatio, "MainViewBodyListHeightRatio", "UI" );
	if (!bSave) {
		// normalize
		m_nMainViewBodyListHeightRatio = normalizeRange( m_nMainViewBodyListHeightRatio, 1, RATIO_MAX );
	}
	SyncIniValue(inifile, bSave, m_nReportViewListHeightRatio, "ReportViewListHeightRatio", "UI" );
	if (!bSave) {
		// normalize
		m_nReportViewListHeightRatio = normalizeRange( m_nReportViewListHeightRatio, 1, RATIO_MAX );
	}
	SyncIniValue(inifile, bSave, m_nReportViewBodyHeightRatio, "ReportViewBodyHeightRatio", "UI" );
	if (!bSave) {
		// normalize
		m_nReportViewBodyHeightRatio = normalizeRange( m_nReportViewBodyHeightRatio, 1, RATIO_MAX );
	}

	// ���X�g�̃J�������i�䗦�j
	SyncIniValue(inifile, bSave, m_nMainViewBodyListCol1Ratio, "MainViewBodyListCol1Ratio", "UI" );
	if (!bSave) {
		// normalize
		m_nMainViewBodyListCol1Ratio = normalizeRange( m_nMainViewBodyListCol1Ratio, 1, RATIO_MAX );
	}
	SyncIniValue(inifile, bSave, m_nMainViewBodyListCol2Ratio, "MainViewBodyListCol2Ratio", "UI" );
	if (!bSave) {
		// normalize
		m_nMainViewBodyListCol2Ratio = normalizeRange( m_nMainViewBodyListCol2Ratio, 1, RATIO_MAX );
	}
	SyncIniValue(inifile, bSave, m_nMainViewCategoryListCol1Ratio, "MainViewCategoryListCol1Ratio", "UI" );
	if (!bSave) {
		// normalize
		m_nMainViewCategoryListCol1Ratio = normalizeRange( m_nMainViewCategoryListCol1Ratio, 1, RATIO_MAX );
	}
	SyncIniValue(inifile, bSave, m_nMainViewCategoryListCol2Ratio, "MainViewCategoryListCol2Ratio", "UI" );
	if (!bSave) {
		// normalize
		m_nMainViewCategoryListCol2Ratio = normalizeRange( m_nMainViewCategoryListCol2Ratio, 1, RATIO_MAX );
	}
	SyncIniValue(inifile, bSave, m_nReportViewListCol1Ratio, "ReportViewListCol1Ratio", "UI" );
	if (!bSave) {
		// normalize
		m_nReportViewListCol1Ratio = normalizeRange( m_nReportViewListCol1Ratio, 1, RATIO_MAX );
	}
	SyncIniValue(inifile, bSave, m_nReportViewListCol2Ratio, "ReportViewListCol2Ratio", "UI" );
	if (!bSave) {
		// normalize
		m_nReportViewListCol2Ratio = normalizeRange( m_nReportViewListCol2Ratio, 1, RATIO_MAX );
	}
	SyncIniValue(inifile, bSave, m_nReportViewListCol3Ratio, "ReportViewListCol3Ratio", "UI" );
	if (!bSave) {
		// normalize
		m_nReportViewListCol3Ratio = normalizeRange( m_nReportViewListCol3Ratio, 1, RATIO_MAX );
	}

	// ���|�[�g��ʂ̃X�N���[���^�C�v
	SyncIniValue(inifile, bSave, (int&)m_reportScrollType, "ReportScrollType", "UI" );
	if (!bSave) {
		// check
		switch (m_reportScrollType) {
		case option::Option::REPORT_SCROLL_TYPE_LINE:
		case option::Option::REPORT_SCROLL_TYPE_PAGE:
			break;
		default:
			MZ3LOGGER_ERROR( L"ReportScrollType ���K��l�ȊO�ł�" );
			m_reportScrollType = option::Option::REPORT_SCROLL_TYPE_LINE;
			break;
		}
	}

	// ���|�[�g��ʂ̃X�N���[���s��
	SyncIniValue(inifile, bSave, m_reportScrollLine, "ReportScrollLine", "UI" );
	if (!bSave) {
		// normalize
		m_reportScrollLine = normalizeRange( m_reportScrollLine, 1, 100 );
	}

	// �����r���[�̃p���X�N���[��
	SyncIniValue(inifile, bSave, m_bUseRan2PanScrollAnimation, "UseRan2PanScrollAnimation", "UI" );

	// ���h���b�O�ł̍��ڈړ�
	SyncIniValue(inifile, bSave, m_bUseRan2HorizontalDragMove, "UseRan2HorizontalDragMove", "UI" );

	// �_�u���N���b�N�ł̍��ڈړ�
	SyncIniValue(inifile, bSave, m_bUseRan2DoubleClickMove, "UseRan2DoubleClickMove", "UI" );

	// MZ3(WM)�Ńs�N�Z���P�ʃX�N���[�������s���邩�H
	SyncIniValue(inifile, bSave, m_bListScrollByPixelOnMZ3, "ListScrollByPixelOnMZ3", "UI");

	// �X�L����
	SyncIniValue(inifile, bSave, m_strSkinname, "SkinName", "UI" );

#ifndef WINCE
	// �E�B���h�E�ʒu�E�T�C�Y
	SyncIniValue(inifile, bSave, m_strWindowPos, "WindowPos", "UI" );
#endif

	// ����擾�Ԋu
	SyncIniValue(inifile, bSave, m_nIntervalCheckSec, "IntervalCheckSec", "UI" );
	if(!bSave) {
		// normalize
		m_nIntervalCheckSec = normalizeIntervalCheckSec(m_nIntervalCheckSec);
	}

	// �O��I�����̃^�u�̃C���f�b�N�X
	SyncIniValue(inifile, bSave, m_lastTopPageTabIndex, "LastTopPageTabIndex", "UI" );

	// �O��I�����̃J�e�S���̃C���f�b�N�X
	SyncIniValue(inifile, bSave, m_lastTopPageCategoryIndex, "LastTopPageCategoryIndex", "UI" );

	// Treo�p�̉�ʐߖ񃂁[�h::�y�C���̃��x����\��
	SyncIniValue(inifile, bSave, m_killPaneLabel, "KillPaneLabel", "UI" );

	//--- Log �֘A
	// �ۑ��t���O
	SyncIniValue(inifile, bSave, m_bSaveLog, "SaveLog", "Log" );

	// ���O�t�H���_
	SyncIniValue(inifile, bSave, m_logFolder, "LogFolder", "Log" );

	// �f�o�b�O���[�h �t���O
	SyncIniValue(inifile, bSave, m_bDebugMode, "DebugMode", "Log" );

	//--- Net
	// ��M�o�b�t�@�T�C�Y
	SyncIniValue(inifile, bSave, m_recvBufSize, "RecvBufferSize", "Net");

	// �����ڑ�
	SyncIniValue(inifile, bSave, m_bUseAutoConnection, "AutoConnect", "Net");

	// ���f�[�^��M��
	SyncIniValue(inifile, bSave, m_totalRecvBytes, "TotalRecvBytes", "Net");

	// User-Agent
	SyncIniValue(inifile, bSave, m_strUserAgent, "UserAgent", "Net" );

	// �ʐM���̃v���O���X�o�[�\��
	SyncIniValue(inifile, bSave, m_bShowNetProgressBar, "ShowNetProgressBar", "Net");

	//--- ���C�����
	// �g�s�b�N���ɃA�C�R����\������H
	SyncIniValue(inifile, bSave, m_bShowMainViewIcon, "ShowMainViewIcon", "MainView");

	// ���[�U��R�~���j�e�B�̉摜��\������H
	SyncIniValue(inifile, bSave, m_bShowMainViewMiniImage, "ShowMainViewMiniImage", "MainView");
	
	// �摜�̎����擾
	SyncIniValue(inifile, bSave, m_bAutoLoadMiniImage, "AutoLoadMiniImage", "MainView");

	// �{�f�B���X�g�̃J�����̓������[�h
	SyncIniValue(inifile, bSave, m_bBodyListIntegratedColumnMode, "MainViewBodyListIntegratedColumnMode", "MainView");

	// ��y�C���̃��X�g�N���b�N���Ɏ擾����
	SyncIniValue(inifile, bSave, m_bOneClickCategoryFetchMode, "MainViewOneClickCategoryFetchMode", "MainView");

	//--- Twitter
	SyncIniValue(inifile, bSave, m_bAddSourceTextOnTwitterPost, "AddSourceTextOnTwitterPost", "Twitter");
	SyncIniValue(inifile, bSave, m_strTwitterPostFotterText, "PostFotterText", "Twitter" );
	SyncIniValue(inifile, bSave, m_nTwitterStatusLineCount, "TwitterStatusLineCount", "Twitter");
	if (!bSave) {
		// normalize
		m_nTwitterStatusLineCount = normalizeTwitterStatusLineCount(m_nTwitterStatusLineCount);
	}
	SyncIniValue(inifile, bSave, m_nTwitterGetPageCount, "GetPageCount", "Twitter");
	if (!bSave) {
		// normalize
		m_nTwitterGetPageCount = normalizeTwitterGetPageCount(m_nTwitterGetPageCount);
	}
	SyncIniValue(inifile, bSave, m_bTwitterReloadTLAfterPost, "ReloadTLAfterPost", "Twitter");

	// �I������
	if (bSave) {
		// Save
		inifile.Save( theApp.m_filepath.inifile, false );
	}
}

void Option::Load()
{
	MZ3LOGGER_DEBUG( L"�I�v�V�����ݒ�ǂݍ��݊J�n" );

	Sync(false);

	MZ3LOGGER_DEBUG( L"�I�v�V�����ݒ�ǂݍ��݊���" );
}

void Option::Save()
{
	MZ3LOGGER_DEBUG( L"�I�v�V�����ݒ�ۑ��J�n" );

	inifile::IniFile inifile;

	Sync(true);

	MZ3LOGGER_DEBUG( L"�I�v�V�����ݒ�ۑ�����" );
}

}// namespace option
