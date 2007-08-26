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

	/**
	 * ファイルが未ロード、またはスキン名が変わったらロードする
	 */
	bool load()
	{
		// 画像パスの生成。
		// スキンフォルダ名、スキン名、画像ファイル名を連結して生成する。
		CString strImagePath;
		strImagePath.Format( L"%s\\%s\\%s", (LPCTSTR)theApp.m_filepath.skinFolder, (LPCTSTR)theApp.m_optionMng.m_strSkinname, m_strImageName );

		if( m_hBitmap == NULL ) {
			// 未ロードなのでロードする
			m_hBitmap = loadBitmap( strImagePath );
			m_strImageFilepath = strImagePath;
		} else {
			// ロード済みなので、スキンファイル名が異なればロードする。
			if (m_strImageFilepath != strImagePath) {
				// 削除する
				deleteBitmap();

				// ロードする
				m_hBitmap = loadBitmap( strImagePath );
				m_strImageFilepath = strImagePath;
			}
		}

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
};

