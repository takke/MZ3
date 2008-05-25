/*
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */
// ReportListCtrl.cpp : 実装ファイル
//

#include "stdafx.h"
#include "MZ3.h"
#include "ReportListCtrl.h"
#include "ReportView.h"
#include "util.h"
#include "util_gui.h"

static const int OFFSET_FIRST = 2;
static const int OFFSET_OTHER = 6;

// CReportListCtrl

IMPLEMENT_DYNAMIC(CReportListCtrl, CTouchListCtrl)

CReportListCtrl::CReportListCtrl()
{
}

CReportListCtrl::~CReportListCtrl()
{
}


BEGIN_MESSAGE_MAP(CReportListCtrl, CTouchListCtrl)
	ON_WM_ERASEBKGND()
	ON_NOTIFY_REFLECT_EX(LVN_ITEMCHANGED, &CReportListCtrl::OnLvnItemchanged)
	ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()



// CReportListCtrl メッセージ ハンドラ

void CReportListCtrl::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	CDC* pDC = CDC::FromHandle(lpDrawItemStruct->hDC);

	CRect rectClient;
	this->GetClientRect( &rectClient );

	if( IsDrawBk() ) {
		// ビットマップの描画
		if( theApp.m_optionMng.IsUseBgImage() && theApp.m_bgImageReportListCtrl.isEnableImage() ) {
			int x = lpDrawItemStruct->rcItem.left;
			int y = lpDrawItemStruct->rcItem.top;
			int w = rectClient.Width();
			int h = lpDrawItemStruct->rcItem.bottom - y;
			int offset = 0;
			if( IsScrollWithBk() ){
				offset = ( h * GetTopIndex() ) % theApp.m_bgImageReportListCtrl.getBitmapSize().cy;
			}
			util::DrawBitmap( pDC->GetSafeHdc(), theApp.m_bgImageReportListCtrl.getHandle(), x, y, w, h, x, y + offset );
		}
	}

	// 再描画するItemの座標を取得
	CRect rcItem(lpDrawItemStruct->rcItem);

	// アイテムのID
	int nItem = lpDrawItemStruct->itemID;

	// 現在コントロールにフォーカスがあるか否かの
	// フラグを設定
	BOOL bFocus = FALSE;
	if (GetFocus() == this) {
		bFocus = TRUE;
	}

	TCHAR szBuff[MAX_PATH];
	LPCTSTR pszText;

	// アイテム データを取得します。
	LV_ITEM lvi;

	lvi.mask = LVIF_TEXT | LVIF_IMAGE | LVIF_STATE | LVIF_PARAM;
	lvi.iItem = nItem;
	lvi.iSubItem = 0;
	lvi.pszText = szBuff;
	lvi.cchTextMax = sizeof(szBuff);
	lvi.stateMask = LVIS_SELECTED | LVIS_STATEIMAGEMASK;
	this->GetItem(&lvi);

	// アイテムが選択状態か否かのフラグを設定
	BOOL bSelected =
		(bFocus || (GetStyle() & LVS_SHOWSELALWAYS))
		&& lvi.state & LVIS_SELECTED;
	bSelected = bSelected || (lvi.state & LVIS_DROPHILITED);

	// アイテムの表示されている幅を取得
	CRect rcAllLabels;
	this->GetItemRect(nItem, rcAllLabels, LVIR_BOUNDS);
	// 表示位置をrcItemに合わせる
	rcAllLabels.MoveToY( rcItem.top );

	// アイテムのラベルの幅を取得
	CRect rcLabel;
	this->GetItemRect(nItem, rcLabel, LVIR_LABEL);
	// 表示位置をrcItemに合わせる
	rcLabel.MoveToY( rcItem.top );

	// 左の位置を同じにする
	rcAllLabels.left = rcLabel.left;

	// 文字の横幅を取得
	CSize szSize = pDC->GetTextExtent(szBuff, (int)wcslen(szBuff));
	// 塗りつぶし範囲を文字部分にする
	CRect rcItemStr;
	rcItemStr = rcLabel;
	rcItemStr.right =	rcItemStr.left + szSize.cx + OFFSET_OTHER;

	// 背景の塗りつぶし（背景画像なしの場合）
	if( !theApp.m_optionMng.IsUseBgImage() || !theApp.m_bgImageReportListCtrl.isEnableImage() ) {
		pDC->FillRect(rcAllLabels, &CBrush(RGB(0xFF, 0xFF, 0xFF)));
	}

	// 選択されている場合は、
	// 選択されている文字のみ一行を塗りつぶす
	if (bSelected == TRUE) {
		pDC->FillRect(rcAllLabels, &CBrush(::GetSysColor(COLOR_HIGHLIGHT)));
	}

	// 色を設定してアイコンをマスクします。
	UINT uiFlags = ILD_TRANSPARENT;						// 背景を透明にするフラグ（イメージデータ用）
	COLORREF clrMaskFg = ::GetSysColor(COLOR_WINDOW);	// 描画する色
	COLORREF clrMaskBk = ::GetSysColor(COLOR_WINDOW);	// 描画する色
	if ((lvi.state & LVIS_CUT) == LVIS_CUT) {
		uiFlags |= ILD_BLEND50;
	} else {
		if (bSelected == TRUE) {
			clrMaskFg = ::GetSysColor(COLOR_HIGHLIGHT);
			uiFlags |= ILD_BLEND50;
		}
	}

	// 状態アイコンを描画します。
	UINT nStateImageMask = lvi.state & LVIS_STATEIMAGEMASK;
	if ((nStateImageMask>>12) > 0) {

		int nImage = (nStateImageMask>>12) - 1;

		CImageList* pImageList = this->GetImageList(LVSIL_STATE);
		if (pImageList) {
			pImageList->Draw(pDC, nImage,
				CPoint(rcItem.left, rcItem.top),
				ILD_TRANSPARENT);
		}
	}

	// 通常のアイコンとオーバーレイアイコンを描画します。
	CRect rcIcon;
	this->GetItemRect(nItem, rcIcon, LVIR_ICON);
	// 表示位置をrcItemに合わせる
	rcIcon.MoveToY( rcItem.top );

	CImageList* pImageList = this->GetImageList(LVSIL_SMALL);

	if (pImageList != NULL) {
		UINT nOvlImageMask = lvi.state & LVIS_OVERLAYMASK;
		if (rcItem.left < rcItem.right - 1) {
			if (lvi.iImage != 0) {
				ImageList_DrawEx(
					pImageList->m_hImageList, lvi.iImage,
					pDC->m_hDC,
					rcIcon.left, rcIcon.top, 16, 16,
					clrMaskBk, clrMaskFg,
					uiFlags | nOvlImageMask);
			}
		}
	}

	// アイテムのラベルを描きます。
	CRect rcSubItem;
	this->GetItemRect(nItem, rcSubItem, LVIR_LABEL);
	// 表示位置をrcItemに合わせる
	rcSubItem.MoveToY( rcItem.top );

	//--- 左側カラム

	pszText = szBuff;

	//rcLabel = rcItem;
	rcLabel.left  += OFFSET_FIRST;
	rcLabel.right -= OFFSET_FIRST;

	// 文字色の変更
	COLORREF clrTextSave = (COLORREF)-1;
	COLORREF clrBkSave   = (COLORREF)-1;
	if (bSelected == TRUE) {
		// 選択状態
		// システム標準の選択色で塗りつぶす
		clrTextSave = pDC->SetTextColor( ::GetSysColor(COLOR_HIGHLIGHTTEXT) );
		clrBkSave = pDC->SetBkColor( ::GetSysColor(COLOR_HIGHLIGHT) );
	} else {
		// 非選択状態
		COLORREF clrTextFg = theApp.m_skininfo.clrReportListText;

		// 色づけ処理
		clrTextSave = pDC->SetTextColor(clrTextFg);
	}

	pDC->DrawText(pszText,
		-1,
		rcLabel,
		DT_LEFT | DT_SINGLELINE | DT_NOPREFIX | DT_NOCLIP | DT_VCENTER | DT_END_ELLIPSIS);

	//--- 右側カラム

	// カラム用のラベルを描画
	LV_COLUMN lvc;
	lvc.mask = LVCF_FMT | LVCF_WIDTH;
	for (int nColumn = 1; this->GetColumn(nColumn, &lvc); nColumn++) {
		rcSubItem.left = rcSubItem.right;
		rcSubItem.right += lvc.cx;

		int nRetLen = this->GetItemText(nItem, nColumn, szBuff, sizeof(szBuff));
		if (nRetLen == 0) {
			continue;
		}

		pszText = szBuff;

		UINT nJustify = DT_LEFT;

		if (pszText == szBuff) {
			switch(lvc.fmt & LVCFMT_JUSTIFYMASK) {
			case LVCFMT_RIGHT:
				nJustify = DT_RIGHT;
				break;
			case LVCFMT_CENTER:
				nJustify = DT_CENTER;
				break;
			default:
				break;
			}
		}

		rcLabel = rcSubItem;
		rcLabel.left  += OFFSET_OTHER;
		rcLabel.right -= OFFSET_OTHER;

		pDC->DrawText(pszText,
			-1,
			rcLabel,
			nJustify | DT_SINGLELINE | DT_NOPREFIX | DT_NOCLIP | DT_VCENTER | DT_END_ELLIPSIS);
	}

	// 色を戻す
	if (clrTextSave != (COLORREF)-1) {
		clrTextSave = pDC->SetTextColor(clrTextSave);
	}
	if (clrBkSave != (COLORREF)-1) {
		clrBkSave = pDC->SetBkColor(clrBkSave);
	}

	// アイテムがフォーカスを持っているときに
	// その選択枠を描画する
	if ((lvi.state & LVIS_FOCUSED) == LVIS_FOCUSED && bFocus == TRUE) {
		pDC->DrawFocusRect(rcAllLabels);
	}
}

