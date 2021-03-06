/*
 * Copyright (c) MZ3 Project Team All rights reserved.
 * Code licensed under the BSD License:
 * http://www.mz3.jp/license.txt
 */
/**
 * GUI 関連ユーティリティ
 */

#include "stdafx.h"
#include "util_gui.h"

/// ユーティリティ
namespace util
{

/**
 * gsgetfile.dll を用いてファイル選択画面を表示する
 *
 * 成功時は IDOK が返る
 */
int GetOpenFileNameEx(OPENFILENAME* pofn)
{
#ifdef WINCE
	HINSTANCE hInst = LoadLibrary(_T("gsgetfile.dll"));
	if (hInst) {
		BOOL (*gsGetOpenFileName)(OPENFILENAME* pofn);
		(FARPROC&)gsGetOpenFileName = GetProcAddress(hInst, _T("gsGetOpenFileName"));
		if (gsGetOpenFileName) {
			BOOL ret = gsGetOpenFileName(pofn);
			FreeLibrary(hInst);
			return ret;
		}
		FreeLibrary(hInst);
	}
	return GetOpenFileName(pofn);
#else
	// for win32
	return GetOpenFileName(pofn);
#endif
}

/**
 * 指定されたビットマップ hBitmap を hdc に描画する
 */
bool DrawBitmap( HDC hdc, HBITMAP hBitmap, int x, int y, int w, int h, int srcx, int srcy )
{
	if( hBitmap == NULL ) {
		return false;
	}

	BITMAP	bmp;
	GetObject( hBitmap, sizeof(bmp), &bmp );

	HDC hdc1 = CreateCompatibleDC(NULL);
	HBITMAP hBitmapOld = (HBITMAP)SelectObject(hdc1,hBitmap);

	int iSrcWidth  = bmp.bmWidth;
	int iSrcHeight = bmp.bmHeight;
	
	int iSrcY = srcy;
	while( iSrcY < 0 )
	{
		iSrcY += iSrcHeight; 
	}

	int iSrcTop = iSrcY % iSrcHeight;
	int iTop = y;
	do {
		int iLeft = x;
		do {
			// 転送
//			TRACE( L"DrawBitmap [%d,%d,%d,%d,%d,%d]\n", x, y, w, h, tox, toy );
			int wid = min( w, iSrcWidth );
			int hei = min( h, iSrcHeight - iSrcTop );
			BitBlt( hdc, iLeft, iTop, wid, hei , hdc1, srcx, iSrcTop, SRCCOPY );
			if( hei < h ){
				BitBlt( hdc, iLeft, iTop + hei , wid, h - hei , hdc1, srcx, 0, SRCCOPY );
			}
			iLeft += iSrcWidth;
		} while( iLeft < x + w );
		iTop += iSrcHeight;
	} while( iTop < y + h );
	SelectObject( hdc1, hBitmapOld );

	DeleteDC( hdc1 );
	return true;
}

/**
 * 指定されたビットマップ hBitmap を hdc に描画する
 */
bool DrawBitmap( HDC hdc, HBITMAP hBitmap, int x, int y, int w, int h, int tox, int toy, int tow, int toh )
{
	if( hBitmap == NULL ) {
		return false;
	}

	if (w==tow && h==toh) {
		return DrawBitmap( hdc, hBitmap, tox, toy, w, h, x, y );
	}

	BITMAP	bmp;
	GetObject( hBitmap, sizeof(bmp), &bmp );

	HDC hdc1 = CreateCompatibleDC(NULL);
	HBITMAP hBitmapOld = (HBITMAP)SelectObject(hdc1,hBitmap);

	// 転送
//	TRACE( L"DrawBitmap [%d,%d,%d,%d,%d,%d]\n", x, y, w, h, tox, toy );
#ifdef WINCE
	SetStretchBltMode( hdc, COLORONCOLOR );
#else
	SetStretchBltMode( hdc, HALFTONE );
#endif
	StretchBlt( hdc, tox, toy, tow, toh, hdc1, x, y, w, h, SRCCOPY );

	SelectObject( hdc1, hBitmapOld );

	DeleteDC( hdc1 );
	return true;
}

/**
 * 指定されたIDのウィンドウを移動する
 */
void MoveDlgItemWindow( CWnd* pParent, int idc, int x, int y, int nWidth, int nHeight )
{
	if (pParent==NULL) {
		return;
	}

	CWnd* pDlgItem = pParent->GetDlgItem(idc);
	if (pDlgItem==NULL) {
		return;
	}

	pDlgItem->MoveWindow( x, y, nWidth, nHeight );
}

/**
 * リストコントロールのフォーカス状態と選択状態を設定する。
 */
void MySetListCtrlItemFocusedAndSelected( CListCtrl& listCtrl, int idx, bool bFocusedAndSelected )
{
	UINT nState = 0;
	if( bFocusedAndSelected ) {
		nState |= LVIS_FOCUSED | LVIS_SELECTED;
	}

	listCtrl.SetItemState( idx, nState, LVIS_FOCUSED | LVIS_SELECTED );
}

/**
 * リストコントロールの最初の選択アイテムのインデックスを取得する
 *
 * @return インデックス（未発見時は -1）
 */
int MyGetListCtrlSelectedItemIndex( CListCtrl& listCtrl )
{
	POSITION pos = listCtrl.GetFirstSelectedItemPosition();
	if (pos==NULL) {
		return -1;
	}
	int idx = listCtrl.GetNextSelectedItem( pos );
	return idx;
}

/**
 * リストコントロールのアイテムのイメージインデックスを変更する
 */
bool MySetListCtrlItemImageIndex( CListCtrl& listCtrl, int itemIndex, int subItemIndex, int imageIndex )
{
	LVITEM lvitem;
	lvitem.mask = LVIF_IMAGE;
	lvitem.iItem = itemIndex;
	lvitem.iSubItem = subItemIndex;
	lvitem.iImage   = imageIndex;	// イメージ変更
	listCtrl.SetItem( &lvitem );

	return true;
}

/**
 * リストコントロール内で指定されたインデックスが表示状態にあるかどうかを返す。
 */
bool IsVisibleOnListBox( CListCtrl& listCtrl, int idx )
{
	int idxTop  = listCtrl.GetTopIndex();				// 画面の一番上の表示項目のインデックス
	int idxLast = idxTop + listCtrl.GetCountPerPage();	// 画面の一番下の表示項目のインデックス+1
	if (idxTop <= idx && idx < idxLast) {
		// 描画範囲内
		return true;
	}
	return false;
}

/**
 * フォルダ選択画面を表示する
 *
 * @param hWnd			[in] 親画面
 * @param szTitle		[in] フォルダ選択画面のタイトル
 * @param strFolderPath	[in/out] 初期選択フォルダ＆選択済みフォルダパス
 * @return 成功時は true、失敗時は false を返す。
 */
bool GetOpenFolderPath( HWND hWnd, LPCTSTR szTitle, CString& strFolderPath )
{
#ifdef WINCE
	// FDQ.DLL を用いてフォルダ選択画面を表示する

	// DLLを取得する（Windowsディレクトに無いときはフルパスで指定する）
	HINSTANCE hInst = LoadLibrary (_T("FDQ.DLL"));
	if( hInst == NULL ) {
		MessageBox( NULL, L"FDQ.DLL not found", L"", MB_OK );
		return false;
	}
	int (pascal *FolderTree) (HWND,TCHAR*,TCHAR*);

	FolderTree = (int (pascal *) (HWND,TCHAR*,TCHAR*)) GetProcAddress (hInst, TEXT("FolderTree"));
	if (FolderTree == 0) {
		FreeLibrary (hInst);
		return false;	//エラー
	}

	// 保存先変更画面の表示
	TCHAR szFileName[256] = L"";
	wcsncpy( szFileName, strFolderPath, 255 );
	TCHAR szTitleBuf[256] = L"";
	wcsncpy( szTitleBuf, szTitle, 255 );
	int rc = FolderTree (hWnd, szTitleBuf, szFileName);
	FreeLibrary (hInst);

	if( rc == IDOK && util::ExistFile(szFileName) ) {
		strFolderPath = szFileName;
		return true;
	}else{
		return false;
	}
#else
    BROWSEINFO bInfo;
    LPITEMIDLIST pIDList;
    TCHAR szDisplayName[MAX_PATH];
    
    // BROWSEINFO構造体に値を設定
    bInfo.hwndOwner             = hWnd;						// ダイアログの親ウインドウのハンドル
    bInfo.pidlRoot              = NULL;                     // ルートフォルダを示すITEMIDLISTのポインタ (NULLの場合デスクトップフォルダが使われます）
    bInfo.pszDisplayName        = szDisplayName;            // 選択されたフォルダ名を受け取るバッファのポインタ
    bInfo.lpszTitle             = szTitle;					// ツリービューの上部に表示される文字列 
    bInfo.ulFlags               = BIF_RETURNONLYFSDIRS;     // 表示されるフォルダの種類を示すフラグ
    bInfo.lpfn                  = NULL;                     // BrowseCallbackProc関数のポインタ
    bInfo.lParam                = (LPARAM)0;                // コールバック関数に渡す値

    // フォルダ選択ダイアログを表示
    pIDList = ::SHBrowseForFolder(&bInfo);
    if(pIDList == NULL){

        // フォルダが選択されずにダイアログが閉じられた
		return false;

    }else{

        // ItemIDListをパス名に変換します
        if(!::SHGetPathFromIDList(pIDList, szDisplayName)){
            // エラー処理
			return false;
        }

        // szDisplayNameに選択されたフォルダのパスが入っています
		strFolderPath = szDisplayName;

        // 最後にpIDListのポイントしているメモリを開放します
        ::CoTaskMemFree( pIDList );
    }
	return true;
#endif
}

POINT GetPopupPos()
{
	POINT pt;

#ifdef WINCE
	// MZ3 : 画面の中心でポップアップする
	RECT rect;
	SystemParametersInfo(SPI_GETWORKAREA, 0, &rect, 0);
	pt.x = rect.left + (rect.right-rect.left) / 2;
	pt.y = rect.top  + (rect.bottom-rect.top) / 2;
#else
	// MZ4 : マウスの位置でポップアップする
	GetCursorPos(&pt);
#endif

	return pt;
}

int GetPopupFlags()
{
#ifdef WINCE
	// MZ3 : 画面の中心でポップアップする
	int flags = TPM_CENTERALIGN | TPM_VCENTERALIGN;
#else
	// MZ4 : マウスの位置でポップアップする
	// マウス位置を左上にして表示、右ボタンを有効にする
	int flags = TPM_LEFTALIGN | TPM_TOPALIGN | TPM_RIGHTBUTTON;
#endif

	return flags;
}

/// アスペクト比固定で拡大・縮小
CSize makeAspectFixedFitSize( int w0, int h0, int w1, int h1 )
{
	if (w0==0 || h0==0 || w1==0 || h1==0) {
		return CSize();
	}

	// 縦,横基準で拡大・縮小
	int w = (int)(h1/(double)h0 * w0);
	int h = (int)(w1/(double)w0 * h0);

	if (w>w1) {
		return CSize(w1, h);
	} else {
		return CSize(w, h1);
	}
}

void MakeResizedImage( CWnd* pWnd, CMZ3BackgroundImage& resizedImage, CMZ3BackgroundImage& image, int w/*=16*/, int h/*=16*/ )
{
	HWND hwnd = pWnd->GetSafeHwnd();

	HDC hdcDisp = ::GetDC( hwnd );
	resizedImage.create( hdcDisp, w, h, 24 );
	::ReleaseDC( hwnd, hdcDisp );

	// メモリDCの生成
	HDC hdcFrom = CreateCompatibleDC(NULL);
	HDC hdcDest = CreateCompatibleDC(NULL);

	SelectObject( hdcFrom, image.getHandle() );
	SelectObject( hdcDest, resizedImage.getHandle() );

	// 白で塗りつぶす
	::FillRect( hdcDest, CRect(0, 0, w, h), (HBRUSH)GetStockObject(WHITE_BRUSH));
#ifdef WINCE
	::SetStretchBltMode( hdcDest, COLORONCOLOR );
#else
	::SetStretchBltMode( hdcDest, HALFTONE );
	::SetBrushOrgEx( hdcDest, 0, 0, NULL );
#endif
	// アスペクト比固定でリサイズ
	CSize size = image.getBitmapSize();
	CSize sizeDest = util::makeAspectFixedFitSize( size.cx, size.cy, w, h );
	// リサイズしたサイズで描画
	int x = (w - sizeDest.cx)/2;
	int y = (h - sizeDest.cy)/2;
//	wprintf(util::FormatString(L" MakeResizedImage : %dx%d => %dx%d\n", 
//									   size.cx, size.cy, sizeDest.cx, sizeDest.cy));
	::StretchBlt( hdcDest, x, y, sizeDest.cx, sizeDest.cy, hdcFrom, 0, 0, size.cx, size.cy, SRCCOPY );

	// メモリDCの解放
	::DeleteDC( hdcDest );
	::DeleteDC( hdcFrom );
}


/**
 * 指定されたIDのウインドウのスタイルをを移動する
 */
void ModifyStyleDlgItemWindow(CWnd* pParent, int idc, DWORD dwRemove, DWORD dwAdd, UINT nFlags/*=NULL*/)
{
	if (pParent==NULL) {
		return;
	}

	CWnd* pDlgItem = pParent->GetDlgItem(idc);
	if (pDlgItem==NULL) {
		return;
	}

	pDlgItem->ModifyStyle(dwRemove,dwAdd,nFlags);
}


}	// end of namespace "util"
