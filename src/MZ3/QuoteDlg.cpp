/*
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */
// QuoteDlg.cpp : 実装ファイル
//

#include "stdafx.h"
#include "MZ3.h"
#include "util.h"
#include "util_gui.h"
#include "QuoteDlg.h"
#include "ViewFilter.h"


// CQuoteDlg ダイアログ

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


// CQuoteDlg メッセージ ハンドラ

BOOL CQuoteDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// フォント
	m_msgEdit.SetFont( &theApp.m_font );
	m_sampleEdit.SetFont( &theApp.m_font );
	m_typeList.SetFont( &theApp.m_font );

	// メッセージ設定
	m_msgEdit.SetWindowTextW( L"引用方法を選んでください" );


	// アイテム追加
	// 番号が -1 なら番号系は追加しない。
	int idx = 0;
	int default_idx = 0;

	CString quoteMark = theApp.m_optionMng.GetQuoteMark();

	m_typeList.InsertString( idx, L"引用なし" );
	m_typeList.SetItemData( idx, quote::QUOTETYPE_NONE );
	idx ++;

	if( m_pMixi->GetCommentIndex() > 0 ) {
		m_typeList.InsertString( idx, quoteMark + L"[番号]" );
		m_typeList.SetItemData( idx, quote::QUOTETYPE_NUM );
		idx ++;
	}

	m_typeList.InsertString( idx, quoteMark + L"[名前]" );
	m_typeList.SetItemData( idx, quote::QUOTETYPE_NAME );
	idx ++;

	if( m_pMixi->GetCommentIndex() > 0 ) {
		m_typeList.InsertString( idx, quoteMark + L"[番号] [名前]" );
		m_typeList.SetItemData( idx, quote::QUOTETYPE_NUM_NAME );
		idx ++;
	}

	m_typeList.InsertString( idx, L"本文のみ" );
	m_typeList.SetItemData( idx, quote::QUOTETYPE_BODY );
	idx ++;

	if( m_pMixi->GetCommentIndex() > 0 ) {
		m_typeList.InsertString( idx, quoteMark + L"[番号] ＋本文" );
		m_typeList.SetItemData( idx, quote::QUOTETYPE_NUM_BODY );
		idx ++;
	}

	m_typeList.InsertString( idx, quoteMark + L"[名前] ＋本文" );
	m_typeList.SetItemData( idx, quote::QUOTETYPE_NAME_BODY );
	default_idx = idx;
	idx ++;

	if( m_pMixi->GetCommentIndex() > 0 ) {
		m_typeList.InsertString( idx, quoteMark + L"[番号] [名前] ＋本文" );
		m_typeList.SetItemData( idx, quote::QUOTETYPE_NUM_NAME_BODY );
		default_idx = idx;
		idx ++;
	}

	m_typeList.InsertString( idx, L"キャンセル" );
	m_typeList.SetItemData( idx, quote::QUOTETYPE_INVALID );
	idx ++;

	// とりあえずデフォルト値を選択
	m_typeList.SetCurSel( default_idx );

	// m_quoteType として指定された値を選択
	for( int i=0; i<m_typeList.GetCount(); i++ ) {
		quote::QuoteType quoteType = (quote::QuoteType)m_typeList.GetItemData(i);
		if( quoteType == m_quoteType ) {
			m_typeList.SetCurSel( i );
			break;
		}
	}

	OnLbnSelchangeTypeList();

#ifndef WINCE
	// 画面サイズを変更
	int w = 360;
	int h = 480;
	SetWindowPos( NULL, 0, 0, w, h, SWP_NOMOVE | SWP_NOOWNERZORDER );
#endif

	return TRUE;  // return TRUE unless you set the focus to a control
	// 例外 : OCX プロパティ ページは必ず FALSE を返します。
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

	// らんらんビュータグを消去
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
				// 一番上の項目選択中なので、一番下に移動
				m_typeList.SetCurSel( m_typeList.GetCount()-1 );
				OnLbnSelchangeTypeList();
				return TRUE;
			}
			break;
		case VK_DOWN:
			if (m_typeList.GetCurSel() == m_typeList.GetCount()-1) {
				// 一番下の項目選択中なので、一番上に移動
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
			// キャンセル動作
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
