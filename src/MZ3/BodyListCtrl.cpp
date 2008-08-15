/*
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */
// BodyListCtrl.cpp : 実装ファイル
//

#include "stdafx.h"
#include "MZ3.h"
#include "BodyListCtrl.h"
#include "MixiData.h"
#include "IniFile.h"
#include "util.h"
#include "util_gui.h"
#include "MixiParserUtil.h"
#include "MZ3View.h"

static const int OFFSET_FIRST	= 2*2;
static const int OFFSET_OTHER	= 6*2;

// CBodyListCtrl

IMPLEMENT_DYNAMIC(CBodyListCtrl, CTouchListCtrl)

CBodyListCtrl::CBodyListCtrl()
	: m_bStopDraw(false)
{
}

CBodyListCtrl::~CBodyListCtrl()
{
}


BEGIN_MESSAGE_MAP(CBodyListCtrl, CTouchListCtrl)
	ON_WM_ERASEBKGND()
	ON_NOTIFY_REFLECT_EX(LVN_ITEMCHANGED, &CBodyListCtrl::OnLvnItemchanged)
	ON_WM_VSCROLL()
	ON_WM_MOUSEWHEEL()
#ifndef WINCE
	ON_WM_NCCALCSIZE()
#endif
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_MEASUREITEM_REFLECT()
END_MESSAGE_MAP()



// CBodyListCtrl メッセージ ハンドラ



/**
 * PreTranslateMessage
 *  ・トラブルシュート用
 */
BOOL CBodyListCtrl::PreTranslateMessage(MSG* pMsg)
{
/*	switch (pMsg->message) {
	case WM_MOUSEMOVE:
		break;

	case WM_PAINT:
		// ちらつき防止のため遮断
		return TRUE;

	default:
		MZ3_TRACE( L"CBodyListCtrl::PreTranslateMessage(0x%04X, 0x%04X, 0x%04X)\n", pMsg->message, pMsg->wParam, pMsg->lParam);
		break;
	}
*/
	return CTouchListCtrl::PreTranslateMessage(pMsg);
}