/**
 * 現在選択中のアイテムのインデックスを返す。
 * 選択中のアイテムがなければ -1 を返す。
 */
int CReportListCtrl::GetSelectedItem(void)
{
	int idx = util::MyGetListCtrlSelectedItemIndex( *this );
	if( GetItemCount() == 0 || idx < 0 ) {
		return -1;
	}
	return idx;
}

BOOL CReportListCtrl::OnEraseBkgnd(CDC* pDC)
{
	pDC->SetBkMode( TRANSPARENT );

	// ビットマップの初期化と描画
	if( theApp.m_optionMng.IsUseBgImage() ) {
		CRect rectClient;
		this->GetClientRect( &rectClient );

		theApp.m_bgImageReportListCtrl.load();

		if (theApp.m_bgImageReportListCtrl.isEnableImage()) {
			int x = rectClient.left;
			int y = rectClient.top;
			int w = rectClient.Width();
			int h = rectClient.Height();
			int offset = 0;
			if( IsScrollWithBk() ){
				if( GetItemCount() > 0) {
					CRect rcItem;
					GetItemRect( 0 , &rcItem , LVIR_BOUNDS );
					offset = ( rcItem.Height() * GetTopIndex() ) % theApp.m_bgImageReportListCtrl.getBitmapSize().cy;
				}
			}
			util::DrawBitmap( pDC->GetSafeHdc(), theApp.m_bgImageReportListCtrl.getHandle(), x, y, w, h, x, y + offset );
			return TRUE;
		}
	}

	return CListCtrl::OnEraseBkgnd(pDC);
}

