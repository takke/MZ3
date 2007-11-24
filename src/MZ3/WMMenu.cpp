/**
 * based on the sample from http://wind-master.dip.jp/
 */
// WMMenu.cpp: CWMMenu クラスのインプリメンテーション
//
//////////////////////////////////////////////////////////////////////


#include "stdafx.h"
#include "WMMenu.h"
#include "resourceppc.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

/*****************************************************************************
 | 定数定義
 *****************************************************************************
*/
// アイテムのサイズ・余白
#define CX_LSP		20	// 左の余白（アイコン描画部分）
#define CX_RSP		16	// 右の余白（メニュー文字列とショートカット文字列の間の余白）
#define CX_STRSP	4	// 文字列の余白
#define CY_ITEM		22	// アイテムの高さ
#define CX_ITEMSP	( CX_LSP + CX_STRSP )	// アイテムの文字列以外の余白全体のサイズ

#define ICON_SIZE	16	// アイコンのサイズ（幅・高さ）
#define X_ICON		( ( CX_LSP - ICON_SIZE ) / 2 )
#define Y_ICON		( ( CY_ITEM - ICON_SIZE ) / 2 )


// カラー
#define CL_SELECTED		RGB( 120, 160, 200 )	// 選択
#define CL_SELBORDER	RGB(   0,  50, 128 )	// 選択の境界線
#define CL_NORMAL_STR	RGB(   0,   0,   0 )	// 通常の文字列
#define CL_SELECTED_STR	RGB( 255, 255, 255 )	// 選択状態の文字列
#define CL_GRAYED_STR	RGB( 140, 140, 140 )	// 無効の文字列
#define CL_SEPARATOR	RGB( 128, 128, 128 )	// セパレータ
#define CL_BACKGROUND	RGB( 241, 240, 239 )	// 背景
#define CL_BGCHECK		RGB( 241, 240, 239 )	// チェックの背景
#define CL_CHECKBORDER	RGB(  64,  64,  64 )	// チェックの境界線



/*****************************************************************************
 | 関数名 | CWMMenu
 +--------+-------------------------------------------------------------------
 |  説明  | コンストラクタ
 +--------+-------------------------------------------------------------------
 |  引数  | なし
 +--------+-------------------------------------------------------------------
 | 戻り値 | なし
 *****************************************************************************
*/
CWMMenu::CWMMenu(CImageList* pImageList)
{
	m_cxMaxStr = 0;
	m_cxMaxAccelerator = 0;
	m_pImageList = pImageList;
}


/*****************************************************************************
 | 関数名 | ~CWMMenu
 +--------+-------------------------------------------------------------------
 |  説明  | デストラクタ
 +--------+-------------------------------------------------------------------
 |  引数  | なし
 +--------+-------------------------------------------------------------------
 | 戻り値 | なし
 *****************************************************************************
*/
CWMMenu::~CWMMenu()
{
	DestroyMenu();
}


/*****************************************************************************
 | 関数名 | SetItemBitmap
 +--------+-------------------------------------------------------------------
 |  説明  | メニューアイテムにビットマップを関連付ける
 +--------+-------------------------------------------------------------------
 |  引数  | pMenuBmps         メニューに関連付ける、コマンドＩＤとビットマッ
 |        |                   プ情報を持つ構造体の配列
 |        +-------------------------------------------------------------------
 |        | nBmps             pMenuBmpの配列数
 +--------+-------------------------------------------------------------------
 | 戻り値 | 正常終了なら０以外を返し、それ以外は０を返す
 *****************************************************************************
*/
BOOL CWMMenu::SetItemBitmaps( WMMENUBITMAP* pMenuBmps, int nBmps )
{
	ASSERT( pMenuBmps );
//	ASSERT( nBmps );

	for ( int i = 0; i < nBmps; i++ )
	{
		// メニューのアイテム情報取得
		MENUITEMINFO miInfo;
		::ZeroMemory( &miInfo, sizeof(MENUITEMINFO) );
		miInfo.cbSize = sizeof(MENUITEMINFO);
		miInfo.fMask = MIIM_DATA;

		if ( ::GetMenuItemInfo( m_hMenu, pMenuBmps[ i ].nID, FALSE, &miInfo ) )
		{
			WMMENUITEM* pMenuItem = (WMMENUITEM*)miInfo.dwItemData;
			ASSERT( pMenuItem );

			pMenuItem->imageIndex = pMenuBmps[ i ].imageIndex;
//			pMenuItem->hBitmap = pMenuBmps[ i ].hBitmap;
/*			pMenuItem->hBitmap = (HBITMAP)::LoadImage( AfxGetInstanceHandle()
													 , MAKEINTRESOURCE( IDB_BITMAP2 )
													 , IMAGE_BITMAP
													 , ICON_SIZE, ICON_SIZE
													 , LR_LOADMAP3DCOLORS );
*/
//			if ( ! pMenuItem->hBitmap )
//			{
//				return FALSE;
//			}
		}
	}

	return TRUE;
}


