#pragma once

/**
 * �w�i�p�摜�Ǘ��N���X
 */
class CMZ3BackgroundImage
{
public:
	HBITMAP m_hBitmap;				///< �w�i�p�r�b�g�}�b�v
	CString	m_strImageFilepath;		///< �w�i�p�r�b�g�}�b�v��ǂݍ��񂾂Ƃ��̔w�i�摜�t�@�C���p�X�B
									///< CMZ3App ���ϐ����؂�ւ�����ۂɁA�����I�Ƀ����[�h���邽�߂Ɏg�p����B

	CString m_strImageName;			///< �摜�t�@�C����

	/// �R���X�g���N�^
	CMZ3BackgroundImage(LPCTSTR szImageName) : m_hBitmap(NULL), m_strImageName(szImageName) 
	{
	}

	/// �f�X�g���N�^
	~CMZ3BackgroundImage()
	{
		deleteBitmap();
	}

	/// m_hBitmap �̎Q��
	HBITMAP getHandle()
	{
		return m_hBitmap;
	}

	bool isValidSkinfile( LPCTSTR szSkinName );
	CString makeSkinFilePath( LPCTSTR szSkinName );
	bool load();

	bool SetImageFileName( LPCTSTR szImageFileName )
	{
		m_strImageName = szImageFileName;
		return true;
	}

private:

	/// �r�b�g�}�b�v�̃��[�h
	static inline HBITMAP loadBitmap( LPCTSTR szFilepath )
	{
#ifdef WINCE
		return SHLoadImageFile( szFilepath );
#else
		return (HBITMAP)LoadImage( 0, szFilepath, IMAGE_BITMAP,0,0,LR_LOADFROMFILE);
#endif
	}

	/// �r�b�g�}�b�v�̍폜
	bool deleteBitmap()
	{
		// �r�b�g�}�b�v�̍폜
		if( m_hBitmap != NULL ) {
			return DeleteObject( m_hBitmap ) ? true : false;
		} else {
			return true;
		}
	}
};