void CBodyListCtrl::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	if( m_bStopDraw )
		return;

	CDC* pDC = CDC::FromHandle(lpDrawItemStruct->hDC);

	// 再描画するItemの座標を取得
	CRect rcItem( lpDrawItemStruct->rcItem );

	// アイテムのID
	int nItem = lpDrawItemStruct->itemID;

	// 現在コントロールにフォーカスがあるか否かのフラグを設定
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

	int nIconSize = 16;
	if (theApp.m_optionMng.m_bMainViewBodyListIntegratedColumnMode && 
		theApp.m_optionMng.GetFontHeight()>=16) 
	{
		nIconSize = 32;
	}

	// アイテムのラベルの幅を取得
	CRect rcLabel;
	this->GetItemRect(nItem, rcLabel, LVIR_LABEL);
	// 表示位置をrcItemに合わせる
	rcLabel.MoveToY( rcItem.top );
	if (m_bUseIcon==false) {
		// アイコンなしの場合は、アイコン分だけオフセットをかける
		if (rcLabel.left > nIconSize) {
			rcLabel.left -= nIconSize;
		}
	}

	// 左の位置を同じにする
	rcAllLabels.left = rcLabel.left;

	// 選択されている場合は、
	// 選択されている文字のみ一行を塗りつぶす
	if (bSelected == TRUE) {
		pDC->FillRect(rcAllLabels, &CBrush(::GetSysColor(COLOR_HIGHLIGHT)));
	}else{
		// 背景の塗りつぶし
		if( IsDrawBk() ) {
			if( !theApp.m_optionMng.IsUseBgImage() || !theApp.m_bgImageMainBodyCtrl.isEnableImage() ) {
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
				int offset = 0;
				if( IsScrollWithBk() ){
					offset = ( h * GetTopIndex() ) % theApp.m_bgImageMainBodyCtrl.getBitmapSize().cy;
				}
				util::DrawBitmap( pDC->GetSafeHdc(), theApp.m_bgImageMainBodyCtrl.getHandle(), x, y , w, h, x, y + offset );
			}
		}
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
	if (m_bUseIcon) {
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
				if (lvi.iImage >= 0) {
					ImageList_DrawEx(
						pImageList->m_hImageList, lvi.iImage,
						pDC->m_hDC,
						rcIcon.left, rcIcon.top, nIconSize, nIconSize,
						clrMaskBk, clrMaskFg,
						uiFlags | nOvlImageMask);
				}
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

//	rcLabel = rcItem;
	rcLabel.left  += OFFSET_FIRST;
	rcLabel.right -= OFFSET_FIRST;

	// 文字色の変更
	COLORREF clrTextSave = (COLORREF)-1;
	COLORREF clrBkSave   = (COLORREF)-1;
	if (bSelected == TRUE) {
		// 選択状態なので、システム標準の選択色で塗りつぶす
		clrTextSave = pDC->SetTextColor(::GetSysColor(COLOR_HIGHLIGHTTEXT));
		clrBkSave = pDC->SetBkColor(::GetSysColor(COLOR_HIGHLIGHT));
	}
	else {
		// 非選択状態なので、状態に応じて色を変更する
		CCategoryItem* pCategory = theApp.m_pMainView->m_selGroup->getSelectedCategory();
		if (pCategory!=NULL && 0 <= lvi.lParam && lvi.lParam < (int)pCategory->m_body.size()) {
			CMixiData* data = &pCategory->m_body[ lvi.lParam ];

			COLORREF clrTextFg = theApp.m_skininfo.clrMainBodyListDefaultText;
			switch (data->GetAccessType()) {
			case ACCESS_BBS:
			case ACCESS_EVENT:
			case ACCESS_EVENT_JOIN:
			case ACCESS_ENQUETE:
				// コミュニティ、イベント、アンケート
				// 既読数に応じて色づけ。
				{
					int lastIndex = mixi::ParserUtil::GetLastIndexFromIniFile(*data);
					if (lastIndex == -1) {
						// 全くの未読
						clrTextFg = theApp.m_skininfo.clrMainBodyListNonreadText;
					} else if (lastIndex >= data->GetCommentCount()) {
						// 既読
						clrTextFg = theApp.m_skininfo.clrMainBodyListDefaultText;
					} else {
						// 未読分あり：新着記事
						clrTextFg = theApp.m_skininfo.clrMainBodyListNewItemText;
					}
				}
				break;

			case ACCESS_DIARY:
			case ACCESS_MYDIARY:
				// 日記
				// 外部ブログは薄く表示
				if( data->GetURL().Find( L"?url=http" ) != -1 ) {
					// "?url=http" を含むので外部ブログとみなす
					clrTextFg = theApp.m_skininfo.clrMainBodyListExternalBlogText;
				} else {
					// mixi 日記
					// 未読なら青、既読なら黒
					if( util::ExistFile(util::MakeLogfilePath( *data )) ) {
						// ログあり:既読
						clrTextFg = theApp.m_skininfo.clrMainBodyListDefaultText;
					}else{
						// ログなし:未読
						clrTextFg = theApp.m_skininfo.clrMainBodyListNonreadText;
					}
				}
				break;

			case ACCESS_NEWS:
			case ACCESS_MESSAGE:
				// ニュース
				// ログがあれば（既読なら）黒、未読なら青
				if( util::ExistFile(util::MakeLogfilePath( *data )) ) {
					// ログあり:既読
					clrTextFg = theApp.m_skininfo.clrMainBodyListDefaultText;
				}else{
					// ログなし:未読
					clrTextFg = theApp.m_skininfo.clrMainBodyListNonreadText;
				}
				break;

			case ACCESS_PROFILE:
			case ACCESS_BIRTHDAY:
				// ユーザプロフィール
				// マイミクなら青にする。
				if( data->IsMyMixi() ) {
					clrTextFg = theApp.m_skininfo.clrMainBodyListFootprintMyMixiText;
				}else{
					clrTextFg = theApp.m_skininfo.clrMainBodyListDefaultText;
				}
				break;

			case ACCESS_COMMUNITY:
				// コミュニティ
				{
					// 暫定処置として、トピック一覧のログ存在チェックで色変更を行う。

					// トピック一覧用 mixi オブジェクトを生成する
					CMixiData mixi = *data;
					mixi.SetAccessType( ACCESS_LIST_BBS );
					CString url;
					url.Format( L"list_bbs.pl?id=%d", mixi::MixiUrlParser::GetID(mixi.GetURL()) );
					mixi.SetURL(url);

					// 存在チェック。
					if( util::ExistFile(util::MakeLogfilePath(mixi) ) ) {
						// ログあり:既読
						clrTextFg = theApp.m_skininfo.clrMainBodyListDefaultText;
					}else{
						// ログなし:未読
						clrTextFg = theApp.m_skininfo.clrMainBodyListNonreadText;
					}
				}
				break;

			case ACCESS_TWITTER_USER:
				// Twitter 項目
				{
					// 選択項目と同じオーナーIDの項目を強調表示する。
					int selectedIdx = util::MyGetListCtrlSelectedItemIndex(*this);
					CCategoryItem* pCategory = theApp.m_pMainView->m_selGroup->getSelectedCategory();
					if (pCategory!=NULL && 0 <= selectedIdx && selectedIdx < (int)pCategory->m_body.size()) {
						const CMixiData& selectedData = pCategory->m_body[ selectedIdx ];
						if (selectedData.GetOwnerID()==data->GetOwnerID()) {
							// 同じオーナーID：強調表示
							clrTextFg = theApp.m_skininfo.clrMainBodyListNonreadText;
						} else {
							// 異なるオーナーID
							clrTextFg = theApp.m_skininfo.clrMainBodyListDefaultText;
						}
					}
				}
				break;

			default:
				// 色づけなし
				// 黒にする
				clrTextFg = theApp.m_skininfo.clrMainBodyListDefaultText;
				break;
			}

			// 色の設定
			clrTextSave = pDC->SetTextColor(clrTextFg);
		}
	}

	if (theApp.m_optionMng.m_bMainViewBodyListIntegratedColumnMode) {
		// 統合カラムモード、1行目
		CRect rcDraw = rcAllLabels;

		rcDraw.left += OFFSET_FIRST;
		pDC->DrawText(pszText,
			-1,
			rcDraw,
			DT_LEFT | DT_SINGLELINE | DT_NOPREFIX | DT_NOCLIP | DT_TOP | DT_END_ELLIPSIS);
	} else {
		// 第1カラム
		pDC->DrawText(pszText,
			-1,
			rcLabel,
			DT_LEFT | DT_SINGLELINE | DT_NOPREFIX | DT_NOCLIP | DT_VCENTER | DT_END_ELLIPSIS);
	}

	//--- 右側カラム

	// カラム用のラベルを描画
	LV_COLUMN lvc;
	lvc.mask = LVCF_FMT | LVCF_WIDTH;
	// 元の色に戻す
	for (int nColumn = 1; this->GetColumn(nColumn, &lvc); nColumn++) {
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

		if (theApp.m_optionMng.m_bMainViewBodyListIntegratedColumnMode) {
			// 統合カラムモード、nColumn 行目の描画
			// TODO とりあえず2行限定とし、半分の高さとする。
			CRect rcDraw = rcAllLabels;
			rcDraw.top    += rcDraw.Height()/2;
			rcDraw.bottom -= 2;
			rcDraw.left   += OFFSET_FIRST;
			rcDraw.left   += 16;

			pDC->DrawText(pszText,
				-1,
				rcDraw,
				nJustify | DT_SINGLELINE | DT_NOPREFIX | DT_NOCLIP | DT_BOTTOM | DT_RIGHT | DT_END_ELLIPSIS);
		} else {
			// 第 N カラム
			rcSubItem.left  = rcSubItem.right;
			rcSubItem.right = rcSubItem.left + lvc.cx;

			CRect rcDraw = rcSubItem;
			rcDraw.left  += OFFSET_OTHER;
			rcDraw.right -= OFFSET_OTHER;

			pDC->DrawText(pszText,
				-1,
				rcDraw,
				nJustify | DT_SINGLELINE | DT_NOPREFIX | DT_NOCLIP | DT_VCENTER | DT_END_ELLIPSIS);
		}
	}

	// 色を戻す
	if (clrTextSave != (COLORREF)-1) {
		clrTextSave = pDC->SetTextColor(clrTextSave);
	}
	if (clrBkSave != (COLORREF)-1) {
		clrBkSave = pDC->SetBkColor(clrBkSave);
	}

	// 日付区切りバーの描画
	// 現在の要素 (nItem) の上側に線を引く
	// 判定処理
	bool bDrawDayBreakBar = false;
	if (nItem>=1) { // 2番目以降の要素であること。
		if (theApp.m_pMainView != NULL &&
			theApp.m_pMainView->m_selGroup != NULL &&
			theApp.m_pMainView->m_selGroup->getSelectedCategory() != NULL) 
		{
			CCategoryItem* pCategory = theApp.m_pMainView->m_selGroup->getSelectedCategory();
			CMixiDataList& list = pCategory->GetBodyList();
			if (list.size()>(size_t)nItem) {
				// 日付取得
				CTime t0 = list[nItem-1].GetDateRaw();
				CTime t1 = list[nItem  ].GetDateRaw();
				if (t0.GetYear()  != t1.GetYear() ||
					t0.GetMonth() != t1.GetMonth() ||
					t0.GetDay()   != t1.GetDay())
				{
					// 日付が異なる
					bDrawDayBreakBar = true;
				}
			}
		}
	}
	// 描画処理
	if (bDrawDayBreakBar) {
		COLORREF clrDayBreakBar = theApp.m_skininfo.clrMainBodyListDayBreakLine;
		CPen penDayBreakBar(PS_SOLID, 1, clrDayBreakBar);

		CPen* pOldPen = pDC->SelectObject(&penDayBreakBar);

		pDC->MoveTo(rcAllLabels.left, rcAllLabels.top);
		pDC->LineTo(rcAllLabels.right, rcAllLabels.top);

		pDC->SelectObject(pOldPen);
//		wprintf( L"draw line (%d,%d) to (%d,%d)\n", 
//			rcAllLabels.left, rcAllLabels.top,
//			rcAllLabels.right, rcAllLabels.top);
	}

	// アイテムがフォーカスを持っているときに
	// その選択枠を描画する
	if ((lvi.state & LVIS_FOCUSED) == LVIS_FOCUSED && bFocus == TRUE) {
		pDC->DrawFocusRect(rcAllLabels);
	}
}

/**
 * ヘッダー文字を変更する。
 *
 * NULL ならそのインデックスの文字は変更しない。
 */
void CBodyListCtrl::SetHeader(LPCTSTR col1, LPCTSTR col2)
{
	HDITEM hdi;
	TCHAR lpBuffer[128];

	hdi.mask = HDI_TEXT;
	hdi.pszText = lpBuffer;
	hdi.cchTextMax = 128;

	if( col1 != NULL ) {
		this->GetHeaderCtrl()->GetItem(0, &hdi);
		wcscpy(hdi.pszText, col1);
		this->GetHeaderCtrl()->SetItem(0, &hdi);
	}

	if( col2 != NULL ) {
		this->GetHeaderCtrl()->GetItem(1, &hdi);
		wcscpy(hdi.pszText, col2);
		this->GetHeaderCtrl()->SetItem(1, &hdi);
	}
}


BOOL CBodyListCtrl::OnEraseBkgnd(CDC* pDC)
{
	//MZ3_TRACE( L" OnEraseBkgnd()\n");

	pDC->SetBkMode( TRANSPARENT );

	// ビットマップの初期化と描画
	if( theApp.m_optionMng.IsUseBgImage() ) {
		CRect rectClient;
		this->GetClientRect( &rectClient );

		theApp.m_bgImageMainBodyCtrl.load();

		if (theApp.m_bgImageMainBodyCtrl.isEnableImage()) {
			int x = rectClient.left;
			int y = rectClient.top;
			int w = rectClient.Width();
			int h = rectClient.Height();
			int offset = 0;
			if( IsScrollWithBk() ){
				if( GetItemCount() > 0) {
					CRect rcItem;
					GetItemRect( 0 , &rcItem , LVIR_BOUNDS );
					offset = ( rcItem.Height() * GetTopIndex() ) % theApp.m_bgImageMainBodyCtrl.getBitmapSize().cy;
				}
			}
			util::DrawBitmap( pDC->GetSafeHdc(), theApp.m_bgImageMainBodyCtrl.getHandle(), x, y, w, h, x, y + offset );
			return TRUE;
		}
	}

	return CListCtrl::OnEraseBkgnd(pDC);
}

BOOL CBodyListCtrl::OnLvnItemchanged(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);

	// 最初か最後の項目であれば全体を再描画（背景画像のずれ防止）
	int iStart = GetTopIndex();
	int iEnd   = iStart + GetCountPerPage();
	if( pNMLV->iItem < iStart || pNMLV->iItem >= iEnd ) {
		if( pNMLV->uNewState & LVIS_FOCUSED ) {
			Invalidate( FALSE );
		}
	}

	*pResult = 0;

	// FALSE を返すことで、親ウィンドウにもイベントを渡す
	return FALSE;
}