/*****************************************************************************
 | 関数名 | ToOwnerDraw
 +--------+-------------------------------------------------------------------
 |  説明  | メニュー全てをオーナー描画にする
 +--------+-------------------------------------------------------------------
 |  引数  | pMenu  CMenuへのポインタ
 |        +-------------------------------------------------------------------
 |        | pMenuBmps         メニューに関連付ける、コマンドＩＤとビットマッ
 |        |                   プ情報を持つ構造体の配列（必要ない場合はNULL)
 |        +-------------------------------------------------------------------
 |        | nBmps             pMenuBmpの配列数（pMenuBmpがNULLの場合は無視）
 +--------+-------------------------------------------------------------------
 | 戻り値 | なし
 *****************************************************************************
*/
void CWMMenu::ToOwnerDraw( CMenu* pMenu, WMMENUBITMAP* pMenuBmps, int nBmps )
{
	ASSERT( pMenu );
	ASSERT( pMenu->m_hMenu );

	UINT i = 0;

	while ( 1 )
	{
		// メニューのアイテム情報取得
		MENUITEMINFO miInfo;
		::ZeroMemory( &miInfo, sizeof(MENUITEMINFO) );
		miInfo.cbSize = sizeof (MENUITEMINFO);
		miInfo.fMask = MIIM_TYPE | MIIM_SUBMENU | MIIM_ID;

		TCHAR buf[ 256 ];
		miInfo.dwTypeData = buf;
		miInfo.cch = 256;
		if ( ! ::GetMenuItemInfo( pMenu->m_hMenu, i, TRUE, &miInfo ) )
		{
			break;
		}

		miInfo.fMask = ( pMenu == this ) ? 0 : MIIM_TYPE;

		// メニューのアイテムデータに保持する構造体の準備
		WMMENUITEM* pMenuItem = NULL;
		if ( ! ( miInfo.fType & MFT_SEPARATOR ) )
		{
			pMenuItem = new WMMENUITEM;

			// TAB前と後をわける
			TCHAR* psz = _tcschr( buf, '\t' );
			if ( psz )
			{
				*psz = '\0';
				psz++;
				pMenuItem->sItemAccelerator = psz;
			}

			pMenuItem->pThisMenu	= (CWMMenu*)pMenu;
			pMenuItem->pSubMenu		= NULL;
			pMenuItem->sItemString	= buf;
			pMenuItem->imageIndex	= -1;

			miInfo.fMask |= MIIM_DATA;
			miInfo.dwItemData = (DWORD)pMenuItem;
		}

		// サブメニューがあったら、CWMMenuを新しく作ってアタッチする
		if ( miInfo.hSubMenu )
		{
			ASSERT( pMenuItem );

			pMenuItem->pSubMenu = new CWMMenu( m_pImageList );
			pMenuItem->pSubMenu->Attach( miInfo.hSubMenu );
			ToOwnerDraw( pMenuItem->pSubMenu, pMenuBmps, nBmps );

			if ( pMenuBmps )
			{
			#ifdef _DEBUG
				BOOL bRet = 
			#endif

				pMenuItem->pSubMenu->SetItemBitmaps( pMenuBmps, nBmps );

			#ifdef _DEBUG
				ASSERT( bRet );
			#endif
			}
		}

		// オーナー描画フラグをセット
		if ( pMenu != this )
		{
			miInfo.fType |= MFT_OWNERDRAW;
		}

		if ( ! ::SetMenuItemInfo( pMenu->m_hMenu, i, TRUE, &miInfo ) )
		{
			break;
		}
		
		i++;
	}
}


