// OptionTabGetPage.cpp : 実装ファイル
//

#include "stdafx.h"
#include "MZ3.h"
#include "util.h"
#include "OptionTabGetPage.h"


// COptionTabGetPage ダイアログ

IMPLEMENT_DYNAMIC(COptionTabGetPage, CPropertyPage)

COptionTabGetPage::COptionTabGetPage()
	: CPropertyPage(COptionTabGetPage::IDD)
{

}

COptionTabGetPage::~COptionTabGetPage()
{
}

void COptionTabGetPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_RECV_BUF_COMBO, mc_RecvBufCombo);
}


BEGIN_MESSAGE_MAP(COptionTabGetPage, CPropertyPage)
END_MESSAGE_MAP()


// COptionTabGetPage メッセージ ハンドラ

void COptionTabGetPage::OnOK()
{
	Save();

	CPropertyPage::OnOK();
}

BOOL COptionTabGetPage::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	// コンボボックスの初期化
	{
		int list[] = { 256, 512, 1024, 2048, 4096, 8192, -1 };

		mc_RecvBufCombo.ResetContent();

		for( int i=0; list[i] != -1; i++ ) {
			CString s;
			s.Format( L"%d Byte", list[i] );
			int idx = mc_RecvBufCombo.InsertString( i, s );
			mc_RecvBufCombo.SetItemData( i, list[i] );
		}
	}

	// オプションをダイアログに反映する
	Load();

	return TRUE;
}

/**
 * theApp.m_optionMng からダイアログに変換する
 */
void COptionTabGetPage::Load()
{
	// 取得種別
	if (theApp.m_optionMng.GetPageType() == 0) {
		((CButton*)GetDlgItem(IDC_PAGE_GETALL_RADIO))->SetCheck(BST_CHECKED);
	}
	else {
		((CButton*)GetDlgItem(IDC_PAGE_GETLATEST_RADIO))->SetCheck(BST_CHECKED);
	}

	// 受信バッファサイズ
	// リストになければ末尾に追加。
	const int recvBufSize = theApp.m_optionMng.GetRecvBufSize();
	bool bFound = false;
	for( int i=0; i<mc_RecvBufCombo.GetCount(); i++ ) {
		int bufSize = mc_RecvBufCombo.GetItemData( i );
		if( bufSize == recvBufSize ) {
			// 一致したので選択
			mc_RecvBufCombo.SetCurSel( i );
			bFound = true;
			break;
		}
	}
	if( !bFound ) {
		// 見つからなかったので末尾に追加、選択。
		CString s;
		s.Format( L"%d Byte", recvBufSize );
		int idx = mc_RecvBufCombo.InsertString( mc_RecvBufCombo.GetCount(), s );
		mc_RecvBufCombo.SetItemData( idx, recvBufSize );

		mc_RecvBufCombo.SetCurSel( idx );
	}
}

/**
 * ダイアログのデータを theApp.m_optionMng に変換する
 */
void COptionTabGetPage::Save()
{
	// 取得種別
	GETPAGE_TYPE type;
	if (((CButton*)GetDlgItem(IDC_PAGE_GETALL_RADIO))->GetCheck() == BST_CHECKED) {
		type = GETPAGE_ALL;
	}
	else {
		type = GETPAGE_LATEST10;
	}
	theApp.m_optionMng.SetPageType(type);

	// 受信バッファサイズ
	theApp.m_optionMng.SetRecvBufSize( mc_RecvBufCombo.GetItemData( mc_RecvBufCombo.GetCurSel() ) );
}
