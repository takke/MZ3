// ReportListCtrl.cpp : 実装ファイル
//

#include "stdafx.h"
#include "MZ3.h"
#include "ReportListCtrl.h"
#include "util.h"

static const int OFFSET_FIRST = 2;
static const int OFFSET_OTHER = 6;

// CReportListCtrl

IMPLEMENT_DYNAMIC(CReportListCtrl, CListCtrl)

CReportListCtrl::CReportListCtrl()
{
	// 色のデフォルト値を設定
	m_clrBgFirst    = ::GetSysColor(COLOR_WINDOW);
	// とりあえず赤に設定
	m_clrBgSecond = RGB( 255,0,0);
	m_clrFgFirst = ::GetSysColor(COLOR_WINDOWTEXT);
	m_clrFgSecond = ::GetSysColor(COLOR_WINDOWTEXT);
}

CReportListCtrl::~CReportListCtrl()
{
}


BEGIN_MESSAGE_MAP(CReportListCtrl, CListCtrl)
	ON_WM_ERASEBKGND()
	ON_NOTIFY_REFLECT_EX(LVN_ITEMCHANGED, &CReportListCtrl::OnLvnItemchanged)
END_MESSAGE_MAP()



// CReportListCtrl メッセージ ハンドラ

void CReportListCtrl::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	CDC* pDC = CDC::FromHandle(lpDrawItemStruct->hDC);

	CRect rectClient;
	this->GetClientRect( &rectClient );

	// ビットマップの描画
	if( theApp.m_optionMng.IsUseBgImage() ) {
		int x = lpDrawItemStruct->rcItem.left;
		int y = lpDrawItemStruct->rcItem.top;
		int w = rectClient.Width();
		int h = lpDrawItemStruct->rcItem.bottom - y;
		util::DrawBitmap( pDC->GetSafeHdc(), theApp.m_bgImageReportListCtrl.getHandle(), x, y, w, h, x, y );
	}

	// 再描画するItemの座標を取得
	CRect rcItem(lpDrawItemStruct->rcItem);

	// 背景を透明にするフラグ（イメージデータ用）
	UINT uiFlags = ILD_TRANSPARENT;

	// イメージオブジェクト格納アドレス
	CImageList* pImageList;

	// アイテムのID
	int nItem = lpDrawItemStruct->itemID;

	// 現在コントロールにフォーカスがあるか否かの
	// フラグを設定
	BOOL bFocus = FALSE;
	if (GetFocus() == this) {
		bFocus = TRUE;
	}

	// 描画する色
	COLORREF clrTextSave, clrBkSave;
	COLORREF clrTextFg;
	COLORREF clrImage = m_clrBgFirst;

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

	// アイテムのラベルの幅を取得
	CRect rcLabel;
	this->GetItemRect(nItem, rcLabel, LVIR_LABEL);

	// 左の位置を同じにする
	rcAllLabels.left = rcLabel.left;

	// 文字の横幅を取得
	CSize szSize = pDC->GetTextExtent(szBuff, (int)wcslen(szBuff));
	// 塗りつぶし範囲を文字部分にする
	CRect rcItemStr;
	rcItemStr = rcLabel;
	rcItemStr.right =	rcItemStr.left + szSize.cx + OFFSET_OTHER;

	// 背景の塗りつぶし（背景画像なしの場合）
	if( !theApp.m_optionMng.IsUseBgImage() ) {
		pDC->FillRect(rcAllLabels, &CBrush(RGB(0xFF, 0xFF, 0xFF)));
	}

	// 選択されている場合は、
	// 選択されている文字のみ一行を塗りつぶす
	if (bSelected == TRUE) {
		pDC->FillRect(rcAllLabels, &CBrush(::GetSysColor(COLOR_HIGHLIGHT)));
	}

	// 色を設定してアイコンをマスクします。
	if ((lvi.state & LVIS_CUT) == LVIS_CUT) {
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
			if (lvi.iImage != 0) {
				ImageList_DrawEx(
					pImageList->m_hImageList, lvi.iImage,
					pDC->m_hDC,
					rcIcon.left, rcIcon.top, 16, 16,
					m_clrBgFirst, clrImage,
					uiFlags | nOvlImageMask);
			}
		}
	}

	// アイテムのラベルを描きます。
	this->GetItemRect(nItem, rcItem, LVIR_LABEL);

	pszText = szBuff;

	rcLabel = rcItem;
	rcLabel.left += OFFSET_FIRST;
	rcLabel.right -= OFFSET_FIRST;

	// システム標準の選択色で塗りつぶす
	if (bSelected == TRUE) {
		clrTextSave = pDC->SetTextColor(::GetSysColor(COLOR_HIGHLIGHTTEXT));
		clrBkSave = pDC->SetBkColor(::GetSysColor(COLOR_HIGHLIGHT));
	}
	else {
		clrTextFg = RGB(0x00, 0x00, 0x00);

		// 色づけ処理
		clrTextSave = pDC->SetTextColor(clrTextFg);
	}

	pDC->DrawText(pszText,
		-1,
		rcLabel,
		DT_LEFT | DT_SINGLELINE | DT_NOPREFIX | DT_NOCLIP | DT_VCENTER | DT_END_ELLIPSIS);

	// カラム用のラベルを描画
	LV_COLUMN lvc;
	lvc.mask = LVCF_FMT | LVCF_WIDTH;
	// 元の色に戻す
	for (int nColumn = 1; this->GetColumn(nColumn, &lvc); nColumn++) {
		rcItem.left = rcItem.right;
		rcItem.right += lvc.cx;

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

		rcLabel = rcItem;
		rcLabel.left += OFFSET_OTHER;
		rcLabel.right -= OFFSET_OTHER;

		pDC->DrawText(pszText,
			-1,
			rcLabel,
			nJustify | DT_SINGLELINE | DT_NOPREFIX | DT_NOCLIP | DT_VCENTER | DT_END_ELLIPSIS);
	}

	if (bSelected == TRUE) {
		clrTextSave = pDC->SetTextColor(clrTextSave);
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
	POSITION pos = GetFirstSelectedItemPosition();
	if( pos == NULL ) {
		return -1;
	}
	int idx = GetNextSelectedItem( pos );
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

		int x = rectClient.left;
		int y = rectClient.top;
		int w = rectClient.Width();
		int h = rectClient.Height();
		util::DrawBitmap( pDC->GetSafeHdc(), theApp.m_bgImageReportListCtrl.getHandle(), x, y, w, h, x, y );
		return TRUE;
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
			Invalidate( FALSE );
			s_nLastPos = nPos;
		}
	}

	CListCtrl::OnVScroll(nSBCode, nPos, pScrollBar);
}