/*****************************************************************************
 | 関数名 | CreateGrayedBitmap
 +--------+-------------------------------------------------------------------
 |  説明  | 無効アイコンを描画するためのビットマップを生成
 +--------+-------------------------------------------------------------------
 |  引数  | pDC  CDCへのポインタ
 |        +-------------------------------------------------------------------
 |        | w    幅
 |        +-------------------------------------------------------------------
 |        | h    高さ
 +--------+-------------------------------------------------------------------
 | 戻り値 | 正常終了したらビットマップハンドル、それ以外はNULLを返す
 +--------+-------------------------------------------------------------------
 |  備考  | ビットマップは DeleteObject()で破棄しなくてはならない
 *****************************************************************************
*/
HBITMAP CWMMenu::CreateGrayedBitmap( CDC* pDC, int w, int h )
{
	ASSERT( pDC );
	ASSERT( pDC->GetSafeHdc() );

	// ビットマップ情報生成
	DWORD dwSize = sizeof(BITMAPINFOHEADER) + sizeof(RGBQUAD) * 16;
	BITMAPINFO* pbmi = (BITMAPINFO*)new BYTE[ dwSize ];
	::ZeroMemory( pbmi, dwSize );

	pbmi->bmiHeader.biSize			= sizeof(BITMAPINFOHEADER);
	pbmi->bmiHeader.biWidth			= w;
	pbmi->bmiHeader.biHeight		= h;
	pbmi->bmiHeader.biPlanes		= 1;
	pbmi->bmiHeader.biBitCount		= 4;
	pbmi->bmiHeader.biCompression	= BI_RGB;

	// パレットの設定
	for ( int i = 0; i < 16; i++ )
	{
		COLORREF clr;
		switch ( i )
		{
			case 1:	clr = ::GetSysColor( COLOR_3DSHADOW );	break; 
			case 2:	clr = ::GetSysColor( COLOR_3DLIGHT );	break; 
			default:
				clr = ::GetSysColor( COLOR_3DFACE );
				break;
		}
		pbmi->bmiColors[ i ].rgbRed		= GetRValue( clr );
		pbmi->bmiColors[ i ].rgbGreen	= GetGValue( clr );
		pbmi->bmiColors[ i ].rgbBlue	= GetBValue( clr );
	}

	BYTE* pBits;
	HBITMAP hBitmap = ::CreateDIBSection( pDC->GetSafeHdc(), pbmi, DIB_RGB_COLORS, (PVOID*)&pBits, 0, 0 );

	delete [] (BYTE*)pbmi;

	return hBitmap;
}


/*****************************************************************************
 | 関数名 | DrawSelected
 +--------+-------------------------------------------------------------------
 |  説明  | 選択を描画
 +--------+-------------------------------------------------------------------
 |  引数  | pDC        描画するCDCへのポインタ
 |        +-------------------------------------------------------------------
 |        | rItem      メニューアイテムの外接四角形
 |        +-------------------------------------------------------------------
 |        | itemState  DRAWITEMSTRUCT構造体のitemStateと同じ
 +--------+-------------------------------------------------------------------
 | 戻り値 | なし
 *****************************************************************************
*/
void CWMMenu::DrawSelected( CDC* pDC, CRect rItem, UINT itemState )
{
	ASSERT( pDC );
	ASSERT( pDC->GetSafeHdc() );
	if ( ! ( itemState & ODS_SELECTED ) || ( itemState & ODS_GRAYED ) )
	{
		return;
	}
	
	// 選択の描画
	CPen	pen( PS_SOLID, 1, CL_SELBORDER );
	CBrush	brush( CL_SELECTED );

	CPen*	pOldPen		= pDC->SelectObject( &pen );
	CBrush* pOldBrush	= pDC->SelectObject( &brush );

	pDC->Rectangle( &rItem );

	pDC->SelectObject( pOldBrush );
	pDC->SelectObject( pOldPen );
}