void CBodyListCtrl::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// スクロール時の背景画像のずれ防止
	if (theApp.m_optionMng.IsUseBgImage()) {
		// スクロール位置が変化していたら再描画
		static int s_nLastPos = nPos;
		if( s_nLastPos != nPos ) {
			//Invalidate( FALSE );			// ちらつき防止のためここでは再描画しない
			s_nLastPos = nPos;

			theApp.m_pMainView->MoveMiniImageDlg();
		}
	}

	CTouchListCtrl::OnVScroll(nSBCode, nPos, pScrollBar);
}

BOOL CBodyListCtrl::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	// スクロール時の背景画像のずれ防止
	if (theApp.m_optionMng.IsUseBgImage()) {
		// スクロール位置が変化していたら再描画
		int pos = GetScrollPos(SB_VERT);
		static int s_lastPos = pos;
		if (pos != s_lastPos) {
			//Invalidate( FALSE );			// ちらつき防止のためここでは再描画しない
			s_lastPos = pos;

			theApp.m_pMainView->MoveMiniImageDlg();
		}
	}

	return CTouchListCtrl::OnMouseWheel(nFlags, zDelta, pt);
	//// 右クリック＋マウスホイール処理のために親呼び出し → CTouchListCtrlに移行
	//return theApp.m_pMainView->OnMouseWheel(nFlags, zDelta, pt);
}

