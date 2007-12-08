#pragma once

/**
 * 画像キャッシュ
 */
class ImageCacheManager
{
	int				m_width;			///< 幅
	int				m_height;			///< 高さ
	CImageList		m_imageList;		///< 画像リスト
	CStringArray	m_imageListMap;		///< m_imageList の各インデックスに
										///< どのファイルが格納されているかを示す疑似マップ

public:
	bool Create()
	{
		m_imageList.Create(16, 16, ILC_COLOR24 | ILC_MASK, 0, 4);
		m_imageListMap.RemoveAll();

		return true;
	}

	/**
	 * 画像リストの取得
	 */
	CImageList& GetImageList() {
		return m_imageList;
	}

	/**
	 * 指定されたパスの画像のインデックスを返す。
	 *
	 * 未ロードであれば -1 を返す。
	 */
	int GetImageIndex( LPCTSTR path ) {
		const int n = m_imageListMap.GetCount();
		for (int i=0; i<n; i++) {
			if (m_imageListMap[i] == path) {
				return i;
			}
		}
		return -1;
	}

	/**
	 * 画像を追加し、追加したインデックスを返す。
	 *
	 * 追加できなかった場合は -1 を返す。
	 */
	int Add( CBitmap* pbmImage, CBitmap* pbmMask, LPCTSTR path )
	{
		// TODO 追加失敗時の処理
		m_imageList.Add( pbmImage, pbmMask );
		m_imageListMap.Add( path );

		// 追加したインデックスを返す
		return m_imageList.GetImageCount() -1;
	}
};

