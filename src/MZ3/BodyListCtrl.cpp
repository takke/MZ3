// BodyListCtrl.cpp : 実装ファイル
//

#include "stdafx.h"
#include "MZ3.h"
#include "BodyListCtrl.h"
#include "MixiData.h"
#include "IniFile.h"
#include "util.h"
#include "MixiParser.h"

static const int OFFSET_FIRST	= 2*2;
static const int OFFSET_OTHER	= 6*2;

// CBodyListCtrl

IMPLEMENT_DYNAMIC(CBodyListCtrl, CListCtrl)

CBodyListCtrl::CBodyListCtrl()
	: m_bStopDraw(false)
{
	// 色のデフォルト値を設定
	m_clrBgFirst    = ::GetSysColor(COLOR_WINDOW);
	// とりあえず赤に設定
	m_clrBgSecond   = RGB( 255,0,0);

	m_clrFgFirst	= ::GetSysColor(COLOR_WINDOWTEXT);
	m_clrFgSecond	= ::GetSysColor(COLOR_WINDOWTEXT);

	m_hBitmap = NULL;
}

CBodyListCtrl::~CBodyListCtrl()
{
	// ビットマップの削除
	if( m_hBitmap != NULL ) {
		DeleteObject( m_hBitmap );
	}
}


BEGIN_MESSAGE_MAP(CBodyListCtrl, CListCtrl)
	ON_WM_ERASEBKGND()
	ON_NOTIFY_REFLECT_EX(LVN_ITEMCHANGED, &CBodyListCtrl::OnLvnItemchanged)
END_MESSAGE_MAP()



// CBodyListCtrl メッセージ ハンドラ



void CBodyListCtrl::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	if( m_bStopDraw )
		return;

	CDC* pDC = CDC::FromHandle(lpDrawItemStruct->hDC);

	// 再描画するItemの座標を取得
	CRect rcItem( lpDrawItemStruct->rcItem );

/*	// 背景を透明にするフラグ（イメージデータ用）
	UINT uiFlags = ILD_TRANSPARENT;

	// イメージオブジェクト格納アドレス
	CImageList* pImageList;
*/
	// アイテムのID
	int nItem = lpDrawItemStruct->itemID;

	// 現在コントロールにフォーカスがあるか否かのフラグを設定
	BOOL bFocus = FALSE;
	if (GetFocus() == this) {
		bFocus = TRUE;
	}

	// 描画する色
	COLORREF clrTextSave, clrBkSave;
	COLORREF clrTextFg = RGB(0x00, 0x00, 0x00);
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

	// 選択されている場合は、
	// 選択されている文字のみ一行を塗りつぶす
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
			util::DrawBitmap( pDC->GetSafeHdc(), m_hBitmap, x, y, w, h, x, y );
		}
	}
/*	// 色を設定してアイコンをマスクします。
	if ((lvi.state & LVIS_CUT) == LVIS_CUT) {
		uiFlags |= ILD_BLEND50;
	}
	else if (bSelected == TRUE) {
		clrImage = ::GetSysColor(COLOR_HIGHLIGHT);
		uiFlags |= ILD_BLEND50;
	}
*/
	// 状態アイコンを描画します。
/*	UINT nStateImageMask = lvi.state & LVIS_STATEIMAGEMASK;
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

	//--- 左側カラム

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
		CMixiData* data = (CMixiData*)(lvi.lParam);

		if( IsUseColor() ) {
			switch (data->GetAccessType()) {
			case ACCESS_BBS:
			case ACCESS_EVENT:
			case ACCESS_ENQUETE:
				// コミュニティ、イベント、アンケート
				// 既読数に応じて色づけ。
				if (data->GetLastIndex() == -1) {
					// 全くの未読
					clrTextFg = RGB(0x00, 0x00, 0xFF);
				}
				else if (data->GetLastIndex() >= data->GetCommentCount()) {
					// 更新なし
					clrTextFg = RGB(0x00, 0x00, 0x00);
				}
				else {
					// 未読分あり
					clrTextFg = RGB(0xFF, 0x00, 0x00);
				}
				break;

			case ACCESS_DIARY:
			case ACCESS_MYDIARY:
				// 日記
				// 外部ブログは薄く表示
				if( data->GetURL().Find( L"?url=http" ) != -1 ) {
					// "?url=http" を含むので外部ブログとみなす
					clrTextFg = RGB(0x80, 0x80, 0x80);
				}else{
					// mixi 日記
					// 未読なら青、既読なら黒
					if( util::ExistFile(util::MakeLogfilePath( *data )) ) {
						// ログあり:既読
						clrTextFg = RGB(0x00, 0x00, 0x00);
					}else{
						// ログなし:未読
						clrTextFg = RGB(0x00, 0x00, 0xFF);
					}
				}
				break;

			case ACCESS_NEWS:
			case ACCESS_MESSAGE:
				// ニュース
				// ログがあれば（既読なら）黒、未読なら青
				if( util::ExistFile(util::MakeLogfilePath( *data )) ) {
					// ログあり:既読
					clrTextFg = RGB(0x00, 0x00, 0x00);
				}else{
					// ログなし:未読
					clrTextFg = RGB(0x00, 0x00, 0xFF);
				}
				break;

			case ACCESS_PROFILE:
				// ユーザプロフィール
				// マイミクなら青にする。
				if( data->IsMyMixi() ) {
					clrTextFg = RGB(0x00, 0x00, 0xFF);
				}else{
					clrTextFg = RGB(0x00, 0x00, 0x00);
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
						clrTextFg = RGB(0x00, 0x00, 0x00);
					}else{
						// ログなし:未読
						clrTextFg = RGB(0x00, 0x00, 0xFF);
					}
				}
				break;

			default:
				// 色づけなし
				// 黒にする
				clrTextFg = RGB(0x00, 0x00, 0x00);
				break;
			}
		}else{
			// 色づけなし
			// 黒にする
			clrTextFg = RGB(0x00, 0x00, 0x00);
		}

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
	// 元の色に戻す
	for (int nColumn = 1; this->GetColumn(nColumn, &lvc); nColumn++) {
		rcItem.left = rcItem.right;
		rcItem.right = rcItem.left + lvc.cx;

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
	pDC->SetBkMode( TRANSPARENT );

	// ビットマップの初期化と描画
	if( theApp.m_optionMng.IsUseBgImage() ) {
		CRect rectClient;
		this->GetClientRect( &rectClient );

		if( m_hBitmap == NULL ) {
			m_hBitmap = SHLoadImageFile( theApp.m_filepath.bodyBgImage );
		}
		int x = rectClient.left;
		int y = rectClient.top;
		int w = rectClient.Width();
		int h = rectClient.Height();
		util::DrawBitmap( pDC->GetSafeHdc(), m_hBitmap, x, y, w, h, x, y );
		return TRUE;
	}

	return CListCtrl::OnEraseBkgnd(pDC);
}

BOOL CBodyListCtrl::OnLvnItemchanged(NMHDR *pNMHDR, LRESULT *pResult)
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

