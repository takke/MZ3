/*
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */
#pragma once

/**
 * �摜�L���b�V��
 */
class ImageCacheManager
{
	int				m_width;			///< ��
	int				m_height;			///< ����
	CImageList		m_imageList16;		///< �摜���X�g(16x16)
	CImageList		m_imageList32;		///< �摜���X�g(32x32)
	CStringArray	m_imageListMap;		///< m_imageList �̊e�C���f�b�N�X��
										///< �ǂ̃t�@�C�����i�[����Ă��邩�������^���}�b�v

public:
	bool Create()
	{
		m_imageList16.Create(16, 16, ILC_COLOR24 | ILC_MASK, 0, 4);
		m_imageList32.Create(32, 32, ILC_COLOR24 | ILC_MASK, 0, 4);
		m_imageListMap.RemoveAll();

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
	 * �w�肳�ꂽ�p�X�̉摜�̃C���f�b�N�X��Ԃ��B
	 *
	 * �����[�h�ł���� -1 ��Ԃ��B
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
	 * �摜��ǉ����A�ǉ������C���f�b�N�X��Ԃ��B
	 *
	 * �ǉ��ł��Ȃ������ꍇ�� -1 ��Ԃ��B
	 */
	int Add( CBitmap* pbmImage16, CBitmap* pbmImage32, CBitmap* pbmMask, LPCTSTR path )
	{
		// TODO �ǉ����s���̏���
		m_imageList16.Add( pbmImage16, pbmMask );
		m_imageList32.Add( pbmImage32, pbmMask );
		m_imageListMap.Add( path );

		// �ǉ������C���f�b�N�X��Ԃ�
		return m_imageList16.GetImageCount() -1;
	}

	/**
	 * �C���f�b�N�X���ǂ̃p�X���������Ԃ��B
	 * �͈͊O�̏ꍇ��-1��Ԃ��B
	 */
	CString GetImagePath(int index){
		CString iconFullPath = TEXT("");
		if( index < 0 || m_imageListMap.GetSize() < index ){
			return(iconFullPath);
		}

		iconFullPath = m_imageListMap.GetAt(index);
		return(iconFullPath);
	}
};