/*****************************************************************************
 | 関数名 | DrawString
 +--------+-------------------------------------------------------------------
 |  説明  | メニューの文字列部分を描画
 +--------+-------------------------------------------------------------------
 |  引数  | pDC        描画するCDCへのポインタ
 |        +-------------------------------------------------------------------
 |        | pszStr     描画する文字列
 |        +-------------------------------------------------------------------
 |        | pszAcl     描画するショートカット文字列
 |        +-------------------------------------------------------------------
 |        | rItem      メニューアイテムの外接四角形
 |        +-------------------------------------------------------------------
 |        | itemState  DRAWITEMSTRUCT構造体のitemStateと同じ
 +--------+-------------------------------------------------------------------
 | 戻り値 | なし
 *****************************************************************************
*/
void CWMMenu::DrawString( CDC* pDC, LPCTSTR pszStr, LPCTSTR pszAcl, CRect rItem, UINT itemState )
{
	ASSERT( pDC );
	ASSERT( pDC->GetSafeHdc() );

	// 描画色の設定
	COLORREF OldClr;
	if ( itemState & ODS_GRAYED )
	{ 
		OldClr = pDC->SetTextColor( CL_GRAYED_STR );
	}
	else if ( itemState & ODS_SELECTED )
	{
		OldClr = pDC->SetTextColor( CL_SELECTED_STR );
	}
	else
	{
		OldClr = pDC->SetTextColor( CL_NORMAL_STR );
	}

	// 背景を透過
	int OldBkMode = pDC->GetBkMode();
	pDC->SetBkMode( TRANSPARENT );

	// 文字列を描画する四角形を計算
	rItem.left	+= CX_LSP + CX_STRSP;
	rItem.right	= rItem.left + m_cxMaxStr;

	// 文字列の描画
	pDC->DrawText( pszStr
				 , _tcslen( pszStr )
				 , &rItem
				 , DT_SINGLELINE | DT_EXPANDTABS | DT_LEFT | DT_VCENTER );

	if ( pszAcl )
	{
		// ショートカット文字列を描画する四角形を計算
		rItem.left = rItem.right + CX_RSP;
		rItem.right= rItem.right + m_cxMaxAccelerator;

		// ショートカット文字列の描画
		pDC->DrawText( pszAcl
					 , _tcslen( pszAcl )
					 , &rItem
					 , DT_SINGLELINE | DT_EXPANDTABS | DT_LEFT | DT_VCENTER );
	}

	pDC->SetBkMode( OldBkMode );
	pDC->SetTextColor( OldClr );
}


