/*
 * Copyright (c) MZ3 Project Team All rights reserved.
 * Code licensed under the BSD License:
 * http://www.mz3.jp/license.txt
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

/// カラムモードのスタイル
namespace COLUMN_MODE_STYLE {
static const int BOX_MARGIN_BOTTOM_PT = 2;	///< ボックス間マージン(下側)[pt]
static const int FIRST_MARGIN_LEFT	= 4;	///< 第1カラム、左側マージン
static const int FIRST_MARGIN_RIGHT	= 4;	///< 第1カラム、右側マージン
static const int OTHER_MARGIN_LEFT	= 12;	///< 第2カラム以降、左側マージン
static const int OTHER_MARGIN_RIGHT	= 12;	///< 第2カラム以降、右側マージン
}

/// 統合カラムモードのスタイル
namespace INTEGRATED_MODE_STYLE {
static const int BOX_MARGIN_BOTTOM_PT   = 2;	///< ボックス間マージン(下側)[pt]
static const int EACH_LINE_MARGIN_PT    = 1;	///< 行間マージン[pt]
static const int FIRST_LINE_MARGIN_LEFT = 4;	///< 1行目、左マージン
static const int OTHER_LINE_MARGIN_LEFT = 4+16;	///< 2行目以降、左マージン
};

/* 統合カラムモード
 *            ------x-----------------------  x : FIRST_LINE_MARGIN_LEFT 
 *            ------xx----------------------  xx: OTHER_LINE_MARGIN_LEFT
 * 1st-line: |<icon> AAAAAAAAAAAAAAAAAAAAAAA|
 * 2nd-line: |<icon>  BBBBBBBBBBBBBBBBBBBBBB| AとB, BとCの行間: EACH_LINE_MARGIN_PT
 * 3rd-line: |<icon>  CCCCCCCCCCCCCCCCCCCCCC|
 *           |                              y y:  BOX_MARGIN_BOTTOM_PT
 * 1st-line: |<icon> DDDDDDDDDDDDDDDDDDDDDDD|
 * 2nd-line: |<icon>  EEEEEEEEEEEEEEEEEEEEEE|
 * 3rd-line: |<icon>  FFFFFFFFFFFFFFFFFFFFFF|
 */


// CBodyListCtrl

IMPLEMENT_DYNAMIC(CBodyListCtrl, CTouchListCtrl)