BOOL CReportListCtrl::OnLvnItemchanged(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);

	// 最初か最後の項目であれば全体を再描画（背景画像のずれ防止）
	int iStart = GetTopIndex();
	int iEnd   = iStart + GetCountPerPage();
	if( pNMLV->iItem < iStart || pNMLV->iItem == iEnd ) {
		if( pNMLV->uNewState & LVIS_FOCUSED ) {
			Invalidate( FALSE );
		}
	}

	*pResult = 0;

	// FALSE を返すことで、親ウィンドウにもイベントを渡す
	return FALSE;
}

void CReportListCtrl::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// スクロール時の背景画像のずれ防止
	if (theApp.m_optionMng.IsUseBgImage()) {
		static int s_nLastPos = nPos;
		if( s_nLastPos != nPos ) {
			//Invalidate( FALSE );			// ちらつき防止のためここでは再描画しない
			s_nLastPos = nPos;
		}
	}

	CTouchListCtrl::OnVScroll(nSBCode, nPos, pScrollBar);
}

BOOL CReportListCtrl::PreTranslateMessage(MSG* pMsg)
{
#ifndef WINCE
	// Win32 の場合は VK_RETURN の OnLvnKeydownReportList が飛ばないのでここで処理する
	switch (pMsg->message) {
	case WM_KEYDOWN:
		switch (pMsg->wParam) {
		case VK_RETURN:
			{
				CReportView* pView = (CReportView*)GetParent();
				pView->MyPopupReportMenu();
			}
			break;
		}
		break;
	}
#endif

	return CTouchListCtrl::PreTranslateMessage(pMsg);
}

