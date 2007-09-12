#pragma once

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
		if (!m_hBitmap) {
			deleteBitmap();
		}
		m_hBitmap = loadBitmap( szFilepath );

		return true;
	}

private:

	/// ビットマップのロード
	static inline HBITMAP loadBitmap( LPCTSTR szFilepath )
	{
#ifdef WINCE
		return SHLoadImageFile( szFilepath );
#else
		return (HBITMAP)LoadImage( 0, szFilepath, IMAGE_BITMAP,0,0,LR_LOADFROMFILE);
#endif
	}

	/// ビットマップの削除
	bool deleteBitmap()
	{
		// ビットマップの削除
		if( m_hBitmap != NULL ) {
			return DeleteObject( m_hBitmap ) ? true : false;
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