/*****************************************************************************
 | 関数名 | DrawCheckMark
 +--------+-------------------------------------------------------------------
 |  説明  | 選チェックマークを描画
 +--------+-------------------------------------------------------------------
 |  引数  | pDC        描画するCDCへのポインタ
 |        +-------------------------------------------------------------------
 |        | rItem      メニューアイテムの外接四角形
 |        +-------------------------------------------------------------------
 |        | itemState  DRAWITEMSTRUCT構造体のitemStateと同じ
 +--------+-------------------------------------------------------------------
 | 戻り値 | なし
 *****************************************************************************
*/
void CWMMenu::DrawCheckMark( CDC* pDC, CRect rItem, UINT itemState )
{
	ASSERT( pDC );
	ASSERT( pDC->GetSafeHdc() );

	if ( ! ( itemState & ODS_CHECKED ) )
	{
		return;
	}
	
	// チェックを描画するビットマップ生成
	HBITMAP hBitmap;
	if ( itemState & ODS_GRAYED )
	{
		hBitmap = CreateGrayedBitmap( pDC, ICON_SIZE, ICON_SIZE );
	}
	else
	{
		hBitmap = ::CreateCompatibleBitmap( pDC->GetSafeHdc(), ICON_SIZE, ICON_SIZE ); 
	}

	ASSERT( hBitmap );

	// メモリDCの生成
	CDC memDC;
	memDC.CreateCompatibleDC( pDC );
	HBITMAP hOldBmp	= (HBITMAP)::SelectObject( memDC.GetSafeHdc(), hBitmap );


	// GDIオブジェクト生成
	CPen pen( PS_SOLID, 1, CL_CHECKBORDER );
	CBrush brush( CL_BGCHECK );

	// GDIオブジェクト選択
	CPen*	pOldPen		= memDC.SelectObject( &pen );
	CBrush* pOldBrush	= memDC.SelectObject( &brush );

	// メモリDCにチェックを描画
	memDC.Rectangle( CRect( 0, 0, ICON_SIZE, ICON_SIZE ) );
	POINT points[ 6 ] = { 
							{  5,  6 }
						  , {  7,  8 }
						  , { 10,  5 }
						  , { 10,  7 }
						  , {  7, 10 }
						  , {  5,  8 }
						};

	CBrush brushCheck( CL_CHECKBORDER );
	memDC.SelectObject( &brushCheck );
	memDC.Polygon( points, 6 );


	// チェックを描画する四角形を計算
	rItem.left	 = X_ICON;
	rItem.top	 = rItem.top + Y_ICON;
	rItem.right	 = rItem.left + ICON_SIZE;
	rItem.bottom = rItem.top + ICON_SIZE;

	// メモリDCからpDCに転送
	pDC->BitBlt( rItem.left, rItem.top, rItem.Width(), rItem.Height(), &memDC, 0, 0, SRCCOPY );

	// GDIオブジェクトを元に戻す
	::SelectObject( memDC.GetSafeHdc(), hOldBmp );
	memDC.SelectObject( pOldBrush );
	memDC.SelectObject( pOldPen );

	::DeleteObject( hBitmap );
}


/*****************************************************************************
 | 関数名 | DrawSeparator
 +--------+-------------------------------------------------------------------
 |  説明  | セパレータを描画
 +--------+-------------------------------------------------------------------
 |  引数  | pDC        描画するCDCへのポインタ
 |        +-------------------------------------------------------------------
 |        | rItem      メニューアイテムの外接四角形
 +--------+-------------------------------------------------------------------
 | 戻り値 | なし
 *****************************************************************************
*/
void CWMMenu::DrawSeparator( CDC* pDC, CRect rItem )
{
	ASSERT( pDC );
	ASSERT( pDC->GetSafeHdc() );

	CPen pen( PS_SOLID, 1, CL_SEPARATOR );
	CPen* pOldPen = pDC->SelectObject( &pen );

	pDC->MoveTo( rItem.left  + CX_LSP + CX_STRSP, rItem.top );
	pDC->LineTo( rItem.right - CX_STRSP, rItem.top );

	pDC->SelectObject( pOldPen );
}


