/*
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */
// QuoteDlg.cpp : �����t�@�C��
//

#include "stdafx.h"
#include "MZ3.h"
#include "util.h"
#include "util_gui.h"
#include "QuoteDlg.h"
#include "ViewFilter.h"


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
	ON_LBN_DBLCLK(IDC_TYPE_LIST, &CQuoteDlg::OnLbnDblclkTypeList)
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

	CString quoteMark = theApp.m_optionMng.GetQuoteMark();

	m_typeList.InsertString( idx, L"���p�Ȃ�" );
	m_typeList.SetItemData( idx, quote::QUOTETYPE_NONE );
	idx ++;

	if( m_pMixi->GetCommentIndex() > 0 ) {
		m_typeList.InsertString( idx, quoteMark + L"[�ԍ�]" );
		m_typeList.SetItemData( idx, quote::QUOTETYPE_NUM );
		idx ++;
	}

	m_typeList.InsertString( idx, quoteMark + L"[���O]" );
	m_typeList.SetItemData( idx, quote::QUOTETYPE_NAME );
	idx ++;

	if( m_pMixi->GetCommentIndex() > 0 ) {
		m_typeList.InsertString( idx, quoteMark + L"[�ԍ�] [���O]" );
		m_typeList.SetItemData( idx, quote::QUOTETYPE_NUM_NAME );
		idx ++;
	}

	m_typeList.InsertString( idx, L"�{���̂�" );
	m_typeList.SetItemData( idx, quote::QUOTETYPE_BODY );
	idx ++;

	if( m_pMixi->GetCommentIndex() > 0 ) {
		m_typeList.InsertString( idx, quoteMark + L"[�ԍ�] �{�{��" );
		m_typeList.SetItemData( idx, quote::QUOTETYPE_NUM_BODY );
		idx ++;
	}

	m_typeList.InsertString( idx, quoteMark + L"[���O] �{�{��" );
	m_typeList.SetItemData( idx, quote::QUOTETYPE_NAME_BODY );
	default_idx = idx;
	idx ++;

	if( m_pMixi->GetCommentIndex() > 0 ) {
		m_typeList.InsertString( idx, quoteMark + L"[�ԍ�] [���O] �{�{��" );
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

#ifndef WINCE
	// ��ʃT�C�Y��ύX
	int w = 360;
	int h = 480;
	SetWindowPos( NULL, 0, 0, w, h, SWP_NOMOVE | SWP_NOOWNERZORDER );
#endif

	return TRUE;  // return TRUE unless you set the focus to a control
	// ��O : OCX �v���p�e�B �y�[�W�͕K�� FALSE ��Ԃ��܂��B
}

void CQuoteDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	int hInfo = theApp.GetInfoRegionHeight(theApp.m_optionMng.m_fontHeight);
	int hList = cy/2 - hInfo;

	util::MoveDlgItemWindow( this, IDC_MESSAGE_EDIT,   0,        0, cx, hInfo          );
	util::MoveDlgItemWindow( this, IDC_SAMPLE_EDIT,    0,    hInfo, cx, cy-hList-hInfo );
	util::MoveDlgItemWindow( this, IDC_TYPE_LIST,      0, cy-hList, cx, hList          );
}

void CQuoteDlg::OnLbnSelchangeTypeList()
{
	if( m_pMixi == NULL ) {
		return;
	}

	m_quoteType = (quote::QuoteType)m_typeList.GetItemData( m_typeList.GetCurSel() );
	CString strQuote = quote::MakeQuoteString( *m_pMixi, m_quoteType );

	// �����r���[�^�O������
	ViewFilter::RemoveRan2ViewTag( strQuote );

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
#ifndef WINCE
		case VK_ESCAPE:
#endif
			// �L�����Z������
			m_quoteType = quote::QUOTETYPE_INVALID;
			OnOK();
			return TRUE;
		}
		break;
	}

	return CDialog::PreTranslateMessage(pMsg);
}

void CQuoteDlg::OnLbnDblclkTypeList()
{
	OnOK();
}