#ifndef WINCE
void CBodyListCtrl::OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS* lpncsp)
{
	// 水平スクロールバーの非表示
	this->EnableScrollBarCtrl( SB_HORZ, FALSE );

	CListCtrl::OnNcCalcSize(bCalcValidRects, lpncsp);
}
#endif

void CBodyListCtrl::OnMouseMove(UINT nFlags, CPoint point)
{
#ifndef WINCE
//	TRACE( L"OnMouseMove %d,%d\n", point.x, point.y );
	// オプションがOnならmini画像画面を移動
	if (theApp.m_optionMng.m_bShowMainViewMiniImage &&
		theApp.m_optionMng.m_bShowMainViewMiniImageDlg &&
		theApp.m_optionMng.m_bShowMainViewMiniImageOnMouseOver ) 
	{
		int idx = HitTest(point);
		if (idx>=0) {
			TRACE( L" idx=%d\n", idx );

			// 画像位置変更
			theApp.m_pMainView->MoveMiniImageDlg( idx, point.x, point.y );
		}
	}
#endif
	CTouchListCtrl::OnMouseMove(nFlags, point);
}

void CBodyListCtrl::OnLButtonDown(UINT nFlags, CPoint point)
{
/* CTouchListCtrl::OnLButtonDown()とCBodyListCtrl::SetSelectItem()に移行
	// 選択変更
	int nItem = HitTest(point);
	if (nItem>=0) {
		int idx = util::MyGetListCtrlSelectedItemIndex( *this );
		if( GetItemCount() > 0 && idx >= 0 ) {
			util::MySetListCtrlItemFocusedAndSelected( *this, idx, false );
			util::MySetListCtrlItemFocusedAndSelected( *this, nItem, true );
		}
	}
*/
/* CTouchListCtrl::OnLButtonDown()とCBodyListCtrl::PopupContextMenu()に移行
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
		theApp.m_pMainView->PopupBodyMenu(point, TPM_LEFTALIGN | TPM_TOPALIGN);
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
HBITMAP CBodyListCtrl::GetBgBitmapHandle()
{
	if( theApp.m_optionMng.IsUseBgImage() ) {
		theApp.m_bgImageMainBodyCtrl.load();
		if(theApp.m_bgImageMainBodyCtrl.isEnableImage()) {
			return theApp.m_bgImageMainBodyCtrl.getHandle();
		}
	}
	return NULL;
}

/**
 * virtual PopupContextMenu()
 *  ポップアップメニューを開く
 */
void CBodyListCtrl::PopupContextMenu( const CPoint point )
{
	// TODO 本来は WM_COMMAND で通知すべき。
	theApp.m_pMainView->PopupBodyMenu(point, TPM_LEFTALIGN | TPM_TOPALIGN);
}

/**
 * virtual SetSelectItem()
 *  アイテムの選択状態を変更する
 */
void CBodyListCtrl::SetSelectItem( const int nItem )
{
	if (nItem>=0) {
		int idx = util::MyGetListCtrlSelectedItemIndex( *this );
		if( GetItemCount() > 0 && idx >= 0 ) {
			util::MySetListCtrlItemFocusedAndSelected( *this, idx, false );
		}
		util::MySetListCtrlItemFocusedAndSelected( *this, nItem, true );
	}
}

void CBodyListCtrl::MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct)
{
	LOGFONT lf;
	GetFont()->GetLogFont( &lf );

	if (lf.lfHeight < 0) {
		lpMeasureItemStruct->itemHeight = -lf.lfHeight;
	} else {
		lpMeasureItemStruct->itemHeight = lf.lfHeight;
	}

	if (theApp.m_optionMng.m_bMainViewBodyListIntegratedColumnMode) {
		// 統合カラムモードのため高さをN倍する
		if (lpMeasureItemStruct->itemHeight < 0) {
			lpMeasureItemStruct->itemHeight = lpMeasureItemStruct->itemHeight*2 -3;
		} else {
			lpMeasureItemStruct->itemHeight = lpMeasureItemStruct->itemHeight*2 +3;
		}
	}
}
