#pragma once
/**
 * GUI 関連ユーティリティ
 */

/// ユーティリティ
namespace util
{
/**
 * gsgetfile.dll を用いてファイル選択画面を表示する
 *
 * 成功時は IDOK が返る
 */
/*
inline int GetOpenFileNameEx(OPENFILENAME* pofn)
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
	// TODO 実装すること
	return 0;
#endif
}
*/

/**
 * 指定されたビットマップ hBitmap を hdc に描画する
 */
inline bool DrawBitmap( HDC hdc, HBITMAP hBitmap, int x, int y, int w, int h, int tox, int toy )
{
	if( hBitmap == NULL ) {
		return false;
	}

	BITMAP	bmp;
	GetObject( hBitmap, sizeof(bmp), &bmp );

	HDC hdc1 = CreateCompatibleDC(NULL);
	HBITMAP hBitmapOld = (HBITMAP)SelectObject(hdc1,hBitmap);

	// 転送
//	TRACE( L"DrawBitmap [%d,%d,%d,%d,%d,%d]\n", x, y, w, h, tox, toy );
	BitBlt( hdc, x, y, w, h, hdc1, tox, toy, SRCCOPY );

	SelectObject( hdc1, hBitmapOld );

	DeleteDC( hdc1 );
	return true;
}

/**
 * 指定されたIDのウィンドウを移動する
 */
inline void MoveDlgItemWindow( CWnd* pParent, int idc, int x, int y, int nWidth, int nHeight )
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
inline void MySetListCtrlItemFocusedAndSelected( CListCtrl& listCtrl, int idx, bool bFocusedAndSelected )
{
	UINT nState = 0;
	if( bFocusedAndSelected ) {
		nState |= LVIS_FOCUSED | LVIS_SELECTED;
	}

	listCtrl.SetItemState( idx, nState, LVIS_FOCUSED | LVIS_SELECTED );
}

/**
 * リストコントロール内で指定されたインデックスが表示状態にあるかどうかを返す。
 */
inline bool IsVisibleOnListBox( CListCtrl& listCtrl, int idx )
{
	int idxTop  = listCtrl.GetTopIndex();				// 画面の一番上の表示項目のインデックス
	int idxLast = idxTop + listCtrl.GetCountPerPage();	// 画面の一番下の表示項目のインデックス
	if( idx >= idxLast ) {
		// 下方にある
		return false;
	}
	if( idx <= idxTop ) {
		// ただし、先頭項目の場合は除く
		if( idx==0 ) {
			return true;
		}

		// 上方にある
		return false;
	}
	return true;
}

/**
 * フォルダ選択画面を表示する
 *
 * @param hWnd			[in] 親画面
 * @param szTitle		[in] フォルダ選択画面のタイトル
 * @param strFolderPath	[in/out] 初期選択フォルダ＆選択済みフォルダパス
 * @return 成功時は true、失敗時は false を返す。
 */
inline bool GetOpenFolderPath( HWND hWnd, LPCTSTR szTitle, CString& strFolderPath )
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


}