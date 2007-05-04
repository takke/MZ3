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

void Option::Load()
{
	MZ3LOGGER_DEBUG( L"�I�v�V�����ݒ�ǂݍ��݊J�n" );

	const CString& fileName = theApp.m_filepath.inifile;

	inifile::IniFile inifile;

	CFileStatus rStatus;
	if (CFile::GetStatus(fileName, rStatus) == FALSE) {
		inifile::StaticMethod::Create( util::my_wcstombs((LPCTSTR)fileName).c_str() );
	}

	if(! inifile.Load( theApp.m_filepath.inifile ) ) {
		return;
	}

	if (inifile.SectionExists("Proxy") != FALSE) {
		// �v���L�V�Z�N�V�������������ꍇ
		m_proxyUse = atoi( inifile.GetValue("Use", "Proxy").c_str() );

		m_proxyServer   = util::my_mbstowcs( inifile.GetValue("Server", "Proxy") ).c_str();
		m_proxyPort     = atoi( inifile.GetValue("Port", "Proxy").c_str() );
		m_proxyUser     = util::my_mbstowcs( inifile.GetValue("User", "Proxy") ).c_str();
		m_proxyPassword = util::my_mbstowcs( inifile.GetValue("Password", "Proxy") ).c_str();

	}
	TRACE(_T("Proxy[%d] %s:%d\n"), m_proxyUse, m_proxyServer, m_proxyPort);

	if (inifile.SectionExists("Page") != FALSE) {
		m_GetPageType = (GETPAGE_TYPE)atoi( inifile.GetValue("GetType", "Page").c_str() );
	}

	if (inifile.SectionExists("Boot") != FALSE) {
		m_bBootCheckMnC = (BOOL)atoi(inifile.GetValue("CheckMnC", "Boot").c_str());
	}

	if (inifile.SectionExists("UI") != FALSE) {
		// UI �Z�N�V����

		// �w�i�摜�̗��p
		std::string s;
		s = inifile.GetValue("UseBgImage", "UI");
		if( s.empty() ) {
			// �����l
			m_bUseBgImage = TRUE;
		}else{
			m_bUseBgImage = (BOOL)atoi(s.c_str());
		}

		// �t�H���g�T�C�Y
		s = inifile.GetValue("FontHeight", "UI");
		if( s.empty() ) {
			// �����l
			m_fontHeight = 0;
		}else{
			m_fontHeight = (BOOL)atoi(s.c_str());
		}
		// �t�H���g�T�C�Y�i��j
		s = inifile.GetValue("FontHeight_Big", "UI");
		if( s.empty() ) {
			// �����l
//			m_fontHeightBig = 28;
		}else{
			m_fontHeightBig = (BOOL)atoi(s.c_str());
		}

		// �t�H���g�T�C�Y�i���j
		s = inifile.GetValue("FontHeight_Medium", "UI");
		if( s.empty() ) {
			// �����l
			m_fontHeightMedium = m_fontHeight;
		}else{
			m_fontHeightMedium = (BOOL)atoi(s.c_str());
		}

		// �t�H���g�T�C�Y�i���j
		s = inifile.GetValue("FontHeight_Small", "UI");
		if( s.empty() ) {
			// �����l
//			m_fontHeightSmall = 18;
		}else{
			m_fontHeightSmall = (BOOL)atoi(s.c_str());
		}

		// �t�H���g��
		m_fontFace = inifile.GetValue("FontFace", "UI").c_str();

		// ���\�t�g�L�[�L���H
		s = inifile.GetValue( "UseLeftSoftKey", "UI" );
		if( s.empty() ) {
			// �����l�����̂܂܎g��
		}else{
			m_bUseLeftSoftKey = (s == "1");
		}

		// ���p����
		s = inifile.GetValue( "QuoteMark", "UI" );
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

		// ���X�g�̍����i�䗦�j
		const int RATIO_MAX = 1000;
		s = inifile.GetValue( "MainViewCategoryListHeightRatio", "UI" );
		if(! s.empty() ) {
			m_nMainViewCategoryListHeightRatio = normalizeRange( atoi(s.c_str()), 1, RATIO_MAX );
		}
		s = inifile.GetValue( "MainViewBodyListHeightRatio", "UI" );
		if(! s.empty() ) {
			m_nMainViewBodyListHeightRatio = normalizeRange( atoi(s.c_str()), 1, RATIO_MAX );
		}
		s = inifile.GetValue( "ReportViewListHeightRatio", "UI" );
		if(! s.empty() ) {
			m_nReportViewListHeightRatio = normalizeRange( atoi(s.c_str()), 1, RATIO_MAX );
		}
		s = inifile.GetValue( "ReportViewBodyHeightRatio", "UI" );
		if(! s.empty() ) {
			m_nReportViewBodyHeightRatio = normalizeRange( atoi(s.c_str()), 1, RATIO_MAX );
		}

		// ���X�g�̃J�������i�䗦�j
		s = inifile.GetValue( "MainViewBodyListCol1Ratio", "UI" );
		if(! s.empty() ) {
			m_nMainViewBodyListCol1Ratio = normalizeRange( atoi(s.c_str()), 1, RATIO_MAX );
		}
		s = inifile.GetValue( "MainViewBodyListCol2Ratio", "UI" );
		if(! s.empty() ) {
			m_nMainViewBodyListCol2Ratio = normalizeRange( atoi(s.c_str()), 1, RATIO_MAX );
		}
		s = inifile.GetValue( "MainViewCategoryListCol1Ratio", "UI" );
		if(! s.empty() ) {
			m_nMainViewCategoryListCol1Ratio = normalizeRange( atoi(s.c_str()), 1, RATIO_MAX );
		}
		s = inifile.GetValue( "MainViewCategoryListCol2Ratio", "UI" );
		if(! s.empty() ) {
			m_nMainViewCategoryListCol2Ratio = normalizeRange( atoi(s.c_str()), 1, RATIO_MAX );
		}
		s = inifile.GetValue( "ReportViewListCol1Ratio", "UI" );
		if(! s.empty() ) {
			m_nReportViewListCol1Ratio = normalizeRange( atoi(s.c_str()), 1, RATIO_MAX );
		}
		s = inifile.GetValue( "ReportViewListCol2Ratio", "UI" );
		if(! s.empty() ) {
			m_nReportViewListCol2Ratio = normalizeRange( atoi(s.c_str()), 1, RATIO_MAX );
		}
		s = inifile.GetValue( "ReportViewListCol3Ratio", "UI" );
		if(! s.empty() ) {
			m_nReportViewListCol3Ratio = normalizeRange( atoi(s.c_str()), 1, RATIO_MAX );
		}

		// ���|�[�g��ʂ̃X�N���[���^�C�v
		s = inifile.GetValue( "ReportScrollType", "UI" );
		if(! s.empty() ) {
			int type = atoi(s.c_str());
			switch( type ) {
			case option::Option::REPORT_SCROLL_TYPE_LINE:
			case option::Option::REPORT_SCROLL_TYPE_PAGE:
				m_reportScrollType = (option::Option::REPORT_SCROLL_TYPE) type;
				break;
			default:
				MZ3LOGGER_ERROR( L"ReportScrollType ���K��l�ȊO�ł�" );
				break;
			}
		}

		// ���|�[�g��ʂ̃X�N���[���s��
		s = inifile.GetValue( "ReportScrollLine", "UI" );
		if(! s.empty() ) {
			m_reportScrollLine = normalizeRange( atoi(s.c_str()), 1, 100 );
		}
	}

	if (inifile.SectionExists("Log") != FALSE) {
		// Log �Z�N�V����

		// �ۑ��t���O
		std::string s = inifile.GetValue( "SaveLog", "Log" );
		if( s.empty() ) {
			// �����l�����̂܂܎g��
		}else{
			m_bSaveLog = (s == "1");
		}

		// ���O�t�H���_
		m_logFolder = inifile.GetValue("LogFolder", "Log").c_str();
	}

	if (inifile.SectionExists("Net") != FALSE) {
		// ��M�o�b�t�@�T�C�Y
		m_recvBufSize = atoi( inifile.GetValue("RecvBufferSize", "Net").c_str() );

		// ���f�[�^��M��
		m_totalRecvBytes = atoi( inifile.GetValue("TotalRecvBytes", "Net").c_str() );
	}

	MZ3LOGGER_DEBUG( L"�I�v�V�����ݒ�ǂݍ��݊���" );
}

