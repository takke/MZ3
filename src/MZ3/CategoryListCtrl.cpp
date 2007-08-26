// CategoryListCtrl.cpp : 実装ファイル
//

#include "stdafx.h"
#include "MZ3.h"
#include "CategoryListCtrl.h"
#include "util.h"

static const int OFFSET_FIRST = 2;
static const int OFFSET_OTHER = 6;

// CCategoryListCtrl

IMPLEMENT_DYNAMIC(CCategoryListCtrl, CListCtrl)

CCategoryListCtrl::CCategoryListCtrl()
	: m_bStopDraw(false)
{
	// 色のデフォルト値を設定
	m_clrBgFirst    = ::GetSysColor(COLOR_WINDOW);
	// とりあえず赤に設定
	m_clrBgSecond	= RGB( 255,0,0);
	m_clrFgFirst	= ::GetSysColor(COLOR_WINDOWTEXT);
	m_clrFgSecond	= ::GetSysColor(COLOR_WINDOWTEXT);
	m_activeItem	= 0;
}

CCategoryListCtrl::~CCategoryListCtrl()
{
}


BEGIN_MESSAGE_MAP(CCategoryListCtrl, CListCtrl)
	ON_WM_ERASEBKGND()
	ON_WM_VSCROLL()
	ON_NOTIFY_REFLECT_EX(LVN_ITEMCHANGED, &CCategoryListCtrl::OnLvnItemchanged)
END_MESSAGE_MAP()


// CCategoryListCtrl メッセージ ハンドラ

void CCategoryListCtrl::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	if( m_bStopDraw )
		return;
	CDC* pDC = CDC::FromHandle(lpDrawItemStruct->hDC);

	// 再描画するItemの座標を取得
	CRect rcItem(lpDrawItemStruct->rcItem);

	// 背景を透明にするフラグ（イメージデータ用）
//	UINT uiFlags = ILD_TRANSPARENT;

	// イメージオブジェクト格納アドレス
//	CImageList* pImageList;

	// アイテムのID
	int nItem = lpDrawItemStruct->itemID;

	// 現在コントロールにフォーカスがあるか否かの
	// フラグを設定
	BOOL bFocus = FALSE;
	if (GetFocus() == this) {
		bFocus = TRUE;
	}

	// 描画する色
	COLORREF clrTextFg;
	COLORREF clrImage = m_clrBgFirst;

	// アイテム データを取得します。
	LV_ITEM lvi;

	lvi.mask = LVIF_IMAGE | LVIF_STATE | LVIF_PARAM;
	lvi.iItem = nItem;
	lvi.iSubItem = 0;
	lvi.stateMask = LVIS_SELECTED | LVIS_STATEIMAGEMASK;
	this->GetItem(&lvi);

	CString strText = this->GetItemText( nItem, 0 );

	// アイテムが選択状態か否かのフラグを設定
	BOOL bSelected = (bFocus || (GetStyle() & LVS_SHOWSELALWAYS))
		&& lvi.state & LVIS_SELECTED;
	bSelected =	bSelected || (lvi.state & LVIS_DROPHILITED);

	// アイテムの表示されている幅を取得
	CRect rcAllLabels;
	this->GetItemRect(nItem, rcAllLabels, LVIR_BOUNDS);

	// アイテムのラベルの幅を取得
	CRect rcLabel;
	this->GetItemRect(nItem, rcLabel, LVIR_LABEL);

	// 左の位置を同じにする
	rcAllLabels.left = rcLabel.left;

	// 選択されている場合は、一行を塗りつぶす
	if (bSelected == TRUE) {
		pDC->FillRect(rcAllLabels, &CBrush(::GetSysColor(COLOR_HIGHLIGHT)));
	}else{
		// 背景の塗りつぶし
		if( !theApp.m_optionMng.IsUseBgImage() ) {
			// 背景画像なしの場合
			pDC->FillRect(rcAllLabels, &CBrush(RGB(0xFF, 0xFF, 0xFF)));
		}else{
			// ビットマップの描画
			CRect rectClient;
			this->GetClientRect( &rectClient );

			int x = lpDrawItemStruct->rcItem.left;
			int y = lpDrawItemStruct->rcItem.top;
			int w = rectClient.Width();
			int h = lpDrawItemStruct->rcItem.bottom - y;
			util::DrawBitmap( pDC->GetSafeHdc(), m_bgImage.getHandle(), x, y, w, h, x, y );
		}
	}

	// 色を設定してアイコンをマスクします。