/*****************************************************************************
 | 関数名 | DrawBitmap
 +--------+-------------------------------------------------------------------
 |  説明  | ビットマップを描画
 +--------+-------------------------------------------------------------------
 |  引数  | pDC        描画するCDCへのポインタ
 |        +-------------------------------------------------------------------
 |        | imageIndex 描画するビットマップのインデックス
 |        +-------------------------------------------------------------------
 |        | rItem      メニューアイテムの外接四角形
 |        +-------------------------------------------------------------------
 |        | itemState  DRAWITEMSTRUCT構造体のitemStateと同じ
 +--------+-------------------------------------------------------------------
 | 戻り値 | なし
 *****************************************************************************
*/
void CWMMenu::DrawBitmap( CDC* pDC, int imageIndex, CRect rItem, UINT itemState )
{
	ASSERT( pDC );
	ASSERT( pDC->GetSafeHdc() );

	if (m_pImageList==NULL) {
		return;
	}

	// ビットマップ生成
	HBITMAP hWorkBmp;
	if ( itemState & ODS_GRAYED )
	{
		hWorkBmp = CreateGrayedBitmap( pDC, ICON_SIZE, ICON_SIZE );
	}
	else
	{
		hWorkBmp = ::CreateCompatibleBitmap( pDC->GetSafeHdc(), ICON_SIZE, ICON_SIZE ); 
	}

	ASSERT( hWorkBmp );

	// メモリDCの生成
	CDC memWorkDC;
	memWorkDC.CreateCompatibleDC( pDC );
	HBITMAP hOldWorkBmp	= (HBITMAP)::SelectObject( memWorkDC.GetSafeHdc(), hWorkBmp );
	
	// 転送用ビットマップに描画
	m_pImageList->DrawIndirect( &memWorkDC, imageIndex, CPoint(0,0), CSize(ICON_SIZE, ICON_SIZE), CPoint(0,0) );

	// ビットマップを描画する四角形を計算
	CRect rBmp = rItem;
	rBmp.left	= X_ICON;
	rBmp.top	= rBmp.top + Y_ICON;
	rBmp.right	= rBmp.left + ICON_SIZE;
	rBmp.bottom	= rBmp.top + ICON_SIZE;

	// 境界線描画のために四角形を再計算
	rItem.left	= 0;
	rItem.right	= rItem.left + CX_LSP;
	rItem.bottom= rItem.top + CY_ITEM;

	// 境界線の描画
	if ( itemState & ODS_CHECKED )
	{
		pDC->DrawEdge( &rItem, EDGE_ETCHED, BF_RECT | BF_SOFT | BF_MIDDLE );
	}
	else if ( ( itemState & ODS_SELECTED ) && ! ( itemState & ODS_GRAYED ) )
	{
		pDC->DrawEdge( &rItem, EDGE_BUMP, BF_RECT | BF_SOFT | BF_MIDDLE );
	}

	// メモリDCからpDCに転送
	pDC->BitBlt( rBmp.left, rBmp.top, rBmp.Width(), rBmp.Height(), &memWorkDC, 0, 0, SRCCOPY );

	// GDIオブジェクトを元に戻す
	::SelectObject( memWorkDC.GetSafeHdc(), hOldWorkBmp );

	::DeleteObject( hWorkBmp );
}


/*****************************************************************************
 | 関数名 | PrepareOwnerDraw
 +--------+-------------------------------------------------------------------
 |  説明  | オーナードロー準備
 +--------+-------------------------------------------------------------------
 |  引数  | pMenuBmps         メニューに関連付ける、コマンドＩＤとビットマッ
 |        |                   プ情報を持つ構造体の配列（必要ない場合はNULL)
 |        +-------------------------------------------------------------------
 |        | nBmps             pMenuBmpの配列数（pMenuBmpがNULLの場合は無視）
 +--------+-------------------------------------------------------------------
 | 戻り値 | 正常終了なら０以外を返し、それ以外は０を返す
 *****************************************************************************
*/
BOOL CWMMenu::PrepareOwnerDraw( WMMENUBITMAP* pMenuBmps/*=NULL*/, int nBmps/*=0*/ )
{
	ToOwnerDraw( this, pMenuBmps, nBmps );

	if ( pMenuBmps )
	{
	#ifdef _DEBUG
		BOOL bRet = 
	#endif

		SetItemBitmaps( pMenuBmps, nBmps );

	#ifdef _DEBUG
		ASSERT( bRet );
	#endif
	}

	return TRUE;
}


//BOOL CWMMenu::LoadMenu( UINT nIDResource, WMMENUBITMAP* pMenuBmps/*=NULL*/, int nBmps/*=0*/ )
//{
//	return LoadMenu( MAKEINTRESOURCE( nIDResource ), pMenuBmps, nBmps );
//}