void Option::Save()
{
	MZ3LOGGER_DEBUG( L"�I�v�V�����ݒ�ۑ��J�n" );

	inifile::IniFile inifile;

	inifile.SetValue("Use", (LPCSTR)util::int2str_a(m_proxyUse), "Proxy");
	inifile.SetValue(L"Server", m_proxyServer, "Proxy");
	inifile.SetValue("Port", (LPCSTR)util::int2str_a(m_proxyPort), "Proxy");

	inifile.SetValue(L"User", m_proxyUser, "Proxy");
	inifile.SetValue(L"Password", m_proxyPassword, "Proxy");
	inifile.SetValue("GetType", (LPCSTR)util::int2str_a(m_GetPageType), "Page");
	inifile.SetValue("CheckMnC", (LPCSTR)util::int2str_a(m_bBootCheckMnC), "Boot");

	//--- UI �n
	CStringA s;
	// �w�i�摜�̗��p
	inifile.SetValue( "UseBgImage", (LPCSTR)util::int2str_a(m_bUseBgImage), "UI");

	// �t�H���g�T�C�Y
	inifile.SetValue( "FontHeight", (LPCSTR)util::int2str_a(m_fontHeight), "UI");

	// �t�H���g�T�C�Y�i��j
	inifile.SetValue( "FontHeight_Big", (LPCSTR)util::int2str_a(m_fontHeightBig), "UI");
	// �t�H���g�T�C�Y�i���j
	inifile.SetValue( "FontHeight_Medium", (LPCSTR)util::int2str_a(m_fontHeightMedium), "UI");
	// �t�H���g�T�C�Y�i���j
	inifile.SetValue( "FontHeight_Small", (LPCSTR)util::int2str_a(m_fontHeightSmall), "UI");

	// �t�H���g��
	inifile.SetValue( L"FontFace", m_fontFace, "UI");
	// ���\�t�g�L�[�L���H
	inifile.SetValue( "UseLeftSoftKey", m_bUseLeftSoftKey ? "1" : "0", "UI" );

	// ���p����
	// �����̔��p�X�y�[�X��ۑ����邽�߁A
	// "/" �ň͂ތ`�ŕۑ�����
	inifile.SetValue( L"QuoteMark", L"/" + m_quoteMark + L"/", "UI" );

	// ���X�g�̍����i�䗦�j
	inifile.SetValue( "MainViewCategoryListHeightRatio", (LPCSTR)util::int2str_a(m_nMainViewCategoryListHeightRatio), "UI" );
	inifile.SetValue( "MainViewBodyListHeightRatio",     (LPCSTR)util::int2str_a(m_nMainViewBodyListHeightRatio), "UI" );
	inifile.SetValue( "ReportViewListHeightRatio", (LPCSTR)util::int2str_a(m_nReportViewListHeightRatio), "UI" );
	inifile.SetValue( "ReportViewBodyHeightRatio", (LPCSTR)util::int2str_a(m_nReportViewBodyHeightRatio), "UI" );

	// ���X�g�̃J�������i�䗦�j
	inifile.SetValue( "MainViewBodyListCol1Ratio", (LPCSTR)util::int2str_a(m_nMainViewBodyListCol1Ratio), "UI" );
	inifile.SetValue( "MainViewBodyListCol2Ratio", (LPCSTR)util::int2str_a(m_nMainViewBodyListCol2Ratio), "UI" );
	inifile.SetValue( "MainViewCategoryListCol1Ratio", (LPCSTR)util::int2str_a(m_nMainViewCategoryListCol1Ratio), "UI" );
	inifile.SetValue( "MainViewCategoryListCol2Ratio", (LPCSTR)util::int2str_a(m_nMainViewCategoryListCol2Ratio), "UI" );

	inifile.SetValue( "ReportViewListCol1Ratio", (LPCSTR)util::int2str_a(m_nReportViewListCol1Ratio), "UI" );
	inifile.SetValue( "ReportViewListCol2Ratio", (LPCSTR)util::int2str_a(m_nReportViewListCol2Ratio), "UI" );
	inifile.SetValue( "ReportViewListCol3Ratio", (LPCSTR)util::int2str_a(m_nReportViewListCol3Ratio), "UI" );

	// ���|�[�g��ʂ̃X�N���[���^�C�v
	inifile.SetValue( "ReportScrollType", (LPCSTR)util::int2str_a(m_reportScrollType), "UI" );

	// ���|�[�g��ʂ̃X�N���[���s��
	inifile.SetValue( "ReportScrollLine", (LPCSTR)util::int2str_a(m_reportScrollLine), "UI" );

	//--- Log �֘A
	// �ۑ��t���O
	inifile.SetValue( "SaveLog", m_bSaveLog ? "1" : "0", "Log" );

	// �ۑ���t�H���_
	inifile.SetValue( L"LogFolder", m_logFolder, "Log");

	// ��M�o�b�t�@�T�C�Y
	inifile.SetValue( "RecvBufferSize", (LPCSTR)util::int2str_a(m_recvBufSize), "Net" );

	// ���f�[�^��M��
	inifile.SetValue( "TotalRecvBytes", (LPCSTR)util::int2str_a(m_totalRecvBytes), "Net" );

	// Save
	inifile.Save( theApp.m_filepath.inifile, false );

	MZ3LOGGER_DEBUG( L"�I�v�V�����ݒ�ۑ�����" );
}

}// namespace option
