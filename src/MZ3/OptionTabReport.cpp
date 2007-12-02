// COptionTabReport.cpp : 実装ファイル
//

#include "stdafx.h"
#include "MZ3.h"
#include "util.h"
#include "OptionTabReport.h"

#ifndef SMARTPHONE2003_UI_MODEL

// COptionTabReport ダイアログ

IMPLEMENT_DYNAMIC(COptionTabReport, CPropertyPage)

COptionTabReport::COptionTabReport()
	: CPropertyPage(COptionTabReport::IDD)
{

}

COptionTabReport::~COptionTabReport()
{
}

void COptionTabReport::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(COptionTabReport, CPropertyPage)
END_MESSAGE_MAP()


// OptionTabReport メッセージ ハンドラ

BOOL COptionTabReport::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	// スクロールタイプ
	int idRadio = -1;
	switch( theApp.m_optionMng.m_reportScrollType ) {
	case option::Option::REPORT_SCROLL_TYPE_LINE:
		idRadio = IDC_SCROLL_LINE_RADIO;
		break;
	case option::Option::REPORT_SCROLL_TYPE_PAGE:
		idRadio = IDC_SCROLL_PAGE_RADIO;
		break;
	default:
		idRadio = IDC_SCROLL_PAGE_RADIO;
		break;
	}
	if( idRadio >= 0 ) {
		CheckRadioButton( IDC_SCROLL_LINE_RADIO, IDC_SCROLL_PAGE_RADIO, idRadio );
	}

	// スクロール行数
	SetDlgItemText( IDC_SCROLL_LINE_EDIT, (LPCTSTR)util::int2str(theApp.m_optionMng.m_reportScrollLine) );

	return TRUE;  // return TRUE unless you set the focus to a control
	// 例外 : OCX プロパティ ページは必ず FALSE を返します。
}

void COptionTabReport::OnOK()
{
	// スクロールタイプ
	int idRadio = GetCheckedRadioButton( IDC_SCROLL_LINE_RADIO, IDC_SCROLL_PAGE_RADIO );
	switch( idRadio ) {
	case IDC_SCROLL_LINE_RADIO:
		theApp.m_optionMng.m_reportScrollType = option::Option::REPORT_SCROLL_TYPE_LINE;
		break;
	case IDC_SCROLL_PAGE_RADIO:
		theApp.m_optionMng.m_reportScrollType = option::Option::REPORT_SCROLL_TYPE_PAGE;
		break;
	default:
		theApp.m_optionMng.m_reportScrollType = option::Option::REPORT_SCROLL_TYPE_PAGE;
		break;
	}

	// スクロール行数
	theApp.m_optionMng.m_reportScrollLine
		= option::Option::normalizeRange( GetDlgItemInt( IDC_SCROLL_LINE_EDIT ), 1, 100 );

	CPropertyPage::OnOK();
}

#endif