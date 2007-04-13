// QuoteDlg.cpp : �����t�@�C��
//

#include "stdafx.h"
#include "MZ3.h"
#include "util.h"
#include "QuoteDlg.h"


// CQuoteDlg �_�C�A���O

IMPLEMENT_DYNAMIC(CQuoteDlg, CDialog)

CQuoteDlg::CQuoteDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CQuoteDlg::IDD, pParent)
	, m_pMixi(NULL)
	, m_quoteType(quote::QUOTETYPE_NUM_NAME_BODY)
{

}

CQuoteDlg::~CQuoteDlg()
{
}

void CQuoteDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TYPE_LIST, m_typeList);
	DDX_Control(pDX, IDC_SAMPLE_EDIT, m_sampleEdit);
	DDX_Control(pDX, IDC_MESSAGE_EDIT, m_msgEdit);
}


BEGIN_MESSAGE_MAP(CQuoteDlg, CDialog)
	ON_WM_SIZE()
	ON_LBN_SELCHANGE(IDC_TYPE_LIST, &CQuoteDlg::OnLbnSelchangeTypeList)
END_MESSAGE_MAP()


// CQuoteDlg ���b�Z�[�W �n���h��

BOOL CQuoteDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// �t�H���g
	m_msgEdit.SetFont( &theApp.m_font );
	m_sampleEdit.SetFont( &theApp.m_font );
	m_typeList.SetFont( &theApp.m_font );

	// ���b�Z�[�W�ݒ�
	m_msgEdit.SetWindowTextW( L"���p���@��I��ł�������" );


	// �A�C�e���ǉ�
	// �ԍ��� -1 �Ȃ�ԍ��n�͒ǉ����Ȃ��B
	int idx = 0;
	int default_idx = 0;

	m_typeList.InsertString( idx, L"���p�Ȃ�" );
	m_typeList.SetItemData( idx, quote::QUOTETYPE_NONE );
	idx ++;

	if( m_pMixi->GetCommentIndex() > 0 ) {
		m_typeList.InsertString( idx, L"> [�ԍ�]" );
		m_typeList.SetItemData( idx, quote::QUOTETYPE_NUM );
		idx ++;
	}

	m_typeList.InsertString( idx, L"> [���O]" );
	m_typeList.SetItemData( idx, quote::QUOTETYPE_NAME );
	idx ++;

	if( m_pMixi->GetCommentIndex() > 0 ) {
		m_typeList.InsertString( idx, L"> [�ԍ�] [���O]" );
		m_typeList.SetItemData( idx, quote::QUOTETYPE_NUM_NAME );
		idx ++;
	}

	m_typeList.InsertString( idx, L"�{���̂�" );
	m_typeList.SetItemData( idx, quote::QUOTETYPE_BODY );
	idx ++;

	if( m_pMixi->GetCommentIndex() > 0 ) {
		m_typeList.InsertString( idx, L"> [�ԍ�] �{�{��" );
		m_typeList.SetItemData( idx, quote::QUOTETYPE_NUM_BODY );
		idx ++;
	}

	m_typeList.InsertString( idx, L"> [���O] �{�{��" );
	m_typeList.SetItemData( idx, quote::QUOTETYPE_NAME_BODY );
	default_idx = idx;
	idx ++;

	if( m_pMixi->GetCommentIndex() > 0 ) {
		m_typeList.InsertString( idx, L"> [�ԍ�] [���O] �{�{��" );
		m_typeList.SetItemData( idx, quote::QUOTETYPE_NUM_NAME_BODY );
		default_idx = idx;
		idx ++;
	}

	m_typeList.InsertString( idx, L"�L�����Z��" );
	m_typeList.SetItemData( idx, quote::QUOTETYPE_INVALID );
	idx ++;

	// �Ƃ肠�����f�t�H���g�l��I��
	m_typeList.SetCurSel( default_idx );

	// m_quoteType �Ƃ��Ďw�肳�ꂽ�l��I��
	for( int i=0; i<m_typeList.GetCount(); i++ ) {
		quote::QuoteType quoteType = (quote::QuoteType)m_typeList.GetItemData(i);
		if( quoteType == m_quoteType ) {
			m_typeList.SetCurSel( i );
			break;
		}
	}

	OnLbnSelchangeTypeList();

	return TRUE;  // return TRUE unless you set the focus to a control
	// ��O : OCX �v���p�e�B �y�[�W�͕K�� FALSE ��Ԃ��܂��B
}

void CQuoteDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	int hInfo = 36;
	int hList = cy/2 - hInfo;

	switch( theApp.GetDisplayMode() ) {
	case SR_VGA:
		hInfo = 36;
		break;
	case SR_QVGA:
	default:
		hInfo = 18;
		break;
	}

	GetDlgItem(IDC_MESSAGE_EDIT)->MoveWindow(   0,        0, cx, hInfo          );
	GetDlgItem(IDC_SAMPLE_EDIT )->MoveWindow(   0,    hInfo, cx, cy-hList-hInfo );
	GetDlgItem(IDC_TYPE_LIST   )->MoveWindow(   0, cy-hList, cx, hList          );
}

void CQuoteDlg::OnLbnSelchangeTypeList()
{
	if( m_pMixi == NULL ) {
		return;
	}

	m_quoteType = (quote::QuoteType)m_typeList.GetItemData( m_typeList.GetCurSel() );
	CString strQuote = quote::MakeQuoteString( *m_pMixi, m_quoteType );

	m_sampleEdit.SetWindowTextW( strQuote );
}

BOOL CQuoteDlg::PreTranslateMessage(MSG* pMsg)
{
	switch( pMsg->message ) {
	case WM_KEYDOWN:
		switch( pMsg->wParam ) {
		case VK_UP:
			if (m_typeList.GetCurSel() == 0) {
				// ��ԏ�̍��ڑI�𒆂Ȃ̂ŁA��ԉ��Ɉړ�
				m_typeList.SetCurSel( m_typeList.GetCount()-1 );
				OnLbnSelchangeTypeList();
				return TRUE;
			}
			break;
		case VK_DOWN:
			if (m_typeList.GetCurSel() == m_typeList.GetCount()-1) {
				// ��ԉ��̍��ڑI�𒆂Ȃ̂ŁA��ԏ�Ɉړ�
				m_typeList.SetCurSel( 0 );
				OnLbnSelchangeTypeList();
				return TRUE;
			}
			break;
		case VK_RETURN:
			OnOK();
			return TRUE;
		case VK_BACK:
			// �L�����Z������
			m_quoteType = quote::QUOTETYPE_INVALID;
			OnOK();
			return TRUE;
		}
		break;
	}

	return CDialog::PreTranslateMessage(pMsg);
}
