// ChooseFontDlg.cpp : 実装ファイル
//

#include "stdafx.h"
#include "MZ3.h"
#include "util_gui.h"
#include "ChooseFontDlg.h"


// CChooseFontDlg ダイアログ

IMPLEMENT_DYNAMIC(CChooseFontDlg, CDialog)

CChooseFontDlg::CChooseFontDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CChooseFontDlg::IDD, pParent)
{

}

CChooseFontDlg::~CChooseFontDlg()
{
}

void CChooseFontDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_FONT_LIST, m_FontList);
}


BEGIN_MESSAGE_MAP(CChooseFontDlg, CDialog)
	ON_WM_SIZE()
	ON_NOTIFY(NM_RETURN, IDC_FONT_LIST, &CChooseFontDlg::OnNMReturnFontList)
	ON_NOTIFY(LVN_KEYDOWN, IDC_FONT_LIST, &CChooseFontDlg::OnLvnKeydownFontList)
	ON_NOTIFY(NM_DBLCLK, IDC_FONT_LIST, &CChooseFontDlg::OnNMDblclkFontList)
END_MESSAGE_MAP()

/// 各フォントに対するコールバック
int CALLBACK enumFontsProc(
        ENUMLOGFONT *pLF,
        NEWTEXTMETRIC *pNM,
        DWORD n,
        LPARAM lp )
{
	CChooseFontDlg* pDlg = (CChooseFontDlg*)lp;

	int count = pDlg->m_FontList.GetItemCount();
	int idx = pDlg->m_FontList.InsertItem( count, pLF->elfLogFont.lfFaceName );
	pDlg->m_FontList.SetItemText( idx, 1, n & TRUETYPE_FONTTYPE ? L"TrueType" : L"" );

	return 1;
}


// CChooseFontDlg メッセージ ハンドラ

BOOL CChooseFontDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// フォント
//	m_FontList.SetFont( &theApp.m_font );

	// カラム追加
	m_FontList.InsertColumn( 0, L"フォント", LVCFMT_LEFT, 300, -1 );
	m_FontList.InsertColumn( 1, L"備考", LVCFMT_LEFT, 150, -1 );

	// 一行選択モードの設定
	ListView_SetExtendedListViewStyle((HWND)m_FontList.m_hWnd, LVS_EX_FULLROWSELECT);

	// フォント一覧の生成
	HDC dc = ::GetDC(HWND_DESKTOP);
	::EnumFontFamilies( dc, 0, (FONTENUMPROC)enumFontsProc, (LPARAM)this );
	::ReleaseDC(HWND_DESKTOP, dc);

	// 現在のフォントを選択する。
	for( int i=0; i<m_FontList.GetItemCount(); i++ ) {
		if( m_FontList.GetItemText(i,0) == theApp.m_optionMng.GetFontFace() ) {
			// 現在のフォント発見。
			// 選択状態にする。
			m_FontList.SetItemState( i, LVIS_FOCUSED | LVIS_SELECTED, LVIS_FOCUSED | LVIS_SELECTED );

			// スクロール
			m_FontList.EnsureVisible( i, FALSE );
			break;
		}
	}

	// Win32 では画面サイズ変更
#ifndef WINCE
	SetWindowPos( NULL, 0, 0, 300, 400, SWP_NOMOVE | SWP_NOZORDER );
#endif

	return TRUE;  // return TRUE unless you set the focus to a control
	// 例外 : OCX プロパティ ページは必ず FALSE を返します。
}

void CChooseFontDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	if (m_FontList.m_hWnd != NULL) {
		m_FontList.MoveWindow( 0, 0, cx, cy );
	}
}

void CChooseFontDlg::OnNMReturnFontList(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: ここにコントロール通知ハンドラ コードを追加します。
	OnOK();
	*pResult = 0;
}

void CChooseFontDlg::OnOK()
{
	// 選択されているフォントを保存し、終了。
	int idx = util::MyGetListCtrlSelectedItemIndex( m_FontList );
	if( idx < 0 ) {
		m_strSelectedFont = L"";
	}else{
		m_strSelectedFont = m_FontList.GetItemText(idx,0);
	}

	CDialog::OnOK();
}

void CChooseFontDlg::OnLvnKeydownFontList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLVKEYDOWN pLVKeyDow = reinterpret_cast<LPNMLVKEYDOWN>(pNMHDR);
	if (pLVKeyDow->wVKey == VK_RETURN) {
		OnOK();
	}
	*pResult = 0;
}

void CChooseFontDlg::OnNMDblclkFontList(NMHDR *pNMHDR, LRESULT *pResult)
{
	OnOK();

	*pResult = 0;
}