/*	if ((lvi.state & LVIS_CUT) == LVIS_CUT) {
		uiFlags |= ILD_BLEND50;
	}
	else if (bSelected == TRUE) {
		clrImage = ::GetSysColor(COLOR_HIGHLIGHT);
		uiFlags |= ILD_BLEND50;
	}

	// 状態アイコンを描画します。
	UINT nStateImageMask = lvi.state & LVIS_STATEIMAGEMASK;
	if ((nStateImageMask>>12) > 0) {

		int nImage = (nStateImageMask>>12) - 1;

		pImageList = this->GetImageList(LVSIL_STATE);
		if (pImageList) {
			pImageList->Draw(pDC, nImage,
				CPoint(rcItem.left, rcItem.top),
				ILD_TRANSPARENT);
		}
	}

	// 通常のアイコンとオーバーレイアイコンを描画します。
	CRect rcIcon;
	this->GetItemRect(nItem, rcIcon, LVIR_ICON);
	pImageList = this->GetImageList(LVSIL_SMALL);

	if (pImageList != NULL) {
		UINT nOvlImageMask = lvi.state & LVIS_OVERLAYMASK;
		if (rcItem.left < rcItem.right - 1) {
			ImageList_DrawEx(
				pImageList->m_hImageList, lvi.iImage,
				pDC->m_hDC,
				rcIcon.left, rcIcon.top, 16, 16,
				m_clrBgFirst, clrImage,
				uiFlags | nOvlImageMask);
		}
	}
*/
	// アイテムのラベルを描きます。
	this->GetItemRect(nItem, rcItem, LVIR_LABEL);

	rcLabel = rcItem;
	rcLabel.left += OFFSET_FIRST;
	rcLabel.right -= OFFSET_FIRST;

	COLORREF clrTextSave, clrBkSave;
	if (bSelected) {
		// システム標準の選択色で塗りつぶす
		clrTextSave = pDC->SetTextColor(::GetSysColor(COLOR_HIGHLIGHTTEXT));
		clrBkSave = pDC->SetBkColor(::GetSysColor(COLOR_HIGHLIGHT));
	}else{
		// 非選択状態なので、アクティブなら赤、そうでなければ黒で描画
		if (lvi.iItem == GetActiveItem()) {
			clrTextFg = RGB(0xFF, 0x00, 0x00);
		}
		else {
			clrTextFg = RGB(0x00, 0x00, 0x00);
		}

		// 色づけ処理
		clrTextSave = pDC->SetTextColor(clrTextFg);
	}

	pDC->DrawText(strText,
		rcLabel,
		DT_LEFT | DT_SINGLELINE | DT_NOPREFIX | DT_NOCLIP | DT_VCENTER| DT_END_ELLIPSIS);

	// カラム用のラベルを描画
	LV_COLUMN lvc;
	lvc.mask = LVCF_FMT | LVCF_WIDTH;

	// 第2カラム以降の描画
	for (int nColumn = 1; this->GetColumn(nColumn, &lvc); nColumn++) {
		rcItem.left = rcItem.right;
		rcItem.right += lvc.cx;

		strText = this->GetItemText(nItem, nColumn);
		if (strText.GetLength() == 0) {
			continue;
		}

		UINT nJustify = DT_LEFT;

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

		rcLabel = rcItem;
		rcLabel.left += OFFSET_OTHER;
		rcLabel.right -= OFFSET_OTHER;

		pDC->DrawText(strText,
			rcLabel,
			nJustify | DT_SINGLELINE | DT_NOPREFIX | DT_NOCLIP | DT_VCENTER | DT_END_ELLIPSIS);
	}

	if (bSelected) {
		clrTextSave = pDC->SetTextColor(clrTextSave);
		clrBkSave = pDC->SetBkColor(clrBkSave);
	}

	// アイテムがフォーカスを持っているときに
	// その選択枠を描画する
	if ((lvi.state & LVIS_FOCUSED) == LVIS_FOCUSED && bFocus == TRUE) {
		pDC->DrawFocusRect(rcAllLabels);
	}
}

BOOL CCategoryListCtrl::OnEraseBkgnd(CDC* pDC)
{
	if( m_bStopDraw )
		return TRUE;

	pDC->SetBkMode( TRANSPARENT );

	// ビットマップの初期化と描画
	if( theApp.m_optionMng.IsUseBgImage() ) {
		CRect rectClient;
		this->GetClientRect( &rectClient );

		m_bgImage.load( L"header.jpg" );

		int x = rectClient.left;
		int y = rectClient.top;
		int w = rectClient.Width();
		int h = rectClient.Height();
		util::DrawBitmap( pDC->GetSafeHdc(), m_bgImage.getHandle(), x, y, w, h, x, y );
		return TRUE;
	}

	return CListCtrl::OnEraseBkgnd(pDC);
}

void CCategoryListCtrl::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// スクロール時の背景画像のずれ防止
	Invalidate( FALSE );

	CListCtrl::OnVScroll(nSBCode, nPos, pScrollBar);
}

BOOL CCategoryListCtrl::OnLvnItemchanged(NMHDR *pNMHDR, LRESULT *pResult)
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