/*****************************************************************************
 | 関数名 | DestroyMenu
 +--------+-------------------------------------------------------------------
 |  説明  | メニュー破棄
 +--------+-------------------------------------------------------------------
 |  引数  | pMenu  破棄するメニューへのポインタ
 |        |        （CWMMenuクラス内部以外からはNULLにする）
 +--------+-------------------------------------------------------------------
 | 戻り値 | 正常終了なら０以外を返し、それ以外は０を返す
 *****************************************************************************
*/
BOOL CWMMenu::DestroyMenu( CWMMenu* pMenu/*=NULL*/ )
{
	BOOL bCallOrgDestroyMenu = FALSE;

	if ( ! pMenu )
	{
		if ( ! m_hMenu )
		{
			return FALSE;
		}
		pMenu = this;
		bCallOrgDestroyMenu = TRUE;
	}

	ASSERT( pMenu->m_hMenu );

	UINT i = 0;

	while ( 1 )
	{
		// メニューのアイテム情報取得
		MENUITEMINFO miInfo;
		::ZeroMemory( &miInfo, sizeof(MENUITEMINFO) );
		miInfo.cbSize = sizeof (MENUITEMINFO);
		miInfo.fMask = MIIM_TYPE | MIIM_SUBMENU | MIIM_DATA;
		if ( ! ::GetMenuItemInfo( pMenu->m_hMenu, i++, TRUE, &miInfo ) )
		{
			break;
		}

		// セパレータ以外に対して処理する
		if ( ! ( miInfo.fType & MFT_SEPARATOR ) )
		{
			WMMENUITEM* pMenuItem = (WMMENUITEM*)miInfo.dwItemData;

//			ASSERT( pMenuItem );
			if (pMenuItem != NULL) {

				// サブメニューがあったら、再起呼び出しして破棄
				if ( pMenuItem->pSubMenu )
				{
					DestroyMenu( pMenuItem->pSubMenu );
					pMenuItem->pSubMenu->Detach();
					delete pMenuItem->pSubMenu;
				}

				delete pMenuItem;
			}
		}
	}

	if ( bCallOrgDestroyMenu )
	{
		CMenu::DestroyMenu();
	}

	return TRUE;
}


/*****************************************************************************
 | 関数名 | DrawItem
 +--------+-------------------------------------------------------------------
 |  説明  | オーナー描画（CMenuの仮想関数）
 +--------+-------------------------------------------------------------------
 |  引数  | pDis
 +--------+-------------------------------------------------------------------
 | 戻り値 | なし
 *****************************************************************************
*/
void CWMMenu::DrawItem( LPDRAWITEMSTRUCT pDis )
{
	if ( pDis->CtlType != ODT_MENU )
	{
		return;
	}

	// メニューのアイテム情報取得
	MENUITEMINFO miInfo;
	::ZeroMemory( &miInfo, sizeof(MENUITEMINFO) );
	miInfo.cbSize = sizeof (MENUITEMINFO);
	miInfo.fMask = MIIM_TYPE | MIIM_DATA;

	if ( ! ::GetMenuItemInfo( m_hMenu, pDis->itemID, FALSE, &miInfo ) )
	{
		ASSERT( FALSE );
		return;
	}

	CDC dc;
	dc.Attach( pDis->hDC );

	// 背景を塗りつぶす
	CRect r = pDis->rcItem;
	r.right = CX_LSP;
	dc.FillRect( &r, &CBrush( ::GetSysColor( COLOR_3DFACE ) ) );

	r.right = pDis->rcItem.right;
	r.left  = CX_LSP;
	dc.FillRect( &r, &CBrush( CL_BACKGROUND ) );

	
	if ( ! ( miInfo.fType & MFT_SEPARATOR ) )
	{
		// 選択を描画
		DrawSelected( &dc, pDis->rcItem, pDis->itemState );

		// 文字列を描画
		WMMENUITEM* pMenuItem = (WMMENUITEM*)miInfo.dwItemData;
		if ( pMenuItem->sItemAccelerator.IsEmpty() )
		{
			DrawString( &dc, pMenuItem->sItemString, NULL, pDis->rcItem, pDis->itemState );
		}
		else
		{
			DrawString( &dc, pMenuItem->sItemString, pMenuItem->sItemAccelerator, pDis->rcItem, pDis->itemState );
		}
		
		// チェックマークを描画
		DrawCheckMark( &dc, pDis->rcItem, pDis->itemState );

		// ビットマップ描画
		if ( pMenuItem->imageIndex>=0 )
		{
			DrawBitmap( &dc, pMenuItem->imageIndex, pDis->rcItem, pDis->itemState );
		}
	}
	else
	{
		// セパレータを描画
		DrawSeparator( &dc, pDis->rcItem );
	}

	dc.Detach();
}


