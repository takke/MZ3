// ChooseFontDlg.cpp : �����t�@�C��
//

#include "stdafx.h"
#include "MZ3.h"
#include "util_gui.h"
#include "ChooseFontDlg.h"


// CChooseFontDlg �_�C�A���O

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

/// �e�t�H���g�ɑ΂���R�[���o�b�N
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


// CChooseFontDlg ���b�Z�[�W �n���h��

BOOL CChooseFontDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// �t�H���g
//	m_FontList.SetFont( &theApp.m_font );

	// �J�����ǉ�
	m_FontList.InsertColumn( 0, L"�t�H���g", LVCFMT_LEFT, 300, -1 );
	m_FontList.InsertColumn( 1, L"���l", LVCFMT_LEFT, 150, -1 );

	// ��s�I�����[�h�̐ݒ�
	ListView_SetExtendedListViewStyle((HWND)m_FontList.m_hWnd, LVS_EX_FULLROWSELECT);

	// �t�H���g�ꗗ�̐���
	HDC dc = ::GetDC(HWND_DESKTOP);
	::EnumFontFamilies( dc, 0, (FONTENUMPROC)enumFontsProc, (LPARAM)this );
	::ReleaseDC(HWND_DESKTOP, dc);

	// ���݂̃t�H���g��I������B
	for( int i=0; i<m_FontList.GetItemCount(); i++ ) {
		if( m_FontList.GetItemText(i,0) == theApp.m_optionMng.GetFontFace() ) {
			// ���݂̃t�H���g�����B
			// �I����Ԃɂ���B
			m_FontList.SetItemState( i, LVIS_FOCUSED | LVIS_SELECTED, LVIS_FOCUSED | LVIS_SELECTED );

			// �X�N���[��
			m_FontList.EnsureVisible( i, FALSE );
			break;
		}
	}

	// Win32 �ł͉�ʃT�C�Y�ύX
#ifndef WINCE
	SetWindowPos( NULL, 0, 0, 300, 400, SWP_NOMOVE | SWP_NOZORDER );
#endif

	return TRUE;  // return TRUE unless you set the focus to a control
	// ��O : OCX �v���p�e�B �y�[�W�͕K�� FALSE ��Ԃ��܂��B
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
	// TODO: �����ɃR���g���[���ʒm�n���h�� �R�[�h��ǉ����܂��B
	OnOK();
	*pResult = 0;
}

void CChooseFontDlg::OnOK()
{
	// �I������Ă���t�H���g��ۑ����A�I���B
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
