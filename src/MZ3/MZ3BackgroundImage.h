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
#ifndef WINCE
	CImage  m_gdiPlusImage;			///< GDI+ �� CImage �N���X�I�u�W�F�N�g
#endif

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
		if (!m_hBitmap) {
			deleteBitmap();
		}
		m_hBitmap = loadBitmap( szFilepath );

		return true;
	}

private:

	/// �r�b�g�}�b�v�̃��[�h
	inline HBITMAP loadBitmap( LPCTSTR szFilepath )
	{
#ifdef WINCE
		return SHLoadImageFile( szFilepath );
#else
//		HBITMAP hBitmap = (HBITMAP)LoadImage( NULL, szFilepath, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE );
//		return hBitmap;
		if (!m_gdiPlusImage.IsNull()) {
			m_gdiPlusImage.Destroy();
		}
		m_gdiPlusImage.Load( szFilepath );
		return (HBITMAP) m_gdiPlusImage;
#endif
	}

	/// �r�b�g�}�b�v�̍폜
	bool deleteBitmap()
	{
		// �r�b�g�}�b�v�̍폜
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