/*****************************************************************************
 | 関数名 | MeasureItem
 +--------+-------------------------------------------------------------------
 |  説明  | メニュー情報の設定（CMenuの仮想関数）
 +--------+-------------------------------------------------------------------
 |  引数  | lpMeasureItemStruct
 +--------+-------------------------------------------------------------------
 | 戻り値 | なし
 *****************************************************************************
*/
void CWMMenu::MeasureItem( LPMEASUREITEMSTRUCT lpMeasureItemStruct )
{
	if ( lpMeasureItemStruct->CtlType != ODT_MENU )
	{
		return;
	}

	WMMENUITEM* pMenuItem = (WMMENUITEM*)lpMeasureItemStruct->itemData;
	if ( ! pMenuItem )
	{
		// セパレータ
		lpMeasureItemStruct->itemWidth = 1;
		lpMeasureItemStruct->itemHeight= 1;
		return;
	}

	// メニュー文字列を表示するための四角形を計算する
	HDC hDC = ::GetDC( NULL );

	// メニューフォントの取得
//	NONCLIENTMETRICS nm;
//	nm.cbSize = sizeof(NONCLIENTMETRICS);
//	SystemParametersInfo( SPI_GETNONCLIENTMETRICS, nm.cbSize, &nm, 0 );
//	HFONT hFont = ::CreateFontIndirect( &nm.lfMenuFont );
//	HFONT hOldFont = (HFONT)::SelectObject( hDC, hFont );

	// 文字列を描画するために必要な四角形を取得する
	CRect rItemStr( 0, 0, 0, 0 );
	::DrawText( hDC, pMenuItem->sItemString, pMenuItem->sItemString.GetLength(), &rItemStr, DT_CALCRECT | DT_SINGLELINE | DT_EXPANDTABS );

	CRect rItemAcl( 0, 0, 0, 0 );
	if ( ! pMenuItem->sItemAccelerator.IsEmpty() )
	{
		::DrawText( hDC, pMenuItem->sItemAccelerator, pMenuItem->sItemAccelerator.GetLength(), &rItemAcl, DT_CALCRECT | DT_SINGLELINE | DT_EXPANDTABS );
	}

//	::SelectObject( hDC, hOldFont );
//	::DeleteObject( hFont );
	::ReleaseDC( NULL, hDC );

	// メニューの幅と高さを設定
	pMenuItem->pThisMenu->m_cxMaxStr = max( pMenuItem->pThisMenu->m_cxMaxStr, (UINT)rItemStr.Width() );
	if ( rItemAcl.Width() )
	{
		pMenuItem->pThisMenu->m_cxMaxAccelerator = max( pMenuItem->pThisMenu->m_cxMaxAccelerator, (UINT)rItemAcl.Width() + CX_RSP );
	}

	lpMeasureItemStruct->itemWidth = pMenuItem->pThisMenu->m_cxMaxStr + pMenuItem->pThisMenu->m_cxMaxAccelerator + CX_ITEMSP;
	lpMeasureItemStruct->itemHeight= max( CY_ITEM, rItemStr.Height() );
}
