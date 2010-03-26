/*
 * Copyright (c) MZ3 Project Team All rights reserved.
 * Code licensed under the BSD License:
 * http://www.mz3.jp/license.txt
 */
#pragma once

#include <hash_map>

/**
 * 画像キャッシュ
 */
class ImageCacheManager
{
	int				m_width;			///< 幅
	int				m_height;			///< 高さ
	CImageList		m_imageList16;		///< 画像リスト(16x16)
	CImageList		m_imageList32;		///< 画像リスト(32x32)
	CImageList		m_imageList48;		///< 画像リスト(48x48)
	CImageList		m_imageList64;		///< 画像リスト(64x64)
	CStringArray	m_imageListMap;		///< m_imageList の各インデックスに
										///< どのファイルが格納されているかを示す疑似マップ

	typedef stdext::hash_map<std::wstring, int> wstring_to_index_map;
	wstring_to_index_map m_loadedFileToIndexMap;	///< ロード済みファイルから
													///< m_imageList のインデックスを引くマップ

public:
	bool Create()
	{
		const unsigned int nGrow = 8;

		m_imageList16.Create(16, 16, ILC_COLOR16 | ILC_MASK, 0, nGrow);
		m_imageList32.Create(32, 32, ILC_COLOR16 | ILC_MASK, 0, nGrow);
		m_imageList48.Create(48, 48, ILC_COLOR16 | ILC_MASK, 0, nGrow);
		m_imageList64.Create(64, 64, ILC_COLOR16 | ILC_MASK, 0, nGrow);
		m_imageListMap.RemoveAll();
		m_loadedFileToIndexMap.clear();

		return true;
	}

	/**
	 * 画像リスト(16x16)の取得
	 */
	CImageList& GetImageList16() {
		return m_imageList16;
	}

	/**
	 * 画像リスト(32x32)の取得
	 */
	CImageList& GetImageList32() {
		return m_imageList32;
	}

	/**
	 * 画像リスト(48x48)の取得
	 */
	CImageList& GetImageList48() {
		return m_imageList48;
	}

	/**
	 * 画像リスト(64x64)の取得
	 */
	CImageList& GetImageList64() {
		return m_imageList64;
	}

	/**
	 * 指定されたサイズに適した画像リストを返す
	 */
	CImageList& GetImageListBySize(int h) {
		if (h>=64) {
			return m_imageList64;
		} else if (h>=48) {
			return m_imageList48;
		} else if (h>=32) {
			return m_imageList32;
		} else {
			return m_imageList16;
		}
	}

	/**
	 * 指定されたサイズに適した画像サイズを返す
	 */
	int GetSuiteImageSizeByHeight(int h) {
		if (h>=64) {
			return 64;
		} else if (h>=48) {
			return 48;
		} else if (h>=32) {
			return 32;
		} else {
			return 16;
		}
	}

	/**
	 * 指定されたパスの画像のインデックスを返す。
	 *
	 * 未ロードであれば -1 を返す。
	 */
	int GetImageIndex( LPCTSTR path ) {

		wstring_to_index_map::iterator it = m_loadedFileToIndexMap.find(path);
		if (it==m_loadedFileToIndexMap.end()) {
			return -1;
		}
		return it->second;

/*		const int n = m_imageListMap.GetCount();
		for (int i=0; i<n; i++) {
			if (m_imageListMap[i] == path) {
				return i;
			}
		}
		return -1;
*/	}

	/**
	 * 画像を追加し、追加したインデックスを返す。
	 *
	 * 追加できなかった場合は -1 を返す。
	 */
	int Add( CBitmap* pbmImage16, CBitmap* pbmImage32, CBitmap* pbmImage48, CBitmap* pbmImage64, CBitmap* pbmMask, LPCTSTR path )
	{
		// TODO 追加失敗時の処理
		m_imageList16.Add( pbmImage16, pbmMask );
		m_imageList32.Add( pbmImage32, pbmMask );
		m_imageList48.Add( pbmImage48, pbmMask );
		m_imageList64.Add( pbmImage64, pbmMask );
		m_imageListMap.Add( path );

		int idx = m_imageList16.GetImageCount() -1;

		m_loadedFileToIndexMap.insert( std::pair<std::wstring, int>(path, idx) );

		// 追加したインデックスを返す
		return idx;
	}

	/**
	 * インデックスがどのパスを示すか返す。
	 * 範囲外の場合は-1を返す。
	 */
	CString GetImagePath(int index) {
		CString iconFullPath = TEXT("");
		if( index < 0 || m_imageListMap.GetSize() < index ){
			return(iconFullPath);
		}

		iconFullPath = m_imageListMap.GetAt(index);
		return(iconFullPath);
	}
};

