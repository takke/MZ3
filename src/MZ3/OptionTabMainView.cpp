// OptionTabMainView.cpp : 実装ファイル
//

#include "stdafx.h"
#include "MZ3.h"
#include "OptionTabMainView.h"


// COptionTabMainView ダイアログ

IMPLEMENT_DYNAMIC(COptionTabMainView, CPropertyPage)

COptionTabMainView::COptionTabMainView()
	: CPropertyPage(COptionTabMainView::IDD)
{

}

COptionTabMainView::~COptionTabMainView()
{
}

void COptionTabMainView::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_MINI_IMAGE_SIZE_COMBO, mc_comboMiniImageSize);
}


BEGIN_MESSAGE_MAP(COptionTabMainView, CPropertyPage)
	ON_BN_CLICKED(IDC_SHOW_MINI_IMAGE_DLG_CHECK, &COptionTabMainView::OnBnClickedShowMiniImageDlgCheck)
END_MESSAGE_MAP()


// COptionTabMainView メッセージ ハンドラ

BOOL COptionTabMainView::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	// ユーザやコミュニティの画像
	CheckDlgButton( IDC_SHOW_MINI_IMAGE_DLG_CHECK, theApp.m_optionMng.m_bShowMainViewMiniImage ? BST_CHECKED : BST_UNCHECKED );

	// WINCE ではカーソル位置に表示しない
#ifdef WINCE
	// 無効
//	GetDlgItem( IDC_SHOW_MINI_IMAGE_DLG_CHECK )->EnableWindow( FALSE );
	GetDlgItem( IDC_SHOW_MINI_IMAGE_DLG_ON_MOUSEOVER_CHECK )->EnableWindow( FALSE );
#else
	CheckDlgButton( IDC_SHOW_MINI_IMAGE_DLG_ON_MOUSEOVER_CHECK, theApp.m_optionMng.m_bShowMainViewMiniImageOnMouseOver ? BST_CHECKED : BST_UNCHECKED );
#endif

	// トピック等のアイコン表示
	CheckDlgButton( IDC_SHOW_ICON_CHECK, theApp.m_optionMng.m_bShowMainViewIcon ? BST_CHECKED : BST_UNCHECKED );

	// mini画面サイズ
#ifdef WINCE
	// 無効
	GetDlgItem( IDC_SIZE_STATIC )->EnableWindow( FALSE );
	GetDlgItem( IDC_MINI_IMAGE_SIZE_COMBO )->EnableWindow( FALSE );
#else
	int sizes[] = { 25, 50, 75, 100, 125, 150, -1 };
	for (int i=0; sizes[i] != -1; i++) {
		CString s;
		s.Format( L"%dx%d", sizes[i], sizes[i] );
		int idx = mc_comboMiniImageSize.InsertString( i, s );
		mc_comboMiniImageSize.SetItemData( idx, sizes[i] );

		if (sizes[i] == theApp.m_optionMng.m_nMainViewMiniImageSize) {
			mc_comboMiniImageSize.SetCurSel(i);
		}
	}
	if (mc_comboMiniImageSize.GetCurSel() < 0) {
		mc_comboMiniImageSize.SetCurSel( 0 );
	}
	// 有効・無効
	OnBnClickedShowMiniImageDlgCheck();
#endif

	return TRUE;  // return TRUE unless you set the focus to a control
	// 例外 : OCX プロパティ ページは必ず FALSE を返します。
}

void COptionTabMainView::OnOK()
{
	// ユーザやコミュニティの画像
	theApp.m_optionMng.m_bShowMainViewMiniImage = IsDlgButtonChecked( IDC_SHOW_MINI_IMAGE_DLG_CHECK ) == BST_CHECKED;
	theApp.m_optionMng.m_bShowMainViewMiniImageOnMouseOver = IsDlgButtonChecked( IDC_SHOW_MINI_IMAGE_DLG_ON_MOUSEOVER_CHECK ) == BST_CHECKED;

	// トピック等のアイコン表示
	theApp.m_optionMng.m_bShowMainViewIcon = IsDlgButtonChecked( IDC_SHOW_ICON_CHECK ) == BST_CHECKED;

	// mini画面サイズ
#ifndef WINCE
	theApp.m_optionMng.m_nMainViewMiniImageSize = mc_comboMiniImageSize.GetItemData( mc_comboMiniImageSize.GetCurSel() );
#endif

	CPropertyPage::OnOK();
}

void COptionTabMainView::OnBnClickedShowMiniImageDlgCheck()
{
// WINCE ではカーソル位置に表示しない
#ifndef WINCE
	bool bChecked = (IsDlgButtonChecked( IDC_SHOW_MINI_IMAGE_DLG_CHECK ) == BST_CHECKED);

	GetDlgItem( IDC_SIZE_STATIC )->EnableWindow( bChecked ? TRUE : FALSE );
	GetDlgItem( IDC_MINI_IMAGE_SIZE_COMBO )->EnableWindow( bChecked ? TRUE : FALSE );
	GetDlgItem( IDC_SHOW_MINI_IMAGE_DLG_ON_MOUSEOVER_CHECK )->EnableWindow( bChecked ? TRUE : FALSE );
#endif
}
