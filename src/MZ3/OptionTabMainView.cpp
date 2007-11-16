// OptionTabMainView.cpp : �����t�@�C��
//

#include "stdafx.h"
#include "MZ3.h"
#include "OptionTabMainView.h"


// COptionTabMainView �_�C�A���O

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
END_MESSAGE_MAP()


// COptionTabMainView ���b�Z�[�W �n���h��

BOOL COptionTabMainView::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	// ���[�U��R�~���j�e�B�̉摜
#ifdef WINCE
	// ����
	GetDlgItem( IDC_SHOW_MINI_IMAGE_DLG_CHECK )->EnableWindow( FALSE );
#else
	CheckDlgButton( IDC_SHOW_MINI_IMAGE_DLG_CHECK, theApp.m_optionMng.m_bShowMainViewMiniImage ? BST_CHECKED : BST_UNCHECKED );
#endif

	// �g�s�b�N���̃A�C�R���\��
	CheckDlgButton( IDC_SHOW_ICON_CHECK, theApp.m_optionMng.m_bShowMainViewIcon ? BST_CHECKED : BST_UNCHECKED );

	// mini��ʃT�C�Y
#ifdef WINCE
	// ����
	mc_comboMiniImageSize.EnableWindow( FALSE );
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
#endif

	return TRUE;  // return TRUE unless you set the focus to a control
	// ��O : OCX �v���p�e�B �y�[�W�͕K�� FALSE ��Ԃ��܂��B
}

void COptionTabMainView::OnOK()
{
	// ���[�U��R�~���j�e�B�̉摜
	theApp.m_optionMng.m_bShowMainViewMiniImage = IsDlgButtonChecked( IDC_SHOW_MINI_IMAGE_DLG_CHECK ) == BST_CHECKED;

	// �g�s�b�N���̃A�C�R���\��
	theApp.m_optionMng.m_bShowMainViewIcon = IsDlgButtonChecked( IDC_SHOW_ICON_CHECK ) == BST_CHECKED;

	// mini��ʃT�C�Y
#ifndef WINCE
	theApp.m_optionMng.m_nMainViewMiniImageSize = mc_comboMiniImageSize.GetItemData( mc_comboMiniImageSize.GetCurSel() );
#endif

	CPropertyPage::OnOK();
}