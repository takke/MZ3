#pragma once

/**
 * �摜�L���b�V��
 */
class ImageCacheManager
{
	int				m_width;			///< ��
	int				m_height;			///< ����
	CImageList		m_imageList;		///< �摜���X�g
	CStringArray	m_imageListMap;		///< m_imageList �̊e�C���f�b�N�X��
										///< �ǂ̃t�@�C�����i�[����Ă��邩�������^���}�b�v

public:
	bool Create()
	{
		m_imageList.Create(16, 16, ILC_COLOR24 | ILC_MASK, 0, 4);
		m_imageListMap.RemoveAll();

		return true;
	}

	/**
	 * �摜���X�g�̎擾
	 */
	CImageList& GetImageList() {
		return m_imageList;
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
	int Add( CBitmap* pbmImage, CBitmap* pbmMask, LPCTSTR path )
	{
		// TODO �ǉ����s���̏���
		m_imageList.Add( pbmImage, pbmMask );
		m_imageListMap.Add( path );

		// �ǉ������C���f�b�N�X��Ԃ�
		return m_imageList.GetImageCount() -1;
	}
};

