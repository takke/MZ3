/*
 * Copyright (c) MZ3 Project Team All rights reserved.
 * Code licensed under the BSD License:
 * http://www.mz3.jp/license.txt
 */
#pragma once
/**
 * GUI 関連ユーティリティ
 */

#include "util_base.h"
#include "MZ3BackgroundImage.h"

/// ユーティリティ
namespace util
{
int GetOpenFileNameEx(OPENFILENAME* pofn);
bool DrawBitmap( HDC hdc, HBITMAP hBitmap, int x, int y, int w, int h, int srcx, int srcy );
bool DrawBitmap( HDC hdc, HBITMAP hBitmap, int x, int y, int w, int h, int tox, int toy, int tow, int toh );
void MoveDlgItemWindow( CWnd* pParent, int idc, int x, int y, int nWidth, int nHeight );
void MySetListCtrlItemFocusedAndSelected( CListCtrl& listCtrl, int idx, bool bFocusedAndSelected );
int MyGetListCtrlSelectedItemIndex( CListCtrl& listCtrl );
bool MySetListCtrlItemImageIndex( CListCtrl& listCtrl, int itemIndex, int subItemIndex, int imageIndex );
bool IsVisibleOnListBox( CListCtrl& listCtrl, int idx );
bool GetOpenFolderPath( HWND hWnd, LPCTSTR szTitle, CString& strFolderPath );
POINT GetPopupPos();
int GetPopupFlags();
CSize makeAspectFixedFitSize( int w0, int h0, int w1, int h1 );
void MakeResizedImage( CWnd* pWnd, CMZ3BackgroundImage& resizedImage, CMZ3BackgroundImage& image, int w=16, int h=16 );
void ModifyStyleDlgItemWindow( CWnd* pParent,int idc, DWORD dwRemove,DWORD dwAdd,UINT nFlags=NULL);


}	// end of namespace "util"
