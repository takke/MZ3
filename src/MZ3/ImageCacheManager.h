/*
 * Copyright (c) MZ3 Project Team All rights reserved.
 * Code licensed under the BSD License:
 * http://www.mz3.jp/license.txt
 */
#pragma once

#include <hash_map>

/**
 * �摜�L���b�V��
 */
class ImageCacheManager
{
	int				m_width;			///< ��
	int				m_height;			///< ����
	CImageList		m_imageList16;		///< �摜���X�g(16x16)
	CImageList		m_imageList32;		///< �摜���X�g(32x32)
	CImageList		m_imageList48;		///< �摜���X�g(48x48)
	CImageList		m_imageList64;		///< �摜���X�g(64x64)
	CStringArray	m_imageListMap;		///< m_imageList �̊e�C���f�b�N�X��
										///< �ǂ̃t�@�C�����i�[����Ă��邩�������^���}�b�v

	typedef stdext::hash_map<std::wstring, int> wstring_to_index_map;
	wstring_to_index_map m_loadedFileToIndexMap;	///< ���[�h�ς݃t�@�C������
													///< m_imageList �̃C���f�b�N�X�������}�b�v

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
	 * �摜���X�g(16x16)�̎擾
	 */
	CImageList& GetImageList16() {
		return m_imageList16;
	}

	/**
	 * �摜���X�g(32x32)�̎擾
	 */
	CImageList& GetImageList32() {
		return m_imageList32;
	}

	/**
	 * �摜���X�g(48x48)�̎擾
	 */
	CImageList& GetImageList48() {
		return m_imageList48;
	}

	/**
	 * �摜���X�g(64x64)�̎擾
	 */
	CImageList& GetImageList64() {
		return m_imageList64;
	}

	/**
	 * �w�肳�ꂽ�T�C�Y�ɓK�����摜���X�g��Ԃ�
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
	 * �w�肳�ꂽ�T�C�Y�ɓK�����摜�T�C�Y��Ԃ�
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
	 * �w�肳�ꂽ�p�X�̉摜�̃C���f�b�N�X��Ԃ��B
	 *
	 * �����[�h�ł���� -1 ��Ԃ��B
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
	 * �摜��ǉ����A�ǉ������C���f�b�N�X��Ԃ��B
	 *
	 * �ǉ��ł��Ȃ������ꍇ�� -1 ��Ԃ��B
	 */
	int Add( CBitmap* pbmImage16, CBitmap* pbmImage32, CBitmap* pbmImage48, CBitmap* pbmImage64, CBitmap* pbmMask, LPCTSTR path )
	{
		// TODO �ǉ����s���̏���
		m_imageList16.Add( pbmImage16, pbmMask );
		m_imageList32.Add( pbmImage32, pbmMask );
		m_imageList48.Add( pbmImage48, pbmMask );
		m_imageList64.Add( pbmImage64, pbmMask );
		m_imageListMap.Add( path );

		int idx = m_imageList16.GetImageCount() -1;

		m_loadedFileToIndexMap.insert( std::pair<std::wstring, int>(path, idx) );

		// �ǉ������C���f�b�N�X��Ԃ�
		return idx;
	}

	/**
	 * �C���f�b�N�X���ǂ̃p�X���������Ԃ��B
	 * �͈͊O�̏ꍇ��-1��Ԃ��B
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

