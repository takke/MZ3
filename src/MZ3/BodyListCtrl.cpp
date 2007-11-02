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

static const int OFFSET_FIRST	= 2*2;
static const int OFFSET_OTHER	= 6*2;

// CBodyListCtrl

IMPLEMENT_DYNAMIC(CBodyListCtrl, CListCtrl)

CBodyListCtrl::CBodyListCtrl()
	: m_bStopDraw(false)
{
}

CBodyListCtrl::~CBodyListCtrl()
{
}


BEGIN_MESSAGE_MAP(CBodyListCtrl, CListCtrl)
	ON_WM_ERASEBKGND()
	ON_NOTIFY_REFLECT_EX(LVN_ITEMCHANGED, &CBodyListCtrl::OnLvnItemchanged)
	ON_WM_VSCROLL()
END_MESSAGE_MAP()



// CBodyListCtrl メッセージ ハンドラ



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
			util::DrawBitmap( pDC->GetSafeHdc(), theApp.m_bgImageMainBodyCtrl.getHandle(), x, y, w, h, x, y );
		}
	}

	// アイテムのラベルを描きます。
	this->GetItemRect(nItem, rcItem, LVIR_LABEL);

	//--- 左側カラム

	pszText = szBuff;

	rcLabel = rcItem;
	rcLabel.left += OFFSET_FIRST;
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
		CMixiData* data = (CMixiData*)(lvi.lParam);

		COLORREF clrTextFg = theApp.m_skininfo.clrMainBodyListDefaultText;
		switch (data->GetAccessType()) {
		case ACCESS_BBS:
		case ACCESS_EVENT:
		case ACCESS_ENQUETE:
			// コミュニティ、イベント、アンケート
			// 既読数に応じて色づけ。
			if (data->GetLastIndex() == -1) {
				// 全くの未読
				clrTextFg = theApp.m_skininfo.clrMainBodyListNonreadText;
			} else if (data->GetLastIndex() >= data->GetCommentCount()) {
				// 既読
				clrTextFg = theApp.m_skininfo.clrMainBodyListDefaultText;
			} else {
				// 未読分あり：新着記事
				clrTextFg = theApp.m_skininfo.clrMainBodyListNewItemText;
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

		default:
			// 色づけなし
			// 黒にする
			clrTextFg = theApp.m_skininfo.clrMainBodyListDefaultText;
			break;
		}

		// 色の設定
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

		theApp.m_bgImageMainBodyCtrl.load();

		if (theApp.m_bgImageMainBodyCtrl.isEnableImage()) {
			int x = rectClient.left;
			int y = rectClient.top;
			int w = rectClient.Width();
			int h = rectClient.Height();
			util::DrawBitmap( pDC->GetSafeHdc(), theApp.m_bgImageMainBodyCtrl.getHandle(), x, y, w, h, x, y );
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
	if( pNMLV->iItem < iStart || pNMLV->iItem == iEnd ) {
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
		static int s_nLastPos = nPos;
		if( s_nLastPos != nPos ) {
			Invalidate( FALSE );
			s_nLastPos = nPos;
		}
	}

	CListCtrl::OnVScroll(nSBCode, nPos, pScrollBar);
}
