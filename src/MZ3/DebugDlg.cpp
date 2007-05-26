// DebugDlg.cpp : �����t�@�C��
//

#include "stdafx.h"
#include "MZ3.h"
#include "DebugDlg.h"
#include "util.h"


// CDebugDlg �_�C�A���O

IMPLEMENT_DYNAMIC(CDebugDlg, CDialog)

CDebugDlg::CDebugDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDebugDlg::IDD, pParent)
{

}

CDebugDlg::~CDebugDlg()
{
}

void CDebugDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_DEBUG_LIST, m_List);
}


BEGIN_MESSAGE_MAP(CDebugDlg, CDialog)
	ON_WM_SIZE()
	ON_NOTIFY(NM_CLICK, IDC_DEBUG_LIST, &CDebugDlg::OnNMClickDebugList)
	ON_NOTIFY(LVN_KEYDOWN, IDC_DEBUG_LIST, &CDebugDlg::OnLvnKeydownDebugList)
END_MESSAGE_MAP()


// CDebugDlg ���b�Z�[�W �n���h��

BOOL CDebugDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// �t�H���g
	m_List.SetFont( &theApp.m_font );

	// �J�����ǉ�
	m_List.InsertColumn( 0, L"����", LVCFMT_LEFT, 150, -1 );
	m_List.InsertColumn( 1, L"���e", LVCFMT_LEFT, 300, -1 );
	// ��s�I�����[�h�̐ݒ�
	ListView_SetExtendedListViewStyle((HWND)m_List.m_hWnd, LVS_EX_FULLROWSELECT);

	// �v�f�ǉ�
	int idx = 0;

	CMixiData* data = m_data;

	m_List.InsertItem( idx, L"�A�N�Z�X���" );
	m_List.SetItemText( idx, 1, util::AccessType2Message( data->GetAccessType() ) );
	idx++;

	m_List.InsertItem( idx, L"Date" );
	m_List.SetItemText( idx, 1, data->GetDate() );
	idx++;

	m_List.InsertItem( idx, L"Name" );
	m_List.SetItemText( idx, 1, data->GetName() );
	idx++;

	m_List.InsertItem( idx, L"Author" );
	m_List.SetItemText( idx, 1, data->GetAuthor() );
	idx++;

	m_List.InsertItem( idx, L"AuthorID" );
	m_List.SetItemText( idx, 1, util::int2str( data->GetAuthorID() ) );
	idx++;

	m_List.InsertItem( idx, L"Title" );
	m_List.SetItemText( idx, 1, data->GetTitle() );
	idx++;

	m_List.InsertItem( idx, L"URL" );
	m_List.SetItemText( idx, 1, data->GetURL() );
	idx++;

	m_List.InsertItem( idx, L"POST�A�h���X" );
	m_List.SetItemText( idx, 1, data->GetPostAddress() );
	idx++;

	m_List.InsertItem( idx, L"�L��ID" );
	m_List.SetItemText( idx, 1, util::int2str( data->GetID() ) );
	idx++;

	m_List.InsertItem( idx, L"�R�����gID" );
	m_List.SetItemText( idx, 1, util::int2str( data->GetCommentID() ) );
	idx++;

	m_List.InsertItem( idx, L"�R�����g�ԍ�" );
	m_List.SetItemText( idx, 1, util::int2str( data->GetCommentIndex() ) );
	idx++;

	m_List.InsertItem( idx, L"�R�����g��" );
	m_List.SetItemText( idx, 1, util::int2str( data->GetCommentCount() ) );
	idx++;

	m_List.InsertItem( idx, L"���ǃR�����g�ԍ�" );
	m_List.SetItemText( idx, 1, util::int2str( data->GetLastIndex() ) );
	idx++;

	m_List.InsertItem( idx, L"POST���" );
	m_List.SetItemText( idx, 1, util::int2str( data->GetContentType() ) );
	idx++;

	m_List.InsertItem( idx, L"�I�[�i�[ID" );
	m_List.SetItemText( idx, 1, util::int2str( data->GetOwnerID() ) );
	idx++;

	m_List.InsertItem( idx, L"�}�C�~�N" );
	m_List.SetItemText( idx, 1, data->IsMyMixi() ? L"true" : L"false" );
	idx++;

	m_List.InsertItem( idx, L"�O���u���O" );
	m_List.SetItemText( idx, 1, data->IsOtherDiary() ? L"YES" : L"NO" );
	idx++;

	m_List.InsertItem( idx, L"�u���E�YURL" );
	m_List.SetItemText( idx, 1, data->GetBrowseUri() );
	idx++;

	m_List.InsertItem( idx, L"BodyArray��" );
	m_List.SetItemText( idx, 1, util::int2str( data->GetBodySize() ) );
	idx++;

	m_List.InsertItem( idx, L"ImageArray��" );
	m_List.SetItemText( idx, 1, util::int2str( data->GetImageCount() ) );
	idx++;

	m_List.InsertItem( idx, L"���o�����N��" );
	m_List.SetItemText( idx, 1, util::int2str( data->m_linkList.size() ) );
	idx++;

	m_List.InsertItem( idx, L"�y�[�W�ύX�����N��" );
	m_List.SetItemText( idx, 1, util::int2str( data->m_linkPage.size() ) );
	idx++;

	m_List.InsertItem( idx, L"�q�v�f��" );
	m_List.SetItemText( idx, 1, util::int2str( data->GetChildrenSize() ) );
	idx++;

	m_List.InsertItem( idx, L"logfile" );
	m_List.SetItemText( idx, 1, util::MakeLogfilePath(*data) );
	idx++;

	m_List.InsertItem( idx, L"logfile-exist" );
	m_List.SetItemText( idx, 1, util::ExistFile(util::MakeLogfilePath(*data)) ? L"true" : L"false" );
	idx++;

	m_List.InsertItem( idx, L"SM_CYBORDER" );
	m_List.SetItemText( idx, 1, util::int2str(GetSystemMetrics(SM_CYBORDER)) );
	idx++;

	m_List.SetItemState( 0, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED );

	return TRUE;  // return TRUE unless you set the focus to a control
	// ��O : OCX �v���p�e�B �y�[�W�͕K�� FALSE ��Ԃ��܂��B
}

void CDebugDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);


	m_List.MoveWindow( 0, 0, cx, cy );
}

void CDebugDlg::OnNMClickDebugList(NMHDR *pNMHDR, LRESULT *pResult)
{

	*pResult = 0;
}

void CDebugDlg::OnLvnKeydownDebugList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLVKEYDOWN pLVKeyDow = reinterpret_cast<LPNMLVKEYDOWN>(pNMHDR);
	if (pLVKeyDow->wVKey == VK_RETURN) {
		POSITION pos = m_List.GetFirstSelectedItemPosition();
		int idx = m_List.GetNextSelectedItem( pos );
		if( idx < 0 ) {
			return;
		}

		CString msg;
		msg.Format( 
			L"[%s]",
			m_List.GetItemText(idx,1) );
		MessageBox( msg, m_List.GetItemText(idx,0) );
	}

	*pResult = 0;
}