void CReportListCtrl::OnLButtonDown(UINT nFlags, CPoint point)
{
/* CTouchListCtrl::OnLButtonDown()とCReportListCtrl::SetSelectItem()に移行
	// 選択変更
	int nItem = HitTest(point);
	if (nItem>=0) {
		util::MySetListCtrlItemFocusedAndSelected( *this, GetSelectedItem(), false );
		util::MySetListCtrlItemFocusedAndSelected( *this, nItem, true );
	}
*/
/* CTouchListCtrl::OnLButtonDown()とCReportListCtrl::PopupContextMenu()に移行
#ifdef WINCE
	// タップ長押しでソフトキーメニュー表示
	SHRGINFO RGesture;
	RGesture.cbSize     = sizeof(SHRGINFO);
	RGesture.hwndClient = m_hWnd;
	RGesture.ptDown     = point;
	RGesture.dwFlags    = SHRG_RETURNCMD;
	if (::SHRecognizeGesture(&RGesture) == GN_CONTEXTMENU) {
		// TODO 本来は WM_COMMAND で通知すべき。
		ClientToScreen(&point);
		theApp.m_pReportView->MyPopupReportMenu(point, TPM_LEFTALIGN | TPM_TOPALIGN);
		return;
	}
#endif
*/

	CTouchListCtrl::OnLButtonDown(nFlags, point);
}

/**
 * virtual GetBgBitmapHandle()
 *  背景Bitmapのハンドルを返す
 */
HBITMAP CReportListCtrl::GetBgBitmapHandle()
{
	if( theApp.m_optionMng.IsUseBgImage() ) {
		theApp.m_bgImageReportListCtrl.load();
		if(theApp.m_bgImageReportListCtrl.isEnableImage()) {
			return theApp.m_bgImageReportListCtrl.getHandle();
		}
	}
	return NULL;
}

/**
 * virtual PopupContextMenu()
 *  ポップアップメニューを開く
 */
void CReportListCtrl::PopupContextMenu( const CPoint point )
{
	// TODO 本来は WM_COMMAND で通知すべき。
	theApp.m_pReportView->MyPopupReportMenu(point, TPM_LEFTALIGN | TPM_TOPALIGN);
}

/**
 * virtual SetSelectItem()
 *  アイテムの選択状態を変更する
 */
void CReportListCtrl::SetSelectItem( const int nItem )
{
	if ( nItem >= 0 ) {
		util::MySetListCtrlItemFocusedAndSelected( *this, GetSelectedItem(), false );
		util::MySetListCtrlItemFocusedAndSelected( *this, nItem, true );
	}
}
