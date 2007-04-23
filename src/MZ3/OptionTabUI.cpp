// OptionTabUI.cpp : 実装ファイル
//

#include "stdafx.h"
#include "MZ3.h"
#include "OptionTabUI.h"
#include "ReportView.h"
#include "ChooseFontDlg.h"
#include "util.h"

// COptionTabUI ダイアログ

IMPLEMENT_DYNAMIC(COptionTabUI, CPropertyPage)

COptionTabUI::COptionTabUI()
	: CPropertyPage(COptionTabUI::IDD)
{

}

COptionTabUI::~COptionTabUI()
{
}

void COptionTabUI::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(COptionTabUI, CPropertyPage)
	ON_BN_CLICKED(IDC_RESET_TAB_BUTTON, &COptionTabUI::OnBnClickedResetTabButton)
	ON_BN_CLICKED(IDC_SELECT_FONT_BUTTON, &COptionTabUI::OnBnClickedSelectFontButton)
END_MESSAGE_MAP()


// COptionTabUI メッセージ ハンドラ
BOOL COptionTabUI::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	// 背景On/Off
	CheckDlgButton( IDC_USE_BGIMAGE_CHECK, theApp.m_optionMng.IsUseBgImage() ? BST_CHECKED : BST_UNCHECKED );

	// フォント（大）
	SetDlgItemText( IDC_FONT_BIG_EDIT, util::int2str(theApp.m_optionMng.m_fontHeightBig) );
	// フォント（中）
	SetDlgItemText( IDC_FONT_MEDIUM_EDIT, util::int2str(theApp.m_optionMng.m_fontHeightMedium) );
	// フォント（小）
	SetDlgItemText( IDC_FONT_SMALL_EDIT, util::int2str(theApp.m_optionMng.m_fontHeightSmall) );

	// 左ソフトキーの有効・無効
	CheckDlgButton( IDC_USE_LEFTSOFTKEY_CHECK, theApp.m_optionMng.m_bUseLeftSoftKey ? BST_CHECKED : BST_UNCHECKED );

	return TRUE;
}

void COptionTabUI::OnOK()
{
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

	// 左ソフトキーの有効・無効
	theApp.m_optionMng.m_bUseLeftSoftKey = IsDlgButtonChecked( IDC_USE_LEFTSOFTKEY_CHECK ) == BST_CHECKED ? true : false;

	CPropertyPage::OnOK();
}

void COptionTabUI::OnBnClickedResetTabButton()
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
void COptionTabUI::OnBnClickedSelectFontButton()
{
	CChooseFontDlg dlg;
	if( dlg.DoModal() == IDOK ) {
		theApp.m_optionMng.m_fontFace = dlg.m_strSelectedFont;
	}
}
