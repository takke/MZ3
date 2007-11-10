#pragma once

#include "MZ3BackgroundImage.h"

// CMiniImageDialog �_�C�A���O

class CMiniImageDialog : public CDialog
{
	DECLARE_DYNAMIC(CMiniImageDialog)

public:
	CMiniImageDialog(CWnd* pParent = NULL);   // �W���R���X�g���N�^
	virtual ~CMiniImageDialog();
	CMZ3BackgroundImage	m_image;	///< �r�b�g�}�b�v

// �_�C�A���O �f�[�^
	enum { IDD = IDD_MINI_IMAGE_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �T�|�[�g

	DECLARE_MESSAGE_MAP()
	virtual void PostNcDestroy();
public:
	void DrawImageFile(LPCTSTR szImageFile);
	afx_msg void OnPaint();
};
