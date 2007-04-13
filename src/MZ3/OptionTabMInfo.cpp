// MInfoTab.cpp : 実装ファイル
//

#include "stdafx.h"
#include "MZ3.h"
#include "OptionTabMInfo.h"
#include "ReportView.h"
#include "ChooseFontDlg.h"
#include "util.h"

// COptionTabMInfo ダイアログ

IMPLEMENT_DYNAMIC(COptionTabMInfo, CPropertyPage)

COptionTabMInfo::COptionTabMInfo()
	: CPropertyPage(COptionTabMInfo::IDD)
{

}

COptionTabMInfo::~COptionTabMInfo()
{
}

void COptionTabMInfo::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(COptionTabMInfo, CPropertyPage)
	ON_BN_CLICKED(IDC_RESET_TAB_BUTTON, &COptionTabMInfo::OnBnClickedResetTabButton)
	ON_BN_CLICKED(IDC_SELECT_FONT_BUTTON, &COptionTabMInfo::OnBnClickedSelectFontButton)
END_MESSAGE_MAP()


// COptionTabMInfo メッセージ ハンドラ
BOOL COptionTabMInfo::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	// 起動時の新着チェック
	((CButton*)GetDlgItem(IDC_BOOT_MNC_CHECK))->SetCheck(
		theApp.m_optionMng.IsBootCheckMnC() ? BST_CHECKED : BST_UNCHECKED );

	// 背景On/Off
	((CButton*)GetDlgItem(IDC_USE_BGIMAGE_CHECK))->SetCheck(
		theApp.m_optionMng.IsUseBgImage() ? BST_CHECKED : BST_UNCHECKED );

	// フォント（大）
	SetDlgItemText( IDC_FONT_BIG_EDIT, util::int2str(theApp.m_optionMng.m_fontHeightBig) );
	// フォント（中）
	SetDlgItemText( IDC_FONT_MEDIUM_EDIT, util::int2str(theApp.m_optionMng.m_fontHeightMedium) );
	// フォント（小）
	SetDlgItemText( IDC_FONT_SMALL_EDIT, util::int2str(theApp.m_optionMng.m_fontHeightSmall) );

	return TRUE;
}

void COptionTabMInfo::OnOK()
{
	// 起動時の新着チェック
	theApp.m_optionMng.SetBootCheckMnC(
		((CButton*)GetDlgItem(IDC_BOOT_MNC_CHECK))->GetCheck() == BST_CHECKED ? TRUE : FALSE );

	// 背景On/Off
	theApp.m_optionMng.SetUseBgImage(
		((CButton*)GetDlgItem(IDC_USE_BGIMAGE_CHECK))->GetCheck() == BST_CHECKED ? TRUE : FALSE );


	// フォント（大）
	{
		CString s;
		GetDlgItemText( IDC_FONT_BIG_EDIT, s );
		int n = _wtoi( s );
		if( n < 8  ) n = 8;
		if( n > 50 ) n = 50;
		theApp.m_optionMng.m_fontHeightBig = n;
	}
	// フォント（中）
	{
		CString s;
		GetDlgItemText( IDC_FONT_MEDIUM_EDIT, s );
		int n = _wtoi( s );
		if( n < 8  ) n = 8;
		if( n > 50 ) n = 50;
		theApp.m_optionMng.m_fontHeightMedium = n;
	}
	// フォント（小）
	{
		CString s;
		GetDlgItemText( IDC_FONT_SMALL_EDIT, s );
		int n = _wtoi( s );
		if( n < 8  ) n = 8;
		if( n > 50 ) n = 50;
		theApp.m_optionMng.m_fontHeightSmall = n;
	}

	CPropertyPage::OnOK();
}

void COptionTabMInfo::OnBnClickedResetTabButton()
{
	if( MessageBox( L"タブをリセットしてもよろしいですか？\n（初期化後にMZ3を終了します）", 
			0, MB_YESNO | MB_ICONQUESTION ) != IDYES ) 
	{
		return;
	}

	// 初期化
	theApp.m_root.initForTopPage();

	// 再起動
	// ・・・できないのでメッセージを表示して終了。
	MessageBox( L"初期化処理が完了したためMZ3を終了します" );
	theApp.m_pReportView->SaveIndex();
	ShowWindow(SW_HIDE);
	AfxGetMainWnd()->PostMessage(WM_CLOSE);
}

/// フォント変更
void COptionTabMInfo::OnBnClickedSelectFontButton()
{
	CChooseFontDlg dlg;
	if( dlg.DoModal() == IDOK ) {
		theApp.m_optionMng.m_fontFace = dlg.m_strSelectedFont;
	}
}