CBodyListCtrl::CBodyListCtrl()
{
#ifdef WINCE
	// スクロール時に iPhone 風の再描画を行う
	// TODO 1画面ではやはり足りない。遅延的に複数画面分確保できるようにしてから復活させること。
//	m_bBlackScrollMode = true;
#endif
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
	ON_MESSAGE(WM_SETFONT, OnSetFont)
	ON_WM_LBUTTONUP()
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

//	MZ3_TRACE(L"CBodyListCtrl::DrawItem(), nItem[%d]\n", lpDrawItemStruct->itemID);

	CDC* pDC = CDC::FromHandle(lpDrawItemStruct->hDC);

//	util::StopWatch sw_draw1, sw_draw2, sw_draw4, sw_draw3;

//sw_draw1.start();

	// 再描画するItemの座標を取得
	CRect rcItem( lpDrawItemStruct->rcItem );

	// アイテムのID
	int nItem = lpDrawItemStruct->itemID;

	// 現在コントロールにフォーカスがあるか否かのフラグを設定
	BOOL bFocus = FALSE;
	if (GetFocus() == this) {
		bFocus = TRUE;
	}

	// アイテム データを取得します。
	LV_ITEM lvi;
	lvi.mask = LVIF_IMAGE | LVIF_STATE;
	lvi.iItem = nItem;
	lvi.iSubItem = 0;
	lvi.stateMask = LVIS_SELECTED | LVIS_STATEIMAGEMASK;
	this->GetItem(&lvi);


	// 出力対象の要素を取得する
	CCategoryItem* pCategory = NULL;
	if (theApp.m_pMainView != NULL &&
		theApp.m_pMainView->m_selGroup != NULL &&
		theApp.m_pMainView->m_selGroup->getSelectedCategory() != NULL)
	{
		pCategory = theApp.m_pMainView->m_selGroup->getSelectedCategory();
	}

	// 出力対象の要素
	const CMixiData* pData = NULL;
	if (pCategory!=NULL && 0 <= nItem && nItem < (int)pCategory->m_body.size()) {
		pData = &pCategory->m_body[ nItem ];
	}

	// 選択要素
	int selectedIdx = util::MyGetListCtrlSelectedItemIndex(*this);
	CMixiData* pSelectedData = NULL;
	if (pCategory!=NULL && 0 <= selectedIdx && selectedIdx < (int)pCategory->m_body.size()) {
		pSelectedData = &pCategory->m_body[ selectedIdx ];
	}

	// 出力文字列の取得(pData から変換)
	CString strTarget1;
	CString strTarget2;
	CString strTarget3;
	if (pCategory!=NULL && pData!=NULL) {
		// 第1カラム
		// どの項目を与えるかは、カテゴリ項目データ内の種別で決める。
		// 改行はスペースに置換する。
		strTarget1 = util::MyGetItemByBodyColType(pData, pCategory->m_bodyColType1, false);
		strTarget1.Replace(L"\r\n", L" ");

		// 第2、第3カラム
		if (theApp.m_optionMng.m_bBodyListIntegratedColumnMode) {
			// 統合カラムモードの場合はカテゴリのカラムタイプ(トグルで変更される)に無関係に
			// カテゴリ種別に応じた文字列を設定する
			AccessTypeInfo::BODY_INDICATE_TYPE bodyColType;
			bodyColType = theApp.m_accessTypeInfo.getBodyHeaderCol2Type(pCategory->m_mixi.GetAccessType());
			strTarget2 = util::MyGetItemByBodyColType(pData, bodyColType, false);
			bodyColType = theApp.m_accessTypeInfo.getBodyHeaderCol3Type(pCategory->m_mixi.GetAccessType());
			strTarget3 = util::MyGetItemByBodyColType(pData, bodyColType, false);
			
		} else {
			strTarget2 = util::MyGetItemByBodyColType(pData, pCategory->m_bodyColType2);
			strTarget3 = util::MyGetItemByBodyColType(pData, pCategory->m_bodyColType3);
		}
	}


	// アイテムが選択状態か否かのフラグを設定
	BOOL bSelected =
		(bFocus || (GetStyle() & LVS_SHOWSELALWAYS))
		&& lvi.state & LVIS_SELECTED;
	bSelected = bSelected || (lvi.state & LVIS_DROPHILITED);

//sw_draw1.stop();
//sw_draw2.start();

	// アイテムの表示されている幅を取得
	CRect rcAllLabels;
	this->GetItemRect(nItem, rcAllLabels, LVIR_BOUNDS);
	// 表示位置をrcItemに合わせる
	rcAllLabels.MoveToY( rcItem.top );

	int nIconSize = 16;
	switch (m_iconMode) {
	case ICON_MODE_48:	nIconSize = 48;	break;
	case ICON_MODE_32:	nIconSize = 32;	break;
	default:			nIconSize = 16;	break;
	}

	// アイテムのラベルの幅を取得
	CRect rcLabel;
	this->GetItemRect(nItem, rcLabel, LVIR_LABEL);
	// 表示位置をrcItemに合わせる
	rcLabel.MoveToY( rcItem.top );
	if (m_iconMode==ICON_MODE_NONE) {
		// アイコンなしの場合は、アイコン分だけオフセットをかける
		if (rcLabel.left > nIconSize) {
			rcLabel.left -= nIconSize;
		}
	}

	// 左の位置を同じにする
	rcAllLabels.left = rcLabel.left;

	// 選択されている場合は、
	// 選択されている文字のみ一行を塗りつぶす
	if (bSelected) {
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
	if (m_iconMode!=ICON_MODE_NONE) {
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

//sw_draw2.stop();
//sw_draw3.start();

	// アイテムのラベルを描きます。
	CRect rcSubItem;
	this->GetItemRect(nItem, rcSubItem, LVIR_LABEL);
	// 表示位置をrcItemに合わせる
	rcSubItem.MoveToY( rcItem.top );

	//--- 左側カラム
//	rcLabel = rcItem;
	rcLabel.left  += COLUMN_MODE_STYLE::FIRST_MARGIN_LEFT;
	rcLabel.right -= COLUMN_MODE_STYLE::FIRST_MARGIN_RIGHT;

	// 文字色の変更
	COLORREF clrTextSave = (COLORREF)-1;
	COLORREF clrBkSave   = (COLORREF)-1;
	if (bSelected == TRUE) {
		// 選択状態なので、システム標準の選択色で塗りつぶす
		clrTextSave = pDC->SetTextColor(::GetSysColor(COLOR_HIGHLIGHTTEXT));
		clrBkSave = pDC->SetBkColor(::GetSysColor(COLOR_HIGHLIGHT));
	} else {
		// 非選択状態なので、状態に応じて色を変更する
//		util::StopWatch sw_coloring;

//		sw_coloring.start();
		if (pData!=NULL) {

			COLORREF clrTextFg = theApp.m_skininfo.clrMainBodyListDefaultText;
			switch (pData->GetAccessType()) {
			case ACCESS_BBS:
			case ACCESS_EVENT:
			case ACCESS_EVENT_JOIN:
			case ACCESS_ENQUETE:
				// コミュニティ、イベント、アンケート
				// 既読数に応じて色づけ。
				{
					int lastIndex = mixi::ParserUtil::GetLastIndexFromIniFile(*pData);
					if (lastIndex == -1) {
						// 全くの未読
						clrTextFg = theApp.m_skininfo.clrMainBodyListNonreadText;
					} else if (lastIndex >= pData->GetCommentCount()) {
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
			case ACCESS_NEIGHBORDIARY:
				// 日記
				// 外部ブログは薄く表示
				if( pData->GetURL().Find( L"?url=http" ) != -1 ) {
					// "?url=http" を含むので外部ブログとみなす
					clrTextFg = theApp.m_skininfo.clrMainBodyListExternalBlogText;
				} else {
					// mixi 日記
					// 未読なら青、既読なら黒
					if( util::ExistFile(util::MakeLogfilePath( *pData )) ) {
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
				if( util::ExistFile(util::MakeLogfilePath( *pData )) ) {
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
				if( pData->IsMyMixi() ) {
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
					CMixiData mixi = *pData;
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
					// デフォルト値設定
					clrTextFg = theApp.m_skininfo.clrMainBodyListDefaultText;

					// 自分宛の発言を強調表示する
					const CString& bodyText = pData->GetBody();
					LPCTSTR szMyTwitterID = theApp.m_loginMng.GetTwitterId();
					if (bodyText.Find(util::FormatString(L"@%s", szMyTwitterID))!=-1) {
						// 強調２
						clrTextFg = theApp.m_skininfo.clrMainBodyListEmphasis2;
						break;
					}

					if (pSelectedData==NULL) {
						break;
					}

					// 自分の発言を強調表示する
					const CString& pTargetName = pData->GetName();
					if (pTargetName==szMyTwitterID) {
						// 同じオーナーID：強調表示
						clrTextFg = theApp.m_skininfo.clrMainBodyListEmphasis4;
						break;
					}

					// 選択項目と同じオーナーIDの項目を強調表示する。
					if (pSelectedData->GetOwnerID()==pData->GetOwnerID()) {
						// 同じオーナーID：強調表示
						clrTextFg = theApp.m_skininfo.clrMainBodyListNonreadText;
						break;
					}

					// 選択項目内の引用ユーザ "@xxx @yyy" のいずれかと同じユーザであれば強調表示する

					// 選択項目内の引用ユーザリストを取得する。なければここで作る。
					util::SetTwitterQuoteUsersWhenNotGenerated(pSelectedData);

					// いずれかと一致すれば強調表示
					int n = pSelectedData->GetTextArraySize(L"quote_users");
					for (int i=0; i<n; i++) {
						LPCTSTR szQuoteUser = pSelectedData->GetTextArrayValue(L"quote_users", i);
						
						// 一致すれば強調表示
						if (pTargetName==szQuoteUser) {
							clrTextFg = theApp.m_skininfo.clrMainBodyListEmphasis3;
							break;
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
//		sw_coloring.stop();

//		MZ3LOGGER_DEBUG(
//			util::FormatString(L" *** coloring detect[%dms]",
//				sw_coloring.getElapsedMilliSecUntilStoped()));
	}

//sw_draw3.stop();


	// 各カラム(各行)の描画
//sw_draw4.start();

	// 絵文字を文字列に変換する
	if( LINE_HAS_EMOJI_LINK(strTarget1) ) {
		mixi::ParserUtil::ReplaceEmojiImageToText( strTarget1 );
	}

	if (theApp.m_optionMng.m_bBodyListIntegratedColumnMode) {
		// 統合カラムモードの描画

		//--- 1行目の描画

		// パターンの変換
		CString strLine1 = m_strIntegratedLinePattern1;
		bool bEmpty = true;
		if (m_strIntegratedLinePattern1.Find(L"%1")>=0 && !strTarget1.IsEmpty()) {
			bEmpty = false;
		}
		strLine1.Replace(L"%1", strTarget1);
		if (m_strIntegratedLinePattern1.Find(L"%2")>=0 && !strTarget2.IsEmpty()) {
			bEmpty = false;
		}
		strLine1.Replace(L"%2", strTarget2);
		if (m_strIntegratedLinePattern1.Find(L"%3")>=0 && !strTarget3.IsEmpty()) {
			bEmpty = false;
		}
		strLine1.Replace(L"%3", strTarget3);
		if (bEmpty) {
			strLine1 = L"";
		}

		// <small>...</small> があれば小さくする
		// TODO 指定された範囲だけ小さくするように。とりあえず全体を小さくしちゃう
		CFont* pOriginalFont = NULL;
		if (strLine1.Find(L"<small>")>=0 && strLine1.Find(L"</small>")>=0) {
			strLine1.Replace(L"<small>", L"");
			strLine1.Replace(L"</small>", L"");
			pOriginalFont = pDC->SelectObject(&theApp.m_fontSmall);
		}
		// <big>...</big> があれば大きくする
		// TODO 指定された範囲だけ大きくするように。とりあえず全体を大きくしちゃう
		else if (strLine1.Find(L"<big>")>=0 && strLine1.Find(L"</big>")>=0) {
			strLine1.Replace(L"<big>", L"");
			strLine1.Replace(L"</big>", L"");
			pOriginalFont = pDC->SelectObject(&theApp.m_fontBig);
		}

		// パターンに\tが含まれていれば左右に分割する
		CString strLine1Left = strLine1;
		CString strLine1Right = L"";
		int iSepPos = strLine1.Find(L"\t");
		if( iSepPos >=0 ){
			strLine1Left = strLine1.Left( iSepPos );
			strLine1Right = strLine1.Mid( iSepPos + 1);
			strLine1.Replace( L"\t" , L" " );
		}
		//MZ3_TRACE( L"一行目：%s>■%s■%s■\n" , strLine1 , strLine1Left ,strLine1Right  );

		// 描画
		CRect rcDraw = rcAllLabels;
		rcDraw.left += INTEGRATED_MODE_STYLE::FIRST_LINE_MARGIN_LEFT;
		// 左右分割描画
		CSize csDrawRight = pDC->GetOutputTextExtent( strLine1Right );
		CSize csDrawAllText = pDC->GetOutputTextExtent( strLine1 );
		if( csDrawAllText.cx > rcDraw.Width() && csDrawRight.cx < rcDraw.Width() ) {
			// 二つに分割した文字列を両端に分けて描画する
			// 左側文字列は右側文字列分を除いた領域に描画する
			CRect rcDrawLeft( rcDraw );
			rcDrawLeft.right -= csDrawRight.cx; 
			pDC->DrawText(strLine1Left,
				-1,
				rcDrawLeft,
				DT_LEFT | DT_SINGLELINE | DT_NOPREFIX | DT_NOCLIP | DT_TOP | DT_END_ELLIPSIS);
			// 右側文字列は右寄せで描画する
			pDC->DrawText(strLine1Right,
				-1,
				rcDraw,
				DT_RIGHT | DT_SINGLELINE | DT_NOPREFIX | DT_NOCLIP | DT_TOP | DT_END_ELLIPSIS);
		} else {
			// 右側文字列の幅が描画領域より広いならしょうがないのでそのまま描画する
			pDC->DrawText(strLine1,
				-1,
				rcDraw,
				DT_LEFT | DT_SINGLELINE | DT_NOPREFIX | DT_NOCLIP | DT_TOP | DT_END_ELLIPSIS);
		}

		// フォントを戻す
		if (pOriginalFont!=NULL) {
			pDC->SelectObject(pOriginalFont);
			pOriginalFont = NULL;
		}

		//--- 2行目の描画

		// パターンの変換
		CString strLine2 = m_strIntegratedLinePattern2;
		bEmpty = true;
		if (m_strIntegratedLinePattern2.Find(L"%1")>=0 && !strTarget1.IsEmpty()) {
			bEmpty = false;
		}
		strLine2.Replace(L"%1", strTarget1);
		if (m_strIntegratedLinePattern2.Find(L"%2")>=0 && !strTarget2.IsEmpty()) {
			bEmpty = false;
		}
		strLine2.Replace(L"%2", strTarget2);
		if (m_strIntegratedLinePattern2.Find(L"%3")>=0 && !strTarget3.IsEmpty()) {
			bEmpty = false;
		}
		strLine2.Replace(L"%3", strTarget3);
		if (bEmpty) {
			strLine2 = L"";
		}

		// <small>...</small> があれば小さくする
		// TODO 指定された範囲だけ小さくするように。とりあえず全体を小さくしちゃう
		if (strLine2.Find(L"<small>")>=0 && strLine2.Find(L"</small>")>=0) {
			strLine2.Replace(L"<small>", L"");
			strLine2.Replace(L"</small>", L"");
			pOriginalFont = pDC->SelectObject(&theApp.m_fontSmall);
		}
		// <big>...</big> があれば大きくする
		// TODO 指定された範囲だけ大きくするように。とりあえず全体を大きくしちゃう
		else if (strLine2.Find(L"<big>")>=0 && strLine2.Find(L"</big>")>=0) {
			strLine2.Replace(L"<big>", L"");
			strLine2.Replace(L"</big>", L"");
			pOriginalFont = pDC->SelectObject(&theApp.m_fontBig);
		}

		// 文字列の最後が"(数字)"で終わっていれば分離する
		CString strLine2Left = strLine2;
		CString strLine2Right = L"";
		mixi::ParserUtil::SepalateCountString( strLine2 , strLine2Left , strLine2Right );

		//MZ3_TRACE( L"二行目：%s>■%s■%s■%d\n" , strLine2 , strLine2Left ,strLine2Right , iSepPos );

		// フォントの高さ取得
		LOGFONT lf;
		GetFont()->GetLogFont( &lf );
		// px値に正規化
//		int lfHeightPx = lf.lfHeight < 0 ? -lf.lfHeight : theApp.pt2px(lf.lfHeight);
//		MZ3LOGGER_DEBUG(util::FormatString(
//			L"CBodyListCtrl::DrawItem(), %s, lfHeight : %d, lfHeightPx : %d, lfWidth : %d", 
//			(LPCTSTR)lf.lfFaceName,
//			(int)lf.lfHeight, (int)lfHeightPx,
//			(int)lf.lfWidth));

		CSize charSize = pDC->GetTextExtent(CString(L"●"));
		int lfHeightPx = charSize.cy;

		// 描画
		rcDraw = rcAllLabels;
		rcDraw.top    += lfHeightPx +theApp.pt2px(INTEGRATED_MODE_STYLE::EACH_LINE_MARGIN_PT);
		rcDraw.left   += INTEGRATED_MODE_STYLE::OTHER_LINE_MARGIN_LEFT;
		// 左右分割描画
		csDrawRight = pDC->GetOutputTextExtent( strLine2Right );
		csDrawAllText = pDC->GetOutputTextExtent( strLine2 );
		if( csDrawAllText.cx > rcDraw.Width() && csDrawRight.cx < rcDraw.Width() ) {
			// 二つに分割した文字列を両端に分けて描画する
			// 左側文字列は右側文字列分を除いた領域に描画する
			CRect rcDrawLeft( rcDraw );
			rcDrawLeft.right -= csDrawRight.cx; 
			pDC->DrawText(strLine2Left,
				-1,
				rcDrawLeft,
				DT_SINGLELINE | DT_NOPREFIX | DT_NOCLIP | DT_BOTTOM | DT_LEFT | DT_END_ELLIPSIS);
			// 右側文字列は右寄せで描画する
			pDC->DrawText(strLine2Right,
				-1,
				rcDraw,
				DT_SINGLELINE | DT_NOPREFIX | DT_NOCLIP | DT_BOTTOM | DT_RIGHT | DT_END_ELLIPSIS);

		} else {
			// 右側文字列の幅が描画領域より広いならしょうがないのでそのまま左詰めで描画する
			pDC->DrawText(strLine2,
				-1,
				rcDraw,
				DT_SINGLELINE | DT_NOPREFIX | DT_NOCLIP | DT_BOTTOM | DT_LEFT | DT_END_ELLIPSIS);
		}

		// フォントを戻す
		if (pOriginalFont!=NULL) {
			pDC->SelectObject(pOriginalFont);
			pOriginalFont = NULL;
		}

	} else {
		// 非統合カラムモードの場合の描画処理

		// 第1カラム
		// 文字列の最後が"(数字)"で終わっていれば分離する
		CString strTarget1Left = strTarget1;
		CString strTarget1Right = L"";
		mixi::ParserUtil::SepalateCountString( strTarget1 , strTarget1Left , strTarget1Right );

		// 左右分割描画
		CSize csLabelRight = pDC->GetOutputTextExtent( strTarget1Right );
		CSize csLabelAllText = pDC->GetOutputTextExtent( strTarget1 );
		if( csLabelAllText.cx > rcLabel.Width() && csLabelRight.cx < rcLabel.Width() ) {
			// 二つに分割した文字列を両端に分けて描画する
			// 左側文字列は右側文字列分を除いた領域に描画する
			CRect rcLabelLeft( rcLabel );
			rcLabelLeft.right -= csLabelRight.cx ; 
			pDC->DrawText(strTarget1Left,
				-1,
				rcLabelLeft,
				DT_LEFT | DT_SINGLELINE | DT_NOPREFIX | DT_NOCLIP | DT_TOP | DT_END_ELLIPSIS);
			// 右側文字列は右寄せで描画する
			pDC->DrawText(strTarget1Right,
				-1,
				rcLabel,
				DT_RIGHT | DT_SINGLELINE | DT_NOPREFIX | DT_NOCLIP | DT_TOP | DT_END_ELLIPSIS);
		} else {
			// 右側文字列の幅が描画領域より広いならしょうがないのでそのまま左詰めで描画する
			pDC->DrawText(strTarget1,
				-1,
				rcLabel,
				DT_LEFT | DT_SINGLELINE | DT_NOPREFIX | DT_NOCLIP | DT_VCENTER | DT_END_ELLIPSIS);
		}

		// 第2カラム以降の描画
		LV_COLUMN lvc;
		lvc.mask = LVCF_FMT | LVCF_WIDTH;
		int nSubColumnStartX = 0;	// 各カラムの領域開始位置(lvc.cxずつ増やす)
		if (this->GetColumn(0, &lvc)) {
			// 第1カラムの幅を加算
			nSubColumnStartX += lvc.cx;
		}
		for (int nColumn = 1; this->GetColumn(nColumn, &lvc); nColumn++) {
			LPCTSTR pszText = L"";
			switch (nColumn) {
			case 1:	
				pszText = strTarget2;
				break;
			case 2:	
				pszText = strTarget3;
				break;
			}

			// 第 N カラム
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
			rcSubItem.left  = nSubColumnStartX;
			rcSubItem.right = rcSubItem.left + lvc.cx;

			CRect rcDraw = rcSubItem;
//			MZ3_TRACE(L"★[%d], w[%d]\n", nColumn, rcDraw.Width());
			if (rcDraw.Width()>=1) {
				// 描画領域が1以上なので描画する
				rcDraw.left  += COLUMN_MODE_STYLE::OTHER_MARGIN_LEFT;
				rcDraw.right -= COLUMN_MODE_STYLE::OTHER_MARGIN_RIGHT;

				pDC->DrawText(pszText,
					-1,
					rcDraw,
					nJustify | DT_SINGLELINE | DT_NOPREFIX | DT_NOCLIP | DT_VCENTER | DT_END_ELLIPSIS);
			}

			// 領域開始位置の更新
			nSubColumnStartX += lvc.cx;
		}
	}

	// 色を戻す
	if (clrTextSave != (COLORREF)-1) {
		clrTextSave = pDC->SetTextColor(clrTextSave);
	}
	if (clrBkSave != (COLORREF)-1) {
		clrBkSave = pDC->SetBkColor(clrBkSave);
	}

	// 日付区切りバー(日付変更線)の描画
	// 現在の要素 (nItem) の上側に線を引く
	// 判定処理
	bool bDrawDayBreakBar = false;
	if (pCategory!=NULL) {
		CMixiDataList& list = pCategory->GetBodyList();
		if (list.size()>(size_t)nItem && (size_t)nItem<list.size()-1) {	// 最終要素でないこと
			// 日付取得
			CTime t0 = list[nItem  ].GetDateRaw();
			CTime t1 = list[nItem+1].GetDateRaw();
			if (t0.GetYear()  != t1.GetYear() ||
				t0.GetMonth() != t1.GetMonth() ||
				t0.GetDay()   != t1.GetDay())
			{
				// 日付が異なる
				bDrawDayBreakBar = true;
			}
		}
	}
	// 描画処理
	if (bDrawDayBreakBar) {
		COLORREF clrDayBreakBar = theApp.m_skininfo.clrMainBodyListDayBreakLine;
		CPen penDayBreakBar(PS_SOLID, 1, clrDayBreakBar);

		CPen* pOldPen = pDC->SelectObject(&penDayBreakBar);

		pDC->MoveTo(rcAllLabels.left,  rcAllLabels.bottom-1);
		pDC->LineTo(rcAllLabels.right, rcAllLabels.bottom-1);

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

//sw_draw4.stop();

	// ベンチマーク結果出力
	/*
	MZ3LOGGER_DEBUG(
		util::FormatString(L"DrawItem[%02d] draw[%dms][%dms][%dms][%dms]",
			nItem,
			sw_draw1.getElapsedMilliSecUntilStoped(),
			sw_draw2.getElapsedMilliSecUntilStoped(),
			sw_draw3.getElapsedMilliSecUntilStoped(),
			sw_draw4.getElapsedMilliSecUntilStoped()
			));
	*/
}

/**
 * ヘッダー文字を変更する。
 *
 * NULL ならそのインデックスの文字は変更しない。
 */
void CBodyListCtrl::SetHeader(LPCTSTR col1, LPCTSTR col2, LPCTSTR col3)
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

	if( col3 != NULL ) {
		this->GetHeaderCtrl()->GetItem(2, &hdi);
		wcscpy(hdi.pszText, col3);
		this->GetHeaderCtrl()->SetItem(2, &hdi);
	}
}


BOOL CBodyListCtrl::OnEraseBkgnd(CDC* pDC)
{
	MZ3_TRACE( L"CBodyListCtrl::OnEraseBkgnd()\n");

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

/**
 * 各項目のサイズ計算
 */
void CBodyListCtrl::MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct)
{
//	LOGFONT lf;
//	GetFont()->GetLogFont( &lf );

//	MZ3LOGGER_DEBUG(util::FormatString(L"CBodyListCtrl::MeasureItem(), lfHeight : %d, itemID : %d", 
//		(int)lf.lfHeight, (int)lpMeasureItemStruct->itemID));

	// px値に変換
//	int lfHeightPx = lf.lfHeight < 0 ? -lf.lfHeight : theApp.pt2px(lf.lfHeight);
	CDC* pDC = GetDC();
	CFont* pOldFont = pDC->SelectObject(&theApp.m_font);
	CSize charSize = pDC->GetTextExtent(CString(L"●"));
	pDC->SelectObject(pOldFont);
	ReleaseDC(pDC);

	int lfHeightPx = charSize.cy;
//	MZ3LOGGER_DEBUG(util::FormatString(L"CBodyListCtrl::MeasureItem(), lfHeight : %d, lfHeightPx : %d", 
//		(int)lf.lfHeight, (int)lfHeightPx));


	if (theApp.m_optionMng.m_bBodyListIntegratedColumnMode) {
		// 統合カラムモード：高さをN倍する
		lpMeasureItemStruct->itemHeight = lfHeightPx*2 +theApp.pt2px(INTEGRATED_MODE_STYLE::BOX_MARGIN_BOTTOM_PT);
	} else {
		// カラムモード
		lpMeasureItemStruct->itemHeight = lfHeightPx   +theApp.pt2px(COLUMN_MODE_STYLE::BOX_MARGIN_BOTTOM_PT);
	}

//	MZ3_TRACE(L"CBodyListCtrl::MeasureItem(), itemHeight : %d\n", lpMeasureItemStruct->itemHeight);
}

LRESULT CBodyListCtrl::OnSetFont(WPARAM wParam, LPARAM lParam)
{
	MZ3_TRACE(L"CBodyListCtrl::OnSetFont()\n");

	LRESULT res =  Default();

	CRect rc;
	GetWindowRect( &rc );

	WINDOWPOS wp;
	wp.hwnd = m_hWnd;
	wp.cx = rc.Width();
	wp.cy = rc.Height();
	wp.flags = SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOOWNERZORDER | SWP_NOZORDER;
	SendMessage( WM_WINDOWPOSCHANGED, 0, (LPARAM)&wp );

	return res;
}

void CBodyListCtrl::OnLButtonUp(UINT nFlags, CPoint point)
{
	bool bDragging = m_bPanDragging || m_bScrollDragging;

	// 先に親を呼び出し、選択変更する
	CTouchListCtrl::OnLButtonUp(nFlags, point);

	if (!bDragging) {
		// Twitterでアイコン領域なら引用を追加
		int idx = HitTest(point);
		if (m_iconMode != ICON_MODE_NONE && point.x < m_iconMode) {
			MZ3_TRACE(L"lbu [%d]\n", idx);

			CCategoryItem* pCategory = theApp.m_pMainView->m_selGroup->getSelectedCategory();
			if (pCategory!=NULL && 0 <= idx && idx < (int)pCategory->m_body.size()) {
				CMixiData* data = &pCategory->m_body[ idx ];

				// アイコン領域クリック通知
				// TODO API 化
				if (data->GetAccessType()==ACCESS_TWITTER_USER) {

					// Lua 関数呼び出しで仮実装
					theApp.MyLuaExecute(L"twitter.on_twitter_reply()");
				}
			}
		}
	}
}
