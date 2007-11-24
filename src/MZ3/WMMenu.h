/**
 * based on the sample from http://wind-master.dip.jp/
 */
// WMMenu.h: CWMMenu クラスのインターフェイス
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_WMMENU_H__56FB55C1_D713_4904_94E8_5B76F6F791DD__INCLUDED_)
#define AFX_WMMENU_H__56FB55C1_D713_4904_94E8_5B76F6F791DD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CWMMenu;

/*****************************************************************************
 | WMMENUBITMAP 構造体
 *****************************************************************************
*/
typedef struct tagWMMENUBITMAP
{
	UINT	nID;		// コマンドＩＤ
	int		imageIndex;	// 画像インデックス
} WMMENUBITMAP;


/*****************************************************************************
 | WMMENUITEM 構造体
 *****************************************************************************
*/
typedef struct tagWMMENUITEM
{
	CWMMenu*	pThisMenu;			// MeasureItem でポップアップがサブメニューのハンドルになってしまうので自分へのポインタを持っておく
	CWMMenu*	pSubMenu;			// サブメニューへのポインタ
	CString		sItemString;		// アイテムの文字列
	CString		sItemAccelerator;	// アイテムのショートカット文字列
//	HBITMAP		hBitmap;			// アイコン用のビットマップ
	int			imageIndex;			// 画像インデックス
} WMMENUITEM;


/*****************************************************************************
 | CWMMenu クラス
 *****************************************************************************
*/
class CWMMenu : public CMenu
{
public:
	CWMMenu(CImageList* pImageList);
	virtual ~CWMMenu();

////////// アトリビュート //////////
private:
	UINT		m_cxMaxStr;			// メニューの文字部分の最大幅
	UINT		m_cxMaxAccelerator;	// メニューのショートカットキー部分の最大幅

	CImageList*	m_pImageList;		// 画像リスト

////////// オペレーション //////////
private:
	BOOL SetItemBitmaps( WMMENUBITMAP* pMenuBmps, int nBmps );				// メニューアイテムにビットマップを関連付ける
	void ToOwnerDraw( CMenu* pMenu, WMMENUBITMAP* pMenuBmp, int nBmps );	// メニュー全てをオーナー描画にする

	// アイコン
	HBITMAP CreateGrayedBitmap( CDC* pDC, int w, int h );  // 無効アイコンを描画するためのビットマップを生成

	// メニュー描画
	void DrawSelected( CDC* pDC, CRect rItem, UINT itemState );		// 選択を描画
	void DrawString( CDC* pDC, LPCTSTR pszStr, LPCTSTR pszAcl, CRect rItem, UINT itemState );  // メニューの文字列部分を描画
	void DrawCheckMark( CDC* pDC, CRect rItem, UINT itemState );	// チェックマークを描画
	void DrawSeparator( CDC* pDC, CRect rItem );					// セパレータを描画
	void DrawBitmap( CDC* pDC, int imageIndex, CRect rItem, UINT itemState );		// ビットマップを描画

public:
	// メニューのロード／破棄
	BOOL PrepareOwnerDraw( WMMENUBITMAP* pMenuBmps=NULL, int nBmps=0 );				// オーナードロー準備
	virtual BOOL DestroyMenu( CWMMenu* pMenu=NULL );								// メニュー破棄

protected:
	// オーバーライド
	virtual void DrawItem( LPDRAWITEMSTRUCT pDis );
	virtual void MeasureItem( LPMEASUREITEMSTRUCT lpMeasureItemStruct );
};

#endif // !defined(AFX_WMMENU_H__56FB55C1_D713_4904_94E8_5B76F6F791DD__INCLUDED_)
