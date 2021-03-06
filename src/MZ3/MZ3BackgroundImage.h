/*
 * Copyright (c) MZ3 Project Team All rights reserved.
 * Code licensed under the BSD License:
 * http://www.mz3.jp/license.txt
 */
#pragma once

#include "util_base.h"

/**
 * 背景用画像管理クラス
 */
class CMZ3BackgroundImage
{
public:
	HBITMAP m_hBitmap;				///< 背景用ビットマップ
	CString	m_strImageFilepath;		///< 背景用ビットマップを読み込んだときの背景画像ファイルパス。
									///< CMZ3App 側変数が切り替わった際に、自動的にリロードするために使用する。

	CString m_strImageName;			///< 画像ファイル名
#ifndef WINCE
	CImage  m_gdiPlusImage;			///< GDI+ の CImage クラスオブジェクト
#endif

	/// コンストラクタ
	CMZ3BackgroundImage(LPCTSTR szImageName) : m_hBitmap(NULL), m_strImageName(szImageName) 
	{
	}

	/// デストラクタ
	~CMZ3BackgroundImage()
	{
		deleteBitmap();
	}

	/// m_hBitmap の参照
	HBITMAP getHandle()
	{
		return m_hBitmap;
	}

	bool isEnableImage() { return m_hBitmap!=NULL ? true : false; }

	bool isValidSkinfile( LPCTSTR szSkinName );
	CString makeSkinFilePath( LPCTSTR szSkinName );
	bool load();

	bool setImageFileName( LPCTSTR szImageFileName )
	{
		m_strImageName = szImageFileName;
		return true;
	}

	bool load( LPCTSTR szFilepath )
	{
		if (!util::ExistFile(szFilepath)) {
			return false;
		}
		if (!m_hBitmap) {
			deleteBitmap();
		}
		m_hBitmap = loadBitmap( szFilepath );

		return isEnableImage();
	}

	bool create( HDC hdc, int w, int h, int depth )
	{
		m_hBitmap = ::CreateCompatibleBitmap( hdc, w, h );

		return isEnableImage();
	}

private:

	/// ビットマップのロード
	inline HBITMAP loadBitmap( LPCTSTR szFilepath )
	{
#ifdef WINCE
		return SHLoadImageFile( szFilepath );
#else
		if (!m_gdiPlusImage.IsNull()) {
			m_gdiPlusImage.Destroy();
		}
		m_gdiPlusImage.Load( szFilepath );
		return (HBITMAP) m_gdiPlusImage;
#endif
	}

	/// ビットマップの削除
	bool deleteBitmap()
	{
		// ビットマップの削除
		if( m_hBitmap != NULL ) {
			BOOL rval = DeleteObject( m_hBitmap );
			if (rval) {
				m_hBitmap = NULL;
				return true;
			}
			return false;
		} else {
			return true;
		}
	}

public:
	CSize getBitmapSize()
	{
		if (m_hBitmap==NULL) {
			return CSize();
		}

		BITMAP bmp;
		GetObject(m_hBitmap, sizeof(BITMAP), &bmp);
		CSize size;
		size.cx = bmp.bmWidth;
		size.cy = bmp.bmHeight;

		return size;
	}

};

