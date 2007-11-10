#pragma once

#include "MZ3BackgroundImage.h"

// CMiniImageDialog ダイアログ

class CMiniImageDialog : public CDialog
{
	DECLARE_DYNAMIC(CMiniImageDialog)

public:
	CMiniImageDialog(CWnd* pParent = NULL);   // 標準コンストラクタ
	virtual ~CMiniImageDialog();
	CMZ3BackgroundImage	m_image;	///< ビットマップ

// ダイアログ データ
	enum { IDD = IDD_MINI_IMAGE_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート

	DECLARE_MESSAGE_MAP()
	virtual void PostNcDestroy();
public:
	void DrawImageFile(LPCTSTR szImageFile);
	afx_msg void OnPaint();
};
